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


void dsMakeMachineID(void);
bool dsCheckLicense(char* errorMsg, size_t errorMsgSize);
bool isLicenseExpired(long runTime);
int  getLicenseUeNum(void);
long getLicenseRunTime(void);
long getLicenseExpireTime(void);
long getLicenseDurationTime(void);

#endif /* End of _LICENSE_INCLUDE_H_ */

