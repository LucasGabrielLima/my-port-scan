all: portscan

portscan: lib.o main.o argparse.o
	gcc -Wall lib.o main.o -o portscan

main.o: main.c
	gcc -Wall -c main.c
# 
# lib.o: lib.c
# 	gcc -Wall -c lib.c

clean:
	rm -f *.o portscan
	rm -r doc
