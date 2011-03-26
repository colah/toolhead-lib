#include "thermistor_tables.h"

// RS thermistor 484-0149; EPCOS B57550G103J
// Made with createTemperatureLookup.py (http://svn.reprap.org/trunk/reprap/firmware/Arduino/utilities/createTemperatureLookup.py)
// ./createTemperatureLookup.py --r0=10000 --t0=25 --r1=0 --r2=4700 --beta=3480 --max-adc=1023
// r0: 10000
// t0: 25
// r1: 0
// r2: 4700
// beta: 3480
// max adc: 1023

short TEN_K_THERMISTOR_MAP[TEN_K_THERMISTOR_LENGTH][2] = {
     {1, 599},
     {54, 160},
     {107, 123},
     {160, 103},
     {213, 90},
     {266, 79},
     {319, 70},
     {372, 62},
     {425, 55},
     {478, 49},
     {531, 43},
     {584, 37},
     {637, 31},
     {690, 25},
     {743, 19},
     {796, 12},
     {849, 5},
     {902, -3},
     {955, -16},
     {1008, -42}
};


//Thermistor 204GT  200K Ohms at ambient 600 ohm ref resister, 5V
// Used by BitsFromBytes (3.0, 3.1)
// 1k5 Ohm to Vcc. In parallel with 4K7 Ohm to ground.

short TWO_HUNDRED_K_THERMISTOR_MAP[TWO_HUNDRED_K_THERMISTOR_LENGTH][2] = {
  {85,300},
  {97,290},
  {110,280},
  {126,270},
  {144,260},
  {165,250},
  {189,240},
  {216,230},
  {247,220},
  {281,210},
  {319,200},
  {359,190},
  {402,180},
  {446,170},
  {491,160},
  {535,150},
  {576,140},
  {614,130},
  {648,120},
  {677,110},
  {701,100},
  {721,90},
  {736,80},
  {748,70},
  {757,60},
  {763,50},
  {768,40},
  {771,30},
  {773,20},
  {774,10}
};

void init_thermistor_tables()
{
  ten_k_thermistor_table.length = TEN_K_THERMISTOR_LENGTH;
  ten_k_thermistor_table.table = (short **) &TEN_K_THERMISTOR_MAP;

  two_hundred_k_thermistor_table.length = TWO_HUNDRED_K_THERMISTOR_LENGTH;
  two_hundred_k_thermistor_table.table = (short **) &TWO_HUNDRED_K_THERMISTOR_MAP;
}
