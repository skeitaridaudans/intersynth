#include <unistd.h>
#include "intersynth.h"

//rtmidi out demo in c. god help me

//struct RtMidiWrapper *midiout;

int main() {
    intersynth_init();
    bool x = intersynth_select_port(1);
    printf(x ? "true" : "false");
        //sleep(1);
        intersynth_send_note(60,120);
        sleep(1);
        //intersynth_add_modulator(2,1);
        //sleep(1);
        intersynth_change_operator_values(2, 0, true, 2.0, 1.0);
        sleep(1);
        intersynth_send_note(60, 0);
    return 0;
}
