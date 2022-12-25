// ************************************************
// Simplified Thread Event Manager
//
// Used for King Kong Xenon Multi-Core Optimization
// By Alexandre David (February 2005)
// ************************************************

#ifndef __EventThread_h__
#define __EventThread_h__

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "EventThread_Native.h"

// ***********************************************************************************************************************

namespace EventThread
{

// ***********************************************************************************************************************
//    Macros
// ***********************************************************************************************************************

#if !defined(CALLBACK)
#define CALLBACK __stdcall
#endif

// ***********************************************************************************************************************
//    Constants
// ***********************************************************************************************************************

const int kMainLoop_WaitTimeOutMS		= 100;
const int kEventComplete_WaitTimeOutMS	= 100;

// ***********************************************************************************************************************
//    Enums
// ***********************************************************************************************************************

typedef enum
{
	eThreadStatus_Idle,
	eThreadStatus_Running,
	eThreadStatus_Stopping,
	eThreadStatus_Stopped
}
eThreadStatus;

// ***********************************************************************************************************************

typedef enum
{
	eEventStatus_None,
	eEventStatus_Requested,
	eEventStatus_Processing,
	eEventStatus_Complete
}
eEventStatus;

// ***********************************************************************************************************************

typedef enum
{
	eReturnCode_OK,
	eReturnCode_InvalidClass,
	eReturnCode_NoEvents
}
eReturnCode;

// ***********************************************************************************************************************
//    Custom Types
// ***********************************************************************************************************************

typedef int		TEventID;
typedef void*	TEventParameter;
typedef void	(CALLBACK* fnCallback)(TEventID Param_iEventID, TEventParameter Param_pParameter);

// ***********************************************************************************************************************
//    Structs
// ***********************************************************************************************************************

typedef struct
{
	// Callback Parameter

	TEventParameter	pParameter;

	// File Read Information

	HANDLE			hFile;
	void*			pTargetBuffer;
	int				iNbBytesToRead;
	int*			pNbBytesRead;
	unsigned int	uiFileOffset;
}
stEventSettings;

// ***********************************************************************************************************************
//    Classes
// ***********************************************************************************************************************

class CEventQueue
{
private:
	HANDLE						m_hEventComplete;
	fnCallback					m_pfnCallback;
	TEventParameter	volatile	m_pCurrentParameter;
	stEventSettings* volatile	m_pQueue;
	int volatile				m_iQueue_BufferSize;
	int volatile				m_iQueue_Start;
	int volatile				m_iQueue_End;
	eEventStatus volatile		m_eEventStatus;

public:
    M_DeclareOperatorNewAndDelete();

	CEventQueue();
	~CEventQueue();

	BOOL				IsEmpty();
	BOOL				IsFull();
	void				EnQueue(TEventParameter Param_pParameter = NULL);
	void				EnQueue(HANDLE Param_hFile, void* Param_pTargetBuffer, int Param_iNbBytesToRead, int* Param_pNbBytesRead = NULL, unsigned int Param_uiFileOffset = 0, TEventParameter Param_pParameter = NULL);
	stEventSettings*	DeQueue();
	BOOL				Pack();
	void				Resize();
	HANDLE				GetCompletionEvent();
	void				SetCallback(fnCallback Param_pfnCallback = NULL);
	fnCallback			GetCallback();
	void				SetEventStatus(eEventStatus Param_eEventStatus = eEventStatus_None);
	eEventStatus		GetEventStatus();
	void				SetCurrentParameter(TEventParameter Param_pParameter = NULL);
	TEventParameter		GetCurrentParameter();
	BOOL				IsReadyForImmediate();
};

// ***********************************************************************************************************************

class CEventThread
{
private:
	HANDLE*					m_pEventHandles;
	CEventQueue**			m_ppEventQueues;
	int						m_iEventCount;
	HANDLE					m_hThread;
	int						m_iThreadID;
	eThreadStatus volatile	m_eThreadStatus;
	BOOL volatile			m_bIsBusy;

public:
    M_DeclareOperatorNewAndDelete();

	CEventThread(int Param_iEventCount, int Param_iStackSizeKB = 16, eThreadID Param_eThreadID = eThreadID_Default);
	~CEventThread();

	void			SetThreadName(char* Param_pName);
	BOOL			SetEventCallback(TEventID Param_iEventID, fnCallback Param_pfnCallback);
	BOOL			RequestEvent(TEventID Param_iEventID, TEventParameter Param_pParameter = NULL, BOOL Param_bIsImmediate = TRUE);
	BOOL			IsBusy();
	BOOL			Pause();
	BOOL			Resume();
	eThreadStatus	GetThreadStatus();
	eEventStatus	GetEventStatus(TEventID Param_iEventID);
	BOOL			IsEventRequested(TEventID Param_iEventID);
	BOOL			IsEventProcessing(TEventID Param_iEventID);
	BOOL			IsEventComplete(TEventID Param_iEventID);
	int				WaitForEventComplete(TEventID Param_iEventID);
	BOOL			FileRead(TEventID Param_iEventID, HANDLE Param_hFile, void* Param_pTargetBuffer, int Param_iNbBytesToRead, int* Param_pNbBytesRead = NULL, unsigned int Param_uiFileOffset = 0, TEventParameter Param_pParameter = NULL, BOOL Param_bIsImmediate = TRUE);
	BOOL			ExecuteCallback(TEventID Param_iEventID, TEventParameter Param_pParameter = NULL);
	BOOL			PerformEvent(TEventID Param_iEventID, TEventParameter Param_pParameter = NULL);
	BOOL			PerformFileRead(TEventID Param_iEventID, HANDLE Param_hFile, void* Param_pTargetBuffer, int Param_iNbBytesToRead, int* Param_pNbBytesRead, unsigned int Param_uiFileOffset, TEventParameter Param_pParameter);
	eReturnCode		MainLoop();
};

// ***********************************************************************************************************************
//    Prototypes - Callbacks
// ***********************************************************************************************************************

eReturnCode CALLBACK GlobalMainLoop(CEventThread* Param_pThreadProcessClass);

// ***********************************************************************************************************************
//    Prototypes - Native
// ***********************************************************************************************************************

void*	Memory_Allocate(int Param_iSize);
void	Memory_Free(void* Param_pHeapBlock);
void	Memory_Fill(void* Param_pMemory, int Param_iSize, char Param_byValue = 0);

// ***********************************************************************************************************************

} // Namespace

using namespace EventThread;

// ***********************************************************************************************************************

#endif // __EventThread_h__
