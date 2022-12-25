/*$T LINKtoed.h GC! 1.081 04/11/00 09:44:31 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#include "BASe/CLIbrary/CLIwin.h"
#ifdef ACTIVE_EDITORS
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

extern BOOL LINK_gb_CanRefresh;
extern BOOL LINK_gb_RefreshEnable;
extern BOOL LINK_gb_AllRefreshEnable;
extern int	LINK_gi_SpeedDraw;
extern BOOL LINK_gb_RefreshOnlyFiles;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

extern void LINK_SendMessageToEditors(ULONG, ULONG, ULONG);
extern void LINK_OneTrameEnding(void);
extern void LINK_Refresh(void);
extern void LINK_RefreshMenu(void);
extern BOOL LINK_b_ProcessEngineWndMsg(MSG *);
extern void LINK_FatHasChanged(void);
extern void LINK_UpdatePointers(void);
extern void LINK_DisplayRasters(void);
extern BOOL LINK_PreTranslateMessage(MSG *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#else
#define LINK_UpdatePointers()
#endif
