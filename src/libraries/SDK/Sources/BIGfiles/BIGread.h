/*$T BIGread.h GC! 1.097 05/01/02 14:34:51 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __BIGREAD_H__
#define __BIGREAD_H__

#include "BASe/BAStypes.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGio.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

extern void		*BIG_gp_GlobalBuffer;
extern int		BIG_gi_GlobalBufferSize;
#ifdef ACTIVE_EDITORS
extern void		*BIG_gp_GlobalSaveBuffer;
extern int		BIG_gi_GlobalSaveBufferSize;
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

extern ULONG	BIG_ul_GetLengthFile(ULONG);
extern void		BIG_Read(ULONG, void *, ULONG);
extern ULONG	BIG_ul_ReadFile(ULONG, void *);
extern char		*BIG_pc_ReadFileTmp(ULONG, ULONG *);
extern char		*BIG_pc_ReadFileTmpMustFree(ULONG, ULONG *);
extern void		*BIG_p_RequestBuffer(int);
#ifdef ACTIVE_EDITORS
extern void		*BIG_p_RequestSaveBuffer(int);
extern ULONG	BIG_ul_EditorGetSizeOfFile(ULONG ul_FileKey);
#endif
extern void		BIG_FreeGlobalBuffer(void);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __BIGREAD_H__ */ 
 