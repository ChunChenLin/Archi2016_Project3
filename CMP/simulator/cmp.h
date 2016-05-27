#ifndef CMP_H
#define CMP_H

#include "global.h"

void initTLB();

void initPTE();

void initCACHE();

void initMemory();


void check_iPTE(int VPN);

void check_iTLB(int VPN);

void check_iCACHE(int PPN);

void check_iMEMOEY(int VA);

void putTLB(VPN);


void check_dPTE(int VPN);

void check_dTLB(int VPN);

void check_dCACHE(int PPN);

void check_dMEMOEY(int VA);

#endif