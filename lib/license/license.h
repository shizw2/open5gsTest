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
#define MAX_STR_LEN 100
typedef struct license_info_s{
    BYTE   szSystemInfoFromFile[MAX_SYS_INFO_LENGTH];
    int  maxUserNum;
    long licenseExpireTime;
    long licenseDuration;
    long licenseCreateTime;
    unsigned char   szDigestFromFile[16];
    int  maxSubscriptions;
    int  maxRanNodes;
    unsigned char   Customer[MAX_STR_LEN];
}PACK_1 license_info_t;

typedef struct runtime_info_s{    
    long licenseCreateTime;
    long totalRunningTime;
}PACK_1 runtime_info_t;

long encrypt_long(long num);
long decrypt_long(long num);

char* timestampToString(time_t timestamp);
char* timestampToStringDate(time_t timestamp);
char* convertSecondsToString(time_t timestamp);

/*生成硬件特征码*/
void dsMakeMachineID(void);

/*检查许可证的有效性*/
bool dsCheckLicense(char* errorMsg, size_t errorMsgSize);

/*在运行时检查许可证的有效性。根据给定的运行时间和剩余天数判断
0: 有效
1：即将到期(根据remainingDays判断是否属于即将到期)
2：已到期*/
int checkLicenseAfterRuntime(long runTime, int remainingDays);

#define LICENSE_STATE_NOT_EXPIRED     0
#define LICENSE_STATE_SOON_TO_EXPIRE  1
#define LICENSE_STATE_EXPIRED         2
/*查看license状态*/
const char *get_license_state_name(int state);

/*获取许可证中最大用户数*/
int  getLicenseUeNum(void);

/*获取许可证的总运行时间*/
long getLicenseRunTime(void);

/*获取许可证的过期时间*/
long getLicenseExpireTime(void);

/*获取许可证的创建时间*/
long getLicenseDurationTime(void);

/*获取许可证的有效时长*/
long getLicenseCreateTime(void);

/*获取许可证的剩余时长*/
long getLicenseRemainingTime(void);

int getLicenseSubscriptions(void);
int getLicenseRanNodes(void);
char* getLicenseCustomer(void);

char* getLicenseSerialno(void);

#endif /* End of _LICENSE_INCLUDE_H_ */

