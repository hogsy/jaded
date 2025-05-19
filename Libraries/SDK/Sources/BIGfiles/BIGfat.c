/*$T BIGfat.c GC!1.52 11/10/99 12:28:27 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGio.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGerrid.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGmerge.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGspecial.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "LINks/LINKmsg.h"

#include "TIMer/PROfiler/PROPS2.h"

#include "../Main/Shared/FileSystem/FileSystem.h"

#ifdef ACTIVE_EDITORS
#include "VERsion/VERsion_Number.h"
#endif // ACTIVE_EDITORS

#define BIG_b_FAT_is_Crypted (L_strcmp(BIG_gst.st_ToSave.ac_Def, BIG_Csz_HeaderCrypted) == 0)

/*
 ===================================================================================================
 ===================================================================================================
 */
ULONG	BIG_ul_FAT_GetCryptKey()
{
	if(BIG_gst.st_ToSave.ul_UniverseKey && (BIG_gst.st_ToSave.ul_UniverseKey != 0xFFFFFFFF))
		return BIG_gst.st_ToSave.ul_UniverseKey;
	
#ifdef ACTIVE_EDITORS

	if(BIG_gst1.st_ToSave.ul_UniverseKey && (BIG_gst1.st_ToSave.ul_UniverseKey != 0xFFFFFFFF))
		return BIG_gst1.st_ToSave.ul_UniverseKey;
#endif	

	ERR_X_Assert(0);
	return 0;
}


/*
 ===================================================================================================
    Aim:    Read the header of the bigfile. The header is the first thing we can't find in a
            bigfile. See BIGdefs.h to see bigfile header struct.
 ===================================================================================================
 */
void BIG_ReadHeader(void)
{
	CHAR * pc_Buf;
	ULONG	ul_Key;	
	int r;
	
    /* Read header struct */
	r = BIG_fread(&BIG_gst.st_ToSave, sizeof(BIG_gst.st_ToSave), BIG_Handle());
    ERR_X_Error
    (
        r == 1,
        L_ERR_Csz_FRead,
        NULL
    );
   
	pc_Buf = (CHAR *) &BIG_gst.st_ToSave;

	LOA_ReadCharArray(&pc_Buf, BIG_gst.st_ToSave.ac_Def, 4);


	/* La FAT est cryptée .. je décrypte */
	if(BIG_b_FAT_is_Crypted)
	{
#ifdef _GAMECUBE	
		SwapDWord(&BIG_gst.st_ToSave.ul_UniverseKey);
#endif		
		
		ul_Key = BIG_ul_FAT_GetCryptKey();
		BIG_special_Decrypt4FAT(pc_Buf, sizeof(BIG_gst.st_ToSave) - 4);
	}

	BIG_gst.st_ToSave.ul_Version 		= LOA_ReadULong(&pc_Buf);
	BIG_gst.st_ToSave.ul_MaxFile 		= LOA_ReadULong(&pc_Buf);
	BIG_gst.st_ToSave.ul_MaxDir 		= LOA_ReadULong(&pc_Buf);
	BIG_gst.st_ToSave.ul_MaxKey 		= LOA_ReadULong(&pc_Buf);
	BIG_gst.st_ToSave.ul_Root 			= LOA_ReadULong(&pc_Buf);
	BIG_gst.st_ToSave.ul_FirstFreeFile 	= LOA_ReadULong(&pc_Buf);
	BIG_gst.st_ToSave.ul_FirstFreeDir 	= LOA_ReadULong(&pc_Buf);
	BIG_gst.st_ToSave.ul_SizeOfFat 		= LOA_ReadULong(&pc_Buf);
	BIG_gst.st_ToSave.ul_NumFat 		= LOA_ReadULong(&pc_Buf);
	BIG_gst.st_ToSave.ul_UniverseKey 	= LOA_ReadULong(&pc_Buf);

	/* En décryptant le Header, j'ai "décrypté" la clef qui est pourtant stockée en clair ... je la remets tel quel */
	if(BIG_b_FAT_is_Crypted)
	{
		BIG_gst.st_ToSave.ul_UniverseKey = ul_Key;
	}


    /*
     * Test mark at the beginning of the bigfile to be sure that this is really a bigfile, and
     * not a dummy file !
     */
    ERR_X_Error
    (
        ((!L_strcmp(BIG_gst.st_ToSave.ac_Def, BIG_Csz_Header)) || (!L_strcmp(BIG_gst.st_ToSave.ac_Def, BIG_Csz_HeaderCrypted))),
        ERR_BIG_Csz_BigFileCorrupt,
        NULL
    );
}

/*
 ===================================================================================================
    Aim:    Read fat of file. In engine mode, the fat is not read in memory. It is read in a
            temporary buffer only to retreive keys of each file. Then the table of all keys is
            construct, and the temporary buffer is released. In editor mode, the fat is really read
            an kept in memory, cause editors can have to retreive the key or the position of a file
            depending of its name.
 ===================================================================================================
 */
static void BIG_ReadFatFile(BIG_tdst_FatDes *_pst_Fat)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* No file in fat. No need to continue. */
    if(_pst_Fat->ul_MaxFile == 0) return;

    /* Seek to the beginning of extended fat */
	i = L_fseek(BIG_Handle(), _pst_Fat->ul_PosFat + BIG_PosFatFile, L_SEEK_SET);
    ERR_X_Error(i == 0,L_ERR_Csz_FSeek,NULL);

    /* Read the fats (read totally in editor mode) */
    i = BIG_fread
    (
        (UCHAR *) (&BIG_gst.dst_FileTable[_pst_Fat->ul_FirstIndex]),
        _pst_Fat->ul_MaxFile * sizeof(BIG_tdst_File),
        BIG_Handle()
    );
    ERR_X_Error(i == 1,L_ERR_Csz_FRead,NULL);

	/* La FAT est cryptée .. je décrypte */
	if(BIG_b_FAT_is_Crypted)
	{
		BIG_special_Decrypt4FAT
		(
			(char *) (&BIG_gst.dst_FileTable[_pst_Fat->ul_FirstIndex]), 
			_pst_Fat->ul_MaxFile * sizeof(BIG_tdst_File)
		);	
	}

    /* Insert keys in table */
    for(i = _pst_Fat->ul_FirstIndex; i < _pst_Fat->ul_FirstIndex + _pst_Fat->ul_MaxFile; i++)
    {
        if(BIG_gst.dst_FileTable[i].ul_Key != BIG_C_InvalidKey)
        {
            BIG_InsertKeyToFat(BIG_gst.dst_FileTable[i].ul_Key, i);
        }
    }
}

#ifdef ACTIVE_EDITORS

/*
 ===================================================================================================
    Aim:    In editor mode, load the extended fat file.
 ===================================================================================================
 */
void BIG_ReadFatFileExt(BIG_tdst_FatDes *_pst_Fat)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   i,r;
	char	*p;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Fat->ul_MaxFile) return;

    /* Seek to the beginning of extended fat */
