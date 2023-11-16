#include "license.h"
char  m_szPrivateKey[32] = "5gc_Security_2023-11-11";  /*存放私有密钥*/
static long encrypt_long(long num);
static int encrypt(int num);
static void dsMakeLicense(int numUsers, long expireTimestamp, long durationTimestamp)
{
    BYTE   szSystemInfo[MAX_SYS_INFO_LENGTH + sizeof(int) + sizeof(long) + sizeof(long)];
    UINT   iSystemInfoLen=MAX_SYS_INFO_LENGTH;
	UINT   iFileLength;	
	FILE  *LicenseInputFile;    
    FILE           *LicenseOutputFile;
    unsigned char   digest[16];
    char FilePathName[32] = "Machine.id";
    char OutFilePathName[32]="License.dat";
    time_t currentTime;
    time(&currentTime);
    long licenseDuration;
    
	memset(szSystemInfo,0,MAX_SYS_INFO_LENGTH);

	if ((LicenseInputFile = fopen(FilePathName, "rb")) == NULL)
	{
		 printf("打开机器特征码文件失败,请查看是否存在该文件!\r\n");
		 return;
	}


    iFileLength = fread(szSystemInfo,sizeof(BYTE),iSystemInfoLen,LicenseInputFile);
	if(iFileLength==0)
	{
		 printf("读入特征码失败，请确认文件内容!\r\n");
		 return;
	}else{
        printf("读入特征码成功!\r\n");
    }
	fclose(LicenseInputFile);

    numUsers = encrypt(numUsers);
    printf("加密用户数成功.\r\n");	
    
    memcpy(szSystemInfo + MAX_SYS_INFO_LENGTH, &numUsers, sizeof(int));//增加用户数

    if (expireTimestamp != 0){
        if (expireTimestamp > (long)currentTime){
            licenseDuration = expireTimestamp - (long)currentTime;
        }else{
            licenseDuration = 0;
        }
    }else{
        licenseDuration = durationTimestamp;
    }
    
    printf("截止时限:%ld,有效时长:%ld秒\r\n",expireTimestamp,licenseDuration);

    expireTimestamp = encrypt_long(expireTimestamp);
    printf("加密时限成功.\r\n");		
	memcpy(szSystemInfo + MAX_SYS_INFO_LENGTH + sizeof(int), &expireTimestamp, sizeof(long));//增加时限
    
    licenseDuration = encrypt_long(licenseDuration);
    printf("加密时长成功.\r\n");		
	memcpy(szSystemInfo + MAX_SYS_INFO_LENGTH + sizeof(int) + sizeof(long), &licenseDuration, sizeof(long));//增加时长
   
    /* 写文件 */
	if ((LicenseOutputFile = fopen(OutFilePathName, "wb")) == NULL)
	{
		 printf("创建License文件失败,请重试!\r\n");
		 return;
	}	
   
	if(fwrite(szSystemInfo,sizeof(BYTE),MAX_SYS_INFO_LENGTH + sizeof(int)+ sizeof(long) + sizeof(long), LicenseOutputFile)!=MAX_SYS_INFO_LENGTH + sizeof(int) + sizeof(long)+ sizeof(long))
	{
		 printf("生成License文件出错，请重试!\r\n");
  	     fclose(LicenseOutputFile);
		 return;
	}
    
    memset(digest,0,16);    
  
	/*添加一个MD5 digest */
    dshmac_md5((unsigned char *)szSystemInfo,MAX_SYS_INFO_LENGTH + sizeof(int) + sizeof(long)+ sizeof(long),(unsigned char *)m_szPrivateKey,32,digest);

	if(fwrite(digest,sizeof(BYTE),16, LicenseOutputFile)!=16)
	{
		 printf("生成License文件出错，请重试!\r\n");
  	     fclose(LicenseOutputFile);
		 return;
	}

	fclose(LicenseOutputFile);

	printf("成功生成License文件，可以拷贝给用户!\r\n");   
}

static int encrypt(int num) {
    int encryptedNum = 0;
    int temp = num;

    // 将数字每一位进行运算
    while (temp != 0) {
        int digit = temp % 10;
        encryptedNum = encryptedNum * 10 + (digit + 5) % 10; // 进行复杂的运算
        temp /= 10;
    }

    return encryptedNum;
}

static long encrypt_long(long num) {
    long encryptedNum = 0;
    long temp = num;

    // 将数字每一位进行运算
    while (temp != 0) {
        long digit = temp % 10;
        encryptedNum = encryptedNum * 10 + (digit + 5) % 10; // 进行复杂的运算
        temp /= 10;
    }

    return encryptedNum;
}



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
        printf("输入有效时长(天): ");
        scanf("%ld", &duration);

        timestamp = 0;
        
        printf("有效时长: %ld(天)，计%ld(秒)\n", duration,duration*86400);
    }
    else if (timeChoice == 2) {
        printf("输入截止日期 (YYYYMMDD): ");
        scanf("%4d%2d%2d", &year, &month, &day);

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


    dsMakeLicense(numUsers, (long)timestamp, duration*86400); // 如果输入的有效时间为00000000，则设置为永久有效
    printf("验证license.\r\n");
    
    char errorMsg[100];
    size_t errorMsgSize = sizeof(errorMsg);

    bool result = dsCheckLicense(errorMsg, errorMsgSize);
    if (!result) {
        printf("错误信息: %s\n", errorMsg);
    }
    return 0;
}
