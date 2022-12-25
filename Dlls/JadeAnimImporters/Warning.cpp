#include "StdAfx.h"
#include "Warning.h"


#define C_lBufferLength 100000			// max size of buffer


CWarning* g_pWarning=NULL;


/*-----------------------------------------------------------------------------
 *  Description : allocate the buffer, and set end and current pos variable
 *-----------------------------------------------------------------------------
 *  Input :		none
 *  Output :	none
 *-----------------------------------------------------------------------------
 *  Creation date : 24/10/96            Author : V.L.
 *-----------------------------------------------------------------------------
 *  Modification date :                 Modification Author :
 *  Modifications :
 *---------------------------------------------------------------------------*/
CWarning::CWarning() :
m_bWarning( false ),
m_bError( false )
{
}


/*-----------------------------------------------------------------------------
 *  Description : deallocate the buffer
 *-----------------------------------------------------------------------------
 *  Input :		none
 *  Output :	none
 *-----------------------------------------------------------------------------
 *  Creation date : 24/10/96            Author : V.L.
 *-----------------------------------------------------------------------------
 *  Modification date :                 Modification Author :
 *  Modifications :
 *---------------------------------------------------------------------------*/
CWarning::~CWarning()
{
}

// Resets the buffer
void CWarning::Reset()
{
    m_p_cBuffer = _T("");
    m_bWarning = false;
    m_bError = false;
}

/*-----------------------------------------------------------------------------
 *  Description : add string in buffer, insert at current position
 *				current position is modified after the inserted string
 *-----------------------------------------------------------------------------
 *  Input :		none
 *  Output :	none
 *-----------------------------------------------------------------------------
 *  Creation date : 24/10/96            Author : V.L.
 *-----------------------------------------------------------------------------
 *  Modification date :                 Modification Author :
 *  Modifications :
 *---------------------------------------------------------------------------*/
void CWarning::AddString( const TCHAR* szString )
{
    m_p_cBuffer.Append( szString );
}


/*-----------------------------------------------------------------------------
 *  Description : write something in buffer (use it like a printf)
 *-----------------------------------------------------------------------------
 *  Input :		
 *  Output :	none
 *-----------------------------------------------------------------------------
 *  Creation date : 24/10/96            Author : V.L.
 *-----------------------------------------------------------------------------
 *  Modification date :                 Modification Author :
 *  Modifications :
 *---------------------------------------------------------------------------*/
void CWarning::Print( const TCHAR *szFormat, ... )
{
	va_list args;
	static TCHAR szTemp[1024];

	va_start( args, szFormat );

	_vstprintf( szTemp, szFormat, args );
	AddString( szTemp );
    
	va_end( args );
}

void CWarning::PrintWarning( const TCHAR* szFormat, ... )
{
	va_list args;
	static TCHAR szTemp[1024];

	va_start( args, szFormat );

	_vstprintf( szTemp, szFormat, args );
	Print( _T("WARNING:\t%s"), szTemp );
    
	va_end( args );

    m_bWarning = true;
}

void CWarning::PrintError( const TCHAR* szFormat, ... )
{
	va_list args;
	static TCHAR szTemp[1024];

	va_start( args, szFormat );

	_vstprintf( szTemp, szFormat, args );
	Print( _T("ERROR:\t%s"), szTemp );
    
	va_end( args );

    m_bError = true;
}

const TCHAR* CWarning::GetBuffer() const
{
    if (m_p_cBuffer.data())
        return m_p_cBuffer.data();

    static const TCHAR temp[] = _T("");
    return temp;
}
