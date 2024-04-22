#include "license.h"
#include <unistd.h>
#include <errno.h>



static void dsGetSerialNumber(unsigned char *szSysInfo, int *piSystemInfoLen);
static void saveRunningTimeToFile(const char* filePath);
static void saveRunningTimeToFiles(void);
static void loadRunningTimeFromFile(void);

static license_info_t g_license_info;
static runtime_info_t g_runtime_info;

int getProgramDirectory(char* programPath, size_t bufferSize);
char g_program_name[100];

#define PATH_MAX 100

char* timestampToString(time_t timestamp) {  
    static char buffer[4][20]; 
    static int index = 0; 
    index++;
    index = index%4;
    if (timestamp == 0) {  
        strcpy(buffer[index], "NA");         
    } else {  
        struct tm *timeInfo = localtime(&timestamp);  
        strftime(buffer[index], sizeof(buffer[0]), "%Y-%m-%d %H:%M:%S", timeInfo);         
    }  

    return buffer[index]; 
}

char* convertSecondsToString(time_t timestamp) {
    static char buffer[4][50]; 
    static int index = 0; 
    index++;
    index = index%4;
    
    int days = timestamp / (24 * 60 * 60);
    int hours = (timestamp % (24 * 60 * 60)) / (60 * 60);
    int minutes = (timestamp % (60 * 60)) / 60;
    int seconds = timestamp % 60;

    snprintf(buffer[index], 50, "%d天%d小时%d分钟%d秒", days, hours, minutes, seconds);
    
    return buffer[index]; 
}

// 从三个文件中读取时间，并且最后选择最大的那个时间(防止被篡改)
static void loadRunningTimeFromFile(void) {
    getProgramName(g_program_name);
    char FILE_PATH_1[100] = "/var/run/running_time.dat";
    char FILE_PATH_2[100] = "/var/log/5gc_time.dat";
    char FILE_PATH_3[100] = "/var/lib/run_seconds.dat";

    runtime_info_t temp_runtime_info;
    time_t maxRunningTime = 0;
    int i;

    // 创建一个数组包含所有的文件路径
    char* files[] = {FILE_PATH_1, FILE_PATH_2, FILE_PATH_3};

    for(i = 0; i < 3; i++) {
        FILE* file = fopen(files[i], "rb");
        if(file != NULL) {
            fread(&temp_runtime_info, sizeof(runtime_info_t), 1, file);
            fclose(file);

            temp_runtime_info.totalRunningTime = decrypt_long(temp_runtime_info.totalRunningTime);
            temp_runtime_info.licenseCreateTime = decrypt_long(temp_runtime_info.licenseCreateTime);
            
            // 如果新的时间大于当前最大时间，则更新最大时间
            if (temp_runtime_info.totalRunningTime >= maxRunningTime) {
                maxRunningTime = temp_runtime_info.totalRunningTime;
                g_runtime_info = temp_runtime_info; // Update the global variable with the new max time
            }
        }else{
            printf("文件不存在:%s\r\n",files[i]);
        }
    }

    if(g_runtime_info.licenseCreateTime != g_license_info.licenseCreateTime){
        g_runtime_info.licenseCreateTime = g_license_info.licenseCreateTime;
        g_runtime_info.totalRunningTime = 0;
    }

    //printf("加载系统信息,系统已运行时间:%ld秒.\r\n",g_runtime_info.totalRunningTime);
}


// 保存已运行时间和许可创建时间到文件
static void saveRunningTimeToFile(const char* filePath) {
    runtime_info_t enc_runtime_info;
    FILE* file = fopen(filePath, "wb");
    if (file == NULL) {
        printf("无法打开文件进行写入。\n");
        exit(1);
    }
     
    enc_runtime_info.licenseCreateTime = encrypt_long(g_runtime_info.licenseCreateTime);
    enc_runtime_info.totalRunningTime  = encrypt_long(g_runtime_info.totalRunningTime);
    fwrite(&enc_runtime_info, sizeof(runtime_info_t), 1, file);
    fclose(file);
}


static void saveRunningTimeToFiles(void){
    char FILE_PATH_1[100] = "/var/run/running_time.dat";
    char FILE_PATH_2[100] = "/var/log/5gc_time.dat";
    char FILE_PATH_3[100] = "/var/lib/run_seconds.dat";
    saveRunningTimeToFile(FILE_PATH_1);
    saveRunningTimeToFile(FILE_PATH_2);
    saveRunningTimeToFile(FILE_PATH_3);
}

