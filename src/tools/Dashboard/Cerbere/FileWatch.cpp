// FileWatch.cpp: implementation of the CFileWatch class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileWatch.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#define ACTIVATE_TRACE  // uncomment to activates trace
#ifdef ACTIVATE_TRACE
    #define MY_TRACE TRACE
#else
    #define MY_TRACE __noop
#endif




//////////////////////////////////////////////////////////////////////////
bool GetLastWriteTime(LPCTSTR lpszFileName, __int64 &ftLastWriteTime)
{
	CFile file;
	if (file.Open(lpszFileName, CFile::shareDenyNone))
	{
		BY_HANDLE_FILE_INFORMATION info;
		if (GetFileInformationByHandle((HANDLE)file.m_hFile, &info))
		{
			ftLastWriteTime = (__int64(info.ftLastWriteTime.dwHighDateTime)<<32) + info.ftLastWriteTime.dwLowDateTime;
			return true;
		}
	}
	MY_TRACE("GetLastWriteTime failed for %s\n", lpszFileName);
	return false;
}

CString GetFileFolder(LPCTSTR lpszFileName)
{
	TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR];
	_tsplitpath(lpszFileName, szDrive, szDir, NULL, NULL);
	return CString(szDrive) + CString(szDir);
}

bool IsFolder(LPCTSTR lpszFileName)
{
	return lpszFileName[strlen(lpszFileName)-1] == '\\';
}

template <class T> T maximum(const T& x, const T& y)
{
	return x>y ? x:y;
}















/////////////////////////////////////////////////////////////////////////////
// CFileWatch

CMap<DWORD, const DWORD&, FILEWATCHITEM, const FILEWATCHITEM&>		CFileWatch::m_FileMap;
CArrayEx<CString, const CString&>									CFileWatch::m_arFolder;
CArrayEx<HANDLE, const HANDLE&>										CFileWatch::m_arNotifyHandle;

CCriticalSection	CFileWatch::m_csDataLock;
CEvent				CFileWatch::m_EventUpdate;
DWORD				CFileWatch::m_dwNextHandle			= 1;
bool				CFileWatch::m_bStopWatch			= false;
UINT				CFileWatch::m_msgFileWatchNotify	= 0;
UINT				CFileWatch::m_msgFileWatchDelete	= 0;
CWinThread*			CFileWatch::m_pThread				= NULL;


void CFileWatch::StartThread()
{
	if (m_pThread == NULL)
	{
		m_bStopWatch = false;
		m_EventUpdate.SetEvent();
		m_pThread = AfxBeginThread(Watch, NULL, THREAD_PRIORITY_NORMAL);
		MY_TRACE("FileWatch thread started\n");
	}
}
void CFileWatch::Stop()
{
	if (m_pThread)
	{
		m_csDataLock.Lock();
		m_bStopWatch = true;
		m_EventUpdate.SetEvent();
		m_csDataLock.Unlock();

		while (m_pThread)
			Sleep(10);
	}
}

DWORD CFileWatch::AddFileFolder(LPCTSTR lpszFileName, HWND hWnd)
{
	m_csDataLock.Lock();

	DWORD dwHandle = 0;
	CString sFolder = GetFileFolder(lpszFileName);
	__int64 ftLastWriteTime=0;
	if (IsFolder(lpszFileName) || GetLastWriteTime(lpszFileName, ftLastWriteTime))
	{

		if (m_msgFileWatchNotify == 0)
        {	m_msgFileWatchNotify = ::RegisterWindowMessage("FileWatchNotify");
        }
        if (m_msgFileWatchDelete == 0)
        {	m_msgFileWatchDelete = ::RegisterWindowMessage("FileWatchDelete");
        }

		FILEWATCHITEM fwItem;
		fwItem.sFileName		= lpszFileName;
		fwItem.ftLastWriteTime	= ftLastWriteTime;
		fwItem.hWnd				= hWnd;

		dwHandle = m_dwNextHandle++;
		m_FileMap.SetAt(dwHandle, fwItem);

		// new directory to watch?
		if (m_arFolder.Find(sFolder) == -1)
		{
			m_EventUpdate.SetEvent();
			HANDLE hChangeNotification = FindFirstChangeNotification(sFolder, FALSE, FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_LAST_WRITE);
			ASSERT(hChangeNotification != INVALID_HANDLE_VALUE);
			m_arFolder.Add(sFolder);
			m_arNotifyHandle.Add(hChangeNotification);
		}

		StartThread();
	}

	m_csDataLock.Unlock();
	return dwHandle;
}

