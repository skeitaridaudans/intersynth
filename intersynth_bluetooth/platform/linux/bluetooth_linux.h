//
// Created by star on 6.3.2023.
//

#include "bluetooth.h"
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


void bluetooth_init(void) {
    bluetooth_handler.addr.rc_family = AF_BLUETOOTH;
    bluetooth_handler.addr.rc_channel = (uint8_t) 1;
    int sockfd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (sockfd < 0) {
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
    bluetooth_handler.socket = sockfd;
    intersynth_set_success_error();
}

void bluetooth_deinit(void) {
    int status = close(bluetooth_handler.socket);
    if (status < 0) {
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
    intersynth_set_success_error();
}

//BLUETOOTH SETUP
void bluetooth_scan(void)
{
    if(intersynth_ii != NULL)
    {
        intersynth_set_error(INTERSYNTH_ERROR_MEMORY_NOT_CLEARED);
        return;
    }

    intersynth_ii = temp;
    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    char addr[19] = { 0 };
    char name[248] = { 0 };
    printf("Scanning for devices...(this may take a while)\n");
    dev_id = hci_get_route(NULL); // Get first available bluetooth device by passing NULL
    sock = hci_open_dev(dev_id);
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        exit(1);
    }
    len  = 16; // 8 * 1.28 = 10.24 seconds
    max_rsp = 255; // 255 devices
    flags = IREQ_CACHE_FLUSH; // flush cache
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags); //actual search
    if (num_rsp < 0) {
        perror("hci_inquiry");
    }
    printf("Found %d devices:\n", num_rsp);
    //print all devices

    intersynth_ii = intersynth_bluetooth_device_inquiry* malloc(sizeof(intersynth_bluetooth_device_inquiry) * num_rsp);
    for (i = 0; i < num_rsp; i++) {
        //get address of device
        ba2str(&(ii+i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
        //get name of device
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), name, 0) < 0) {
            strcpy(name, "[unknown]"); // if name is not found
        }
        intersynth_ii[i].bdaddr = &(ii+i)->bdaddr;
        intersynth_ii[i].name = name;
        printf("Device %d: [bdaddr]:%s  [hostname]:%s\n", i , addr, name);
    }
    free(ii);
    close(sock);
}

void bluetooth_scan_free(void)
{
    if(intersynth_ii == NULL)
    {
        intersynth_set_error(INTERSYNTH_ERROR_MEMORY_NOT_ALLOCATED);
        return;
    }
    free(intersynth_ii);
    intersynth_ii = NULL;
    intersynth_set_success_error();
}

void bluetooth_get_rssi(void)
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

void bluetooth_scan_free(void)
{
    for (int i = 0; i < total_devices - 1; i++) {
        free(intersynth_ii[i].name);
    }
    free(intersynth_ii);
    intersynth_ii = NULL;
    total_devices = 0;
}

intersynth_bluetooth_device_inquiry* bluetooth_scan_get_results(void)
{
    return intersynth_ii;
}

int bluetooth_scan_devices_found(void)
{
    return total_devices;
}

void bluetooth_select_device(int device_index)
{
    //Select a device from bluetooth_handler.ii
    bluetooth_connect(intersynth_ii[device_index].btaddr);
}

static void bluetooth_connect(char btaddr[18])
{
    //Connect to a device via bluetooth_handler.socket using bluetooth_handler.addr
    str2ba(btaddr, &bluetooth_handler.addr.rc_bdaddr);
    if(connect(bluetooth_handler.socket, (struct sockaddr *)&bluetooth_handler.addr, sizeof(bluetooth_handler.addr)) < 0) {
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
    intersynth_set_success_error();
}
void bluetooth_disconnect(void)
{
    //Disconnect from a device via bluetooth_handler.socket
    int status = close(bluetooth_handler.socket);
    if(status < 0)
    {
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
    intersynth_set_success_error();
}

//BLUETOOTH SEND
void bluetooth_send(char *data, int length)
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
    intersynth_set_success_error();
}