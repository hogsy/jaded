/*$T VERsion_Update35.cpp GC 1.138 12/09/03 12:09:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/VERsion/VERsion_Update.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGkey.h"


#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDmodifier.h"

#define SND_Cul_ESF_FaderGroup1			0x00000040
#define SND_Cul_ESF_FaderGroup2			0x00000080
#define SND_Cul_ESF_FaderGroup3			0x00000100
#define SND_Cul_ESF_FaderGroup4			0x00000200
#define SND_Cul_ESF_FaderGroup5			0x00000400
#define SND_Cte_SModifierFade			1
#define SND_Cte_SModifierFadeIn			3
#define SND_Cte_SModifierFadeOut		4


typedef struct	SND_tdst_SModifierPlay0_
{
	BIG_KEY ul_SndKey;
	ULONG	ul_SndFlags;
	int		i_SndIndex;
	int		i_Frequency;
	float	f_Volume;
	float	f_Near;
	float	f_Far;
} SND_tdst_SModifierPlay0;
typedef struct	SND_tdst_SModifierPlay1_
{
	BIG_KEY			ul_SndKey;
	ULONG			ul_SndFlags;
	int				i_SndIndex;
	int				i_Frequency;
	float			f_Volume;
	float			f_Near;
	float			f_Far;
	unsigned int	ui_SndExtFlags;
	unsigned int	ui_Version;
} SND_tdst_SModifierPlay1;

typedef struct	SND_tdst_SModifierExtPlayer0_
{
	BIG_KEY			ul_SndKey;
	ULONG			ul_SndFlags;
	int				i_SndIndex;
	unsigned int	ui_SndExtFlags;
	int				i_SndInstance;
	unsigned int	ui_PlayerFlag;
	float			f_HightVolume;
	float			f_LowVolume;
	float			f_FadeInDuration;
	float			f_FadeOutDuration;
	unsigned int	ui_Frequency;
	int				i_Pan;
	float			af_Near[3];
	float			af_Far[3];
	float			f_DeltaFar;
	int				i_Unused;
	int				i_SndTrack;
} SND_tdst_SModifierExtPlayer0;

typedef struct	SND_tdst_SModifierExtPlayer1_
{
	BIG_KEY			ul_SndKey;
	ULONG			ul_SndFlags;
	int				i_SndIndex;
	unsigned int	ui_SndExtFlags;
	int				i_SndInstance;
	unsigned int	ui_PlayerFlag;
	float			f_HightVolume;
	float			f_LowVolume;
	float			f_FadeInDuration;
	float			f_FadeOutDuration;
	unsigned int	ui_Frequency;
	int				i_Pan;
	float			af_Near[3];
	float			af_Far[3];
	float			f_DeltaFar;
	int				i_Unused;
	int				i_SndTrack;
	float			af_MiddleBlend[3];
	float			f_FarCoeff;
	float			f_MiddleCoeff;
	float			f_MinPan;
} SND_tdst_SModifierExtPlayer1;

typedef struct	SND_tdst_SModifierExtPlayer2_
{
	/* SModifier header */
	BIG_KEY			ul_SndKey;
	ULONG			ul_SndFlags;
	int				i_SndIndex;

	/* sound/instance data */
	unsigned int	ui_SndExtFlags;
	int				i_SndInstance;

	/* player data */
	unsigned int	ui_PlayerFlag;

	/* volume and fade */
	float			f_HightVolume;
	float			f_LowVolume;
	float			f_FadeInDuration;
	float			f_FadeOutDuration;

	/* Play settings */
	unsigned int	ui_Frequency;
	int				i_Pan;

	/* 3D settings */
	float			af_Near[3];
	float			af_Far[3];
	float			f_DeltaFar;

	int				i_Unused;
	int				i_SndTrack;

	float			af_MiddleBlend[3];
	float			f_FarCoeff;
	float			f_MiddleCoeff;
	float			f_MinPan;

	/* version */
	unsigned int	ui_Version;
} SND_tdst_SModifierExtPlayer2;

