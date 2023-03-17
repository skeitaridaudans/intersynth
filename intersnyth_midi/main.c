#include <unistd.h>
#include "intersynth.h"

//rtmidi out demo in c. god help me

//struct RtMidiWrapper *midiout;

int main() {
    intersynth_init();
    bool x = intersynth_select_port(1);
    printf(x ? "true" : "false");
        //sleep(1);
    intersynth_send_note_on(60,120);
    sleep(1);
    intersynth_add_modulator(2,1);
        //sleep(1);
    sleep(1);
    intersynth_change_operator_values(1, 0, true, 6.50419998169, 6.50419998169);
    sleep(1);
    intersynth_remove_modulator(2,1);
    sleep(1);
    intersynth_send_note_off(60);
    return 0;
}
