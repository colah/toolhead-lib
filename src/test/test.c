#include "../temperature_sensor.h"
#include "../heater.h"
#include "../toolhead_error.h"
#include <stdio.h>
#include <stdlib.h>

struct temperature_sensor sensor;
struct heater h;

int test_analog_read(int * pin, int * temperature)
{
  *temperature = sensor.pins[0];
  return 0;
}


void verify_test(char* test_name, int actual, int expected)
{
  if (actual != expected)
  {
    fprintf(stderr, "FAILURE: %s\n", test_name);
  }
  else
  {
    printf("SUCCESS: %s\n", test_name);
  }
}


void test_temperature_sensors()
{
  init_temperature_sensor(&sensor);

  int pin_value = 0;
  sensor.pins = &pin_value;
  sensor.raw_read = &test_analog_read;

  int temperature;

  // Thermistor
  // =========================

  sensor.type = temperature_sensor_type_thermistor;

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


void test_heater()
{
  // init heater
  h.pid_gains[pid_p] = 0;
  h.pid_gains[pid_i] = 0;
  h.pid_gains[pid_d] = 0;

  h.sensor = &sensor;
  sensor.type = temperature_sensor_type_thermistor;

  h.target = 100;
  h.thermal_cutoff = 200;

  heater_init(&h, 0);

  // normal heater operation
  sensor.pins[0] = 20;
  h.pid_gains[pid_p] = 1;
  h.pid_gains[pid_i] = 50;
  h.pid_gains[pid_d] = 1;

  int instantaneous = 0;
  read_temperature_sensor(&sensor, &instantaneous);
  int average = instantaneous/HEATER_HISTORY_LENGTH;

  unsigned long time = 10;

  int error = 100 - instantaneous;
  int integral = h.pid_gains[pid_i] * ((int)time) * error;

  heater_pump(&h, time);

  verify_test("Normal Heater Operation: Averaged Current Value", h.current, average);
  verify_test("Normal Heater Operation: Instantaneous Value", h.instantaneous, instantaneous);
  verify_test("Normal Heater Operation: P", h.pid_values[pid_p], error);
  verify_test("Normal Heater Operation: I", h.pid_values[pid_p], integral);

}


int main(void)
{
  test_temperature_sensors();
  test_heater();
}