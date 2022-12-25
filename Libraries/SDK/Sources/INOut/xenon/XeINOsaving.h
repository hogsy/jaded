#ifndef __XeINOsaving_h__
#define __XeINOsaving_h__

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

void	xeINO_SavModuleClose(void);
void	xeINO_SavModuleInit(void);
void	xeINO_SavUpdate(void);
int		xeINO_SavSeti(int i, int ii);
void	xeINO_FlagNewUser( );
DWORD	xeINO_GetSavegameSize( );
bool	xeINO_SavegameExists( int nSaveNumber );

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif // __XeINOsaving_h__