static void dsGetSerialNumber(unsigned char *szSysInfo, int *piSystemInfoLen)
{
    if (NULL == szSysInfo || NULL == piSystemInfoLen)
    {
        return;
    }

	FILE *fp;
	char ch;
	fp = popen("dmidecode |grep 'Serial Number'","r");
	while(((ch=fgetc(fp))!=EOF) && *piSystemInfoLen < MAX_SYS_INFO_LENGTH)  //该程序将结果输出到屏幕，其它形式类似，保存到变量，strcpy就可以了
	{
	    szSysInfo[(*piSystemInfoLen)++]= ~ch+1;/*取反再加1*/
	}
	pclose(fp);
}


long encrypt_long(long num) {
    long LONG_KEY = 0xABCDEFABCDEF;
    long result = num ^ LONG_KEY; // 进行异或操作
    result += 123456; // 加法操作
    result *= 2; // 乘法操作
    return result;
}

long decrypt_long(long num) {
    long LONG_KEY = 0xABCDEFABCDEF;
    long result = num;
    result /= 2; // 逆向乘法操作：除法
    result -= 123456; // 逆向加法操作：减法
    return result ^ LONG_KEY; // 逆向异或操作
}

int getProgramDirectory(char* programPath, size_t bufferSize) {
    char path[PATH_MAX];
    ssize_t pathLen = readlink("/proc/self/exe", path, sizeof(path) - 1);

    if (pathLen == -1) {
        perror("获取可执行文件路径失败");
        return -1;
    }

    // 确保字符串以 null 结尾
    path[pathLen] = '\0';

    // 提取目录部分
    char* lastSlash = strrchr(path, '/');
    if (lastSlash != NULL) {
        *lastSlash = '\0'; // 替换最后一个斜杠为 null
    }

    strncpy(programPath, path, bufferSize);

    return 0;
}

int getProgramName(char* programName) {
    // 获取当前程序的文件路径
    char path[100];
    if (readlink("/proc/self/exe", path, sizeof(path)) == -1) {
        perror("readlink");
        exit(EXIT_FAILURE);
    }
    
    // 从文件路径中提取程序名称
    char* programName = strrchr(path, '/');
    if (programName != NULL) {
        programName++; // 移动到下一个字符，跳过斜杠
    } else {
        programName = path; // 如果没有斜杠，则使用整个路径作为程序名称
    }
    strcpy(programName, programName);

    return 0;
}

void dsMakeMachineID(void)
{
    char  m_szPrivateKey[32] = "5gc_Security_2023-11-11";  /*存放私有密钥*/
    unsigned char szSysInfo[MAX_SYS_INFO_LENGTH] = { 0 };/*生成szSysInfo的算法可以修改*/
    int iSystemInfoLen = 0;
	FILE  *MachineFile;
    char FilePathName[32] = "Machine.id";

    if (NULL == szSysInfo)
    {
        return;
    }
    
	if ((MachineFile = fopen(FilePathName, "w")) == NULL)
	{
		 printf("创建Machine.id文件失败\r\n");
		 return;
	}

	dsGetSerialNumber(szSysInfo, &iSystemInfoLen);

    if(fwrite(szSysInfo,sizeof(BYTE),iSystemInfoLen, MachineFile)!=iSystemInfoLen)
	{
		printf("特征码写入文件失败，请重试!\r\n");
	}
    else
    {
        printf("生成硬件特征码成功.\r\n");
    }
    
	fclose(MachineFile);
}

const char *get_license_state_name(int state)
{
    switch (state) {
    case LICENSE_STATE_NOT_EXPIRED:
        return "not expired";
    case LICENSE_STATE_SOON_TO_EXPIRE:
        return "soon to expire";
    case LICENSE_STATE_EXPIRED:
        return "expired";
    default:
        break;
    }

    return "unknown state";
}


int checkLicenseAfterRuntime(long runTime, int remainingDays)
{
    time_t currentTime;
    time(&currentTime);
    time_t tempTime;
    
    if (g_runtime_info.totalRunningTime == 0){
        loadRunningTimeFromFile();
    }
    
    g_runtime_info.totalRunningTime += runTime;   
    
    long currentTimestamp = (long)currentTime; // 转换为整数时间戳
    int remainingSeconds = remainingDays * 24 * 60 * 60; // 将剩余天数转换为剩余秒数
 
    if ((g_license_info.licenseExpireTime >= currentTimestamp ) && g_license_info.licenseDuration >= g_runtime_info.totalRunningTime ) {
        if ((g_license_info.licenseExpireTime - currentTimestamp < remainingSeconds) ||
            (g_license_info.licenseDuration - g_runtime_info.totalRunningTime < remainingSeconds)) {
            saveRunningTimeToFiles();
            return 1; // 未到期但剩余时间小于指定天数或有效时长还剩余指定天数
        } else {
            saveRunningTimeToFiles();
            return 0; // 未到期且剩余时间大于等于指定天数且有效时长还剩余指定天数以上
        }
    } else {
        g_runtime_info.totalRunningTime = g_license_info.licenseDuration;
        saveRunningTimeToFiles(); 
        return 2; // 已到期
    }
}

