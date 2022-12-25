/*$T OGLtex.c GC!1.55 01/21/00 12:03:51 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Load texture in memory to be used by OpenGL RC.
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include <eeregs.h>
#include <eestruct.h>
#include <libgraph.h>
#include <libdma.h>
#include <libvu0.h>
#include <sifdev.h>
#include <libpc.h>


#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMpro.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/BAStypes.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "TEXture/TEXconvert.h"
#include "TEXture/TEXmemory.h"

#include "Gsp.h"
#include "GSPinit.h"
#include "GSPtex.h"

//#undef GSP_USE_TEXTURE_CACHE

#if defined PSX2_TARGET && defined __cplusplus
    extern "C" {
#endif

extern u_int TextureUsed = 0;
extern u_int ShowBpp;
extern __declspec(scratchpad) u_int volatile gsulSyncroPath;


GSP_PaletteManager *GSP_p_First_PM = NULL;


GSP_PaletteManager *GSP_GetAPaletteManager(u_int Size)
{
	GSP_PaletteManager *pNew;
	pNew = MEM_p_Alloc(sizeof(GSP_PaletteManager) );
	L_memset(pNew , 0 , sizeof (GSP_PaletteManager ) ) ;
	pNew -> ColorField = MEM_p_AllocAlign(Size , 16);
	pNew -> pNext = GSP_p_First_PM;
	GSP_p_First_PM = pNew;
	return GSP_p_First_PM;
}


/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */
void GSP_CacheFlushSegment(u_int Address , u_int SizeInOctets)
{
	FlushCache(0);
}
/*
 =======================================================================================================================
    Aim:    Create a texture from given texture data
 =======================================================================================================================
 */
void GSP_DrawFrame(u_int *p_FirstPixel , u_int W , u_int H , u_int BPP)
{
	ULONG XCounter, Color;
	static ULONG ColorCounter = 0;
	switch (BPP)
	{
	case 4:
		W >>= 3;
		break;
	case 8:
		W >>= 2;
		break;
	case 16:
		W >>= 1;
		break;
	case 32:
		break;
	}/*
	XCounter = W;
	while (XCounter--) 
	{
		*(p_FirstPixel + XCounter) = 0xff;
		*(p_FirstPixel + (H - 1) * W + XCounter) = 0xff;
	}
	YCounter = H;
	while (YCounter--) 
	{
		*(p_FirstPixel + YCounter * W + 0) = 0xff;
		*(p_FirstPixel + YCounter * W + (W - 1)) = 0xff;
	}
	*/
	XCounter = W * H;
	Color = (ColorCounter & 0x3) << 6;
	Color |= ((ColorCounter >> 2) & 0x3) << 14;
	Color |= ((ColorCounter >> 4) & 0x3) << 22;
	while (XCounter--)
		*(p_FirstPixel + XCounter) = Color;
	
	
	ColorCounter++;
}
void GSP_TEX_Copy32(u_int *pSRC , u_int *pDST , u_int Size)
{
	while   (Size--) 
	{
		u_int Color;
		Color = *(pSRC++);
		Color = (Color & 0xffffff) | ((Color & 0xfe000000) >> 1);
		*(pDST++) = Color;
	}
}
void GSP_TEX_Copy4(u_int *pSRC , u_int *pDST , u_int Size)
{
	while (Size--)
	{
		*pDST = (*pSRC & 0x0f0f0f0f) << 4;
		*pDST |= (*pSRC & 0xf0f0f0f0) >> 4;
		pDST++;
		pSRC++;
	}
}
GspGifTag stPictureGifTag[4] __attribute__((aligned(64)));
void GSP_VRamLoadText(u_int VramPtr , u_int DBW , u_int *p_FirstPixel , u_int W , u_int H , u_int BPP , u_int IsPalette)
{
	GspGifTag *p_PictureGifTag ;
	u_int 		PSM;
	u_int Size;
	u_int *p_SPR;
	u_int Base;
	gsulSyncroPath |= GIF_LOCKED;
	if (BPP == 32) 
		PSM = 0;
	else
	if (BPP == 16) 
		PSM = 2;
	else
	if (BPP == 8) 
		PSM = 0x13;
	else
	if (BPP == 4) 
		PSM = 0x14;
	
	p_PictureGifTag = &stPictureGifTag[0];
	GSP_UnCachePtr(p_PictureGifTag);
	Gsp_Flush(FLUSH_ALL);

	GSP_SetRegister(GSP_BITBLTBUF 	, 
			(/*SBP*/0L << 0L)|
			(/*SBW*/0L << 16L)|
			(/*SPSM*/((u_long)PSM) << 24L)|
			(/*DBP*/((u_long)VramPtr) << 32L)|
			(/*DBW*/((u_long)DBW) << 48L)|
			(/*DPSM*/((u_long)PSM) << 56L));
	GSP_SetRegister(GSP_TRXPOS 	, (/*SX*/0L << 0L)|(/*SY*/0L << 16L)|(/*DX*/0L << 32L)|(/*DY*/0L << 48L)|/*DIR*/(0L << 59L));
	GSP_SetRegister(GSP_TRXREG 	, (/*RRW*/((u_long)W) << 0L)|(/*RRH*/((u_long)H) << 32L));
	GSP_SetRegister(GSP_TRXDIR 	, (0L << 0L));
	Gsp_SetGSRegisters();
	
	
	Size = (W * H * BPP) >> 5 ;
	p_SPR = (u_int *)0x70002000;
	Base = 0;
	while (Size)
	{
		u_int  GifSize;
		if (Size > (1024 * 2))
		{
			Size -= (1024 * 2);
			GifSize = 1024 * 2;
		} else
		{
			GifSize = Size;
			Size = 0;
		}
		if (BPP == 32)
		{
			GSP_TEX_Copy32(p_FirstPixel + Base , p_SPR , GifSize);
		} else
		if (BPP == 4)
		{
			GSP_TEX_Copy4(p_FirstPixel + Base , p_SPR , GifSize);
		} 	else
		{
			L_memcpy(p_SPR , p_FirstPixel + Base , GifSize << 2);
		} 			
		Gsp_M_SetGifTag(p_PictureGifTag, GifSize>>2 , 1 , 0 , 0 , GSP_GIF_FLG_IMAGE , 0 , 0);
		Gsp_SendToGif((u_int)p_PictureGifTag, 1L);
		Gsp_SendToGif( (u_int) p_SPR, GifSize >> 2);
		Gsp_Flush(FLUSH_DMA2);
		Base += GifSize;
	}
	Gsp_Flush(FLUSH_ALL);
	gsulSyncroPath &= ~GIF_LOCKED;
}

#define GSP_SetRegister_SYNCRO_DMA1(Register , Value) \
	((MyRegister *)p_CurrentRegWriter)[NumReg].ulRegister = (unsigned long)Register;\
	((MyRegister *)p_CurrentRegWriter)[NumReg].ulValueToWrite = (unsigned long)Value;\
	NumReg++;