#if defined(_XBOX) || defined(_XENON)
	r = L_fseek(BIG_Handle(), _pst_Fat->ul_PosFat + BIG_PosFatFileExt, L_SEEK_SET);
    ERR_X_Error( r != L_SEEK_ERROR,L_ERR_Csz_FSeek,NULL);
#else // _XBOX
	r=L_fseek(BIG_Handle(), _pst_Fat->ul_PosFat + BIG_PosFatFileExt, L_SEEK_SET);
    ERR_X_Error( r == 0, L_ERR_Csz_FSeek, NULL );
#endif // _XBOX
	
	// -------------------------------------------------------
	// NOTE:
	// Some new variables have been added in BIG_tdst_FileExt structure.
	// To be able to read the FatFileExt in a BF prior to version 36, we 
	// need to remove those variables from the calculation of the size 
	// of the BIG_tdst_FileExt structure. 
	if ( BIG_Version() < BIG_FATChanged_Version36 )
	{
		UINT StructToSaveSize = sizeof(BIG_gst.dst_FileTableExt[0].st_ToSave);
		UINT oldStructToSaveSize = StructToSaveSize-sizeof(BIG_gst.dst_FileTableExt[0].st_ToSave.ul_P4RevisionClient);
#ifdef JADEFUSION
		p = (char* )L_malloc(_pst_Fat->ul_MaxFile * oldStructToSaveSize);
#else
		p = L_malloc(_pst_Fat->ul_MaxFile * oldStructToSaveSize);
#endif		
		r = BIG_fread(p, _pst_Fat->ul_MaxFile * oldStructToSaveSize, BIG_Handle());
		ERR_X_Error	(r == 1, L_ERR_Csz_FRead, NULL	);
		
		L_memset(&(BIG_gst.dst_FileTableExt[_pst_Fat->ul_FirstIndex]), 0, _pst_Fat->ul_MaxFile * (sizeof(BIG_tdst_FileExt)));
		for(i = _pst_Fat->ul_FirstIndex; i < _pst_Fat->ul_FirstIndex + _pst_Fat->ul_MaxFile; i++)
		{
			L_memcpy(&(BIG_gst.dst_FileTableExt[i].st_ToSave), (p + (oldStructToSaveSize * (i - _pst_Fat->ul_FirstIndex))), oldStructToSaveSize);
			BIG_gst.dst_FileTableExt[i].st_ToSave.ul_P4RevisionClient = 0;
		}
		L_free(p);
	}
	else
	{
#ifdef JADEFUSION
		p = (char*)L_malloc(_pst_Fat->ul_MaxFile * sizeof(BIG_gst.dst_FileTableExt[0].st_ToSave));
#else
		p = L_malloc(_pst_Fat->ul_MaxFile * sizeof(BIG_gst.dst_FileTableExt[0].st_ToSave));
#endif
		r = BIG_fread
			(
				p,
				_pst_Fat->ul_MaxFile * sizeof(BIG_gst.dst_FileTableExt[0].st_ToSave),
				BIG_Handle()
			);

		ERR_X_Error( r == 1,L_ERR_Csz_FRead,NULL);

		/* La FAT est cryptée .. je décrypte */
		if(BIG_b_FAT_is_Crypted)
		{
			BIG_special_Decrypt4FAT
			(
				p, 
				_pst_Fat->ul_MaxFile * sizeof(BIG_gst.dst_FileTableExt[0].st_ToSave)
			);
		}

		L_memset(&(BIG_gst.dst_FileTableExt[_pst_Fat->ul_FirstIndex]), 0, _pst_Fat->ul_MaxFile * sizeof(BIG_tdst_FileExt));
		for(i = _pst_Fat->ul_FirstIndex; i < _pst_Fat->ul_FirstIndex + _pst_Fat->ul_MaxFile; i++)
		{
			L_memcpy(&(BIG_gst.dst_FileTableExt[i].st_ToSave), p + (sizeof(BIG_gst.dst_FileTableExt[0].st_ToSave) * (i - _pst_Fat->ul_FirstIndex)), sizeof(BIG_gst.dst_FileTableExt[0].st_ToSave));

			// Keep copy revision stored in BF to be able to compared to revision on P4 before it is flushed when validating BF with Perforce
			BIG_TmpRevisionClient(i) = BIG_P4RevisionClient(i);
		}
		L_free(p);
	}
#if 0
    /*
     * Read file one per one, cause the fat saved in the bigfile is not the exact copy of the
     * structure in memory.
     */
    for(i = _pst_Fat->ul_FirstIndex; i < _pst_Fat->ul_FirstIndex + _pst_Fat->ul_MaxFile; i++)
    {
        L_memset(&(BIG_gst.dst_FileTableExt[i]), 0, sizeof(BIG_tdst_FileExt));
        r = BIG_fread
        (
            (UCHAR *) &(BIG_gst.dst_FileTableExt[i].st_ToSave),
            sizeof(BIG_gst.dst_FileTableExt[i].st_ToSave),
            BIG_Handle()
        );
        ERR_X_Error(r == 1, L_ERR_Csz_FRead,NULL);

		if(BIG_b_FAT_is_Crypted)
		{
			BIG_special_Decrypt4FAT
			(
#ifdef JADEFUSION
				(char *) &(BIG_gst.dst_FileTableExt[i].st_ToSave), 
#else
				(UCHAR *) &(BIG_gst.dst_FileTableExt[i].st_ToSave), 
#endif
				sizeof(BIG_gst.dst_FileTableExt[i].st_ToSave)
			);
		}

    }
#endif
}

/*
 ===================================================================================================
    Aim:    In editor mode, load the fat of directories.
 ===================================================================================================
 */
void BIG_ReadFatDir(BIG_tdst_FatDes *_pst_Fat)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   i,r;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Fat->ul_MaxDir) return;

    /* Seek to the beginning of the fat in the bigfile */
#if defined(_XBOX) || defined(_XENON)
	r = L_fseek(BIG_Handle(), _pst_Fat->ul_PosFat + BIG_PosFatDir, L_SEEK_SET);
    ERR_X_Error(r != L_SEEK_ERROR,L_ERR_Csz_FSeek,NULL);
#else // _XBOX

	if ( BIG_Version() < 36 )
	{
		// -------------------------------------------------------
		// NOTE:
		// Some new variables have been added in BIG_tdst_FileExt.st_ToSave structure.
		// To be able to seek to the proper position for FatDir in a BF prior 
		// to version 36, we need to remove those variables from the calculation
		// of the size of the BIG_tdst_FileExt.st_ToSave structure. 
		UINT oldStructToSaveSize = sizeof(BIG_gst.dst_FileTableExt[0].st_ToSave)-sizeof(BIG_gst.dst_FileTableExt[0].st_ToSave.ul_P4RevisionClient);
		ULONG PosFatDir = BIG_PosFatFileExt + (BIG_SizeFat() * oldStructToSaveSize);
		r = L_fseek(BIG_Handle(), _pst_Fat->ul_PosFat + PosFatDir, L_SEEK_SET);
		ERR_X_Error(r == 0,L_ERR_Csz_FSeek,NULL);
	}
	else
	{
		r = L_fseek(BIG_Handle(), _pst_Fat->ul_PosFat + BIG_PosFatDir, L_SEEK_SET);
		ERR_X_Error(r == 0,L_ERR_Csz_FSeek,NULL);
	}
