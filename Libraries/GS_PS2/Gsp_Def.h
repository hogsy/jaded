#ifndef GSP_DEF_H
#define GSP_DEF_H

#include "MATHs/MATH.h"

#define GIGSCALE0 200.0f

#define GSP_GEO_ACTIVE_CACHE
#define GSP_GEO_ACTIVE_COLOR_CACHE
#define USE_GO_DATA
#define	GSP_USE_TEXTURE_CACHE


#define GSP_ASYNC_FLIP_ENABLE

#define GSP_USE_TRIPLE_BUFFERRING

//#define USE_DRAW_BUFFER_FROM_RAM
// ELSE!
//#define USE_SAVE_XYZ_BUFFER
extern u32 volatile InterruptLocker;

_inline_ void Interrupt_Entry()
{
/*	if (InterruptLocker)
		*(u32 *)0 = *(u32 *)4; // Direct Crash
	InterruptLocker = 1;*/
}

_inline_ void Interrupt_Exit()
{
//	InterruptLocker = 0;
}

typedef struct	GSP_tdst_DualShock2_
{
	/* offset */
	unsigned char Status : 8;			/* 0 */
	unsigned char LowerUnit : 4;		/* 1-0 */
	unsigned char UpperUnit : 4;		/* 1-4 */
	unsigned char SEL : 1;			/* 2-0 */
	unsigned char L3 : 1;				/* 2-1 */
	unsigned char R3 : 1;				/* 2-2 */
	unsigned char START : 1;			/* 2-3 */
	unsigned char UP : 1;				/* 2-4 */
	unsigned char RIGHT : 1;			/* 2-5 */
	unsigned char DOWN : 1;			/* 2-6 */
	unsigned char LEFT : 1;			/* 2-7 */
	unsigned char L2 : 1;				/* 3-0 */
	unsigned char R2 : 1;				/* 3-1 */
	unsigned char L1 : 1;				/* 3-2 */
	unsigned char R1 : 1;				/* 3-3 */
	unsigned char TRIANGLE : 1;		/* 3-4 */
	unsigned char CIRCLE : 1;			/* 3-5 */
	unsigned char CROSS : 1;			/* 3-6 */
	unsigned char SQUARE : 1;			/* 3-7 */
	unsigned char AnaStickRightX : 8; /* 4 */
	unsigned char AnaStickRightY : 8; /* 5 */
	unsigned char AnaStickLeftX : 8;	/* 6 */
	unsigned char AnaStickLeftY : 8;	/* 7 */
	unsigned char InfoR : 8;			/* 8 only DualShock2 */
	unsigned char InfoL : 8;			/* 9 */
	unsigned char InfoU : 8;			/* 10 */
	unsigned char InfoD : 8;			/* 11 */
	unsigned char InfoT : 8;			/* 12 */
	unsigned char InfoC : 8;			/* 13 */
	unsigned char InfoX : 8;			/* 14 */
	unsigned char InfoS : 8;			/* 15 */
	unsigned char InfoL1 : 8;			/* 16 */
	unsigned char InfoR1 : 8;			/* 17 */
	unsigned char InfoL2 : 8;			/* 18 */
	unsigned char InfoR2 : 8;			/* 19 */
} GSP_tdst_DualShock2;



typedef struct {
	tGS_PMODE		pmode;
	tGS_SMODE2		smode2;
	tGS_DISPFB2		dispfb;
	tGS_DISPLAY2	display;
	tGS_BGCOLOR		bgcolor;
	tGS_DISPFB1     dispfb1;
	tGS_DISPLAY1    display1;
	tGS_DISPLAY1    pad;
} sceGsDispEnv2Circuits;



typedef struct {
	u_long ulValueToWrite;
	u_long ulRegister;
} MyRegister;

typedef struct {
	unsigned long Command;
	unsigned long Regs;
} GspGifTag __attribute__((aligned(16)));

typedef struct {
	u_int VifTag[4];
} GspVifTag __attribute__((aligned(16)));

#define Gsp_M_SetGifTag(GT, NLOOP , EOP , PRE , PRIM , FLG , NREGS , REGS)\
		(GT)->Command = (unsigned long)NLOOP | ((unsigned long)EOP << 15L) | ((unsigned long)EOP << 46L) | ((unsigned long)PRIM << 47L)| ((unsigned long)FLG << 58L)| ((unsigned long)NREGS << 60L);\
		(GT)->Regs = (unsigned long)REGS;
	
