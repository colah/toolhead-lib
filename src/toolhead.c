#include "heater.h"
#include "toolhead.h"

void pump_toolhead(struct toolhead * t, unsigned long time)
{
  // Pumping the heater
  struct heater * h = t->heater;
  heater_pump(h, time);

  // Pumping the motor
  t->pump_motor(t->motor_data);

  // Pumping the solenoids

}
