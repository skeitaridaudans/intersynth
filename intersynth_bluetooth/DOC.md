### Functions
Simple document that shows the functions of the library and how to use them.

### Errors & how to check them
The library uses a windows approach to errors. Like with WSAgetlasterror.
You need to get the error structure and check the error code.
```
intersynth_error_t* error = intersynth_get_error();
error->error_code
error->error_string
```
You can also use
```
intersynth_error_code intersynth_get_error_code(void);
const char* intersynth_get_error_string(intersynth_error_code error_code);
```
Where intersynth_error_code is an enum over the respective error codes. and intersynth_get_error_string returns the string representation of the error code. We recommend using the first approach since that updates after each function call. 
### Bluetooth specific functions

#### intersynth_init
###### Description
Initializes Bluetooth. This function must be called before any other bluetooth function.
###### Parameters
None
###### Return value
None
_________________
#### intersynth_die
###### Description
Closes the respective socket and frees the bluetooth socket.
###### Parameters
None
###### Return value
None
_________________
#### intersynth_scan
###### Description
Scans the bluetooth devices in range. This function is blocking and will take a while to finish.
###### Parameters
None
###### Return value
Technically void, but you will need to call 2 other functions to get the results in a usable structure.
_________________
#### intersynth_scan_free
###### Description
Free the results of the scan. This function must be called after you are done with the results.
###### Parameters
None
###### Return value
None
_________________
#### intersynth_scan_devices_found
###### Description
The number of devices found
###### Parameters
None
###### Return value
Integer with the number of devices found
_________________
#### intersynth_scan_get_results
###### Description
Function returns a struct with the results of the scan. The struct is defined as:
```
struct intersynth_bluetooth_device_inquiry {
    platform_dependant_address_t btaddr;
    char* name; 
};
```
Please take note that ```name``` is a pointer to an allocated string. Here's a code example how to print out the results:
```
int total_devices = intersynth_scan_devices_found();
//iterating over the found devices
for (int i = 0; i < total_devices; i++) {
    printf("Device %d: Name: %s, Address: %02llX:%02llX:%02llX:%02llX:%02llX:%02llX\n", i, ii[i].name,
           (ii[i].btaddr >> 40) & 0xFF, (ii[i].btaddr >> 32) & 0xFF,
           (ii[i].btaddr >> 24) & 0xFF, (ii[i].btaddr >> 16) & 0xFF,
           (ii[i].btaddr >> 8) & 0xFF, (ii[i].btaddr & 0xFF));
}
```
Notice that we need to use bit shifting to get the address in the correct format to the C-style string.
###### Parameters
None
###### Return value
Structure with the results of the scan function.
_________________
#### intersynth_select_device
###### Description
Attempts to connect to that respective device. This function is blocking and on some platforms can fail, please check the error code.
###### Parameters
Integer with the index of the device you want to connect to. The index is the same as the index in the array of structs returned by intersynth_scan_get_results.
###### Return value
None
_________________

### How to get the current state (NOT IMPLEMENTED YET)

### Messages
_________________
#### intersynth_send_note_on
###### Description
Sends a note to a connected device, if there is no device connected, the function will raise an error
###### Parameters
```unsigned char``` key - the key you want to send
```unsigned char``` velocity - the velocity of the key
###### Return value
None
_________________
#### intersynth_send_note_off
###### Description
Turns off a note on a connected device, if there is no device connected, the function will raise an error
###### Parameters
```unsigned char``` key - the key you want to send
###### Return value
None
_________________
#### intersynth_add_modulator
###### Description
Adds modulation to an operator
###### Parameters
```int``` operator_id 
```int``` modulator_id
###### Return value
None
_________________
#### intersynth_remove_modulator
###### Description
Removes modulation from an operator
###### Parameters
```int``` operator_id
```int``` modulator_id
###### Return value
None
_________________
#### intersynth_add_carrier
###### Description
Adds a carrier to an operator
###### Parameters
```int``` operator_id
###### Return value
None
_________________
#### intersynth_remove_operator
###### Description
Removes carrier from an operator
###### Parameters
```int``` operator_id
###### Return value
None
_________________
#### intersynth_change_operator_values
###### Description
Changes the values of an operator
###### Parameters
```unsigned char``` operator_id
```unsigned char``` alg_index
```bool``` attack
```float``` frequency_factor
```float``` amplitude
###### Return value
None
_________________
#### intersynth_clear_operators
###### Description
Clears all modulators and carriers from an operator and resets the values to default
###### Parameters
None
###### Return value
None
_________________
```
//ALGORITHM_ENVELOPE_INFO
void intersynth_alg_envelope_info_insert(bool attack, int alg_index, float time); //ekki intersynth_midi
void intersynth_alg_envelope_info_remove(bool attack, int alg_index);
void intersynth_alg_envelope_info_envelope_size(bool attack, int new_size);
void intersynth_alg_envelope_info_update_time(bool attack, int alg_index, float new_time);
void intersynth_alg_envelope_info_solo_value(bool attack, int alg_index);

//AMP FIXED
void intersynth_amp_fixed_ignore_envelope(float amp_level);
void intersynth_amp_fixed_ignore_velocity(bool ignore);


//FREQ SYNCED BY
void intersynth_freq_synced_by(int operator_id, int sync_operator_num, bool on_off);


//NOTE INFO
void intersynth_note_info_note_on(int key, int velocity, float freq);
void intersynth_note_info_note_off(int key);
void intersynth_note_info_note_update(int key, int velocity, float freq);

//NOTE MIDICHANNEL
void intersynth_note_midi_channel(int midichannel);

//EXPRESSIVE CHANGE
void intersynth_expressive_change(int op_num,float freq_factor, float amp_factor);
```