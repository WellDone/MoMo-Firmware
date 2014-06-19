#define MOMO_STATE_CONTROLLER
#include "momo_config.h"
#include "memory.h"
#include "scheduler.h"
#include "flashblock.h"

MoMoState               current_momo_state;
static flash_block_info config_block;
static ScheduledTask    flush_config_task;

typedef enum {
  kClean = 0,
  kDirty = 1
} ConfigState;
volatile ConfigState config_state;

void init_momo_config( unsigned int subsection_index )
{
  fb_init( &config_block, subsection_index, sizeof(MoMoState) );
  load_momo_state();
}

void reset_momo_state()
{
  current_momo_state.registered = false;
  init_report_config();
  config_state = kClean;
  save_momo_state();
}
void load_momo_state()
{
  if ( fb_count( &config_block ) == 0 )
    reset_momo_state();
  else
    fb_read( &config_block, &current_momo_state );
}

void flush_config_to_memory( void* arg )
{
  //There could be a race condition here
  fb_write( &config_block, &current_momo_state );
  config_state = kClean;
}

void save_momo_state()
{
  if ( config_state == kClean )
    scheduler_schedule_task( flush_config_to_memory, kEveryHalfSecond, 1, &flush_config_task, NULL );
  config_state = kDirty;
}