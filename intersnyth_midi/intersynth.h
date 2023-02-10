//
// Created by star on 9.2.2023.
//

#ifndef MIDI_SYNTH_INTERSYNTH_H
#define MIDI_SYNTH_INTERSYNTH_H

#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>
#include <utils.h>
#include <rtmidi/rtmidi_c.h>

#define MIDI_NOTE_MESSAGE_SIZE 3

struct RtMidiWrapper* midiout = {0};

void intersynth_init();
uint32_t intersynth_get_num_ports();
char* intersnyth_get_port_name(uint32_t port_num);
bool intersynth_select_port(uint32_t port_num);
bool intersnyth_send_note(unsigned char key, unsigned char velocity);


#endif //MIDI_SYNTH_INTERSYNTH_H
