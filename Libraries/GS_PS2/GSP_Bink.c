#include <eeregs.h>
#include <eestruct.h>
#include <libgraph.h>
#include <libdma.h>
#include <libvu0.h>
#include <sifdev.h>
#include <libpc.h>
#include <devvif1.h>
#include <libvifpk.h>
#include <libipu.h>
#define GSP_BINK_ACTIVATE_VIDEO_LOADING

#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMpro.h"

#include "Gsp.h"
#include "GSPinit.h"
#include "GSPtex.h"
#include "GDInterface/GDInterface.h"

#include "ENGine/Sources/WORld/WORstruct.h"
#include "MATerial/MATstruct.h"
#include "GSPdebugfct.h"
#include "Gsp_Bench.h"
#include "BASe/BENch/Bench.h"
#include "GSP_Video.h"
#include "GSPtex.h"
#include "SDK/Sources/IOP/RPC_Manager.h"
#include "SDK/Sources/IOP/iop/iopCLI.h"
#include "SDK/Sources/IOP/iop/iopCLI.h"
#include "SDK/Sources/INOut/INO.h"
#include "Bink.h"
#include "rad_iop.h"
#include "radbase.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDmacros.h"
#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDinterface.h"
#include "SouND/Sources/SNDloadingsound.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDdebug.h"
#include "SouND/Sources/SNDstream.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDmodifier.h"
#include "SouND/Sources/SNDfx.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/ps2/Ps2SND.h"


void *BINK_Mem_p_Alloc(u32 Size);
void BINK_Mem_Free(u32 *Pointer);
int BinkZob( );
void BINK_FLIP();
void GSP_Bink_BeginLoad(u32 WOLBK);
void GSP_Bink_EndLoad();
extern int ps2SND_i_ChangePriority(int);


extern void BINK_SUBOpen(char *TXTFile);
extern void BINK_SUBClose();
extern void BINK_DrawSUB(HBINK bink,u32 *Buffer,u32 XSize,u32 YSize);
extern void BINK_DrawString_Demo(char *TEXT,u32 *Buffer,u32 XSize,u32 YSize);


#undef __FILE__
#define __FILE__ "BNK"
extern u_int volatile TRIPLE_BIGSWAP;
static sceDmaTag dma_chain[ 64 ];
#define UC_ADDR(_x)             ((void *)(((u32)(_x)) | 0x20000000))
#define OSRoundUp128B( _x ) ( ( (U32) ( _x ) + 128 - 1 ) & ~ ( 128 - 1 ) )

#define SCREEN_WIDTH    512
#define SCREEN_HEIGHT   GspGlobal_ACCESS(Ysize)


extern u_int volatile VBlankCounter;
extern u32 LocalVblankCounter ;

extern u_int volatile GSPDMASEMAPHORE;
extern u_int volatile TRIPLE_BIGSWAP;
extern void Gsp_ImpulseBX(Gsp_BigStruct	*p_BIG);
extern void Gsp_FlipFB_ASYNC();


S32 RAD_IOPHardwareVolumes_OVER( U32 which_core, S32 master_core_volume_L, S32 master_core_volume_R, S32 channel_volume_L, S32 channel_volume_R )
{
	master_core_volume_L += master_core_volume_L >> 1;
	master_core_volume_R += master_core_volume_R >> 1;
	if (master_core_volume_L > 0x3fff) master_core_volume_L = 0x3fff;
	if (master_core_volume_R > 0x3fff) master_core_volume_R = 0x3fff;
	return RAD_IOPHardwareVolumes( which_core, master_core_volume_L, master_core_volume_R, channel_volume_L, channel_volume_R );
}

void BINK_FLIP()
{
	Gsp_FlipFB_ASYNC();
	Gsp_FIFO_STOP();
}

static inline void setGsAD( void *dest, u_int addr, u_long data )
{
	u_long * packet = (u_long*) dest;

	*packet++ = data;
	*packet++ = (u_long) addr;
}
void *SPG2_SpaceBaseSave = NULL;
void *SPG2_SpaceBase = NULL;
u32 SPG2_SpaceSize = 0;
u32 SPG2_Allocate(u32 *size);
void SPG2_Free();

void *GSPDB_SpaceBaseSave = NULL;
void *GSPDB_SpaceBase = NULL;
u32 GSPDB_SpaceSize = 0;
u32 GSPDB_Allocate(u32 *size);
void GSPDB_Free();

