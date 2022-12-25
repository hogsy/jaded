//////////////////////////////////////////////////////////////////////////
// Binarized BF Manager
// ---------------------
// Keeps track of the binarized BF and switches to the proper binarized
// BF when loading a map.
//////////////////////////////////////////////////////////////////////////

#ifndef __BINARIZEDBF_MANGER_H__
#define __BINARIZEDBF_MANGER_H__

#include "BIGfiles/BIGdefs.h"

//////////////////////////////////////////////////////////////////////////
// CBinarizedBFManager

enum eBinarizedBFType
{
	eBinarizedBFTextures = 0,
	eBinarizedBFMaps,
	
	eBinarizedBFTypeMax
};

#define ENUM_BINARZEDBF(_ENUMVALUE, _ENUM_BINARZEDBF) case _ENUMVALUE: return #_ENUM_BINARZEDBF;

static char* MissingBinarizedBF(int _iType)
{
	switch(_iType)
	{
		ENUM_BINARZEDBF(eBinarizedBFTextures		,missing binarized BF containing textures in JadeXe.ini or on HD)			
		ENUM_BINARZEDBF(eBinarizedBFMaps			,missing binarized BF containing maps in JadeXe.ini or on HD)			
	}
	return "Invalid binarized BF type";
}

class CBinarizedBFManager
{
protected:
	BIG_tdst_BigFile* arrayBinarizedBF[eBinarizedBFTypeMax];

	void CopyBinarizedBFStruct(BIG_tdst_BigFile* pSrcBF, BIG_tdst_BigFile* pDstBF, BOOL bLoading);

public:
	CBinarizedBFManager();
	~CBinarizedBFManager();

	void LoadBinarizedBF();
	void InsertBinarizedBF(char* szFileName, eBinarizedBFType eBinBFType);
	void SwitchBinarizedBF(eBinarizedBFType eBinBFType);
};

extern __declspec(align(32)) CBinarizedBFManager g_BinarizedBFManager;

#endif // __BINARIZEDBF_MANGER_H__
