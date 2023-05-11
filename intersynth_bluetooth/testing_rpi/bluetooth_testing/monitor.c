#include "monitor.h"

//ONLY ONE CONNECTION CAN BE MADE WITH THE SERVER CURRENTLY
int8_t get_rssi_from_connection(bdaddr_t* bdaddr)
{
    //bdaddr is the client's mac address
    struct hci_conn_info_req *cr; 
    int8_t rssi;
    int dd;

    dd = hci_open_dev(NULL);
    if(dd < 0)
    {
        perror("HCI device open failed");
        exit(1);
    }
    
    cr = malloc(sizeof(struct hci_conn_info_req) + sizeof(struct hci_conn_info));
    if(!cr)
    {
        perror("Cannot allocate memory for cr struct");
        exit(1);
    }
    bacpy(&cr->bdaddr, bdaddr); 
    cr->type = ACL_LINK;

    if(ioctl(dd, HCIGETCONNINFO, (unsigned long) cr) < 0)
    {
        perror("Getting connection info failed");
        exit(1);
    }
        // print connection info
	if (hci_read_rssi(dd, htobs(cr->conn_info->handle), &rssi, 1000) < 0) {
		perror("Read RSSI failed");
		exit(1);
	}
    free(cr);
    hci_close_dev(dd);
    return rssi;
}