int getLicenseUeNum(void)
{
    return g_license_info.maxUserNum;
}

long getLicenseRunTime(void)
{
    return g_runtime_info.totalRunningTime;
}

long getLicenseExpireTime(void)
{
    return g_license_info.licenseExpireTime;
}

long getLicenseCreateTime(void)
{
    return g_license_info.licenseCreateTime;
}

long getLicenseDurationTime(void)
{
    return g_license_info.licenseDuration;
}

long getLicenseRemainingTime(void)
{
    return g_license_info.licenseDuration - g_runtime_info.totalRunningTime;
}

bool dsCheckLicense(char* errorMsg, size_t errorMsgSize) {
    char  m_szPrivateKey[32] = "5gc_Security_2023-11-11";  /*存放私有密钥*/
    unsigned char szDigest[16];
    unsigned char szSystemInfo[MAX_SYS_INFO_LENGTH];
    char FilePathName[PATH_MAX+30] = {};
    FILE *LicenseInputFile;
    int iSystemInfoLen = 0;
    license_info_t license_info;
    char programDir[PATH_MAX];   

    memset(szDigest, 0, sizeof(szDigest));
    memset(szSystemInfo, 0, sizeof(szSystemInfo));

    if (getProgramDirectory(programDir, sizeof(programDir)) != 0) {
        snprintf(errorMsg, errorMsgSize, "获取程序所在目录失败: %s", strerror(errno));
        return false;
    }
    
    snprintf(FilePathName, sizeof(FilePathName), "%s/License.dat", programDir);

    /* 读文件 */
    if ((LicenseInputFile = fopen(FilePathName, "rb")) == NULL) {
        snprintf(errorMsg, errorMsgSize, "打开License文件失败: %s", FilePathName);
        return false;
    }

    int readlen;
    readlen = fread(&license_info, sizeof(license_info_t), 1, LicenseInputFile);
    if (readlen != 1) {
        snprintf(errorMsg, errorMsgSize, "读取License文件失败,该文件被人为修改,readlen:%d, filelen:%ld.", readlen, sizeof(license_info_t));
        fclose(LicenseInputFile);
        return false;
    }
    
    fclose(LicenseInputFile);        

    g_license_info.maxUserNum = (int)decrypt_long(license_info.maxUserNum);
   
    g_license_info.licenseExpireTime = decrypt_long(license_info.licenseExpireTime);

    g_license_info.licenseDuration = decrypt_long(license_info.licenseDuration);
   
    g_license_info.licenseCreateTime = decrypt_long(license_info.licenseCreateTime); 
  
    /*拷贝系统信息到临时变量中*/
    dsGetSerialNumber(szSystemInfo, &iSystemInfoLen);

    /*比较文件中的特征码是否与原特征码文件中的信息一致*/
    if (memcmp(szSystemInfo, license_info.szSystemInfoFromFile, MAX_SYS_INFO_LENGTH) != 0) {
        snprintf(errorMsg, errorMsgSize, "License文件信息与特征信息输入文件不一致!");
        return false;
    } else {
        //printf("硬件特征信息验证成功!\n");
    }

    /*算临时信息的MD5 digest */
    dshmac_md5((unsigned char*)&license_info, sizeof(license_info)-16, (unsigned char*)m_szPrivateKey, 32, szDigest);

    if (memcmp(szDigest, license_info.szDigestFromFile, sizeof(szDigest)) != 0) {
        snprintf(errorMsg, errorMsgSize, "License文件信息被人为修改!");
        return false;
    }

    //printf("License文件正确!\n");
    int state = checkLicenseAfterRuntime(0,30);
    if (state == LICENSE_STATE_SOON_TO_EXPIRE) {
        snprintf(errorMsg, errorMsgSize, "许可即将过期!");
        return true;
    }else if (state == LICENSE_STATE_EXPIRED) {
        snprintf(errorMsg, errorMsgSize, "许可已过期!");
        return false;
    }else{
        return true;
    }
}