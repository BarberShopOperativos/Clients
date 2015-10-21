#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "BarberShopStructures.h"

#define SHMSZ 50

int main()
{
    programInit();
}


void programInit()
{
    int chairsShmID, barbersShmID, cashiershmID, specialClientsCounterShmID,stopClientsShmID, stopSpecialClientesShmID;
    key_t chairsKey, barbersKey, cashierKey, specialClientsCounterKey,stopClientsKey, stopSpecialClientsKey;

    chairsKey = 5677;
    barbersKey = 5678;
    cashierKey = 5679;
    specialClientsCounterKey = 5680;
    stopClientsKey = 5681;
    stopSpecialClientsKey = 5682;

    Container *chairsQueue;
    Container *barbersList;
    Container *cashierQueue;
    int *specialClientsCounterPtr, *stopClientesPtr, *stopSpecialClientsPtr;

    // Locate Chairs queue segment
    if ((chairsShmID = shmget(chairsKey, SHMSZ, 0666)) < 0) {
        printf("Error locating chairs queue segment! ");
        exit(1);
    }
    // Locate barbers list segment
    if ((barbersShmID = shmget(barbersKey, SHMSZ, 0666)) < 0) {
        printf("Error locating barbers list segment! ");
        exit(1);
    }
    // Locate cashiers queue segment
    if ((cashiershmID = shmget(cashierKey, SHMSZ, 0666)) < 0) {
        printf("Error locating cashiers queue segment! ");
        exit(1);
    }
    // Locate the special clients counter
    if ((specialClientsCounterShmID = shmget(specialClientsCounterKey, SHMSZ, 0666)) < 0) {
        printf("Error locating special clients segment! ");
        exit(1);
    }
    // Locate the clients creation variable
    if ((stopClientsShmID = shmget(stopClientsKey, SHMSZ, 0666)) < 0) {
        printf("Error locating clients control segment! ");
        exit(1);
    }
    // Locate the special clients creation variable
    if ((stopSpecialClientesShmID = shmget(stopSpecialClientsKey, SHMSZ, 0666)) < 0) {
        printf("Error locating special clients control segment! ");
        exit(1);
    }

    // Get the chairs structure from segment
    if ((chairsQueue = shmat(chairsShmID, NULL, 0)) == (Container *) -1) {
        printf("Error getting chairs structure");
        exit(1);
    }
    // Get the barbers structure from segment
    if ((barbersList = shmat(barbersShmID, NULL, 0)) == (Container *) -1) {
        printf("Error getting barbers structure");
        exit(1);
    }
    // Get the chairs structure from segment
    if ((cashierQueue = shmat(cashiershmID, NULL, 0)) == (Container *) -1) {
        printf("Error getting cashiers structure");
        exit(1);
    }
    // Get the chairs structure from segment
    if ((specialClientsCounterPtr = shmat(specialClientsCounterShmID, NULL, 0)) == (int *) -1) {
        printf("Error getting special counter");
        exit(1);
    }
    // Get the chairs structure from segment
    if ((stopClientesPtr = shmat(stopClientsShmID, NULL, 0)) == (bool *) -1) {
        printf("Error getting clients control variable");
        exit(1);
    }
    // Get the chairs structure from segment
    if ((stopSpecialClientsPtr = shmat(stopSpecialClientesShmID, NULL, 0)) == (bool *) -1) {
        printf("Error getting clients control variable");
        exit(1);
    }

    printf("Cantidad sillas: %d ... \n", chairsQueue->maxLenght);
    printf("Cantidad barberos: %d ... \n", barbersList->maxLenght);
    printf("Special clients counter: %d \n", *specialClientsCounterPtr);

    if(*stopClientesPtr == 1)
    {
        printf("Stop clients recuperada ... \n");
    }

    if(*stopSpecialClientsPtr == 1)
    {
        printf("Stop special clients recuperada ... \n");
    }


    ClientThreadList *list = createClientTreadList();

    printf("Creando Clientes... \n");

    /*
    while(true)
    {
        createClient(list->length,false,NULL,list,NULL,NULL,NULL);
        sleep(generateRandomInRange(2,4));
    }

    joinThreadList(list);

    */
}

