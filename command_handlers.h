/* 
 * File:   command_handlers.h
 * Author: Austin
 *
 * Created on March 14, 2013, 5:10 AM
 */

#ifndef __command_handlers_h__
#define	__command_handlers_h__

void handle_echo_params(command_params *params);
void handle_device(command_params *params);
void handle_rtcc(command_params *params);
void handle_gsm(command_params *params);
void handle_sensor(command_params *params);
void handle_memory(command_params *params);

#endif	/* __command_handlers_h__ */

