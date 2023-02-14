//
// Created by star on 9.2.2023.
//

#include "intersynth.h"
struct RtMidiWrapper* midiout = {0};
void intersynth_init()
{
    midiout = rtmidi_out_create_default();
}
const char * intersynth_get_error()
{
    return midiout->msg;
}

void intersynth_die()
{
    // Close the port from the device
    rtmidi_close_port(midiout);
    // Free the device (The object itself (Call the deconstructor))
    rtmidi_out_free(midiout);
}

uint32_t intersynth_get_num_ports()
{
    uint32_t port_num;
    port_num = rtmidi_get_port_count(midiout); //Add error checking
    return port_num;
}
char* intersnyth_get_port_name(uint32_t port_num)
{        // Close the port from the device
    rtmidi_close_port(midiout);
    // Free the device (The object itself (Call the deconstructor))
    rtmidi_out_free(midiout);
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

void intersynth_free_get_port_name(char * arr)
{
    free(arr);
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
bool intersynth_change_operator_values(unsigned char operator, unsigned char alg_index, bool attack, float frequency_factor, float amplitude)
{
    /*
     * Start message with 0xF0
     * Next is            0x70  - These 2 bytes define the start of SYSEX message
     * Size  4 <= x <= 255
     * Function id 0 <= x <= 255
     * Parameters
     * ENDMSG  = 0xF7
     * Parameters for this function:
     * Operator_value = 1 byte
     * alg_index = 1 byte
     * attack = 1 byte
     * freq_factor = 5 bytes
     * amplitude = 5 bytes
     * 13 bytes total for parameters
     * 4 bytes for the message
     * 17 bytes total
     */
    //ok fuck it just hardcode the size xd
    unsigned char msg[17]; // Need to dynamically create the message fucks

    msg[0] = 0xF0; // Start of syssex
    msg[1] = 0x70; // intersynth identifier
    msg[2] = 0x15; // Size fuck im having an strok
    msg[4] = 0x10 + operator;//  function id??
    //msg[4] = operator; // Param 1
    msg[5] = ((unsigned char) attack<<7) + alg_index;
    //Sultu slakur
    fragment_floating(frequency_factor, &msg[6]); // Param 4
    fragment_floating(amplitude, &msg[11]); // Param 5
    msg[16] = 0xF7; // END
    rtmidi_out_send_message(midiout, msg, 17); // Send the actual serial message
    // Return the ok status of the last function done on midiout
    return midiout->ok;
}

bool intersynth_add_modulator(int operator_id, int modulator_id)
{
    //0x40 + operator_id
    //0x10 + modulator_id
    //i have no idea what im doing
    unsigned char msg[6];
    msg[0] = 0xF0; // Start of syssex
    msg[1] = 0x70; // intersynth identifier
    msg[2] = 0x02;
    msg[3] = 0x40 + operator_id;
    msg[4] = 0x10 + modulator_id;
    msg[5] = 0xF7;
    rtmidi_out_send_message(midiout, msg, 6); // Send the actual serial message
    return midiout->ok;

}


