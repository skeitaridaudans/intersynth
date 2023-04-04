//
// Created by star on 6.3.2023.
//
#include "include/intersynth.h"

int main(int argc, char **argv) {
    intersynth_init_bluetooth();
    intersynth_error_t* error = intersynth_get_error();
    printf("%s\n",error->error_string);
}
