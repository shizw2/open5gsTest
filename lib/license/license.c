#include "license.h"

#define MAX_SYS_INFO_LENGTH 4096


char  m_szPrivateKey[32] = "pttptt_Security_2016-03-07";  /*存放私有密钥*/

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


void dsMakeMachineID()
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


void dsMakeLicense(int numUsers, int timeValue)
{

#ifdef _CN_VER_REL
    return;
#endif

    BYTE   szSystemInfo[MAX_SYS_INFO_LENGTH + sizeof(int) + sizeof(int)];
    UINT   iSystemInfoLen=MAX_SYS_INFO_LENGTH;
	UINT   iFileLength;	
	FILE  *LicenseInputFile;    
    FILE           *LicenseOutputFile;
    unsigned char   digest[16];
    char FilePathName[32] = "Machine.id";
    char OutFilePathName[32]="License.dat";
    //int numUsers = 100; 
    //int timeValue = 20231109;

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


    timeValue = encrypt(timeValue);
    printf("加密时限成功.\r\n");		
	memcpy(szSystemInfo + MAX_SYS_INFO_LENGTH + sizeof(int), &timeValue, sizeof(int));//增加时限
    

    /* 写文件 */
	if ((LicenseOutputFile = fopen(OutFilePathName, "wb")) == NULL)
	{
		 printf("创建License文件失败,请重试!\r\n");
		 return;
	}
	
   
	if(fwrite(szSystemInfo,sizeof(BYTE),MAX_SYS_INFO_LENGTH + sizeof(int)+ sizeof(int), LicenseOutputFile)!=MAX_SYS_INFO_LENGTH + sizeof(int) + sizeof(int))
	{
		 printf("生成License文件出错，请重试!\r\n");
  	     fclose(LicenseOutputFile);
		 return;
	}
    
    memset(digest,0,16);    
  
	/*添加一个MD5 digest */
    dshmac_md5((unsigned char *)szSystemInfo,MAX_SYS_INFO_LENGTH + sizeof(int) + sizeof(int),(unsigned char *)m_szPrivateKey,32,digest);

	if(fwrite(digest,sizeof(BYTE),16, LicenseOutputFile)!=16)
	{
		 printf("生成License文件出错，请重试!\r\n");
  	     fclose(LicenseOutputFile);
		 return;
	}

	fclose(LicenseOutputFile);

	printf("成功生成License文件，可以拷贝给用户!\r\n");   
}

bool isLicenseValid(int timeValue)
{
    time_t currentTime;
    time(&currentTime);
    struct tm *localTime = localtime(&currentTime);
    int currentYear = localTime->tm_year + 1900;
    int currentMonth = localTime->tm_mon + 1;
    int currentDay = localTime->tm_mday;
    int currenttimeValue = currentYear * 10000 + currentMonth * 100 + currentDay;


    if (timeValue >= currenttimeValue) {
        printf("License未过期.\n");
        return true;
    } else {
        printf("License已过期\n");
        return false;
    }
}


void dsCheckLicense()
{ 
    unsigned char   szDigest[16];
    unsigned char   szDigestFromFile[16];
    unsigned char   szSystemInfo[MAX_SYS_INFO_LENGTH];
    unsigned char   szSystemInfoFromFile[MAX_SYS_INFO_LENGTH];
    unsigned char   szSystemInfoAll[MAX_SYS_INFO_LENGTH + sizeof(int) + sizeof(int)];
    char            FilePathName[32] = "License.dat";
    FILE           *LicenseInputFile;
    int             iSystemInfoLen   = 0;
    int numUsers;
    int timeValue;
  

    memset(szDigest,0,16);
    memset(szDigestFromFile,0,16);
    memset(szSystemInfo,0,MAX_SYS_INFO_LENGTH);
    memset(szSystemInfoFromFile,0,MAX_SYS_INFO_LENGTH);
    memset(szSystemInfoAll,0,MAX_SYS_INFO_LENGTH + sizeof(int) + sizeof(int));


    /* 读文件 */
    if ((LicenseInputFile = fopen(FilePathName, "rb")) == NULL)
    {
         printf("License文件不存在失败,请重新选择文件!\r\n");
         return;
    }
    
    /* 读取系统信息 */
    if(fread(szSystemInfoFromFile,sizeof(BYTE),MAX_SYS_INFO_LENGTH, LicenseInputFile)!=MAX_SYS_INFO_LENGTH)
    {
         printf("License文件出错，该文件被人为修改1!\r\n");
         fclose(LicenseInputFile);
         return;
    }
    
    memcpy(szSystemInfoAll, szSystemInfoFromFile, MAX_SYS_INFO_LENGTH);
    
    if (fread(&numUsers, sizeof(int), 1, LicenseInputFile) != 1)
    {
        printf("License文件出错，无法读取用户数!\r\n");
        fclose(LicenseInputFile);
        return;
    }
    
    memcpy(szSystemInfoAll + MAX_SYS_INFO_LENGTH, &numUsers, sizeof(int));
    
    numUsers = decrypt(numUsers);
    printf("用户数解密成功: %d\n", numUsers);
    
    if (fread(&timeValue, sizeof(int), 1, LicenseInputFile) != 1)
    {
        printf("License文件出错，无法读取时限!\r\n");
        fclose(LicenseInputFile);
        return;
    }
    memcpy(szSystemInfoAll + MAX_SYS_INFO_LENGTH + sizeof(int), &timeValue, sizeof(int));
    
    timeValue = decrypt(timeValue);
    printf("时限解密成功: %d\n", timeValue);

    if(fread(szDigestFromFile,sizeof(BYTE),16, LicenseInputFile)!=16)
    {
         printf("License文件出错，该文件被人为修改2!\r\n");
         fclose(LicenseInputFile);
         return;
    }
    fclose(LicenseInputFile);


    /*拷贝系统信息到临时变量中*/
    dsGetSerialNumber(szSystemInfo, &iSystemInfoLen);

    
    /*比较文件中的特征码是否与原特征码文件中的信息一致*/
    if (memcmp(szSystemInfo,szSystemInfoFromFile, MAX_SYS_INFO_LENGTH) != 0)  
    {
        printf("License文件信息与特征信息输入文件不一致!\r\n");
        return;
    }else{
        printf("硬件特征信息验证成功!\r\n"); 
    }
  
    /*算临时信息的MD5 digest */
    dshmac_md5((unsigned char *)szSystemInfoAll,MAX_SYS_INFO_LENGTH + sizeof(int) + sizeof(int),(unsigned char *)m_szPrivateKey,32,szDigest);
   
    if (memcmp(szDigest, szDigestFromFile, 16) != 0)  
    {
         printf("License文件信息被人为修改!\r\n");
         return;
    }

    printf("License文件正确!\r\n");
    
    isLicenseValid(timeValue);

}

