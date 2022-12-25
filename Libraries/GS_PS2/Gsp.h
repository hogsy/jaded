#ifndef GSP_H
#define GSP_H
#include "GS_PS2/Gsp_Def.h"
#include "GS_PS2/Gsp_Bench.h"
//#include "BASsys.h"
#include "ENGvars.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define GSP_PAL /* else is NTSC */

#define GSP_UnCachePtr(a) *((u_int *)&(a)) |= 0x20000000;
#define GSP_UnCacheAccPtr(a) *((u_int *)&(a)) |= 0x30000000;
void PUSH_COLOR(u32 Value);
void POP_COLOR();


#define GSP_ON_OFF

#ifdef USE_SAVE_XYZ_BUFFER
//this will be aliased on the flush (gsulSyncroPath) flag
#define PUSH_XYZW_SAVE		0x200
#define PUSH_XYZW_RESTORE	0x100
#define PUSH_XYZW_ACTIVE	(PUSH_XYZW_SAVE | PUSH_XYZW_RESTORE)
#endif
#ifndef _FINAL_	
#define FLUSH_WL() if (gsulSyncroPath & FLUSH_WORLDLOAD) *(int *)0 = *(int *)0;
#else
#define FLUSH_WL()
#endif
#define	FLUSH_FLOATINGFLIP	0x400
#define	FLUSH_WORLDLOAD		0x200
#define	GIF_LOCKED			0x100
#define	FLUSH_USER			0x80
#define FLUSH_DMA9			0x40
#define FLUSH_DMA8			0x20
#define FLUSH_GIF 			0x10
#define FLUSH_VU1 			0x08
#define FLUSH_VIF1 			0x04
#define FLUSH_DMA2 			0x02
#define FLUSH_DMA1			0x01
#define FLUSH_ALL			0x7fL

#define GSP_ALIGN_X(a,b) ((((u_int)a) + (b-1)) & ~(b - 1))
#define GSP_ALIGN(a) GSP_ALIGN_X(a,16)

void Gsp_InitFB(u_int H_Resolution, u_int VRES, u_int ColorBitNum , u_int ZBufferBitNum);
struct GSP_tdst_SpecificData_ *GSP_pst_CreateDevice(void);
void Gsp_FlipFB();
void Gsp_FloatingFlip_START();
void GSP_FlushAllFloatingFlip();
void Gsp_FloatingFlip_STOP(u_int ulCalledFrom);
void GSP_SetVideoCRTC(int bufferNum , int both);
int  Gsp_SyncVB(u_int VBNumber);
void Gsp_SendToVif1(u_int Address , u_int Size);
void Gsp_SendToGif(u_int Address , u_int Size);
void Gsp_SetGSRegisters();
void Gsp_DrawRectangle(float x, float y, float w, float h, u_int C0,u_int C1,unsigned long T);
void Gsp_LoadVU1Code();
void GSP_TV_SetBackColor(u_int Color);
void GSP_RestoreCorrectPath(u_int Mode);
void GSP_Save_SPR();
void GSP_Restore_SPR();
void Gsp_Crash(ULONG CrashAddress , unsigned char *TEXT );
extern __declspec(scratchpad) Gsp_BigStruct gs_st_Globals 	;
#define  	GspGlobal_ACCESS(a) gs_st_Globals.a
extern __declspec(scratchpad) u_int volatile gsulSyncroPath;
//#define GSP_FLUSH_DBG
void Gsp_Flush_FAST(int ulAnd);

#ifdef _DEBUG
void Gsp_Flush_DBG(u_int Waht);
#define Gsp_Flush Gsp_Flush_DBG
#else
#define Gsp_Flush Gsp_Flush_FAST 
#endif