typedef struct {
	GspGifTag giftag;
	u_long128 pValues[64];
	u_int NumRegs;
}	Gsp_Primitiv __attribute__((aligned(64)));

typedef struct {
	GspGifTag giftag;
	MyRegister tCommand[32];
	u_int NumRegs;
}	MyRegWriter __attribute__((aligned(64)));

typedef struct 
{
	float x,y,z,w;
} MyVectorFormat __attribute__((aligned (16)));

typedef struct 
{
	int x,y,z,w;
} MyIVectorFormat __attribute__((aligned (16)));



typedef struct 
{
	MyVectorFormat	I,J,K;
	MyVectorFormat	T;
	MyVectorFormat  AxAyxxZM;
	MyVectorFormat  CxCyCzFC;
	MyVectorFormat  Additionnal_Information1;
	MyVectorFormat  Additionnal_Information2;
} MyMatrixFormat __attribute__((aligned (16)));

typedef struct 
{
	MyVectorFormat	ColorMul;
	MyVectorFormat	ColorAdd;
} MyColorMulAdd __attribute__((aligned (16)));

typedef struct 
{
	MyVectorFormat	ab,cd,T;
} MyMatrixUVFormat __attribute__((aligned (16)));


typedef union 
{
  u_long      ul64;
  u_int       ui32[2];
} Gsp_DmaTag __attribute__((aligned (16)));//*/


#ifdef USE_DRAW_BUFFER_FROM_RAM

typedef struct {
	u_int SPR_Adress;
	u_int SPR_Number;
} 
Gsp_tdst_DMA8_Stack_Node ;

typedef struct 
{
	Gsp_tdst_DMA8_Stack_Node Sn[8];
	Gsp_tdst_DMA8_Stack_Node *p_CurrentCommand;
	u_long128 				 *p_CURRENT_RAMPTR;
} 
Gsp_tdst_DMA8_Stack __attribute__((aligned(4)));

typedef struct {
	Gsp_tdst_DMA8_Stack Stacks[2];
	Gsp_tdst_DMA8_Stack *p_Current_CPU;
	Gsp_tdst_DMA8_Stack *p_Current_DMA;
	u_long128 			*p_RAMPTR_BASE;
} 
Gsp_tdst_DMA8_BiStack __attribute__((aligned(4)));

extern Gsp_tdst_DMA8_BiStack gst_CurrentStack;
extern __declspec(scratchpad) u_int volatile gsulSyncroPath;

#define GSP_DMA8_TRANSFERT(ADDRESS_SRC,LENGHT)\
{\
	gsulSyncroPath |= FLUSH_DMA8;\
	*D_CTRL &= ~(2|(7 << 8)); /* Cycling on 8 qword's */\
	*D_CTRL |= 2; /* Cycle Stealing on */\
	*D8_QWC  = LENGHT;\
	*D8_SADR = ADDRESS_SRC/* & 0x3fff is done automaticaly */;\
	*D8_MADR = (u_int)gst_CurrentStack.p_Current_DMA->p_CURRENT_RAMPTR;\
	gst_CurrentStack.p_Current_DMA->p_CURRENT_RAMPTR += LENGHT;\
	gst_CurrentStack.p_Current_DMA->p_CurrentCommand++;\
	*D8_CHCR = (1<<8);\
}

#define GSP_SOFT8_TRANSFERT(ADDRESS_SRC,LENGHT)\
{\
	u_int LL;\
	u_long128 *p_Src,*p_Dst;\
	LL = LENGHT;\
	p_Src = (u_long128 *)ADDRESS_SRC;\
	p_Dst = gst_CurrentStack.p_Current_DMA->p_CURRENT_RAMPTR;\
	gst_CurrentStack.p_Current_DMA->p_CURRENT_RAMPTR+=LENGHT;\
	gst_CurrentStack.p_Current_DMA->p_CurrentCommand++;\
	GSP_UnCachePtr(p_Dst);\
	while (LL--)\
	{\
		*(p_Dst++) = *(p_Src++);\
	}\
}
#endif


