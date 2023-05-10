#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#define BUF_SIZE 1024
#define TEST_TIME_SEC 20 // test duration in seconds

// https://forums.raspberrypi.com/viewtopic.php?t=178485

int main(int argc, char *argv[]) {
    if(argc != 2)
    {
        printf("Usage: ./wifi_latency <server_ip>\n");
        exit(1);
    }
    int s;
    struct sockaddr_in server;
    // allocate socket
    printf("CREATING SOCKET\n");
    s = socket(AF_INET, SOCK_STREAM, 0);
    int yes = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    if(inet_pton(AF_INET, argv[1], &server.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // set server address parameters
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    //server.sin_addr.s_addr = inet_addr(argv[1]);
    // connect to server
    printf("CONNECTING TO SERVER\n");
    if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect");
        printf("errno: %d\n", errno);
        printf("strerror: %s\n", strerror(errno));
        exit(1);
    }
    printf("CONNECTED\n");
    // Begin latency loop
    char sbuf[1024] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam velit ligula, tristique sed ante a, ultricies semper quam amet.";
    char rbuf[1024] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam velit ligula, tristique sed ante a, ultricies semper quam amet.";
    while(1)
    {
        int status = send(s, sbuf, 1024, 0);
        if(status < 0)
        {
            break;
        }
        int r_status = recv(s, rbuf, 1024, 0);
        if (r_status < 0)
        {
            break;
        }
        if(strcmp(rbuf, "STOP") == 0)
        {
            break;
        }
    }
    // close connection
    close(s);

    return 0;
}
