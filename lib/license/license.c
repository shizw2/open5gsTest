#include "license.h"
#include <unistd.h>
#include <errno.h>



static void dsGetSerialNumber(unsigned char *szSysInfo, int *piSystemInfoLen);
static int decrypt(int num);
static long encrypt_long(long num);
static long decrypt_long(long num);
static void saveRunningTimeToFile(const char* filePath);
static void saveRunningTimeToFiles(void);
static void loadRunningTimeFromFile(void);

static license_info_t g_license_info;
static runtime_info_t g_runtime_info;

int getProgramDirectory(char* programPath, size_t bufferSize);


char  m_szPrivateKey[32] = "5gc_Security_2023-11-11";  /*存放私有密钥*/


// 全局变量，存储文件名
const char* FILE_PATH_1 = "/var/run/running_time1.dat";
const char* FILE_PATH_2 = "/var/log/running_time2.dat";

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

// 加载已运行时间从文件
static void loadRunningTimeFromFile(void) {
    time_t tempTime;
    FILE* file = fopen(FILE_PATH_1, "rb"); // 注意这里应为"rb"
    if (file != NULL) {
        fread(&g_runtime_info, sizeof(runtime_info_t), 1, file);
        fclose(file);
    } else {
        file = fopen(FILE_PATH_2, "rb");
        if (file != NULL) {
            fread(&g_runtime_info, sizeof(runtime_info_t), 1, file);
            fclose(file);
        }
    }

    if (g_runtime_info.totalRunningTime != 0){
        g_runtime_info.totalRunningTime = decrypt_long(g_runtime_info.totalRunningTime);
    }
    
    g_runtime_info.licenseCreateTime = decrypt_long(g_runtime_info.licenseCreateTime);
    
    if (g_runtime_info.licenseCreateTime != g_license_info.licenseCreateTime){
        printf("license已更新,旧license创建时间:%s,新license创建时间:%s,运行时间清0.\r\n",timestampToString(g_runtime_info.licenseCreateTime) , timestampToString(g_license_info.licenseCreateTime));
        g_runtime_info.licenseCreateTime = g_license_info.licenseCreateTime;
        g_runtime_info.totalRunningTime = 0;
    }

    printf("加载系统信息,系统已运行时间:%ld秒.\r\n",g_runtime_info.totalRunningTime);
}

// 保存已运行时间和许可创建时间到文件
static void saveRunningTimeToFile(const char* filePath) {
    runtime_info_t enc_runtime_info;
    FILE* file = fopen(filePath, "wb");
    if (file == NULL) {
        printf("无法打开文件进行写入。\n");
        exit(1);
    }
    
    //char buffer[30];    
    ///time_t tempTime = g_runtime_info.licenseCreateTime;//解决告警
    //strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&tempTime));
    //printf("保存运行信息:license创建时间: %s,系统已运行时间:%ld秒,.\r\n",buffer,g_runtime_info.totalRunningTime);
    
    enc_runtime_info.licenseCreateTime = encrypt_long(g_runtime_info.licenseCreateTime);
    enc_runtime_info.totalRunningTime  = encrypt_long(g_runtime_info.totalRunningTime);
    fwrite(&enc_runtime_info, sizeof(runtime_info_t), 1, file);
    fclose(file);
}


static void saveRunningTimeToFiles(void){
    saveRunningTimeToFile(FILE_PATH_1);
    saveRunningTimeToFile(FILE_PATH_2);
}

