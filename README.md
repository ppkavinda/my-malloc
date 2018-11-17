# mymalloc

a simple malloc implementation. RAM is defined as a ```char``` array.

#### RAM structure

[ ```[initalized_flag]``` ```[RAM_HEADER_BLOCK]``` ```[memory_block 1]``` ```[memory_block 2]``` ... ```[memory_block n]``` ]
 
* ```[initialized_flag]``` states wheather the RAM is initialized or not (**1 byte**)
  * ```i``` (initialized)
  * ```0``` (not initialized)
 
#### [RAM_HEADER_BLOCK] structure

Used for store the variables that used in functions. Can only store ```int```s. Considers as blocks of 5 bytes. Each block is 2 parts. ```status bit``` and ```slot``` for store data. 

byte 1 to byte 100 are reserved for this. (**100 bytes**)
 
[```[ [status bit][slot] ]``` ```[ [status bit][slot] ]``` ```[ [status bit][slot] ]``` ]

* ```[status bit]``` states wheather the block is allocated or not
  * ```a``` (allocated)
  * ```f``` (free)
* ```[slot]``` slot for store the value

#### [memory_block] structure

[```[status flag] [size] [slot]```]

* ```[status flag]``` states wheather the block is allocated or not
  * ```a``` (allocated)
  * ```f``` (free)
* ```[size]``` size of the block
* ```[slot]``` slot for store the data
