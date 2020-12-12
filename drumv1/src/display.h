#include "u8g2_esp32_hal.h"
#ifndef FUNCTIONS_DISPLAY_INCLUDED
#define FUNCTIONS_DISPLAY_INCLUDED
/* ^^ these are the include guards */

/* Prototypes for the functions */
int COUNTER;
//BaseType_t task_params;
u8g2_t display_struct;
void printValue(u8g2_t*);
void display_task(void *task_params);

#endif