typedef struct {
	u_long				VIF_Points	__attribute__((aligned (8)));
	u_long				VIF_Colors	__attribute__((aligned (8)));
	u_long				VIF_PointsUV	__attribute__((aligned (8)));
	u_long				VIF_Command1	__attribute__((aligned (8)));
	u_long				VIF_SRS		__attribute__((aligned (8)));
	u_long				VIF_Command2	__attribute__((aligned (8)));
	u_long				VIF_COMPRESSOR	__attribute__((aligned (8)));
	u_long				VIF_COMPRESSOR0	__attribute__((aligned (8)));
	u_long				VIF_PlanarUV	__attribute__((aligned (8)));
	u_long				VIF_Matrix	__attribute__((aligned (8)));
	u_long				VIF_LIGHT	__attribute__((aligned (8)));
	u_long				VIF_CMA	__attribute__((aligned (8)));
	u_long				VIF_FUR	__attribute__((aligned (8)));
	u_long				VIF_MatrixUV	__attribute__((aligned (8)));

	u_int 				p_Command[8] __attribute__((aligned (16)));
	GspGifTag			GIF_SRS		__attribute__((aligned (16)));
	u_int 				Size;
	u_int 				Base;
	struct Gsp_tdst_DrawBuffer_ *p_OtherMDB;
}	Gsp_tdst_DrawBuffer __attribute__((aligned(64)));

#define GSP_InterfaceTexture 				1
#define GSP_IsInVram		 				2
#define GSP_IsNeeded		 				4
#define GSP_IsChildOfInterfaceTexture		8
#define GSP_MustBeFlushed	 				16
typedef struct GSP_PaletteManager_
{
	u_int  *ColorField;
	struct GSP_PaletteManager_ *pNext;	
} GSP_PaletteManager;

typedef struct {
	u_long 	  	TEX0_REG;
	u_long 	  	TEX1_REG;
	u_long 	  	TEXCLUT;
	u_short		W;
	u_short		H;
	u_int		ulBigKey;		
	u_int		FBP;
	u_int 		ZOB;
	u_int 		DBW;
	u_int 		InterfaceFlags;
#ifdef GSP_USE_TEXTURE_CACHE	
	u_int 		ulPaletteIndex;
	u_int 		ulMotherTextureIndex;
#endif
	u_int 		*p_BitmapPtr;
}	Gsp_tdst_Texture;
typedef struct {
	u_long 		ul64_SetTexturePaletteParam;
	u_int 		VramPtr;
	u_int 		W;
	u_int 		H;
	u_int 		BPP;
#ifdef GSP_USE_TEXTURE_CACHE
	u_int 		InterfaceFlags;
	GSP_PaletteManager	*	Clut256;
#endif	
}	Gsp_tdst_Palette;

typedef struct GSP_ReflexionHiddenScreenParams_
{
	int XPos;
	int YPos;
	int XSize;
	int YSize;
	int bEnable;
} GSP_ReflexionHiddenScreenParams;


#define GSP_DB_XYZ 			0x00000010
#define GSP_DB_Colors 		0x00000020
#define GSP_DB_UV 			0x00000040
#define GSP_DB_Matrix 		0x00000080
#define GSP_DB_RenderState 	0x00000100
#define GSP_DB_MatrixUV	 	0x00000200
#define GSP_DB_PlanarUV	 	0x00000400
#define DP_MaxNumT 		16
#define DP_MaxNumSTRP 	48

#define GSP_DB_Base1				0x106
#define GSP_DB_Base2				0x286
#define GSP_DB_MatrixBase			0x0D0
#define GSP_DB_LightBase			0x3FF0
#define GSP_DB_Color_Mul_Add_Base	0x0D8
#define GSP_DB_MatrixUVBase			0x0DA
#define GSP_DB_CommandWBase			0x0F0
#define GSP_DB_CommandWBase2		0x0F2
#define GSP_DB_PlanarMatrixUVBase	0x0f5
#define GSP_DB_FURBASE				0x0FF



