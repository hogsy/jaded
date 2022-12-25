#include "Precomp.h"

#ifdef _XENON

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "xeSND_Engine.h"

// ***********************************************************************************************************************
//    Class Methods : CFile
// ***********************************************************************************************************************

CFile::CFile(const char* Param_pName, BOOL bIsReadWrite)
{
	m_bIsOpen = FALSE;

#if !defined(_FINAL_)
	if (bIsReadWrite)
	{
		// Create Handles - Read / Write - No Share

	    m_hHandle_Sync	= CreateFile(Param_pName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	    m_hHandle_Async	= INVALID_HANDLE_VALUE;

		// Ensure Successful Open

		if (m_hHandle_Sync != INVALID_HANDLE_VALUE)
		{
			m_bIsOpen		= TRUE;
			m_bIsReadWrite	= TRUE;
		}
	}
	else
#endif
	{
		// Create Handles - Read Only - Shared

		m_hHandle_Sync	= CreateFile(Param_pName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL                       , NULL);
		m_hHandle_Async	= CreateFile(Param_pName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

		// Ensure Successful Open

		if ((m_hHandle_Sync != INVALID_HANDLE_VALUE) && (m_hHandle_Async != INVALID_HANDLE_VALUE))
		{
			m_bIsOpen		= TRUE;
			m_bIsReadWrite	= FALSE;
		}
	}
}

// ***********************************************************************************************************************

CFile::~CFile()
{
	// Close Handle - Synchronous

	if (m_hHandle_Sync != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hHandle_Sync);
	}

	// Close Handle - Asynchronous

	if (m_hHandle_Async != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hHandle_Async);
	}
}

// ***********************************************************************************************************************

BOOL CFile::IsOpen()
{
	return m_bIsOpen;
}

// ***********************************************************************************************************************

HANDLE CFile::GetHandle_Sync()
{
	// Error Checking

	if (!m_bIsOpen)
	{
		return INVALID_HANDLE_VALUE;
	}

	return m_hHandle_Sync;
}

// ***********************************************************************************************************************

HANDLE CFile::GetHandle_Async()
{
	// Error Checking

	if (!m_bIsOpen)
	{
		return INVALID_HANDLE_VALUE;
	}

	return m_hHandle_Async;
}

// ***********************************************************************************************************************

unsigned int CFile::GetSize()
{
	// Error Checking

	if (!m_bIsOpen)
	{
		return 0;
	}

	return GetFileSize(m_hHandle_Sync, NULL);
}

// ***********************************************************************************************************************

void CFile::Read(char* Param_pData, unsigned int Param_uiSize)
{
	// Error Checking

	if (!m_bIsOpen)
	{
		return;
	}

	// Read From File

	unsigned int uiBytesRead;

	ReadFile(m_hHandle_Sync, Param_pData, Param_uiSize, (LPDWORD)&uiBytesRead, NULL);
}

// ***********************************************************************************************************************

void CFile::Write(char* Param_pData, unsigned int Param_uiSize)
{
#if !defined(_FINAL_)
	// Error Checking

	if (!m_bIsOpen)
	{
		return;
	}

	// Write To File

	unsigned int uiBytesWritten;

	WriteFile(m_hHandle_Sync, Param_pData, Param_uiSize, (LPDWORD)&uiBytesWritten, NULL);
#endif
}

// ***********************************************************************************************************************

void CFile::Write32(unsigned int Param_uiValue)
{
#if !defined(_FINAL_)
	if (m_bIsOpen && m_bIsReadWrite)
	{
		unsigned int uiBytesWritten;

		WriteFile(m_hHandle_Sync, &Param_uiValue, sizeof(unsigned int), (LPDWORD)&uiBytesWritten, NULL);
	}
#endif
}

// ***********************************************************************************************************************
//    Class Methods : CBuffer
// ***********************************************************************************************************************

CBuffer::CBuffer(BOOL Param_bIsPersistent)
{
	m_pMemory			= NULL;
	m_iSize_Capacity	= 0;
	m_iSize_Data		= 0;
	m_iReferenceCounter	= 0;
	m_bIsPersistent		= Param_bIsPersistent;
	m_bIsMemoryOwner	= TRUE;
	m_eMemoryTracker	= eMemoryTracker_General;
}

// ***********************************************************************************************************************

CBuffer::~CBuffer()
{
	Release(TRUE);
}

// ***********************************************************************************************************************

BOOL CBuffer::IsEmpty()
{
	return (m_pMemory == NULL);
}

// ***********************************************************************************************************************

BOOL CBuffer::IsPersistent()
{
	return m_bIsPersistent;
}

// ***********************************************************************************************************************

BOOL CBuffer::IsMemoryOwner()
{
	return m_bIsMemoryOwner;
}

// ***********************************************************************************************************************

int CBuffer::GetSize_Capacity()
{
	return m_iSize_Capacity;
}

// ***********************************************************************************************************************

int CBuffer::GetSize_Data()
{
	return m_iSize_Data;
}

// ***********************************************************************************************************************

char* CBuffer::GetMemoryPointer(int Param_iOffsetInBuffer)
{
	if ((m_pMemory == NULL) || (Param_iOffsetInBuffer < 0))
	{
		return NULL;
	}
	else
	{
		return (m_pMemory + Param_iOffsetInBuffer);
	}
}

// ***********************************************************************************************************************

