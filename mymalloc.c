#include <stdio.h>

#define TOTAL_RAM 2500
#define RAM_HEADER_SIZE 100
#define BLOCK_HEADER_SIZE sizeof(TOTAL_RAM)
#define HEADER_START 1
#define HEADER_END HEADER_START + RAM_HEADER_SIZE
#define RAM_BODY_START HEADER_END

static char RAM[TOTAL_RAM];

// prototype
// void *allocVar(int);
// void *mymalloc(int);
// void printRAM();
// void initRAM();
// void *bestFit(int);

/**
 * initialize ram if not initialized
*/
void initRAM() {
    
    if (RAM[0] == 0) {      // not initialized
        printf("Initilizing RAM\n");

        RAM[0] = 'i';       // i for initilized
        RAM[RAM_BODY_START] = 'f';                      // state very first block is free
        * (int *) &RAM[RAM_BODY_START + 1] = TOTAL_RAM - RAM_HEADER_SIZE - 1 ;      // state the size of free block
    }

    // RAM[RAM_BODY_START] = 'a';
    // * (int *) &RAM[RAM_BODY_START + 1] = 10;
    
    // RAM[RAM_BODY_START+BLOCK_HEADER_SIZE+11] = 'f';
    // * (int *) &RAM[RAM_BODY_START+BLOCK_HEADER_SIZE+12] = 3000;
}

/**
 * allocate a variable from RAM array
*/
void *allocVar(int size) {
}

/**
 * bestFit algorithm for find the best location to allocate memory
*/
void *bestFit(int requiredSize) {
    int currentPosition = RAM_BODY_START;
    int bestPosition = currentPosition;
    int previousLeftSpace = TOTAL_RAM;
    int found = 0;
int i = 0;
    while (currentPosition < TOTAL_RAM && i<10) {
        i++;
        int currentBlockSize = *(int *) &RAM[currentPosition + 1];
        printf("finding location, current@ram %d %p status:%c\n", currentPosition, &RAM[currentPosition], RAM[currentPosition]);

        if (currentBlockSize > requiredSize) {       // found enough space
            if (RAM[currentPosition] == 'f') {
                printf("found a location@ram %d %p\n", currentPosition, &RAM[currentPosition]);
                int leftSpace = currentBlockSize - requiredSize;
                found = 1;

                if (leftSpace < previousLeftSpace) {                // found a better location
                    printf("found a better location@ram %d %p leftSpace: %d\n", currentPosition, &RAM[currentPosition] ,leftSpace);
                    previousLeftSpace = currentBlockSize;
                    bestPosition = currentPosition;
                }
            } else {
                printf("Not a hole %c", RAM[currentPosition]);
            }

        }

        printf("currentPosition %d\n", currentPosition);
        printf("currentPosition %c\n", RAM[currentPosition]);
        
        currentPosition += *(int *) &RAM[currentPosition + 1] + BLOCK_HEADER_SIZE + 1;

    }

    if (found) return &RAM[bestPosition];
    return NULL;
}

/**
 * actuall mymalloc
*/
void * mymalloc(int requiredSize) {
    initRAM();      // initialize RAM if not already

    char *allocPosition = bestFit(requiredSize);
    if (allocPosition == NULL) return NULL;

    printf("hole %c %p\n", *allocPosition, allocPosition);

    // allocating
    int freeBlockSize = *(int *)(allocPosition + 1);
    // printf("freeblcksize %d", *(int *)(allocPosition + 1));
    *(char *)allocPosition = 'a';                   // change flag to 'allocated'
    *(int *)(allocPosition + 1) = requiredSize;    // store the size of block
    *(char *)(allocPosition + 1 + BLOCK_HEADER_SIZE + requiredSize) = 'f';         // allocate left space as a free block
    *(int *)(allocPosition + 1 + BLOCK_HEADER_SIZE +  requiredSize + 1) = freeBlockSize - requiredSize;        // store left space's (block) size

    printf("%s\n", allocPosition + 1 + (int) BLOCK_HEADER_SIZE);
    // printRAM();
    return allocPosition + 1 + (int) BLOCK_HEADER_SIZE;
}

/**
 * just for print RAM array
 * 
*/
void printRAM() {
    for(int i = 100; i < 160; i++) {
        if (i%100 == 0) printf("\n");

        printf("%p : ram@%d: %d\tletter: %c\tsize: %d\n", &RAM[i], i, RAM[i], RAM[i], (int) *(int *) &RAM[i]);

    }

    printf("\n");
}

int main(int argc, char const *argv[])
{
    mymalloc(10);
    mymalloc(3);
    mymalloc(3);
    mymalloc(5);
    mymalloc(5);
    printRAM();
    
    return 0;
}
