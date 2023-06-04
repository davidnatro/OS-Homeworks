#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 1024  // Размер разделяемой памяти

typedef struct {
  int number;
  int isProduced;
} SharedData;

int main() {
  key_t key = ftok("shared_memory", 1234);  // Генерация ключа для разделяемой памяти

  int shmid = shmget(key, SHM_SIZE, 0666);  // Получение разделяемой памяти
  if (shmid == -1) {
    perror("Не удалось получить разделяемую память");
    return 1;
  }

  SharedData *sharedData = (SharedData *) shmat(shmid, NULL, 0);  // Присоединение разделяемой памяти
  if (sharedData == (void *) -1) {
    perror("Не удалось присоединить разделяемую память");
    return 1;
  }

  while (1) {
    if (sharedData->isProduced) {
      printf("Получено число: %d\n", sharedData->number);
      sharedData->isProduced = 0;  // Пометка числа как обработанного
    }

    sleep(1);
  }

  shmdt(sharedData);  // Отсоединение разделяемой памяти
  shmctl(shmid, IPC_RMID, NULL);  // Удаление разделяемой памяти

  return 0;
}
