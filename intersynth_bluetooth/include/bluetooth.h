//
// Created by star on 6.3.2023.
//

#ifndef INTERSYNTH_BLUETOOTH_BLUETOOTH_H
#define INTERSYNTH_BLUETOOTH_BLUETOOTH_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "error.h"

#ifdef _WIN32
#include <winsock2.h>
#include <bthdef.h>
typedef struct
{
    BTH_ADDR btaddr;
    LPSTR name;
} intersynth_bluetooth_device_inquiry;
#elif defined(__APPLE__)
#include "bluetooth_osx.h"
#elif defined(__linux__)
#include "bluetooth_linux.h"
#else
#error Unsupported platform
#endif



int total_devices = 0;
intersynth_bluetooth_device_inquiry* intersynth_ii = NULL;

/*
#ifdef _WIN32
typedef struct {
    int socket;
    SOCKADDR_BTH addr;
    int8_t rssi;
} intersynth_bluetooth_t;
#elif defined(__APPLE__)
typedef struct {
    int socket;
    struct sockaddr addr;
    int8_t rssi;
} intersynth_bluetooth_t;
#elif defined(__linux__)
typedef struct {
    int socket;
    struct sockaddr_rc addr;
    int8_t rssi;
} intersynth_bluetooth_t;

#else
#error Unsupported platform
#endif
*/


void intersynth_init_bluetooth(void);
void intersynth_deinit_bluetooth(void);
//BLUETOOTH SETUP
void intersynth_scan(void);
intersynth_bluetooth_device_inquiry* intersynth_scan_get_results(void);
void intersynth_scan_free(void);
int intersynth_scan_devices_found(void);
void intersynth_select_device(int device_index);
void intersynth_disconnect(void);

//BLUETOOTH SEND
void intersynth_send(char *data, int length); //In the future add a possibility to choose a socket?


#endif //INTERSYNTH_BLUETOOTH_BLUETOOTH_H