/* VIF dma */
_inline_ void Gsp_SendToDma1(u_int Address , u_int CHCR)
{
	while (gsulSyncroPath & FLUSH_DMA1){};
#ifdef _DEBUG	
	if (Address & 0xf)
	{
		*(u_int *)0 = *(u_int *)0; /* DBUG : Adress must be aligned on 64! */	
	}
#endif	
	gsulSyncroPath |= FLUSH_DMA1;
        *D1_QWC  = 0x00;
        if ((Address & 0xf0000000) != 0x70000000)
		*D1_TADR = Address & 0x8fffffff;
	else
		*D1_TADR = (Address ^ 0xf0000000);
	*D1_CHCR = CHCR;
}
/* GIF dma */
_inline_ void Gsp_SendToDma2(u_int Address , u_int CHCR)
{
	while (gsulSyncroPath & FLUSH_DMA2){};
#ifdef _DEBUG	
	if (Address & 0xf)
	{
		*(u_int *)0 = *(u_int *)0; /* DBUG : Adress must be aligned on 64! */	
	}
#endif	
	gsulSyncroPath |= FLUSH_DMA2;
        *D2_QWC  = 0x00;
        if ((Address & 0xf0000000) != 0x70000000)
		*D2_TADR = Address & 0x8fffffff;
	else
		*D2_TADR = (Address ^ 0xf0000000);
	*D2_CHCR = CHCR;
}
/* SPR DMA */
_inline_ void Gsp_SendToDma9(u_int Address , u_int AddressDest , u_int CHCR)
{
	while (gsulSyncroPath & FLUSH_DMA9){};
#ifdef _DEBUG	
	if (Address & 0xf)
	{
		*(u_int *)0 = *(u_int *)0; /* DBUG : Adress must be aligned on 64! */	
	}
#endif	
	gsulSyncroPath |= FLUSH_DMA9;
        *D9_QWC  = 0x00;
        *D9_SADR  = AddressDest;
        if ((Address & 0xf0000000) != 0x70000000)
		*D9_TADR = Address & 0x8fffffff;
	else
		*D9_TADR = (Address ^ 0xf0000000);
	*D9_CHCR = CHCR;
}
_inline_ u_int GetSP()
{
	register u_int ReturnVal;
	asm __volatile__ ("add     ReturnVal , sp, $0");
	return ReturnVal;
}
_inline_ void SetSP(u_int SPValue) {asm __volatile__ ("add     sp, SPValue , $0");}


/* Draw function */
void GS_GetZBuffer(ULONG *p_Destination);
void GSP_SetTextureBlending12(Gsp_BigStruct *p_Globals);

_inline_ void GSP_SetTextureBlending(ULONG _l_Texture, ULONG BM , ULONG AF)
{
	register Gsp_BigStruct *p_LocalBig;
	p_LocalBig = &gs_st_Globals;
#ifdef GSP_PS2_BENCH
	if (p_LocalBig->Status & GSP_Status_Show_Depth)
	{
		_l_Texture = 0xffffffff;
		BM = MAT_Cul_Flag_NoZWrite | MAT_Cul_Flag_UseLocalAlpha;
		MAT_SET_Blending(BM, MAT_Cc_Op_Alpha);
	}
	{
		extern ULONG ShowNormals;
		if (ShowNormals && ((ShowNormals & 3) != 2))
		{
			_l_Texture = 0xffffffff;
			if (ShowNormals & 4)
			{
				BM = MAT_Cul_Flag_NoZWrite;
				MAT_SET_Blending(BM, MAT_Cc_Op_Add);
			} else {
				MAT_SET_Blending(BM, MAT_Cc_Op_Copy);
			}
		}
	}
#endif	
#ifdef GSP_PS2_BENCH
	if (!(p_LocalBig->Status & GSP_Status_SetRenderStateOn)) return;
#endif
/*	if ((p_LocalBig->ulLastBMD == BM) && (p_LocalBig->ulLastTXT == _l_Texture) && (!p_LocalBig->bSRS_FromScratch)) 
	{
		p_LocalBig->bSRSMustBeUodate = 0;
	} else*/
	{
		p_LocalBig->ulNewTXT = _l_Texture;
		p_LocalBig->ulNewBMD = BM;
		p_LocalBig->bSRSMustBeUodate = 1;
	}
}

void Gsp_DrawBufferFirstInit();
void Gsp_DrawBuffer_Prepare();

void Gsp_InitVU1Matrix_Extention();

void Gsp_InitVU1Matrix_FOG(ULONG Color,float Zmin,float Zmax);

void Gsp_SetDrawBuffer();
void Gsp_SetDrawBufferRenderState();
void Gsp_DrawBuf_JustAfterFlip();

void Gsp_SetDiffuse2XMode();

MyVectorFormat 	*Gsp_SetDrawBuffer_XYZ_COLORS	(Gsp_BigStruct *p_BIG , u_int NumberOfPoints , u_int *p_Colors);
void Gsp_SetDrawBuffer_XYZ_COLORS_Extern(Gsp_BigStruct *p_BIG , u_int NumberOfPoints , MyVectorFormat *p_Vertexes , ULONG *p_Colors);
MyVectorFormat 	*Gsp_SetDrawBuffer_XYZ_COLORS_UV(Gsp_BigStruct *p_BIG , u_int NumberOfPoints , u_int *p_Colors);
MyVectorFormat 	*Gsp_SetDrawBuffer_XYZ_FUR_COLORS_UV(Gsp_BigStruct *p_BIG , u_int NumberOfPoints , u_int *p_Colors , void *p_UV);

