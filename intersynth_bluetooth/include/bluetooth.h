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
#include <stdio.h>
#include <stdbool.h>
#define MAX_DEVICE_II 10

//Bluetooth handler structure, this is only used once but we need to keep our data bundled for a connection.
typedef struct {
    int socket;
    SOCKADDR_BTH addr;
    bool connected;
} intersynth_bluetooth_t;

typedef struct
{
    BTH_ADDR btaddr;
    LPSTR name;
} intersynth_bluetooth_device_inquiry;

/*
 * We use the word extern since we want to allow other c files to define these variables.
 * We define them in bluetooth.c but also makes sure that the user wont run into problems when using the scanning function.
 */

extern intersynth_bluetooth_device_inquiry* intersynth_ii;// = NULL;
extern int total_devices;

static intersynth_bluetooth_t bluetooth_handler;
void intersynth_init_bluetooth(void);
void intersynth_deinit_bluetooth(void);
//BLUETOOTH SETUP
void intersynth_scan(void);
intersynth_bluetooth_device_inquiry* intersynth_scan_get_results(void);
void intersynth_scan_free(void);
int intersynth_scan_devices_found(void);
void intersynth_select_device(int device_index);

static void intersynth_connect(BTH_ADDR btaddr);
void intersynth_connect_fixed_addr(void);
void intersynth_disconnect(void);

//BLUETOOTH SEND
void intersynth_send(char *data, int length); //In the future add a possibility to choose a socket?



#endif //INTERSYNTH_BLUETOOTH_BLUETOOTH_H