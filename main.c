#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define OFFSET 4

//Returns start and end IP separated by a space character
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

	for(i = 0; i < 8; i++){
		printf("%d \n", range[i]);
	}

	// sprintf(range, "%s.%s.%s.%s %s.%s.%s.%s", bytes_start[0], bytes_start[1], bytes_start[2], bytes_start[3], bytes_end[0], bytes_end[1], bytes_end[2], bytes_end[3]);
	return range;

}

int main(int argc, char *argv[]) {
  //in_addr struct has a single member s_addr
  struct in_addr addr;
  char *ip_input; char *port_input; char *ip_end;
	int *range; int *port_range
	int i, j, k, l, m;

  if(argc < 3){
  	printf("Error parsing params. Correct usage: porscan <IP address (or range)> <Port (range)>");
  	exit(1);
  }

  ip_input = argv[1];
  port_input = argv[2];


  printf("%s\n", ip_input);

  range = parse_ip_range(ip_input);

	//Iterates over IP and port ranges
	for(i = range[0]; i <= range[0 + OFFSET]; i++){
		for(j = range[1]; j <= range[1 + OFFSET]; j++){
			for(k = range[2]; k <= range[2 + OFFSET]; k++){
				for(l = range[3]; l <= range[3 + OFFSET]; l++){
					for(m = port_range[0]; <= port_range[1]; m++){

					}
				}
			}
		}
	}

	free(range);
  return 0;
}
