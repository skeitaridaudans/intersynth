#include "error.h"
/**
 * @brief The error struct
 * Stores the error code and the error string
 */
static intersynth_error_t intersynth_error;

/**
 * @brief The error codes
 * The error codes are stored in an array of strings
 */
const char* error_codes[] = {
        "Success",
        "Failure",
        "Not implemented",
        "Not supported",
        "Not connected",
        "Not initialized",
        "Bluetooth error",
        "Not enough memory",
        "Memory not cleared"};

const intersynth_error_t* intersynth_get_error(void) {
    /**
     * @brief Returns the error struct
     */
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

//Helper functions if you don't want to use the error struct
intersynth_error_code intersynth_get_error_code(void)
{
    return intersynth_error.error_code;
}

const char* intersynth_get_error_string(intersynth_error_code error_code)
{
    return error_codes[error_code];
}