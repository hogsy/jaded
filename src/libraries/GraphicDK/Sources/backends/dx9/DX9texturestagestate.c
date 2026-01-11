// Dx9texturestagestate.c


#include "Dx9texturestagestate.h"
#include "TEXture/TEXstruct.h"


/************************************************************************************************************************
    Public Function
 ************************************************************************************************************************/

///////////////
#define INIT_TSS_HANDLER(StateName) \
	Dx9_TSS_Get##StateName(stage, &gDx9SpecificData.TextureStageState[stage].StateName)


void	Dx9_InitTextureStageStates( void )
{
	DWORD stage;

	for ( stage = 0; stage < gDx9SpecificData.d3dCaps.MaxTextureBlendStages; stage ++ )
	{
		gDx9SpecificData.TextureStageState[stage].LastTexture = -1;
/*
		INIT_TSS_HANDLER(ColorOp);
		INIT_TSS_HANDLER(ColorArg1);
		INIT_TSS_HANDLER(ColorArg2);
		INIT_TSS_HANDLER(AlphaOp);
		INIT_TSS_HANDLER(AlphaArg1);
		INIT_TSS_HANDLER(AlphaArg2);
		INIT_TSS_HANDLER(BumpEnvMat00);
		INIT_TSS_HANDLER(BumpEnvMat01);
		INIT_TSS_HANDLER(BumpEnvMat10);
		INIT_TSS_HANDLER(BumpEnvMat11);
		INIT_TSS_HANDLER(TexCoordIndex);
		INIT_TSS_HANDLER(BumpEnvLScale);
		INIT_TSS_HANDLER(BumpEnvLOffset);
		INIT_TSS_HANDLER(TextureTransformFlags);
		INIT_TSS_HANDLER(ColorArg0);
		INIT_TSS_HANDLER(AlphaArg0);
		INIT_TSS_HANDLER(ResultArg);
		INIT_TSS_HANDLER(Constant);*/
	}
}

///////////////////
LONG	Dx9_TSS_UseTexture( DWORD stage, LONG textureId )
{
    TEX_tdst_Data			* pst_Tex;
    IDirect3DBaseTexture9	* T;

    if( textureId != gDx9SpecificData.TextureStageState[stage].LastTexture )
    {
        if ( ( textureId == -1 ) || ( !gDx9SpecificData.TextureList ) )
        {
            Dx9_TSS_ColorArg1( stage, D3DTA_DIFFUSE );
		    Dx9_TSS_ColorOp  ( stage, D3DTOP_SELECTARG1 );
		    Dx9_TSS_AlphaArg1( stage, D3DTA_DIFFUSE );
		    Dx9_TSS_AlphaOp  ( stage, D3DTOP_SELECTARG1 );
        }
        else
        {
            // shadow Texture ?
//            if(textureId & 0x80000000)
//		    {
//                T = (IDirect3DBaseTexture9 *) gDx9SpecificData.dul_SDW_Texture[textureId &~0x80000000];
//    		}
//            else
            {
                if (gDx9SpecificData.TextureList[ textureId ] == 0 )
                {
                    pst_Tex = &TEX_gst_GlobalList.dst_Texture[ textureId ];
                
                    // raw + pal
                    if (pst_Tex->uw_Flags & TEX_uw_RawPal)
                    {
                        textureId = pst_Tex->w_Height;
                        IDirect3DDevice9_SetCurrentTexturePalette( gDx9SpecificData.pD3DDevice, pst_Tex->w_Width);
                    }
                    // animated texture
                    else if (pst_Tex->uw_Flags & TEX_uw_Ani)
                    {
                        textureId = pst_Tex->w_Height;
                        if (textureId == -1)
                        {
                            return Dx9_TSS_UseTexture( stage, -1 );
                        }
                        else if (gDx9SpecificData.TextureList[ textureId ] == 0 )
                        {
                            pst_Tex = &TEX_gst_GlobalList.dst_Texture[ textureId ];
                            if (pst_Tex->uw_Flags & TEX_uw_RawPal)
                            {
                                textureId = pst_Tex->w_Height;
                                IDirect3DDevice9_SetCurrentTexturePalette( gDx9SpecificData.pD3DDevice, pst_Tex->w_Width);
                            }
                        }
                    }
                }

                T = (IDirect3DBaseTexture9 *) gDx9SpecificData.TextureList[textureId];
            }
            
            IDirect3DDevice9_SetTexture( gDx9SpecificData.pD3DDevice, stage, T );

            if (gDx9SpecificData.TextureStageState[stage].LastTexture == -1)
            {
                Dx9_TSS_ColorArg1(stage, D3DTA_TEXTURE);
		        Dx9_TSS_ColorArg2(stage, D3DTA_DIFFUSE);
		        Dx9_TSS_ColorOp  (stage, D3DTOP_MODULATE);
		        Dx9_TSS_AlphaArg1(stage, D3DTA_TEXTURE);
		        Dx9_TSS_AlphaArg2(stage, D3DTA_DIFFUSE);
		        Dx9_TSS_AlphaOp  (stage, D3DTOP_MODULATE);
            }
            
        }

        gDx9SpecificData.TextureStageState[stage].LastTexture = textureId;
    }
    
    return textureId;
}
