/*$T BIGcomp.h GC! 1.081 07/28/00 12:31:49 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef _BIGCOMP_H_
#define _BIGCOMP_H_

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#ifdef PSX2_TARGET
#include <eetypes.h>
#else
#ifndef _SYS_TYPES_H
typedef unsigned char	u_char;
typedef unsigned short	u_short;
typedef unsigned int	u_int;
#endif
#endif

/*$4
 ***********************************************************************************************************************
    public macros & variables
 ***********************************************************************************************************************
 */

#define BIGcomp_M_CompressBufferSize(_a)	(_a + _a / 64 + 16 + 3)

extern unsigned int BIGcomp_M_BlockSize;
extern unsigned int BIGcomp_M_CompressedSize;

#define L_ERR_Csz_BIGcomp					"Error while compressing file"
#define L_ERR_Csz_BIGcomp_Block 			"Error while compressing a block"


/*$4
 ***********************************************************************************************************************
    public types
 ***********************************************************************************************************************
 */

typedef struct	BIGcomp_tdst_CompressParams_
{
	u_char	*in;
	u_char	*out;
	u_int	uInBufLen;
	u_int	uOutBufLen;
} BIGcomp_tdst_CompressParams;

typedef struct	BIGcomp_tdst_DecompressParams_
{
	u_char			*in;
	u_char			*out;
	u_int			uUncompressedLen;
	int				bWait;
	volatile u_char **ppLoadPos;
} BIGcomp_tdst_DecompressParams;

/*$4
 ***********************************************************************************************************************
    public functions
 ***********************************************************************************************************************
 */

extern u_char	*BIGcomp_DecodeBlocks(BIGcomp_tdst_DecompressParams *, u_int);
extern int		BIGcomp_EncodeBlocks(BIGcomp_tdst_CompressParams *, u_int);
extern int		BIGcomp_CheckBlock(u_char *, u_int);
extern int		BIGcomp_CheckBuffer(u_char *, u_int);
extern int      BIGcomp_i_CompressedSize(u_char *_p_StartBlockAddr);
extern int      BIGcomp_i_DecompressedSize(u_char *_p_StartBlockAddr);


#endif /* _BIGCOMP_H_ */

