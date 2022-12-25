#include "Precomp.h"

#ifdef _XENON

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "xeSND_BigFile.h"
#include "xeSND_Engine.h"

// ***********************************************************************************************************************
//    Class Methods : CBigFileReader
// ***********************************************************************************************************************

CBigFileReader::CBigFileReader(char* Param_pName, BOOL Param_bReadHeader)
{
	m_bIsOpen			= FALSE;
	m_pFile				= NULL;
	m_iFileCount		= 0;
	m_pstKeyPosEntries	= NULL;

	if (Param_pName != NULL)
	{
		Open(Param_pName, Param_bReadHeader);
	}
}

// ***********************************************************************************************************************

CBigFileReader::~CBigFileReader()
{
	Close();
}

// ***********************************************************************************************************************

BOOL CBigFileReader::IsOpen()
{
	return m_bIsOpen;
}

// ***********************************************************************************************************************

BOOL CBigFileReader::ReadHeader()
{
	// Open Stream

	CStreamReader* pStreamReader = GetAudioEngine()->GetStreamReader();

	if (!pStreamReader->Open(m_pFile->GetHandle_Sync()))
	{
		return FALSE;
	}

	// Read File Count From Header

	pStreamReader->Skip(8);

	m_iFileCount = pStreamReader->Read32();

	if (m_iFileCount <= 0)
	{
		pStreamReader->Close();
		return FALSE;
	}

	pStreamReader->Skip(24);

	// Read FAT Count From Header

	int iFATCount = pStreamReader->Read32();

	pStreamReader->Skip(4);

	// Allocate Key Storage Array

	m_pstKeyPosEntries = (stKeyPosEntry*)GetAudioEngine()->MemoryAllocate(m_iFileCount * sizeof(stKeyPosEntry), eMemoryTracker_General);

	// Read All FATs

	int iCurrentFile = 0;

	for (int iLoop_FAT = 0; iLoop_FAT < iFATCount; iLoop_FAT++)
	{
		// Read File Count In Current FAT

		int iLocalFileCount = pStreamReader->Read32();

		pStreamReader->Skip(8);

		// Read Position Of Next FAT

		unsigned int uiNextFATPosition = pStreamReader->Read32();

		// Skip Rest Of FAT

		pStreamReader->Skip(8);

		// Read Key Position Entries (If Applicable)

		for (int iLoop_File = 0; iLoop_File < iLocalFileCount; iLoop_File++)
		{
			m_pstKeyPosEntries[iCurrentFile].uiPosition	= pStreamReader->Read32();
			m_pstKeyPosEntries[iCurrentFile].uiKey		= pStreamReader->Read32();

			iCurrentFile++;
		}

		// Skip To Next FAT (If Applicable)

		if (uiNextFATPosition != 0xFFFFFFFF)
		{
			pStreamReader->SeekInData(uiNextFATPosition - sizeof(BIG_tdst_FatDes));
		}
	}

	// Close Stream

	pStreamReader->Close();

	return TRUE;
}

// ***********************************************************************************************************************

BOOL CBigFileReader::Open(char* Param_pName, BOOL Param_bReadHeader)
{
	// Error Checking

	if ((Param_pName == NULL) || m_bIsOpen)
	{
		return FALSE;
	}

	// Create File Handle

	m_pFile = new CFile(Param_pName);

	if ((m_pFile == NULL) || !m_pFile->IsOpen())
	{
		return FALSE;
	}

	// Read Header (If Applicable)

	if (Param_bReadHeader)
	{
		if (!ReadHeader())
		{
			return FALSE;
		}
	}

	m_bIsOpen = TRUE;

	return TRUE;
}

// ***********************************************************************************************************************

void CBigFileReader::Close()
{
	// Free Key Position Table

	if (m_pstKeyPosEntries != NULL)
	{
		GetAudioEngine()->MemoryFree(m_pstKeyPosEntries, eMemoryTracker_General);

		m_pstKeyPosEntries = NULL;
	}

	// Destroy File Handle

	if (m_pFile != NULL)
	{
		delete m_pFile;

		m_pFile = NULL;
	}

	// Reset Fields

	m_bIsOpen		= FALSE;
	m_iFileCount	= 0;
}

// ***********************************************************************************************************************

CFile* CBigFileReader::GetFile()
{
	if ((m_pFile == NULL) || !m_pFile->IsOpen())
	{
		return NULL;
	}

	return m_pFile;
}

// ***********************************************************************************************************************

int CBigFileReader::GetEntryCount()
{
	return m_iFileCount;
}

// ***********************************************************************************************************************

stKeyPosEntry* CBigFileReader::GetEntry(int Param_iIndex)
{
	// Error Checking

	if ((Param_iIndex < 0) || (Param_iIndex >= m_iFileCount))
	{
		return NULL;
	}

	return &m_pstKeyPosEntries[Param_iIndex];
}

// ***********************************************************************************************************************

unsigned int CBigFileReader::FindPositionFromKey(unsigned int Param_uiKey)
{
	// Error Checking

	if (!m_bIsOpen || (m_pstKeyPosEntries == NULL))
	{
		return 0xFFFFFFFF;
	}

	// Scan All Entries

	for (int iLoop = 0; iLoop < m_iFileCount; iLoop++)
	{
		if (m_pstKeyPosEntries[iLoop].uiKey == Param_uiKey)
		{
			return m_pstKeyPosEntries[iLoop].uiPosition;
		}
	}

	// Return Invalid Position

	return 0xFFFFFFFF;
}

// ***********************************************************************************************************************

#endif // _XENON
