/*
 * thermistor_tables.h
 *
 *  Created on: 2011-03-25
 *      Author: Rob Gilson
 */

#ifndef THERMISTOR_TABLES_H_
#define THERMISTOR_TABLES_H_

struct thermistor_temperature_table
{
  short ** table;
  int length;
};

#define TEN_K_THERMISTOR_LENGTH (20)
#define TWO_HUNDRED_K_THERMISTOR_LENGTH (30)

// Commonly used thermistor value -> deg. C lookup tables
static struct thermistor_temperature_table ten_k_thermistor_table;
static struct thermistor_temperature_table two_hundred_k_thermistor_table;

// Thermistor tables initialization
void init_thermistor_tables();


#endif /* THERMISTOR_TABLES_H_ */
