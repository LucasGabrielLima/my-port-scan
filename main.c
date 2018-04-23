#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#define OFFSET 4

void parse_port_range(char *ports, int *port_range){
	char *token;
	int i = 0;

	token = strtok(ports, "-");
	while(token){
		port_range[i++] = atoi(token);
		token = strtok(NULL, "-");
	}
}

//Returns start and end IP on a array
int *parse_ip_range(char* start){
	char *token;
	char *bytes_start[4], *bytes_end[4];
	int *range = malloc(8 * sizeof(int));
	int i = 0, isrange = 0;


	//Separates IP's bytes by points
	token = strtok(start, ".");
	while(token){
		bytes_start[i++] = token;
		token = strtok(NULL, ".");
	}

	//Check for dashes on bytes
	for(i = 0; i < 4; i++){
		token = strtok(bytes_start[i], "-");
		bytes_start[i] = token;
		token = strtok(NULL, "-");
		if(token){
			bytes_end[i] = token;
		}
		else{
			bytes_end[i] = bytes_start[i];
		}
	}

	for(i = 0; i < 4; i++){
		range[i] = atoi(bytes_start[i]);
		if(range[i] > 255){
			printf("IP address or range not valid. Exiting...\n");
			exit(1);
		}
	}

	for(i = 0; i < 4; i++){
		range[i + OFFSET] = atoi(bytes_end[i]);
		if(range[i + OFFSET] > 255){
			printf("IP address or range not valid. Exiting...\n");
			exit(1);
		}

		if(range[i + OFFSET] != range[i]){
			isrange = 1;
		}
	}

	if(isrange){
		printf("Scanning range %d.%d.%d.%d to %d.%d.%d.%d \n", range[0], range[1], range[2], range[3], range[4], range[5], range[6], range[7]);
	}
	return range;
}

void init_target(struct sockaddr_in *target, int port, char *ip){
	target->sin_family = AF_INET;
	target->sin_port = htons(port);
	target->sin_addr.s_addr = inet_addr(ip);
}

int asyncconnected(int fd)
{
   struct sockaddr_in junk;
   socklen_t length = sizeof(junk);
   memset(&junk, 0, sizeof(junk));
   return (getpeername(fd, (struct sockaddr *)&junk, &length) == 0);
}

int main(int argc, char *argv[]) {
	struct sockaddr_in target;
  char *ip_input; char *port_input; char host[16]; char cmd[64]; char buffer[4096];
	int *range;
	int port_range[] = {1, 65535};
	//iterators for IP and port range
	int i, j, k, l, p;
	int sock, conn, hostdown, closed_ports, n;

  if(argc < 2){
  	printf("Error parsing params. Correct usage: porscan <IP address (or range)> [<Port range (xxx-yyy)>]");
  	exit(1);
  }

	ip_input = argv[1];

	//If a port range was passed
	if(argc == 3){
		port_input = argv[2];
		parse_port_range(port_input, port_range);
	}

  range = parse_ip_range(ip_input);

	//Iterates over IP and port ranges
	for(i = range[0]; i <= range[0 + OFFSET]; i++){
		for(j = range[1]; j <= range[1 + OFFSET]; j++){
			for(k = range[2]; k <= range[2 + OFFSET]; k++){
				for(l = range[3]; l <= range[3 + OFFSET]; l++){
					sprintf(host, "%d.%d.%d.%d", i, j, k, l);
					printf("Scanning host %s\n", host );

					//check if host is up
					snprintf(cmd, sizeof(cmd), "ping %s -c 2 -W 1 > /dev/null", host);
					if((hostdown = system(cmd))){
						printf("Host %s unreacheable. Check if it responds to ping commands.\n", host);
					}
					closed_ports = 0;

					for(p = port_range[0]; p <= port_range[1]; p++){
						if(hostdown){
							break;
						}
						sock = socket(AF_INET, SOCK_STREAM, 0);
						if(sock < 0){
							printf("Error creating socket. Exiting.\n");
							exit(1);
						}

						//Set socket to be non-blocking
						if (fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK) == -1){
								printf("Error setting non-blocking socket. Exiting.\n");
								exit(1);
						}

						init_target(&target, p, host);
						conn = connect(sock, (struct sockaddr *)&target, sizeof(target));

						//Timeout in 250 miliseconds
						usleep(250 * 1000);
						if(asyncconnected(sock)){
							printf("Port open: %d\n", p);
							sleep(10);
							n = read(sock, buffer, 4096);
 							if(n < 0){
  							printf("[Error]: Could not read port banner.\n\n");
 							}
							else{
								printf("Banner: %s\n\n", buffer);
							}
							close(conn);
						}
						else{
							closed_ports++;
							close(conn);
						}

						close(sock);
					}

					if(closed_ports){
						printf("Ports closed or filtered: %d\n", closed_ports);
					}
				}
			}
		}
	}

	free(range);
  return 0;
}
