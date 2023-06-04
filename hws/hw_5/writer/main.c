#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main() {
  const char *fifoFile = "/tmp/myfifo";  // Имя именованного канала

  // Создание именованного канала
  mkfifo(fifoFile, 0666);

  // Открытие канала для записи
  int fd = open(fifoFile, O_WRONLY);
  if (fd == -1) {
    perror("Не удалось открыть канал для записи");
    return 1;
  }

  char buffer[BUFFER_SIZE];
  printf("Введите данные для записи в канал (введите 'exit' для выхода):\n");

  // Чтение данных из стандартного ввода и запись их в канал
  while (1) {
    fgets(buffer, BUFFER_SIZE, stdin);

    // Проверка на выход
    if (strcmp(buffer, "exit\n") == 0)
      break;

    // Запись данных в канал
    write(fd, buffer, strlen(buffer) + 1);
  }

  // Закрытие канала
  close(fd);

  // Удаление именованного канала
  unlink(fifoFile);

  return 0;
}
