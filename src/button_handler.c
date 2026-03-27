#include "button_handler.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

volatile bool button_state   = false;
volatile bool button_changed = false;
volatile uint32_t last_irq_ms    = 0;

static void button_irq_handler(uint gpio, uint32_t events) {
    uint32_t now = to_ms_since_boot(get_absolute_time());

    if (now - last_irq_ms < DEBOUNCE_MS){
        return;
    } 
    
    last_irq_ms = now;
    button_state = (events & GPIO_IRQ_EDGE_FALL) ? true : false;
    button_changed = true;
}

void button_init(void) {
    gpio_init(PIN_BUTTON);
    gpio_set_dir(PIN_BUTTON, GPIO_IN);
    gpio_pull_up(PIN_BUTTON);
    gpio_set_irq_enabled_with_callback(PIN_BUTTON, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &button_irq_handler);
}

bool button_get_state(void){ 
    return button_state; 
}

bool button_was_changed(void){ 
    return button_changed; 
}

void button_clear_changed(void){ 
    button_changed = false; 
}