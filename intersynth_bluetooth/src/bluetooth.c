//
// Created by star on 6.3.2023.
//


#include "../include/bluetooth.h"
#include "../include/error.h"
#include <stdio.h>


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
    printf("SOCKET WSA ERROR %d", WSAGetLastError());
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
}

//BLUETOOTH SETUP
void intersynth_scan(void);

void intersynth_get_rssi(void)
{
    //Get RSSI of current connection via bluetooth_handler.socket
    //int dev_id = hci_get_route(NULL);
    //int sock = hci_open_dev( dev_id );
    //if (dev_id < 0 || sock < 0) {
    //    fprintf(stderr, "Could not open socket");
    //    intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    //}
    //int8_t rssi = 0;
    //struct hci_conn_info_req *cr;
    //cr = malloc(sizeof(*cr) + sizeof(struct hci_conn_info));
    //bacpy(&cr->bdaddr, &bluetooth_handler.addr.rc_bdaddr);
    //cr->type = ACL_LINK;
    //if (ioctl(sock, HCIGETCONNINFO, (unsigned long) cr) < 0) {
    //    fprintf(stderr, "Could not get connection info");
    //    intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    //}
    //if(hci_read_rssi(sock, htobs(cr->conn_info->handle), &rssi, 1000)< 0)
    //{
    //    fprintf(stderr, "Could not read RSSI");
    //    intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    //}
    //free(cr);
    //hci_close_dev(sock);
    //bluetooth_handler.rssi = rssi;
    //intersynth_set_success_error();
    return;
}

void intersynth_connect(char btaddr[18])
{
    //Connect to a device via bluetooth_handler.socket using bluetooth_handler.addr#
    // TODO: FIND WINDOWS EQ = str2ba(btaddr, &bluetooth_handler.addr.rc_bdaddr);
    BLUETOOTH_ADDRESS btaddrs;

    btaddrs.rgBytes[0] = 0xFC;
    btaddrs.rgBytes[1] = 0x9B;
    btaddrs.rgBytes[2] = 0xC4;
    btaddrs.rgBytes[3] = 0x01;
    btaddrs.rgBytes[4] = 0x5F;
    btaddrs.rgBytes[5] = 0xE4;

    bluetooth_handler.addr.btAddr = btaddrs.ullLong;

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