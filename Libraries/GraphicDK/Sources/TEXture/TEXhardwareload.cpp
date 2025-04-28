/*$T TEXhardwareload.c GC! 1.081 09/04/00 14:51:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/MEMory/MEM.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "TEXture/TEXstruct.h"
#include "TEXture/TEXconvert.h"
#include "TEXture/TEXmemory.h"
#include "BIGfiles/BIGmdfy_file.h"

#ifdef JADEFUSION
#include "TEXture/TEXxenon.h"
#endif

#ifdef ACTIVE_EDITORS
#include "BIGfiles/BIGread.h"
#ifdef JADEFUSION
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#endif


#ifdef PSX2_TARGET
//mamagouille
#include "PSX2debug.h"
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

extern BOOL LOA_gb_SpeedMode;
#ifdef JADEFUSION
extern BOOL	EDI_gb_ComputeMap;
#endif
/*
 =======================================================================================================================
    Aim:    Create a texture from given texture data
 =======================================================================================================================
 */
void TEX_Hardware_Load(GDI_tdst_DisplayData *_pst_DD, TEX_tdst_Data *_pst_TexData, TEX_tdst_File_Desc *_pst_Tex, int _i_Texture )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_File_Desc	st_NewTex;
	TEX_tdst_File_Desc	*pst_Tex;
	ULONG				ul_Size;
	LONG				l_InMemory;
	int					iCpt;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	PROPS2_StartRaster(&PROPS2_gst_OGL_ul_Texture_Create);

	l_InMemory = 0;

	if(_pst_Tex->uw_DescFlags & TEX_Cuw_DF_SpecialNoLoad)
	{
        /* procedural */
		ul_Size = (LONG) _pst_Tex->uw_CompressedWidth * (LONG) _pst_Tex->uw_CompressedHeight;
		ul_Size = (ul_Size * _pst_Tex->uc_FinalBPP) >> 3;
		PROPS2_StartRaster(&PROPS2_gst_TEX_M_File_Alloc);
		TEX_M_File_Alloc(_pst_Tex->p_Bitmap, ul_Size, void);
		PROPS2_StopRaster(&PROPS2_gst_TEX_M_File_Alloc);
		pst_Tex = _pst_Tex;
		goto hardwareload_endofload;
	}

	/* 
     * watch if bitmap is already in memory 
     * force reload if flag TEX_Manager_ForceReload is set
     * or if dimension of bitmap loaded are not the dimension needed
     */
	PROPS2_StartRaster(&PROPS2_gst_TEX_pst_Memory_FindTexture);
	pst_Tex = TEX_pst_Memory_FindTexture(_pst_TexData->ul_Key);
	PROPS2_StopRaster(&PROPS2_gst_TEX_pst_Memory_FindTexture);
	if(pst_Tex != NULL)
	{
		if
		(
			(pst_Tex->uw_CompressedHeight != _pst_Tex->uw_CompressedHeight)
		||	(pst_Tex->uw_CompressedWidth != _pst_Tex->uw_CompressedWidth)
		||	(_pst_DD->st_TexManager.ul_Flags & TEX_Manager_ForceReload)
		)
		{
			PROPS2_StartRaster(&PROPS2_gst_TEX_l_Memory_DeleteTexture);
			TEX_l_Memory_DeleteTexture(_pst_TexData->ul_Key);
			PROPS2_StopRaster(&PROPS2_gst_TEX_l_Memory_DeleteTexture);
		}
		else
			l_InMemory = 1;
	}

	/* sprite generator special code */
	if(_pst_Tex->uw_FileFlags & TEX_uw_IsSpriteGen)
	{
		_pst_TexData->uw_Flags = _pst_Tex->uw_FileFlags;
		*(void **) &_pst_TexData->w_Width = _pst_Tex->p_Bitmap;
		PROPS2_StopRaster(&PROPS2_gst_OGL_ul_Texture_Create);
		goto mustrealalloc;
	}

    /* load and transform bitmap if not in memory */
	if(l_InMemory == 0)
	{
		/*~~*/
		int i;
		/*~~*/

		GDI_M_TimerStart(GDI_f_Delay_AttachWorld_TextureCreate_GetContent);
		PROPS2_StartRaster(&PROPS2_gst_TEX_l_File_GetContent);

        /* special load for RawPal file */
        if (_pst_Tex->st_Params.uc_Type == TEX_FP_RawPalFile)
        {
            /* If a raw pal texture comes here that's mean that she is not supported by hardware */
            /* and bitmap is already loaded */
            i = 1;
        }
        else
        {
		    i = (int) TEX_l_File_GetContent(_pst_TexData->ul_Key, _pst_Tex);
        }
		    PROPS2_StopRaster(&PROPS2_gst_TEX_l_File_GetContent);
		if(i == 0)
		{
			GDI_M_TimerStop(GDI_f_Delay_AttachWorld_TextureCreate_GetContent);
			PROPS2_StopRaster(&PROPS2_gst_OGL_ul_Texture_Create);
			goto mustrealalloc;
		}

		GDI_M_TimerStop(GDI_f_Delay_AttachWorld_TextureCreate_GetContent);

		GDI_M_TimerStart(GDI_f_Delay_AttachWorld_TextureCreate_Compress);
		if
		(
			(_pst_Tex->uw_Width != _pst_Tex->uw_CompressedWidth)
		||	(_pst_Tex->uw_CompressedHeight != _pst_Tex->uw_Height)
		)
		{
#if defined(_XENON_RENDER)
            // Do not alter DDS files
            if (_pst_Tex->uc_FinalBPP != 0x80)
#endif
			{
			if(!(_pst_DD->st_TexManager.ul_Flags & TEX_Manager_FastCompression) || !TEX_l_ConvertSizeFast(_pst_Tex))
			{
				ul_Size = (LONG) _pst_Tex->uw_CompressedWidth * (LONG) _pst_Tex->uw_CompressedHeight;
				ul_Size = (ul_Size * _pst_Tex->uc_FinalBPP) >> 3;

				L_memcpy(&st_NewTex, _pst_Tex, sizeof(TEX_tdst_File_Desc));

				st_NewTex.uw_Width = st_NewTex.uw_CompressedWidth;
				st_NewTex.uw_Height = st_NewTex.uw_CompressedHeight;
				PROPS2_StartRaster(&PROPS2_gst_TEX_M_File_Alloc);
				TEX_M_File_Alloc(st_NewTex.p_Bitmap, ul_Size, void);
				PROPS2_StopRaster(&PROPS2_gst_TEX_M_File_Alloc);
				L_memset(st_NewTex.p_Bitmap, 0, ul_Size);

				PROPS2_StartRaster(&PROPS2_gst_TEX_l_ConvertSize);
				TEX_l_ConvertSize(_pst_Tex, &st_NewTex);
				PROPS2_StopRaster(&PROPS2_gst_TEX_l_ConvertSize);

				PROPS2_StartRaster(&PROPS2_gst_TEX_File_FreeDescription);
				TEX_File_FreeDescription(_pst_Tex);
				PROPS2_StopRaster(&PROPS2_gst_TEX_File_FreeDescription);

				L_memcpy(_pst_Tex, &st_NewTex, sizeof(TEX_tdst_File_Desc));
			}
		}
		}
		GDI_M_TimerStop(GDI_f_Delay_AttachWorld_TextureCreate_Compress);

        /* change eventually color depth */
        if (_pst_Tex->uc_BPP != _pst_Tex->uc_FinalBPP )
        {
            iCpt = _pst_Tex->uw_Width * _pst_Tex->uw_Height;
            GDI_M_TimerStart(GDI_f_Delay_AttachWorld_TextureCreate_Convert24To32);
            if (_pst_Tex->uc_BPP == 24) 
            {
                if (_pst_Tex->uc_FinalBPP == 32)
                {
#ifdef PSX2_TARGET
					if(LOA_gb_SpeedMode)
					{
						TEX_Convert_24To32((ULONG*)_pst_Tex->p_Bitmap, iCpt);
					}
					else
					{
						if(_pst_DD->st_TexManager.ul_Flags & TEX_Manager_RGB2BGR)
						{
							TEX_Convert_24To32SwapRB((ULONG*)_pst_Tex->p_Bitmap, iCpt);
						}
						else
							TEX_Convert_24To32((ULONG*)_pst_Tex->p_Bitmap, iCpt);
					}
#else
	#ifdef JADEFUSION
						if (!EDI_gb_ComputeMap && (_pst_DD->st_TexManager.ul_Flags & TEX_Manager_RGB2BGR))
	#else
						if (_pst_DD->st_TexManager.ul_Flags & TEX_Manager_RGB2BGR)
	#endif
							TEX_Convert_24To32SwapRB((ULONG*)_pst_Tex->p_Bitmap, iCpt);
						else
							TEX_Convert_24To32((ULONG*)_pst_Tex->p_Bitmap, iCpt);
#endif
                }
            }
            GDI_M_TimerStop(GDI_f_Delay_AttachWorld_TextureCreate_Convert24To32);
        }
        else 
        {
            GDI_M_TimerStart(GDI_f_Delay_AttachWorld_TextureCreate_Convert32Colors);
	#ifdef JADEFUSION
			if (!EDI_gb_ComputeMap && (_pst_DD->st_TexManager.ul_Flags & TEX_Manager_RGB2BGR))
	#else
		   if (_pst_DD->st_TexManager.ul_Flags & TEX_Manager_RGB2BGR)
	#endif
		   {
                iCpt = _pst_Tex->uw_Width * _pst_Tex->uw_Height;
#ifdef PSX2_TARGET
                if( (_pst_Tex->uc_BPP == 32) && !LOA_gb_SpeedMode)
                {
                    TEX_Convert_32SwapRB((ULONG*)_pst_Tex->p_Bitmap, iCpt);
                }
#else
                if (_pst_Tex->uc_BPP == 32)
                    TEX_Convert_32SwapRB((ULONG*)_pst_Tex->p_Bitmap, iCpt);
#endif
            }
            GDI_M_TimerStop(GDI_f_Delay_AttachWorld_TextureCreate_Convert32Colors);
        }
		pst_Tex = _pst_Tex;
	}

