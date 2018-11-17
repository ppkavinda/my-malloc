#include <stdio.h>

#define TOTAL_RAM 25000
#define RAM_HEADER_SIZE 100
#define BLOCK_HEADER_SIZE sizeof(TOTAL_RAM)
#define HEADER_START 1
#define HEADER_END HEADER_START + RAM_HEADER_SIZE
#define RAM_BODY_START HEADER_END

static char RAM[TOTAL_RAM];

// prototype
void *defVar(int);
void *mymalloc(int);
void printRAM();
void initRAM();
void *bestFit(int);
int delVar(int *);

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
}

/**
 * allocate a slot from RAM_HEADER for a variable
 * RAM_HEADER structure: [status bit][slot 1:[status bit][slot]][slot 2:[status bit][slot]]
 * i 00000 00000 000...00
 * i a2032 a2342 000...00
*/
void *defVar(int value) {
    void *pointer = &RAM[HEADER_START];

    while (pointer < (void *) &RAM[RAM_BODY_START]) {
        if (*(char *)pointer != 'a') {
            *(char *)pointer = 'a';
            *(int *)((char *)pointer + 1) = value;
            return (int *)((char *)pointer + 1);
        }

        pointer += BLOCK_HEADER_SIZE + 1;
    }
    return NULL;
}

/**
 * release a slot from RAM_HEADER 
 */
int delVar(int * pointer) {
    *((char *)pointer - 1) = 'f';
    return *pointer;
}

/**
 * bestFit algorithm for find the best location to allocate memory
*/
void *bestFit(int requiredSize) {
    printRAM();
    int *currentPosition = defVar(RAM_BODY_START);
    int *bestPosition = defVar(*currentPosition);
    int *previousLeftSpace = defVar(TOTAL_RAM);
    int *found = defVar(0);

    printf("Finding location for %d bytes\n", requiredSize);

    while (*currentPosition < TOTAL_RAM) {
        int currentBlockSize = *(int *) &RAM[*currentPosition + 1];

        if (currentBlockSize > requiredSize) {       // *found enough space
            if (RAM[*currentPosition] == 'f') {
                int leftSpace = currentBlockSize - requiredSize;
                *found = 1;

                if (leftSpace < *previousLeftSpace) {                // *found a better location
                    *previousLeftSpace = currentBlockSize;
                    *bestPosition = *currentPosition;
                }
            }
        }

        *currentPosition += *(int *) &RAM[*currentPosition + 1] + BLOCK_HEADER_SIZE + 1;
    }

    delVar(currentPosition);
    delVar(previousLeftSpace);

    if (delVar(found)) {
        printf("Found a location @ %p\n", &RAM[*bestPosition]);
        return &RAM[delVar(bestPosition)];
    }

    printf ("ERROR! not enough space!!\n");
    return NULL;
}

/**
 * actuall mymalloc
*/
void * mymalloc(int requiredSize) {
    initRAM();      // initialize RAM if not already

    char *allocPosition = bestFit(requiredSize);
    if (allocPosition == NULL) return NULL;         // EXITING :: not *found suitable location

    // allocating
    int *freeBlockSize = defVar(*(int *)(allocPosition + 1));
    *(char *)allocPosition = 'a';                   // change flag to 'allocated'
    *(int *)(allocPosition + 1) = requiredSize;    // store the size of block
    *(char *)(allocPosition + 1 + BLOCK_HEADER_SIZE + requiredSize) = 'f';         // allocate left space as a free block
    *(int *)(allocPosition + 1 + BLOCK_HEADER_SIZE +  requiredSize + 1) = delVar(freeBlockSize) - requiredSize;        // store left space's (block) size

    return allocPosition + BLOCK_HEADER_SIZE + 1;
}

/**
 * just for print RAM array
 * 
*/
void printRAM() {
    for(int i = 0; i < 20; i++) {
        if (i%100 == 0) printf("\n");

        printf("%p : ram@%d: %d\tletter: %c\tsize: %d\n", &RAM[i], i, RAM[i], RAM[i], (int) *(int *) &RAM[i]);

    }

    printf("\n");
}

int main(int argc, char const *argv[])
{
    mymalloc(10);
    mymalloc(3);
    for (int i=0; i<50; i++){
        mymalloc(3);
    }
    mymalloc(5);
    mymalloc(5);

    printRAM(30);
    
    return 0;
}
