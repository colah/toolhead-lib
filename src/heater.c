#include <stdio.h>
#include <stdlib.h>
#include "toolhead_error.h"
#include "temperature_sensor.h"
#include "heater.h"
#include <math.h>
//#include "pins_arduino.h"

void heater_init(struct heater * h, unsigned long time)
{
  int i;

  // Current value defaults
  h->current = 0.0;
  for (i = 0; i<PID_LENGTH; i++)
  {
    h->pid_values[i] = 0.0;
    h->pid_gains[i] = 0.0;
  }
  h->at_target = 0;
  h->heater_timeout = 0;

  // Private variable defaults
  h->_target_reached_at = 0;
  for (i = 0; i<HEATER_HISTORY_LENGTH; i++)
    h->_history[i] = 0.0;

  // Resetting the rest of the heater variables
  heater_reset(h, time);

  // Hardware setup
  if (h->init_heater_pins != NULL) h->init_heater_pins(h->heater_pins);
}


/*
 * Private: reads and returns the heater's temperature in degrees celsius.
 */
int heater_read_sensor(struct heater * h)
{
  int temperature;

  // Read the sensor temperature and record any errors
  int error_code = read_temperature_sensor(h->sensor, &temperature);

  // Overheating error
  if (temperature > h->thermal_cutoff && h->target != 0)
  {
    error_code |= heater_overheated_error;
  }

  h->instantaneous = (float) temperature;

  return error_code;
}


void heater_update_target_temperature_listener(struct heater * h)
{
  int inside_deadzone = ( fabs(h->current - h->target) <= HALF_DEAD_ZONE );

  if (h->_previous_target != h->target || inside_deadzone == 0)
  {
    h->at_target = 0;
    h->_target_reached_at = 0;
  }
  h->_previous_target = h->target;

  if(inside_deadzone)
  {
    // reset the timestamp if we have rolled over the timestamp or we he have just reached the target.
    if (h->_target_reached_at == 0 || h->_target_reached_at > h->_previous_time)
      h->_target_reached_at = h->_previous_time;

    // wait to verify the temperature has stabalized in the deadzone before signalling
    // that we have reached the target temperature.
    if(h->_previous_time - h->_target_reached_at > WAIT_AT_TEMPERATURE)
    {
      h->at_target = 1;
      // Fire a target temperature event to signal that we have acheived a steady state temperature.
      if (h->target_temperature_listener != NULL) h->target_temperature_listener();
    }
  }
}



int heater_pump(struct heater * h, unsigned long time)
{
  int i; //iterator (NOT integral)
  int error_code = heater_read_sensor(h);
  float temperature = h->instantaneous;
  float previous_p = h->pid_values[pid_p];

  // Updating the time since the last iteration
  int dt = (int)(time - h->_previous_time); // in miliseconds
  h->_previous_time = time;
  if (dt <= 0) // Don't execute the pid when time has rolled over
    return error_code;


  // Updating the averaged current temperature value. Average is over the last 5 temperature values.
  float history_sum = 0.0;
  for (i = HEATER_HISTORY_LENGTH-1; i>=0; i--)
  {
    h->_history[i] = ( (i == 0)? temperature : h->_history[i-1] );
    history_sum += h->_history[i];
  }
  h->current = history_sum / HEATER_HISTORY_LENGTH;


  // Calculating proportional value (also known as error)
  // -----------------------------------------------------
  h->pid_values[pid_p] = (float)(h->target - temperature);


  // Calculating integral value
  // ----------------------------
  float integral = h->pid_values[pid_i] + (h->pid_values[pid_p]*dt)*0.001;

  // Prevent integral overshoots/wind-down from getting too large
  if(abs(integral) > PID_INTEGRAL_LIMIT)
  {
    integral = ( (integral>= 0)? 1 : -1 ) * PID_INTEGRAL_LIMIT;
  }

  h->pid_values[pid_i] = integral;


  // Calculating derivative value
  // ------------------------------
  h->pid_values[pid_d] = ( 1000 * (h->pid_values[pid_p] - previous_p ) )/((float)dt);


  // Heater not heating sensor error
  // ----------------------------------
  if (h->_previous_target != h->target ||
      time < h->_last_sensor_heating ||
      h->pid_values[pid_d] > 0 ||
      temperature >= h->target - HALF_DEAD_ZONE)
  {
    h->_last_sensor_heating = time;
  }

  if (time - h->_last_sensor_heating > h->heater_timeout && h->heater_timeout != 0)
  {
    // Temp isn't increasing - extruder hardware error
    error_code = heater_not_heating_sensor_error;
  }


  // Summing the pid and updating the heater
  // -----------------------------------------
  float _output = 0;
  for (i = 0; i < PID_LENGTH; i++)
    _output += h->pid_values[i]*h->pid_gains[i];

  int output = (_output < 0)? 0 : (_output > 255)? 255 : (int) _output;

  if (h->write_heater_pins != NULL) h->write_heater_pins(h->heater_pins, (char)output);


  // Updating the event listener if we have reached a steady state target temperature
  heater_update_target_temperature_listener(h);

  return error_code;
}


/*
 Reset the PID to, for example, remove accumulated integral error from
 a long period when the heater was off and the requested temperature was 0 (which it
 won't go down to, even with the heater off, so the integral error grows).  Call this 
 whenever you change the target value.
 */
void heater_reset(struct heater * h, unsigned long time)
{
  h->_previous_time = time;
  h->pid_values[pid_p] = 0;
  h->pid_values[pid_i] = 0;
  h->_last_sensor_heating = time;
}


/*
 * Shuts down the heater and frees it's memory for later use.
 */
void heater_shutdown(struct heater * h)
{
  if (h == NULL) return;
  if (h->shutdown_heater_pins != NULL) h->shutdown_heater_pins(h);
  free(h);        /* free the structure itself */
}

char PID_LETTERS[PID_LENGTH] = {'p', 'i', 'd'};