struct GSP_GEOCacheBlock_ *GSP_GetABlock(struct GSP_GEOCacheBlock_ **p_FreeList);
void GSP_FreeABlock(struct GSP_GEOCacheBlock_ **p_FreeList , struct GSP_GEOCacheBlock_ *p_Block );
void GSP_ManageGeoCache();
void GSP_InitGeoCache(u_int CacheSize);
void GSP_InvalidateGeoCache(u_int CacheSize);
void Gsp_FIFO_STOP();
void Gsp_FIFO_STOP_LAST();

extern u_int My_Triangle_code 	__attribute__((section(".vudata")));
extern u_int My_Jungle_code		__attribute__((section(".vudata")));
void GSP_Vu1LoadCode(u_int Label);



_inline_ u_long128 Gsp_64_to_128(u_long X)
{
	u_long128 Retu;
	asm __volatile__ ("
		pcpyld		Retu,X,zero
   ");
   return Retu;
	
}

void Gsp_SetDrawBuffer_CMD1(Gsp_BigStruct *p_BIG , u_int NumberOfPrims);

_inline_ MyVectorFormat *Gsp_SetDrawBuffer_PLANAR_UVS(Gsp_BigStruct *p_BIG )
{
	register u_long128 *p_ReturnValue;
	
	GSP_Vu1LoadCode(( u_int )&My_Triangle_code);
	
	p_ReturnValue = (u_long128 *)p_BIG->p_CurrentBufferPointer;
	*(p_ReturnValue++) = Gsp_64_to_128(p_BIG->MDB->VIF_PlanarUV);
	p_BIG->p_CurrentBufferPointer = (u_int)p_ReturnValue + GSP_ALIGN(sizeof(MyVectorFormat) * 2);
	return (MyVectorFormat *)p_ReturnValue;
}


_inline_ MyRegister *Gsp_SetDrawBuffer_SRS(Gsp_BigStruct *p_BIG )
{
	return (MyRegister *)(p_BIG->p_CurrentBufferPointer + 16 /* VIF */ + 16 /* GIF */);
}

void Gsp_SetDrawBuffer_CMD2(Gsp_BigStruct *p_BIG , u_int TNumregs );
void Gsp_BE_PrepareHidenBuffer();
LONG GSP_l_DrawElementIndexedTriangles_Clone
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex				*_pst_Point,
	GEO_tdst_UV				*_pst_UV,
	ULONG 					ulNbPoints,
	ULONG 					*pNormals
);


void GSP_DB_Transfert(u_int Number ,struct SOFT_tdst_AVertex_	*_pst_Point ,struct GEO_tdst_UV_ *_pst_UV);


/* Texture loader */
void GSP_ClearVRAM();
void Gsp_ShowVram(u_int Address , u_int W , u_int ColorMode);

/* Real-time texture loader */
void GSP_SelectAndLoadTexture(ULONG TextureToSelect);
ULONG GSP_GetTextureMemoryAvailable();
void GSP_LoadInterfaceTexture();
void GSP_LoadTexture_SYNCRO_DMA(u_int ulTextureIndex , void *PixelPtr);
void GSP_LoadPalette_SYNCRO_DMA(u_int ulTextureIndex , void *PixelPtr);
void GSP_LoadCachedPalette(u_int PaletteIndex);
void GSP_LoadCachedTexture(u_int TIndex);
void GSP_FLushAllTextureCache();

void GSP_AllocABackCache(struct GEO_tdst_ElementIndexedTriangles_ 	*p_ElementUser);
void GSP_FreeABackCache(struct GSP_GEOBackCache_ *p_BackCache);


/* Interrupts */
int Gsp_vblankHandler(int);
int Gsp_DMA1End(int);
int Gsp_DMA2End(int);
int Gsp_DMA8End(int);
int Gsp_DMA9End(int);


void Gsp_SendToDma1(u_int Address , u_int CHCR);
void Gsp_Send_To_VU1_Ref(u_int pAdress , ULONG ulSize);
void Gsp_Send_To_VU1_Call(u_int pAdress);
void Gsp_SendToDma2(u_int Address , u_int CHCR);

/* Cache function */
void GSP_CacheFlushSegment(u_int Address , u_int SizeInOctets);

