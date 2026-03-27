#ifndef LDR_HANDLER_H
#define LDR_HANDLER_H

#define PIN_LDR 26

void  ldr_init(void);

float ldr_read_percent(void);

#endif