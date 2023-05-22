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
    if (argc != 5) {
        printf("invalid program arguments length.\n");
        exit(1);
    }

    srand(time(NULL));
    char *address = argv[1];
    int port = atoi(argv[2]);
    char *forwar_address = argv[3];
    int forwar_port = atoi(argv[4]);

    int server_socket, new_socket, forward_socket;
    struct sockaddr_in server_address, forward_address;
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

    // Now forward the received Bulavka to Program #3
    if ((forward_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("forward socket failed");
        exit(EXIT_FAILURE);
    }

    forward_address.sin_family = AF_INET;
    forward_address.sin_port = htons(forwar_port);

    if (inet_pton(AF_INET, forwar_address, &forward_address.sin_addr) <= 0) {
        perror("forward address error");
        exit(EXIT_FAILURE);
    }

    if (connect(forward_socket, (struct sockaddr *) &forward_address, sizeof(forward_address)) < 0) {
        perror("forward connection failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        recv(new_socket, &b, sizeof(b), 0);

        printf("received bulavka #%d\n", b.id);
        if (rand() % 2) {
            printf("bulavka's quality is bad\n");
            continue;
        }

        printf("sending bulavka #%d to packager\n", b.id);
        send(forward_socket, &b, sizeof(b), 0);
        sleep(3);
    }

    close(new_socket);
    close(server_socket);
    close(forward_socket);
    return 0;
}
