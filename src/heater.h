#ifndef HEATER_H
#define HEATER_H


typedef enum { pid_p=0, pid_i=1, pid_d=2 } pid;
#define PID_LENGTH (3)
char PID_LETTERS[PID_LENGTH] = {'p', 'i', 'd'};

// the number of heater temperatures to average in the current value.
#define HEATER_HISTORY_LENGTH (5)

#define PID_INTEGRAL_LIMIT (200/0.15)

// The temperature control dead zone in deg C
#define HALF_DEAD_ZONE 5


/* ----------------------
 * Heater data structure
 * ---------------------- */
typedef struct
{

  // Heater Settings
  int heater_pin;
  float pid_gains[PID_LENGTH]; // configurable gain constants for each of proportional integral and derivative.

  // Temperature Sensor Settings
  struct temperature_sensor * sensor;
  float thermal_cutoff; // The degrees celcius at which the machine is shut down to prevent overheating.

  // Current values
  float target; // target temperature we are attempting to reach.
  float current; // averaged current temperature.
  float instantaneous; // instantaneous temperature reading (non-averaged).
  float pid_values[PID_LENGTH]; // proportional, integral and derivative values.

  int at_target; // flags that the a steady-state target temperature has been reached (+ or - dead zone / 2 )

  /** Event called each time the temperature stabalizes at or slightly above the target temperature. */
  int (*target_temperature_listener)();

  // Private values
  float _previous_target;
  unsigned long _previous_time; // ms
  unsigned long _target_reached_at; // ms in the time the target temperature deadzone was reached or zero if outside the deadzone
  int _raw_analog_input;

  float _history[HEATER_HISTORY_LENGTH]; // previous temperature values
} heater;


/* ----------------------
 * Heater functions
 * ---------------------- */

void heater_init(heater * h);

int heater_update(heater * h);

void heater_reset(heater * h);
void heater_shutdown(heater * h);

char* heater_error_message(heater * h, int error_code);

#endif
