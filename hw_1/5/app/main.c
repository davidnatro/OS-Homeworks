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

    // Открываем именованный канал для передачи данных между процессами
    char *channel_name = "mychannel";
    mkfifo(channel_name, 0666);

    // Создаем процессы для чтения, обработки и записи данных
    pid_t pid1, pid2;
    pid1 = fork();
    if (pid1 < 0) {
        printf("Не удается создать процесс");
        return -1;
    }
    if (pid1 != 0) {
        // Процесс 1: чтение данных из входного файла и передача их через именованный канал
        int input_fd = open(argv[3], O_RDONLY, 0666);
        if (input_fd < 0) {
            printf("%s", "Не удается открыть файл для чтения!");
            exit(-1);
        }

        int channel_fd = open(channel_name, O_WRONLY);
        if (channel_fd < 0) {
            printf("Не удается открыть канал");
            exit(-1);
        }

        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;

        while ((bytes_read = read(input_fd, buffer, BUFFER_SIZE)) > 0) {
            write(channel_fd, buffer, bytes_read);
        }

        if (close(input_fd) < 0) {
            printf("%s", "Не удается закрыть файл!");
            exit(-1);
        }
        if (close(channel_fd)) {
            printf("Не удается закрыть канал");
            exit(-1);
        }
    } else {
        pid2 = fork();
        if (pid2 == 0) {
            // Процесс 2: обработка данных и передача результата через именованный канал
            int channel_fd = open(channel_name, O_RDONLY);
            if (channel_fd < 0) {
                printf("Не удается открыть канал");
                exit(-1);
            }

            char buffer[BUFFER_SIZE];
            ssize_t bytes_read = read(channel_fd, buffer, BUFFER_SIZE);

            int start_pos = atoi(argv[1]);
            int end_pos = atoi(argv[2]);

            stringReverse(start_pos, end_pos, bytes_read, buffer);

            write(channel_fd, buffer, bytes_read);

            int close_res = close(channel_fd);
            if (close_res == -1) {
                printf("Не удается закрыть канал");
                exit(-1);
            }

            pid_t pid3 = fork();
            if (pid3 == 0) {
                int output_fd = open(argv[4], O_WRONLY | O_CREAT, S_IWOTH | S_IWUSR | 0644);
                if (output_fd < 0) {
                    printf("%s", "Не удается открыть файл для записи!");
                    exit(-1);
                }

                write(output_fd, buffer, bytes_read);
                if (close(output_fd) == -1) {
                    printf("%s", "Не удается закрыть файл для записи!");
                    exit(-1);
                }
            }
        }
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