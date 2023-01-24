
#ifndef KNOCKTUNAL_MIDI_H_23423523534
#define KNOCKTUNAL_MIDI_H_23423523534

#include <vector>
#include <list>

#include "helpers_midi.h"

struct NoteMessage{
    NoteMessage(){}
    NoteMessage(int note, int vel){
        midi_note = note;
        velocity = vel;
    }
    int midi_note;
    int velocity;
};

struct CollisionNote{
    CollisionNote(int note, int vel, float ttl){
        midi_note = note;
        velocity = vel;
        time_to_live = ttl;
        send_midi_note(note, vel);
        // printf("Note on\n");
    }
    bool update(float elapsed_time){
        // printf("Calling update on note\n");
        time_to_live -= elapsed_time;
        if(time_to_live <= 0.0){
            send_midi_note(midi_note, 0); // NOTE OFF
            // printf("Note off\n");
            return false;
        }
        return true;
    }
    int midi_note;
    int velocity;
    float time_to_live;
};

bool has_note_list_changed();
void set_note_list(std::list<NoteMessage> *note_list);

void midiCallback( double deltatime, std::vector< unsigned char > *message, void */*userData*/ );


#endif //KNOCKTUNAL_MIDI_H_23423523534