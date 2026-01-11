/*$T VAVview_sndkey.cpp GC! 1.097 06/20/01 16:09:34 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "DIAlogs/DIAvector_dlg.h"
#include "VAVview/VAVview.h"
#include "MATHs/MATH.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGfat.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "EDIpaths.h"

#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDwave.h"

#include "EDItors/Sources/SOuNd/SONframe.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "EDIeditors_infos.h"
#include "EDImsg.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CString EVAV_SndKey_DrawItem(EVAV_cl_ViewItem *_po_Data, void *_p_Value)
{
	/*~~~~~~~~~~~~~~~~~*/
	BIG_KEY		ul_Key;
	BIG_INDEX	ul_Index;
    char    asz_Temp[32];
    /*~~~~~~~~~~~~~~~~~*/

    if(
        (L_stricmp(_po_Data->masz_Name, "Edit") == 0)||
        (L_stricmp(_po_Data->masz_Name, "Find") == 0)
        )
    {
        sprintf(asz_Temp, "(Click on button)");
        return asz_Temp;
    }

	ul_Key = *(BIG_KEY *) _p_Value;
	if(ul_Key == BIG_C_InvalidKey) return "INVALID KEY";
	ul_Index = BIG_ul_SearchKeyToFat(ul_Key);
	if(ul_Index == BIG_C_InvalidIndex) return "KEY NOT EXISTS";
	return BIG_NameFile(ul_Index);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_SndKey_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File
						(
							"Choose File",
							0,
							0,
							1,
							NULL,
							"*" EDI_Csz_ExtSoundFile ",*" EDI_Csz_ExtLoadingSound ",*" EDI_Csz_ExtSModifier ",*" EDI_Csz_ExtSoundMusic ",*" EDI_Csz_ExtSoundAmbience ",*" EDI_Csz_ExtSoundDialog
						);
	CString				o_Temp;
	BIG_INDEX			ul_Index;
	ULONG				*pul_Flags;
	char				*pz_Temp, *psz_Name;
	char				asz_Path[1024];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    o_File.mb_UseFavorite = TRUE;
    if(_p_Value != 0)
    {
        ESON_cl_Frame*po_SoundEdi;
        EBRO_cl_Frame *po_Browser;
        ULONG ul_key;
        ULONG ul_fat;

        ul_key = *(BIG_KEY *) _p_Value;
        ul_fat= BIG_ul_SearchKeyToFat(ul_key);
        
        if((ul_key != -1) && (ul_fat!=-1))
        {
            if(L_stricmp(_po_Data->masz_Name, "Edit") == 0)
            {
                po_SoundEdi = (ESON_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_SOUND, 0);
				po_SoundEdi->mpo_MyView->IWantToBeActive(po_SoundEdi);
                po_SoundEdi->i_OnMessage( EDI_MESSAGE_SELFILE, BIG_ParentFile( ul_fat), ul_fat);
                return;
            }
            else if(L_stricmp(_po_Data->masz_Name, "Show in browser") == 0)
            {
                po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
				po_Browser ->mpo_MyView->IWantToBeActive(po_Browser);
                po_Browser->i_OnMessage( EDI_MESSAGE_SELFILE, BIG_ParentFile( ul_fat), ul_fat);
                return;
            }
        }
    }

	pul_Flags = (ULONG *) _po_Data->mi_Param1;

	/*
	 * _po_Data->mi_Param1 == &ul_Flag £
	 * _po_Data->mi_Param2 == libre pour l'instant
	 */
	if(o_File.DoModal() == IDOK)
	{
		o_File.GetItem(o_File.mo_File, 0, o_Temp);
		ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
		if(ul_Index == BIG_C_InvalidIndex)
			*(BIG_KEY *) _p_Value = BIG_C_InvalidKey;
		else
			*(BIG_KEY *) _p_Value = BIG_FileKey(ul_Index);

		if(pul_Flags)
		{
			*pul_Flags &= ~(SND_Cul_SF_LoadingSound| SND_Cul_SF_Dialog | SND_Cul_SF_Music | SND_Cul_SF_Ambience|SND_Cul_SF_SModifier);
			if(ul_Index != BIG_C_InvalidIndex)
			{
				BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
				psz_Name = BIG_NameFile(ul_Index);

				pz_Temp = L_strrchr(psz_Name, '.');
				if(pz_Temp)
				{
					if(!L_strnicmp(pz_Temp, EDI_Csz_ExtSoundMusic,4))
						*pul_Flags |= SND_Cul_SF_Music;
					else if(!L_strnicmp(pz_Temp, EDI_Csz_ExtSoundAmbience,4))
						*pul_Flags |= SND_Cul_SF_Ambience;
					else if(!L_strnicmp(pz_Temp, EDI_Csz_ExtSoundDialog,4))
						*pul_Flags |= SND_Cul_SF_Dialog;
					else if(!L_strnicmp(pz_Temp, EDI_Csz_ExtSModifier,4))
						*pul_Flags |= SND_Cul_SF_SModifier;
					else if(!L_strnicmp(pz_Temp, EDI_Csz_ExtLoadingSound,4))
						*pul_Flags |= SND_Cul_SF_LoadingSound;
				}
			}
		}
	}
}

#endif /* ACTIVE_EDITORS */
