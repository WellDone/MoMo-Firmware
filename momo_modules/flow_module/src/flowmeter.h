//flow_meter.h

#ifndef __flowmeter_h__
#define __flowmeter_h__

#include "platform.h"

void flowmeter_set_enabled(uint8 enabled);
void flowmeter_startsampling();
void flowmeter_endsampling();
void flowmeter_singlesample(uint8 sampletime);


//MIB Endpoints
void flowmeter_statuscmd();
void flowmeter_readcmd();
void flowmeter_oneshotcmd();
#endif