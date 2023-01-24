
#include "daudasynthinn_music.h"
#include "notes.h"
#include "patch.h"

#include <iostream>
#include <stdio.h>
#include <string.h>

void midiCallback( double deltatime, std::vector< unsigned char > *message, void */*userData*/ )
{
    if (message->size() >= 3){
        if ((int)message->at(0) == 0x90){
            NoteCollections::get_instance().MIDI_change_note((int)message->at(1), (int)message->at(2));
            if((int)message->at(2) == 0){
                // printf("NOTE OFF (note on 0)\n");
            }
            // else{
            //     printf("NOTE ON\n");
            // }
        }
        else if ((int)message->at(0) == 0x80){
            NoteCollections::get_instance().MIDI_change_note((int)message->at(1), 0);
            // printf("NOTE OFF (specific)\n");
        }
        //printf("Midi:  %d, %d, %d\n", (int)message->at(0), (int)message->at(1), (int)message->at(2));
    }
}

// #define MAX_SAMPLE_CHANGE 0.01
// double last_value = 0.0;

Patch &patch = Patch::get_instance();

double get_next_value_for_carrier(std::vector<OperatorDriver> &algorithm_drivers, int index){
    if(patch.operators_visited_by_sound[index] == 2){
        return 0.0;
    }
    patch.operators_visited_by_sound[index] += 1;
    double previous_sample = 0.0;
    for(int i = 0; i < patch.modulation_order[index].size(); i++){
        previous_sample += get_next_value_for_carrier(algorithm_drivers, patch.modulation_order[index][i]);
    }
    patch.operators_visited_by_sound[index] -= 1;
    return algorithm_drivers[index].get_next_output(previous_sample);
}


/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int soundOutputCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    short *out = (short*)outputBuffer;
    (void) inputBuffer; /* Prevent unused variable warning. */

    NoteCollections::get_instance().notes_mutex.lock();

    for(int act_note = 0; act_note < NoteCollections::get_instance().active_notes.size(); act_note++){
        std::vector<OperatorDriver> &algorithm_drivers = NoteCollections::get_instance().active_notes[act_note].get_operator_drivers();
        for(int driv = 0; driv < algorithm_drivers.size(); driv++){
            algorithm_drivers[driv].build_increments(NoteCollections::get_instance().active_notes[act_note].base_frequency);
        }
    }

    // double max_sample = 0.0;

    for(unsigned int i = 0; i < framesPerBuffer; i++){
        double sample = 0.0;
        double temp_sample;

        for(int act_note = 0; act_note < NoteCollections::get_instance().active_notes.size(); act_note++){
            std::vector<OperatorDriver> &algorithm_drivers = NoteCollections::get_instance().active_notes[act_note].get_operator_drivers();
            OperatorDriver &amp_driver = NoteCollections::get_instance().active_notes[act_note].get_amp_driver();
            for(int i = 0; i < algorithm_drivers.size(); i++){
                algorithm_drivers[i].increment_once();
            }
            amp_driver.increment_once();

            temp_sample = 0.0;
            for(int i = 0; i < patch.carriers.size(); i++){
                temp_sample += get_next_value_for_carrier(algorithm_drivers, patch.carriers[i]);
            }
            sample += (temp_sample * amp_driver.current_value.amplitude);
        }
        // if(sample > max_sample){
        //     max_sample = sample;
        // }
        short output = sample >= 16.0 ? 32000 : sample <= -16.0 ? -32000 : (short)(sample * 2000.0); // ADD BETTER COMPRESSION
        *out++ = output;
        *out++ = output; // STERIO (not really) for ASIO driver
        
        // // if(sample > max_sample){
        // //     max_sample = sample;
        // // }
        // sample = sample * 2000.0;
        // // if(sample < 0){
        // //     sample = 30000.0 * sample / (30000.0 - sample);
        // // }
        // // else{
        // //     sample = 30000.0 * sample / (30000.0 + sample);
        // // }
        // *out++ = (short)sample;
    }

    // if(max_sample > 8.0){
    //     printf("max sample: %f\n", max_sample);
    // }

    NoteCollections::get_instance().notes_mutex.unlock();

    return 0;
}

