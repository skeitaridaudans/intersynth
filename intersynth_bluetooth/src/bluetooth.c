
//Bluetooth includes
#include "bluetooth.h"
#include <stdio.h>
//Platform specific includes (Windows, OSX, Linux) for bluetooth functionality

#ifdef _WIN32
#include "../platform/windows/bluetooth_win.h"
#elif defined(__APPLE__)
#include "bluetooth_osx.h"
#elif defined(__linux__)
#include "bluetooth_linux.h"
#else
#error Unsupported platform
#endif

void intersynth_init_bluetooth(void) {
    bluetooth_init();
}

void intersynth_deinit_bluetooth(void) {
    bluetooth_deinit();
}

void intersynth_scan(void)
{
    bluetooth_scan();
}

struct intersynth_bluetooth_device_inquiry* intersynth_scan_get_results(void)
{
    return bluetooth_scan_get_results();
}

void intersynth_scan_free(void)
{
    bluetooth_scan_free();
}

int intersynth_scan_devices_found(void)
{
    return bluetooth_scan_devices_found();
}

void intersynth_select_device(int device_index)
{
    bluetooth_select_device(device_index);
}

void intersynth_disconnect(void)
{
    bluetooth_disconnect();
}

//BLUETOOTH SEND
void intersynth_send(char *data, int length)
{
    bluetooth_send(data, length);
}