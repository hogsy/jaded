/*$T SOFTbackgroundimage.c GC! 1.081 05/19/00 09:32:51 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGread.h"
#include "../Main/WinEditors/Sources/EDIpaths.h"

#include "MATHs/MATH.h"

#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/ANImation/ANIinit.h"

#include "GRObject/GROedit.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOsubobject.h"
#include "MATerial/MATstruct.h"
#include "MATerial/MATmultitexture.h"
#include "TEXture/TEXconvert.h"

#include "ENGine/Sources/WORld/WORstruct.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"
#include "SOFT/SOFTbackgroundimage.h"

#include "Mpeglib/mpg_export.h"

//#define USE_WEBCAM
#ifdef USE_WEBCAM
extern int  WEBCAM_INIT(MPG_IOStruct *p_IO);
extern int  WEBCAM_FRAME(void);
extern void WEBCAM_GOTO(int ulNum);
extern int  WEBCAM_END(void);
#define STREAMER(a) WEBCAM_##a
#else
#define STREAMER(a) MPEG_##a
#endif

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

int									SOFT_i_Init = 0;
short                               SOFT_gw_BI_Texture = -1;

SOFT_tdst_BackgroundImage			*SOFT_pst_CurrentBI;

GEO_tdst_Object						SOFT_BI_Obj;
GEO_tdst_ElementIndexedTriangles	SOFT_BI_Element;
GEO_tdst_IndexedTriangle			SOFT_BI_Triangle[2];
GEO_Vertex					        SOFT_BI_Points[4];
GEO_tdst_UV							SOFT_BI_UV[4];

MAT_tdst_MultiTexture				*SOFT_BI_Mat;
MAT_tdst_MTLevel					*SOFT_BI_MatLevel;

/*$4
 ***********************************************************************************************************************
    General Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_BackgroundImage_MainInit(void)
{
	if(SOFT_i_Init) return;
	SOFT_i_Init = 1;

	L_memset(&SOFT_BI_Obj, 0, sizeof(GEO_tdst_Object));
	SOFT_BI_Obj.st_Id.i = &GRO_gast_Interface[GRO_Geometric];
	SOFT_BI_Obj.dst_Element = &SOFT_BI_Element;
	SOFT_BI_Obj.l_NbElements = 1;
	SOFT_BI_Obj.l_NbPoints = 4;
	SOFT_BI_Obj.dst_Point = SOFT_BI_Points;
	SOFT_BI_Obj.l_NbUVs = 4;
	SOFT_BI_Obj.dst_UV = SOFT_BI_UV;

	SOFT_BI_Obj.dst_UV[0].fV = 1;
	SOFT_BI_Obj.dst_UV[1].fU = 1;
	SOFT_BI_Obj.dst_UV[2].fU = 1;
	SOFT_BI_Obj.dst_UV[2].fV = 1;

	SOFT_BI_Triangle[0].auw_Index[0] = 0;
	SOFT_BI_Triangle[0].auw_Index[1] = 1;
	SOFT_BI_Triangle[0].auw_Index[2] = 2;
	SOFT_BI_Triangle[0].auw_UV[0] = 3;
	SOFT_BI_Triangle[0].auw_UV[1] = 1;
	SOFT_BI_Triangle[0].auw_UV[2] = 2;
	SOFT_BI_Triangle[1].auw_Index[0] = 0;
	SOFT_BI_Triangle[1].auw_Index[1] = 2;
	SOFT_BI_Triangle[1].auw_Index[2] = 3;
	SOFT_BI_Triangle[1].auw_UV[0] = 3;
	SOFT_BI_Triangle[1].auw_UV[1] = 2;
	SOFT_BI_Triangle[1].auw_UV[2] = 0;

	SOFT_BI_Element.dst_Triangle = SOFT_BI_Triangle;
	SOFT_BI_Element.l_MaterialId = 0;
	SOFT_BI_Element.l_NbTriangles = 2;
	SOFT_BI_Element.p_MrmElementAdditionalInfo = NULL;
	SOFT_BI_Element.pus_ListOfUsedIndex = NULL;
#ifdef GSP_PS2
	SOFT_BI_Element.p_GSP_TO = NULL;
#endif
	
	SOFT_BI_Element.ul_NumberOfUsedIndex = 0;

	SOFT_BI_Mat = MAT_pst_CreateMultiTexture("");
	SOFT_BI_MatLevel = (MAT_tdst_MTLevel *) L_malloc(sizeof(MAT_tdst_MTLevel));
	L_memset(SOFT_BI_MatLevel, 0, sizeof(MAT_tdst_MTLevel));
	SOFT_BI_Mat->f_Opacity = 0.5;
	SOFT_BI_Mat->pst_FirstLevel = SOFT_BI_MatLevel;
	SOFT_BI_Mat->ul_Ambiant = 0xFFFFFFFF;
	SOFT_BI_Mat->ul_Specular = 0xFFFFFFFF;
	SOFT_BI_Mat->ul_ValidateMask = MAT_ValidateMask_EnalbleAll;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_BackgroundImage_MainClose(void)
{
	if(!SOFT_i_Init) return;
	SOFT_i_Init = 0;
    SOFT_BI_Mat->pst_FirstLevel = NULL;
	SOFT_BI_Mat->st_Id.i->pfn_Destroy(SOFT_BI_Mat);
	L_free(SOFT_BI_MatLevel);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_BackGroundImage_SetTextureInfo( SOFT_tdst_BackgroundImage *_pst_BI, TEX_tdst_File_Desc*_pst_TexDesc )
{
    TEX_tdst_File_Desc *pst_Tex;

    _pst_BI->w_Texture = SOFT_gw_BI_Texture;
    if(_pst_BI->w_Texture != -1)
	{
	    pst_Tex = _pst_TexDesc + _pst_BI->w_Texture;
		pst_Tex->uw_Width = 256;
		pst_Tex->uw_Height = 256;
		pst_Tex->uc_BPP = 32;
		pst_Tex->uw_DescFlags &= ~TEX_Cuw_DF_VeryBadBoy;
		pst_Tex->uw_DescFlags |= TEX_Cuw_DF_SpecialNoLoad;
		pst_Tex->st_Params.uw_Flags &= ~TEX_FP_MipmapOn;
		pst_Tex->st_Params.uw_Flags |= TEX_FP_QualityVeryHigh;
        pst_Tex->st_Params.uw_Width = 256;
        pst_Tex->st_Params.uw_Height = 256;
        pst_Tex->st_Params.uc_Format = TEX_FP_32bpp;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_BackGroundImage_SetAnimGao( SOFT_tdst_BackgroundImage *_pst_BI, struct OBJ_tdst_GameObject_ *_pst_GO )
{
    _pst_BI->pst_Gao = _pst_GO;
}


/*$4
 ***********************************************************************************************************************
    mpeg functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_BI_OutBF_GoStart(void)
{
	if(SOFT_pst_CurrentBI->hp_File) fclose(SOFT_pst_CurrentBI->hp_File);
	SOFT_pst_CurrentBI->hp_File = fopen(SOFT_pst_CurrentBI->sz_MpegFile, "rb");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SOFT_i_BI_OutBF_Read2048(int Ptr)
{
	return fread((void *) Ptr, 1, 2048, SOFT_pst_CurrentBI->hp_File);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_BI_InBF_GoStart(void)
{
	SOFT_pst_CurrentBI->ul_MpegOffset = BIG_PosFile(SOFT_pst_CurrentBI->ul_MpegIndex) + 4;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SOFT_i_BI_InBF_Read2048(int Ptr)
{
	/*~~~~~~~*/
	int i_Size;
	/*~~~~~~~*/

	i_Size = SOFT_pst_CurrentBI->ul_MpegEndOffset - SOFT_pst_CurrentBI->ul_MpegOffset;
	if(i_Size > 2048) i_Size = 2048;
	BIG_Read(SOFT_pst_CurrentBI->ul_MpegOffset, (void *) Ptr, i_Size);
	SOFT_pst_CurrentBI->ul_MpegOffset += i_Size;
	return i_Size;
}

