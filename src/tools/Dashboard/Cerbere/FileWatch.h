// FileWatch.h: interface for the CFileWatch class.
//
// by Herbert Griebel
// herbertgriebel@yahoo.com
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEWATCH_H__104CC652_50A5_4354_8415_920044C93FD7__INCLUDED_)
#define AFX_FILEWATCH_H__104CC652_50A5_4354_8415_920044C93FD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Helper Class

template<class TYPE, class ARG_TYPE>
class CArrayEx : public CArray<TYPE,ARG_TYPE>
{
public:
// Operations
	int Find(ARG_TYPE searchValue, int nStartIndex=0) const;
};
template<class TYPE, class ARG_TYPE>
int CArrayEx<TYPE, ARG_TYPE>::Find(ARG_TYPE searchValue, int nStartIndex) const
{
	for (int nIndex=nStartIndex; nIndex<GetSize(); nIndex++)
	{
		if (GetAt(nIndex) == searchValue)
			return nIndex;
	}
	return -1;
}



/////////////////////////////////////////////////////////////////////////////
// CFileWatch

struct FILEWATCHITEM {
	CString		sFileName;
	__int64		ftLastWriteTime;
	HWND		hWnd;
	DWORD		dwData;
};

class CFileWatch
{
public:
	static void StartThread();
	static void Stop();
	static UINT Watch(LPVOID lpParam);

	static DWORD AddFileFolder(LPCTSTR lpszFileName, HWND hWnd);
	static void SetData(DWORD dwHandle, DWORD dwData);
	static void RemoveHandle(DWORD dwHandle);

    static UINT m_msgFileWatchNotify;
    static UINT m_msgFileWatchDelete;
    

protected:
	static CMap<DWORD, const DWORD&, FILEWATCHITEM, const FILEWATCHITEM&>	m_FileMap;
	static CArrayEx<CString, const CString&>								m_arFolder;
	static CArrayEx<HANDLE, const HANDLE&>									m_arNotifyHandle;

	static CCriticalSection	m_csDataLock;
	static CEvent			m_EventUpdate;
	static DWORD			m_dwNextHandle;
	static bool				m_bWatchClosed;
	static bool				m_bStopWatch;
	static CWinThread		*m_pThread;
};




#endif // !defined(AFX_FILEWATCH_H__104CC652_50A5_4354_8415_920044C93FD7__INCLUDED_)
