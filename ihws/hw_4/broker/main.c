#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

typedef struct {
  int id;
  int bent;
} Bulavka;

void err(const char *error_message) {
  perror(error_message);
  exit(1);
}

int main(int argc, char **argv) {
  if (argc != 5) {
    printf("Invalid program arguments length.\n");
    exit(1);
  }

  srand(time(NULL));
  char *address = argv[1];
  int port = atoi(argv[2]);
  char *forwar_address = argv[3];
  int forward_port = atoi(argv[4]);

  int server_socket;
  struct sockaddr_in server_address;
  Bulavka b;
  socklen_t addr_len = sizeof(server_address);
  char buffer[sizeof(Bulavka)];

  if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    err("Socket creation failed");
  }

  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(address);
  server_address.sin_port = htons(port);

  if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
    err("Bind failed");
  }

  printf("Listening on port %d\n", port);

  // Now forward the received Bulavka to Program #3
  int forward_socket;
  struct sockaddr_in forward_address;

  if ((forward_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    err("Forward socket creation failed");
  }

  memset(&forward_address, 0, sizeof(forward_address));
  forward_address.sin_family = AF_INET;
  forward_address.sin_addr.s_addr = inet_addr(forwar_address);
  forward_address.sin_port = htons(forward_port);

  while (1) {
    ssize_t recv_len = recvfrom(server_socket, buffer, sizeof(Bulavka), 0, (struct sockaddr *)&server_address, &addr_len);
    if (recv_len == -1) {
      err("Receive failed");
    }

    memcpy(&b, buffer, sizeof(Bulavka));
    printf("Received bulavka id: %d, bent: %d\n", b.id, b.bent);

    if (rand() % 2) {
      printf("Bulavka's quality is bad\n");
      continue;
    }

    printf("Sending bulavka #%d to packager\n", b.id);
    sendto(forward_socket, &b, sizeof(Bulavka), 0, (struct sockaddr *)&forward_address, sizeof(forward_address));

    sleep(3);
  }

  close(server_socket);
  close(forward_socket);
  return 0;
}
