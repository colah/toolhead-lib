#include "../src/toolhead.h"

/*
 * test_hardware.c
 *
 *  Created on: 2011-04-01
 *      Author: rob
 */

/* This must be provided by a hardware implementation.
 * Takes a configured solenoid struct and initialises its pins */
int init_solenoid_hardware(struct solenoid * s)
{
  return 0;
}

/* This must be provided by a hardware implementation. Updates the solenoid hardware
 * to switch it to the s->_target state */
int pump_solenoid(struct solenoid * s)
{
  return 0;
}

int shutdown_heater_pins(struct heater * h)
{
  return 0;
}
