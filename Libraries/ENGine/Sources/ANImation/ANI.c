/*$T ANI.c GC 1.138 06/28/05 14:00:06 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/BASsys.h"
#include "BASE/MEMory/MEM.h"
#include "BIGfiles/IMPort/IMPbase.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGio.h"
#include "EDIpaths.h"
#include "ANIload.h"
#include "NIMtoTracks.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEload.h"
#include "AIinterp/Sources/Events/EVEsave.h"
#include "AIinterp/Sources/Events/EVEnt_interpolationkey.h"
#include "LINks/LINKstruct_reg.h"
#include "LINks/LINKstruct.h"

#if defined(PSX2_TARGET) && (__cplusplus)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS
extern BOOL ANI_b_ImportTRLCallBack(char *, char *, char *);
extern BOOL ANI_b_PlayedTrackList(EVE_tdst_ListTracks *, EVE_tdst_Data **, int *);
extern void ANI_MergeListTracks(EVE_tdst_Data *, EVE_tdst_ListTracks *, BOOL);

int			ANI_gi_ImportMode = -1;

int			IMP_AnimScale_Num = 0;
char		IMP_AnimScale_Names[16][256];
float		IMP_AnimScale_Values[16];

char		IMP_AnimKeepT_Names[32][256];
int			IMP_AnimKeepT_Gizmo[32][64];
int			IMP_AnimKeepT_Num = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ANI_b_ImportNIMCallback(char *_psz_BigPathName, char *_psz_BigFileName, char *_psz_ExternName)
{
	/*~~~~~~~~~~~~~~~*/
	char	*pc_Buffer;
	LONG	l_Size;
	L_FILE	h_File;
	ULONG	ul_File;
	/*~~~~~~~~~~~~~~~*/

	h_File = L_fopen(_psz_ExternName, "rb");
	if(!CLI_FileOpen(h_File)) return 0;
	L_fseek(h_File, 0, SEEK_END);
	l_Size = L_ftell(h_File);
	L_fseek(h_File, 0, SEEK_SET);

	pc_Buffer = (char*)L_malloc(l_Size);

	L_fread(pc_Buffer, 1, l_Size, h_File);
	L_fclose(h_File);
	BIG_UpdateFileFromBuffer(_psz_BigPathName, _psz_BigFileName, pc_Buffer, l_Size);
	L_free(pc_Buffer);

	ul_File = BIG_ul_SearchFileExt(_psz_BigPathName, _psz_BigFileName);
	if(ul_File != BIG_C_InvalidIndex) ANI_ConvertNIMfileToTracks(ul_File);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ char *L_FindSubString(char *_psz_SubString, char *_psz_String)
{
	/*~~~~~~~~~~~~~~*/
	char	*psz_Sub;
	char	*psz_Str;
	char	*psz_Str2;
	/*~~~~~~~~~~~~~~*/

	psz_Sub = _psz_SubString;
	psz_Str = _psz_String;

	while((*psz_Str))
	{
		psz_Str2 = psz_Str;
		while((*psz_Str2 == *psz_Sub) || (L_toupper(*psz_Str2) == L_toupper(*psz_Sub)))
		{
			psz_Str2++;
			psz_Sub++;

			if(!(*psz_Sub))
			{
				return _psz_SubString;
			}
		}

		psz_Str++;
		psz_Sub = _psz_SubString;
	}

	return (char *) NULL;
}

/*
 =======================================================================================================================
    Aim: Import a Sioux generated TRL into JADE. Note: Must take care of extra sound Tracks (-> Merge needed)
 =======================================================================================================================
 */
