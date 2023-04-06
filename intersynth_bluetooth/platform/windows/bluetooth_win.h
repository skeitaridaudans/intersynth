//
// Created by star on 6.3.2023.
//


#include "bluetooth.h"
#include "error.h"
#include <stdio.h>
#include <winsock2.h>
#include <ws2bth.h>
#include <bthsdpdef.h>
#include <bthdef.h>
#include <BluetoothAPIs.h>
typedef struct {
    int socket;
    SOCKADDR_BTH addr;
    int8_t rssi;
} intersynth_bluetooth_t;

static intersynth_bluetooth_t bluetooth_handler;

void bluetooth_init(void);

void bluetooth_deinit(void);

//BLUETOOTH SETUP
void bluetooth_scan(void);

intersynth_bluetooth_device_inquiry* bluetooth_scan_get_results(void);

void bluetooth_scan_free(void);

int bluetooth_scan_devices_found(void);

void bluetooth_select_device(int device_index);


static void bluetooth_connect(BTH_ADDR btaddr);
void bluetooth_disconnect(void);
//BLUETOOTH SEND
void bluetooth_send(char *data, int length);