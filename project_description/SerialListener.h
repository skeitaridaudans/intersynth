
#ifndef SERIALLISTENER_H_443346
#define SERIALLISTENER_H_443346

#include "Music.h"
#include "JunkTests.h"

// 12 bita Teensy 4.0 og 4.1: Serial7
// Gamla 16 bita Teensy 4.1: Serial5
#define PI_SERIAL Serial6

class SerialListener{
public:
    SerialListener(){
        PI_SERIAL.begin(115200);
        // Serial.begin(9600);
        // Serial.write("STARTING\n");
        bytes_in_transit = 0;
        read_bytes = NULL;
    }

    unsigned char *check(){
        delete[] read_bytes;
        read_bytes = NULL;
        if(bytes_in_transit == 0 && PI_SERIAL.available() > 0){
            bytes_in_transit = PI_SERIAL.read();
        }
        if (bytes_in_transit > 0 && PI_SERIAL.available() >= bytes_in_transit) {
            read_bytes = new unsigned char[bytes_in_transit];
            PI_SERIAL.readBytes(read_bytes, bytes_in_transit);
            // for(int i = 0; i < bytes_in_transit; i++){
            //     read_bytes[i] = PI_SERIAL.read();
            // }
            bytes_in_transit = 0;

            // Serial.write("Stuff on wire.  Size: ");
            // serial_write_integer_value(bytes_in_transit);
            // Serial.write("\n");
        }
        return read_bytes;
    }

private:
    unsigned char bytes_in_transit;
    unsigned char *read_bytes;
};

#include <stdint.h>

static inline float  get_float32le(const void *ptr)
{
    const unsigned char *const  data = (const unsigned char *)ptr;
    union {
        float     f;
        uint32_t  u;
    } temp;
    temp.u = data[0] | ((uint32_t)(data[1]) << 8) | ((uint32_t)(data[2]) << 16) | ((uint32_t)(data[3]) << 24);
    return temp.f;
}


OperatorValue expressive_change_amounts[MAX_OPERATORS];


void change_operator_values_throughout_envelope(int oper_num, float freq_factor, float amp_factor, bool linear){
    if(oper_num < MAX_OPERATORS){
        EnvelopeValue<Algorithm> *env_val = NULL;
        int i = 0;
        while(true){
            env_val = alg_attack_envelope.get_envelope_value(i++);
            if(env_val == NULL) break;
            if(linear){
                env_val->value.operators[oper_num].frequency_factor += freq_factor;
                env_val->value.operators[oper_num].amplitude += amp_factor;
            }
            else{
                env_val->value.operators[oper_num].frequency_factor *= freq_factor;
                env_val->value.operators[oper_num].amplitude *= amp_factor;
            }
        }
        i = 0;
        while(true){
            env_val = alg_release_envelope.get_envelope_value(i++);
            if(env_val == NULL) break;
            if(linear){
                env_val->value.operators[oper_num].frequency_factor += freq_factor;
                env_val->value.operators[oper_num].amplitude += amp_factor;
            }
            else{
                env_val->value.operators[oper_num].frequency_factor *= freq_factor;
                env_val->value.operators[oper_num].amplitude *= amp_factor;
            }
        }
    }
}


SerialListener serial_listener;

#define OPERATOR_VALUES 0x10
#define EXPRESSIVE_CHANGE 0x20  // is applied to single operator throughout all envelopes
#define AMP_ENVELOPE_VALUE 0x30
    #define AMP_ATTACK 0x01
    #define AMP_RELEASE 0x02
#define MODULATED_BY 0x40
#define CARRIER_ON 0x50
#define CARRIER_OFF 0x60
#define CLEAR_OPERATORS 0x70
#define ALGORITHM_ENVELOPE_INFO 0x80
    #define INSERT 0x01
    #define REMOVE 0x02
    #define ENVELOPE_SIZE 0x03
    #define UPDATE_TIME 0x04
    #define SOLO_POINT 0x05
#define FREQ_SYNCED_BY 0x90
#define AMP_FIXED 0xA0
    #define IGNORE_ENVELOPE 0x01
    #define IGNORE_VELOCITY 0x02
#define NOTE_INFO 0xB0
    #define NOTE_ON 0x01
    #define NOTE_OFF 0x02
    #define NOTE_UPDATE 0x03
#define MIDI_CHANNEL 0xC0
#define CHANGE_BY_PARAMETER 0xD0
    // #define SET_AMOUNTS_PER_T 0x01 // TOO BIG FOR THE 64 BYTE SERIAL BUFFER
    #define SET_FREQ_PER_T 0x02
    #define SET_AMPS_PER_T 0x03
    #define LINEAR_T 0x04
    #define FREQ_T 0x05
    #define AMP_T 0x06

