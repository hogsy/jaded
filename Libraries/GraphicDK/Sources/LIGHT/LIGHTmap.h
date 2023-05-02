#ifndef __LIGHTMAP_H__
#define __LIGHTMAP_H__

#include "Engine/Sources/WORld/WORstruct.h"

#include "TEXture/TEXstruct.h"
#include "TEXture/TEXhardwareload.h"
#include "TEXture/TEXfile.h"

#define LIGHT_Csz_LightmapsFile "LMData.LM1"

#ifdef ACTIVE_EDITORS
    #include "BIGfiles/BIGdefs.h"

    void	LIGHT_Lightmaps_CleanUnusedLightmapTextures(void);
#endif

#ifdef __cplusplus
    extern "C"
    {
#endif

	    extern LIGHT_tdst_LightmapPageInfo *LIGHT_gFirstLightmapPage;
	    //extern ULONG	LIGHT_g_NbOfLightMappedObjects;

	    LIGHT_tdst_LightmapPageInfo *LIGHT_Lightmaps_GetPointerForKey( ULONG _ulKey, bool _bCreateIfNeeded, bool _bAddRefIfPresent, bool _bIsNotaKey );
	    void LIGHT_Lightmaps_RemoveRefLightmapPage( LIGHT_tdst_LightmapPageInfo * );

	    void LIGHT_Lightmaps_UsedTexture( char *_pcUsedTextures );

#if defined( ACTIVE_EDITORS )
	    void LIGHT_Lightmaps_DestroyGAOLightmaps( OBJ_tdst_GameObject *_pst_GO );
#endif

#ifdef __cplusplus
    };
#endif

#endif // __LIGHTMAP_H__ 