/*$4
 ***********************************************************************************************************************
    structure functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern "C" void SOFT_BackgroundImage_AddTexture( void )
{
    SOFT_gw_BI_Texture = TEX_w_List_AddTexture(&TEX_gst_GlobalList, 0xFFFFFFF0, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_BackgroundImage_Init(SOFT_tdst_BackgroundImage *_pst_BI)
{
    if (!_pst_BI) return;
	L_memset(_pst_BI, 0, sizeof(SOFT_tdst_BackgroundImage));

	_pst_BI->ul_Flags = SOFT_C_BI_Init;

	_pst_BI->ul_PLKey = (ULONG) - 1;
	_pst_BI->ul_PLLastKey = (ULONG) - 1;
	_pst_BI->ul_MpegKey = (ULONG) - 1;

	SOFT_BackGroundImage_MoveWindow(_pst_BI, 0, 0, .4f, .4f, 1.0f);
    SOFT_BackgroundImage_AddTexture();

    _pst_BI->w_Texture = SOFT_gw_BI_Texture;

	_pst_BI->pst_MpegIO = (MPG_IOStruct *) L_malloc(sizeof(MPG_IOStruct));
	L_memset(_pst_BI->pst_MpegIO, 0, sizeof(MPG_IOStruct));
	_pst_BI->pst_MpegIO->ulFlags = MPEG_Mode_VerticalFlip | MPEG_Mode_SwapRedBlue;

    _pst_BI->w_AnimFreq = 60;
    _pst_BI->w_FilmFreq = 60;
    _pst_BI->f_FreqRatio = 1;

    _pst_BI->ul_Key = 0xFFFFFFFF;
    L_strcpy( _pst_BI->sz_Name, "BackImage" );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_BackgroundImage_Free(SOFT_tdst_BackgroundImage *_pst_BI)
{
	/*~~*/
	int i;
	/*~~*/

	if(_pst_BI->i_NumberOfImages)
	{
		for(i = 0; i < _pst_BI->i_NumberOfImages; i++)
		{
			if(_pst_BI->dpc_Image[i]) L_free(_pst_BI->dpc_Image[i]);
		}

		L_free(_pst_BI->dpc_Image);
		_pst_BI->i_Number = _pst_BI->i_NumberOfImages = 0;
		_pst_BI->dpc_Image = NULL;
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_BackgroundImage_Close(SOFT_tdst_BackgroundImage *_pst_BI)
{
	if(!(_pst_BI->ul_Flags & SOFT_C_BI_Init)) return;

	L_free(_pst_BI->pst_MpegIO);
    SOFT_BackgroundImage_Free( _pst_BI );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SOFT_i_BackgroundImage_TestData(SOFT_tdst_BackgroundImage *_pst_BI)
{
	if(_pst_BI->ul_Flags & SOFT_C_BI_NotInBF)
	{
		if(_pst_BI->ul_Flags & SOFT_C_BI_Mpeg)
		{
			if(*_pst_BI->sz_MpegFile == 0) return 0;
		}
		else
		{
			if(*_pst_BI->sz_PLFile == 0) return 0;
		}
	}
	else
	{
		if(_pst_BI->ul_Flags & SOFT_C_BI_Mpeg)
		{
			if(_pst_BI->ul_MpegKey == (ULONG) - 1) return 0;
		}
		else
		{
			if(_pst_BI->ul_PLKey == (ULONG) - 1) return 0;
		}
	}

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_BackgroundImage_Create(SOFT_tdst_BackgroundImage *_pst_BI, GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_Data		*pst_Tex;
	TEX_tdst_File_Desc	st_TexDesc, st_TexDesc2;
	int					i;
	ULONG				ul_Size, ul_Color, *pul_Pixel, *pul_Last;
	L_FILE				hp_File;
	ULONG				ul_SizeOfBuffer, ul_TempSize;
	char				*pc_Buffer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    _pst_BI->w_Texture = SOFT_gw_BI_Texture;
	pst_Tex = &TEX_gst_GlobalList.dst_Texture[_pst_BI->w_Texture];
	ul_Size = pst_Tex->w_Width;
	ul_Size *= pst_Tex->w_Height;
	ul_Size *= 4;

	SOFT_BackgroundImage_Free(_pst_BI);

	if(!SOFT_i_BackgroundImage_TestData(_pst_BI)) return;

	if(_pst_BI->ul_Flags & SOFT_C_BI_Mpeg)
	{
		SOFT_pst_CurrentBI = _pst_BI;
		_pst_BI->i_Number = 1;
		_pst_BI->dpc_Image = (char **) L_malloc(4 * _pst_BI->i_Number);
		_pst_BI->dpc_Image[0] = (char *) L_malloc(ul_Size);
		if(_pst_BI->ul_Flags & SOFT_C_BI_NotInBF)
		{
			_pst_BI->pst_MpegIO->GoStart = SOFT_BI_OutBF_GoStart;
			_pst_BI->pst_MpegIO->Read2048 = SOFT_i_BI_OutBF_Read2048;
		}
		else
		{
			_pst_BI->pst_MpegIO->GoStart = SOFT_BI_InBF_GoStart;
			_pst_BI->pst_MpegIO->Read2048 = SOFT_i_BI_InBF_Read2048;
			_pst_BI->ul_MpegIndex = BIG_ul_SearchKeyToFat(_pst_BI->ul_MpegKey);
			_pst_BI->ul_MpegEndOffset = BIG_PosFile(_pst_BI->ul_MpegIndex);
			_pst_BI->ul_MpegEndOffset += BIG_ul_GetLengthFile(BIG_PosFile(_pst_BI->ul_MpegIndex)) + 4;
		}

		STREAMER(INIT)(_pst_BI->pst_MpegIO);

		_pst_BI->pst_MpegIO->ulFlags |= MPEG_Mode_SuperFast;
		ul_Size = 0;
		while(1)
		{
			STREAMER(FRAME)();
			if(_pst_BI->pst_MpegIO->ulCurrentFrame == 0) break;
			ul_Size = _pst_BI->pst_MpegIO->ulCurrentFrame;
		}

		_pst_BI->i_Number = ul_Size;
		STREAMER(GOTO)(0);
		_pst_BI->pst_MpegIO->ulFlags &= ~MPEG_Mode_SuperFast;

        _pst_BI->ImageRatio = (float) _pst_BI->pst_MpegIO->SizeX / (float) _pst_BI->pst_MpegIO->SizeY;
	}
	else
	{
		if(_pst_BI->ul_Flags & SOFT_C_BI_NotInBF)
			_pst_BI->i_Number = _pst_BI->i_NumberOfImages = _pst_BI->i_LastIndex - _pst_BI->i_FirstIndex + 1;
		else
			_pst_BI->i_Number = _pst_BI->i_NumberOfImages = _pst_BI->ul_PLLastKey - _pst_BI->ul_PLKey + 1;
		_pst_BI->dpc_Image = (char **) L_malloc(4 * _pst_BI->i_Number);
		pc_Buffer = NULL;
		ul_SizeOfBuffer = 0;

		for(i = 0; i < _pst_BI->i_Number; i++)
		{
			if(_pst_BI->ul_Flags & SOFT_C_BI_NotInBF)
			{
				sprintf(_pst_BI->sz_MpegFile, _pst_BI->sz_PLFile, _pst_BI->i_FirstIndex + i);
				hp_File = L_fopen(_pst_BI->sz_MpegFile, "rb");

                if (!hp_File)
                {
                    _pst_BI->dpc_Image[i] = NULL;
                    continue;
                }

				L_fseek(hp_File, 0, SEEK_END);
				ul_TempSize = L_ftell(hp_File);
				L_fseek(hp_File, 0, SEEK_SET);
				if(ul_TempSize > ul_SizeOfBuffer)
				{
					if(ul_SizeOfBuffer) L_free(pc_Buffer);
					ul_SizeOfBuffer = ul_TempSize;
					pc_Buffer = (char*)L_malloc(ul_SizeOfBuffer);
				}

				L_fread(pc_Buffer, ul_TempSize, 1, hp_File);
				L_fclose(hp_File);
				L_memset(&st_TexDesc, 0, sizeof(TEX_tdst_File_Desc));
				st_TexDesc.uw_DescFlags = TEX_Cuw_DF_Info | TEX_Cuw_DF_Content;
				if ( !TEX_l_File_LoadTga(pc_Buffer, &st_TexDesc) )
                {
                    _pst_BI->dpc_Image[i] = NULL;
                    continue;
                }
			}
			else
			{
				if ( !TEX_l_File_GetInfoAndContent(_pst_BI->ul_PLKey + i, &st_TexDesc) )
                {
                    _pst_BI->dpc_Image[i] = NULL;
                    continue;
                }
			}

			L_memcpy(&st_TexDesc2, &st_TexDesc, sizeof(TEX_tdst_File_Desc));
			st_TexDesc.p_Bitmap = L_malloc(ul_Size);
			L_memset(st_TexDesc.p_Bitmap, 0, ul_Size);

			if(((SHORT) st_TexDesc.uw_Width != pst_Tex->w_Width) || ((SHORT) st_TexDesc.uw_Height != pst_Tex->w_Height))
			{
				st_TexDesc.uw_Width = (USHORT) pst_Tex->w_Width;
				st_TexDesc.uw_Height = (USHORT) pst_Tex->w_Height;
				st_TexDesc.uc_BPP = 32;
				TEX_l_ConvertSize_24To32(&st_TexDesc2, &st_TexDesc);
			}
			else
			{
				st_TexDesc.uc_BPP = 32;
				TEX_l_ConvertPF(&st_TexDesc2, &st_TexDesc);
			}

			/* RGB -> BGR */
			pul_Pixel = (ULONG*)st_TexDesc.p_Bitmap;
			pul_Last = (ULONG *) (((char *) pul_Pixel) + ul_Size);
			while(pul_Pixel < pul_Last)
			{
				ul_Color = (*pul_Pixel & 0xFF) << 16;
				ul_Color |= (*pul_Pixel & 0xFF00FF00);
				ul_Color |= (*pul_Pixel & 0xFF0000) >> 16;
				*pul_Pixel++ = ul_Color;
			}

			TEX_File_FreeDescription(&st_TexDesc2);
			_pst_BI->dpc_Image[i] = (char*)st_TexDesc.p_Bitmap;
		}

        _pst_BI->ImageRatio = (float) st_TexDesc2.uw_Width / (float) st_TexDesc.uw_Height;

		if(pc_Buffer) L_free(pc_Buffer);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_BackGroundImage_MoveWindow(SOFT_tdst_BackgroundImage *_pst_BI, float x, float y, float w, float h, float z)
{
	if(x > -1)
	{
		_pst_BI->x = x;
		_pst_BI->y = y;
	}

	if(_pst_BI->w > -1)
	{
		_pst_BI->w = w;
		_pst_BI->h = h;
	}

	_pst_BI->depth = 1.0f;
}

/*$4
 ***********************************************************************************************************************
    display functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_BackgroundImage_Update(SOFT_tdst_BackgroundImage *_pst_BI, GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GDI_tdst_Request_ReloadTextureParams	st_RTP;
	ULONG									ul_SaveDD;
	ULONG									ul_SaveFC;
	int										i_OldCounter;
	TEX_tdst_File_Desc						st_Src, st_Tgt;
    CAM_tdst_Camera                         st_Cam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    GDI_gpst_CurDD = _pst_DD;

	SOFT_BackgroundImage_MainInit();
    _pst_BI->w_Texture = SOFT_gw_BI_Texture;

	if(_pst_BI->ul_Flags & SOFT_C_BI_Reload)
	{
		SOFT_BackgroundImage_Create(_pst_BI, _pst_DD);
		_pst_BI->ul_Flags &= ~SOFT_C_BI_Reload;
	}

	if(!(_pst_BI->ul_Flags & SOFT_C_BI_Visible)) return;
	if(!SOFT_i_BackgroundImage_TestData(_pst_BI)) return;

    i_OldCounter = _pst_BI->i_Counter;
    if(!(_pst_BI->ul_Flags & SOFT_C_BI_Freeze))
    {
	    if (_pst_BI->pst_Gao)
        {
            _pst_BI->i_Counter = (int) (ANI_i_CurFrame(_pst_BI->pst_Gao) * _pst_BI->f_FreqRatio);
            if (_pst_BI->i_Counter >= _pst_BI->i_Number) 
                _pst_BI->i_Counter = _pst_BI->i_Number -1;
        }
        else
        {
            _pst_BI->i_Counter = ((int) ( TIM_f_Clock_TrueRead() * (float) _pst_BI->w_FilmFreq)) % _pst_BI->i_Number;
	    }
    }

	if ( (_pst_BI->i_Counter != i_OldCounter) && (_pst_BI->dpc_Image) )
	{
		if(_pst_BI->ul_Flags & SOFT_C_BI_Mpeg)
		{
			SOFT_pst_CurrentBI = _pst_BI;

            if (_pst_BI->ul_Flags & SOFT_C_BI_MPEG_BlackAndWhite)
                _pst_BI->pst_MpegIO->ulFlags |= MPEG_Mode_BlackAndWhite;
            else
                _pst_BI->pst_MpegIO->ulFlags &= ~MPEG_Mode_BlackAndWhite;

			STREAMER(GOTO)(_pst_BI->i_Counter);
#ifdef USE_WEBCAM
			STREAMER(FRAME)();
#endif

			if(_pst_BI->pst_MpegIO->ulBufferResult)
			{
				st_Src.uw_Width = (unsigned short) _pst_BI->pst_MpegIO->SizeX;
				st_Src.uw_Height = (unsigned short) _pst_BI->pst_MpegIO->SizeY;
				st_Src.p_Bitmap = (void *) _pst_BI->pst_MpegIO->ulBufferResult;

				st_Tgt.uw_Width = 256;
				st_Tgt.uw_Height = 256;
				st_Tgt.p_Bitmap = _pst_BI->dpc_Image[0];

				TEX_l_ConvertSize_32(&st_Src, &st_Tgt);
#ifdef USE_WEBCAM
				TEX_Convert_32SwapRB(st_Tgt.p_Bitmap,256*256);
#endif
				st_RTP.w_Texture = _pst_BI->w_Texture;
				st_RTP.pc_Data = _pst_BI->dpc_Image[0];
				_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_ReloadTexture, (ULONG) & st_RTP);
			}
		}
		else
		{
			st_RTP.w_Texture = _pst_BI->w_Texture;
			st_RTP.pc_Data = _pst_BI->dpc_Image[_pst_BI->i_Counter];
            if (st_RTP.pc_Data)
			    _pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_ReloadTexture, (ULONG) & st_RTP);
		}
	}

    /* camera iso taille en pixel */
    L_memcpy( &st_Cam, &GDI_gpst_CurDD->st_Camera, sizeof( CAM_tdst_Camera ) );
    st_Cam.ul_Flags = CAM_Cul_Flags_Perspective | CAM_Cul_Flags_Ortho;
    GDI_gpst_CurDD->st_GDI.pfnv_SetProjectionMatrix( &st_Cam );

    /*
	_pst_DD->st_GDI.pfnv_SetProjectionMatrix(&CAM_gst_IsoNoClip);
	GDI_SetViewMatrix((*_pst_DD) , &MATH_gst_IdentityMatrix);
    */
	

    /*
    SOFT_BI_Points[0].x = (_pst_BI->x * 2) - 1;
    SOFT_BI_Points[0].y = _pst_BI->y * 2 - 1;
    SOFT_BI_Points[1].x = (_pst_BI->x + _pst_BI->w) * 2 - 1;
    SOFT_BI_Points[1].y = _pst_BI->y * 2 - 1;
    SOFT_BI_Points[2].x = (_pst_BI->x + _pst_BI->w) * 2 - 1;
    SOFT_BI_Points[2].y = (_pst_BI->y + _pst_BI->h) * 2 - 1;
    SOFT_BI_Points[3].x = _pst_BI->x * 2 - 1;
    SOFT_BI_Points[3].y = (_pst_BI->y + _pst_BI->h) * 2 - 1;
    */

    SOFT_BI_Points[0].x = _pst_BI->x * st_Cam.f_Width;
    SOFT_BI_Points[0].y = _pst_BI->y * st_Cam.f_Height;
    SOFT_BI_Points[1].x = (_pst_BI->x + _pst_BI->w) * st_Cam.f_Width ;
    SOFT_BI_Points[1].y = SOFT_BI_Points[0].y;
    SOFT_BI_Points[2].x = SOFT_BI_Points[1].x;
    SOFT_BI_Points[2].y = (_pst_BI->y + _pst_BI->h) * st_Cam.f_Height;
    SOFT_BI_Points[3].x = SOFT_BI_Points[0].x;
    SOFT_BI_Points[3].y = SOFT_BI_Points[2].y;

    SOFT_BI_Points[0].z = SOFT_BI_Points[1].z = SOFT_BI_Points[2].z = SOFT_BI_Points[3].z = (_pst_BI->depth == 0) ? 1 : (1.0f / _pst_BI->depth);

	SOFT_BI_Mat->pst_FirstLevel->s_TextureId = _pst_BI->w_Texture;

	ul_SaveDD = _pst_DD->ul_CurrentDrawMask;
	ul_SaveFC = _pst_DD->ul_ColorConstant;
	_pst_DD->ul_CurrentDrawMask = GDI_Cul_DM_UseTexture | GDI_Cul_DM_DontShowBV | GDI_Cul_DM_Draw | GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor;
	_pst_DD->ul_ColorConstant = 0x80808080;

	memset(_pst_DD->pst_ComputingBuffers->ComputedColors,0x80,128);

    _pst_DD->p_Current_Vertex_List =  SOFT_BI_Points;
	MAT_DrawIndexedTriangle(&SOFT_BI_Obj, (MAT_tdst_Material *) SOFT_BI_Mat, &SOFT_BI_Element);
    

	_pst_DD->ul_CurrentDrawMask = ul_SaveDD;
	_pst_DD->ul_ColorConstant = ul_SaveFC;

	if (_pst_DD->st_Camera.pst_ObjectToCameraMatrix)
	{
		GDI_SetViewMatrix((*_pst_DD) , _pst_DD->st_Camera.pst_ObjectToCameraMatrix);
		
		_pst_DD->st_GDI.pfnv_SetProjectionMatrix(&_pst_DD->st_Camera);
	}
}

/*$4
 ***********************************************************************************************************************
    save/load functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_BackgroundImage_Save( SOFT_tdst_BackgroundImage *_pst_BI )
{
    char  sz_Path[128], sz_Name[64];
    
    sprintf( sz_Name, "%s.bid", _pst_BI->sz_Name );
    sprintf( sz_Path, "%s/BackgroundImageData", EDI_Csz_Path_DisplayData );

    SAV_Begin( sz_Path, sz_Name );

    SAV_Buffer( ".BID", 4);
    SAV_Buffer( &_pst_BI->ul_Flags, 4);
    SAV_Buffer( &_pst_BI->ul_PLKey, 4);
    SAV_Buffer( &_pst_BI->ul_PLLastKey, 4);
    SAV_Buffer( _pst_BI->sz_PLFile, 260 );
    SAV_Buffer( &_pst_BI->i_FirstIndex, 4);
    SAV_Buffer( &_pst_BI->i_LastIndex, 4);
    SAV_Buffer( &_pst_BI->ul_MpegKey, 4);
    SAV_Buffer( _pst_BI->sz_MpegFile, 260 );
    SAV_Buffer( &_pst_BI->x, 4);
    SAV_Buffer( &_pst_BI->y, 4);
    SAV_Buffer( &_pst_BI->depth, 4);
    SAV_Buffer( &_pst_BI->w, 4);
    SAV_Buffer( &_pst_BI->h, 4);

    SAV_Buffer( &_pst_BI->w_AnimFreq, 2);
    SAV_Buffer( &_pst_BI->w_FilmFreq, 2);
    SAV_Buffer( &_pst_BI->f_FreqRatio, 4);

    _pst_BI->ul_Key = BIG_FileKey( SAV_ul_End() );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_BackgroundImage_Load( SOFT_tdst_BackgroundImage *_pst_BI, ULONG _ul_Key )
{
    ULONG ul_Index, ul_Size;
    char  sz_Name[64], *pc_Buf;
    

    if (_ul_Key == BIG_C_InvalidKey) return;
    ul_Index = BIG_ul_SearchKeyToFat( _ul_Key );
    if (ul_Index== BIG_C_InvalidIndex) return;

    L_strcpy( sz_Name, BIG_NameFile(ul_Index) );
    pc_Buf = L_strrchr( sz_Name, '.' );
    if (!pc_Buf) return;
    if (L_stricmp( pc_Buf, ".bid") ) return;
    *pc_Buf = 0;
    
    pc_Buf = BIG_pc_ReadFileTmp( BIG_PosFile(ul_Index) , &ul_Size );

    if (L_strncmp( pc_Buf, ".BID", 4) ) return;
    pc_Buf += 4;
#if defined (__cplusplus)
	_pst_BI->ul_Flags = *((ULONG *&) pc_Buf)++;
    _pst_BI->ul_PLKey = *((ULONG *&) pc_Buf)++;
    _pst_BI->ul_PLLastKey = *((ULONG *&) pc_Buf)++;
#else
    _pst_BI->ul_Flags = *((ULONG *) pc_Buf)++;
    _pst_BI->ul_PLKey = *((ULONG *) pc_Buf)++;
    _pst_BI->ul_PLLastKey = *((ULONG *) pc_Buf)++;
#endif
	L_memcpy( _pst_BI->sz_PLFile, pc_Buf, 260 );
    pc_Buf += 260;
#if defined (__cplusplus)  
    _pst_BI->i_FirstIndex = *((ULONG *&) pc_Buf)++;
    _pst_BI->i_LastIndex = *((ULONG *&) pc_Buf)++;
    _pst_BI->ul_MpegKey = *((ULONG *&) pc_Buf)++;
#else
	_pst_BI->i_FirstIndex = *((ULONG *) pc_Buf)++;
    _pst_BI->i_LastIndex = *((ULONG *) pc_Buf)++;
    _pst_BI->ul_MpegKey = *((ULONG *) pc_Buf)++;
#endif
	L_memcpy( _pst_BI->sz_MpegFile, pc_Buf, 260 );
    pc_Buf += 260;

#if defined (__cplusplus) 
	_pst_BI->x = *((float *&) pc_Buf)++;
    _pst_BI->y = *((float *&) pc_Buf)++;
    _pst_BI->depth = *((float *&) pc_Buf)++;
    _pst_BI->w = *((float *&) pc_Buf)++;
    _pst_BI->h = *((float *&) pc_Buf)++;

    _pst_BI->w_AnimFreq = *((short *&) pc_Buf)++;
    _pst_BI->w_FilmFreq = *((short *&) pc_Buf)++;
 
#else
    _pst_BI->x = *((float *) pc_Buf)++;
    _pst_BI->y = *((float *) pc_Buf)++;
    _pst_BI->depth = *((float *) pc_Buf)++;
    _pst_BI->w = *((float *) pc_Buf)++;
    _pst_BI->h = *((float *) pc_Buf)++;

    _pst_BI->w_AnimFreq = *((short *) pc_Buf)++;
    _pst_BI->w_FilmFreq = *((short *) pc_Buf)++;
    _pst_BI->f_FreqRatio = *((float *) pc_Buf)++;
#endif
    L_strcpy( _pst_BI->sz_Name, sz_Name );
    _pst_BI->ul_Key = _ul_Key;

    _pst_BI->ul_Flags |= SOFT_C_BI_Reload;
}

/*$4
 ***********************************************************************************************************************
    edit : pick / move / size
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SOFT_i_BackgroundImage_Pick( SOFT_tdst_BackgroundImage *_pst_BI, int x, int y, struct GDI_tdst_DisplayData_ *_pst_DD )
{
    float fx, fy;
    int i;


    fx = (float) (x - _pst_DD->st_Camera.l_ViewportRealLeft) / _pst_DD->st_Camera.f_Width;
    fy = 1.0f - ((float) (y - _pst_DD->st_Camera.l_ViewportRealTop) / _pst_DD->st_Camera.f_Height);

    if ( fx > _pst_BI->x + _pst_BI->w + 0.01f )
        return 0;
    else if (fx > _pst_BI->x + _pst_BI->w - 0.01f )
        i = 6;
    else if (fx > _pst_BI->x + 0.01f )
        i = 7;
    else if (fx > _pst_BI->x - 0.01f )
        i = 8;
    else 
        return 0;

    if ( ( fy > _pst_BI->y + _pst_BI->h + 0.01f ) || (fy < _pst_BI->y - 0.01f) )
        return 0;
    else if (fy > _pst_BI->y + _pst_BI->h - 0.01f )
        return ( i == 8) ? 2 : ( (i == 7) ? 3 : 4 );
    else if (fy > _pst_BI->y + 0.01f )
        return ( i == 8) ? 9 : ( (i == 7) ? 1 : 5 );

    return i;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_BackgroundImage_MoveSize( SOFT_tdst_BackgroundImage *_pst_BI, int move, float dx, float dy, int i_Ratio )
{
    float	ratio;

    if (i_Ratio)
    {
        ratio = (_pst_BI->h) ? (_pst_BI->w / _pst_BI->h) : 0;
        i_Ratio = (ratio != 0);
    }

	switch( move )
    {
    case 1: /* inside widow : move */
		_pst_BI->x += dx;
		_pst_BI->y += dy;
		break;

	case 2: /* top-left corner */
        _pst_BI->h += dy;
		if (i_Ratio)
        {
            dx = _pst_BI->h * ratio;
            _pst_BI->x += _pst_BI->w - dx; 
            _pst_BI->w = dx;
        }
        else
        {
            _pst_BI->x += dx;
			_pst_BI->w -= dx;
        }
		break;
	
    case 3: /* top edge */
		_pst_BI->h += dy;
        if (i_Ratio)
        {
            dx = _pst_BI->h * ratio;
            _pst_BI->x += _pst_BI->w - dx; 
            _pst_BI->w = dx;
        }
		break;

	case 4: /* top_right corner */
		_pst_BI->w += dx;
        if(i_Ratio)
            _pst_BI->h = _pst_BI->w / ratio;
        else
			_pst_BI->h += dy;
		break;

	case 5: /* right edge */
		_pst_BI->w += dx;
        if(i_Ratio)
            _pst_BI->h = _pst_BI->w / ratio;
		break;

	case 6: /* bottom-right corner */
        _pst_BI->y += dy;
		_pst_BI->h -= dy;
        if (i_Ratio)
            _pst_BI->w = _pst_BI->h * ratio;
        else
			_pst_BI->w += dx;
		break;

	case 7: /* bottom edge */
		_pst_BI->y += dy;
		_pst_BI->h -= dy;
        if (i_Ratio)
            _pst_BI->w = _pst_BI->h * ratio;
		break;

	case 8: /* bottom left corner */
		_pst_BI->x += dx;
		_pst_BI->w -= dx;
        if (i_Ratio)
        {
            dy = _pst_BI->w / ratio;
            _pst_BI->y += _pst_BI->h - dy; 
            _pst_BI->h = dy;
        }
        else
        {   
            _pst_BI->y += dy;
			_pst_BI->h -= dy;
        }
		break;
	
    case 9: /* left edge */
		_pst_BI->x += dx;
		_pst_BI->w -= dx;
        if (i_Ratio)
        {
            dy = _pst_BI->w / ratio;
            _pst_BI->y += _pst_BI->h - dy; 
            _pst_BI->h = dy;
        }
		break;
    }

	if(_pst_BI->w < 0) _pst_BI->w = 0;
	if(_pst_BI->h < 0) _pst_BI->h = 0;
}


#endif
