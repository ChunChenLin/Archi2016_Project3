#ifndef global_h
#define global_h

#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

class Register {
public:
	static unsigned reg[32], PC, cycle;
};

class Memory {
public:
	static unsigned char DMemory[1024], IMemory[1024];
	static unsigned address, position;
};

class Instruction {
public:
	static unsigned op, rs, rt, rd, func, shamt, immediate;
};

class Terminal {
public:
	static bool halt;
	static bool write2Zero, numberOverflow, memoryOverflow, dataMisaaligned;
};

typedef struct _TLB {
    int VPN;
    int PPN;
    int last_cycle;
    int valid;
} TLB;

typedef struct _PTE {
    int PPN;
    int valid;
} PTE;

typedef struct _CACHE {
    int tag;
    int MRU;
    int valid;
} CACHE;

typedef struct _MEMORY {
    int last_cycle;
    int valid;
} MEMORY;

extern TLB *iTLB,*dTLB;
extern PTE *iPTE,*dPTE;
extern CACHE **iCACHE,**dCACHE; //2 dim
extern MEMORY *iMEMORY,*dMEMORY;

extern int iPAGE_SIZE;
extern int iDISK_SIZE;
extern int iPTE_entries;
extern int iTLB_entries;
extern int iCACHE_SIZE;
extern int iCACHE_associate;
extern int iCACHE_entries;
extern int iBLOCK_SIZE;
extern int iMEMORY_SIZE;
extern int iMEMORY_entries;
extern int iPAGE_OFFSET;

#endif
