#include <stdio.h>

#define TOTAL_MEMORY 25000
#define HEADER_SIZE sizeof(TOTAL_MEMORY)

char RAM[TOTAL_MEMORY];

void* mymalloc(int requiredSize) {
    // initialize
    if (RAM[0] != 1) {
        for (int i=0; i<TOTAL_MEMORY; i++) RAM[i] = 0;

        RAM[0] = 1;     // ram-initialized flag
        *(int *) &RAM[1] = 'f';     // block is free
        *(int *) &RAM[2] = 25000 - 2 - HEADER_SIZE;   // block size
        printf("Ram initialized\n");
    }

    int found = 0;
    int current = 1;

    while(current < TOTAL_MEMORY && !found) {
        if(RAM[current] == 'f') {
            if (*(int *) &RAM[current + 1] > requiredSize) {
                printf("found %d\n", *(int *) &RAM[current + HEADER_SIZE]);
                found = 1;
                break;
            }
        } else {
            current += *(int *) &RAM[current + 1] + HEADER_SIZE + 1;
            printf("not found. got next position :%d\n", current);
        }
    }

    if (!found) {
        printf("Not found enough space.\n");
        return NULL;
    }

    // allocate space
    if (*(int *)(&RAM[current+1]) > requiredSize + HEADER_SIZE + 1) {
        RAM[current] = 'a';
        *(int *) &RAM[current + 1] = requiredSize ;
        RAM[current + HEADER_SIZE + 1 + requiredSize] = 'f';
        *(int *)(&RAM[current + HEADER_SIZE + 2 + requiredSize]) = TOTAL_MEMORY - requiredSize - HEADER_SIZE -1;
        printf("Allocated %ld\n", current + HEADER_SIZE + 1);

        return &RAM[current + HEADER_SIZE + 1];
    } else {
        RAM[current] = 'a';
        printf("Allocated %ld\n", current + HEADER_SIZE + 1);
        return &RAM[current + HEADER_SIZE + 1];
    }

}

int main() {
    mymalloc(2);
    // mymalloc(2);
    // mymalloc(4);
    for (int i=0; i<20; i++) {
        printf("printRAM %d %d\n", RAM[i], *(int *)&RAM[i]);
    }
    // int i = 0;
    // while(i < 1000) {
        // printf("printRAM %d\n", i);
        // i += HEADER_SIZE + *(int *) &RAM[i + 1] + 1;
    // }
    // printf("%d", *(int *) &RAM[0+HEADER_SIZE]);
    return 0;
}