#endif // _XBOX

    /*
     * Read dir one per one, cause the fat saved in the bigfile is not the exact copy of the
     * structure in memory.
     */
    for(i = _pst_Fat->ul_FirstIndex; i < _pst_Fat->ul_FirstIndex + _pst_Fat->ul_MaxDir; i++)
    {		
		L_memset(&(BIG_gst.dst_DirTable[i]), 0, sizeof(BIG_tdst_Directory));

        r = BIG_fread
        (
            (UCHAR *) &(BIG_gst.dst_DirTable[i].st_ToSave),
            sizeof(BIG_gst.dst_DirTable[i].st_ToSave),
            BIG_Handle()
        );
        ERR_X_Error(r == 1,L_ERR_Csz_FRead,NULL);

		if(BIG_b_FAT_is_Crypted)
		{
			BIG_special_Decrypt4FAT
			(
#ifdef JADEFUSION
				(char *) &(BIG_gst.dst_DirTable[i].st_ToSave), 
#else
				(UCHAR *) &(BIG_gst.dst_DirTable[i].st_ToSave), 
#endif
				sizeof(BIG_gst.dst_DirTable[i].st_ToSave)
			);
		}

    }
}

#endif

/*
 ===================================================================================================
    Aim:    To read all fats. Must be called just after the header.
 ===================================================================================================
 */
void BIG_ReadAllFats(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   i,r;
    CHAR * pc_Buf;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Allocate table for all fats */
#ifdef ACTIVE_EDITORS
    BIG_gst.dst_FatTable = (BIG_tdst_FatDes*)L_malloc(BIG_NumFat() * sizeof(BIG_tdst_FatDes));
#else
    BIG_gst.dst_FatTable = (BIG_tdst_FatDes*)MEM_p_Alloc(BIG_NumFat() * sizeof(BIG_tdst_FatDes));
#endif
    ERR_X_Error(BIG_gst.dst_FatTable != NULL, L_ERR_Csz_NotEnoughMemory, NULL);

    /* Read all fat des */
    for(i = 0; i < BIG_NumFat(); i++)
    {
        /* Read one fat */
		r=BIG_fread
        (
            (UCHAR *) &(BIG_gst.dst_FatTable[i]),
            sizeof(BIG_gst.dst_FatTable[i]),
            BIG_Handle()
        );
        ERR_X_Error(r == 1,L_ERR_Csz_FRead,NULL);

		if(BIG_b_FAT_is_Crypted)
		{
			BIG_special_Decrypt4FAT
			(
#ifdef JADEFUSION
				(char *) &(BIG_gst.dst_FatTable[i]), 
#else
				(UCHAR *) &(BIG_gst.dst_FatTable[i]), 
#endif
				sizeof(BIG_gst.dst_FatTable[i])
			);
		}

		pc_Buf = (CHAR *) &(BIG_gst.dst_FatTable[i]);
		BIG_gst.dst_FatTable[i].ul_MaxFile 		= LOA_ReadULong(&pc_Buf);
		BIG_gst.dst_FatTable[i].ul_MaxDir 		= LOA_ReadULong(&pc_Buf);
		BIG_gst.dst_FatTable[i].ul_PosFat 		= LOA_ReadULong(&pc_Buf);
		BIG_gst.dst_FatTable[i].ul_NextPosFat 	= LOA_ReadULong(&pc_Buf);
		BIG_gst.dst_FatTable[i].ul_FirstIndex 	= LOA_ReadULong(&pc_Buf);
		BIG_gst.dst_FatTable[i].ul_LastIndex 	= LOA_ReadULong(&pc_Buf);
		
        /* Goes to next fat */
        if((int) BIG_gst.dst_FatTable[i].ul_NextPosFat != -1)
        {
			r=L_fseek
	            (
	                BIG_Handle(),
	                BIG_gst.dst_FatTable[i].ul_NextPosFat - sizeof(BIG_tdst_FatDes),
	                L_SEEK_SET
	            );
            ERR_X_Error(r == 0,L_ERR_Csz_FSeek,NULL);
        }
    }

    /* Allocate all fats */
    BIG_gst.dst_FileTable = (BIG_tdst_File*)L_malloc(BIG_NumFat() * BIG_SizeFat() * sizeof(BIG_tdst_File));
    ERR_X_Error(BIG_gst.dst_FileTable != NULL, L_ERR_Csz_NotEnoughMemory, NULL);

    BIG_gst.dst_FileTableExt = (BIG_tdst_FileExt*)L_malloc(BIG_NumFat() * BIG_SizeFat() * sizeof(BIG_tdst_FileExt));
    ERR_X_Error(BIG_gst.dst_FileTableExt != NULL, L_ERR_Csz_NotEnoughMemory, NULL);

    BIG_gst.dst_DirTable = (BIG_tdst_Directory*)L_malloc(BIG_NumFat() * BIG_SizeFat() * sizeof(BIG_tdst_Directory));
    ERR_X_Error(BIG_gst.dst_DirTable != NULL, L_ERR_Csz_NotEnoughMemory, NULL);

    /*
     * In editor mode, we need to associated keys to the corresponding position in the fat. This
     * table is named st_KeyTableToFat.
     */
#ifdef ACTIVE_EDITORS
    BAS_binit(&BIG_gst.st_KeyTableToFat, 0);
    BIG_gst.st_KeyTableToFat.gran = 200;
    BIG_gst.st_KeyTableToFat.size = BIG_NumFat() * BIG_SizeFat();
    BIG_gst.st_KeyTableToFat.base = (BAS_tdst_Key*)L_malloc(BIG_gst.st_KeyTableToFat.size * sizeof(BAS_tdst_Key));
    ERR_X_Error(BIG_gst.st_KeyTableToFat.base != NULL, L_ERR_Csz_NotEnoughMemory, NULL);

    BAS_binit(&BIG_gst.st_PosTableToFat, 0);
    BIG_gst.st_PosTableToFat.gran = 200;
    BIG_gst.st_PosTableToFat.size = BIG_NumFat() * BIG_SizeFat();
    BIG_gst.st_PosTableToFat.base = (BAS_tdst_Key*)L_malloc(BIG_gst.st_PosTableToFat.size * sizeof(BAS_tdst_Key));
    ERR_X_Error(BIG_gst.st_PosTableToFat.base != NULL, L_ERR_Csz_NotEnoughMemory, NULL);
#endif
    /*
     * Allocate key table. In editor mode, we allocate one key per possible file (BIG_MAX_FAT)
     * cause we will create keys later. In engine mode, we only allocate necessary memory to
     * receive the keys that are really in the bigfile.
     */
#ifndef ACTIVE_EDITORS
    BAS_binit(&BIG_gst.st_KeyTableToPos, 0);
    BIG_gst.st_KeyTableToPos.gran = 200;
    BIG_gst.st_KeyTableToPos.size = BIG_MaxFile() + 1;
    BIG_gst.st_KeyTableToPos.base = (BAS_tdst_Key *) MEM_p_VMAlloc(BIG_gst.st_KeyTableToPos.size * sizeof(BAS_tdst_Key));
    ERR_X_Error(BIG_gst.st_KeyTableToPos.base != NULL, L_ERR_Csz_NotEnoughMemory, NULL);
#endif

    /* Will be reinit by loading */
    BIG_MaxKey() = 0;

    /* Read all fats */
	BAS_bsortmode = FALSE;
    for(i = 0; i < BIG_NumFat(); i++)
    {
        /*
         * Read fat to create the list of file keys. In pure engine mode, the fat of files is not
         * in memory.
         */
        BIG_ReadFatFile(&BIG_gst.dst_FatTable[i]);

        /*
         * In editor mode, load extended fat for files (that contain editors informations for
         * each file in the bigfile), and loads fat of directories. This is useless in engine
         * mode.
         */
#ifdef ACTIVE_EDITORS
        BIG_ReadFatFileExt(&BIG_gst.dst_FatTable[i]);
        BIG_ReadFatDir(&BIG_gst.dst_FatTable[i]);
#endif
    }

	/* Sort array */
	BAS_bsortmode = TRUE;
#ifdef ACTIVE_EDITORS
	BAS_bsort(&BIG_gst.st_KeyTableToFat);
	BAS_bsort(&BIG_gst.st_PosTableToFat);
#else
	BAS_bsort(&BIG_gst.st_KeyTableToPos);
#endif

#ifndef ACTIVE_EDITORS

    /* Free temporary buffer */
    MEM_Free(BIG_gst.dst_FileTable);
    BIG_gst.dst_FileTable = NULL;
#endif
}

