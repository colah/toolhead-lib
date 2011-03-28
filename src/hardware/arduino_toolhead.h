#ifndef ARDUINO_TOOLHEAD_H
#define ARDUINO_TOOLHEAD_H

// The pin indexes for the max6755 in the temperature_sensor->pins array
enum max6755_pins
{
  max6755_sck = 0,
  max6755_ss = 1,
  max6755_miso = 2
};

int init_heater_pins(int * heater_pins, int * sensor_pin);
int shutdown_heater_pins(int * heater_pins, int * sensor_pin);

int init_analog_thermal_sensor_pin(int * pin);
int shutdown_analog_thermal_sensor_pin(int * pin);

int read_analog_thermal_sensor(int * pin, int * temperature);
int read_max6675(int * pins, int * temperature);


#endif