#define GSP_Status_FogOn				0x00000001
#define GSP_Status_Setup_On 			0x00000002
#define GSP_Status_Raster_On 			0x00000004
#define GSP_Status_AE_WATEREFFECT		0x00000008
#define GSP_Status_PAL_On 				0x00000010
#define GSP_Status_VU1DontDraw 			0x00000020
#define GSP_Status_TNumCounterOn 		0x00000040
#define GSP_Status_SetRenderStateOn 	0x00000080
#define GSP_Status_AE_CB_On 			0x00000100
#define GSP_Status_AE_DB 				0x00000200
#define GSP_Status_AE_MB 				0x00000400
#define GSP_Status_AE_SM 				0x00000800
#define GSP_Status_Raster_Details_On 	0x00001000
#define GSP_Status_AE_EMB 				0x00002000
#define GSP_Status_AE_SMSM				0x00004000
#define GSP_Status_AE_SHOWSDW			0x00008000
#define GSP_Status_GetZBufferBack		0x00010000
#define GSP_Status_ZBufferBackValid		0x00020000
#define GSP_Status_AE_SHOWZBUF			0x00040000
#define GSP_Status_AE_ZS				0x00080000
#define GSP_Status_AE_REMANANCE			0x00100000
#define GSP_Status_VAA_CRTS				0x00200000
#define GSP_Status_Show_Depth			0x00400000
#define GSP_Status_AE_DBN				0x00800000
#define GSP_Status_AE_BlurTest44		0x01000000
#define GSP_Status_AE_ModeCompare		0x02000000
#define GSP_Status_AE_RS				0x04000000
#define GSP_Status_SVS	 				0x08000000
#define GSP_Status_Raster_Prop_On 		0x10000000
#define GSP_Status_AE_ShowTexture		0x20000000
#define GSP_Status_Console_On 			0x40000000
#define GSP_Status_Strips_On 			0x80000000

#define GSP_MAX_AE 32

typedef struct 
{
	float Depth_Blur_ZStart;
	float Depth_Blur_ZEnd;
	float Depth_Blur_Near;
	float Depth_Blur_Factor;
	float Contrast;
	float Brighness;
	float ZoomSmoothFactor;
	MATH_tdst_Vector	ZSDir	;
	ULONG	ZSDirIsValidated;
	float SpinSmoothFactor;
	float MotionBlurFactor;
	float RemanaceFactor;
	float Glow;
	float BlurFactor;
	float EmbossFactor;
	float BWFactor;
	float CB_Spectre;
	float CB_Intensity;
	float CB_PhotoFilterEffect;
	float CB_Pastel;
	float fDiaphragmStrength;
	u_int AA22BlurMode;
	/* water effect */
	float fWR_ZHeight;
	float fWR_ZDepth;
	float fWR_ZPlane;
	float fWR_TransparencyMax;
	float fWR_SinusEffect;
	float fWR_SinusEffectBalance;
	float fWR_DarkFactor;
	int   ulWR_ZOrder;
	/* glow effect */
	float fG_Glowintensity;
	float fG_GlowMotionBlur;
	u_int ulG_GlowMode;
	
	float 				ZoomSmoothRemananceFactor;
	MATH_tdst_Vector	ZoomSmoothRemananceLightDirection;
	
	
	float BorderBrithness;
	
	
	/* Warp */
	float fWarpFactor;
		
} GSP_AfterEffectParams;


#define BX_MULTIPLE_BUFFERS

#ifdef BX_MULTIPLE_BUFFERS

#define BX_READY_FOR_CPU	0
#define BX_USED_BY_CPU 		1
#define BX_USED_BY_DMA 		2
#define BX_READY_FOR_DMA	3
#define BX_READY_FOR_DMA_VSYNC_BASE	4
//#define BX_READY_FOR_ZGET	16

#define BX_NumberOfBuffers 40
#define RAM_BUF_SIZE 0x12200


#ifdef GSP_GEO_ACTIVE_CACHE
#define GSP_GEO_Cache_Store_XYZW	0x00
#define GSP_GEO_Cache_Store___UV	0x01
#define GSP_GEO_Cache_Use_XYZW		0x02
#define GSP_GEO_Cache_Use___UV		0x04
#endif

#define GSP_SCDma_ID_Refe 	0x00
#define GSP_SCDma_ID_Cnt	0x10
#define GSP_SCDma_ID_Next	0x20
#define GSP_SCDma_ID_Ref	0x30
#define GSP_SCDma_ID_Refs	0x40
#define GSP_SCDma_ID_Call	0x50
#define GSP_SCDma_ID_Ret	0x60
#define GSP_SCDma_ID_End	0x70

typedef struct 
{
	u_short			qwc;		// transfer count
	u_char			mark;		// mark
	u_char			id;		// tag
	u_int			next;		// next tag
	u_int			p[2];		// padding
} GSP_DMA_Source_Chain_TAG __attribute__ ((aligned(16)));