void receiveSerialMessages(){
    unsigned char *bytes_read = serial_listener.check();
    if(bytes_read != NULL){                             // CHANGE IF TO WHILE !!!!!!!!!!!!!!!!!!!!!!!
        // Serial.write("Some message: ");
        // serial_write_integer_value(bytes_read[0]);
        // Serial.write("\n");
        if((bytes_read[0] & 0xF0) == OPERATOR_VALUES){
            unsigned char oper_num = bytes_read[0] & 0x0F;
            if(oper_num < MAX_OPERATORS){
                bool attack = (bytes_read[1] & 0x80) != 0;
                int alg_index = bytes_read[1] & 0x7F;
                if(attack){
                    EnvelopeValue<Algorithm> *env_val = alg_attack_envelope.get_envelope_value(alg_index);
                    if(env_val != NULL){
                        env_val->value.operators[oper_num].frequency_factor = get_float32le(bytes_read + 2);
                        env_val->value.operators[oper_num].amplitude = get_float32le(bytes_read + 6);
                    }
                }
                else{
                    EnvelopeValue<Algorithm> *env_val = alg_release_envelope.get_envelope_value(alg_index);
                    if(env_val != NULL){
                        env_val->value.operators[oper_num].frequency_factor = get_float32le(bytes_read + 2);
                        env_val->value.operators[oper_num].amplitude = get_float32le(bytes_read + 6);
                    }
                }
            }
        }
        if((bytes_read[0] & 0xF0) == EXPRESSIVE_CHANGE){
            unsigned char oper_num = bytes_read[0] & 0x0F;
            if(oper_num < MAX_OPERATORS){
                change_operator_values_throughout_envelope(bytes_read[0] & 0x0F,
                                                        1 + get_float32le(bytes_read + 1),
                                                        1 + get_float32le(bytes_read + 5),
                                                        false);
            }
        }
        if((bytes_read[0] & 0xF0) == CHANGE_BY_PARAMETER){
            // if((bytes_read[0] & 0x0F) == SET_AMOUNTS_PER_T){ // TOO BIG FOR THE 64 BYTE SERIAL BUFFER
            //     for(int i = 0; i < MAX_OPERATORS; i++){
            //         expressive_change_amounts[i].frequency_factor = get_float32le(bytes_read + 1 + i*8);
            //         expressive_change_amounts[i].amplitude = get_float32le(bytes_read + 5 + i*8);
            //     }
            // }
            // else
            if((bytes_read[0] & 0x0F) == SET_FREQ_PER_T){
                for(int i = 0; i < MAX_OPERATORS; i++){
                    expressive_change_amounts[i].frequency_factor = get_float32le(bytes_read + 1 + i*4);
                }
            }
            else if((bytes_read[0] & 0x0F) == SET_AMPS_PER_T){
                for(int i = 0; i < MAX_OPERATORS; i++){
                    expressive_change_amounts[i].amplitude = get_float32le(bytes_read + 1 + i*4);
                }
            }
            else if((bytes_read[0] & 0x0F) == LINEAR_T){
                double t = get_float32le(bytes_read + 1);
                for(int i = 0; i < MAX_OPERATORS; i++){
                    change_operator_values_throughout_envelope(i,
                                                            expressive_change_amounts[i].frequency_factor * t,
                                                            expressive_change_amounts[i].amplitude * t,
                                                            true);
                }
            }
            else if((bytes_read[0] & 0x0F) == FREQ_T){
                double t = get_float32le(bytes_read + 1);
                for(int i = 0; i < MAX_OPERATORS; i++){
                    change_operator_values_throughout_envelope(i,
                                                            1.0 + expressive_change_amounts[i].frequency_factor * t,
                                                            1.0, false);
                }
            }
            else if((bytes_read[0] & 0x0F) == AMP_T){
                double t = get_float32le(bytes_read + 1);
                for(int i = 0; i < MAX_OPERATORS; i++){
                    change_operator_values_throughout_envelope(i,
                                                            1.0,
                                                            1.0 + expressive_change_amounts[i].amplitude * t,
                                                            false);
                }
            }
        }
        else if((bytes_read[0] & 0xF0) == ALGORITHM_ENVELOPE_INFO){
            if((bytes_read[0] & 0x0F) == INSERT){
                bool attack = (bytes_read[1] & 0x80) != 0;
                int alg_index = bytes_read[1] & 0x7F;
                // Serial.write("INSERT: ");
                // serial_write_integer_value((int)attack);
                // Serial.write(", ");
                // serial_write_integer_value((int)alg_index);
                // Serial.write("\n");
                Algorithm new_alg;
                double time = get_float32le(bytes_read + 2);
                if(attack){
                    if(time < 0){
                        alg_attack_envelope.set_envelope_value(alg_index, new_alg, time);
                    }
                    else{
                        if(alg_index < 0x7F){
    // FIX IN TEENSY CODE - INSERT ON INDEX = SIZE SHOULD APPEND.  INSTEAD I HAVE TO RESIZE AND THEN INSERT (insert also overwrites the value on the index).
                            EnvelopeValue<Algorithm> *env_val = alg_attack_envelope.get_envelope_value(alg_index);
                            if(env_val != NULL){
                                new_alg = env_val->value;
                            }
                        }
                        alg_attack_envelope.insert_envelope_value(new EnvelopeValue<Algorithm>(new_alg, time));
                    }
                }
                else{
                    if(time < 0){
                        alg_release_envelope.set_envelope_value(alg_index, new_alg, time);
                    }
                    else{
                        if(alg_index < 0x7F){
                            EnvelopeValue<Algorithm> *env_val = alg_release_envelope.get_envelope_value(alg_index);
                            if(env_val != NULL){
                                new_alg = env_val->value;
                            }
                        }
                        alg_release_envelope.insert_envelope_value(new EnvelopeValue<Algorithm>(new_alg, time));
                    }
                }
            }
            else if((bytes_read[0] & 0x0F) == REMOVE){
                bool attack = (bytes_read[1] & 0x80) != 0;
                int alg_index = bytes_read[1] & 0x7F;
                // Serial.write("REMOVE: ");
                // serial_write_integer_value((int)attack);
                // Serial.write(", ");
                // serial_write_integer_value((int)alg_index);
                // Serial.write("\n");
                if(attack){
                    alg_attack_envelope.remove_envelope_value(alg_index);
                }
                else{
                    alg_release_envelope.remove_envelope_value(alg_index);
                }
            }
            else if((bytes_read[0] & 0x0F) == ENVELOPE_SIZE){
                bool attack = (bytes_read[1] & 0x80) != 0;
                int new_size = bytes_read[1] & 0x7F;
                if(attack){
                    alg_attack_envelope.set_envelope_size(new_size);
                }
                else{
                    alg_release_envelope.set_envelope_size(new_size);
                }
            }
            else if((bytes_read[0] & 0x0F) == UPDATE_TIME){
                bool attack = (bytes_read[1] & 0x80) != 0;
                int alg_index = bytes_read[1] & 0x7F;
                if(attack){
                    alg_attack_envelope.set_envelope_value_time(alg_index, get_float32le(bytes_read + 2));
                    // EnvelopeValue<Algorithm> *env_val = alg_attack_envelope.get_envelope_value(alg_index);
                    // env_val->time = get_float32le(bytes_read + 2);
                    // Serial.write("ATTACK MOTION\n");
                }
                else{
                    alg_release_envelope.set_envelope_value_time(alg_index, get_float32le(bytes_read + 2));
                    // EnvelopeValue<Algorithm> *env_val = alg_release_envelope.get_envelope_value(alg_index);
                    // env_val->time = get_float32le(bytes_read + 2);
                    // Serial.write("RELEASE MOTION\n");
                }
            }
            else if((bytes_read[0] & 0x0F) == SOLO_POINT){
                bool attack = (bytes_read[1] & 0x80) != 0;
                int alg_index = bytes_read[1] & 0x7F;
                if(alg_index == 0x7F){
                    attack_solo = false;
                    release_solo = false;
                    alg_attack_envelope.set_solo(0x7F);
                    alg_release_envelope.set_solo(0x7F);
                }
                else if(attack){
                    attack_solo = true;
                    release_solo = false;
                    alg_attack_envelope.set_solo(alg_index);
                    alg_release_envelope.set_solo(0x7F);
                }
                else{
                    attack_solo = false;
                    release_solo = true;
                    alg_attack_envelope.set_solo(0x7F);
                    alg_release_envelope.set_solo(alg_index);
                }
            }
            // else if((bytes_read[0] & 0xF0) == ALGORITHM_ENVELOPE_SWAP){
            //     bool attack = (bytes_read[1] & 0x80) != 0;
            //     int alg_index_1 = bytes_read[1] & 0x7F;
            //     int alg_index_2 = bytes_read[2];
            //     if(attack){
            //         alg_attack_envelope.swap_envelope_values(alg_index_1, alg_index_2);
            //     }
            //     else{
            //         alg_release_envelope.swap_envelope_values(alg_index_1, alg_index_2);
            //     }
            // }
        }
        else if((bytes_read[0] & 0xF0) == AMP_ENVELOPE_VALUE){
            if((bytes_read[0] & 0x0F) == AMP_ATTACK){
                amp_attack_envelope.set_envelope_value(bytes_read[1], get_float32le(bytes_read + 2), get_float32le(bytes_read + 6));
            }
            else if((bytes_read[0] & 0x0F) == AMP_RELEASE){
                amp_release_envelope.set_envelope_value(bytes_read[1], get_float32le(bytes_read + 2), get_float32le(bytes_read + 6));
            }
        }
        else if((bytes_read[0] & 0xF0) == AMP_FIXED){
            if((bytes_read[0] & 0x0F) == IGNORE_ENVELOPE){
                amplitude_level = get_float32le(bytes_read + 1);
                if(amplitude_level < 0){
                    amplitude_solo = false;
                    amplitude_level = 1.0;
                }
                else{
                    amplitude_solo = true;
                }
            }
            else if((bytes_read[0] & 0x0F) == IGNORE_VELOCITY){
                ignore_velocity = bytes_read[1];
            }
        }
        else if((bytes_read[0] & 0xF0) == MODULATED_BY){
            unsigned char oper_num = bytes_read[0] & 0x0F;
            if(oper_num < MAX_OPERATORS){
                unsigned int mod_oper_num = bytes_read[1] & 0x0F;
                if(mod_oper_num < MAX_OPERATORS){
                    int current_location = -1;
                    for(unsigned int i = 0; i < modulated_by[oper_num].size(); i++){
                        if(modulated_by[oper_num][i] == mod_oper_num){
                            current_location = i;
                            break;
                        }
                    }
                    if(bytes_read[1] & 0xF0){
                        if(current_location == -1){
                            modulated_by[oper_num].push_back(mod_oper_num);
                        }
                    }
                    else{
                        if(current_location >= 0){
                            modulated_by[oper_num].erase(modulated_by[oper_num].begin() + current_location);
                        }
                    }
                }
            }
        }
        else if((bytes_read[0] & 0xF0) == FREQ_SYNCED_BY){
            unsigned char oper_num = bytes_read[0] & 0x0F;
            if(oper_num < MAX_OPERATORS){
                unsigned int sync_oper_num = bytes_read[1] & 0x0F;
                if(sync_oper_num < MAX_OPERATORS){
                    frequency_synced_by[oper_num] = sync_oper_num;
                }
                else{
                    frequency_synced_by[oper_num] = -1;
                }
            }
        }
        else if((bytes_read[0] & 0xF0) == CARRIER_ON){
            unsigned char oper_num = bytes_read[0] & 0x0F;
            if(oper_num < MAX_OPERATORS){
                bool already_there = false;
                for(unsigned int i = 0; i < carriers.size(); i++){
                    if(carriers[i] == oper_num){
                        already_there = true;
                        break;
                    }
                }
                if(!already_there){
                    carriers.push_back(oper_num);
                    // Serial.write("\nCarrier added\n\n");
                }
            }
        }
        else if((bytes_read[0] & 0xF0) == CARRIER_OFF){
            unsigned char oper_num = bytes_read[0] & 0x0F;
            if(oper_num < MAX_OPERATORS){
                int current_location = -1;
                for(unsigned int i = 0; i < carriers.size(); i++){
                    if(carriers[i] == oper_num){
                        current_location = i;
                        break;
                    }
                }
                if(current_location >= 0){
                    carriers.erase(carriers.begin() + current_location);
                    // Serial.write("\nCarrier removed\n\n");
                }
            }
        }
        else if((bytes_read[0] & 0xF0) == CLEAR_OPERATORS){
            carriers.clear();
            for(int i = 0; i < MAX_OPERATORS; i++){
                modulated_by[i].clear();
            }
        }
        else if((bytes_read[0] & 0xF0) == NOTE_INFO){
            if((bytes_read[0] & 0x0F) == NOTE_ON){
                handleNoteUpdate(bytes_read[1], bytes_read[2], get_float32le(bytes_read + 3));
            }
            else if((bytes_read[0] & 0x0F) == NOTE_OFF){
                handleNoteUpdate(bytes_read[1], 0, 0.0);
            }
            else if((bytes_read[0] & 0x0F) == NOTE_UPDATE){
                handleNoteUpdate(bytes_read[1], bytes_read[2], get_float32le(bytes_read + 3));
            }
        }
        else if((bytes_read[0] & 0xF0) == MIDI_CHANNEL){
            midi_channel = bytes_read[0] & 0x0F;
        }
    }
}

#endif