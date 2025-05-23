// ************************************************
// Hack Management
//
// Used for King Kong Xenon Debugging
// By Alexandre David (February 2005)
// ************************************************

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "Precomp.h"

#include "HackManager.h"

// ***********************************************************************************************************************

#include "BASe/ERRors/ERRasser.h"

// ***********************************************************************************************************************

namespace HackManager
{

// ***********************************************************************************************************************
//    Functions - Exposed
// ***********************************************************************************************************************

void Assert()
{
	ERR_X_Assert(FALSE);
}

// ***********************************************************************************************************************

} // Namespace

// ***********************************************************************************************************************
