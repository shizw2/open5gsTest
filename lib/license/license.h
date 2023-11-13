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

void dsGetSerialNumber(unsigned char *szSysInfo, int *piSystemInfoLen);
int encrypt(int num);
int decrypt(int num);
long encrypt_long(long num);
long decrypt_long(long num);

void dsMakeMachineID(void);
void dsMakeLicense(int numUsers, long expireTimestamp, long durationTimestamp);
bool isLicenseExpired(void);
bool dsCheckLicense(void);
int getLicenseUeNum(void);

void saveRunningTimeToFile(const char* filePath);
void saveRunningTimeToFiles(void);
void loadRunningTimeFromFile(void);

#endif /* End of _LICENSE_INCLUDE_H_ */

