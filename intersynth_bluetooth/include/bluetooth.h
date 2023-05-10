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
#elif defined(__APPLE__)
#include "bluetooth_osx.h"
#elif defined(__linux__)
#include "bluetooth_linux.h"
#else
#error Unsupported platform
#endif

extern struct intersynth_bluetooth_device_inquiry* intersynth_ii;


void intersynth_init_bluetooth(void);
void intersynth_deinit_bluetooth(void);
//BLUETOOTH SETUP
void intersynth_scan(void);
struct intersynth_bluetooth_device_inquiry* intersynth_scan_get_results(void);
void intersynth_scan_free(void);
int intersynth_scan_devices_found(void);
void intersynth_select_device(int device_index);
void intersynth_disconnect(void);
void intersynth_static_connect(void);
//BLUETOOTH SEND
void intersynth_send(char *data, int length);

void intersynth_latency_test(void);

#endif //INTERSYNTH_BLUETOOTH_BLUETOOTH_H