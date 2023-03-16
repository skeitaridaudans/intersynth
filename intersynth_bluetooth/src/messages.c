//
// Created by star on 6.3.2023.
//

#include "../include/messages.h"
#include <assert.h>
#include "../include/bluetooth.h"

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
void intersynth_send_note_on(unsigned char key, unsigned char velocity) //Intersynth_midi
{
// Range check
    assert(key <= 127 && key >= 0);
    assert(velocity <= 127 && velocity >= 0);


// Construct the message
    unsigned char msg[MIDI_NOTE_MESSAGE_SIZE];
    msg[0] = 0x90;
    msg[1] = key;
    msg[2] = velocity;
    intersynth_send(msg, MIDI_NOTE_MESSAGE_SIZE);
}
void intersynth_send_note_off(unsigned char key) //Intersynth_midi
{
    intersynth_send_note_on(key, 0);
}

//OPERATOR CHANGE
void intersynth_change_operator_values(unsigned char operator, unsigned char alg_index, bool attack, float frequency_factor, float amplitude) //Intersynth_midi
{
unsigned char msg[16];
msg[0] = SYSSEX_START;
msg[1] = INTERSYNTH_IDENTIFIER;
msg[2] = 0x15; // Size
msg[4] = OPERATOR_VALUES + operator; // 0x10 + operator value
msg[5] = ((unsigned char) attack<<7) + alg_index;


// need to call store_float_in_buffer instead of fragment_floating
// to convert the float frequency_factor
store_float_in_buffer(&msg[6], frequency_factor);


// doing the same to convert the float amplitude
store_float_in_buffer(&msg[11], amplitude); // Param 5

msg[16] = MESSAGE_END;
// no need to have rtmidiout stuff here, the function send the msg
// no need for a return either for that void here
intersynth_send(msg, 16);
}

//OPERATOR MODULATION
void intersynth_add_modulator(int operator_id, int modulator_id) //Intersynth_midi
{
    unsigned char msg[6];
    msg[0] = SYSSEX_START;
    msg[1] = INTERSYNTH_IDENTIFIER;
    msg[2] = 0x02;
    msg[3] = MODULATED_BY + operator_id;
    msg[4] = MODULATOR_ON + modulator_id;
    msg[5] = MESSAGE_END;
    intersynth_send(msg, 6);
}
void intersynth_remove_modulator(int operator_id, int modulator_id) //Intersynth_midi
{
    unsigned char msg[6];
    msg[0] = SYSSEX_START;
    msg[1] = INTERSYNTH_IDENTIFIER;
    msg[2] = 0x02;
    msg[3] = MODULATED_BY + operator_id;
    msg[4] = MODULATOR_OFF + modulator_id;
    msg[5] = MESSAGE_END;
    intersynth_send(msg, 6);
}


//OPERATOR CARRIERS
void intersynth_add_carrier(int operator_id) //intersynth_midi
{
    unsigned char msg[5];
    msg[0] = SYSSEX_START;
    msg[1] = INTERSYNTH_IDENTIFIER;
    msg[2] = 0x01;
    msg[3] = CARRIER_ON + operator_id;
    msg[4] = MESSAGE_END;
    intersynth_send(msg, 5);
}


void intersynth_remove_carrier(int operator_id) //intersynth_midi
{
    unsigned char msg[5];
    msg[0] = SYSSEX_START;
    msg[1] = INTERSYNTH_IDENTIFIER;
    msg[2] = 0x01;
    msg[3] = CARRIER_OFF + operator_id;
    msg[4] = MESSAGE_END;
    intersynth_send(msg, 5);
}
