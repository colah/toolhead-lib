#ifndef toolhead_h
#define toolhead_h

struct toolhead
{
  // Heater
  struct heater * heater; // Temperature control data

  // Valve
//  struct solenoid * valve; // Valve solenoid control data

  // Fan
//  struct solenoid * fan; // Valve solenoid control data

  // Coolant (TODO: how the fuck do CNC mill coolants work?)
//  struct solenoid * coolant; // Valve solenoid control data

  // Motor
  int (* pump_motor)(void *);  // Updates the motor on each pump_toolhead call.
  void * motor_data; // Reserved for external use by the function pointed to by move_motor.
};

void pump_toolhead(struct toolhead * t, unsigned long time);

#endif
