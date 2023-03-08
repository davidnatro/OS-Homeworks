#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void closeFd(int fd[]);

void hasCorrectNumberOfArguments(int argc);

void stringReverse(int start, int end, int size, char *data);

int main(int argc, char **argv) {
    hasCorrectNumberOfArguments(argc);

    const int str_max_size = 10000;
    char *input_data = malloc(str_max_size);
    char *buffer = malloc(str_max_size);

    int fd_file;
    if ((fd_file = open(argv[3], O_RDONLY)) < 0) {
        printf("%s", "Не удается открыть файл!");
        exit(-1);
    }
    read(fd_file, input_data, 0);
    FILE *input_file = fopen(argv[3], "r");
    fseek(input_file, 0, SEEK_END);
    int input_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    input_data = malloc(input_size);
    buffer = malloc(input_size);

    fread(input_data, 1, input_size, input_file);
    fclose(input_file);


    int fd[2], pipe_result, fork_result;

    pipe_result = pipe(fd);
    if (pipe_result < 0) {
        printf("%s", "Не удается создать канал!");
        exit(-1);
    }

    fork_result = fork();

    if (fork_result < 0) {
        printf("%s", "Не удается создать процесс!");
        exit(-1);
    } else if (fork_result == 0) {
        ssize_t read_size = read(fd[0], buffer, input_size);
        if (read_size != input_size) {
            printf("%s", "Не удается завершить прочитать из канала!");
            exit(-1);
        }
        stringReverse(atoi(argv[1]), atoi(argv[2]), input_size, buffer);

    } else {
        ssize_t write_size = write(fd[1], input_data, input_size);
        if (write_size != input_size) {
            printf("%s", "Не удается завершить запись в канал!");
            exit(-1);
        }
    }

    closeFd(fd);
    free(input_data);
    free(buffer);

    return 0;
}

void closeFd(int fd[]) {
    if (close(fd[0]) < 0) {
        printf("%s", "Не удается закрыть канал!");
        exit(-1);
    }

    if (close(fd[1]) < 0) {
        printf("%s", "Не удается закрыть канал!");
        exit(-1);
    }
}

void hasCorrectNumberOfArguments(const int argc) {
    if (argc != 5) {
        printf("%s", "Неверное количество аргументов!");
        exit(-1);
    }
}

void stringReverse(const int start, const int end, const int size, char *data) {
    int n = end - start;
    for (int i = 0; i < n / 2; i++) {
        char temp = data[n - i - 1 + start];
        data[n - i - 1 + start] = data[i + start];
        data[i + start] = temp;
    }
}