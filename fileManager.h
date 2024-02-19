#pragma once
//#include <pthread.h>
#include <fcntl.h>
#include <stdlib.h>

typedef struct {
    int fdcrc;
    int fddata;
    int index;
    char * filename;
} dataEntry;


//pthread_mutex_t lock; // Da problemas al compilar: /usr/bin/ld: /tmp/cc1e8qPP.o:(.bss+0x0): multiple definition of `lock'; /tmp/ccfupCF9.o:(.bss+0x0): first defined here. collect2: error: ld returned 1 exit status

typedef struct {
    int * fdData;
    int * fdCRC;
    int * fileFinished;
    int * fileAvailable;
    char** fileName;

    int nFilesRemaining; // Aquí van los archivos aún por leer o leyendose, el int resultante de hacer open()
    int nFilesTotal; // Aquí van los terminados por leer, el int resultante de hacer open()
} FileManager;

void initialiseFdProvider(FileManager * fm, int argc, char **argv);
void destroyFdProvider(FileManager * fm);
int getAndReserveFile(FileManager *fm, dataEntry * d);
void unreserveFile(FileManager *fm,dataEntry * d);
void markFileAsFinished(FileManager * fm, dataEntry * d);