hardwareload_endofload:

    _pst_TexData->w_Width = (short) pst_Tex->uw_CompressedWidth;
    _pst_TexData->w_Height = (short) pst_Tex->uw_CompressedHeight;

#ifdef ACTIVE_EDITORS
	{
		BIG_INDEX		ul_File;
		char			az_Path[BIG_C_MaxLenPath];
		char			az_File[BIG_C_MaxLenName];
		TEX_tdst_File_Desc	t_Desc;
		extern BOOL		EDI_gb_ComputeMap;
#ifdef JADEFUSION
		extern unsigned int	WOR_gul_WorldKey;
#else
		extern ULONG	WOR_gul_WorldKey;
#endif
		ULONG			ul_World;
		char			*pz;

		if(EDI_gb_ComputeMap)
		{
			ul_World = BIG_ul_SearchKeyToFat(WOR_gul_WorldKey);
			if(ul_World != BIG_C_InvalidIndex)
			{
#if defined(_XENON_RENDER)
                sprintf(az_Path, "%s", TEX_Csz_X360TexturesPath);
                sprintf(az_File, "0x%08x.xtx", _pst_TexData->ul_Key);
#else
				BIG_ComputeFullName(BIG_ParentFile(ul_World), az_Path);
				L_strcat(az_Path, "/BinTextures");
				sprintf(az_File, "%x", _pst_TexData->ul_Key);
#endif
				ul_File = BIG_ul_SearchKeyToFat(_pst_TexData->ul_Key);
				if(ul_File != BIG_C_InvalidIndex)
				{
					L_memcpy(&t_Desc, pst_Tex, sizeof(TEX_tdst_File_Desc));
					t_Desc.uw_Width = _pst_TexData->w_Width;
					t_Desc.uw_Height = _pst_TexData->w_Height;
					t_Desc.uc_BPP = t_Desc.uc_FinalBPP;
					t_Desc.st_Params.uw_Width = t_Desc.uw_Width;
					t_Desc.st_Params.uw_Height = t_Desc.uw_Height;
					pz = L_strrchr(BIG_NameFile(ul_File), '.');
#ifdef JADEFUSION
                    if (_pst_Tex->w_TexFlags & TEX_XenonConvert)
                    {
                        ULONG ul_TempKey = BIG_ul_SearchFileExt(az_Path, az_File);
                        if (ul_TempKey == BIG_C_InvalidKey)
                        {
                            TEX_XeGenerateDDS(&_pst_Tex->st_Tex.st_XeProperties, az_Path, az_File);
                        }
                    }
                    else
                    {
                        if(pz && !L_strcmpi(pz, ".raw"))
                        {
                            TEX_ul_File_SaveRawInBF(az_Path, az_File, &t_Desc, TRUE);
                        }
                        else if(pz && !L_strcmpi(pz, ".tga"))
                        {
                            TEX_ul_File_SaveTgaInBF(az_Path, az_File, &t_Desc);
                        }
                        else if(pz && !L_strcmpi(pz, ".dds"))
                        {
                            ULONG ul_Pos = BIG_ul_SearchKeyToPos(_pst_TexData->ul_Key);
                            ULONG ul_Size;
                            CHAR* pBuffer = BIG_pc_ReadFileTmpMustFree(ul_Pos, &ul_Size);

                            SAV_Begin(az_Path, az_File);
                            SAV_Buffer(pBuffer, ul_Size);
                            SAV_ul_End();

                            L_free(pBuffer);
                        }
                    }
#else
					if(pz && !L_strcmpi(pz, ".raw"))
					{
						TEX_ul_File_SaveRawInBF(az_Path, az_File, &t_Desc, TRUE);
					}
					else if(pz && !L_strcmpi(pz, ".tga"))
					{
						TEX_ul_File_SaveTgaInBF(az_Path, az_File, &t_Desc);
					}
#endif				
				}
			}
		}
	}