u32 ALLOCATEDSIZE = 0;
void *BINK_Mem_p_Alloc(u32 Size)
{
	void *ret;
	if (SPG2_SpaceSize < GSPDB_SpaceSize)
	{
		if (Size < SPG2_SpaceSize)
		{
			ret = SPG2_SpaceBase;
			*(u32 *)&SPG2_SpaceBase += Size;
			SPG2_SpaceSize -= Size;
			L_memset(ret , 0  ,Size);
			return ret;
		}
		if (Size < GSPDB_SpaceSize)
		{
			ret = GSPDB_SpaceBase;
			*(u32 *)&GSPDB_SpaceBase += Size;
			GSPDB_SpaceSize -= Size;
			L_memset(ret , 0  ,Size);
			return ret;
		}
	}
	else
	{
		if (Size < GSPDB_SpaceSize)
		{
			ret = GSPDB_SpaceBase;
			*(u32 *)&GSPDB_SpaceBase += Size;
			GSPDB_SpaceSize -= Size;
			L_memset(ret , 0  ,Size);
			return ret;
		}
		if (Size < SPG2_SpaceSize)
		{
			ret = SPG2_SpaceBase;
			*(u32 *)&SPG2_SpaceBase += Size;
			SPG2_SpaceSize -= Size;
			L_memset(ret , 0  ,Size);
			return ret;
		}
	}
	ALLOCATEDSIZE += Size;
	ret = MEM_p_Alloc(Size);
	L_memset(ret , 0  ,Size);
	return ret;
}
void BINK_Mem_Free(u32 *Pointer)
{
	if ((Pointer >= GSPDB_SpaceBaseSave) && (Pointer <= GSPDB_SpaceBase))
		return; // SPG2 allocation
	if ((Pointer >= SPG2_SpaceBaseSave) && (Pointer <= SPG2_SpaceBase))
		return; // SPG2 allocation
	MEM_Free(Pointer);
}

static void setup_backbuffer_upload( sceDmaTag ** dma_tag, int dest_GS_addr, U32 * rgb32_buffer, U32 buffer_width, U32 buffer_height , u32 Up)
{
// GS struct and variables to upload rgb32_buffer.
	typedef struct
	{
	sceGifTag   giftag0;
	sceGsBitbltbuf  bitbltbuf;
	long        bitbltbufaddr;
	sceGsTrxpos trxpos;
	long        trxposaddr;
	sceGsTrxreg trxreg;
	long        trxregaddr;
	sceGsTrxdir trxdir;
	long        trxdiraddr;
	} BITBLTBUFDATA;
 
	int x, y;
	int size;
	BITBLTBUFDATA * blt_data;
	u_long128 * image;

	if ( buffer_height > SCREEN_HEIGHT )
		buffer_height = SCREEN_HEIGHT;

	// Try to center 
	x = ( SCREEN_WIDTH - buffer_width ) / 2;
	y = ( ((int)SCREEN_HEIGHT) - (int)(buffer_height <<1)) / 2;

	
	x = 0;
	if (SCREEN_HEIGHT > buffer_height <<1)
		y = 0;
	

	if (Up) y += buffer_height;
	
	if (y < 0)
	{
		rgb32_buffer -= y * buffer_width;
		buffer_height += y;
		y = 0;
	}
	
	if (y + buffer_height > SCREEN_HEIGHT)
	{
		buffer_height = SCREEN_HEIGHT - y;
	}


	// Initialize our blt_data structure
	blt_data = (BITBLTBUFDATA*) sceDmaAddCont( dma_tag, sizeof(BITBLTBUFDATA) / sizeof(u_long128) );
	SCE_GIF_CLEAR_TAG( &blt_data->giftag0 );
	blt_data->giftag0.NLOOP = 4;
	blt_data->giftag0.NREG = 1;
	blt_data->giftag0.REGS0 = SCE_GIF_PACKED_AD;

	setGsAD( &blt_data->bitbltbuf, SCE_GS_BITBLTBUF, 
	SCE_GS_SET_BITBLTBUF( 0, 0, 0 , dest_GS_addr, buffer_width / 64, SCE_GS_PSMCT32) );
	setGsAD( &blt_data->trxpos, SCE_GS_TRXPOS, SCE_GS_SET_TRXPOS( 0, 0, x, y, 0 ) );
	setGsAD( &blt_data->trxreg, SCE_GS_TRXREG, SCE_GS_SET_TRXREG( buffer_width, buffer_height ) );
	setGsAD( &blt_data->trxdir, SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR( 0 ) );


	// Break the image upload into chunks so we don't break the dma limit
	image = (u_long128*) rgb32_buffer;
	size = (( buffer_width * buffer_height ) * 4) >> 4;
	while ( size )
	{
		sceGifTag * giftag;

		int len = ( size <= 0x7fff ) ? size : 0x7fff;

		giftag = (sceGifTag*) sceDmaAddCont( dma_tag, 1 );
		SCE_GIF_CLEAR_TAG( giftag );
		giftag->NLOOP = len;
		giftag->FLG = SCE_GIF_IMAGE;
		giftag->EOP = ( len == size );

		sceDmaAddRef( dma_tag, len, image );

		image += len;
		size -= len;
	}
}
void INO_Update(void);
void ps2SND_RecallFirstStream(void);

