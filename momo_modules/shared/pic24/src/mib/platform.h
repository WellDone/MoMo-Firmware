/*
 * MIB Platform defines that control the degree of specialization/optimization of the i2c and mib code.
 * This lets us maintain a unified codebase while targeting both the pic24 and the much less powerful pic12.
 */

#ifndef __platform_h__
#define __platform_h__

//Optimize the i2c code by 
#define ALWAYS_ASSUME_BUS_MESSAGE

#endif