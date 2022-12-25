/*$T xbINOsaving.h GC 1.138 04/08/05 15:06:24 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __xbINOsaving_h__
#define __xbINOsaving_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

void	xbINO_SavModuleInit(void);
void	xbINO_SavModuleClose(void);
BOOL	xbINO_b_AsyncInProgress(int _i_Slot);
int		xbINO_i_TestPresence(int _i_Slot);
int		xbINO_i_MountCard(int _i_Slot);
int		xbINO_i_UnmountCard(int _i_Slot);
int		xbINO_i_CheckCard(int _i_Slot);
int		xbINO_i_FormatCard(int _i_Slot);
int		xbINO_i_TestFreeSpace(int _i_Slot);
int		xbINO_i_Retry(int _i_Slot);
int		xbINO_i_Continue(int _i_Slot);
int		xbINO_i_ActiveConsole(int _i_Slot);
int		xbINO_i_Open(int _i_Slot);
int		xbINO_i_Close(int _i_Slot);
int		xbINO_i_ReadIndex_1(int _i_Slot);
int		xbINO_i_WriteIndex(int _i_Slot);
int		xbINO_i_ReadSlot_1(int _i_Slot);
int		xbINO_i_ReadSlot_2(int _i_Slot);
int		xbINO_i_ReadSlot_3(int _i_Slot);
int		xbINO_i_WriteSlot_C(int _i_Slot);
int		xbINO_i_WriteSlot_E(int _i_Slot);
int		xbINO_i_WriteSlot_F(int _i_Slot);

BOOL	xbINO_b_SavIsWorking(void);
//befo int		xbINO_i_ReadFile(char *p, int i, int *pi);
//befo int		xbINO_i_WriteFile(char *p, int i, int *pi);
void	xbINO_SavModuleClose(void);
void	xbINO_SavModuleInit(void);
void	xbINO_SavUpdate(void);
//befo int		xbINO_SavGeti(int i);
//befo int		xbINO_SavSeti(int i, int ii);

int		xbINO_i_ReadFile(char *, int, int *,int val);
int		xbINO_i_WriteFile(char *, int, int *,int val);
int		xbINO_i_GetCurrAction(void);//---
int		xbINO_SavGeti(int);
int		xbINO_SavSeti(int,int);
//int		win32INO_i_SavDbgSeti(int,int);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __INOsaving_h__ */
