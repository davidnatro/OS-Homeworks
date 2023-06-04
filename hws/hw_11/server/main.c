#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void error(const char *msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <ip_address> <port>\n", argv[0]);
    exit(1);
  }

  int server_socket, client_socket;
  struct sockaddr_in server_address, client_address;
  socklen_t client_address_length;
  char buffer[BUFFER_SIZE];

  // Создание сокета
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    error("Error: could not create socket");
  }

  // Настройка адреса сервера
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(argv[1]);
  server_address.sin_port = htons(atoi(argv[2]));

  // Привязка сокета к адресу сервера
  if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
    error("Error: could not bind socket");
  }

  // Ожидание подключения клиента
  listen(server_socket, 1);
  printf("Сервер запущен на %s:%s\n", argv[1], argv[2]);

  client_address_length = sizeof(client_address);
  client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_length);
  if (client_socket < 0) {
    error("Error: could not accept connection");
  }

  printf("Подключение от клиента %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

  while (1) {
    // Получение сообщения от клиента №1
    memset(buffer, 0, BUFFER_SIZE);
    ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0) {
      error("Error: could not receive message from client");
    }

    printf("Сообщение от клиента №1: %s\n", buffer);

    // Пересылка сообщения клиенту №2
    if (send(client_socket, buffer, strlen(buffer), 0) < 0) {
      error("Error: could not send message to client");
    }

    if (strcmp(buffer, "The End") == 0) {
      break;
    }
  }

  // Завершение работы сервера
  close(client_socket);
  close(server_socket);

  return 0;
}
