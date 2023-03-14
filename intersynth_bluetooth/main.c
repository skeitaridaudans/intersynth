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
    intersynth_die();
    printf("%s\n",error->error_string); // ERROR 2
    return 0;
}
