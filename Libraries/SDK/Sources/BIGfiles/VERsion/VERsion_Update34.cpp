/*$T VERsion_Update34.cpp GC! 1.097 01/29/02 15:53:42 */


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

#define SND_Cte_SModifierBasicPlayer	2
#define SND_Cte_SModifierFade			1
#define SND_Cte_SModifierFadeIn			3
#define SND_Cte_SModifierFadeOut		4


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

typedef struct	SND_tdst_SModifierExtPlayerA_
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
} SND_tdst_SModifierExtPlayerA;

typedef struct	SND_tdst_SModifierFade_
{
	float			f_StartVolume;
	float			f_EndVolume;
	float			f_Duration;
	float			f_Factor;
	unsigned int	ui_Type;
	float			f_Curr;
} SND_tdst_SModifierFade;

/*
 =======================================================================================================================
    Aim:    Update bigfile from version 33 to Version 34:: .smd volume -> x2
 =======================================================================================================================
 */
void VERsion_UpdateOneFileVersion34(char *_pc_Buf, ULONG _ul_Len, char *_psz_Path, char *_psz_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierChunk			*pst_SModifierHeader;
	SND_tdst_SModifierChunk			*pst_SModifierChunk;
	SND_tdst_SModifierBasic			*pv_Data;

	/* Extension of file */
	char							*psz_ext;
	char							az[256];

	SND_tdst_SModifierFade			*pst_Fade;
	SND_tdst_SModifierPlay			*pst_Play;

	SND_tdst_SModifierExtPlayerA		*pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Find extension of file:: .smd
	 -------------------------------------------------------------------------------------------------------------------
	 */

	psz_ext = strrchr(_psz_File, '.');

	if(psz_ext && !strcmp(psz_ext, EDI_Csz_ExtSModifier))
	{
		pst_SModifierHeader = (SND_tdst_SModifierChunk *) _pc_Buf;

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    check the file version
		 ---------------------------------------------------------------------------------------------------------------
		 */

		switch(pst_SModifierHeader->i_SModifierId)
		{
		case SND_Cte_SModifierFileVersion:
			break;
		default:
			sprintf(az, "Bad SModifier file (.smd) : %s//%s, version update canceled failed for this file", _psz_Path, _psz_File);
			ERR_X_Warning(0, az, NULL);
			return;
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    parse all SModifiers
		 ---------------------------------------------------------------------------------------------------------------
		 */

		pst_SModifierChunk = (SND_tdst_SModifierChunk *) (_pc_Buf + sizeof(SND_tdst_SModifierChunk));
		while(pst_SModifierChunk->i_SModifierId != -1)
		{
			if(pst_SModifierChunk->ui_DataSize)
			{
				pv_Data = (SND_tdst_SModifierBasic *) ((char *) pst_SModifierChunk + sizeof(SND_tdst_SModifierChunk));
				pv_Data->i_SndIndex = -1;
			}
			else
				pv_Data = NULL;

			/*$1
			 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			    each smodifier
			 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			 */

			switch(pst_SModifierChunk->i_SModifierId)
			{
			case SND_Cte_SModifierFade:
			case SND_Cte_SModifierFadeIn:
			case SND_Cte_SModifierFadeOut:
				pst_Fade = (SND_tdst_SModifierFade *) pv_Data;
				pst_Fade->f_EndVolume *= 2.0f;
				SND_M_VolumeSaturation(pst_Fade->f_EndVolume);
				pst_Fade->f_StartVolume *= 2.0f;
				SND_M_VolumeSaturation(pst_Fade->f_StartVolume);
				break;

			case SND_Cte_SModifierBasicPlayer:
				pst_Play = (SND_tdst_SModifierPlay *) pv_Data;
				pst_Play->f_Volume *= 2.0f;
				SND_M_VolumeSaturation(pst_Play->f_Volume);
				break;

			case SND_Cte_SModifierExtPlayer:
				pst_ExtPlay = (SND_tdst_SModifierExtPlayerA *) pv_Data;
				pst_ExtPlay->f_HightVolume *= 2.0f;
				SND_M_VolumeSaturation(pst_ExtPlay->f_HightVolume);
				pst_ExtPlay->f_LowVolume *= 2.0f;
				SND_M_VolumeSaturation(pst_ExtPlay->f_LowVolume);
				break;


			default:
				sprintf(az, "Unknown SModifier ID in file (.smd) : %s//%s", _psz_Path, _psz_File);
				ERR_X_Warning(0, az, NULL);
				break;
			}

			/*$1
			 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			    go to next one
			 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			 */

			pst_SModifierChunk = (SND_tdst_SModifierChunk *) ((char *) pst_SModifierChunk + sizeof(SND_tdst_SModifierChunk) + pst_SModifierChunk->ui_DataSize);
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Finally, we update the file
		 ---------------------------------------------------------------------------------------------------------------
		 */

		BIG_UpdateFileFromBuffer(_psz_Path, _psz_File, _pc_Buf, _ul_Len);
	}
}

#endif
