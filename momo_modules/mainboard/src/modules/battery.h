/*
 * battery.h
 * Module for managing battery charge state and charging it if needed
 */

#ifndef __battery_h__
#define __battery_h__

#include "common.h"
#include <stdint.h>

#define kBatteryChargedLevel 	758 //This is 4.15V / 2.8V * 1024
#define kBatteryHysteresisLevel	731 //This is 4V / 2.8V * 1024

#define disable_charging() 		DIR(CHARGE_ENABLE) = INPUT
#define enable_charging()		DIR(CHARGE_ENABLE) = OUTPUT

void battery_init();
void battery_callback( void* );
uint8_t report_battery(uint8_t length);

void battery_set_charging_allowed(int allowed);

#endif