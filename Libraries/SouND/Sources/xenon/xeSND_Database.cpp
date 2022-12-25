#include "Precomp.h"

#ifdef _XENON

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "xeSND_Engine.h"

// ***********************************************************************************************************************
//    Globals
// ***********************************************************************************************************************

#if defined(_FINAL_)
	g_bSoundDatabaseEnabled = TRUE;
#else
	g_bSoundDatabaseEnabled = FALSE;
#endif

// ***********************************************************************************************************************
//    Constants
// ***********************************************************************************************************************

const char* kszHeaderDatabase_Filename	= "GAME:\\Sound\\SoundHeaders.db";
const int	kiHeaderDatabase_Version	= 2;

// ***********************************************************************************************************************
//    Class Methods : CCompactHeader
// ***********************************************************************************************************************

CCompactHeader::CCompactHeader(unsigned int Param_uiKey) : CListItem()
{
	m_uiKey = Param_uiKey;

	m_uiExitPoint_Count			= 0;
	m_uiExitPoints				= NULL;
	m_uiSignalPoint_Count		= 0;
	m_uiSignalPoint_Positions	= NULL;
	m_uiSignalPoint_Labels		= NULL;
	m_uiRegion_Count			= 0;
	m_uiRegion_Labels			= NULL;
	m_uiRegion_StartPositions	= NULL;
	m_uiRegion_StopPositions	= NULL;
}

// ***********************************************************************************************************************

