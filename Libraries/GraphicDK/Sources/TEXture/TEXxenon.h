// ------------------------------------------------------------------------------------------------
// File   : TEXxenon.h
// Date   : 2005-08-03
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

#ifndef GUARD_TEXXENON_H
#define GUARD_TEXXENON_H

#if defined(_XENON_RENDER)

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
#include "BIGfiles/BIGkey.h"
#include "BASe/BAStypes.h"

// ------------------------------------------------------------------------------------------------
// FUNCTIONS
// ------------------------------------------------------------------------------------------------

ULONG TEX_ul_XeGetValidSize(ULONG _ul_Size);
ULONG TEX_ul_XeGetMaxMipMaps(ULONG _ul_Size);
ULONG TEX_ul_XeGetMaxMipMapsCount(ULONG _ul_Width, ULONG _ul_Height);
void   TEX_XeValidateProperties(TEX_tdst_XenonFileTex* _pst_Properties);
void   TEX_XeResetProperties(TEX_tdst_XenonFileTex* _pst_Properties);
void   TEX_XeLoadProperties(TEX_tdst_XenonFileTex* _pst_Properties, char** _ppc_Buffer);
void   TEX_XeSaveProperties(TEX_tdst_XenonFileTex* _pst_Properties);

LONG TEX_l_File_LoadDDS(char* _pc_Buf, TEX_tdst_File_Desc* _pst_Desc, ULONG _ul_FileLength);

BOOL TEX_XeConvertXenonDDSFileToARGB32(void* _p_FileBuffer, ULONG _ul_FileLength, TEX_tdst_File_Desc* _pst_TexDesc);

#if defined(ACTIVE_EDITORS)

BOOL TEX_XeConvertDDSFileToARGB32(void* _p_FileBuffer, ULONG _ul_FileLength, struct TEX_tdst_File_Desc_* _pst_TexDesc);

BOOL TEX_XeGenerateDDS(TEX_tdst_XenonFileTex* _p_SrcTex, CHAR* _sz_DirName, CHAR* _sz_Name);
BOOL TEX_XeGenerateCBM(TEX_tdst_XenonCubeMapFileTex* _p_SrcTex, CHAR* _sz_DirName, CHAR* _sz_Name, BOOL _b_Export = FALSE);

void TEX_ReplaceAllTGAs(void);
void TEX_ReplaceAllDDSs(void);

#endif

#endif

#endif // #ifdef GUARD_TEXXENON_H
