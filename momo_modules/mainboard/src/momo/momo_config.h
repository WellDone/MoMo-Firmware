#ifndef __momo_config_h__
#define __momo_config_h__

#include "platform.h"
#include "rtcc.h"
#include "flash_queue.h"
#include "report_manager.h"

typedef struct {
  uint16              state_flags;
  uint32              uuid;
  ReportConfiguration report_config;
} MoMoState;

typedef enum {
	kStateFlagReportingEnabled = 0b0001
} MoMoStateFlag;



#ifndef MOMO_STATE_CONTROLLER
extern MoMoState current_momo_state;
#endif

void init_momo_config( unsigned int subsection_index );
void reset_momo_state();
void save_momo_state();
void load_momo_state();
void flush_config_to_memory( void* );

bool get_momo_state_flag( MoMoStateFlag flag );
void set_momo_state_flag( MoMoStateFlag flag, bool value );

#endif