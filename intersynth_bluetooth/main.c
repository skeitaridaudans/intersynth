//
// Created by star on 6.3.2023.
//
#include "include/intersynth.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    //😃😡🤬🥶emoji gang
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
    printf("intersynth die\n");
    intersynth_die();
    printf("%s\n",error->error_string); // ERROR 2
    return 0;
}
