/*$T TEXconvert.c GC! 1.081 05/22/01 08:42:41 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Bitmap conversion fonction */
#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/MEMory/MEM.h"
#include "TEXture/TEXconvert.h"
#include "TEXture/TEXstruct.h"

#ifdef PSX2_TARGET
#include "MainPsx2/Sources/PSX2debug.h"
#define _ReadLong(_a)	ReadLong((char *) _a)
#define _WriteLong(_a, _b)	WriteLong((char *) _a, _b)
#else
#define _ReadLong(_a)	*(ULONG *) _a
#define _WriteLong(_a, _b)	*(ULONG*)_a = _b
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)

extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Macros
 ***********************************************************************************************************************
 */

#ifdef PSX2_TARGET

/*$F mamagouille */

#define TEX_M_AddColor(bitmap, decal, mask, color)				color += (ReadLong(bitmap) & mask) >> decal;
#define TEX_M_AddMulColor(bitmap, decal, mask, color, factor)	color += ((ReadLong(bitmap) & mask) >> decal) * factor;
#define TEX_M_SetColor(bitmap, decal, mask, color)				WriteLong(bitmap, (color << decal) & mask);
#define TEX_M_SetColorOr(bitmap, decal, mask, color)			WriteLong(bitmap, ReadLong(bitmap) | (color << decal) & mask);
#else
#define TEX_M_AddColor(bitmap, decal, mask, color)				color += (*(ULONG *) bitmap & mask) >> decal;
#define TEX_M_AddMulColor(bitmap, decal, mask, color, factor)	color += ((*(ULONG *) bitmap & mask) >> decal) * factor;
#define TEX_M_SetColor(bitmap, decal, mask, color)				*(ULONG *) bitmap = (color << decal) & mask;
#define TEX_M_SetColorOr(bitmap, decal, mask, color)			*(ULONG *) bitmap |= (color << decal) & mask;
#endif

/*
 =======================================================================================================================
    Retrieve mask parameters (number of bits, shift)
 =======================================================================================================================
 */