/* Debug */
void GSP_DrawTExt(int X,int Y,char *P_String,u_int Color,u_int ColorBack,unsigned long TBack,unsigned long TFront);
void GSP_DrawTExt_X2Y2(int X,int Y,char *P_String,u_int Color,u_int ColorBack,unsigned long TFront,unsigned long TBack,u_int X2,u_int Y2);
void GSP_OutputConsole(char *P_String);
void GSP_ClsConsole();
void GSP_DrawConsole();
u_int Gsp_Stp_GetLenght(char *Str);
void Gsp_Setup();
void GSP_Snapshot(u_int bFullVRAM);

/* After effects */
void Gsp_AE_DepthBlur(float ZStart ,float floatZEnd);
void Gsp_AE_DepthBlurNear(float ZStart ,float ZEnd);
void Gsp_AE_MotionBlur(u_int CurZ);
void Gsp_AE_Blur(u_int Number , u_int Type);
void Gsp_AE_Emboss(u_int DX , u_int DY , u_int Strenght);
void Gsp_AE_MotionSmooth(int DX , int DY);
void Gsp_AE_ShowShadows();
void Gsp_AE_CopyShadows();
void Gsp_AE_ShowZbuffer();
void Gsp_AE_ShowAlphaBuffer();
void Gsp_AE_ZoomSmooth(float Coef,ULONG Mode);
void GSP_Remanance(float Factor,u32 NoZBuff);
void Gsp_AE();
void Gsp_AE_Vertical_AA();
void Gsp_AE_FADE();
void Gsp_AE_FADE_PUSH(float TimeInSeconds);
void Gsp_AE_FlushAllSpecial();

_inline_ void Gsp_SetPlanarProjectionMode(ULONG ProjMode)
{
/* 0: Nothing , 1: OX , 2: OY , 3:OZ , 4:WP */
	GspGlobal_ACCESS(bPlanarGizmoEnable) = ProjMode;
}
void Gsp_SetUVMatrix_Identity();
void Gsp_SetUVMatrix(struct MAT_tdst_Decompressed_UVMatrix_ *TDST_ummAT);
void Gsp_SetUVMatrixAndMul(struct MAT_tdst_Decompressed_UVMatrix_ *TDST_ummAT);

void GSP_DrawSpriteX(SOFT_tdst_AVertex	*_pst_Point , u_int Color);
void GSP_DrawSpriteEnd();
void GSP_DrawSprites();
void GSP_DrawSpriteBegin();
void Gsp_SetPlanarProjectionUVMatrix(MATH_tdst_Vector *VU,MATH_tdst_Vector *VV,MATH_tdst_Vector *VT);
void Gsp_SetPlanarProjectionUVMatrix_For_Shadows(MATH_tdst_Vector *VU,MATH_tdst_Vector *VV,MATH_tdst_Vector *VT,MATH_tdst_Vector *VAxis,float fDist);


void GSP_SetCullingMask(UCHAR c_CullingMask);


void GSP_FF_Init();
void GSP_FF_GO();
#ifdef GSP_PS2_BENCH
void GSP_SS_TestScreenSaver();
#endif

_inline_ void GSP_SpecialStoreDma(register u_int ID , register u_int QWC , register u_int Next , register GSP_DMA_Source_Chain_TAG *Adress)
{
#if 0
/*	if (!((u32)Adress & 0xf0000000))
		*(u32 *)0 = *(u32 *)4;
	if (((u32)Adress & 0x0000000f))
		*(u32 *)0 = *(u32 *)4;
	if ((u32)Adress < ((u32)gs_st_Globals.GSP_BX_VAR.pCurrentCpuBuffer->p_BufferBase))
		*(u32 *)0 = *(u32 *)4;
	if ((u32)Adress > (((u32)gs_st_Globals.GSP_BX_VAR.pCurrentCpuBuffer->p_BufferBase) + (RAM_BUF_SIZE)))
		*(u32 *)0 = *(u32 *)4;*/
	Adress->id = ID;
	Adress->qwc = QWC;
	Adress->mark = 0;
	Adress->next = Next;
//	*(u_long64 *)Adress->p = 0L;
#else
	register u_long128 localdd;
	asm __volatile__ ("
		sll		localdd,ID,8
		pextlh	localdd,localdd,QWC
		pextlw	localdd,Next,localdd
		sd		localdd,0(Adress)
		");
	
#endif	
}

u_int GSP_GetCacheINFO(u_int ulType);
void Gsp_AE_MASTER(ULONG AENum , ULONG OnOff , ULONG P1 , float Pf1);
float Gsp_AE_MASTER_GET_P1(ULONG AENum , ULONG P1 );
ULONG Gsp_AE_MASTER_GET_ONOFF(ULONG AENum );

/* Prefetch function */
void GSP_Prefetch(u_int P);

#ifdef __cplusplus
}
#endif

#endif// GSP_H
