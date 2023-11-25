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
        printf("license状态:%s,系统已运行:%lu秒, 有效时长:%lu秒, 截止时间:%s,在线用户数:%d\r\n", 
                        get_license_state_name(ret),
                        getLicenseRunTime(),
                        getLicenseDurationTime(),
                        timestampToString(getLicenseExpireTime()),
                        getLicenseUeNum());      
        if (ret == LICENSE_STATE_EXPIRED) {
            return 1;
        }
    }
    
    return 0;
}