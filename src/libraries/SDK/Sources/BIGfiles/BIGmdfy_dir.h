/*$T BIGmdfy_dir.h GC!1.20 04/01/99 14:36:08 */

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
extern BOOL         BIG_gb_ImportRecurse;
extern BOOL			BIG_gb_ImportDirMirror;
extern BOOL			BIG_gb_ImportDirInvMirror;
extern BOOL			BIG_gb_ImportWithExt;
extern BOOL         BIG_gb_ImportDirOnlyLoaded;
extern BOOL         BIG_gb_ImportDirOnlyTruncated;

extern void         BIG_UpdateDirFromDisk(char *, char *);
extern BIG_INDEX    BIG_ul_CreateDir(char *);
extern void         BIG_RenDir(char *, char *);
extern void         BIG_DelDir(char *);
extern void         BIG_CopyDir(char *, char *);
extern void         BIG_MoveDir(char *, char *);
extern void         BIG_MoveUnchangedFiles( BIG_INDEX, char *, char * );
extern void         BIG_UntouchFilesRec( BIG_INDEX );
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* ACTIVE_EDITORS */
