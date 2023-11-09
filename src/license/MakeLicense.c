#include "license.h"


int main(void)
{
    int numUsers;
    int year, month, day;
    int timeValue;
    time_t currentTime;
    struct tm *localTime;
    
    printf("Enter the number of users: ");
    scanf("%d", &numUsers);


    // 获取当前日期
    currentTime = time(NULL);
    localTime = localtime(&currentTime);
    int currentYear = localTime->tm_year + 1900;
    int currentMonth = localTime->tm_mon + 1;
    int currentDay = localTime->tm_mday;

    printf("Enter a expiration date (YYYYMMDD): ");
    scanf("%4d%2d%2d", &year, &month, &day);

    // 检查输入日期是否小于当前日期
    if (year < currentYear || (year == currentYear && month < currentMonth) || (year == currentYear && month == currentMonth && day < currentDay)) {
        printf("Invalid date. Please enter a date not earlier than the current date.\n");
        return 0;
    }

    timeValue = year * 10000 + month * 100 + day;
    
    dsMakeLicense(numUsers,timeValue);
    printf("验证license.\r\n");
    dsCheckLicense();
    return 0;
}