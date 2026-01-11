//------------------------------------------------------------------------------
// Filename   :DATCP4FileSysSubmit.cpp
/// \author    YCharbonneau
/// \date      06/04/2005
/// \par       Description: Implementation of DATCP4FileSysSubmit
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"

#include "DATCP4FileSysSubmit.h"


//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PreProcessor defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// externals and global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// statics and constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------
//   void DAT_CP4FileSysSubmit::Open( FileOpenMode mode, Error *e )
/// \author    YCharbonneau
/// \date      06/04/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void DAT_CP4FileSysSubmit::Open( FileOpenMode mode, Error *e )
{
	// Get key associated to P4 file name
	if (  m_ulIndex == BIG_C_InvalidIndex ) 
	{
		static char aszMessage[MAX_PATH] = {0};
		sprintf(aszMessage,"Key not found in BF: The system cannot find the file specified: %s",Name());
		e->Set(E_FATAL,aszMessage );
		return;
	}

	m_pBuffer = DAT_CUtils::PrepareFileBuffer( m_ulIndex, m_ulBufferSize,TRUE );

	// setting up the cursor for read process
	m_pBufferCursor = (char*)m_pBuffer ;
}

//------------------------------------------------------------
//   int DAT_CP4FileSysSubmit::Read( char *buf, int len, Error *e )
/// \author    YCharbonneau
/// \date      06/04/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CP4FileSysSubmit::Read( char *buf, int len, Error *e )
{
	int iBufferSize = len;

	if ( (int)m_ulBufferSize < len )
	{
		iBufferSize = m_ulBufferSize;
	}

	if ( iBufferSize > 0 ) 
	{
		memcpy(buf,m_pBufferCursor,iBufferSize); 
		m_pBufferCursor += iBufferSize;
		m_ulBufferSize -= iBufferSize;
	}
	return iBufferSize;
}

//------------------------------------------------------------
//   void  DAT_CP4FileSysSubmit::Close( Error *e )
/// \author    YCharbonneau
/// \date      06/04/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void  DAT_CP4FileSysSubmit::Close( Error *e )
{
	delete [] m_pBuffer;
	m_pBufferCursor = NULL;
	m_pBuffer = NULL;
	m_ulBufferSize = -1;
}
//------------------------------------------------------------------------------

