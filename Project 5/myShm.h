#ifndef MY_SHM_H
#define MY_SHM_H

struct CLASS
{
    int index;        // Index to the next available response slot
    int response[10]; // Array where each child writes its child number & lucky number
};

#endif
