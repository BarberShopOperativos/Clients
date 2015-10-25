#include "ClientThread.h"

char SPECIAL[] = "Especial";
char REGULAR[] = "Regular";
char NO_CHAIR_FOUND[] = "Cliente %s %d no encuentra silla disponible y se retira. Hora: %s \n";
char SP_CLIENT_QUEUE_OUT[] = "Cliente %s %d no permite la ejecucion de cliente normal, este se retira. Hora: %s \n";
char CHAIR_FOUND[] = "Cliente %s %d logra encontrar silla disponible. Hora: %s \n";
char CHAIR_MOVE[] = "Cliente %s %d se mueve a otra silla en la cola. Hora: %s \n";
char MOVED_TO_BARBER[] = "Cliente %s %d va ser atendido por un barbero. Hora: %s \n";
char EXEC_BARBER[] = "Cliente %s %d ya fue atentido por el barbero. Hora: %s \n";
char EXEC_CASHIER[] = "Cliente %s %d ya fue atentido por el cajero. Ahora se retira. Hora: %s \n";


/// <summary>
/// Method to allocate memory for a single client
/// </summary>
ClientThread *createClient (
    int pId,
    int *pSpecialClientsCounterPtr,
    bool pHasPriority,
    ClientThreadList *pList,
    Container *pChairsQueue,
    Container *pBarbersList,
    Container *pCashiersQueue,
    Semaphore *pChairsSem,
    Semaphore *pBarbersSem,
    Semaphore *pCashierSem,
    Semaphore *pFileSem,
    Semaphore *pSClientsCounterSem)
{
    // Initialize client
    ClientThread *client = malloc(sizeof(ClientThread));

    // Set client props
    client->id = pId;
    client->specialClientsCounterPtr = pSpecialClientsCounterPtr;
    client->isActive = true;
    client->state = 1;
    client->hasPriority = pHasPriority;
    client->actualNode = NULL;
    client->nextClient = NULL;
    client->chairsQueue = pChairsQueue;
    client->barbersList = pBarbersList;
    client->cashiersQueue = pCashiersQueue;
    client->chairsSem = pChairsSem;
    client->barbersSem = pBarbersSem;
    client->cashierSem = pCashierSem;
    client->fileSem = pFileSem;
    client->sClientsCounterSem = pSClientsCounterSem;

    // Add thread to list
    addNodeToClientThreadList(pList,client);
    printf("Thread: %d:  creado \n", client->id);

    // Initialize client thread
    if(pthread_create(&client->thread, 0, threadRun, client))
    {
		fprintf(stderr, "Error creating thread\n");
	}

    return client;
}

/// <summary>
/// Method to allocate memory for the ClientThreadList structure
/// </summary>
ClientThreadList *createClientTreadList()
{
    ClientThreadList *list = malloc(sizeof(ClientThreadList));
    list->first = list->last = NULL;
    return list;
}

/// <summary>
/// Struct method to add an existing client to a list
/// </summary>
void addNodeToClientThreadList(ClientThreadList *pList, ClientThread *pClient)
{
    if (pList->first == NULL)
    {
        pList->first = pList->last = pClient;
    }
    else
    {
        pList->last->nextClient = pClient;
        pList->last = pClient;
    }
    pList->length += 1;
}

/// <summary>
/// Method to wait all threads to finish!
/// </summary>
void joinThreadList(ClientThreadList* pList)
{
	ClientThread *client = pList->first;
	while(client){
        pthread_join(client->thread,0);
		client = client->nextClient;
	}
}

/// <summary>
/// Client run method
/// </summary>
void *threadRun(void  *threadArg)
{
    ClientThread *client = (ClientThread*)threadArg;

    while (client->isActive)
    {
        switch(client->state)
        {
            case 1 :
                verifyChairPosition(client);
                break;
            case 2 :
                executeBarberLogic(client);
                break;
            case 3 :
              executeCashierLogic(client);
              break;
            default :
              printf("Error en el thread: %d \n", client->id);
        }
    }
    pthread_exit(NULL);
    return NULL;
}

/// <summary>
/// Client method to assign a chair to a thread or move to the barbers list
/// </summary>
void verifyChairPosition(ClientThread *pClient)
{
    // Find the client chair
    if(pClient->actualNode == NULL) assignNewChair(pClient);
    else moveFromChair(pClient);
    // Just to free CPU
    sleep(1);
}

/// <summary>
/// Assigns a chair to the new client
/// </summary>
void assignNewChair(ClientThread *pClient)
{
    Node *chair = findEmptyChair(pClient);
    if(chair != NULL)
    {
        pClient->actualNode = chair;
        writeLog(100,pClient,CHAIR_FOUND);
    }
    else
    {
        writeLog(100,pClient,NO_CHAIR_FOUND);
        pClient->isActive = false;
    }
}

/// <summary>
/// Moves from one chair to another ultill first chair is found
/// </summary>
void moveFromChair(ClientThread *pClient)
{
    // Special Clients Logic
    if(pClient->hasPriority)
    {
        moveSpecialClient(pClient);
    }
    else
    {
        moveRegularClient(pClient);
    }
}

