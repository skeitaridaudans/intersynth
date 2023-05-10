#include "error.h"
#include <stdio.h>
#include <winsock2.h>
#include <ws2bth.h>
#include <bthsdpdef.h>
#include <bthdef.h>
#include <BluetoothAPIs.h>
#include <stdint.h>

/**
 * @brief Strucutre for storing the bluetooth inquiry results
 * Contains the bluetooth address and the name of the device
 */
struct intersynth_bluetooth_device_inquiry
{
    BTH_ADDR btaddr;
    LPSTR name;
};
/**
 * @brief Structure for storing the bluetooth connection
 * Contains the socket and the address
 */
typedef struct {
    int socket;
    SOCKADDR_BTH addr;
} intersynth_bluetooth_t;

// Static bluetooth handler. Currently only one connection is supported.
static intersynth_bluetooth_t bluetooth_handler;
//Bluetooth setup functions
void bluetooth_init(void);
void bluetooth_deinit(void);
//Bluetooth scan
void bluetooth_scan(void);
struct intersynth_bluetooth_device_inquiry* bluetooth_scan_get_results(void);
void bluetooth_scan_free(void);
int bluetooth_scan_devices_found(void);
void bluetooth_select_device(int device_index);
//Bluetooth connect
static void bluetooth_connect(BTH_ADDR btaddr);
void bluetooth_disconnect(void);
//BLUETOOTH SEND
void bluetooth_send(char *data, int length);
//Internal testing functions use at your own discretion
void bluetooth_latency(void);
void bluetooth_connect_static(void);