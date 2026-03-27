#ifndef WIFI_HTTP_H
#define WIFI_HTTP_H

#include <stdbool.h>

#define WIFI_SSID      "SHARE-RESIDENTE"
#define WIFI_PASSWORD  "Share@residente23"
#define BACKEND_IP     "10.254.20.74"
#define BACKEND_PORT   8088
#define BACKEND_PATH   "/dados"

typedef struct {
    bool done;
    bool success;
} http_state_t;

bool wifi_connect(void);

bool http_post(const char *json_body);

#endif