void CBuffer::SetPersistence(BOOL Param_bIsPersistent)
{
	m_bIsPersistent = Param_bIsPersistent;
}

// ***********************************************************************************************************************

eMemoryTrackers CBuffer::GetTracker()
{
	return m_eMemoryTracker;
}

// ***********************************************************************************************************************

void CBuffer::SetTracker(eMemoryTrackers Param_eMemoryTracker)
{
	m_eMemoryTracker = Param_eMemoryTracker;
}

// ***********************************************************************************************************************

void CBuffer::Acquire(CBuffer* Param_pBuffer, BOOL Param_bIsNewBufferOwner)
{
	// Error Checking

	if (Param_pBuffer == NULL)
	{
		return;
	}

	// Destroy Previous Buffer

	Release(TRUE);

	// Assign New Memory Pointer

	m_pMemory			= Param_pBuffer->GetMemoryPointer();
	m_iSize_Capacity	= Param_pBuffer->GetSize_Capacity();
	m_iSize_Data		= Param_pBuffer->GetSize_Data();
	m_bIsPersistent		= Param_pBuffer->IsPersistent();
	m_iReferenceCounter	= 0;
	m_bIsMemoryOwner	= Param_bIsNewBufferOwner;
	m_eMemoryTracker	= Param_pBuffer->GetTracker();
}

// ***********************************************************************************************************************

void CBuffer::Acquire(char* Param_pMemory, int Param_iSize_Capacity, BOOL Param_bIsNewBufferOwner)
{
	// Error Checking

	if ((Param_pMemory == NULL) || (Param_iSize_Capacity <= 0))
	{
		return;
	}

	// Destroy Previous Buffer

	Release(TRUE);

	// Assign New Memory Pointer

	m_pMemory			= Param_pMemory;
	m_iSize_Capacity	= Param_iSize_Capacity;
	m_iSize_Data		= 0;
	m_iReferenceCounter	= 0;
	m_bIsMemoryOwner	= Param_bIsNewBufferOwner;
}

// ***********************************************************************************************************************

char* CBuffer::RequestStorage(int Param_iSize_Capacity, int Param_iOffsetInBuffer)
{
	// Error Checking

	// - Negative Request Size

	if (Param_iSize_Capacity < 0)
	{
		return NULL;
	}

	// - No Request Size On Empty Buffer

	if ((Param_iSize_Capacity == 0) && (m_iSize_Capacity <= 0))
	{
		return NULL;
	}

	// - Need Reallocation On In-Use Buffer

	if (((Param_iSize_Capacity + Param_iOffsetInBuffer) > m_iSize_Capacity) && (m_iReferenceCounter > 0))
	{
		return NULL;
	}

	// Reallocate Buffer (If Applicable)

	if ((Param_iSize_Capacity + Param_iOffsetInBuffer) > m_iSize_Capacity)
	{
		// Destroy Previous Buffer

		Release(TRUE);

		// Round Up Size To Next Multiple of 4

		m_iSize_Capacity = ((Param_iSize_Capacity + Param_iOffsetInBuffer) + 3) & -4;

		// Allocate Memory (With 16-Bytes Safety Padding)

		m_pMemory			= (char*)GetAudioEngine()->MemoryAllocate(m_iSize_Capacity + 16, m_eMemoryTracker);
		m_bIsMemoryOwner	= TRUE;
	}

	// Increment Reference Counter

	m_iReferenceCounter++;

	// Return Memory Pointer

	return (m_pMemory + Param_iOffsetInBuffer);
}

// ***********************************************************************************************************************

void CBuffer::StoreData(char* Param_pSourcePointer, int Param_iSize_Data)
{
	// Error Checking

	if ((Param_pSourcePointer == NULL) || (Param_iSize_Data <= 0))
	{
		return;
	}

	// Request Storage Capacity

	char* pBuffer = RequestStorage(Param_iSize_Data);

	if (pBuffer == NULL)
	{
		return;
	}

	// Transfer From Memory

	memcpy(pBuffer, Param_pSourcePointer, Param_iSize_Data);

	// Update Fields

	m_iSize_Data = Param_iSize_Data;
}

// ***********************************************************************************************************************

void CBuffer::RetrieveData(char* Param_pDestPointer, int Param_iSize)
{
	// Use Store Size By Default

	if (Param_iSize <= 0)
	{
		Param_iSize = m_iSize_Data;
	}

	// Error Checking

	if ((Param_pDestPointer == NULL) || (Param_iSize <= 0))
	{
		return;
	}

	// Transfer To Memory

	memcpy(Param_pDestPointer, m_pMemory, Param_iSize);
}

// ***********************************************************************************************************************

void CBuffer::Release(BOOL Param_bForceDestroy)
{
	// Decrement Reference Counter

	if (m_iReferenceCounter > 0)
	{
		m_iReferenceCounter--;
	}

	// Destroy Buffer (If Applicable)

	if (m_pMemory != NULL)
	{
		if (Param_bForceDestroy || (!m_bIsPersistent && (m_iReferenceCounter <= 0)))
		{
			// Free Memory

			if (m_bIsMemoryOwner)
			{
				GetAudioEngine()->MemoryFree(m_pMemory, m_eMemoryTracker);
			}

			// Reset Fields

			m_pMemory			= NULL;
			m_iSize_Capacity	= 0;
			m_iSize_Data		= 0;
			m_iReferenceCounter	= 0;
		}
	}
}

