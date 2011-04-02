#include "toolhead_error.h"
#include "temperature_sensor.h"

void init_temperature_sensor(struct temperature_sensor * sensor)
{
  sensor->_thermistor_table = TEN_K_THERMISTOR_MAP;
  sensor->_thermistor_table_length = THERMISTOR_MAP_LENGTH;
}

/*
 * Private: reads the heater pin and returns it's analog value (ranging from zero to 1022). Fails if sensor is disconnected.
 */
int read_raw_temperature_sensor_input(struct temperature_sensor * sensor, int * raw_analog_input)
{
  int error_code = sensor->raw_read(sensor->pins, raw_analog_input);

  // Fatal error if the sensor is disconnected
  if (*raw_analog_input == 1023)
    error_code |= temperature_sensor_disconnected_error;

  return error_code;
}


/* Reads the given thermistor or thermocouple and stores it's value in the temperature int */
int read_temperature_sensor(struct temperature_sensor * sensor, int * temperature)
{
  int i; //iterator
  int error_code = 0;
  int raw_analog_input = 0;

  //Thermistor Vars
  int n_of_samples = 3;
  int raw = 0;

  short (* table)[2] = sensor->_thermistor_table;
  int length = sensor->_thermistor_table_length;


  switch(sensor->type)
  {
    case temperature_sensor_type_thermistor:

      for(i = 0; i < n_of_samples; i++)
      {
        error_code |= read_raw_temperature_sensor_input(sensor, &raw_analog_input);
        raw += raw_analog_input;
      }

      raw = raw / n_of_samples;

      // TODO: This should do a binary chop
      for (i=1; i<length; i++)
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
      *temperature = table[0][1];
      break;

    case temperature_sensor_type_ad595_thermocouple:
      error_code |= read_raw_temperature_sensor_input(sensor, &raw_analog_input);
      *temperature = ( 5.0 * (raw_analog_input) * 100.0) / 1024.0;
      break;

    case temperature_sensor_type_max6675_thermocouple:
      error_code |= read_raw_temperature_sensor_input(sensor, temperature);
      break;

    default:
      error_code |= temperature_sensor_invalid_error;
  }

  return error_code;
}

// TODO: calculate this in C for different valued resistors.
// RS thermistor 484-0149; EPCOS B57550G103J
// Made with createTemperatureLookup.py (http://svn.reprap.org/trunk/reprap/firmware/Arduino/utilities/createTemperatureLookup.py)
// ./createTemperatureLookup.py --r0=10000 --t0=25 --r1=0 --r2=4700 --beta=3480 --max-adc=1023
// r0: 10000
// t0: 25
// r1: 0
// r2: 4700
// beta: 3480
// max adc: 1023

short TEN_K_THERMISTOR_MAP[THERMISTOR_MAP_LENGTH][2] = {
     {1, 599},
     {54, 160},
     {107, 123},
     {160, 103},
     {213, 90},
     {266, 79},
     {319, 70},
     {372, 62},
     {425, 55},
     {478, 49},
     {531, 43},
     {584, 37},
     {637, 31},
     {690, 25},
     {743, 19},
     {796, 12},
     {849, 5},
     {902, -3},
     {955, -16},
     {1008, -42}
};

char * TEMPERATURE_SENSOR_NAMES[3] =
    {"thermistor", "AD595 thermocouple", "MAX6675 thermocouple"};
