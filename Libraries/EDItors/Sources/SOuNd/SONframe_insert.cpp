/*$T SONframe_insert.cpp GC 1.138 11/27/03 13:23:17 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

/*$2- editor ---------------------------------------------------------------------------------------------------------*/

#include "DIAlogs/DIAsndinsert_dlg.h"
#include "Res/Res.h"
#include "BROwser/BROframe.h"
#include "EDImsg.h"
#include "EDIpaths.h"
#include "SDK/Sources/BIGfiles/LOAding/LOAdefs.h"
#include "SDK/Sources/BIGfiles/BIGread.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "DIAlogs/DIAname_dlg.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "LINks/LINKtoed.h"

/*$2- sound ----------------------------------------------------------------------------------------------------------*/

#include "SouND/sources/SND.h"
#include "SouND/sources/SNDconst.h"
#include "SouND/sources/SNDwave.h"
#include "SouND/sources/SNDinsert.h"

/*$2- sound editor ---------------------------------------------------------------------------------------------------*/

#include "SONframe.h"
#include "SONview.h"


/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Insert_OnNew(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog("Enter a new insert file name");
	char				asz_Name[BIG_C_MaxLenPath];

	char				*psz_Name;
	BIG_INDEX			ul_New;
	SND_tdst_Insert		*pInsert;
	EDIA_cl_SndInsert	*poInsert;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_Dialog.DoModal() == IDOK)
	{
		psz_Name = (char *) (LPCSTR) o_Dialog.mo_Name;
		if(BIG_b_CheckName(psz_Name) == FALSE) return;

		L_strcpy(asz_Name, psz_Name);
		L_strcat(asz_Name, EDI_Csz_ExtSoundInsert);
		if(BIG_ul_SearchFileExt(EDI_Csz_Path_Audio, asz_Name) != BIG_C_InvalidIndex)
		{
			ERR_X_ForceError("Insert name already exists : ", psz_Name);
		}
		else
		{
			BIG_ul_CreateDir(EDI_Csz_Path_Audio);
			ul_New = BIG_ul_CreateFile(EDI_Csz_Path_Audio, asz_Name);

			/* Inform all editors */
			M_MF()->FatHasChanged();

			/* create empty file */
			pInsert = SND_p_InsertCreate(BIG_FileKey(ul_New), en_InsYaxisVol, en_InsXaxisTime, 2);
			SND_InsertSave(pInsert, BIG_FileKey(ul_New));
			

			/* dialog */ 
			poInsert = new EDIA_cl_SndInsert(pInsert);
			
			poInsert->DoModal();
			delete poInsert;

			/* unload */
			SND_InsertUnload(SND_M_GetInsertFromPtr(pInsert));
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Insert_OnOpen(ULONG _ulFat)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_SndInsert	*poInsert;
	CString				o_Temp;
	EDIA_cl_FileDialog	o_File("Choose File", 0, 0, 1, NULL, "*" EDI_Csz_ExtSoundInsert ",*" EDI_Csz_ExtSoundFade);
	int					ulFatIndex;
	SND_tdst_Insert		*pInsert;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ulFat != BIG_C_InvalidIndex)
	{
		if( !BIG_b_IsFileExtension(_ulFat, EDI_Csz_ExtSoundInsert) && !BIG_b_IsFileExtension(_ulFat, EDI_Csz_ExtSoundFade) )
			_ulFat = BIG_C_InvalidIndex;
	}

	if(_ulFat == BIG_C_InvalidIndex)
	{
		/*$1- get file name ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(o_File.DoModal() != IDOK) return;
		o_File.GetItem(o_File.mo_File, 0, o_Temp);

		/*$1- get file index/key ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ulFatIndex = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
	}
	else
	{
		ulFatIndex = _ulFat;
	}

	if(BIG_ul_GetLengthFile(BIG_PosFile(ulFatIndex)) == 0)
	{
		pInsert = SND_p_InsertCreate(BIG_FileKey(ulFatIndex), en_InsYaxisVol, en_InsXaxisTime, 2);
		SND_InsertSave(pInsert, BIG_FileKey(ulFatIndex));
		SND_InsertUnload(SND_M_GetInsertFromPtr(pInsert));
	}

	/*$1- get the pointer ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LOA_MakeFileRef(BIG_FileKey(ulFatIndex), (ULONG *) &pInsert, SND_ul_CallbackInsertLoad, LOA_C_MustExists|LOA_C_HasUserCounter);
	LOA_Resolve();

	/*$1- launch the dialog box ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	poInsert = new EDIA_cl_SndInsert(pInsert);
	poInsert->DoModal();
	delete poInsert;

	/*$1- detach the Insert TODO check que si modif dans la boîte de dialogue, alors l'index de insert est bien mis */

	SND_InsertUnload(SND_M_GetInsertFromPtr(pInsert));
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
