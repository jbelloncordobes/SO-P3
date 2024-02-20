#include "crc.h"
#include "fileManager.h"
#include "myutils.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for sleep function : waits for seconds

#define N 1

FileManager fm;
pthread_mutex_t lock;
// my_semaphore sem;


void* worker_function(void * arg){
    while (1){
        dataEntry  d;
        char * buff[256];
        short crc;
        int res = getAndReserveFile(&fm, &d); // Reserves a file. The release is missing. Where should you put it?
        read(d.fdcrc, &crc, sizeof(short int));
        int nBytesReadData = read(d.fddata, buff, 256);

        if (crc != crcSlow(buff, nBytesReadData)) {
            printf("CRC error in file %d\n", d.filename);
        }
    }
}

int main(int argc, char ** argv) {
    // my_sem_init(&sem, 1); 
    initialiseFdProvider(&fm, argc, argv);
    printf("%d - %d | %d - %d | %d, %d\n", fm.fileAvailable[0], fm.fileFinished[0], fm.fdData[0],fm.fdCRC[0], fm.nFilesRemaining, fm.nFilesTotal);
    // printf("%d - %d | %d\n", fm.fileAvailable[0],fm.fileFinished[0], argc);
    pthread_t threadID[N];
    startTimer(0);
    for (int i = 0; i < N; ++i) {
        pthread_t thread;
        pthread_create(&threadID[i], NULL, worker_function, NULL);
    }

    for (int i = 0; i < N; ++i) {
        pthread_join(threadID[i], NULL);
    }
    long total_time = endTimer(0);
    printf("For %d threads the total time is %ld\n", N, total_time);
    destroyFdProvider(&fm);
}