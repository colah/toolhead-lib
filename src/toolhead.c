#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heater.h"
#include "solenoid.h"
#include "toolhead.h"


/* =======================
 * Toolhead Functions
 * ======================= */

int init_toolhead(struct toolhead * t)
{
  //BAD. Caller should malloc.
  //t = malloc(sizeof(struct toolhead));

  //Check the caller has malloced properly
  if(t == NULL)
  {
    //BADWRONG should never happen. Should barf.
    //TODO Nice error propogation.
    return 1;
  }

  t->heater = NULL;
  t->motor_data = NULL;
  t->pump_motor = NULL;
  t->valve = NULL;
  t->fan = NULL;
  t->coolant = NULL;

  return 0;
}

int pump_toolhead_solenoid(struct solenoid * s)
{
  if (s != NULL) return pump_solenoid(s);
  return 0;
}

int pump_toolhead(struct toolhead * t, unsigned long time)
{
  int error_code = 0;
  // Pumping the heater
  if (t->heater != NULL) error_code |= heater_pump(t->heater, time);

  // Pumping the motor
  if (t->pump_motor != NULL) error_code |= t->pump_motor(t->motor_data, time);

  // Pumping the solenoids
  error_code |= pump_toolhead_solenoid(t->valve);
  error_code |= pump_toolhead_solenoid(t->fan);
  error_code |= pump_toolhead_solenoid(t->coolant);

  return error_code;
}



/* ====================================
 * Toolhead Array Functions and Data
 * ==================================== */

struct
{
  struct toolhead ** array;
  int size;
  int alloc_size;
} toolhead_array;

int toolhead_array_init_flag = 0;


void init_toolhead_array()
{
  toolhead_array.alloc_size = 1;
  toolhead_array.size = 0;
  toolhead_array.array = malloc(sizeof(struct toolhead *));
  toolhead_array_init_flag = 1;
}


void add_toolhead(struct toolhead * t)
{
  if (toolhead_array_init_flag == 0) init_toolhead_array();

  // Growing the array if necessary
  if (toolhead_array.size + 1 > toolhead_array.alloc_size)
  {
      toolhead_array.alloc_size *= 2;
      struct toolhead ** old_array = toolhead_array.array;
      toolhead_array.array = calloc(toolhead_array.alloc_size, sizeof(struct toolhead *));

      memcpy(toolhead_array.array, old_array, toolhead_array.size * sizeof(struct toolhead *));

      free(old_array);
  }
  // Adding the new toolhead
  toolhead_array.array[toolhead_array.size] = t;
  toolhead_array.size++;
}


//TODO: fix memory leak: the toolhead_array alloc_size never shrinks.
// Note this memory leak will rarely be relevant because it will only grow to the max
// number of toolheads used simultaneously with the machine.
int remove_toolhead(struct toolhead * t)
{
  if (toolhead_array_init_flag == 0) init_toolhead_array();
  int offset = 0;
  int i;
  for(i=0; i<toolhead_array.size; i++)
  {
      if (toolhead_array.array[i] == t) offset = 1;
      if (offset != 0 && i != toolhead_array.size) {
        toolhead_array.array[i] = toolhead_array.array[i+offset];
        free(toolhead_array.array[i]);
        memmove(&toolhead_array.array[i], &toolhead_array.array[i + 1], toolhead_array.size - (i - 1));
        break;
      }
  }
  toolhead_array.size--;

  //Return if we successfully found the toolhead.
  return (!offset);
}


int pump_all_toolheads(unsigned long time)
{
  int error_code = 0;
  if (toolhead_array_init_flag == 0) init_toolhead_array();
  int i;
  for(i=0; i<toolhead_array.size; i++)
  {
      error_code |= pump_toolhead(toolhead_array.array[i], time);
  }
  return error_code;
}

struct toolhead * get_toolhead(int index)
{
  return (index > toolhead_array.size-1)? NULL : toolhead_array.array[index];
}

int shutdown_all_toolheads()
{
  int error_code = 0;
  int i;
  for(i=0; i<toolhead_array.size; i++)
  {
      struct toolhead * t = toolhead_array.array[i];
      error_code |= shutdown_heater_pins(t->heater);

      //TODO: more shutdown logic here for solenoids, etc.

      //TODO: Free toolheads
      free(t);
  }

  free(toolhead_array.array);

  return error_code;
}
