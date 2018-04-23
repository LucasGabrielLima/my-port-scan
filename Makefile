all: portscan

portscan: main.c
	gcc -Wall main.c -o portscan

clean:
	rm -f *.o portscan
