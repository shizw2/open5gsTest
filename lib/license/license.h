#ifndef _LICENSE_INCLUDE_H_
#define _LICENSE_INCLUDE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "psMD5.h"
#ifndef bool
#define bool int
#endif
#define true 1
#define false 0
#define MAX_SYS_INFO_LENGTH 4096
typedef struct license_info_s{
    BYTE   szSystemInfoFromFile[MAX_SYS_INFO_LENGTH];
    int  maxUserNum;
    long licenseExpireTime;
    long licenseDuration;
    long licenseCreateTime;
    unsigned char   szDigestFromFile[16];
}PACK_1 license_info_t;

typedef struct runtime_info_s{    
    long licenseCreateTime;
    long totalRunningTime;
}PACK_1 runtime_info_t;

long encrypt_long(long num);
long decrypt_long(long num);

char* timestampToString(time_t timestamp);
void dsMakeMachineID(void);
bool dsCheckLicense(char* errorMsg, size_t errorMsgSize);
bool isLicenseExpired(long runTime);
int  getLicenseUeNum(void);
long getLicenseRunTime(void);
long getLicenseExpireTime(void);
long getLicenseDurationTime(void);
long getLicenseCreateTime(void);
#endif /* End of _LICENSE_INCLUDE_H_ */

