//------------------------------------------------------------------------------
//   DATCP4FileSys.h
/// \author    YCharbonneau
/// \date      2005-01-07
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4FILESYS_H__
#define __DATCP4FILESYS_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "assert.h"
//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4FileSys : public FileSys
{
public :

	DAT_CP4FileSys()
	{
	}

	virtual ~DAT_CP4FileSys()
	{
	}

	
	
	virtual void	Chmod( FilePerm perms, Error *e )		
	{
		assert(0 && "DAT_CP4FileSys::Chmod() - This should NOT be called." );
	}

	virtual void	ChmodTime( Error *e )				
	{
		assert(0 && "DAT_CP4FileSys::ChmodTime() - This should NOT be called." );
	}
	
	virtual void Close( Error *e )
	{
		assert(0 && "DAT_CP4FileSys::Close() - This should NOT be called." );
	}

	virtual void Open( FileOpenMode mode, Error *e )
	{
		assert(0 && "DAT_CP4FileSys::Open() - This should NOT be called." );
	}

	virtual int Read( char *buf, int len, Error *e ) 
	{		
		assert(0 && "DAT_CP4FileSys::Read() - This should NOT be called." );
		return 0 ;
	}

	virtual void	Rename( FileSys *target, Error *e )		
	{
		assert(0 && "DAT_CP4FileSys::Rename() - This should NOT be called." );
	}

	virtual int Stat() 
	{		
		assert(0 && "DAT_CP4FileSys::Stat() - This should NOT be called." );
		return 0 ;
	}

	virtual int		StatModTime()
	{
		assert(0 && "DAT_CP4FileSys::StatModTime() - This should NOT be called." );
		return 0 ;
	}

	virtual void	Truncate( Error *e )				
	{
		assert(0 && "DAT_CP4FileSys::Truncate() - This should NOT be called." );
	}

	virtual void	Unlink( Error *e = 0 )				
	{
		assert(0 && "DAT_CP4FileSys::Unlink() - This should NOT be called." );
	}

	virtual void Write( const char *buf, int len, Error *e )
	{
		assert(0 && "DAT_CP4FileSys::Write() - This should NOT be called." );
	}

protected:

};

//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4FILESYS_H__