/*
 ===================================================================================================
    Aim:    To clear allocations due to fat file. Free table of keys, and in editor mode, free fat
            of files.
 ===================================================================================================
 */
void BIG_ResetFat(void)
{
    /* Key table */
#ifndef ACTIVE_EDITORS
    BAS_bfree(&BIG_gst.st_KeyTableToPos);
#endif

    /* Fat table */
#ifdef ACTIVE_EDITORS
    L_free(BIG_gst.dst_FatTable);
#else
    MEM_Free(BIG_gst.dst_FatTable);
#endif
    BIG_gst.dst_FatTable = NULL;

    /* Editor fats */
#ifdef ACTIVE_EDITORS
    BAS_bfree(&BIG_gst.st_KeyTableToFat);
    BAS_bfree(&BIG_gst.st_PosTableToFat);
    L_free(BIG_gst.dst_FileTable);
    BIG_gst.dst_FileTable = NULL;
#endif
}

/*$4
 ***************************************************************************************************
    Basic save functions (update fat)
 ***************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 ===================================================================================================
    Aim:    To save the header of a bigfile on disk. This is used to update infos in header when
            the bigfile changed (one more file etc...).
 ===================================================================================================
 */
void BIG_WriteHeader(void)
{
	UCHAR	*pc_Buf;
	ULONG	ul_Save,r;

    /* Seek to beginning of file */
	r = L_fseek(BIG_Handle(), 0, L_SEEK_SET);
    ERR_X_Error(r == 0, L_ERR_Csz_FSeek, NULL);

	/* Marque indiquant que la FAT est cryptée */
	if(!L_strcmp(BIG_gst.st_ToSave.ac_Def, BIG_Csz_HeaderCrypted))
	{
#ifdef JADEFUSION
		pc_Buf = (UCHAR* )MEM_p_Alloc(sizeof(BIG_gst.st_ToSave));
#else
		pc_Buf = MEM_p_Alloc(sizeof(BIG_gst.st_ToSave));
#endif
		L_memcpy(pc_Buf, (UCHAR *) &(BIG_gst.st_ToSave), sizeof(BIG_gst.st_ToSave));

		/* Universe Key is used to crypt the FAT */
		ul_Save = BIG_ul_FAT_GetCryptKey();

		BIG_special_Encrypt4FAT
		(
#ifdef JADEFUSION
		(char* )(pc_Buf + 4), 
#else
			pc_Buf + 4, 
#endif
			sizeof(BIG_gst.st_ToSave) - 4
		);

		/* Key has been encrypted ... resave it decrypted */
		((BIG_tdst_BigFile *)pc_Buf)->st_ToSave.ul_UniverseKey = ul_Save;
	}
	else
		pc_Buf = (UCHAR *) &(BIG_gst.st_ToSave);


    /* Write save struct */
    r=BIG_fwrite
    (
        pc_Buf,
        sizeof(BIG_gst.st_ToSave),
        BIG_Handle()
    );
    ERR_X_Error(r == 1,L_ERR_Csz_FWrite,NULL);

	if(!L_strcmp(BIG_gst.st_ToSave.ac_Def, BIG_Csz_HeaderCrypted))
	{
		MEM_Free(pc_Buf);
	}
}

/*
 ===================================================================================================
    Aim:    Convert an index in fat into an index in fat des (for a given index, determins which is
            the corresponding fat).

    In:     _ul_Index   Bigfile index in fat to convert. 

    Out:    Return the fat des index (in BIG_gst.dst_FatTable table).
 ===================================================================================================
 */
ULONG BIG_IndexToFatDes(BIG_INDEX _ul_Index)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   ul_Fat;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(ul_Fat = 0; ul_Fat < BIG_NumFat(); ul_Fat++)
    {
        if(_ul_Index <= BIG_gst.dst_FatTable[ul_Fat].ul_LastIndex) break;
    }

    return ul_Fat;
}

/*
 ===================================================================================================
    Aim:    Call to update on file in fat of bigfile. This will update normal fat and extended one.

    In:     _ul_Index   Index of file in fat.
 ===================================================================================================
 */
void BIG_UpdateOneFileInFat(BIG_INDEX _ul_Index)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG           ul_Fat,r;
    BIG_tdst_FatDes *pst_Fat;
	char			*pc_Buf;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Search the good fat in bigfile */
    ul_Fat = BIG_IndexToFatDes(_ul_Index);
    pst_Fat = &BIG_gst.dst_FatTable[ul_Fat];

    /* Seek to the right pos in fat file */
	r = L_fseek
        (
            BIG_Handle(),
            pst_Fat->ul_PosFat + BIG_PosFatFile +
                ((_ul_Index - pst_Fat->ul_FirstIndex) * sizeof(BIG_tdst_File)),
            L_SEEK_SET
        );
    ERR_X_Error(r == 0,L_ERR_Csz_FSeek,NULL);

	/* Marque indiquant que la FAT est cryptée */
	if(!L_strcmp(BIG_gst.st_ToSave.ac_Def, BIG_Csz_HeaderCrypted))
	{
#ifdef JADEFUSION
		pc_Buf = (char* )MEM_p_Alloc(sizeof(BIG_tdst_File));
#else
		pc_Buf = MEM_p_Alloc(sizeof(BIG_tdst_File));
#endif
		L_memcpy(pc_Buf, &(BIG_gst.dst_FileTable[_ul_Index]), sizeof(BIG_tdst_File));
		BIG_special_Encrypt4FAT(pc_Buf, sizeof(BIG_tdst_File));
	}
	else
