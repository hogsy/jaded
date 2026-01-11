/*$T RECdyn.h GC!1.5 12/07/99 18:11:05 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Main functions of the dynamics module
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef _RECDYN_H_
#define _RECDYN_H_

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#define REC_ComputeAllReports		0

void    REC_MainCall(struct WOR_tdst_World_ *, ULONG);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
