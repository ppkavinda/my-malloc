#include <stdio.h>
#include "mymalloc.h"

int main(int argc, char const *argv[])
{
    void *OS = MyMalloc(5000);
    void *A = MyMalloc(2000);
    void *B = MyMalloc(8000);
    MyFree(A);
    void *C = MyMalloc(8000);
    void *D = MyMalloc(1000);

    printRAM();
    // testprint();
    
    return 0;
}