#ifdef JADEFUSION
		pc_Buf = (char *)&(BIG_gst.dst_FileTable[_ul_Index]);
#else
	pc_Buf = (UCHAR *)&(BIG_gst.dst_FileTable[_ul_Index]);
#endif

    /* Update fat */
    r=BIG_fwrite
    (
        pc_Buf,
        sizeof(BIG_tdst_File),
        BIG_Handle()
    );
    ERR_X_Error(r == 1,L_ERR_Csz_FWrite,NULL);

	if(!L_strcmp(BIG_gst.st_ToSave.ac_Def, BIG_Csz_HeaderCrypted))
		MEM_Free(pc_Buf);


    /* Seek to right pos in extended fat file */
    r=L_fseek
    (
        BIG_Handle(),
        pst_Fat->ul_PosFat + BIG_PosFatFileExt +
            (
                (_ul_Index - pst_Fat->ul_FirstIndex) *
                sizeof(BIG_gst.dst_FileTableExt[0].st_ToSave)
            ),
        L_SEEK_SET
    );
    ERR_X_Error(r == 0,L_ERR_Csz_FSeek,NULL);

	/* Marque indiquant que la FAT est cryptée */
	if(!L_strcmp(BIG_gst.st_ToSave.ac_Def, BIG_Csz_HeaderCrypted))
	{
#ifdef JADEFUSION
		pc_Buf = (char* )MEM_p_Alloc(sizeof(BIG_gst.dst_FileTableExt[0].st_ToSave));
#else
		pc_Buf = MEM_p_Alloc(sizeof(BIG_gst.dst_FileTableExt[0].st_ToSave));
#endif
		L_memcpy(pc_Buf, &(BIG_gst.dst_FileTableExt[_ul_Index].st_ToSave), sizeof(BIG_gst.dst_FileTableExt[0].st_ToSave));
		BIG_special_Encrypt4FAT(pc_Buf, sizeof(BIG_gst.dst_FileTableExt[0].st_ToSave));
	}
	else
#ifdef JADEFUSION
		pc_Buf = (char *) &(BIG_gst.dst_FileTableExt[_ul_Index].st_ToSave);
#else
		pc_Buf = (UCHAR *) &(BIG_gst.dst_FileTableExt[_ul_Index].st_ToSave);
#endif
    /* Update fat */
	r=BIG_fwrite
        (
            pc_Buf,
            sizeof(BIG_gst.dst_FileTableExt[0].st_ToSave),
            BIG_Handle()
        );
    ERR_X_Error(r == 1,L_ERR_Csz_FWrite,NULL);

	if(!L_strcmp(BIG_gst.st_ToSave.ac_Def, BIG_Csz_HeaderCrypted))
		MEM_Free(pc_Buf);
}

/*
 ===================================================================================================
    Aim:    Call to update on directory in fat of bigfile.

    In:     _ul_Index   Index of dir in fat.
 ===================================================================================================
 */
void BIG_UpdateOneDirInFat(BIG_INDEX _ul_Index)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG           ul_Fat,r;
    BIG_tdst_FatDes *pst_Fat;
	char			*pc_Buf;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Search the good fat in bigfile */
    ul_Fat = BIG_IndexToFatDes(_ul_Index);
    pst_Fat = &BIG_gst.dst_FatTable[ul_Fat];

    /* Seek to right pos in bigfile */
    r=L_fseek
    (
        BIG_Handle(),
        pst_Fat->ul_PosFat +
            BIG_PosFatDir +
            (_ul_Index - pst_Fat->ul_FirstIndex) *
            sizeof(BIG_gst.dst_DirTable[0].st_ToSave),
            L_SEEK_SET
    );
    ERR_X_Error(r == 0,L_ERR_Csz_FSeek,NULL);

	/* Marque indiquant que la FAT est cryptée */
	if(!L_strcmp(BIG_gst.st_ToSave.ac_Def, BIG_Csz_HeaderCrypted))
	{
#ifdef JADEFUSION
		pc_Buf = (char* )MEM_p_Alloc(sizeof(BIG_gst.dst_DirTable[0].st_ToSave));
#else
		pc_Buf = MEM_p_Alloc(sizeof(BIG_gst.dst_DirTable[0].st_ToSave));
#endif
		L_memcpy(pc_Buf, &(BIG_gst.dst_DirTable[_ul_Index].st_ToSave), sizeof(BIG_gst.dst_DirTable[0].st_ToSave));
		BIG_special_Encrypt4FAT(pc_Buf, sizeof(BIG_gst.dst_DirTable[0].st_ToSave));
	}
	else
#ifdef JADEFUSION
		pc_Buf = (char *) &(BIG_gst.dst_DirTable[_ul_Index].st_ToSave);
#else
	pc_Buf = (UCHAR *) &(BIG_gst.dst_DirTable[_ul_Index].st_ToSave);
#endif

    /* Update fat */
	r=BIG_fwrite
        (
            pc_Buf,
            sizeof(BIG_gst.dst_DirTable[0].st_ToSave),
            BIG_Handle()
        );
    ERR_X_Error(r == 1,L_ERR_Csz_FWrite,NULL);

	if(!L_strcmp(BIG_gst.st_ToSave.ac_Def, BIG_Csz_HeaderCrypted))
		MEM_Free(pc_Buf);
}

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Aim:    Call to retreive of fat pos of a file depending on its name, and of the directory.

    In:     _ul_Dir     Index in fat of the directory to search the file. This index can be
                        retreive with BIG_ul_SearchDir function.
            _psz_Name   Name of file to retreive (the atomic file name). 

    Out:    Return the index in the fat of the file, or BIG_C_InvalidIndex if the file has not been
            found.
 ===================================================================================================
 */
BIG_INDEX BIG_ul_SearchFile(BIG_INDEX _ul_Dir, const char *_psz_Name)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   i;
    char    asz_Temp[BIG_C_MaxLenName];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* If the name of the file is too big, truncate it */
    if(L_strlen(_psz_Name) >= BIG_C_MaxLenName)
    {
        L_memcpy(asz_Temp, _psz_Name, BIG_C_MaxLenName - 1);
        asz_Temp[BIG_C_MaxLenName - 1] = 0;
    }
    else
    {
        L_strcpy(asz_Temp, _psz_Name);
    }

    /* Search the file in the directory */
    i = BIG_FirstFile(_ul_Dir);
    while(i != BIG_C_InvalidIndex)
    {
        /* Compare names */
        if(!(L_strcmpi(BIG_NameFile(i), asz_Temp))) return i;

        /* Pass to brother */
        i = BIG_NextFile(i);
    }

    /* Failed */
    return BIG_C_InvalidIndex;
}

