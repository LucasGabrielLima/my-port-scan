#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char const *argv[]) {
  //in_addr struct has a single member s_addr
  struct in_addr addr;
  char ip_input[16];


  fgets(ip_input, sizeof(ip_input), stdin);
  inet_aton(ip_input, &(addr));

  printf("Hello %s\n", inet_ntoa(addr));
  return 0;
}
