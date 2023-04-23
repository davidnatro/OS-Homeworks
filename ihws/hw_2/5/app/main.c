#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

typedef struct {
    int pin;
    int is_curved;
    int is_sharpened;
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
    pin.is_sharpened = 0;

    sem_wait(&shared_mem->empty);
    sem_wait(&shared_mem->mutex);

    shared_mem->buffer[shared_mem->in] = pin;
    shared_mem->in = (shared_mem->in + 1) % BUFFER_SIZE;

    sem_post(&shared_mem->mutex);
    sem_post(&shared_mem->full);
}

void check_and_sharpen(SharedMemory *shared_mem, SharedMemory *shared_mem2) {
    Pin pin;

    sem_wait(&shared_mem->full);
    sem_wait(&shared_mem->mutex);

    pin = shared_mem->buffer[shared_mem->out];
    if (!pin.is_curved) {
        printf("Sharpening pin %d\n", pin.pin);
        pin.is_sharpened = 1;
    } else {
        printf("Discarding pin %d\n", pin.pin);
    }

    shared_mem->out = (shared_mem->out + 1) % BUFFER_SIZE;

    sem_post(&shared_mem->mutex);
    sem_post(&shared_mem->empty);

    if (!pin.is_curved) {
        sem_wait(&shared_mem2->empty);
        sem_wait(&shared_mem2->mutex);

        shared_mem2->buffer[shared_mem2->in] = pin;
        shared_mem2->in = (shared_mem2->in + 1) % BUFFER_SIZE;

        sem_post(&shared_mem2->mutex);
        sem_post(&shared_mem2->full);
    }
}

void quality_control(SharedMemory *shared_mem) {
    Pin pin;

    sem_wait(&shared_mem->full);
    sem_wait(&shared_mem->mutex);

    pin = shared_mem->buffer[shared_mem->out];
    if (pin.is_sharpened) {
        printf("Quality control passed for pin %d\n", pin.pin);
    } else {
        printf("Quality control failed for pin %d\n", pin.pin);
    }

    shared_mem->out = (shared_mem->out + 1) % BUFFER_SIZE;

    sem_post(&shared_mem->mutex);
    sem_post(&shared_mem->empty);
}

int main() {
    srand(time(NULL));

    SharedMemory *shared_mem, *shared_mem2;

    shared_mem = mmap(NULL, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    shared_mem2 = mmap(NULL, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    sem_init(&shared_mem->mutex, 1, 1);
    sem_init(&shared_mem->empty, 1, BUFFER_SIZE);
    sem_init(&shared_mem->full, 1, 0);
    shared_mem->in = shared_mem->out = 0;

    sem_init(&shared_mem2->mutex, 1, 1);
    sem_init(&shared_mem2->empty, 1,
             BUFFER_SIZE);
    sem_init(&shared_mem2->full, 1, 0);
    shared_mem2->in = shared_mem2->out = 0;

    pid_t pid1, pid2;

    pid1 = fork();

    if (pid1 == 0) {// First child process
        for (int i = 0; i < 20; ++i) {
            produce(shared_mem);
            sleep(rand() % 3);
        }
        exit(0);
    } else {
        pid2 = fork();
        if (pid2 == 0) {// Second child process
            for (int i = 0; i < 20; ++i) {
                check_and_sharpen(shared_mem, shared_mem2);
                sleep(rand() % 3);
            }
            exit(0);
        } else {// Parent process
            for (int i = 0; i < 20; ++i) {
                quality_control(shared_mem2);
                sleep(rand() % 3);
            }
            wait(NULL);
            wait(NULL);
        }
    }

    sem_destroy(&shared_mem->mutex);
    sem_destroy(&shared_mem->empty);
    sem_destroy(&shared_mem->full);

    sem_destroy(&shared_mem2->mutex);
    sem_destroy(&shared_mem2->empty);
    sem_destroy(&shared_mem2->full);

    munmap(shared_mem, sizeof(SharedMemory));
    munmap(shared_mem2, sizeof(SharedMemory));

    return 0;
}
