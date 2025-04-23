/*$T BIGopen.c GC! 1.098 11/29/00 15:15:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMpro.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGio.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGspecial.h"
#include "BIGfiles/VERsion/VERsion_Number.h"
#include "BIGfiles/VERsion/VERsion_Check.h"

#include "TIMer/PROfiler/PROPS2.h"

BIG_tdst_BigFile	BIG_gst;
BOOL				BIG_gb_CanOpenFats = TRUE;
extern BIG_tdst_BigFile BIG_gst1;


/*
 =======================================================================================================================
    Aim:    Call to open a bigfile on disk. This function makes all necessary reading, allocate fat and so on. This is
            the main function to call before accessing a bigfile.

    Note:   Only one bigfile can be open at the same time, cause all bigfiles functions use the BIG_gst global struct
            to get the bigfile infos.

    In:     _psz_FileName   Full path of bigfile to load.
 =======================================================================================================================
 */
bool BIG_Open(const char *_psz_FileName)
{
	int r;
	/* First init global bigfile struct */
	L_memset(&BIG_gst, 0, sizeof(BIG_tdst_BigFile));

	/* Open the bigfile on disk */
	
	/* EDITOR */
#if defined(ACTIVE_EDITORS)
	BIG_gst.h_CLibFileHandle = L_fopen(_psz_FileName, "r+bR");

    /* PSX2 AND CD */
#elif defined(PSX2_TARGET)

#if defined(PSX2_USE_iopCDV)
	BIG_gst.h_CLibFileHandle = eeCDV_i_OpenFile(_psz_FileName);
	gi_SpecialHandler = eeRPC_i_OpenBigfile();
	gi_SpecialHandler2 = eeRPC_i_OpenBigfile();
    /* PSX2 AND HOST */
#else
	BIG_gst.h_CLibFileHandle = L_fopen(_psz_FileName, L_fopen_RPB);
	gi_SpecialHandler = eeRPC_i_OpenBigfile();
	gi_SpecialHandler2 = eeRPC_i_OpenBigfile();

	/* OTHERS CASE */
#endif
#else

#ifdef  _XBOX 
	BIG_gst.h_CLibFileHandle = XBCompositeFile_Open(_psz_FileName, FILE_ATTRIBUTE_NORMAL, Gx8_FileError );
#  ifdef _XBOX_ASYNCLOAD
	{
		//extern BIGFileHandle gi_SpecialHandler;
		//gi_SpecialHandler = BIG_gst.h_CLibFileHandle;
	}
#  endif
#else
	BIG_gst.h_CLibFileHandle = L_fopen(_psz_FileName, "rbS" );
#endif

#endif 


	if(!CLI_FileOpen(BIG_gst.h_CLibFileHandle))
	{
		if(L_access(_psz_FileName, 2))
		{
			L_chmod(_psz_FileName, L_S_IWRITE);
#if defined(PSX2_USE_iopCDV)
			BIG_gst.h_CLibFileHandle = eeCDV_i_OpenFile(_psz_FileName);
#elif defined( _XBOX )
			BIG_gst.h_CLibFileHandle = XBCompositeFile_Open(_psz_FileName, FILE_ATTRIBUTE_NORMAL, Gx8_FileError );
#else
			BIG_gst.h_CLibFileHandle = L_fopen(_psz_FileName, L_fopen_RPB);
#endif			
		}
	}
#if defined(_XENON) && !defined(_FINAL_)
	if(!CLI_FileOpen(BIG_gst.h_CLibFileHandle))
	{
		char sz_Msg[256];
		sprintf(sz_Msg, "Unable to open file %s\n", _psz_FileName);
		OutputDebugString(sz_Msg);
	}
#endif
	r=CLI_FileOpen(BIG_gst.h_CLibFileHandle);
	ERR_X_Error(r, L_ERR_Csz_FOpen, _psz_FileName);
	if ( r == NULL )
		return false;

	/*
	 * L_setvbuf(BIG_gst.h_CLibFileHandle, NULL, L_IONBF, 0); £
	 * L_setvbuf(BIG_gst.h_CLibFileHandle, NULL, _IOFBF, 4000000);
	 */
	MEMpro_StartMemRaster();

	/* Read header of bigfile */
	BIG_ReadHeader();

	/* Read the fat */
	if(BIG_gb_CanOpenFats) BIG_ReadAllFats();

	MEMpro_StopMemRaster(MEMpro_Id_BIG_Open);

	/* Remember name of the bigfile */
	L_strcpy(BIG_gst.asz_Name, _psz_FileName);

	/* Check current version of bigfile */
	if ( !VERsion_CheckCurrent() )
	{
		BIG_Close();
		return false;
	}
	
	return true;
}

/*
 =======================================================================================================================
    Aim:    Call to close a bigfile. The bigfile closed must have been opened with BIG_Open function. The bigfile infos
            are retreived from BIG_gst global struct.
 =======================================================================================================================
 */
void BIG_Close(void)
{
	int r;
	/* Delete fat file, delete list of keys */
	BIG_ResetFat();

	/* In editor mode, delete additional fats */
#ifdef ACTIVE_EDITORS
	L_free(BIG_gst.dst_FileTableExt);
	L_free(BIG_gst.dst_DirTable);
#endif
	/* Really close the file */
#if defined(PSX2_USE_iopCDV)
	r=eeCDV_i_CloseFile(BIG_gst.h_CLibFileHandle);
	ERR_X_Error(r == 0, L_ERR_Csz_FClose, NULL);
#else
#if defined(_XENON)
	if (BIG_gst.h_CLibFileHandle)
	{
		r = L_fclose(BIG_gst.h_CLibFileHandle);
		ERR_X_Error(r, L_ERR_Csz_FClose, NULL);
	}
#else
	if(BIG_gst.h_CLibFileHandle)
	{
		r=L_fclose(BIG_gst.h_CLibFileHandle);
		ERR_X_Error(r == 0, L_ERR_Csz_FClose, NULL);
	}
#endif // _XENON
#endif // PSX2_USE_iopCDV
}

