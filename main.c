#include <stdio.h>
#include <stdlib.h>
#include "BarberShopStructures.h"


int main()
{

    ClientThreadList *list = createClientTreadList();

    createClient(1,false,NULL,list);
    createClient(2,false,NULL,list);
    createClient(3,false,NULL,list);
    joinThreadList(list);

    printf("Hello world!\n");
    return 0;
}
