#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <stdbool.h>
#include <stdint.h>

#define PIN_BUTTON 15
#define DEBOUNCE_MS 50

void    button_init(void);

bool    button_get_state(void);

bool    button_was_changed(void);

void    button_clear_changed(void);

#endif