u32 BinkVideoIsSkippable = 1;
u32 BinkVideoMode = 0;
u32 BinkVideoWith = 0;
u32 BinkVideoHeight = 0;
int BinkZobHOOK(char *BinkName )
{
	void * iop_addr;
	u32 SoftBuffer = 0;
	U32 * rgb32_buffer = 0;
	U32 * rgb32_buffer_BASE = 0;
	u32 LoopOK;
	HBINK bink;
	static u32 bfirst = 1;
    float   fVolume;
    s32     s32Volume;
	
	extern u32 ps2INO_IsJOYTouched(u32 NoMushrooms);
		extern u32 GSP_BIGFADETOBLACK;
	extern void SND_MuteAll(u32);
	extern void SND_TrackPauseAll(u32);
	extern void SND_Update(void *);
	extern void ps2SND_FlushModif();
	extern char *GET_BINK_NAME(u32 Context);
	extern unsigned short Demo_gus_playmode;
	

	fVolume = (float)0x7FFF ;
	fVolume = fVolume * SND_f_GroupVolumeGet(SND_e_UserGrpMaster);
	fVolume = fVolume * SND_f_GroupVolumeGet(SND_e_MasterGrp);
	s32Volume = 0x7FFF & (s32)fVolume ;

	
	Gsp_Flush(FLUSH_ALL);
	
//	GSP_ClearVRAM();
	
	SND_MuteAll(TRUE);
	SND_TrackPauseAll(TRUE);
	SND_Update(NULL);

	ps2SND_i_ChangePriority(-1);
	ps2SND_FlushModif();
	eeRPC_FlushCommandAfterEngine();
	
	BINK_SUBOpen(BinkName);
	
//	Demo_gus_playmode = 1;
	
LoopAttract:

	SPG2_SpaceBaseSave = SPG2_SpaceBase = (void *)SPG2_Allocate(&SPG2_SpaceSize);
	GSPDB_SpaceBaseSave = GSPDB_SpaceBase = (void *)GSPDB_Allocate(&GSPDB_SpaceSize);

//	if (BinkVideoMode != 0) // BUG PRESOUM !!!
	{
	    s32 ss;
#if 1
		BinkSetMemory(BINK_Mem_p_Alloc,BINK_Mem_Free);
		
		// allocate some IOP memory for Bink
		iop_addr = sceSifAllocIopHeap( RAD_IOPMemoryAmount( RAD_IOP_NEED_CORE1 | RAD_IOP_NO_INIT_LIBSD	 ) );//*/
		//printf("sceSifAllocIopHeap %x\n", iop_addr );

		// start up the RAD reading and sound IOP module
		if ( RAD_IOPStartUp( 1, -1, iop_addr, RAD_IOP_NEED_CORE1 | RAD_IOP_NO_INIT_LIBSD	 ) )
		{
		    
			// use core1's pcm channel
			ss = BinkSoundUseRAD_IOP( 1 );
			//printf("BinkSoundUseRAD_IOP %d\n", ss);
			
			ss = RAD_IOPHardwareVolumes_OVER( 1, ps2SND_sx_MasterSnd.s_LeftVol, ps2SND_sx_MasterSnd.s_RightVol, s32Volume , s32Volume);
			//printf("RAD_IOPHardwareVolumes_OVER %d\n", ss);

    		BinkSetIOSize((u32)(1024.0f * 1024.0f * 1.5f)); // 2 megs
    		if (BinkName)
    			bink = BinkOpen( BinkName, BINKSNDTRACK | BINKIOSIZE | BINKNOSKIP);
    		else
				bink = BinkOpen( GET_BINK_NAME(BinkVideoMode), BINKSNDTRACK | BINKIOSIZE | BINKNOSKIP);
    		

			if ( bink )
			{
			#if 1
				// allocate some memory to hold the rgb pixels
				rgb32_buffer_BASE = (U32*) BINK_Mem_p_Alloc(OSRoundUp128B( bink->Width * bink->Height * 2 ) + bink->Width * 32);
				rgb32_buffer = (u32 *)(((u32)rgb32_buffer_BASE + 15) & ~15);
				
				LoopOK = 1;

				// Play and show the movie.
				while ( LoopOK)
				{
					int bink_wait;
					
//					if (BinkVideoMode != 1)
					{
						if (  bink->Frames - 1 <= bink->playedframes)
							LoopOK = 0;
					}

					bink_wait = BinkWait( bink );

					if ( !bink_wait )
					{
						extern u_int RealFBP2;		
						
						// decompress a frame (this is compress is usually happening while 
						//   the previous frame is being uploaded)
						
						BinkDoFrame( bink );

						// wait until the previous dma is done.  Note that we don't wait for the 
						//   page flip, since we want to overlap the BinkCopyToBuffer
						//   with the waiting for vblank time (which will be from 0 to 16 ms).

						// Copy into our rgb buffer - we use the UC_ADDR macro to access our
						//   in uncached memory (so that we don't pollute our data cache
						//   with a bunch of pixels that are on the way out the door anyway).
						if ( BinkCopyToBufferRect( bink, (void *)UC_ADDR( rgb32_buffer ),
						   bink->Width * 4, bink->Height>>1, 0, 0, 
						   0,
						   0,
						   bink->Width,
						   bink->Height>>1,
						   BINKCOPYALL | BINKSURFACE32) == 0 )
						{
							
							sceDmaTag * dma_tag;
							if ((BinkVideoMode == 1) && (VBlankCounter & (64+32)))
								BINK_DrawString_Demo("DEMO",(u32 *)rgb32_buffer,bink->Width,bink->Height);
							FlushCache(0);
							dma_tag = UC_ADDR( dma_chain );
							setup_backbuffer_upload( &dma_tag, RealFBP2 * 64, 
							                 rgb32_buffer, bink->Width, bink->Height>>1 ,0);
				
							sceDmaAddEnd( &dma_tag, 0, 0 );
							FlushCache(0);
							sceDmaSend( sceDmaGetChan( SCE_DMA_GIF ), dma_chain );
							while ( *D2_CHCR & ( 1 << 8 ) ) {;};//*/

							if (BinkCopyToBufferRect( bink, (void *)UC_ADDR( rgb32_buffer),
							   bink->Width * 4, bink->Height>>1, 0, 0, 
							   0,
							   bink->Height>>1,
							   bink->Width,
							   bink->Height>>1,
							   BINKCOPYALL | BINKSURFACE32) == 0)//*/
						   {
								u32 SaveBack;
								BINK_DrawSUB(bink,(u32 *)rgb32_buffer,bink->Width,bink->Height);
								//BINK_DrawTEXT("Aber lmsdqsmdlk qmsldk.",(u32 *)rgb32_buffer,bink->Width,bink->Height);
								dma_tag = UC_ADDR( dma_chain );
								setup_backbuffer_upload( &dma_tag, RealFBP2 * 64, 
								                 rgb32_buffer, bink->Width, bink->Height>>1 ,1);
								sceDmaAddEnd( &dma_tag, 0, 0 );
								FlushCache(0);
								sceDmaSend( sceDmaGetChan( SCE_DMA_GIF ), dma_chain );
								while ( *D2_CHCR & ( 1 << 8 ) ) {;};
								SaveBack = GspGlobal_ACCESS(ulBackColor);
								BinkVideoWith = bink->Width;
								BinkVideoHeight = bink->Height;
								GspGlobal_ACCESS(ulBackColor) = 0;
								//Gsp_FlipFB();
								//Gsp_FloatingFlip_START();
								BINK_FLIP();
								GspGlobal_ACCESS(ulBackColor) = SaveBack;
								//Gsp_FloatingFlip_START();
								
								
								INO_Update();
								if ((BinkVideoMode == 1) && (BinkVideoIsSkippable))
								{
									if (ps2INO_IsJOYTouched(0))
										LoopOK = 0;
									Demo_gus_playmode = 0;
								}
								
								if ((BinkVideoMode == 0) && ps2INO_IsJOYTouched(2)) // Intro end
								{
									if (((float)bink->FrameNum/((float)bink->FrameRate/(float)bink->FrameRateDiv)) > 5.0f)
									 	LoopOK = 0;
								}
							   	
						   }
							
						}//*/
						// advance to the next frame 
						BinkNextFrame( bink );
						// make sure we didn't overflow our buffer
						// start the dma up!
					}

					if ( bink->ReadError)
					{
						LoopOK = 0;
						goto READ_ERROR;
					}
				}
				#endif
READ_ERROR:				
			BinkClose( bink );
			}

			// never gets here, but this is how you shutdown...
			ss=RAD_IOPCancelAllAsyncs();
			//printf("RAD_IOPCancelAllAsyncs %d\n", ss);
			
			ss=RAD_IOPShutDown();
			//printf("RAD_IOPShutDown %d\n", ss);
			
		}

		// free the memory we allocated previously
		//sceSifInitIopHeap();
		sceSifFreeIopHeap( iop_addr );
		BINK_Mem_Free(rgb32_buffer_BASE);
#endif	
	}//*/
	BinkVideoMode = 1;
	
	SPG2_Free();	
	SPG2_SpaceBase = NULL; 
	SPG2_SpaceSize = 0;
	
	GSPDB_Free();	
	GSPDB_SpaceBase = NULL; 
	GSPDB_SpaceSize = 0;
	Gsp_Flush(FLUSH_ALL);

	if (Demo_gus_playmode) goto LoopAttract;
	
	BINK_SUBClose();

	ps2SND_i_ChangePriority(-4);
	SND_MuteAll(FALSE);
	SND_TrackPauseAll(FALSE);
	SND_Update(NULL);

	
	ps2SND_FlushModif();
	eeRPC_FlushCommandAfterEngine();

	ps2SND_RecallFirstStream();
	/*
	if (BinkVideoMode == 1)
	{
		while (ps2INO_IsJOYTouched(2))
		{
			INO_Update();
		}
	}*/
	INO_Update();
	
	INO_Update();
	
	bfirst = 0; 
	
	TRIPLE_BIGSWAP = 0;

	GSP_BIGFADETOBLACK = 0;
	
	LocalVblankCounter = VBlankCounter;
	{
		Demo_gus_playmode = 0; // End of ATTRACT
	}
	BinkVideoWith = 0;
	ALLOCATEDSIZE = 0;
	BinkVideoIsSkippable = 1;
	return( EXIT_SUCCESS );
}