/*
 ===================================================================================================
    Aim:    Call to retreive the fat pos of a directory depening on its name. The directory is
            searched in all the fat, depending on its full name. To search a directory in a known
            parent directory, call BIG_ul_SearchDirInDir function.

    In:     _psz_Name   Name of the directory to retreive (full part). 

    Note:   The path must be specified with '/', and not '\'. For example, "Root/Dr/Try" is a valid
            path. The '/' must not be present at the end of the name (like "Root/Dr/Try/ that is
            incorrect).

    Out:    Return the index of the directory, or BIG_C_InvalidIndex if the dir doesn't exists.
 ===================================================================================================
 */
BIG_INDEX BIG_ul_SearchDir(const char *_psz_Name)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   ul_ReturnIndex;
    const char    *psz_Temp, *psz_Beg, *psz_Cur;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Begin at the root of all directories */
    ul_ReturnIndex = BIG_Root();
    psz_Temp = psz_Beg = _psz_Name;
    while(ul_ReturnIndex != BIG_C_InvalidIndex)
    {
        /* Try to recognize the name */
        psz_Cur = BIG_NameDir(ul_ReturnIndex);
        while((L_toupper(*psz_Cur) == L_toupper(*psz_Temp)) && (*psz_Temp != '/') && (*psz_Cur))
        {
            psz_Cur++;
            psz_Temp++;
        }

        /* Recognize, but it's not the end of the path name */
        if((*psz_Temp == '/') && (*psz_Cur == '\0'))
        {
            ul_ReturnIndex = BIG_SubDir(ul_ReturnIndex);
			if(ul_ReturnIndex > BIG_MaxDir()) 
				return BIG_C_InvalidIndex;

            /* Pass the '/' */
            psz_Temp++;

            /* Remember beginning of the source name */
            psz_Beg = psz_Temp;
        }

        /* Fail to recognize, pass to brother */
        else if((*psz_Cur) || (*psz_Temp))
        {
            ul_ReturnIndex = BIG_NextDir(ul_ReturnIndex);

            /* At the beginning again... */
            psz_Temp = psz_Beg;
        }

        /* Else end of name : Success */
        else
            return ul_ReturnIndex;
    }

    /* Failed */
    return BIG_C_InvalidIndex;
}

/*
 ===================================================================================================
    Aim:    To search a sub-directory in a parent one.

    In:     _ul_Ref     Index in fat of dirs of the parent dir.
            _psz_Name   Name of the directory to search (atomic part). 

    Out:    Returns the index of the sub-directory, BIG_C_InvalidIndex if it does not exists.
 ===================================================================================================
 */
BIG_INDEX BIG_ul_SearchDirInDir(BIG_INDEX _ul_Ref, const char *_psz_Name)
{
    _ul_Ref = BIG_SubDir(_ul_Ref);
    while(_ul_Ref != BIG_C_InvalidIndex)
    {
        if(!L_strcmpi(_psz_Name, BIG_NameDir(_ul_Ref))) return _ul_Ref;

        _ul_Ref = BIG_NextDir(_ul_Ref);
    }

    return BIG_C_InvalidIndex;
}

/*
 ===================================================================================================
    Aim:    Search a file depending on a full path name, and an atomic file name.

    In:     _psz_PathName   Full path name of dir to search.
            _psz_FileName   Atomic file name. 

    Out:    Returns index in fat file of the file, or BIG_C_InvalidIndex.
 ===================================================================================================
 */
BIG_INDEX BIG_ul_SearchFileExt( const char *_psz_PathName, const char *_psz_FileName )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char buf[ L_MAX_PATH ];
	snprintf( buf, sizeof( buf ), "%s/%s", _psz_PathName, _psz_FileName );
	ULONG ul_Index = Jaded_FileSystem_SearchFileExt( buf );
	if ( ul_Index != BIG_C_InvalidIndex )
	{
		return ul_Index;
	}

	/* Retrieve index of parent directory */
	ul_Index = BIG_ul_SearchDir( _psz_PathName );
	if ( ul_Index == BIG_C_InvalidIndex )
	{
		return ( BIG_C_InvalidIndex );
	}

	/* Retrieve index of file in parent directory */
	ul_Index = BIG_ul_SearchFile( ul_Index, _psz_FileName );
	return ul_Index;
}

/*
 ===================================================================================================
    Aim:    To search a sub-directory in a parent one.

    In:     _ul_Ref     Index in fat of dirs of the parent dir.
            _psz_Name   Name of the directory to search (atomic part). 

    Out:    Returns the index of the sub-directory, BIG_C_InvalidIndex if it does not exists.
 ===================================================================================================
 */
BIG_INDEX BIG_ul_SearchFileInDirRec(BIG_INDEX _ul_Ref, const char *_psz_Name)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_Res;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ul_Res = BIG_ul_SearchFile(_ul_Ref, _psz_Name);
    if(ul_Res != BIG_C_InvalidIndex) return ul_Res;

    _ul_Ref = BIG_SubDir(_ul_Ref);
    while(_ul_Ref != BIG_C_InvalidIndex)
    {
        ul_Res = BIG_ul_SearchFileInDirRec(_ul_Ref, _psz_Name);
        if(ul_Res != BIG_C_InvalidIndex) return ul_Res;
        _ul_Ref = BIG_NextDir(_ul_Ref);
    }

    return BIG_C_InvalidIndex;
}

/*
 ===================================================================================================
    Aim:    To search a sub-directory in a parent one.

    In:     _ul_Ref Index in fat of dirs of the parent dir. _psz_Name Name of the directory to
                            search (atomic part). 

    Out:    Returns the index of the sub-directory, BIG_C_InvalidIndex if it does not exists.
 ===================================================================================================
 */
BOOL BIG_b_IsFileInDirRec(BIG_INDEX _ul_RefDir, BIG_INDEX _ul_RefFile)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_Ref;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if((_ul_RefDir == BIG_C_InvalidIndex) || (_ul_RefFile == BIG_C_InvalidIndex)) return FALSE;

    ul_Ref = BIG_ParentFile(_ul_RefFile);
    while((ul_Ref != BIG_C_InvalidIndex) && (ul_Ref != _ul_RefDir))
        ul_Ref = BIG_ParentDir(ul_Ref);

    return(ul_Ref == _ul_RefDir);
}

/*
 ===================================================================================================
    Aim:    To search a sub-directory in a parent one.

    In:     _ul_Ref Index in fat of dirs of the parent dir. _psz_Name Name of the directory to
                            search (atomic part). 

    Out:    Returns the index of the sub-directory, BIG_C_InvalidIndex if it does not exists.
 ===================================================================================================
 */
BOOL BIG_b_IsDirInDirRec(BIG_INDEX _ul_RefDir, BIG_INDEX _ul_RefSubDir)
{
    if(_ul_RefDir == BIG_C_InvalidIndex) return FALSE;

    while((_ul_RefSubDir != BIG_C_InvalidIndex) && (_ul_RefSubDir != _ul_RefDir))
        _ul_RefSubDir = BIG_ParentDir(_ul_RefSubDir);

    return(_ul_RefSubDir == _ul_RefDir);
}

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Aim:    When a file is deleted, it is stored in a free list. This free list is useful to
            retreive a free place in a bigfile when a new file is created. This function try to get
            a free pos in bigfile that can accept a given length of a file.

    In:     _ul_Length          Minimal length of the free position.
            _pul_LengthDisk     If a free pos is found, this will contains the real length on disk
                                of the free pos.
            _pul_Fat            To receive index in fat of choosen file. 

    Out:    Returnd the position in fat of file that can be used to store the new file.
            BIG_C_InvalidIndex if there's no free place, or if there's no free place with enough
            place for _ul_Length.
 ===================================================================================================
 */
