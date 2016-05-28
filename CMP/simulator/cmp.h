#ifndef CMP_H
#define CMP_H

#include <stdio.h>

FILE *fptr_report;

int findITLB(int VPN);

int findIPTE(int VPN);

void IPTEmiss(int VPN);

void ITLBmiss(int VPN);

int findICACHE(int PPN);

void ICACHEmiss(int PPN);

int findDPTE(int VPN);

void DPTEmiss(int VPN);

void DTLBmiss(int VPN);

int findDCACHE(int PPN);

void DCACHEmiss(int PPN);

void checkDMEMORY(int VA);

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

TLB *iTLB,*dTLB;
PTE *iPTE,*dPTE;
CACHE **iCACHE,**dCACHE; //2 dim
MEMORY *iMEMORY,*dMEMORY;

int iPAGE_SIZE;
int iDISK_SIZE;
int iPTE_entries;
int iTLB_entries;
int iCACHE_SIZE;
int iCACHE_associate;
int iCACHE_entries;
int iBLOCK_SIZE;
int iMEMORY_SIZE;
int iMEMORY_entries;
int iPAGE_OFFSET;

int dPAGE_SIZE;
int dDISK_SIZE;
int dPTE_entries;
int dTLB_entries;
int dCACHE_SIZE;
int dCACHE_associate;
int dCACHE_entries;
int dBLOCK_SIZE;
int dMEMORY_SIZE;
int dMEMORY_entries;
int dPAGE_OFFSET;

int iTLB_hit;
int iTLB_miss;
int iPTE_hit;
int iPTE_miss;
int iCACHE_hit;
int iCACHE_miss;

int dTLB_hit;
int dTLB_miss;
int dPTE_hit;
int dPTE_miss;
int dCACHE_hit;
int dCACHE_miss;

int IVPN;
int IPPN;

int DVPN;
int DPPN;

#endif
