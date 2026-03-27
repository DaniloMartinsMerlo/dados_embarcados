#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "inc/button_handler.h"
#include "inc/ldr_handler.h"
#include "inc/wifi_handler.h"

#define DEVICE_ID      "pico-01"
#define SEND_INTERVAL  5000

static void build_timestamp(char *buf, size_t len) {
    uint32_t s = to_ms_since_boot(get_absolute_time()) / 1000;
    uint32_t h = s / 3600; s %= 3600;
    uint32_t m = s / 60;   s %= 60;
    snprintf(buf, len, "2026-03-25T%02u:%02u:%02uZ", h % 24, m, s);
}

int main(void) {
    stdio_init_all();
    sleep_ms(2000);

    printf("[BOOT] Iniciando firmware...\n");

    ldr_init();
    button_init();
    wifi_connect();

    uint32_t last_send = 0;

    for(;;) {
        cyw43_arch_poll();

        uint32_t now = to_ms_since_boot(get_absolute_time());
        if (now - last_send >= SEND_INTERVAL) {
            last_send = now;
            float ldr  = ldr_read_percent();
            char ts[32], json[256];
            build_timestamp(ts, sizeof(ts));
            snprintf(json, sizeof(json),
                "{\"device_id\":\"%s\",\"timestamp\":\"%s\","
                "\"sensor_type\":\"ldr\",\"reading_type\":\"analogica\","
                "\"value\":\"%.2f\"}",
                DEVICE_ID, ts, ldr);
            printf("[LDR] %.2f%% → enviando...\n", ldr);
            printf("[JSON] %s\n", json);
            bool ok = http_post(json);
            if (!ok) { sleep_ms(1000); ok = http_post(json); }
            printf("[LDR] %s\n", ok ? "OK" : "FALHOU");
        }

        if (button_was_changed()) {
            button_clear_changed();
            char ts[32], json[256];
            build_timestamp(ts, sizeof(ts));
            snprintf(json, sizeof(json),
                "{\"device_id\":\"%s\",\"timestamp\":\"%s\","
                "\"sensor_type\":\"button\",\"reading_type\":\"discreto\","
                "\"value\":\"%s\"}",
                DEVICE_ID, ts, button_get_state() ? "1" : "0");
            printf("[BTN] %s → enviando...\n", button_get_state() ? "PRESSIONADO" : "SOLTO");
            bool ok = http_post(json);
            printf("[BTN] %s\n", ok ? "OK" : "FALHOU");
        }

        sleep_ms(10);
    }

    cyw43_arch_deinit();
    return 0;
}