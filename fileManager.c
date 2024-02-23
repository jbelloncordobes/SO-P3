#include "fileManager.h"

#include "myutils.h"
my_semaphore sem;

void  initialiseFdProvider(FileManager * fm, int argc, char **argv) {
    // Complete the initialisation
    /* Your rest of the initailisation comes here*/
    my_sem_init(&sem, 1);
    fm->nFilesTotal = argc -1;
    fm->nFilesRemaining = fm->nFilesTotal;
    // Initialise enough memory to  store the arrays
    fm->fdData = malloc(sizeof(int) * fm->nFilesTotal);
    fm->fdCRC= malloc(sizeof(int) * fm->nFilesTotal);
    fm->fileFinished = malloc(sizeof(int) * fm->nFilesTotal);
    fm->fileAvailable = malloc(sizeof(int) * fm->nFilesTotal);
    fm->fileName = malloc(sizeof(char)*fm->nFilesTotal);
    for (int i = 0; i < fm->nFilesTotal; i++){
        fm->fileName[i] = malloc(sizeof(char)*50);
    }

    printf("files: %d\n", fm->nFilesTotal);
    for (int i = 1; i < fm->nFilesTotal +1; ++i) {
        char path[100];
        strcpy(path, argv[i]);
        strcat(path, ".crc");
        fm->fdData[i-1] = open(argv[i], O_RDONLY);
        fm->fdCRC[i-1] = open(path, O_RDONLY);
        strcpy(fm->fileName[i-1], argv[i]);

        // printf("%d, %d\n", fm->fdData[i-1], fm->fdCRC[i-1]);

        if (fm->fdData[i-1] < 0 || fm->fdCRC[i-1] < 0){
            printf("Couldn't open at least one of the files\n");
            exit(1);
        }

        fm->fileFinished[i-1] = 0;
        fm->fileAvailable[i-1] = 1;

        // printf("%d - %d | %d - %d\n", fm->fileAvailable[i], fm->fileFinished[i], fm->fdData[i], fm->fdCRC[i]);
    }
}
void  destroyFdProvider(FileManager * fm) {
    int i;
    for (i = 0; i < fm->nFilesTotal; i++) {
        close(fm->fdData[i]);
        close(fm->fdCRC[i]);
        free(fm->fileName[i]);
    }
    free(fm->fdData);
    free(fm->fdCRC);
    free(fm->fileFinished);
    free(fm->fileName);
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
            d->filename = fm->fileName[i];
            //strcpy(d->filename, fm->fileName[i]);
            // You should mark that the file is not available 
            my_sem_wait(&sem);
            fm->fileAvailable[i] = 0;
            my_sem_signal(&sem);
            return 0;
        }
    }             
    return 1;
}
void unreserveFile(FileManager *fm,dataEntry * d) {
    // sem 
    my_sem_wait(&sem);
    fm->fileAvailable[d->index] = 1; 
    my_sem_signal(&sem);
    // sem
}

void markFileAsFinished(FileManager * fm, dataEntry * d) {
    my_sem_wait(&sem);
    fm->fileFinished[d->index] = 1;
    fm->nFilesRemaining--; //mark that a file has finished
    my_sem_signal(&sem);
    if (fm->nFilesRemaining == 0) {
        printf("All files have been processed\n");
        pthread_cond_broadcast(&sem.cond);
        // while hasta que my_sem_signal(&sem) sea 0
        //TO COMPLETE: unblock all waiting threads, if needed
        
    }
}
