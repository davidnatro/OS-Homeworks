#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

static int ids = 0;

typedef struct {
  int id;
  int bent;
} Bulavka;

void err(const char *error_message) {
  perror(error_message);
  exit(1);
}

int main(int argc, char **argv) {
  srand(time(NULL));
  if (argc != 3) {
    printf("Invalid arguments number\n");
    exit(1);
  }

  int sock;
  struct sockaddr_in server_address;
  Bulavka b;

  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    err("Socket creation error");
  }

  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(argv[1]);
  server_address.sin_port = htons(atoi(argv[2]));

  while (1) {
    b.id = ++ids;
    b.bent = rand() % 2;
    if (b.bent == 1) {
      printf("Bulavka is bent\n");
      continue;
    } else {
      printf("Bulavka is sent to broker\n");
      if (sendto(sock, &b, sizeof(b), 0, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        err("Sendto failed");
      }
    }
    sleep(3);
  }

  close(sock);
  return 0;
}
