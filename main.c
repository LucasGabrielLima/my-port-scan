#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

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
	int i = 0;


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
	}

	for(i = 0; i < 4; i++){
		range[i + OFFSET] = atoi(bytes_end[i]);
	}

	// sprintf(range, "%s.%s.%s.%s %s.%s.%s.%s", bytes_start[0], bytes_start[1], bytes_start[2], bytes_start[3], bytes_end[0], bytes_end[1], bytes_end[2], bytes_end[3]);
	return range;
}

void init_target(struct sockaddr_in *target, int port, char *ip){
	target->sin_family = AF_INET;
	target->sin_port = htons(port);
	target->sin_addr.s_addr = inet_addr(ip);
}

int main(int argc, char *argv[]) {
	struct sockaddr_in target;
  char *ip_input; char *port_input; char host[16];
	int *range;
	int port_range[] = {1, 65535};
	int i, j, k, l, p, sock, conn;

  if(argc < 2){
  	printf("Error parsing params. Correct usage: porscan <IP address (or range)> [<Port range (xxx-yyy)>]");
  	exit(1);
  }

	ip_input = argv[1];

	if(argc == 3){
		port_input = argv[2];
		parse_port_range(port_input, port_range);
	}


  printf("%s\n", ip_input);

  range = parse_ip_range(ip_input);

	for(i = 0; i < 8; i++){
		printf("%d \n", range[i]);
	}


	//Iterates over IP and port ranges
	for(i = range[0]; i <= range[0 + OFFSET]; i++){
		for(j = range[1]; j <= range[1 + OFFSET]; j++){
			for(k = range[2]; k <= range[2 + OFFSET]; k++){
				for(l = range[3]; l <= range[3 + OFFSET]; l++){
					sprintf(host, "%d.%d.%d.%d", i, j, k, l);
					printf("Scanning host %s\n", host );

					for(p = port_range[0]; p <= port_range[1]; p++){
						sock = socket(AF_INET, SOCK_STREAM, 0);
						if(sock < 0){
							printf("Erro na criação do socket\n");
							exit(1);
						}

						init_target(&target, p, host);
						conn = connect(sock, (struct sockaddr *)&target, sizeof(target));
						if(conn == 0){
							printf("Port open: %d\n", p);
							close(conn);
						}
						else{
							printf("Port closed: %d\n", p);
							close(conn);
						}

						close(sock);
					}
				}
			}
		}
	}

	free(range);
  return 0;
}
