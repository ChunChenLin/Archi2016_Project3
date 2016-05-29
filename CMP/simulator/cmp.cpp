#include "cmp.h"
#include "global.h"

void report() {
    fptr_report = fopen("report.rpt", "wb");
    fprintf( fptr_report, "ICache :\n");
    fprintf( fptr_report, "# hits: %u\n", iCACHE_hit);
    fprintf( fptr_report, "# misses: %u\n\n", iCACHE_miss);

    fprintf( fptr_report, "DCache :\n");
    fprintf( fptr_report, "# hits: %u\n", dCACHE_hit);
    fprintf( fptr_report, "# misses: %u\n\n", dCACHE_miss);

    fprintf( fptr_report, "ITLB :\n");
    fprintf( fptr_report, "# hits: %u\n", iTLB_hit);
    fprintf( fptr_report, "# misses: %u\n\n", iTLB_miss);

    fprintf( fptr_report, "DTLB :\n");
    fprintf( fptr_report, "# hits: %u\n", dTLB_hit);
    fprintf( fptr_report, "# misses: %u\n\n", dTLB_miss);

    fprintf( fptr_report, "IPageTable :\n");
    fprintf( fptr_report, "# hits: %u\n", iPTE_hit);
    fprintf( fptr_report, "# misses: %u\n\n", iPTE_miss);

    fprintf( fptr_report, "DPageTable :\n");
    fprintf( fptr_report, "# hits: %u\n", dPTE_hit);
    fprintf( fptr_report, "# misses: %u\n\n", dPTE_miss);

    fclose(fptr_report);
}

void initITLB() {
    iTLB_entries = iPTE_entries/4; // small version of PTE
    iTLB = (TLB*)malloc(iTLB_entries * sizeof(TLB));
    for(int i=0; i<iTLB_entries; i++) {
        iTLB[i].last_cycle = 0;
        iTLB[i].PPN = 0;
        iTLB[i].VPN = 0;
        iTLB[i].valid = 0;
    }
    iTLB_hit = 0;
    iTLB_miss=0;
}

void initIPTE() {
    iPTE_entries = iDISK_SIZE/iPAGE_SIZE; // big size record (Memory) VA --> PA
    iPTE = (PTE*)malloc(iPTE_entries * sizeof(PTE));
    for(int i=0; i<iPTE_entries; i++) {
        iPTE[i].PPN=0;
        iPTE[i].valid=0;
    }
    iPTE_hit=0;
    iPTE_miss=0;
}

void initICACHE() { // 2 dim because of associate 
    iCACHE_entries = iCACHE_SIZE/iCACHE_associate/4;
    iCACHE = (CACHE**)malloc(iCACHE_entries * sizeof(CACHE *));
    for(int i=0; i<iCACHE_entries; i++) {
        iCACHE[i] = (CACHE*)malloc(iCACHE_associate * sizeof(CACHE));
    }
    for(int i=0; i<iCACHE_entries; i++) {
        for(int j=0; j<iCACHE_associate; j++) {
            iCACHE[i][j].MRU = 0;
            iCACHE[i][j].tag = 0;
            iCACHE[i][j].valid = 0;
        }
    }
    iCACHE_hit=0;
    iCACHE_miss=0;
}

void initIMEMORY()
{
    iMEMORY_entries = iMEMORY_SIZE/iPAGE_SIZE; 
    iMEMORY = (MEMORY*)malloc(iMEMORY_entries * sizeof(MEMORY));   
    for(int i=0; i<iMEMORY_entries; i++) {
        iMEMORY[i].last_cycle = 0;
        iMEMORY[i].valid = 0;
    }   
}

int findITLB(int VPN)
{
    for(int i=0; i<iTLB_entries; i++) {
        if(iTLB[i].VPN==VPN && iTLB[i].valid==1) {
            iTLB[i].last_cycle = Register::cycle;
            return iTLB[i].PPN;
        }

    }
    return -1;
}

