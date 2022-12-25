/*$T INO.h GC 1.138 03/16/04 09:57:43 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __INO_H__
#define __INO_H__

#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

typedef enum	INO_tden_MasterType_
{
	e_Master_None	= 0,
	e_MasterPS2_SCEE= 1,
	e_MasterPS2_SCEA= 2,
	e_MasterGC_PAL	= 3,
	e_MasterGC_NTSC = 4,
	e_Master_Dummy	= 0xFFFFFFFF
} INO_tden_MasterType;

typedef enum	INO_tden_LanguageId_
{
	INO_e_French		= 0,
	INO_e_English		= 1,
	INO_e_Danish		= 2,
	INO_e_Dutch			= 3,
	INO_e_Finnish		= 4,
	INO_e_German		= 5,
	INO_e_Italian		= 6,
	INO_e_Spanish		= 7,
	INO_e_Portuguese	= 8,
	INO_e_Swedish		= 9,
	INO_e_Polish		= 10,
	INO_e_Russian		= 11,
	INO_e_Japanese		= 12,
	INO_e_Chinese		= 13,
	INO_e_Albanian		= 14,
	INO_e_Arabic		= 15,
	INO_e_Bulgarian		= 16,
	INO_e_Byelorussian	= 17,
	INO_e_Greek			= 18,
	INO_e_Korean		= 19,
	INO_e_Norwegian		= 20,
	INO_e_Romanian		= 21,
	INO_e_Serbian		= 22,
	INO_e_Slovak		= 23,
	INO_e_Slovenian		= 24,
	INO_e_Turkish		= 25,
	INO_e_Czech			= 26,
	INO_e_Hungarian		= 27,
	INO_e_Taiwanese		= 28,
	INO_e_MaxLangNb		= 29,
	INO_e_Dummy			= 0xFFFFFFFF
} INO_tden_LanguageId;

typedef struct	INO_tdst_LanguageName_
{
	char	az_ShortName[3];
	char	az_LongName[30];
} INO_tdst_LanguageName;

typedef enum
{
    INO_tden_RunMode_Undefined,
    INO_tden_RunMode_Demo,
    INO_tden_RunMode_Game
} INO_tden_RunMode;


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

extern INO_tdst_LanguageName	INO_dst_LanguageName[INO_e_MaxLangNb];
extern INO_tden_MasterType		INO_ge_MasterType;


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

extern void INO_InitModule(void);
extern void INO_CloseModule(void);
extern void INO_Update(void);
extern void INO_Reinit(void);

#ifdef JADEFUSION
extern BOOL INO_b_LanguageIsPresent(int i, BOOL bSound = FALSE);
#else
extern BOOL INO_b_LanguageIsPresent(int i);
#endif

#ifdef ACTIVE_EDITORS
extern char *INO_p_StoreLanguage(char *psz_Scan);
extern void INO_SaveLanguageFile(void);
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __INO_H__ */
