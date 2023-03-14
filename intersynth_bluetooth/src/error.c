//
// Created by star on 6.3.2023.
//

#include "../include/error.h"
//TODO: Get error string from error code enum as defined in error.h

static intersynth_error_t intersynth_error;

const char* error_codes[] = {
        "Success",
        "Failure",
        "Not implemented",
        "Not supported",
        "Not connected",
        "Not initialized",
        "Bluetooth error"};

const intersynth_error_t* intersynth_get_error(void) {
    return &intersynth_error;
}

void intersynth_set_error(intersynth_error_code error_code) {
    intersynth_error.error_code = error_code;
    intersynth_error.error_string = error_codes[error_code];
}

void intersynth_set_success_error(void){
    intersynth_error.error_code = INTERSYNTH_ERROR_SUCCESS;
    intersynth_error.error_string = error_codes[INTERSYNTH_ERROR_SUCCESS];
}

void intersynth_clear_error(void) {
    intersynth_error.error_code = INTERSYNTH_ERROR_SUCCESS;
    intersynth_error.error_string = error_codes[INTERSYNTH_ERROR_SUCCESS];
}
