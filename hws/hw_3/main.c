#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int fibonacci(int n) {
  if (n <= 0) {
    return 0;
  } else if (n == 1) {
    return 1;
  } else {
    return fibonacci(n-1) + fibonacci(n-2);
  }
}

int factorial(int n) {
  if (n <= 1) {
    return 1;
  } else {
    return n * factorial(n-1);
  }
}

void parent_process(int n) {
  int fib_result = fibonacci(n);
  printf("Родительский процесс: Число Фибоначчи для %d равно %d\n", n, fib_result);
}

void child_process(int n) {
  int fact_result = factorial(n);
  printf("Процесс-ребенок: Факториал %d равен %d\n", n, fact_result);
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("Использование: ./program <аргумент>\n");
    return 1;
  }

  int arg = atoi(argv[1]);

  pid_t pid = fork();

  if (pid < 0) {
    fprintf(stderr, "Ошибка при создании процесса\n");
    return 1;
  } else if (pid == 0) {
    // Процесс-ребенок
    child_process(arg);
  } else {
    // Родительский процесс
    wait(NULL);
    parent_process(arg);
  }

  return 0;
}
