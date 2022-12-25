/*$T ENGstruct.h GC!1.32 05/20/99 10:47:48 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Structures of the engine
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef __ENG_Struct_H__
#define __ENG_Struct_H__

#include "GDInterface/GDInterface.h"
#include "BASe/CLIbrary/CLIfile.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*
 ---------------------------------------------------------------------------------------------------
    For initialisation.
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  MAI_tdst_InitStruct_
{
    char    asz_ProjectName[L_MAX_PATH];
} MAI_tdst_InitStruct;

typedef struct
{
    HWND                    h_OwnerWindow;
    HWND                    h_DisplayWindow;
    struct WOR_tdst_World_  *pst_World;
    GDI_tdst_DisplayData    *pst_DisplayData;
} MAI_tdst_WinHandles;

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif