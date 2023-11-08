#include <string.h>
#include "psMD5.h"

/***********************************************************************
 **  Message-digest routines:                                         **
 **  To form the message digest for a message M                       **
 **    (1) Initialize a context buffer mdContext using dsMD5Init        **
 **    (2) Call dsMD5Update on mdContext and M                          **
 **    (3) Call MD5Final on mdContext                                 **
 **  The message digest is now in mdContext->digest[0...15]           **
 ***********************************************************************/

/* forward declaration */
static void Transform (WORD32 *buf, WORD32 *in);

static BYTE PADDING[64] =
{
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* F, G, H and I are basic MD5 functions */
#define F(x, y, z) (((x) & (y)) | ((~(x)) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~(z))))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~(z))))

/* ROTATE_LEFT rotates x left n bits */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4 */
/* Rotation is separate from addition to prevent recomputation */
#define FF(a, b, c, d, x, s, ac) \
    {   (a) += F ((b), (c), (d)) + (x) + (WORD32)(ac); \
        (a) = ROTATE_LEFT ((a), (s)); \
        (a) += (b); \
    }
#define GG(a, b, c, d, x, s, ac) \
    {   (a) += G ((b), (c), (d)) + (x) + (WORD32)(ac); \
        (a) = ROTATE_LEFT ((a), (s)); \
        (a) += (b); \
    }
#define HH(a, b, c, d, x, s, ac) \
    {   (a) += H ((b), (c), (d)) + (x) + (WORD32)(ac); \
        (a) = ROTATE_LEFT ((a), (s)); \
        (a) += (b); \
    }
#define II(a, b, c, d, x, s, ac) \
    {   (a) += I ((b), (c), (d)) + (x) + (WORD32)(ac); \
        (a) = ROTATE_LEFT ((a), (s)); \
        (a) += (b); \
    }

#ifdef __STDC__
#define UL(x)   x##U
#else
#define UL(x)   x
#endif

/* The routine dsMD5Init initializes the message-digest context
   mdContext. All fields are set to zero.
 */
void dsMD5Init ( MD5_CTX * pmdContext )
{
    if(NULL == pmdContext)
    {
        return;
    }
    
    pmdContext->i[0] = pmdContext->i[1] = (WORD32)0;

    /* Load magic initialization constants.  */
    pmdContext->buf[0] = (WORD32)0x67452301L;
    pmdContext->buf[1] = (WORD32)0xefcdab89L;
    pmdContext->buf[2] = (WORD32)0x98badcfeL;
    pmdContext->buf[3] = (WORD32)0x10325476L;
}

/* The routine dsMD5Update updates the message-digest context to
   account for the presence of each of the characters inBuf[0..inLen-1]
   in the message whose digest is being computed.
 */
void dsMD5Update ( MD5_CTX * pmdContext, BYTE *inBuf, WORD32 inLen )
{
    WORD32           in[16];
    unsigned int    mdi;
    unsigned int    i, ii;

    if(NULL == pmdContext || NULL == inBuf)
    {
        return;
    }
    
    memset((void*)in, 0x0, 16*sizeof(WORD32));
    
    /* compute number of bytes mod 64 */
    mdi = (unsigned int)((pmdContext->i[0] >> 3) & 0x3F);

    /* update number of bits */
    if ((pmdContext->i[0] + ((WORD32)inLen << 3)) < pmdContext->i[0])
        pmdContext->i[1]++;
    pmdContext->i[0] += ((WORD32)inLen << 3);
    pmdContext->i[1] += ((WORD32)inLen >> 29);

    while (inLen--)
    {
        /* add new character to buffer, increment mdi */
        pmdContext->in[mdi++] = *inBuf++;

        /* transform if necessary */
        if (mdi == 0x40)
        {
            for (i = 0, ii = 0; i < 16; i++, ii += 4)
                in[i] = (((WORD32)pmdContext->in[ii+3]) << 24) |
                    (((WORD32)pmdContext->in[ii+2]) << 16) |
                    (((WORD32)pmdContext->in[ii+1]) << 8) |
                    ((WORD32)pmdContext->in[ii]);
            Transform (pmdContext->buf, in);
            mdi = 0;
        }
    }
}

/* The routine MD5Final terminates the message-digest computation and
   ends with the desired message digest in mdContext->digest[0...15].
 */
void dsMD5Final ( MD5_CTX * pmdContext )
{
    WORD32           in[16];
    unsigned int    mdi;
    unsigned int    i, ii;
    WORD32           padLen;

    if(NULL == pmdContext)
    {
        return;
    }
    
    memset((void*)in, 0x0, 16*sizeof(WORD32));

    /* save number of bits */
    in[14] = pmdContext->i[0];
    in[15] = pmdContext->i[1];

    /* compute number of bytes mod 64 */
    mdi = (unsigned int)((pmdContext->i[0] >> 3) & 0x3F);

    /* pad out to 56 mod 64 */
    padLen = (mdi < 56) ? (56 - mdi) : (120 - mdi);
    dsMD5Update (pmdContext, PADDING, padLen);

    /* append length in bits and transform */
    for (i = 0, ii = 0; i < 14; i++, ii += 4)
        in[i] = (((WORD32)pmdContext->in[ii+3]) << 24) |
            (((WORD32)pmdContext->in[ii+2]) << 16) |
            (((WORD32)pmdContext->in[ii+1]) << 8) |
            ((WORD32)pmdContext->in[ii]);
    Transform (pmdContext->buf, in);

    /* store buffer in digest */
    for (i = 0, ii = 0; i < 4; i++, ii += 4)
    {
        pmdContext->digest[ii] = (BYTE)(pmdContext->buf[i] & 0xFF);
        pmdContext->digest[ii+1] =
            (BYTE)((pmdContext->buf[i] >> 8) & 0xFF);
        pmdContext->digest[ii+2] =
            (BYTE)((pmdContext->buf[i] >> 16) & 0xFF);
        pmdContext->digest[ii+3] =
            (BYTE)((pmdContext->buf[i] >> 24) & 0xFF);
    }
}

/* Basic MD5 step. Transforms buf based on in.
 */
static void Transform (WORD32 *buf, WORD32 *in)
{
    WORD32 a , b , c , d ;
#define S11 (BYTE)7
#define S12 (BYTE)12
#define S13 (BYTE)17
#define S14 (BYTE)22

    if(NULL == buf || NULL == in)
    {
        return;
    }
    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];

  /* Round 1 */
    FF ( a, b, c, d, in[ 0], S11, UL(3614090360L)); /* 1 */
    FF ( d, a, b, c, in[ 1], S12, UL(3905402710L)); /* 2 */
    FF ( c, d, a, b, in[ 2], S13, UL( 606105819L)); /* 3 */
    FF ( b, c, d, a, in[ 3], S14, UL(3250441966L)); /* 4 */
    FF ( a, b, c, d, in[ 4], S11, UL(4118548399L)); /* 5 */
    FF ( d, a, b, c, in[ 5], S12, UL(1200080426L)); /* 6 */
    FF ( c, d, a, b, in[ 6], S13, UL(2821735955L)); /* 7 */
    FF ( b, c, d, a, in[ 7], S14, UL(4249261313L)); /* 8 */
    FF ( a, b, c, d, in[ 8], S11, UL(1770035416L)); /* 9 */
    FF ( d, a, b, c, in[ 9], S12, UL(2336552879L)); /* 10 */
    FF ( c, d, a, b, in[10], S13, UL(4294925233L)); /* 11 */
    FF ( b, c, d, a, in[11], S14, UL(2304563134L)); /* 12 */
    FF ( a, b, c, d, in[12], S11, UL(1804603682L)); /* 13 */
    FF ( d, a, b, c, in[13], S12, UL(4254626195L)); /* 14 */
    FF ( c, d, a, b, in[14], S13, UL(2792965006L)); /* 15 */
    FF ( b, c, d, a, in[15], S14, UL(1236535329L)); /* 16 */

  /* Round 2 */
#define S21 (BYTE)5
#define S22 (BYTE)9
#define S23 (BYTE)14
#define S24 (BYTE)20
    GG ( a, b, c, d, in[ 1], S21, UL(4129170786L)); /* 17 */
    GG ( d, a, b, c, in[ 6], S22, UL(3225465664L)); /* 18 */
    GG ( c, d, a, b, in[11], S23, UL( 643717713L)); /* 19 */
    GG ( b, c, d, a, in[ 0], S24, UL(3921069994L)); /* 20 */
    GG ( a, b, c, d, in[ 5], S21, UL(3593408605L)); /* 21 */
    GG ( d, a, b, c, in[10], S22, UL(  38016083L)); /* 22 */
    GG ( c, d, a, b, in[15], S23, UL(3634488961L)); /* 23 */
    GG ( b, c, d, a, in[ 4], S24, UL(3889429448L)); /* 24 */
    GG ( a, b, c, d, in[ 9], S21, UL( 568446438L)); /* 25 */
    GG ( d, a, b, c, in[14], S22, UL(3275163606L)); /* 26 */
    GG ( c, d, a, b, in[ 3], S23, UL(4107603335L)); /* 27 */
    GG ( b, c, d, a, in[ 8], S24, UL(1163531501L)); /* 28 */
    GG ( a, b, c, d, in[13], S21, UL(2850285829L)); /* 29 */
    GG ( d, a, b, c, in[ 2], S22, UL(4243563512L)); /* 30 */
    GG ( c, d, a, b, in[ 7], S23, UL(1735328473L)); /* 31 */
    GG ( b, c, d, a, in[12], S24, UL(2368359562L)); /* 32 */

  /* Round 3 */
#define S31 (BYTE)4
#define S32 (BYTE)11
#define S33 (BYTE)16
#define S34 (BYTE)23
    HH ( a, b, c, d, in[ 5], S31, UL(4294588738L)); /* 33 */
    HH ( d, a, b, c, in[ 8], S32, UL(2272392833L)); /* 34 */
    HH ( c, d, a, b, in[11], S33, UL(1839030562L)); /* 35 */
    HH ( b, c, d, a, in[14], S34, UL(4259657740L)); /* 36 */
    HH ( a, b, c, d, in[ 1], S31, UL(2763975236L)); /* 37 */
    HH ( d, a, b, c, in[ 4], S32, UL(1272893353L)); /* 38 */
    HH ( c, d, a, b, in[ 7], S33, UL(4139469664L)); /* 39 */
    HH ( b, c, d, a, in[10], S34, UL(3200236656L)); /* 40 */
    HH ( a, b, c, d, in[13], S31, UL( 681279174L)); /* 41 */
    HH ( d, a, b, c, in[ 0], S32, UL(3936430074L)); /* 42 */
    HH ( c, d, a, b, in[ 3], S33, UL(3572445317L)); /* 43 */
    HH ( b, c, d, a, in[ 6], S34, UL(  76029189L)); /* 44 */
    HH ( a, b, c, d, in[ 9], S31, UL(3654602809L)); /* 45 */
    HH ( d, a, b, c, in[12], S32, UL(3873151461L)); /* 46 */
    HH ( c, d, a, b, in[15], S33, UL( 530742520L)); /* 47 */
    HH ( b, c, d, a, in[ 2], S34, UL(3299628645L)); /* 48 */

  /* Round 4 */
#define S41 (BYTE)6
#define S42 (BYTE)10
#define S43 (BYTE)15
#define S44 (BYTE)21
    II ( a, b, c, d, in[ 0], S41, UL(4096336452L)); /* 49 */
    II ( d, a, b, c, in[ 7], S42, UL(1126891415L)); /* 50 */
    II ( c, d, a, b, in[14], S43, UL(2878612391L)); /* 51 */
    II ( b, c, d, a, in[ 5], S44, UL(4237533241L)); /* 52 */
    II ( a, b, c, d, in[12], S41, UL(1700485571L)); /* 53 */
    II ( d, a, b, c, in[ 3], S42, UL(2399980690L)); /* 54 */
    II ( c, d, a, b, in[10], S43, UL(4293915773L)); /* 55 */
    II ( b, c, d, a, in[ 1], S44, UL(2240044497L)); /* 56 */
    II ( a, b, c, d, in[ 8], S41, UL(1873313359L)); /* 57 */
    II ( d, a, b, c, in[15], S42, UL(4264355552L)); /* 58 */
    II ( c, d, a, b, in[ 6], S43, UL(2734768916L)); /* 59 */
    II ( b, c, d, a, in[13], S44, UL(1309151649L)); /* 60 */
    II ( a, b, c, d, in[ 4], S41, UL(4149444226L)); /* 61 */
    II ( d, a, b, c, in[11], S42, UL(3174756917L)); /* 62 */
    II ( c, d, a, b, in[ 2], S43, UL( 718787259L)); /* 63 */
    II ( b, c, d, a, in[ 9], S44, UL(3951481745L)); /* 64 */

    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}

 
