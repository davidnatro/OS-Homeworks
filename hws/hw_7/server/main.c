#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

int main()
{
    int shm_fd;
    void *ptr;
    int *shared_mem;
    const int SIZE = 4096;
    const char *name = "shm";

    shm_fd = shm_open(name, O_RDONLY, 0666);

    ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

    shared_mem = (int *) ptr;
    for(int i=0; i<10; i++)
    {
        printf("%d ", shared_mem[i]);
    }
    printf("\n");

    munmap(ptr, SIZE);

    close(shm_fd);

    shm_unlink(name);

    return 0;
}
