#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#define THERMISTOR_MAP_LENGTH (20)

extern short TEN_K_THERMISTOR_MAP[THERMISTOR_MAP_LENGTH][2];
extern char * TEMPERATURE_SENSOR_NAMES[3];


enum temperature_sensor_type {
  temperature_sensor_type_thermistor = 0,
  temperature_sensor_type_ad595_thermocouple = 1,
  temperature_sensor_type_max6675_thermocouple = 2
};

struct temperature_sensor
{
  int * pins; // sensor pins (array of pins or pointer to the sensor pin)
  enum temperature_sensor_type type;

  // Thermistor and AD595 analog read function, max6755 spi read
  int (*raw_read)(int *, int *);

  // Thermistor temperature lookup table
  short (* _thermistor_table)[2];
  int _thermistor_table_length;
};

void init_temperature_sensor(struct temperature_sensor * sensor);

// Reads the temperature sensor's value in degrees celsius to the temperature variable
int read_temperature_sensor(struct temperature_sensor * sensor, int * temperature);

#endif
