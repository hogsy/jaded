//------------------------------------------------------------------------------
// Filename   :PERCheckframe_msg.cpp
/// \author    Nbeaufils
/// \date      2005-01-17
/// \par       Description: Implementation of PERCheckframe_msg
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "EDIbaseview.h"
#include "EDImainframe.h"
#include "PERCheckframe.h"
#include "PERforce/PERmsg.h"

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
//   int EPERCheck_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
/// \author    Nbeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int EPERCheck_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
{
	BIG_INDEX DirIndex = _ul_Param1;

	switch(_ul_Msg)
	{
		case EPER_MESSAGE_DIFFDIR_SELECTED:
			ExpandDiff(DirIndex, TRUE);

			// Make sure frame is active and shown
			this->mpo_MyView->SetCurSel(this->mi_NumPane);
			this->mpo_MyView->OnChangePane(NULL, NULL);

			return TRUE;
	}
	return FALSE;
}

#endif /* ACTIVE_EDITORS */
