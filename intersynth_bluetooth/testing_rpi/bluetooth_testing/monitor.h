#include <stdint.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <stdlib.h>

int8_t get_rssi_from_connection(bdaddr_t* bdaddr);