#define SND_Cte_SModifierBasicPlayer	2


typedef struct	SND_tdst_SModifierPlay_
{
	BIG_KEY			ul_SndKey;
	ULONG			ul_SndFlags;
	int				i_SndIndex;
	int				i_Frequency;
	float			f_Volume;
	float			f_Near;
	float			f_Far;
	unsigned int	ui_SndExtFlags;
	unsigned int	ui_Version;
	float			f_Delay;
} SND_tdst_SModifierPlay;

typedef struct	SND_tdst_SModifierBasic_
{
	BIG_KEY ul_SndKey;
	ULONG	ul_SndFlags;
	int		i_SndIndex;
} SND_tdst_SModifierBasic;

typedef struct	SND_tdst_SModifierExtPlayerB_
{
	/* SModifier header */
	BIG_KEY			ul_SndKey;
	ULONG			ul_SndFlags;
	int				i_SndIndex;

	/* sound/instance data */
	unsigned int	ui_SndExtFlags;
	int				i_SndInstance;

	/* player data */
	unsigned int	ui_PlayerFlag;

	/* volume and fade */
	float			f_HightVolume;
	float			f_LowVolume;
	float			f_FadeInDuration;
	float			f_FadeOutDuration;

	/* Play settings */
	unsigned int	ui_Frequency;
	int				i_Pan;

	/* 3D settings */
	float			af_Near[3];
	float			af_Far[3];
	float			f_DeltaFar;

	int				i_Unused;
	int				i_SndTrack;

	float			af_MiddleBlend[3];
	float			f_FarCoeff;
	float			f_MiddleCoeff;
	float			f_MinPan;

	/* version */
	unsigned int	ui_Version;

	/* play delay */
	float			f_Delay;
} SND_tdst_SModifierExtPlayerB;

/*
 =======================================================================================================================
    Aim: Update bigfile from version 34 to Version 35
 =======================================================================================================================
 */
