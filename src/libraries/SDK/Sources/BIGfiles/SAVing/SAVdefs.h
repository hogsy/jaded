/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#include "BASe/BAStypes.h"
#include "BIGfiles/BIGdefs.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

extern void         SAV_Begin(char *, char *);
extern void         SAV_Buffer(void *, int);
extern BIG_INDEX    SAV_ul_End(void);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

