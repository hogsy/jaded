//------------------------------------------------------------------------------
//   DATCP4FileSys.h
/// \author    YCharbonneau
/// \date      2005-01-07
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4FILESYS_DELETE_H__
#define __DATCP4FILESYS_DELETE_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "DATCP4FileSys.h"

//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------
#ifdef JADEFUSION
extern void         BIG_DeleteFile(BIG_INDEX, BIG_INDEX);
#else
extern "C" void         BIG_DeleteFile(BIG_INDEX, BIG_INDEX);
#endif

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4FileSysDelete : public DAT_CP4FileSys
{
public :

	DAT_CP4FileSysDelete()
	{
	}

	virtual ~DAT_CP4FileSysDelete()
	{
	}

	
	

	virtual int Stat() 
	{		
		BIG_KEY ulKey = DAT_CUtils::GetKeyFromString( Name() );

		// Find file in BF 
		BIG_INDEX ulIndex =	BIG_ul_SearchKeyToFat(ulKey);
		if ( ulIndex == BIG_C_InvalidIndex)
			return 0;

		return FSF_EXISTS;
	}

	virtual void	Unlink( Error *e = 0 )				
	{
		BIG_KEY ulKey = DAT_CUtils::GetKeyFromString( Name() );

		// Find file in BF 
		BIG_INDEX ulIndex =	BIG_ul_SearchKeyToFat(ulKey);
		if ( ulIndex == BIG_C_InvalidIndex)
		{
			e->Set(E_FAILED,"File not in bigfile");
			return;
		}

		BIG_DeleteFile(BIG_ParentFile(ulIndex), ulIndex);
	}
protected:

};

//------------------------------------------------------------------------------

#endif //#ifndef __DATCP4FILESYS_DELETE_H__




