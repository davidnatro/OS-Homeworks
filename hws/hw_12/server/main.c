#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8888
#define MAX_BUFFER_SIZE 1024

int main() {
  int sockfd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len;
  char buffer[MAX_BUFFER_SIZE];
  const int broadcast = 1;

  // Создание сокета
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("Ошибка при создании сокета");
    exit(1);
  }

  // Настройка сокета для широковещательной рассылки
  if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
    perror("Ошибка при настройке сокета для широковещательной рассылки");
    exit(1);
  }

  // Настройка адреса сервера
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(PORT);

  // Привязка адреса сервера к сокету
  if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("Ошибка при привязке адреса сервера");
    exit(1);
  }

  printf("Сервер запущен и ожидает сообщений...\n");

  while (1) {
    // Прием сообщения от клиента
    memset(buffer, 0, sizeof(buffer));
    client_len = sizeof(client_addr);
    if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &client_len) < 0) {
      perror("Ошибка при приеме сообщения от клиента");
      exit(1);
    }

    // Вывод полученного сообщения
    printf("Получено сообщение от клиента: %s", buffer);
  }

  // Закрытие сокета
  close(sockfd);

  return 0;
}
