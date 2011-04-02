#ifndef toolhead_h
#define toolhead_h

#include "temperature_sensor.h"
#include "heater.h"
#include "solenoid.h"
#include "toolhead_error.h"

/*enum components
{
  toolhead_heater,
  toolhead_fan,
  toolhead_coolant,
  toolhead_valve,
  motor
};*/

struct toolhead
{
  // Identifies which components this toolhead contains.
  //boolean component_flags;

  // Heater
  struct heater * heater; // Temperature control data

  // Valve
  struct solenoid * valve; // Valve solenoid control data

  // Fan
  struct solenoid * fan; // Valve solenoid control data

  // Coolant (TODO: how the fuck do CNC mill coolants work?)
  struct solenoid * coolant; // Valve solenoid control data

  // Motor
  int (* pump_motor)(void *, unsigned long);  // Updates the motor on each pump_toolhead call.
  void * motor_data; // Reserved for external use by the function pointed to by move_motor.
};

int pump_all_toolheads(unsigned long time);

int shutdown_all_toolheads();

/*
 * adds and initialises the toolhead (todo: init)
 */
void add_toolhead(struct toolhead * t);

/*
 * removes the toolhead, shuts down it's components and frees it's memory. (todo: free/shutdown)
 */
void remove_toolhead(struct toolhead *t);

struct toolhead * get_toolhead(int index);

/*
 * Must be run before any toolhead components are set.
 */
void init_toolhead(struct toolhead *t);

#endif
