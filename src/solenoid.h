#ifndef SOLENOID_H
#define SOLENOID_H

enum
{
  SOLENOID_CLOSED,
  SOLENOID_OPEN
};

struct solenoid
{
  int * pin_config;
  int invert_output;
  int _state;
  int _target;
};

/* This must be provided by a hardware implementation.
 * Takes a configured solenoid struct and initialises its pins */
int init_solenoid_hardware(struct solenoid *);

/* This must be provided by a hardware implementation. Updates the solenoid hardware
 * to switch it to the s->_target state */
int pump_solenoid(struct solenoid *);

/*
 * Updates the solenoid's target state to the given target_state, closing or opening
 * the solenoid as required on the next pump_solenoid call.
 */
inline void set_solenoid(struct solenoid * s, int target_state)
{
  s->_target = target_state;
}

#endif
