#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <errno.h>
#include <math.h>

#define BUF_SIZE 1024
//#define TEST_TIME_SEC 20 // test duration in seconds

// https://forums.raspberrypi.com/viewtopic.php?t=178485

int main(int argc, char *argv[]) {
    int distance = 0, test_time_sec = 0;
    char clientos = 0;
    if(argc == 4)
    {
        // Arguments supplied
        // Argument order: Distance, Client OS, Test time
        distance = atoi(argv[1]);
        clientos = *argv[2];
        test_time_sec = atoi(argv[3]);
    }
    else if(argc > 4)
    {
        printf("Too many arguments supplied.\nExiting, goodbye.\n");
        exit(1);
    }
    else
    {
        printf("Expecting 3 arguments. Distance, Client OS, Test time\nExiting, goodbye.\n");
        exit(1);
    }

    int client, bytes;
    char buf[BUF_SIZE] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad";
    double total_time, throughput;
    int total_bytes = 0;
    
    struct sockaddr_in loc_addr = { 0 }, rem_addr = { 0 };
    int s;
    long bytes_read;
    socklen_t opt = sizeof(rem_addr);

    // allocate socket
    printf("CREATING SOCKET\n");
    s = socket(AF_INET, SOCK_STREAM, 0);
    int yes = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    loc_addr.sin_family = AF_INET;
    loc_addr.sin_addr.s_addr = INADDR_ANY;
    loc_addr.sin_port = htons(8080); 
    printf("BINDING\n");
    if(bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr)) < 0) {
        perror("bind failed\n");
        fprintf(stderr, "errno: %s\n", strerror(errno));
        return 1;
    }
    // Listen for connections and accept them
    printf("LISTENING\n");
    if(listen(s, 1) < 0) {
        perror("listen failed\n");
        fprintf(stderr, "errno: %s\n", strerror(errno));
        return 1;
    }

    // Accept connection
    printf("ACCEPT\n");
    client = accept(s, (struct sockaddr *)&rem_addr, &opt);
    // Check if accept worked
    if (client < 0) {
        perror("accept failed\n");
        fprintf(stderr, "errno: %s\n", strerror(errno));
        return 1;
    }
    // Read data from client into buffer and print it
    char ipbuf[100] = {0};
    memset(ipbuf, 0, sizeof(ipbuf));
    inet_ntop(AF_INET, &(rem_addr.sin_addr), ipbuf, INET_ADDRSTRLEN);
    printf("Client connected\n");
    printf("Address: %s", ipbuf);
    // start timing
    time_t end_time = time(NULL) + test_time_sec;
    // read and write data for TEST_TIME_SEC seconds
    time_t old_rem;
    struct timeval start, stop, last_stop;
    unsigned long rtt = 0, jitter = 0, prev_rtt = 0, total_rtt = 0;
    int packets = 0, total_rssi = 0;
    int8_t rssi;
    while (time(NULL) < end_time) {
        time_t remaining_time = end_time - time(NULL);
        old_rem = remaining_time;
        gettimeofday(&start, NULL);
        //printf("Time remaining: %ld seconds\n", remaining_time);
        bytes = read(client, buf, BUF_SIZE);
        if (bytes < 0) {
            perror("Read failed");
            close(client);
            close(s);
            exit(1);
        }
        total_bytes += bytes;
        packets += 1;
        //04:ED:33:3D:48:79
        gettimeofday(&stop, NULL);
        rtt = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
        total_rtt += rtt;
        //rssi = get_rssi_from_connection(&rem_addr.rc_bdaddr);
        total_rssi += rssi;
        //printf("RTT: %lu us\n", rtt);
        if (packets > 1) {
            jitter += abs(rtt - prev_rtt);
        }
        prev_rtt = rtt;
        bytes = write(client, buf, BUF_SIZE);
        if (bytes < 0) {
            perror("Write failed");
            close(client);
            close(s);
            exit(1);
        }   

        //usleep(1000);
        last_stop = stop;
    }

    write(client, "STOP", strlen("STOP"));

    // calculate throughput in bytes per second
    throughput = total_bytes / test_time_sec;
    float avg_rtt = total_rtt/packets;
    float avg_rssi = total_rssi/packets;
    float avg_jitter = jitter / (packets - 1);
    //printf("Test complete: %d bytes transferred in %d seconds (%.2f kilobytes/sec)\n", total_bytes, test_time_sec, (throughput/1024));
    //printf("Average RTT: %f us\n", avg_rtt);
    //printf("Average jitter: %lu us\n", jitter / (packets - 1));
    //printf("Avarage RSSI: %d", avg_rssi);
    printf("Writing to file\n");
    FILE *file_pointer;

    file_pointer = fopen("DATA_WIFI.csv","a");
    if(file_pointer == NULL)
    {
        printf("Error opening file. \n");
        exit(1);
    }
    fprintf(file_pointer, "%d,%c,%d,%.0f,%.0f,%.0f,%.0f,%d\n",
            distance,clientos,test_time_sec, //d,c,d
            avg_rssi,avg_jitter,avg_rtt,throughput, //f,f,f,f
            packets);//d
    fclose(file_pointer);

    close(client);
    close(s);

    return 0;
}