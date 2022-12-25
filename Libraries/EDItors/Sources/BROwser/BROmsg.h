/*$T BROmsg.h GC 1.138 11/08/04 11:31:18 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#include "EDIeditors_infos.h"

#define EBRO_MESSAGE_LINKTOPATH		    EDI_MSG_BROWSER+0
#define EBRO_MESSAGE_IMPORTFROMPATH    EDI_MSG_BROWSER+1

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

extern BOOL BRO_b_FileIsExcluded(ULONG ul_Index);
extern BOOL BRO_b_FileIsForced(ULONG ul_Index);
extern BOOL BRO_b_DirIsExcluded(char*);
extern BOOL BRO_b_DirIsForced(char*);
extern void BRO_AutoCheckOutFile(char *_asz_Path, char *_asz_Name);
extern void BRO_AutoCheckOutDir(char *_asz_Path);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