u_int GSP_VRamLoadText_FROM_RAM(u_int VramPtr , u_int DBW , u_int *p_FirstPixel , u_int W , u_int H ,u_int DX,u_int DY,  u_int BPP , u_int TagAddress)
{
	u_int 		PSM;
	u_int 		Size;
	u_int		NumReg;
	u_long128 *p_CurrentRegWriter;
	u_int TotalGifSize;
	u_int *VifDirectAddress;
	if (BPP == 32) 
		PSM = 0;
	else
	if (BPP == 16) 
		PSM = 2;
	else
	if (BPP == 8) 
		PSM = 0x13;
	else
	if (BPP == 4) 
		PSM = 0x14;
		
	TotalGifSize = 0;
	
	p_CurrentRegWriter = (u_long128 *)TagAddress;
	VifDirectAddress =  (u_int *)(p_CurrentRegWriter + 1);
	p_CurrentRegWriter += 3;
	NumReg = 0;
	GSP_SetRegister_SYNCRO_DMA1(GSP_BITBLTBUF 	, 
			(/*SBP*/0L << 0L)|
			(/*SBW*/0L << 16L)|
			(/*SPSM*/((u_long)PSM) << 24L)|
			(/*DBP*/((u_long)VramPtr) << 32L)|
			(/*DBW*/((u_long)DBW) << 48L)|
			(/*DPSM*/((u_long)PSM) << 56L));
	GSP_SetRegister_SYNCRO_DMA1(GSP_TRXPOS 	, (/*SX*/0L << 0L)|(/*SY*/0L << 16L)|(/*DX*/((u_long)DX) << 32L)|(/*DY*/((u_long)DY) << 48L)|/*DIR*/(0L << 59L));
	GSP_SetRegister_SYNCRO_DMA1(GSP_TRXREG 	, (/*RRW*/((u_long)W) << 0L)|(/*RRH*/((u_long)H) << 32L));
	GSP_SetRegister_SYNCRO_DMA1(GSP_TRXDIR 	, (0L << 0L));
	p_CurrentRegWriter--;
	Gsp_M_SetGifTag((GspGifTag *)p_CurrentRegWriter, NumReg , 1 , 0 , 0 , GSP_GIF_FLG_PACKED , 1 ,   0xE);
	TotalGifSize += NumReg + 1;
	p_CurrentRegWriter-= 2;
	GSP_SpecialStoreDma(GSP_SCDma_ID_Cnt , NumReg + 2/*VIFTAG + GIFTAG*/ , 0 , 	(GSP_DMA_Source_Chain_TAG*)p_CurrentRegWriter);
	p_CurrentRegWriter += NumReg + 3/* DMATAG + VIFTAG + GIF */;

	Size = (W * H * BPP) >> 5 ;
	while (Size)
	{
		u_int  GifSize;
		if (Size > 2048)
		{
			Size -= 2048;
			GifSize = 2048;
		} else
		{
			GifSize = Size;
			Size = 0;
		}
		GSP_SpecialStoreDma(GSP_SCDma_ID_Cnt , 1/*GIF*/ , 0 , 	(GSP_DMA_Source_Chain_TAG*)p_CurrentRegWriter);
		p_CurrentRegWriter++;
		Gsp_M_SetGifTag((GspGifTag *)p_CurrentRegWriter, GifSize>>2 , 1 , 0 , 0 , GSP_GIF_FLG_IMAGE , 0 , 0);
		p_CurrentRegWriter++;
		GSP_SpecialStoreDma(GSP_SCDma_ID_Ref , GifSize >> 2 , (u_int) p_FirstPixel , 	(GSP_DMA_Source_Chain_TAG*)p_CurrentRegWriter);
		p_CurrentRegWriter++;
		TotalGifSize += (GifSize >> 2) + 1 /* GIFTAG */;
		
		p_FirstPixel += GifSize;
	}
	VifDirectAddress[0] = 0;
	VifDirectAddress[1] = 0;
	VifDirectAddress[2] = 0x11000000; /* FLUSH*/
	VifDirectAddress[3] = 0x50000000 | TotalGifSize; /* DIRECT */
	GSP_SpecialStoreDma(GSP_SCDma_ID_End , 0 , 0 , 	(GSP_DMA_Source_Chain_TAG*)p_CurrentRegWriter);
	return ((u_int)p_CurrentRegWriter);
}
 
typedef struct GSP_VRAM_BLOCK_MASK_
{
	u_int ulNumberOfMasks;
	u_int ulNumberOfShifts;
	u_int ulMask[256];
} GSP_VRAM_BLOCK_MASK;
//static u_int CurrentVMemPtr;
static u_int VRM_BlockBits[514];

void Gsp_InitVRAMManager()
{
//	CurrentVMemPtr = 0x200000; /* 4 Mo */
//	CurrentVMemPtr = 0x4000; /* 4 Mo */
	memset(VRM_BlockBits , 0 , 512 * 4);
	VRM_BlockBits[512] = 0xffffffff;
	VRM_BlockBits[513] = 0xffffffff;
}
void VRM_SetBit(GSP_VRAM_BLOCK_MASK *p_Mask , ULONG Num)
{
	while (p_Mask->ulNumberOfMasks <= (Num >> 5)) p_Mask->ulMask[p_Mask->ulNumberOfMasks++] = 0;
	p_Mask->ulMask[Num >> 5] |= 0x80000000 >> (Num & 31);
}

void VRM_MoveLeft(GSP_VRAM_BLOCK_MASK *p_Mask)
{
	u_int MaskCounter;
	MaskCounter = p_Mask->ulNumberOfMasks;
	while (MaskCounter--) p_Mask-> ulMask[MaskCounter + 1] = p_Mask-> ulMask[MaskCounter] ;
	p_Mask-> ulMask[0] = 0;
	p_Mask->ulNumberOfMasks++;
	while (!(p_Mask-> ulMask[p_Mask->ulNumberOfMasks - 1] & 1))
	{
		MaskCounter = p_Mask->ulNumberOfMasks;
		while (MaskCounter--)
		{	
			p_Mask-> ulMask[MaskCounter] >>= 1;
			if (MaskCounter)
			{
				p_Mask-> ulMask[MaskCounter] |= p_Mask-> ulMask[MaskCounter-1] << 31;
			}
		}
	}
}

void VRM_GetMask8Bits(u_int W , u_int H , GSP_VRAM_BLOCK_MASK *p_Mask)
{
	u_int WD4 , HD4;
	u_int WCounter , HCounter , ulNumberOfPages;
	WD4 = lMax(W>>4 , 1);
	HD4 = lMax(H>>4 , 1);
	ulNumberOfPages = (WD4 * HD4) >> 5;
	if (WD4 > 8) WD4 = 8;
	if (HD4 > 4) HD4 = 4;
	/* Compute for 1 page */
	p_Mask->ulNumberOfMasks = 0;
	for (WCounter = 0 ; WCounter < WD4 ; WCounter ++)
		for (HCounter = 0 ; HCounter < HD4 ; HCounter ++)
		{
			u_int ulBlockResult;
			ulBlockResult =  ((WCounter & 1) << 0) | ((WCounter & 2) << 1) | ((WCounter & 4) << 2) | ((WCounter & 8) << 3) | ((WCounter & 16) << 4); // 0 2 4 6 8
			ulBlockResult |= ((HCounter & 1) << 1) | ((HCounter & 2) << 2) | ((HCounter & 4) << 3) | ((HCounter & 8) << 4) | ((HCounter & 16) << 5); // 1 3 5 7 9
			VRM_SetBit(p_Mask , ulBlockResult);
		}
	/* then extend to nexts pages */
	while (ulNumberOfPages --)
	{
		p_Mask->ulMask[p_Mask->ulNumberOfMasks++] = p_Mask->ulMask[p_Mask->ulNumberOfMasks - 1];
	}
	VRM_MoveLeft(p_Mask);
}

void VRM_GetMask4Bits(u_int W , u_int H , GSP_VRAM_BLOCK_MASK *p_Mask)
{
	u_int WD4 , HD4;
	u_int WCounter , HCounter , ulNumberOfPages;
	WD4 = lMax(W>>5,1);
	HD4 = lMax(H>>4,1);
	ulNumberOfPages = (WD4 * HD4) >> 5;
	if (WD4 > 4) WD4 = 4;
	if (HD4 > 8) HD4 = 8;
	p_Mask->ulNumberOfMasks = 0;
	for (WCounter = 0 ; WCounter < WD4 ; WCounter ++)
		for (HCounter = 0 ; HCounter < HD4 ; HCounter ++)
		{
			u_int ulBlockResult;
			ulBlockResult =  ((HCounter & 1) << 0) | ((HCounter & 2) << 1) | ((HCounter & 4) << 2) | ((HCounter & 8) << 3) | ((HCounter & 16) << 4); // 0 2 4 6 8
			ulBlockResult |= ((WCounter & 1) << 1) | ((WCounter & 2) << 2) | ((WCounter & 4) << 3) | ((WCounter & 8) << 4) | ((WCounter & 16) << 5); // 1 3 5 7 9
			VRM_SetBit(p_Mask , ulBlockResult);
		}
	while (ulNumberOfPages --)
	{
		p_Mask->ulMask[p_Mask->ulNumberOfMasks++] = p_Mask->ulMask[p_Mask->ulNumberOfMasks - 1];
	}
	VRM_MoveLeft(p_Mask);
}

