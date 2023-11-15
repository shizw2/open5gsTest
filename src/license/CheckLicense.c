#include "license.h"

int main(void)
{
    dsCheckLicense();
    
    printf("程序运行中检查license.\r\n");
    isLicenseExpired(1000);
    isLicenseExpired(2000);   
    isLicenseExpired(2000000);
    return 0;
}