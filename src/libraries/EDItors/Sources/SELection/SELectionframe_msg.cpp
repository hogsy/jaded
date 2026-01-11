//------------------------------------------------------------------------------
// Filename   :SELectionframe_msg.cpp
/// \author    Nbeaufils
/// \date      2005-01-17
/// \par       Description: Implementation of SELectionframe_msg
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "SELectionframe.h"
#include "EDIbaseview.h"
#include "EDImainframe.h"
#include "EDImsg.h"
#include "OUTput\OUTmsg.h"

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
//   int ESELection_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
/// \author    Nbeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int ESELection_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
{
	switch(_ul_Msg)
	{
		case EOUT_MESSAGE_DESTROYWORLD:
			mp_GridCtrl->DeleteNonFixedRows();
			mp_GridCtrl->Refresh();
			return TRUE;
	}
	return FALSE;
}

#endif /* ACTIVE_EDITORS */
