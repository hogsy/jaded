#ifndef __CERBERE_H__
#define __CERBERE_H__



#include <assert.h>
#include "PCwatch.h"





/////////////////////////////////////////
class cl_Cerbere
{
protected:

public:
    cl_Cerbere();
    ~cl_Cerbere();

    void                Init(HWND _hWnd);
    void                DeInit();

    void                RunOneFrame();

    void                OnFileWatchNotification ( LPCTSTR _pzFile, DWORD _uiParam );
    void                OnFileWatchDelete       ( LPCTSTR _pzFile, DWORD _uiParam );

    void                ReadInfoFromCommonIniFile();
    void                SaveInfoToUserIniFile();
    void                ReadInfoFromUserIniFile();

    UINT                uiGetNbPC()                         {   return (UINT)mo_PCWatched.size();   }
    const cl_PCwatch&   roGetPC(UINT _uiIndex)              {   assert(_uiIndex<uiGetNbPC()); return mo_PCWatched[_uiIndex];      }
    
    DWORD               ulGetGlobalStatus()                 {   return m_ulGlobalStatus; }
    const char*         szGlobalStatus() const;


    // utils
    static DWORD ulEncodeHighLow    (DWORD _ulHigh, DWORD _ulLow)   {   assert( _ulHigh < 0x10000 );  assert( _ulLow < 0x10000 ); return ( (_ulHigh << 16) | _ulLow);    }
    static DWORD ulGetHigh          (DWORD _ulData)                 {   return ( _ulData >> 16);        }
    static DWORD ulGetLow           (DWORD _ulData)                 {   return ( _ulData & 0xffff );    }


protected:
    void                AddPCtoWatch(const char* _pzPCName);
    void                WatchPC(DWORD _ulIndex);


private:
    HWND                m_hWnd;         // handle of the main window

    DWORD               m_ulGlobalStatus;
    PCvector            mo_PCWatched;
};






#endif//__CERBERE_H__