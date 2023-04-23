#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct {
    int pin;
    int is_curved;
} Pin;

#define BUFFER_SIZE 5

typedef struct {
    Pin buffer[BUFFER_SIZE];
    int in, out;
    sem_t mutex, empty, full;
} SharedMemory;

void produce(SharedMemory *shared_mem) {
    Pin pin;
    pin.pin = rand() % 1000;
    pin.is_curved = rand() % 2;

    sem_wait(&shared_mem->empty);
    sem_wait(&shared_mem->mutex);

    shared_mem->buffer[shared_mem->in] = pin;
    shared_mem->in = (shared_mem->in + 1) % BUFFER_SIZE;

    sem_post(&shared_mem->mutex);
    sem_post(&shared_mem->full);
}

void check_and_sharpen(SharedMemory *shared_mem) {
    Pin pin;

    sem_wait(&shared_mem->full);
    sem_wait(&shared_mem->mutex);

    pin = shared_mem->buffer[shared_mem->out];
    if (!pin.is_curved) {
        printf("Sharpening pin %d\n", pin.pin);
    } else {
        printf("Discarding pin %d\n", pin.pin);
    }

    shared_mem->out = (shared_mem->out + 1) % BUFFER_SIZE;

    sem_post(&shared_mem->mutex);
    sem_post(&shared_mem->empty);
}

int main() {
    srand(time(NULL));

    SharedMemory *shared_mem;
    int shm_fd;

    shm_fd = shm_open("/pins_shm", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedMemory));
    shared_mem = (SharedMemory *)mmap(NULL, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_init(&shared_mem->mutex, 1, 1);
    sem_init(&shared_mem->empty, 1, BUFFER_SIZE);
    sem_init(&shared_mem->full, 1, 0);
    shared_mem->in = shared_mem->out = 0;

    pid_t pid = fork();

    if (pid == 0) { // Child process
        for (int i = 0; i < 20; ++i) {
            check_and_sharpen(shared_mem);
            sleep(rand() % 3);
        }
        exit(0);
    } else { // Parent process
        for (int i = 0; i < 20; ++i) {
            produce(shared_mem);
            sleep(rand() % 3);
        }
        wait(NULL);
    }

    sem_destroy(&shared_mem->mutex);
    sem_destroy(&shared_mem->empty);
    sem_destroy(&shared_mem->full);

    munmap(shared_mem, sizeof(SharedMemory));
    shm_unlink("/pins_shm");

    return 0;
}