void VRM_GetMask32Bits(u_int W , u_int H , GSP_VRAM_BLOCK_MASK *p_Mask)
{
	u_int WD4 , HD4;
	u_int WCounter , HCounter , ulNumberOfPages;
	WD4 = lMax(W>>3 , 1);
	HD4 = lMax(H>>3 , 1);
	ulNumberOfPages = (WD4 * HD4) >> 5;
	if (WD4 > 8) WD4 = 8;
	if (HD4 > 4) HD4 = 4;
	/* Compute for 1 page */
	p_Mask->ulNumberOfMasks = 0;
	for (WCounter = 0 ; WCounter < WD4 ; WCounter ++)
		for (HCounter = 0 ; HCounter < HD4 ; HCounter ++)
		{
			u_int ulBlockResult;
			ulBlockResult =  ((WCounter & 1) << 0) | ((WCounter & 2) << 1) | ((WCounter & 4) << 2) | ((WCounter & 8) << 3) | ((WCounter & 16) << 4); // 0 2 4 6 8
			ulBlockResult |= ((HCounter & 1) << 1) | ((HCounter & 2) << 2) | ((HCounter & 4) << 3) | ((HCounter & 8) << 4) | ((HCounter & 16) << 5); // 1 3 5 7 9
			VRM_SetBit(p_Mask , ulBlockResult);
		}
	/* then extend to nexts pages */
	while (ulNumberOfPages --)
	{
		p_Mask->ulMask[p_Mask->ulNumberOfMasks++] = p_Mask->ulMask[p_Mask->ulNumberOfMasks - 1];
	}
	VRM_MoveLeft(p_Mask);
}

void VRM_GetMask(u_int BPP , u_int W , u_int H , GSP_VRAM_BLOCK_MASK *p_Mask)
{
	u_int LinearOK;
	LinearOK = 1;
	memset(p_Mask , 0 , sizeof(GSP_VRAM_BLOCK_MASK));
	if (W < H) 
	{
		switch (BPP)
		{
			case 32:
				if (W >= 64)
				{
					LinearOK = 1;
				}
				else
				{
					LinearOK = 0;
					VRM_GetMask32Bits(W , H , p_Mask);
				} ;//*/
				break;
			case 16:
				if (W >= 64)
				{
					LinearOK = 1;
				} else W = H;
				break;
			case 8:
				if (W >= 128)
				{
					LinearOK = 1;
				} else 
				{
					LinearOK = 0;
					VRM_GetMask8Bits(W , H , p_Mask);
				}
				break;
			case 4:
				if (W >= 128)
				{
					LinearOK = 1;
				} else
				{
					LinearOK = 0;
					VRM_GetMask4Bits(W , H , p_Mask);
				}
				break;
		}
	} //*/
	else 
	{
		if (H < W >> 1) H = W >> 1;
	}
	if (LinearOK)
	{
		u_int ulSizeInBlocks;
		/* Contiguous Vram -> OK */
		ulSizeInBlocks = (W * H * BPP) >> 3;
		ulSizeInBlocks += 0xff;
		ulSizeInBlocks >>= 8; // Align on blocks boundary
		p_Mask->ulNumberOfMasks = 1;
		p_Mask->ulMask[0] = 0;
		
		if (ulSizeInBlocks & 31) p_Mask->ulMask[p_Mask->ulNumberOfMasks++] = ~(0xffffffff << (ulSizeInBlocks & 31));
		while (ulSizeInBlocks >= 32) 
		{
			p_Mask->ulMask[p_Mask->ulNumberOfMasks++] = 0xffffffff;
			ulSizeInBlocks -= 32;
		}
	}
	p_Mask->ulMask[p_Mask->ulNumberOfMasks] = 0;
	p_Mask->ulMask[p_Mask->ulNumberOfMasks + 1] = 0;
	p_Mask->ulNumberOfShifts = 0;
	return;
}
void VRM_ShiftOne(GSP_VRAM_BLOCK_MASK *p_Mask)
{
	u_int MaskCounter , ulLastByte ;
	ulLastByte = 0;
	p_Mask->ulNumberOfShifts++;
	for (MaskCounter = 0 ; MaskCounter < p_Mask->ulNumberOfMasks ; MaskCounter++)
	{
		p_Mask-> ulMask[MaskCounter] <<= 1;
		p_Mask-> ulMask[MaskCounter] |= p_Mask-> ulMask[MaskCounter + 1] >> 31;
	}
	if (p_Mask->ulNumberOfShifts == 32) 
	{
		MaskCounter = p_Mask->ulNumberOfMasks;
		while (MaskCounter--)
			p_Mask-> ulMask[MaskCounter + 1] = p_Mask-> ulMask[MaskCounter] ;
		p_Mask-> ulMask[0] = 0;
		p_Mask->ulNumberOfShifts = 0;
	}
}

u_int VRM_GetFirstBit(GSP_VRAM_BLOCK_MASK *p_Mask)
{
	ULONG ReturnValue , Counter;
	ReturnValue = 0;
	Counter = 0;
	while (p_Mask -> ulMask[Counter] == 0)
	{
		Counter ++;
		ReturnValue += 32;
	}
	Counter= p_Mask -> ulMask[Counter];
	while (!(Counter & 0x80000000))
	{
		Counter <<= 1;
		ReturnValue++;
	}
	return ReturnValue;
}

u_int VRM_CanStore(u_int Num , u_int *p_VRM , u_int *p_TXT)
{
	while (Num--) if (*(p_VRM++) & *(p_TXT++)) return 0;
	return 1;
}
u_int VRM_findAGoodBlock(GSP_VRAM_BLOCK_MASK *p_Mask , u_int BlockBase)
{
	u_int ulBitCounter;
	u_int *p_VRM_BlockBits;
	/* Start from Last Block */
	p_VRM_BlockBits = &VRM_BlockBits[BlockBase];
	p_VRM_BlockBits -= p_Mask -> ulNumberOfMasks;
	while (*(p_VRM_BlockBits+1) == 0xffffffff) p_VRM_BlockBits--;
	while (p_VRM_BlockBits >= VRM_BlockBits)
	{
		ulBitCounter = 32;
		while (ulBitCounter)
		{
			if (VRM_CanStore(p_Mask -> ulNumberOfMasks , p_VRM_BlockBits , p_Mask -> ulMask)) 
			{
				return ((p_VRM_BlockBits - VRM_BlockBits) << 5) + VRM_GetFirstBit(p_Mask);
			}
			VRM_ShiftOne(p_Mask);
			ulBitCounter--;
		}
		p_VRM_BlockBits--;
		while (*(p_VRM_BlockBits+1) == 0xffffffff) p_VRM_BlockBits--;
	}
	return 0;
}
void VRM_SetMask(u_int BlockNumber , GSP_VRAM_BLOCK_MASK *p_Mask)
{
	u_int Counter;
	BlockNumber -= VRM_GetFirstBit(p_Mask);
	BlockNumber>>= 5;
	Counter = p_Mask  -> ulNumberOfMasks;
	while (Counter--) VRM_BlockBits[BlockNumber + Counter] |= p_Mask  -> ulMask[Counter];
}

