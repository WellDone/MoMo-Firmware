#ifndef __momo_config_h__
#define __momo_config_h__

#include "rtcc.h"
#include "flash_queue.h"

#define MOMO_VERSION 4

typedef struct {
  unsigned int version;
  bool registered;
  rtcc_date last_reported;
  bool event_log_created;
  flash_queue event_log;
} MoMoState;

extern MoMoState current_momo_state;

void init_momo_config( unsigned int subsection_index );
void reset_momo_state();
void save_momo_state();
void load_momo_state();
void flush_config_to_memory();

#endif