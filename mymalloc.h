#include <stdio.h>

#define TOTAL_RAM 25000
#define RAM_HEADER_SIZE 30
#define BLOCK_HEADER_SIZE sizeof(TOTAL_RAM)
#define HEADER_START 1
#define HEADER_END HEADER_START + RAM_HEADER_SIZE
#define RAM_BODY_START HEADER_END

#define enableMessages 0

static char RAM[TOTAL_RAM];

// prototype
int delVar(int *);
int *defVar(int);
void *bestFit(int);
void *MyMalloc(int);
void initRAM();
void printRAM();
void MyFree(void *);
void mergeFreeBlocks(int *, int *);

/**
 * initialize ram if not initialized
*/
void initRAM() {
    if (RAM[0] == 0) {      // not initialized
        if (enableMessages) printf("Initilizing RAM\n");

        RAM[0] = 'i';       // i for initilized
        RAM[RAM_BODY_START] = 'f';                      // state very first block is free
        * (int *) &RAM[RAM_BODY_START + 1] = TOTAL_RAM - RAM_HEADER_SIZE - 1 ;      // write the size of free block
    }
}

/**
 * allocate a slot from RAM_HEADER for a variable
 * RAM_HEADER structure: [status bit][slot 1:[status bit][slot]][slot 2:[status bit][slot]]
 * i 00000 00000 000...00
 * i a2032 a2342 000...00
*/
int *defVar(int value) {
    char *pointer = &RAM[HEADER_START];

    while (pointer < (char *) &RAM[RAM_BODY_START]) {
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
    int *currentPosition = defVar(RAM_BODY_START);
    int *bestPosition = defVar(*currentPosition);
    int *previousLeftSpace = defVar(TOTAL_RAM);
    int *found = defVar(0);
    int *currentBlockSize = defVar(*(int *) &RAM[*currentPosition + 1]);

    if (enableMessages) printf("\nSearching for %d bytes\n", requiredSize);

    while (*currentPosition < TOTAL_RAM) {
        if (enableMessages) 
            printf("searching...%d %p size: %d\n", 
                *currentPosition, &RAM[*currentPosition], *currentBlockSize);
        
        *currentBlockSize = *(int *) &RAM[*currentPosition + 1];

        if (*currentBlockSize >= requiredSize) {       // found enough space
            if (RAM[*currentPosition] == 'f') {
                int leftSpace = *currentBlockSize - requiredSize;
                *found = 1;

                if (leftSpace < *previousLeftSpace) {                // found a better location
                    *previousLeftSpace = *currentBlockSize;
                    *bestPosition = *currentPosition;
                }
            }
        }

        *currentPosition += *(int *) &RAM[*currentPosition + 1] + BLOCK_HEADER_SIZE + 1;
    }

    delVar(currentPosition);
    delVar(previousLeftSpace);
    delVar(currentBlockSize);

    if (delVar(found)) {
        if (enableMessages) printf("Found a location @ %p\n", &RAM[*bestPosition]);
        return &RAM[delVar(bestPosition)];
    }

    if (enableMessages) printf ("ERROR! not enough space!!\n");
    return NULL;
}

/**
 * actuall MyMalloc
*/
void * MyMalloc(int requiredSize) {
    initRAM();      // initialize RAM if not already

    char *allocPosition = (char *) bestFit(requiredSize);
    if (allocPosition == NULL) return NULL;         // EXITING :: not found suitable location

    // allocating
    int *completeBlockSize = defVar(*(int *)(allocPosition + 1));
    *(char *)allocPosition = 'a';                   // change flag to 'allocated'

    if (*completeBlockSize - requiredSize >  BLOCK_HEADER_SIZE + 1) {               // the block has spare space to allocate a new block
        *(int *)(allocPosition + 1) = requiredSize;    // store the size of block
        *(char *)(allocPosition + 1 + BLOCK_HEADER_SIZE + requiredSize) = 'f';           // allocate left space as a free block
        *(int *)(allocPosition + 1 + BLOCK_HEADER_SIZE +  requiredSize + 1) = 

        delVar(completeBlockSize) - requiredSize - BLOCK_HEADER_SIZE - 1;        // store left space's (block) size

    }
    
    return allocPosition + BLOCK_HEADER_SIZE + 1;
}

/**
 * merge free blocks (previous, current & next) of memory 
 * given the currentLocation and previousLocation of the RAM
*/ 
void mergeFreeBlocks(int *currentLocation, int *previousLocation) {
    int *nextLocation = defVar(*currentLocation + *(int *) &RAM[*currentLocation + 1] + BLOCK_HEADER_SIZE + 1);

    *(char *)&RAM[*currentLocation] = 'f';        // define the block as free

    if (RAM[*nextLocation] == 'f') {                                                    // if next block is free
        *(int *) &(RAM[*currentLocation + 1]) += *(int *) &RAM[*nextLocation + 1];      // merge it with the current block
    }

    if (RAM[*previousLocation]  == 'f' && *currentLocation != *previousLocation) {      // if previous block is free
        *(int *) &(RAM[*previousLocation + 1]) += *(int *) &RAM[*currentLocation + 1];  // merge current block with the previous one
    }

    delVar(nextLocation);
}

/**
 * actual MyFree
 * find the targetLocation & define it as free
 * if adjecent blocks are free merge them
 */
void MyFree(void *targetLocation) {
    int *currentLocation = defVar(RAM_BODY_START);
    int *previousLocation = defVar(RAM_BODY_START);

    while (*currentLocation < TOTAL_RAM) {
        if (((char *) &RAM[*currentLocation]) + 1 + BLOCK_HEADER_SIZE == targetLocation) {
            mergeFreeBlocks(currentLocation, previousLocation);
            break;
        }

        *previousLocation = *currentLocation;
        *currentLocation += *(int *) &RAM[*currentLocation + 1] + BLOCK_HEADER_SIZE + 1;
    }
    delVar(currentLocation);
    delVar(previousLocation);
}

/**
 * just for print RAM array byte by byte
 * 
*/
void testprint() {
    for(int i = 00; i < 33; i++) {
        if (i%100 == 0) printf("\n");

        printf("%p : ram@%d: %d\tletter: %c\tsize: %d\n", 
            &RAM[i], i, RAM[i], RAM[i], 
            (int) *(int *) &RAM[i]);
    }

    printf("\n");
}

/**
 * print RAM pretty
*/
void printRAM() {
    int current = 1;
    int toggleHeader = 0;

    if (toggleHeader) {
        printf("\n+---------------------------------%s-----------------------------------+\n", 
            (RAM[0] == 'i') ? "INITIALIZED" : "-----------");

        for (;current < RAM_HEADER_SIZE; current+=5) {
            
            printf("|\t%p @ RAM[%.5d] : value: %.5d :  status: %9s\t\t|\n", 
                &RAM[current+1], current, 
                *(int *)&RAM[current+1], 
                (RAM[current] == 'A' ? "ALLOCATED" : "FREE")
            );

        printf("+-------------------------------------------------------------------------------+\n");
        }

        printf("+---------------------------------RAM CONTENT-----------------------------------+");
    }

    printf("\n+-------------------------------------------------------------------------------+\n");

    current = RAM_BODY_START;

    while (current < TOTAL_RAM) {

        printf("|\t%p @ RAM[%.5d] : size: %.5d :  status: %9s  \t|", 
            &RAM[current], current, 
            *(int *)&RAM[current + 1], 
            (RAM[current] == 'f' ? "FREE" : "ALLOCATED")
        );

        printf("\n+-------------------------------------------------------------------------------+\n");
        
        current += *(int *)&RAM[current+1] + BLOCK_HEADER_SIZE + 1;
    }
}
