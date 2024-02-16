#include "fileManager.h"

#include <errno.h> // temp
#include <string.h> // temp
// my_semaphore sem;

void  initialiseFdProvider(FileManager * fm, int argc, char **argv) {
    // Complete the initialisation
    /* Your rest of the initailisation comes here*/
    // my_sem_init(&sem,1)
    // Inicializar semaforo binario 
    fm->nFilesTotal = argc -1;
    fm->nFilesRemaining = fm->nFilesTotal;
    // Initialise enough memory to  store the arrays
    fm->fdData = malloc(sizeof(int) * fm->nFilesTotal);
    fm->fdCRC= malloc(sizeof(int) * fm->nFilesTotal);
    fm->fileFinished = malloc(sizeof(int) * fm->nFilesTotal);
    fm->fileAvailable = malloc(sizeof(int) * fm->nFilesTotal);

    int i;
    for (i = 1; i < fm->nFilesTotal +1; ++i) {
        char path[100];
        strcpy(path, argv[i]);
        strcat(path, ".crc");
        fm->fdData[i-1] = open(argv[i], O_RDONLY);
        fm->fdCRC[i-1] = open(path, O_RDONLY);

        printf("%d, %d\n");

        if (fm->fdData[i] == 0 || fm->fdCRC == 0){
            printf("Couldn't open at least one of the files\n");
            exit(1);
        }

        fm->fileFinished[i] = 0;
        fm->fileAvailable[i] = 1;

        printf("%d - %d | %d - %d | %s\n", fm->fileAvailable[i], fm->fileFinished[i], fm->fdData[i], fm->fdCRC[i], strerror(errno));
    }
}
void  destroyFdProvider(FileManager * fm) {
    int i;
    for (i = 0; i < fm->nFilesTotal; i++) {
        close(fm->fdData[i]);
        close(fm->fdCRC[i]);
    }
    free(fm->fdData);
    free(fm->fdCRC);
    free(fm->fileFinished);
}
int getAndReserveFile(FileManager *fm, dataEntry * d) {
    // This function needs to be implemented by the students
    int i;
    // Semaforo antes del if pero despues del for. Signal antes del return 0 y, por si no entra, despues del if pero antes de terminar el for
    for (i = 0; i < fm->nFilesTotal; ++i) {
        if (fm->fileAvailable[i] && !fm->fileFinished[i]) {
            d->fdcrc = fm->fdCRC[i];
            d->fddata = fm->fdData[i];
            d->index = i;

            // You should mark that the file is not available 
            fm->fileAvailable[i] = 0;

            return 0;
        }
    }             
    return 1;
}
void unreserveFile(FileManager *fm,dataEntry * d) {
    // sem 
    fm->fileAvailable[d->index] = 1; 
    // sem
}

void markFileAsFinished(FileManager * fm, dataEntry * d) {
    fm->fileFinished[d->index] = 1;
    fm->nFilesRemaining--; //mark that a file has finished
    if (fm->nFilesRemaining == 0) {
        printf("All files have been processed\n");
        //TO COMPLETE: unblock all waiting threads, if needed
    }
}