// ***********************************************************************************************************************
//    Class Methods : CPacketTracker
// ***********************************************************************************************************************

CPacketTracker::CPacketTracker()
{
	Reset();
}

// ***********************************************************************************************************************

CPacketTracker::~CPacketTracker()
{
}

// ***********************************************************************************************************************

void CPacketTracker::Reset()
{
	m_iCircularIndex_Loading	= 0;
	m_iCircularIndex_Playing	= 0;
	m_iCounter_Loading			= 0;
	m_iCounter_Submitted		= 0;
	m_iCounter_LoadingTimeout	= kiLoadingCounterTimeout;
}

// ***********************************************************************************************************************

void CPacketTracker::Notify_Submit(int Param_iPosition_Buffer)
{
	// Increment Submit Counter

	m_iCounter_Submitted++;

	// Store Sound Buffer Position

	m_astPositions[m_iCircularIndex_Playing].iBuffer = Param_iPosition_Buffer;
}

// ***********************************************************************************************************************

void CPacketTracker::Notify_PlayEnd()
{
	// Decrement Submit Counter

	m_iCounter_Submitted--;

	// Move To Next Sound Data Position

	m_iCircularIndex_Playing = (m_iCircularIndex_Playing + 1) % kiStreamPacketCount;
}

// ***********************************************************************************************************************

void CPacketTracker::Notify_LoadStart(int Param_iPosition_File)
{
	// Increment Loading Counter

	m_iCounter_Loading++;

	// Reset Timeout Counter

	m_iCounter_LoadingTimeout = kiLoadingCounterTimeout;

	// Store Sound Data Position

	m_astPositions[m_iCircularIndex_Loading].iFile = Param_iPosition_File;
	m_iCircularIndex_Loading = (m_iCircularIndex_Loading + 1) % kiStreamPacketCount;
}

// ***********************************************************************************************************************

void CPacketTracker::Notify_LoadEnd(BOOL Param_bPacketDropped)
{
	// Decrement Loading Counter

	m_iCounter_Loading--;

	// Move Back To Previous Data Position (If Applicable)

	if (Param_bPacketDropped)
	{
		m_iCircularIndex_Loading--;

		if (m_iCircularIndex_Loading < 0)
		{
			m_iCircularIndex_Loading += kiStreamPacketCount;
		}
	}
}

// ***********************************************************************************************************************

int CPacketTracker::GetCurrentPosition_File()
{
	if (m_iCounter_Submitted > 0)
	{
		return m_astPositions[m_iCircularIndex_Playing].iFile;
	}
	else
	{
		return 0;
	}
}

// ***********************************************************************************************************************

int CPacketTracker::GetCurrentPosition_Buffer()
{
	if (m_iCounter_Submitted > 0)
	{
		return m_astPositions[m_iCircularIndex_Playing].iBuffer;
	}
	else
	{
		return 0;
	}
}

// ***********************************************************************************************************************

int CPacketTracker::GetSubmitCounter()
{
	return m_iCounter_Submitted;
}

// ***********************************************************************************************************************

int CPacketTracker::GetLoadingCounter()
{
	return m_iCounter_Loading;
}

// ***********************************************************************************************************************

BOOL CPacketTracker::GetLoadingStateWithTimeout()
{
	if ((m_iCounter_Loading <= 0) || (m_iCounter_LoadingTimeout <= 0))
	{
		return FALSE;
	}

	m_iCounter_LoadingTimeout--;

	return TRUE;
}

// ***********************************************************************************************************************
//    Class Methods : CStreamReader
// ***********************************************************************************************************************

CStreamReader::CStreamReader(unsigned int Param_uiBufferSizeKB)
{
	// Error Checking

	if (Param_uiBufferSizeKB < 1)
	{
		Param_uiBufferSizeKB = 1;
	}

	// Initialize Fields

	m_uiBufferSize						= Param_uiBufferSizeKB * 1024;
	m_hFile								= NULL;
	m_uiOffset_StartInFile				= 0;
	m_iOffset_BufferInData				= 0;
	m_uiOffset_CurrentInBuffer			= 0;
	m_bDirtyBuffer						= TRUE;
	m_bIsOpen							= FALSE;
	m_uiOffset_LastLoadedBufferInFile	= 0xFFFFFFFF;

	// Allocate Buffer

	m_pBuffer = (unsigned char*)GetAudioEngine()->MemoryAllocate(m_uiBufferSize, eMemoryTracker_General);
}

// ***********************************************************************************************************************

CStreamReader::~CStreamReader()
{
	// Close File

	Close();

	// Clear Buffer

	GetAudioEngine()->MemoryFree(m_pBuffer, eMemoryTracker_General);
}

// ***********************************************************************************************************************

BOOL CStreamReader::Open(HANDLE Param_hFile, unsigned int Param_uiStartOffsetInFile, BOOL Param_bIsBigEndian)
{
	// Error Checking

	if ((Param_hFile == INVALID_HANDLE_VALUE) || m_bIsOpen)
	{
		return FALSE;
	}

	// Force Buffer Cache Clear If Different File

	if (m_hFile != Param_hFile)
	{
		m_uiOffset_LastLoadedBufferInFile = 0xFFFFFFFF;
	}

	// Populate Fields

	m_hFile						= Param_hFile;
	m_uiOffset_StartInFile		= Param_uiStartOffsetInFile;
	m_iOffset_BufferInData		= 0;
	m_uiOffset_CurrentInBuffer	= 0;
	m_bDirtyBuffer				= TRUE;
	m_bIsOpen					= TRUE;
	m_bIsBigEndian				= Param_bIsBigEndian;

	return TRUE;
}

