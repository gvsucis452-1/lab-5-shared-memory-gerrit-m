// zk What C compiler do you have that accepts #-style comments?
// # Reader.c Gerrit Mitchell

#include <stdio.h>
#include <stdlib.h>
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
    printf("\nReader shutting down...\n");
    if (shmdt(shmptr) == -1) perror("shmdt");
    exit(0);
}

int main() {
    key_t key = 11511711;

    shmid = shmget(key, sizeof(shmseg), 0666);
    if (shmid == -1) { perror("shmget"); exit(1); }

    shmptr = (shmseg*) shmat(shmid, NULL, 0);
    if (shmptr == (void*) -1) { perror("shmat"); exit(1); }

    signal(SIGINT, cleanup);

    while (1) {
        // Wait for writer to provide data
        while (shmptr->ready == 0) {
            // zk Try removing the calls to sleep and see if everything still works as expected. 
            sleep(1);
        }

        printf("Reader [%d] read: %s\n", getpid(), shmptr->buffer);
        fflush(stdout);

        // Update read count
        shmptr->read_count++;

        // If both readers have read, unlock writer
        if (shmptr->read_count >= 2) {
            shmptr->ready = 0;
        }

        sleep(1); // avoid tight loop
    }

    cleanup(0);
    return 0;
}