#ifdef ACTIVE_EDITORS

extern BIG_KEY	BIG_ul_UniverseKey;
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_CreateEmptyPriv(char *_psz_Name, ULONG _ul_Version, ULONG _ul_SizeFat, ULONG _ul_NumDirs, ULONG _ul_NumFiles, ULONG _b_EncryptFAT)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	BIG_tdst_FatDes st_Fat;
	char			*pc_Buf;
	int 			r;
	/*~~~~~~~~~~~~~~~~~~~*/

	if
	(
		_b_EncryptFAT 
	&&	(
			(!BIG_gst1.st_ToSave.ul_UniverseKey) 
		||	(BIG_gst1.st_ToSave.ul_UniverseKey == 0xFFFFFFFF)
		)
	)
	_b_EncryptFAT = FALSE;


_Try_
	/* Init the bigfile header */
	L_memset(&BIG_gst, 0, sizeof(BIG_tdst_BigFile));

	if(_b_EncryptFAT)
		L_memcpy(BIG_gst.st_ToSave.ac_Def, BIG_Csz_HeaderCrypted, 4);
	else
		L_memcpy(BIG_gst.st_ToSave.ac_Def, BIG_Csz_Header, 4);

	BIG_Version() = _ul_Version;
	BIG_Root() = BIG_C_InvalidIndex;
	BIG_MaxFile() = 0;
	BIG_MaxDir() = 0;
	BIG_FreeFile() = BIG_C_InvalidIndex;
	BIG_FreeDir() = BIG_C_InvalidIndex;
	BIG_SizeFat() = _ul_SizeFat < BIG_MAX_FAT ? BIG_MAX_FAT : _ul_SizeFat;
	BIG_NumFat() = 1;
	BIG_UniverseKey() = BIG_C_InvalidKey;

	/* Open the filename for writing */
	BIG_gst.h_CLibFileHandle = L_fopen(_psz_Name, L_fopen_WB);
	r=CLI_FileOpen(BIG_gst.h_CLibFileHandle);
	ERR_X_Error(r, L_ERR_Csz_FOpen, _psz_Name);

	/* Save the header */
	BIG_WriteHeader();

	/* Save the first fat */
	L_memset(&st_Fat, 0, sizeof(BIG_tdst_FatDes));
	st_Fat.ul_PosFat = sizeof(BIG_gst.st_ToSave) + sizeof(BIG_tdst_FatDes);
	st_Fat.ul_NextPosFat = (int) - 1;
	st_Fat.ul_FirstIndex = 0;
	st_Fat.ul_LastIndex = _ul_SizeFat - 1;
	st_Fat.ul_MaxFile = _ul_NumFiles;
	st_Fat.ul_MaxDir = _ul_NumDirs;

	if(_b_EncryptFAT)
	{
		pc_Buf = (char* )MEM_p_Alloc(sizeof(BIG_tdst_FatDes));
		L_memcpy(pc_Buf, &st_Fat, sizeof(BIG_tdst_FatDes));
		BIG_special_Encrypt4FAT(pc_Buf, sizeof(BIG_tdst_FatDes));
	}
	else
		pc_Buf = (char *) &st_Fat;
	r=BIG_fwrite((UCHAR *) pc_Buf, 
		sizeof(BIG_tdst_FatDes), 
		BIG_Handle());
	ERR_X_Error
	(
		r == 1, 
		L_ERR_Csz_FWrite, 
		NULL
	);

	if(_b_EncryptFAT)
		MEM_Free(pc_Buf);

	/* A big seek to "reserve" all the fat on disk */
	r=L_fseek
	(
		BIG_gst.h_CLibFileHandle,
		st_Fat.ul_PosFat + BIG_PosFatDir + (_ul_SizeFat * sizeof(BIG_gst.dst_DirTable[0].st_ToSave)),
		L_SEEK_SET
	);
	ERR_X_Error
	(
		r == 0,
		L_ERR_Csz_FSeek,
		NULL
	);

	/*
	 * Make a write access to force bigfile to be create with the previous seek. Do
	 * not really care of what is saved !
	 */
	BIG_fwrite(&BIG_gst.h_CLibFileHandle, 1, BIG_gst.h_CLibFileHandle);

	/* Close the bigfile */
	r=L_fclose(BIG_gst.h_CLibFileHandle);
	ERR_X_Error(r == 0, L_ERR_Csz_FClose, _psz_Name);
	BIG_gst.h_CLibFileHandle = NULL;

	/* Create root directory */
_Try_
	BIG_Open(_psz_Name);
	BIG_ul_CreateDir(BIG_Csz_Root);
	BIG_Close();
_Catch_
_EndThrow_
_Catch_
	if(CLI_FileOpen(BIG_gst.h_CLibFileHandle)) L_fclose(BIG_gst.h_CLibFileHandle);
_EndThrow_
}

/*
 =======================================================================================================================
    Aim:    Editor function to create an empty bigfile on disk. This function does not open the bigfile to read.

    In:     _psz_Name   Full name (on disk) of bigfile to create.
 =======================================================================================================================
 */
void BIG_CreateEmpty(char *_psz_Name)
{
	BIG_CreateEmptyPriv(_psz_Name, BIG_Cu4_Version, BIG_MAX_FAT, 0, 0, 0);
}

#endif