// ***********************************************************************************************************************

BOOL CStreamReader::Close()
{
	// Error Checking

	if (!m_bIsOpen)
	{
		return FALSE;
	}

	// Update Fields

	m_bIsOpen = FALSE;

	return TRUE;
}

// ***********************************************************************************************************************

BOOL CStreamReader::SetDataPositionInFile(unsigned int Param_uiStartOffsetInFile)
{
	// Error Checking

	if (!m_bIsOpen)
	{
		return FALSE;
	}

	// Ensure Difference

	if (m_uiOffset_StartInFile == Param_uiStartOffsetInFile)
	{
		return TRUE;
	}

	// Update Fields

	m_uiOffset_StartInFile	= Param_uiStartOffsetInFile;
	m_bDirtyBuffer			= TRUE;

	return TRUE;
}

// ***********************************************************************************************************************

BOOL CStreamReader::Skip(unsigned int Param_uiCount)
{
	// Error Checking

	if (Param_uiCount <= 0)
	{
		return FALSE;
	}

	// Seek To New Position

	return SeekInData(m_iOffset_BufferInData + m_uiOffset_CurrentInBuffer + Param_uiCount);
}

// ***********************************************************************************************************************

BOOL CStreamReader::SeekInData(int Param_iOffsetInData)
{
	if (!m_bDirtyBuffer && (Param_iOffsetInData >= m_iOffset_BufferInData) && (Param_iOffsetInData < (m_iOffset_BufferInData + (signed int)m_uiBufferSize)))
	{
		// Already In Buffer

		m_uiOffset_CurrentInBuffer = Param_iOffsetInData - m_iOffset_BufferInData;
	}
	else
	{
		// Request New Buffer

		m_uiOffset_CurrentInBuffer	= 0;
		m_iOffset_BufferInData		= Param_iOffsetInData;
		m_bDirtyBuffer				= TRUE;
	}

	return TRUE;
}

// ***********************************************************************************************************************

BOOL CStreamReader::Read(char* Param_pBuffer, unsigned int Param_uiCount)
{
	// Error Checking

	if ((Param_pBuffer == NULL) || (Param_uiCount < 1))
	{
		return FALSE;
	}

	// Refresh Buffer (If Applicable)

	if (m_bDirtyBuffer)
	{
		if (!RefreshBuffer(FALSE))
		{
			return FALSE;
		}
	}

	// Head Block Loads (If Applicable)

	while (Param_uiCount > (m_uiBufferSize - m_uiOffset_CurrentInBuffer))
	{
		// Calculate Block Size

		unsigned int uiReadSize = (m_uiBufferSize - m_uiOffset_CurrentInBuffer);

		// Copy Block

		L_memcpy(Param_pBuffer, &m_pBuffer[m_uiOffset_CurrentInBuffer], uiReadSize);

		// Update Fields

		Param_pBuffer += uiReadSize;
		Param_uiCount -= uiReadSize;

		// Fetch Next Buffer Page

		m_iOffset_BufferInData += m_uiBufferSize;

		if (!RefreshBuffer(TRUE))
		{
			return FALSE;
		}
	}

	// Tail Block Load

	L_memcpy(Param_pBuffer, &m_pBuffer[m_uiOffset_CurrentInBuffer], Param_uiCount);

	m_uiOffset_CurrentInBuffer += Param_uiCount;

	return TRUE;
}

// ***********************************************************************************************************************

unsigned int CStreamReader::ReadString(char* Param_pBuffer)
{
	// Error Checking

	if (Param_pBuffer == NULL)
	{
		return 0;
	}

	// Refresh Buffer (If Applicable)

	if (m_bDirtyBuffer)
	{
		if (!RefreshBuffer(FALSE))
		{
			return 0;
		}
	}

	// Prepare Fields

	int iSize = -1;
	char uCharacter;

	// Scan Until Zero Found

	do
	{
		uCharacter = Read8();
		*Param_pBuffer++ = uCharacter;
		iSize++;
	}
	while (uCharacter != '\0');

	return iSize;
}

// ***********************************************************************************************************************

unsigned char CStreamReader::Read8()
{
	// Refresh Buffer (If Applicable)

	if (m_bDirtyBuffer)
	{
		if (!RefreshBuffer(FALSE))
		{
			return 0;
		}
	}

	if (m_uiOffset_CurrentInBuffer < m_uiBufferSize)
	{
		// Return Character

		return m_pBuffer[m_uiOffset_CurrentInBuffer++];
	}
	else
	{
		// Load Next Buffer Page

		m_iOffset_BufferInData += m_uiBufferSize;

		if (!RefreshBuffer(TRUE))
		{
			return 0;
		}

		return m_pBuffer[m_uiOffset_CurrentInBuffer++];
	}
}

// ***********************************************************************************************************************

