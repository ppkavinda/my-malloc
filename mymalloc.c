#include <stdio.h>
#include "mymalloc.h"

int main(int argc, char const *argv[])
{
    void *one = MyMalloc(8);
    void *two = MyMalloc(3);
    MyFree(one);
    MyMalloc(2);
    MyMalloc(1);
    MyMalloc(2);
    MyMalloc(5);
    MyMalloc(5);
    MyFree(two);

    printRAM();
    // testprint();
    
    return 0;
}
