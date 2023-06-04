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

  int client_socket;
  struct sockaddr_in server_address;
  char buffer[BUFFER_SIZE];

  // Создание сокета
  client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket == -1) {
    error("Error: could not create socket");
  }

  // Настройка адреса сервера
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(argv[1]);
  server_address.sin_port = htons(atoi(argv[2]));

  // Установка соединения с сервером
  if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
    error("Error: could not connect to server");
  }

  while (1) {
    // Ввод сообщения с клавиатуры
    printf("Введите сообщение (или 'The End' для завершения): ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';  // Удаление символа новой строки

    // Отправка сообщения серверу
    if (send(client_socket, buffer, strlen(buffer), 0) < 0) {
      error("Error: could not send message to server");
    }

    if (strcmp(buffer, "The End") == 0) {
      break;
    }

    // Получение ответа от сервера
    memset(buffer, 0, BUFFER_SIZE);
    ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0) {
      error("Error: could not receive message from server");
    }

    printf("Ответ от сервера: %s\n", buffer);
  }

  // Завершение работы клиента
  close(client_socket);

  return 0;
}