#ifdef GSP_USE_TEXTURE_CACHE
static u_int CurrentTextCachePtr = 0;
static u_int CurrentTextCachePtr_I = 0;
#endif
u_int GSP_GetBlockSize( u_int BPP , u_int W , u_int H ) 
{
	switch (BPP)
	{
		case 32:
			if (W < 64)
			{
				if (H > W) W = H;
				if (W > H) H = W;
			} 
			break;
		case 8:
			if (W < 128)
			{
				if (H > W) W = H;
				if (W > H) H = W;
			}
			break;
		case 4:
			if (W < 128)
			{
				if (H > W) W = H;
				if (W > H) H = W;
			} 
			break;
	}
	return (W * H * BPP) >> 3;
}
u_int GSP_GetMemPtr_Interface( u_int BPP , u_int W , u_int H ) 
{
	GSP_VRAM_BLOCK_MASK stMask;
	u_int ulBlckNum;
#ifdef GSP_USE_TEXTURE_CACHE
	if (GspGlobal_ACCESS(ulAvailableTextureCacheSize))
	{
		u_int ulSizeInBlocks;
		u_int ulReturnValue;
		ulSizeInBlocks = GSP_GetBlockSize( BPP , W , H ) ;
		ulSizeInBlocks += 0xff;
		ulSizeInBlocks >>= 8; // Align on blocks boundary
		ulReturnValue = 320 + CurrentTextCachePtr_I + (GspGlobal_ACCESS(ZBP) << 5);
		CurrentTextCachePtr_I += ulSizeInBlocks;
		return ulReturnValue;
	}
#endif
	VRM_GetMask(BPP , W , H , &stMask);
	ulBlckNum = VRM_findAGoodBlock(&stMask , GspGlobal_ACCESS(TBP));
	VRM_SetMask(ulBlckNum , &stMask);
	return (ulBlckNum) << 0; // Return WORD(WORD! = 2 BYTE) adress
}
u_int GSP_GetMemPtr( u_int BPP , u_int W , u_int H ) 
{
	GSP_VRAM_BLOCK_MASK stMask;
	u_int ulBlckNum;
	if ((BPP == 4) && (W == 64) && (H == 256))
		W = 128;
#ifdef GSP_USE_TEXTURE_CACHE
	if (GspGlobal_ACCESS(ulAvailableTextureCacheSize))
	{
		u_int ulSizeInBlocks;
		u_int ulReturnValue;
		ulSizeInBlocks = GSP_GetBlockSize( BPP , W , H ) ;
		ulSizeInBlocks += 0xff;
		ulSizeInBlocks >>= 8; // Align on blocks boundary
		ulReturnValue = CurrentTextCachePtr + (GspGlobal_ACCESS(TBP) << 5);
		CurrentTextCachePtr += ulSizeInBlocks;
		if (CurrentTextCachePtr + (GspGlobal_ACCESS(TBP) << 5) >= (1024 * 1024 * 4 - 1024 * 8) >> 8)
		{
			GSP_FLushAllTextureCache();
			ulReturnValue = CurrentTextCachePtr + (GspGlobal_ACCESS(TBP) << 5);
			CurrentTextCachePtr += ulSizeInBlocks;
		}
		return ulReturnValue;
	}
#endif	
	VRM_GetMask(BPP , W , H , &stMask);
	ulBlckNum = VRM_findAGoodBlock(&stMask , 512);
	VRM_SetMask(ulBlckNum , &stMask);
	return (ulBlckNum) << 0; // Return WORD(WORD! = 2 BYTE) adress
}

u_long GSP_GetPowerOf2( u_int K ) 
{
	switch (K)
	{
		case 1:return 	0L;
		case 2:return 	1L;
		case 4:return 	2L;
		case 8:return 	3L;
		case 16:return 	4L;
		case 32:return 	5L;
		case 64:return 	6L;
		case 128:return 7L;
		case 256:return 8L;
		case 512:return 9L;
		case 1024:return 10L;
	}
	return 0;
}
void GSP_TEX_ComputeMipMap_32(u32 *BitMapP,u32 W ,u32 H)
{
	u32 H2;
	u32 *pBMpMM,*pBMpLast;
	while ((H > 1) && (W > 1))
	{
		pBMpMM = BitMapP + W*H;
		H2 = H;
		while (H2)
		{
			pBMpLast = BitMapP+W;
			while (BitMapP < pBMpLast)
			{
				*pBMpMM = ((*BitMapP & 0xfcfcfcfc)>>2) +((*(BitMapP+1) & 0xfcfcfcfc)>>2) + ((*(BitMapP+W) & 0xfcfcfcfc)>>2) + ((*(BitMapP+W+1) & 0xfcfcfcfc)>>2);
				BitMapP += 2;
				pBMpMM++;
			}
			H2-=2;
			BitMapP += W;
		}
		H >>= 1;
		W >>= 1;
	} 
}
void GSP_TEX_ComputeMipMap_8(u8 *BitMapP,u32 W ,u32 H)
{ 
	u32 H2;
	u8 *pBMpMM,*pBMpLast;
	while ((H > 1) && (W > 1))
	{
		pBMpMM = BitMapP + W*H;
		H2 = H;
		while (H2)
		{
			pBMpLast = BitMapP+W;
			while (BitMapP < pBMpLast)
			{
				*pBMpMM = *BitMapP;
				BitMapP += 2;
				pBMpMM++;
			}
			H2-=2;
			BitMapP += W;
		}
		H >>= 1;
		W >>= 1;
	} 
}
void GSP_TEX_ComputeMipMap_24(u8 *BitMapP,u32 W ,u32 H)
{ 
	u32 H2;
	u8 *pBMpMM,*pBMpLast;
	while ((H > 1) && (W > 1))
	{
		pBMpMM = BitMapP + W*H*3;
		H2 = H;
		while (H2)
		{
			pBMpLast = BitMapP+W*3;
			while (BitMapP < pBMpLast)
			{
				*pBMpMM = (*(BitMapP)>>2) + (*(BitMapP+3)>>2) + (*(BitMapP+W*3)>>2) + (*(BitMapP+W*3+3)>>2);
				pBMpMM++;
				BitMapP++;
				*pBMpMM = (*(BitMapP)>>2) + (*(BitMapP+3)>>2) + (*(BitMapP+W*3)>>2) + (*(BitMapP+W*3+3)>>2);
				pBMpMM++;
				BitMapP++;
				*pBMpMM = (*(BitMapP)>>2) + (*(BitMapP+3)>>2) + (*(BitMapP+W*3)>>2) + (*(BitMapP+W*3+3)>>2);
				pBMpMM++;
				BitMapP++;
				BitMapP += 3;
			}
			H2-=2;
			BitMapP += W*3;
		}
		H >>= 1;
		W >>= 1;
	} 
}
void GSP_TEX_ComputeMipMap_4(u8 *BitMapP,u32 W ,u32 H)
{
	u32 H2;
	u8 *pBMpMM,*pBMpLast;
	W >>= 1;
	while ((H > 1) && (W > 1))
	{
		pBMpMM = BitMapP + W*H;
		H2 = H;
		while (H2)
		{
			pBMpLast = BitMapP+W;
			while (BitMapP < pBMpLast)
			{
				*pBMpMM = (*BitMapP & 0xf) | ((*(BitMapP+1) & 0xf)<<4);
				BitMapP += 2;
				pBMpMM++;
			}
			H2-=2;
			BitMapP += W;
		}
		H >>= 1;
		W >>= 1;
	} 
}
void GSP_TEX_ComputeMipMap(u32 *BitMapP,u32 W ,u32 H, u32 BPP)
{
	if (BPP==32)
		GSP_TEX_ComputeMipMap_32(BitMapP,W ,H);
	else
	if (BPP==24)
		GSP_TEX_ComputeMipMap_24(BitMapP,W ,H);
	else
	if (BPP==8)
		GSP_TEX_ComputeMipMap_8((u8 *)BitMapP,W ,H);
	else
	if (BPP==4)
		GSP_TEX_ComputeMipMap_4((u8 *)BitMapP,W ,H);
}

