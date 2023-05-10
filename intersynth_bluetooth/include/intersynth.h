#ifndef INTERSYNTH_BLUETOOTH_INTERSYNTH_H
#define INTERSYNTH_BLUETOOTH_INTERSYNTH_H

#include <inttypes.h>
#include <stdbool.h>

/*
 * IN THE FUTURE WE WILL MOVE ALL FUNCTIONS TO THEIR OWN FILES AND ONLY HAVE THE INCLUDES FOR THEM... PRIVATE OR PUBLIC THAT IS
 */
#include "bluetooth.h"
#include "error.h"
#include "messages.h"
#ifdef _WIN32
#include "../platform/windows/bluetooth_win.h"
#elif defined(__APPLE__)
#include "bluetooth_osx.h"
#elif defined(__linux__)
#include "bluetooth_linux.h"
#else
#error Unsupported platform
#endif


//init
intersynth_error_t intersynth_init(void);
//Not init
intersynth_error_t intersynth_die(void);

typedef struct {
    char *data;
    uint16_t length;
} intersynth_message_t;


#endif //INTERSYNTH_BLUETOOTH_INTERSYNTH_H
