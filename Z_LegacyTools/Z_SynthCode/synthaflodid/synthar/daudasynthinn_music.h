
#ifndef DAUDASYNTHINN_MUSIC_H_982739482
#define DAUDASYNTHINN_MUSIC_H_982739482

/*
 * $Id$
 *
 * This program uses the PortAudio Portable Audio Library.
 * For more information see: http://www.portaudio.com
 * Copyright (c) 1999-2000 Ross Bencina and Phil Burk
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The text above constitutes the entire PortAudio license; however,
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also
 * requested that these non-binding requests be included along with the
 * license above.
 */

#include <stdio.h>
#include <vector>
#include <math.h>

#include <portaudio.h>

#include <mutex>

#define SAMPLE_RATE   (44100)

bool init_portaudio();
bool uninit_portaudio();

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int soundOutputCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData );

void midiCallback( double deltatime, std::vector< unsigned char > *message, void */*userData*/ );

class OperatorValue{
public:
    OperatorValue(int init_not_used = 0){
        frequency_factor = 1.0;
        amplitude = 1.0;
        phase_offset = 0.0;
        sync_to_base_frequency = false;
    }
    OperatorValue(float freq_f, float amp, float phase_off, bool sync){
        frequency_factor = freq_f;
        amplitude = amp;
        phase_offset = phase_off;
        sync_to_base_frequency = sync;
    }
    double frequency_factor;
    double amplitude;
    double phase_offset;
    bool sync_to_base_frequency;

    OperatorValue operator+(const OperatorValue &other) const{
        return OperatorValue(this->frequency_factor + other.frequency_factor, this->amplitude + other.amplitude, this->phase_offset + other.phase_offset, this->sync_to_base_frequency);
    }
    OperatorValue operator*(const float &scalar) const{
        return OperatorValue(this->frequency_factor * scalar, this->amplitude * scalar, this->phase_offset * scalar, this->sync_to_base_frequency);
    }
};

class Algorithm{
public:
    Algorithm(int not_used = 0){

    }
    std::vector<OperatorValue> operators;
    Algorithm operator+(const Algorithm &other) const{
        Algorithm return_algorithm;
        int oper_count = 0;
        if(operators.size() <= other.operators.size()){
            oper_count = operators.size();
        }
        else{
            oper_count = other.operators.size();
        }
        for(int i = 0; i < oper_count; i++){
            return_algorithm.operators.push_back(operators[i] + other.operators[i]);
        }
        return return_algorithm;
    }
    Algorithm operator*(const float &scalar) const{
        Algorithm return_algorithm;
        for(OperatorValue value : operators){
            return_algorithm.operators.push_back(value * scalar);
        }
        return return_algorithm;
    }
};

class OperatorDriver{
public:
    OperatorDriver(OperatorValue value = OperatorValue(0)){
        millis_until_next = 0;
        phase = 0.0;
        phase_increment = 0.0;
        phase_increment_increment = 0.0;
        amplitude_increment = 0;
        freq_fact_increment = 0;
        phase_offset_increment = 0;
        millis_per_frame = 1000.0 / (double)SAMPLE_RATE;

        next_value = value;
        current_value = value;
        current_value.amplitude = 0.0;
        // change_value(value, 25.0);

        sync_steps_total = 24;
        sync_steps_left = 0;

        base_frequency = 110.0;
    }
    void change_value(OperatorValue &value, float millis){
        next_value = value;
        current_value.sync_to_base_frequency = value.sync_to_base_frequency;
        if(millis != 0.0){
            millis_until_next = 1.0 * (double)millis;

            amplitude_increment = 1000.0 * (next_value.amplitude - current_value.amplitude) / (millis_until_next * SAMPLE_RATE);
            freq_fact_increment = 1000.0 * (next_value.frequency_factor - current_value.frequency_factor) / (millis_until_next * SAMPLE_RATE);
            phase_offset_increment = 1000.0 * (next_value.phase_offset - current_value.phase_offset) / (millis_until_next * SAMPLE_RATE);
        }
    }
    void build_increments(double base_frequency){
        this->base_frequency = base_frequency;
        phase_increment = current_value.frequency_factor * base_frequency * 6.284/(double)SAMPLE_RATE;
        phase_increment_increment = freq_fact_increment * base_frequency * 6.284/(double)SAMPLE_RATE;
        if(!current_value.sync_to_base_frequency)
        while(6.28318530718 < phase){
            phase -= 6.28318530718;
        }
    }
    void increment_once(){
        if(current_value.amplitude == 0.0 && next_value.amplitude == 0.0){
            phase = current_value.phase_offset;
        }

        if(millis_until_next != 0.0){
            millis_until_next -= millis_per_frame;
            if(millis_until_next > 0.0){
                current_value.amplitude += amplitude_increment;
                current_value.frequency_factor += freq_fact_increment;
                current_value.phase_offset += phase_offset_increment;
                phase_increment += phase_increment_increment;
            }
            else{
                finish_incr_early_count++;
                millis_until_next = 0.0;
                current_value = next_value;
                phase_increment = current_value.frequency_factor * base_frequency * 6.284/(double)SAMPLE_RATE;
            }
        }
        phase += phase_increment;

        // PHASE SYNC
        if(current_value.sync_to_base_frequency){
            if(sync_steps_left == 0){
                if(phase > 6.28318530718 * current_value.frequency_factor){
                    sync_steps_left = sync_steps_total;
                }
            }
            else if(sync_steps_left > 0){
                // phase -= phase_increment;
                --sync_steps_left;
                if(sync_steps_left == 0){
                    phase -= 6.28318530718 * current_value.frequency_factor;
                }
            }
        }
    }
    double get_next_output(double modulation){
        if(sync_steps_left == 0){
            return current_value.amplitude * sin(phase + current_value.phase_offset + modulation);
        }
        else{
            double clean_output = sin(phase + current_value.phase_offset + modulation);
            double sync_output = sin(phase - 6.28318530718 * current_value.frequency_factor + current_value.phase_offset + modulation);
            double ratio = (double)(sync_steps_total - sync_steps_left) / (double)sync_steps_total;
            return current_value.amplitude * ((1.0 - ratio) * clean_output + ratio * sync_output);
        }
    }
public:
    OperatorValue next_value;
    OperatorValue current_value;
private:
    double millis_per_frame;
    double millis_until_next;
    double phase;
    double phase_increment;
    double phase_increment_increment;
    double amplitude_increment;
    double freq_fact_increment;
    double phase_offset_increment;
    double base_frequency;

