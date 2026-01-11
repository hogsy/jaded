// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#include "ProjectConfig.h"

#ifndef XENON_PRECOMP_HEADER
#define XENON_PRECOMP_HEADER

// SC: Define to enable rough front to back opaque sorting
#if !defined(_XENON_OPAQUES_FRONT_TO_BACK)
#define _XENON_OPAQUES_FRONT_TO_BACK
#pragma message("_XENON_OPAQUES_FRONT_TO_BACK defined")
#endif

#if defined(_XENON_RENDER_PC)

#define D3DRS_MAX   210
#define D3DSAMP_MAX 14

#define D3DCOLORWRITEENABLE_ALL (D3DCOLORWRITEENABLE_RED   | \
                                 D3DCOLORWRITEENABLE_GREEN | \
                                 D3DCOLORWRITEENABLE_BLUE  | \
                                 D3DCOLORWRITEENABLE_ALPHA)

#define D3DFMT_LIN_A8       D3DFMT_A8
#define D3DFMT_LIN_A4R4G4B4 D3DFMT_A4R4G4B4
#define D3DFMT_LIN_R5G6B5   D3DFMT_R5G6B5
#define D3DFMT_LIN_A8R8G8B8 D3DFMT_A8R8G8B8
#define D3DFMT_LIN_X8R8G8B8 D3DFMT_X8R8G8B8
#define D3DFMT_LIN_DXT1     D3DFMT_DXT1
#define D3DFMT_LIN_DXT2     D3DFMT_DXT2
#define D3DFMT_LIN_DXT3     D3DFMT_DXT3
#define D3DFMT_LIN_DXT4     D3DFMT_DXT4
#define D3DFMT_LIN_DXT5     D3DFMT_DXT5

#define D3DPT_QUADLIST      (D3DPRIMITIVETYPE)13

//#define _PC_FORCE_DYNAMIC_VB

#if defined(_PC_FORCE_DYNAMIC_VB)
#pragma message("_PC_FORCE_DYNAMIC_VB is defined :(")
#endif

#endif // defined(_XENON_RENDER_PC)

#if defined(_XENON)
#include <xtl.h>
#endif // defined(_XENON)

#define D3DFMT_PAL4 (D3DFORMAT)(MAKEFOURCC('P','A','L','4'))
#define D3DFMT_PAL8 (D3DFORMAT)(MAKEFOURCC('P','A','L','8'))
#define D3DFMT_DDS  (D3DFORMAT)(MAKEFOURCC('D','D','S','!'))

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/ERRors/ERR.h"
#include "ENGine/Sources/WORld/WOR.h"
#include "GraphicDK/Sources/GDInterface/GDInterface.h"
#include "GraphicDK/Sources/MATerial/MATstruct.h"
#include "GraphicDK/Sources/TEXture/TEXstruct.h"
#include "GraphicDK/Sources/TEXture/TEXconvert.h"
#include "GraphicDK/Sources/TEXture/TEXxenon.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "SDK/Sources/BIGfiles/LOAding/LOAread.h"

#if defined(_XENON_RENDER_PC)
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>
#endif

#include "XeUtils.h"

// TODO: reference additional headers your program requires here

#endif // XENON_PRECOMP_HEADER