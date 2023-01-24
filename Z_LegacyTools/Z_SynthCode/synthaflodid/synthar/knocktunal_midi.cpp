
#include "knocktunal_midi.h"

#include <vector>
#include <list>
#include "helpers_midi.h"

std::list<NoteMessage> *note_message_list;
bool note_list_changed = false;

bool has_note_list_changed(){
    if(note_list_changed){
        note_list_changed = false;
        return true;
    }
    return false;
}

void set_note_list(std::list<NoteMessage> *note_list){
    note_message_list = note_list;
}

void handle_note_off(int midi_note){
    for(std::list<NoteMessage>::iterator it = note_message_list->begin(); it != note_message_list->end(); ++it){
        if(it->midi_note == midi_note){
            it = note_message_list->erase(it);
            note_list_changed = true;
        }
    }
}

void midiCallback( double deltatime, std::vector< unsigned char > *message, void */*userData*/ )
{
    if (message->size() >= 3){
        if ((int)message->at(0) == 0x90){
            // send_midi_note((int)message->at(1), (int)message->at(2));
            if((int)message->at(2) > 0){
                note_message_list->push_back(NoteMessage((int)message->at(1), (int)message->at(2)));
                note_list_changed = true;
            }
            else{
                handle_note_off((int)message->at(1));
            }
        }
        else if ((int)message->at(0) == 0x80){
            // send_midi_note((int)message->at(1), 0);
            handle_note_off((int)message->at(1));
        }
        else if ((int)message->at(0) == 0xE0){ // 
            send_midi_message(*message);
        }
        else{
            send_midi_message(*message);
        }
    }
}
