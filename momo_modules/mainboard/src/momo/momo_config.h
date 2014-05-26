#ifndef __momo_config_h__
#define __momo_config_h__

#include "rtcc.h"
#include "flash_queue.h"
#include "report_manager.h"

typedef struct {
  bool                registered;
  ReportConfiguration report_config;
} MoMoState;

#ifndef MOMO_STATE_CONTROLLER
extern MoMoState current_momo_state;
#endif

void init_momo_config( unsigned int subsection_index );
void reset_momo_state();
void save_momo_state();
void load_momo_state();
void flush_config_to_memory( void* );

#endif