void CFileWatch::SetData(DWORD dwHandle, DWORD dwData)
{
	m_csDataLock.Lock();
	FILEWATCHITEM fwItem;
	if (m_FileMap.Lookup(dwHandle, fwItem))
	{
		fwItem.dwData = dwData;
		m_FileMap.SetAt(dwHandle, fwItem);
	}
	m_csDataLock.Unlock();
}


void CFileWatch::RemoveHandle(DWORD dwHandle)
{
	m_csDataLock.Lock();

	FILEWATCHITEM fwItem;
	if (m_FileMap.Lookup(dwHandle, fwItem))
	{

		m_FileMap.RemoveKey(dwHandle);
		CString sFolder = GetFileFolder(fwItem.sFileName);
		bool bDeleteDir = true;
		POSITION pos = m_FileMap.GetStartPosition();
		while (pos)
		{
			DWORD dwHandle;
			FILEWATCHITEM fwItem;
			m_FileMap.GetNextAssoc(pos, dwHandle, fwItem);
			if (GetFileFolder(fwItem.sFileName) == sFolder)
			{
				bDeleteDir = false;
				break;
			}
		}

		if (bDeleteDir)
		{
			m_EventUpdate.SetEvent();
			int i = m_arFolder.Find(sFolder);
			FindCloseChangeNotification(m_arNotifyHandle[i]);
			m_arFolder.RemoveAt(i);
			m_arNotifyHandle.RemoveAt(i);
		}

	}

	m_csDataLock.Unlock();
	if (m_FileMap.IsEmpty())
		Stop();
}


UINT CFileWatch::Watch(LPVOID)
{
	CArray<HANDLE, const HANDLE&> arHandle;
	arHandle.Add(m_EventUpdate);

	for (;;)
	{
		// wait for event or notification
		DWORD dwResult = WaitForMultipleObjects( (DWORD)arHandle.GetSize(), arHandle.GetData(), FALSE, INFINITE);
		m_csDataLock.Lock();
		MY_TRACE("Notification\n");

		if (m_bStopWatch)
			break;

		int nObject = dwResult - WAIT_OBJECT_0;
		if (nObject==0)
		{
			MY_TRACE("Update\n");
			m_EventUpdate.ResetEvent();

			// refresh list
			arHandle.SetSize(1);
			arHandle.Append(m_arNotifyHandle);
		}
		else if (nObject>0 && nObject<arHandle.GetSize())
		{
			POSITION pos = m_FileMap.GetStartPosition();
			MY_TRACE("Notification Directory = %s\n", m_arFolder[nObject-1]);
			while (pos)
			{
				DWORD dwHandle;
				FILEWATCHITEM fwItem;
				m_FileMap.GetNextAssoc(pos, dwHandle, fwItem);
				if (GetFileFolder(fwItem.sFileName) == m_arFolder[nObject-1])
				{
					MY_TRACE("Folder File = %s\n", fwItem.sFileName);
					__int64 ftLastWriteTime=0;
                    bool bGetTime = GetLastWriteTime(fwItem.sFileName, ftLastWriteTime);
					if ( bGetTime && (fwItem.ftLastWriteTime!=ftLastWriteTime))
					{
						MY_TRACE("Notification File = %s\n", fwItem.sFileName);
						CWnd *pWnd = CWnd::FromHandle(fwItem.hWnd);
						if (pWnd)
							pWnd->PostMessage(m_msgFileWatchNotify, WPARAM(fwItem.dwData), LPARAM(LPCTSTR(fwItem.sFileName)));

						fwItem.ftLastWriteTime = ftLastWriteTime;
						m_FileMap.SetAt(dwHandle, fwItem);
					}
                    else if(!bGetTime)
                    {   // file was deleted
                        CWnd *pWnd = CWnd::FromHandle(fwItem.hWnd);
                        if (pWnd)
                            pWnd->PostMessage(m_msgFileWatchDelete, WPARAM(fwItem.dwData), LPARAM(LPCTSTR(fwItem.sFileName)));

                        fwItem.ftLastWriteTime = 0;     // reset time, so that a notification is triggered if the file re-appears
                        m_FileMap.SetAt(dwHandle, fwItem);
                    }

				}
			}
			FindNextChangeNotification(arHandle[nObject]);
		}

		m_csDataLock.Unlock();
	}


	for (int i=0; i<m_arNotifyHandle.GetSize(); i++)
		FindCloseChangeNotification(m_arNotifyHandle[i]);
	m_FileMap.RemoveAll();
	m_arFolder.RemoveAll();
	m_arNotifyHandle.RemoveAll();

	m_csDataLock.Unlock();


	m_pThread = NULL;
	return 0;
}
