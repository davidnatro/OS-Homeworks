#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int receivedNumber = 0;
int bitPosition = 31;

void signalHandler(int sig) {
  if (sig == SIGUSR1) {
    // Получен сигнал SIGUSR1, бит равен 0
    receivedNumber &= ~(1 << bitPosition);
  } else if (sig == SIGUSR2) {
    // Получен сигнал SIGUSR2, бит равен 1
    receivedNumber |= (1 << bitPosition);
  }

  bitPosition--;

  if (bitPosition < 0) {
    // Все биты приняты, печатаем число и завершаемся
    printf("Принято число: %d\n", receivedNumber);
    exit(0);
  }
}

int main() {
  printf("PID приемника: %d\n", getpid());

  signal(SIGUSR1, signalHandler);
  signal(SIGUSR2, signalHandler);

  while (1) {
    // Бесконечный цикл, ожидаем сигналов
  }

  return 0;
}
