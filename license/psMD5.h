

#ifndef _MD5_INCLUDE_H_
#define _MD5_INCLUDE_H_

#include "psPlat.h"
#include "psDefOpr.h"

#ifdef	PDS_VC_PROJECT
#include "dsSPSInclude.h"
#endif

#pragma pack(1)

/* Data structure for MD5 (Message-Digest) computation */
typedef struct tagMD5_CTX 
{
    WORD32       i[2];           /* number of _bits_ handled mod 2^64 */
    WORD32       buf[4];         /* scratch buffer */
    BYTE        in[64];         /* input buffer */
    BYTE        digest[16];     /* actual digest after MD5Final call */
}   PACK_1  MD5_CTX;

 void dsMD5Init ( MD5_CTX *pmdContext );
 void dsMD5Update ( MD5_CTX *pmdContext, BYTE *inBuf, WORD32 inLen );
 void dsMD5Final ( MD5_CTX * pmdContext );
 void dshmac_md5( unsigned char*  text,                /* pointer to data stream */
            int             text_len,            /* length of data stream */
            unsigned char*  key,                 /* pointer to authentication key */
            int             key_len,             /* length of authentication key */
            BYTE            *poDigest);          /* caller digest to be filled in */

#pragma pack()

#endif /* End of _MD5_INCLUDE_H_ */