/// <summary>
/// Applies the regular client move logic
/// </summary>
void moveRegularClient(ClientThread *pClient)
{
    // If client can move to barbers
    if(pClient->actualNode == pClient->chairsQueue->lastNode)
    {
        sem_wait(pClient->sClientsCounterSem->mutex);
        if(moveToBarber(pClient))
        {
            int *specialClientsCounter = pClient->specialClientsCounterPtr;
            *specialClientsCounter = 0;
        }
        sem_post(pClient->sClientsCounterSem->mutex);
    }
    else // Client tries to get another chair
    {
        sem_wait(pClient->chairsSem->mutex);

        // Move from chair
        if(pClient->actualNode->next->isOcupied == false)
        {
            pClient->actualNode->isOcupied = false;
            pClient->actualNode = pClient->actualNode->next;
            pClient->actualNode->isOcupied = true;
            writeLog(100,pClient,CHAIR_MOVE);
        }

        sem_post(pClient->chairsSem->mutex);
    }
}

/// <summary>
/// Applies the special client move logic
/// </summary>
void moveSpecialClient(ClientThread *pClient)
{
    bool triedToMove = false;

    sem_wait(pClient->sClientsCounterSem->mutex);
    // Only 3 special clients in a row can move
    if(*pClient->specialClientsCounterPtr < 3)
    {
        triedToMove = true;

        if(moveToBarber(pClient))
        {
            int *specialClientsCounter = pClient->specialClientsCounterPtr;
            *specialClientsCounter += 1;
        }
    }
    sem_post(pClient->sClientsCounterSem->mutex);

    // In this case the special client will stop the progra normal execution so
    // we take it out of the chairs
    if((triedToMove == false) &&
        (pClient->actualNode == pClient->chairsQueue->lastNode))
    {

        sem_wait(pClient->chairsSem->mutex);
        pClient->actualNode->isOcupied = false;
        sem_post(pClient->chairsSem->mutex);

        pClient->isActive = false;
        writeLog(100,pClient,SP_CLIENT_QUEUE_OUT);
    }
}

/// <summary>
/// Tries to move the client to the barbers list
/// </summary>
bool moveToBarber(ClientThread *pClient)
{
    Node *emptyBarber = findEmptyBarber(pClient);
    bool isMoved = false;

    if(emptyBarber != NULL)
    {
        // Deocoupate chair
        sem_wait(pClient->chairsSem->mutex);
        pClient->actualNode->isOcupied = false;
        sem_post(pClient->chairsSem->mutex);
        pClient->actualNode = emptyBarber;
        pClient->state = 2;
        writeLog(100,pClient,MOVED_TO_BARBER);
        isMoved = true;
    }

    return isMoved;
}


/// <summary>
/// Search the chairs queue for an empty chair
/// </summary>
Node *findEmptyChair(ClientThread *pClient)
{
    sem_wait(pClient->chairsSem->mutex);

    Container *chairsQueue = pClient->chairsQueue;
    Node *tempNode, *emptyChair = NULL;
    tempNode = chairsQueue->lastNode;

    while(tempNode != NULL)
    {
        if(tempNode->isOcupied == false)
        {
            emptyChair = tempNode;
            tempNode->isOcupied = true;
            break;
        }
        tempNode = tempNode->before;
    }

    sem_post(pClient->chairsSem->mutex);

    return emptyChair;
}

/// <summary>
/// Looks for an empty barber to assign it to a client
/// </summary>
Node *findEmptyBarber(ClientThread *pClient)
{
    sem_wait(pClient->barbersSem->mutex);

    Container *barbersList = pClient->barbersList;
    Node *tempNode, *emptyBarber = NULL;

    tempNode = barbersList->firstNode;

    while(tempNode != NULL)
    {
        if(tempNode->isOcupied == false)
        {
            emptyBarber = tempNode;
            tempNode->isOcupied = true;
            break;
        }
        tempNode = tempNode->next;
    }

    sem_post(pClient->barbersSem->mutex);

    return emptyBarber;
}

/// <summary>
/// Sleeps and leaves the barbers list
/// </summary>
void executeBarberLogic(ClientThread *pClient)
{
    pClient->state = 3;
    sem_wait(pClient->barbersSem->mutex);
    pClient->actualNode->isOcupied = false;
    // Sleep to simulate the barber execution
    sleep(generateRandomInRange(4,8));
    sem_post(pClient->barbersSem->mutex);
    writeLog(100,pClient,EXEC_BARBER);
}


/// <summary>
/// Sleeps and leaves the barbers list
/// </summary>
void executeCashierLogic(ClientThread *pClient)
{
    sem_wait(pClient->cashierSem->mutex);
    sleep(generateRandomInRange(2,6));
    sem_post(pClient->cashierSem->mutex);
    writeLog(150,pClient,EXEC_CASHIER);
    pClient->isActive = false;
}


/// <summary>
/// Formats a string to be written in the file
/// </summary>
void writeLog(int pBufferSize, ClientThread *pClient, char *pFormat)
{
    char str[pBufferSize];
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    char *clientType;

    if(pClient->hasPriority) clientType = SPECIAL;
    else clientType = REGULAR;

    sprintf(str, pFormat,clientType,pClient->id,asctime (timeinfo));
    str[pBufferSize - 1] = NULL;

    printf(str);
    writeFileAppend(str,pClient->fileSem->mutex);

    return str;
}






