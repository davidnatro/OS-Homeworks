#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
    int shm_fd;
    void *ptr;
    int *shared_mem;
    const int SIZE = 4096;
    const char *name = "shm";

    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

    ftruncate(shm_fd, SIZE);

    ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    shared_mem = (int *) ptr;
    for (int i = 0; i < 10; i++) {
        shared_mem[i] = rand() % 100;
    }

    munmap(ptr, SIZE);

    close(shm_fd);

    return 0;
}
