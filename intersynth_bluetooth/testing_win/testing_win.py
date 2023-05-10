import subprocess
from subprocess import Popen, PIPE
from time import sleep
def run_test(wifi,ipaddr):
    if(wifi == 0):
        command = r"C:\Users\pi\Desktop\win_testing\intersynth_bluetooth_runner.exe"
    else:
        command = [r"C:\Users\pi\Desktop\win_testing\intersynth_wifi_latency_testing.exe", ipaddr]
    p = Popen(command)
    (out, err) = p.communicate()
    p_code = p.wait()
    return p.returncode

def main():
    error = 0
    success = 0
    ipaddr = ""
    wifi = int(input("1 for wifi, 0 for bluetooth : "))
    if wifi:
        ipaddr = input("Ip address of server: ")
    for distance in range(0,25,5):
        for test_time_sec in [10, 20]:
            test_iteration = 0
            success = 0
            while success < 10:
                test_iteration += 1
                print(f"Starting test #{test_iteration} s:{success} e:{error} dist:{distance} test_time:{test_time_sec}")
                ret_code = run_test(wifi, ipaddr)
                print(f"Stopping Test {test_iteration} - Exit code: {ret_code}")
                if ret_code == 0:
                    success += 1
                else:
                    error += 1
                sleep(2)

        input(f"Press enter to continue - new dist {distance + 5}")

if __name__ == "__main__":
    main()