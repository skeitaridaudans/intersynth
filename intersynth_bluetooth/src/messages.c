//
// Created by star on 6.3.2023.
//

#include "../include/messages.h"

//FLOATING FUNCTIONS
// function to store a floating point value in 5 bytes with each byte containing 7 bits
void store_float_in_buffer(char *buffer, float value) {
    uint32_t float_bits = *((uint32_t*) &value);  // get the bits of the floating point value
    buffer[0] = (char) (float_bits & 0x7F);
    buffer[1] = (char) ((float_bits >> 7) & 0x7F);
    buffer[2] = (char) ((float_bits >> 14) & 0x7F);
    buffer[3] = (char) ((float_bits >> 21) & 0x7F);
    buffer[4] = (char) ((float_bits >> 28) & 0x7F);
}

// function to reconstruct a floating point value from a 5-byte buffer with each byte containing 7 bits
float reconstruct_float_from_buffer(const char *buffer) {
    uint32_t float_bits = ((uint32_t) buffer[0] & 0x7F) |
                          (((uint32_t) buffer[1] & 0x7F) << 7) |
                          (((uint32_t) buffer[2] & 0x7F) << 14) |
                          (((uint32_t) buffer[3] & 0x7F) << 21) |
                          (((uint32_t) buffer[4] & 0x7F) << 28);
    return *((float*) &float_bits);  // convert the bits back to a floating point value
}

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
