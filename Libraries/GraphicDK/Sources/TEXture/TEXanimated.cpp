/*$T TEXprocedural.c GC! 1.081 06/27/00 11:11:37 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/BAStypes.h"
#include "TIMer/TIMdefs.h"

#include "BIGfiles/BIGread.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"

#include "LINks/LINKstruct_reg.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKtoed.h"

#include "TEXture/TEXfile.h"
#include "TEXture/TEXstruct.h"
#include "TEXture/TEXanimated.h"
#include "GDInterface/GDInterface.h"

#include "INOut/INOfile.h"

#if defined(_XENON_RENDER)
#include "XenonGraphics/XeGDInterface.h"
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

#define TEXANIM_Granularity 16


int                             TEX_gi_NbAnimatedMax = 0;
int								TEX_gi_NbAnimated = 0;
TEX_tdst_Animated               *TEX_gdst_Animated = NULL;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Anim_Init(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Anim_Reinit( GDI_tdst_DisplayData *_pst_DD )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	TEX_tdst_Animated   *pst_Ani;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(TEX_gi_NbAnimated)
	{
		pst_Ani = TEX_gdst_Animated;

		for(i = 0; i < TEX_gi_NbAnimated; i++, pst_Ani++)
		{
            if (pst_Ani->uc_Number)
                MEM_Free( pst_Ani->dst_Tex );
		}

        TEX_gi_NbAnimatedMax = 0;
		TEX_gi_NbAnimated = 0;
		MEM_Free(TEX_gdst_Animated);
		TEX_gdst_Animated = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
TEX_tdst_Animated *TEX_pst_Anim_Add(SHORT _w_Index, UCHAR _uc_Number, USHORT w_Flags, void *_p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_Animated   *pst_Ani;
	TEX_tdst_Data		*pst_Tex;
    ULONG               ul_Size;
    SHORT               w_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(TEX_gi_NbAnimated == TEX_gi_NbAnimatedMax)
	{
        TEX_gi_NbAnimatedMax += TEXANIM_Granularity;
        ul_Size = TEX_gi_NbAnimatedMax * sizeof(TEX_tdst_Animated);
        if (TEX_gi_NbAnimatedMax == TEXANIM_Granularity)
		    TEX_gdst_Animated = (TEX_tdst_Animated*) MEM_p_Alloc( ul_Size );
        else
            TEX_gdst_Animated = (TEX_tdst_Animated*) MEM_p_Realloc( TEX_gdst_Animated, ul_Size );
	}

	pst_Ani = &TEX_gdst_Animated[TEX_gi_NbAnimated++];
    pst_Ani->w_Index = _w_Index;
    pst_Ani->uw_Flags = w_Flags;
    pst_Ani->uc_Requested = 0;
    pst_Ani->uc_AniFlags = 0;
    
    pst_Ani->uc_Number = _uc_Number;
    pst_Ani->dst_Tex = (TEX_tdst_AnimTex *) _p_Data;
    
    if (pst_Ani->dst_Tex)
    {
        pst_Ani->uc_Current = 0;
        pst_Ani->w_Counter = pst_Ani->dst_Tex->w_Time;
        w_Index = pst_Ani->dst_Tex->w_Index;
    }
    else
    {
        pst_Ani->uc_Current = -1;
        pst_Ani->w_Counter = 0;
        w_Index = -1;
    }
	
    pst_Tex = TEX_gst_GlobalList.dst_Texture + _w_Index;
    pst_Tex->w_Height = w_Index;

#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		BIG_INDEX	ul_Fat;
		char		asz_Path[BIG_C_MaxLenPath];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ul_Fat = BIG_ul_SearchKeyToFat(pst_Tex->ul_Key);
		BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);
		LINK_RegisterPointer(pst_Ani, LINK_C_TEX_Animated, BIG_NameFile(ul_Fat), asz_Path);
		LINK_UpdatePointers();
		LOA_AddAddress(ul_Fat, pst_Ani);
	}

#endif

    return pst_Ani;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Anim_AfterLoad( GDI_tdst_DisplayData *_pst_DD )
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
TEX_tdst_Animated *TEX_pst_Anim_Get( SHORT _w_Index )
{
    int i;

    for ( i = 0; i < TEX_gi_NbAnimated; i++)
    {
        if (TEX_gdst_Animated[ i ].w_Index == _w_Index)
            return &TEX_gdst_Animated[ i ];
    }
    return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef PSX2_TARGET
extern void GSP_SetAnimatedTexture(u_int ulTextureAntIndex , u_int ulTextureToSet);
#elif defined(_GAMECUBE)
extern void GXI_SetAnimatedTexture(u32 ulTextureAntIndex , u32 ulTextureToSet);
#endif
 
void TEX_Anim_Update(GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    static float                            sf_TimeLeft = 0;
	TEX_tdst_Animated						*pst_Ani;
	int										i, n, update;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    sf_TimeLeft += TIM_gf_dt;
    n = (int) (sf_TimeLeft * 60.0f);
    sf_TimeLeft -= n * (1.0f / 60.0f);

	pst_Ani = TEX_gdst_Animated;

    for(i = 0; i < TEX_gi_NbAnimated; i++, pst_Ani++)
	{
        if (!pst_Ani->uc_Number) continue;

        update = 0;
        if ( ( pst_Ani->uc_AniFlags & TEXANI_Flags_UseRequested ) && (pst_Ani->uc_Requested != pst_Ani->uc_Current) )
        {
            pst_Ani->uc_Requested %= pst_Ani->uc_Number;
            pst_Ani->uc_Current = pst_Ani->uc_Requested;
            update = 1;
        }
        else if ( !( pst_Ani->uc_AniFlags & TEXANI_Flags_Frozen ) )
        {
            pst_Ani->w_Counter -= n;
            if ( pst_Ani->w_Counter < 0 )
            {
                pst_Ani->uc_Current++;
                pst_Ani->uc_Current %= pst_Ani->uc_Number;
                update = 1;
            }
        }

		if ( update )
        {
            pst_Ani->w_Counter = pst_Ani->dst_Tex[ pst_Ani->uc_Current ].w_Time;

            TEX_gst_GlobalList.dst_Texture[ pst_Ani->w_Index ].w_Height = pst_Ani->dst_Tex[ pst_Ani->uc_Current].w_Index;
#ifdef PSX2_TARGET
			GSP_SetAnimatedTexture(pst_Ani->w_Index , pst_Ani->dst_Tex[ pst_Ani->uc_Current].w_Index);
#elif defined(_GAMECUBE)
			GXI_SetAnimatedTexture(pst_Ani->w_Index , pst_Ani->dst_Tex[ pst_Ani->uc_Current].w_Index);
#elif defined(_XENON_RENDER)
            Xe_SetAnimatedTexture((LONG)pst_Ani->w_Index, (LONG)pst_Ani->dst_Tex[pst_Ani->uc_Current].w_Index);
#endif
        }
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
void TEX_Anim_Save(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_Animated   *pst_Ani;
	int					i, tex;
	char				sz_Path[BIG_C_MaxLenPath];
	ULONG				ul_Value;
	LONG                l_Length;
    TEX_tdst_File_Params st_Params;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Ani = TEX_gdst_Animated;
	for(i = 0; i < TEX_gi_NbAnimated; i++, pst_Ani++)
	{
		ul_Value = LOA_ul_SearchKeyWithAddress((ULONG) pst_Ani);
		if(ul_Value == BIG_C_InvalidKey) continue;
		ul_Value = BIG_ul_SearchKeyToFat(ul_Value);
		if(ul_Value == BIG_C_InvalidIndex) continue;

		BIG_ComputeFullName(BIG_ParentFile(ul_Value), sz_Path);
		SAV_Begin(sz_Path, BIG_NameFile(ul_Value));


		l_Length = 8 + (((int) pst_Ani->uc_Number) * 8);
		SAV_Buffer(&l_Length, 4);
		SAV_Buffer(&pst_Ani->uw_Flags, 2);
        SAV_Buffer(&pst_Ani->uc_Number, 1 );
        SAV_Buffer(&pst_Ani->uc_Number, 1 );

        for (tex = 0; tex < (int) pst_Ani->uc_Number; tex++)
        {
            SAV_Buffer( &pst_Ani->dst_Tex[tex].ul_Key, 4 );
            SAV_Buffer( &pst_Ani->dst_Tex[tex].w_Time, 2 );
            SAV_Buffer( &pst_Ani->dst_Tex[tex].w_Index, 2 );
        }

        TEX_File_InitParams( &st_Params );
        st_Params.uw_Flags = TEX_FP_QualityVeryHigh;
        st_Params.uc_Type = TEX_FP_AniFile;
        st_Params.uw_Height = 0;
        st_Params.uw_Width = 0;

        SAV_Buffer( &st_Params, sizeof(TEX_tdst_File_Params) );

		SAV_ul_End();
	}
}
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