BOOL ANI_b_ImportTRLCallBack(char *_psz_BigPathName, char *_psz_BigFileName, char *_psz_ExternName)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_ListTracks *pst_OldListTracks;
	EVE_tdst_Data		*pst_SaveData, *pst_NewData;
	char				*pc_Buffer, *sz_Ext;
	char				asz_Log[512];
	LONG				l_Size;
	L_FILE				h_File;
	ULONG				ul_File;
	USHORT				uw_ExternalNumTracks, i;
	ULONG				ul_NbInstances;
	BOOL				b_Merge, b_OldExists, b_AlreadyLoaded;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Open TRL external file */
	h_File = L_fopen(_psz_ExternName, "rb");
	if(!CLI_FileOpen(h_File)) return 0;

	L_fseek(h_File, 0, SEEK_END);
	l_Size = L_ftell(h_File);
	L_fseek(h_File, 0, SEEK_SET);

	pc_Buffer = (char*)L_malloc(l_Size + 32);
	L_fread(pc_Buffer, 1, l_Size, h_File);
	L_fclose(h_File);

	/* The number of Tracks is stored in the first 2 bytes of a TRL File. */
	uw_ExternalNumTracks = *(USHORT *) pc_Buffer;

	b_Merge = FALSE;
	b_AlreadyLoaded = FALSE;
	pst_SaveData = NULL;

	/* Create temp data */
	pst_NewData = (EVE_tdst_Data *) MEM_p_Alloc(sizeof(EVE_tdst_Data));
	L_memset(pst_NewData, 0, sizeof(EVE_tdst_Data));

	/* Put extension into lower case */
	sz_Ext = strrchr(_psz_BigFileName, '.');
	if(sz_Ext)
	{
		while(*(++sz_Ext)) *sz_Ext = tolower(*sz_Ext);
	}

	/* Try to find the TRL in the current directory */
	ul_File = BIG_ul_SearchFileExt(_psz_BigPathName, _psz_BigFileName);

	if(ul_File == BIG_C_InvalidIndex)
	{
		/* The File dont exist. */
		b_OldExists = FALSE;
		b_AlreadyLoaded = FALSE;
		BIG_UpdateFileFromBuffer(_psz_BigPathName, _psz_BigFileName, pc_Buffer, l_Size);
		ul_File = BIG_ul_SearchFileExt(_psz_BigPathName, _psz_BigFileName);
	}
	else
	{
		/* The File exists. */
		b_OldExists = TRUE;

		/* Is it already loaded ? */
		pst_OldListTracks = (EVE_tdst_ListTracks *) LOA_ul_SearchAddress(BIG_PosFile(ul_File));
		if(pst_OldListTracks && (pst_OldListTracks != (EVE_tdst_ListTracks *) BIG_C_InvalidKey))
		{
			ul_NbInstances = pst_OldListTracks->ul_NbOfInstances;
			b_AlreadyLoaded = TRUE;
		}
		else
			b_AlreadyLoaded = FALSE;

		LOA_MakeFileRef
		(
			BIG_FileKey(ul_File),
			(ULONG *) &pst_OldListTracks,
			EVE_ul_CallbackLoadListTracks,
			LOA_C_MustExists
		);
		LOA_Resolve();

		if(!b_AlreadyLoaded) EVE_ResolveListTrackRef(pst_OldListTracks);

		if(ANI_b_PlayedTrackList(pst_OldListTracks, NULL, NULL))
		{
			ERR_X_Warning(0, "TRL cannot be imported when currently played.", _psz_BigFileName);
			MEM_Free(pst_NewData);
			return 0;
		}

		/* If there are more tracks in the Old List of Tracks, we set the b_Merge BOOL; */
		b_Merge = (pst_OldListTracks->uw_NumTracks != uw_ExternalNumTracks);

		if(b_Merge)
		{
			pst_SaveData = (EVE_tdst_Data *) MEM_p_Alloc(sizeof(EVE_tdst_Data));
			L_memset(pst_SaveData, 0, sizeof(EVE_tdst_Data));
			pst_SaveData->pst_ListTracks = (EVE_tdst_ListTracks *) MEM_p_Alloc(sizeof(EVE_tdst_ListTracks));
			L_memset(pst_SaveData->pst_ListTracks, 0, sizeof(EVE_tdst_ListTracks));
			ANI_MergeListTracks(pst_SaveData, pst_OldListTracks, 0);
			b_Merge = TRUE;
		}

		for(i = 0; i < pst_OldListTracks->uw_NumTracks; i++) EVE_FreeTrack(&pst_OldListTracks->pst_AllTracks[i]);

		MEM_Free(pst_OldListTracks->pst_AllTracks);
		LOA_DeleteAddress(pst_OldListTracks);
		LINK_DelRegisterPointer(pst_OldListTracks);
		MEM_Free(pst_OldListTracks);
	}

	/* Replace/Update the BigFile File with the External TRL. */
	BIG_UpdateFileFromBuffer(_psz_BigPathName, _psz_BigFileName, pc_Buffer, l_Size);

	/* Load the new file */
	LOA_MakeFileRef
	(
		BIG_FileKey(ul_File),
		(ULONG *) &(pst_NewData->pst_ListTracks),
		EVE_ul_CallbackLoadListTracks,
		LOA_C_MustExists
	);
	LOA_Resolve();

	if(!b_AlreadyLoaded) EVE_ResolveListTrackRef(pst_NewData->pst_ListTracks);

	EVE_InitData(pst_NewData, pst_NewData->pst_ListTracks);
	pst_NewData->pst_ListTracks->ul_NbOfInstances = 1;

	if(b_Merge) ANI_MergeListTracks(pst_NewData, pst_SaveData->pst_ListTracks, 1);
	LOA_AddAddress(ul_File, pst_NewData->pst_ListTracks);

	if(ANI_gi_ImportMode == 1)	/* No Optim Mode */
	{
		for(i = 0; i < pst_NewData->pst_ListTracks->uw_NumTracks; i++)
			pst_NewData->pst_ListTracks->pst_AllTracks[i].uw_Flags |= EVE_C_Track_Optimized;
	}

	/* Skip Translation Management */
	if(1)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		int				i, j, rank, length;
		BOOL			b_Found;
		EVE_tdst_Track	*pst_Track;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		rank = -1;
		for(i = 0; i < IMP_AnimKeepT_Num; i++)
		{
			length = L_strlen(&IMP_AnimKeepT_Names[i][0]);
			if(!L_strnicmp(_psz_BigFileName, &IMP_AnimKeepT_Names[i][0], length))
			{
				rank = i;
				break;
			}
		}

		/* We found info about this character in KeepTranslation.ini */
		for(i = 0; i < pst_NewData->pst_ListTracks->uw_NumTracks; i++)
		{
			pst_Track = &pst_NewData->pst_ListTracks->pst_AllTracks[i];

			if
			(
				!(pst_Track->pst_AllEvents)
			||	((EVE_w_Event_InterpolationKey_GetFlags(pst_Track->pst_AllEvents) & EVE_C_EventFlag_Type) != EVE_C_EventFlag_InterpolationKey)
			||	(EVE_w_Event_InterpolationKey_GetType(pst_Track->pst_AllEvents) & 4)
			||	(EVE_w_Event_InterpolationKey_GetType(pst_Track->pst_AllEvents) & 16)
			||	(pst_Track->uw_Gizmo == (USHORT) - 1)	/* Keep Magic Box Tracks */
			||	(pst_Track->uw_Gizmo == 0)				/* Keep Main Bone Tracks */
			) continue;

			b_Found = FALSE;
			j = 0;

			if(rank != -1)
			{
				while(IMP_AnimKeepT_Gizmo[rank][j] != -1)
				{
					if(pst_Track->uw_Gizmo == IMP_AnimKeepT_Gizmo[rank][j++])
					{
						b_Found = TRUE;
						break;
					}
				}
			}

			/* Gizmo is in KeepTranslation.ini file ... Keept it */
			if(b_Found) continue;

			EVE_FreeTrack(pst_Track);

			L_memcpy
			(
				&pst_NewData->pst_ListTracks->pst_AllTracks[i],
				&pst_NewData->pst_ListTracks->pst_AllTracks[i + 1],
				(pst_NewData->pst_ListTracks->uw_NumTracks - i - 1) * sizeof(EVE_tdst_Track)
			);
			pst_NewData->pst_ListTracks->uw_NumTracks--;

			i--;
		}
	}

	EVE_MysticalOptimization(NULL, pst_NewData->pst_ListTracks, 0.0f, 0.0f);

	/* Scale Management */
	{
		/*~~~~~~~~~~~~~~*/
		int		i, length;
		float	f_Scale;
		/*~~~~~~~~~~~~~~*/

		f_Scale = 1.0f;
		for(i = 0; i < IMP_AnimScale_Num; i++)
		{
			length = L_strlen(&IMP_AnimScale_Names[i][0]);

			if(!L_strnicmp(_psz_BigFileName, &IMP_AnimScale_Names[i][0], length))
			{
				f_Scale = IMP_AnimScale_Values[i];
				break;
			}
		}

		if(f_Scale != 1.0f)
		{
			EVE_ScaleAllTranslations(pst_NewData->pst_ListTracks, f_Scale);

			sprintf
			(
				asz_Log,
				"%s has been imported. [String \"%s\" found, Scale set to %f]",
				_psz_BigFileName,
				&IMP_AnimScale_Names[i][0],
				IMP_AnimScale_Values[i]
			);
			LINK_PrintStatusMsg(asz_Log);
		}
		else
		{
			sprintf(asz_Log, "%s has been imported. [No Scale Set]", _psz_BigFileName);
			LINK_PrintStatusMsg(asz_Log);
		}
	}

	/* Save Track list */
	EVE_SaveListTracks(NULL, pst_NewData->pst_ListTracks);

	if(!b_AlreadyLoaded)
		EVE_DeleteListTracks(pst_NewData->pst_ListTracks);
	else
		pst_NewData->pst_ListTracks->ul_NbOfInstances = ul_NbInstances;

	/* Free Temp Events Data. */
	if(pst_NewData->pst_ListParam) MEM_Free(pst_NewData->pst_ListParam);
	MEM_Free(pst_NewData);

	if(pst_SaveData)
	{
		if(pst_SaveData->pst_ListParam) MEM_Free(pst_SaveData->pst_ListParam);
		if(pst_SaveData->pst_ListTracks)
		{
			for(i = 0; i < pst_SaveData->pst_ListTracks->uw_NumTracks; i++)
				EVE_FreeTrack(&pst_SaveData->pst_ListTracks->pst_AllTracks[i]);
			MEM_Free(pst_SaveData->pst_ListTracks->pst_AllTracks);
			MEM_Free(pst_SaveData->pst_ListTracks);
		}

		MEM_Free(pst_SaveData);
	}

	L_free(pc_Buffer);

	return 0;
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ANI_InitModule(void)
{
#ifdef ACTIVE_EDITORS
	IMP_b_AddImportCallback("nim", ANI_b_ImportNIMCallback);
	IMP_b_AddImportCallback("trl", ANI_b_ImportTRLCallBack);
#endif
}

#if defined(PSX2_TARGET) && (__cplusplus)
}
#endif
