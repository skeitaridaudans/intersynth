#ifndef INTERSYNTH_BLUETOOTH_INTERSYNTH_H
#define INTERSYNTH_BLUETOOTH_INTERSYNTH_H

#include <stdbool.h>
#include <inttypes.h>
/*
 * IN THE FUTURE WE WILL MOVE ALL FUNCTIONS TO THEIR OWN FILES AND ONLY HAVE THE INCLUDES FOR THEM... PRIVATE OR PUBLIC THAT IS
 */
#include "error.h"
#include "bluetooth.h"
#include "messages.h"
//init
intersynth_error_t intersynth_init(void);
//Not init
intersynth_error_t intersynth_die(void);

typedef struct {
    char *data;
    uint16_t length;
} intersynth_message_t;

//intersynth_error_t intersynth_error;

//MORE TO COME



#endif //INTERSYNTH_BLUETOOTH_INTERSYNTH_H
