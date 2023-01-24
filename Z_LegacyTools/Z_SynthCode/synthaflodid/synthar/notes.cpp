#include <stdio.h>
#include "notes.h"
#include "patch.h"


// NOTE

// Note::Note(){
    
// }

Note::Note(int midi_note, float freq, double velocity, int time_ms, bool released){
    this->midi_note = midi_note;
    base_frequency = freq;
    velocity_amp = velocity;
    last_event_time = time_ms;
    this->released = released;
    stopped = false;
    Algorithm algo = Patch::get_instance().get_algo_value(0, released);
    for(int i = 0; i < algo.operators.size(); i++){
        this->operators.push_back(OperatorDriver(algo.operators[i]));
    }
    this->amp_driver.current_value.amplitude = Patch::get_instance().get_amp_value(0, released) * velocity_amp;
}

void Note::release(float time_ms){

    amp_value_on_release = Patch::get_instance().get_amp_value(time_ms - last_event_time, false);
    algo_value_on_release = Patch::get_instance().get_algo_value(time_ms - last_event_time, false);

    released = true;
    last_event_time = time_ms;
    current_time_ms = time_ms;

}

void Note::restart(float time_ms){

    // amp_value_on_release = Patch::get_instance().get_amp_value(time_ms - last_event_time, false);
    // algo_value_on_release = Patch::get_instance().get_algo_value(time_ms - last_event_time, false);

    released = false;
    last_event_time = time_ms;
    current_time_ms = time_ms;

}

void Note::stop(){
    stopped = true;
}

void Note::update(float time_ms){
    float time_since_last_update = time_ms - current_time_ms;
    float time_since_last_event = time_ms - last_event_time;

    OperatorValue amp_val;
    Patch &patch = Patch::get_instance();

    if(released && patch.past_release(current_time_ms - last_event_time)){
        stopped = true;
    }

    if(stopped){
        amp_val.amplitude = 0.0;
    }
    else{
        Algorithm algo;

        if(released){
            algo = patch.get_algo_value(time_since_last_event, true, &algo_value_on_release);
            amp_val.amplitude = patch.get_amp_value(time_since_last_event, true, &amp_value_on_release) * velocity_amp;
        }
        else{
            algo = patch.get_algo_value(time_since_last_event, false);
            amp_val.amplitude = patch.get_amp_value(time_since_last_event, false) * velocity_amp;
        }

        // WERE ANY OPERATORS DELETED?
        while(this->operators.size() > algo.operators.size()){
            printf("OPERATORS DELETED MID-NOTE\n");
            this->operators.pop_back();
        }

        // MODULATORS
        for(int i = 0; i < algo.operators.size(); i++){
            // WERE ANY OPERATORS ADDED?
            if(this->operators.size() <= i){
                printf("OPERATORS ADDED MID-NOTE\n");
                this->operators.push_back(OperatorDriver(algo.operators[i]));
            }
            else{
                this->operators[i].change_value(algo.operators[i], time_since_last_update);
            }
        }
    }
    amp_driver.change_value(amp_val, time_since_last_update);

    current_time_ms = time_ms;
}

bool Note::is_finished(){
    return (stopped && amp_driver.current_value.amplitude <= 0.0);
}

std::vector<OperatorDriver> &Note::get_operator_drivers(){
    return this->operators;
}

OperatorDriver &Note::get_amp_driver(){
    return this->amp_driver;
}

// NOTE COLLECTIONS

NoteCollections &NoteCollections::get_instance(){
    static NoteCollections instance;
    return instance;
}

NoteCollections::NoteCollections(){
    note_stack_top = NULL;
}

NoteNode *NoteCollections::MIDI_remove_note_from_stack(int note, NoteNode *node){
    if(node == NULL){
        return NULL;
    }
    else if(node->note == note){
        NoteNode *tmp = node->previous;
        delete node;
        return tmp;
    }
    node->previous = MIDI_remove_note_from_stack(note, node->previous);
    return node;
}

void NoteCollections::MIDI_change_note(int note, int velocity){
//printf("BEGIN MIDI change note\n");

    notes_mutex.lock();

    if(POLYPHONIC){
        if(velocity == 0){ // NOTE OFF
            for(int i = 0; i < this->active_notes.size(); i++){
                if(active_notes[i].midi_note == note){
                    if(!active_notes[i].released){
                        active_notes[i].release(current_time_ms);
                    }
                }
            }
        }
        else{ // NOTE ON
            bool found_note = false;
            for(int i = 0; i < this->active_notes.size(); i++){  // IN CASE THE SAME NOTE IS STILL RELEASING OR SOMETHING
                if(active_notes[i].midi_note == note){ // SAME NOTE ALREADY THERE (PROBABLY IN RELEASE)

                    if(active_notes[i].released){
                        active_notes[i].stop();
                    }
                    else{
                        found_note = true;
                    }
                }
            }
            if(!found_note){
                double note_frequency = 440.0 * pow(1.059463094359, note - 69);
                active_notes.push_back(Note(note, note_frequency, double(velocity) / 127.0, current_time_ms));
            }
        }
    }
    else{ // MONOPHONIC
        int current_note = -1;
        if(note_stack_top != NULL){
            current_note = note_stack_top->note;
            note_stack_top = MIDI_remove_note_from_stack(note, note_stack_top); // FIRST CLEAN STACK OF THIS NOTE
        }
        if(velocity == 0){ // NOTE OFF
            if(note == current_note){ // RELEASING THE CURRENT NOTE
                if(note_stack_top == NULL){ // LAST NOTE RELEASED
                    active_notes[0].release(current_time_ms);
                }
                else{ // OTHER_NOTES_WAITING
                    note = note_stack_top->note;

                    active_notes[active_notes.size() - 1].stop();

                    double note_frequency = 440.0 * pow(1.059463094359, note - 69);
                    // VELOCITY NEEDS TO BE STORED ON THE STACK FOR MONOPHONY
                    // active_notes.push_back(Note(note, note_frequency, double(velocity) / 127.0, current_time_ms));
                    active_notes.push_back(Note(note, note_frequency, 1.0, current_time_ms));
                    // printf("Playing tone: %f\n", active_notes[0].base_frequency);
                }
            }
        }
        else{
        // NOTE ON
            // printf("Note-on: %d\n", note);
            note_stack_top = new NoteNode(note, note_stack_top);

            if(active_notes.size() > 0){
                active_notes[active_notes.size() - 1].stop();
            }

            double note_frequency = 440.0 * pow(1.059463094359, note - 69);
            // VELOCITY NEEDS TO BE STORED ON THE STACK FOR MONOPHONY
            // active_notes.push_back(Note(note, note_frequency, double(velocity) / 127.0, current_time_ms));
            active_notes.push_back(Note(note, note_frequency, 1.0, current_time_ms));
            // printf("Playing tone: %f\n", active_notes[0].base_frequency);
        }
    }
    notes_mutex.unlock();
}

void NoteCollections::update_notes(float time_ms){
    notes_mutex.lock();
    this->current_time_ms = time_ms;

    for(int i = 0; i < this->active_notes.size(); i++){
        if(active_notes[i].is_finished()){
            // printf("Stopping tone: %f\n", active_notes[i].base_frequency);
            active_notes.erase(active_notes.begin() + i);
            i--;
        }
    }
    
    for(int i = 0; i < this->active_notes.size(); i++){
        active_notes[i].update(current_time_ms);
    }
    // go through all notes in active collection
    notes_mutex.unlock();
}

bool NoteCollections::toggle_polyphony(){
    POLYPHONIC = !POLYPHONIC;
    return POLYPHONIC;
}