#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

#define MESSAGE_SIZE 20
#define MESSAGE_COUNT 10

int main() {
  int pipefd[2];
  pid_t pid;
  sem_t *sem;

  char parentMessage[MESSAGE_SIZE] = "дочерний процесс";
  char childMessage[MESSAGE_SIZE] = "родительский процесс";

  // Создаем неименованный канал
  if (pipe(pipefd) == -1) {
    perror("Не удалось создать канал");
    return 1;
  }

  // Создаем семафор
  sem = sem_open("my_semaphore", O_CREAT, 0666, 1);
  if (sem == SEM_FAILED) {
    perror("Не удалось создать семафор");
    return 1;
  }

  pid = fork();

  if (pid < 0) {
    perror("Не удалось создать дочерний процесс");
    return 1;
  }

  if (pid > 0) {
    // Родительский процесс

    // Закрываем ненужный конец канала
    close(pipefd[0]);

    // Отправляем сообщения дочернему процессу
    for (int i = 0; i < MESSAGE_COUNT; i++) {
      sem_wait(sem);  // Блокируем доступ к каналу

      write(pipefd[1], parentMessage, strlen(parentMessage) + 1);
      printf("Родитель отправил сообщение: %s\n", parentMessage);

      sem_post(sem);  // Разблокируем доступ к каналу
      sleep(1);
    }

    close(pipefd[1]);
    wait(NULL);

    // Удаляем семафор
    sem_unlink("my_semaphore");
    sem_close(sem);
  } else {
    // Дочерний процесс

    // Закрываем ненужный конец канала
    close(pipefd[1]);

    // Принимаем сообщения от родительского процесса
    for (int i = 0; i < MESSAGE_COUNT; i++) {
      sem_wait(sem);  // Блокируем доступ к каналу

      read(pipefd[0], childMessage, sizeof(childMessage));
      printf("Дочерний получил сообщение: %s\n", childMessage);

      sem_post(sem);  // Разблокируем доступ к каналу
      sleep(1);
    }

    close(pipefd[0]);

    // Выходим из дочернего процесса
    exit(0);
  }

  return 0;
}
