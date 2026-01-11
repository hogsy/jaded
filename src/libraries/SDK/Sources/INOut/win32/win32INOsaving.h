/*$T win32INOsaving.h GC 1.138 03/24/05 15:32:59 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __win32INOsaving_h__
#define __win32INOsaving_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

void	win32INO_SavModuleInit(void);
void	win32INO_SavModuleClose(void);

BOOL	win32INO_b_SavIsWorking(void);
int		win32INO_i_ReadFile(char *, int, int *);
int		win32INO_i_WriteFile(char *, int, int *);
int		win32INO_i_GetCurrAction(void);
void	win32INO_SavUpdate(void);
int		win32INO_i_SavGeti(int);
int		win32INO_i_SavSeti(int,int);
int		win32INO_i_SavDbgSeti(int,int);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __INOsaving_h__ */
