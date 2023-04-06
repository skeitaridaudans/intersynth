//
// Created by star on 6.3.2023.
//
#include "include/intersynth.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

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
    int id_wanted = 0;
    //iterating over the found devices like so.
    for (int i = 0; i < total_devices; i++) {
        printf("Device %d: Name: %s, Address: %02llX:%02llX:%02llX:%02llX:%02llX:%02llX\n", i, ii[i].name,
               (ii[i].btaddr >> 40) & 0xFF, (ii[i].btaddr >> 32) & 0xFF,
               (ii[i].btaddr >> 24) & 0xFF, (ii[i].btaddr >> 16) & 0xFF,
               (ii[i].btaddr >> 8) & 0xFF, (ii[i].btaddr & 0xFF));
        if(strcmp(ii[i].name, "raspberrypi") == 0){
            id_wanted = i;
        }
    }


    //Device 3: Name: raspberrypi, Address: E4:5F:01:C4:9B:FC
    printf("%s\n",error->error_string);

    printf("intersynth_connect_fixed_addr device\n");
    //intersynth_connect_fixed_addr();
    intersynth_select_device(id_wanted);
    //printf("%s\n",error->error_string);
    printf("connect_fixed_addr done\n");
    intersynth_send("gamli virkar þetta", strlen("gamli virkar þetta"));
    printf("sleep start\n");
    sleep(10);
    printf("sleep stopped\n");
    intersynth_send("gamli virkar þett2", strlen("gamli virkar þett2"));
    printf("intersynth die\n");
    sleep(10);
    printf("sleep stopped - time to die\n");
    intersynth_die();
    printf("%s\n",error->error_string); // ERROR 2
    return 0;
}
