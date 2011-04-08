#include <solenoid.h>

void set_solenoid(struct solenoid * s, int target_state)
{
  s->_target = target_state;
}
