#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Использование: %s файл_чтения файл_записи\n", argv[0]);
    return 1;
  }

  // Открываем файл для чтения
  FILE *file_in = fopen(argv[1], "r");
  if (file_in == NULL) {
    printf("Не удалось открыть файл для чтения: %s\n", argv[1]);
    return 1;
  }

  // Открываем файл для записи
  FILE *file_out = fopen(argv[2], "w");
  if (file_out == NULL) {
    printf("Не удалось открыть файл для записи: %s\n", argv[2]);
    fclose(file_in);
    return 1;
  }

  // Буфер для чтения и записи
  char buffer[BUFFER_SIZE];

  // Читаем и записываем содержимое файла
  size_t bytesRead;
  while ((bytesRead = fread(buffer, 1, sizeof(buffer), file_in)) > 0) {
    fwrite(buffer, 1, bytesRead, file_out);
  }

  // Закрываем файлы
  fclose(file_in);
  fclose(file_out);

  printf("Файл успешно скопирован.\n");

  return 0;
}
