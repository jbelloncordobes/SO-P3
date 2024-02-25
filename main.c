#include "crc.h"
#include "fileManager.h"
#include "myutils.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for sleep function : waits for seconds

#define N 2

FileManager fm;
pthread_mutex_t lock;


void* worker_function(void * arg){
    while (1){
        dataEntry  d;
        char * buff[256];
        unsigned short crc;
        int res = getAndReserveFile(&fm, &d); // Reserves a file. The release is missing. Where should you put it?
        if (res == 1){
            return;
        }
        read(d.fdcrc, &crc, sizeof(unsigned short));
        int nBytesReadData = read(d.fddata, buff, 256);

        if (crc != crcSlow(buff, nBytesReadData)) {
            printf("CRC error in file %s\n", d.filename);
        }

        unreserveFile(&fm, &d);
        

        if (nBytesReadData <256){
            markFileAsFinished(&fm, &d);
        }

    }
}

int main(int argc, char ** argv) {
    initialiseFdProvider(&fm, argc, argv);
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