extern void GSP_ComputeVideoName(char *FileName);
extern void GSP_ComputeVideoPath(char *FileName,char *BNKName);
void Bink_IA(u32 Mode)
{
	u32 TID,Language;
	char FileName[32];
	char BNKName[256];
	switch (Mode)
	{
		case 0:
			sprintf(FileName,"NEWGAME.BIK");
			{
				extern int  TEXT_gi_ChangedLang;
				extern int  AI_EvalFunc_IoConsLangGet_C(void);
				extern int  TEXT_i_GetLang(void);
				BOOL INO_b_LanguageIsPresent(int i);
    			BinkVideoIsSkippable = 1;
			    Language = (TEXT_gi_ChangedLang>1) ? TEXT_i_GetLang() : AI_EvalFunc_IoConsLangGet_C();
			    TID = 0;
			    switch (Language)
			    {
					case INO_e_English:	TID = 0;break;
					case INO_e_French:	TID = 1;break;
					case INO_e_Spanish:	TID = 2;break;
					case INO_e_German:	TID = 3;break;
					case INO_e_Italian:	TID = 4;break;
			    }
    			BinkSetSoundTrack(1,&TID);
			}
			break;
		case 1:sprintf(FileName,"BONUS_0.BIK");break;
		case 2:sprintf(FileName,"BONUS_1.BIK");break;
		case 3:sprintf(FileName,"BONUS_2.BIK");break;
		case 4:sprintf(FileName,"BONUS_3.BIK");
			{
				extern int  TEXT_gi_ChangedLang;
				extern int  AI_EvalFunc_IoConsLangGet_C(void);
				extern int  TEXT_i_GetLang(void);
				BOOL INO_b_LanguageIsPresent(int i);
    			BinkVideoIsSkippable = 0;
			    Language = (TEXT_gi_ChangedLang>1) ? TEXT_i_GetLang() : AI_EvalFunc_IoConsLangGet_C();
			    TID = 0;
			    switch (Language)
			    {
					case INO_e_English:	TID = 0;break;
					case INO_e_French:	TID = 1;break;
					case INO_e_Spanish:	TID = 2;break;
					case INO_e_German:	TID = 3;break;
					case INO_e_Italian:	TID = 4;break;
			    }
    			BinkSetSoundTrack(1,&TID);
			}
			break;
		
		case 5:sprintf(FileName,"BONUS_4.BIK");break;
		case 6:sprintf(FileName,"BONUS_5.BIK");break;
		case 7:sprintf(FileName,"BONUS_6.BIK");break;
		case 8:sprintf(FileName,"BONUS_7.BIK");break;
	}
	BinkVideoMode = 0;
	GSP_ComputeVideoPath(FileName,BNKName);
	BinkZobHOOK(BNKName);
	
	TID = 0;
	BinkSetSoundTrack(1,&TID);
	
}