ULONG BIG_ul_GetFreePos(ULONG _ul_Length, ULONG *_pul_LengthDisk, BIG_INDEX *_pul_Fat)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   ul_Index, ul_Res, ul_DifRes, ul_ActLength;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*
     * Search a free place with enough place on disk to recieve the file. We parse all the free
     * files to get the one that is the ideal one (the free size is very closest to requested
     * _ul_Length). We return, if there's one, the better one.
     */
    ul_Index = BIG_FreeFile();
    ul_Res = BIG_C_InvalidIndex;
    ul_DifRes = BIG_C_InvalidIndex;
    while(ul_Index != BIG_C_InvalidIndex)
    {
        ul_ActLength = BIG_LengthDiskFile(ul_Index);
        if(ul_ActLength && (ul_ActLength >= _ul_Length))
        {
            if(ul_ActLength - _ul_Length < ul_DifRes)
            {
                ul_Res = ul_Index;
                ul_DifRes = ul_ActLength - _ul_Length;
            }
        }

        /* Short cut if ideal place */
        if(ul_DifRes == 0)
        {
            break;
        }

        /* Go to next file in free list. Remember previous file in list */
        ul_Index = BIG_NextFile(ul_Index);
    }

    /* Failed ? */
    if(ul_Res == BIG_C_InvalidIndex)
    {
        *_pul_LengthDisk = _ul_Length;
        if(*_pul_LengthDisk < 4) *_pul_LengthDisk = 4;
        return BIG_C_InvalidIndex;
    }

    /*
     * We have found a free place. We force length on disk to be 0 (the next time we will call
     * the function, this file will be ignored). But we don't delete file from free list cause
     * the place in the fat is already here, even if no associated pos.
     */
    *_pul_LengthDisk = BIG_LengthDiskFile(ul_Res);
    BIG_LengthDiskFile(ul_Res) = 0;
    if(_pul_Fat) *_pul_Fat = ul_Res;
    return BIG_PosFile(ul_Res);
}

/*
 ===================================================================================================
    Aim:    Write to disk a given fat description.

    In:     _ul_Fat     Index of fat in BIG_gst.dst_FatTable.
 ===================================================================================================
 */
void BIG_UpdateFatDes(ULONG _ul_Fat)
{
	char	*pc_Buf;
	int r;

    /* Seek to beginning of fat des */
	r=L_fseek
        (
            BIG_Handle(),
            BIG_gst.dst_FatTable[_ul_Fat].ul_PosFat - sizeof(BIG_tdst_FatDes),
            L_SEEK_SET
        );
    ERR_X_Error(r == 0,L_ERR_Csz_FSeek,NULL);

	/* Marque indiquant que la FAT est cryptée */
	if(!L_strcmp(BIG_gst.st_ToSave.ac_Def, BIG_Csz_HeaderCrypted))
	{
#ifdef JADEFUSION
		pc_Buf = (char* )MEM_p_Alloc(sizeof(BIG_tdst_FatDes));
#else
		pc_Buf = MEM_p_Alloc(sizeof(BIG_tdst_FatDes));
#endif
		L_memcpy(pc_Buf, &(BIG_gst.dst_FatTable[_ul_Fat]), sizeof(BIG_tdst_FatDes));
		BIG_special_Encrypt4FAT(pc_Buf, sizeof(BIG_tdst_FatDes));
	}
	else
#ifdef JADEFUSION
		pc_Buf = (char *) &(BIG_gst.dst_FatTable[_ul_Fat]);
#else
	pc_Buf = (UCHAR *) &(BIG_gst.dst_FatTable[_ul_Fat]);
#endif

    /* Write fat des */
	r=BIG_fwrite
        (
            pc_Buf,
            sizeof(BIG_tdst_FatDes),
            BIG_Handle()
        );
    ERR_X_Error(r == 1,L_ERR_Csz_FWrite,NULL);

	/* Marque indiquant que la FAT est cryptée */
	if(!L_strcmp(BIG_gst.st_ToSave.ac_Def, BIG_Csz_HeaderCrypted))
		MEM_Free(pc_Buf);
}

/*
 ===================================================================================================
    Aim:    Call when all fats are filled. So need to create a new fat description. £
            A new fat is added in BIG_gst.dstFatTable. £
            A new fat is added at the end of the bigfile.
 ===================================================================================================
 */
void BIG_AddOneFatDes(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG           ul_Fat,r;
    BIG_tdst_FatDes *pst_NewFat;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Realloc fat table */
    ul_Fat = BIG_NumFat();
    BIG_NumFat()++;
    BIG_gst.dst_FatTable = (BIG_tdst_FatDes*)L_realloc
        (
            BIG_gst.dst_FatTable,
            BIG_NumFat() * sizeof(BIG_tdst_FatDes)
        );
    ERR_X_Error(BIG_gst.dst_FatTable != NULL, L_ERR_Csz_NotEnoughMemory, NULL);

    /* Realloc file table */
    BIG_gst.dst_FileTable = (BIG_tdst_File*)L_realloc
        (
            BIG_gst.dst_FileTable,
            BIG_NumFat() * BIG_SizeFat() * sizeof(BIG_tdst_File)
        );
    ERR_X_Error(BIG_gst.dst_FileTable != NULL, L_ERR_Csz_NotEnoughMemory, NULL);

    BIG_gst.dst_FileTableExt = (BIG_tdst_FileExt*)L_realloc
        (
            BIG_gst.dst_FileTableExt,
            BIG_NumFat() * BIG_SizeFat() * sizeof(BIG_tdst_FileExt)
        );
    ERR_X_Error(BIG_gst.dst_FileTableExt != NULL, L_ERR_Csz_NotEnoughMemory, NULL);

    BIG_gst.dst_DirTable = (BIG_tdst_Directory*)L_realloc
        (
            BIG_gst.dst_DirTable,
            BIG_NumFat() * BIG_SizeFat() * sizeof(BIG_tdst_Directory)
        );
    ERR_X_Error(BIG_gst.dst_DirTable != NULL, L_ERR_Csz_NotEnoughMemory, NULL);

    /* Update fat des */
    pst_NewFat = &BIG_gst.dst_FatTable[ul_Fat];
    L_memset(pst_NewFat, 0, sizeof(BIG_tdst_FatDes));

    /* Position of the fat in disk (at the end of bigfile) */
	r=L_fseek(BIG_Handle(), 0, L_SEEK_END);
    ERR_X_Error(r == 0, L_ERR_Csz_FSeek, NULL);
    pst_NewFat->ul_PosFat = L_ftell(BIG_Handle()) + sizeof(BIG_tdst_FatDes);
    pst_NewFat->ul_NextPosFat = (ULONG) - 1;

    pst_NewFat->ul_FirstIndex = BIG_gst.dst_FatTable[ul_Fat - 1].ul_LastIndex + 1;
    pst_NewFat->ul_LastIndex = pst_NewFat->ul_FirstIndex + BIG_SizeFat() - 1;

    /* Update previous fat des */
    BIG_gst.dst_FatTable[ul_Fat - 1].ul_NextPosFat = BIG_gst.dst_FatTable[ul_Fat].ul_PosFat;

    /* Update fat on disk */
    BIG_UpdateFatDes(ul_Fat - 1);
    BIG_UpdateFatDes(ul_Fat);
    BIG_WriteHeader();

    /* A big seek to "reserve" all the new fat on disk */
    r=L_fseek
    (
        BIG_gst.h_CLibFileHandle,
        pst_NewFat->ul_PosFat + BIG_PosFatDir +
            (BIG_SizeFat() * sizeof(BIG_gst.dst_DirTable[0].st_ToSave)),
        L_SEEK_SET
    );
    ERR_X_Error(r== 0,L_ERR_Csz_FSeek,NULL);
    BIG_fwrite(&BIG_gst.h_CLibFileHandle, 1, BIG_gst.h_CLibFileHandle);
}

