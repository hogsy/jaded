/*$T BIGmdfy_file.h GC!1.38 05/28/99 16:59:32 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BIGfiles/BIGdefs.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$2
 ---------------------------------------------------------------------------------------------------
    Global variables
 ---------------------------------------------------------------------------------------------------
 */

extern char         *BIG_gpsz_RealName;
extern void         *BIG_gp_Buffer;
extern void         *BIG_gp_Buffer1;
extern void         *BIG_gp_Buffer2;
extern ULONG        BIG_gul_Length;
extern ULONG        BIG_gul_Length1;
extern ULONG        BIG_gul_Length2;
extern BOOL         BIG_gb_GlobalTimeValid;
extern L_time_t     BIG_gx_GlobalTimeWrite;
extern BOOL         BIG_gb_ImportOne;

/*$2
 ---------------------------------------------------------------------------------------------------
    Functions
 ---------------------------------------------------------------------------------------------------
 */

extern void         BIG_CallbackLoadFileFromDisk(ULONG, ULONG);
extern ULONG        BIG_ul_UpdateCreateFileOptim
                    (
                        BIG_INDEX,
                        BIG_INDEX,
                        char *,
                        char *,
                        BIG_tdpfnv_Callback,
                        BOOL
                    );
extern ULONG        BIG_ul_UpdateCreateFile(char *, char *, BIG_tdpfnv_Callback, BOOL);
extern void         BIG_DeleteFile(BIG_INDEX, BIG_INDEX);
extern void         BIG_UpdateFileFromDisk(char *, char *, char *);
extern void         BIG_UpdateFileFromBuffer(char *, char *, void *, ULONG);
extern void         BIG_UpdateFileFromBufferWithDate(char *, char *, void *, ULONG, L_time_t);
extern void         BIG_RenFile(char *, char *, char *);
extern void         BIG_DelFile(char *, char *);
extern BIG_INDEX    BIG_CopyFile(char *, char *, char *);
extern void         BIG_MoveFile(char *, char *, char *);
extern BIG_INDEX    BIG_ul_CreateFile(char *, char *);
extern void         BIG_BuildNewName( char *, char *, char *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* ACTIVE_EDITORS */