void VERsion_UpdateOneFileVersion35(char *_pc_Buf, ULONG _ul_Len, char *_psz_Path, char *_psz_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierChunk		*pst_SModifierHeader;
	SND_tdst_SModifierChunk		*pst_SModifierChunk;
	SND_tdst_SModifierBasic		*pv_Data;
	/* Extension of file */
	char						*psz_ext;
	char						az[256];

	SND_tdst_SModifierPlay		*pst_Play;
	SND_tdst_SModifierExtPlayerB *pNewExtPlay;
	char						*pNew;
	BOOL b_Found=FALSE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Find extension of file:: .smd
	 -------------------------------------------------------------------------------------------------------------------
	 */

	psz_ext = strrchr(_psz_File, '.');
	if(!psz_ext || L_strcmp(psz_ext, EDI_Csz_ExtSModifier)) return;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    new file
	 -------------------------------------------------------------------------------------------------------------------
	 */

	pNew = (char*)malloc(3*sizeof(SND_tdst_SModifierChunk) + sizeof(SND_tdst_SModifierExtPlayerB));

	pst_SModifierHeader = (SND_tdst_SModifierChunk *) pNew;
	pst_SModifierHeader->i_SModifierId = SND_Cte_SModifierFileVersion;
	pst_SModifierHeader->ui_DataSize = 2*sizeof(SND_tdst_SModifierChunk) + sizeof(SND_tdst_SModifierExtPlayerB);

	pst_SModifierChunk = (SND_tdst_SModifierChunk *) (pNew + sizeof(SND_tdst_SModifierChunk));
	pst_SModifierChunk->i_SModifierId = SND_Cte_SModifierExtPlayer;
	pst_SModifierChunk->ui_DataSize = sizeof(SND_tdst_SModifierExtPlayerB);

	pNewExtPlay = (SND_tdst_SModifierExtPlayerB *) (pNew + 2*sizeof(SND_tdst_SModifierChunk));
	pNewExtPlay->ul_SndKey = -1;
	pNewExtPlay->ul_SndFlags = 0;
	pNewExtPlay->i_SndIndex = -1;
	pNewExtPlay->ui_SndExtFlags = 0;
	pNewExtPlay->i_SndInstance = -1;
	pNewExtPlay->ui_PlayerFlag = 0;
	pNewExtPlay->f_HightVolume = 1.0f;
	pNewExtPlay->f_LowVolume = 0.0f;
	pNewExtPlay->f_FadeInDuration = 0.0f;
	pNewExtPlay->f_FadeOutDuration = 0.0f;
	pNewExtPlay->ui_Frequency = 0;
	pNewExtPlay->i_Pan = 0;
	pNewExtPlay->af_Near[0] = 20.0f;
	pNewExtPlay->af_Near[1] = 20.0f;
	pNewExtPlay->af_Near[2] = 20.0f;
	pNewExtPlay->af_Far[0] = 80.0f;
	pNewExtPlay->af_Far[1] = 80.0f;
	pNewExtPlay->af_Far[2] = 80.0f;
	pNewExtPlay->f_DeltaFar = 2.0f;
	pNewExtPlay->i_Unused = 0;
	pNewExtPlay->i_SndTrack = -1;
	pNewExtPlay->af_MiddleBlend[0] = 0.5f;
	pNewExtPlay->af_MiddleBlend[1] = 0.5f;
	pNewExtPlay->af_MiddleBlend[2] = 0.5f;
	pNewExtPlay->f_FarCoeff = 0.0f;
	pNewExtPlay->f_MiddleCoeff = 0.5f;
	pNewExtPlay->f_MinPan = 0;
	pNewExtPlay->ui_Version = SND_Cte_SModifierExtPlayerVersion;
	pNewExtPlay->f_Delay = 0.0f;

	pst_SModifierChunk = (SND_tdst_SModifierChunk *)  (pNew + 2*sizeof(SND_tdst_SModifierChunk)+ sizeof(SND_tdst_SModifierExtPlayerB));
	pst_SModifierChunk->i_SModifierId = -1;
	pst_SModifierChunk->ui_DataSize = 0;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    parse all SModifiers
	 -------------------------------------------------------------------------------------------------------------------
	 */

	pst_SModifierChunk = (SND_tdst_SModifierChunk *) (_pc_Buf + sizeof(SND_tdst_SModifierChunk));
	while(pst_SModifierChunk->i_SModifierId != -1)
	{
		if(b_Found)
		{
			sprintf(az, "2nd pass for file (.smd) : %s//%s", _psz_Path, _psz_File);
			ERR_X_Warning(0, az, NULL);
		}

		if(pst_SModifierChunk->ui_DataSize)
		{
			pv_Data = (SND_tdst_SModifierBasic *) ((char *) pst_SModifierChunk + sizeof(SND_tdst_SModifierChunk));
			pv_Data->i_SndIndex = -1;
		}
		else
			pv_Data = NULL;

		/*$1
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		    each smodifier
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 */

		switch(pst_SModifierChunk->i_SModifierId)
		{

		/*$1
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 */

		case SND_Cte_SModifierFade:
		case SND_Cte_SModifierFadeIn:
		case SND_Cte_SModifierFadeOut:
			sprintf(az, "fade  for file (.smd) : %s//%s", _psz_Path, _psz_File);
			ERR_X_Warning(0, az, NULL);
			break;

		/*$1
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 */

		case SND_Cte_SModifierBasicPlayer:
			b_Found = TRUE;
			pst_Play = (SND_tdst_SModifierPlay *) pv_Data;
			
			pNewExtPlay->i_SndIndex = pst_Play->i_SndIndex;
			pNewExtPlay->f_HightVolume = pst_Play->f_Volume;
			pNewExtPlay->f_Delay = pst_Play->f_Delay;
			pNewExtPlay->ui_Frequency = (unsigned int) pst_Play->i_Frequency;
			pNewExtPlay->af_Near[0] = pst_Play->f_Near;
			pNewExtPlay->af_Far[0] = pst_Play->f_Far;
			pNewExtPlay->ui_SndExtFlags &= ~
				(
					SND_Cul_ESF_FaderGroup1 |
					SND_Cul_ESF_FaderGroup2 |
					SND_Cul_ESF_FaderGroup3 |
					SND_Cul_ESF_FaderGroup4 |
					SND_Cul_ESF_FaderGroup5
				);
			pNewExtPlay->ui_SndExtFlags |= pst_Play->ui_SndExtFlags &
				(
					SND_Cul_ESF_FaderGroup1 |
					SND_Cul_ESF_FaderGroup2 |
					SND_Cul_ESF_FaderGroup3 |
					SND_Cul_ESF_FaderGroup4 |
					SND_Cul_ESF_FaderGroup5
				);

			pNewExtPlay->ul_SndKey = pst_Play->ul_SndKey;
			pNewExtPlay->ul_SndFlags = pst_Play->ul_SndFlags;

			if(pst_SModifierChunk->ui_DataSize <= sizeof(SND_tdst_SModifierPlay1))
			{
				pNewExtPlay->f_Delay = 0.0f;
			}
			break;

		/*$1
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 */

		case SND_Cte_SModifierExtPlayer:
			b_Found = TRUE;
			L_memcpy(pNewExtPlay, pv_Data, sizeof(SND_tdst_SModifierExtPlayerB));

			switch(pst_SModifierChunk->ui_DataSize)
			{
			case sizeof(SND_tdst_SModifierExtPlayer0):
				pNewExtPlay->af_MiddleBlend[0] = 0.5f;
				pNewExtPlay->af_MiddleBlend[1] = 0.5f;
				pNewExtPlay->af_MiddleBlend[2] = 0.5f;
				pNewExtPlay->f_FarCoeff = 0.0f;
				pNewExtPlay->f_MiddleCoeff = 0.5f;
				pNewExtPlay->f_MinPan = 0.0f;

			case sizeof(SND_tdst_SModifierExtPlayer1):
				pNewExtPlay->ui_SndExtFlags &= ~
					(
						SND_Cul_ESF_FaderGroup1 |
						SND_Cul_ESF_FaderGroup2 |
						SND_Cul_ESF_FaderGroup3 |
						SND_Cul_ESF_FaderGroup4 |
						SND_Cul_ESF_FaderGroup5
					);
				pNewExtPlay->ui_SndExtFlags &= SND_Cul_ESF_MaskDynExtFlags;
				pNewExtPlay->ui_SndExtFlags |= SND_Cul_ESF_FaderGroup1;

			case sizeof(SND_tdst_SModifierExtPlayer2):
				pNewExtPlay->f_Delay = 0.0f;
				pNewExtPlay->ui_Version = SND_Cte_SModifierExtPlayerVersion;
				break;

			case sizeof(SND_tdst_SModifierExtPlayerB):
				break;

			default:
				sprintf(az, "Unknown SModifier version in file (.smd) : %s//%s", _psz_Path, _psz_File);
				ERR_X_Warning(0, az, NULL);
				break;
			}
			break;

		/*$1
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 */

		default:
			sprintf(az, "Unknown SModifier ID in file (.smd) : %s//%s", _psz_Path, _psz_File);
			ERR_X_Warning(0, az, NULL);
			break;
		}

		pst_SModifierChunk = (SND_tdst_SModifierChunk *) ((char *) pst_SModifierChunk + sizeof(SND_tdst_SModifierChunk) + pst_SModifierChunk->ui_DataSize);
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Finally, we update the file
	 -------------------------------------------------------------------------------------------------------------------
	 */

	BIG_UpdateFileFromBuffer(_psz_Path, _psz_File, pNew, pst_SModifierHeader->ui_DataSize+sizeof(SND_tdst_SModifierChunk));
	free(pNew);

}
#endif
