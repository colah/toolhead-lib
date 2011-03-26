#include "toolhead_error.h"
#include "temperature_sensor.h"
#include "thermistor_tables.h"

/*
 * Private: reads the heater pin and returns it's analog value (ranging from zero to 1022). Fails if sensor is disconnected.
 */
int read_raw_temperature_sensor_input(struct temperature_sensor * sensor, int * raw_analog_input)
{
  int raw = sensor->analog_read(sensor->pins[0]);
  *raw_analog_input = raw;

  // Fatal error if the sensor is disconnected
  if (raw == 1023)
  {
    return temperature_sensor_disconnected_error;
  }
  //Otherwise set the analog input value
  else
  {
    return 0;
  }
}


/* Reads the given thermistor or thermocouple and stores it's value in the temperature int */
int read_temperature_sensor(struct temperature_sensor * sensor, int * temperature)
{
  int error_code = 0;
  int raw_analog_input = 0;

  //Thermistor Vars
  int n_of_samples = 3;
  int raw = 0;

  short** table = (sensor->table)->table;
  int length = (sensor->table)->length;


  switch(sensor->type)
  {
    case temperature_sensor_type_thermistor:

      for(int i = 0; i < n_of_samples; i++)
      {
        error_code |= read_raw_temperature_sensor_input(sensor, &raw_analog_input);
        raw += raw_analog_input;
      }

      raw = raw / n_of_samples;

      // TODO: This should do a binary chop
      for (int i=1; i<length; i++)
      {
        if (table[i][0] > raw)
        {
          *temperature  = table[i-1][1] + 
            (raw - table[i-1][0]) * 
            (table[i][1] - table[i-1][1]) /
            (table[i][0] - table[i-1][0]);

          break;
        }
      }

      // Overflow: Set to last value in the table
      *temperature = table[length-1][1];
      break;

    case temperature_sensor_type_ad595_thermocouple:
      error_code |= read_raw_temperature_sensor_input(sensor, &raw_analog_input);
      *temperature = ( 5.0 * (raw_analog_input) * 100.0) / 1024.0;
      break;

    case temperature_sensor_type_max6675_thermocouple:
      error_code |= temperature_sensor_not_supported;
      *temperature = 0;

/*      int value = 0;
      //TODO: non-arduino-specific max6675 implementation
      digitalWrite(sensor->pins[max6755_ss], 0); // Enable device

      /* Cycle the clock for dummy bit 15 */
/*      digitalWrite(sensor->pins[max6755_sck],1);
      digitalWrite(sensor->pins[max6755_sck],0);

      /* Read bits 14-3 from MAX6675 for the Temp
       	 Loop for each bit reading the value 
       */
/*      for (int i=11; i>=0; i--)
      {
        digitalWrite(sensor->pins[max6755_sck],1);  // Set Clock to HIGH
        value += digitalRead(sensor->pins[max6755_miso]) << i;  // Read data and add it to our variable
        digitalWrite(sensor->pins[max6755_sck],0);  // Set Clock to LOW
      }

      /* Read the TC Input inp to check for TC Errors */
/*      digitalWrite(sensor->pins[max6755_sck],1); // Set Clock to HIGH
      int error_tc = digitalRead(sensor->pins[max6755_miso]); // Read data
      digitalWrite(sensor->pins[max6755_sck],0);  // Set Clock to LOW

      digitalWrite(sensor->pins[max6755_ss], 1); //Disable Device

      if(error_tc)
      {
        *temperature = 2000;
        if (h->target !=0) error_code |= temperature_sensor_internal_error;
      }
      else
        *temperature = value/4;
*/
      break;

    default:
      error_code |= temperature_sensor_invalid_error;
  }

  return error_code;
}