int BinkZob( )
{
	
	if (BinkVideoMode == 0)
	{
		char FileName[32];
		char BNKName[256];

/*		sprintf(FileName,"BONUS_0.BIK");
		GSP_ComputeVideoPath(FileName,BNKName);
		BinkZobHOOK(BNKName);
		BinkVideoMode = 0;//*/
		sprintf(FileName,"INTRO.BIK");
		GSP_ComputeVideoPath(FileName,BNKName);
		BinkZobHOOK(BNKName);
		BinkVideoMode = 0;
		sprintf(FileName,"UNIVERSL.BIK");
		GSP_ComputeVideoPath(FileName,BNKName);
		BinkZobHOOK(BNKName);

		BinkVideoMode = 0;
		sprintf(FileName,"WETA.BIK");
		GSP_ComputeVideoPath(FileName,BNKName);
		BinkZobHOOK(BNKName);
	}
	else 
		BinkZobHOOK(NULL);
}

volatile HBINK binkOffLine;
volatile U32 * SPECIAL_rgb32_buffer = 0;
volatile U32 * SPECIAL_rgb32_buffer_BASE = 0;

u32 GXP_BINK_Multithread();
volatile u32 BINK_MTH_Run = 0;
volatile u32 BINK_MTH_ReadyToFinish = 0;
#define BNK_StackSize	5000
static u_int BNK_ThreadStack[BNK_StackSize] __attribute__((aligned(16)));
static u_int BNK_SemaphID;
static u_int BNK_ThreadID;
static u_int BNK_MainThreadID;
u32 GXP_BINK_LoaderThread();
u32 BNK_DEFAULTHASBEENCHOOSED = 0;

