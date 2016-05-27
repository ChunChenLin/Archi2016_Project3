#include "cmp.h"

void initTLB();

void initPTE();

void initCACHE();

void initMemory();


void check_iTLB(int VPN) {
	//VPN = VA / iPAGE_SIZE; need to pass VPN in from outside!

	for(int i=0; i<iTLB_entries; i++)
    {
        if(iTLB[i].VPN==VPN && iTLB[i].valid==1)
        {
        	//TLB hit -- > goto iCACHE
            iTLB[i].last_cycle = cycle;
            iTLB_hit ++;
            check_iCACHE(iTLB[i].PPN);
        } else {
        	// TLB miss --> goto iPTE
        	iTLB_miss ++;
        	check_iPTE(VPN);
        }

    }
}

void putTLB(int VPN) {
	int min = 9999999;
    int tmp = 0;
    int PPN;
    PPN = iPT[VPN].PPN;
    for(int i=0; i<iTLB_entries; i++)
    {
        if(iTLB[i].valid==0)
        {
            tmp=i;
            break;
        }
        else
        {
            if(iTLB[i].last_cycle < min)
            {
                min = iTLB[i].last_cycle;
                tmp = i;
            }
        }
    }

    iTLB[tmp].last_cycle = cycle;
    iTLB[tmp].valid = 1;
    iTLB[tmp].PPN = PPN;
    iTLB[tmp].VPN = VPN;
}

void check_iPTE(int VPN) {
	if(IPT[VPN].valid==1) {
		// iPTE hit
        iPTE_hit ++;
        // put into iTLB
        putTLB(VPN);
	} else {
        // iPTE miss
        iPTE_miss ++;
        // goto  DISK
	}
}

void check_iCACHE(int PPN) {
    int PA = PPN * Ipage_size + IPageoffset;
    int PAB = PA / Iblock_size;
    int index = PAB % ICA_entries;
    int tag = PA / Iblock_size / ICA_entries;
    int flag=0;
    int put=0;

    if(ICA_associate==1)
    {
        if(tag==ICA[index][0].tag&&ICA[index][0].valid==1)
        {
        	iCACHE_hit ++;
        	return;
        }
    }
    else
    {
        for(int i=0; i<ICA_associate; i++)
        {
            if(tag == ICA[index][i].tag && ICA[index][i].valid == 1)
            {

                for(int j=0; j<ICA_associate; j++)
                {
                    if(ICA[index][j].MRU==0)
                    {

                        if(flag==0)
                        {
                            put =j;
                            flag=1;
                        }
                        else
                        {
                            flag=2;
                            break;
                        }
                    }
                }
                if(flag==1 && put == i)
                {
                    for(j=0; j<ICA_associate; j++)
                    {
                        ICA[index][j].MRU=0;
                    }
                }
                ICA[index][i].MRU=1;
                
                iCACHE_hit ++;
                return;
            }
        }
    }
    
    iCACHE_miss ++;
    //goto memory
    
}

void check_iMEMOEY(int VA);


void check_dTLB(int VPN);

void check_dPTE(int VPN);

void check_dCACHE(int PPN);

void check_dMEMOEY(int VA);