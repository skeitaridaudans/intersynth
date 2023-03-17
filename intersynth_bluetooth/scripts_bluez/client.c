#include <stdio.h>
#include <stdlib.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include "messages.h"

int main() {
    // create Bluetooth socket
    int s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // connect to Linux server
    struct sockaddr_rc serv_addr = { 0 };
    serv_addr.rc_family = AF_BLUETOOTH;
    //serv_addr.rc_bdaddr = (bdaddr_t) {{0xE4, 0x5F, 0x01, 0xC4, 0x9B, 0xFC}}; // replace with Linux server's Bluetooth address
    serv_addr.rc_bdaddr = (bdaddr_t) {{0xFC, 0x9B, 0xC4, 0x01, 0x5F, 0xE4}}; // replace with Linux server's Bluetooth address
    serv_addr.rc_channel = (uint8_t) 4; // port number on Linux server
    //Check if connect worked
    printf("Connecting to Linux server...\n");
    if (connect(s, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect failed\n");
        fprintf(stderr, "errno: %s\n", strerror(errno));
        fprintf(stderr, "errno: %d\n", errno);
        return 1;
    }
    //TODO:
    // Send float to server via bluetooth and respecting the MIDI standard


    float value = -3.14159265358979323846;
    char buffer[5];
    memset(buffer, 0, 5);
    store_float_in_buffer(buffer, value);
    printf("Buffer: %02x %02x %02x %02x %02x\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
    printf("Sending float to Linux server: %f\n", value);
    send(s, buffer, 5, 0);

    //const char *msg = "Hello, Linux server!";
    //send(s, msg, strlen(msg), 0);
    //printf("Sent message to Linux server: %s\n", msg);
    // receive data from Linux server
    //char buf[1024] = { 0 };
    //recv(s, buf, sizeof(buf), 0);
    //printf("Received message from Linux server: %s\n", buf);

    // close Bluetooth socket
    printf("Closing Bluetooth socket...\n");
    close(s);
    // check close error


    return 0;
}
