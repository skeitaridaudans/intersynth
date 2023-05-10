#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib,"ws2_32.lib") // Link to winsock library

#define BUF_SIZE 1024
#define TEST_TIME_SEC 20 // test duration in seconds

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./wifi_latency <server_ip>\n");
        exit(1);
    }
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }
    SOCKET s;
    struct sockaddr_in server;
    // create socket
    printf("CREATING SOCKET\n");
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
        printf("Socket creation failed with error code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // set server address parameters
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    inet_pton(AF_INET, argv[1], &server.sin_addr);

    // connect to server
    printf("CONNECTING TO SERVER\n");
    if (connect(s, (struct sockaddr *) &server, sizeof(server)) == SOCKET_ERROR) {
        printf("Connect failed with error code: %d\n", WSAGetLastError());
        closesocket(s);
        WSACleanup();
        return 1;
    }
    printf("CONNECTED\n");
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
    // close connections
    closesocket(s);
    WSACleanup();
    return 0;
}