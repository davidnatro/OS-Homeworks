#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#define SHM_SIZE 1024  // Размер разделяемой памяти

typedef struct {
  int number;
  int isProduced;
} SharedData;

int main() {
  key_t key = ftok("shared_memory", 1234);  // Генерация ключа для разделяемой памяти

  int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);  // Создание разделяемой памяти
  if (shmid == -1) {
    perror("Не удалось создать разделяемую память");
    return 1;
  }

  SharedData *sharedData = (SharedData *) shmat(shmid, NULL, 0);  // Присоединение разделяемой памяти
  if (sharedData == (void *) -1) {
    perror("Не удалось присоединить разделяемую память");
    return 1;
  }

  srand(time(NULL));  // Инициализация генератора случайных чисел

  while (1) {
    if (!sharedData->isProduced) {
      sharedData->number = rand() % 100;  // Генерация случайного числа
      sharedData->isProduced = 1;  // Пометка числа как сгенерированного
      printf("Сгенерировано число: %d\n", sharedData->number);
    }

    sleep(1);
  }

  shmdt(sharedData);  // Отсоединение разделяемой памяти

  return 0;
}
