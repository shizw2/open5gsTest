#include "license.h"

#define MAX_SYS_INFO_LENGTH 4096


char  m_szPrivateKey[32] = "pttptt_Security_2016-03-07";  /*存放私有密钥*/
int numUsers;
long licenseExpireTime;
long licenseDuration;
long totalRunningTime = 0;//半小时统计一次，并记录到文件


// 全局变量，存储文件名
const char* FILE_PATH_1 = "/var/run/running_time.dat";
const char* FILE_PATH_2 = "/var/log/running_time.dat";

// 加载已运行时间从文件
void loadRunningTimeFromFile(void) {
    FILE* file = fopen(FILE_PATH_1, "r");
    if (file != NULL) {
        fscanf(file, "%ld", &totalRunningTime);        
        fclose(file);
    }else {
        file = fopen(FILE_PATH_2, "r");
        if (file != NULL) {
            fscanf(file, "%ld", &totalRunningTime);            
            fclose(file);
        }
    }

    if (totalRunningTime != 0){
        totalRunningTime = decrypt_long(totalRunningTime);
    }
    
    printf("系统已运行时间:%ld秒.\r\n",totalRunningTime);
}

// 保存已运行时间到文件
void saveRunningTimeToFile(const char* filePath) {
    FILE* file = fopen(filePath, "wb");
    if (file == NULL) {
        printf("无法打开文件进行写入。\n");
        exit(1);
    }
    fprintf(file, "%ld", encrypt_long(totalRunningTime));
    fclose(file);
}

void saveRunningTimeToFiles(void){
    saveRunningTimeToFile(FILE_PATH_1);
    saveRunningTimeToFile(FILE_PATH_2);
}

void dsGetSerialNumber(unsigned char *szSysInfo, int *piSystemInfoLen)
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

// 加密函数
int encrypt(int num) {
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

// 解密函数
int decrypt(int num) {
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

long encrypt_long(long num) {
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

long decrypt_long(long num) {
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


void dsMakeLicense(int numUsers, long expireTimestamp, long durationTimestamp)
{

#ifdef _CN_VER_REL
    return;
#endif

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

//半小时执行一次
bool isLicenseExpired(void)
{
    time_t currentTime;
    time(&currentTime);
    
    totalRunningTime += 1800;

    long currentTimestamp = (long)currentTime; // 转换为整数时间戳
    
    char buffer[80];
    if (licenseExpireTime != 0){
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&licenseExpireTime));
    }else{
        strcpy(buffer,"未限制");
    }
  
    if ((licenseExpireTime >= currentTimestamp || licenseExpireTime == 0) && licenseDuration >= totalRunningTime ) {
        printf("License未过期,系统已运行:%ld秒,截止时限:%s, 有效时长:%ld秒.\n",totalRunningTime,buffer, licenseDuration);
        saveRunningTimeToFiles();
        return true;
    } else {
        totalRunningTime -= 1800;
        printf("License已过期,系统已运行:%ld秒,截止时限:%s, 有效时长:%ld秒.\n",totalRunningTime,buffer, licenseDuration);
        return false;
    }
}

int getLicenseUeNum(void)
{
    return numUsers;
}


bool dsCheckLicense(void)
{ 
    unsigned char   szDigest[16];
    unsigned char   szDigestFromFile[16];
    unsigned char   szSystemInfo[MAX_SYS_INFO_LENGTH];
    unsigned char   szSystemInfoFromFile[MAX_SYS_INFO_LENGTH];
    unsigned char   szSystemInfoAll[MAX_SYS_INFO_LENGTH + sizeof(int) + sizeof(long) + sizeof(long)];
    char            FilePathName[32] = "License.dat";
    FILE           *LicenseInputFile;
    int             iSystemInfoLen   = 0;

    memset(szDigest,0,16);
    memset(szDigestFromFile,0,16);
    memset(szSystemInfo,0,MAX_SYS_INFO_LENGTH);
    memset(szSystemInfoFromFile,0,MAX_SYS_INFO_LENGTH);
    memset(szSystemInfoAll,0,MAX_SYS_INFO_LENGTH + sizeof(int) + sizeof(long) + sizeof(long));


    /* 读文件 */
    if ((LicenseInputFile = fopen(FilePathName, "rb")) == NULL)
    {
         printf("License文件不存在失败,请重新选择文件!\r\n");
         return false;
    }
    
    /* 读取系统信息 */
    if(fread(szSystemInfoFromFile,sizeof(BYTE),MAX_SYS_INFO_LENGTH, LicenseInputFile)!=MAX_SYS_INFO_LENGTH)
    {
         printf("License文件出错，该文件被人为修改1!\r\n");
         fclose(LicenseInputFile);
         return false;
    }
    
    memcpy(szSystemInfoAll, szSystemInfoFromFile, MAX_SYS_INFO_LENGTH);
    
    if (fread(&numUsers, sizeof(int), 1, LicenseInputFile) != 1)
    {
        printf("License文件出错，无法读取用户数!\r\n");
        fclose(LicenseInputFile);
        return false;
    }
    
    memcpy(szSystemInfoAll + MAX_SYS_INFO_LENGTH, &numUsers, sizeof(int));
    
    numUsers = decrypt(numUsers);
    printf("用户数解密成功: %d\n", numUsers);
    
    if (fread(&licenseExpireTime, sizeof(long), 1, LicenseInputFile) != 1)
    {
        printf("License文件出错，无法读取时限!\r\n");
        fclose(LicenseInputFile);
        return false;
    }
    memcpy(szSystemInfoAll + MAX_SYS_INFO_LENGTH + sizeof(int), &licenseExpireTime, sizeof(long));
    
    licenseExpireTime = decrypt_long(licenseExpireTime);
    printf("时限解密成功: %ld\n", licenseExpireTime);
    
    if (fread(&licenseDuration, sizeof(long), 1, LicenseInputFile) != 1)
    {
        printf("License文件出错，无法读取时限!\r\n");
        fclose(LicenseInputFile);
        return false;
    }
    memcpy(szSystemInfoAll + MAX_SYS_INFO_LENGTH + sizeof(int) + sizeof(long), &licenseDuration, sizeof(long));
    
    licenseDuration = decrypt_long(licenseDuration);
    printf("时长解密成功: %ld\n", licenseDuration);

    if(fread(szDigestFromFile,sizeof(BYTE),16, LicenseInputFile)!=16)
    {
         printf("License文件出错，该文件被人为修改2!\r\n");
         fclose(LicenseInputFile);
         return false;
    }
    fclose(LicenseInputFile);


    /*拷贝系统信息到临时变量中*/
    dsGetSerialNumber(szSystemInfo, &iSystemInfoLen);

    
    /*比较文件中的特征码是否与原特征码文件中的信息一致*/
    if (memcmp(szSystemInfo,szSystemInfoFromFile, MAX_SYS_INFO_LENGTH) != 0)  
    {
        printf("License文件信息与特征信息输入文件不一致!\r\n");
        return false ;
    }else{
        printf("硬件特征信息验证成功!\r\n"); 
    }
  
    /*算临时信息的MD5 digest */
    dshmac_md5((unsigned char *)szSystemInfoAll,MAX_SYS_INFO_LENGTH + sizeof(int) + sizeof(long) + sizeof(long),(unsigned char *)m_szPrivateKey,32,szDigest);
   
    if (memcmp(szDigest, szDigestFromFile, 16) != 0)  
    {
         printf("License文件信息被人为修改!\r\n");
         return false;
    }

    printf("License文件正确!\r\n");
    
    loadRunningTimeFromFile();
    if (isLicenseExpired()){
        return false;
    }

    return true;
}

