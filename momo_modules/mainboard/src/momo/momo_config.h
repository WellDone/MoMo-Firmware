#ifndef __momo_config_h__
#define __momo_config_h__

#include "rtcc.h"
#include "flash_queue.h"

typedef struct {
  bool registered;
} MoMoState;

extern MoMoState current_momo_state;

void init_momo_config( unsigned int subsection_index );
void reset_momo_state();
void save_momo_state();
void load_momo_state();
void flush_config_to_memory();

#endif