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

int main(int argc, char **argv) {
    srand(time(NULL));
    if (argc != 3) {
        printf("invalid arguments size.\n");
        exit(1);
    }

    int port = atoi(argv[2]);
    char *address = argv[1];

    int server_socket, new_socket;
    struct sockaddr_in server_address;
    int addr_len = sizeof(server_address);
    Bulavka b;

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(address);
    server_address.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d\n", port);

    if ((new_socket = accept(server_socket, (struct sockaddr *) &server_address, (socklen_t *) &addr_len)) < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    while (1) {

        recv(new_socket, &b, sizeof(b), 0);
        printf("Received bulavka id: %d, bent: %d\n", b.id, b.bent);
        printf("received bulavka #%d", b.id);
        if (rand() % 2) {
            printf("bulavka's quality is bad. throwing away.\n");
            continue ;
        } else {
            printf("packaging bulavka.\n");
        }
        sleep(3);
    }

    close(new_socket);
    close(server_socket);
    return 0;
}
