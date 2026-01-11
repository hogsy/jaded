//------------------------------------------------------------------------------
//   DATCP4FileSysDiff.h
/// \author    NBeaufils
/// \date      18-Feb-05
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4FILESYSDIFF_H__
#define __DATCP4FILESYSDIFF_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "p4/clientapi.h"
#include "BIGfiles\BIGread.h"
#include "DATCUtils.h"
#include "DATCP4BFObject.h"
#include "DATCP4FileSys.h"
#include "assert.h"

//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4FileSysDiff : public DAT_CP4FileSys
{
public :

	DAT_CP4FileSysDiff() : 
	m_pBufferClient(NULL),
	m_ulBufferClientSize(-1),
	m_ulFileSize(0),
	m_pFileData(NULL),
	m_ulIndex(BIG_C_InvalidIndex)
	{
	}

	virtual ~DAT_CP4FileSysDiff()
	{
		if ( m_pFileData != NULL )
		{
			m_ulFileSize = 0;
			delete[] m_pFileData;
			m_pFileData = NULL;
		}
	}

	virtual void Open( FileOpenMode mode, Error *e );
	virtual void Write( const char *buf, int len, Error *e );
	virtual int Read( char *buf, int len, Error *e );
	virtual int Stat();
	virtual void Close( Error *e );

	virtual void	Chmod( FilePerm perms, Error *e )		
	{
	}

	virtual void	Rename( FileSys *target, Error *e )		
	{	
		// this is used by revert client, to sync back to server version. 
		// we do not want to sync reverted files unless user wants to. 
	}

	const std::string& GetP4Filename(){return m_strFileName;}
	size_t& GetP4FileSize(){return m_ulFileSize;}
	void* GetP4FileData(){return m_pFileData;}
	
private:

	// Variables used for buffer read from BF on client
	char* m_pBufferClient;
	char* m_pBufferClientCursor;
	ULONG m_ulBufferClientSize;

	// Values coming from the server
	std::string m_strFileName;
	size_t m_ulFileSize;
	void* m_pFileData;

	BIG_INDEX m_ulIndex;

	// Variable used for buffer write from Perforce
	static StrBuf ms_BufferServer;
};

//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4FILESYSDIFF_H__

