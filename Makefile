monitor-daemon :
	gcc src/monitor-daemon.c -o bin/monitor-daemon

clean :
	rm bin/monitor-daemon
