//------------------------------------------------------------------------------
//   DATCP4FileSysSubmit.h
/// \author    YCharbonneau
/// \date      2005-01-07
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4FILESYSSUBMIT_H__
#define __DATCP4FILESYSSUBMIT_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "p4/clientapi.h"
#include "BIGfiles\BIGread.h"
#include "DATCUtils.h"
#include "DATCP4BFObject.h"
#include "DATCP4FileSys.h"
#include "BIGfiles\BIGmdfy_file.h"
#include "DATCUtils.h"
//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4FileSysSubmit : public DAT_CP4FileSys
{
public :

	DAT_CP4FileSysSubmit() :  DAT_CP4FileSys(),
	m_ulIndex(BIG_C_InvalidIndex),
	m_pBufferCursor(NULL),
	m_pBuffer(NULL),
	m_ulBufferSize(-1)
	{
	}

	virtual ~DAT_CP4FileSysSubmit() 
	{
	}

	virtual void    Open( FileOpenMode mode, Error *e );
	virtual int		Read( char *buf, int len, Error *e );
	virtual void    Close( Error *e );

	virtual int		StatModTime()
	{
		BIG_KEY ulKey = DAT_CUtils::GetKeyFromString( Name() );
		m_ulIndex = BIG_ul_SearchKeyToFat(ulKey);

		if ( m_ulIndex == BIG_C_InvalidIndex ) 
			return 0;

		return BIG_TimeFile(m_ulIndex) ;
	}

	virtual void	Chmod( FilePerm perms, Error *e )		
	{
	}

private:

	char* m_pBuffer;
	char* m_pBufferCursor;
	ULONG m_ulBufferSize;
	BIG_INDEX m_ulIndex;
};

//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4FILESYSSUBMIT_H__




