// zk What C compiler do you have that accepts #-style comments?
// # Writer.c Gerrit Mitchell

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>

#define MAX_LEN 256
#define SHM_FILE "shmfile"
#define SHM_PROJ_ID 65

typedef struct {
    int ready;
    int read_count;
    char buffer[MAX_LEN];
} shmseg;

int shmid;
shmseg *shmptr;

void cleanup(int sig) {
    printf("\nWriter shutting down...\n");
    if (shmdt(shmptr) == -1) perror("shmdt");
    if (shmctl(shmid, IPC_RMID, NULL) == -1) perror("shmctl");
    exit(0);
}

int main() {
    key_t key = 11511711;

    shmid = shmget(key, sizeof(shmseg), 0666 | IPC_CREAT);
    if (shmid == -1) { perror("shmget"); exit(1); }

    shmptr = (shmseg*) shmat(shmid, NULL, 0);
    if (shmptr == (void*) -1) { perror("shmat"); exit(1); }

    // Initialize shared memory
    shmptr->ready = 0;
    shmptr->read_count = 0;

    signal(SIGINT, cleanup);

    char input[MAX_LEN];
    while (1) {
        // Wait until readers finish
        while (shmptr->ready == 1) {
            // zk I don't think I'd sleep that long.
            // For this lap, even a busy wait is fine.
            sleep(1);
        }

        printf("Enter message: ");
        fflush(stdout);
        if (!fgets(input, MAX_LEN, stdin)) break;
        input[strcspn(input, "\n")] = '\0';

        strncpy(shmptr->buffer, input, MAX_LEN);
        shmptr->read_count = 0;
        shmptr->ready = 1; // Readers can now read
    }

    cleanup(0);
    return 0;
}

