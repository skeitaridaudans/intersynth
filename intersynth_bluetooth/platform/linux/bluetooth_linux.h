//
// Created by star on 6.3.2023.
//

#include "error.h"
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/ioctl.h>

struct intersynth_bluetooth_device_inquiry {
    char name[248];
    char bdaddr[19];
};

typedef struct {
    int socket;
    struct sockaddr_rc addr;
} intersynth_bluetooth_t;

static intersynth_bluetooth_t bluetooth_handler;

void bluetooth_init(void);
void bluetooth_deinit(void);
//BLUETOOTH SETUP
void bluetooth_scan(void);
struct intersynth_bluetooth_device_inquiry* bluetooth_scan_get_results(void);
void bluetooth_scan_free(void);
int bluetooth_scan_devices_found(void);
void bluetooth_select_device(int device_index);

static void bluetooth_connect(char address[18]);
void bluetooth_connect_fixed();
void bluetooth_disconnect(void);
//BLUETOOTH SEND
void bluetooth_send(char *data, int length);
void bluetooth_latency(void);