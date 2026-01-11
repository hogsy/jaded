/*$T WAYload.h GC!1.41 08/19/99 15:12:36 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __WAYLOAD_H__
#define __WAYLOAD_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern ULONG    WAY_ul_AllNetWorksCallback(ULONG);
extern ULONG	WAY_ul_NetWorkCallback(ULONG);
extern ULONG	WAY_ul_AllLinkListsCallback(ULONG);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif 
 