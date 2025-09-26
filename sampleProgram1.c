/**********************************************************
 *5.  Submit your screenshots5.  Submit your screenshots
 *
 * sampleProgram1.c
 * CIS 451 Lab 5
 *
 * !!!NAMES!!!
 *************************************************************/

#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/ipc.h> 
#include <sys/shm.h>

#define SHM_SIZE 4096

int main () 
{ 
   int shmId; 
   char *sharedMemoryPtr;
   
   if((shmId = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT|S_IRUSR|S_IWUSR)) < 0) { 
      perror ("Unable to get shared memory\n"); 
      exit (1); 
   } 
   if((sharedMemoryPtr = shmat (shmId, 0, 0)) == (void*) -1) { 
      perror ("Unable to attach\n"); 
      exit (1); 
   }

   printf("Value a: %p\t Value b: %p\n", (void *) sharedMemoryPtr, (void *) sharedMemoryPtr + SHM_SIZE); 

   struct shmid_ds shm_info;

   if (shmctl(shmId, IPC_STAT, &shm_info) == -1) {
      perror("shmctl failed");
      exit(1); 
   }

   size_t size = shm_info.shm_segsz;

   printf("Size of Shared Memory Segment is %ld bytes.\n", size);
   printf("Shared Memory Segment ID: %d\n", shmId);
   pause();
   if(shmdt (sharedMemoryPtr) < 0) { 
      perror ("Unable to detach\n"); 
      exit (1); 
   } 
   if(shmctl (shmId, IPC_RMID, 0) < 0) { 
      perror ("Unable to deallocate\n"); 
      exit(1); 
   }

   return 0; 
}
