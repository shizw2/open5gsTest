#include "license.h"
#include <errno.h>

int main(void)
{
    int i,ret;
    char errorMsg[100];
    size_t errorMsgSize = sizeof(errorMsg);
    
    bool result = dsCheckLicense(errorMsg, errorMsgSize);
    if (!result) {
        printf("错误信息: %s\n", errorMsg);       
    }
        
    printf("程序运行中检查license.\r\n");

    for (i = 0; i < 10; i++){
        ret = checkLicenseAfterRuntime(1000*(i+1),30);
        if (ret == LICENSE_SOON_TO_EXPIRE) {
            printf("license即将过期,系统已运行:%lu秒, 有效时长:%lu秒, 截止时间:%s,在线用户数:%d\r\n", getLicenseRunTime(),
                        getLicenseDurationTime(),
                        timestampToString(getLicenseExpireTime()),
                        getLicenseUeNum());            
        }else if (ret == LICENSE_EXPIRED) {
            printf("license已过期,系统已运行:%lu秒, 有效时长:%lu秒, 截止时间:%s,在线用户数:%d\r\n", getLicenseRunTime(),
                        getLicenseDurationTime(),
                        timestampToString(getLicenseExpireTime()),
                        getLicenseUeNum());
            return 1;
        }else {
            printf("license未过期,系统已运行:%lu秒, 有效时长:%lu秒, 截止时间:%s,在线用户数:%d\r\n", getLicenseRunTime(),
                        getLicenseDurationTime(),
                        timestampToString(getLicenseExpireTime()),
                        getLicenseUeNum());
        }
    }
    
    return 0;
}