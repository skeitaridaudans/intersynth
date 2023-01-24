
#include "helpers_midi.h"

RtMidiOut *midiout = NULL;

bool init_midi_out()
{
    try {

        // RtMidiIn constructor
        midiout = new RtMidiOut();

        // Call function to select port.
        if ( chooseMidiPort( midiout ) == false ) return false;

    } catch ( RtMidiError &error ) {
        error.printMessage();
        return false;
    }

    return true;
}

void uninit_midi_out(){

    delete midiout;
    midiout = NULL;
}

void send_midi_message(std::vector<unsigned char> &message){
    midiout->sendMessage( &message );
}

void send_midi_note(int note, int velocity){
    std::vector<unsigned char> message;
    message.push_back( 144 );
    message.push_back( note );
    message.push_back( velocity );
    send_midi_message( message );
}

std::vector<RtMidiIn *> midiin_vector;

bool init_midi_in(void (*callback)( double, std::vector< unsigned char > *, void *))
{
    // try {

    printf("1\n");
        // RtMidiIn constructor
        //midiin = new RtMidiIn();

        // Call function to select port.
        // if ( chooseMidiPort( midiin ) == false ) return false;
        autoConnectMiniIn(midiin_vector, callback);

        // Set our callback function.  This should be done immediately after
        // opening the port to avoid having incoming messages written to the
        // queue instead of sent to the callback function.
        //midiin->setCallback( callback );

    printf("2\n");
        // Don't ignore sysex, timing, or active sensing messages.
        //midiin->ignoreTypes( false, false, false );

    // } catch ( RtMidiError &error ) {
    //     error.printMessage();
    //     return false;
    // }

    return true;
}

void uninit_midi_in(){

    // delete midiin;
    // midiin = NULL;

}

bool chooseMidiPort(RtMidi *rtmidi)
{
    std::string portName;
    unsigned int i = 0, nPorts = rtmidi->getPortCount();
    if ( nPorts == 0 ) {
        std::cout << "No ports available!" << std::endl;
        return false;
    }

    if ( nPorts == 1 ) {
        std::cout << "\nOpening " << rtmidi->getPortName() << std::endl;
    }
    else {
        for ( i=0; i<nPorts; i++ ) {
            portName = rtmidi->getPortName(i);
            std::cout << "  Port #" << i << ": " << portName << '\n';
        }

        do {
            std::cout << "\nChoose a port number: ";
            std::cin >> i;
        } while ( i >= nPorts );
        //std::getline( std::cin, keyHit );  // used to clear out stdin
    }

    std::cout << "\n";
    rtmidi->openPort( i );

    return true;
}

void autoConnectMiniIn(std::vector<RtMidiIn *> &midi_ins, void (*callback)( double, std::vector< unsigned char > *, void *)){
    std::thread *my_thread = new std::thread(auto_connect_loop_thread, std::ref(midi_ins), callback);
}

#include <chrono>

void auto_connect_loop_thread(std::vector<RtMidiIn *> &midi_ins, void (*callback)( double, std::vector< unsigned char > *, void *)){
    unsigned int nPorts = 0;
    midi_ins.push_back(new RtMidiIn());
    while(true){
        nPorts = midi_ins[midi_ins.size() - 1]->getPortCount();
        if(nPorts < midi_ins.size()){
            if(nPorts + 1 < midi_ins.size()){ // FEWER THAN BEFORE
                for(int i = 0; i < midi_ins.size(); i++){
                    std::cout << "  Disconnecting from MIDI IN on port #" << i << std::endl;
                    midi_ins[i]->closePort();
                    delete midi_ins[i];
                }
                midi_ins.clear();
                midi_ins.push_back(new RtMidiIn());
            }
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        else {
            while(nPorts >= midi_ins.size()) {
                try{
                    int next_port_index = midi_ins.size() - 1;
                    RtMidiIn *next_midiin = midi_ins[next_port_index];
                    std::string portName = next_midiin->getPortName(next_port_index);
                    std::cout << "  Connecting to MIDI IN on port #" << next_port_index << ": " << portName << std::endl;
                    next_midiin->openPort( next_port_index );
                    next_midiin->setCallback( callback );
                    next_midiin->ignoreTypes( false, false, false );
                    midi_ins.push_back(new RtMidiIn());
                } catch ( RtMidiError &error ) {
                    error.printMessage();
                    break;
                }

            }
        }
    }
}





void usage( void ) {
    // Error function in case of incorrect command-line
    // argument specifications.
    std::cout << "\nuseage: cmidiin/cmidiout <port>\n";
    std::cout << "    where port = the device to use (first / default = 0).\n\n";
    exit( 0 );
}
