#ifndef NOTES_H_942629834
#define NOTES_H_942629834

#include <vector>
#include <mutex>
#include "daudasynthinn_music.h"

struct NoteNode{
    NoteNode(int note, NoteNode* prev) { this->note = note; previous = prev; }
    int note;
    NoteNode *previous;
};

class Note{
public:
    // Note();
    Note(int midi_note, float freq, double velocity, int time_ms, bool released = false);

    void release(float time_ms);
    void restart(float time_ms);
    void stop();

    void update(float time_ms);
    bool is_finished();

    std::vector<OperatorDriver> &get_operator_drivers();
    OperatorDriver &get_amp_driver();

    float base_frequency;
    int midi_note;
    double velocity_amp;
    float current_time_ms;
    float last_event_time;
    bool released;
    bool stopped;
private:
    std::vector<OperatorDriver> operators;
    OperatorDriver amp_driver;
    float amp_value_on_release;
    Algorithm algo_value_on_release;
};

class NoteCollections{
public:
    static NoteCollections &get_instance();

    NoteNode *MIDI_remove_note_from_stack(int note, NoteNode *node);
    void MIDI_change_note(int note, int velocity);

    void update_notes(float time_ms);

    bool toggle_polyphony();

    NoteNode *note_stack_top;
    std::vector<Note> active_notes;

    std::mutex notes_mutex;

    bool POLYPHONIC = true;

private:
    NoteCollections();

    float current_time_ms;
};

#endif //NOTES_H_942629834