typedef struct GSP_GEOCacheBlock_
{
	u_long128 					*DataBlock;
	struct GSP_GEOCacheBlock_	*p_stNextBlock;
} GSP_GEOCacheBlock;

#define GCF_AllStripXyzAreIn	1
#define GCF_AllStripUVAreIn 	2
#define GCF_AllStripCCAreIn 	4
#define GCF_AllStripNormalsAreIn 	8
typedef struct GSP_GEOBackCache_
{
	struct GEO_tdst_ElementIndexedTriangles_ 	*p_ElementUser;
	u_short										ulSTRIP_Ponderation_XYZW; /* = total number of bytes used by reads */
	u_short										ulSTRIP_Ponderation_UV; /* = total number of bytes used by reads */
	u_int										CacheFLags;
	GSP_GEOCacheBlock							*p_STRIPED_XYZW_Block_Entry; /* (DP_MaxNumT * 3) * 16 blocks size = 2016 bytes */
	GSP_GEOCacheBlock							*p_STRIPED_UV___Block_Entry; /* (DP_MaxNumT * 3) * 8  blocks size = 1008 bytes */
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE	
	u_short										ulSTRIP_Ponderation_CC; /* = total number of bytes used by reads */
	GSP_GEOCacheBlock							*p_STRIPED_CC___Block_Entry; /* (DP_MaxNumT * 3) * 16 blocks size = 2016 bytes */
#endif
} GSP_GEOBackCache;


typedef struct 
{
	u_long128					*p_BufferBase;
	u_int 						volatile BX_Size;
	u_short						volatile BX_State;
	u_short						volatile BX_Option;
	u_int 						volatile BX_BackSize;
	u_int 						volatile BX_BackAddress;
	struct GSP_BX_				*p_NextBuffer;
} GSP_BX;

typedef struct 
{
	GSP_BX 	Buffer[BX_NumberOfBuffers];
	GSP_BX 	volatile *pCurrentDmaBuffer;
	GSP_BX	*pCurrentCpuBuffer;
} GSP_BX_BUFFER;
#endif

typedef struct Gsp_BigStruct_{
	u_int 			FIRSTVALUE;
	sceGsDispEnv2Circuits 	gs_DE1 __attribute__((aligned(16)));
	sceGsDispEnv2Circuits 	gs_DE2 __attribute__((aligned(16)));
	
	Gsp_Primitiv 	PrimX  __attribute__((aligned(16)));
	MyRegWriter		stMRW1 __attribute__((aligned(16)));
	MyRegWriter		stMRW2 __attribute__((aligned(16)));
	MyVectorFormat	stCurrentMatrix[4] __attribute__((aligned(16)));
	MyVectorFormat	stSymetrisedMatrix[4] __attribute__((aligned(16)));
	MyColorMulAdd	stCurrentCMA __attribute__((aligned(16)));
	MyVectorFormat	stCurrentFUR_SizeInX_Undef_Undef_Undef __attribute__((aligned(16)));
	MyRegWriter		*p_CurrentRegWriter;
	
	u_int Xsize;
	u_int Ysize;
	u_int Rendering2D;
	
	
	int HRepos;
	u_int Morfling;
	u_int FBP1;
	u_int FBP2;
	u_int TFBP1;
	u_int TFBP2;
	u_int FBP3;
	u_int ZBP;
	u_int TBP;
	u_int BX0,BY0;
	u_int BppMode; /* 0 : 32 bits 1 : 24 bits 2 : 16 bits */
	u_int ZBppMode; /* 0 : 32 bits 1 : 24 bits 2 : 16 bits */
	u_int VideoMode;
	u_int ModePEnable;
	u_int LastVBlankCounter;
	u_int FrameCounter;
	u_int ulBackColor;
	u_int ulBorderColor;
	float YCorrectionCoef;
	float fCurrentFocale;
	float ZFactor;
	u_int ulColorOr;
	float fCurrentFrquency;
	float fAddY;
	
	u_int FogColor; 
	float FogZNear; 
	float FogZFar; 
	u_int ulCurrentCullingMask;
	/* V-SYNC stability */
	u_int ulCounterOfStability;
	u_int ulVSyncStability;
	void (*Gsp_DrawHook)(struct Gsp_BigStruct	*p_BigStruct, u_int NumberOfPrims);
	void (*Gsp_DrawHook2)(struct Gsp_BigStruct	*p_BigStruct, u_int NumberOfPrims);
	
	GSP_AfterEffectParams *p_AE_Params;
	
	u_long CUR_REG_FRAME 	 __attribute__((aligned(8)));
	u_long CUR_REG_ZBUF	 __attribute__((aligned(8)));


	/* SPR BUFFERS */
	u_int 					p_CurrentBufferPointer;
	u_int 					p_BufferPointerBase;
	u_int	volatile		ulFloatingFlipFlag;
#ifdef BX_MULTIPLE_BUFFERS
	GSP_BX_BUFFER 	 	GSP_BX_VAR;
#endif
	Gsp_tdst_DrawBuffer 	*MDB;
	u_int 					ulNextVU1Jump;
	Gsp_tdst_DrawBuffer 	MDB_TABLE[2];
	u_int 		    		bLastWasSRS;
	
	/* Cache manager */
#ifdef GSP_GEO_ACTIVE_CACHE
	GSP_DMA_Source_Chain_TAG 	*BX_LastCntPtr;

	u_int 				CacheFlags;
	GSP_GEOCacheBlock 	*gs_st_First_Free_XYZW;
	GSP_GEOCacheBlock 	*gs_st_First_Free___UV;
	GSP_GEOCacheBlock 	*gs_st_First_Free_XYZW_SPECIAL_VB;
	GSP_GEOCacheBlock 	*gs_st_First_Free_XYZW_SPECIAL_VB_PUSHED;
	GSP_GEOCacheBlock 	*gs_st_First_Free_XYZW_SPECIAL_VB_PUSHED_1;
	GSP_GEOCacheBlock 	*gs_st_First_Free_XYZW_SPECIAL_VB_PUSHED_2;
	GSP_GEOCacheBlock 	*Current_XYZW;
	GSP_GEOCacheBlock 	*Current___UV;
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
	GSP_GEOCacheBlock 	*gs_st_First_Free___CC;
	GSP_GEOCacheBlock 	*gs_st_First_Free___CC_SPECIAL_VB;
	GSP_GEOCacheBlock 	*gs_st_First_Free___CC_SPECIAL_VB_PUSHED;
	GSP_GEOCacheBlock 	*gs_st_First_Free___CC_SPECIAL_VB_PUSHED_1;
	GSP_GEOCacheBlock 	*gs_st_First_Free___CC_SPECIAL_VB_PUSHED_2;
	GSP_GEOCacheBlock 	*Current___CC;
#endif	
#endif	
	MyVectorFormat 		*p_Last_XYZW_pointer;
	u_long		 		*p_Last_UV_pointer;
	u_int				*p_Last_COLOR_pointer;
	u_int				*p_Last_FUR_pointer;
	u_int 				bGSUpdateFlag;

	
	/* TEXTURES */
	u_long				ul64LastText0Used;
	u_int 				*p_MyTextureBuffer;
	Gsp_tdst_Texture 	*p_MyTextureDescriptorsAligned;
	Gsp_tdst_Palette	*p_MyPalletteDescriptorAligned;
	u_int 				ulNumberOfTextures;
	u_int				ulNumberOfPalettes;
	u_int				Current_UV_Compute_VU0_Jump;
	u_int 				VU0_JumpTatble[5];
	u_int				Mode2X;
	u_int				*pCompresseddNormals;
#ifdef GSP_USE_TEXTURE_CACHE
	u_int				ulAvailableTextureCacheSize;
#endif
	
	
	/* Fade  */
	u_int 				ulFade_Counter;
	u_int				ulFade_Video_Enable;
	u_int 				ulFade_Time;
	float				ulFade_Alpha;
	float				ulFade_Inc;
	
	
	/* Hidden screen */
	GSP_ReflexionHiddenScreenParams stHSP;
	

	/* Set renderstate */
	u_int 				ulLastBMD;
	u_int 				ulLastTXT;
	u_int 				bSRS_FromScratch;
	u_int 				Last_rPRMODE;
	u_long				last_GSP_TEST_1 __attribute__((aligned(8)));
	u_int				RenderingInterface;
	u_int 				ulNewBMD;
	u_int 				ulNewTXT;
	u_int 				ulLastDrawMask;
	u_int 				bSRSMustBeUodate;
	u_int 				bPlanarGizmoEnable;
	MyVectorFormat		PlanarMatrix[2] __attribute__((aligned(16)));
	MyMatrixUVFormat	stLastUVMatrix __attribute__((aligned(16)));
	MyVectorFormat  	stCurrentAxAyxxZM __attribute__((aligned(16)));
	MyVectorFormat  	stCurrentCxCyCzFC __attribute__((aligned(16)));
	MyVectorFormat  	Additionnal_Information1 __attribute__((aligned(16)));
	MyVectorFormat  	Additionnal_Information2 __attribute__((aligned(16)));
	
	float 				fFogCorrector;
	float				fFogCorrectorFar;
	float 				f2DFFX;
	float				f2DFFY;
	float 				f2DFFX_B2D;
	float				f2DFFX_A2D;
	float 				f2DFFY_B2D;
	float				f2DFFY_A2D;

	
	/* GSDrawP*/
	u_int 								Status;
	u_int								ulOGLSetCol_XOr;
	u_int 								ColorParam;
	struct GEO_tdst_IndexedTriangle_	*t ,*tUV;
	struct GEO_tdst_IndexedTriangle_	*TColor;
	
	
	
	/* Screen saver */
	void volatile			(* VBlankHook)();
	


	/* DBUG */
	u_int ulRotativBufferTurns;

	u_int Locker;
	u_int Tnum;
	u_int NCalls;
	u_int NCallsSRS;
	u_int DMA_VS_CPU;
	u_int CPU_VS_DMA;
	u_int NSPRITES;

}	Gsp_BigStruct __attribute__((aligned(64)));


