/*$T BIGcomp.c GC! 1.097 12/14/00 14:23:26 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */
#include "Precomp.h"
#ifndef _GAMECUBE
#include <sys/types.h>
#endif
#include <stdio.h>
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGcomp.h"
#include "minilzo.h"
#include "BASe/ERRors/ERRasser.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "BASe/MEMory/MEM.h"

/*$4
 ***********************************************************************************************************************
    private macros
 ***********************************************************************************************************************
 */

#define BIGcomp_M_min(a, b)			(a > b ? b : a)
#define BIGcomp_M_max(a, b)			(a < b ? b : a)
#define BIGcomp_M_MinSizeCompressed 30
#define BIGcomp_M_WorkMemoryLength	65535

unsigned int BIGcomp_M_BlockSize = 500 * 1024;
unsigned int BIGcomp_M_CompressedSize = 1024 * 1024;


/*$4
 ***********************************************************************************************************************
    (de)compression procedures
 ***********************************************************************************************************************
 */

//#define __METHOD_dblminilzo
#define __METHOD_minilzo

#if defined(__METHOD_minilzo)
#define M_Decompress(_in_, _inlen_, _out_, _outlen_)	(lzo1x_decompress(_in_, _inlen_, _out_, &_outlen_, 0) == LZO_E_OK)
#define M_Compress(_in_, _inlen_, _out_, _outlen_) \
		(lzo1x_1_compress(_in_, _inlen_, _out_, &_outlen_, s_WorkMemory) == LZO_E_OK)
#elif defined(__METHOD_dblminilzo)
#define M_Decompress(_in_, _inlen_, _out_, _outlen_)	s_DbleDecompression(_in_, _inlen_, _out_, &_outlen_)
#define M_Compress(_in_, _inlen_, _out_, _outlen_)		s_DbleCompression(_in_, _inlen_, _out_, &_outlen_)
#else
#define M_Decompress(_in, _inlen, _out, _outlen)	(L_memcpy(_out, _in, _inlen), (_outlen = _inlen, 1))
#define M_Compress(_in, _inlen, _out, _outlen)		(L_memcpy(_out, _in, _inlen), (_outlen = _inlen, 1))
#endif

/*
 * #define M_MarkMask 0x80000000 #define M_ReadSize(a) (a & ~M_MarkMask) #define
 * M_ReadMark(a) (a & M_MarkMask) £
 * mask for double compression
 */
#ifdef __METHOD_dblminilzo
#define M_MarkMask		0x80000000
#define M_ReadSize(a)	(a &~M_MarkMask)
#define M_ReadMark(a)	(a & M_MarkMask)
#else
#define M_ReadSize(a)	a
#define M_ReadMark(a)	a
#endif

/*$4
 ***********************************************************************************************************************
    static variables
 ***********************************************************************************************************************
 */

/* caution : only one work memory area => no paralel compression */
char	s_WorkMemory[BIGcomp_M_WorkMemoryLength];

/*$4
 ***********************************************************************************************************************
    private functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
u_int BIGcomp_ui_Read32(u_char *f)
{
	/*~~~~~~*/
	u_int	v;
	/*~~~~~~*/

	v = (u_int) (*f++) << 0;
	v |= (u_int) (*f++) << 8;
	v |= (u_int) (*f++) << 16;
	v |= (u_int) (*f++) << 24;
	return v;
}

#ifdef __METHOD_dblminilzo

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static u_int s_DbleCompression(u_char *_psrc, u_int _src_len, u_char *_pdst, u_int *_pdst_len)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	u_char	*ptemp;
	u_int	templen, templen2;
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	ptemp = (u_char *) MEM_p_Alloc(BIGcomp_M_CompressBufferSize(_src_len));
    L_memset(ptemp, 0, BIGcomp_M_CompressBufferSize(_src_len));

    L_memset(s_WorkMemory, 0, BIGcomp_M_WorkMemoryLength);
	lzo1x_1_compress(_psrc, _src_len, ptemp, &templen, s_WorkMemory);
	if(_src_len <= templen)
	{
		L_memcpy(_pdst, _psrc, _src_len);
		*_pdst_len = _src_len;
	}
	else
	{
        L_memset(s_WorkMemory, 0, BIGcomp_M_WorkMemoryLength);
		lzo1x_1_compress(ptemp, templen, _pdst, &templen2, s_WorkMemory);
		if(templen <= templen2)
		{
			L_memcpy(_pdst, ptemp, templen);
			*_pdst_len = templen;
		}
		else
			*_pdst_len = templen2 | M_MarkMask;
	}

	MEM_Free(ptemp);
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static u_int s_DbleDecompression(u_char *_psrc, u_int _src_len, u_char *_pdst, u_int *_pdst_len)
{
	/*~~~~~~~~~~~~*/
	u_char	*ptemp;
	u_int	templen;
/*#ifdef PSX2_TARGET
	extern u_long128 GS_RAMBUFFER1[];
#endif*/
	/*~~~~~~~~~~~~*/
	if(M_ReadMark(_src_len))
	{
#ifdef PSX2_TARGET11
        ptemp = (u_char *) GS_RAMBUFFER1;
        *((u_int *)&(ptemp)) |= 0x30000000;
#else	
		ptemp = (u_char *) MEM_p_AllocFromEnd(BIGcomp_M_BlockSize);
#endif		
		lzo1x_decompress(_psrc, M_ReadSize(_src_len), ptemp, &templen, 0);
		lzo1x_decompress(ptemp, templen, _pdst, _pdst_len, 0);
#ifndef PSX2_TARGET
		MEM_FreeFromEnd(ptemp);
#endif		
	}
	else
		lzo1x_decompress(_psrc, _src_len, _pdst, _pdst_len, 0);

	return 1;
}

