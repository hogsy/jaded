// ------------------------------------------------------------------------------------------------
// File   : Precomp.h
// Date   : 2005-06-15
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

#ifndef GUARD_XMP_PRECOMP_H
#define GUARD_XMP_PRECOMP_H

#pragma message("*******************************************************************************")
#pragma message("*** XenonMeshProcessing Precompiled Header ************************************")

#include "projectconfig.h"
#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/CLIbrary/CLIxxx.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "BASe/ERRors/ERR.h"

#if defined(_XENON_RENDER_PC)
#include <d3d9.h>
#include <d3dx9.h>
#endif

#endif // #ifdef GUARD_XMP_PRECOMP_H
