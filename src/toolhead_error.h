#ifndef toolhead_error_h
#define toolhead_error_h

enum toolhead_errors
{
  heater_overheated_error = 1<<0,
  heater_not_heating_sensor_error = 1<<1,

  temperature_sensor_disconnected_error = 1<<2,
  temperature_sensor_internal_error = 1<<3,
  temperature_sensor_invalid_error = 1<<4,
  temperature_sensor_not_supported = 1<<5
};

#endif
