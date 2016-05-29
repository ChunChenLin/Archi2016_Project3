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

void report();

//void initICMP();
void initITLB();
void initIPTE();
void initICACHE();
void initIMEMORY();

void checkIMEMORY(int VA);
void checkDMEMORY(int VA);

//void initDCMP();
void initDTLB();
void initDPTE();
void initDCACHE();
void initDMEMORY();

extern int iDISK_SIZE;
extern int iMEMORY_SIZE;
extern int iPAGE_SIZE;
extern int iCACHE_SIZE;
extern int iBLOCK_SIZE;
extern int iCACHE_associate;

extern int dDISK_SIZE;
extern int dMEMORY_SIZE;
extern int dPAGE_SIZE;
extern int dCACHE_SIZE;
extern int dBLOCK_SIZE;
extern int dCACHE_associate;

#endif
