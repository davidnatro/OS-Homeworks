#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8888
#define MAX_BUFFER_SIZE 1024

int main() {
  int sockfd;
  struct sockaddr_in server_addr;
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
  server_addr.sin_port = htons(PORT);

  // Преобразование IP-адреса в формат, подходящий для сокета
  if (inet_pton(AF_INET, "255.255.255.255", &(server_addr.sin_addr)) <= 0) {
    perror("Ошибка при преобразовании IP-адреса");
    exit(1);
  }

  while (1) {
    printf("Введите сообщение для отправки серверу:\n");
    fgets(buffer, sizeof(buffer), stdin);

    // Отправка сообщения на сервер
    if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
      perror("Ошибка при отправке сообщения на сервер");
      exit(1);
    }
    printf("Сообщение отправлено серверу.\n");
  }


  // Закрытие сокета
  close(sockfd);

  return 0;
}
