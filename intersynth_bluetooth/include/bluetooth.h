//
// Created by star on 6.3.2023.
//

#ifndef INTERSYNTH_BLUETOOTH_BLUETOOTH_H
#define INTERSYNTH_BLUETOOTH_BLUETOOTH_H

#if(defined(__linux__) || defined(__unix__))
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <malloc.h>
#include <sys/ioctl.h>
#include <errno.h>

#elif defined(_WIN32)
#include <winsock2.h>
#include <ws2bth.h>
#include <bthsdpdef.h>
#include <bthdef.h>
#include <bthledef.h>
#include <BluetoothAPIs.h>

#endif

#include "error.h"
typedef struct {
    int socket;
    struct sockaddr_rc addr;
    int8_t rssi;


} intersynth_bluetooth_t;

intersynth_bluetooth_t bluetooth_handler;

void intersynth_init_bluetooth(void);
void intersynth_deinit_bluetooth(void);
//BLUETOOTH SETUP
void intersynth_scan(void);
void intersynth_free_scan(void);

int8_t intersynth_get_rssi(void);
void intersynth_connect(char btaddr[18]);
void intersynth_disconnect(void);

//BLUETOOTH SEND
void intersynth_send(char *data, int length); //In the future add a possibility to choose a socket?


#endif //INTERSYNTH_BLUETOOTH_BLUETOOTH_H