#include "momo_config.h"
#include "memory.h"

MoMoState current_momo_state;
static unsigned long momo_config_memory_address = 0;

void init_momo_config( unsigned int subsection_index )
{
  momo_config_memory_address = MEMORY_SUBSECTION_ADDR( subsection_index );
  load_momo_state();
}

void reset_momo_state()
{
  // This is the first time we've run, initialize everything.
  current_momo_state.version = MOMO_VERSION;
  current_momo_state.registered = false;
  current_momo_state.event_log_created = false;
  // current_momo_state.last_reported = 0x0 // UNINITIALIZED
  // current_momo_state.event_log = 0x0 // UNINITIALIZED
}
void load_momo_state()
{
  mem_read( momo_config_memory_address, (BYTE*)&current_momo_state, sizeof(MoMoState) );
  if ( current_momo_state.version == 0xFFFF )
  {
    reset_momo_state();
  }
}
void save_momo_state()
{
  mem_clear_subsection( momo_config_memory_address );
  mem_write( momo_config_memory_address, (BYTE*)&current_momo_state, sizeof(MoMoState) );
}