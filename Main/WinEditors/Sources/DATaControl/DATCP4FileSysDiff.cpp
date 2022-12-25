//------------------------------------------------------------------------------
// Filename   :DATCP4FileSysDiff.cpp
/// \author    NBeaufils
/// \date      18-Feb-05
/// \par       Description: Implementation of DATCP4FileSysDiff
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"
#include "DATCUtils.h"
#include "DATCP4FileSysDiff.h"
#include "DATCP4ClientInfo.h"
#include "DATCP4Message.h"

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
StrBuf DAT_CP4FileSysDiff::ms_BufferServer;

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------
//------------------------------------------------------------
//   void DAT_CP4FileSysDiff::Open( FileOpenMode mode, Error *e )
/// \author    YCharbonneau
/// \date      06/04/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void DAT_CP4FileSysDiff::Open( FileOpenMode mode, Error *e )
{
	BIG_KEY ulKey = DAT_CUtils::GetKeyFromString( Name() );

	if ( ulKey == BIG_C_InvalidKey )
	{
		// Open file from perforce
		ms_BufferServer.Clear();
	}
	else
	{
		if ( mode == FOM_READ )
		{
			// Open file in BF
			BIG_KEY ulKey = DAT_CUtils::GetKeyFromString( Name() );
			m_ulIndex = BIG_ul_SearchKeyToFat(ulKey);
			if ( m_ulIndex == BIG_C_InvalidIndex ) 
			{
				static char aszMessage[MAX_PATH] = {0};
				sprintf(aszMessage," 0x%08X : Key not found in BF: The system cannot find the file specified.",ulKey,Name());
				e->Set(E_FATAL,aszMessage );
				return;
			}
			m_pBufferClient = DAT_CUtils::PrepareFileBuffer( m_ulIndex, m_ulBufferClientSize );

			// setting up the cursor for read process
			m_pBufferClientCursor = (char*)m_pBufferClient ;
		}
	}
}

//------------------------------------------------------------
//   void DAT_CP4FileSysDiff::Write( const char *buf, int len, Error *e )
/// \author    YCharbonneau
/// \date      06/04/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void DAT_CP4FileSysDiff::Write( const char *buf, int len, Error *e )
{
	BIG_KEY ulKey = DAT_CUtils::GetKeyFromString( Name() );

	if ( ulKey == BIG_C_InvalidKey )
	{
		// write file from perforce
		ms_BufferServer.Append(buf,len);
	}
	else
	{
		// write file in BF
		assert(false);
	}
};

//------------------------------------------------------------
//   int DAT_CP4FileSysDiff::Read( char *buf, int len, Error *e )
/// \author    YCharbonneau
/// \date      06/04/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CP4FileSysDiff::Read( char *buf, int len, Error *e )
{
	BIG_KEY ulKey = DAT_CUtils::GetKeyFromString( Name() );

	if ( ulKey == BIG_C_InvalidKey )
	{
		// Read file in perforce 
		assert(false);
		return 0;
	}
	else
	{
		// Read file in BF
		int iBufferClientSize = len;

		if ( (int)m_ulBufferClientSize < len )
		{
			iBufferClientSize = m_ulBufferClientSize;
		}

		if ( iBufferClientSize > 0 ) 
		{
			memcpy(buf,m_pBufferClientCursor,iBufferClientSize); 
			m_pBufferClientCursor += iBufferClientSize;
			m_ulBufferClientSize -= iBufferClientSize;
		}
		return iBufferClientSize;
	}
	return 0;
}

//------------------------------------------------------------
//   int DAT_CP4FileSysDiff::Stat()
/// \author    YCharbonneau
/// \date      06/04/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CP4FileSysDiff::Stat()
{
	BIG_KEY ulKey = DAT_CUtils::GetKeyFromString( Name() );

	if ( ulKey == BIG_C_InvalidKey )
	{
		// Find file in perforce 
		assert(false);
		return 0;
	}
	else
	{
		// Find file in BF 
		BIG_INDEX ulIndex =	BIG_ul_SearchKeyToFat(ulKey);
		if ( ulIndex == BIG_C_InvalidIndex)
			return 0;

		return FSF_EXISTS;
	}
}
//------------------------------------------------------------
//   void DAT_CP4FileSysDiff::Close( Error *e )
/// \author    NBeaufils
/// \date      18-Feb-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void DAT_CP4FileSysDiff::Close( Error *e )
{
	BIG_KEY ulKey = DAT_CUtils::GetKeyFromString( Name() );

	// Find file in perforce 
	if ( ulKey == BIG_C_InvalidKey )
	{
		// Uncompressing data buffer from server.
		if ( ms_BufferServer.Length() > 0 ) 
		{
			DAT_CP4BFObjectData		ObjectData;
			DAT_CP4BFObjectHeader	ObjectHeader;

			ObjectHeader.Read(ms_BufferServer.Value(),ms_BufferServer.Length());
			ObjectHeader.Path(m_strFileName);

			// Keep a copy of the server uncompressed buffer/size/filename 
			ObjectData.Read(ms_BufferServer.Value() + ObjectHeader.Size(),ms_BufferServer.Length() - ObjectHeader.Size());
			ObjectData.Data(*((void**)&m_pFileData), m_ulFileSize);

			if ( m_ulFileSize == -1 ) 
			{
				char aszMessage[MAX_PATH] = {0};
				sprintf(aszMessage,"File with key: 0x%08X is corrupted.\n Please contact your data manager.",ulKey);
				DAT_CP4Message::OutputCriticalErrorMessage(aszMessage);
			}
		}
		else if ( ms_BufferServer.Length() == 0 ) 
		{
			char aszMessage[MAX_PATH] = {0};
			sprintf(aszMessage,"File with key: 0x%08X has empty content.\n Please contact your data manager.",ulKey);
			DAT_CP4Message::OutputCriticalErrorMessage(aszMessage);
		}

		// Clear buffer
		ms_BufferServer.Clear();
	}
	else 
	{
		// Close file in BF
		delete [] m_pBufferClient;
		m_pBufferClientCursor = NULL;
		m_pBufferClient = NULL;
		m_ulBufferClientSize = -1;
	}
}

//------------------------------------------------------------------------------

