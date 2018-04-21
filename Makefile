all: portscan

portscan: main.c
	gcc -Wall main.c -o portscan
#
# lib.o: lib.c
# 	gcc -Wall -c lib.c

clean:
	rm -f *.o portscan
	rm -r doc
