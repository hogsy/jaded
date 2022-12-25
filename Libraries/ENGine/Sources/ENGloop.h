/*$T ENGloop.h GC!1.67 01/03/00 16:09:47 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __ENGLOOP_H__
#define __ENGLOOP_H__

#include "BASe/BAStypes.h"
#include "GDInterface/GDInterface.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

extern BOOL     ENG_gb_ExitApplication;
extern BOOL     ENG_gb_ForceEndEngine;
extern BOOL     ENG_gb_ForcePauseEngine;

#ifndef ACTIVE_EDITORS
extern BOOL     sgb_DisplayRasters;
extern BOOL     sgb_FullScreen;
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

extern void     ENG_Loop(void);
extern void (*ENG_p_InOut) (void);
extern void (*ENG_p_Engine) (void);
extern void (*ENG_p_Display) (HWND, GDI_tdst_DisplayData *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __ENGLOOP_H__ */