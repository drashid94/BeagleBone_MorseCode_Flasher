//SharedDataStruct.h
#ifndef _SHARED_DATA_STRUCT_H_
#define _SHARED_DATA_STRUCT_H_
#include <stdbool.h>
#include <stdint.h>

//Shared Data Struct
typedef struct {
    // Linux Written Data
    uint16_t morseCodeData[100];
    uint32_t morseCodeDataLength;
    bool isMorseDataReady;  
    
    //PRU Written Data
    bool isLedOn;
    bool isButtonPressed;
    bool isflashingCompleted;
} sharedMemStruct_t;
#endif
