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


//INITIAL SETUP FUNCTIONS
void intersynth_init();
void intersynth_die(); // perfect, thanks its amazing
const char * intersynth_get_error();
uint32_t intersynth_get_num_ports();
char* intersnyth_get_port_name(uint32_t port_num);
void intersnyth_free_get_port_name(char* arr);
bool intersynth_select_port(uint32_t port_num);

//MIDI NOTES
bool intersynth_send_note_on(unsigned char key, unsigned char velocity);
bool intersynth_send_note_off(unsigned char key);

//OPERATOR CHANGE
bool intersynth_change_operator_values(unsigned char operator, unsigned char alg_index, bool attack, float frequency_factor, float amplitude);

//OPERATOR MODULATION
bool intersynth_add_modulator(int operator_id, int modulator_id);
bool intersynth_remove_modulator(int operator_id, int modulator_id);

//OPERATOR CARRIERS
bool intersynth_add_carrier(int operator_id);
bool intersynth_remove_carrier(int operator_id);


#endif //MIDI_SYNTH_INTERSYNTH_H
