//
// Created by star on 6.3.2023.
//

#ifndef INTERSYNTH_BLUETOOTH_MESSAGES_H
#define INTERSYNTH_BLUETOOTH_MESSAGES_H

#include <stdbool.h>
// TODO need to include the following to not have uint32_t error in src/messages.c
#include <inttypes.h>


// if need to define something, to it here
#define MIDI_NOTE_MESSAGE_SIZE 3
#define OPERATOR_VALUES 0x10
#define MODULATED_BY 0x40
#define MODULATOR_ON 0x10
#define MODULATOR_OFF 0x00
#define CARRIER_ON 0x50
#define CARRIER_OFF 0x60
#define SYSSEX_START 0xF0
#define INTERSYNTH_IDENTIFIER 0x70
#define MESSAGE_END 0xF7


//FLOATING FUNCTIONS
void store_float_in_buffer(char *buffer, float value);
float reconstruct_float_from_buffer(const char *buffer);

//MIDI NOTES
void intersynth_send_note_on(unsigned char key, unsigned char velocity); //Intersynth_midi
void intersynth_send_note_off(unsigned char key); //Intersynth_midi

//OPERATOR CHANGE
void intersynth_change_operator_values(unsigned char operator, unsigned char alg_index, bool attack, float frequency_factor, float amplitude); //Intersynth_midi

//OPERATOR MODULATION
void intersynth_add_modulator(int operator_id, int modulator_id); //Intersynth_midi
void intersynth_remove_modulator(int operator_id, int modulator_id); //Intersynth_midi

//OPERATOR CARRIERS
void intersynth_add_carrier(int operator_id); //intersynth_midi
void intersynth_remove_carrier(int operator_id); //intersynth_midi

/*
 * @TODO: ALL OF THE FUNCTIONS BELOW NEED TO BE IMPLEMENTED, FUNCTIONS ABOVE HAVE BEEN IMPLEMENTED IN THE MIDI VERSION JUST NEEDS TO BE MOVED TO THIS PROJECT
 */

void intersynth_clear_operators(void); //ekki intersynth_midi

//AMPLITUDE ENVELOPE INFO
void intersynth_amp_envelope_info_insert(bool attack, int alg_index, float time); //ekki intersynth_midi
void intersynth_amp_envelope_info_remove(bool attack, int alg_index);
void intersynth_amp_envelope_info_envelope_size(bool attack, int new_size);
void intersynth_amp_envelope_info_update_time(bool attack, int alg_index, float new_time);
void intersynth_amp_envelope_info_solo_value(bool attack, int alg_index);

//AMP FIXED
void intersynth_amp_fixed_ignore_envelope(float amp_level);
void intersynth_amp_fixed_ignore_velocity(bool ignore);


//FREQ SYNCED BY
void intersynth_freq_synced_by(int operator_id, int sync_operator_num, bool on_off);


//NOTE INFO
void intersynth_note_info_note_on(int note, int velocity, float freq);
void intersynth_note_info_note_off(int note);
void intersynth_note_info_note_update(int note, int velocity, float freq);

//NOTE MIDICHANNEL
void intersynth_note_midi_channel(int midichannel);

//EXPRESSIVE CHANGE
void intersynth_expressive_change(int op_num,float freq_factor, float amp_factor);

//CHANGE PARAMETER
//void intersynth_change_by_parameter_set_freq_per_t(float[num_ops] freq_per_t);

#endif //INTERSYNTH_BLUETOOTH_MESSAGES_H
