
//------------------------------------------------------------------------------
// Filename   :DATCP4FileSysSync.cpp
/// \author    YCharbonneau
/// \date      2005-02-15
/// \par       Description: Implementation of DATCP4FileSysSync
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"
#include "DATCP4FileSysSync.h"
#include "DATCUtils.h"
#include "DATCP4ClientInfo.h"
#include "DATCP4Message.h"
#include "DATCPerforce.h"


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
StrBuf DAT_CP4FileSysSync::ms_Buffer;
std::string DAT_CP4FileSysSync::ms_strFilename;

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

//------------------------------------------------------------
//   void DAT_CP4FileSysSync::Write( const char *buf, int len, Error *e )
/// \author    YCharbonneau
/// \date      06/04/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void DAT_CP4FileSysSync::Write( const char *buf, int len, Error *e )
{
    // Anti-bug (incompréhensible)
    if ((*((int*)buf) == 0x00088b1f) && 
        (*(((int*)buf)+1) == 0x00000000) && 
        (*(int*)(buf+15) == 0x12340000))
    {
        buf += 15;
        len -= 23;
    }

	ms_Buffer.Append(buf,len);

	// if we only want to sync a single directory, skip buffer read after 
	// the header has been retreived.
	if ( !m_bReadHeader ) 
	{
		m_ObjectHeader.Read(ms_Buffer.Value(),ms_Buffer.Length());
		if ( (UINT)ms_Buffer.Length() >= m_ObjectHeader.Size() && ! m_bReadHeader) 
		{
			m_bReadHeader = true;

			m_ObjectHeader.Path(ms_strFilename);

			BIG_KEY ulKey = DAT_CUtils::GetKeyFromString( Name() );
			BIG_KEY ulHeaderKey = BIG_C_InvalidKey; 
			m_ObjectHeader.JadeKey( ulHeaderKey ) ;

			if ( ulHeaderKey != ulKey ) 
			{
				char aszMessage[MAX_PATH] = {0};
				sprintf(aszMessage,"File with key mismatch: 0x%08X does not match with perforce header key:0x%08X .",
					ulKey,ulHeaderKey);
				DAT_CP4Message::OutputErrorKeyMismatchMessage(aszMessage);	
				e->Set(E_FAILED,"");
				ms_Buffer.Clear();
				m_bUpdate = false;
			}


			// making sure the bigfile version correspond to the requested file
			UINT uiBFVersion = 0;
			m_ObjectHeader.Version(uiBFVersion);
			if ( BIG_Version() != uiBFVersion ) 
			{
				// for now we disable the option of getting a file that does not correspond to bfversion
				// We will need to convert each file later on.
				std::string strMessage = "File version does not correspond with bigfile version. File cannot be sync:";
				strMessage += ms_strFilename;
				DAT_CP4Message::OutputErrorMessage(strMessage.c_str());
				e->Set(E_FAILED,"");
				ms_Buffer.Clear();
				m_bUpdate = false;
			}

			if ( m_strBigDir != ""  )
			{
				// if we just want to sync a specific directory, failed if file 
				// is not in that directory	

				if ( m_strBigDir != "" && ms_strFilename.find(m_strBigDir) == -1 ) 
				{
					e->Set(E_FAILED,"");
					ms_Buffer.Clear();
					m_bUpdate = false;
					OutputDebugString(Name());
					OutputDebugString("\n");
				}
			}

		}
	}
}

//------------------------------------------------------------
//   void DAT_CP4FileSysSync::Close( Error *e )
/// \author    YCharbonneau
/// \date      2005-02-15
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void DAT_CP4FileSysSync::Close( Error *e )
{
	if ( !m_bUpdate ) return ;

	// Get key associated to P4 file name
	BIG_KEY ulKey = DAT_CUtils::GetKeyFromString( Name() );

	char* pData = NULL;
	size_t uiDataSize = 0;

	// Uncompressing data buffer.
	if ( ms_Buffer.Length() > 0 ) 
	{
		char* pBuffer = ms_Buffer.Value();
		ULONG ulLenth = 0;
	
		m_ObjectData.Read(ms_Buffer.Value() + m_ObjectHeader.Size(),ms_Buffer.Length() - m_ObjectHeader.Size());
		m_ObjectData.Data(*((void**)&pData),uiDataSize);

		if ( uiDataSize == -1 ) 
		{
			char aszMessage[MAX_PATH] = {0};
			sprintf(aszMessage,"File with key: 0x%08X is corrupted.\n Please contact your data manager.",ulKey);
			DAT_CP4Message::OutputCriticalErrorMessage(aszMessage);
			// delete compressed or uncompressed data.
			m_ObjectData.Free();
			ms_Buffer.Clear();
			return;
		}
	}
	
	// -- IMPORTANT NOTE --
	// If pData is NULL and uiDataSize is 0, we still want the file
	// to be updated in the BF. So we set a value for the pointer
	// pData (see BIG_ul_UpdateCreateFileOptim function for details)
	if ( pData == NULL && uiDataSize == 0)
		pData = ms_Buffer.Value(); 

	UINT uiIsUniverseKey = FALSE;
	m_ObjectHeader.IsUniverseKey(uiIsUniverseKey);
	if( uiIsUniverseKey ) BIG_UniverseKey() = ulKey;

	// Update file buffer in BF corresponding to key
	DAT_CPerforce::UpdateBFFileBuffer(ulKey, pData, uiDataSize,ms_strFilename.c_str());

	std::string strMessage = " Getting - " ;
	strMessage += ms_strFilename; 	;
	DAT_CP4Message::OutputInfoMessage(strMessage.c_str());
}

//------------------------------------------------------------------------------



