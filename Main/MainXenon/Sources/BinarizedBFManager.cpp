//////////////////////////////////////////////////////////////////////////
// Binarized BF Manager
// ---------------------
// Keeps track of the binarized BF and switches to the proper binarized
// BF when loading a map.
//////////////////////////////////////////////////////////////////////////

#include "Precomp.h"
#include "BinarizedBFManager.h" 

#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGdefs.h"

#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BIGfiles/BIGfat.h"

//////////////////////////////////////////////////////////////////////////
// CBinarizedBFManager

CBinarizedBFManager g_BinarizedBFManager;

CBinarizedBFManager::CBinarizedBFManager() 
{
	for ( int index = 0 ; index < eBinarizedBFTypeMax ; index++ )
		arrayBinarizedBF[index] = NULL;
}

CBinarizedBFManager::~CBinarizedBFManager()
{
}

void CBinarizedBFManager::CopyBinarizedBFStruct(BIG_tdst_BigFile* pSrcBF, BIG_tdst_BigFile* pDstBF, BOOL bLoading)
{
	if ( ! pSrcBF && ! pDstBF )
		return;

	// Copy name of Binarized BF
	strcpy(pDstBF->asz_Name, pSrcBF->asz_Name);

	// Copy main header of BF
	memcpy(&pDstBF->st_ToSave, &pSrcBF->st_ToSave, sizeof(BIG_tdst_BigFile_::st_ToSave_));

	// Copy file handle of BF
	pDstBF->h_CLibFileHandle = pSrcBF->h_CLibFileHandle;

	// Copy FatTable pointer
	pDstBF->dst_FatTable = pSrcBF->dst_FatTable;

	// Copy FileTable pointer
	pDstBF->dst_FileTable = pSrcBF->dst_FileTable;

	// Copy array Key <-> Pos
	if ( ! bLoading )
		BAS_bfree(&pDstBF->st_KeyTableToPos);

	BAS_binit(&pDstBF->st_KeyTableToPos, 0);
	pDstBF->st_KeyTableToPos.gran = 200;
	pDstBF->st_KeyTableToPos.size = pDstBF->st_ToSave.ul_MaxFile + 1;
	pDstBF->st_KeyTableToPos.base = (BAS_tdst_Key *) MEM_p_VMAlloc(pDstBF->st_KeyTableToPos.size * sizeof(BAS_tdst_Key));

	BAS_bsortmode = FALSE;
	for ( int index=0 ; index < pSrcBF->st_KeyTableToPos.num ; ++index )
	{
		ULONG ulKey = pSrcBF->st_KeyTableToPos.base[index].ul_Key;
		ULONG ulPos = pSrcBF->st_KeyTableToPos.base[index].ul_Val;
		BAS_binsert(ulKey, ulPos, &pDstBF->st_KeyTableToPos);
	}
	BAS_bsortmode = TRUE;
	BAS_bsort(&pDstBF->st_KeyTableToPos);
}

void CBinarizedBFManager::InsertBinarizedBF(char* szFileName, eBinarizedBFType eBinBFType)
{
	// Create empty BF structure
	BIG_tdst_BigFile* pBigFile = (BIG_tdst_BigFile*)MEM_p_Alloc(sizeof(BIG_tdst_BigFile));
	strcpy(pBigFile->asz_Name, szFileName);

	// Insert to vector of binarized BF
	arrayBinarizedBF[eBinBFType] = pBigFile;
}

void CBinarizedBFManager::LoadBinarizedBF()
{
	for ( int index = 0 ; index < eBinarizedBFTypeMax ; index++ )
	{
		BIG_tdst_BigFile* pBigFile = arrayBinarizedBF[index];
		
		ERR_X_Error(pBigFile, L_ERR_Csz_FOpen, MissingBinarizedBF(index));
		
		// Open BigFile using global variable BIG_gst
		BIG_Open(pBigFile->asz_Name);

		// Copy loaded BIG_gst to vector of BinarizedBF
		CopyBinarizedBFStruct(&BIG_gst, pBigFile, TRUE);
	}
}

void CBinarizedBFManager::SwitchBinarizedBF(eBinarizedBFType eBinBFType)
{
	CopyBinarizedBFStruct(arrayBinarizedBF[eBinBFType], &BIG_gst, FALSE);
}