PaStream *stream;

bool init_portaudio(){

    PaStreamParameters params;
    PaError err;

    printf("PortAudio Test: output FM synthesis.\n");
    /* Initialize library before making any other calls. */
    err = Pa_Initialize();
    if( err != paNoError ){
        Pa_Terminate();
        fprintf( stderr, "An error occurred in portaudio: Pa_Initialize()\n" );
        return false;
    }

// int     structVersion
// const char *    name
// PaHostApiIndex  hostApi
// int     maxInputChannels
// int     maxOutputChannels
// PaTime  defaultLowInputLatency
// PaTime  defaultLowOutputLatency
// PaTime  defaultHighInputLatency
// PaTime  defaultHighOutputLatency
// double  defaultSampleRate

    int numDevices = Pa_GetDeviceCount();

    int *ASIO_devices = new int[numDevices];
    int numASIOdevices = 0;
    const PaDeviceInfo *deviceInfo;
    for(int i = 0; i < numDevices; i++)
    {
        deviceInfo = Pa_GetDeviceInfo(i);
        int name_len = strlen(deviceInfo->name);
        for(int l = 0; l < (name_len - 4); l++){
            if(deviceInfo->name[l] == 'A' && deviceInfo->name[l+1] == 'S' && deviceInfo->name[l+2] == 'I' && deviceInfo->name[l+3] == 'O'){
                ASIO_devices[numASIOdevices] = i;
                ++numASIOdevices;
                break;
            }
        }
    }
    std::cout << "Following are all ASIO devices and the default device (bottom)" << std::endl;
    for(int i = 0; i < numASIOdevices; i++){
        deviceInfo = Pa_GetDeviceInfo(ASIO_devices[i]);
        std::cout << i + 1 << ": " << deviceInfo->name << std::endl;
        std::cout << "maxOutputChannels: " << deviceInfo->maxOutputChannels << std::endl;
        std::cout << "defaultLowOutputLatency: " << deviceInfo->defaultLowOutputLatency << std::endl;
        std::cout << "defaultSampleRate: " << deviceInfo->defaultSampleRate << std::endl;
        
        std::cout << std::endl;
    }

    deviceInfo = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice());
    std::cout << numASIOdevices + 1 << ": " << deviceInfo->name << std::endl;
    std::cout << "maxOutputChannels: " << deviceInfo->maxOutputChannels << std::endl;
    std::cout << "defaultLowOutputLatency: " << deviceInfo->defaultLowOutputLatency << std::endl;
    std::cout << "defaultSampleRate: " << deviceInfo->defaultSampleRate << std::endl;
    
    std::cout << std::endl;

    std::cout << "Hit ENTER to use the top device," << std::endl;
    std::cout << "otherwise enter the list number" << std::endl;
    std::cout << "(enter 0 to get full list of devices): ";
    char input;
    input = std::cin.get();
    std::cout << std::endl;
    if(input == '\n'){
        if(numASIOdevices == 0){
            params.device = Pa_GetDefaultOutputDevice();
        }
        else{
            params.device = ASIO_devices[0];
        }
    }
    else if(input > '0'){
        if(input - '0' <= numASIOdevices){
            params.device = ASIO_devices[input - '1'];
        }
        else{
            params.device = Pa_GetDefaultOutputDevice();
        }
    }
    else{
        for(int i = 0; i < numDevices; i++)
        {
            deviceInfo = Pa_GetDeviceInfo(i);
            std::cout << i + 1 << ": " << deviceInfo->name << std::endl;
            std::cout << "maxOutputChannels: " << deviceInfo->maxOutputChannels << std::endl;
            std::cout << "defaultLowOutputLatency: " << deviceInfo->defaultLowOutputLatency << std::endl;
            std::cout << "defaultSampleRate: " << deviceInfo->defaultSampleRate << std::endl;
            
            std::cout << std::endl;
        }

        std::cout << "Please enter the list number: ";
        do{
            input = std::cin.get();
        }while(input == '\n');
        params.device = input - '1';
    }

    std::cout << "This is the chosen device:" << std::endl;
    deviceInfo = Pa_GetDeviceInfo(params.device);
    std::cout << deviceInfo->name << std::endl;
    std::cout << "maxInputChannels: " << deviceInfo->maxInputChannels << std::endl;
    std::cout << "maxOutputChannels: " << deviceInfo->maxOutputChannels << std::endl;
    std::cout << "defaultLowInputLatency: " << deviceInfo->defaultLowInputLatency << std::endl;
    std::cout << "defaultLowOutputLatency: " << deviceInfo->defaultLowOutputLatency << std::endl;
    std::cout << "defaultHighInputLatency: " << deviceInfo->defaultHighInputLatency << std::endl;
    std::cout << "defaultHighOutputLatency: " << deviceInfo->defaultHighOutputLatency << std::endl;
    std::cout << "defaultSampleRate: " << deviceInfo->defaultSampleRate << std::endl;
    
    std::cout << std::endl;

    params.channelCount = 2;
    params.sampleFormat = paInt16;
    params.suggestedLatency = deviceInfo->defaultLowOutputLatency; // 0.016;  // 0.025  // 0.045  // 0.09
    params.hostApiSpecificStreamInfo = NULL;

    delete[] ASIO_devices;

    /* Open an audio I/O stream. */
    err = Pa_OpenStream( &stream,
                         NULL,          /* no input channels */
                                &params,          /* mono 16 bit integer output */
                                SAMPLE_RATE,
                                512,        /* frames per buffer */
                                paNoFlag,
                                soundOutputCallback,
                                NULL );
    if( err != paNoError ){
        Pa_Terminate();
        fprintf( stderr, "An error occurred in portaudio: Pa_OpenStream()\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
        return false;
    }

    // /* Open an audio I/O stream. */
    // err = Pa_OpenDefaultStream( &stream,
    //                             0,          /* no input channels */
    //                             2,          /* stereo output */
    //                             paFloat32,  /* 32 bit floating point output */
    //                             SAMPLE_RATE,
    //                             64,        /* frames per buffer */
    //                             soundOutputCallback,
    //                             &data );
    // if( err != paNoError ) goto error;

    err = Pa_StartStream( stream );
    if( err != paNoError ){
        Pa_Terminate();
        fprintf( stderr, "An error occurred in portaudio: Pa_StartStream()\n" );
        return false;
    }

    /* Sleep for several seconds. */
    //Pa_Sleep(NUM_SECONDS*1000);

    //TUI_change_values();

    return true;

// error:
//     Pa_Terminate();
//     fprintf( stderr, "An error occurred while using the portaudio stream\n" );
//     fprintf( stderr, "Error number: %d\n", err );
//     fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
//     return false;
}

bool uninit_portaudio(){
    
    PaError err;

    err = Pa_StopStream( stream );
    if( err != paNoError ){
        Pa_Terminate();
        fprintf( stderr, "An error occurred in portaudio: Pa_StopStream()\n" );
        return false;
    }
    err = Pa_CloseStream( stream );
    if( err != paNoError ){
        Pa_Terminate();
        fprintf( stderr, "An error occurred in portaudio: Pa_CloseStream()\n" );
        return false;
    }
    Pa_Terminate();
    return true;
// error:
//     Pa_Terminate();
//     fprintf( stderr, "An error occurred while using the portaudio stream\n" );
//     fprintf( stderr, "Error number: %d\n", err );
//     fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
//     return false;
}