int findIPTE(int VPN)
{
    if(iPTE[VPN].valid==1) return iPTE[VPN].PPN;
    else return -1;
}

void IPTEmiss(int VPN)
{
    /////////////////SWAP////////////////////
    int PPN=0;
    int min=999999;
    int flag=0;
    for(int i=0; i<iMEMORY_entries; i++)
    {
        if(iMEMORY[i].valid==0) {
            PPN = i;
            flag = 1;
            break;
        } else {
            if(iMEMORY[i].last_cycle < min) {
                min = iMEMORY[i].last_cycle;
                PPN = i;
            }
        }
    }
    iMEMORY[PPN].last_cycle = Register::cycle;
    iMEMORY[PPN].valid = 1;

    /////////////////UPDATE PTE//////////////////
    if(flag==1) {
        iPTE[VPN].PPN = PPN;
        iPTE[VPN].valid = 1;
    } else {
        for(int i=0; i<iPTE_entries; i++) {
            if(iPTE[i].PPN==PPN) {
                iPTE[i].valid = 0;
            }
        }
        iPTE[VPN].PPN = PPN;
        iPTE[VPN].valid = 1;

        for(int i=0; i<iTLB_entries; i++) {
            if(iTLB[i].PPN==PPN) {
                iTLB[i].valid=0;
            }
        }
        for(int j=0; j<iPAGE_SIZE; j+=4) {
            int PA = PPN * iPAGE_SIZE + j;
            int PAB = PA / iBLOCK_SIZE;
            int index = PAB % iCACHE_entries;
            int tag = PA / iBLOCK_SIZE / iCACHE_entries;
            if(iCACHE_associate==1) {
                if(iCACHE[index][0].tag==tag) {
                    iCACHE[index][0].valid=0;
                }
            } else {
                for(int i=0; i<iCACHE_associate; i++) {
                    if(iCACHE[index][i].tag==tag) {
                        iCACHE[index][i].valid=0;
                        iCACHE[index][i].MRU=0;
                    }
                }
            }
        }
    }
}

void updateITLB(int VPN)
{
    int min = 999999;
    int temp = 0;
    int PPN;
    PPN = iPTE[VPN].PPN;
    for(int i=0; i<iTLB_entries; i++) {
        if(iTLB[i].valid==0) {
            temp=i;
            break;
        } else {
            if(iTLB[i].last_cycle<min) {
                min=iTLB[i].last_cycle;
                temp=i;
            }
        }
    }

    iTLB[temp].last_cycle=Register::cycle;
    iTLB[temp].valid=1;
    iTLB[temp].PPN=PPN;
    iTLB[temp].VPN=VPN;
}
bool findICACHE(int PPN)
{
    int PA = PPN * iPAGE_SIZE + iPAGE_OFFSET;
    int PAB = PA / iBLOCK_SIZE;
    int index = PAB % iCACHE_entries;
    int tag = PA / iBLOCK_SIZE / iCACHE_entries;
    int flag=0;
    int put=0;

    if(iCACHE_associate==1) {
        if(tag==iCACHE[index][0].tag&&iCACHE[index][0].valid==1) {
            return true;
        }
    }
    else {
        for(int i=0; i<iCACHE_associate; i++) {
            if(tag == iCACHE[index][i].tag && iCACHE[index][i].valid == 1) {
                for(int j=0; j<iCACHE_associate; j++) {
                    if(iCACHE[index][j].MRU==0) {
                        if(flag==0) {
                            put =j;
                            flag=1;
                        } else {
                            flag=2;
                            break;
                        }
                    }
                }
                if(flag==1 && put == i) {
                    for(int j=0; j<iCACHE_associate; j++) {
                        iCACHE[index][j].MRU=0;
                    }
                }
                iCACHE[index][i].MRU=1;
                return true;
            }
        }
    }
    return false;
}

