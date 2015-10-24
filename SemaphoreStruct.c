#include "SemaphoreStruct.h"

/// <summary>
/// Method to create a new Semaphore struct
/// </summary>
Semaphore *createSemaphore (char *pName)
{
    Semaphore *semaphore = malloc(sizeof(Semaphore));
    semaphore->name = pName;

    //create & initialize existing semaphore
    semaphore->mutex = sem_open(pName,O_CREAT,0644,1);
    if(semaphore->mutex == SEM_FAILED)
    {
        printf("Error inicializando semaforo: %s \n", pName);
        sem_unlink(pName);
        exit(-1);
    }

    printf("Semaforo %s creado... \n", pName);

    return semaphore;
}
