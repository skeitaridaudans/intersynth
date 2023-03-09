//
// Created by star on 28.2.2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

int main(int argc, char **argv) {
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
    for (i = 0; i < num_rsp; i++) {
        //get address of device
        ba2str(&(ii+i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
        //get name of device
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), name, 0) < 0) {
            strcpy(name, "[unknown]"); // if name is not found
        }
        printf("Device %d: [bdaddr]:%s  [hostname]:%s\n", i , addr, name);
    }

    free(ii);
    close(sock);
    return 0;
}