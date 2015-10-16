#include "BarberShopStructures.h"

// Necessary sleep variables
#define NANO_SECOND_MULTIPLIER  1000000  // 1 millisecond = 1,000,000 Nanoseconds
const long INTERVAL_MS = 500 * NANO_SECOND_MULTIPLIER;
struct timespec sleepValue = {0};
pthread_mutex_t mut;
void *threadRun(void * threadArg);


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
/// Method to allocate memory for a single client
/// </summary>
ClientThread *createClient (int pId, bool pHasPriority,
    Node *pActualNode, ClientThreadList *pList)
{
    // Create client
    ClientThread *client = malloc(sizeof(ClientThread));

    // Set client props
    client->id = pId;
    client->isActive = true;
    client->isInBarbersList = false;
    client->isInCashierQueue = false;
    client->isInChairsQueue = false;
    client->hasPriority = pHasPriority;
    client->actualNode = pActualNode;
    client->nextClient = NULL;

    // Add thread to list
    addNodeToClientThreadList(pList,client);

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
/// Method to allocate memory for a simple node
/// </summary>
Node *createNode (int pId)
{
    Node *newNode = malloc(sizeof(Node));
    newNode->id = pId;
    newNode->isOcupied = false;
    newNode->actualClient = false;
    newNode->next = newNode->before = NULL;
    return newNode;
}

/// <summary>
/// Method to allocate memory for the Container
/// </summary>
Container *createContainer()
{
    Container *container = malloc(sizeof(Container));
    container->firstNode = container->lastNode = NULL;
    return container;
}

/// <summary>
/// Struct method to add a node to a list container
/// </summary>
void addNodeToListContainer(Container *pContainer, int pId)
{
    Node *node = createNode(pId);
    if (pContainer->firstNode == NULL)
    {
        pContainer->firstNode = pContainer->lastNode = node;
    }
    else
    {
        pContainer->lastNode->next = node;
        pContainer->lastNode = node;
    }
    pContainer->length += 1;
}

/// <summary>
/// Struct method to print all the list container node values
/// </summary>
void printListContainer(Container *pContainer)
{
    Node *tempNode = pContainer->firstNode;

    while(tempNode != NULL)
    {
        printf("Node Value: %d: \n", tempNode->id);
        tempNode = tempNode->next;
    }

    printf("List End");
}

/// <summary>
/// Struct method to add a node to a queue container
/// </summary>
void addNodeToQueueContainer(Container *pContainer, int pId)
{
    Node *node = createNode(pId);

    if (pContainer->firstNode == NULL)
    {
        pContainer->firstNode = pContainer->lastNode = node;
    }
    else
    {
        pContainer->firstNode->before = node;
        node->next = pContainer->firstNode;
        pContainer->firstNode = node;
    }
    pContainer->length += 1;
}

/// <summary>
/// Struct method remove the Queue head
/// </summary>
void removeQueueContainerHead(Container *pContainer)
{
    Node *removedNode = pContainer->lastNode;
    pContainer->lastNode = pContainer->lastNode->before;
    pContainer->lastNode->next = NULL;
    free(removedNode);
}


/// <summary>
/// Struct method to print all the queue container node values
/// </summary>
void printQueueContainer(Container *pContainer)
{
    Node *tempNode = pContainer->lastNode;

    while(tempNode != NULL)
    {
        printf("Node Value: %d: \n", tempNode->id);
        tempNode = tempNode->before;
    }

    printf("List End");
}

void *threadRun(void  *threadArg)
{
    ClientThread *client = (ClientThread*)threadArg;
    sleepValue.tv_nsec = INTERVAL_MS;

    while (client->isActive)
    {
        printf("Soy el thread: %d:  corriendo \n", client->id);
        nanosleep(&sleepValue, NULL);
    }
    pthread_exit(NULL);
    return NULL;
}
