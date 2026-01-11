/*$T MATSprite_Load.c GC!1.71 02/11/00 10:39:43 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"

#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"

#include "BIGfiles/BIGread.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

#include "LINks/LINKstruct_reg.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKtoed.h"

#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#ifndef PSX2_TARGET
#ifndef _GAMECUBE
#ifdef ACTIVE_EDITORS
#include "MAD_loadsave/Sources/MAD_Struct_V0.h"
#endif
#endif
#endif
#include "MATerial/MATSingle.h"
#include "MATerial/MATmulti.h"
#include "MATerial/MATSprite.h"
#include "GRObject/GROstruct.h"
#include "TEXture/TEXprocedural.h"
#include "TEXture/TEXfile.h"

#include "TIMer/TIMdefs.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C" {
#endif

#ifdef ACTIVE_EDITORS
	extern ULONG	EDI_OUT_gl_ForceSetMode;
#endif

/*
 =======================================================================================================================
    Internal struct and constants
 =======================================================================================================================
 */

/* list of sprite generator loaded */
typedef struct MAT_tdst_SpriteGen_Load_
{
    ULONG                           ul_Key;
    MAT_tdst_SpriteGen              *pst_SG;
    struct MAT_tdst_SpriteGen_Load_ *pst_Next;
} MAT_tdst_SpriteGen_Load;

static MAT_tdst_SpriteGen_Load *MAT_SPRT_AllSprt = NULL;

/* list of bump map data loaded */
MAT_SPR_tdst_BumpMapList MAT_gst_BumpMapList = { 0, 0, NULL};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG SPR_LOA_ul_GetPO2(ULONG ul_Size)
{
    /*~~~~~~~~~*/
	ULONG RET;
    /*~~~~~~~~~*/
	RET = 0;
	while (((ULONG)1L << (RET + RET)) <= ul_Size) RET ++;
	return 1L << (RET - 1);
}