void TEX_GetMaskParams(ULONG _ul_Mask, unsigned char *_puc_NbBits, unsigned char *_puc_Shift)
{
	*_puc_NbBits = 0;
	*_puc_Shift = 0;

	if(_ul_Mask == 0) return;

	while(!(_ul_Mask & 1))
	{
		_ul_Mask >>= 1;
		(*_puc_Shift)++;
	}

	while(_ul_Mask & 1)
	{
		_ul_Mask >>= 1;
		(*_puc_NbBits)++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned char TEX_uc_GetMaskShift(ULONG _ul_Mask)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	unsigned char	uc_Decal;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	uc_Decal = 0;

	if(_ul_Mask == 0) return 0;

	while(!(_ul_Mask & 1))
	{
		_ul_Mask >>= 1;
		uc_Decal++;
	}

	return uc_Decal;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG TEX_ul_GetPower2Up(ULONG _l_Value)
{
	/*~~~~~~~~~~~~~~*/
	ULONG	i, power2;
	/*~~~~~~~~~~~~~~*/

	for(power2 = 1, i = 0; i < 30; i++)
	{
		if(_l_Value <= power2) return power2;
		power2 <<= 1;
	}

	return power2;
}

/*
 =======================================================================================================================
    Compute conversion parameters between two masks
 =======================================================================================================================
 */
void TEX_GetColorConvertParams(ULONG _ul_From, ULONG _ul_To, LONG *_pl_Shift, ULONG *_pul_Mask)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned char	uc_FromBit, uc_ToBit, uc_FromShift, uc_ToShift;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((_ul_From == 0) || (_ul_To == 0))
	{
		*_pl_Shift = 0;
		*_pul_Mask = 0;
		return;
	}

	TEX_GetMaskParams(_ul_From, &uc_FromBit, &uc_FromShift);
	TEX_GetMaskParams(_ul_To, &uc_ToBit, &uc_ToShift);

	if(uc_FromBit >= uc_ToBit)
	{
		*_pl_Shift = (LONG) (uc_FromBit + uc_FromShift) - (LONG) (uc_ToBit + uc_ToShift);
		if(*_pl_Shift > 0)
			*_pul_Mask = _ul_To << *_pl_Shift;
		else
			*_pul_Mask = _ul_To >> -(*_pl_Shift);
	}
}

/*
 =======================================================================================================================
    Convert a texture bitmap
 =======================================================================================================================
 */
LONG TEX_l_ConvertPF(TEX_tdst_File_Desc *_pst_SrcTex, TEX_tdst_File_Desc *_pst_TgtTex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned char	*puc_Src, *puc_Tgt;
	char			uc_SrcInc, uc_TgtInc;
	unsigned short	uw_Width, uw_Height;
	LONG			l_RDecal, l_RMask;
	LONG			l_GDecal, l_GMask;
	LONG			l_BDecal, l_BMask;
	LONG			l_ADecal, l_AMask;
	ULONG			ul_Color;
	ULONG			ul_TgtMask, ul_InvertTgtMask;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Color = 0;

	TEX_GetColorConvertParams(_pst_SrcTex->ul_RMask, _pst_TgtTex->ul_RMask, &l_RDecal, (ULONG *) &l_RMask);
	TEX_GetColorConvertParams(_pst_SrcTex->ul_GMask, _pst_TgtTex->ul_GMask, &l_GDecal, (ULONG *) &l_GMask);
	TEX_GetColorConvertParams(_pst_SrcTex->ul_BMask, _pst_TgtTex->ul_BMask, &l_BDecal, (ULONG *) &l_BMask);
	TEX_GetColorConvertParams(_pst_SrcTex->ul_AMask, _pst_TgtTex->ul_AMask, &l_ADecal, (ULONG *) &l_AMask);
	puc_Src = (unsigned char *) _pst_SrcTex->p_Bitmap;
	uc_SrcInc = _pst_SrcTex->uc_BPP >> 3;
	puc_Tgt = (unsigned char *) _pst_TgtTex->p_Bitmap;
	uc_TgtInc = _pst_TgtTex->uc_BPP >> 3;
	if(_pst_TgtTex->uc_BPP == 16)
		ul_TgtMask = 0xFFFF;
	else if(_pst_TgtTex->uc_BPP == 24)
		ul_TgtMask = 0xFFFFFF;
	else
		ul_TgtMask = 0xFFFFFFFF;
	ul_InvertTgtMask = ~ul_TgtMask;

	for(uw_Height = 0; uw_Height < _pst_SrcTex->uw_Height; uw_Height++)
	{
		for(uw_Width = 0; uw_Width < _pst_SrcTex->uw_Width; uw_Width++)
		{
			if(l_RDecal >= 0)
				ul_Color = (_ReadLong(puc_Src) & l_RMask) >> l_RDecal;
			else if(l_RDecal < 0) ul_Color = (_ReadLong(puc_Src) & l_RMask) << (-l_RDecal);

			if(l_GDecal >= 0)
				ul_Color |= (_ReadLong(puc_Src) & l_GMask) >> l_GDecal;
			else if(l_GDecal < 0) ul_Color |= (_ReadLong(puc_Src) & l_GMask) << (-l_GDecal);

			if(l_BDecal >= 0)
				ul_Color |= (_ReadLong(puc_Src) & l_BMask) >> l_BDecal;
			else if(l_BDecal < 0) ul_Color |= (_ReadLong(puc_Src) & l_BMask) << (-l_BDecal);

			if(l_ADecal >= 0)
				ul_Color |= (_ReadLong(puc_Src) & l_AMask) >> l_ADecal;
			else if(l_ADecal < 0) ul_Color |= (_ReadLong(puc_Src) & l_AMask) << (-l_ADecal);

			_WriteLong(puc_Tgt, (ul_Color & ul_TgtMask) | (_ReadLong(puc_Tgt) & ul_InvertTgtMask));

			puc_Tgt += uc_TgtInc;
			puc_Src += uc_SrcInc;
		}
	}

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEX_l_DimensionAreGood
(
	TEX_tdst_File_Desc	*_pst_SrcTex,
    LONG				_l_MinSize,
	LONG				_l_MaxSize,
	BOOL				_b_AcceptNonSquare,
	TEX_tdst_File_Desc	*_pst_Result
)
{
	L_memcpy(_pst_Result, _pst_SrcTex, sizeof(TEX_tdst_File_Desc));
	_pst_Result->p_Bitmap = _pst_Result->p_Palette = NULL;

	_pst_Result->uw_Height = (unsigned short) TEX_ul_GetPower2Up(_pst_Result->uw_Height);
    if(_pst_Result->uw_Height < _l_MinSize) 
            _pst_Result->uw_Height = (unsigned short) _l_MinSize;
	if(_pst_Result->uw_Height > _l_MaxSize) _pst_Result->uw_Height = (unsigned short) _l_MaxSize;


	_pst_Result->uw_Width = (unsigned short) TEX_ul_GetPower2Up(_pst_Result->uw_Width);
    if(_pst_Result->uw_Width < _l_MinSize) 
        _pst_Result->uw_Width = (unsigned short) _l_MinSize;
	if(_pst_Result->uw_Width > _l_MaxSize) _pst_Result->uw_Width = (unsigned short) _l_MaxSize;

	if(!_b_AcceptNonSquare)
	{
		if(_pst_Result->uw_Width < _pst_Result->uw_Height)
			_pst_Result->uw_Width = _pst_Result->uw_Height;
		else
			_pst_Result->uw_Height = _pst_Result->uw_Width;
	}

	return((_pst_SrcTex->uw_Height == _pst_Result->uw_Height) && (_pst_SrcTex->uw_Width == _pst_Result->uw_Width));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEX_l_ConvertSize(TEX_tdst_File_Desc *_pst_SrcTex, TEX_tdst_File_Desc *_pst_TgtTex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			ul_X, ul_Y;
	ULONG			ul_SrcX, ul_SrcY, ul_DeltaX, ul_DeltaY, ul_SquareSize;
	char			*pc_Bitmap;
	ULONG			aul_Color[4], aul_SrcMask[4], aul_TgtMask[4];
	unsigned char	auc_SrcDecal[4], auc_TgtDecal[4];
	ULONG			ul_SrcColorInc, ul_TgtColorInc;
	ULONG			ul_LineInc;
	char			*pc_SrcBitmap, *pc_Current;
	ULONG			ul_XFactor, ul_YFactor, ul_Factor;
	ULONG			ul_MaskToReset;

	typedef struct
	{
		ULONG	ul_First;
		ULONG	ul_RatioFirst;
		ULONG	ul_Last;
		ULONG	ul_RatioLast;
	} TEX_tdst_ConvertSizeTableEntry;

	TEX_tdst_ConvertSizeTableEntry	*pst_XTable, *pst_YTable, *pst_X, *pst_Y;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_XTable = (TEX_tdst_ConvertSizeTableEntry *) L_malloc(sizeof(TEX_tdst_ConvertSizeTableEntry) * _pst_TgtTex->uw_Width);
	pst_YTable = (TEX_tdst_ConvertSizeTableEntry *) L_malloc(sizeof(TEX_tdst_ConvertSizeTableEntry) * _pst_TgtTex->uw_Height);

	ul_SrcX = 0;
	ul_DeltaX = (_pst_SrcTex->uw_Width << 8) / (_pst_TgtTex->uw_Width);
	pst_X = pst_XTable;
	for(ul_X = 0; ul_X < _pst_TgtTex->uw_Width; ul_X++, pst_X++)
	{
		pst_X->ul_First = ul_SrcX >> 8;
		pst_X->ul_RatioFirst = 0x100 - (ul_SrcX & 0xFF);
		ul_SrcX += ul_DeltaX;
		pst_X->ul_Last = ul_SrcX >> 8;
		if(pst_X->ul_Last == pst_X->ul_First)
			pst_X->ul_RatioFirst -= 0x100 - (ul_SrcX & 0xFF);
		else
			pst_X->ul_RatioLast = ul_SrcX & 0xFF;
	}

	pst_X--;
	pst_X->ul_Last = _pst_SrcTex->uw_Width - 1;
	pst_X->ul_RatioLast = 0xFF;

	ul_SrcY = 0;
	ul_DeltaY = (_pst_SrcTex->uw_Height << 8) / (_pst_TgtTex->uw_Height);
	pst_Y = pst_YTable;
	for(ul_Y = 0; ul_Y < _pst_TgtTex->uw_Height; ul_Y++, pst_Y++)
	{
		pst_Y->ul_First = ul_SrcY >> 8;
		pst_Y->ul_RatioFirst = 0x100 - (ul_SrcY & 0xFF);
		ul_SrcY += ul_DeltaY;
		pst_Y->ul_Last = ul_SrcY >> 8;
		if(pst_Y->ul_Last == pst_Y->ul_First)
			pst_Y->ul_RatioFirst -= 0x100 - (ul_SrcY & 0xFF);
		else
			pst_Y->ul_RatioLast = ul_SrcY & 0xFF;
	}

	pst_Y--;
	pst_Y->ul_Last = _pst_SrcTex->uw_Height - 1;
	pst_Y->ul_RatioLast = 0xFF;

	auc_SrcDecal[0] = TEX_uc_GetMaskShift(aul_SrcMask[0] = _pst_SrcTex->ul_AMask);
	auc_SrcDecal[1] = TEX_uc_GetMaskShift(aul_SrcMask[1] = _pst_SrcTex->ul_RMask);
	auc_SrcDecal[2] = TEX_uc_GetMaskShift(aul_SrcMask[2] = _pst_SrcTex->ul_GMask);
	auc_SrcDecal[3] = TEX_uc_GetMaskShift(aul_SrcMask[3] = _pst_SrcTex->ul_BMask);

	auc_TgtDecal[0] = TEX_uc_GetMaskShift(aul_TgtMask[0] = _pst_TgtTex->ul_AMask);
	auc_TgtDecal[1] = TEX_uc_GetMaskShift(aul_TgtMask[1] = _pst_TgtTex->ul_RMask);
	auc_TgtDecal[2] = TEX_uc_GetMaskShift(aul_TgtMask[2] = _pst_TgtTex->ul_GMask);
	auc_TgtDecal[3] = TEX_uc_GetMaskShift(aul_TgtMask[3] = _pst_TgtTex->ul_BMask);

	ul_MaskToReset = ~(aul_TgtMask[0] | aul_TgtMask[1] | aul_TgtMask[2] | aul_TgtMask[3]);

	pc_Bitmap = (char *) _pst_TgtTex->p_Bitmap;
	pc_SrcBitmap = (char *) _pst_SrcTex->p_Bitmap;

	ul_SquareSize = ul_DeltaX * ul_DeltaY;

	ul_TgtColorInc = _pst_TgtTex->uc_BPP >> 3;
	ul_SrcColorInc = _pst_SrcTex->uc_BPP >> 3;

	ul_LineInc = ul_SrcColorInc * _pst_SrcTex->uw_Width;

	pst_Y = pst_YTable;
	for(ul_Y = 0; ul_Y < _pst_TgtTex->uw_Height; ul_Y++, pst_Y++)
	{
		pst_X = pst_XTable;
		for(ul_X = 0; ul_X < _pst_TgtTex->uw_Width; ul_X++, pst_X++)
		{
			aul_Color[0] = aul_Color[1] = aul_Color[2] = aul_Color[3] = 0;
			ul_SrcY = pst_Y->ul_First;

			while(ul_SrcY <= pst_Y->ul_Last)
			{
				ul_YFactor = (ul_SrcY == pst_Y->ul_First) ? pst_Y->ul_RatioFirst : (ul_SrcY == pst_Y->ul_Last) ? pst_Y->ul_RatioLast : 0x100;

				ul_SrcX = pst_X->ul_First;

				while(ul_SrcX <= pst_X->ul_Last)
				{
					ul_XFactor = (ul_SrcX == pst_X->ul_First) ? pst_X->ul_RatioFirst : (ul_SrcX == pst_X->ul_Last) ? pst_X->ul_RatioLast : 0x100;
					ul_Factor = ul_XFactor * ul_YFactor;
					pc_Current = pc_SrcBitmap + ul_SrcX * ul_SrcColorInc + ul_SrcY * ul_LineInc;
					TEX_M_AddMulColor(pc_Current, auc_SrcDecal[0], aul_SrcMask[0], aul_Color[0], ul_Factor);
					TEX_M_AddMulColor(pc_Current, auc_SrcDecal[1], aul_SrcMask[1], aul_Color[1], ul_Factor);
					TEX_M_AddMulColor(pc_Current, auc_SrcDecal[2], aul_SrcMask[2], aul_Color[2], ul_Factor);
					TEX_M_AddMulColor(pc_Current, auc_SrcDecal[3], aul_SrcMask[3], aul_Color[3], ul_Factor);
					ul_SrcX++;
				}

				ul_SrcY++;
			}

			aul_Color[0] /= ul_SquareSize;
			aul_Color[1] /= ul_SquareSize;
			aul_Color[2] /= ul_SquareSize;
			aul_Color[3] /= ul_SquareSize;
#ifdef PSX2_TARGET
			/* mamagouille */
			WriteLong(pc_Bitmap, ReadLong(pc_Bitmap) & ul_MaskToReset);
#else
			*(ULONG *) pc_Bitmap &= ul_MaskToReset;
#endif
			TEX_M_SetColorOr(pc_Bitmap, auc_TgtDecal[0], aul_TgtMask[0], aul_Color[0]);
			TEX_M_SetColorOr(pc_Bitmap, auc_TgtDecal[1], aul_TgtMask[1], aul_Color[1]);
			TEX_M_SetColorOr(pc_Bitmap, auc_TgtDecal[2], aul_TgtMask[2], aul_Color[2]);
			TEX_M_SetColorOr(pc_Bitmap, auc_TgtDecal[3], aul_TgtMask[3], aul_Color[3]);

			pc_Bitmap += ul_TgtColorInc;
		}
	}

	L_free(pst_XTable);
	L_free(pst_YTable);

	return 1;
}

/*
 =======================================================================================================================
    Fast size convertion
 =======================================================================================================================
 */
LONG TEX_l_ConvertSizeFast(TEX_tdst_File_Desc *_pst_Tex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_Size, ul_Size2;
	void	*p_Bitmap;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Size = TEX_ul_GetPower2Up(_pst_Tex->uw_Width);
	if(ul_Size != _pst_Tex->uw_Width) return 0;
	ul_Size = TEX_ul_GetPower2Up(_pst_Tex->uw_CompressedWidth);
	if(ul_Size != _pst_Tex->uw_CompressedWidth) return 0;
	ul_Size = TEX_ul_GetPower2Up(_pst_Tex->uw_Height);
	if(ul_Size != _pst_Tex->uw_Height) return 0;
	ul_Size = TEX_ul_GetPower2Up(_pst_Tex->uw_CompressedHeight);
	if(ul_Size != _pst_Tex->uw_CompressedHeight) return 0;

	ul_Size = (ULONG) _pst_Tex->uw_Width * _pst_Tex->uw_Height;
	ul_Size2 = (ULONG) _pst_Tex->uw_CompressedWidth * _pst_Tex->uw_CompressedHeight;

	if(ul_Size2 > ul_Size)
	{
		ul_Size2 *= _pst_Tex->uc_BPP;
		p_Bitmap = _pst_Tex->p_Bitmap;
		TEX_M_File_Alloc(_pst_Tex->p_Bitmap, ul_Size2, void);
	}
	else
		p_Bitmap = NULL;

	if(_pst_Tex->uc_BPP == 24)
	{
		while(_pst_Tex->uw_Width > _pst_Tex->uw_CompressedWidth)
		{
			if(p_Bitmap)
			{
				TEX_Compress24_Xo2
				(
					(UCHAR *) p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
				TEX_M_File_Free(p_Bitmap);
				p_Bitmap = NULL;
			}
			else
			{
				TEX_Compress24_Xo2
				(
					(UCHAR *) _pst_Tex->p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
			}

			_pst_Tex->uw_Width >>= 1;
		}

		while(_pst_Tex->uw_Height > _pst_Tex->uw_CompressedHeight)
		{
			if(p_Bitmap)
			{
				TEX_Compress24_Yo2
				(
					(UCHAR *) p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
				TEX_M_File_Free(p_Bitmap);
				p_Bitmap = NULL;
			}
			else
			{
				TEX_Compress24_Yo2
				(
					(UCHAR *) _pst_Tex->p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
			}

			_pst_Tex->uw_Height >>= 1;
		}

		while(_pst_Tex->uw_Width < _pst_Tex->uw_CompressedWidth)
		{
			if(p_Bitmap)
			{
				TEX_Expand24_XBy2
				(
					(UCHAR *) p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
				TEX_M_File_Free(p_Bitmap);
				p_Bitmap = NULL;
			}
			else
			{
				TEX_Expand24_XBy2
				(
					(UCHAR *) _pst_Tex->p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
			}

			_pst_Tex->uw_Width <<= 1;
		}

		while(_pst_Tex->uw_Height < _pst_Tex->uw_CompressedHeight)
		{
			if(p_Bitmap)
			{
				TEX_Expand24_YBy2
				(
					(UCHAR *) p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
				TEX_M_File_Free(p_Bitmap);
				p_Bitmap = NULL;
			}
			else
			{
				TEX_Expand24_YBy2
				(
					(UCHAR *) _pst_Tex->p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
			}

			_pst_Tex->uw_Height <<= 1;
		}
	}
	else if(_pst_Tex->uc_BPP == 32)
	{
		while(_pst_Tex->uw_Width > _pst_Tex->uw_CompressedWidth)
		{
			if(p_Bitmap)
			{
				TEX_Compress_Xo2
				(
					(ULONG *) p_Bitmap,
					(ULONG *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
				TEX_M_File_Free(p_Bitmap);
				p_Bitmap = NULL;
			}
			else
			{
				TEX_Compress_Xo2
				(
					(ULONG *) _pst_Tex->p_Bitmap,
					(ULONG *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
			}

			_pst_Tex->uw_Width >>= 1;
		}

		while(_pst_Tex->uw_Height > _pst_Tex->uw_CompressedHeight)
		{
			if(p_Bitmap)
			{
				TEX_Compress_Yo2
				(
					(ULONG *) p_Bitmap,
					(ULONG *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
				TEX_M_File_Free(p_Bitmap);
				p_Bitmap = NULL;
			}
			else
			{
				TEX_Compress_Yo2
				(
					(ULONG *) _pst_Tex->p_Bitmap,
					(ULONG *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
			}

			_pst_Tex->uw_Height >>= 1;
		}

		while(_pst_Tex->uw_Width < _pst_Tex->uw_CompressedWidth)
		{
			if(p_Bitmap)
			{
				TEX_Expand_XBy2
				(
					(ULONG *) p_Bitmap,
					(ULONG *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
				TEX_M_File_Free(p_Bitmap);
				p_Bitmap = NULL;
			}
			else
			{
				TEX_Expand_XBy2
				(
					(ULONG *) _pst_Tex->p_Bitmap,
					(ULONG *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
			}

			_pst_Tex->uw_Width <<= 1;
		}

		while(_pst_Tex->uw_Height < _pst_Tex->uw_CompressedHeight)
		{
			if(p_Bitmap)
			{
				TEX_Expand_YBy2
				(
					(ULONG *) p_Bitmap,
					(ULONG *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
				TEX_M_File_Free(p_Bitmap);
				p_Bitmap = NULL;
			}
			else
			{
				TEX_Expand_YBy2
				(
					(ULONG *) _pst_Tex->p_Bitmap,
					(ULONG *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
			}

			_pst_Tex->uw_Height <<= 1;
		}
	}
	else if(_pst_Tex->uc_BPP == 8)
	{
		while(_pst_Tex->uw_Width > _pst_Tex->uw_CompressedWidth)
		{
			if(p_Bitmap)
			{
				TEX_Compress8_Xo2
				(
					(UCHAR *) p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
				TEX_M_File_Free(p_Bitmap);
				p_Bitmap = NULL;
			}
			else
			{
				TEX_Compress8_Xo2
				(
					(UCHAR *) _pst_Tex->p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
			}

			_pst_Tex->uw_Width >>= 1;
		}

		while(_pst_Tex->uw_Height > _pst_Tex->uw_CompressedHeight)
		{
			if(p_Bitmap)
			{
				TEX_Compress8_Yo2
				(
					(UCHAR *) p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
				TEX_M_File_Free(p_Bitmap);
				p_Bitmap = NULL;
			}
			else
			{
				TEX_Compress8_Yo2
				(
					(UCHAR *) _pst_Tex->p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
			}

			_pst_Tex->uw_Height >>= 1;
		}

		while(_pst_Tex->uw_Width < _pst_Tex->uw_CompressedWidth)
		{
			if(p_Bitmap)
			{
				TEX_Expand8_XBy2
				(
					(UCHAR *) p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
				TEX_M_File_Free(p_Bitmap);
				p_Bitmap = NULL;
			}
			else
			{
				TEX_Expand8_XBy2
				(
					(UCHAR *) _pst_Tex->p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
			}

			_pst_Tex->uw_Width <<= 1;
		}

		while(_pst_Tex->uw_Height < _pst_Tex->uw_CompressedHeight)
		{
			if(p_Bitmap)
			{
				TEX_Expand8_YBy2
				(
					(UCHAR *) p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
				TEX_M_File_Free(p_Bitmap);
				p_Bitmap = NULL;
			}
			else
			{
				TEX_Expand8_YBy2
				(
					(UCHAR *) _pst_Tex->p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
			}

			_pst_Tex->uw_Height <<= 1;
		}
	}
	else if(_pst_Tex->uc_BPP == 4)
	{
		while(_pst_Tex->uw_Width > _pst_Tex->uw_CompressedWidth)
		{
			if(p_Bitmap)
			{
				TEX_Compress4_Xo2
				(
					(UCHAR *) p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
				TEX_M_File_Free(p_Bitmap);
				p_Bitmap = NULL;
			}
			else
			{
				TEX_Compress4_Xo2
				(
					(UCHAR *) _pst_Tex->p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
			}

			_pst_Tex->uw_Width >>= 1;
		}

		while(_pst_Tex->uw_Height > _pst_Tex->uw_CompressedHeight)
		{
			if(p_Bitmap)
			{
				TEX_Compress4_Yo2
				(
					(UCHAR *) p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
				TEX_M_File_Free(p_Bitmap);
				p_Bitmap = NULL;
			}
			else
			{
				TEX_Compress4_Yo2
				(
					(UCHAR *) _pst_Tex->p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
			}

			_pst_Tex->uw_Height >>= 1;
		}

		while(_pst_Tex->uw_Width < _pst_Tex->uw_CompressedWidth)
		{
			if(p_Bitmap)
			{
				TEX_Expand4_XBy2
				(
					(UCHAR *) p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
				TEX_M_File_Free(p_Bitmap);
				p_Bitmap = NULL;
			}
			else
			{
				TEX_Expand4_XBy2
				(
					(UCHAR *) _pst_Tex->p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
			}

			_pst_Tex->uw_Width <<= 1;
		}

		while(_pst_Tex->uw_Height < _pst_Tex->uw_CompressedHeight)
		{
			if(p_Bitmap)
			{
				TEX_Expand4_YBy2
				(
					(UCHAR *) p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
				TEX_M_File_Free(p_Bitmap);
				p_Bitmap = NULL;
			}
			else
			{
				TEX_Expand4_YBy2
				(
					(UCHAR *) _pst_Tex->p_Bitmap,
					(UCHAR *) _pst_Tex->p_Bitmap,
					_pst_Tex->uw_Width,
					_pst_Tex->uw_Height
				);
			}

			_pst_Tex->uw_Height <<= 1;
		}
	}
	else
		return 0;

	return 1;
}

/*$4
 ***********************************************************************************************************************
    24 to 32 operations
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEX_l_ConvertSize_24To32(TEX_tdst_File_Desc *_pst_SrcTex, TEX_tdst_File_Desc *_pst_TgtTex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float			x, dx, y, dy;
	int				ax[1024];
	int				i, j;
	unsigned char	*uc_Src;
	ULONG			*ul_Tgt;
	int				i_SrcLineLength;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Tgt = (ULONG *) _pst_TgtTex->p_Bitmap;
	i_SrcLineLength = _pst_SrcTex->uw_Width * 3;

	dx = (float) _pst_SrcTex->uw_Width / _pst_TgtTex->uw_Width;
	for(x = 0, i = 0; i < _pst_TgtTex->uw_Width; i++, x += dx) ax[i] = ((int) x) * 3;

	dy = (float) _pst_SrcTex->uw_Height / _pst_TgtTex->uw_Height;
	for(y = 0, j = 0; j < _pst_TgtTex->uw_Height; j++, y += dy)
	{
		uc_Src = ((unsigned char *) _pst_SrcTex->p_Bitmap) + (int) y * i_SrcLineLength;
		for(i = 0; i < _pst_TgtTex->uw_Width; i++) *ul_Tgt++ = _ReadLong((uc_Src + ax[i])) & 0xFFFFFF;
	}

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Convert_24To32(ULONG *c, ULONG l)
{
	/*~~~~~~~~~~~~~~~~~*/
	unsigned char	*c24;
	/*~~~~~~~~~~~~~~~~~*/

	c24 = (unsigned char *) c + l * (LONG) 3;
	c += l;
	while(l--)
	{
		c24 -= 3;
		*(--c) = 0xFF000000 | ((ULONG) (*(c24))) + ((ULONG) (*(c24 + 1)) << 8) + ((ULONG) (*(c24 + 2)) << 16);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Convert_24To32SwapRB(ULONG *c, ULONG l)
{
	/*~~~~~~~~~~~~~~~~~*/
	unsigned char	*c24;
	/*~~~~~~~~~~~~~~~~~*/

	c24 = (unsigned char *) c + l * (LONG) 3;
	c += l;
	while(l--)
	{
		c24 -= 3;
		*(--c) = 0xFF000000 | ((ULONG) (*(c24)) << 16) + ((ULONG) (*(c24 + 1)) << 8) + ((ULONG) (*(c24 + 2)));
	}
}


/*$4
 ***********************************************************************************************************************
    32 To 24 operations
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Convert_32To24(ULONG *c, ULONG l)
{
    /*~~~~~~~~~~~~~~~~~*/
	unsigned char	*c32, *c24;
	/*~~~~~~~~~~~~~~~~~*/

    c32 = c24 = (unsigned char *) c;
	while(l--)
	{
        *c24++ = *c32++;
        *c24++ = *c32++;
        *c24++ = *c32++;
        c32++;
	}
}

/*$4
 ***********************************************************************************************************************
    32 to 32 bit operation
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEX_l_ConvertSize_32(TEX_tdst_File_Desc *_pst_SrcTex, TEX_tdst_File_Desc *_pst_TgtTex)
{
	/*~~~~~~~~~~~~~~~~~*/
	float	x, dx, y, dy;
	int		ax[1024];
	int		i, j;
	ULONG	*ul_Src;
	ULONG	*ul_Tgt;
	/*~~~~~~~~~~~~~~~~~*/

	ul_Tgt = (ULONG *) _pst_TgtTex->p_Bitmap;

	dx = (float) _pst_SrcTex->uw_Width / _pst_TgtTex->uw_Width;
	for(x = 0, i = 0; i < _pst_TgtTex->uw_Width; i++, x += dx) ax[i] = (int) x;

	dy = (float) _pst_SrcTex->uw_Height / _pst_TgtTex->uw_Height;
	for(y = 0, j = 0; j < _pst_TgtTex->uw_Height; j++, y += dy)
	{
		ul_Src = ((ULONG *) _pst_SrcTex->p_Bitmap) + (int) y * _pst_SrcTex->uw_Width;
		for(i = 0; i < _pst_TgtTex->uw_Width; i++) *ul_Tgt++ = ul_Src[ax[i]];
	}

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Compress_Xo2(ULONG *src, ULONG *dst, ULONG w, ULONG h)
{
	/*~~~~~~*/
	ULONG	l;
	/*~~~~~~*/

	l = (w * h) >> 1;
	if(!l) return;
	while(l--)
	{
		*dst = ((*(src) & 0xFCFCFCFC) >> 1) + ((*(src + 1) & 0xFCFCFCFC) >> 1);
		src += 2;
		dst++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Compress_Yo2(ULONG *src, ULONG *dst, ULONG w, ULONG h)
{
	/*~~~~~~*/
	ULONG	l;
	/*~~~~~~*/

	h >>= 1;
	if(!h) return;
	while(h--)
	{
		l = w;
		while(l--)
		{
			*dst = ((*(src) & 0xFCFCFCFC) >> 1) + ((*(src + w) & 0xFCFCFCFC) >> 1);
			src++;
			dst++;
		}

		src += w;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Compress_Yo2InvertColor(ULONG *src, ULONG *dst, ULONG w, ULONG h)
{
	/*~~~~~~*/
	ULONG	l;
	/*~~~~~~*/

	h >>= 1;
	if(!h) return;
	while(h--)
	{
		l = w;
		while(l--)
		{
			*dst = ((*(src) & 0xFCFCFCFC) >> 1) + ((*(src + w) & 0xFCFCFCFC) >> 1);
			*dst = ~(*dst & 0xFFFFFF) | (*dst & 0xFF000000);
			src++;
			dst++;
		}

		src += w;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Expand_XBy2(ULONG *src, ULONG *dst, ULONG w, ULONG h)
{
	/*~~~~~~~~~~~~*/
	ULONG	*srccur;
	/*~~~~~~~~~~~~*/

	dst += (w * h) << 1;
	srccur = src + (w * h);

	while(srccur-- > src)
	{
		*(--dst) = *srccur;
		*(--dst) = *srccur;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Expand_YBy2(ULONG *src, ULONG *dst, ULONG w, ULONG h)
{
	/*~~~~~~*/
	ULONG	l;
	/*~~~~~~*/

	dst += (w * h) << 1;
	src += (w * h);

	while(h--)
	{
		l = w;
		while(l--)
		{
			dst--;
			src--;
			*dst = *src;
			*(dst - w) = *src;
		}

		dst -= w;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Convert_32SwapRB(ULONG *c, ULONG l)
{
	while(l--)
	{
		*c++ = (*c & 0xFF00FF00) | ((*c & 0xFF0000) >> 16) | ((*c & 0xFF) << 16);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Blend_AlphaColor50(ULONG *src, ULONG *dst, ULONG size, ULONG color)
{
	color = (((color) & 0xFEFEFEFE) >> 1);
	while(size--) *dst++ = (((*src++) & 0xFEFEFEFE) >> 1) + color;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Blend_AlphaColor25(ULONG *src, ULONG *dst, ULONG size, ULONG color)
{
	color = (((color) & 0xFCFCFCFC) >> 2);
	while(size--)
	{
		*dst++ = (*src) - (((*src) & 0xFCFCFCFC) >> 2) + color;
		src++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Blend_Alpha50(ULONG *src, ULONG *dst, ULONG size, ULONG color)
{
	color = (((color) & 0xFE000000) >> 1);
	while(size--)
	{
		*dst++ = ((*src) & 0xFFFFFF) + (((*src) & 0xFE000000) >> 1) + color;
		src++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Blend_Alpha25(ULONG *src, ULONG *dst, ULONG size, ULONG color)
{
	color = (((color) & 0xFC000000) >> 2);
	while(size--)
	{
		*dst++ = (*src) - (((*src) & 0xFC000000) >> 2) + color;
		src++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Blend_Color50(ULONG *src, ULONG *dst, ULONG size, ULONG color)
{
	color = (((color) & 0x00FEFEFE) >> 1);
	while(size--)
	{
		*dst++ = ((*src) & 0xFF000000) + (((*src) & 0x00FEFEFE) >> 1) + color;
		src++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Blend_Color25(ULONG *src, ULONG *dst, ULONG size, ULONG color)
{
	color = (((color) & 0x00FCFCFC) >> 2);
	while(size--)
	{
		*dst++ = (*src) - (((*src) & 0x00FCFCFC) >> 2) + color;
		src++;
	}
}

/*
 =======================================================================================================================
    indirectly keep border of image:: the border is enlarge by 2 so mipmap division will kept the good border
 =======================================================================================================================
 */
void TEX_Blend_KeepBorder(ULONG *src, ULONG x, ULONG y)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	*line, *lastline;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	/* up and down */
	L_memcpy(src + x, src, x << 2);
	L_memcpy(src + (y - 2) * x, src + (y - 1) * x, x << 2);

	/* right and left */
	line = src;
	lastline = line + y * x;
	while(line < lastline)
	{
		line[1] = line[0];
		line[x - 2] = line[x - 1];
		line += x;
	}
}

/*$4
 ***********************************************************************************************************************
    24 to 24 bits operations
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Compress24_Xo2(unsigned char *src, unsigned char *dst, ULONG w, ULONG h)
{
	/*~~~~~~*/
	ULONG	l;
	/*~~~~~~*/

	l = (w * h) >> 1;
	if(!l) return;
	while(l--)
	{
		dst[0] = ((src[0] & 0xFC) >> 1) + ((src[3] & 0xFC) >> 1);
		dst[1] = ((src[1] & 0xFC) >> 1) + ((src[4] & 0xFC) >> 1);
		dst[2] = ((src[2] & 0xFC) >> 1) + ((src[5] & 0xFC) >> 1);
		src += 6;
		dst += 3;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Compress24_Yo2(unsigned char *src, unsigned char *dst, ULONG w, ULONG h)
{
	/*~~~~~~*/
	ULONG	l;
	/*~~~~~~*/

	h >>= 1;
	w *= 3;

	if(!h) return;
	while(h--)
	{
		l = w;
		while(l--)
		{
			dst[0] = ((*src & 0xFC) >> 1) + ((*(src + w) & 0xFC) >> 1);
			src++;
			dst++;
		}

		src += w;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Compress24_Yo2InvertColor(unsigned char *src, unsigned char *dst, ULONG w, ULONG h)
{
	/*~~~~~~*/
	ULONG	l;
	/*~~~~~~*/

	h >>= 1;
	w *= 3;

	if(!h) return;
	while(h--)
	{
		l = w;
		while(l--)
		{
			dst[0] = ~(((*src & 0xFC) >> 1) + ((*(src + w) & 0xFC) >> 1));
			src++;
			dst++;
		}

		src += w;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Expand24_XBy2(unsigned char *src, unsigned char *dst, ULONG w, ULONG h)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	unsigned char	*srccur;
	/*~~~~~~~~~~~~~~~~~~~~*/

	dst += (w * h * 3) << 1;
	srccur = src + (w * h * 3) - 1;

	while(srccur > src)
	{
		*(--dst) = *srccur;
		*(--dst) = *(srccur - 1);
		*(--dst) = *(srccur - 2);
		*(--dst) = *srccur--;
		*(--dst) = *srccur--;
		*(--dst) = *srccur--;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Expand24_YBy2(unsigned char *src, unsigned char *dst, ULONG w, ULONG h)
{
	/*~~~~~~*/
	ULONG	l;
	/*~~~~~~*/

	w *= 3;
	dst += (w * h << 1) - 1;
	src += (w * h) - 1;

	while(h--)
	{
		l = w;
		while(l--)
		{
			*dst = *src;
			*(dst - w) = *src;
			src--;
			dst--;
		}

		dst -= w;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Blend24_Color50(unsigned char *src, unsigned char *dst, ULONG size, ULONG color)
{
	color = (((color) & 0x00FEFEFE) >> 1);
	while(size--)
	{
		*(ULONG *) dst = (((*(ULONG *) src) & 0x00FEFEFE) >> 1) + color;
		src += 3;
		dst += 3;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Blend24_Color25(unsigned char *src, unsigned char *dst, ULONG size, ULONG color)
{
	color = (((color) & 0x00FCFCFC) >> 2);
	while(size--)
	{
		*(ULONG *) dst = (*(ULONG *) src) - (((*src) & 0x00FCFCFC) >> 2) + color;
		src += 3;
		dst += 3;
	}
}

/*
 =======================================================================================================================
    indirectly keep border of image:: the border is enlarge by 2 so mipmap division will kept the good border
 =======================================================================================================================
 */
void TEX_Blend24_KeepBorder(unsigned char *src, ULONG x, ULONG y)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned char	*line, *lastline;
	ULONG			offset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* up and down */
	L_memcpy(src + x, src, x * 3);
	L_memcpy(src + (y - 2) * x * 3, src + (y - 1) * x * 3, x * 3);

	/* right and left */
	line = src;
	lastline = line + y * x * 3;
	offset = (x - 2) * 3;

	while(line < lastline)
	{
		line[3] = line[0];
		line[4] = line[1];
		line[5] = line[2];

		line[offset] = line[offset + 3];
		line[offset + 1] = line[offset + 4];
		line[offset + 2] = line[offset + 5];

		line += x * 3;
	}
}

/*$4
 ***********************************************************************************************************************
    8 to 8 bits operations
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Compress8_Xo2(unsigned char *src, unsigned char *dst, ULONG w, ULONG h)
{
	/*~~~~~~*/
	ULONG	l;
	/*~~~~~~*/

	l = (w * h) >> 1;
	if(!l) return;
	while(l--)
	{
		*dst++ = *src;
		src += 2;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Compress8_Yo2(unsigned char *src, unsigned char *dst, ULONG w, ULONG h)
{
	/*~~~~~~*/
	ULONG	l;
	/*~~~~~~*/

	h >>= 1;

	if(!h) return;
	while(h--)
	{
		l = w;
		while(l--) *dst++ = *src++;
		src += w;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Expand8_XBy2(unsigned char *src, unsigned char *dst, ULONG w, ULONG h)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	unsigned char	*srccur;
	/*~~~~~~~~~~~~~~~~~~~~*/

	dst += (w * h) << 1;
	srccur = src + (w * h) - 1;

	while(srccur > src)
	{
		*(--dst) = *srccur;
		*(--dst) = *srccur--;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Expand8_YBy2(unsigned char *src, unsigned char *dst, ULONG w, ULONG h)
{
	/*~~~~~~*/
	ULONG	l;
	/*~~~~~~*/

	dst += (w * h << 1) - 1;
	src += (w * h) - 1;

	while(h--)
	{
		l = w;
		while(l--)
		{
			*dst = *src;
			*(dst - w) = *src;
			src--;
			dst--;
		}

		dst -= w;
	}
}

/*$4
 ***********************************************************************************************************************
    4 to 4 bits operations
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Compress4_Xo2(unsigned char *src, unsigned char *dst, ULONG w, ULONG h)
{
	/*~~~~~~*/
	ULONG	l;
	/*~~~~~~*/

	l = (w * h) >> 2;
	if(!l) return;
	while(l--)
	{
		*dst++ = ((*src) & 0xF0) | ((*(src + 1)) >> 4);
		src += 2;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Compress4_Yo2(unsigned char *src, unsigned char *dst, ULONG w, ULONG h)
{
	/*~~~~~~*/
	ULONG	l;
	/*~~~~~~*/

	h >>= 1;
	w >>= 1;

	if(!h) return;
	while(h--)
	{
		l = w;
		while(l--) *(dst++) = *(src++);
		src += w;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Expand4_XBy2(unsigned char *src, unsigned char *dst, ULONG w, ULONG h)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	unsigned char	*srccur;
	/*~~~~~~~~~~~~~~~~~~~~*/

	dst += (w * h);
	srccur = src + ((w * h) >> 1) - 1;

	while(srccur > src)
	{
		*dst-- = (*srccur & 0xF) | ((*srccur & 0xF) << 4);
		*dst-- = (*srccur & 0xF0) | ((*srccur & 0xF0) >> 4);
		srccur--;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Expand4_YBy2(unsigned char *src, unsigned char *dst, ULONG w, ULONG h)
{
	/*~~~~~~*/
	ULONG	l;
	/*~~~~~~*/

	dst += (w * h) - 1;
	src += ((w * h) >> 1) - 1;
	w >>= 1;

	while(h--)
	{
		l = w;
		while(l--)
		{
			*dst = *src;
			*(dst - w) = *src;
			src--;
			dst--;
		}

		dst -= w;
	}
}

/*$4
 ***********************************************************************************************************************
    4 to 8 operations
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Convert_4To8( unsigned char *dst, unsigned char *src, ULONG W, ULONG H)
{
    int count;
    
    count = (W * H) >> 1;
    while (count--)
    {
        *dst++ = *src >> 4;
        *dst++ = *src++ & 0xF;
    }
}

/*$4
 ***********************************************************************************************************************
    8 to 4 operations
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Convert_8To4( unsigned char *dst, unsigned char *src, ULONG W, ULONG H)
{
    int count;
    
    count = (W * H) >> 1;
    while (count--)
    {
        *dst++ = ((src[0] & 0xF) << 4) | (src[1] & 0xF);
        src += 2;
    }
}


/*$4
 ***********************************************************************************************************************
    8 to 32 operations
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Convert_8To32(ULONG *dst, unsigned char *src, ULONG *pal, ULONG count)
{
	if(!pal) return;
	while(count--) *dst++ = pal[*src++];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Convert_8To32SwapRB(ULONG *dst, unsigned char *src, ULONG *pal, ULONG count)
{
    ULONG c;
	if(!pal) return;
	while(count--) 
    {
        c = pal[*src++];
        *dst++ = (c & 0xFF00FF00) | ((c & 0xFF) << 16) | ((c & 0xFF0000) >> 16);
    }
}

/*$4
 ***********************************************************************************************************************
    4 to 32 operations
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Convert_4To32(ULONG *dst, unsigned char *src, ULONG *pal, ULONG count)
{
	if(!pal) return;
	while(count)
	{
		*dst++ = pal[(*src) >> 4];
		*dst++ = pal[(*src++) & 0xF];
		count -= 2;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Convert_4To32SwapRB(ULONG *dst, unsigned char *src, ULONG *pal, ULONG count)
{
    ULONG c;

	if(!pal) return;
	while(count)
	{
        c = pal[(*src) >> 4];
        *dst++ = (c & 0xFF00FF00) | ((c & 0xFF) << 16) | ((c & 0xFF0000) >> 16);
				
        c = pal[(*src++) & 0xF];
        *dst++ = (c & 0xFF00FF00) | ((c & 0xFF) << 16) | ((c & 0xFF0000) >> 16);
	
        count -= 2;
	}
}


/*$4
 ***********************************************************************************************************************
    8 to 24 operations
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Convert_8To24(UCHAR *dst, UCHAR *src, ULONG *pal, int w, int h, int pitch)
{
	/*~~~~~*/
	int x, y;
	/*~~~~~*/

	if(!pal) return;

	for(y = 0; y < h; y++)
	{
		for(x = 0; x < w; x++)
		{
			*dst++ = (UCHAR) (pal[*src] & 0xFF);
			*dst++ = (UCHAR) ((pal[*src] >> 8) & 0xFF);
			*dst++ = (UCHAR) ((pal[*src] >> 16) & 0xFF);
			src++;
		}

		dst += pitch;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Convert_8To24_ChannelAlpha(UCHAR *dst, UCHAR *src, ULONG *pal, int w, int h, int pitch)
{
	/*~~~~~*/
	int x, y;
	/*~~~~~*/

	if(!pal) return;

	for(y = 0; y < h; y++)
	{
		for(x = 0; x < w; x++)
		{
			*dst = (UCHAR) (pal[*src] >> 24);
			*(dst + 1) = *dst;
			*(dst + 2) = *dst;
			src++;
			dst += 3;
		}

		dst += pitch;
	}
}

/*$4
 ***********************************************************************************************************************
    4 to 24 operations
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Convert_4To24(UCHAR *dst, UCHAR *src, ULONG *pal, int w, int h, int pitch)
{
	/*~~~~~~~~*/
	int x, y, i;
	/*~~~~~~~~*/

	if(!pal) return;

	for(y = 0; y < h; y++)
	{
		for(x = 0; x < w; x += 2)
		{
			i = *src >> 4;
			*dst++ = (UCHAR) (pal[i] & 0xFF);
			*dst++ = (UCHAR) ((pal[i] >> 8) & 0xFF);
			*dst++ = (UCHAR) ((pal[i] >> 16) & 0xFF);
			i = *src & 0xF;
			*dst++ = (UCHAR) (pal[i] & 0xFF);
			*dst++ = (UCHAR) ((pal[i] >> 8) & 0xFF);
			*dst++ = (UCHAR) ((pal[i] >> 16) & 0xFF);
			src++;
		}

		dst += pitch;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Convert_4To24_ChannelAlpha(UCHAR *dst, UCHAR *src, ULONG *pal, int w, int h, int pitch)
{
	/*~~~~~~~~*/
	int x, y, i;
	/*~~~~~~~~*/

	if(!pal) return;

	for(y = 0; y < h; y++)
	{
		for(x = 0; x < w; x += 2)
		{
			i = *src >> 4;
			*dst++ = (UCHAR) (pal[i] >> 24);
			*dst++ = (UCHAR) (pal[i] >> 24);
			*dst++ = (UCHAR) (pal[i] >> 24);
			i = *src & 0xF;
			*dst++ = (UCHAR) (pal[i] >> 24);
			*dst++ = (UCHAR) (pal[i] >> 24);
			*dst++ = (UCHAR) (pal[i] >> 24);
			src++;
		}

		dst += pitch;
	}
}

#undef LongFromAdd

/* mamagouille */
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
