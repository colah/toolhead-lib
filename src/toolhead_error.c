#include <stdio.h>
#include <stdlib.h>
#include "heater.h"
#include "temperature_sensor.h"
#include "toolhead_error.h"

char* heater_error_message(struct heater * h, int error_code)
{
  char* sensor_name = TEMPERATURE_SENSOR_NAMES[h->sensor->type];
  char* message ="";

  switch (error_code)
  {
  case 0:
    break;
  case heater_overheated_error:
    sprintf(message,
        "Temperature above thermal cutoff (extruder overheated) or sensor disconnected. Please fix and reset.");
    //    "Temperature (%f degrees C) above thermal cutoff (extruder overheated) or sensor disconnected. Please fix and reset.", h->current);
    break;
  case temperature_sensor_disconnected_error:
    sprintf(message, "%s disconnected or broken, please reconnect and reset.", sensor_name);
    break;
  case temperature_sensor_internal_error:
    sprintf(message,
        "%s internal error. Please verify that the %s is connected and not broken.", sensor_name, sensor_name);
    break;
  case temperature_sensor_not_supported:
    sprintf(message,
        "%s not supported at this time.", sensor_name);
    break;
  case heater_not_heating_sensor_error:
    sprintf(message,
        "Heater is on but the temperature is not increasing. Check that both the heater and %s are connected. (target: %G)", sensor_name, h->target);
    break;
  default:
    sprintf(message, "Unexpected error code %d, machine highly broken. Please restart machine.", error_code);
  }

  return message;
}