/*$4
 ***********************************************************************************************************************
    Bump map manager
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_BumpMap_Close( char _c_TheEnd )
{
    /*~~~~~~~~*/
    ULONG i;
    /*~~~~~~~~*/

    if ( !MAT_gst_BumpMapList.ul_Max ) return;

    for ( i = 0; i < MAT_gst_BumpMapList.ul_Number; i++)
    {
        if ( !(MAT_gst_BumpMapList.dpst_BM[ i ]->Flags & MAT_BumpMap_BufferIsRef) )
            MEM_Free( MAT_gst_BumpMapList.dpst_BM[ i ]->p_Pixels );
        MEM_Free( MAT_gst_BumpMapList.dpst_BM[ i ] );
    }

    if (_c_TheEnd )
    {
        MEM_Free( MAT_gst_BumpMapList.dpst_BM );
        L_memset( &MAT_gst_BumpMapList, 0, sizeof(MAT_gst_BumpMapList) );
    }
    else
        MAT_gst_BumpMapList.ul_Number = 0;

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAT_SPR_tdst_BumpMap *MAT_pst_BumpMap_Add( ULONG _ul_Key )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG	i, ul_Size, ul_Pos, ul_Length;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if ( !_ul_Key || (_ul_Key == BIG_C_InvalidKey) ) return NULL;

    // sinon ca plante
    return NULL;

    ul_Pos = BIG_ul_SearchKeyToPos(_ul_Key);
	if(ul_Pos == (ULONG) - 1) return NULL;

    for ( i = 0; i < MAT_gst_BumpMapList.ul_Number; i++)
    {
        if (MAT_gst_BumpMapList.dpst_BM[ i ]->ulBigKey == _ul_Key) 
        {
            MAT_gst_BumpMapList.dpst_BM[ i ]->Semaphore++;
            return MAT_gst_BumpMapList.dpst_BM[ i ];
        }
    }

    /* not found, add it */
    if (!MAT_gst_BumpMapList.ul_Max)
    {
        MAT_gst_BumpMapList.dpst_BM = (MAT_SPR_tdst_BumpMap **) MEM_p_Alloc( 4 );
        MAT_gst_BumpMapList.ul_Max++;
    }
    else if (MAT_gst_BumpMapList.ul_Max == MAT_gst_BumpMapList.ul_Number)
    {
        MAT_gst_BumpMapList.ul_Max++;
        MAT_gst_BumpMapList.dpst_BM = (MAT_SPR_tdst_BumpMap **) MEM_p_Realloc( MAT_gst_BumpMapList.dpst_BM, 4 * MAT_gst_BumpMapList.ul_Max);
    }

    i = MAT_gst_BumpMapList.ul_Number++;
    MAT_gst_BumpMapList.dpst_BM[ i ] = (MAT_SPR_tdst_BumpMap *) MEM_p_Alloc( sizeof( MAT_SPR_tdst_BumpMap ) );
    MAT_gst_BumpMapList.dpst_BM[ i ]->ulBigKey = _ul_Key;
    
    ul_Length = BIG_ul_GetLengthFile ( ul_Pos ); 
    ul_Size = SPR_LOA_ul_GetPO2( ul_Length );
    
    /* test for raw file */
    if ( ul_Length == ul_Size * ul_Size )
    {
        MAT_gst_BumpMapList.dpst_BM[ i ]->Flags = 0;
        MAT_gst_BumpMapList.dpst_BM[ i ]->Semaphore = 1;
        MAT_gst_BumpMapList.dpst_BM[ i ]->Size = (USHORT) (ul_Size >> 1L);

        MAT_gst_BumpMapList.dpst_BM[ i ]->p_Pixels = (ULONG*) MEM_p_Alloc ( ul_Length );
	    BIG_ul_ReadFile(ul_Pos, MAT_gst_BumpMapList.dpst_BM[ i ]->p_Pixels );
    }
    else
    {
        TEX_tdst_Data           *pst_Texture;
        TEX_tdst_File_Desc		st_Tex;
        int                     i_Type;
        void                    *p_Data;
        TEX_tdst_Procedural     *pst_Pro;
        SHORT                   w_Texture;

        pst_Texture = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, _ul_Key);
        if(pst_Texture)
            w_Texture = pst_Texture->w_Index;
        else
            w_Texture = TEX_w_List_AddTexture( &TEX_gst_GlobalList, _ul_Key, 1 );

        /* just hope that key is a .pro file key */
        TEX_l_File_GetInfoAndContent( _ul_Key, &st_Tex );
        i_Type = (int) st_Tex.st_Params.ul_Params[0];
        p_Data = (void *) st_Tex.st_Params.ul_Params[1];
        TEX_gst_GlobalList.dst_Texture[ w_Texture ].w_Height = st_Tex.uw_Height;
        TEX_gst_GlobalList.dst_Texture[ w_Texture ].w_Width = st_Tex.uw_Width;
        TEX_Procedural_Add( w_Texture, i_Type, 0, p_Data );
        pst_Pro = &TEX_gdst_Procedural[ TEX_gi_NbProcedural - 1 ];

        MAT_gst_BumpMapList.dpst_BM[ i ]->Flags = MAT_BumpMap_BufferIsRef;
        MAT_gst_BumpMapList.dpst_BM[ i ]->Semaphore = 1;
        if (pst_Pro->uw_Height > pst_Pro->uw_Width)
            MAT_gst_BumpMapList.dpst_BM[ i ]->Size = (USHORT) pst_Pro->uw_Width;
        else
            MAT_gst_BumpMapList.dpst_BM[ i ]->Size = (USHORT) pst_Pro->uw_Height;
        MAT_gst_BumpMapList.dpst_BM[ i ]->p_Pixels = (ULONG *) pst_Pro->pc_Buffer;
    }

    

    /*$F
    MAT_gst_BumpMapList.dpst_BM[ i ] = (MAT_SPR_tdst_BumpMap *) MEM_p_Alloc( sizeof( MAT_SPR_tdst_BumpMap ) );
    MAT_gst_BumpMapList.dpst_BM[ i ]->Flags = 0;
    MAT_gst_BumpMapList.dpst_BM[ i ]->Semaphore = 1;
    MAT_gst_BumpMapList.dpst_BM[ i ]->ulBigKey = _ul_Key;
    MAT_gst_BumpMapList.dpst_BM[ i ]->Size = (USHORT) SPR_LOA_ul_GetPO2(ul_Size >> 2L);

	MAT_gst_BumpMapList.dpst_BM[ i ]->p_Pixels = (ULONG*) MEM_p_Alloc ( ul_Size );
	BIG_ul_ReadFile(ul_Pos, MAT_gst_BumpMapList.dpst_BM[ i ]->p_Pixels );
    */
	
    return MAT_gst_BumpMapList.dpst_BM[ i ];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_BumpMap_Del( MAT_SPR_tdst_BumpMap *_pst_BM )
{
    /*~~~~~~~~~*/
    ULONG i;
    /*~~~~~~~~~*/

    _pst_BM->Semaphore--;
	if ( _pst_BM->Semaphore ) return;
	
    if ( !(_pst_BM->Flags & MAT_BumpMap_BufferIsRef ) )
	    MEM_Free( _pst_BM->p_Pixels );
    MEM_Free( _pst_BM );
	
    for ( i = 0; i < MAT_gst_BumpMapList.ul_Number; i++)
    {
        if (MAT_gst_BumpMapList.dpst_BM[i] == _pst_BM )
        {
            MAT_gst_BumpMapList.ul_Number--;
            MAT_gst_BumpMapList.dpst_BM[ i ] = MAT_gst_BumpMapList.dpst_BM[ MAT_gst_BumpMapList.ul_Number ];
            break;
        }
    }
}



