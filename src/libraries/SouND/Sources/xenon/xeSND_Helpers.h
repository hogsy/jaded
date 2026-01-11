#ifndef __xeSND_Helpers_h__
#define __xeSND_Helpers_h__

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDstream.h"

#include "BASe/MEMory/MEM.h"

// ***********************************************************************************************************************
//    Constants
// ***********************************************************************************************************************

#define kiStreamPacketCount 3

const int kiStreamBufferSizeKB		= 128;
const int kiLoadingCounterTimeout	= 32;

// ***********************************************************************************************************************
//    Enums
// ***********************************************************************************************************************

typedef enum
{
	eMemoryTracker_General,
	eMemoryTracker_Buffer_Static,
	eMemoryTracker_Buffer_Stream,

	eMemoryTracker_Count
}
eMemoryTrackers;

// ***********************************************************************************************************************

typedef enum
{
	ePrefetchStatus_Empty,
	ePrefetchStatus_Setup,
	ePrefetchStatus_Loading,
	ePrefetchStatus_Useable,

	ePrefetchStatus_Count
}
ePrefetchStatus;

// ***********************************************************************************************************************

typedef enum
{
	eSoundBF_None = -1,
	eSoundBF_Extra,
	eSoundBF_Localized,
	eSoundBF_Common,

	eSoundBF_Count
}
eSoundBFIndex;

// ***********************************************************************************************************************
//    Structs
// ***********************************************************************************************************************

typedef struct
{
	int iAllocatedSize;
	int iBlockCount;
}
stMemoryTracker;

// ***********************************************************************************************************************

typedef struct
{
	int iFile;
	int iBuffer;
}
stPacketTracker_Positions;

// ***********************************************************************************************************************

typedef struct
{
	unsigned int	uiFilePosition;
	eSoundBFIndex	eSoundBFIndex;
	int				iCachedListIndex;
}
stPrefetchArrayMap;

// ***********************************************************************************************************************
//    Classes
// ***********************************************************************************************************************

class CFile
{
private:
	HANDLE	m_hHandle_Sync;
	HANDLE	m_hHandle_Async;
	BOOL	m_bIsOpen;
	BOOL	m_bIsReadWrite;

public:
    M_DeclareOperatorNewAndDelete();

	CFile(const char* Param_pName, BOOL bIsReadWrite = FALSE);
	~CFile();

	BOOL			IsOpen();
	HANDLE			GetHandle_Sync();
	HANDLE			GetHandle_Async();
	unsigned int	GetSize();
	void			Read(char* Param_pData, unsigned int Param_uiSize);
	void			Write(char* Param_pData, unsigned int Param_uiSize);
	void			Write32(unsigned int Param_uiValue);
};

// ***********************************************************************************************************************

class CBuffer
{
private:
	char*			m_pMemory;
	int				m_iSize_Capacity;
	int				m_iSize_Data;
	int volatile	m_iReferenceCounter;
	BOOL			m_bIsPersistent;
	BOOL			m_bIsMemoryOwner;
	eMemoryTrackers	m_eMemoryTracker;

public:
    M_DeclareOperatorNewAndDelete();

	CBuffer(BOOL Param_bIsPersistent = TRUE);
	~CBuffer();

	BOOL			IsEmpty();
	BOOL			IsPersistent();
	BOOL			IsMemoryOwner();
	int				GetSize_Capacity();
	int				GetSize_Data();
	char*			GetMemoryPointer(int Param_iOffsetInBuffer = 0);
	void			SetPersistence(BOOL Param_bIsPersistent);
	eMemoryTrackers	GetTracker();
	void			SetTracker(eMemoryTrackers Param_eMemoryTracker);
	void			Acquire(CBuffer* Param_pBuffer, BOOL Param_bIsNewBufferOwner = FALSE);
	void			Acquire(char* Param_pMemory, int Param_iSize_Capacity, BOOL Param_bIsNewBufferOwner = FALSE);
	char*			RequestStorage(int Param_iSize_Capacity = 0, int Param_iOffsetInBuffer = 0);
	void			StoreData(char* Param_pSourcePointer, int Param_iSize_Data);
	void			RetrieveData(char* Param_pDestPointer, int Param_iSize = 0);
	void			Release(BOOL Param_bForceDestroy = FALSE);
};

// ***********************************************************************************************************************

class CPacketTracker
{
private:
	stPacketTracker_Positions	m_astPositions[kiStreamPacketCount];
	int	volatile				m_iCircularIndex_Loading;
	int	volatile				m_iCircularIndex_Playing;
	int volatile				m_iCounter_Loading;
	int volatile				m_iCounter_Submitted;
	int volatile				m_iCounter_LoadingTimeout;

public:
    M_DeclareOperatorNewAndDelete();

	CPacketTracker();
	~CPacketTracker();

	void	Reset();
	void	Notify_Submit(int Param_iPosition_Buffer = 0);
	void	Notify_PlayEnd();
	void	Notify_LoadStart(int Param_iPosition_File = 0);
	void	Notify_LoadEnd(BOOL Param_bPacketDropped = FALSE);
	int		GetCurrentPosition_File();
	int		GetCurrentPosition_Buffer();
	int		GetSubmitCounter();
	int		GetLoadingCounter();
	BOOL	GetLoadingStateWithTimeout();
};

