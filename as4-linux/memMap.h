#ifndef _MEMMAP_H_
#define _MEMMAP_H_

volatile void* getPruMmapAddr(void);

void freePruMmapAddr(volatile void* pPruBase);

void writeToDataArray(unsigned short code, int index);

void memMap_set_data_ready_flag(void);

void memMap_set_data_length(int len);

#endif