//------------------------------------------------------------------------------
// Filename   :DATCPerforce.cpp
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: Implementation of DATCPerforce
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"
#include "DATCPerforce.h"

#include "DATCCompression.h"
#include "DATCP4BFObject.h"

// client users
#include "DATCP4ClientUserPrint.h"

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
//   int DAT_CPerforce::P4Print()
/// \author    FFerland
/// \date      2005-02-07
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Print( ULONG _ul_Key, ULONG _ul_Rev, StrBuf& _DataBuffer, bool bGetRawData )
{
	DAT_CP4ClientUserPrint UserClient;

	// get the content of the file
	{
		int argCount = 2;

		char szP4FilePath[ MAX_PATH + 1 ];
		DAT_CUtils::GetP4FileFromKey( _ul_Key, szP4FilePath,DAT_CPerforce::GetInstance()->GetP4Root().c_str() );

		char szP4FullFilePath[ MAX_PATH + 16 ];

		if( _ul_Rev != 0 )
			sprintf( szP4FullFilePath, "%s#%d", szP4FilePath, _ul_Rev );
		else
			strcpy( szP4FullFilePath, szP4FilePath );

		char* argValues[] =
		{
			"-q",
				szP4FullFilePath
		};

		m_Client.SetArgv( argCount, argValues );
		m_Client.Run( P4_PRINT, &UserClient );
	}

	const StrBuf& buffer = UserClient.GetDataBuffer( );

	// get data
	if( bGetRawData )
	{
		_DataBuffer.Set( buffer.Value( ), buffer.Length( ) );
	}
	else
	{
		DAT_CP4BFObjectHeader ObjectHeader;
		DAT_CP4ClientInfoHeader InfoHeader;
		DAT_CP4BFObjectData ObjectData;
		char* pData = NULL; 
		size_t uiDataSize = 0;

	ObjectHeader.Read( buffer.Value( ), buffer.Length( ) );
	DAT_CUtils::GetHeader( ObjectHeader, InfoHeader );

		ObjectData.Read( buffer.Value( ) + ObjectHeader.Size( ), buffer.Length( ) - ObjectHeader.Size( ) );
		ObjectData.Data( *((void**)&pData), uiDataSize );

		_DataBuffer.Set( pData, uiDataSize );

		ObjectData.Free();
	}

	return ! UserClient.IsError();
}