// ***********************************************************************************************************************

class CStreamReader
{
private:
	unsigned char*	m_pBuffer;
	HANDLE			m_hFile;
	unsigned int	m_uiBufferSize;
	unsigned int	m_uiOffset_StartInFile;
	int				m_iOffset_BufferInData;
	unsigned int	m_uiOffset_CurrentInBuffer;
	unsigned int	m_uiOffset_LastLoadedBufferInFile;
	BOOL			m_bDirtyBuffer;
	BOOL			m_bIsOpen;
	BOOL			m_bIsBigEndian;

	BOOL	RefreshBuffer(BOOL Param_bSequential);

public:
    M_DeclareOperatorNewAndDelete();

	CStreamReader(unsigned int Param_uiBufferSizeKB);
	~CStreamReader();

	BOOL			Open(HANDLE Param_hFile, unsigned int Param_uiStartOffsetInFile = 0, BOOL Param_bIsBigEndian = FALSE);
	BOOL			Close();
	BOOL			SetDataPositionInFile(unsigned int Param_uiStartOffsetInFile);
	BOOL			Skip(unsigned int Param_uiCount = 1);
	BOOL			SeekInData(int Param_iOffsetInData = 0);
	BOOL			Read(char* Param_pBuffer, unsigned int Param_uiCount);
	unsigned int	ReadString(char* Param_pBuffer);
	unsigned char	Read8();
	unsigned short	Read16();
	unsigned int	Read32();
	unsigned int	GetCurrentOffsetInData();
	unsigned int	GetRemainingUnreadInBuffer();
	unsigned char*	GetCurrentBufferPointer();
};

// ***********************************************************************************************************************

class CListItem
{
protected:
	int		m_iListIndex;
	BOOL	m_bIsOwnedByList;

public:
    M_DeclareOperatorNewAndDelete();

	CListItem(BOOL Param_bIsOwnedByList = TRUE);
	virtual ~CListItem();

	virtual BOOL CanBeDeleted();

	int		GetListIndex();
	void	SetListIndex(int Param_iListIndex);
	BOOL	IsOwnedByList();
};

// ***********************************************************************************************************************

class CItemList
{
protected:
	CListItem**		m_pList;
	int				m_iSize_Buffer;
	int volatile	m_iSize_Items;
	BOOL			m_bFastGrowth;

	void			Grow();

public:
    M_DeclareOperatorNewAndDelete();

	CItemList(BOOL Param_bFastGrowth = TRUE);
	virtual ~CItemList();

	BOOL		IsEmpty();
	BOOL		IsFull();
	int			GetCapacity();
	int			GetItemCount();
	int			AddItem(CListItem* Param_pItem);
	CListItem*	GetItem(int Param_iListIndex);
	int			GetItemIndex(CListItem* Param_pItem);
	void		RemoveItem(int Param_iListIndex);
	void		RemoveItem(CListItem* Param_pItem);
	void		Flush();
};

// ***********************************************************************************************************************

class CVoiceList : public CItemList
{
public:
    M_DeclareOperatorNewAndDelete();

	CVoice*	RequestVoice(SND_tdst_WaveData* Param_pWaveData, int Param_iFilePositionInBF, int Param_iStreamID, CBuffer* Param_pBuffer_Playback);
	void	DestroyVoice(CVoice* Param_pVoice);
	CVoice*	GetVoice(int Param_iListIndex);
};

// ***********************************************************************************************************************

class CPrefetchBuffer : public CListItem
{
private:
	unsigned int	m_uiPosition;
	unsigned int	m_uiSize;
	unsigned int	m_uiSizeRead;
	eSoundBFIndex	m_eSoundBFIndex;
	CBuffer*		m_pBuffer;

	ePrefetchStatus volatile m_eStatus;

public:
    M_DeclareOperatorNewAndDelete();

	CPrefetchBuffer();
	virtual ~CPrefetchBuffer();

	virtual BOOL CanBeDeleted();

	ePrefetchStatus	GetStatus();
	unsigned int	GetPosition();
	eSoundBFIndex	GetSoundBFIndex();
	unsigned int	GetSize();
	void			Setup(unsigned int Param_uiPosition, unsigned int Param_uiSize, eSoundBFIndex Param_eSoundBFIndex);
	void			StoreDataToPrefetch(char* Param_pSourceBuffer);
	void			RecoverDataFromPrefetch(char* Param_pDestBuffer);
	void			StartLoad();
	void			EndLoad();
	void			Clear();
};

// ***********************************************************************************************************************

class CPrefetchList : public CItemList
{
private:
	stPrefetchArrayMap	m_aArrayMap[SND_Cte_StreamPrefetchMax];

public:
    M_DeclareOperatorNewAndDelete();

	CPrefetchList();

	CPrefetchBuffer*	Find(unsigned int Param_uiPosition, eSoundBFIndex Param_eSoundBFIndex);
	CPrefetchBuffer*	FindFromMap(int Param_iArrayMapIndex);
	CPrefetchBuffer*	Request(unsigned int Param_uiPosition, unsigned int Param_uiSize, eSoundBFIndex Param_eSoundBFIndex, int Param_iArrayMapIndex = -1);
	void				Discard(CPrefetchBuffer* Param_pBuffer);
};

// ***********************************************************************************************************************

#endif // __xeSND_Helpers_h__
