//
// Created by star on 28.2.2023.
//

#include "messages.h"

#include <stdio.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>

extern int errno;

int main()
{
    struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
    char buf[1024] = { 0 };
    const char *msg = "Hello, client!";
    int s, client;
    long bytes_read;
    socklen_t opt = sizeof(rem_addr);

    // allocate socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // bind socket to port 1 of the first available
    // local bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = (uint8_t) 1;
    if(bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr)) < 0) {
        perror("bind failed\n");
        fprintf(stderr, "errno: %s\n", strerror(errno));
        return 1;
    }
    // Listen for connections and accept them
    if(listen(s, 1) < 0) {
        perror("listen failed\n");
        fprintf(stderr, "errno: %s\n", strerror(errno));
        return 1;
    }

    // Accept connection
    client = accept(s, (struct sockaddr *)&rem_addr, &opt);
    // Check if accept worked
    if (client < 0) {
        perror("accept failed\n");
        fprintf(stderr, "errno: %s\n", strerror(errno));
        return 1;
    }
    // Read data from client into buffer and print it
    memset(buf, 0, sizeof(buf));
    ba2str( &rem_addr.rc_bdaddr, buf );
    printf("accepted connection from %s\n", buf);
    // Keep connection alive until client closes it

    //Master loop, essentially a state machine and a message handler.
    // TODO: Message handler, dynamic memory allocation and then send to the synth (Ask Kári)
    while (true) {
        // Read data from client into buffer and print it
        memset(buf, 0, sizeof(buf));
        bytes_read = read(client, buf, sizeof(buf));
        if (bytes_read > 0) {
            printf("bytes read: %ld", bytes_read);
            float value = reconstruct_float_from_buffer(buf);
            printf("Received float from client: %f\n", value);
        }// Check if client closed connection
        else if (bytes_read == 0) {
            printf("client closed connection");
            break;
        } else {
            perror("read failed");
            fprintf(stderr, "errno: %s\n", strerror(errno));
            return 1;
        }
    }
    return 0;
}