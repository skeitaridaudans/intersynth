//
// Created by star on 6.3.2023.
//
#include "include/intersynth.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
    //Demo usage.
    intersynth_init();
    //Notice here the error is constant and gets fresh errors.
    intersynth_error_t* error = intersynth_get_error();
    if(error->error_code != 0)
        exit(-1);

    printf("Intersynth scan starting\n");
    intersynth_scan();
    printf("%s\n",error->error_string); // ERROR 2
    struct intersynth_bluetooth_device_inquiry* ii = intersynth_scan_get_results();
    int total_devices = intersynth_scan_devices_found();
    int id_wanted = 0;
    //iterating over the found devices like so.
    for (int i = 0; i < total_devices; i++) {
        printf("Device %d: Name: %s, Address: %s\n", i, ii[i].name, ii[i].bdaddr);
        if(strcmp(ii[i].name, "raspberrypi"))
        {
            id_wanted = i;
        }
    }
    intersynth_select_device(id_wanted);
    printf("%s\n",error->error_string);
    //after checking if we got an error after selecting a device and if its good we can then
    //send some messages here.
    sleep(10);
    intersynth_disconnect();
    printf("%s\n",error->error_string);
    intersynth_die();
    printf("%s\n",error->error_string);
    return 0;

}