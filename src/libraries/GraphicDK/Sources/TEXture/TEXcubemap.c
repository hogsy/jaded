// ------------------------------------------------------------------------------------------------
// File   : TEXcubemap.c
// Date   : 2005-01-18
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

#include "Precomp.h"

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERR.h"
#include "INOut/INOfile.h"
#include "TEXture/TEXstruct.h"
#include "TEXture/TEXfile.h"
#include "TEXture/TEXcubemap.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "GraphicDK/Sources/GDInterface/GDInterface.h"

#if defined(ACTIVE_EDITORS)
#include "LINks/LINKmsg.h"
#include "LINks/LINKtoed.h"
#endif // defined(ACTIVE_EDITORS)

#if defined(_XENON_RENDER)
#include "XenonGraphics/XeTextureManager.h"
#endif

#if defined(TEX_USE_CUBEMAPS)

// ------------------------------------------------------------------------------------------------
// CONSTANTS
// ------------------------------------------------------------------------------------------------
const ULONG TEX_CUBEMAP_REALLOC = 256;

// ------------------------------------------------------------------------------------------------
// STRUCTURES
// ------------------------------------------------------------------------------------------------
// Cube map list
typedef struct TEX_tdst_CubeMapList_
{
    ULONG                 ul_NumberOfCubeMaps;
    ULONG                 ul_MaxNumberOfCubeMaps;
    TEX_tdst_CubeMapInfo* dst_CubeMaps;
} TEX_tdst_CubeMapList;

// ------------------------------------------------------------------------------------------------
// PRIVATE VARIABLES
// ------------------------------------------------------------------------------------------------
static TEX_tdst_CubeMapList TEX_gst_CubeMapList = { 0, 0, NULL };
static BOOL s_bCanHWLoad = FALSE;
// ------------------------------------------------------------------------------------------------
// FUNCTIONS
// ------------------------------------------------------------------------------------------------
static BOOL  TEX_CubeMap_ReadFile(BIG_KEY _ul_Key, SHORT s_Index);

