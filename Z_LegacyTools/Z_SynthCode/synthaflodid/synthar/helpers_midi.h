
#ifndef HELPERS_MIDI_H_8736482734
#define HELPERS_MIDI_H_8736482734

/*
    This class implements some common functionality for the realtime
    MIDI input/output subclasses RtMidiIn and RtMidiOut.

    RtMidi GitHub site: https://github.com/thestk/rtmidi
    RtMidi WWW site: http://www.music.mcgill.ca/~gary/rtmidi/

    RtMidi: realtime MIDI i/o C++ classes
    Copyright (c) 2003-2019 Gary P. Scavone

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation files
    (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    Any person wishing to distribute modifications to the Software is
    asked to send the modifications to the original developer so that
    they can be incorporated into the canonical version.  This is,
    however, not a binding provision of this license.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#include <RtMidi.h>


#include <iostream>
#include <cstdlib>

#include <thread>
#include <mutex>

void usage( void );

// This function should be embedded in a try/catch block in case of
// an exception.  It offers the user a choice of MIDI ports to open.
// It returns false if there are no ports available.
bool chooseMidiPort(RtMidi *rtmidi);

void autoConnectMiniIn(std::vector<RtMidiIn *> &midi_ins, void (*callback)( double, std::vector< unsigned char > *, void *));
void auto_connect_loop_thread(std::vector<RtMidiIn *> &midi_ins, void (*callback)( double, std::vector< unsigned char > *, void *));

bool init_midi_out();
void uninit_midi_out();
void send_midi_message(std::vector<unsigned char> &message);
void send_midi_note(int note, int velocity = 90);

bool init_midi_in(void (*callback)( double, std::vector< unsigned char > *, void *));
void uninit_midi_in();





// void midiCallback( double deltatime, std::vector< unsigned char > *message, void */*userData*/ );


#endif //HELPERS_MIDI_H_8736482734