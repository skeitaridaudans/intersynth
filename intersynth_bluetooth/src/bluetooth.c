//
// Created by star on 6.3.2023.
//


#include "../include/bluetooth.h"
#include "../include/error.h"
#include <stdio.h>

int total_devices = 0;
intersynth_bluetooth_device_inquiry* intersynth_ii = NULL;

void intersynth_init_bluetooth(void) {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        //TODO: Error handling - SET ERROR CODE, possibly return a "intersynth_error_t" struct
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
        return;
    }
    int sockfd = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM); // use AF_BTH and BTHPROTO_RFCOMM on Windows
    if (sockfd < 0) {
        //TODO: Error handling - SET ERROR CODE, possibly return a "intersynth_error_t" struct
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
        return;
    }
    //printf("SOCKET WSA ERROR %d", WSAGetLastError());
    bluetooth_handler.addr.addressFamily = AF_BTH;
    bluetooth_handler.addr.btAddr = 0; // set the Bluetooth address to 0, will be updated later
    bluetooth_handler.addr.port = 4;
    bluetooth_handler.socket = sockfd;
    intersynth_set_success_error();
}

void intersynth_deinit_bluetooth(void) {
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

//BLUETOOTH SETUP
void intersynth_scan(void)
{
    if(intersynth_ii != NULL)
    {
        intersynth_set_error(INTERSYNTH_ERROR_MEMORY_NOT_CLEARED);
        return;
    }
    //Scan the local area for devices and store MAC and Name
    //TODO: PUT EVERY FOUND DEVICE INTO A GLOBAL ARRAY OF intersynth_bluetooth_device_inquiry structures.
    //Set flags so we flush the cache and get the names and addresses (MAC)
    DWORD flags =  LUP_CONTAINERS | LUP_FLUSHCACHE | LUP_RETURN_ADDR | LUP_RETURN_NAME;
    WSAQUERYSET querySet;
    memset(&querySet, 0x00, sizeof(WSAQUERYSET)); // Clear it
    querySet.dwSize = sizeof(WSAQUERYSET); // No idea
    querySet.dwNameSpace = NS_BTH; //We want to find bluetooth devices
    //querySet.lpServiceClassId = (LPGUID) &SerialPortServiceClass_UUID;
    HANDLE hLookup = NULL; // We get a handle back from WSALookupServiceBegin function to iterate over found devices

    int result = WSALookupServiceBegin(&querySet, flags, &hLookup);
    if (result != 0) {
        //If WSALookup fails
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
        return;
    }

    //Buffers and setup for the next system call
    BYTE buffer[4096];
    DWORD bufferLength = sizeof(buffer);
    WSAQUERYSET* pResults = (WSAQUERYSET*) buffer;

    //intersynth_ii = (intersynth_bluetooth_device_inquiry*)malloc(MAX_DEVICE_II * sizeof(intersynth_bluetooth_device_inquiry));
    while (TRUE) {
        //While loop to iterate over all of the devices we found
        result = WSALookupServiceNext(hLookup, flags, &bufferLength, pResults);
        if (result == WSA_E_NO_MORE || result == WSAENOMORE) {
            break; //If no more devices where found
        }
        else if (result == -1 && WSAGetLastError() == 10110){
            break; //Same as above
        }
        else if (result != 0) {
            printf("result: %d\n", result);
            continue;
        }
        total_devices++;
        intersynth_bluetooth_device_inquiry* temp = realloc(intersynth_ii, total_devices * sizeof(intersynth_bluetooth_device_inquiry));
        if(temp == NULL)
        {
            intersynth_set_error(INTERSYNTH_ERROR_MEMORY);
            free(intersynth_ii);
            intersynth_ii = NULL;
            return;
        }
        intersynth_ii = temp;

        // Extract the MAC address from the Bluetooth address structure
        BTH_ADDR bthAddr = ((SOCKADDR_BTH*) pResults->lpcsaBuffer->RemoteAddr.lpSockaddr)->btAddr;

        // Convert the MAC address to a string
        char szMacAddr[18];
        snprintf(szMacAddr, sizeof(szMacAddr), "%02X:%02X:%02X:%02X:%02X:%02X",
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
            intersynth_ii = NULL;
            return;
        }
        memset(intersynth_ii[total_devices - 1].name, 0, sizeof(TCHAR) * (256 + 1));
        strcpy(intersynth_ii[total_devices - 1].name,pResults->lpszServiceInstanceName);
        // Here, we'll just print them to the console
        printf("Device Name: %s, MAC Address: %s\n", pResults->lpszServiceInstanceName, szMacAddr);
    }
    WSALookupServiceEnd(hLookup);
}

intersynth_bluetooth_device_inquiry* intersynth_scan_get_results(void)
{
    return intersynth_ii;
}

void intersynth_scan_free(void)
{
    for (int i = 0; i < total_devices - 1; i++) {
        free(intersynth_ii[i].name);
    }
    free(intersynth_ii);
    intersynth_ii = NULL;
    total_devices = 0;
}

int intersynth_scan_devices_found(void)
{
    return total_devices;
}

void intersynth_select_device(int device_index)
{
    //Tries to connect to the device index inside of intersynth_ii global array. Check error to see if connection has ben established.
    intersynth_connect(intersynth_ii[device_index].btaddr);
}


static void intersynth_connect(BTH_ADDR btaddr)
{
    //Connect to a device via bluetooth_handler.socket using bluetooth_handler.addr#
    // TODO: FIND WINDOWS EQ = str2ba(btaddr, &bluetooth_handler.addr.rc_bdaddr);
    /*
    BLUETOOTH_ADDRESS btaddrs;

    btaddrs.rgBytes[0] = 0xFC;
    btaddrs.rgBytes[1] = 0x9B;
    btaddrs.rgBytes[2] = 0xC4;
    btaddrs.rgBytes[3] = 0x01;
    btaddrs.rgBytes[4] = 0x5F;
    btaddrs.rgBytes[5] = 0xE4;

    bluetooth_handler.addr.btAddr = btaddrs.ullLong;
    */
    bluetooth_handler.addr.btAddr = btaddr;
    if(connect(bluetooth_handler.socket, (const struct sockaddr *) &bluetooth_handler.addr, sizeof(bluetooth_handler.addr)) < 0) {
        printf("WSA ERROR %d\n",WSAGetLastError());
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
}
void intersynth_disconnect(void)
{
    //Disconnect from a device via bluetooth_handler.socket
    int status = closesocket(bluetooth_handler.socket);
    if(status < 0)
    {
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
}

//BLUETOOTH SEND
void intersynth_send(char *data, int length)
{
    //Send data via bluetooth_handler.socket
    //TODO:
    // More precise errors, IE: Could not send due to connection being closed, etc.
    // Add a way to choose a socket
    int status = send(bluetooth_handler.socket, data, length, 0); //Note on flags, Could be useful to use MSG_CONFIRM since it doesint really matter.
    if(status < 0)
    {
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
}