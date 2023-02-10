//
// Created by star on 9.2.2023.
//

#include "intersynth.h"

void intersynth_init()
{
    midiout = rtmidi_out_create_default();
}
uint32_t intersynth_get_num_ports()
{
    uint32_t port_num;
    port_num = rtmidi_get_port_count(midiout); //Add error checking
    return port_num;
}
char* intersnyth_get_port_name(uint32_t port_num)
{
    int32_t size_needed = 0;
    char * arr = NULL;
    rtmidi_get_port_name(midiout, port_num, arr, &size_needed);
    //Error checking via midiout->ok needed
    arr = (char *)malloc(size_needed);
    memset(arr, 0, size_needed);
    rtmidi_get_port_name(midiout, port_num, arr, &size_needed);
    //Error checking via midiout->ok needed
    //Need to free arr after copying to a std::string.
    return arr;
}
bool intersynth_select_port(uint32_t port_num)
{
    rtmidi_open_port(midiout, port_num, "Intersynth");
    return midiout->ok;
}
bool intersnyth_send_note(unsigned char key, unsigned char velocity)
{
    // Range check
    assert(key <= 127 && key >= 0);
    assert(velocity <= 127 && velocity >= 0);

    // Construct the message
    unsigned char msg[MIDI_NOTE_MESSAGE_SIZE];
    msg[0] = 0x90;
    msg[1] = key;
    msg[2] = velocity;
    // Send the message
    rtmidi_out_send_message(midiout, msg, MIDI_NOTE_MESSAGE_SIZE);
    // Return status code.
    return midiout->ok;
}

