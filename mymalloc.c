#include <stdio.h>
#include "mymalloc.h"

int main(int argc, char const *argv[])
{
    void *one = mymalloc(8);
    void *two = mymalloc(3);
    myfree(one);
    mymalloc(2);
    mymalloc(1);
    mymalloc(2);
    mymalloc(5);
    mymalloc(5);
    myfree(two);

    printRAM();
    // testprint();
    
    return 0;
}
