#include "../temperature_sensor.h"
#include "../toolhead_error.h"
#include "../thermistor_tables.h"
#include <stdio.h>
#include <stdlib.h>

struct temperature_sensor sensor;

int test_analog_read(int pin)
{
  return sensor.pins[0];
}

void verify_test(char* test_name, int actual, int expected)
{
  if (actual != expected)
  {
    printf("FAILURE: %s", test_name);
  }
  else
  {
    printf("SUCCESS: %s", test_name);
  }
}

int main(void) {
  init_thermistor_tables();

  int pin_value = 0;
  sensor.pins = &pin_value;
  sensor.analog_read = &test_analog_read;

  int temperature;

  // Thermistor
  // =========================

  sensor.type = temperature_sensor_type_thermistor;
  sensor.table = &ten_k_thermistor_table;

  // Thermistor Test 1: normal operation
  pin_value = 100;
  verify_test(
    "Thermistor Test 1: Normal Operation",
    read_temperature_sensor(&sensor, &temperature),
    0
  );


  // Thermistor Test 2: thermistor disconnected
  pin_value = 1023;
  verify_test(
    "Thermistor Test 2: Thermistor Disconnected",
    read_temperature_sensor(&sensor, &temperature),
    temperature_sensor_disconnected_error
  );


  // AD595 Thermocouple
  // =========================

  sensor.type = temperature_sensor_type_ad595_thermocouple;
  sensor.table = 0;

  //AD595 Test 1: normal operation
  pin_value = 100;
  verify_test(
    "AD595 Test 1: Normal Operation",
    read_temperature_sensor(&sensor, &temperature),
    0
  );


  //AD595 Test 2: thermocouple disconnected from ad595 or ad595 disconnected from MCU
  pin_value = 1023;
  verify_test(
    "AD595 Test 2: Thermocouple disconnected from AD595 or ad595 disconnected from MCU",
    read_temperature_sensor(&sensor, &temperature),
    temperature_sensor_disconnected_error
  );


  // TODO: MAX6755 Thermocouple
  // =========================

}