void GSP_Texture_32_2_16(u_int *p_32,u_int Size)
{
	int *p_16;
	p_16 = (int*)p_32;
	Size >>= 1;
	while (Size--)
	{
		*p_16 = (*p_32 & 0x80000000) | ((*p_32 & 0xf80000) << 7) | ((*p_32 & 0xf800) << 10) | ((*p_32 & 0xf8) << 13);
		p_32++;
		*p_16 |= ((*p_32 & 0x80000000) | ((*p_32 & 0xf80000) << 7) | ((*p_32 & 0xf800) << 10) | ((*p_32 & 0xf8) << 13)) >> 16;
		p_32++;
		p_16++;
	}
}
/*
void STOREPIXEL4(unsigned char *Image,u_int W,u_int H,u_int X,u_int Y,u_int Color)
{	
	Image+= (H * Y) >> 1;
	Color &= 0xf;
	if (X & 1)
	{
		Image[X >> 1] ^= Color << 4;
	} else
		Image[X >> 1] ^= Color<<0;
	
}

/* Stocke la texture en RAM pour chargement différé en VRAM */
/* retourne 1 si la texture est stockée et ne doit pas être stocké immédiatement en VRAM */
/* retourne 0 sinon */
LONG GSP_l_Texture_Store
(
    GDI_tdst_DisplayData    *_pst_DD,
    TEX_tdst_Data           *_pst_TexData,
    TEX_tdst_File_Desc      *_pst_Tex,
    ULONG 		    ulTextNum 
)
{
    return 0;
}

//*/
void GSP_Texture_Load(
    GDI_tdst_DisplayData    *_pst_DD,
    TEX_tdst_Data           *_pst_TexData,
    TEX_tdst_File_Desc      *_pst_Tex,
    ULONG 		    ulTextNum 
)
{
	u_int DPtr , DPW , BPP , PSM , W, H , IsInterface;
	BPP = _pst_Tex->uc_FinalBPP;
	W = _pst_Tex->uw_Width;
	H = _pst_Tex->uw_Height;
	MEMpro_StartMemRaster();
	
	IsInterface = 0;
	//IsInterface = (_pst_Tex->st_Params.uw_Flags & TEX_FP_Interface);
//	if (_pst_Tex->uw_DescFlags & TEX_Cuw_DF_SpecialNoLoad) IsInterface = 1;//*/
	DPtr = 0;
	if (IsInterface)
	{
		DPtr = GSP_GetMemPtr_Interface( BPP , W , H ) ;
	}
	else
		DPtr = GSP_GetMemPtr( BPP , W , H ) ;	
	if ((BPP == 4) && (W == 64) && (H == 256))
		DPW  = (W >> 5);
	else//*/
		DPW  = (W >> 6);
	
	if (BPP == 32) 
	{
		PSM = 0;
	}
	else
	{
		if (BPP == 8) 
		{
			PSM = 0x13;
		}
		else
		if (BPP == 4) 
		{
			PSM = 0x14;
		}
	}

/*	if (W == H)
	_pst_Tex->st_Params.uw_Flags |= TEX_FP_MipmapOn;
	else*/
	_pst_Tex->st_Params.uw_Flags &= ~TEX_FP_MipmapOn;

	if (!DPW) DPW = 1;
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].DBW = DPW;
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].p_BitmapPtr = NULL;
#ifdef GSP_USE_TEXTURE_CACHE
	if (IsInterface || (GspGlobal_ACCESS(ulAvailableTextureCacheSize)))
#else	
	if (IsInterface)
#endif	
	{
		u32 MemorySize;
		MemorySize = (W * H * BPP) >> 3;
		if(_pst_Tex->st_Params.uw_Flags & TEX_FP_MipmapOn)
		{
			MemorySize += MemorySize >> 1;
		}
		
		GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].p_BitmapPtr = MEM_p_AllocAlign(MemorySize,16);
		if (BPP == 32)
		{
			GSP_TEX_Copy32((u_int*)_pst_Tex->p_Bitmap , GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].p_BitmapPtr , W * H);
		} else
		if (BPP == 4)
		{
			GSP_TEX_Copy4((u_int*)_pst_Tex->p_Bitmap , GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].p_BitmapPtr , (W * H)>>3);
		} 	else// 8 bits
		{
			L_memcpy(GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].p_BitmapPtr , (u_int*)_pst_Tex->p_Bitmap , W * H);
		}
		/* Compute Mipmapping */
		if(_pst_Tex->st_Params.uw_Flags & TEX_FP_MipmapOn)
		{
			GSP_TEX_ComputeMipMap(GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].p_BitmapPtr,W , H, BPP);
		}
		
	}
	else
	{
		GSP_VRamLoadText((u_int)DPtr ,(u_int)DPW , (u_int*)_pst_Tex->p_Bitmap, (u_int)W , (u_int)H , (u_int)BPP , 0 );
	}
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].W = W;
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].H = H;
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].FBP = DPtr;
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].ulBigKey = _pst_TexData->ul_Key;

	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].TEX0_REG = 
		((u_long)DPtr)| 
		(((u_long)DPW)<<14L)|
		
		(((u_long)PSM)<<20L)|
		(GSP_GetPowerOf2( W ) << 26L)|
		(GSP_GetPowerOf2( H )<< 30L)|
		(1L<<34L)|
		(0L<<35L)|
		0L;

	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].TEX1_REG = 0L;
	if(_pst_Tex->st_Params.uw_Flags & TEX_FP_MipmapOn)
	{
		GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].TEX1_REG = 
			(0L)| 
			(3L<<2L)| 
			(1L<<5L)| 
			(4L<<6L)| // Bilinear
			(1L<<9L)| 
			(0L<<19L)| // L
			(0L<<32L); // K
	}
	

	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].ZOB = BPP;
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].TEXCLUT = 0L;
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].InterfaceFlags = 0;
	if (IsInterface)
	{
		GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].InterfaceFlags = GSP_InterfaceTexture;
	}
	Gsp_DrawBuffer_Prepare(); /* Must be call after each SPR acces */
	MEMpro_StopMemRaster(MEMpro_Id_GSP);
}
void GSP_RemapInterfaceTexture(u_int OldTBP)
{
	u_int CounterShnik , NewBP;
	CounterShnik = GspGlobal_ACCESS(ulNumberOfTextures);
	GSP_FLushAllTextureCache();
	NewBP = GspGlobal_ACCESS(TBP);
	OldTBP <<= 5;
	NewBP <<= 5;
	
	while (CounterShnik--)
	{
		if (GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[CounterShnik].InterfaceFlags & (GSP_InterfaceTexture | GSP_IsChildOfInterfaceTexture))
		{
			GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[CounterShnik].FBP -= OldTBP;
			GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[CounterShnik].FBP += NewBP;
			GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[CounterShnik].TEX0_REG -= (u_long)OldTBP;
			GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[CounterShnik].TEX0_REG += (u_long)NewBP;
		}
	}
}
void GSP_SetAnimatedTexture(u_int ulTextureAntIndex , u_int ulTextureToSet)
{
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextureAntIndex] = GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextureToSet];
}
void GSP_LoadTexture_SYNCRO_DMA(u_int ulTextureIndex , void *PixelPtr)
{
	u_int pCBPS,TagAddress ;
	*(u_int *)&PixelPtr &= 0x0fffffff;
	TagAddress = ((u_int)GspGlobal_ACCESS(p_CurrentBufferPointer));
	TagAddress = GSP_VRamLoadText_FROM_RAM
				((u_int)GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextureIndex].FBP ,
				 (u_int)GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextureIndex].DBW ,
				 (u_int*)PixelPtr,
				 (u_int)GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextureIndex].W ,
				 (u_int)GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextureIndex].H ,
				 0,
				 0,
				 (u_int)GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextureIndex].ZOB ,
				 TagAddress );
	pCBPS = (((u_int)GspGlobal_ACCESS(p_CurrentBufferPointer) - (u_int)GspGlobal_ACCESS(BX_LastCntPtr)) >> 4) - 1;
	GSP_SpecialStoreDma(GSP_SCDma_ID_Cnt , pCBPS , 0 , GspGlobal_ACCESS(BX_LastCntPtr));
	GspGlobal_ACCESS(BX_LastCntPtr) = (GSP_DMA_Source_Chain_TAG *)TagAddress;
	GspGlobal_ACCESS(p_CurrentBufferPointer) = TagAddress + 16;
