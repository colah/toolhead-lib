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

void setup()
{
  // Toolhead setup
  init_toolhead(&tool);
  tool.heater = &heat;

  // Heater setup
  heat.sensor = &temp;

  // Temperature sensor setup
  int temp_pin = A0;
  init_temperature_sensor(&temp);
  temp.pins = &temp_pin;
  temp.raw_read = read_analog_thermal_sensor;
  init_analog_thermal_sensor_pin(temp.pins); //TODO: this sucks as a solution
  digitalWrite(A0, HIGH);
  Serial.begin(9600); 

  Serial.println("setup complete");

}

void loop()
{
  Serial.println("loop");
  delay(500);
  test_ad595_thermocouple();
}

void test_thermistor()
{
  temp.type = temperature_sensor_type_thermistor;

}

void test_ad595_thermocouple()
{
  temp.type = temperature_sensor_type_ad595_thermocouple;
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
  //int raw_analog_input = analogRead(A0);
  //Serial.println(raw_analog_input);
  //Serial.println(( 5.0 * (raw_analog_input) * 100.0) / 1024.0);
}
