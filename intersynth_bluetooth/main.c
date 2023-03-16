//
// Created by star on 6.3.2023.
//
#include "include/intersynth.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {

    printf("Intersynth INIT\n");
    intersynth_init();
    printf("INTERSYNTH INIT DONE\n");
    //Notice here the error is constant and gets fresh errors. This is good. :)
    intersynth_error_t* error = intersynth_get_error();
    printf("%s\n",error->error_string); // ERROR 1
    //intersynth_die();
    //printf("Intersynth die finished\n");
    //printf("%s\n",error->error_string); // ERROR 2
    //{{0xFC, 0x9B, 0xC4, 0x01, 0x5F, 0xE4}}
    /*intersynth_connect("00:11:22:33:44:55");
    printf("Intersynth Connect\n");
    printf("%s\n",error->error_string); // ERROR 2
    char* mesg = "gamli virkar þetta";
    intersynth_send(mesg, strlen(mesg));
    printf("%s\n",error->error_string); // ERROR 2
    intersynth_die();
    */
    printf("Intersynth scan starting\n");
    intersynth_scan();
    printf("%s\n",error->error_string); // ERROR 2
    intersynth_bluetooth_device_inquiry* ii = intersynth_scan_get_results();
    //iterating over the found devices like so.
    for (int i = 0; i < total_devices; i++) {
        printf("Device %d: Name: %s, Address: %02X:%02X:%02X:%02X:%02X:%02X\n", i+1, ii[i].name,
               (ii[i].btaddr >> 40) & 0xFF, (ii[i].btaddr >> 32) & 0xFF,
               (ii[i].btaddr >> 24) & 0xFF, (ii[i].btaddr >> 16) & 0xFF,
               (ii[i].btaddr >> 8) & 0xFF, (ii[i].btaddr & 0xFF));
    }

    printf("intersynth_scan_free");
    intersynth_scan_free();
    printf("%s\n",error->error_string);
    printf("intersynth die\n");
    intersynth_die();
    printf("%s\n",error->error_string); // ERROR 2
    return 0;
}
