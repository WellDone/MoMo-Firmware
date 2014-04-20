/*
 * battery.h
 * Module for managing battery charge state and charging it if needed
 */

#ifndef __battery_h__
#define __battery_h__

#include "common.h"

#define kBatteryChargedLevel 	758 //This is 4.15V / 2.8V * 1024
#define kBatteryHysteresisLevel	731 //This is 4V / 2.8V * 1024

#define disable_charging() 		LAT(CHARGE_ENABLE) = 0
#define enable_charging()		LAT(CHARGE_ENABLE) = 1

void battery_init();
void battery_callback();
void report_battery();

void battery_set_charging_allowed(int allowed);

#endif