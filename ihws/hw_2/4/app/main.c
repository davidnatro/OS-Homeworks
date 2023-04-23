#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define SHARED_MEMORY_SIZE sizeof(Pin)

typedef struct {
    int pin;
    int is_curved;
    int is_sharpened;
    int is_quality_checked;
} Pin;

volatile sig_atomic_t terminate = 0;

void sigint_handler(int sig) {
    terminate = 1;
}

void produce(Pin *pin) {
    pin->pin = rand() % 1000;
    pin->is_curved = rand() % 2;
    pin->is_sharpened = 0;
    pin->is_quality_checked = 0;
}

void check_curvature(sem_t *sem_produce, sem_t *sem_check, Pin *shared_memory) {
    while (!terminate) {
        sem_wait(sem_produce);
        if (terminate) break;
        produce(shared_memory);
        sem_post(sem_check);
        usleep(rand() % 100000);
    }
}

void sharpen(sem_t *sem_check, sem_t *sem_sharpen, Pin *shared_memory) {
    while (!terminate) {
        sem_wait(sem_check);
        if (terminate) break;
        if (!shared_memory->is_curved) {
            shared_memory->is_sharpened = 1;
            sem_post(sem_sharpen);
        }
        usleep(rand() % 100000);
    }
}

void quality_check(sem_t *sem_sharpen, sem_t *sem_produce, Pin *shared_memory) {
    while (!terminate) {
        sem_wait(sem_sharpen);
        if (terminate) break;
        if (shared_memory->is_sharpened) {
            shared_memory->is_quality_checked = 1;
            printf("Pin %d passed quality check and is ready for packaging\n", shared_memory->pin);
        } else {
            printf("Pin %d failed quality check\n", shared_memory->pin);
        }
        sem_post(sem_produce);
        usleep(rand() % 100000);
    }
}

int main() {
    srand(time(NULL));
    sem_t *sem_produce = sem_open("sem_produce", O_CREAT, 0644, 1);
    sem_t *sem_check = sem_open("sem_check", O_CREAT, 0644, 0);
    sem_t *sem_sharpen = sem_open("sem_sharpen", O_CREAT, 0644, 0);

    int shm_fd = shm_open("shared_memory", O_CREAT | O_RDWR, 0644);
    ftruncate(shm_fd, SHARED_MEMORY_SIZE);
    Pin *shared_memory = (Pin *) mmap(0, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    signal(SIGINT, sigint_handler);

    pid_t pids[3];
    pids[0] = fork();
    if (pids[0] == 0) {
        check_curvature(sem_produce, sem_check, shared_memory);
        exit(0);
    }

    pids[1] = fork();
    if (pids[1] == 0) {
        sharpen(sem_check, sem_sharpen, shared_memory);
        exit(0);
    }

    pids[2] = fork();
    if (pids[2] == 0) {
        quality_check(sem_sharpen, sem_produce, shared_memory);
        exit(0);
    }

    for (int i = 0; i < 3; i++) {
        waitpid(pids[i], NULL, 0);
    }

    sem_close(sem_produce);
    sem_close(sem_check);
    sem_close(sem_sharpen);

    sem_unlink("sem_produce");
    sem_unlink("sem_check");
    sem_unlink("sem_sharpen");

    munmap(shared_memory, SHARED_MEMORY_SIZE);
    close(shm_fd);
    shm_unlink("shared_memory");

    return 0;
}