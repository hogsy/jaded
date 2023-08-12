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
#include "LINks/LINKmsg.h"

//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------
#ifdef JADEFUSION
extern BOOL BIG_b_CheckUserAbortCommand(void);
#else
extern "C" BOOL BIG_b_CheckUserAbortCommand(void);
#endif

#endif //#ifndef __DATCUSERBREAK_H__
