#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define BUFFER_SIZE 5

typedef struct {
    int buffer[BUFFER_SIZE];
    int in;
    int out;
} SharedMemory;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void produce(SharedMemory *shared_mem, int sem_id) {
    int item = rand() % 100;
    int is_curved = rand() % 2;

    if (is_curved) {
        printf("Отбраковано: %d\n", item);
        return;
    }

    struct sembuf sops[2] = {
            {1, -1, 0},// Wait for empty
            {0, 1, 0}  // Lock the buffer
    };
    semop(sem_id, sops, 2);

    shared_mem->buffer[shared_mem->in] = item;
    shared_mem->in = (shared_mem->in + 1) % BUFFER_SIZE;
    printf("Произведено: %d\n", item);

    struct sembuf signal_mutex = {0, -1, 0};
    semop(sem_id, &signal_mutex, 1);
}

void check_and_sharpen(SharedMemory *shared_mem, SharedMemory *shared_mem2, int sem_id) {
    struct sembuf sops[2] = {
            {0, -1, 0},// Wait for mutex
            {1, 1, 0}  // Signal empty
    };
    semop(sem_id, sops, 2);

    int item = shared_mem->buffer[shared_mem->out];
    shared_mem->out = (shared_mem->out + 1) % BUFFER_SIZE;
    printf("Проверено и заточено: %d\n", item);

    struct sembuf sops2[2] = {
            {3, -1, 0},// Wait for empty2
            {2, 1, 0}  // Lock the buffer2
    };
    semop(sem_id, sops2, 2);

    shared_mem2->buffer[shared_mem2->in] = item;
    shared_mem2->in = (shared_mem2->in + 1) % BUFFER_SIZE;

    struct sembuf signal_mutex2 = {2, -1, 0};
    semop(sem_id, &signal_mutex2, 1);
}

void quality_control(SharedMemory *shared_mem2, int sem_id) {
    struct sembuf sops[2] = {
            {2, -1, 0},// Wait for mutex2
            {3, 1, 0}  // Signal empty2
    };
    semop(sem_id, sops, 2);

    int item = shared_mem2->buffer[shared_mem2->out];
    shared_mem2->out = (shared_mem2->out + 1) % BUFFER_SIZE;
    printf("Контроль качества: %d\n", item);

    struct sembuf signal_mutex = {2, -1, 0};
    semop(sem_id, &signal_mutex, 1);
}

int main() {
    srand(time(NULL));

    int shm_id = shmget(IPC_PRIVATE, sizeof(SharedMemory), IPC_CREAT | 0666);
    int shm_id2 = shmget(IPC_PRIVATE, sizeof(SharedMemory), IPC_CREAT | 0666);
    SharedMemory *shared_mem = (SharedMemory *) shmat(shm_id, NULL, 0);
    SharedMemory *shared_mem2 = (SharedMemory *) shmat(shm_id2, NULL, 0);

    shared_mem->in = shared_mem->out = 0;
    shared_mem2->in = shared_mem2->out = 0;

    int sem_id = semget(IPC_PRIVATE, 4, IPC_CREAT | 0666);

    union semun sem_init;
    unsigned short sem_vals[] = {1, BUFFER_SIZE, 1, BUFFER_SIZE};
    sem_init.array = sem_vals;
    semctl(sem_id, 0, SETALL, sem_init);

    pid_t pid = fork();

    if (pid == 0) {// Child 1
        for (int i = 0; i < 10; i++) {
            produce(shared_mem, sem_id);
            sleep(rand() % 3);
        }
        exit(0);
    } else {
        pid_t pid2 = fork();

        if (pid2 == 0) {// Child 2
            for (int i = 0; i < 10; i++) {
                check_and_sharpen(shared_mem, shared_mem2, sem_id);
                sleep(rand() % 3);
            }
            exit(0);
        } else {// Parent
            for (int i = 0; i < 10; i++) {
                quality_control(shared_mem2, sem_id);
                sleep(rand() % 3);
            }

            wait(NULL);
            wait(NULL);

            shmdt(shared_mem);
            shmdt(shared_mem2);
            shmctl(shm_id, IPC_RMID, NULL);
            shmctl(shm_id2, IPC_RMID, NULL);
            semctl(sem_id, 0, IPC_RMID, sem_init);
        }
    }

    return 0;
}
