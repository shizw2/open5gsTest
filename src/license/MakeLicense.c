#include "license.h"

static void dsMakeLicense(int numUsers, long expireTimestamp, long durationTimestamp)
{
    char  m_szPrivateKey[32] = "5gc_Security_2023-11-11";  /*存放私有密钥*/
    UINT   iFileLength;    
    FILE  *LicenseInputFile;    
    FILE           *LicenseOutputFile;
    unsigned char   digest[16];
    char FilePathName[32] = "Machine.id";
    char OutFilePathName[32]="License.dat";
    time_t currentTime;
    time(&currentTime);
    long licenseDuration;
    license_info_t license_info;
    
    memset(&license_info,0,sizeof(license_info_t));

    if ((LicenseInputFile = fopen(FilePathName, "rb")) == NULL)
    {
         printf("打开机器特征码文件失败,请查看是否存在该文件!\r\n");
         return;
    }


    iFileLength = fread(license_info.szSystemInfoFromFile,sizeof(BYTE),MAX_SYS_INFO_LENGTH,LicenseInputFile);
    if(iFileLength==0)
    {
         printf("读入特征码失败，请确认文件内容!\r\n");
         return;
    }else{
        printf("读入特征码成功!\r\n");
    }
    fclose(LicenseInputFile);

    license_info.maxUserNum = (int)encrypt_long(numUsers);

 
    if (durationTimestamp == 0){
        if (expireTimestamp > (long)currentTime){
            licenseDuration = expireTimestamp - (long)currentTime;
        }else{
            licenseDuration = 0;
        }
    }else{
        licenseDuration = durationTimestamp;
    }
    
    printf("license创建时间:%s(%ld), 截止时间:%s, 有效时长:%ld秒\r\n",timestampToString(currentTime),currentTime,timestampToString(expireTimestamp),licenseDuration);

    license_info.licenseExpireTime = encrypt_long(expireTimestamp);   

    license_info.licenseDuration = encrypt_long(licenseDuration);        
    
    license_info.licenseCreateTime = encrypt_long(currentTime);
   
    /* 写文件 */
    if ((LicenseOutputFile = fopen(OutFilePathName, "wb")) == NULL)
    {
         printf("创建License文件失败,请重试!\r\n");
         return;
    }    
   
    if(fwrite((unsigned char *)&license_info,sizeof(BYTE),sizeof(license_info)-16, LicenseOutputFile)!= sizeof(license_info)-16)
    {
         printf("生成License文件出错，请重试!\r\n");
           fclose(LicenseOutputFile);
         return;
    }
    
    memset(digest,0,16);   
  
    /*添加一个MD5 digest */
    dshmac_md5((unsigned char *)&license_info,sizeof(license_info)-16,(unsigned char *)m_szPrivateKey,32,digest);

    if(fwrite(digest,sizeof(BYTE),16, LicenseOutputFile)!=16)
    {
         printf("生成License文件出错，请重试!\r\n");
           fclose(LicenseOutputFile);
         return;
    }

    fclose(LicenseOutputFile);

    printf("成功生成License文件，可以拷贝给用户!\r\n");   
}

int main(void)
{
    int numUsers;
    int timeChoice;
    long duration = 0;
    int year, month, day;
    time_t currentTime;
    struct tm *localTime;
    time_t timestamp;
    
    printf("输入在线用户数: ");
    scanf("%d", &numUsers);

    printf("请选择许可证类型:\n");
    printf("1. 有效时长\n");
    printf("2. 有效时间\n");
    scanf("%d", &timeChoice);

    if (timeChoice == 1) {
        printf("输入有效时长(天): ");
        scanf("%ld", &duration);

        if (duration <= 0) {
            printf("有效时长必须大于0.\n");
            return 0;
        }

        struct tm timeinfo = {0};
        timeinfo.tm_year = 9999 - 1900; // 年份需要减去1900
        timeinfo.tm_mon = 12 - 1; // 月份需要减去1
        timeinfo.tm_mday = 31;

        timestamp = mktime(&timeinfo);
        
        printf("有效时长: %ld(天)，计%ld(秒)\n", duration,duration*86400);
    }
    else if (timeChoice == 2) {
        printf("输入截止日期 (YYYYMMDD): ");
        scanf("%4d%2d%2d", &year, &month, &day);
        
        // 检查是否超过最大日期99991231
        if (year > 9999 || month > 12 || day > 31) {
            printf("无效的日期,截止日期需小于9999年12月31日,且年月日需合法\n");
            return 0;
        }

        if (year == 0 && month == 0 && day == 0) {
            struct tm timeinfo = {0};
            timeinfo.tm_year = 9999 - 1900; // 年份需要减去1900
            timeinfo.tm_mon = 12 - 1; // 月份需要减去1
            timeinfo.tm_mday = 31;

            timestamp = mktime(&timeinfo);
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

            printf("截止时间: %s\n", timestampToString(timestamp));
        }
    }
    else {
        printf("Invalid choice. Please enter either 1 or 2.\n");
        return 0;
    }


    dsMakeLicense(numUsers, (long)timestamp, duration*86400); // 如果输入的有效时间为00000000，则设置为永久有效
    printf("验证license.\r\n");
    
    char errorMsg[100];
    size_t errorMsgSize = sizeof(errorMsg);

    bool result = dsCheckLicense(errorMsg, errorMsgSize);
    if (!result) {
        printf("错误信息: %s\n", errorMsg);
    }

    int ret = checkLicenseAfterRuntime(0,30);
    printf("license状态:%s,系统已运行:%lu秒, 有效时长:%lu秒, 截止时间:%s,创建时间:%s,在线用户数:%d\r\n",
                    get_license_state_name(ret), 
                    getLicenseRunTime(),
                    getLicenseDurationTime(),
                    timestampToString(getLicenseExpireTime()),
                    timestampToString(getLicenseCreateTime()),
                    getLicenseUeNum());  
    if (ret == LICENSE_STATE_SOON_TO_EXPIRE) {        
        return 1;
    }else if (ret == LICENSE_STATE_EXPIRED) {
        return 1;
    }

    return 0;
}
