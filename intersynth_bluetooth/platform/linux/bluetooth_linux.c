

#include "bluetooth_linux.h"

struct intersynth_bluetooth_device_inquiry *intersynth_ii;
int total_devices = 0;

void bluetooth_init(void) {
    bluetooth_handler.addr.rc_family = AF_BLUETOOTH;
    bluetooth_handler.addr.rc_channel = (uint8_t) 1;
    int sockfd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (sockfd < 0) {
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
    bluetooth_handler.socket = sockfd;
    intersynth_set_success_error();
}

void bluetooth_deinit(void) {
    int status = close(bluetooth_handler.socket);
    if (status < 0) {
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
    intersynth_set_success_error();
}

static void bluetooth_connect(char addr[19])
{
    //Connect to a device via bluetooth_handler.socket using bluetooth_handler.addr
    str2ba(addr, &bluetooth_handler.addr.rc_bdaddr);
    //bluetooth_handler.addr.rc_bdaddr = btaddr;
    if(connect(bluetooth_handler.socket, (struct sockaddr *)&bluetooth_handler.addr, sizeof(bluetooth_handler.addr)) < 0) {
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
    intersynth_set_success_error();
}

void bluetooth_connect_fixed()
{
    //str2ba(btaddr, &bluetooth_handler.addr.rc_bdaddr);

    bluetooth_handler.addr.rc_bdaddr = (bdaddr_t) {{0x5E, 0x65, 0x53, 0x22, 0x11, 0x58}};

    if(connect(bluetooth_handler.socket, (struct sockaddr *)&bluetooth_handler.addr, sizeof(bluetooth_handler.addr)) < 0) {
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
    intersynth_set_success_error();
}

void bluetooth_disconnect(void)
{
    //Disconnect from a device via bluetooth_handler.socket
    int status = close(bluetooth_handler.socket);
    if(status < 0)
    {
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
    intersynth_set_success_error();
}

//BLUETOOTH SEND
void bluetooth_send(char *data, int length)
{
    //Send data via bluetooth_handler.socket
    int status = send(bluetooth_handler.socket, data, length, 0); //Note on flags, Could be useful to use MSG_CONFIRM since it doesint really matter.
    if(status < 0)
    {
        intersynth_set_error(INTERSYNTH_ERROR_BLUETOOTH);
    }
    intersynth_set_success_error();
}

void bluetooth_latency()
{
    char buf[1024] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam velit ligula, tristique sed ante a, ultricies semper quam amet.";
    char rbuf[1024] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam velit ligula, tristique sed ante a, ultricies semper quam amet.";
    while(1)
    {
        int status = send(bluetooth_handler.socket, buf, 1024, 0);
        if(status < 0)
        {
            break;
        }
        int r_status = recv(bluetooth_handler.socket, rbuf, 1024, 0);
        if (r_status < 0)
        {
            break;
        }
        if(strcmp(rbuf, "STOP") == 0)
        {
            break;
        }
    }
}

void bluetooth_scan(void)
{
    if(intersynth_ii != NULL)
    {
        intersynth_set_error(INTERSYNTH_ERROR_MEMORY_NOT_CLEARED);
        return;
    }
    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    char addr[19] = { 0 };
    char name[248] = { 0 };
    printf("Scanning for devices...(this may take a while)\n");
    dev_id = hci_get_route(NULL); // Get first available bluetooth device by passing NULL
    sock = hci_open_dev(dev_id);
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        exit(1);
    }
    len  = 16; // 8 * 1.28 = 10.24 seconds
    max_rsp = 255; // 255 devices
    flags = IREQ_CACHE_FLUSH; // flush cache
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags); //actual search
    if (num_rsp < 0) {
        perror("hci_inquiry");
    }
    printf("Found %d devices:\n", num_rsp);
    total_devices = num_rsp;
    //print all devices

    intersynth_ii = malloc(sizeof(struct intersynth_bluetooth_device_inquiry) * num_rsp);
    for (i = 0; i < num_rsp; i++) {
        //get address of device
        ba2str(&(ii+i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
        //get name of device
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), name, 0) < 0) {
            strcpy(name, "[unknown]"); // if name is not found
        }
        //intersynth_ii[i].bdaddr = addr;
        memcpy(intersynth_ii[i].bdaddr, &addr, sizeof(addr));
        memcpy(intersynth_ii[i].name, name, sizeof(intersynth_ii[i].name));
        printf("Device %d: [bdaddr]:%s  [hostname]:%s\n", i , addr, name);
    }
    free(ii);
    close(sock);
}

int bluetooth_scan_devices_found(void)
{
    return total_devices;
}

struct intersynth_bluetooth_device_inquiry* bluetooth_scan_get_results(void)
{
    return intersynth_ii;
}
void bluetooth_scan_free(void)
{
    free(intersynth_ii);
    intersynth_ii = NULL;
    total_devices = 0;
}
void bluetooth_select_device(int device_index)
{
    //Select a device from bluetooth_handler.ii
    bluetooth_connect(intersynth_ii[device_index].bdaddr);
}
