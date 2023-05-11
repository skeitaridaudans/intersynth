import subprocess
from subprocess import Popen, PIPE

def run_test(distance, client_os, test_time, wifi):
    if(wifi == 1):
        command = ["./wifi_latency", str(distance), str(client_os), str(test_time)];
    else:
        command = ["./btserver_latency", str(distance), str(client_os), str(test_time)];

    process = Popen(command);

    (out,err) = process.communicate();
    process_status = process.wait(None); #Make wait inf
    return process_status


def main():
    client_os = input("Client os : ");
    wifi = int(input("Wifi: 1 / Bluetooth: 0 - : "))
    for distance in range(0, 25, 5):
        for test_time in [10, 20]:
            for test_iteration in range(10):
                print(f"Starting Test #{test_iteration} - Distance : {distance}");
                p_stat = run_test(distance, client_os, test_time, wifi);
                print(f"Finished Test #{test_iteration} - Distance : {distance} - Exit code : {p_stat}");
            

if __name__ == '__main__':
    main()
