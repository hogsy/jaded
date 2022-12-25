/*$T ENGmsg.h GC!1.40 07/15/99 10:27:23 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __ENGMSG_H__
#define __ENGMSG_H__

#include "ENGstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
extern BOOL ENG_gb_GlobalLock;
#endif

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

extern BOOL MAI_b_TreatMainWndMessages(HWND, UINT, WPARAM, LPARAM, LRESULT *);
extern BOOL MAI_b_TreatOwnerWndMessages
            (
                HWND,
                MAI_tdst_WinHandles *,
                UINT,
                WPARAM,
                LPARAM,
                LRESULT *
            );
extern BOOL MAI_b_TreatDisplayWndMessages
            (
                HWND,
                MAI_tdst_WinHandles *,
                UINT,
                WPARAM,
                LPARAM,
                LRESULT *
            );

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __ENGMSG_H__ */