#endif /* __METHOD_dblminilzo */

/*$4
 ***********************************************************************************************************************
    public functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int BIGcomp_EncodeBlocks(BIGcomp_tdst_CompressParams *_p_Param, u_int _uiBlockSize)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	u_int						ui_Rest;
	BIGcomp_tdst_CompressParams cparam;
	u_int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i = 0;

	ui_Rest = _p_Param->uInBufLen;
	_p_Param->uOutBufLen = 0;

	cparam.in = _p_Param->in;
	cparam.uInBufLen = BIGcomp_M_min(_p_Param->uInBufLen, _uiBlockSize);

	cparam.out = _p_Param->out + 8;
	cparam.uOutBufLen = BIGcomp_M_CompressBufferSize(cparam.uInBufLen);

	do
	{
		i++;

		L_memset(s_WorkMemory, 0, BIGcomp_M_WorkMemoryLength);

		/* compression */
		ERR_X_Error
		(
			M_Compress(cparam.in, cparam.uInBufLen, cparam.out, cparam.uOutBufLen),
			L_ERR_Csz_BIGcomp_Block,
			NULL
		);

		/* case compression failed */
		if(cparam.uInBufLen <= M_ReadSize(cparam.uOutBufLen))
		{
			L_memcpy(cparam.out, cparam.in, cparam.uInBufLen);
			cparam.uOutBufLen = cparam.uInBufLen;
		}

		/* block header */
		*(u_int *) (cparam.out - 8) = cparam.uInBufLen;
		*(u_int *) (cparam.out - 4) = cparam.uOutBufLen;

		cparam.in += cparam.uInBufLen;
		cparam.out += M_ReadSize(cparam.uOutBufLen) + 8;

		ui_Rest = BIGcomp_M_max(0, ui_Rest - cparam.uInBufLen);
		cparam.uInBufLen = BIGcomp_M_min(_uiBlockSize, ui_Rest);
	} while(ui_Rest > BIGcomp_M_MinSizeCompressed);

	if(ui_Rest)
	{
		i++;
		*(u_int *) (cparam.out - 8) = ui_Rest;
		*(u_int *) (cparam.out - 4) = ui_Rest;
		L_memcpy(cparam.out, cparam.in, ui_Rest);
		cparam.out += ui_Rest + 8;
	}

	*(u_int *) (cparam.out - 8) = 0;
	_p_Param->uOutBufLen = cparam.out - 4 - _p_Param->out;

	/* fin verif */
	return (int) i;
}

/*
 =======================================================================================================================
    // Decode // exit when meet with a end-of-compressed-data flag // return a pointer which points to next
    un-decompressed data
 =======================================================================================================================
 */
