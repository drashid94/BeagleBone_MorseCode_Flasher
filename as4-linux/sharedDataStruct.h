//SharedDataStruct.h
#ifndef _SHARED_DATA_STRUCT_H_
#define _SHARED_DATA_STRUCT_H_
#include <stdbool.h>
#include <stdint.h>

//Shared Data Struct
typedef struct {
    // Linux Written Data
    uint16_t data[100];
    
    //PRU Written Data
    bool isLedOn;
    bool isButtonPressed;
} sharedMemStruct_t;
#endif