void * g_iop_addr = NULL;
volatile s32 g_FinalVolume;
volatile s32 g_CurrentVolume;
volatile s32 g_IncrementVolume;
volatile s32 g_MasterVolume;

void GSP_Bink_BeginLoad(u32 WOLBK)
{
#ifdef GSP_BINK_ACTIVATE_VIDEO_LOADING
	extern float BIG_gf_DispBinProgressSave ;
	u32 SoftBuffer = 0;
	s32		BINK_FILE;
	s32     BINK_SIZE;
	static u32 bfirst = 1;
	extern u32 ps2INO_IsJOYTouched(u32 NoMushrooms);
	char FileName[32];
	char BNKName[256];
		extern u32 GSP_BIGFADETOBLACK;
	extern void SND_MuteAll(u32);
	extern void SND_TrackPauseAll(u32);
	extern void SND_Update(void *);
	extern void ps2SND_FlushModif();
	extern char *GET_BINK_NAME(u32 Context);
	extern unsigned short Demo_gus_playmode;
	static u32 IsInit = 1;
	static u32 IsInit2 = 4;
	if (IsInit2) 
	{
		IsInit2--;
		return;
	}
	
	BIG_gf_DispBinProgressSave  = 0.0f;
	if (BinkVideoMode != 1) return;
	
	if (IsInit)
	{
		struct SemaParam sema;
		struct ThreadParam param;

		BNK_MainThreadID = GetThreadId();
		sema.initCount 		= 0;
		sema.maxCount 		= 1;
		sema.numWaitThreads = 1;
		BNK_SemaphID 		= CreateSema(&sema);
		
		param.entry 		= GXP_BINK_LoaderThread;
		param.stack 		= BNK_ThreadStack;
		param.stackSize 	= BNK_StackSize * 4;
		param.gpReg 		= &_gp;
		param.initPriority 	= 10;
		BNK_ThreadID 		= CreateThread(&param);
		StartThread(BNK_ThreadID, NULL);
		IsInit = 0;
	}
	
	SPECIAL_rgb32_buffer = NULL;
	binkOffLine = NULL;
	
	Gsp_Flush(FLUSH_ALL);
	SPG2_SpaceBaseSave = SPG2_SpaceBase = (void *)SPG2_Allocate(&SPG2_SpaceSize);
	GSPDB_SpaceBaseSave = GSPDB_SpaceBase = (void *)GSPDB_Allocate(&GSPDB_SpaceSize);
	
	{
		
		ps2SND_i_ChangePriority(-1);
		ps2SND_FlushModif();
		eeRPC_FlushCommandAfterEngine();
		
		GSP_ComputeVideoName(FileName);
		if ( *FileName )
		{
			GSP_ComputeVideoPath(FileName,BNKName);
			BINK_FILE = sceOpen(BNKName,SCE_RDONLY);
			BNK_DEFAULTHASBEENCHOOSED = 0;
			
			if (BINK_FILE < 0)
			{
				u8 OVR[256];
				sprintf(OVR,"OVR\\%s",FileName);
				GSP_ComputeVideoPath(OVR,BNKName);
				BINK_FILE = sceOpen(BNKName,SCE_RDONLY);
			}
			if (BINK_FILE < 0)
			{
				u8 OVR[256];
				sprintf(OVR,"OVR2\\%s",FileName);
				GSP_ComputeVideoPath(OVR,BNKName);
				BINK_FILE = sceOpen(BNKName,SCE_RDONLY);
			}


			if (BINK_FILE < 0)
			{
				BNK_DEFAULTHASBEENCHOOSED = 1;
				GSP_ComputeVideoPath("DEFAULT.BIK",BNKName);
				BINK_FILE = sceOpen(BNKName,SCE_RDONLY);
			}
		}
		else
			BINK_FILE = -1;
		
		if (BINK_FILE >= 0)
		{
			BINK_SIZE = sceRead(BINK_FILE , SPG2_SpaceBase ,SPG2_SpaceSize);
			sceClose(BINK_FILE);
			
			BINK_SIZE += 127;
			BINK_SIZE &= ~127;
			*(u32 *)&SPG2_SpaceBase += BINK_SIZE;
			SPG2_SpaceSize -= BINK_SIZE;
			
			BinkSetMemory(BINK_Mem_p_Alloc,BINK_Mem_Free);
			g_iop_addr = sceSifAllocIopHeap( RAD_IOPMemoryAmount( RAD_IOP_NEED_CORE1 | RAD_IOP_NO_INIT_LIBSD	 ) );
			if ( RAD_IOPStartUp( 1, -1, g_iop_addr, RAD_IOP_NEED_CORE1 | RAD_IOP_NO_INIT_LIBSD	 ) )
			{
			    float   fVolume;
			    s32     s32Volume;
			    
			    BinkSoundUseRAD_IOP( 1 );
			    
            	fVolume = (float)0x7FFF ;
            	fVolume = fVolume * SND_f_GroupVolumeGet(SND_e_UserGrpMaster);
            	fVolume = fVolume * SND_f_GroupVolumeGet(SND_e_MasterGrp);
            	s32Volume = 0x7FFF & (s32)fVolume ;
            	g_FinalVolume = s32Volume ;
            	g_CurrentVolume = 0;
            	g_IncrementVolume = g_FinalVolume / 10;
            	g_MasterVolume = ps2SND_sx_MasterSnd.s_LeftVol;
    			
    			RAD_IOPHardwareVolumes_OVER( 1, g_MasterVolume , g_MasterVolume , g_CurrentVolume, g_CurrentVolume);
			        			
    			binkOffLine = BinkOpen( SPG2_SpaceBaseSave , BINKFROMMEMORY );
    			if ( binkOffLine )
    			{
    				// allocate some memory to hold the rgb pixels
    				SPECIAL_rgb32_buffer_BASE = (U32*) BINK_Mem_p_Alloc(OSRoundUp128B( binkOffLine->Width * binkOffLine->Height * 2 ) + binkOffLine->Width * 32);
    				SPECIAL_rgb32_buffer = (u32 *)(((u32)SPECIAL_rgb32_buffer_BASE + 15) & ~15);
    				BINK_MTH_Run = 1;
    			}
			}
		}

		ps2SND_i_ChangePriority(-2);
		ps2SND_FlushModif();
		eeRPC_FlushCommandAfterEngine();
		if (BINK_MTH_Run)
		    GspGlobal_ACCESS(VBlankHook) = GXP_BINK_Multithread;

	}

#endif	
}
extern u_int volatile VBlankCounter ;
u32 volatile IsOK = 1;

