import subprocess
from subprocess import Popen, PIPE
from time import sleep
def run_test(wifi,ipaddr):
    if(wifi):
        command = ["/home/star/RU/intersynth/intersynth_bluetooth/wifi_latency", ipaddr];
    else:
        command = ["/home/star/RU/intersynth/intersynth_bluetooth/cmake-build-debug/intersynth_bluetooth_runner"];
    process = Popen(command, close_fds=True);

    (out,err) = process.communicate();
    process_status = process.wait(None); #Make wait inf
    return process_status

def main():
    wifi = int(input("Enter 1 for wifi, 0 for bluetooth: "))
    ipaddr = ""
    if(wifi):
        ipaddr = input("Enter IP address of laptop: ")
    success = 0
    error = 0
    counter = 0
    for distance in range(0, 25, 5):
        for test_time_sec in [10, 20]:
            success = 0
            while(success < 10):
                #for test_iteration in range(11):
                print(f"Starting test #{counter} s:{success} e:{error} dist:{distance} test_time:{test_time_sec}")
                stat = run_test(wifi,ipaddr)
                print(f"Test finished #{counter} - Exit Code: {stat}");
                if((stat == -13) or (stat == 0)): # -13 for the wifi testing for some reason.
                    success += 1
                else:
                    error += 1
                counter += 1
                time.sleep(2)
                print("SLEEPING")

        input(f"ENTER WHEN LAPTOP HAS MOVED TO DISTANCE: {distance}")
    print(f"Total Errors: {error}\nTotal runs:{counter}")
if __name__ == "__main__":
    main();
