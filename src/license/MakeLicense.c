#include "license.h"

int main(void)
{
    int numUsers;
    int timeChoice;
    long duration;
    int year, month, day;
    time_t currentTime;
    struct tm *localTime;
    time_t timestamp;
    
    printf("输入最大用户数: ");
    scanf("%d", &numUsers);

    printf("请选择许可证类型:\n");
    printf("1. 有效时长\n");
    printf("2. 有效时间\n");
    scanf("%d", &timeChoice);

    if (timeChoice == 1) {
        printf("输入有效时长（秒）: ");
        scanf("%ld", &duration);

        timestamp = 0;
        
        printf("有效时长: %ld\n", duration);
    }
    else if (timeChoice == 2) {
        printf("输入截止日期 (YYYYMMDD，永久有效请输入00000000): ");
        scanf("%4d%2d%2d", &year, &month, &day);

        if (year == 0 && month == 0 && day == 0) {
            printf("永久有效\n");
        }
        else {
            // 检查输入日期是否小于当前日期
            currentTime = time(NULL);
            localTime = localtime(&currentTime);
            int currentYear = localTime->tm_year + 1900;
            int currentMonth = localTime->tm_mon + 1;
            int currentDay = localTime->tm_mday;

            if (year < currentYear || (year == currentYear && month < currentMonth) || (year == currentYear && month == currentMonth && day < currentDay)) {
                printf("Invalid date. Please enter a date not earlier than the current date.\n");
                return 0;
            }
            
            struct tm timeinfo = {0};
            timeinfo.tm_year = year - 1900; // 年份需要减去1900
            timeinfo.tm_mon = month - 1; // 月份需要减去1
            timeinfo.tm_mday = day;

            timestamp = mktime(&timeinfo);

            // 将时间戳转换为日期时间字符串
            char buffer[80];
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&timestamp));

            printf("截止时间: %s\n", buffer);
        }
    }
    else {
        printf("Invalid choice. Please enter either 1 or 2.\n");
        return 0;
    }


    dsMakeLicense(numUsers, (long)timestamp, duration); // 如果输入的有效时间为00000000，则设置为永久有效
    printf("验证license.\r\n");
    dsCheckLicense();
    return 0;
}
