#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

enum temperature_sensor_type {
  temperature_sensor_type_thermistor = 0,
  temperature_sensor_type_ad595_thermocouple = 1,
  temperature_sensor_type_max6675_thermocouple = 2
};

// The pin indexes for the max6755 in the temperature_sensor->pins array
enum max6755_pins
{
  max6755_sck = 0,
  max6755_ss = 1,
  max6755_miso = 2
};

static char * TEMPERATURE_SENSOR_NAMES[3] = {"thermistor", "AD595 thermocouple", "MAX6675 thermocouple"};

struct temperature_sensor
{
  int * pins; // sensor pins (array of pins or pointer to the sensor pin)
  enum temperature_sensor_type type;

  // Thermistor and AD595 analog read function
  int (*analog_read)(int);
  // Thermistor temperature lookup table
  struct thermistor_temperature_table * table;
};

// Reads the temperature sensor's value in degrees celsius to the temperature variable
int read_temperature_sensor(struct temperature_sensor * sensor, int * temperature);

#endif
