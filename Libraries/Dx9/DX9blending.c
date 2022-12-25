// Dx9blending.c


#include "Dx9blending.h"
#include "Dx9renderstate.h"
#include "Dx9samplerstate.h"
#include "Dx9texturestagestate.h"
#include "GDInterface/GDInterface.h"


/************************************************************************************************************************
    Private Function declaration
 ************************************************************************************************************************/

void	Dx9_SetCullMode( void );
void	Dx9_SetColorWrite( ULONG Flag );
void	Dx9_SetAlphaTest( ULONG blendingMode, ULONG Flag );
void	Dx9_SetAlphaBlend( ULONG blendingMode, ULONG Flag );
void	Dx9_SetFog( ULONG blendingMode );
void	Dx9_SetWireframe( DWORD wireframeEnable );

/************************************************************************************************************************
    Public Function
 ************************************************************************************************************************/

void	Dx9_SetTextureBlending( ULONG textureId, ULONG blendingMode, ULONG additionalFlags )
{
	ULONG					Delta;
	ULONG					Flag;

	Delta = GDI_gpst_CurDD->LastBlendingMode ^ blendingMode;
	Flag = MAT_GET_FLAG( blendingMode );

	Dx9_SetWireframe( !( GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_NotWired ) );

    Dx9_RS_ZEnable( ( GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_ZTest ) ? D3DZB_TRUE : D3DZB_FALSE );
	Dx9_RS_ZFunc( (Flag & MAT_Cul_Flag_ZEqual) ? D3DCMP_EQUAL : D3DCMP_LESSEQUAL );
    Dx9_RS_ZWriteEnable( (Flag & MAT_Cul_Flag_NoZWrite) ? FALSE : TRUE );

	Dx9_SetCullMode( );

	Dx9_TSS_UseTexture( 0, textureId );
    
	Dx9_SS_AddressU( 0, ( Flag & MAT_Cul_Flag_TileU ) ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP );
	Dx9_SS_AddressV( 0, ( Flag & MAT_Cul_Flag_TileV ) ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP );
	Dx9_SS_MinFilter( 0, ( Flag & MAT_Cul_Flag_Bilinear ) ? D3DTEXF_LINEAR : D3DTEXF_POINT );
	Dx9_SS_MagFilter( 0, ( Flag & MAT_Cul_Flag_Bilinear ) ? D3DTEXF_LINEAR : D3DTEXF_POINT );
	Dx9_SS_MipFilter( 0, ( Flag & MAT_Cul_Flag_Trilinear ) ? D3DTEXF_LINEAR : D3DTEXF_POINT );

	Dx9_SetColorWrite( Flag );

	Dx9_SetAlphaTest( blendingMode, Flag );

	Dx9_SetAlphaBlend( blendingMode, Flag );

	Dx9_SetFog( blendingMode );

	GDI_gpst_CurDD->LastTextureUsed = textureId;
	GDI_gpst_CurDD->LastBlendingMode = blendingMode;

	(void) additionalFlags;
}


/************************************************************************************************************************
    Private Functions
 ************************************************************************************************************************/

////////////////////
__inline void	Dx9_SetCullMode( void )
{
	if( GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_TestBackFace )
	{
		if( GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_NotInvertBF )
			Dx9_RS_CullMode( D3DCULL_CW );
		else
			Dx9_RS_CullMode( D3DCULL_CCW );
	}
	else
	{
		Dx9_RS_CullMode( D3DCULL_NONE );
	}
}

////////////////////
__inline void	Dx9_SetColorWrite( ULONG Flag )
{
	DWORD value = 0;

	if ( !( Flag & MAT_Cul_Flag_HideAlpha ) )
		value |= D3DCOLORWRITEENABLE_ALPHA;

	if ( !( Flag & MAT_Cul_Flag_HideColor ) )
		value |= D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE;

	Dx9_RS_ColorWriteEnable( value );
}

////////////////////
__inline void	Dx9_SetAlphaTest( ULONG blendingMode, ULONG Flag )
{
	if( Flag & MAT_Cul_Flag_AlphaTest )
	{
		Dx9_RS_AlphaTestEnable( TRUE );
		if( Flag & MAT_Cul_Flag_InvertAlpha )
			Dx9_RS_AlphaFunc( D3DCMP_LESS );
		else
			Dx9_RS_AlphaFunc( D3DCMP_GREATER );

		Dx9_RS_AlphaRef( MAT_GET_AlphaTresh( blendingMode ) );
	}
	else
	{
		Dx9_RS_AlphaTestEnable( FALSE );
	}

	if( Flag & MAT_Cul_Flag_UseLocalAlpha )
		gDx9SpecificData.ulColorOr = 0xff000000;
	else
		gDx9SpecificData.ulColorOr = 0;
}

////////////////////
__inline void	Dx9_SetAlphaBlend( ULONG blendingMode, ULONG Flag )
{
	switch( MAT_GET_Blending( blendingMode ) )
	{
	case MAT_Cc_Op_Copy:
	case MAT_Cc_Op_Glow:
		Dx9_RS_AlphaBlendEnable( FALSE );
		Dx9_RS_SrcBlend( D3DBLEND_ONE );
		Dx9_RS_DestBlend( D3DBLEND_ZERO );
		break;

	case MAT_Cc_Op_Alpha:
		Dx9_RS_AlphaBlendEnable( TRUE );
        if(Flag & MAT_Cul_Flag_InvertAlpha)
        {
			Dx9_RS_SrcBlend( D3DBLEND_INVSRCALPHA );
			Dx9_RS_DestBlend( D3DBLEND_SRCALPHA );
        }
        else
        {
			Dx9_RS_SrcBlend( D3DBLEND_SRCALPHA );
			Dx9_RS_DestBlend( D3DBLEND_INVSRCALPHA );
        }
		break;

	case MAT_Cc_Op_AlphaPremult:
		Dx9_RS_AlphaBlendEnable( TRUE );
		Dx9_RS_SrcBlend( D3DBLEND_ONE );
        if(Flag & MAT_Cul_Flag_InvertAlpha)
			Dx9_RS_DestBlend( D3DBLEND_SRCALPHA );
        else
			Dx9_RS_DestBlend( D3DBLEND_INVSRCALPHA );
		break;

	case MAT_Cc_Op_AlphaDest:
		Dx9_RS_AlphaBlendEnable( TRUE );
		if(Flag & MAT_Cul_Flag_InvertAlpha)
		{
			Dx9_RS_SrcBlend( D3DBLEND_DESTALPHA );
			Dx9_RS_DestBlend( D3DBLEND_INVDESTALPHA );
		}
		else
		{
			Dx9_RS_SrcBlend( D3DBLEND_INVDESTALPHA );
			Dx9_RS_DestBlend( D3DBLEND_DESTALPHA );
		}
		break;

	case MAT_Cc_Op_AlphaDestPremult:
		Dx9_RS_AlphaBlendEnable( TRUE );
		if(Flag & MAT_Cul_Flag_InvertAlpha)
		{
			Dx9_RS_SrcBlend( D3DBLEND_DESTALPHA );
			Dx9_RS_DestBlend( D3DBLEND_ONE );
		}
		else
		{
			Dx9_RS_SrcBlend( D3DBLEND_INVDESTALPHA );
			Dx9_RS_DestBlend( D3DBLEND_ONE );
		}
		break;

	case MAT_Cc_Op_Add:
		Dx9_RS_AlphaBlendEnable( TRUE );
		Dx9_RS_SrcBlend( D3DBLEND_ONE );
		Dx9_RS_DestBlend( D3DBLEND_ONE );
		break;

	case MAT_Cc_Op_Sub:
	case MAT_Cc_Op_PSX2ShadowSpecific:
		Dx9_RS_AlphaBlendEnable( TRUE );
		Dx9_RS_SrcBlend( D3DBLEND_ZERO );
		Dx9_RS_DestBlend( D3DBLEND_INVSRCCOLOR );
		break;

	}
}

////////////////////
__inline void	Dx9_SetFog( ULONG blendingMode )
{
	Dx9_RS_FogEnable( ( ( GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Fogged ) && ( gDx9SpecificData.fogState ) ) );

	if ( gDx9SpecificData.fogState )
	{
		switch( MAT_GET_Blending( blendingMode ) )
		{
		case MAT_Cc_Op_Copy:
		case MAT_Cc_Op_Glow:
		case MAT_Cc_Op_Alpha:
		case MAT_Cc_Op_AlphaDest:
			if ( gDx9SpecificData.fogState != 1 )
			{
				gDx9SpecificData.fogState = 1;
				Dx9_RS_FogColor( gDx9SpecificData.fogColor );
			}
			break;
		case MAT_Cc_Op_AlphaPremult:
		case MAT_Cc_Op_AlphaDestPremult:
			if(gDx9SpecificData.fogState != 5)
			{
				gDx9SpecificData.fogState = 5;
				Dx9_RS_FogColor( gDx9SpecificData.fogColor );
			}
			break;
		case MAT_Cc_Op_Add:
		case MAT_Cc_Op_Sub:
			if(gDx9SpecificData.fogState != 3)
			{
				gDx9SpecificData.fogState = 3;
				Dx9_RS_FogColor( gDx9SpecificData.fogColorBlack );
			}
			break;
		}
	}
}

///////////////////
__inline void	Dx9_SetWireframe( DWORD wireframeEnable )
{
	if ( wireframeEnable )
		Dx9_RS_FillMode( D3DFILL_WIREFRAME );
	else
		Dx9_RS_FillMode( D3DFILL_SOLID );
}

