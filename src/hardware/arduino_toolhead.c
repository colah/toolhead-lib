#include "arduino_toolhead.h"

#ifdef ARDUINO

// Heater
// =======================

int init_heater_pins(int * heater_pins)
{
  pinMode(*heater_pins, OUTPUT);
  analogWrite(*heater_pin, 0); // heater is off by default

  return 0;
}

int shutdown_heater_pins(int * heater_pins, int * sensor_pin)
{
  analogWrite(*heater_pin, 0); // turn off the heater!
  return 0;
}

// Sensors
// =======================

int init_analog_thermal_sensor_pin(int * pin)
{
   pinMode(*pin, INPUT);
   digitalWrite(*pin); // pull down resistor for the sensor input

   return 0;
}

int shutdown_analog_thermal_sensor_pin(int * pin)
{
}

int read_analog_thermal_sensor(int * pin, int * temperature)
{
  *temperature = analogRead(*pins);
  return 0;
}


//TODO: untested.
int read_max6675(int * pins, int * temperature)
{
  int value = 0;
  //TODO: non-arduino-specific max6675 implementation
  digitalWrite(pins[max6755_ss], 0); // Enable device

  /* Cycle the clock for dummy bit 15 */
  digitalWrite(pins[max6755_sck],1);
  digitalWrite(pins[max6755_sck],0);

  /* Read bits 14-3 from MAX6675 for the Temp
     Loop for each bit reading the value
   */
  for (int i=11; i>=0; i--)
  {
    digitalWrite(pins[max6755_sck],1);  // Set Clock to HIGH
    value += digitalRead(pins[max6755_miso]) << i;  // Read data and add it to our variable
    digitalWrite(pins[max6755_sck],0);  // Set Clock to LOW
  }

  /* Read the TC Input inp to check for TC Errors */
  digitalWrite(pins[max6755_sck],1); // Set Clock to HIGH
  int error_tc = digitalRead(pins[max6755_miso]); // Read data
  digitalWrite(pins[max6755_sck],0);  // Set Clock to LOW

  digitalWrite(pins[max6755_ss], 1); //Disable Device

  if(error_tc)
  {
    *temperature = 2000;
    if (h->target !=0) error_code |= temperature_sensor_internal_error;
  }
  else
    *temperature = value/4;
}

#endif
