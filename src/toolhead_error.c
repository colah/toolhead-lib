#include <stdio.h>
#include <stdlib.h>
#include "heater.h"
#include "temperature_sensor.h"
#include "toolhead_error.h"

enum
{
  HEATER_MESSAGE_SIZE = 512
};

char* heater_error_message(struct temperature_sensor * s, int error_code)
{
  char* sensor_name = TEMPERATURE_SENSOR_NAMES[s->type];
  char* message = malloc(HEATER_MESSAGE_SIZE);
  int valid_error = 0;

  if (error_code == 0)
    return message;

  if (error_code & heater_overheated_error)
  {
    sprintf(message,
        "Temperature above thermal cutoff (extruder overheated) or sensor disconnected. Please fix and reset.");
    //    "Temperature (%f degrees C) above thermal cutoff (extruder overheated) or sensor disconnected. Please fix and reset.", h->current);
    valid_error = 1;
  }

  if (error_code & temperature_sensor_disconnected_error)
  {
    sprintf(message, "%s disconnected or broken, please reconnect and reset.", sensor_name);
    valid_error = 1;
  }
  if (error_code & temperature_sensor_internal_error)
  {
    sprintf(message,
        "%s internal error. Please verify that the %s is connected and not broken.", sensor_name, sensor_name);
    valid_error = 1;
  }

  if (error_code & temperature_sensor_not_supported)
  {
    sprintf(message,
        "%s not supported at this time.", sensor_name);
    valid_error = 1;
  }

  if (error_code & heater_not_heating_sensor_error)
  {
    sprintf(message,
        "Heater is on but the temperature is not increasing. Check that both the heater and %s are connected.", sensor_name);
    valid_error = 1;
  }

  if (valid_error == 0)
    sprintf(message, "Unexpected error code %d, machine highly broken. Please restart machine.", error_code);

  return message;
}