#define GSP_DMAFormatAddress(a) (((a) & 0xf0000000) == 0x70000000) ? (((a) ^ 0xf0000000)) : ((a) & 0x8fffffff)
/* Default DMA CHCR */
#define DX_CHCR (1/*From memory */ | /* 1 address pushed */(1<<4)| (1<<2)/* Chain*/| /* Transfer DMATAG */(1<<6) | /*Start */ (1<<8))
#define DX_CHCR_NODTG (1/*From memory */ | /* 1 address pushed */(0<<4)| (1<<2)/* Chain*/| /* Transfer DMATAG */(0<<6) | /*Start */ (1<<8))


#define GST_ADD     	(0x80L << 32L) | (1L << 6L) | (2L << 4L) | (2L << 2L) | (0L << 0L)

#define GST_SUB     	(0x40L << 32L) | (1L << 6L) | (2L << 4L) | (0L << 2L) | (2L << 0L)
#define GST_SUBMUL     	(0x30L << 32L) | (1L << 6L) | (2L << 4L) | (0L << 2L) | (2L << 0L)
#define GST_SUB2x     	(0x80L << 32L) | (1L << 6L) | (2L << 4L) | (0L << 2L) | (2L << 0L)

#define GST_SHRPN     	(0x80L << 32L) | (1L << 6L) | (2L << 4L) | (0L << 2L) | (1L << 0L)

#define GST_COPY    				(0x00L << 32L) | (0L << 6L) | (2L << 4L) | (2L << 2L) | (1L << 0L)
#define GST_COPY_By_AlphaSRC    	(0x00L << 32L) | (2L << 6L) | (0L << 4L) | (2L << 2L) | (0L << 0L)

#define GST_ALPHA   	(0x00L << 32L) | (0L << 6L) | (0L << 4L) | (0L << 2L) | (1L << 0L)
#define GST_ALPHA_I   	(0x00L << 32L) | (1L << 6L) | (0L << 4L) | (1L << 2L) | (0L << 0L)

#define GST_PALPHA   	(0x00L << 32L) | (0L << 6L) | (1L << 4L) | (0L << 2L) | (1L << 0L)
#define GST_PALPHA_I   	(0x00L << 32L) | (1L << 6L) | (1L << 4L) | (1L << 2L) | (0L << 0L)

#define GST_ALPHAP  	(0x00L << 32L) | (0L << 6L) | (0L << 4L) | (2L << 2L) | (1L << 0L)
#define GST_PALPHAP 	(0x00L << 32L) | (1L << 6L) | (1L << 4L) | (2L << 2L) | (0L << 0L)
#define GST_ALPHAFIX 	(0x00L << 32L) | (1L << 6L) | (2L << 4L) | (1L << 2L) | (0L << 0L)\

