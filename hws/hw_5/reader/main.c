#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main() {
  const char *fifoFile = "/tmp/myfifo";  // Имя именованного канала

  // Открытие канала для чтения
  int fd = open(fifoFile, O_RDONLY);
  if (fd == -1) {
    perror("Не удалось открыть канал для чтения");
    return 1;
  }

  char buffer[BUFFER_SIZE];

  // Чтение данных из канала и вывод на экран
  while (read(fd, buffer, BUFFER_SIZE) > 0) {
    printf("Прочитано из канала: %s", buffer);
  }

  // Закрытие канала
  close(fd);

  return 0;
}
