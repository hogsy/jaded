/*$T TEXmemory.c GC!1.40 09/29/99 10:13:35 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "TEXture/TEXmemory.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/BAStypes.h"
#include "TIMer/TIMdefs.h"

#ifdef TEX_USEMEMORYCACHE


#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif
/*$4
 ***************************************************************************************************
    Globals
 ***************************************************************************************************
 */

TEX_tdst_Memory             TEX_gst_Memory;

/*$4
 ***************************************************************************************************
    Private Function header
 ***************************************************************************************************
 */

void                        TEX_Memory_FreeTillBelow(ULONG);
TEX_tdst_OneTextureInMemory *TEX_pst_Memory_GetEmpty(void);
TEX_tdst_OneTextureInMemory *TEX_pst_Memory_FindTextureInMemory(ULONG);
TEX_tdst_OneTextureInMemory *TEX_pst_Memory_FindOlder(void);

/*$4
 ***************************************************************************************************
    Public functions
 ***************************************************************************************************
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
void TEX_Memory_Init(ULONG _ul_MaxMemory)
{
    L_memset(&TEX_gst_Memory, 0, sizeof(TEX_tdst_Memory));
    TEX_gst_Memory.ul_MaxMemory = _ul_MaxMemory;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void TEX_Memory_Close(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEX_tdst_OneTextureInMemory *pst_Texture, *pst_LastTexture;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(TEX_gst_Memory.ul_MaxTextures)
    {
        pst_Texture = TEX_gst_Memory.dst_Texture;
        pst_LastTexture = pst_Texture + TEX_gst_Memory.ul_MaxTextures;
        for(; pst_Texture < pst_LastTexture; pst_Texture++)
        {
            if(pst_Texture->ul_Key != BIG_C_InvalidKey)
                TEX_File_FreeDescription(&pst_Texture->st_Texture);
        }

        L_free(TEX_gst_Memory.dst_Texture);
        TEX_Memory_Init(0);
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
LONG TEX_l_Memory_AddTexture(BIG_KEY _ul_Key, TEX_tdst_File_Desc *_pst_TexDesc)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG               ul_MemoryNeeded;
    TEX_tdst_OneTextureInMemory *pst_Texture;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Compute memory taken by texture */
    ul_MemoryNeeded = _pst_TexDesc->uw_Width *
        _pst_TexDesc->uw_Height *
        (_pst_TexDesc->uc_BPP >> 3);
    ul_MemoryNeeded += _pst_TexDesc->uw_PaletteLength * (_pst_TexDesc->uc_PaletteBPC >> 3);

    /* Is there enough memory ? */
    if(ul_MemoryNeeded > TEX_gst_Memory.ul_MaxMemory) return 0;

    if(TEX_gst_Memory.ul_MemoryTaken + ul_MemoryNeeded > TEX_gst_Memory.ul_MaxMemory)
    {
        TEX_Memory_FreeTillBelow(TEX_gst_Memory.ul_MaxMemory - ul_MemoryNeeded);
        if(TEX_gst_Memory.ul_MemoryTaken + ul_MemoryNeeded > TEX_gst_Memory.ul_MaxMemory) return 0;
    }

    /* Search for texture to fill */
    pst_Texture = TEX_pst_Memory_GetEmpty();
    if(pst_Texture == NULL) return 0;

    /* Update texture in memory */
    pst_Texture->f_Time = TIM_f_Clock_TrueRead();
    pst_Texture->ul_Key = _ul_Key;
    L_memcpy(&pst_Texture->st_Texture, _pst_TexDesc, sizeof(TEX_tdst_File_Desc));
    TEX_gst_Memory.ul_MemoryTaken += ul_MemoryNeeded;
    return 1;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
LONG TEX_l_Memory_DeleteTexture(BIG_KEY _ul_Key)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG               ul_MemoryTaken;
    TEX_tdst_OneTextureInMemory *pst_Texture;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Find texture */
    if (_ul_Key == BIG_C_InvalidKey) return 0;
    pst_Texture = TEX_pst_Memory_FindTextureInMemory(_ul_Key);
    if(pst_Texture == NULL) return 0;

    ul_MemoryTaken = pst_Texture->st_Texture.uw_Width * pst_Texture->st_Texture.uw_Height *
        (pst_Texture->st_Texture.uc_BPP >> 3);
    ul_MemoryTaken += pst_Texture->st_Texture.uw_PaletteLength *
        (pst_Texture->st_Texture.uc_PaletteBPC >> 3);
    TEX_File_FreeDescription(&pst_Texture->st_Texture);
    pst_Texture->ul_Key = BIG_C_InvalidKey;
    TEX_gst_Memory.ul_MaxMemory -= ul_MemoryTaken;
    return 1;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
