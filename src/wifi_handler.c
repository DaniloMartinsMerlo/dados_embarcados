#include "wifi_handler.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static err_t http_recv_cb(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err) {
    printf("[HTTP] recv_cb chamado\n");
    http_state_t *state = (http_state_t *)arg;
    if (p == NULL) { state->done = true; return ERR_OK; }
    u16_t len = p->tot_len;
    char buf[256] = {0};
    pbuf_copy_partial(p, buf, sizeof(buf) - 1, 0);
    printf("[HTTP] Resposta: %s\n", buf);
    if (strstr(buf, "200")) state->success = true;
    pbuf_free(p);
    tcp_recved(pcb, len);
    state->done = true;
    return ERR_OK;
}

static void http_err_cb(void *arg, err_t err) {
    printf("[HTTP] err_cb chamado: %d\n", err);
    http_state_t *state = (http_state_t *)arg;
    state->done    = true;
    state->success = false;
}

static err_t http_connected_cb(void *arg, struct tcp_pcb *pcb, err_t err) {
    printf("[HTTP] connected_cb chamado\n");  
    if (err != ERR_OK) { ((http_state_t *)arg)->done = true; return err; }

    const char *json_body = (const char *)((uint8_t *)arg + sizeof(http_state_t));
    char request[512];
    int  body_len = strlen(json_body);

    snprintf(request, sizeof(request),
        "POST %s HTTP/1.1\r\n"
        "Host: %s:%d\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        BACKEND_PATH, BACKEND_IP, BACKEND_PORT, body_len, json_body);

    tcp_write(pcb, request, strlen(request), TCP_WRITE_FLAG_COPY);
    tcp_output(pcb);
    return ERR_OK;
}

bool wifi_connect(void) {
    if (cyw43_arch_init()) {
        printf("[WIFI] Falha ao inicializar\n");
        return false;
    }
    cyw43_arch_enable_sta_mode();

    int retry = 0;
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000) != 0){
        printf("[WIFI] Tentativa %d falhou, reconectando...\n", ++retry);
        sleep_ms(2000);
    }
    printf("[WIFI] Conectado!\n");
    return true;
}

bool http_post(const char *json_body) {
    size_t  body_len = strlen(json_body) + 1;
    uint8_t *block   = calloc(sizeof(http_state_t) + body_len, 1);
    if (!block) return false;

    http_state_t *state = (http_state_t *)block;
    memcpy(block + sizeof(http_state_t), json_body, body_len);

    ip_addr_t server_ip;
    ipaddr_aton(BACKEND_IP, &server_ip);

    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_V4);
    if (!pcb) { free(block); return false; }

    tcp_arg(pcb, state);
    tcp_recv(pcb, http_recv_cb);
    tcp_err(pcb, http_err_cb);

    err_t err = tcp_connect(pcb, &server_ip, BACKEND_PORT, http_connected_cb);
    if (err != ERR_OK) { tcp_abort(pcb); free(block); return false; }

    uint32_t deadline = to_ms_since_boot(get_absolute_time()) + 5000;
    while (!state->done && to_ms_since_boot(get_absolute_time()) < deadline) {
        cyw43_arch_poll();
        sleep_ms(10);
    }

    bool ok = state->success;
    if (!state->done) {
        tcp_abort(pcb);
    } else {
        tcp_arg(pcb, NULL);
        tcp_recv(pcb, NULL);
        tcp_err(pcb, NULL);
        tcp_close(pcb);
    }

    free(block);
    sleep_ms(1000);
    cyw43_arch_poll();

    return ok;
}