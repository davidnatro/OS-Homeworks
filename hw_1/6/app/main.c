#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 5000

void stringReverse(const int start, const int end, const ssize_t size, char *data);

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("%s", "Неверное количество аргументов!");
        exit(-1);
    }

    char *input_file = argv[3];
    char *output_file = argv[4];
    int start_pos = atoi(argv[1]);
    int end_pos = atoi(argv[2]);

    // Создаем неименованной канал
    int fd[2];
    if (pipe(fd) == -1) {
        perror("Не удается создать канал!");
        exit(-1);
    }

    // Создаем второй процесс
    pid_t pid = fork();
    if (pid == -1) {
        perror("Не удается создать процесс!");
        exit(-1);
    }

    if (pid == 0) {
        char buffer[BUFFER_SIZE];
        ssize_t total_read = read(fd[0], buffer, BUFFER_SIZE);

        // Закрываем конец чтения из неименованного канала
        close(fd[0]);

        stringReverse(start_pos, end_pos, total_read, buffer);

        ssize_t total_written = write(fd[1], buffer, total_read);

        close(fd[1]);
    } else {
        int input_fd = open(input_file, O_RDONLY);
        if (input_fd == -1) {
            perror("Не удается открыть файл для чтения!");
            exit(-1);
        }

        int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (output_fd == -1) {
            perror("Не удается открыть файл для записи!");
            exit(-1);
        }

        char buffer[BUFFER_SIZE];
        ssize_t total_read = 0;
        ssize_t total_written = 0;

        while ((total_read = read(input_fd, buffer, BUFFER_SIZE)) > 0) {
            total_written = write(fd[1], buffer, total_read);
        }

        // Закрываем конец записи в неименованный канал
        close(fd[1]);

        while ((total_read = read(fd[0], buffer, BUFFER_SIZE)) > 0) {
            total_written = write(output_fd, buffer, total_read);
        }

        close(fd[0]);
        close(input_fd);
        close(output_fd);
    }

    while (wait(NULL) > 0) {}

    return 0;
}

void stringReverse(const int start, const int end, const ssize_t size, char *data) {
    int n = end - start;
    for (int i = 0; i < n / 2; i++) {
        char temp = data[n - i - 1 + start];
        data[n - i - 1 + start] = data[i + start];
        data[i + start] = temp;
    }
}