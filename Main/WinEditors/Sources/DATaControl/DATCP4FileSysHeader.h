//------------------------------------------------------------------------------
//   DAT_CP4FileSysHeader.h
/// \author    YCharbonneau
/// \date      2005-01-07
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DAT_CP4FILESYSHEADER__H_
#define __DAT_CP4FILESYSHEADER__H_

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "p4/clientapi.h"
#include "DATCP4BFObject.h"
//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4FileSysHeader : public FileSys
{
public :

	DAT_CP4FileSysHeader() : 
	m_uiReadSize(0)
	{
	
	}

	virtual void Open( FileOpenMode mode, Error *e )
	{
		m_uiReadSize = 0;
		ms_Buffer.Clear();
	}

	virtual void Write( const char *buf, int len, Error *e )
	{
		ms_Buffer.Append(buf,len);
		m_uiReadSize += len;
		DAT_CP4BFObjectHeader ObjectHeader;
		ObjectHeader.Read(ms_Buffer.Value(),ms_Buffer.Length());
		if ( m_uiReadSize >= ObjectHeader.Size() ) 
		{

			// we do not want to get all the file, we just need the header to retreive 
			// file info. This is a bit of a hack, but their is no other known way to get
			// only a certain number of byte from perforce server.
			e->Set(E_FATAL,"");
		}
	}

	virtual void Close( Error *e )
	{
	}

	virtual int Read( char *buf, int len, Error *e ) {return 0;}
	virtual int Stat() {return 0;}
	virtual int StatModTime() {return 0;}
	virtual void Chmod( FilePerm perms, Error *e ) {}
	virtual void Truncate( Error *e ){}
	virtual void Unlink( Error *e = 0 ){}
	virtual void Rename( FileSys *target, Error *e ){}
	virtual void ChmodTime( Error *e ){}

	static StrBuf* GetBuffer() { return &ms_Buffer; }

private :
	static StrBuf	ms_Buffer;
	BOOL			m_bHeaderOnly;
	UINT			m_uiReadSize;

};

StrBuf DAT_CP4FileSysHeader::ms_Buffer;

//------------------------------------------------------------------------------

#endif //#ifndef __DAT_CP4FILESYSHEADER__H_




