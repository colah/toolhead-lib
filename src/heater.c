#include "toolhead_error.h"
#include "temperature_sensor.h"
#include "heater.h"
//#include "pins_arduino.h"

void heater_init(heater * h)
{
  //Setup our pin modes
  pinMode(h->heater_pin, OUTPUT);
  pinMode(h->sensor_pin, INPUT);

  analogWrite(h->heater_pin, 0); // heater is off by default
  digitalWrite(h->sensor_pin, false); // pull down resistor for the sensor input

  // Current value defaults
  h->target = 0.0;
  h->current = 0.0;
  for (int i = 0; i<PID_LENGTH; i++)
    h->pid_values[i] = 0.0;
  h->at_target = false;

  // Private variable defaults
  h->_target_reached_at = 0;
  for (int i = 0; i<HEATER_HISTORY_LENGTH; i++)
    h->_history[i] = 0.0;

  // Resetting the rest ofthe heater variables
  heater_reset(h);

}


/*
 * Private: reads and returns the heater's temperature in degrees celsius.
 */
int heater_read_sensor(heater * h)
{
  float temperature;

  // Read the sensor temperature and record any errors
  int error_code = read_temperature_sensor(h->sensor, &temperature);

  // Overheating error
  if (temperature > h->thermal_cutoff && h->target != 0)
  {
    error_code |= heater_overheated_error;
  }

  h->instantaneous = temperature;

  return error_code;
}


void heater_update_target_temperature_listener(Heater *h)
{
  bool inside_deadzone = ( h->current >= h->target - HALF_DEAD_ZONE );

  if (h->_previous_target != h->target || inside_deadzone == false)
  {
    h->at_target = false;
    h->_target_reached_at = 0;
  }
  h->_previous_target = h->target;

  if(inside_deadzone)
  {
    // reset the timestamp if we have rolled over the timestamp or we he have just reached the target.
    if (h->_target_reached_at == 0 || h->_target_reached_at > h->_previous_time) h->_target_reached_at = h->_previous_time;

    // wait to verify the temperature has stabalized in the deadzone before signalling
    // that we have reached the target temperature.
    if(h->_previous_time - h->_target_reached_at > WAIT_AT_TEMPERATURE)
    {
      h->at_target = true;
      // Fire a target temperature event to signal that we have acheived a steady state temperature.
      if (h->target_temperature_listener != NULL) h->target_temperature_listener();
    }
  }
}



int heater_update(heater * h)
{
  int error_code = heater_read_sensor(h);
  float temperature = h->instantaneous;
  float previous_p = h->pid_values[pid_p];

  // Updating the time since the last iteration
  unsigned long time = millis();
  float dt = 0.001*(float)(time - h->_previous_time);
  h->_previous_time = time;
  if (dt <= 0) // Don't do it when millis() has rolled over
    return error_code;


  // Updating the averaged current temperature value. Average is over the last 5 temperature values.
  float history_sum = 0.0;
  for (int i = 0; i<HEATER_HISTORY_LENGTH; i++)
  {
    h->_history[i] = ( i == 0? temperature : h->_history[i-1] );
    history_sum += h->_history[i];
  }
  h->current = history_sum / HEATER_HISTORY_LENGTH;


  // Calculating proportional value (also known as error)
  // -----------------------------------------------------
  h->pid_values[pid_p] = (float)(h->target - temperature);


  // Calculating integral value
  // ----------------------------
  float integral = h->pid_values[pid_i] + h->pid_values[pid_p]*dt;
  // Prevent integral overshoots
  if(integral > PID_INTEGRAL_LIMIT)
  {
    // Temp isn't increasing - extruder hardware error
    error_code = HEATER_NOT_HEATING_SENSOR;
    integral = PID_INTEGRAL_LIMIT;
  }
  // Integral below limit (integral wind-down?)
  if(integral < -PID_INTEGRAL_LIMIT)
  {
    integral = -PID_INTEGRAL_LIMIT;
  }
  h->pid_values[pid_i] = integral;


  // Calculating derivative value
  // ------------------------------
  h->pid_values[pid_d] = (h->pid_values[pid_p] - previous_p )/dt;


  // Summing the pid and updating the heater
  // -----------------------------------------
  float _output = 0;
  for (int i = 0; i < PID_LENGTH; i++)
    _output += h->pid_values[i]*h->pid_gains[i];

  int output = constrain( (int) _output, 0, 255);

  analogWrite(h->heater_pin, (byte)output);


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
void heater_reset(heater * h)
{
  h->_previous_time = millis();
  h->pid_values[pid_p] = 0;
  h->pid_values[pid_i] = 0;
}


/*
 * Shuts down the heater and frees it's memory for later use.
 */
void heater_shutdown(heater * h)
{
  if (h == NULL) return;
  analogWrite(h->heater_pin, 0); //good to not leave that heater on fire :)
  free(h);        /* free the structure itself */
  //free(h->data);  /* free any child arrays held by the structure */
}

