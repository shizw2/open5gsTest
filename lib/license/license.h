#ifndef _LICENSE_INCLUDE_H_
#define _LICENSE_INCLUDE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "psMD5.h"
typedef enum { false, true } bool;
void dsGetSerialNumber(unsigned char *szSysInfo, int *piSystemInfoLen);
int encrypt(int num);
int decrypt(int num);
void dsMakeMachineID(void);
void dsMakeLicense(int numUsers, int timeValue);
bool isLicenseValid(int timeValue);
void dsCheckLicense(void);
#endif /* End of _LICENSE_INCLUDE_H_ */