unsigned short CStreamReader::Read16()
{
	// Refresh Buffer (If Applicable)

	if (m_bDirtyBuffer)
	{
		if (!RefreshBuffer(FALSE))
		{
			return 0;
		}
	}

	if ((m_uiOffset_CurrentInBuffer + 2) <= m_uiBufferSize)
	{
		// Return Short

		unsigned short usResult;

		if (m_bIsBigEndian)
		{
			usResult = (m_pBuffer[m_uiOffset_CurrentInBuffer] << 8) | m_pBuffer[m_uiOffset_CurrentInBuffer + 1];
		}
		else
		{
			usResult = m_pBuffer[m_uiOffset_CurrentInBuffer] | (m_pBuffer[m_uiOffset_CurrentInBuffer + 1] << 8);
		}

		m_uiOffset_CurrentInBuffer += 2;

		return usResult;
	}
	else
	{
		// Load Bytes Individually Through Page Boundary

		if (m_bIsBigEndian)
		{
			return (((unsigned short)Read8() << 8) | (unsigned short)Read8());
		}
		else
		{
			return ((unsigned short)Read8() | ((unsigned short)Read8() << 8));
		}
	}
}

// ***********************************************************************************************************************

unsigned int CStreamReader::Read32()
{
	// Refresh Buffer (If Applicable)

	if (m_bDirtyBuffer)
	{
		if (!RefreshBuffer(FALSE))
		{
			return 0;
		}
	}

	if ((m_uiOffset_CurrentInBuffer + 4) <= m_uiBufferSize)
	{
		// Return Integer

		unsigned int uiResult;

		if (m_bIsBigEndian)
		{
			uiResult = (m_pBuffer[m_uiOffset_CurrentInBuffer] << 24) | (m_pBuffer[m_uiOffset_CurrentInBuffer + 1] << 16) | (m_pBuffer[m_uiOffset_CurrentInBuffer + 2] << 8) | m_pBuffer[m_uiOffset_CurrentInBuffer + 3];
		}
		else
		{
			uiResult = m_pBuffer[m_uiOffset_CurrentInBuffer] | (m_pBuffer[m_uiOffset_CurrentInBuffer + 1] << 8) | (m_pBuffer[m_uiOffset_CurrentInBuffer + 2] << 16) | (m_pBuffer[m_uiOffset_CurrentInBuffer + 3] << 24);
		}

		m_uiOffset_CurrentInBuffer += 4;

		return uiResult;
	}
	else
	{
		// Load Bytes Individually Through Page Boundary

		if (m_bIsBigEndian)
		{
			return (((unsigned int)Read8() << 24) | ((unsigned int)Read8() << 16) | ((unsigned int)Read8() << 8) | (unsigned int)Read8());
		}
		else
		{
			return ((unsigned int)Read8() | ((unsigned int)Read8() << 8) | ((unsigned int)Read8() << 16) | ((unsigned int)Read8() << 24));
		}
	}
}

// ***********************************************************************************************************************

unsigned int CStreamReader::GetCurrentOffsetInData()
{
	return (m_iOffset_BufferInData + m_uiOffset_CurrentInBuffer);
}

// ***********************************************************************************************************************

BOOL CStreamReader::RefreshBuffer(BOOL Param_bSequential)
{
	// Error Checking

	if (!m_bIsOpen)
	{
		return FALSE;
	}

	// Determine Seek Position

	unsigned int uiSeekPositionInFile = (m_uiOffset_StartInFile + m_iOffset_BufferInData);

	// Check If Already In Cache

	if ((uiSeekPositionInFile >= m_uiOffset_LastLoadedBufferInFile) && (uiSeekPositionInFile < (m_uiOffset_LastLoadedBufferInFile + m_uiBufferSize)))
	{
		// Update Offset Positions

		m_iOffset_BufferInData		= (m_uiOffset_LastLoadedBufferInFile - m_uiOffset_StartInFile);
		m_uiOffset_CurrentInBuffer	= (uiSeekPositionInFile - m_uiOffset_LastLoadedBufferInFile);
	}
	else
	{
		// Seek To New Position (If Applicable)

		if (!Param_bSequential)
		{
			unsigned int uiHigh64 = 0;

			if (SetFilePointer(m_hFile, uiSeekPositionInFile, (PLONG)&uiHigh64, FILE_BEGIN) < 0)
			{
				return FALSE;
			}
		}

		// Load Page

		unsigned int uiBytesRead;

		if (!ReadFile(m_hFile, m_pBuffer, m_uiBufferSize, (LPDWORD)&uiBytesRead, NULL))
		{
			return FALSE;
		}

		// Update Offset Positions

		m_uiOffset_CurrentInBuffer			= 0;
		m_uiOffset_LastLoadedBufferInFile	= uiSeekPositionInFile;
	}

	// Update Dirty Flag

	m_bDirtyBuffer = FALSE;

	return TRUE;
}

// ***********************************************************************************************************************

unsigned int CStreamReader::GetRemainingUnreadInBuffer()
{
	// Ensure Current Buffer Isn't Obsolete

	if (m_bDirtyBuffer)
	{
		return 0;
	}

	return (m_uiBufferSize - m_uiOffset_CurrentInBuffer);
}

// ***********************************************************************************************************************

unsigned char* CStreamReader::GetCurrentBufferPointer()
{
	// Error Checking

	if (m_bDirtyBuffer || (m_uiOffset_CurrentInBuffer >= m_uiBufferSize))
	{
		return NULL;
	}

	// Retrieve Buffer Pointer

	return &m_pBuffer[m_uiOffset_CurrentInBuffer];
}