//	if (!(gsulSyncroPath & FLUSH_DMA1)) Gsp_FIFO_STOP();
}
void GSP_LoadPalette_SYNCRO_DMA(u_int ulPaletteIndex , void *ColorPtr)
{
	u_int pCBPS,TagAddress ;
	TagAddress = ((u_int)GspGlobal_ACCESS(p_CurrentBufferPointer));
	TagAddress = GSP_VRamLoadText_FROM_RAM
				((u_int)GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[ulPaletteIndex].VramPtr ,
				 1 ,
				 (u_int*)ColorPtr,
				 (u_int)GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[ulPaletteIndex].W ,
				 (u_int)GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[ulPaletteIndex].H ,
				 0,
				 0,
				 (u_int)GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[ulPaletteIndex].BPP ,
				 TagAddress );
	pCBPS = (((u_int)GspGlobal_ACCESS(p_CurrentBufferPointer) - (u_int)GspGlobal_ACCESS(BX_LastCntPtr)) >> 4) - 1;
	GSP_SpecialStoreDma(GSP_SCDma_ID_Cnt , pCBPS , 0 , GspGlobal_ACCESS(BX_LastCntPtr));
	GspGlobal_ACCESS(BX_LastCntPtr) = (GSP_DMA_Source_Chain_TAG *)TagAddress;
	GspGlobal_ACCESS(p_CurrentBufferPointer) = TagAddress + 16;
//	if (!(gsulSyncroPath & FLUSH_DMA1)) Gsp_FIFO_STOP();
}

void GSP_LoadCachedTexture(u_int TIndex)
{
#ifdef GSP_USE_TEXTURE_CACHE
	u_int pCBPS,TagAddress , VRAMPTR;
	if (GspGlobal_ACCESS(ulAvailableTextureCacheSize))
	{
		U32 MipMapCounter,W,H,DBW;
		u32 p_BitmapPtr;
		if (ShowBpp) return;
		if (!GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TIndex].p_BitmapPtr) return;
		if (GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TIndex].InterfaceFlags & GSP_IsInVram) return;
		
		W = (u_int)GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TIndex].W;
		H = (u_int)GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TIndex].H;
		p_BitmapPtr = (u_int)GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TIndex].p_BitmapPtr ;
		DBW = GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TIndex].DBW ;
		
		MipMapCounter = 0;
		while (MipMapCounter < 4)
		{
			if(GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TIndex].InterfaceFlags & GSP_InterfaceTexture)
				VRAMPTR = GSP_GetMemPtr_Interface( GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TIndex].ZOB , W , H );
			else
				VRAMPTR = GSP_GetMemPtr( GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TIndex].ZOB , W , H );
			if (!MipMapCounter)
			{
				GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TIndex].InterfaceFlags|=GSP_IsInVram;
				GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TIndex].TEX0_REG -= GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TIndex].FBP;
				GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TIndex].FBP = VRAMPTR;
				GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TIndex].TEX0_REG += GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TIndex].FBP;
			}
			
			TagAddress = pCBPS = ((u_int)GspGlobal_ACCESS(p_CurrentBufferPointer));
			TagAddress = GSP_VRamLoadText_FROM_RAM(
							 VRAMPTR ,
							 DBW ,
							 (u32*)p_BitmapPtr ,
							 W ,
							 H ,
							 0,
							 0,
							 (u_int)GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TIndex].ZOB ,
							 TagAddress );
			GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TIndex].InterfaceFlags |= GSP_MustBeFlushed;
			if (TagAddress != pCBPS)
			{
				u_int sUBv;
				sUBv = (((u_int)GspGlobal_ACCESS(p_CurrentBufferPointer) - (u_int)GspGlobal_ACCESS(BX_LastCntPtr)) >> 4) - 1;
				GSP_SpecialStoreDma(GSP_SCDma_ID_Cnt , sUBv , 0 , GspGlobal_ACCESS(BX_LastCntPtr));
				GspGlobal_ACCESS(BX_LastCntPtr) = (GSP_DMA_Source_Chain_TAG *)TagAddress;
				GspGlobal_ACCESS(p_CurrentBufferPointer) = TagAddress + 16;
			}
			if (GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TIndex].TEX1_REG == 0L)
			{
				
				W = H = 0;
				MipMapCounter = 1000;
			}
			else
			{
				p_BitmapPtr += (W*H*(u_int)GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[TIndex].ZOB)>>3;
				W>>=1;
				H>>=1;	
				MipMapCounter++;
				if (DBW != 1) DBW>>=1;
			}
			//W = H = 0;
		}
	}
#endif
}
void GSP_LoadCachedPalette(u_int PIndex)
{
#ifdef GSP_USE_TEXTURE_CACHE
	u_int pCBPS,TagAddress , VRAMPTR;
	if (GspGlobal_ACCESS(ulAvailableTextureCacheSize))
	{	
		if (ShowBpp) return;
		if (!GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PIndex].Clut256) return;
		
		if (GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PIndex].InterfaceFlags & GSP_IsInVram) return;
		VRAMPTR = GSP_GetMemPtr( GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PIndex].BPP , 
								GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PIndex].W , 
								GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PIndex].H );

		GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PIndex].InterfaceFlags |= GSP_IsInVram;
		GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PIndex].ul64_SetTexturePaletteParam -= 
					(((u_long)GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PIndex].VramPtr)<<37L); //CLut Base PTR
		GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PIndex].VramPtr = VRAMPTR;
		GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PIndex].ul64_SetTexturePaletteParam += 
					(((u_long)GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PIndex].VramPtr)<<37L); //CLut Base PTR
		
		TagAddress = pCBPS = ((u_int)GspGlobal_ACCESS(p_CurrentBufferPointer));
		TagAddress = GSP_VRamLoadText_FROM_RAM
				((u_int)GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PIndex].VramPtr ,
				 1 ,
				 (u_int*)GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PIndex].Clut256->ColorField,
				 (u_int)GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PIndex].W ,
				 (u_int)GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PIndex].H ,
				 0,
				 0,
				 (u_int)GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PIndex].BPP ,
				 TagAddress );
		if (TagAddress != pCBPS)
		{
			u_int sUBv;
			sUBv = (((u_int)GspGlobal_ACCESS(p_CurrentBufferPointer) - (u_int)GspGlobal_ACCESS(BX_LastCntPtr)) >> 4) - 1;
			GSP_SpecialStoreDma(GSP_SCDma_ID_Cnt , sUBv , 0 , GspGlobal_ACCESS(BX_LastCntPtr));
			GspGlobal_ACCESS(BX_LastCntPtr) = (GSP_DMA_Source_Chain_TAG *)TagAddress;
			GspGlobal_ACCESS(p_CurrentBufferPointer) = TagAddress + 16;
		}
	}
#endif
}
void GSP_FLushAllTextureCache()
{
#ifdef GSP_USE_TEXTURE_CACHE
	if (GspGlobal_ACCESS(ulAvailableTextureCacheSize))
	{
		u_int CounterShnik;
		CurrentTextCachePtr	 = 0;
		TextureUsed = 1;
		CounterShnik = GspGlobal_ACCESS(ulNumberOfTextures);
		while (CounterShnik--)
			GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[CounterShnik].InterfaceFlags &= ~GSP_IsInVram;
		CounterShnik = GspGlobal_ACCESS(ulNumberOfPalettes);
		while (CounterShnik--)
			GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[CounterShnik].InterfaceFlags &= ~GSP_IsInVram;
	}
#endif		
}
void GSP_LoadInterfaceTexture()
{
	u_int CounterShnik;
	u_int pCBPS,TagAddress;
#ifdef GSP_USE_TEXTURE_CACHE
	if (GspGlobal_ACCESS(ulAvailableTextureCacheSize))
	{
		CurrentTextCachePtr_I = 0;
		CounterShnik = GspGlobal_ACCESS(ulNumberOfTextures);
		while (CounterShnik--)
		{
			if (GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[CounterShnik].InterfaceFlags & GSP_InterfaceTexture)
				GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[CounterShnik].InterfaceFlags &= ~GSP_IsInVram;
		}
		return;
	}
#endif
	TagAddress = pCBPS = ((u_int)GspGlobal_ACCESS(p_CurrentBufferPointer));
	CounterShnik = GspGlobal_ACCESS(ulNumberOfTextures);
	while (CounterShnik--)
	{
		if (GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[CounterShnik].InterfaceFlags & GSP_InterfaceTexture)
		{
			TagAddress = GSP_VRamLoadText_FROM_RAM
							((u_int)GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[CounterShnik].FBP ,
							 (u_int)GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[CounterShnik].DBW ,
							 (u_int*)GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[CounterShnik].p_BitmapPtr ,
							 (u_int)GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[CounterShnik].W ,
							 (u_int)GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[CounterShnik].H ,
							 0,
							 0,
							 (u_int)GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[CounterShnik].ZOB ,
							 TagAddress );
			GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[CounterShnik].InterfaceFlags &= ~GSP_IsNeeded;
		}
	}

	if (TagAddress != pCBPS)
	{
		CounterShnik = (((u_int)GspGlobal_ACCESS(p_CurrentBufferPointer) - (u_int)GspGlobal_ACCESS(BX_LastCntPtr)) >> 4) - 1;
		GSP_SpecialStoreDma(GSP_SCDma_ID_Cnt , CounterShnik , 0 , GspGlobal_ACCESS(BX_LastCntPtr));
		GspGlobal_ACCESS(BX_LastCntPtr) = (GSP_DMA_Source_Chain_TAG *)TagAddress;
		GspGlobal_ACCESS(p_CurrentBufferPointer) = TagAddress + 16;
/*		if (!(gsulSyncroPath & FLUSH_DMA1))
			Gsp_FIFO_STOP();*/
	}
}

