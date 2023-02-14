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
#include "floating_fix.h"

#define MIDI_NOTE_MESSAGE_SIZE 3
#define MIDI_MESSAGE_FUCK 4

void intersynth_init();
void intersynth_die();
uint32_t intersynth_get_num_ports();
char* intersynth_get_port_name(uint32_t port_num);
void intersynth_free_get_port_name(char* arr);
bool intersynth_select_port(uint32_t port_num);
bool intersynth_send_note(unsigned char key, unsigned char velocity);
bool intersynth_change_operator_values(unsigned char operator, unsigned char alg_index, bool attack, float frequency_factor, float amplitude);
bool intersynth_add_modulator(int operator_id, int modulator_id);

#endif //MIDI_SYNTH_INTERSYNTH_H
