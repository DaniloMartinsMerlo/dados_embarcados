#include "ldr_handler.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"

void ldr_init(void) {
    adc_init();
    adc_gpio_init(PIN_LDR);
    adc_select_input(0);
}

float ldr_read_percent(void) {
    uint32_t sum = 0;
    for (int i = 0; i < 8; i++) {
        sum += adc_read();
        sleep_ms(2);
    }
    float avg = sum / 8.0f;
    return (avg / 4095.0f) * 100.0f;
}