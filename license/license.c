#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
typedef enum { false, true } bool;

#define MAX_SYS_INFO_LENGTH 4096
const int MAX_USER_COUNT = 100;
const int MAX_LICENSE_DURATION = 365; // 365 days

//#include "psBase.h"
//#include "psPrint.h"

// 加密和解密的私钥
char m_szPrivateKey[32] = "pttptt_Security_2016-03-07";

// 加密函数
void encrypt(int* data)
{
    // 将data与私钥进行异或操作
    *data = *data ^ atoi(m_szPrivateKey);
}

// 解密函数
void decrypt(int* data)
{
    // 将data与私钥进行异或操作
    *data = *data ^ atoi(m_szPrivateKey);
}

// 生成机器特征码
void dsGetSerialNumber(char* szSysInfo, int* piSystemInfoLen)
{
    //char szSysInfo[MAX_SYS_INFO_LENGTH] = { 0 };/*生成szSysInfo的算法可以修改*/
	FILE  *MachineFile;

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

// 生成License文件
void dsMakeLicense()
{
    unsigned char szSystemInfo[MAX_SYS_INFO_LENGTH];
    int iSystemInfoLen = MAX_SYS_INFO_LENGTH;
    int iFileLength;
    FILE* LicenseInputFile;
    FILE* LicenseOutputFile;
    unsigned char digest[16];
    char FilePathName[32] = "Machine.id";
    char OutFilePathName[32] = "License.dat";

    memset(szSystemInfo, 0, 4096);

    if ((LicenseInputFile = fopen(FilePathName, "rb")) == NULL)
    {
        printf("打开机器特征码文件失败,请查看是否存在该文件!\r\n");
        return;
    }

    iFileLength = fread(szSystemInfo, sizeof(unsigned char), iSystemInfoLen, LicenseInputFile);
    if (iFileLength == 0)
    {
        printf("读入特征码失败，请确认文件内容!\r\n");
        return;
    }
    fclose(LicenseInputFile);

    memset(digest, 0, 16);

    /*添加一个MD5 digest */
    dshmac_md5((unsigned char*)szSystemInfo, 4096, (unsigned char*)m_szPrivateKey, 32, digest);

    /* 写文件 */
    if ((LicenseOutputFile = fopen(OutFilePathName, "wb")) == NULL)
    {
        printf("创建License文件失败,请重试!\r\n");
        return;
    }

    /* 为了防止原来该文件存在，并且有内容，先清除该文件中的信息 */
    if (fwrite(szSystemInfo, sizeof(unsigned char), 4096, LicenseOutputFile) != 4096)
    {
        printf("生成License文件出错，请重试!\r\n");
        fclose(LicenseOutputFile);
        return;
    }

    if (fwrite(digest, sizeof(unsigned char), 16, LicenseOutputFile) != 16)
    {
        printf("生成License文件出错，请重试!\r\n");
        fclose(LicenseOutputFile);
        return;
    }

    // 加密用户数和时限信息
    int encryptedUserCount = MAX_USER_COUNT;
    int encryptedLicenseDuration = MAX_LICENSE_DURATION;
    encrypt(&encryptedUserCount);
    encrypt(&encryptedLicenseDuration);

    // 写入加密后的用户数和时限信息
    fprintf(LicenseOutputFile, "%d\n", encryptedUserCount);
    fprintf(LicenseOutputFile, "%d\n", encryptedLicenseDuration);

    fclose(LicenseOutputFile);

    printf("成功生成License文件，可以拷贝给用户!\r\n");
}

// 检查License文件
void dsCheckLicense()
{
    int i;
    unsigned char szDigest[16];
    unsigned char szDigestFromFile[16];
    unsigned char szSystemInfo[4096];
    unsigned char szSystemInfoFromFile[4096];
    char FilePathName[32] = "License.dat";
    FILE* LicenseInputFile;
    int iSystemInfoLen = 0;

    memset(szDigest, 0, 16);
    memset(szDigestFromFile, 0, 16);
    memset(szSystemInfo, 0, 4096);
    memset(szSystemInfoFromFile, 0, 4096);

    /* 读文件 */
    if ((LicenseInputFile = fopen(FilePathName, "rb")) == NULL)
    {
        printf("License文件不存在失败,请重新选择文件!\r\n");
        return;
    }

    /* 读取系统信息 */
    if (fread(szSystemInfoFromFile, sizeof(unsigned char), 4096, LicenseInputFile) != 4096)
    {
        printf("License文件出错，该文件被人为修改!\r\n");
        fclose(LicenseInputFile);
        return;
    }

    if (fread(szDigestFromFile, sizeof(unsigned char), 16, LicenseInputFile) != 16)
    {
        printf("License文件出错，该文件被人为修改!\r\n");
        fclose(LicenseInputFile);
        return;
    }
    fclose(LicenseInputFile);

    /*拷贝系统信息到临时变量中*/
    dsGetSerialNumber(szSystemInfo, &iSystemInfoLen);

    /*比较文件中的特征码是否与原特征码文件中的信息一致*/
    if (memcmp(szSystemInfo, szSystemInfoFromFile, 4096) != 0)
    {
        printf("License文件信息与特征信息输入文件不一致!\r\n");
        return;
    }

    /*算临时信息的MD5 digest */
    dshmac_md5((unsigned char*)szSystemInfoFromFile, 4096, (unsigned char*)m_szPrivateKey, 32, szDigest);
    if (memcmp(szDigest, szDigestFromFile, 16) != 0)
    {
        printf("License文件信息被人为修改!\r\n");
        return;
    }

    printf("License文件正确!\r\n");

    // 解密用户数和时限信息
    int encryptedUserCount, encryptedLicenseDuration;
    sscanf(szSystemInfoFromFile + 4096, "%d\n%d\n", &encryptedUserCount, &encryptedLicenseDuration);
    decrypt(&encryptedUserCount);
    decrypt(&encryptedLicenseDuration);

    printf("用户数：%d\n", encryptedUserCount);
    printf("时限：%d\n", encryptedLicenseDuration);
}

bool isLicenseValid()
{
    time_t now = time(0);
    struct tm* now_tm = localtime(&now);
    int currentYear = now_tm->tm_year + 1900;
    int currentMonth = now_tm->tm_mon + 1;
    int currentDay = now_tm->tm_mday;

    FILE* licenseFile = fopen("License.dat", "r");
    if (!licenseFile)
    {
        printf("License文件不存在\n");
        return false;
    }

    char line[256];
    fgets(line, sizeof(line), licenseFile);
    fgets(line, sizeof(line), licenseFile);
    fgets(line, sizeof(line), licenseFile);
    fgets(line, sizeof(line), licenseFile);

    int encryptedYear, encryptedMonth, encryptedDay;
    fscanf(licenseFile, "%d\n%d\n%d\n", &encryptedYear, &encryptedMonth, &encryptedDay);
    fclose(licenseFile);

    // 解密年份、月份和日期
    decrypt(&encryptedYear);
    decrypt(&encryptedMonth);
    decrypt(&encryptedDay);

    if (currentYear > encryptedYear || (currentYear == encryptedYear && currentMonth > encryptedMonth) || (currentYear == encryptedYear && currentMonth == encryptedMonth && currentDay > encryptedDay))
    {
        printf("License已过期\n");
        return false;
    }

    return true;
}

bool isUserCountValid()
{
    FILE* licenseFile = fopen("License.dat", "r");
    if (!licenseFile)
    {
        printf("License文件不存在\n");
        return false;
    }

    char line[256];
    fgets(line, sizeof(line), licenseFile);
    fgets(line, sizeof(line), licenseFile);
    fgets(line, sizeof(line), licenseFile);
    fgets(line, sizeof(line), licenseFile);

    int encryptedUserCount;
    fscanf(licenseFile, "%d\n", &encryptedUserCount);
    fclose(licenseFile);

    // 解密用户数
    decrypt(&encryptedUserCount);

    if (encryptedUserCount > MAX_USER_COUNT)
    {
        printf("用户数超过限制\n");
        return false;
    }

    return true;
}
