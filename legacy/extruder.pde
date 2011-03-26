

#include "configuration.h"
#include "pins.h"
#include "intercom.h"
#include "extruder.h" 
#include "heater.h" 



// Select a new extruder

void newExtruder(byte e)
{
  if(e < 0)
    return;
  if(e >= EXTRUDER_COUNT)
    return;

  if(e != extruder_in_use)
  {  
    extruder_in_use = e;
    setUnits(cdda[0]->get_units());
  }
}

//*************************************************************************

// Extruder functions that are the same for all extruders.

void extruder::waitForTemperature()
{
  while (heater->at_target == false)
  {
    for(int i = 0; i<1000; i++)
    {
      manage();
      delay(1);
    }
  }
}


/***************************************************************************************************************************
 * 
 * Arduino Mega motherboard
 */



//*******************************************************************************************

// Motherboard 3 - Arduino Mega

extruder::extruder(byte stp, byte dir, byte en, byte heat, byte temp, float spm, int t_cutoff)
{

 // extruder heater settings
	// Extruder Heater
	heater_init(heater);
	heater->heater_pin = (int)heat;

	heater->pid_gains[pid_p] = E_TEMP_PID_PGAIN;
	heater->pid_gains[pid_i] = E_TEMP_PID_IGAIN;
	heater->pid_gains[pid_d] = E_TEMP_PID_DGAIN;
	
	// Extruder Temperature Sensor
	heater->sensor_pin = (int)temp;
	heater->sensor = EXTRUDER_TEMPERATURE_SENSOR;
	heater->thermal_cutoff = t_cutoff;
	heater->thermister_temp_table = temp_table;


  motor_step_pin = stp;
  motor_dir_pin = dir;
  motor_en_pin = en;
  sPerMM = spm;
  manageCount = 0;
  
  //fan_pin = ;

  //setup our pins
  pinMode(motor_step_pin, OUTPUT);
  pinMode(motor_dir_pin, OUTPUT);
  pinMode(motor_en_pin, OUTPUT);
  
  disableStep();

  //initialize values
  digitalWrite(motor_dir_pin, 1);
  digitalWrite(motor_step_pin, 0);


#ifdef PASTE_EXTRUDER
  valve_dir_pin = vd_pin;
  valve_en_pin = ve_pin;
  pinMode(valve_dir_pin, OUTPUT); 
  pinMode(valve_en_pin, OUTPUT);
  digitalWrite(valve_dir_pin, false);
  digitalWrite(valve_en_pin, 0);
  valve_open = false;
#endif
}

int extruder::controlTemperature()
{
  return heater_update(heater);
}



int extruder::slowManage()
{
  manageCount = 0;

  return controlTemperature();
}

int extruder::manage()
{
  
#ifdef PASTE_EXTRUDER
  valveMonitor();
#endif

  manageCount++;
  if(manageCount > SLOW_CLOCK)
    return slowManage();
    
  return 0;
}



// Stop everything

void extruder::shutdown()
{
  // Heater off;
  heater_shutdown(heater);
  // Motor off
  digitalWrite(motor_en_pin, !ENABLE_ON);
  // Close valve
#ifdef PASTE_EXTRUDER
  valveSet(false, 500);
#endif
}



void extruder::setDirection(bool direction)
{
  digitalWrite(motor_dir_pin, direction);  
}

void extruder::setCooler(byte e_speed)
{
  //analogWrite(fan_pin, e_speed);   
}

void extruder::setTemperature(float tp)
{
  heater_reset(heater);
  heater->target = tp;
}

float extruder::getTemperature()
{
  return heater->current;
}



void extruder::enableStep()
{
    digitalWrite(motor_en_pin, ENABLE_ON);
}

void extruder::disableStep()
{
#if DISABLE_E
    digitalWrite(motor_en_pin, !ENABLE_ON);
#endif
}


void extruder::valveSet(bool closed, int dTime)
{
#ifdef PASTE_EXTRUDER
  requiredValveState = closed;
  kickStartValve();
#endif
}

#ifdef PASTE_EXTRUDER

bool extruder::valveTimeCheck(int millisecs)
{
  if(valveAlreadyRunning)
  {
    if(millis() >= valveEndTime)
    {
      valveAlreadyRunning = false;
      return true;
    }
    return false;
  }

  valveEndTime = millis() + millisecs*MILLI_CORRECTION;
  valveAlreadyRunning = true;
  return false;
}

void extruder::valveTurn(bool close)
{
  if(valveAtEnd)
    return;
    
  byte valveRunningState = VALVE_STARTING;
  if(digitalRead(OPTO_PIN))
  {
    seenHighLow = true;
    valveRunningState = VALVE_RUNNING;
  } else
  {
    if(!seenHighLow)
     valveRunningState = VALVE_STARTING;
    else
     valveRunningState = VALVE_STOPPING; 
  }    
   
  switch(valveRunningState)
  {
  case VALVE_STARTING: 
          if(close)
             digitalWrite(H1D, 1);
          else
             digitalWrite(H1D, 0);
          digitalWrite(H1E, HIGH);
          break;
          
  case VALVE_RUNNING:
          return;
  
  case VALVE_STOPPING:
          if(close)
            digitalWrite(H1D, 0);
          else
            digitalWrite(H1D, 1);
            
          if(!valveTimeCheck(10))
            return;
            
          digitalWrite(H1E, LOW);
          valveState = close;
          valveAtEnd = true;
          seenHighLow = false;
          break;
          
  default:
          break;
  }  
}

void extruder::valveMonitor()
{
  if(valveState == requiredValveState)
    return;
  valveAtEnd = false;
  valveTurn(requiredValveState);
} 

void extruder::kickStartValve()
{
  if(digitalRead(OPTO_PIN))
  {
     if(requiredValveState)
       digitalWrite(H1D, 1);
     else
       digitalWrite(H1D, 0);
     digitalWrite(H1E, HIGH);    
  }
} 
#endif
