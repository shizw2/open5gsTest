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
    int  numUsers;
    long licenseExpireTime;
    long licenseDuration;
    unsigned char   szDigestFromFile[16];
}PACK_1 license_info_t;


void dsMakeMachineID(void);
void dsMakeLicense(int numUsers, long expireTimestamp, long durationTimestamp);
bool isLicenseExpired(long runTime);
bool dsCheckLicense(void);
int getLicenseUeNum(void);



#endif /* End of _LICENSE_INCLUDE_H_ */

