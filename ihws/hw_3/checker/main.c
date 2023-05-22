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

int main(int argc, char **argv) {
    srand(time(NULL));
    if (argc != 3) {
        printf("invalid arguments number\n");
        exit(1);
    }
    int sock = 0;
    struct sockaddr_in serv_addr;
    Bulavka b;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    while (1) {
        b.id = ++ids;
        b.bent = rand() % 2;
        if (b.bent == 1) {
            printf("Bulavka is bent\n");
            continue;
        } else {
            printf("Bulavka is sent to broker\n");
            send(sock, &b, sizeof(b), 0);
        }
        sleep(3);
    }

    close(sock);
    return 0;
}