/*
 ===================================================================================================
    Aim:    Basic function to get a new file pos in fat. See BIG_ul_AddFileInTable function for
            more details.

    Out:    The index in fat, ASSERT if we can't (see below).
 ===================================================================================================
 */
static BIG_INDEX sul_GetNewFile(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_Res, ul_Memo, ul_Fat;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ul_Res = BIG_FreeFile();
    if(ul_Res == BIG_C_InvalidIndex)
    {
        /* No more place */
        if(BIG_MaxFile() >= BIG_gst.dst_FatTable[BIG_NumFat() - 1].ul_LastIndex) BIG_AddOneFatDes();

        /* Update fat */
        ul_Res = BIG_MaxFile()++;

        /* Update fat des */
        ul_Fat = BIG_IndexToFatDes(ul_Res);
        BIG_gst.dst_FatTable[ul_Fat].ul_MaxFile++;
        BIG_UpdateFatDes(ul_Fat);
    }
    else
    {
        /* Unlink from free list */
        ul_Memo = BIG_NextFile(ul_Res);
        if(ul_Memo != BIG_C_InvalidIndex)
        {
            BIG_PrevFile(ul_Memo) = BIG_C_InvalidIndex;
            BIG_UpdateOneFileInFat(ul_Memo);
        }

        BIG_FreeFile() = ul_Memo;
        BIG_WriteHeader();
    }

    return ul_Res;
}

/*
 ===================================================================================================
    Aim:    Call to add one entry in fat of files. We first search a free place in list of free
            files. If not found, we add the new file at the end of the fat.

    Note:   This function will failed if there's no enough place in fat. So this function must
            NEVER failed. We should change the bigfile on disk to reserve a biggest file if needed,
            but BEFORE.

    In:     _ul_Length
            _pul_Pos
            _pul_LengthDisk
            _b_AskKey           

    Out:    Index, in fat files, of the new file.
 ===================================================================================================
 */
BIG_INDEX BIG_ul_AddFileInTable
(
    ULONG   _ul_Length,
    ULONG   *_pul_Pos,
    ULONG   *_pul_LengthDisk,
    BOOL    _b_AskKey
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_Res, ul_Memo;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

_Try_
    *_pul_Pos = BIG_ul_GetFreePos(_ul_Length, _pul_LengthDisk, &ul_Res);

    /* If pos is correct, we get the fat index of the free file */
    if(*_pul_Pos != BIG_C_InvalidIndex)
    {
        ul_Memo = BIG_NextFile(ul_Res);
        if(ul_Memo != BIG_C_InvalidIndex)
        {
            BIG_PrevFile(ul_Memo) = BIG_PrevFile(ul_Res);
            BIG_UpdateOneFileInFat(ul_Memo);
        }

        ul_Memo = BIG_PrevFile(ul_Res);
        if(ul_Memo != BIG_C_InvalidIndex)
        {
            BIG_NextFile(ul_Memo) = BIG_NextFile(ul_Res);
            BIG_UpdateOneFileInFat(ul_Memo);
        }
        else
        {
            BIG_FreeFile() = BIG_NextFile(ul_Res);
            BIG_WriteHeader();
        }
    }

    /* Else search a new file (from free list or not, we don't know here !) */
    else
    {
        ul_Res = sul_GetNewFile();
        ERR_X_Assert(ul_Res != BIG_C_InvalidIndex);
    }

    L_memset(&BIG_gst.dst_FileTable[ul_Res], 0, sizeof(BIG_tdst_File));
    L_memset(&BIG_gst.dst_FileTableExt[ul_Res], 0, sizeof(BIG_tdst_FileExt));

    /* A new key is requested ? */
    if(_b_AskKey)
        BIG_FileKey(ul_Res) = BIG_ul_GetNewKey(ul_Res);

_Catch_
    _Return_(BIG_C_InvalidIndex);
_End_
    return ul_Res;
}

/*
 ===================================================================================================
    Aim:    Call to add one entry in fat of directories. We first search a free place in list of
            free dirs. If not found, we add the new dir at the end of the fat.

    Note:   This function will failed if there's no enough place in fat. So this function must
            NEVER failed. We should change the bigfile on disk to reserve a biggest file if needed,
            but BEFORE.

    Out:    Index, in fat dir, of the new dir.
 ===================================================================================================
 */
BIG_INDEX BIG_ul_AddDirInTable(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_Res, ul_Next;
    ULONG       ul_Fat;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Is there a free dir ? */
    ul_Res = BIG_FreeDir();
    if(ul_Res == BIG_C_InvalidIndex)
    {
        /* No more place */
        if(BIG_MaxDir() >= BIG_gst.dst_FatTable[BIG_NumFat() - 1].ul_LastIndex) BIG_AddOneFatDes();

        /* No. We add the dir at the end of fat */
        if(BIG_MaxDir() == 0) BIG_Root() = 0;

        /* Update fat */
        ul_Res = BIG_MaxDir()++;

        /* Update fat des */
        ul_Fat = BIG_IndexToFatDes(ul_Res);
        BIG_gst.dst_FatTable[ul_Fat].ul_MaxDir++;
        BIG_UpdateFatDes(ul_Fat);
    }
    else
    {
        /* Yes, unlink of free list */
        ul_Next = BIG_NextDir(ul_Res);
        if(ul_Next != BIG_C_InvalidIndex)
        {
            BIG_PrevDir(ul_Next) = BIG_C_InvalidIndex;
            BIG_UpdateOneDirInFat(ul_Next);
        }

        BIG_FreeDir() = ul_Next;
        BIG_WriteHeader();
    }

    /* Init dir */
    L_memset(&BIG_gst.dst_DirTable[ul_Res], 0, sizeof(BIG_tdst_Directory));
    return ul_Res;
}

#endif /* ACTIVE_EDITORS */