void updateCACHE(int PPN)
{
    int PA = PPN * iPAGE_SIZE + iPAGE_OFFSET;
    int PAB = PA / iBLOCK_SIZE;
    int index = PAB % iCACHE_entries;
    int tag = (PA / iBLOCK_SIZE) / iCACHE_entries;
    int flag = 0;
    int put = 0;

    if(iCACHE_associate==1) //direct-mapped
    {
        iCACHE[index][0].MRU=0;
        iCACHE[index][0].tag = tag;
        iCACHE[index][0].valid = 1;
    }
    else
    {
        for(int i=0; i<iCACHE_associate; i++)
        {
            if(iCACHE[index][i].MRU==0)
            {

                if(flag==0)
                {
                    put = i;
                    flag=1;
                }
                else
                {
                    flag=2;
                    break;
                }
            }
        }
        if(flag==1)
        {
            for(int i=0; i<iCACHE_associate; i++)
            {
                iCACHE[index][i].MRU=0;
            }
        }
        iCACHE[index][put].MRU=1;
        iCACHE[index][put].tag=tag;
        iCACHE[index][put].valid=1;
    }
    iMEMORY[PPN].last_cycle = Register::cycle;

}

void checkIMEMORY(int VA) {
    IVPN = VA / iPAGE_SIZE;
    iPAGE_OFFSET = VA % iPAGE_SIZE;
    IPPN = findITLB(IVPN); // VA -> PA

    if(IPPN!=-1) { //TLB hit
        iTLB_hit++;
        //go check CACHE
        //IPPN = findITLB(IVPN);
        if(findICACHE(IPPN)) { // CASE:1
            //CAHCE hit
            iCACHE_hit++;
        } else {               // CASE:2
            //CACHE miss
            iCACHE_miss++;
            // go search memory --> update cache
            updateCACHE(IPPN);
        }
    } else { //TLB miss
        iTLB_miss++;
        //go check PTE
        //IPPN = findIPTE(IVPN);
        if(IPPN!=-1) { //PTE hit
            iPTE_hit++;
            //update TLB
            updateITLB(IVPN);
            //go check CACHE
            IPPN = findITLB(IVPN);
            if(findICACHE(IPPN)) {   //CASE:3
                //CACHE hit
                iCACHE_hit++;
            } else {           //CASE:4
                //CAHCE miss
                iCACHE_miss++;
                // go search memory --> update cache
                updateCACHE(IPPN);
            }

        } else { //PTE miss   //CASE:5 the worst one
            iPTE_miss++;
            // SWAP --> update PTE --> update TLB
            IPTEmiss(IVPN);
            updateITLB(IVPN);

            //CACHE must miss
            IPPN = findITLB(IVPN);      
            if(!findICACHE(IPPN)) {
                iCACHE_miss++;
                // go update CACHE
                updateCACHE(IPPN);
            }
        }
    }

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void initDTLB() {
    dTLB_entries = dPTE_entries/4;
    dTLB = (TLB*)malloc(dTLB_entries * sizeof(TLB));
    for(int i=0; i<dTLB_entries; i++) {
        dTLB[i].last_cycle=0;
        dTLB[i].PPN=0;
        dTLB[i].VPN=0;
        dTLB[i].valid=0;
    }
    dTLB_hit=0;
    dTLB_miss=0;
}

void initDPTE() {
    dPTE_entries = dDISK_SIZE/dPAGE_SIZE;
    dPTE = (PTE*)malloc(dPTE_entries * sizeof(PTE));
    for(int i=0; i<dPTE_entries; i++) {
        dPTE[i].PPN=0;
        dPTE[i].valid=0;
    }
    dPTE_hit=0;
    dPTE_miss=0;
}

void initDCACHE() {
    dCACHE_entries = dCACHE_SIZE/dCACHE_associate/4;
    dCACHE = (CACHE**)malloc(dCACHE_entries * sizeof(CACHE *));
    for(int i=0; i<dCACHE_entries; i++) {
        dCACHE[i] = (CACHE*)malloc(dCACHE_associate * sizeof(CACHE));
    }
    for(int i=0; i<dCACHE_entries; i++) {
        for(int j=0; j<dCACHE_associate; j++) {
            dCACHE[i][j].MRU=0;
            dCACHE[i][j].tag=0;
            dCACHE[i][j].valid=0;
        }
    }
    dCACHE_hit=0;
    dCACHE_miss=0;
}
void initDMEMORY()
{ 
    dMEMORY_entries = dMEMORY_SIZE/dPAGE_SIZE;
    dMEMORY = (MEMORY*)malloc(dMEMORY_entries * sizeof(MEMORY)); 
    for(int i=0; i<dMEMORY_entries; i++) {
        dMEMORY[i].last_cycle=0;
        dMEMORY[i].valid=0;
    }
}

int findDTLB(int VPN)
{
    for(int i=0; i<dTLB_entries; i++)
    {
        if(dTLB[i].VPN==VPN && dTLB[i].valid==1)
        {
            dTLB[i].last_cycle=Register::cycle;
            return dTLB[i].PPN;
        }

    }
    return -1;
}

int findDPTE(int VPN)
{
    if(dPTE[VPN].valid==1)
        return dPTE[VPN].PPN;
    else
        return -1;
}

void DPTEmiss(int VPN)
{
    /////////////////SWAP////////////////////
    int PPN=0;
    int min=999999;
    int flag=0;
    for(int i=0; i<dMEMORY_entries; i++)
    {
        if(dMEMORY[i].valid==0)
        {
            PPN=i;
            flag=1;
            break;
        }
        else
        {
            if(dMEMORY[i].last_cycle<min)
            {
                min=dMEMORY[i].last_cycle;
                PPN=i;
            }
        }
    }

    dMEMORY[PPN].last_cycle=Register::cycle;
    dMEMORY[PPN].valid=1;
    /////////////////UPDATE PT//////////////////
    if(flag==1)
    {

        dPTE[VPN].PPN=PPN;
        dPTE[VPN].valid=1;
    }
    else
    {
        for(int i=0; i<dPTE_entries; i++)
        {
            if(dPTE[i].PPN==PPN)
            {
                dPTE[i].valid=0;
            }
        }
        dPTE[VPN].PPN=PPN;
        dPTE[VPN].valid=1;
        for(int i=0; i<dTLB_entries; i++)
        {
            if(dTLB[i].PPN==PPN)
            {
                dTLB[i].valid=0;
            }
        }

        for(int j=0; j<dPAGE_SIZE; j+=4)
        {
            int PA = PPN * dPAGE_SIZE + j;
            int PAB = PA / dBLOCK_SIZE;
            int index = PAB % dCACHE_entries;
            int tag = PA / dBLOCK_SIZE / dCACHE_entries;
            if(dCACHE_associate==1)
            {
                if(dCACHE[index][0].tag==tag)
                {
                    dCACHE[index][0].valid=0;
                }

            }
            else
            {
                for(int i=0; i<dCACHE_associate; i++)
                {

                    if(dCACHE[index][i].tag==tag)
                    {
                        dCACHE[index][i].valid=0;
                        dCACHE[index][i].MRU=0;
                    }
                }
            }
        }


    }


    ////////////////UPDATE TLB//////////////////
    min=999999;
    int temp=0;
    for(int i=0; i<dTLB_entries; i++)
    {
        if(dTLB[i].valid==0)
        {
            temp=i;
            break;
        }
        else
        {
            if(dTLB[i].last_cycle<min)
            {
                min=dTLB[i].last_cycle;
                temp=i;
            }
        }
    }

    dTLB[temp].last_cycle=Register::cycle;
    dTLB[temp].valid=1;
    dTLB[temp].PPN=PPN;
    dTLB[temp].VPN=VPN;

}

void DTLBmiss(int VPN)
{
    int min=999999;
    int temp=0;
    int PPN;
    PPN = dPTE[VPN].PPN;
    for(int i=0; i<dTLB_entries; i++)
    {
        if(dTLB[i].valid==0)
        {
            temp=i;
            break;
        }
        else
        {
            if(dTLB[i].last_cycle<min)
            {
                min=dTLB[i].last_cycle;
                temp=i;
            }
        }
    }

    dTLB[temp].last_cycle=Register::cycle;
    dTLB[temp].valid=1;
    dTLB[temp].PPN=PPN;
    dTLB[temp].VPN=VPN;
}
int findDCACHE(int PPN)
{
    int PA = PPN * dPAGE_SIZE + dPAGE_OFFSET;
    int PAB = PA / dBLOCK_SIZE;
    int index = PAB % dCACHE_entries;
    int tag = PA / dBLOCK_SIZE / dCACHE_entries;
    int flag=0;
    int put;
    if(dCACHE_associate==1)
    {
        if(tag==dCACHE[index][0].tag&&dCACHE[index][0].valid==1)
        {
            return 1;
        }

    }
    else
    {

        for(int i=0; i<dCACHE_associate; i++)
        {
            if(tag == dCACHE[index][i].tag && dCACHE[index][i].valid == 1)
            {
                for(int j=0; j<dCACHE_associate; j++)
                {
                    if(dCACHE[index][j].MRU==0)
                    {

                        if(flag==0)
                        {
                            put = j;
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
                    for(int j=0; j<dCACHE_associate; j++)
                    {
                        dCACHE[index][j].MRU=0;
                    }
                }
                dCACHE[index][i].MRU=1;
                return 1;
            }
        }
    }
    return -1;
}

void DCACHEmiss(int PPN)
{
    int PA = PPN * dPAGE_SIZE + dPAGE_OFFSET;
    int PAB = PA / dBLOCK_SIZE;
    int index =PAB % dCACHE_entries;
    int tag = PA / dBLOCK_SIZE / dCACHE_entries;
    int flag=0;
    int put;
    if(dCACHE_associate==1)
    {
        dCACHE[index][0].MRU=0;
        dCACHE[index][0].tag=tag;
        dCACHE[index][0].valid=1;
    }
    else
    {
        for(int i=0; i<dCACHE_associate; i++)
        {
            if(dCACHE[index][i].MRU==0)
            {

                if(flag==0)
                {
                    put = i;
                    flag=1;
                }
                else
                {
                    flag=2;
                    break;
                }
            }
        }
        if(flag==1)
        {
            for(int i=0; i<dCACHE_associate; i++)
            {
                dCACHE[index][i].MRU=0;
            }
        }
        dCACHE[index][put].MRU=1;
        dCACHE[index][put].tag=tag;
        dCACHE[index][put].valid=1;
    }

    dMEMORY[PPN].last_cycle=Register::cycle;

}

void checkDMEMORY(int VA)
{
    DVPN = VA / dPAGE_SIZE;

    dPAGE_OFFSET = VA % dPAGE_SIZE;
    DPPN = findDTLB(DVPN);


    if(DPPN==-1)           ///TLB miss
    {
        dTLB_miss++;
        DPPN=findDPTE(DVPN);

        if(DPPN==-1)      ///PT miss
        {
            dPTE_miss++;
            DPTEmiss(DVPN);
        }
        else            ///PT hit
        {
            dPTE_hit++;
            DTLBmiss(DVPN);
        }

    }
    else               ///TLB hit
    {
        dTLB_hit++;
    }
    int find=0;
    DPPN = findDTLB(DVPN);
    find = findDCACHE(DPPN);

    if(find==-1)        ///CA miss
    {
        dCACHE_miss++;
        DCACHEmiss(DPPN);
    }
    else                ///CA hit
    {
        dCACHE_hit++;
    }
}
