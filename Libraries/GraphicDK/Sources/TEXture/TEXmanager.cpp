/*$T TEXmanager.c GC!1.55 01/21/00 11:55:02 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "TEXture/TEXstruct.h"
#include "TEXture/TEXmanager.h"
#include "TEXture/TEXconvert.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"

#ifdef JADEFUSION
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

#if defined(_XENON_RENDER)
#include "GDInterface/GDInterface.h"
#endif
#endif
/*$4
 ***********************************************************************************************************************
    Public functions
 ***********************************************************************************************************************
 */

extern BOOL EDI_gb_ComputeMap;
extern BOOL WOR_gi_CurrentConsole;

#ifdef _GAMECUBE
int	mem_available;
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 
#ifdef GSP_PS2
extern ULONG GSP_GetTextureMemoryAvailable(); 
#endif

#ifdef _GAMECUBE
BOOL	gb_UseDisplayLists = TRUE;
#endif
int TEX_gi_ForceText = 0;

void TEX_Manager_Init(TEX_tdst_Manager *_pst_TM)
{
	_pst_TM->c_TexSlotOrder[0] = TEX_Manager_SlotNone;
	_pst_TM->c_TexSlotOrder[1] = TEX_Manager_SlotNone;
	_pst_TM->c_TexSlotOrder[2] = TEX_Manager_SlotNone;
	_pst_TM->c_TexSlotOrder[3] = TEX_Manager_SlotNone;

#ifdef PSX2_TARGET    
    _pst_TM->l_MaxTextureSize = 512;
    _pst_TM->l_MinTextureSize = 8;
	_pst_TM->l_VRamAvailable = GSP_GetTextureMemoryAvailable();
    _pst_TM->l_RamAvailable = 512 * 1024;

    _pst_TM->ul_Flags = TEX_Manager_FastCompression ;
    _pst_TM->ul_Flags |= TEX_Manager_UseMipmap;
    _pst_TM->ul_Flags |= TEX_Manager_Accept32bpp ;
    _pst_TM->ul_Flags |= TEX_Manager_RGB2BGR;
            
  	_pst_TM->ul_Flags |= TEX_Manager_Accept8bpp ;
    _pst_TM->ul_Flags |= TEX_Manager_Accept4bpp;
    _pst_TM->ul_Flags |= TEX_Manager_AcceptAlphaPalette;
    
    _pst_TM->ul_Flags |= TEX_Manager_AcceptNonSquare;
    _pst_TM->ul_Flags |= TEX_Manager_InvertMipmap;
    _pst_TM->ul_Flags |= TEX_Manager_StoreInterfaceTex | TEX_Manager_LoadStoredITex;

	_pst_TM->c_TexSlotOrder[ 0 ] = TEX_Manager_SlotPS2;

#elif defined(_XENON)
    // JFP: We want 1024 for max texture size on Xbox 360.
    _pst_TM->l_MaxTextureSize = 1024;
    _pst_TM->l_MinTextureSize = 8;
	_pst_TM->l_VRamAvailable = 512 * 1024 * 1024;
    _pst_TM->l_RamAvailable = 512 * 1024;

    _pst_TM->ul_Flags = TEX_Manager_FastCompression ;
//    _pst_TM->ul_Flags |= TEX_Manager_UseMipmap;
    _pst_TM->ul_Flags |= TEX_Manager_Accept32bpp ;
//    _pst_TM->ul_Flags |= TEX_Manager_RGB2BGR;
            
//  	_pst_TM->ul_Flags |= TEX_Manager_Accept8bpp ;
//    _pst_TM->ul_Flags |= TEX_Manager_Accept4bpp;
//    _pst_TM->ul_Flags |= TEX_Manager_AcceptAlphaPalette;
    
    _pst_TM->ul_Flags |= TEX_Manager_AcceptNonSquare;
    _pst_TM->ul_Flags |= TEX_Manager_InvertMipmap;

//    _pst_TM->ul_Flags |= TEX_Manager_StoreInterfaceTex | TEX_Manager_LoadStoredITex;

	_pst_TM->c_TexSlotOrder[ 0 ] = TEX_Manager_SlotPS2;

#elif defined(_XBOX)
    _pst_TM->l_MaxTextureSize = 512;
    _pst_TM->l_MinTextureSize = 8;
	_pst_TM->l_VRamAvailable = 4 * 1024 * 1024;
    _pst_TM->l_RamAvailable = 512 * 1024;

    _pst_TM->ul_Flags = TEX_Manager_FastCompression ;
    _pst_TM->ul_Flags |= TEX_Manager_UseMipmap;
    _pst_TM->ul_Flags |= TEX_Manager_Accept32bpp ;
    _pst_TM->ul_Flags |= TEX_Manager_RGB2BGR;
            
  	_pst_TM->ul_Flags |= TEX_Manager_Accept8bpp ;
    _pst_TM->ul_Flags |= TEX_Manager_Accept4bpp;
    _pst_TM->ul_Flags |= TEX_Manager_AcceptAlphaPalette;
    
    _pst_TM->ul_Flags |= TEX_Manager_AcceptNonSquare;
    _pst_TM->ul_Flags |= TEX_Manager_InvertMipmap;

//    _pst_TM->ul_Flags |= TEX_Manager_StoreInterfaceTex | TEX_Manager_LoadStoredITex;

	_pst_TM->c_TexSlotOrder[ 0 ] = TEX_Manager_SlotXBOX;
	_pst_TM->c_TexSlotOrder[ 1 ] = TEX_Manager_SlotPS2;

#elif defined(_GAMECUBE)

    _pst_TM->l_MaxTextureSize = 512;
    _pst_TM->l_MinTextureSize = 0;

	{
#ifdef JADEFUSION
		extern __declspec(align(32)) MEM_tdst_MainStruct MEM_gst_MemoryInfo;
#else
		extern MEM_tdst_MainStruct	MEM_gst_MemoryInfo;
#endif
    	if(mem_available > ((2*1024 + 512)*1024))
    	{
	    	mem_available = ((2*1024 + 512)*1024);
    	}    	    	

    	_pst_TM->l_VRamAvailable = mem_available;

    	    	
    	if(mem_available < (1024 + 512)*1024)
    	{
			extern unsigned int WOR_gul_WorldKey;		    	

			if((WOR_gul_WorldKey == 0xC100b7AD))
		    	_pst_TM->l_VRamAvailable = (1024)*1024;    		    	    	
			else	
		    	_pst_TM->l_VRamAvailable = (1024 + 512)*1024;    		    	    	
		    	
	    	gb_UseDisplayLists = FALSE;
	    }
	    else
	    {
	    	gb_UseDisplayLists = TRUE;	    	    	
	    }

/*
#ifdef _DEBUG
    	OSReport("\nTexture memory available : %ld k\n", _pst_TM->l_VRamAvailable/1024);
#endif    	
*/
    	
	    _pst_TM->l_RamAvailable = 512*1024;  // valeur arbitraire pour le moment	    		    	
	}
	
       
    _pst_TM->ul_Flags = TEX_Manager_FastCompression;
  	_pst_TM->ul_Flags |= TEX_Manager_AcceptAlphaPalette;
    _pst_TM->ul_Flags |= TEX_Manager_Accept32bpp;
  	_pst_TM->ul_Flags |= TEX_Manager_Accept8bpp ;
    _pst_TM->ul_Flags |= TEX_Manager_Accept4bpp;
    _pst_TM->ul_Flags |= TEX_Manager_AcceptNonSquare;

	_pst_TM->c_TexSlotOrder[ 0 ] = TEX_Manager_SlotGC;
	_pst_TM->c_TexSlotOrder[ 1 ] = TEX_Manager_SlotPS2;

#else

// hogsy: (todo) we could actually easily query the hardware more accurately for this info...

    _pst_TM->l_MaxTextureSize = 4096;
	_pst_TM->l_MinTextureSize = 8;
	_pst_TM->l_VRamAvailable  = 512 * 1024 * 1024;
	_pst_TM->l_RamAvailable   = 512 * 1024;
    
    _pst_TM->ul_Flags = TEX_Manager_FastCompression;
    _pst_TM->ul_Flags |= TEX_Manager_UseMipmap;
    _pst_TM->ul_Flags |= TEX_Manager_Accept32bpp;
    _pst_TM->ul_Flags |= TEX_Manager_RGB2BGR;
    _pst_TM->ul_Flags |= TEX_Manager_AcceptNonSquare;
  
    /* 
     * force flags with command line switch : £
     * TEX_Manager_Accept8bpp or £
     * TEX_Manager_Accept4bpp or £
     * TEX_Manager_AcceptAlphaPalette
     */
    _pst_TM->ul_Flags  |= TEX_gi_ForceText;

	_pst_TM->c_TexSlotOrder[ 0 ] = TEX_Manager_SlotPC;
	_pst_TM->c_TexSlotOrder[ 1 ] = TEX_Manager_SlotXBOX;
	_pst_TM->c_TexSlotOrder[ 2 ] = TEX_Manager_SlotGC;
	_pst_TM->c_TexSlotOrder[ 3 ] = TEX_Manager_SlotPS2;

#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Manager_Reinit(TEX_tdst_Manager *_pst_TM)
{
#ifdef PSX2_TARGET    
    _pst_TM->l_VRamAvailable = GSP_GetTextureMemoryAvailable();
#elif defined(_GAMECUBE)
   	{
#ifdef JADEFUSION
		extern __declspec(align(32)) MEM_tdst_MainStruct MEM_gst_MemoryInfo;
#else
		extern MEM_tdst_MainStruct	MEM_gst_MemoryInfo;
#endif
/*
    	mem_available = ((((u32) OSGetArenaHi() - (u32) OSGetArenaLo()) - (((char *) MEM_gst_MemoryInfo.pv_DynamicNextFree - (char *) MEM_gst_MemoryInfo.pv_DynamicBloc)))-(4*512*1024));
    	if(mem_available > (1024*1024))
    	{
    	    mem_available = (1024*1024);
    	}
*/    	
    	
    	    	
    	if(mem_available > ((2*1024 + 512)*1024))
    	{
	    	mem_available = ((2*1024 + 512)*1024);
    	}    	    	
    	
    	_pst_TM->l_VRamAvailable = mem_available;    	
    	    	
    	if(mem_available < (1024 + 512)*1024)
    	{
			extern unsigned int WOR_gul_WorldKey;		    	

			if((WOR_gul_WorldKey == 0xC100b7AD))
		    	_pst_TM->l_VRamAvailable = (1024)*1024;    		    	    			    	
			else	
		    	_pst_TM->l_VRamAvailable = (1024 + 512)*1024;    		    	    	
	    	gb_UseDisplayLists = FALSE;
	    }
	    else
	    	gb_UseDisplayLists = TRUE;
	    
/*#ifdef _DEBUG	    	
    	OSReport("\nTexture memory available : %ld k\n", _pst_TM->l_VRamAvailable/1024);	    	
#endif    	*/
	}
#endif

    _pst_TM->l_VRamLeft = _pst_TM->l_VRamAvailable;
    _pst_TM->l_VRamNeeded = 0;
    _pst_TM->l_RamLeft = _pst_TM->l_RamAvailable;
    _pst_TM->l_RamNeeded = 0;
    _pst_TM->l_MemoryTakenByLoading = 0;

    _pst_TM->l_TextureNumber = 0;
    _pst_TM->l_VRamTextureNumber = 0;
    _pst_TM->l_RamTextureNumber = 0;

#ifdef ACTIVE_EDITORS
    L_memset( &_pst_TM->al_TextureNumberByBpp, 0, sizeof( _pst_TM->al_TextureNumberByBpp ) );
    L_memset( &_pst_TM->aaal_TextureNumberByBppWidthHeight, 0, sizeof( _pst_TM->aaal_TextureNumberByBppWidthHeight) );
    _pst_TM->l_NbTextureInterface = 0;
    L_memset( &_pst_TM->aul_TextureInterface, 0, sizeof( _pst_TM->aul_TextureInterface ) );
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Manager_ComputeCompression(TEX_tdst_Manager *_pst_TM, TEX_tdst_File_Desc *_pst_Tex, LONG _l_Number, char _c_Interface)
{
    // hogsy:   originally, this compressed the textures based on (poor) assumptions about available VRAM/RAM
    //          in future we'll handle this better, but I doubt we'll go over any limits anytime soon...

	TEX_tdst_File_Desc *pst_CurTex = _pst_Tex;
	TEX_tdst_File_Desc *pst_LastTex = _pst_Tex + _l_Number;

    bool b_AcceptNonSquare = _pst_TM->ul_Flags & TEX_Manager_AcceptNonSquare;
	for ( ; pst_CurTex < pst_LastTex; pst_CurTex++ )
	{
		if ( pst_CurTex->uw_DescFlags & TEX_Cuw_DF_VeryBadBoy ) continue;

		if ( _c_Interface ^ ( ( ( pst_CurTex->st_Params.uw_Flags & TEX_FP_Interface ) && ( _pst_TM->ul_Flags & TEX_Manager_StoreInterfaceTex ) ) ? 1 : 0 ) )
			continue;

		pst_CurTex->uw_CompressedHeight = pst_CurTex->uw_Height;
		pst_CurTex->uw_CompressedWidth  = pst_CurTex->uw_Width;
		pst_CurTex->uc_FinalBPP         = pst_CurTex->uc_BPP;

        TEX_tdst_File_Desc st_GoodTex;
        if ( !TEX_l_DimensionAreGood( pst_CurTex, _pst_TM->l_MinTextureSize, _pst_TM->l_MaxTextureSize, b_AcceptNonSquare, &st_GoodTex ) )
		{
			pst_CurTex->uw_CompressedHeight = st_GoodTex.uw_Height;
			pst_CurTex->uw_CompressedWidth  = st_GoodTex.uw_Width;
		}

		if ( ( pst_CurTex->uc_BPP == 24 ) && ( !( _pst_TM->ul_Flags & TEX_Manager_Accept24bpp ) ) )
			pst_CurTex->uc_FinalBPP = 32;
		else if ( ( pst_CurTex->uc_BPP == 16 ) && ( !( _pst_TM->ul_Flags & TEX_Manager_Accept16bpp_1555 ) ) )
			pst_CurTex->uc_FinalBPP = 32;
		else if ( ( pst_CurTex->uc_BPP == 8 ) || ( pst_CurTex->uc_BPP == 4 ) )
		{
			if ( pst_CurTex->st_Params.uc_Type == TEX_FP_RawPalFile )
			{
				pst_CurTex->uc_FinalBPP = 32;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Manager_ValidateTextures(TEX_tdst_Manager *_pst_TM, TEX_tdst_File_Desc *_pst_Tex, LONG _l_Number)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEX_tdst_File_Desc  *pst_CurTex, *pst_LastTex, st_GoodTex, *pst_OtherTex;
    BOOL                b_NonSquare, b_AlphaPal, b_24, b_8, b_4;
    TEX_tdst_Palette    *pst_Pal;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_CurTex = _pst_Tex;
    pst_LastTex = _pst_Tex + _l_Number;

    b_NonSquare = _pst_TM->ul_Flags & TEX_Manager_AcceptNonSquare;
    b_AlphaPal = _pst_TM->ul_Flags & TEX_Manager_AcceptAlphaPalette;
    b_24 = _pst_TM->ul_Flags & TEX_Manager_Accept24bpp;
    b_8 = _pst_TM->ul_Flags & TEX_Manager_Accept8bpp;
    b_4 = _pst_TM->ul_Flags & TEX_Manager_Accept4bpp;

    for(; pst_CurTex < pst_LastTex; pst_CurTex++)
    {
        if (pst_CurTex->uw_DescFlags & TEX_Cuw_DF_VeryBadBoy) continue;

        pst_CurTex->uw_ValidHeight = pst_CurTex->uw_Height;
        pst_CurTex->uw_ValidWidth = pst_CurTex->uw_Width;
        pst_CurTex->uc_FinalBPP = pst_CurTex->uc_BPP;

        /* palette texture */
        if (pst_CurTex->st_Params.uc_Type == TEX_FP_RawPalFile)
        {
            pst_Pal = TEX_gst_GlobalList.dst_Palette + pst_CurTex->st_Tex.ast_Slot[0].ul_Pal;
            
            if ( ( pst_Pal->uc_Flags & TEX_uc_AlphaPalette ) && !b_AlphaPal )
                pst_CurTex->uc_FinalBPP = 32;
            else
            {
                if ( (pst_Pal->uc_Flags & TEX_uc_Palette16) && ! b_4)
                    pst_CurTex->uc_FinalBPP = 8;
                if ( (pst_CurTex->uc_FinalBPP == 8) && !b_8)
                    pst_CurTex->uc_FinalBPP = 24;
            }

            if (pst_CurTex->uc_FinalBPP > 8)
            {
                if (pst_CurTex->st_Tex.ast_Slot[0].ul_TC != -1)
                    pst_OtherTex = _pst_Tex + pst_CurTex->st_Tex.ast_Slot[0].ul_TC;
                else
                    pst_OtherTex = _pst_Tex + pst_CurTex->st_Tex.ast_Slot[0].ul_Raw;
                
                pst_CurTex->uw_ValidWidth = pst_OtherTex->uw_ValidWidth;
                pst_CurTex->uw_ValidHeight = pst_OtherTex->uw_ValidHeight;
            }
        }

        if ( (pst_CurTex->uc_FinalBPP == 24) && !b_24 )
            pst_CurTex->uc_FinalBPP = 32;

        if(!TEX_l_DimensionAreGood(pst_CurTex, _pst_TM->l_MinTextureSize, _pst_TM->l_MaxTextureSize, b_NonSquare, &st_GoodTex))
        {
            pst_CurTex->uw_ValidHeight = st_GoodTex.uw_Height;
            pst_CurTex->uw_ValidWidth = st_GoodTex.uw_Width;
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SHORT TEX_w_Manager_ChooseGoodInTex( TEX_tdst_Manager *_pst_TM, struct TEX_tdst_File_Desc_ *_pst_Tex )
{
    BOOL    b_Palette;
    ULONG   ul_PaletteCapa;
	int		i_SlotIndex, i_Slot;

    ul_PaletteCapa = _pst_TM->ul_Flags & TEX_Manager_AcceptAllPalette;
	_pst_Tex->w_TexFlags = 0;
	_pst_Tex->w_TexRaw = _pst_Tex->w_TexPal = _pst_Tex->w_TexTC = -1;

#if defined(_XENON_RENDER)
    if (GDI_b_IsXenonGraphics())
    {
        // Xenon native texture present?
        if ((_pst_Tex->st_Tex.st_XeProperties.ul_NativeTexture != 0) &&
            (_pst_Tex->st_Tex.st_XeProperties.ul_NativeTexture != BIG_C_InvalidKey))
        {
            _pst_Tex->w_TexTC    = TEX_w_List_AddTexture(&TEX_gst_GlobalList, _pst_Tex->st_Tex.st_XeProperties.ul_NativeTexture, 1);
            _pst_Tex->w_TexFlags = TEX_TakeTrueColor | TEX_XenonTexture;
            return _pst_Tex->w_TexTC;
        }

        // Xenon original texture to convert?
        if ((_pst_Tex->st_Tex.st_XeProperties.ul_OriginalTexture != 0) &&
            (_pst_Tex->st_Tex.st_XeProperties.ul_OriginalTexture != BIG_C_InvalidKey))
        {
            _pst_Tex->w_TexTC    = TEX_w_List_AddTexture(&TEX_gst_GlobalList, _pst_Tex->st_Tex.st_XeProperties.ul_OriginalTexture, 1);
            _pst_Tex->w_TexFlags = TEX_TakeTrueColor | TEX_XenonNeedsConversion | TEX_XenonTexture;
            return _pst_Tex->w_TexTC;
        }
    }
#endif

	for (i_SlotIndex = 0; i_SlotIndex < 4; i_SlotIndex++ )
	{
		i_Slot = _pst_TM->c_TexSlotOrder[ i_SlotIndex ];
		if (i_Slot == -1) return -1;
		if ( ( _pst_Tex->st_Tex.ast_Slot[ i_Slot ].ul_Pal != -1 ) ||
			 ( _pst_Tex->st_Tex.ast_Slot[ i_Slot ].ul_Raw != -1 ) ||
			 ( _pst_Tex->st_Tex.ast_Slot[ i_Slot ].ul_TC != -1 )
			) break;
			
	}
	if (i_SlotIndex == 4 ) return -1;

    if ( (_pst_Tex->st_Tex.ast_Slot[ i_Slot ].ul_Raw == -1) || (_pst_Tex->st_Tex.ast_Slot[ i_Slot ].ul_Pal == -1) )
        b_Palette = FALSE;
    else if (_pst_Tex->st_Tex.ast_Slot[ i_Slot ].ul_TC == -1)
        b_Palette = TRUE;
    else
    {
        // on a tout ce qu'il faut, faut voir maintenant avec les capacités du hardware
        if (ul_PaletteCapa == TEX_Manager_AcceptAllPalette)
            b_Palette = TRUE;           // le hardware accepte toutes les palettes
        else if (ul_PaletteCapa == 0)
            b_Palette = FALSE;          // le hardware n'accepte aucune palette
        else
        {
            // autres cas possibles à traiter
            //      palette 8 ou 4 mais pas alpha
            //      palette 8 avec ou sans alpha mais pas 4
            //  pour l'instant ca forcerait une lecture supplémentaire
            // a voir après avec les paramètres des refs des tex mis dans les params du tex
            // on force le true color
            b_Palette = FALSE;
        }
    }

    if (b_Palette)
    {
        _pst_Tex->w_TexPal = TEX_w_List_AddPalette( &TEX_gst_GlobalList, _pst_Tex->st_Tex.ast_Slot[ i_Slot ].ul_Pal);
		//_pst_Tex->st_Tex.ast_Slot[ i_Slot ].ul_Pal = (ULONG) _pst_Tex->w_TexPal;

		_pst_Tex->w_TexRaw = TEX_w_List_AddTexture( &TEX_gst_GlobalList, _pst_Tex->st_Tex.ast_Slot[ i_Slot ].ul_Raw, 0 );
        //_pst_Tex->st_Tex.ast_Slot[ i_Slot ].ul_Raw = _pst_Tex->w_TexRaw;

		_pst_Tex->w_TexFlags = TEX_TakeRawPal | ((ul_PaletteCapa != TEX_Manager_AcceptAllPalette) ? TEX_ChangeToTrueColor : 0);

        // keep palette into memory ?
        if ( (_pst_Tex->st_Params.uw_Flags & TEX_FP_KeepPaletteInMem) && (_pst_Tex->st_Tex.ast_Slot[ i_Slot ].ul_Pal != -1) )
			TEX_gst_GlobalList.dst_Palette[ _pst_Tex->w_TexPal ].uc_Flags |= TEX_uc_UpdatablePal | TEX_uc_KeepPalInMem;
        
        //TESTV _pst_Tex->st_Params.ul_Params[0] = _pst_Tex->w_TexRaw;
        //TESTV _pst_Tex->st_Params.ul_Params[1] = _pst_Tex->w_TexPal;
        return _pst_Tex->w_TexRaw;
    }

    if ( _pst_Tex->st_Tex.ast_Slot[ i_Slot ].ul_TC != -1 )
	{
		_pst_Tex->w_TexTC = TEX_w_List_AddTexture( &TEX_gst_GlobalList, _pst_Tex->st_Tex.ast_Slot[0].ul_TC, 1 );
        //_pst_Tex->st_Tex.ast_Slot[ i_Slot ].ul_TC = (ULONG) _pst_Tex->w_TexTC;
	}
		

	_pst_Tex->w_TexFlags = TEX_TakeTrueColor;
    //TESTV _pst_Tex->st_Params.ul_Params[2] = _pst_Tex->w_TexTC;
    return _pst_Tex->w_TexTC; 
}

#ifdef ACTIVE_EDITORS
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Manager_AddInfo( TEX_tdst_Manager *_pst_TM, TEX_tdst_Data *pst_TexData, TEX_tdst_File_Desc *_pst_Tex )
{
    char    c_Bpp, c_Width, c_Height;
    USHORT  uw_Value;

    c_Bpp = _pst_Tex->uc_FinalBPP >> 3;
#ifdef JADEFUSION
	if (c_Bpp > 4)
    {
        c_Bpp = 4;
    }
#endif
    c_Width = 0;
    uw_Value = _pst_Tex->uw_CompressedWidth;
    while (!(uw_Value & 1) )
    {
        c_Width++;
        if (c_Width == 10) break;
        uw_Value >>= 1;
    }
    c_Width = (c_Width < 3) ? 0 : c_Width - 3;

    c_Height = 0;
    uw_Value = _pst_Tex->uw_CompressedHeight;
    while (!(uw_Value & 1) )
    {
        c_Height++;
        if (c_Height == 10) break;
        uw_Value >>= 1;
    }
    c_Height = (c_Height < 3) ? 0 : c_Height - 3;

    _pst_TM->al_TextureNumberByBpp[c_Bpp]++;
    _pst_TM->aaal_TextureNumberByBppWidthHeight[c_Bpp][c_Width][c_Height]++;

    /* interface texture information */
    if (_pst_Tex->st_Params.uw_Flags & TEX_FP_Interface)
    {
        if (_pst_TM->l_NbTextureInterface < 64)
        {
            _pst_TM->aul_TextureInterface[ _pst_TM->l_NbTextureInterface ] = pst_TexData->ul_Key;
            _pst_TM->auw_TIH[ _pst_TM->l_NbTextureInterface ] = _pst_Tex->uw_Height;
            _pst_TM->auw_TIW[ _pst_TM->l_NbTextureInterface ] = _pst_Tex->uw_Width;
            _pst_TM->auw_TICH[ _pst_TM->l_NbTextureInterface ] = _pst_Tex->uw_CompressedHeight;
            _pst_TM->auw_TICW[ _pst_TM->l_NbTextureInterface ] = _pst_Tex->uw_CompressedWidth;
            _pst_TM->l_NbTextureInterface++;
        }
    }

}
#endif

