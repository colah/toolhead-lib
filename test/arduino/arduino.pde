extern "C" {
  #include "arduino.h"
  #include <temperature_sensor.h>
  #include <heater.h>
  #include <toolhead.h>
  #include <toolhead_error.h>
  #include <hardware/arduino_toolhead.h>
}

#include <hardware/arduino_toolhead.c>

struct temperature_sensor temp;
struct heater heat;
struct toolhead tool;

enum
{
  AD595,
  THERMISTOR,
  HEATER_NONE,
  HEATER_P,
  HEATER_I,
  HEATER_D,
  HEATER_ALL
}


int temp_pin = A0;
int heater_pin = 13;

int mode = AD595;

void setup()
{
  // Toolhead setup
  init_toolhead(&tool);
  tool.heater = &heat;

  // Heater setup
  heater_init(&heat);
  heat.sensor = &temp;
  *(heat.heater_pins) = heater_pin;
  heat.thermal_cutoff = 300;

  //TODO: these should be implicit.
  heat.init_heater_pins = init_heater_pins;
  heat.write_heater_pins = write_heater_pins;
  heat.shutdown_heater_pins = shutdown_heater_pins;

  // Temperature sensor setup
  init_temperature_sensor(&temp);
  temp.pins = (int*)malloc(sizeof(int));
  *(temp.pins) = A0;
  temp.raw_read = read_analog_thermal_sensor;
  init_analog_thermal_sensor_pin(temp.pins); //TODO: this sucks as a solution
  Serial.begin(9600);

  Serial.println("setup complete");
}

void test_analog_sensor()
{
  int reading;
  int error = read_temperature_sensor(&temp, &reading);
  Serial.print("ad595 temperature reading:");
  Serial.print(reading);
  if (error != 0)
  {
      Serial.print(" error: ");
      Serial.print(heater_error_message(&temp, error));
  }
  Serial.println();
}

void test_ad595()
{
  temp.type = temperature_sensor_type_ad595_thermocouple;
  test_analog_sensor();
}

void test_thermistor()
{
  temp.type = temperature_sensor_type_thermistor;
  test_analog_sensor();
}

void test_heater()
{
  Serial.print("pumping heater.. ");
  int error = heater_pump(&heat);
  if (error != 0)
  {
      Serial.print(" error: ");
      Serial.print(heater_error_message(&temp, error));
  }
  Serial.println();
}


void loop()
{
  Serial.println("loop");
  delay(3000);
  
  int i = 0;
  for (i = 0; i<PID_LENGTH; i++)
    heat.pid_values[i] = 0.0;

  
  switch(mode)
  {
    case AD595:
    {
      test_ad595();
      break;
    }
    case THERMISTOR:
    {
      test_thermistor();
      break;
    case: HEATER_NONE
    {
      test_heater();
      break;
    }
    case: HEATER_P
    {
      heat.pid_values[p] = 1;
      test_heater();
      break;
    }
    case: HEATER_I
    {
      heat.pid_values[i] = 0.1;
      test_heater();
      break;
    }
    case: HEATER_D
    {
      heat.pid_values[d] = 10;
      test_heater();
      break;
    }
    case: HEATER_ALL
    {
      heat.pid_values[p] = 1;
      heat.pid_values[i] = 0.1;
      heat.pid_values[d] = 10;
      test_heater();
      break;
    }
  }
}