extern void StoreZBUFVIF1( u_long128* base_addr, short start_addr, short pixel_mode, int buff_width,short x, short y, short width, short height );

void    GSP_Texture_GetBackFromVram()
{
#ifdef GSP_USE_TEXTURE_CACHE
#endif	
}

void    GSP_Texture_Unload( GDI_tdst_DisplayData * )
{
	u_int CounterShnik;
	MEMpro_StartMemRaster();
	GSP_FLushAllTextureCache();
	CounterShnik = GspGlobal_ACCESS(ulNumberOfTextures);
	while (CounterShnik--)
	{
		if (( GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[CounterShnik].p_BitmapPtr) && 
			(!(GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[CounterShnik].InterfaceFlags & GSP_IsChildOfInterfaceTexture)))
		{
			MEM_FreeAlign(GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[CounterShnik].p_BitmapPtr);
		}
	}
	if (GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)) MEM_FreeAlign(GspGlobal_ACCESS(p_MyTextureDescriptorsAligned));
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned) = NULL;
	GspGlobal_ACCESS(ulNumberOfTextures) = 0;
	if (GspGlobal_ACCESS(ulNumberOfPalettes))
	{
		MEM_FreeAlign(GspGlobal_ACCESS(p_MyPalletteDescriptorAligned));
		GspGlobal_ACCESS(ulNumberOfPalettes) = 0;
	}
#ifdef GSP_USE_TEXTURE_CACHE
	while (GSP_p_First_PM)
	{
		GSP_PaletteManager *pN;
		pN = GSP_p_First_PM->pNext;
		MEM_FreeAlign(GSP_p_First_PM->ColorField);
		MEM_Free(GSP_p_First_PM);
		GSP_p_First_PM = pN;
	}
	GSP_p_First_PM = NULL;
#endif
	MEMpro_StopMemRaster(MEMpro_Id_GSP);
}
/* Following 2 variables are used to compress 4 bits palette in VRAM */
static u_short GSP_ul_Current16Bitspallette_4BPP[1024];
static u_int GSP_ul_CurrentPalletteNum = 0; /* 0 to 16 */
static u_int GSP_ul_CurrentPalletteDPTR;
u_short GSP_32_to_16(u_int b32)
{
	u_short b16;
	b16 = (u_short)((b32 >> 3) & 0x1f);
	b16 |= (u_short)((b32 >> 6) & (0x1f << 5));
	b16 |= (u_short)((b32 >> 9) & (0x1f << 10));
	if (b32 & 0xf0000000)
		b16 |= 0x8000;
	return b16;
}

u_int Gsp_Test4BitsPaletteForAlpha(u_int *p_Palette)
{
	u_int  ColorCounter;
 	u_int  ulNumBerOfAlpha;
	ColorCounter = 16;
	ulNumBerOfAlpha = 0;
	while (ColorCounter --)
	{
		u_int Alpha;
		Alpha = *(p_Palette++) >> 24;
		if ((Alpha > 16) && (Alpha < 256-16)) return 0;
		
	}
	return 1;
}


void    GSP_Palette_Load( GDI_tdst_DisplayData *WhatstheFuck, TEX_tdst_Palette *p_PAL, ULONG PaletteNum)
{
	u_int DPtr;
	
	MEMpro_StartMemRaster();
	if (!GspGlobal_ACCESS(ulNumberOfPalettes))
	{
		GspGlobal_ACCESS(ulNumberOfPalettes)			 = 512;
		GspGlobal_ACCESS(p_MyPalletteDescriptorAligned) = (Gsp_tdst_Palette *) MEM_p_AllocAlign(sizeof(Gsp_tdst_Palette) * GspGlobal_ACCESS(ulNumberOfPalettes) , 8);
		memset(GspGlobal_ACCESS(p_MyPalletteDescriptorAligned),0,sizeof(Gsp_tdst_Palette) * GspGlobal_ACCESS(ulNumberOfPalettes));
	}
	if (p_PAL->uc_Flags & TEX_uc_Palette16)
	{ // Convert palette;
		ULONG SRC,DST;
		u_short Clut[512];
		u_int *p_SRC;
		p_SRC = p_PAL->pul_Color;
		if (!p_SRC) p_SRC = (u_int *)Clut;
		
#ifdef GSP_USE_TEXTURE_CACHE
		if ((!GspGlobal_ACCESS(ulAvailableTextureCacheSize)) && (Gsp_Test4BitsPaletteForAlpha(p_PAL->pul_Color)))
#else		
		if (Gsp_Test4BitsPaletteForAlpha(p_PAL->pul_Color))
#endif		
		{
/*----------------------------------------------------------*/		
/* Save it As a 16 bits Palettes for best filling of memory */
/*----------------------------------------------------------*/		
			if (GSP_ul_CurrentPalletteNum == 0) 
			{	
				SRC = 32 * 32;
				while (SRC--) GSP_ul_Current16Bitspallette_4BPP[SRC] = ((SRC ^ (SRC >> 4)) & 4) ? 0xffff : 0;
				GSP_ul_CurrentPalletteDPTR = GSP_GetMemPtr( 16 , 32 , 32 ) ;
			}
			DPtr = GSP_ul_CurrentPalletteDPTR;
			
			for (SRC = 0;SRC < 16 ; SRC ++)
			{
				GSP_ul_Current16Bitspallette_4BPP[SRC + GSP_ul_CurrentPalletteNum] = GSP_32_to_16(p_SRC[SRC]);
			}
			for (SRC = 0;SRC < 512 ; SRC ++)
			{
				DST = SRC;
				Clut[DST] = GSP_ul_Current16Bitspallette_4BPP[SRC];
			}
			GSP_VRamLoadText((u_int)DPtr ,1 , (u_int*)GSP_ul_Current16Bitspallette_4BPP/*Clut*/, 32 , 32 , 16 , 1);
			GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].VramPtr = (u_int)DPtr;
			GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].W = 32;
			GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].H = 32;
			GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].BPP = 16;
			GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].ul64_SetTexturePaletteParam = 
				/* TEX0_1 Settings */
				(((u_long)DPtr)<<37L)| //CLut Base PTR
				(2L << 51L)|/* 16 bits */
				(2L << 61L)|
				(1L << 55L)|/* CSM2 */
				((u_long)(0L) << (56L - 4L))| // CSA = CLUT MENTRY OFFSET
				/* TEXCLUT Settings */
				(1L << 0L)|/* TEX buf W / 64*/
				((u_long)(1 & (GSP_ul_CurrentPalletteNum >> 4)) << 6L)|
				((u_long)(GSP_ul_CurrentPalletteNum >> 5) << 12L)|
				0L;
			GSP_ul_CurrentPalletteNum += 16;
			GSP_ul_CurrentPalletteNum &= 0x3ff;
		} else
		{
/*--------------------------------------------------------------------*/		
/* without if Alphas are used.. In this case, I use A 32 bits palette */
/*--------------------------------------------------------------------*/
			ULONG SRC,DST;
			u_int Clut[256];
			u_int *p_SRC;
			p_SRC = p_PAL->pul_Color;
			if (!p_SRC) p_SRC = Clut;
			DPtr = GSP_GetMemPtr( 32 , 8 , 2 ) ;
			for (SRC = 0;SRC < 16 ; SRC ++)
			{
				DST = SRC;
				Clut[DST] = p_SRC[SRC];
			}
#ifdef GSP_USE_TEXTURE_CACHE
			if (GspGlobal_ACCESS(ulAvailableTextureCacheSize))
			{
				GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].InterfaceFlags |= GSP_InterfaceTexture;
				GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].Clut256 = GSP_GetAPaletteManager(16 * 4);
				GSP_TEX_Copy32(Clut , GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].Clut256->ColorField ,  16);
			} else
