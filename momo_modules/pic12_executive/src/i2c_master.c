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

uint8 i2c_wait_flag()
{
    while (SSP1IF == 0)
    {
        if (BCL1IF == 1)
            return 1;
    }

    SSP1IF = 0;

    return 0;
}

uint8 i2c_master_start_address()
{
    i2c_start_transmission();

    if (i2c_wait_flag() == 0)
    {
        SSP1BUF = mib_state.send_address;

        if(i2c_wait_flag() == 0)
            return 0;

        return 1;
    }

    return 1;
}

void i2c_start_transmission()
{
    SSP1IF = 0;
    //Reads are always repeated starts, writes are always starts
    if (mib_state.send_address & 1)
        i2c_send_repeatedstart();
    else
        i2c_send_start();
}

void i2c_finish_transmission()
{
    SSP1IF = 0;
    i2c_send_stop();

    i2c_wait_flag(); //eat errors when we're sending a stop since we're going to either restart from scratch or be done.
}

/*
 * Synchronously send the number of bytes specifed in the mib_state.
 * return 1 if there was a collision during reception
 * return 0 otherwise.
 *
 */
uint8 i2c_master_send_message()
{
    CLEAR_BIT(mib_state.send_address, 0); //set write indication
    if (i2c_master_start_address() == 0)
    {
    	while(mib_state.curr_loc != mib_state.buffer_end)
    	{
    		i2c_loadbuffer();
    		i2c_write();

            if (i2c_wait_flag() == 1)
                return 1;
    	}

        return 0;
    }

    return 1;
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
    
    if (i2c_master_start_address() == 0)
    {
    	do
        {
    		if (i2c_master_receivedata() == 1)
            {
                 if (i2c_wait_flag() == 1)
                    return 1;

    			return 0;
            }
    	} while (i2c_wait_flag() == 0);

    	return 1;
    }

    return 1;
}