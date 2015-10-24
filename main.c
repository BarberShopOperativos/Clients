#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "BarberShopStructures.h"

#define INT_SEG_SIZE 5
#define STRUCT_SEG_SIZE 10

void programInit();
int locateSegment(key_t pKey, int pSegmentSize);
int *pointIntSegment(int pShmID);
Container *pointContainerSegment(int pShmID);
Node *pointNodeSegment(int pShmID);
key_t locateSharedStrutures(Container *pContainer, key_t pBaseNodeKey, bool pIsQueue);

int main()
{
    programInit();
}


void programInit()
{
    // Declare needed variables
    int chairsShmID, barbersShmID, cashiershmID, specialClientsCounterShmID,stopClientsShmID,
        stopSpecialClientesShmID,chairsQuantityShmID, barbersQuantityShmID,
        *specialClientsCounterPtr, *stopClientesPtr, *stopSpecialClientsPtr, *chairsQuantityPtr, *barbersQuantityPtr;
    key_t chairsKey, barbersKey, cashierKey, specialClientsCounterKey,stopClientsKey,
        stopSpecialClientsKey,chairsQuantityKey,barbersQuantityKey,baseNodeKey;
    Container *chairsQueue, *barbersList,*cashierQueue;

    // Segment Keys
    chairsKey = 5677;
    barbersKey = 5678;
    cashierKey = 5679;
    specialClientsCounterKey = 5680;
    stopClientsKey = 5681;
    stopSpecialClientsKey = 5682;
    chairsQuantityKey = 5683;
    barbersQuantityKey = 5684;

    // Locate User input values from shared memory
    chairsQuantityShmID = locateSegment(chairsQuantityKey,INT_SEG_SIZE);
    barbersQuantityShmID = locateSegment(barbersQuantityKey,INT_SEG_SIZE);
    chairsQuantityPtr = pointIntSegment(chairsQuantityShmID);
    barbersQuantityPtr = pointIntSegment(barbersQuantityShmID);

    // Locate Structures Segments
    chairsShmID = locateSegment(chairsKey,STRUCT_SEG_SIZE * *chairsQuantityPtr);
    barbersShmID = locateSegment(barbersKey,STRUCT_SEG_SIZE * *barbersQuantityPtr);
    cashiershmID = locateSegment(cashierKey,STRUCT_SEG_SIZE);
    specialClientsCounterShmID = locateSegment(specialClientsCounterKey,INT_SEG_SIZE);
    stopClientsShmID = locateSegment(stopClientsKey,INT_SEG_SIZE);
    stopSpecialClientesShmID = locateSegment(stopSpecialClientsKey,INT_SEG_SIZE);

    // Get the structures pointers
    chairsQueue = pointContainerSegment(chairsShmID);
    barbersList = pointContainerSegment(barbersShmID);
    cashierQueue = pointContainerSegment(cashiershmID);
    specialClientsCounterPtr = pointIntSegment(specialClientsCounterShmID);
    stopClientesPtr = pointIntSegment(stopClientsShmID);
    stopSpecialClientsPtr = pointIntSegment(stopSpecialClientesShmID);

    chairsQueue->firstNode = chairsQueue->lastNode = NULL;
    chairsQueue->length = barbersList->length = 0;
    barbersList->firstNode = barbersList->lastNode = NULL;
    cashierQueue->firstNode = cashierQueue->lastNode = NULL;

    baseNodeKey = barbersQuantityKey;
    baseNodeKey = locateSharedStrutures(chairsQueue,baseNodeKey,true);
    baseNodeKey = locateSharedStrutures(barbersList,baseNodeKey,false);

    printf("Cantidad sillas: %d ... \n", chairsQueue->length);
    printf("Cantidad barberos: %d ... \n", barbersList->length);
    printf("Special clients counter: %d \n", *specialClientsCounterPtr);


    ClientThreadList *list = createClientTreadList();
    printf("Creando Clientes... \n");

    while(*stopClientesPtr == 1)
    {
        createClient(list->length,false,NULL,list,NULL,NULL,NULL);
        sleep(generateRandomInRange(2,4));
    }

    joinThreadList(list);


}


/// <summary>
/// Locates a shared memory segment
/// </summary>
int locateSegment(key_t pKey, int pSegmentSize)
{
    int shmID;

    if ((shmID = shmget(pKey, pSegmentSize, 0666)) < 0) {
        printf("Error locating segment with key: %d /n",pKey);
        exit(1);
    }

    //printf("Segment located ... \n");
    return shmID;
}

/// <summary>
/// Gets the pointer to an int value segment
/// </summary>
int *pointIntSegment(int pShmID)
{

    int *pointer;

    if ((pointer = shmat(pShmID, NULL, 0)) == (int *) -1) {
        printf("Error pointing to segment with key: %d /n", pShmID);
        exit(1);
    }

    //printf("Segment pointed ... \n");
    return pointer;
}

/// <summary>
/// Gets the pointer to an Container value segment
/// </summary>
Container *pointContainerSegment(int pShmID)
{

    Container *pointer;
    if ((pointer = shmat(pShmID, NULL, 0)) == (Container *) -1) {
        printf("Error pointing to segment with key: %d /n", pShmID);
        exit(1);
    }

    //printf("Segment pointed ... \n");
    return pointer;
}

/// <summary>
/// Gets the pointer to an Node value segment
/// </summary>
Node *pointNodeSegment(int pShmID)
{

    Node *pointer;
    if ((pointer = shmat(pShmID, NULL, 0)) == (Node *) -1) {
        printf("Error pointing to segment with key: %d /n", pShmID);
        exit(1);
    }

    //printf("Segment pointed ... \n");
    return pointer;
}

/// <summary>
/// Gets the node segments of an specific container
/// </summary>
key_t locateSharedStrutures(Container *pContainer, key_t pBaseNodeKey, bool pIsQueue)
{
    // Initialize the chairs queue spaces
    Node *actualNode;
    key_t baseNodeKey = pBaseNodeKey;
    int nodeShmID;
    int baseIndex;

    for(baseIndex = 0; baseIndex < pContainer->maxLenght; baseIndex++)
    {
        baseNodeKey += 1;
        nodeShmID = locateSegment(baseNodeKey,STRUCT_SEG_SIZE);
        actualNode = pointNodeSegment(nodeShmID);

        if(pIsQueue) addExistingNodeToQueueContainer(pContainer,actualNode);
        else addExistingNodeToListContainer(pContainer,actualNode);

        printf("Node BaseKey: %d \n", actualNode->id);
    }

    return baseNodeKey;

}