#endif
				GSP_VRamLoadText((u_int)DPtr ,1 , (u_int*)Clut, 8 , 2 , 32 , 1);
			GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].VramPtr = (u_int)DPtr;
			GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].W = 8;
			GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].H = 2;
			GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].BPP = 32;
			GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].ul64_SetTexturePaletteParam = 
				(((u_long)DPtr)<<37L)| //CLut Base PTR
				(1L << 61L)|
				0L;
		}
	} else	
	{ // Convert palette;
		ULONG SRC,DST;
		u_int Clut[256];
		u_int *p_SRC;
		p_SRC = p_PAL->pul_Color;
		if (!p_SRC) p_SRC = Clut;
		DPtr = GSP_GetMemPtr( 32 , 16 , 16 ) ;
		for (SRC = 0;SRC < 256 ; SRC ++)
		{
			DST = SRC;
			if ((DST & 0x18) == 0x08)
			{
				DST += 8;
			} else
			if ((DST & 0x18) == 0x10)
			{
				DST -= 8;
			}
			Clut[DST] = p_SRC[SRC];
		}
#ifdef GSP_USE_TEXTURE_CACHE
		if (GspGlobal_ACCESS(ulAvailableTextureCacheSize))
		{
			GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].InterfaceFlags |= GSP_InterfaceTexture;
			GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].Clut256 = GSP_GetAPaletteManager(256 * 4);
			GSP_TEX_Copy32(Clut , GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].Clut256->ColorField ,  256);
		} else
#endif		
			GSP_VRamLoadText((u_int)DPtr ,1 , (u_int*)Clut, 16 , 16 , 32 , 1);
		GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].VramPtr = (u_int)DPtr;
		GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].W = 16;
		GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].H = 16;
		GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].BPP = 32;
		GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[PaletteNum].ul64_SetTexturePaletteParam = 
			(((u_long)DPtr)<<37L)| //CLut Base PTR
			(1L << 61L)|
			0L;
	}
	Gsp_DrawBuffer_Prepare(); /* Must be call after each SPR acces */
	MEMpro_StopMemRaster(MEMpro_Id_GSP);
}
void    GSP_Set_Texture_Palette( GDI_tdst_DisplayData *WhatstheFuck, ULONG ulTextNum, ULONG IdexTex, ULONG IndexPal )
{
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].TEX0_REG =  GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[IdexTex].TEX0_REG;
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].TEX0_REG |= GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[IndexPal].ul64_SetTexturePaletteParam & 0xffffffff00000000L;
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].TEX1_REG =  GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[IdexTex].TEX1_REG;
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].TEXCLUT  =  GspGlobal_ACCESS(p_MyPalletteDescriptorAligned)[IndexPal].ul64_SetTexturePaletteParam & 0x00000000ffffffffL;
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].W =  GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[IdexTex].W;
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].H =  GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[IdexTex].H;
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].ZOB =  GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[IdexTex].ZOB;
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].ulBigKey = GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[IdexTex].ulBigKey;
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].InterfaceFlags = 0;
#ifdef GSP_USE_TEXTURE_CACHE
	if (GspGlobal_ACCESS(ulAvailableTextureCacheSize))
	{
		GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].ulPaletteIndex = IndexPal + 1;
		GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].ulMotherTextureIndex = IdexTex + 1;
	} 
#endif
	if (GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[IdexTex].InterfaceFlags & GSP_InterfaceTexture)
	{
		GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].InterfaceFlags = GSP_IsChildOfInterfaceTexture;
		GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[ulTextNum].p_BitmapPtr = (u_int *)&GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)[IdexTex];
	}
}

#define CB 0x00	
#define CBD 0xff0000
#define CF 0xff
void GSP_ShowAvailableTextureMemory()
{
	ULONG Texture[8*8];
	u_int VramPtr,Counter;
	Texture = (ULONG [])
	{
		CBD,CB,CF,CF,CF,CF,CB,CB,
		CBD,CB,CF,CF,CF,CF,CB,CB,
		CBD,CB,CF,CF,CB,CB,CB,CB,
		CBD,CB,CF,CF,CF,CF,CB,CB,
		CBD,CB,CF,CF,CB,CB,CB,CB,
		CBD,CB,CF,CF,CB,CB,CB,CB,
		CBD,CB,CF,CF,CB,CB,CB,CB,
		CBD,CB,CB,CB,CB,CB,CB,CB,
	};
	VramPtr = GSP_GetMemPtr( 32 , 8 , 8 );
	while (VramPtr > GspGlobal_ACCESS(TBP) << 5)
	{
		GSP_VRamLoadText(VramPtr , 1 , Texture , 8 , 8 , 32 , 0);
		VramPtr = GSP_GetMemPtr( 32 , 8 , 8 );
	} 
	Counter = 64;
	while (Counter--) Texture[Counter] ^= 0xffffff;
	GSP_VRamLoadText(VramPtr , 1 , Texture , 8 , 8 , 32 , 0);

}

#define TDESCALIGN 0xf
LONG    GSP_l_Texture_Init( GDI_tdst_DisplayData *_pst_DD, ULONG Number)
{
	MEMpro_StartMemRaster();

	if (GspGlobal_ACCESS(p_MyTextureDescriptorsAligned)) MEM_FreeAlign(GspGlobal_ACCESS(p_MyTextureDescriptorsAligned));
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned) = NULL;
	GspGlobal_ACCESS(ulNumberOfTextures) = Number;
	GspGlobal_ACCESS(p_MyTextureDescriptorsAligned) = (Gsp_tdst_Texture *)MEM_p_AllocAlign(sizeof(Gsp_tdst_Texture) * GspGlobal_ACCESS(ulNumberOfTextures) , 8);
	memset(GspGlobal_ACCESS(p_MyTextureDescriptorsAligned),0,sizeof(Gsp_tdst_Texture) * GspGlobal_ACCESS(ulNumberOfTextures));

	if (GspGlobal_ACCESS(ulNumberOfPalettes)) MEM_FreeAlign(GspGlobal_ACCESS(p_MyPalletteDescriptorAligned));
	GspGlobal_ACCESS(ulNumberOfPalettes)			 = 0;
	GspGlobal_ACCESS(p_MyPalletteDescriptorAligned) = NULL;
#ifdef GSP_USE_TEXTURE_CACHE
	while (GSP_p_First_PM)
	{
		GSP_PaletteManager *pN;
		pN = GSP_p_First_PM->pNext;
		MEM_FreeAlign(GSP_p_First_PM->ColorField);
		MEM_Free(GSP_p_First_PM);
		GSP_p_First_PM = pN;
	}
	GSP_p_First_PM = NULL;
#endif
	
	GSP_ul_CurrentPalletteNum = 0; 
	Gsp_InitVRAMManager();
	MEMpro_StopMemRaster(MEMpro_Id_GSP);

	return 1;
}


#if defined PSX2_TARGET && defined __cplusplus
    }
#endif