    int sync_steps_left;
    int sync_steps_total;

    int finish_incr_early_count = 0;
    int zero_length_changes = 0;
};

template <class T>
struct EnvelopeValue{
    EnvelopeValue(){
        value = T();
        time_ms = 0;
    }
    EnvelopeValue(T v, int t){
        value = v;
        time_ms = t;
    }
    T value;
    float time_ms;
};

template <class T>
class Envelope{
public:
    Envelope(){
        env_mutex.lock();
        start_time_ms = 0;
        finished = true;
        env_mutex.unlock();
    }
    std::vector<EnvelopeValue<T> *> env_values;
    int start_time_ms;
private:
    bool finished;

public:
    std::mutex env_mutex;

    int insert_envelope_value(EnvelopeValue<T> *value){
        env_mutex.lock();
        int inserted_location = -1;
        int removed_location = -1;
        for(int i = 0; i < env_values.size(); i++){
            if(value == env_values[i]){
                //printf("ERASING\n");
                removed_location = i;
                if(inserted_location >= 0) { break; }
            }
            if(inserted_location < 0 && value->time_ms < env_values[i]->time_ms){
                //printf("INSERTING\n");
                inserted_location = i;
                if(removed_location >= 0){ break; }
            }
        }

        if(removed_location >= 0){ // SOME REMOVE
            if(inserted_location == -1){
                inserted_location = env_values.size();
            }
            if(removed_location < inserted_location){ // ACTUALLY SAME??
                --inserted_location;
            }
            if(inserted_location != removed_location){
                env_values.erase(env_values.begin() + removed_location);
                if(inserted_location >= 0){ // BOTH REMOVE AND INSERT
                    //printf("inserted_location: %d, size(): %d, removed_location: %d\n", inserted_location, env_values.size(), removed_location);
                    env_values.insert(env_values.begin() + inserted_location, value);
                }
            }
        }
        else if(inserted_location >= 0){ // ONLY INSERTING
            env_values.insert(env_values.begin() + inserted_location, value);
        }
        else{
            // printf("Envelope.insert_envelope_value: ELSE\n");
            inserted_location = env_values.size();
            env_values.push_back(value);
        }
        env_mutex.unlock();
        //printf("RETURNING\n");
        return inserted_location;
    }

    bool remove_envelope_value(int index){
        if(env_values.size() > 1){
            env_values.erase(env_values.begin() + index);
            return true;
        }
        return false;
    }

    void begin(int time_ms, T *start_value = NULL){
        env_mutex.lock();
        start_time_ms = time_ms;

        finished = false;
        env_mutex.unlock();
    }

    bool is_finished(int time_ms){
        return (time_ms > env_values[env_values.size() - 1]->time_ms);
    }

    T get_current_value(int time_ms, T *initial_value = NULL){
        env_mutex.lock();
        int current_time = time_ms - start_time_ms;
        T value_to_return;
        if(is_finished(current_time)){
            value_to_return = env_values[env_values.size() - 1]->value;
        }
        else{
            int next_time = env_values[0]->time_ms;
            T next_value = env_values[0]->value;
            int previous_time = 0;
            T previous_value = (initial_value == NULL) ? next_value : *initial_value;
            int next_value_index = 1;
            while(next_value_index < env_values.size() && next_time < current_time){
                next_time = env_values[next_value_index]->time_ms;
                next_value = env_values[next_value_index]->value;
                previous_time = env_values[next_value_index - 1]->time_ms;
                previous_value = env_values[next_value_index - 1]->value;
                ++next_value_index;
            }
            float time_ratio = next_time == previous_time ? 0.0 : (float)(current_time - previous_time) / (float)(next_time - previous_time);
            value_to_return = previous_value * (1.0 - time_ratio) + next_value * time_ratio;
        }
        env_mutex.unlock();
        return value_to_return;
    }
};

#endif //#DAUDASYNTHINN_MUSIC_H_982739482