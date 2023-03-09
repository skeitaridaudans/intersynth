//
// Created by star on 6.3.2023.
//


#include "../include/bluetooth.h"

void intersynth_init_bluetooth(void) {
    bluetooth_handler.addr.rc_family = AF_BLUETOOTH;
    bluetooth_handler.addr.rc_channel = (uint8_t) 1;
    int sockfd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (sockfd < 0) {
        //TODO: Error handling - SET ERROR CODE, possibly return a "intersynth_error_t" struct
        fprintf(stderr,"Error creating socket");
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
    bluetooth_handler.socket = sockfd;
    intersynth_set_success_error();
}

void intersynth_deinit_bluetooth(void) {
    close(bluetooth_handler.socket);
    intersynth_set_success_error();
}

//BLUETOOTH SETUP
void intersynth_scan(void);

int8_t intersynth_get_rssi(void)
{
    //Get RSSI of current connection via bluetooth_handler.socket
    int dev_id = hci_get_route(NULL);
    int sock = hci_open_dev( dev_id );
    if (dev_id < 0 || sock < 0) {
        fprintf(stderr, "Could not open socket");
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
    int8_t rssi = 0;
    struct hci_conn_info_req *cr;
    cr = malloc(sizeof(*cr) + sizeof(struct hci_conn_info));
    bacpy(&cr->bdaddr, &bluetooth_handler.addr.rc_bdaddr);
    cr->type = ACL_LINK;
    if (ioctl(sock, HCIGETCONNINFO, (unsigned long) cr) < 0) {
        fprintf(stderr, "Could not get connection info");
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }

    if(hci_read_rssi(sock, htobs(cr->conn_info->handle), &rssi, 1000)< 0)
    {
        fprintf(stderr, "Could not read RSSI");
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
    free(cr);
    hci_close_dev(sock);
    bluetooth_handler.rssi = rssi;
    intersynth_set_success_error();
}

void intersynth_connect(char btaddr[18])
{
    //Connect to a device via bluetooth_handler.socket using bluetooth_handler.addr
    str2ba(btaddr, &bluetooth_handler.addr.rc_bdaddr);
    if(connect(bluetooth_handler.socket, (struct sockaddr *)&bluetooth_handler.addr, sizeof(bluetooth_handler.addr)) < 0) {
        fprintf(stderr, "Could not connect");
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
    intersynth_set_success_error();
}
void intersynth_disconnect(void)
{
    //Disconnect from a device via bluetooth_handler.socket
    int status = close(bluetooth_handler.socket);
    if(status < 0)
    {
        fprintf(stderr, "Could not disconnect");
        fprintf(stderr, "Error code: %d", errno);
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
    intersynth_set_success_error();
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
        fprintf(stderr, "Could not send data");
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
    intersynth_set_success_error();
}