// ***********************************************************************************************************************
//    Class Methods : CListItem
// ***********************************************************************************************************************

CListItem::CListItem(BOOL Param_bIsOwnedByList)
{
	m_iListIndex		= 0;
	m_bIsOwnedByList	= Param_bIsOwnedByList;
}

// ***********************************************************************************************************************

CListItem::~CListItem()
{
}

// ***********************************************************************************************************************

int CListItem::GetListIndex()
{
	return m_iListIndex;
}

// ***********************************************************************************************************************

void CListItem::SetListIndex(int Param_iListIndex)
{
	m_iListIndex = Param_iListIndex;
}

// ***********************************************************************************************************************

BOOL CListItem::IsOwnedByList()
{
	return m_bIsOwnedByList;
}

// ***********************************************************************************************************************

BOOL CListItem::CanBeDeleted()
{
	return TRUE;
}

// ***********************************************************************************************************************
//    Class Methods : CItemList
// ***********************************************************************************************************************

CItemList::CItemList(BOOL Param_bFastGrowth)
{
	m_pList			= NULL;
	m_iSize_Buffer	= 0;
	m_iSize_Items	= 0;
	m_bFastGrowth	= Param_bFastGrowth;
}

// ***********************************************************************************************************************

CItemList::~CItemList()
{
	Flush();
}

// ***********************************************************************************************************************

BOOL CItemList::IsEmpty()
{
	return ((m_pList == NULL) || (m_iSize_Items <= 0));
}

// ***********************************************************************************************************************

BOOL CItemList::IsFull()
{
	return ((m_pList == NULL) || (m_iSize_Items >= m_iSize_Buffer));
}

// ***********************************************************************************************************************

int CItemList::GetCapacity()
{
	return m_iSize_Buffer;
}

// ***********************************************************************************************************************

int CItemList::GetItemCount()
{
	return m_iSize_Items;
}

// ***********************************************************************************************************************

int CItemList::AddItem(CListItem* Param_pItem)
{
	// Error Checking

	if (Param_pItem == NULL)
	{
		return -1;
	}

	// Check If There's Room

	if (IsFull())
	{
		Grow();
	}

	// Update Item List Index

	int iIndex = m_iSize_Items;

	Param_pItem->SetListIndex(iIndex);

	// Add To List

	m_pList[iIndex] = Param_pItem;

	m_iSize_Items++;

	return iIndex;
}

// ***********************************************************************************************************************

CListItem* CItemList::GetItem(int Param_iListIndex)
{
	// Error Checking

	if (IsEmpty() || (Param_iListIndex < 0) || (Param_iListIndex >= m_iSize_Items))
	{
		return NULL;
	}

	// Return Item Pointer

	return m_pList[Param_iListIndex];
}

// ***********************************************************************************************************************

int CItemList::GetItemIndex(CListItem* Param_pItem)
{
	// Error Checking

	if (IsEmpty() || (Param_pItem == NULL))
	{
		return -1;
	}

	// Find Item (In Case Pointer Is No Longer In List)

	for (int iLoop = 0; iLoop < m_iSize_Items; iLoop++)
	{
		if (m_pList[iLoop] == Param_pItem)
		{
			Param_pItem->SetListIndex(iLoop);
			return iLoop;
		}
	}

	return -1;
}

// ***********************************************************************************************************************

void CItemList::RemoveItem(int Param_iListIndex)
{
	// Error Checking

	if (IsEmpty() || (Param_iListIndex < 0) || (Param_iListIndex >= m_iSize_Items))
	{
		return;
	}

	// Destroy Voice

	if ((m_pList[Param_iListIndex] != NULL) && m_pList[Param_iListIndex]->IsOwnedByList())
	{
		delete m_pList[Param_iListIndex];
	}

	// Concatenate Items (If Applicable)

	if (Param_iListIndex < (m_iSize_Items - 1))
	{
		m_pList[Param_iListIndex] = m_pList[m_iSize_Items - 1];
		m_pList[Param_iListIndex]->SetListIndex(Param_iListIndex);
	}

	// Shorten List

	m_iSize_Items--;
	m_pList[m_iSize_Items] = NULL;
}

// ***********************************************************************************************************************

void CItemList::RemoveItem(CListItem* Param_pItem)
{
	// Error Checking

	if (Param_pItem == NULL)
	{
		return;
	}

	// Use Removal By Index

	RemoveItem(Param_pItem->GetListIndex());
}

// ***********************************************************************************************************************

void CItemList::Grow()
{
	// Compute New Buffer Size

	int iNewBufferSize = 1;

	if (m_iSize_Buffer > 0)
	{
		iNewBufferSize = m_iSize_Buffer * 2;
	}

	// Allocate New List

	CListItem** pNewList = (CListItem**)GetAudioEngine()->MemoryAllocate(iNewBufferSize * sizeof(CListItem*), eMemoryTracker_General);

	if (m_pList != NULL)
	{
		// Copy Old List To New

		for (int iLoop = 0; iLoop < m_iSize_Items; iLoop++)
		{
			pNewList[iLoop] = m_pList[iLoop];
		}

		// Free Old List

		GetAudioEngine()->MemoryFree(m_pList, eMemoryTracker_General);
	}

	m_iSize_Buffer = iNewBufferSize;

	// Assign New Queue

	m_pList = pNewList;
}

