//------------------------------------------------------------------------------
//   DAT_CP4FileSysSycn.h
/// \author    YCharbonneau
/// \date      2005-01-07
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4FILESYSSYNC_H__
#define __DATCP4FILESYSSYNC_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include <string>
#include "DATCP4FileSys.h"
#include "p4/clientapi.h"
#include "BIGfiles\BIGread.h"
#include "BIGfiles\BIGmdfy_file.h"
#include "DATCP4BFObject.h"
#include "DATCP4Message.h"
#include "DATCUtils.h"



//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4FileSysSync : public DAT_CP4FileSys
{
public :

	DAT_CP4FileSysSync(const std::string& _strBigDir) : 
	m_strBigDir(_strBigDir),
	m_bReadHeader(false),
	m_bUpdate(true)
	{
		ms_Buffer.Clear();
	}

	virtual ~DAT_CP4FileSysSync()
	{
		// delete compressed or uncompressed data.
		m_ObjectData.Free();
		ms_Buffer.Clear();
	}


	virtual void Open( FileOpenMode mode, Error *e )
	{
	}
	virtual void Write( const char *buf, int len, Error *e );
	virtual void Close( Error *e );

	virtual int Stat() 
	{		
		// we do not need to test if file exists or not.
		return 0 ;
	}

	virtual void	Unlink( Error *e = 0 )				
	{
		// Get key associated to P4 file name
		BIG_KEY ulKey = DAT_CUtils::GetKeyFromString( Name() );
		BIG_INDEX ulIndex = BIG_ul_SearchKeyToFat(ulKey);
		if ( ulIndex == BIG_C_InvalidIndex ) 
			return ;

		BIG_DeleteFile(BIG_ParentFile(ulIndex), ulIndex);
	}

private :
	static StrBuf			ms_Buffer;
	static std::string		ms_strFilename;
	std::string				m_strBigDir;
	bool					m_bReadHeader;
	bool					m_bUpdate;
	DAT_CP4BFObjectHeader	m_ObjectHeader;
	DAT_CP4BFObjectData		m_ObjectData;
};

//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4FILESYSSYNC_H__




