#ifndef HEATER_H
#define HEATER_H


typedef enum { pid_p=0, pid_i=1, pid_d=2 } pid;
#define PID_LENGTH (3)
extern char PID_LETTERS[PID_LENGTH];

// the number of heater temperatures to average in the current value.
#define HEATER_HISTORY_LENGTH (5)

#define PID_INTEGRAL_LIMIT (200/0.15)

// The temperature control dead zone in deg C
#define HALF_DEAD_ZONE 5

#define WAIT_AT_TEMPERATURE 10


/* ----------------------
 * Heater data structure
 * ---------------------- */
struct heater
{

  // Heater Settings
  int * heater_pins;
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

  // Callback functions

  int (* init_heater_pins) (int *); // int * pins
  int (* write_heater_pins) (int *, char); //int * pins, byte value
  void (* shutdown_heater_pins) (int *); // int * pins

  /** Event called each time the temperature stabalizes at or slightly above the target temperature. */
  int (* target_temperature_listener)();

  // Private values
  float _previous_target;
  unsigned long _previous_time; // ms
  unsigned long _target_reached_at; // ms in the time the target temperature deadzone was reached or zero if outside the deadzone
  int _raw_analog_input;

  float _history[HEATER_HISTORY_LENGTH]; // previous temperature values
};


/* ----------------------
 * Heater functions
 * ---------------------- */

void heater_init(struct heater * h, unsigned long time);

int heater_pump(struct heater * h, unsigned long time);

void heater_reset(struct heater * h, unsigned long time);
void heater_shutdown(struct heater * h);

char* heater_error_message(struct heater * h, int error_code);

int shutdown_heater_pins(struct heater * h);

#endif
