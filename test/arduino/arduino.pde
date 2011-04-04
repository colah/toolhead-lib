extern "C" {
  #include "arduino.h"
  #include <temperature_sensor.h>
  #include <solenoid.h>
  #include <heater.h>
  #include <toolhead.h>
  #include <toolhead_error.h>
  #include <hardware/arduino_toolhead.h>
}

#include <hardware/arduino_toolhead.c>

struct temperature_sensor temp;
struct heater heat;
struct toolhead tool;
struct solenoid noid;

enum
{
  AD595 = 'a',
  THERMISTOR = 't',
  HEATER_NONE = '0',
  HEATER_P = 'p',
  HEATER_I = 'i',
  HEATER_D = 'd',
  HEATER_ALL = '3',
  SOLENOID_PUMP = 's',
  SOLENOID_FLIP = 'S',
  SOLENOID_INVERT = 'n'
};


int temp_pin = A0;
int heater_pin = 13;
int valve_pin = 19;

int mode = AD595;

void setup()
{
  // Toolhead setup
  init_toolhead(&tool);
  tool.heater = &heat;
  tool.valve = &noid;

  //Valve setup
  noid.pin_config = &valve_pin;
  init_solenoid_hardware(&noid);

  // Heater setup
  heater_init(&heat, millis());
  heat.sensor = &temp;
  *(heat.heater_pins) = heater_pin;
  heat.thermal_cutoff = 300;
  heat.target = 100;

  //TODO: these should be implicit.
  heat.init_heater_pins = init_heater_pins;
  heat.write_heater_pins = write_heater_pins;
  heat.shutdown_heater_pins = shutdown_heater_pins;

  // Temperature sensor setup
  init_temperature_sensor(&temp);
  temp.pins = (int*)malloc(sizeof(int));
  *(temp.pins) = A0;
  temp.raw_read = read_analog_thermal_sensor;
  init_analog_thermal_sensor_pin(temp.pins); //TODO: this sucks as a solution
  Serial.begin(9600);

  Serial.println("setup complete");
}

void printError(int error)
{
  if (error != 0)
  {
      Serial.print(" error: ");
      char * error_msg = heater_error_message(&temp, error);
      Serial.print(error_msg);
      free(error_msg);
  }
}

void test_analog_sensor()
{
  int reading;
  int error = read_temperature_sensor(&temp, &reading);
  Serial.print(TEMPERATURE_SENSOR_NAMES[temp.type]);
  Serial.print(" reading: ");
  Serial.print(reading);
  printError(error);
  Serial.println();
}

void test_ad595()
{
  temp.type = temperature_sensor_type_ad595_thermocouple;
  test_analog_sensor();
}

void test_thermistor()
{
  temp.type = temperature_sensor_type_thermistor;
  test_analog_sensor();
}

void test_heater()
{
  Serial.print("pumping heater.. ");
  Serial.print(" temperature: ");
  Serial.print(heat.current);
  Serial.print(" instant temperature: ");
  Serial.print(heat.instantaneous);
  Serial.print(" target: ");
  Serial.print(heat.target);
  Serial.print(" at_target: ");
  Serial.print( (heat.at_target==1)? "true" : "false" );
  Serial.println();
  
  Serial.print("_last_sensor_heating: ");
  Serial.print(heat._last_sensor_heating);
  Serial.println();

  Serial.print("p: ");
  Serial.print(heat.pid_gains[pid_p]);
  Serial.print("  i: ");
  Serial.print(heat.pid_gains[pid_i]);
  Serial.print("  d: ");
  Serial.print(heat.pid_gains[pid_d]);
  Serial.print("  (gains)");
  Serial.println();

  int error = 0;
  int dt = heater_pump(&heat, millis());
  Serial.println(dt);

  Serial.print("p: ");
  Serial.print(heat.pid_values[pid_p]);
  Serial.print("  i: ");
  Serial.print(heat.pid_values[pid_i]);
  Serial.print("  d: ");
  Serial.print(heat.pid_values[pid_d]);
  Serial.print("  (values)");
  Serial.println();

  Serial.println();
  printError(error);
  Serial.println();
}

void test_solenoid_status()
{
  //Print out header for status.
  Serial.print("Solenoid Status.. ");
  Serial.print("State: ");
  //Print current state.
  Serial.print(noid._state);
  Serial.print(" Inverted: ");
  Serial.println(noid.invert_output);
}

void test_solenoid_flip()
{
  Serial.print("Setting solenoid target state to: ");
  set_solenoid(!noid._state);
  Serial.println(noid._target);
}

void test_solenoid_invert()
{
  //Invert the solenoid output modus operandi.
  Serial.println("Inverting solenoid voltage output modus operandi");
  noid.invert_output = !noid.invert_output;
}

void test_solenoid_pump()
{
  Serial.print("pumping solenoid.. ");
  pump_solenoid(&noid);
}


void loop()
{
  Serial.println("loop");
  delay(1000);
  while (Serial.available() > 0) {
    int new_char = Serial.read();
    if (new_char != '\n') mode = new_char;
  }
  
  int i = 0;
  for (i = 0; i<PID_LENGTH; i++)
  {
    heat.pid_gains[i] = 0.0;
  }

  
  switch(mode)
  {
    case AD595:
    {
      test_ad595();
      break;
    }
    case THERMISTOR:
    {
      test_thermistor();
      break;
    }
    case HEATER_NONE:
    {
      test_heater();
      break;
    }
    case HEATER_P:
    {
      heat.pid_gains[pid_p] = 1;
      test_heater();
      break;
    }
    case HEATER_I:
    {
      heat.pid_gains[pid_i] = 0.1;
      test_heater();
      break;
    }
    case HEATER_D:
    {
      heat.pid_gains[pid_d] = 10;
      test_heater();
      break;
    }
    case HEATER_ALL:
    {
      heat.pid_gains[pid_p] = 1;
      heat.pid_gains[pid_i] = 0.1;
      heat.pid_gains[pid_d] = 10;
      test_heater();
      break;
    }
    case SOLENOID_FLIP:
    case SOLENOID_INVERT:
    case SOLENOID_PUMP:
    {
      test_solenoid_status();
      if(mode == SOLENOID_FLIP)
      {
        test_solenoid_flip();
        mode = SOLENOID_PUMP;
      }
      else if (mode == SOLENOID_INVERT)
      {
        test_solenoid_invert();
        mode = SOLENOID_PUMP;
      }
      test_solenoid_pump();
      break;
    }
  }
}