u32 GXP_BINK_Multithread()
{
	if (IsOK)
	{
		iSuspendThread(BNK_MainThreadID);
		iSignalSema(BNK_SemaphID);//*/
	}
	return 0;
}


u32 GXP_BINK_LoaderThread()
{
	while (1)
	{
		WaitSema(BNK_SemaphID);
		IsOK = 0;
		if (binkOffLine)
		{
			if ( SPECIAL_rgb32_buffer && (BNK_DEFAULTHASBEENCHOOSED || (binkOffLine->Frames - 1 >= binkOffLine->playedframes)))
			{
				int bink_wait;
				extern volatile float BIG_gf_DispBinProgress ;
				if (binkOffLine->playedframes < 10)
					BIG_gf_DispBinProgress = 0.0f;
				BINK_MTH_Run = 1;
				bink_wait = BinkWait( binkOffLine );

				if ( !bink_wait )
				{
					extern u_int RealFBP2;		
					BinkDoFrame( binkOffLine );
					if ( BinkCopyToBufferRect( binkOffLine, (void *)UC_ADDR( SPECIAL_rgb32_buffer ),
					   binkOffLine->Width * 4, binkOffLine->Height>>1, 0, 0, 
					   0,
					   0,
					   binkOffLine->Width,
					   binkOffLine->Height>>1,
					   BINKCOPYALL | BINKSURFACE32) == 0 )
					{
						
						sceDmaTag * dma_tag;
						dma_tag = UC_ADDR( dma_chain );
						setup_backbuffer_upload( &dma_tag, RealFBP2 * 64, 
						                 (u32 *)SPECIAL_rgb32_buffer, binkOffLine->Width, binkOffLine->Height>>1 ,0);
			
						sceDmaAddEnd( &dma_tag, 0, 0 );
						sceDmaSend( sceDmaGetChan( SCE_DMA_GIF ), dma_chain );
						while ( *D2_CHCR & ( 1 << 8 ) ) {;};//*/

						if (BinkCopyToBufferRect( binkOffLine, (void *)UC_ADDR( SPECIAL_rgb32_buffer),
						   binkOffLine->Width * 4, binkOffLine->Height>>1, 0, 0, 
						   0,
						   binkOffLine->Height>>1,
						   binkOffLine->Width,
						   binkOffLine->Height>>1,
						   BINKCOPYALL | BINKSURFACE32) == 0)//*/
					   {
							u32 SaveBack;
							dma_tag = UC_ADDR( dma_chain );
							setup_backbuffer_upload( &dma_tag, RealFBP2 * 64, (u32 *)SPECIAL_rgb32_buffer, binkOffLine->Width, binkOffLine->Height>>1 ,1);
							sceDmaAddEnd( &dma_tag, 0, 0 );
							sceDmaSend( sceDmaGetChan( SCE_DMA_GIF ), dma_chain );
							while ( *D2_CHCR & ( 1 << 8 ) ) {;};//*/
							SaveBack = GspGlobal_ACCESS(ulBackColor);
							BinkVideoWith = binkOffLine->Width;
							BinkVideoHeight = binkOffLine->Height;
							GspGlobal_ACCESS(ulBackColor) = 0;
							//Gsp_FlipFB();
							BINK_FLIP();
							GspGlobal_ACCESS(ulBackColor) = SaveBack;
		//							Gsp_Flush(FLUSH_ALL);
					   }
					}//*/
					BinkNextFrame( binkOffLine );
					
					if(g_CurrentVolume < g_FinalVolume)
					{
    					g_CurrentVolume = g_CurrentVolume + g_IncrementVolume;
    					
    					if(g_CurrentVolume > g_FinalVolume) 
    					{
    					    g_CurrentVolume = g_FinalVolume;
    					    g_IncrementVolume = 0;
    					}
    					
    					RAD_IOPHardwareVolumes_OVER( 1, g_MasterVolume , g_MasterVolume , g_CurrentVolume, g_CurrentVolume);
					}
				}
			}
			else 
				BINK_MTH_Run = 0;
			
		}
		IsOK = 1;
		ResumeThread(BNK_MainThreadID);		
	}
}

