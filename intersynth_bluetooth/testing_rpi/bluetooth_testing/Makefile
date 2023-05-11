all:
	gcc -o btserver btserver_float.c -lbluetooth messages.h messages.c monitor.h monitor.c

btlatency:
	gcc -o btserver_latency btserver_latency.c -lbluetooth -lm monitor.h monitor.c

clean:
	rm btserver
	rm btserver_latency