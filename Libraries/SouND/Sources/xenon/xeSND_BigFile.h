#ifndef __xeSND_BigFile_h__
#define __xeSND_BigFile_h__

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "xeSND_Helpers.h"

#include "BASe/MEMory/MEM.h"

// ***********************************************************************************************************************
//    Structs
// ***********************************************************************************************************************

typedef struct
{
	unsigned int uiKey;
	unsigned int uiPosition;
}
stKeyPosEntry;

// ***********************************************************************************************************************
//    Classes
// ***********************************************************************************************************************

class CBigFileReader
{
private:
	BOOL			m_bIsOpen;
	CFile*			m_pFile;
	int				m_iFileCount;
	stKeyPosEntry*	m_pstKeyPosEntries;

	BOOL ReadHeader();

public:
    M_DeclareOperatorNewAndDelete();

	CBigFileReader(char* Param_pName = NULL, BOOL Param_bReadHeader = TRUE);
	~CBigFileReader();

	BOOL			IsOpen();
	BOOL			Open(char* Param_pName, BOOL Param_bReadHeader = TRUE);
	void			Close();
	CFile*			GetFile();
	int				GetEntryCount();
	stKeyPosEntry*	GetEntry(int Param_iIndex);
	unsigned int	FindPositionFromKey(unsigned int Param_uiKey);
};

// ***********************************************************************************************************************

#endif // _xeSND_BigFile_h_
