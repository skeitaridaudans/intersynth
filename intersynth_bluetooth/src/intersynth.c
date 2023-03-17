#include "../include/intersynth.h"

intersynth_error_t intersynth_init(void) {
    intersynth_init_bluetooth();
}

intersynth_error_t intersynth_die(void) {
    intersynth_deinit_bluetooth();
}