// ***********************************************************************************************************************

void CItemList::Flush()
{
	int iSurvivingItems = 0;

	if (m_pList != NULL)
	{
		// Destroy All Elements In List

		for (int iLoop = 0; iLoop < m_iSize_Items; iLoop++)
		{
			// Skip If Object Not Managed By List

			if ((m_pList[iLoop] != NULL) && m_pList[iLoop]->IsOwnedByList())
			{
				// Ensure Safe To Delete

				if (m_pList[iLoop]->CanBeDeleted())
				{
					// Destroy Item

					delete m_pList[iLoop];

					m_pList[iLoop] = NULL;
				}
				else
				{
					// Move Item To Beginning Of List

					if (iLoop != iSurvivingItems)
					{
						m_pList[iSurvivingItems] = m_pList[iLoop];
						m_pList[iLoop          ] = NULL;
					}

					iSurvivingItems++;
				}
			}
			else
			{
				m_pList[iLoop] = NULL;
			}
		}

		// Free Queue (If Applicable)

		if (iSurvivingItems <= 0)
		{
			GetAudioEngine()->MemoryFree(m_pList, eMemoryTracker_General);

			m_pList			= NULL;
			m_iSize_Buffer	= 0;
		}
	}

	// Update Item Count

	m_iSize_Items = iSurvivingItems;
}

// ***********************************************************************************************************************
//    Class Methods : CVoiceList
// ***********************************************************************************************************************

CVoice* CVoiceList::RequestVoice(SND_tdst_WaveData* Param_pWaveData, int Param_iFilePositionInBF, int Param_iStreamID, CBuffer* Param_pBuffer_Playback)
{
	// Create New Voice

	CVoice* pVoice = new CVoice(Param_pWaveData, Param_iFilePositionInBF, Param_iStreamID, Param_pBuffer_Playback);

	// Add To List

	AddItem((CListItem*)pVoice);

	return pVoice;
}

// ***********************************************************************************************************************

void CVoiceList::DestroyVoice(CVoice* Param_pVoice)
{
	RemoveItem(Param_pVoice->GetListIndex());
}

// ***********************************************************************************************************************

CVoice* CVoiceList::GetVoice(int Param_iListIndex)
{
	return (CVoice*)GetItem(Param_iListIndex);
}

// ***********************************************************************************************************************
//    Class Methods : CPrefetchBuffer
// ***********************************************************************************************************************

CPrefetchBuffer::CPrefetchBuffer() : CListItem()
{
	m_uiPosition	= 0xFFFFFFFF;
	m_uiSize		= 0xFFFFFFFF;
	m_eStatus		= ePrefetchStatus_Empty;
	m_pBuffer		= NULL;
}

// ***********************************************************************************************************************

CPrefetchBuffer::~CPrefetchBuffer()
{
	Clear();
}

// ***********************************************************************************************************************

BOOL CPrefetchBuffer::CanBeDeleted()
{
	return (m_eStatus != ePrefetchStatus_Loading);
;
}

// ***********************************************************************************************************************

ePrefetchStatus CPrefetchBuffer::GetStatus()
{
	return m_eStatus;
}

// ***********************************************************************************************************************

unsigned int CPrefetchBuffer::GetPosition()
{
	return m_uiPosition;
}

// ***********************************************************************************************************************

eSoundBFIndex CPrefetchBuffer::GetSoundBFIndex()
{
	return m_eSoundBFIndex;
}

// ***********************************************************************************************************************

unsigned int CPrefetchBuffer::GetSize()
{
	return m_uiSize;
}

// ***********************************************************************************************************************

void CPrefetchBuffer::Setup(unsigned int Param_uiPosition, unsigned int Param_uiSize, eSoundBFIndex Param_eSoundBFIndex)
{
	if ((m_uiPosition != Param_uiPosition) || (m_uiSize != Param_uiSize) || (m_eSoundBFIndex != Param_eSoundBFIndex))
	{
		m_uiPosition	= Param_uiPosition;
		m_uiSize		= Param_uiSize;
		m_eSoundBFIndex	= Param_eSoundBFIndex;
		m_eStatus		= ePrefetchStatus_Setup;
	}
}

// ***********************************************************************************************************************

void CPrefetchBuffer::StoreDataToPrefetch(char* Param_pSourceBuffer)
{
	// Error Checking

	if ((Param_pSourceBuffer == NULL) || (m_uiSize == 0xFFFFFFFF))
	{
		return;
	}

	// Copy Data To Buffer

	if (m_pBuffer == NULL)
	{
		m_pBuffer = new CBuffer();
	}

	m_pBuffer->StoreData(Param_pSourceBuffer, m_uiSize);

	m_eStatus = ePrefetchStatus_Useable;
}

// ***********************************************************************************************************************

void CPrefetchBuffer::RecoverDataFromPrefetch(char* Param_pDestBuffer)
{
	// Error Checking

	if ((Param_pDestBuffer == NULL) || (m_eStatus != ePrefetchStatus_Useable) || (m_pBuffer == NULL))
	{
		return;
	}

	// Copy Data From Buffer

	m_pBuffer->RetrieveData(Param_pDestBuffer, m_uiSize);
}

// ***********************************************************************************************************************