#endif

    if (!(_pst_DD->st_TexManager.ul_Flags & TEX_Manager_UseMipmap))
        _pst_Tex->st_Params.uw_Flags &= ~TEX_FP_MipmapOn;

    if ( ( _pst_Tex->st_Params.uw_Flags & TEX_FP_Interface) && (_pst_DD->st_TexManager.ul_Flags & TEX_Manager_StoreInterfaceTex) )
        if (_pst_DD->st_GDI.pfnl_StoreTexture( _pst_DD, _pst_TexData, pst_Tex, _i_Texture ) )
            goto mustrealalloc;

	_pst_DD->st_GDI.pfnv_LoadTexture( _pst_DD, _pst_TexData, pst_Tex, _i_Texture );

	if(l_InMemory == 0)
	{
		if ( (_pst_Tex->uw_DescFlags & TEX_Cuw_DF_SpecialNoLoad) ||	(TEX_l_Memory_AddTexture(_pst_TexData->ul_Key, pst_Tex) == 0) ) 
			TEX_File_FreeDescription(pst_Tex);
	    else
		    goto mustrealalloc;
    }
    else
	    goto mustrealalloc;

	PROPS2_StopRaster(&PROPS2_gst_OGL_ul_Texture_Create);
	return;

mustrealalloc:
	pst_Tex = pst_Tex;
	return;
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