CCompactHeader::~CCompactHeader()
{
	// Free All Lists (If Applicable)

	if (m_uiExitPoints != NULL)
	{
		GetAudioEngine()->MemoryFree(m_uiExitPoints, eMemoryTracker_General);
	}

	if (m_uiSignalPoint_Positions != NULL)
	{
		GetAudioEngine()->MemoryFree(m_uiSignalPoint_Positions, eMemoryTracker_General);
	}

	if (m_uiSignalPoint_Labels != NULL)
	{
		GetAudioEngine()->MemoryFree(m_uiSignalPoint_Labels, eMemoryTracker_General);
	}

	if (m_uiRegion_Labels != NULL)
	{
		GetAudioEngine()->MemoryFree(m_uiRegion_Labels, eMemoryTracker_General);
	}

	if (m_uiRegion_StartPositions != NULL)
	{
		GetAudioEngine()->MemoryFree(m_uiRegion_StartPositions, eMemoryTracker_General);
	}

	if (m_uiRegion_StopPositions != NULL)
	{
		GetAudioEngine()->MemoryFree(m_uiRegion_StopPositions, eMemoryTracker_General);
	}
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_Key()
{
	return m_uiKey;
}

// ***********************************************************************************************************************

int CCompactHeader::Get_SoundBF_Index()
{
	return m_iSoundBF_Index;
}

// ***********************************************************************************************************************

void CCompactHeader::Set_SoundBF_Index(int Param_iSoundBF_Index)
{
	m_iSoundBF_Index = Param_iSoundBF_Index;
}

// ***********************************************************************************************************************

//unsigned int CCompactHeader::Get_SoundBF_FilePosition()
//{
	//return m_uiSoundBF_FilePosition;
//}

// ***********************************************************************************************************************

//void CCompactHeader::Set_SoundBF_FilePosition(unsigned int Param_uiSoundBF_FilePosition)
//{
	//m_uiSoundBF_FilePosition = Param_uiSoundBF_FilePosition;
//}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_BigFile_DataPosition()
{
	return m_uiBigFile_DataPosition;
}

// ***********************************************************************************************************************

void CCompactHeader::Set_BigFile_DataPosition(unsigned int Param_uiDataPosition)
{
	m_uiBigFile_DataPosition = Param_uiDataPosition;
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_BigFile_DataSize()
{
	return m_uiBigFile_DataSize;
}

// ***********************************************************************************************************************

void CCompactHeader::Set_BigFile_DataSize(unsigned int Param_uiDataSize)
{
	m_uiBigFile_DataSize = Param_uiDataSize;
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_Format_Tag()
{
	return m_uiFormat_Tag;
}

// ***********************************************************************************************************************

void CCompactHeader::Set_Format_Tag(unsigned int Param_uiTag)
{
	m_uiFormat_Tag = Param_uiTag;
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_Format_ChannelCount()
{
	return m_uiFormat_ChannelCount;
}

// ***********************************************************************************************************************

void CCompactHeader::Set_Format_ChannelCount(unsigned int Param_uiChannelCount)
{
	m_uiFormat_ChannelCount = Param_uiChannelCount;
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_Format_SamplesPerSecond()
{
	return m_uiFormat_SamplesPerSecond;
}

// ***********************************************************************************************************************

void CCompactHeader::Set_Format_SamplesPerSecond(unsigned int Param_uiSamplesPerSecond)
{
	m_uiFormat_SamplesPerSecond = Param_uiSamplesPerSecond;
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_Format_AverageBytesPerSecond()
{
	return m_uiFormat_AverageBytesPerSecond;
}

// ***********************************************************************************************************************

void CCompactHeader::Set_Format_AverageBytesPerSecond(unsigned int Param_uiAverageBytes)
{
	m_uiFormat_AverageBytesPerSecond = Param_uiAverageBytes;
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_Format_BlockAlign()
{
	return m_uiFormat_BlockAlign;
}

// ***********************************************************************************************************************

void CCompactHeader::Set_Format_BlockAlign(unsigned int Param_uiBlockAlign)
{
	m_uiFormat_BlockAlign = Param_uiBlockAlign;
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_Format_BitsPerSample()
{
	return m_uiFormat_BitsPerSample;
}

// ***********************************************************************************************************************

void CCompactHeader::Set_Format_BitsPerSample(unsigned int Param_uiBitsPerSample)
{
	m_uiFormat_BitsPerSample = Param_uiBitsPerSample;
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_Format_ExtraInfoSize()
{
	return m_uiFormat_ExtraInfoSize;
}

// ***********************************************************************************************************************

void CCompactHeader::Set_Format_ExtraInfoSize(unsigned int Param_uiExtraInfoSize)
{
	m_uiFormat_ExtraInfoSize = Param_uiExtraInfoSize;
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_Format_SamplesPerBlock()
{
	return m_uiFormat_SamplesPerBlock;
}

// ***********************************************************************************************************************

void CCompactHeader::Set_Format_SamplesPerBlock(unsigned int Param_uiSamplesPerBlock)
{
	m_uiFormat_SamplesPerBlock = Param_uiSamplesPerBlock;
}

// ***********************************************************************************************************************

BOOL CCompactHeader::Get_Loop()
{
	return m_bHasLoop;
}

// ***********************************************************************************************************************

void CCompactHeader::Set_Loop(BOOL Param_bHasLoop)
{
	m_bHasLoop = Param_bHasLoop;
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_Loop_BeginOffset()
{
	return m_uiLoop_BeginOffset;
}

// ***********************************************************************************************************************

void CCompactHeader::Set_Loop_BeginOffset(unsigned int Param_uiLoop_BeginOffset)
{
	m_uiLoop_BeginOffset = Param_uiLoop_BeginOffset;
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_Loop_EndOffset()
{
	return m_uiLoop_EndOffset;
}

// ***********************************************************************************************************************

void CCompactHeader::Set_Loop_EndOffset(unsigned int Param_uiLoop_EndOffset)
{
	m_uiLoop_EndOffset = Param_uiLoop_EndOffset;
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_Offset_Start()
{
	return m_uiOffset_Start;
}

// ***********************************************************************************************************************

void CCompactHeader::Set_Offset_Start(unsigned int Param_uiOffset_Start)
{
	m_uiOffset_Start = Param_uiOffset_Start;
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_Offset_End()
{
	return m_uiOffset_End;
}

// ***********************************************************************************************************************

void CCompactHeader::Set_Offset_End(unsigned int Param_uiOffset_End)
{
	m_uiOffset_End = Param_uiOffset_End;
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_ExitPoint_Count()
{
	return m_uiExitPoint_Count;
}

// ***********************************************************************************************************************

void CCompactHeader::Set_ExitPoint_Count(unsigned int Param_uiExitPoint_Count)
{
	// Ensure Different List Sizes

	if (m_uiExitPoint_Count == Param_uiExitPoint_Count)
	{
		return;
	}

	// Free Old List (If Applicable)

	if (m_uiExitPoints != NULL)
	{
		GetAudioEngine()->MemoryFree(m_uiExitPoints, eMemoryTracker_General);
		m_uiExitPoints = NULL;
	}

	// Allocate New List (If Applicable)

	m_uiExitPoint_Count = Param_uiExitPoint_Count;

	if (m_uiExitPoint_Count > 0)
	{
		m_uiExitPoints = (unsigned int*)GetAudioEngine()->MemoryAllocate(m_uiExitPoint_Count * sizeof(unsigned int), eMemoryTracker_General);
	}
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_ExitPoint_Value(unsigned int Param_uiIndex)
{
	if ((m_uiExitPoints == NULL) || (Param_uiIndex >= m_uiExitPoint_Count))
	{
		return 0;
	}

	return m_uiExitPoints[Param_uiIndex];
}

// ***********************************************************************************************************************

void CCompactHeader::Set_ExitPoint_Value(unsigned int Param_uiIndex, unsigned int Param_uiExitPoint_Value)
{
	if ((m_uiExitPoints == NULL) || (Param_uiIndex >= m_uiExitPoint_Count))
	{
		return;
	}

	m_uiExitPoints[Param_uiIndex] = Param_uiExitPoint_Value;
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_SignalPoint_Count()
{
	return m_uiSignalPoint_Count;
}

// ***********************************************************************************************************************

void CCompactHeader::Set_SignalPoint_Count(unsigned int Param_uiSignalPoint_Count)
{
	// Ensure Different List Sizes

	if (m_uiSignalPoint_Count == Param_uiSignalPoint_Count)
	{
		return;
	}

	// Free Old Lists (If Applicable)

	if (m_uiSignalPoint_Positions != NULL)
	{
		GetAudioEngine()->MemoryFree(m_uiSignalPoint_Positions, eMemoryTracker_General);
		m_uiSignalPoint_Positions = NULL;
	}

	if (m_uiSignalPoint_Labels != NULL)
	{
		GetAudioEngine()->MemoryFree(m_uiSignalPoint_Labels, eMemoryTracker_General);
		m_uiSignalPoint_Labels = NULL;
	}

	// Allocate New Lists (If Applicable)

	m_uiSignalPoint_Count = Param_uiSignalPoint_Count;

	if (m_uiSignalPoint_Count > 0)
	{
		m_uiSignalPoint_Positions	= (unsigned int*)GetAudioEngine()->MemoryAllocate(m_uiSignalPoint_Count * sizeof(unsigned int), eMemoryTracker_General);
		m_uiSignalPoint_Labels		= (unsigned int*)GetAudioEngine()->MemoryAllocate(m_uiSignalPoint_Count * sizeof(unsigned int), eMemoryTracker_General);
	}
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_SignalPoint_Position(unsigned int Param_uiIndex)
{
	if ((m_uiSignalPoint_Positions == NULL) || (Param_uiIndex >= m_uiSignalPoint_Count))
	{
		return 0;
	}

	return m_uiSignalPoint_Positions[Param_uiIndex];
}

// ***********************************************************************************************************************

void CCompactHeader::Set_SignalPoint_Position(unsigned int Param_uiIndex, unsigned int Param_uiSignalPoint_Position)
{
	if ((m_uiSignalPoint_Positions == NULL) || (Param_uiIndex >= m_uiSignalPoint_Count))
	{
		return;
	}

	m_uiSignalPoint_Positions[Param_uiIndex] = Param_uiSignalPoint_Position;
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_SignalPoint_Label(unsigned int Param_uiIndex)
{
	if ((m_uiSignalPoint_Labels == NULL) || (Param_uiIndex >= m_uiSignalPoint_Count))
	{
		return 0;
	}

	return m_uiSignalPoint_Labels[Param_uiIndex];
}

// ***********************************************************************************************************************

void CCompactHeader::Set_SignalPoint_Label(unsigned int Param_uiIndex, unsigned int Param_uiSignalPoint_Label)
{
	if ((m_uiSignalPoint_Labels == NULL) || (Param_uiIndex >= m_uiSignalPoint_Count))
	{
		return;
	}

	m_uiSignalPoint_Labels[Param_uiIndex] = Param_uiSignalPoint_Label;
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_Region_Count()
{
	return m_uiRegion_Count;
}

// ***********************************************************************************************************************

void CCompactHeader::Set_Region_Count(unsigned int Param_uiRegion_Count)
{
	// Ensure Different List Sizes

	if (m_uiRegion_Count == Param_uiRegion_Count)
	{
		return;
	}

	// Free Old Lists (If Applicable)

	if (m_uiRegion_Labels != NULL)
	{
		GetAudioEngine()->MemoryFree(m_uiRegion_Labels, eMemoryTracker_General);
		m_uiRegion_Labels = NULL;
	}

	if (m_uiRegion_StartPositions != NULL)
	{
		GetAudioEngine()->MemoryFree(m_uiRegion_StartPositions, eMemoryTracker_General);
		m_uiRegion_StartPositions = NULL;
	}

	if (m_uiRegion_StopPositions != NULL)
	{
		GetAudioEngine()->MemoryFree(m_uiRegion_StopPositions, eMemoryTracker_General);
		m_uiRegion_StopPositions = NULL;
	}

	// Allocate New Lists (If Applicable)

	m_uiRegion_Count = Param_uiRegion_Count;

	if (m_uiRegion_Count > 0)
	{
		m_uiRegion_Labels			= (unsigned int*)GetAudioEngine()->MemoryAllocate(m_uiRegion_Count * sizeof(unsigned int), eMemoryTracker_General);
		m_uiRegion_StartPositions	= (unsigned int*)GetAudioEngine()->MemoryAllocate(m_uiRegion_Count * sizeof(unsigned int), eMemoryTracker_General);
		m_uiRegion_StopPositions	= (unsigned int*)GetAudioEngine()->MemoryAllocate(m_uiRegion_Count * sizeof(unsigned int), eMemoryTracker_General);
	}
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_Region_Label(unsigned int Param_uiIndex)
{
	if ((m_uiRegion_Labels == NULL) || (Param_uiIndex >= m_uiRegion_Count))
	{
		return 0;
	}

	return m_uiRegion_Labels[Param_uiIndex];
}

// ***********************************************************************************************************************

void CCompactHeader::Set_Region_Label(unsigned int Param_uiIndex, unsigned int Param_uiRegion_Label)
{
	if ((m_uiRegion_Labels == NULL) || (Param_uiIndex >= m_uiRegion_Count))
	{
		return;
	}

	m_uiRegion_Labels[Param_uiIndex] = Param_uiRegion_Label;
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_Region_StartPosition(unsigned int Param_uiIndex)
{
	if ((m_uiRegion_StartPositions == NULL) || (Param_uiIndex >= m_uiRegion_Count))
	{
		return 0;
	}

	return m_uiRegion_StartPositions[Param_uiIndex];
}

// ***********************************************************************************************************************

void CCompactHeader::Set_Region_StartPosition(unsigned int Param_uiIndex, unsigned int Param_uiRegion_StartPosition)
{
	if ((m_uiRegion_StartPositions == NULL) || (Param_uiIndex >= m_uiRegion_Count))
	{
		return;
	}

	m_uiRegion_StartPositions[Param_uiIndex] = Param_uiRegion_StartPosition;
}

// ***********************************************************************************************************************

unsigned int CCompactHeader::Get_Region_StopPosition(unsigned int Param_uiIndex)
{
	if ((m_uiRegion_StopPositions == NULL) || (Param_uiIndex >= m_uiRegion_Count))
	{
		return 0;
	}

	return m_uiRegion_StopPositions[Param_uiIndex];
}

// ***********************************************************************************************************************

void CCompactHeader::Set_Region_StopPosition(unsigned int Param_uiIndex, unsigned int Param_uiRegion_StopPosition)
{
	if ((m_uiRegion_StopPositions == NULL) || (Param_uiIndex >= m_uiRegion_Count))
	{
		return;
	}

	m_uiRegion_StopPositions[Param_uiIndex] = Param_uiRegion_StopPosition;
}

// ***********************************************************************************************************************
//    Class Methods : CHeaderList
// ***********************************************************************************************************************

CHeaderList::CHeaderList() : CItemList()
{
	m_pDatabaseFile			= NULL;
	m_uiDatabaseEntryCount	= 0;
	m_pMapFile_Handle		= NULL;
	m_pMapFile_Data			= NULL;
}

// ***********************************************************************************************************************

CHeaderList::~CHeaderList()
{
	// Delete File References

	if (m_pDatabaseFile != NULL)
	{
		delete m_pDatabaseFile;
	}

	if (m_pMapFile_Handle != NULL)
	{
		delete m_pMapFile_Handle;
	}

	// Empty Data Buffer

	if (m_pMapFile_Data != NULL)
	{
		delete m_pMapFile_Data;
	}
}

// ***********************************************************************************************************************

BOOL CHeaderList::IsDatabaseOpen()
{
	// Error Checking

	if (m_pDatabaseFile == NULL)
	{
		return FALSE;
	}

	return m_pDatabaseFile->IsOpen();
}

// ***********************************************************************************************************************

CCompactHeader* CHeaderList::Find(unsigned int Param_uiKey)
{
	// Error Checking

	if (IsEmpty())
	{
		return NULL;
	}

	// Scan List

	for (int iLoop = 0; iLoop < m_iSize_Items; iLoop++)
	{
		CCompactHeader* pHeader = (CCompactHeader*)m_pList[iLoop];

		if (pHeader->Get_Key() == Param_uiKey)
		{
			// Return Matching Header

			return pHeader;
		}
	}

	// Return NULL When Not Found

	return NULL;
}

// ***********************************************************************************************************************

CCompactHeader* CHeaderList::Request(unsigned int Param_uiKey, BOOL Param_bNoSearch)
{
	CCompactHeader* pHeader = NULL;

	// Scan For Existing Header (If Applicable)

	if (!Param_bNoSearch)
	{
		pHeader = Find(Param_uiKey);
	}

	// Create New If Not Found

	if (pHeader == NULL)
	{
		// Setup New Header

		pHeader = new CCompactHeader(Param_uiKey);

		// Add To List

		AddItem((CListItem*)pHeader);
	}

	return pHeader;
}

// ***********************************************************************************************************************

void CHeaderList::BuildEntireDatabase()
{
#if !defined(_FINAL_)
	// Error Checking

	if (!IsDatabaseOpen())
	{
		return;
	}

	// Parse All Waves

	CWaveParser* pWaveParser = new CWaveParser();

	for (int iLoop_BF = 0; iLoop_BF < eSoundBF_Count; iLoop_BF++)
	{
		CBigFileReader* pSoundBF = GetAudioEngine()->SoundFile_GetBF((eSoundBFIndex)iLoop_BF);

		if (pSoundBF != NULL)
		{
			for (int iLoop_Entry = 0; iLoop_Entry < pSoundBF->GetEntryCount(); iLoop_Entry++)
			{
				stKeyPosEntry* pEntry = pSoundBF->GetEntry(iLoop_Entry);

				if (pEntry != NULL)
				{
					pWaveParser->ProcessRIFFFile(pEntry->uiPosition, pEntry->uiKey, (eSoundBFIndex)iLoop_BF);
				}
			}
		}
	}

	delete pWaveParser;
#endif
}

// ***********************************************************************************************************************

void CHeaderList::OpenDatabase()
{
	// Error Checking

	if (!g_bSoundDatabaseEnabled || IsDatabaseOpen())
	{
		return;
	}

	// Create / Open Database File

	m_pDatabaseFile = new CFile(kszHeaderDatabase_Filename, TRUE);

	if (m_pDatabaseFile == NULL)
	{
		return;
	}

	if (!m_pDatabaseFile->IsOpen())
	{
		delete m_pDatabaseFile;
		m_pDatabaseFile = NULL;
		return;
	}

	// Check For Empty File

	unsigned int uiFileSize = m_pDatabaseFile->GetSize();

	if (uiFileSize < sizeof(unsigned int))
	{
#if !defined(_FINAL_)
		// Write Version To Empty Database

		m_pDatabaseFile->Write32(kiHeaderDatabase_Version);
#endif
	}
	else
	{
		// Read Database In Loading Buffer

		CBuffer* pBuffer = new CBuffer();

		m_pDatabaseFile->Read(pBuffer->RequestStorage(uiFileSize), uiFileSize);

		unsigned int* pMemoryDatabase = (unsigned int*)pBuffer->GetMemoryPointer();

		// Read Version

		if (*(pMemoryDatabase++) != kiHeaderDatabase_Version)
		{
			// Close Database

			delete pBuffer;
			delete m_pDatabaseFile;
			m_pDatabaseFile = NULL;

#if !defined(_FINAL_)
			// Delete Obsolete Database

			DeleteFile(kszHeaderDatabase_Filename);

			// Re-Open

			OpenDatabase();
#endif
			return;
		}

		// Read Headers

		while (pMemoryDatabase < (unsigned int*)pBuffer->GetMemoryPointer(uiFileSize))
		{
			CCompactHeader* pHeader = Request(*(pMemoryDatabase++), TRUE);

			//pHeader->Set_SoundBF_FilePosition(*(pMemoryDatabase++));
			pHeader->Set_SoundBF_Index(*(pMemoryDatabase++));
			pHeader->Set_BigFile_DataPosition(*(pMemoryDatabase++));
			pHeader->Set_BigFile_DataSize(*(pMemoryDatabase++));
			pHeader->Set_Format_Tag(*(pMemoryDatabase++));
			pHeader->Set_Format_ChannelCount(*(pMemoryDatabase++));
			pHeader->Set_Format_SamplesPerSecond(*(pMemoryDatabase++));
			pHeader->Set_Format_AverageBytesPerSecond(*(pMemoryDatabase++));
			pHeader->Set_Format_BlockAlign(*(pMemoryDatabase++));
			pHeader->Set_Format_BitsPerSample(*(pMemoryDatabase++));
			pHeader->Set_Format_ExtraInfoSize(*(pMemoryDatabase++));
			pHeader->Set_Format_SamplesPerBlock(*(pMemoryDatabase++));

			pHeader->Set_Loop(*(pMemoryDatabase++));
			pHeader->Set_Loop_BeginOffset(*(pMemoryDatabase++));
			pHeader->Set_Loop_EndOffset(*(pMemoryDatabase++));
			pHeader->Set_Offset_Start(*(pMemoryDatabase++));
			pHeader->Set_Offset_End(*(pMemoryDatabase++));
			pHeader->Set_ExitPoint_Count(*(pMemoryDatabase++));
			pHeader->Set_SignalPoint_Count(*(pMemoryDatabase++));
			pHeader->Set_Region_Count(*(pMemoryDatabase++));

			for (unsigned int uiLoop = 0; uiLoop < pHeader->Get_ExitPoint_Count(); uiLoop++)
			{
				pHeader->Set_ExitPoint_Value(uiLoop, *(pMemoryDatabase++));
			}

			for (unsigned int uiLoop = 0; uiLoop < pHeader->Get_SignalPoint_Count(); uiLoop++)
			{
				pHeader->Set_SignalPoint_Position(uiLoop, *(pMemoryDatabase++));
				pHeader->Set_SignalPoint_Label(uiLoop, *(pMemoryDatabase++));
			}

			for (unsigned int uiLoop = 0; uiLoop < pHeader->Get_Region_Count(); uiLoop++)
			{
				pHeader->Set_Region_Label(uiLoop, *(pMemoryDatabase++));
				pHeader->Set_Region_StartPosition(uiLoop, *(pMemoryDatabase++));
				pHeader->Set_Region_StopPosition(uiLoop, *(pMemoryDatabase++));
			}

			m_uiDatabaseEntryCount++;
		}

		// Delete Loading Buffer

		delete pBuffer;
	}
}

// ***********************************************************************************************************************

void CHeaderList::UpdateDatabase()
{
#if !defined(_FINAL_)
	// Error Checking

	if (!IsDatabaseOpen())
	{
		return;
	}

	// Write All New Headers

	while (m_uiDatabaseEntryCount < (unsigned int)m_iSize_Items)
	{
		CCompactHeader* pHeader = (CCompactHeader*)m_pList[m_uiDatabaseEntryCount];

		m_pDatabaseFile->Write32(pHeader->Get_Key());

		//m_pDatabaseFile->Write32(pHeader->Get_SoundBF_FilePosition());
		m_pDatabaseFile->Write32(pHeader->Get_SoundBF_Index());
		m_pDatabaseFile->Write32(pHeader->Get_BigFile_DataPosition());
		m_pDatabaseFile->Write32(pHeader->Get_BigFile_DataSize());
		m_pDatabaseFile->Write32(pHeader->Get_Format_Tag());
		m_pDatabaseFile->Write32(pHeader->Get_Format_ChannelCount());
		m_pDatabaseFile->Write32(pHeader->Get_Format_SamplesPerSecond());
		m_pDatabaseFile->Write32(pHeader->Get_Format_AverageBytesPerSecond());
		m_pDatabaseFile->Write32(pHeader->Get_Format_BlockAlign());
		m_pDatabaseFile->Write32(pHeader->Get_Format_BitsPerSample());
		m_pDatabaseFile->Write32(pHeader->Get_Format_ExtraInfoSize());
		m_pDatabaseFile->Write32(pHeader->Get_Format_SamplesPerBlock());
		m_pDatabaseFile->Write32(pHeader->Get_Loop());
		m_pDatabaseFile->Write32(pHeader->Get_Loop_BeginOffset());
		m_pDatabaseFile->Write32(pHeader->Get_Loop_EndOffset());
		m_pDatabaseFile->Write32(pHeader->Get_Offset_Start());
		m_pDatabaseFile->Write32(pHeader->Get_Offset_End());
		m_pDatabaseFile->Write32(pHeader->Get_ExitPoint_Count());
		m_pDatabaseFile->Write32(pHeader->Get_SignalPoint_Count());
		m_pDatabaseFile->Write32(pHeader->Get_Region_Count());

		for (unsigned int uiLoop = 0; uiLoop < pHeader->Get_ExitPoint_Count(); uiLoop++)
		{
			m_pDatabaseFile->Write32(pHeader->Get_ExitPoint_Value(uiLoop));
		}

		for (unsigned int uiLoop = 0; uiLoop < pHeader->Get_SignalPoint_Count(); uiLoop++)
		{
			m_pDatabaseFile->Write32(pHeader->Get_SignalPoint_Position(uiLoop));
			m_pDatabaseFile->Write32(pHeader->Get_SignalPoint_Label(uiLoop));
		}

		for (unsigned int uiLoop = 0; uiLoop < pHeader->Get_Region_Count(); uiLoop++)
		{
			m_pDatabaseFile->Write32(pHeader->Get_Region_Label(uiLoop));
			m_pDatabaseFile->Write32(pHeader->Get_Region_StartPosition(uiLoop));
			m_pDatabaseFile->Write32(pHeader->Get_Region_StopPosition(uiLoop));
		}

		m_uiDatabaseEntryCount++;
	}
#endif
}

// ***********************************************************************************************************************

void CHeaderList::CloseDatabase()
{
	// Error Checking

	if (!IsDatabaseOpen())
	{
		return;
	}

	// Delete Database Reference

	delete m_pDatabaseFile;
	m_pDatabaseFile = NULL;
}

// ***********************************************************************************************************************

void CHeaderList::AddToDatabase(SND_tdst_WaveDesc* Param_pWaveDescriptor, unsigned int Param_uiSoundBF_FilePosition)
{
	// Error Checking

	if (!IsDatabaseOpen())
	{
		return;
	}

	CCompactHeader* pHeader = GetAudioEngine()->GetHeaderList()->Request(Param_pWaveDescriptor->ul_FileKey, TRUE);

	if (pHeader == NULL)
	{
		return;
	}

	//pHeader->Set_SoundBF_FilePosition(Param_uiSoundBF_FilePosition);
	pHeader->Set_SoundBF_Index(Param_pWaveDescriptor->cSoundBFIndex);
	pHeader->Set_BigFile_DataPosition(Param_pWaveDescriptor->ul_DataPosition);
	pHeader->Set_BigFile_DataSize(Param_pWaveDescriptor->ul_DataSize);
	pHeader->Set_Format_Tag(Param_pWaveDescriptor->wFormatTag);
	pHeader->Set_Format_ChannelCount(Param_pWaveDescriptor->wChannels);
	pHeader->Set_Format_SamplesPerSecond(Param_pWaveDescriptor->dwSamplesPerSec);
	pHeader->Set_Format_AverageBytesPerSecond(Param_pWaveDescriptor->dwAvgBytesPerSec);
	pHeader->Set_Format_BlockAlign(Param_pWaveDescriptor->wBlockAlign);
	pHeader->Set_Format_BitsPerSample(Param_pWaveDescriptor->wBitsPerSample);
	pHeader->Set_Format_ExtraInfoSize(Param_pWaveDescriptor->cbSize);
	pHeader->Set_Format_SamplesPerBlock(Param_pWaveDescriptor->wSamplesPerBlock);
	pHeader->Set_Loop(Param_pWaveDescriptor->b_HasLoop);
	pHeader->Set_Loop_BeginOffset(Param_pWaveDescriptor->ul_LoopBeginOffset);
	pHeader->Set_Loop_EndOffset(Param_pWaveDescriptor->ul_LoopEndOffset);
	pHeader->Set_Offset_Start(Param_pWaveDescriptor->ul_StartOffset);
	pHeader->Set_Offset_End(Param_pWaveDescriptor->ul_EndOffset);
	pHeader->Set_ExitPoint_Count(Param_pWaveDescriptor->ul_ExitPointNb);
	pHeader->Set_SignalPoint_Count(Param_pWaveDescriptor->ul_SignalPointNb);
	pHeader->Set_Region_Count(Param_pWaveDescriptor->ul_RegionNb);

	for (unsigned int uiLoop = 0; uiLoop < Param_pWaveDescriptor->ul_ExitPointNb; uiLoop++)
	{
		pHeader->Set_ExitPoint_Value(uiLoop, Param_pWaveDescriptor->aul_ExitPoint[uiLoop]);
	}

	for (unsigned int uiLoop = 0; uiLoop < Param_pWaveDescriptor->ul_SignalPointNb; uiLoop++)
	{
		pHeader->Set_SignalPoint_Position(uiLoop, Param_pWaveDescriptor->dst_SignalTable[uiLoop].ul_Position);
		pHeader->Set_SignalPoint_Label(uiLoop, Param_pWaveDescriptor->dst_SignalTable[uiLoop].ul_Label);
	}

	for (unsigned int uiLoop = 0; uiLoop < Param_pWaveDescriptor->ul_RegionNb; uiLoop++)
	{
		pHeader->Set_Region_Label(uiLoop, Param_pWaveDescriptor->dst_RegionTable[uiLoop].ul_Label);
		pHeader->Set_Region_StartPosition(uiLoop, Param_pWaveDescriptor->dst_RegionTable[uiLoop].ul_StartPosition);
		pHeader->Set_Region_StopPosition(uiLoop, Param_pWaveDescriptor->dst_RegionTable[uiLoop].ul_StopPosition);
	}

	GetAudioEngine()->GetHeaderList()->UpdateDatabase();
}

// ***********************************************************************************************************************

SND_tdst_WaveDesc* CHeaderList::RetrieveFromDatabase(unsigned int Param_uiKey/*, unsigned int Param_uiSoundBF_FilePosition, int Param_iSoundBF_Index*/)
{
	// Error Checking

	if (!IsDatabaseOpen())
	{
		return (SND_tdst_WaveDesc*)0xFFFFFFFF;
	}

	// Search For Key

	CCompactHeader* pHeader = Find(Param_uiKey);

	// Header Not Found Or Position Mismatch

	if ((pHeader == NULL)/* || (Param_uiSoundBF_FilePosition != pHeader->Get_SoundBF_FilePosition()) || (Param_iSoundBF_Index != pHeader->Get_SoundBF_Index())*/)
	{
		return (SND_tdst_WaveDesc*)0xFFFFFFFF;
	}

	// Allocate Wave Descriptor

	SND_tdst_WaveDesc* pWaveDescriptor = (SND_tdst_WaveDesc*)GetAudioEngine()->MemoryAllocate(sizeof(SND_tdst_WaveDesc), eMemoryTracker_General);

	// Populate Wave Descriptor

	pWaveDescriptor->ul_FileKey			= Param_uiKey;
	pWaveDescriptor->ul_UserCount		= 1;
	pWaveDescriptor->cSoundBFIndex		= pHeader->Get_SoundBF_Index();
	pWaveDescriptor->ul_DataPosition	= pHeader->Get_BigFile_DataPosition();
	pWaveDescriptor->ul_DataSize		= pHeader->Get_BigFile_DataSize();
	pWaveDescriptor->wFormatTag			= pHeader->Get_Format_Tag();
	pWaveDescriptor->wChannels			= pHeader->Get_Format_ChannelCount();
	pWaveDescriptor->dwSamplesPerSec	= pHeader->Get_Format_SamplesPerSecond();
	pWaveDescriptor->dwAvgBytesPerSec	= pHeader->Get_Format_AverageBytesPerSecond();
	pWaveDescriptor->wBlockAlign		= pHeader->Get_Format_BlockAlign();
	pWaveDescriptor->wBitsPerSample		= pHeader->Get_Format_BitsPerSample();
	pWaveDescriptor->cbSize				= pHeader->Get_Format_ExtraInfoSize();
	pWaveDescriptor->wSamplesPerBlock	= pHeader->Get_Format_SamplesPerBlock();
	pWaveDescriptor->b_HasLoop			= pHeader->Get_Loop();
	pWaveDescriptor->ul_LoopBeginOffset	= pHeader->Get_Loop_BeginOffset();
	pWaveDescriptor->ul_LoopEndOffset	= pHeader->Get_Loop_EndOffset();
	pWaveDescriptor->ul_StartOffset		= pHeader->Get_Offset_Start();
	pWaveDescriptor->ul_EndOffset		= pHeader->Get_Offset_End();
	pWaveDescriptor->ul_ExitPointNb		= pHeader->Get_ExitPoint_Count();
	pWaveDescriptor->ul_SignalPointNb	= pHeader->Get_SignalPoint_Count();
	pWaveDescriptor->ul_RegionNb		= pHeader->Get_Region_Count();

	if (pWaveDescriptor->ul_ExitPointNb > 0)
	{
		// Allocate List

		pWaveDescriptor->aul_ExitPoint = (ULONG*)GetAudioEngine()->MemoryAllocate(pWaveDescriptor->ul_ExitPointNb * sizeof(ULONG), eMemoryTracker_General);

		// Populate List

		for (unsigned int uiLoop = 0; uiLoop < pWaveDescriptor->ul_ExitPointNb; uiLoop++)
		{
			pWaveDescriptor->aul_ExitPoint[uiLoop] = pHeader->Get_ExitPoint_Value(uiLoop);
		}
	}
	else
	{
		pWaveDescriptor->aul_ExitPoint = NULL;
	}

	if (pWaveDescriptor->ul_SignalPointNb > 0)
	{
		// Allocate List

		pWaveDescriptor->dst_SignalTable = (SND_tdst_SignalPoint*)GetAudioEngine()->MemoryAllocate(pWaveDescriptor->ul_SignalPointNb * sizeof(SND_tdst_SignalPoint), eMemoryTracker_General);

		// Populate List

		for (unsigned int uiLoop = 0; uiLoop < pWaveDescriptor->ul_SignalPointNb; uiLoop++)
		{
			pWaveDescriptor->dst_SignalTable[uiLoop].ul_Position	= pHeader->Get_SignalPoint_Position(uiLoop);
			pWaveDescriptor->dst_SignalTable[uiLoop].ul_Label		= pHeader->Get_SignalPoint_Label(uiLoop);
		}
	}
	else
	{
		pWaveDescriptor->dst_SignalTable = NULL;
	}

	if (pWaveDescriptor->ul_RegionNb > 0)
	{
		// Allocate List

		pWaveDescriptor->dst_RegionTable = (SND_tdst_Region*)GetAudioEngine()->MemoryAllocate(pWaveDescriptor->ul_RegionNb * sizeof(SND_tdst_Region), eMemoryTracker_General);

		// Populate List

		for (unsigned int uiLoop = 0; uiLoop < pWaveDescriptor->ul_RegionNb; uiLoop++)
		{
			pWaveDescriptor->dst_RegionTable[uiLoop].ul_Label			= pHeader->Get_Region_Label(uiLoop);
			pWaveDescriptor->dst_RegionTable[uiLoop].ul_StartPosition	= pHeader->Get_Region_StartPosition(uiLoop);
			pWaveDescriptor->dst_RegionTable[uiLoop].ul_StopPosition	= pHeader->Get_Region_StopPosition(uiLoop);
		}
	}
	else
	{
		pWaveDescriptor->dst_RegionTable = NULL;
	}

	return pWaveDescriptor;
}

// ***********************************************************************************************************************

void CHeaderList::OpenMapFile(unsigned int Param_uiMapKey)
{
	// Error Checking

	if (!g_bSoundDatabaseEnabled)
	{
		return;
	}

	// Close Previous Map File (If Applicable)

	if (m_pMapFile_Handle != NULL)
	{
		CloseMapFile();
	}

	// Prepare Filename

	sprintf(m_acFilename, "GAME:\\Sound\\Sound_%08x.mapfile", Param_uiMapKey);

	// Open File

	m_pMapFile_Handle = new CFile(m_acFilename, TRUE);

	// Read Size

	unsigned int uiFileSize = m_pMapFile_Handle->GetSize();

	// Read Data (If Applicable)

	if (uiFileSize > 0)
	{
		// Allocate Buffer

		if (m_pMapFile_Data == NULL)
		{
			m_pMapFile_Data = new CBuffer();
		}

		// Read Into Buffer

		m_pMapFile_Handle->Read(m_pMapFile_Data->RequestStorage(uiFileSize), uiFileSize);

		// Create Prefetch Entries

		unsigned int uiDataIndex = 0;

		while (uiDataIndex < uiFileSize)
		{
			stMapFileEntry*		pEntry	= (stMapFileEntry*)m_pMapFile_Data->GetMemoryPointer(uiDataIndex);
			CPrefetchBuffer*	pBuffer	= GetAudioEngine()->GetPrefetchList()->Request(pEntry->uiPosition, pEntry->uiSize, pEntry->eSoundBF);

			pBuffer->StoreDataToPrefetch(&pEntry->acData);

			uiDataIndex += (3 * sizeof(int)) + pEntry->uiSize;
		}

		// Close Map File

		CloseMapFile();
	}
}

// ***********************************************************************************************************************

void CHeaderList::AddToMapFile(unsigned int Param_uiPosition, eSoundBFIndex Param_eSoundBF, unsigned int Param_uiSize, char* Param_pData)
{
#if !defined(_FINAL_)
	// Error Checking

	if (m_pMapFile_Handle == NULL)
	{
		return;
	}

	// Write Entry

	m_pMapFile_Handle->Write32(Param_uiPosition);
	m_pMapFile_Handle->Write32(Param_eSoundBF);
	m_pMapFile_Handle->Write32(Param_uiSize);

	m_pMapFile_Handle->Write(Param_pData, Param_uiSize);
#endif
}

// ***********************************************************************************************************************

void CHeaderList::CloseMapFile()
{
	// Close File

	if (m_pMapFile_Handle != NULL)
	{
		int iSize = m_pMapFile_Handle->GetSize();

		delete m_pMapFile_Handle;
		m_pMapFile_Handle = NULL;

		// Delete Actual File If Empty

#if !defined(_FINAL_)
		if (iSize == 0)
		{
			DeleteFile(m_acFilename);
		}
#endif
	}

	// Discard Data

	if (m_pMapFile_Data != NULL)
	{
		delete m_pMapFile_Data;
		m_pMapFile_Data = NULL;
	}
}

// ***********************************************************************************************************************

#endif // _XENON
