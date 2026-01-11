// ************************************************
// Simplified Thread Event Manager
//
// Used for King Kong Xenon Multi-Core Optimization
// By Alexandre David (February 2005)
// ************************************************

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "Precomp.h"

#include "EventThread.h"

// ***********************************************************************************************************************

namespace EventThread
{

// ***********************************************************************************************************************
//    Class Methods : CEventQueue
// ***********************************************************************************************************************

CEventQueue::CEventQueue()
{
	m_pQueue			= NULL;
	m_iQueue_BufferSize	= 1;
	m_iQueue_Start		= 0;
	m_iQueue_End		= 0;
	m_pfnCallback		= NULL;
	m_eEventStatus		= eEventStatus_None;

	// Create Completion Event Handle

	m_hEventComplete	= CreateEvent(NULL, FALSE, FALSE, NULL);

	Resize();
}

// ***********************************************************************************************************************

CEventQueue::~CEventQueue()
{
	// Free Queue

	if (m_pQueue != NULL)
	{
		Memory_Free(m_pQueue);
	}

	// Close Completion Event Handle

	if (m_hEventComplete != NULL)
	{
		CloseHandle(m_hEventComplete);
	}

}

// ***********************************************************************************************************************

inline BOOL CEventQueue::IsEmpty()
{
	return (m_iQueue_Start == m_iQueue_End);
}

// ***********************************************************************************************************************

inline BOOL CEventQueue::IsFull()
{
	return ((m_iQueue_End - m_iQueue_Start) == m_iQueue_BufferSize);
}

// ***********************************************************************************************************************

void CEventQueue::EnQueue(TEventParameter Param_pParameter)
{
	// Error Checking

	if (m_pQueue == NULL)
	{
		return;
	}

	// Check If There's Room

	if (m_iQueue_End >= m_iQueue_BufferSize)
	{
		if (IsFull())
		{
			Resize();
		}
		else
		{
			Pack();
		}
	}

	// Store Value

	m_pQueue[m_iQueue_End].pParameter	= Param_pParameter;
	m_pQueue[m_iQueue_End].hFile		= NULL;

	// Grow Queue From End

	m_iQueue_End++;
}

// ***********************************************************************************************************************

void CEventQueue::EnQueue(HANDLE Param_hFile, void* Param_pTargetBuffer, int Param_iNbBytesToRead, int* Param_pNbBytesRead, unsigned int Param_uiFileOffset, TEventParameter Param_pParameter)
{
	// Standard Parameter EnQueue

	EnQueue(Param_pParameter);

	// Add FileRead Settings To Entry

	m_pQueue[m_iQueue_End - 1].hFile			= Param_hFile;
	m_pQueue[m_iQueue_End - 1].pTargetBuffer	= Param_pTargetBuffer;
	m_pQueue[m_iQueue_End - 1].iNbBytesToRead	= Param_iNbBytesToRead;
	m_pQueue[m_iQueue_End - 1].pNbBytesRead		= Param_pNbBytesRead;
	m_pQueue[m_iQueue_End - 1].uiFileOffset		= Param_uiFileOffset;
}

// ***********************************************************************************************************************

stEventSettings* CEventQueue::DeQueue()
{
	// Error Checking

	if (IsEmpty() || (m_pQueue == NULL))
	{
		return NULL;
	}

	// Fetch Return Value

	stEventSettings* ReturnValue = &m_pQueue[m_iQueue_Start];

	// Shrink Queue From Start

	m_iQueue_Start++;

	// Pack (If Empty)

	if (IsEmpty())
	{
		Pack();
	}

	return ReturnValue;
}

// ***********************************************************************************************************************

BOOL CEventQueue::Pack()
{
	// Error Checking

	if ((m_iQueue_Start == 0) || (m_pQueue == NULL))
	{
		return FALSE;
	}

	// Move Queue Start Back To Zero

	for (int Loop = m_iQueue_Start; Loop < m_iQueue_End; Loop++)
	{
		m_pQueue[Loop - m_iQueue_Start] = m_pQueue[Loop];
	}

	// Update Start / End Fields

	m_iQueue_End	-= m_iQueue_Start;
	m_iQueue_Start	= 0;

	return TRUE;
}

// ***********************************************************************************************************************

void CEventQueue::Resize()
{
	m_iQueue_BufferSize *= 2;

	// Allocate New Queue

	stEventSettings* pNewQueue = (stEventSettings*)Memory_Allocate(m_iQueue_BufferSize * sizeof(stEventSettings));

	if (m_pQueue != NULL)
	{
		// Copy Old Queue To New

		for (int Loop = m_iQueue_Start; Loop < m_iQueue_End; Loop++)
		{
			pNewQueue[Loop - m_iQueue_Start] = m_pQueue[Loop];
		}

		// Update Start / End Fields

		m_iQueue_End	-= m_iQueue_Start;
		m_iQueue_Start	= 0;

		// Free Old Queue

		Memory_Free(m_pQueue);
	}

	// Assign New Queue

	m_pQueue = pNewQueue;
}

// ***********************************************************************************************************************

inline HANDLE CEventQueue::GetCompletionEvent()
{
	return m_hEventComplete;
}

// ***********************************************************************************************************************

inline void CEventQueue::SetCallback(fnCallback Param_pfnCallback)
{
	m_pfnCallback = Param_pfnCallback;
}

// ***********************************************************************************************************************

inline fnCallback CEventQueue::GetCallback()
{
	return m_pfnCallback;
}

// ***********************************************************************************************************************

inline void CEventQueue::SetEventStatus(eEventStatus Param_eEventStatus)
{
	m_eEventStatus = Param_eEventStatus;
}

// ***********************************************************************************************************************

inline eEventStatus CEventQueue::GetEventStatus()
{
	return m_eEventStatus;
}

// ***********************************************************************************************************************

inline void CEventQueue::SetCurrentParameter(TEventParameter Param_pParameter)
{
	m_pCurrentParameter = Param_pParameter;
}

// ***********************************************************************************************************************

inline TEventParameter CEventQueue::GetCurrentParameter()
{
	return m_pCurrentParameter;
}

// ***********************************************************************************************************************

inline BOOL CEventQueue::IsReadyForImmediate()
{
	return ((m_eEventStatus != eEventStatus_Requested) && (m_eEventStatus != eEventStatus_Processing));
}

// ***********************************************************************************************************************
//    Class Methods : CEventThread
// ***********************************************************************************************************************

CEventThread::CEventThread(int Param_iEventCount, int Param_iStackSizeKB, eThreadID Param_eThreadID)
{
	// Basic Initialization

	m_pEventHandles	= NULL;
	m_ppEventQueues	= NULL;
	m_iEventCount	= 0;
	m_hThread		= NULL;
	m_eThreadStatus	= eThreadStatus_Idle;
	m_bIsBusy		= FALSE;

	// Parameter Check

	if ((Param_iEventCount <= 0) || (Param_iStackSizeKB <= 0))
	{
		return;
	}

	if (Param_iEventCount >= MAXIMUM_WAIT_OBJECTS)
	{
		return;
	}

	// Create Event Arrays
	// - An extra event is allocated for thread shutdown

	m_pEventHandles	= (HANDLE*)Memory_Allocate((Param_iEventCount + 1) * sizeof(HANDLE));
	m_ppEventQueues	= (CEventQueue**)Memory_Allocate((Param_iEventCount + 1) * sizeof(CEventQueue*));

	if ((m_pEventHandles == NULL) || (m_ppEventQueues == NULL))
	{
		return;
	}

	m_iEventCount = Param_iEventCount + 1;

	// Populate Event Array

	for (TEventID Loop = 0; Loop < m_iEventCount; Loop++)
	{
		m_pEventHandles[Loop] = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_ppEventQueues[Loop] = new CEventQueue();
	}

	// Create Thread

	m_hThread = CreateThread(NULL, Param_iStackSizeKB * 1024, (LPTHREAD_START_ROUTINE)GlobalMainLoop, this, 0, (DWORD*)&m_iThreadID);

	if (m_hThread == NULL)
	{
		return;
	}

	// Set Thread Name

	SetThreadName(kastThreadConfiguration[Param_eThreadID].pName);

	// Set Thread Priority

	SetThreadPriority(m_hThread, kastThreadConfiguration[Param_eThreadID].iPriority);

#if defined(_XENON)
	XSetProcessQuantumLength(10);
#endif

	// Set Executing Processor

#if defined(_XENON)
	XSetThreadProcessor(m_hThread, kastThreadConfiguration[Param_eThreadID].iProcessorCoreID);
#endif

	// Enable Thread Processing

	m_eThreadStatus = eThreadStatus_Running;
}

// ***********************************************************************************************************************

CEventThread::~CEventThread()
{
	// Terminate Thread Loop
	// Last event in list was specially created for thread shutdown

	TEventID iEventID_Shutdown = m_iEventCount - 1;

	m_eThreadStatus = eThreadStatus_Stopping;

	RequestEvent(iEventID_Shutdown);

	// Wait For Thread To Exit

	WaitForSingleObject(m_ppEventQueues[iEventID_Shutdown]->GetCompletionEvent(), INFINITE);

	// Clear Event Handles (If Applicable)

	if (m_pEventHandles != NULL)
	{
		for (TEventID Loop = 0; Loop < m_iEventCount; Loop++)
		{
			if (m_pEventHandles[Loop] != NULL)
			{
				CloseHandle(m_pEventHandles[Loop]);
			}
		}

		Memory_Free(m_pEventHandles);
	}

	// Clear Event List (If Applicable)

	if (m_ppEventQueues != NULL)
	{
		for (TEventID Loop = 0; Loop < m_iEventCount; Loop++)
		{
			if (m_ppEventQueues[Loop] != NULL)
			{
				delete m_ppEventQueues[Loop];
			}
		}

		Memory_Free(m_ppEventQueues);
	}
}

// ***********************************************************************************************************************

void CEventThread::SetThreadName(char* Param_pName)
{
#if _XENON_THREAD_ALLOWSETNAME
	// This awful hack is actually the Microsoft-recommended way to go

	struct
	{
		int		iType;		// Must be 0x1000
		char*	pName;		// Pointer to name (in user address space)
		int		iThreadID;	// Thread ID (-1 for caller thread)
		int		iFlags;		// Reserved for future use (must be zero)
	}
	stThreadNameInfo =
	{
		0x1000,
		Param_pName,
		m_iThreadID,
		0
	};

	__try
	{
		RaiseException(0x406D1388, 0, sizeof(stThreadNameInfo) / sizeof(DWORD), (DWORD*)&stThreadNameInfo);
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
#endif
}

// ***********************************************************************************************************************

BOOL CEventThread::SetEventCallback(TEventID Param_iEventID, fnCallback Param_pfnCallback)
{
	// Error Checking

	if ((m_ppEventQueues == NULL) || (Param_iEventID < 0) || (Param_iEventID >= m_iEventCount))
	{
		return FALSE;
	}

	// Update Event Callback Pointer

	m_ppEventQueues[Param_iEventID]->SetCallback(Param_pfnCallback);

	return TRUE;
}

// ***********************************************************************************************************************

BOOL CEventThread::RequestEvent(TEventID Param_iEventID, TEventParameter Param_pParameter, BOOL Param_bIsImmediate)
{
	// Error Checking

	if ((m_ppEventQueues == NULL) || (m_pEventHandles == NULL) || (Param_iEventID < 0) || (Param_iEventID >= m_iEventCount))
	{
		return FALSE;
	}

	// Request / EnQueue Event

	if (Param_bIsImmediate && m_ppEventQueues[Param_iEventID]->IsReadyForImmediate())
	{
		// Immediate

		// - Udpate Status

		m_ppEventQueues[Param_iEventID]->SetEventStatus(eEventStatus_Requested);

		// - Update Parameter Pointer

		m_ppEventQueues[Param_iEventID]->SetCurrentParameter(Param_pParameter);

		// - Signal Event To Wake-up Thread

		SetEvent(m_pEventHandles[Param_iEventID]);
	}
	else
	{
		// Delayed

		// - Add Event To Queue

		m_ppEventQueues[Param_iEventID]->EnQueue(Param_pParameter);
	}

	return TRUE;
}

// ***********************************************************************************************************************

inline BOOL CEventThread::IsBusy()
{
	return m_bIsBusy;
}

// ***********************************************************************************************************************

BOOL CEventThread::Pause()
{
	if (m_eThreadStatus == eThreadStatus_Running)
	{
		m_eThreadStatus = eThreadStatus_Idle;
		return TRUE;
	}

	return FALSE;
}

// ***********************************************************************************************************************

BOOL CEventThread::Resume()
{
	if (m_eThreadStatus == eThreadStatus_Idle)
	{
		m_eThreadStatus = eThreadStatus_Running;
		return TRUE;
	}

	return FALSE;
}

// ***********************************************************************************************************************

inline eThreadStatus CEventThread::GetThreadStatus()
{
	return m_eThreadStatus;
}

// ***********************************************************************************************************************

eEventStatus CEventThread::GetEventStatus(TEventID Param_iEventID)
{
	// Error Checking

	if ((m_ppEventQueues == NULL) || (Param_iEventID < 0) || (Param_iEventID >= m_iEventCount))
	{
		return eEventStatus_None;
	}

	// Fetch Event Status

	return m_ppEventQueues[Param_iEventID]->GetEventStatus();
}

// ***********************************************************************************************************************

BOOL CEventThread::IsEventRequested(TEventID Param_iEventID)
{
	// Error Checking

	if ((m_ppEventQueues == NULL) || (Param_iEventID < 0) || (Param_iEventID >= m_iEventCount))
	{
		return FALSE;
	}

	// Check Event Status

	return (m_ppEventQueues[Param_iEventID]->GetEventStatus() == eEventStatus_Requested);
}

// ***********************************************************************************************************************

BOOL CEventThread::IsEventProcessing(TEventID Param_iEventID)
{
	// Error Checking

	if ((m_ppEventQueues == NULL) || (Param_iEventID < 0) || (Param_iEventID >= m_iEventCount))
	{
		return FALSE;
	}

	// Check Event Status

	return (m_ppEventQueues[Param_iEventID]->GetEventStatus() == eEventStatus_Processing);
}

// ***********************************************************************************************************************

BOOL CEventThread::IsEventComplete(TEventID Param_iEventID)
{
	// Error Checking

	if ((m_ppEventQueues == NULL) || (Param_iEventID < 0) || (Param_iEventID >= m_iEventCount))
	{
		return FALSE;
	}

	// Check Event Status

	return (m_ppEventQueues[Param_iEventID]->GetEventStatus() == eEventStatus_Complete);
}

// ***********************************************************************************************************************

int CEventThread::WaitForEventComplete(TEventID Param_iEventID)
{
	// Error Checking

	if ((m_ppEventQueues == NULL) || (Param_iEventID < 0) || (Param_iEventID >= m_iEventCount))
	{
		return -1;
	}

	// Check Thread Status

	if (m_eThreadStatus != eThreadStatus_Running)
	{
		return -1;
	}

	// Check Event Status

	if (m_ppEventQueues[Param_iEventID]->GetEventStatus() == eEventStatus_None)
	{
		return -1;
	}

	// Fetch Time (Before Wait)

	unsigned int uiTime = GetTickCount();

	// Loop Until Complete

	while (m_ppEventQueues[Param_iEventID]->GetEventStatus() != eEventStatus_Complete)
	{
		WaitForSingleObject(m_ppEventQueues[Param_iEventID]->GetCompletionEvent(), kEventComplete_WaitTimeOutMS);
	}

	// Return Time Delta

	return (GetTickCount() - uiTime);
}

// ***********************************************************************************************************************

BOOL CEventThread::FileRead(TEventID Param_iEventID, HANDLE Param_hFile, void* Param_pTargetBuffer, int Param_iNbBytesToRead, int* Param_pNbBytesRead, unsigned int Param_uiFileOffset, TEventParameter Param_pParameter, BOOL Param_bIsImmediate)
{
	// Error Checking

	if ((m_ppEventQueues == NULL) || (m_pEventHandles == NULL) || (Param_iEventID < 0) || (Param_iEventID >= m_iEventCount))
	{
		return FALSE;
	}

	// Request / EnQueue Event

	if (Param_bIsImmediate && m_ppEventQueues[Param_iEventID]->IsReadyForImmediate())
	{
		// Immediate

		return PerformFileRead(Param_iEventID, Param_hFile, Param_pTargetBuffer, Param_iNbBytesToRead, Param_pNbBytesRead, Param_uiFileOffset, Param_pParameter);
	}
	else
	{
		// Delayed

		// - Add Event To Queue

		m_ppEventQueues[Param_iEventID]->EnQueue(Param_hFile, Param_pTargetBuffer, Param_iNbBytesToRead, Param_pNbBytesRead, Param_uiFileOffset, Param_pParameter);

		return TRUE;
	}
}

// ***********************************************************************************************************************

BOOL CEventThread::ExecuteCallback(TEventID Param_iEventID, TEventParameter Param_pParameter)
{
	// Error Checking

	if ((m_ppEventQueues == NULL) || (Param_iEventID < 0) || (Param_iEventID >= m_iEventCount))
	{
		return FALSE;
	}

	if (m_ppEventQueues[Param_iEventID]->GetCallback() == NULL)
	{
		return FALSE;
	}

	// Execute Callback

	m_ppEventQueues[Param_iEventID]->GetCallback()(Param_iEventID, Param_pParameter);

	return TRUE;
}

// ***********************************************************************************************************************

BOOL CEventThread::PerformEvent(TEventID Param_iEventID, TEventParameter Param_pParameter)
{
	// Error Checking

	if ((m_ppEventQueues == NULL) || (Param_iEventID < 0) || (Param_iEventID >= m_iEventCount))
	{
		return FALSE;
	}

	// Update Status To "Processing"

	m_bIsBusy = TRUE;

	m_ppEventQueues[Param_iEventID]->SetEventStatus(eEventStatus_Processing);

	// Execute Callback

	if (m_eThreadStatus == eThreadStatus_Running)
	{
		ExecuteCallback(Param_iEventID, Param_pParameter);
	}

	// Update Status To "Complete"

	m_ppEventQueues[Param_iEventID]->SetEventStatus(eEventStatus_Complete);

	// Trigger Callback Completion Event

	SetEvent(m_ppEventQueues[Param_iEventID]->GetCompletionEvent());

	m_bIsBusy = FALSE;

	return TRUE;
}

// ***********************************************************************************************************************

BOOL CEventThread::PerformFileRead(TEventID Param_iEventID, HANDLE Param_hFile, void* Param_pTargetBuffer, int Param_iNbBytesToRead, int* Param_pNbBytesRead, unsigned int Param_uiFileOffset, TEventParameter Param_pParameter)
{
	// Error Checking

	if ((m_ppEventQueues == NULL) || (Param_pTargetBuffer == NULL) || (Param_hFile == NULL))
	{
		return FALSE;
	}

	if ((Param_iNbBytesToRead <= 0) || (Param_iEventID < 0) || (Param_iEventID >= m_iEventCount))
	{
		return FALSE;
	}

	// Setup Asynchronous Read Parameters

	OVERLAPPED SOverlapped;

	Memory_Fill(&SOverlapped, sizeof(SOverlapped));

	SOverlapped.hEvent = m_pEventHandles[Param_iEventID];
	SOverlapped.Offset = Param_uiFileOffset;

	// Udpate Status

	m_ppEventQueues[Param_iEventID]->SetEventStatus(eEventStatus_Requested);

	// Update Parameter Pointer

	m_ppEventQueues[Param_iEventID]->SetCurrentParameter(Param_pParameter);

	// Start Asynchronous File Read

	BOOL bResult = ReadFile(Param_hFile, Param_pTargetBuffer, Param_iNbBytesToRead, (LPDWORD)Param_pNbBytesRead, &SOverlapped);

	// Check Return Code

	if (!bResult)
	{
		// If operation is pending, assume success nonetheless

		int iError = GetLastError();

		if (iError == ERROR_IO_PENDING)
		{
			bResult = TRUE;
		}
	}

	return bResult;
}

// ***********************************************************************************************************************

eReturnCode CEventThread::MainLoop()
{
	// Error Checking

	if (m_ppEventQueues == NULL)
	{
		return eReturnCode_NoEvents;
	}

	// Master Wait Loop

	while (m_eThreadStatus <= eThreadStatus_Running)
	{
		// Process Queued Events

		TEventID Loop = 0;

		while (Loop < m_iEventCount)
		{
			// Fetch Event Queue

			CEventQueue* pEventQueue = m_ppEventQueues[Loop];

			// Check For Any Queued Events

			if (!pEventQueue->IsEmpty() && pEventQueue->IsReadyForImmediate())
			{
				stEventSettings* Settings = pEventQueue->DeQueue();

				if (Settings != NULL)
				{
					if (Settings->hFile == NULL)
					{
						// Perform Event Processing

						PerformEvent(Loop, Settings->pParameter);
					}
					else
					{
						// Perform File Read

						PerformFileRead(Loop, Settings->hFile, Settings->pTargetBuffer, Settings->iNbBytesToRead, Settings->pNbBytesRead, Settings->uiFileOffset, Settings->pParameter);
					}
				}
			}

			Loop++;
		}

		// Asynchronous wait (will not return until an event is signaled or "TimeOut" delay has elapsed)

		TEventID iEventID = WaitForMultipleObjects(m_iEventCount, m_pEventHandles, FALSE, kMainLoop_WaitTimeOutMS);

		// Handle Signaled Event (If Applicable)

		if ((iEventID != WAIT_FAILED) && (iEventID != WAIT_TIMEOUT))
		{
			// Fetch Real Event Index

			iEventID -= WAIT_OBJECT_0;

			// Perform Event Processing

			PerformEvent(iEventID, m_ppEventQueues[iEventID]->GetCurrentParameter());
		}
	}

	// Update Thread Status To "Stopped"

	m_eThreadStatus = eThreadStatus_Stopped;

	// Trigger Shutdown Completion Event

	TEventID iEventID_Shutdown = m_iEventCount - 1;

	SetEvent(m_ppEventQueues[iEventID_Shutdown]->GetCompletionEvent());

	// Successfully Exit Thread

	return eReturnCode_OK;
}

// ***********************************************************************************************************************
//    Functions
// ***********************************************************************************************************************

eReturnCode CALLBACK GlobalMainLoop(CEventThread* Param_pThreadProcessClass)
{
	// Error Checking

	if (Param_pThreadProcessClass == NULL)
	{
		return eReturnCode_InvalidClass;
	}

	return Param_pThreadProcessClass->MainLoop();
}

// ***********************************************************************************************************************

} // Namespace

// ***********************************************************************************************************************