#define GST_SHADOWMODE 	(0x00L << 32L) | (1L << 6L) | (0L << 4L) | (1L << 2L) | (2L << 0L)

#define GSP_PRIM			0x00
#define GSP_RGBAQ			0x01
#define GSP_ST				0x02
#define GSP_UV				0x03
#define GSP_XYZF2			0x04
#define GSP_XYZ2			0x05
#define GSP_FOG				0x0a
#define GSP_XYZF3			0x0c
#define GSP_XYZ3			0x0d
#define GSP_TEX0_1			0x06
#define GSP_TEX0_2			0x07
#define GSP_CLAMP_1			0x08
#define GSP_CLAMP_2			0x09
#define GSP_TEX1_1			0x14
#define GSP_TEX1_2			0x15
#define GSP_TEX2_1			0x16
#define GSP_TEX2_2			0x17
#define GSP_XYOFFSET_1			0x18	
#define GSP_XYOFFSET_2			0x19	
#define GSP_MIPTBP1_1			0x34	
#define GSP_MIPTBP1_2			0x35	
#define GSP_MIPTBP2_1			0x36	
#define GSP_MIPTBP2_2			0x37	
#define GSP_SCISSOR_1			0x40	
#define GSP_SCISSOR_2			0x41	
#define GSP_TEST_1			0x47
#define GSP_TEST_2			0x48
#define GSP_ALPHA_1			0x42
#define GSP_ALPHA_2			0x43
#define GSP_FBA_1			0x4a
#define GSP_FBA_2			0x4b
#define GSP_FRAME_1			0x4c
#define GSP_FRAME_2			0x4d
#define GSP_ZBUF_1			0x4e
#define GSP_ZBUF_2			0x4f
#define GSP_PRMODECONT			0x1a	
#define GSP_PRMODE			0x1b
#define GSP_TEXCLUT			0x1c
#define GSP_SCANMSK			0x22
#define GSP_TEXA			0x3b
#define GSP_FOGCOL			0x3d
#define GSP_TEXFLUSH			0x3f	
#define GSP_DIMX			0x44
#define GSP_DTHE			0x45
#define GSP_COLCLAMP			0x46	
#define GSP_PABE			0x49
#define GSP_BITBLTBUF			0x50	
#define GSP_TRXPOS			0x51
#define GSP_TRXREG			0x52
#define GSP_TRXDIR			0x53
#define GSP_HWREG			0x54
#define GSP_SIGNAL			0x60
#define GSP_FINISH			0x61
#define GSP_LABEL			0x62

#define GSP_SetRegister(Register , Value) \
	GspGlobal_ACCESS(p_CurrentRegWriter)->tCommand[GspGlobal_ACCESS(p_CurrentRegWriter)->NumRegs].ulRegister = (unsigned long)Register;\
	GspGlobal_ACCESS(p_CurrentRegWriter)->tCommand[GspGlobal_ACCESS(p_CurrentRegWriter)->NumRegs++].ulValueToWrite = (unsigned long)Value;

#define GSP_GIF_FLG_PACKED  		0L
#define GSP_GIF_FLG_REGLIST  		1L
#define GSP_GIF_FLG_IMAGE  		2L

#define GSP_PMODE			0x00
#define GSP_SMODE2			0x02
#define GSP_DISPFB1			0x07
#define GSP_DISPLAY1		0x08	
#define GSP_DISPFB2			0x09
#define GSP_DISPLAY2		0x0a	
#define GSP_EXTBUF			0x0b
#define GSP_EXTDATA			0x0c
#define GSP_EXTWRITE		0x0d	
#define GSP_BGCOLOR			0x0e
#define GSP_CSR				0x40
#define GSP_IMR				0x41
#define GSP_BUSDIR			0x44
#define GSP_SIGLBLID		0x48

#define GSP_720x2__			720
#define GSP_640x2__			640
#define GSP_512x2__			512
#define GSP_384x2__			384
#define GSP_320x2__			320
#define GSP_256x2__			256

#define GSP_VRES_x1					0
#define GSP_VRES_xAUTO				1
#define GSP_VRES_x2					2
#define GSP_VRES_x2_TripleAA		3

#define GSP_IsFrameODD() (GspGlobal_ACCESS(FrameCounter) & 1)
#define GSP_IsFrameODD2() (p_Globals->FrameCounter & 1)
#endif //GSP_DEF_H
