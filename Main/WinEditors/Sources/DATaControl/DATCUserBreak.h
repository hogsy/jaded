//------------------------------------------------------------------------------
//   DATCUserBreak.h
/// \author    YCharbonneau
/// \date      2005-01-31
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCUSERBREAK_H__
#define __DATCUSERBREAK_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "p4/clientapi.h"
#include "LINks/LINKmsg.h"
//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------
#ifdef JADEFUSION
extern BOOL BIG_b_CheckUserAbortCommand(void);
#else
extern "C" BOOL BIG_b_CheckUserAbortCommand(void);
#endif
//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CUserBreak : public KeepAlive
{
	virtual int IsAlive()
	{
		BIG_b_CheckUserAbortCommand();
		// check for user cancel 
		if(LINK_gb_EscapeDetected) 
		{
			std::string strMessage = "[P4:WARNING]";
			strMessage += " - User break - " ;
			LINK_PrintStatusMsg(const_cast<char*>(strMessage.c_str()));
			return  0;
		}

		return 1;
	}
};
//------------------------------------------------------------------------------

#endif //#ifndef __DATCUSERBREAK_H__