TEX_tdst_File_Desc *TEX_pst_Memory_FindTexture(BIG_KEY _ul_Key)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEX_tdst_OneTextureInMemory *pst_Texture;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Texture = TEX_pst_Memory_FindTextureInMemory(_ul_Key);
    return((pst_Texture == NULL) ? NULL : &pst_Texture->st_Texture);
}

/*$4
 ***************************************************************************************************
    Private function
 ***************************************************************************************************
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
TEX_tdst_OneTextureInMemory *TEX_pst_Memory_FindTextureInMemory(ULONG _ul_Key)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEX_tdst_OneTextureInMemory *pst_Texture, *pst_LastTexture;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(TEX_gst_Memory.ul_MaxTextures == 0) return NULL;

    pst_Texture = TEX_gst_Memory.dst_Texture;
    pst_LastTexture = pst_Texture + TEX_gst_Memory.ul_MaxTextures;
    for(; pst_Texture < pst_LastTexture; pst_Texture++)
    {
        if(pst_Texture->ul_Key == _ul_Key) return pst_Texture;
    }

    return NULL;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void TEX_Memory_FreeTillBelow(ULONG _ul_Memory)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG               ul_MemoryTaken;
    TEX_tdst_OneTextureInMemory *pst_Texture;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    while(TEX_gst_Memory.ul_MaxMemory > _ul_Memory)
    {
        pst_Texture = TEX_pst_Memory_FindOlder();
        if(pst_Texture == NULL) return;

        ul_MemoryTaken = pst_Texture->st_Texture.uw_Width * pst_Texture->st_Texture.uw_Height *
            (pst_Texture->st_Texture.uc_BPP >> 3);
        ul_MemoryTaken += pst_Texture->st_Texture.uw_PaletteLength *
            (pst_Texture->st_Texture.uc_PaletteBPC >> 3);
        TEX_File_FreeDescription(&pst_Texture->st_Texture);
        pst_Texture->ul_Key = BIG_C_InvalidKey;
        TEX_gst_Memory.ul_MaxMemory -= ul_MemoryTaken;
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
TEX_tdst_OneTextureInMemory *TEX_pst_Memory_GetEmpty(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEX_tdst_OneTextureInMemory *pst_Texture, *pst_Last;
    ULONG               ul_Size;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Texture = TEX_pst_Memory_FindTextureInMemory(BIG_C_InvalidKey);
    if(pst_Texture) return pst_Texture;

    /* No more empty texture, add some */
    ul_Size = (TEX_gst_Memory.ul_MaxTextures + 50) * sizeof(TEX_tdst_OneTextureInMemory);
    if(TEX_gst_Memory.ul_MaxTextures == 0)
        TEX_gst_Memory.dst_Texture = (TEX_tdst_OneTextureInMemory *) L_malloc(ul_Size);
    else
    {
        TEX_gst_Memory.dst_Texture = (TEX_tdst_OneTextureInMemory *) L_realloc
            (
                TEX_gst_Memory.dst_Texture,
                ul_Size
            );
    }

    /* Init new added texture */
    pst_Texture = TEX_gst_Memory.dst_Texture + TEX_gst_Memory.ul_MaxTextures;
    pst_Last = pst_Texture + 50;
    for(; pst_Texture < pst_Last; pst_Texture++)
        pst_Texture->ul_Key = BIG_C_InvalidKey;

    TEX_gst_Memory.ul_MaxTextures += 50;
    return TEX_pst_Memory_FindTextureInMemory(BIG_C_InvalidKey);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
TEX_tdst_OneTextureInMemory *TEX_pst_Memory_FindOlder(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEX_tdst_OneTextureInMemory *pst_Texture, *pst_LastTexture, *pst_Find = NULL;
    float                       f_Time=0;
    char                        c_First = 1;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(TEX_gst_Memory.ul_MaxTextures == 0) return NULL;

    pst_Texture = TEX_gst_Memory.dst_Texture;
    pst_LastTexture = pst_Texture + TEX_gst_Memory.ul_MaxTextures;
    for(; pst_Texture < pst_LastTexture; pst_Texture++)
    {
        if(pst_Texture->ul_Key != BIG_C_InvalidKey)
        {
            if(c_First)
            {
                c_First = 0;
                pst_Find = pst_Texture;
                f_Time = pst_Texture->f_Time;
            }
            else if(pst_Texture->f_Time < f_Time)
            {
                pst_Find = pst_Texture;
                f_Time = pst_Texture->f_Time;
            }
        }
    }

    return pst_Find;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

#endif /*TEX_USEMEMORYCACHE*/