u32 EndWorlLoadDetected = 0;
void GSP_Bink_EndLoad()
{
		EndWorlLoadDetected = 1;
}
void GSP_Bink_EndLoad_Bis()
{
	extern u32 ps2INO_IsJOYTouched(u32 NoMushrooms);
	if (!EndWorlLoadDetected) return;
	EndWorlLoadDetected = 0;
	BINK_MTH_ReadyToFinish = 1;
	
	if(!BNK_DEFAULTHASBEENCHOOSED)
	{
	    while (BINK_MTH_Run && !ps2INO_IsJOYTouched(2)) {INO_Update();}
	}
	
	GspGlobal_ACCESS(VBlankHook) = NULL;
	BINK_MTH_Run = 0;
	BINK_MTH_ReadyToFinish = 0;
	if ( binkOffLine )
	{
	    
	  //  RAD_IOPHardwareVolumes_OVER( 1, g_MasterVolume , g_MasterVolume , 0, 0);
	    
		BinkClose( binkOffLine );
		binkOffLine = NULL;
		RAD_IOPCancelAllAsyncs();
		RAD_IOPShutDown();
		sceSifFreeIopHeap( g_iop_addr );
		g_iop_addr = NULL;
        ps2SND_i_ChangePriority(-5);
	}
	
	if (SPECIAL_rgb32_buffer)
	{
		BINK_Mem_Free((u32 *)SPECIAL_rgb32_buffer_BASE);
		// correction vince 18/09/2205
		SPECIAL_rgb32_buffer_BASE = NULL;
		SPECIAL_rgb32_buffer = NULL;
		// fin correction vince
	}
	
	BinkVideoWith = 0;
	SPG2_Free();	
	SPG2_SpaceBase = NULL; 
	SPG2_SpaceSize = 0;
	GSPDB_Free();	
	GSPDB_SpaceBase = NULL; 
	GSPDB_SpaceSize = 0;
	Gsp_Flush(FLUSH_ALL);
}

#if defined PSX2_TARGET && defined __cplusplus
}
#endif

