//
// Created by star on 6.3.2023.
//

#include <assert.h>
#include "../include/messages.h"
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

// following functions not in intersynth_midi
// @TODO to implement

void intersynth_clear_operators();

/*
 fra https://github.com/skeitaridaudans/lovesynths-cpp/blob/main/operators.cpp
 WTF is that??? SOLO_POINT? hvad er attack solo?

void send_default_algorithm(){

    send_full_envelopes();

    send_algorithm();

    send_current_operators(); // MAY HAVE TO DO THIS FOR EVERY ALGORITHM ENVELOPE POINT ... LATER FOR send_full_algorithm, if needed

    send_byte(2);
    send_byte(ALGORITHM_ENVELOPE_INFO + SOLO_POINT);         // #define ALGORITHM_ENVELOPE_INFO 0x80 + #define SOLO_POINT 0x05
    send_byte(0x7F);                                         // # BIG NUMBER MEANS REMOVE SOLO
    // send_byte(self.editing_point[1] + 0x80)     // # ATTACK SOLO
    // send_byte(self.editing_point[1])            // # RELEASE SOLO
}
*/

/*

void send_algorithm_envelopes(){
    int attack_envelope_size = 0;
    for(; attack_envelope_size < 4 && alg_env_point_active[attack_envelope_size]; attack_envelope_size++){}
    int release_envelope_size = alg_env_point_active[4] ? 1 : 0;

    // FIRST SEND THAT THERE IS ONLY ONE ATTACK ENVELOPE POINT AND NO RELEASE
    send_byte(2);       // message size: 2 bytes
    send_byte(ALGORITHM_ENVELOPE_INFO + ENVELOPE_SIZE);    // #define ALGORITHM_ENVELOPE_INFO 0x80 + #define ENVELOPE_SIZE 0x03
    send_byte(attack_envelope_size + 0x80);// attack envelope size
    send_byte(2);
    send_byte(ALGORITHM_ENVELOPE_INFO + ENVELOPE_SIZE);    // #define ALGORITHM_ENVELOPE_INFO 0x80 + #define ENVELOPE_SIZE 0x03
    send_byte(release_envelope_size + 0);   // release envelope size
}

 */

// @TODO Sophie need to check again amp env in teensy code

//AMP_ENVELOPE_INFO

//AMP_ENVELOPE_VALUE
//    attack + index
//    time
//    amp_value

//ALGORITHM_ENVELOPE_INFO
// ATTACK 0x80
void intersynth_alg_envelope_info_insert(bool attack, int alg_index, float time);
void intersynth_alg_envelope_info_remove(bool attack, int alg_index);
void intersynth_alg_envelope_info_envelope_size(bool attack, int new_size);
void intersynth_alg_envelope_info_update_time(bool attack, int alg_index, float new_time);
void intersynth_alg_envelope_info_solo_value(bool attack, int alg_index);

//AMP FIXED
void intersynth_amp_fixed_ignore_envelope(float amp_level);
void intersynth_amp_fixed_ignore_velocity(bool ignore);


//FREQ SYNCED BY
void intersynth_freq_synced_by(int operator_id, int sync_operator_num, bool on_off);



//NOTE INFO
void intersynth_note_info_note_on(int key, int velocity, float freq)
{
// Range check
assert(key <= 127 && key >= 0);
assert(velocity <= 127 && velocity >= 0);
// assert(freq )
}

void intersynth_note_info_note_off(int key);
void intersynth_note_info_note_update(int key, int velocity, float freq);

//NOTE MIDICHANNEL
void intersynth_note_midi_channel(int midichannel);

//EXPRESSIVE CHANGE
void intersynth_expressive_change(int op_num,float freq_factor, float amp_factor);