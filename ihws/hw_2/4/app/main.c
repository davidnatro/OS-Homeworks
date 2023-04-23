#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define PIN_COUNT 100
#define SHARED_MEM_NAME "/shared_mem"
#define SEM_CURVE_CHECK "/sem_curve_check"
#define SEM_SHARPEN "/sem_sharpen"
#define SEM_QUALITY_CONTROL "/sem_quality_control"

typedef struct {
    int pins[PIN_COUNT];
    int pinIndex;
} SharedMemory;

void curve_check(SharedMemory *sharedMem, sem_t *semCurveCheck, sem_t *semSharpen) {
    while (sharedMem->pinIndex < PIN_COUNT) {
        sem_wait(semCurveCheck);
        int pin = sharedMem->pins[sharedMem->pinIndex];
        int check = rand() % 2;

        if (check == 0) {
            printf("Worker 1: Pin %d is not crooked.\n", pin);
            sem_post(semSharpen);
        } else {
            printf("Worker 1: Pin %d is crooked. Discarding it.\n", pin);
        }
        sharedMem->pinIndex++;
        sleep(rand() % 3);
    }
}

void sharpen_pin(SharedMemory *sharedMem, sem_t *semSharpen, sem_t *semQualityControl) {
    while (sharedMem->pinIndex < PIN_COUNT) {
        sem_wait(semSharpen);
        int pin = sharedMem->pins[sharedMem->pinIndex];
        printf("Worker 2: Sharpening pin %d.\n", pin);
        sem_post(semQualityControl);
        sleep(rand() % 3);
    }
}

void quality_control(SharedMemory *sharedMem, sem_t *semQualityControl) {
    while (sharedMem->pinIndex < PIN_COUNT) {
        sem_wait(semQualityControl);
        int pin = sharedMem->pins[sharedMem->pinIndex];
        int quality = rand() % 2;

        if (quality == 0) {
            printf("Worker 3: Pin %d passed quality control. Sending to packaging.\n", pin);
        } else {
            printf("Worker 3: Pin %d failed quality control. Discarding it.\n", pin);
        }
        sleep(rand() % 3);
    }
}

int main() {
    srand(time(NULL));

    sem_t *semCurveCheck = sem_open(SEM_CURVE_CHECK, O_CREAT, 0644, 1);
    sem_t *semSharpen = sem_open(SEM_SHARPEN, O_CREAT, 0644, 0);
    sem_t *semQualityControl = sem_open(SEM_QUALITY_CONTROL, O_CREAT, 0644, 0);

    int sharedMemFd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0644);
    ftruncate(sharedMemFd, sizeof(SharedMemory));
    SharedMemory *sharedMem = (SharedMemory *) mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemFd, 0);

    for (int i = 0; i < PIN_COUNT; i++) {
        sharedMem->pins[i] = i + 1;
    }
    sharedMem->pinIndex = 0;

    pid_t pid1, pid2, pid3;

    if ((pid1 = fork()) == 0) {
        curve_check(sharedMem, semCurveCheck, semSharpen);
        exit(0);
    }

    if ((pid2 = fork()) == 0) {
        sharpen_pin(sharedMem, semSharpen, semQualityControl);
        exit(0);
    }
    if ((pid3 = fork()) == 0) {
        quality_control(sharedMem, semQualityControl);
        exit(0);
    }

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    waitpid(pid3, NULL, 0);

    sem_close(semCurveCheck);
    sem_close(semSharpen);
    sem_close(semQualityControl);

    sem_unlink(SEM_CURVE_CHECK);
    sem_unlink(SEM_SHARPEN);
    sem_unlink(SEM_QUALITY_CONTROL);

    munmap(sharedMem, sizeof(SharedMemory));
    close(sharedMemFd);
    shm_unlink(SHARED_MEM_NAME);

    return 0;
}
