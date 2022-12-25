/*$T IMPbase.h GC!1.38 05/25/99 15:04:24 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __IMPBASE_H__
#define __IMPBASE_H__

#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$2
 ---------------------------------------------------------------------------------------------------
    Import callback
 ---------------------------------------------------------------------------------------------------
 */
typedef BOOL (*IMP_tdpfn_Import) (char *, char *, char *);

typedef struct  IMP_tdst_ImportDialog_
{
    int    (*pfnb_Request)(char *);
} IMP_tdst_ImportDialog;



/*$2
 ---------------------------------------------------------------------------------------------------
    Protos
 ---------------------------------------------------------------------------------------------------
 */
extern void IMP_Init(void);
extern BOOL IMP_b_AddImportCallback( char *, IMP_tdpfn_Import );

extern BOOL IMP_b_SpecialImportFile( char *, char *, char *);


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif