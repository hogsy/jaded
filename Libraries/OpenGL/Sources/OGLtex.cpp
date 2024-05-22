/*$T OGLtex.c GC!1.55 01/21/00 12:03:51 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Load texture in memory to be used by OpenGL RC.
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/BAStypes.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "TEXture/TEXconvert.h"
#include "TEXture/TEXmemory.h"

#include "OGLinit.h"
#include "OGLtex.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/BIGread.h"
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
    extern "C" {
#endif

static GDI_tdst_DisplayData     *TEX_DD;
static OGL_tdst_SpecificData    *TEX_SD;

#if !defined(PSX2_TARGET) && !defined(ACTIVE_EDITORS)
#define OPTOGLTEX
#endif

#ifdef OPTOGLTEX
static int ikeys[5000];
static int ibind[5000];
static int icount[5000];
static int nkeys = 0;
static int curcount = 0;
#endif

static float maxTextureAnistropy = 0.0f;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Init specific data for texture creation
 =======================================================================================================================
 */
LONG OGL_l_Texture_Init( GDI_tdst_DisplayData *_pst_DD, ULONG _ul_NbTextures )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OGL_tdst_SpecificData	*pst_SD;
    LONG                    l_Size;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    
	pst_SD = (OGL_tdst_SpecificData *) _pst_DD->pv_SpecificData;

    /* If device is still not created do not create texture */
	if(pst_SD->h_RC == NULL) return 0;
    wglMakeCurrent(pst_SD->h_DC, pst_SD->h_RC);

    if (_ul_NbTextures == 0)
        _ul_NbTextures = 1;
    pst_SD->l_NumberOfTextures = _ul_NbTextures;

    l_Size = (_ul_NbTextures+1)* sizeof(ULONG);
    pst_SD->dul_Texture = (ULONG *) MEM_p_Alloc(l_Size);
    L_memset(pst_SD->dul_Texture, 0, l_Size);
	pst_SD->dul_TextureDeltaBlend = (ULONG *) MEM_p_Alloc(l_Size);
    L_memset(pst_SD->dul_TextureDeltaBlend, 0, l_Size);

    // hogsy:   probably put this somewhere else in future - 
    //          this gets called EVERY time we load a new world,
    //          and obviously this isn't going to change :p
    if ( GLEW_ARB_texture_filter_anisotropic )
	{
		glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxTextureAnistropy );
	}

    return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_Texture_Unload(GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					*pul_Texture, *pul_LastTexture;
	OGL_tdst_SpecificData	*pst_SD;
    int                     i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = (OGL_tdst_SpecificData *) _pst_DD->pv_SpecificData;

	if(!(_pst_DD->st_TexManager.ul_Flags & TEX_Manager_FixVRam))
	{
		wglMakeCurrent(pst_SD->h_DC, pst_SD->h_RC);
		OGL_CALL( glBindTexture(GL_TEXTURE_2D, 0) );
		OGL_RS_UseTexture(pst_SD, -1);
	}

	/* Delete textures */
	pul_Texture = pst_SD->dul_Texture;
	pul_LastTexture = pul_Texture + pst_SD->l_NumberOfTextures;

	if(!(_pst_DD->st_TexManager.ul_Flags & TEX_Manager_FixVRam))
	{
		for(; pul_Texture < pul_LastTexture; pul_Texture++)
		{
			if ( (*pul_Texture) && (*pul_Texture != -1) )
			{
				OGL_CALL( glDeleteTextures( 1, ( GLuint * ) pul_Texture ) );
			}

			*pul_Texture = -1;
		}
	}

	if(pst_SD->dul_Texture)
	{
		MEM_Free(pst_SD->dul_Texture);
		pst_SD->dul_Texture = NULL;
	}

	if(pst_SD->dul_TextureDeltaBlend)
	{
		MEM_Free(pst_SD->dul_TextureDeltaBlend);
		pst_SD->dul_TextureDeltaBlend = NULL;
	}

    if (pst_SD->dst_UdatePalette)
    {
        for (i = 0; i < pst_SD->l_NumberOfUpdatablePalettes; i++)
        {
            if (pst_SD->dst_UdatePalette[i].p_Raw)
                MEM_Free( pst_SD->dst_UdatePalette[i].p_Raw );
        }
        MEM_Free( pst_SD->dst_UdatePalette);
        pst_SD->dst_UdatePalette = NULL;
    }

	pst_SD->l_NumberOfTextures = 0;
    pst_SD->l_NumberOfUpdatablePalettes = 0;

    if (pst_SD->dst_InterfaceTex)
    {
        for (i = 0; i < pst_SD->l_NbInterfaceTex; i++)
        {
            if ( pst_SD->dst_InterfaceTex[ i ].Mipmap )
            {
                for (j = 0; j < pst_SD->dst_InterfaceTex[i].Mipmap; j++)
                {
                    if (pst_SD->dst_InterfaceTex[ i ].ppc_Bufs[ j ] )
                        MEM_Free( pst_SD->dst_InterfaceTex[ i ].ppc_Bufs[ j ] );
                }
                MEM_Free( pst_SD->dst_InterfaceTex[ i ].pc_Buf );
            }
        }
        MEM_Free( pst_SD->dst_InterfaceTex );
    }
    pst_SD->dst_InterfaceTex = NULL;
    pst_SD->l_NbInterfaceTex = 0;

    pst_SD->l_NbBumpTex = 0;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef OPTOGLTEX
void OGL_Texture_UnLoadCompare(void)
{
	int		i;
	for(i  = 0; i < nkeys; i++)
	{
		if(icount[i] != curcount)
		{
#ifdef JADEFUSION
			glDeleteTextures(1, (const GLuint*)&ibind[i]);
#else
			glDeleteTextures(1, &ibind[i]);
#endif
			ikeys[i] = ikeys[nkeys - 1];
			icount[i] = icount[nkeys - 1];
			ibind[i] = ibind[nkeys - 1];
			nkeys--;
			i--;
		}
	}

	curcount++;
}
#else
void OGL_Texture_UnLoadCompare(void)
{
}
#endif

/*
 =======================================================================================================================
    Aim:    load a palette
 =======================================================================================================================
 */
void    OGL_Palette_Load( GDI_tdst_DisplayData *_pst_DD, TEX_tdst_Palette *p_PAL , ULONG Index)
{
}

/*
 =======================================================================================================================
    Aim:    make association between a raw and a pal
 =======================================================================================================================
 */
void    OGL_Set_Texture_Palette( GDI_tdst_DisplayData *_pst_DD, ULONG _ulTexNum, ULONG IdexTex, ULONG IndexPal )
{
}

/*
 =======================================================================================================================
    Aim:    set palette
 =======================================================================================================================
 */
void OGL_Texture_SetPalette(int _i_Palette)
{
}

/*
 =======================================================================================================================
    Aim:    add data to reload texture that use an updatable palette 
 =======================================================================================================================
 */
void OGL_Texture_AddDataForUpdatablePalette( OGL_tdst_SpecificData	*pst_SD, short w_Tex, short w_Pal, short W, short H, LONG l_Size, char *pc_Buf )
{
    LONG L;

    L = sizeof( OGL_tdst_UpdatablePalette ) * (pst_SD->l_NumberOfUpdatablePalettes + 1);
    if (pst_SD->l_NumberOfUpdatablePalettes)
        pst_SD->dst_UdatePalette = (OGL_tdst_UpdatablePalette *) MEM_p_Realloc( pst_SD->dst_UdatePalette, L );
    else
        pst_SD->dst_UdatePalette = (OGL_tdst_UpdatablePalette *) MEM_p_Alloc( L );

    pst_SD->dst_UdatePalette[ pst_SD->l_NumberOfUpdatablePalettes ].l_Size = l_Size;
    pst_SD->dst_UdatePalette[ pst_SD->l_NumberOfUpdatablePalettes ].w_Width = W;
    pst_SD->dst_UdatePalette[ pst_SD->l_NumberOfUpdatablePalettes ].w_Height = H;
    pst_SD->dst_UdatePalette[ pst_SD->l_NumberOfUpdatablePalettes ].w_Texture = w_Tex;
    pst_SD->dst_UdatePalette[ pst_SD->l_NumberOfUpdatablePalettes ].w_Palette = w_Pal;
    pst_SD->dst_UdatePalette[ pst_SD->l_NumberOfUpdatablePalettes ].p_Raw = (char *) MEM_p_Alloc( l_Size );
    L_memcpy( pst_SD->dst_UdatePalette[ pst_SD->l_NumberOfUpdatablePalettes ].p_Raw, pc_Buf, l_Size );

    pst_SD->l_NumberOfUpdatablePalettes++;
}

/*
 =======================================================================================================================
    Aim:    calcule le niveau suivant de mipmapping pour une texture 32 bits 
 =======================================================================================================================
 */
LONG OGL_l_Texture_ComputeNextMipmapLevel_32( LONG l_MipmapFlag, int i_MipmapLevel, ULONG *TX, ULONG *TY, ULONG *pul_Buffer, int c )
{
    ULONG X, Y;

    X = *TX;
    Y = *TY;

    GDI_M_TimerStart(GDI_f_Delay_AttachWorld_TextureCreate_Mipmap);
    switch(l_MipmapFlag & (TEX_FP_MipmapUseAlpha | TEX_FP_MipmapUseColor))
    {
    case TEX_FP_MipmapUseAlpha | TEX_FP_MipmapUseColor : 
        if(i_MipmapLevel < 4) 
            TEX_Blend_AlphaColor50(pul_Buffer, pul_Buffer, X*Y, c);
        else
            TEX_Blend_AlphaColor25(pul_Buffer, pul_Buffer, X*Y, c);
        break;
    case TEX_FP_MipmapUseAlpha:
        if(i_MipmapLevel < 4)
            TEX_Blend_Alpha50(pul_Buffer, pul_Buffer, X*Y, c);
        else
            TEX_Blend_Alpha25(pul_Buffer, pul_Buffer, X*Y, c);
        break;
    case TEX_FP_MipmapUseColor:
        if(i_MipmapLevel < 4)
            TEX_Blend_Color50(pul_Buffer, pul_Buffer, X*Y, c);
        else
            TEX_Blend_Color25(pul_Buffer, pul_Buffer, X*Y, c);
        break;
    }

    /* Keep good Border Here */
    if ( (X > 2) && (Y > 2) && (l_MipmapFlag & TEX_FP_MipmapKeepBorder) )
        TEX_Blend_KeepBorder( pul_Buffer, X, Y );

    /* Compress Image by Two */
    if ((X == 1) && ( Y == 1))
    {
        GDI_M_TimerStop(GDI_f_Delay_AttachWorld_TextureCreate_Mipmap);
        return 0;
    }

    if (X > 1)
    {
        TEX_Compress_Xo2(pul_Buffer, pul_Buffer, X, Y);
        X >>= 1;
    }
    if (Y > 1)
    {
        if (TEX_DD->st_TexManager.ul_Flags & TEX_Manager_InvertMipmap)
            TEX_Compress_Yo2InvertColor(pul_Buffer, pul_Buffer, X, Y);
        else
            TEX_Compress_Yo2(pul_Buffer, pul_Buffer, X, Y);
        Y >>= 1;
    }

    *TX = X;
    *TY = Y;
    GDI_M_TimerStop(GDI_f_Delay_AttachWorld_TextureCreate_Mipmap);
    return 1;
}

/*
 =======================================================================================================================
    Aim:    Create a texture and set parameters
 =======================================================================================================================
 */
ULONG OGL_ul_Texture_Create( ULONG ul_Key, int i_Mipmap )
{
    ULONG ul_Texture;

    // création de la texture 
	OGL_CALL( glEnable(GL_TEXTURE_2D) );
    OGL_CALL( glGenTextures(1, (GLuint*)&ul_Texture) );
    OGL_CALL( glBindTexture(GL_TEXTURE_2D, ul_Texture) );

#ifdef OPTOGLTEX
	if(TEX_DD->st_TexManager.ul_Flags & TEX_Manager_FixVRam)
	{
		ikeys[nkeys] = ul_Key;
		ibind[nkeys] = ul_Texture;
		icount[nkeys++] = curcount;
		ERR_X_Assert(nkeys != 5000);
	}
#endif

    /* Initialisation */
	OGL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) );
	OGL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) );
	OGL_CALL( glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) );
    OGL_CALL( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ( i_Mipmap == 0 ) ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR ) );
	if ( GLEW_ARB_texture_filter_anisotropic )
	{
		OGL_CALL( glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, ( i_Mipmap == 0 ) ? 0.0f : maxTextureAnistropy ) );
	}

    return ul_Texture;
}

/*
 =======================================================================================================================
    Aim:    load buffer into VRam
 =======================================================================================================================
 */
LONG OGL_Texture_LoadCB( ULONG ul_Texture, ULONG _ul_Key, ULONG *_pul_Texture, int i_Mipmap, int MMC, int BPP, int W, int H, int format, ULONG *p_Buffer, short w_Palette )
{
    if (MMC == 0)
    {
        *_pul_Texture = OGL_ul_Texture_Create( _ul_Key, i_Mipmap );
        if ( w_Palette != -1 )
            OGL_Texture_SetPalette( w_Palette );
    }

    TEX_DD->st_TexManager.l_MemoryTakenByLoading += (W * H * BPP) >> 3;
    GDI_M_TimerStart(GDI_f_Delay_AttachWorld_TextureCreate_LoadHard);
	OGL_CALL( glTexImage2D( GL_TEXTURE_2D, MMC, GL_RGBA, W, H, 0, format, GL_UNSIGNED_BYTE, p_Buffer ) );
    GDI_M_TimerStop(GDI_f_Delay_AttachWorld_TextureCreate_LoadHard);

    // hogsy: we're now just using i_Mipmap as a flag to indicate if we want mipmapping or not...
    if ( i_Mipmap )
	{
		if ( GLEW_ARB_framebuffer_object )
		{
			glGenerateMipmap( GL_TEXTURE_2D );
		}
		else
		{
			glGenerateMipmapEXT( GL_TEXTURE_2D );
		}
	}
    
    return 1;
}

/*
 =======================================================================================================================
    Aim:    store buffer into ram
 =======================================================================================================================
 */
LONG OGL_Texture_StoreCB( ULONG ul_Texture, ULONG _ul_Key, ULONG *_pul_Texture, int i_Mipmap, int MMC, int BPP, int W, int H, int format, ULONG *p_Buffer, short w_Palette )
{
    ULONG                       ul_Size;
    OGL_tdst_InterfaceTexture   *pst_ITex;

    // add one interface texture
    if (MMC == 0)
    {
        ul_Size = ( TEX_SD->l_NbInterfaceTex + 1 ) * sizeof ( OGL_tdst_InterfaceTexture );
        if (TEX_SD->l_NbInterfaceTex)
            TEX_SD->dst_InterfaceTex = (OGL_tdst_InterfaceTexture *) MEM_p_Realloc( TEX_SD->dst_InterfaceTex, ul_Size );
        else
            TEX_SD->dst_InterfaceTex = (OGL_tdst_InterfaceTexture *) MEM_p_Alloc( ul_Size );

        pst_ITex = TEX_SD->dst_InterfaceTex + TEX_SD->l_NbInterfaceTex++;
        pst_ITex->BPP = BPP;
        pst_ITex->W = W;
        pst_ITex->H = H;
        pst_ITex->format = format;
        pst_ITex->ul_Key = _ul_Key;
        pst_ITex->Mipmap = i_Mipmap;
        pst_ITex->ul_Texture = ul_Texture;
        pst_ITex->w_Palette = w_Palette;

        TEX_gst_GlobalList.dst_Palette[ w_Palette ].uc_Flags |= TEX_uc_KeepPalInMem;
        
        if (i_Mipmap)
        {
            pst_ITex->ppc_Bufs = (char **) MEM_p_Alloc( i_Mipmap * sizeof( char *) );
            L_memset( pst_ITex->ppc_Bufs, 0, i_Mipmap * sizeof( char *) );
        }
        else
        {
            if (BPP == 4)   ul_Size = W*H;
            else            ul_Size = (W * H * BPP) >> 3;

            pst_ITex->pc_Buf = (char *) MEM_p_Alloc( ul_Size );
            L_memcpy( pst_ITex->pc_Buf, p_Buffer, ul_Size );
            return 1;
        }
    }

    pst_ITex = TEX_SD->dst_InterfaceTex + (TEX_SD->l_NbInterfaceTex - 1);
    ul_Size = (W * H * BPP) >> 3;
    pst_ITex->ppc_Bufs[ MMC ] = (char *) MEM_p_Alloc( ul_Size );
    L_memcpy( pst_ITex->ppc_Bufs[ MMC ], p_Buffer, ul_Size );
    return 1;
}

/*
 =======================================================================================================================
    Aim:    Create a texture from given texture data
 =======================================================================================================================
 */
void OGL_Texture_InternalLoad
(
    TEX_tdst_Data           *_pst_TexData,
    TEX_tdst_File_Desc      *_pst_Tex,
    ULONG                    _ul_Texture,
    LONG (*LoadCB) ( ULONG, ULONG, ULONG *, int, int, int, int, int, int, ULONG *, short)

)
{
#ifdef OPTOGLTEX
	int						i;
#endif

#ifdef ACTIVE_EDITORS
	extern int WOR_gi_CurrentConsole;
	extern BOOL EDI_gb_ComputeMap;
	if(EDI_gb_ComputeMap)
	{
		//no hardware loading while preprocess for PS2 / GC
		if( WOR_gi_CurrentConsole== 1) return;
		if( WOR_gi_CurrentConsole== 2) return;
	}
#endif
	
    TEX_SD->dul_Texture[ _ul_Texture ] = -1;

    // calcul du nombre de niveau de mipmapping
	ULONG TX, TY, MMC, BPP;
    TX = _pst_TexData->w_Width;
    TY = _pst_TexData->w_Height;
    BPP = _pst_Tex->uc_FinalBPP;
    MMC = 0;
	int i_MipmapLevel = 0;

    int c;
    if( _pst_Tex->st_Params.uw_Flags & TEX_FP_MipmapOn ) 
    {
        _pst_Tex->uw_FileFlags |= TEX_uw_Mipmap;
        c = (TX > TY) ? TX : TY;
        while(c)
        {
            i_MipmapLevel++;
            c >>= 1;
        }
    }
    else
    {
        _pst_Tex->uw_FileFlags &= ~TEX_uw_Mipmap;
    }
    
    // paramètres
    _pst_TexData->uw_Flags = _pst_Tex->uw_FileFlags;
    c = _pst_Tex->st_Params.ul_Color;
	ULONG l_MipmapFlag = _pst_Tex->st_Params.uw_Flags & ( TEX_FP_MipmapUseAlpha | TEX_FP_MipmapUseColor | TEX_FP_MipmapKeepBorder );
	ULONG *pul_ConvertBuffer = ( ULONG * ) _pst_Tex->p_Bitmap;

    char *p_Buf = NULL;
    // So it turns out with some drivers (or mine at least), palleted textures don't work so great
    // so we'll do some conversion here - core spec drops support for palleted textures anyway, so ~hogsy
	if ( BPP != 32 )
	{
		TEX_tdst_Data *pst_RawPal;
		// loop through texture to find a good association
		if ( TEX_DD->st_TexManager.ul_Flags & TEX_Manager_OneTexForRawPal )
		{
			pst_RawPal = TEX_gst_GlobalList.dst_Texture;
			for ( c = 0; c < TEX_gst_GlobalList.l_NumberOfTextures; c++, pst_RawPal++ )
			{
				if ( ( pst_RawPal->uw_Flags & TEX_uw_RawPal ) && ( pst_RawPal->w_Height == ( SHORT ) _ul_Texture ) )
				{
					// watch if palette is updatable, in such case we have to keep the buffer and association of buffer and palette
					if ( TEX_gst_GlobalList.dst_Palette[ pst_RawPal->w_Width ].uc_Flags & TEX_uc_UpdatablePal )
					{
						OGL_Texture_AddDataForUpdatablePalette( TEX_SD, ( short ) c, pst_RawPal->w_Width, ( short ) TX, ( short ) TY, TX * TY, ( char * ) pul_ConvertBuffer );
					}
					break;
				}
			}
		}
		else
		{
			pst_RawPal = TEX_gst_GlobalList.dst_Texture;
			for ( c = 0; c < TEX_gst_GlobalList.l_NumberOfTextures; c++, pst_RawPal++ )
			{
				if ( ( pst_RawPal->uw_Flags & TEX_uw_RawPal ) && ( pst_RawPal->w_Height == ( SHORT ) _ul_Texture ) )
				{
					break;
				}
			}

			// watch if palette is updatable, in such case we have to keep the buffer and association of buffer and palette
			if ( c != TEX_gst_GlobalList.l_NumberOfTextures )
			{
				if ( TEX_gst_GlobalList.dst_Palette[ pst_RawPal->w_Width ].uc_Flags & TEX_uc_UpdatablePal )
				{
					OGL_Texture_AddDataForUpdatablePalette( TEX_SD, _pst_TexData->w_Index, pst_RawPal->w_Width, ( short ) TX, ( short ) TY, TX * TY, ( char * ) pul_ConvertBuffer );
				}
			}
		}

		size_t size = TX * TY;
		p_Buf = ( char * ) L_malloc( size * 4 );
		L_zero( p_Buf, size * 4 );
		if ( BPP == 4 )
		{
			TEX_Convert_4To32( ( ULONG * ) p_Buf, ( UCHAR * ) pul_ConvertBuffer, TEX_gst_GlobalList.dst_Palette[ pst_RawPal->w_TexPal ].pul_Color, size );
			pul_ConvertBuffer = ( ULONG * ) p_Buf;
		}
		else if ( BPP == 8 )
		{
			TEX_Convert_8To32( ( ULONG * ) p_Buf, ( UCHAR * ) pul_ConvertBuffer, TEX_gst_GlobalList.dst_Palette[ pst_RawPal->w_TexPal ].pul_Color, size );
			pul_ConvertBuffer = ( ULONG * ) p_Buf;
		}
	}

	ULONG ul_Texture;
    LoadCB( _ul_Texture, _pst_TexData->ul_Key, &ul_Texture, ( _pst_Tex->st_Params.uw_Flags & TEX_FP_MipmapOn ), MMC, 32, TX, TY, GL_RGBA, pul_ConvertBuffer, -1 );

    L_free( p_Buf );

    TEX_SD->dul_Texture[ _ul_Texture ] = ul_Texture;
}


/*
 =======================================================================================================================
    Aim:    load a texture into vram
 =======================================================================================================================
 */
void OGL_Texture_Load
(
    GDI_tdst_DisplayData    *_pst_DD,
    TEX_tdst_Data           *_pst_TexData,
    TEX_tdst_File_Desc      *_pst_Tex,
    ULONG                    _ul_Texture
)
{
    TEX_DD = _pst_DD;
    TEX_SD = (OGL_tdst_SpecificData *) _pst_DD->pv_SpecificData;
    
    OGL_Texture_InternalLoad( _pst_TexData, _pst_Tex, _ul_Texture, OGL_Texture_LoadCB );
}

/*
 =======================================================================================================================
    Aim:    store texture in ram
 =======================================================================================================================
 */
LONG OGL_l_Texture_Store
(
    GDI_tdst_DisplayData    *_pst_DD,
    TEX_tdst_Data           *_pst_TexData,
    TEX_tdst_File_Desc      *_pst_Tex,
    ULONG                    _ul_Texture
)
{
    if ( !(_pst_DD->st_TexManager.ul_Flags & TEX_Manager_StoreInterfaceTex)) 
        return 0;

    TEX_DD = _pst_DD;
    TEX_SD = (OGL_tdst_SpecificData *) _pst_DD->pv_SpecificData;

    OGL_Texture_InternalLoad( _pst_TexData, _pst_Tex, _ul_Texture, OGL_Texture_StoreCB );
    return 1;
}

/*
 =======================================================================================================================
    Aim:    load texture store in ram to vram
 =======================================================================================================================
 */
void OGL_Texture_LoadInterfaceTex( GDI_tdst_DisplayData *_pst_DD )
{
    if ( !(_pst_DD->st_TexManager.ul_Flags & TEX_Manager_LoadStoredITex) ) 
        return;

    TEX_SD = (OGL_tdst_SpecificData *) _pst_DD->pv_SpecificData;
	OGL_tdst_InterfaceTexture *pst_ITex = TEX_SD->dst_InterfaceTex;
    for (int i = 0; i < TEX_SD->l_NbInterfaceTex; i++, pst_ITex++)
    {
		ULONG ul_Texture;
        OGL_Texture_LoadCB( pst_ITex->ul_Texture, pst_ITex->ul_Key, &ul_Texture, ( pst_ITex->Mipmap > 0 ), 0, pst_ITex->BPP, pst_ITex->W, pst_ITex->H, pst_ITex->format, ( ULONG * ) pst_ITex->pc_Buf, pst_ITex->w_Palette );
        TEX_SD->dul_Texture[ pst_ITex->ul_Texture ] = ul_Texture;
    }
}

/*
 =======================================================================================================================
    Aim:    unload texture store in ram that are in vram
 =======================================================================================================================
 */
void OGL_Texture_UnloadInterfaceTex( GDI_tdst_DisplayData *_pst_DD)
{
    OGL_tdst_InterfaceTexture   *pst_ITex;
    ULONG                       ul_Texture;
    int                         i, j, TX, TY;
    
    TEX_SD = (OGL_tdst_SpecificData *) _pst_DD->pv_SpecificData;
    wglMakeCurrent( TEX_SD->h_DC, TEX_SD->h_RC);
	OGL_CALL( glBindTexture(GL_TEXTURE_2D, 0) );
	OGL_RS_UseTexture(TEX_SD, -1);

    TEX_SD = (OGL_tdst_SpecificData *) _pst_DD->pv_SpecificData;
    for (i = 0, pst_ITex = TEX_SD->dst_InterfaceTex; i < TEX_SD->l_NbInterfaceTex; i++, pst_ITex++)
    {
        ul_Texture = TEX_SD->dul_Texture[ pst_ITex->ul_Texture ];
        if ( (ul_Texture) && (ul_Texture != -1)) 
        {
			OGL_CALL( glDeleteTextures( 1, ( GLuint * ) & ul_Texture ) );

            TX = pst_ITex->W;
            TY = pst_ITex->H;
            if (pst_ITex->Mipmap)
            {
                for (j = 0; j < pst_ITex->Mipmap; j++)
                {
                    TEX_DD->st_TexManager.l_MemoryTakenByLoading -= (TX * TY * pst_ITex->BPP) >> 3;
                    if (TX > 1) TX >>= 1;
                    if (TY > 1) TY >>= 1;
                }
            }
            else
            {
                TEX_DD->st_TexManager.l_MemoryTakenByLoading -= (TX * TY * pst_ITex->BPP) >> 3;
            }

        }
        TEX_SD->dul_Texture[ pst_ITex->ul_Texture ] = -1;
    }
}

/*
 =======================================================================================================================
    Aim:    return texture buffer associated to an interface texture
 =======================================================================================================================
 */
void *OGL_p_Texture_GetInterfaceTexBuffer( GDI_tdst_DisplayData *_pst_DD, ULONG _ul_Key )
{
    OGL_tdst_InterfaceTexture   *pst_ITex;
    int                         i;

    if ( !(_pst_DD->st_TexManager.ul_Flags & TEX_Manager_LoadStoredITex) ) 
        return NULL;

    TEX_SD = (OGL_tdst_SpecificData *) _pst_DD->pv_SpecificData;
    for (i = 0, pst_ITex = TEX_SD->dst_InterfaceTex; i < TEX_SD->l_NbInterfaceTex; i++, pst_ITex++)
    {
        if (pst_ITex->ul_Key == _ul_Key)
        {
            return pst_ITex->pc_Buf;
        }
    }
    return NULL;
}

#if defined (__cplusplus) && !defined(JADEFUSION)
    }
#endif
