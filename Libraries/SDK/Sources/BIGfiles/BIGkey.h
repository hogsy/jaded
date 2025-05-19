/*$T BIGkey.h GC!1.71 02/17/00 11:55:39 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif
#include "BIGfiles/BIGdefs.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Constants
 -----------------------------------------------------------------------------------------------------------------------
 */

#define KEY_ROOT    "JADE_Key"
#define KEY_ID      "JADE_ID"
#define KEY_VALUE   "JADE_Value"

extern BOOL     BIG_gb_CanAskKey;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Functions
 -----------------------------------------------------------------------------------------------------------------------
 */

extern UCHAR    BIG_uc_ComputeKeyID(void);
extern void     BIG_SetKeyID(UCHAR);
extern UCHAR    BIG_uc_GetKeyID(char *);
extern void     BIG_SetKeyValue(ULONG);
extern ULONG    BIG_ul_GetKeyValue(char *);
extern BIG_KEY  BIG_ul_GetNewKey(BIG_INDEX);

#define BIG_C_InvalidKey    0xFFFFFFFF
extern void     BIG_InsertKeyToPos(BIG_KEY, ULONG);
extern void     BIG_DeleteKeyToPos(BIG_KEY);
extern ULONG    BIG_ul_SearchKeyToPos(BIG_KEY);

#ifdef ACTIVE_EDITORS
extern void     BIG_InsertKeyToFat(BIG_KEY, BIG_INDEX);
extern void     BIG_DeleteKeyToFat(BIG_KEY);
extern ULONG    BIG_ul_SearchKeyToFat(BIG_KEY);
extern void     BIG_ChangeKey(BIG_INDEX, BIG_KEY);
extern void		BIG_GetLastKey(int *);
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
