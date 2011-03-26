
// Class for controlling each extruder
//
// Adrian Bowyer 14 May 2009

#ifndef EXTRUDER_H
#define EXTRUDER_H



void manageAllExtruders();

void newExtruder(byte e);


class extruder
{

  public:
     extruder(byte step, byte dir, byte en, byte heat, byte temp, float spm, int t_cutoff);
     void waitForTemperature();
     
     void setDirection(bool direction);
     void setCooler(byte e_speed);
     void setTemperature(float temp);
     float getTemperature();
     int slowManage();
     int manage();
     void sStep();
     void enableStep();
     void disableStep();
     void shutdown();
     float stepsPerMM();
     int controlTemperature();   
     void valveSet(bool open, int dTime); 

     Heater* heater;    // Temperature control - extruder


  private:

     int count;
     float sPerMM;
     long manageCount;

    // The pins we control
     byte motor_step_pin, motor_dir_pin, heater_pin,  temp_pin,  motor_en_pin;

     //byte fan_pin;

#ifdef PASTE_EXTRUDER
     byte valve_dir_pin, valve_en_pin;
     bool valve_open;
#endif

};

inline void extruder::sStep()
{
	digitalWrite(motor_step_pin, HIGH);
	digitalWrite(motor_step_pin, LOW);  
}



//*********************************************************************************************************

extern extruder* ex[ ];
extern byte extruder_in_use;

inline float extruder::stepsPerMM()
{
  return sPerMM;
}

#endif
