#include "license.h"

int main(void)
{
    dsCheckLicense();
    
    loadRunningTimeFromFile();
    printf("程序运行中检查license.\r\n");
    isLicenseExpired();
    isLicenseExpired();   
    isLicenseExpired();
    return 0;
}