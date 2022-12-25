/*$T TEXTload.h GC 1.138 03/23/04 16:30:27 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __TEXTload_h__
#define __TEXTload_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#include "BASe/BAStypes.h"
#include "TEXTstruct.h"

/*$4
 ***********************************************************************************************************************
    Prototypes
 ***********************************************************************************************************************
 */
extern struct TEXT_tdst_OneText_* TEXT_gp_CallbackLoadTxlForSound ;

/* world text */
void	TEXT_LoadWorldText(void);
void	TEXT_FreeAll(void);
void	TEXT_ChangeLang(int);
void	TEXT_ChangeSndLang(int);

/* one text */
ULONG	TEXT_ul_CallbackLoadTxl(ULONG _ul_PosFile, ULONG key);
ULONG	TEXT_ul_CallbackLoadTxlForSound(ULONG _ul_PosFile, ULONG key);
void	TEXT_Free(TEXT_tdst_OneText *);
int		TEXT_i_GetEntryIndex(TEXT_tdst_OneText *pText, int i_EntryKey);
int		TEXT_i_GetOneTextIndex(ULONG ulKey);

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
