#ifndef UBI_INCLUDED_PRECOMP_H__143CF755_1C86_4828_B6D1_E14CE05BBE11
#define UBI_INCLUDED_PRECOMP_H__143CF755_1C86_4828_B6D1_E14CE05BBE11

#ifdef JADEFUSION
#pragma message("*******************************************************************************")
#pragma message("*** EDItors Precompiled Header ************************************************")

#include "projectconfig.h"
#include "BASe/ERRors/ERR.h"
#endif

#pragma comment( linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"" )

#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/CLIbrary/CLIxxx.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"

#if defined(_XENON_RENDER_PC)
#include <d3d9.h>
#include <d3dx9.h>
#endif
#endif //UBI_INCLUDED_PRECOMP_H__143CF755_1C86_4828_B6D1_E14CE05BBE11