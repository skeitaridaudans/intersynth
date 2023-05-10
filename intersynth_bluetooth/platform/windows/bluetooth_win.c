//
// Created by star on 6.3.2023.
//

#include "bluetooth_win.h"
struct intersynth_bluetooth_device_inquiry* intersynth_ii = NULL;
int total_devices = 0;

void bluetooth_init(void) {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
        return;
    }
    int sockfd = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM); // use AF_BTH and BTHPROTO_RFCOMM on Windows
    if (sockfd < 0) {
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
        return;
    }
    bluetooth_handler.addr.addressFamily = AF_BTH;
    bluetooth_handler.addr.btAddr = 0; //
    bluetooth_handler.addr.port = 1;
    bluetooth_handler.socket = sockfd;
    intersynth_set_success_error();
}

void bluetooth_deinit(void) {
    int status = closesocket(bluetooth_handler.socket);
    if(status < 0)
    {
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
    int WSAclean = WSACleanup();
    if(WSAclean < 0)
    {
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
}

void bluetooth_scan(void)
{
    if(intersynth_ii != NULL)
    {
        intersynth_set_error(INTERSYNTH_ERROR_MEMORY_NOT_CLEARED);
        return;
    }
    BLUETOOTH_DEVICE_INFO device_info;
    BLUETOOTH_DEVICE_SEARCH_PARAMS search_criteria;
    HBLUETOOTH_DEVICE_FIND found_device;
    BOOL next = TRUE;
    int duration = 8; //Scan for 8*1.25 seconds.
    int flush_cache = 1; //Flush the cache, essentially get the newest results instead from memory.

    memset((&device_info), 0, (sizeof(BLUETOOTH_DEVICE_INFO)));
    memset((&search_criteria), 0, (sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS)));

    device_info.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
    search_criteria.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
    search_criteria.fReturnAuthenticated = TRUE;
    search_criteria.fReturnRemembered = !flush_cache;
    search_criteria.fReturnConnected = TRUE;
    search_criteria.fReturnUnknown = TRUE;
    search_criteria.fIssueInquiry = TRUE;
    search_criteria.cTimeoutMultiplier = duration;
    search_criteria.hRadio = NULL;

    found_device = BluetoothFindFirstDevice(&search_criteria, &device_info);

    if(found_device == NULL)
    {
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
        return;
    }

    while(next)
    {
        BTH_ADDR bthAddr = device_info.Address.ullLong;
        total_devices++;
        struct intersynth_bluetooth_device_inquiry* temp = realloc(intersynth_ii, total_devices * sizeof(struct intersynth_bluetooth_device_inquiry));
        if(temp == NULL)
        {
            intersynth_set_error(INTERSYNTH_ERROR_MEMORY);
            free(intersynth_ii);
            return;
        }
        intersynth_ii = temp;
        // Convert the MAC address to a string
        char szMacAddr[18];
        snprintf(szMacAddr, sizeof(szMacAddr), "%02llX:%02llX:%02llX:%02llX:%02llX:%02llX",
                 (bthAddr >> (5 * 8)) & 0xff,
                 (bthAddr >> (4 * 8)) & 0xff,
                 (bthAddr >> (3 * 8)) & 0xff,
                 (bthAddr >> (2 * 8)) & 0xff,
                 (bthAddr >> (1 * 8)) & 0xff,
                 (bthAddr >> (0 * 8)) & 0xff);
        // Store the MAC address and name in a global array or data structure
        intersynth_ii[total_devices-1].btaddr = bthAddr;
        intersynth_ii[total_devices-1].name = (LPSTR)malloc(sizeof(TCHAR) * (256 + 1));
        if (intersynth_ii[total_devices - 1].name == NULL) {
            intersynth_set_error(INTERSYNTH_ERROR_MEMORY);
            for (int i = 0; i < total_devices - 1; i++) {
                free(intersynth_ii[i].name);
            }
            free(intersynth_ii);
            return;
        }
        memset(intersynth_ii[total_devices - 1].name, 0, sizeof(TCHAR) * (256 + 1));
        size_t tst = wcstombs(intersynth_ii[total_devices - 1].name, device_info.szName, 256);
        if (tst == -1)
            intersynth_ii[total_devices - 1].name = "Unknown";
        next = BluetoothFindNextDevice(found_device, &device_info);
    }
}

void bluetooth_scan_free(void)
{
    //Free last scan results.
    for (int i = 0; i < total_devices - 1; i++) {
        free(intersynth_ii[i].name);
    }
    free(intersynth_ii);
    total_devices = 0;
}

struct intersynth_bluetooth_device_inquiry* bluetooth_scan_get_results(void)
{
    //Return device names and mac addresses within a common structure.
    return intersynth_ii;
}

int bluetooth_scan_devices_found(void)
{
    //Total amount of devices found from scan.
    return total_devices;
}

static void bluetooth_connect(BTH_ADDR btaddr)
{
    //Connect to a device via bluetooth_handler.socket using bluetooth_handler.addr#
    bluetooth_handler.addr.btAddr = btaddr;
    if(connect(bluetooth_handler.socket, (const struct sockaddr *) &bluetooth_handler.addr, sizeof(bluetooth_handler.addr)) < 0) {
        printf("WSA ERROR %d\n",WSAGetLastError());
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
}

void bluetooth_select_device(int device_index)
{
    //Tries to connect to the device index inside intersynth_ii global array. Check error to see if connection has been established.
    bluetooth_connect(intersynth_ii[device_index].btaddr);
}


void bluetooth_disconnect(void)
{
    //Disconnect from a device via bluetooth_handler.socket
    int status = closesocket(bluetooth_handler.socket);
    if(status < 0)
    {
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
}

//BLUETOOTH SEND
void bluetooth_send(char *data, int length)
{
    //Send data via bluetooth_handler.socket
    //TODO:
    // More precise errors, IE: Could not send due to connection being closed, etc.
    int status = send(bluetooth_handler.socket, data, length, 0);
    if(status < 0)
    {
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
}

void bluetooth_latency(void)
{
    //Internal testing function. Sends and recives the same buffer again & again.
    char buf[1024] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam velit ligula, tristique sed ante a, ultricies semper quam amet.";
    char rbuf[1024] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam velit ligula, tristique sed ante a, ultricies semper quam amet.";
    while(1)
    {
        int status = send(bluetooth_handler.socket, buf, 1024, 0);
        if(status < 0)
        {
            break;
        }
        int r_status = recv(bluetooth_handler.socket, rbuf, 1024, 0);
        if (r_status < 0)
        {
            break;
        }
        if(strcmp(rbuf, "STOP") == 0)
        {
            break;
        }
    }
}

void bluetooth_connect_static(void)
{
    //Static connection function for testing, Used for not having to scan every time
    BLUETOOTH_ADDRESS btaddrs;
    //58:11:22:53:65:5E
    btaddrs.rgBytes[0] = 0x5E;
    btaddrs.rgBytes[1] = 0x65;
    btaddrs.rgBytes[2] = 0x53;
    btaddrs.rgBytes[3] = 0x22;
    btaddrs.rgBytes[4] = 0x11;
    btaddrs.rgBytes[5] = 0x58;

    bluetooth_handler.addr.btAddr = btaddrs.ullLong;

    //bluetooth_handler.addr.btAddr = btaddr;
    if(connect(bluetooth_handler.socket, (const struct sockaddr *) &bluetooth_handler.addr, sizeof(bluetooth_handler.addr)) < 0) {
        printf("WSA ERROR %d\n",WSAGetLastError());
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
}