/*$4
 ***********************************************************************************************************************
    Sprite generator functions 
 ***********************************************************************************************************************
 */


#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_SpriteGen_Save(MAT_tdst_SpriteGen *_pst_SG)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char                    sz_Path[BIG_C_MaxLenPath];
    ULONG                   ul_Value;
    TEX_tdst_File_Params    st_Params;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ERR_X_Assert( (_pst_SG->Identifier1 == MAT_SpriteGen_KEY1) && (_pst_SG->Identifier2 == MAT_SpriteGen_KEY2) );

    ul_Value = LOA_ul_SearchKeyWithAddress((ULONG) _pst_SG);
    if(ul_Value != BIG_C_InvalidKey)
    {
        ul_Value = BIG_ul_SearchKeyToFat(ul_Value);
        if(ul_Value != BIG_C_InvalidIndex)
        {
            BIG_ComputeFullName(BIG_ParentFile(ul_Value), sz_Path);
            SAV_Begin(sz_Path, BIG_NameFile(ul_Value));

            SAV_Buffer(_pst_SG, sizeof(MAT_tdst_SpriteGen));
            
            TEX_File_InitParams( &st_Params );
            st_Params.uc_Type = TEX_FP_SprFile;
            SAV_Buffer(&st_Params, sizeof( TEX_tdst_File_Params ) );
            
            SAV_ul_End();
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG MAT_pst_SaveAllSpritesGenerator(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAT_tdst_SpriteGen_Load    *pp_SprtLst;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pp_SprtLst = MAT_SPRT_AllSprt;
    while(pp_SprtLst != NULL)
    {
        MAT_SpriteGen_Save(pp_SprtLst->pst_SG);
        pp_SprtLst = pp_SprtLst->pst_Next;
    }

    return 0;
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAT_tdst_SpriteGen *MAT_pst_SpriteGen_Load( ULONG _ul_Key, char *_pc_Buf, ULONG _ul_Size )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAT_tdst_SpriteGen       *pst_SG;
    TEX_tdst_Data            *pst_Texture;
    ULONG                    ul_Key;
    MAT_tdst_SpriteGen_Load  *p_SprtLst;
    MAT_SPR_tdst_BumpMap     *p_OldpBmp;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* search if already in list */
    p_SprtLst = MAT_SPRT_AllSprt;
    pst_SG = NULL;
    while ( p_SprtLst )
    {
        if (p_SprtLst->ul_Key == _ul_Key)
        {
            pst_SG = p_SprtLst->pst_SG;

            pst_Texture = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, pst_SG->TEXTURE_BIGKEY);
            if(pst_Texture)
                pst_SG->s_TextureIndex = pst_Texture->w_Index;
            else
                pst_SG->s_TextureIndex = TEX_w_List_AddTexture( &TEX_gst_GlobalList, pst_SG->TEXTURE_BIGKEY, 1 );

			if (p_SprtLst->pst_SG->p_BMap)
			{
				p_OldpBmp = p_SprtLst->pst_SG->p_BMap;
				p_SprtLst->pst_SG->p_BMap = MAT_pst_BumpMap_Add( pst_SG->XYZSMap_BIGKEY );
				if ( p_OldpBmp && (p_SprtLst->pst_SG->p_BMap != p_OldpBmp) )
					MAT_BumpMap_Del( p_OldpBmp );
			}
            return pst_SG;
        }
        p_SprtLst = p_SprtLst->pst_Next;
    }
    
    pst_SG = (MAT_tdst_SpriteGen*) MEM_p_Alloc(sizeof(MAT_tdst_SpriteGen));
    L_memset((void *) pst_SG, 0, sizeof(MAT_tdst_SpriteGen));

    /* Add sprite in list */
    p_SprtLst = (MAT_tdst_SpriteGen_Load *) MEM_p_Alloc(sizeof(MAT_tdst_SpriteGen_Load));
    p_SprtLst->pst_Next = MAT_SPRT_AllSprt;
    p_SprtLst->pst_SG = pst_SG;
    p_SprtLst->ul_Key = _ul_Key;
    MAT_SPRT_AllSprt = p_SprtLst;

    if( _ul_Size == 0)
    {
        pst_SG->Identifier1 = MAT_SpriteGen_KEY1;
        pst_SG->Identifier2 = MAT_SpriteGen_KEY2;
        pst_SG->TEXTURE_BIGKEY = BIG_C_InvalidIndex;
        pst_SG->Size = 1.0f;
        pst_SG->ZExtraction = 0.0f;
        pst_SG->s_TextureIndex = (unsigned short)-1;
        pst_SG->MipMapCoef = 1.0f;
		pst_SG->bEnableTexture = 0x80000000;
        pst_SG->DistortionMax = 1.2f;
		pst_SG->fBumpFactor = 1.0f;
		pst_SG->XYZSMap_BIGKEY = BIG_C_InvalidIndex;
    }
    else
    {
		pst_SG->Identifier1 = LOA_ReadULong(&_pc_Buf);
		pst_SG->Identifier2 = LOA_ReadULong(&_pc_Buf);
		pst_SG->TEXTURE_BIGKEY = LOA_ReadULong(&_pc_Buf);
		pst_SG->flags = LOA_ReadUShort(&_pc_Buf);
		pst_SG->s_TextureIndex = LOA_ReadUShort(&_pc_Buf);
		pst_SG->Size = LOA_ReadFloat(&_pc_Buf);
		pst_SG->ZExtraction = LOA_ReadFloat(&_pc_Buf);
		pst_SG->bEnableTexture = LOA_ReadULong(&_pc_Buf);
		pst_SG->Noise = LOA_ReadFloat(&_pc_Buf);
		pst_SG->SizeNoise = LOA_ReadFloat(&_pc_Buf);
		pst_SG->MipMapCoef = LOA_ReadFloat(&_pc_Buf);
		pst_SG->DistortionMax = LOA_ReadFloat(&_pc_Buf);
		pst_SG->XYZSMap_BIGKEY = LOA_ReadULong(&_pc_Buf);
		*((ULONG*)&pst_SG->p_BMap) = LOA_ReadULong(&_pc_Buf); // Pointer in engine mode ,  BIGKEY in bigfile mode
		pst_SG->fBumpFactor = LOA_ReadFloat(&_pc_Buf);
		pst_SG->Undefined[0] = LOA_ReadULong(&_pc_Buf);
		pst_SG->Undefined[1] = LOA_ReadULong(&_pc_Buf);

        ul_Key = pst_SG->TEXTURE_BIGKEY;
        
        if( ul_Key == BIG_C_InvalidIndex )
        {
            pst_SG->s_TextureIndex = (unsigned short) -1;
        }
        else
        {
            pst_Texture = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, ul_Key);
            if(pst_Texture)
                pst_SG->s_TextureIndex = pst_Texture->w_Index;
            else
                pst_SG->s_TextureIndex = TEX_w_List_AddTexture( &TEX_gst_GlobalList, ul_Key, 1 );
        }
        pst_SG->p_BMap = MAT_pst_BumpMap_Add(pst_SG->XYZSMap_BIGKEY);
    }

#ifdef ACTIVE_EDITORS
    {
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        BIG_INDEX   ul_Fat;
        char        asz_Path[BIG_C_MaxLenPath];
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        ul_Fat = BIG_ul_SearchKeyToFat( _ul_Key );
        BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);
        LINK_RegisterPointer(pst_SG, LINK_C_ENG_SpriteGen, BIG_NameFile(ul_Fat), asz_Path);
		LOA_AddAddress(ul_Fat, pst_SG);
    }
#endif

    return pst_SG;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_SpriteGen_Close(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAT_tdst_SpriteGen_Load    *SprLst, *SprLstLocal;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    SprLst = MAT_SPRT_AllSprt;
    while(SprLst)
    {
        SprLstLocal = SprLst;
        SprLst = SprLst->pst_Next;
        if (SprLstLocal->pst_SG)
		{
			if (SprLstLocal->pst_SG->p_BMap)
                MAT_BumpMap_Del( SprLstLocal->pst_SG->p_BMap );
            MEM_Free( SprLstLocal->pst_SG);
		}
        MEM_Free(SprLstLocal);
    }
    MAT_SPRT_AllSprt = NULL;

    MAT_BumpMap_Close( 1 );
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
