#include "intersynth.h"

intersynth_error_t intersynth_init(void) {
    // Initialize library
    intersynth_init_bluetooth();
}

intersynth_error_t intersynth_die(void) {
    // Deinitialize library
    intersynth_deinit_bluetooth();
}