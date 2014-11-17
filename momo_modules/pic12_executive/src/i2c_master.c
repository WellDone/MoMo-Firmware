#include "i2c.h"
#include "bus_master.h"

uint8 i2c_master_receivedata()
{
	if (i2c_received_data())
	{
		i2c_loadbuffer();
    	i2c_read();

		if (mib_state.curr_loc == mib_state.buffer_end)
		{
			i2c_send_nack();
			return 1;
		}
		else
			i2c_send_ack();
	}
	else
		i2c_begin_receive();

	return 0;
}

/*
 * Synchronously receive the number of bytes specifed in the mib_state.
 * return 1 if there was a colision during reception
 * return 0 otherwise.
 *
 */
uint8 i2c_master_receive_message()
{
    SET_BIT(mib_state.send_address, 0); //set read indication
    
    i2c_master_start_address();
    if (!DC)
    {
    	do
        {
    		if (i2c_master_receivedata() == 1)
                return i2c_wait_flag();

    	} while (i2c_wait_flag() == 0);
    }

    return 1;
}