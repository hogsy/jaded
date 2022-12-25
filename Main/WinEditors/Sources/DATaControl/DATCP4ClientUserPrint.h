//------------------------------------------------------------------------------
//   DATCP4ClientUserPrint.h
/// \author    FFerland
/// \date      2005-02-07
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTUSERPRINT_H__
#define __DATCP4CLIENTUSERPRINT_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------

#include "PerforceUI.h"
#include "DATCP4Message.h"

//------------------------------------------------------------------------------
// Structures
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientUserPrint : public DAT_CP4Message, public PerforcePrintUI
{
public:
	DAT_CP4ClientUserPrint( ) : 
	DAT_CP4Message("Print")
	{		
	}

	virtual void OutputBinary(const_char* data, int length)
	{
		if( length > 0 )
			m_buffer.Append( data, length );
	}

	const StrBuf& GetDataBuffer( )
	{
		return m_buffer;
	}

private:
	StrBuf	m_buffer;
};
//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4CLIENTUSEROPENED_H__