extern BOOL LOA_gb_CompressBin;
u_char *BIGcomp_DecodeBlocks(BIGcomp_tdst_DecompressParams *_p_Param, u_int _ui_MaxBlockNumber)
{
	/*~~~~~~~~*/
	u_char	*fi;
	u_char	*fo;
	u_int	j;
	/*~~~~~~~~*/

    if(LOA_gb_CompressBin)
    {
	    PROPS2_StartRaster(&PROPS2_gst_BIGcomp_DecodeBlocks);
	    fi = _p_Param->in;
	    fo = _p_Param->out;

	    _p_Param->uUncompressedLen = 0;

	    /* process blocks */
	    for(j = 0; j < _ui_MaxBlockNumber; j++)
	    {
		    /*~~~~~~~~~~~~~~~~*/
		    lzo_uint	in_len;
		    lzo_uint	out_len;
		    /*~~~~~~~~~~~~~~~~*/

		    /* read uncompressed size */
		    out_len = BIGcomp_ui_Read32(fi);
		    fi += 4;

		    /* exit if last block (EOF marker) */
		    if(out_len == 0) break;

		    /* read compressed size */
		    in_len = BIGcomp_ui_Read32(fi);
		    fi += 4;

		    if(M_ReadSize(in_len) < out_len)
		    {
			    /*~~~~~~~~~~~~~~~~*/
			    lzo_uint	new_len;
			    /*~~~~~~~~~~~~~~~~*/

			    new_len = out_len;
			    PROPS2_StartRaster(&PROPS2_gst_BIGcomp_Decompress);
			    M_Decompress(fi, in_len, fo, new_len);
			    PROPS2_StopRaster(&PROPS2_gst_BIGcomp_Decompress);
			    if(new_len != out_len)
			    {
				    ERR_X_Error(0, "Decode error", NULL);
			    }
		    }
		    else
		    {
			    /* write original (incompressible) block */
			    L_memcpy(fo, fi, M_ReadSize(in_len));
		    }

		    /* move to next block */
		    fi += M_ReadSize(in_len);
		    fo += out_len;
		    _p_Param->uUncompressedLen += out_len;
	    }

	    PROPS2_StopRaster(&PROPS2_gst_BIGcomp_DecodeBlocks);
	    return fi;	/* point to the next un-decompressed data */
    }
    else
    {
        L_memmove(_p_Param->out, _p_Param->in, _ui_MaxBlockNumber*BIGcomp_M_BlockSize);
        _p_Param->uUncompressedLen = _ui_MaxBlockNumber*BIGcomp_M_BlockSize;
        return (_p_Param->in + _ui_MaxBlockNumber*BIGcomp_M_BlockSize);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int BIGcomp_CheckBlock(u_char *_p_StartBlockAddr, u_int _ui_FreeLength)
{
    if(LOA_gb_CompressBin)
	    return (_ui_FreeLength >= M_ReadSize(BIGcomp_ui_Read32(_p_StartBlockAddr + 4)) + 8);
    else
        return (_ui_FreeLength >= BIGcomp_M_BlockSize);
}

/*
 =======================================================================================================================
 return the compressed block size (with 8 oct header)
 =======================================================================================================================
 */
int BIGcomp_i_CompressedSize(u_char *_p_StartBlockAddr)
{
    if(LOA_gb_CompressBin)
	    return(M_ReadSize(BIGcomp_ui_Read32(_p_StartBlockAddr + 4)) + 8);
    else
        return BIGcomp_M_BlockSize;
}

int BIGcomp_i_DecompressedSize(u_char *_p_StartBlockAddr)
{
    if(LOA_gb_CompressBin)
	    return(M_ReadSize(BIGcomp_ui_Read32(_p_StartBlockAddr)));
    else
        return BIGcomp_M_BlockSize;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int BIGcomp_CheckBuffer(u_char *_p_StartBlockAddr, u_int _ui_FreeLength)
{
	if(LOA_gb_CompressBin)
        return(_ui_FreeLength >= BIGcomp_ui_Read32(_p_StartBlockAddr));
    else
        return(_ui_FreeLength >= BIGcomp_M_BlockSize);
}
/*
 =======================================================================================================================
 =======================================================================================================================
*/
void LZO_UseIndication(char *Src,ULONG SrcLen,ULONG Indication)
{
	char *Src2,*Src3,*LastSrc;
	if (!Indication) return;
	Src3 = Src2 = Src + Indication;
	LastSrc = Src + SrcLen;
	while (Src2 < LastSrc)
	{
		*(Src2++) ^= *(Src++);
		if (Src3 == Src) Src -= Indication;
	}
}
/*
 =======================================================================================================================
 =======================================================================================================================
*/
void LZO_Compress(void *Src,void *Dst,ULONG SrcLen,ULONG *DstLen,ULONG Indication)
{
	LZO_UseIndication((char*)Src,SrcLen,Indication);
	L_memset(s_WorkMemory, 0, BIGcomp_M_WorkMemoryLength);
	lzo1x_1_compress((const unsigned char*)Src, SrcLen, (unsigned char*)Dst, (lzo_uint*)DstLen, s_WorkMemory);
	if (*DstLen >= SrcLen) *DstLen = SrcLen;
	LZO_UseIndication((char*)Src,SrcLen,Indication);
}

/*
 =======================================================================================================================
 =======================================================================================================================
*/
void LZO_DeCompress(void *Src,void *Dst,ULONG SrcLen,ULONG *DstLen,ULONG Indication)
{
	L_memset(s_WorkMemory, 0, BIGcomp_M_WorkMemoryLength);	
	lzo1x_decompress((const unsigned char*)Src, SrcLen,(unsigned char*)Dst,(lzo_uint*)DstLen,s_WorkMemory);
}

