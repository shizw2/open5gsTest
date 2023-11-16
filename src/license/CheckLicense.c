#include "license.h"
#include <errno.h>

int main(void)
{
    
    char errorMsg[100];
    size_t errorMsgSize = sizeof(errorMsg);

    bool result = dsCheckLicense(errorMsg, errorMsgSize);
    if (!result) {
        printf("错误信息: %s\n", errorMsg);
        return 1;
    }
        
    printf("程序运行中检查license.\r\n");
    isLicenseExpired(1000);
    isLicenseExpired(2000);   
    isLicenseExpired(2000000);
    return 0;
}