void CPrefetchBuffer::StartLoad()
{
	// Error Checking

	if ((m_uiPosition == 0xFFFFFFFF) || (m_uiSize == 0xFFFFFFFF) || (m_eStatus != ePrefetchStatus_Setup) || (m_pBuffer == NULL))
	{
		return;
	}

	// Start Load

	m_eStatus = ePrefetchStatus_Loading;

	GetAudioEngine()->SoundFile_Read(m_pBuffer->RequestStorage(m_uiSize), m_uiSize, (int*)&m_uiSizeRead, m_uiPosition, (void*)m_iListIndex, m_eSoundBFIndex, eEventID_PrefetchLoadDone);
}

// ***********************************************************************************************************************

void CPrefetchBuffer::EndLoad()
{
	if (m_eStatus == ePrefetchStatus_Loading)
	{
		// Make Data Useable

		m_eStatus = ePrefetchStatus_Useable;
	}
	else
	{
		// Prefetch Was Flushed During Loading, Discard Loaded Data

		Clear();
	}
}

// ***********************************************************************************************************************

void CPrefetchBuffer::Clear()
{
	if (m_pBuffer != NULL)
	{
		delete m_pBuffer;
		m_pBuffer = NULL;
	}

	m_uiPosition	= 0xFFFFFFFF;
	m_uiSize		= 0xFFFFFFFF;
	m_eSoundBFIndex = eSoundBF_None;
	m_eStatus		= ePrefetchStatus_Empty;
}

// ***********************************************************************************************************************
//    Class Methods : CPrefetchList
// ***********************************************************************************************************************

CPrefetchList::CPrefetchList() : CItemList()
{
	memset(m_aArrayMap, 0, sizeof(m_aArrayMap));
}

// ***********************************************************************************************************************

CPrefetchBuffer* CPrefetchList::Find(unsigned int Param_uiPosition, eSoundBFIndex Param_eSoundBFIndex)
{
	// Error Checking

	if (IsEmpty())
	{
		return NULL;
	}

	// Scan List

	for (int iLoop = 0; iLoop < m_iSize_Items; iLoop++)
	{
		CPrefetchBuffer* pBuffer = (CPrefetchBuffer*)m_pList[iLoop];

		if (pBuffer != NULL)
		{
			if ((pBuffer->GetPosition() == Param_uiPosition) && (pBuffer->GetSoundBFIndex() == Param_eSoundBFIndex))
			{
				// Return Matching Buffer

				return pBuffer;
			}
		}
	}

	// Return NULL When Not Found

	return NULL;
}

// ***********************************************************************************************************************

CPrefetchBuffer* CPrefetchList::FindFromMap(int Param_iArrayMapIndex)
{
	// Error Checking

	if ((Param_iArrayMapIndex < 0) || (Param_iArrayMapIndex > SND_Cte_StreamPrefetchMax))
	{
		return NULL;
	}

	// Fetch Buffer At Index

	CPrefetchBuffer* pBuffer = (CPrefetchBuffer*)GetItem(m_aArrayMap[Param_iArrayMapIndex].iCachedListIndex);

	// Update Cached Buffer Index (If Applicable)

	if ((pBuffer == NULL) || (pBuffer->GetPosition() != m_aArrayMap[Param_iArrayMapIndex].uiFilePosition) || (pBuffer->GetSoundBFIndex() != m_aArrayMap[Param_iArrayMapIndex].eSoundBFIndex))
	{
		// Find New Prefetch Buffer

		CPrefetchBuffer* pBuffer_New = Find(m_aArrayMap[Param_iArrayMapIndex].uiFilePosition, m_aArrayMap[Param_iArrayMapIndex].eSoundBFIndex);

		if (pBuffer_New == NULL)
		{
			return NULL;
		}

		// Cache List Index

		m_aArrayMap[Param_iArrayMapIndex].iCachedListIndex = pBuffer_New->GetListIndex();

		pBuffer = pBuffer_New;
	}

	return pBuffer;
}

// ***********************************************************************************************************************

CPrefetchBuffer* CPrefetchList::Request(unsigned int Param_uiPosition, unsigned int Param_uiSize, eSoundBFIndex Param_eSoundBFIndex, int Param_iArrayMapIndex)
{
	// Scan For Existing Buffer

	CPrefetchBuffer* pBuffer = Find(Param_uiPosition, Param_eSoundBFIndex);

	// Create New If Not Found

	if (pBuffer == NULL)
	{
		// Setup New Buffer

		pBuffer = new CPrefetchBuffer();

		pBuffer->Setup(Param_uiPosition, Param_uiSize, Param_eSoundBFIndex);

		// Add To List

		AddItem((CListItem*)pBuffer);
	}

	// Store To Array Map (If Applicable)

	if ((Param_iArrayMapIndex >= 0) && (Param_iArrayMapIndex < SND_Cte_StreamPrefetchMax))
	{
		m_aArrayMap[Param_iArrayMapIndex].uiFilePosition	= Param_uiPosition;
		m_aArrayMap[Param_iArrayMapIndex].eSoundBFIndex		= Param_eSoundBFIndex;
		m_aArrayMap[Param_iArrayMapIndex].iCachedListIndex	= pBuffer->GetListIndex();
	}
	
	return pBuffer;
}

// ***********************************************************************************************************************

void CPrefetchList::Discard(CPrefetchBuffer* Param_pBuffer)
{
	RemoveItem(Param_pBuffer->GetListIndex());
}

// ***********************************************************************************************************************

#endif // _XENON
