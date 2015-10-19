#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "BarberShopStructures.h"

#define SHMSZ     50

int main()
{
    programInit();
    return 0;
}


void programInit()
{
    int chairsShmID, barbersShmID, cashiershmID;
    key_t chairsKey, barbersKey, cashierKey;

    chairsKey = 5677;
    barbersKey = 5678;
    cashierKey = 5679;

    Container *chairsQueue;
    Container *barbersList;
    Container *cashierQueue;

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

    printf("Cantidad sillas: %d ... \n", chairsQueue->maxLenght);
    printf("Cantidad barberos: %d ... \n", barbersList->maxLenght);

    /*
    ClientThreadList *list = createClientTreadList();

    printf("Creando Clientes... \n");

    while(true)
    {
        createClient(list->length,false,NULL,list,NULL,NULL,NULL);
        sleep(generateRandomInRange(2,4));
    }
    */
    //joinThreadList(list);


}

