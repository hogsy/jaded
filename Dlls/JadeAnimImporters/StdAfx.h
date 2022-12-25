#ifndef __SDTAFX__H
#define __SDTAFX__H


#pragma warning(disable : 4786) // truncation of debug symbols with STL templates

#include "Max.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "modstack.h"
#include "notify.h"
#include <utilapi.h>


extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;
extern const TCHAR* C_szVersionString;


template<class T>
bool TRead( HANDLE& hFile, T& var )
{
    DWORD read;
    if (!ReadFile( hFile, &var, sizeof(T), &read, NULL ))
        return false;
    return (read==sizeof(T));
}


template<class T>
bool TWrite( HANDLE& hFile, const T& var )
{
    DWORD written;
    if (!WriteFile( hFile, &var, sizeof(T), &written, NULL ))
        return false;
    return (written==sizeof(T));
}


#endif __SDTAFX__H
