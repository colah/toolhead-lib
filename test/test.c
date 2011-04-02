#include "../src/toolhead.h"
#include <stdio.h>
#include <stdlib.h>

struct temperature_sensor * sensor;
struct heater * h;
int pin_value;

int test_analog_read(int * pin, int * temperature)
{
  *temperature = sensor->pins[0];
  return 0;
}


void verify_test(char* test_name, float actual, float expected)
{
  if (actual != expected)
  {
    fprintf(stderr, "FAILURE: %s   (actual: %f, expected: %f)\n", test_name, actual, expected);
  }
  else
  {
    printf("SUCCESS: %s\n", test_name);
  }
}


void test_temperature_sensors()
{
  sensor = malloc(sizeof(struct temperature_sensor));
  init_temperature_sensor(sensor);

  pin_value = 0;
  sensor->pins = &pin_value;
  sensor->raw_read = &test_analog_read;

  int temperature;

  // Thermistor
  // =========================

  sensor->type = temperature_sensor_type_thermistor;

  // Thermistor Test 1: normal operation
  pin_value = 100;
  verify_test(
    "Thermistor Test 1: Normal Operation",
    read_temperature_sensor(sensor, &temperature),
    0
  );


  // Thermistor Test 2: thermistor disconnected
  pin_value = 1023;
  verify_test(
    "Thermistor Test 2: Thermistor Disconnected",
    read_temperature_sensor(sensor, &temperature),
    temperature_sensor_disconnected_error
  );


  // AD595 Thermocouple
  // =========================

  sensor->type = temperature_sensor_type_ad595_thermocouple;

  //AD595 Test 1: normal operation
  pin_value = 100;
  verify_test(
    "AD595 Test 1: Normal Operation",
    read_temperature_sensor(sensor, &temperature),
    0
  );


  //AD595 Test 2: thermocouple disconnected from ad595 or ad595 disconnected from MCU
  pin_value = 1023;
  verify_test(
    "AD595 Test 2: Thermocouple disconnected from AD595 or ad595 disconnected from MCU",
    read_temperature_sensor(sensor, &temperature),
    temperature_sensor_disconnected_error
  );


  // TODO: MAX6755 Thermocouple
  // =========================

}


void test_heater()
{
  // init heater
  h = malloc(sizeof(struct heater));
  heater_init(h, (unsigned long)0);

  h->sensor = sensor;
  sensor->type = temperature_sensor_type_thermistor;

  h->target = 100;
  h->thermal_cutoff = 200;

  // normal heater operation
  pin_value = 20;
  h->pid_gains[pid_p] = 1;
  h->pid_gains[pid_i] = 50;
  h->pid_gains[pid_d] = 1;

  int instantaneous = 0;
  read_temperature_sensor(sensor, &instantaneous);
  float average = ( (float)instantaneous)/HEATER_HISTORY_LENGTH;

  unsigned long time = 10;
  float time_millis = (((int)time)*0.001);

  float error = 100 - instantaneous;
  float derivative = error / time_millis;
  float integral = time_millis * error;

  heater_pump(h, time);

  verify_test("Normal Heater Operation: Averaged Current Value", h->current, average);
  verify_test("Normal Heater Operation: Instantaneous Value", h->instantaneous, instantaneous);
  verify_test("Normal Heater Operation: P", h->pid_values[pid_p], error);
  verify_test("Normal Heater Operation: I", h->pid_values[pid_i], integral);
  verify_test("Normal Heater Operation: D", h->pid_values[pid_d], derivative);

}


int main(void)
{
  test_temperature_sensors();
  test_heater();

  return 0;
}
