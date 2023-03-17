//
// Created by star on 6.3.2023.
//

#ifndef INTERSYNTH_BLUETOOTH_ERROR_H
#define INTERSYNTH_BLUETOOTH_ERROR_H

//DEFINE ERROR_T STRUCT / ENUM

/**
 * @brief Error codes for intersynth
 * @TODO: Add more error codes & descriptions for each error code
 */

typedef enum {
    INTERSYNTH_ERROR_SUCCESS, //success value
    INTERSYNTH_ERROR_FAILURE, // general failure value
    INTERSYNTH_ERROR_NOT_IMPLEMENTED, //not implemented
    INTERSYNTH_ERROR_NOT_SUPPORTED, //not supported by the system for example bluetooth version
    INTERSYNTH_ERROR_NOT_CONNECTED, //not connected to a bluetooth device
    INTERSYNTH_ERROR_NOT_INITIALIZED, //not initialized
    INTERSYNTH_ERROR_BLUETOOTH, //bluetooth error (Look at ERRNO)
} intersynth_error_code;

typedef struct intersynth_error_t
{
    intersynth_error_code error_code;
    const char *error_string;
} intersynth_error_t;


void intersynth_set_error(intersynth_error_code error_code);

void intersynth_set_success_error(void);

intersynth_error_t intersynth_error;
#endif //INTERSYNTH_BLUETOOTH_ERROR_H