static void dsGetSerialNumber(unsigned char *szSysInfo, int *piSystemInfoLen)
{
    //char szSysInfo[MAX_SYS_INFO_LENGTH] = { 0 };/*生成szSysInfo的算法可以修改*/
	//FILE  *MachineFile;

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



// 解密函数
static int decrypt(int num) {
    int decryptedNum = 0;
    int temp = num;

    // 将数字每一位进行运算
    while (temp != 0) {
        int digit = temp % 10;
        decryptedNum = decryptedNum * 10 + (digit + 5) % 10; // 进行复杂的运算
        temp /= 10;
    }

    return decryptedNum;
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

static long decrypt_long(long num) {
    long decryptedNum = 0;
    long temp = num;

    // 将数字每一位进行运算
    while (temp != 0) {
        long digit = temp % 10;
        decryptedNum = decryptedNum * 10 + (digit + 5) % 10; // 进行复杂的运算
        temp /= 10;
    }

    return decryptedNum;
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

void dsMakeMachineID(void)
{
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


//半小时执行一次
bool isLicenseExpired(long runTime)
{
    time_t currentTime;
    time(&currentTime);
    time_t tempTime;
    
    if (g_runtime_info.totalRunningTime == 0){
        loadRunningTimeFromFile();
    }
    
    g_runtime_info.totalRunningTime += runTime;

    long currentTimestamp = (long)currentTime; // 转换为整数时间戳
 
    if ((g_license_info.licenseExpireTime >= currentTimestamp || g_license_info.licenseExpireTime == 0) && g_license_info.licenseDuration >= g_runtime_info.totalRunningTime ) {
        printf("License未过期,系统已运行:%ld秒,截止时间:%s, 有效时长:%ld秒.\n",g_runtime_info.totalRunningTime,timestampToString(g_license_info.licenseExpireTime), g_license_info.licenseDuration);
        saveRunningTimeToFiles();
        return true;
    } else {
        g_runtime_info.totalRunningTime -= 1800;
        printf("License已过期,系统已运行:%ld秒,截止时间:%s, 有效时长:%ld秒.\n",g_runtime_info.totalRunningTime,timestampToString(g_license_info.licenseExpireTime), g_license_info.licenseDuration);
        return false;
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

long getLicenseDurationTime(void)
{
    return g_license_info.licenseDuration;
}

bool dsCheckLicense(char* errorMsg, size_t errorMsgSize) {
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
        snprintf(errorMsg, errorMsgSize, "读取License文件失败，该文件被人为修改，readlen:%d, filelen:%ld.", readlen, sizeof(license_info_t));
        fclose(LicenseInputFile);
        return false;
    }
    
    fclose(LicenseInputFile);        

    g_license_info.maxUserNum = decrypt(license_info.maxUserNum);
    printf("license最大用户数: %d\n", g_license_info.maxUserNum);

    g_license_info.licenseExpireTime = decrypt_long(license_info.licenseExpireTime);

    printf("license截止时间: %s\n", timestampToString(g_license_info.licenseExpireTime));
    
    g_license_info.licenseDuration = decrypt_long(license_info.licenseDuration);
    printf("license有效时长: %ld(秒)\n", g_license_info.licenseDuration);
   
    g_license_info.licenseCreateTime = decrypt_long(license_info.licenseCreateTime); 
    printf("license创建时间: %s\n", timestampToString(g_license_info.licenseCreateTime));

    /*拷贝系统信息到临时变量中*/
    dsGetSerialNumber(szSystemInfo, &iSystemInfoLen);

    /*比较文件中的特征码是否与原特征码文件中的信息一致*/
    if (memcmp(szSystemInfo, license_info.szSystemInfoFromFile, MAX_SYS_INFO_LENGTH) != 0) {
        snprintf(errorMsg, errorMsgSize, "License文件信息与特征信息输入文件不一致!");
        return false;
    } else {
        printf("硬件特征信息验证成功!\n");
    }

    /*算临时信息的MD5 digest */
    dshmac_md5((unsigned char*)&license_info, sizeof(license_info)-16, (unsigned char*)m_szPrivateKey, 32, szDigest);

    if (memcmp(szDigest, license_info.szDigestFromFile, sizeof(szDigest)) != 0) {
        snprintf(errorMsg, errorMsgSize, "License文件信息被人为修改!");
        return false;
    }

    printf("License文件正确!\n");

    if (isLicenseExpired(0) == false) {
        snprintf(errorMsg, errorMsgSize, "许可已过期!");
        return false;
    }

    return true;
}