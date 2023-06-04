#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void sendBit(int pid, int bit) {
  if (bit == 0) {
    kill(pid, SIGUSR1);  // Отправляем сигнал SIGUSR1
  } else {
    kill(pid, SIGUSR2);  // Отправляем сигнал SIGUSR2
  }
}

void sendMessage(int pid, int number) {
  int i;
  for (i = 31; i >= 0; i--) {
    int bit = (number >> i) & 1;  // Получаем i-й бит числа

    sendBit(pid, bit);  // Отправляем бит
    usleep(100);  // Пауза между передачей битов
  }
}

void signalHandler(int sig) {
  // Обработчик сигналов
}

int main() {
  int pid;
  int number;

  printf("PID передатчика: %d\n", getpid());
  printf("Введите PID приемника: ");
  scanf("%d", &pid);

  printf("Введите целое число: ");
  scanf("%d", &number);

  signal(SIGUSR1, signalHandler);
  signal(SIGUSR2, signalHandler);

  sendMessage(pid, number);

  return 0;
}
3