static BOOL  TEX_CubeMap_IsValidIndex(SHORT _s_Index);
static SHORT TEX_CubeMap_ReserveIndex(void);
static SHORT TEX_CubeMap_FindCubeMap(BIG_KEY _ul_Key);
static void  TEX_CubeMap_FreeCubeMap(SHORT _s_Index, BOOL _b_Shutdown);

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Name   : TEX_CubeMap_Init
// Params : _ul_InitCubeMapCount : Initial cube map list size
// RetVal : None
// Descr. : Initialize the cube map manager
// ------------------------------------------------------------------------------------------------
void TEX_CubeMap_Init(ULONG _ul_InitCubeMapCount)
{
    BOOL  bFirstInit  = (TEX_gst_CubeMapList.ul_MaxNumberOfCubeMaps == 0);
    ULONG ul_FileSize = 0;
    ULONG ul_Pos      = 0;
    CHAR* pc_Buffer   = NULL;

    if (bFirstInit)
    {
        if (_ul_InitCubeMapCount == 0)
            _ul_InitCubeMapCount = 256;

        TEX_gst_CubeMapList.ul_MaxNumberOfCubeMaps = _ul_InitCubeMapCount;
        TEX_gst_CubeMapList.ul_NumberOfCubeMaps    = 0;
        TEX_gst_CubeMapList.dst_CubeMaps           = (TEX_tdst_CubeMapInfo*)MEM_p_Alloc(_ul_InitCubeMapCount * sizeof(TEX_tdst_CubeMapInfo));

        memset(TEX_gst_CubeMapList.dst_CubeMaps, 0, _ul_InitCubeMapCount * sizeof(TEX_tdst_CubeMapInfo));

        for (ULONG i = 0; i < _ul_InitCubeMapCount; ++i)
        {
            TEX_gst_CubeMapList.dst_CubeMaps[i].ul_Key  = BIG_C_InvalidKey;
            TEX_gst_CubeMapList.dst_CubeMaps[i].s_Index = TEX_CubeMap_InvalidIndex;
        }
    }
    else
    {
        for (ULONG i = 0; i < _ul_InitCubeMapCount; ++i)
        {
            if (TEX_gst_CubeMapList.dst_CubeMaps[i].ul_Key != BIG_C_InvalidKey)
            {
                ul_Pos = BIG_ul_SearchKeyToPos(TEX_gst_CubeMapList.dst_CubeMaps[i].ul_Key);
                if (ul_Pos == BIG_C_InvalidIndex)
                    continue;
                pc_Buffer = BIG_pc_ReadFileTmpMustFree(ul_Pos, &ul_FileSize);
                if ((pc_Buffer == NULL) || (ul_FileSize == 0))
                    continue;

                if (s_bCanHWLoad)
                {
#if defined(_XENON_RENDER)
                    if (GDI_b_IsXenonGraphics())
                    {
                        g_oXeTextureMgr.LoadCubeMap(i, pc_Buffer, ul_FileSize);
                    }
#endif
                    TEX_gst_CubeMapList.dst_CubeMaps[i].b_HWLoaded = TRUE;

#if defined(_DEBUG)
                    ERR_OutputDebugString("[CubeMap] - Hardware loaded [0x%08x], index %u\n", TEX_gst_CubeMapList.dst_CubeMaps[i].ul_Key, i);
#endif
                }

                L_free(pc_Buffer);
            }
        }
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : TEX_CubeMap_Free
// Params : _b_Shutdown : Free all for shutdown
// RetVal : None
// Descr. : Shutdown the cube map manager
// ------------------------------------------------------------------------------------------------
void TEX_CubeMap_Free(BOOL _b_Shutdown)
{
    ULONG i;

    for (i = 0; i < TEX_gst_CubeMapList.ul_MaxNumberOfCubeMaps; ++i)
    {
        if ((TEX_gst_CubeMapList.dst_CubeMaps[i].ul_Key != BIG_C_InvalidKey) &&
            (TEX_gst_CubeMapList.dst_CubeMaps[i].ul_NbReferences > 0)        &&
            (TEX_gst_CubeMapList.dst_CubeMaps[i].s_Index != TEX_CubeMap_InvalidIndex))
        {
            TEX_CubeMap_FreeCubeMap((SHORT)i, _b_Shutdown);
        }
    }

    if (_b_Shutdown)
    {
        MEM_Free(TEX_gst_CubeMapList.dst_CubeMaps);
        TEX_gst_CubeMapList.dst_CubeMaps           = NULL;
        TEX_gst_CubeMapList.ul_MaxNumberOfCubeMaps = 0;
        TEX_gst_CubeMapList.ul_NumberOfCubeMaps    = 0;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : TEX_CubeMap_Add
// Params : _ul_Key : Key of the .cbm file to load
// RetVal : Cube map index
// Descr. : Load a cube map or increment its reference count if already loaded
// ------------------------------------------------------------------------------------------------
SHORT TEX_CubeMap_Add(BIG_KEY _ul_Key)
{
    TEX_tdst_CubeMapInfo* pst_Info;
    SHORT s_Index;

    if ((_ul_Key == 0) || (_ul_Key == BIG_C_InvalidKey))
        return TEX_CubeMap_InvalidIndex;

    s_Index = TEX_CubeMap_FindCubeMap(_ul_Key);
    if (s_Index != TEX_CubeMap_InvalidIndex)
    {
        // Increment the reference count
        TEX_gst_CubeMapList.dst_CubeMaps[s_Index].ul_NbReferences++;
        return s_Index;
    }

    // Get a free slot
    s_Index  = TEX_CubeMap_ReserveIndex();
    pst_Info = &TEX_gst_CubeMapList.dst_CubeMaps[s_Index];

    pst_Info->ul_Key          = _ul_Key;
    pst_Info->ul_NbReferences = 1;
    pst_Info->s_Index         = s_Index;

    if (s_bCanHWLoad)
    {
        if (!TEX_CubeMap_ReadFile(_ul_Key, s_Index))
        {
            TEX_CubeMap_FreeCubeMap(s_Index, TRUE);
            return TEX_CubeMap_InvalidIndex;
        }
    }

    return s_Index;
}

// ------------------------------------------------------------------------------------------------
// Name   : TEX_CubeMap_Remove
// Params : _s_Index : Index of the cube map to unreference
// RetVal : None
// Descr. : Unreference a cube map, destroy if it is no longer referenced
// ------------------------------------------------------------------------------------------------
void TEX_CubeMap_Remove(SHORT _s_Index)
{
    if (!TEX_CubeMap_IsValidIndex(_s_Index))
    {
        ERR_OutputDebugString("[CubeMap] Error: TEX_CubeMap_Remove() - Invalid cube map index\n");
        return;
    }

    TEX_gst_CubeMapList.dst_CubeMaps[_s_Index].ul_NbReferences--;

    // Free only if the cube map is not referenced anymore
    if (TEX_gst_CubeMapList.dst_CubeMaps[_s_Index].ul_NbReferences == 0)
    {
        TEX_CubeMap_FreeCubeMap(_s_Index, TRUE);
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : TEX_CubeMap_GetInfo
// Params : _s_Index : Index of the cube map
// RetVal : Cube map information
// Descr. : Fetch a cube map information structure
// ------------------------------------------------------------------------------------------------
TEX_tdst_CubeMapInfo_* TEX_CubeMap_GetInfo(SHORT _s_Index)
{
    if (!TEX_CubeMap_IsValidIndex(_s_Index))
    {
        ERR_OutputDebugString("[CubeMap] Error: TEX_CubeMap_GetInfo() - Invalid cube map index\n");
        return NULL;
    }

    return &TEX_gst_CubeMapList.dst_CubeMaps[_s_Index];
}

// ------------------------------------------------------------------------------------------------
// Name   : TEX_CubeMap_GetKey
// Params : _s_Index : Index of the cube map
// RetVal : Key of the cube map
// Descr. : Return the key of a loaded cube map
// ------------------------------------------------------------------------------------------------
BIG_KEY TEX_CubeMap_GetKey(SHORT _s_Index)
{
    if (!TEX_CubeMap_IsValidIndex(_s_Index))
    {
        return BIG_C_InvalidKey;
    }

    return TEX_gst_CubeMapList.dst_CubeMaps[_s_Index].ul_Key;
}

// ------------------------------------------------------------------------------------------------
// Name   : TEX_CubeMap_CheckUsedTextures
// Params : _pc_UsedIndex : Texture usage flag buffer
// RetVal : None
// Descr. : Mark the 2D textures used by cube maps
// ------------------------------------------------------------------------------------------------
void TEX_CubeMap_CheckUsedTextures(CHAR* _pc_UsedIndex)
{
    // SC: Nothing to do since we are not using 2D textures for the cube maps anymore
}

// ------------------------------------------------------------------------------------------------
// Name   : TEX_CubeMap_FindCubeMap
// Params : _ul_Key : Key of the cube map
// RetVal : Index of the cube map
// Descr. : Find a cube map in the list
// ------------------------------------------------------------------------------------------------
SHORT TEX_CubeMap_FindCubeMap(BIG_KEY _ul_Key)
{
    if (_ul_Key == BIG_C_InvalidKey)
        return TEX_CubeMap_InvalidIndex;

    for (ULONG i = 0; i < TEX_gst_CubeMapList.ul_MaxNumberOfCubeMaps; ++i)
    {
        if (TEX_gst_CubeMapList.dst_CubeMaps[i].ul_Key == _ul_Key)
            return (SHORT)i;
    }

    return TEX_CubeMap_InvalidIndex;
}

// ------------------------------------------------------------------------------------------------
// Name   : TEX_CubeMap_IsValidIndex
// Params : _s_Index : Index of a cube map
// RetVal : Valid?
// Descr. : Check if a cube map index is valid
// ------------------------------------------------------------------------------------------------
BOOL TEX_CubeMap_IsValidIndex(SHORT _s_Index)
{
    if (_s_Index < 0)
        return FALSE;

    if (_s_Index >= (SHORT)TEX_gst_CubeMapList.ul_MaxNumberOfCubeMaps)
        return FALSE;

    if (TEX_gst_CubeMapList.dst_CubeMaps[_s_Index].ul_Key == BIG_C_InvalidKey)
        return FALSE;

    if (TEX_gst_CubeMapList.dst_CubeMaps[_s_Index].s_Index < 0)
        return FALSE;

    if (TEX_gst_CubeMapList.dst_CubeMaps[_s_Index].ul_NbReferences == 0)
        return FALSE;

    return TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : TEX_CubeMap_ReserveIndex
// Params : None
// RetVal : Cube map index
// Descr. : Reserve a cube map index
// ------------------------------------------------------------------------------------------------
SHORT TEX_CubeMap_ReserveIndex(void)
{
    SHORT s_Index;

    if (TEX_gst_CubeMapList.ul_NumberOfCubeMaps == TEX_gst_CubeMapList.ul_MaxNumberOfCubeMaps)
    {
        // Array is full, increase it's size

        s_Index = (SHORT)TEX_gst_CubeMapList.ul_NumberOfCubeMaps;

        TEX_gst_CubeMapList.ul_MaxNumberOfCubeMaps += TEX_CUBEMAP_REALLOC;

        TEX_gst_CubeMapList.dst_CubeMaps = (TEX_tdst_CubeMapInfo*)MEM_p_Realloc(TEX_gst_CubeMapList.dst_CubeMaps, 
                                                                                TEX_gst_CubeMapList.ul_MaxNumberOfCubeMaps * 
                                                                                    sizeof(TEX_tdst_CubeMapInfo));
    }
    else
    {
        // There is at least one free slot

        for (s_Index = 0; s_Index < (SHORT)TEX_gst_CubeMapList.ul_MaxNumberOfCubeMaps; ++s_Index)
        {
            if (TEX_gst_CubeMapList.dst_CubeMaps[s_Index].ul_Key == BIG_C_InvalidKey)
                break;
        }
    }

    TEX_gst_CubeMapList.ul_NumberOfCubeMaps++;

    return s_Index;
}

// ------------------------------------------------------------------------------------------------
// Name   : TEX_CubeMap_FreeCubeMap
// Params : _s_Index : Index of a cube map
// RetVal : None
// Descr. : Free|Unload a cube map
// ------------------------------------------------------------------------------------------------
void TEX_CubeMap_FreeCubeMap(SHORT _s_Index, BOOL _b_Shutdown)
{
#if defined(_DEBUG)
    ERR_OutputDebugString("[CubeMap] - Hardware unloaded [0x%08x], index %u\n", TEX_gst_CubeMapList.dst_CubeMaps[_s_Index].ul_Key, (ULONG)_s_Index);
#endif

    if (_b_Shutdown)
    {
        TEX_gst_CubeMapList.dst_CubeMaps[_s_Index].ul_Key          = BIG_C_InvalidKey;
        TEX_gst_CubeMapList.dst_CubeMaps[_s_Index].s_Index         = TEX_CubeMap_InvalidIndex;
        TEX_gst_CubeMapList.dst_CubeMaps[_s_Index].ul_NbReferences = 0;
    }

#if defined(_XENON_RENDER)
    if (GDI_b_IsXenonGraphics())
    {
        g_oXeTextureMgr.UnloadCubeMap(_s_Index);
    }
#endif

    TEX_gst_CubeMapList.dst_CubeMaps[_s_Index].b_HWLoaded = FALSE;
}

// ------------------------------------------------------------------------------------------------
// Name   : TEX_CubeMap_ReadFile
// Params : _ul_Key : Cube map file
//          s_Index : Index of the cube map
// RetVal : Success
// Descr. : Read an load a cube map from a file
// ------------------------------------------------------------------------------------------------
static BOOL TEX_CubeMap_ReadFile(BIG_KEY _ul_Key, SHORT s_Index)
{
    TEX_tdst_CubeMapInfo* pst_Info = &TEX_gst_CubeMapList.dst_CubeMaps[s_Index];
    ULONG ul_FileSize = 0;
    CHAR* pc_Buffer   = NULL;
    ULONG ul_Pos;

    ul_Pos = BIG_ul_SearchKeyToPos(_ul_Key);
    if (ul_Pos == BIG_C_InvalidIndex)
    {
#if defined(ACTIVE_EDITORS)
        CHAR sz_Msg[256];
        sprintf(sz_Msg, "Cube map file not found [%08x]", _ul_Key);
        ERR_X_Warning(0, sz_Msg, NULL);
#endif
        ERR_OutputDebugString("[CubeMap] Error: TEX_CubeMap_ReadFile() - Cube map file not found [0x%08x]\n", _ul_Key);
        return FALSE;
    }

    pc_Buffer = BIG_pc_ReadFileTmp(ul_Pos, &ul_FileSize);

    // SC: Mark the buffer as read for binarization
    CHAR* pcBufferTemp = pc_Buffer;
    LOA_ReadCharArray(&pcBufferTemp, NULL, ul_FileSize);

    // This is a .DDS file and we should use it as-is
#if defined(_XENON_RENDER)
    if (GDI_b_IsXenonGraphics())
    {
        g_oXeTextureMgr.LoadCubeMap(s_Index, pc_Buffer, ul_FileSize);
    }
#endif
    pst_Info->b_HWLoaded = TRUE;

#if defined(_DEBUG)
    ERR_OutputDebugString("[CubeMap] - Hardware loaded [0x%08x], index %u\n", _ul_Key, (ULONG)s_Index);
#endif

    return TRUE;
}

void TEX_CubeMap_HardwareLoad(void)
{
    s_bCanHWLoad = TRUE;

    for (ULONG i = 0; i < TEX_gst_CubeMapList.ul_NumberOfCubeMaps; ++i)
    {
        if (TEX_gst_CubeMapList.dst_CubeMaps[i].ul_Key != BIG_C_InvalidKey)
        {
            TEX_CubeMap_ReadFile(TEX_gst_CubeMapList.dst_CubeMaps[i].ul_Key, (SHORT)i);
        }
    }
}

void TEX_CubeMap_HardwareUnload(void)
{
    for (ULONG i = 0; i < TEX_gst_CubeMapList.ul_NumberOfCubeMaps; ++i)
    {
        if ((TEX_gst_CubeMapList.dst_CubeMaps[i].ul_Key != BIG_C_InvalidKey) &&
            (TEX_gst_CubeMapList.dst_CubeMaps[i].b_HWLoaded))
        {
            TEX_CubeMap_FreeCubeMap((SHORT)i, FALSE);
        }
    }

    s_bCanHWLoad = FALSE;
}

#endif // defined(TEX_USE_CUBEMAPS)
