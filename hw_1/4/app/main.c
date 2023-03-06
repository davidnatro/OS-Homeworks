#include <stdio.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("%s", "Неверное количество аргументов!");
        return 0;
    }

    for (int i = 1; i < argc; ++i) {
        printf("%s\n", argv[i]);
    }
    return 0;
}