/*
** Function: hmac_md5
*/
void dshmac_md5( unsigned char*  text,                /* pointer to data stream */
            int             text_len,            /* length of data stream */
            unsigned char*  key,                 /* pointer to authentication key */
            int             key_len,             /* length of authentication key */
            BYTE            *poDigest)              /* caller digest to be filled in */
{
    MD5_CTX         context;
    unsigned char   k_ipad[65];    /* inner padding - key XORd with ipad */
    unsigned char   k_opad[65];    /* outer padding - key XORd with opad */
    unsigned char   tk[16];
    int             i;

    if(NULL == text || NULL == key || NULL == poDigest)
    {
        return;
    }
    
    memset((void*)&context, 0x0, sizeof(MD5_CTX));
    memset((void*)tk,       0x0, 16*sizeof(unsigned char));    
    /* if key is longer than 64 bytes reset it to key=MD5(key) */
    if (key_len > 64)
    {
        MD5_CTX      tctx;
        memset((void*)&tctx, 0x0, sizeof(MD5_CTX));

        dsMD5Init(&tctx);
        dsMD5Update(&tctx, key, (WORD32)key_len);
        dsMD5Final( &tctx);
        memcpy( tk, tctx.digest, 16 );

        key = tk;
        key_len = 16;
    }

    /*
     * the HMAC_MD5 transform looks like:
     *
     * MD5(K XOR opad, MD5(K XOR ipad, text))
     *
     * where K is an n byte key
     * ipad is the byte 0x36 repeated 64 times
     * opad is the byte 0x5c repeated 64 times
     * and text is the data being protected
     */

    /* start out by storing key in pads */
    
    memset( k_ipad, 0, sizeof(k_ipad) );
    memset( k_opad, 0, sizeof(k_opad) );
    memcpy( k_ipad, key, (WORD32)key_len);
    memcpy( k_opad, key, (WORD32)key_len);

    /* XOR key with ipad and opad values */
    for (i=0; i<64; i++)
    {
        k_ipad[i] ^= 0x36;
        k_opad[i] ^= 0x5c;
    }
    /*
     * perform inner MD5
     */
    dsMD5Init(&context);                   /* init context for 1st
                                          * pass */
    dsMD5Update(&context, k_ipad, 64);      /* start with inner pad */
    dsMD5Update(&context, text, (WORD32)text_len); /* then text of datagram */
    dsMD5Final(&context);          /* finish up 1st pass */
    /*
     * perform outer MD5
     */
    dsMD5Init(&context);                   /* init context for 2nd
                                          * pass */
    dsMD5Update(&context, k_opad, 64);     /* start with outer pad */
    dsMD5Update(&context, context.digest, 16);     /* then results of 1st
                                          * hash */
    dsMD5Final(&context);          /* finish up 2nd pass */

    memcpy( (void *)poDigest, context.digest, 16 );
}

