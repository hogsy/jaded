/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifndef __BASLOG_H__
#define __BASLOG_H__

#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

extern void BAS_UpdateLog(char *, char *, UCHAR);
extern void BAS_UpdateLogSourceFile(char *, char *, ULONG, char *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __BASLOG_H__ */