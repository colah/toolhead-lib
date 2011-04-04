#include "arduino_toolhead.h"
#include "../solenoid.h"
#include "../heater.h"

#ifdef ARDUINO

// Solenoid
// =======================
int init_solenoid_hardware(struct solenoid * s)
{
  pinMode(s->pin_config[0], OUTPUT);
  digitalWrite(s->pin_config[0], (s->invert_output != 0));
  return 0;
}

int pump_solenoid(struct solenoid * s)
{
  int target = s->_target;
  digitalWrite(s->pin_config[0], (s->invert_output != 0) ^ (target != 0));
  s->_state = target;
  return 0;
}


// Heater
// =======================

int init_heater_pins(int * heater_pin)
{
  pinMode(*heater_pin, OUTPUT);
  analogWrite(*heater_pin, 0); // heater is off by default

  return 0;
}

int write_heater_pins(int * heater_pin, char value)
{
  analogWrite(*heater_pin, value);
}

int shutdown_heater_pins(struct heater * h)
{
  analogWrite(*(h->heater_pins), 0); // turn off the heater!
  return 0;
}

// Sensors
// =======================

int init_analog_thermal_sensor_pin(int * pin)
{
   pinMode(*pin, INPUT);
   digitalWrite(*pin, HIGH); // pull up resistor for the sensor input

   return 0;
}

int shutdown_analog_thermal_sensor_pin(int * pin)
{
}

int read_analog_thermal_sensor(int * pin, int * temperature)
{
  analogRead(*pin);
  *temperature = analogRead(*pin);
  return 0;
}


//TODO: untested.
int read_max6675(struct heater * h, int * temperature)
{
  int value = 0;
  int * pins = h->heater_pins;
  int error_code = 0;
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
  return error_code;
}

#endif
