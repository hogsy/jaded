/*$T MATSprite.h GC!1.71 02/21/00 16:38:00 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "BASe/BAStypes.h"

#ifndef __MATSPRITE_H__
#define __MATSPRITE_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

#define MAT_SpriteGen_KEY1                  0xC0DE1999
#define MAT_SpriteGen_KEY2                  0xC0DE2000

#define MAT_SPR_DisearpearWthTransparency   0x0001
#define MAT_SPR_DisearpearWthSize           0x0002
#define MAT_SPR_DisearpearWthDensity        0x0004
#define MAT_SPR_VertexAlphaIsSize           0x0008
#define MAT_SPR_VertexAlphaIsTransparency   0x0010
#define MAT_SPR_VertexAlphaIsDensity        0x0020
#define MAT_SPR_SizeFactorIsRealSize        0x0040
#define MAT_SPR_SortSprites                 0x0080
#define MAT_SPR_TableMapped                 0x0100

#define MAT_BumpMap_BufferIsRef             0x01

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct MAT_SPR_tdst_BumpMap_
{
	USHORT  Size;
	UCHAR   Semaphore;
    UCHAR   Flags;
	ULONG   ulBigKey;
	ULONG   *p_Pixels;
} MAT_SPR_tdst_BumpMap;

typedef struct MAT_SPR_tdst_BumpMapList_
{
    ULONG                   ul_Number;
    ULONG                   ul_Max;
    MAT_SPR_tdst_BumpMap    **dpst_BM;
} MAT_SPR_tdst_BumpMapList;
   
/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct  MAT_tdst_SpriteGen_
{
    ULONG					Identifier1;    /* Must be = to MAT_SpriteGen_KEY1 */
    ULONG					Identifier2;    /* Must be = to MAT_SpriteGen_KEY2 */
    ULONG           		TEXTURE_BIGKEY;
    unsigned short  		flags;
    unsigned short  		s_TextureIndex;
    float           		Size;
    float           		ZExtraction;
    ULONG           		bEnableTexture; /* True if 0x80000000 */
    float           		Noise;
    float           		SizeNoise;
    float           		MipMapCoef;
    float           		DistortionMax;
	ULONG					XYZSMap_BIGKEY;
	MAT_SPR_tdst_BumpMap	*p_BMap;		/* Pointer in engine mode ,  BIGKEY in bigfile mode */
	float           		fBumpFactor;	
    ULONG           		Undefined[2];
} MAT_tdst_SpriteGen;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

void                    MAT_RasterizeUV( struct GDI_tdst_DisplayData_ *, struct GEO_tdst_Object_ *, struct MAT_tdst_MTLevel_ *, struct GEO_tdst_ElementIndexedTriangles_ * );
MAT_tdst_SpriteGen      *MAT_pst_SpriteGen_Load( ULONG, char *, ULONG );
void                    MAT_SpriteGen_Close( void );

void                    MAT_BumpMap_Close( char );
MAT_SPR_tdst_BumpMap    *MAT_pst_BumpMap_Add( ULONG );
void                    MAT_BumpMap_Del( MAT_SPR_tdst_BumpMap * );


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __MATSPRITE_H__ */
