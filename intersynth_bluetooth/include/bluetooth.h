//
// Created by star on 6.3.2023.
//

#ifndef INTERSYNTH_BLUETOOTH_BLUETOOTH_H
#define INTERSYNTH_BLUETOOTH_BLUETOOTH_H

#include <winsock2.h>
#include <ws2bth.h>
#include <bthsdpdef.h>
#include <bthdef.h>
#include <BluetoothAPIs.h>
typedef struct {
    int socket;
    SOCKADDR_BTH addr;
} intersynth_bluetooth_t;

static intersynth_bluetooth_t bluetooth_handler;
void intersynth_init_bluetooth(void);
void intersynth_deinit_bluetooth(void);
//BLUETOOTH SETUP
void intersynth_scan(void);
void intersynth_free_scan(void);

void intersynth_get_rssi(void);
void intersynth_connect(char btaddr[18]);
void intersynth_disconnect(void);

//BLUETOOTH SEND
void intersynth_send(char *data, int length); //In the future add a possibility to choose a socket?



#endif //INTERSYNTH_BLUETOOTH_BLUETOOTH_H