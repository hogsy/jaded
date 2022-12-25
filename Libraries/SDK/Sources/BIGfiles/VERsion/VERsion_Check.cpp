/*$T VERsion_Check.cpp GC! 1.081 05/26/00 10:56:08 */


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
#include "BIGfiles/VERsion/VERsion_Check.h"
#include "BIGfiles/VERsion/VERsion_Update.h"
#include "BIGfiles/VERsion/VERsion_Number.h"
#include "BASe/CLIbrary/CLImem.h"
#include "EDIapp.h"
#include "LINKs/LINKmsg.h"
#include "EDImainframe.h"

BIG_INDEX	BIG_VERSION_ul_CurrentFile;
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VERsion_UpdateFile(ULONG _ul_Before, BIG_INDEX _ul_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_Path[BIG_C_MaxLenPath];
	char	*pc_Tmp;
	ULONG	ul_Length;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	BIG_ComputeFullName(BIG_ParentFile(_ul_File), asz_Path);
	pc_Tmp = BIG_pc_ReadFileTmpMustFree(BIG_PosFile(_ul_File), &ul_Length);
	BIG_VERSION_ul_CurrentFile = _ul_File;
	if(pc_Tmp) VERsion_UpdateOneFile(_ul_Before, pc_Tmp, ul_Length, asz_Path, BIG_NameFile(_ul_File));
	L_free(pc_Tmp);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VERsion_Change(ULONG _ul_Before, BIG_INDEX _ul_Dir)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Sub;
	char		asz_Msg[255];
	static int	i = 0;
	char		Tourne[8] = { '|', '/', '-', '\\', '|', '/', '-', '\\' };
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Recurse call for each dirs */
	ul_Sub = BIG_SubDir(_ul_Dir);
	while(ul_Sub != BIG_C_InvalidIndex)
	{
		VERsion_Change(_ul_Before, ul_Sub);
		ul_Sub = BIG_NextDir(ul_Sub);
	}

	/* Call for each files */
	ul_Sub = BIG_FirstFile(_ul_Dir);
	while(ul_Sub != BIG_C_InvalidIndex)
	{
		VERsion_UpdateFile(_ul_Before, ul_Sub);
		ul_Sub = BIG_NextFile(ul_Sub);
	}

	sprintf(asz_Msg, "Update BigFile Version %d to Version %d... %c", BIG_Version(), BIG_Version() + 1, Tourne[i]);
	if(EDI_gpo_EnterWnd)
		EDI_gpo_EnterWnd->DisplayMessage(asz_Msg);
	else
	{
		LINK_gb_CanLog = FALSE;
		LINK_PrintStatusMsg(asz_Msg);
		LINK_gb_CanLog = TRUE;
	}
	i = (i + 1) % 8;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void VERsion_CheckCurrent(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	int		i_Res;
	char	asz_Msg[512];
	/*~~~~~~~~~~~~~~~~~*/

	/* Same version, do nothing */
	if(BIG_Version() == BIG_Cu4_Version) return;

	/* Newer version, error */
	if(BIG_Version() >= BIG_Cu4_Version)
	{
		sprintf
		(
			asz_Msg,
			"Version of BigFile \"%s\" is invalid.\nYour version of Jade is too old.\nExit.",
			BIG_gst.asz_Name
		);
		ERR_X_ForceError(asz_Msg, NULL);
		exit(-1);
	}

	sprintf
	(
		asz_Msg,
		"BigFile \"%s\" must be update to version %d.\nIf you say YES, be sure that this is your BigFile AND NOT THE BASE.\nIf you say NO, Jade will exit.\nDo you want to update ?",
		BIG_gst.asz_Name,
		BIG_Cu4_Version
	);
	i_Res = M_MF()->MessageBox(asz_Msg, "Please Confirm", MB_ICONQUESTION | MB_YESNO);
	if(i_Res == IDNO) exit(-1);

	
	/* Change version */
	while(BIG_Version() != BIG_Cu4_Version)
	{

		// ----------------------------------------------------
		// HACK :
		// In order to be able to use Perforce has a source control for the BF,
		// we had to add some new variables to the structure BIG_tdst_FileExt.
		// To convert from version to 35 to version 36, we read the BF, and then we need
		// to do a Clean BF to recreate the BF with the proper FatFileExt structure.

		if ( BIG_Version() == 35)
		{
			// this is a special case because it updates the fat 
			VERsion_UpdateVersion36();
			M_MF()->MessageBox("FAT Conversion has been done for version 36, reopen the _convert.bf to continue\n Exiting...", "Operation succeded", MB_OK);
			exit(1);
			return;
		}

		VERsion_Change(BIG_Version(), BIG_Root());
		BIG_Version()++;
	}

	/* Finally write header of bigfile */
	BIG_Version() = BIG_Cu4_Version;

	BIG_WriteHeader();
}

#endif
