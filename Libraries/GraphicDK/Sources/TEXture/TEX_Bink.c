/*$T TEXprocedural.c GC! 1.081 03/06/02 09:55:19 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

 
#include "Precomp.h"

#ifdef _GAMECUBE

#ifdef NO_BINK
void GC_VideoFullScreenCreate(ULONG ulBK){}
void GXI_StartBinkLoad(BIG_KEY _ul_WorldFileKey){}
void GXI_StopBinkLoad(){}
void GC_VideoFullScreenKill(){}
ULONG GC_VideoFullScreenIsFinish(){return 0;}
void GC_VideoFullScreenPlay(){}
void BINK_JADE_Create(struct TEX_tdst_Bink *TB , ULONG	ul_BINK_Key) {}
void BINK_JADE_Update(struct TEX_tdst_Bink *TB) {}
void BINK_JADE_Restart(struct TEX_tdst_Bink *TB) {}
void BINK_JADE_Destroy(struct TEX_tdst_Bink *TB) {}

#else // NO_BINK

#include <fcntl.h>

#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/BAStypes.h"

#include "BIGfiles/BIGread.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

#include "LINks/LINKstruct_reg.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKtoed.h"

#include "TEXture/TEXfile.h"
#include "TEXture/TEXstruct.h"
#include "TEXture/TEX_Bink.h"
#include "TEXture/TEXprocedural.h"
#include "TEXture/TEXconvert.h"
#include "GDInterface/GDInterface.h"

#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "SOFT/SOFTlinear.h"
#include "SOFT/SOFTcolor.h"

#include "INOut/INOfile.h"
#include "INOut/INO.h"

#include "BASe/BENch/BENch.h"

#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGopen.h"


#include "SouND/Sources/SNDvolume.h"

#include "GameCube/GC_File.h"
#include "GameCube/GC_Stream.h"

#include "SDK/Sources/GameCube/GC_arammng.h"
#include "SDK/Sources/GameCube/GC_aramheap.h"
#include "SouND/Sources/gc/gcSND_ARAM.h"


/* ************************************************************************************************* 

	VARIABLES
	
************************************************************************************************* */
extern GXRenderModeObj *g_pst_mode;


// LOGOS 
#define LOGO_NB 4

char *g_asLogoFileNames[] = 
{
	"Licensed_By_Nintendo.bik",
	"INTRO.BIK",
	"UNIVERSL.BIK",
	"WETA.bik"
};

float a_fNoButtonTime[] =
{
	1.f,
	3.f,
	3.f,
	3.f
};


ULONG GC_g_ulVideoNumber = -1;

typedef struct RAD3DIMAGE
{
  U32 width;
  U32 height;
  S32 alpha_pixels;
  U32 pixel_size;
  U32 rad_surface_type;
  void *texture;
  U32 texture_size;
  GXTexObj texObj;
} RAD3DIMAGE;
typedef struct RAD3DIMAGE* HRAD3DIMAGE;


// Variables
static HBINK GXI_Bink = NULL;
static ULONG BINK_AudioTrackID = 0;

static U32          fbSize;
static S32 Loop_current = 0;
static HRAD3DIMAGE Image = 0;
static tdstGC_File *hBinkFile = NULL;





// These bools watch the buttons state
BOOL bWasButtonPressedLastFrame; 

BOOL bDisplayLoadingBar;
BOOL bLoadingInProgress; // Is loading in progress (in which case only reset can stop the video) ?
void *BINK_g_pFileBuffer = NULL;

// This bool tells us if a video start was called (so that we can start the video at flip).
BOOL bVideoStartCalled = FALSE;

static float fDispProgress;
static float g_fNoButtonTime;
static OSTime g_ulStartBinkTime;
static ULONG BINK_g_ulAudioFadeInCounter; 
static float BINK_g_fAudioVolume; 

extern BOOL SND_gb_PauseAll;


// Thread management (if video is played during loading).
// The thread has a higher priority than main thread, 
// and is resumed after each VBL interruption. 
// It is suspended after one video loop.
static OSThread BinkThread;
#define BINK_THREAD_STACK_SIZE 0x10000 // Size of stack MUST be at least 0x10000 (0x20000 is the main thread ). 
unsigned char *BinkThreadStack = NULL; 
#ifndef NO_BINK
OSThread* g_pBinkThread = NULL;
#endif // NO_BINK



//############################################################################
//## Setup the array to convert from 3D image formats to Bink formats.      ##
//############################################################################

#define RAD3DSURFACE32    0
#define RAD3DSURFACE32A   1
#define RAD3DSURFACE565   2
#define RAD3DSURFACE4444  3 // actually RGB4A3
#define RADSURFACEYUY2    4
#define RAD3DSURFACECOUNT ( RADSURFACEYUY2 + 1 )

static U32 Bink_surface_type [ RAD3DSURFACECOUNT ];

static S32 Built_tables = 0;
static GXTexFmt D3D_surface_type [ RAD3DSURFACECOUNT ];
static U32 Pixel_info [ RAD3DSURFACECOUNT ];














/* ************************************************************************************************* 

	FUNCTIONS
	
************************************************************************************************* */

extern void GXI_SetLoadMode(BOOL _bHalfFrameMode);
extern void GXI_SetPlayMode();
extern void s_EngineCheatFinal(void);
extern BOOL GC_b_IsExiting();
extern BOOL gcINO_bIsJoystickTouched();
extern void CheckMediaError(void);
extern void GC_s_CheckResetRequest(void);
extern void GXIInitGXWithRenderMode(GXRenderModeObj *pmode,BOOL _bInvertField);


// Empty functions (unused)
void GC_VideoFullScreenKill(){}
ULONG GC_VideoFullScreenIsFinish(){return 0;}
void BINK_JADE_Create(TEX_tdst_Bink *TB , ULONG	ul_BINK_Key) {}
void BINK_JADE_Update(TEX_tdst_Bink *TB) {}
void BINK_JADE_Restart(TEX_tdst_Bink *TB) {}
void BINK_JADE_Destroy(TEX_tdst_Bink *TB) {}


// Bink allocation 
void *BINK_JADE_CALLBACK_Alloc(u32 size)
{
	return MEM_p_AllocMem(size,&MEM_gst_GCMemoryInfo);
}

void BINK_JADE_CALLBACK_Free(void *Adr)
{
	MEM_Free(Adr);
}

// The sound can allocate ARAM through the aram manager.
void *aram_malloc( U32 num_bytes )
{
    return ( ( void * ) ARAM_Alloc(eARAM_Audio,gcSND_e_ARAMOneShot, num_bytes, FirstFreeBlockAlloc) );  
}
void aram_free( void * ptr )  
{
	ARAM_Free(eARAM_Audio,gcSND_e_ARAMOneShot,(ULONG)ptr ) ;
}



static void Setup_surface_array( void )
{
  Bink_surface_type[ RAD3DSURFACE32 ] = BINKSURFACE32;
  Bink_surface_type[ RAD3DSURFACE32A ] = BINKSURFACE32A;
  Bink_surface_type[ RAD3DSURFACE565 ] = BINKSURFACE565;
  Bink_surface_type[ RAD3DSURFACE4444 ] = BINKSURFACE4444;
  Bink_surface_type[ RADSURFACEYUY2 ] = BINKSURFACEYUY2;
}


//############################################################################
//## Setup the array to convert from 3D image formats to D3D formats.       ##
//############################################################################


static void RAD3D_Setup_surface_array( void )
{
  if ( !Built_tables )
  {
    D3D_surface_type[ RAD3DSURFACE32 ] = GX_TF_RGBA8;
    D3D_surface_type[ RAD3DSURFACE32A ] = GX_TF_RGBA8;
    D3D_surface_type[ RAD3DSURFACE565 ] = GX_TF_RGB565;
    D3D_surface_type[ RAD3DSURFACE4444 ] = GX_TF_RGB5A3;
    D3D_surface_type[ RADSURFACEYUY2 ] = (GXTexFmt)-1;

    Pixel_info[ RAD3DSURFACE32 ] = 4;
    Pixel_info[ RAD3DSURFACE32A ] = 0x80000000|4;
    Pixel_info[ RAD3DSURFACE565 ] = 2;
    Pixel_info[ RAD3DSURFACE4444 ] = 0x80000000|2;
    Pixel_info[ RADSURFACEYUY2 ] = 2;

    Built_tables = 1;
  }
}


//############################################################################
//## Open a RAD 3D image (a data structure to blit an image through D3D).   ##
//############################################################################
HRAD3DIMAGE Open_RAD_3D_image( int d3d_device,
                                        U32 width,
                                        U32 height,
                                        U32 rad3d_surface_format )
{
  HRAD3DIMAGE rad_image;
  U32 pixel_size;

  RAD3D_Setup_surface_array();

  // Calculate the pixel size
  pixel_size = Pixel_info[ rad3d_surface_format ] & 255;

  // Allocate enough memory for a RAD image, a list of textures and a buffer.
  rad_image = ( HRAD3DIMAGE ) MEM_p_AllocAlignMem( sizeof( RAD3DIMAGE ),32,&MEM_gst_GCMemoryInfo);

  if ( rad_image == 0 )
  {
    return( 0 );
  }

  // Set all the variables in our new structure.
  rad_image->width = width;
  rad_image->height = height;
  rad_image->alpha_pixels = (Pixel_info[rad3d_surface_format]&0x80000000)?1:0;
  rad_image->pixel_size = pixel_size;
  rad_image->rad_surface_type = rad3d_surface_format;

  rad_image->texture_size = GXGetTexBufferSize( width, height,
    D3D_surface_type[ rad3d_surface_format ], GX_FALSE, 0 );

  rad_image->texture = MEM_p_AllocAlignMem( rad_image->texture_size,32,&MEM_gst_GCMemoryInfo);
  L_memset(rad_image->texture,0,rad_image->texture_size);

  GXInitTexObj(&rad_image->texObj, rad_image->texture,
      width, height, D3D_surface_type[ rad3d_surface_format ],
      GX_CLAMP, GX_CLAMP, GX_FALSE);
  GXInitTexObjLOD(&rad_image->texObj, GX_LINEAR, GX_NEAR,
      0.0F, 0.0F, 0.0F, GX_FALSE, GX_FALSE, GX_ANISO_1);

  return( rad_image );
}


//############################################################################
//## Closes a RAD 3D image (frees textures and memory).                     ##
//############################################################################
void Close_RAD_3D_image( HRAD3DIMAGE rad_image )
{
  if ( rad_image )
  {
    if ( rad_image->texture )
		MEM_Free( rad_image->texture );
    MEM_Free( rad_image );
  }
}


/*---------------------------------------------------------------------------*
    Name:           DEMOBeforeRender

    Description:    This function sets up the viewport to render the
                    appropriate field if field rendering is enabled.
                    Field rendering is a property of the render mode.

    Arguments:      None

    Returns:        None
 *---------------------------------------------------------------------------*/
void RADDEMOBeforeRender( void )
{
    // Set up viewport (This is inappropriate for full-frame AA.)
    if (g_pst_mode->field_rendering)
    {
    	BOOL field = VIGetNextField();
    	
    	if (!bLoadingInProgress)
    		field = !field;
    	
        GXSetViewportJitter(
          0.0F, 0.0F, (float)g_pst_mode->fbWidth, (float)g_pst_mode->efbHeight,
          0.0F, 1.0F, field);
    }
    else
    {
        GXSetViewport(
          0.0F, 0.0F, (float)g_pst_mode->fbWidth, (float)g_pst_mode->efbHeight,
          0.0F, 1.0F);
    }

    // Invalidate vertex cache in GP
    GXInvalidateVtxCache();

    // Invalidate texture cache in GP
    GXInvalidateTexAll();
}



/*---------------------------------------------------------------------------*
    Name:           Draw_Init

    Description:    Initialize various drawing parameters.

    Arguments:      None

    Returns:        None
 *---------------------------------------------------------------------------*/
static void Draw_Init()
{
    Mtx v;
    Mtx44 p;
         
    // Camera Init
    MTXOrtho(p, 0, (float)g_pst_mode->viHeight, 0, (float)gul_FRAME_BUFFER_WIDTH, 0.0F, 10000.0F);
    GXSetProjection(p, GX_ORTHOGRAPHIC);

    MTXIdentity(v);
    GXLoadPosMtxImm(v, GX_PNMTX0);
    
	GX_GXSetCullMode(GX_CULL_NONE);
	GX_GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_DISABLE);
	GX_GXSetChanCtrl(  GX_COLOR0A0,
    			   GX_DISABLE,		// enable Channel
    			   GX_SRC_VTX,		// amb source
    			   GX_SRC_VTX,		// mat source
    			   GX_LIGHT_NULL,	// light mask
    			   GX_DF_CLAMP,		// diffuse function
    			   GX_AF_NONE );	// atten   function

    GX_GXSetVtxDesc(GX_VA_POS,  GX_DIRECT);
    GX_GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GX_GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GX_GXSetVtxDesc(GX_VA_TEX1, GX_NONE);
    GX_GXSetVtxDesc(GX_VA_NRM,  GX_NONE);

	GX_GXSetZCompLoc(GX_FALSE);
    GX_GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);

    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS,  GX_POS_XYZ,  GX_F32,    0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8,  0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST,   GX_F32,    0);
	
    //  Set the Texture Environment (Tev) Mode for stage 0
    GX_GXSetNumTexGens(1);     // # of Tex gen
    GX_GXSetNumChans(1);       // # of color channels
    GX_GXSetNumTevStages(1);   // # of Tev Stage

    GX_GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY,GX_FALSE, GX_PTIDENTITY);
    GX_GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
    GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
}


//############################################################################
//## Lock a RAD 3D image and return the buffer address and pitch.           ##
//############################################################################
S32 Lock_RAD_3D_image( HRAD3DIMAGE rad_image,
                                void* out_pixel_buffer,
                                U32* out_buffer_pitch,
                                U32* out_surface_type )
{
  if ( rad_image == 0 )
    return( 0 );

  // Fill the variables that were requested.
  if ( out_pixel_buffer )
    *( void** )out_pixel_buffer = rad_image->texture;

  if ( out_buffer_pitch )
    *out_buffer_pitch = rad_image->width * rad_image->pixel_size;

  if ( out_surface_type )
    *out_surface_type = rad_image->rad_surface_type;
    
  return( 1 );
}


//############################################################################
//## Unlock a RAD 3D image                                                  ##
//############################################################################
void Unlock_RAD_3D_image( HRAD3DIMAGE rad_image )
{
  if ( rad_image == 0)
    return;

  // Flush texture data.
  DCStoreRange(rad_image->texture, rad_image->texture_size);
}


//############################################################################
//## Advance a Bink file by one frame into a 3D image buffer.               ##
//############################################################################
static void Decompress_frame( HBINK bink, HRAD3DIMAGE image, S32 copy_all )
{
  void* pixels;
  U32 pixel_pitch;
  U32 pixel_format;
  
  // Decompress the Bink frame.
  BinkDoFrame( bink );
  
		
  // Lock the 3D image so that we can copy the decompressed frame into it.
  if ( Lock_RAD_3D_image( image, &pixels, &pixel_pitch, &pixel_format ) )
  {
  
    // Copy the decompressed frame into the 3D image.
    BinkCopyToBuffer( bink,
                      pixels,
                      (S32)pixel_pitch,
                      bink->Height,
                      0,0,
                      Bink_surface_type[ pixel_format ] |
                      ( ( copy_all ) ? BINKCOPYALL : 0 ) );

    // Unlock the 3D image.
    Unlock_RAD_3D_image( image );
  }
}

//############################################################################
//## Submit the vertices for one texture.                                   ##
//############################################################################
static void Submit_vertices( F32 dest_x,
                             F32 dest_y,
                             F32 scale_x,
                             F32 scale_y,
                             S32 width,
                             S32 height,
                             F32 alpha_level )
{
    F32 x1 = (S16)dest_x;
    F32 y1 = (S16)dest_y;
    F32 x2 = (S16)(dest_x + ( ( ( F32 ) width ) * scale_x ));
    F32 y2 = (S16)(dest_y + ( ( ( F32 ) height ) * scale_y ));
    U8 alpha = (U8)(alpha_level * 255);

    // Draw a triangle
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);

    GXPosition3f32(x1, y2, 0);
    GXColor4u8(0xff, 0xff, 0xff, alpha);
    GXTexCoord2f32(0, 1);

    GXPosition3f32(x1, y1, 0);
    GXColor4u8(0xff, 0xff, 0xff, alpha);
    GXTexCoord2f32(0, 0);

    GXPosition3f32(x2, y1, 0);
    GXColor4u8(0xff, 0xff, 0xff, alpha);
    GXTexCoord2f32(1, 0);

    GXPosition3f32(x2, y2, 0);
    GXColor4u8(0xff, 0xff, 0xff, alpha);
    GXTexCoord2f32(1, 1);

    GXEnd();
}


//############################################################################
//## Blit a 3D image onto the render target.                                ##
//############################################################################
void Blit_RAD_3D_image( HRAD3DIMAGE rad_image,
                                 F32 x_offset,
                                 F32 y_offset,
                                 F32 x_scale,
                                 F32 y_scale,
                                 F32 alpha_level )
{
  if ( rad_image == 0 )
    return;

  // If alpha is disabled, turn blending off
  if ( ( alpha_level >= 0.998f ) )
  {
    if ( ! rad_image->alpha_pixels )
    {
      // If the texture doesn't have alpha pixels, use no blending
      GX_GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    }
    else
    {
      GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
      GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_ONE, GX_CA_ZERO);
    }
  }
  else
  {
    GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);

    if ( ! rad_image->alpha_pixels )
    {
      // If the texture doesn't have alpha pixels, use the color alpha_level
      GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_ONE, GX_CA_ZERO);
    }
    else
    {
      // Use the texture alpha modulated with the color alpha_level
      GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
    }
  }

  // Select the texture.
  GX_GXLoadTexObj(&rad_image->texObj, GX_TEXMAP0);
  

  // Submit the vertices.
  Submit_vertices( x_offset,
                   y_offset,
                   x_scale,
                   y_scale,
                   rad_image->width,
                   rad_image->height,
                   alpha_level );
                   
  //GX_GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
  
}
  
  
//############################################################################
//## Show_frame - shows the next Bink frame.                                ##
//############################################################################

static void Show_frame_from_texture( HBINK Bink)
{
	F32 x,y;
	F32 Width_scale = (F32)gul_FRAME_BUFFER_WIDTH / (F32)Bink->Width;
	F32 Height_scale = (F32)gul_FRAME_BUFFER_HEIGHT / (F32)Bink->Height;

	// Begin a 3D frame.
	RADDEMOBeforeRender ( );

	// Draw the image on the screen (centered)...
	x = ( gul_FRAME_BUFFER_WIDTH - ( Width_scale * Bink->Width ) ) / 2 ;
	y = ( gul_FRAME_BUFFER_HEIGHT - ( Height_scale * Bink->Height ) ) / 2;

	Blit_RAD_3D_image( Image,
	                 x, y,
	                 Width_scale, Height_scale,
	                 1.0F );
	                 
	if (bDisplayLoadingBar)
	{
		extern volatile float BIG_gf_DispBinProgress ;
		ULONG ulColor = (bLoadingInProgress ? 0xffa0a0a0 : 0xff00a000);
		
		float fWExt = 200.f / (float)gul_FRAME_BUFFER_WIDTH;
		float fXExt = .5f - fWExt * .5f;
		
		float fYExt = 0.15f;
		float fHExt = 6.f / (float)g_pst_mode->viHeight;
		float fBorder = 2.f / (float)g_pst_mode->viHeight;
		
		const u32 ulTimeBeforeDispBar = 1;
		
		if (Bink->LastFrameNum * Bink->FrameRateDiv > ulTimeBeforeDispBar * Bink->FrameRate)
			fDispProgress = (fDispProgress > BIG_gf_DispBinProgress ? fDispProgress : BIG_gf_DispBinProgress);

		GXI_DrawRectangle(fXExt - 2*fBorder, fYExt -2*fBorder,fWExt + 4.f*fBorder, fHExt + 4.f*fBorder,ulColor,MAT_Cc_Op_Copy);
		GXI_DrawRectangle(fXExt -   fBorder, fYExt -  fBorder,fWExt + 2.f*fBorder, fHExt + 2.f*fBorder,0xff000000,MAT_Cc_Op_Copy);
		GXI_DrawRectangle(fXExt, fYExt, fWExt*fDispProgress, fHExt,ulColor ,MAT_Cc_Op_Copy);
	}
	
	//BINK_DrawSUB(Bink,NULL,Bink->Width,Bink->Height);
	//BINK_DrawSUB(Bink,NULL,gul_FRAME_BUFFER_WIDTH,gul_FRAME_BUFFER_HEIGHT);
	BINK_DrawSUB(Bink,NULL,640,gul_FRAME_BUFFER_HEIGHT);
	
	// End a 3D frame.
	GXI_VideoFlip();
}

void BINK_GC_Destroy()
{
    ULONG TID = 0;
    BinkSetSoundTrack(1,&TID);
    BINK_AudioTrackID = 0;

	if (GXI_Bink)
	{
		BinkClose( GXI_Bink );
		GXI_Bink = NULL;
	}
	
	BINK_SUBClose();

	if (BINK_g_pFileBuffer)
	{
		MEM_Free(BINK_g_pFileBuffer);
		BINK_g_pFileBuffer = NULL;
	}

	if (hBinkFile)
	{
		GC_fClose(hBinkFile);
		hBinkFile = NULL;
	}
	
	if (Image)
	{
		Close_RAD_3D_image(Image);
		Image = NULL;
	}

	// Set buffers and rendermode for display by game engine.
	GXI_SetPlayMode();
	
	if (SND_gb_PauseAll)
	{
		SND_MuteAll(FALSE);
		SND_TrackPauseAll(FALSE);
		SND_Update(NULL);
	}
	
#ifdef _DEBUG
	OSCheckActiveThreads();
#endif// _DEBUG
}

BOOL BINK_GC_bCreate(char *_sBinkFileName)
{
	// Heavy video means 2 full size frame buffers (which eats up more RAM).
	BOOL bHalfFrameMode;
	
	RADARAMCALLBACKS aram_callbacks = { aram_malloc, aram_free };  
	u32 ulFileLength,ulTemp;
	        
	bDisplayLoadingBar = FALSE;
	fDispProgress = 0;

 	// Read audio volume before mute
 	BINK_g_fAudioVolume = SND_f_GroupVolumeGet(SND_e_UserGrpMaster) * SND_f_GroupVolumeGet(SND_e_MasterGrp);
	BINK_g_ulAudioFadeInCounter = 0;
	

    g_ulStartBinkTime = OSGetTime();
    
	hBinkFile = GC_fOpen(_sBinkFileName, NULL);
	if (hBinkFile == NULL)
		return FALSE;

    ERR_X_Assert(BINK_g_pFileBuffer == NULL);
        
    ulFileLength = OSRoundUp32B(DVDGetLength(&hBinkFile->stFileInfo));
    
    bHalfFrameMode = bLoadingInProgress; // Use half frames while loading

	// Set bink memory callbacks
	RADSetMemory(BINK_JADE_CALLBACK_Alloc,BINK_JADE_CALLBACK_Free);
	
	// Set buffers and rendermode for display by bink.
	GXI_SetLoadMode(bHalfFrameMode);
	
	ERR_X_Assert(BINK_g_pFileBuffer == NULL);
	
	// Use buffer if loading
	if (bLoadingInProgress)
	{
		ULONG *pTemp;
	    BINK_g_pFileBuffer = MEM_p_AllocAlignMem(ulFileLength,32,&MEM_gst_GCMemoryInfo);
    	GC_fRead(hBinkFile,BINK_g_pFileBuffer,ulFileLength);
	    
		// big endian swap.
		pTemp = ((ULONG *)BINK_g_pFileBuffer) + ((ulFileLength>>2) - 1);
		while(pTemp >= BINK_g_pFileBuffer)
		{
			ulTemp = *pTemp;
			*pTemp =  ((ulTemp&0x000000ff)<<24) | ((ulTemp&0x0000ff00)<<8) | ((ulTemp&0x00ff0000)>>8) | ((ulTemp&0xff000000)>>24);
			pTemp--;
		}
		GC_fClose(hBinkFile);
		hBinkFile = NULL;
	}
	
	
	BINK_SUBOpen(_sBinkFileName);

	if (!SND_gb_PauseAll)
	{
		SND_MuteAll(TRUE);
		SND_TrackPauseAll(TRUE);
		SND_Update(NULL);    
	}	
	
	BinkSoundUseAX( &aram_callbacks );
	
	// Setup up the array that converts between surface types.
  	Setup_surface_array( );
	
	// BINK_AudioTrackID = ID de la track audio, à faire si besoin.
	if (BINK_AudioTrackID)
	{
		BinkSetSoundTrack(1, &BINK_AudioTrackID);
		if (BINK_g_pFileBuffer)
		{
			GXI_Bink = BinkOpen( BINK_g_pFileBuffer , BINKFROMMEMORY | BINKSNDTRACK);
		}
		else
		{
			do 
			{ 	
				// Read first 32 bytes of file to check for errors.
				char sBuffer[64];
				char *pAlignedBuffer = (char *) ((((ULONG)sBuffer)+31) & 0xFFFFFFE0);
		    	if (GC_fRead(hBinkFile,pAlignedBuffer,32))
		    	{				
					GXI_Bink = BinkOpen(&(hBinkFile->stFileInfo) , BINKFILEHANDLE | BINKSNDTRACK);
				}
			} while (!GXI_Bink);
		}
	}
	else
	{
		if (BINK_g_pFileBuffer)
			GXI_Bink = BinkOpen( BINK_g_pFileBuffer , BINKFROMMEMORY); 
		else
		{
			GXI_Bink = NULL;
			do 
			{ 	
				// Read first 32 bytes of file to check for errors.
				char sBuffer[64];
				char *pAlignedBuffer = (char *)((((ULONG)sBuffer)+31) & 0xFFFFFFE0);
		    	if (GC_fRead(hBinkFile,pAlignedBuffer,32))
				{
					GXI_Bink = BinkOpen(&(hBinkFile->stFileInfo) , BINKFILEHANDLE);
				}
			} while (!GXI_Bink);
		}
	}
	
	
	// Si read error (open cover, etc), alors on arrête le bink.
	if (!GXI_Bink || GXI_Bink->ReadError)
		return FALSE;
		
	// Try to open a 3D image for the Bink.
	Image = Open_RAD_3D_image( 0,
	                         GXI_Bink->Width,
	                         GXI_Bink->Height,
	                         RAD3DSURFACE32 );
	
    
	bWasButtonPressedLastFrame = gcINO_bGetButton((PAD_BUTTON_A|PAD_BUTTON_START));
	
	return TRUE;
}



void BINK_GC_SetVolume( )
{
	s32 s32Volume = (s32)(65536.0f * BINK_g_fAudioVolume);
	
	if (BINK_g_ulAudioFadeInCounter < 50)
	{
		BINK_g_ulAudioFadeInCounter++;
		s32Volume = (s32Volume * BINK_g_ulAudioFadeInCounter) / 50;
	}		

	BinkSetVolume(GXI_Bink, BINK_AudioTrackID, s32Volume) ;
}


// Return if we should continue video.
BOOL BINK_GC_bOneLoop()
{
	BOOL bPause;

#ifdef _DEBUG
	OSCheckActiveThreads();
#endif // _DEBUG
	
	// Update joystick and pad reset.
	INO_Joystick_Update();
	
	// Manage pad reset 
	s_EngineCheatFinal(); 

	// Check if reset has been requested.	
	GC_s_CheckResetRequest();
	
	// Reset button always exits video.
	//if (GC_b_IsExiting())
	//	return FALSE;
	

	BINK_GC_SetVolume();		
		
	bPause = IsMediaError();
	
	if (bPause)
		BinkPause(GXI_Bink,TRUE);
	
	// Tant qu'il y a une erreur, on reste ici.
	while (IsMediaError())
		SafeCheckMediaError();

	if (bPause)
		BinkPause(GXI_Bink,FALSE);
	
	if ( !BinkWait( GXI_Bink ) && GXI_Bink->FrameNum != GXI_Bink->Frames  )
	{
        // Decompress the Bink frame into the image buffer.
        Decompress_frame( GXI_Bink, Image, 1 );
        		
		// Keep playing the movie.
		BinkNextFrame( GXI_Bink );
		
		// En cas de read error, close
		if (GXI_Bink->ReadError)
			return FALSE;
    }
    
    // We need to render render bink texture in EBF and copy it to XFB each frame
    // because the buffers are interlaced.
    
    // Configure GX
    GXIInitGXWithRenderMode(g_pst_mode,!bLoadingInProgress);	 
    
	Draw_Init();

	// Draw the next frame.
	Show_frame_from_texture(GXI_Bink);
	



	// Either check (the input status and the loading status) or (the loading status only).
	if (g_fNoButtonTime < (float)(OSTicksToSeconds(OSDiffTick(OSGetTime(),g_ulStartBinkTime))))
	{
		BOOL bIsButtonPressed = gcINO_bGetButton((PAD_BUTTON_A|PAD_BUTTON_START));
		
		if (bIsButtonPressed && !bWasButtonPressedLastFrame)
		{
			// If user presses skip button and loading is finished, stop
			// else, display loading bar.
			if (bLoadingInProgress)
				bDisplayLoadingBar = TRUE;
			else
				return FALSE;	
		}
		bWasButtonPressedLastFrame = bIsButtonPressed;
	}
//	else if (!bLoadingInProgress)
//		return FALSE;

	// If we hit the end of the movie, and we are not loading, stop.
	if (( GXI_Bink->FrameNum == GXI_Bink->Frames ) && (!bLoadingInProgress))
		return FALSE;
	else
		return TRUE;
}




static void* GXI_BinkLoopThread(void* param)
{
	// The bink thread is now responsible for display
	GXSetCurrentGXThread (  );

	while (BINK_GC_bOneLoop())
	{
		OSSuspendThread(g_pBinkThread);
	}

	
	return 0;
}

// Play a video on this thread.
void GC_PlayBink(char *_sFileName)
{
	bLoadingInProgress = FALSE;
	
	// Créer la vidéo.
	if (BINK_GC_bCreate(_sFileName))
	{
		// Faire une boucle tant qu'on peut
		while (BINK_GC_bOneLoop());
		
		// Détruire la vidéo.
		BINK_GC_Destroy();
	}
};

void GXI_ComputeVideoName(char *FileName, u32 ulFileKey)
{
	char HEXAConv[] = "0123456789ABCDEF";
	sprintf(FileName,"BK_xxxx.BIK");
	FileName[3] = HEXAConv[((ulFileKey >> 12) & 0xf)];
	FileName[4] = HEXAConv[((ulFileKey >> 8) & 0xf)];
	FileName[5] = HEXAConv[((ulFileKey >> 4) & 0xf)];
	FileName[6] = HEXAConv[((ulFileKey >> 0) & 0xf)];
}



void GXI_StartBinkLoad(BIG_KEY _ul_WorldFileKey)
{
	char sFileName[16];
	static int iCallNbBeforeStart = 4;
	
	// Launch logo videos (once) without loading
	if (iCallNbBeforeStart == 3)
	{
		int i;
		for (i=0; i<LOGO_NB; i++)
		{
			g_fNoButtonTime = a_fNoButtonTime[i]; 
			GC_PlayBink(g_asLogoFileNames[i]);
		}
	}

	if (iCallNbBeforeStart)
	{
		iCallNbBeforeStart--; 
		return;
	}

	// Maps sans vidéo (retour au menu, crédits, etc)
	if (_ul_WorldFileKey == 0x3D00C45A ||
		_ul_WorldFileKey == 0x3D0129BE ||
		_ul_WorldFileKey == 0xC10222D4)
		return;

	// Launch loading video	(if present) during loading
	bLoadingInProgress = TRUE;
	g_fNoButtonTime = 0.f; 
	BINK_AudioTrackID = 0; 
	
	// Key -> file name
	GXI_ComputeVideoName(sFileName,_ul_WorldFileKey);
	
	if (!BINK_GC_bCreate(sFileName))
	{
		if (!BINK_GC_bCreate("default.bik"))
			return; // Could not create video thread.
	}
		
	// Creates a new thread. The thread is suspended by default.
	g_pBinkThread = &BinkThread;
	ERR_X_Assert(BinkThreadStack == NULL);
	
#ifdef _DEBUG
	// In debug, write markers at both ends of stack to check for overwriting.
	BinkThreadStack = MEM_p_AllocMem(BINK_THREAD_STACK_SIZE + 8,&MEM_gst_GCMemoryInfo);
	((u32 *)BinkThreadStack)[0] = 0xBADC0FFE;
	((u32 *)BinkThreadStack)[(4+BINK_THREAD_STACK_SIZE)>>2] = 0xF1D0F00D;
	BinkThreadStack += 4;
#else // _DEBUG
	BinkThreadStack = MEM_p_AllocMem(BINK_THREAD_STACK_SIZE,&MEM_gst_GCMemoryInfo);
#endif // _DEBUG
		
	OSCreateThread(
		g_pBinkThread, // pointer to the thread to initialize
		GXI_BinkLoopThread, // pointer to the start routine
		NULL, // parameter passed to the start routine
		BinkThreadStack + BINK_THREAD_STACK_SIZE , // initial stack address
		BINK_THREAD_STACK_SIZE, // stack size
		15, // scheduling priority - 0 (highest) -> 31 (lowest), 16 = main
		OS_THREAD_ATTR_DETACH); // detached by default
		
	// Starts the thread
	OSResumeThread(g_pBinkThread);
	
}

void GXI_StopBinkLoad()
{
	if (!g_pBinkThread)
		return;
		
	// Uncomment to stop video at end of loading.
	bLoadingInProgress = FALSE;

	// Wait here until the video is finished.
	while (!OSIsThreadTerminated(g_pBinkThread))
		OSResumeThread(g_pBinkThread);
		
	// The main thread is now responsible for display
	GXSetCurrentGXThread (  );
	
#ifdef _DEBUG
	BinkThreadStack -= 4;
	ERR_X_Assert(((u32 *)BinkThreadStack)[0] == 0xBADC0FFE);
	ERR_X_Assert(((u32 *)BinkThreadStack)[(4+BINK_THREAD_STACK_SIZE)>>2] == 0xF1D0F00D);
#endif // _DEBUG
	MEM_Free(BinkThreadStack);

	BinkThreadStack = NULL;
	g_pBinkThread = NULL;
	
	BINK_GC_Destroy();
	
}

// Call GC_VideoFullScreenCreate to say that you want the video to be played at next flip (on main thread).
void GC_VideoFullScreenCreate(ULONG _ulVideoNumber)
{
	GC_g_ulVideoNumber = _ulVideoNumber;
	bVideoStartCalled = TRUE;
}

// Really start video play (after flip)
void GC_VideoFullScreenPlay()
{
	if (bVideoStartCalled)
	{
		char sName[100];
		bVideoStartCalled = FALSE;
		g_fNoButtonTime = 0;
		
		if (GC_g_ulVideoNumber == 0)
		{
			// La video de la 1A n'est pas skippable
			g_fNoButtonTime = FLT_MAX;
			
			sprintf(sName,"%s","NEWGAME.bik");
		}
		else
		{
			sprintf(sName,"Bonus_%d.bik",GC_g_ulVideoNumber-1);
		}
		
		if ((GC_g_ulVideoNumber == 0) || (GC_g_ulVideoNumber == 4))
		{
			extern int  TEXT_gi_ChangedLang;
			extern int  AI_EvalFunc_IoConsLangGet_C(void);
			extern int  TEXT_i_GetLang(void);
			int Language = (TEXT_gi_ChangedLang>1) ? TEXT_i_GetLang() : AI_EvalFunc_IoConsLangGet_C();
			switch (Language)
			{
				case INO_e_English: BINK_AudioTrackID = 0;break;
				case INO_e_French:  BINK_AudioTrackID = 1;break;
				case INO_e_Spanish: BINK_AudioTrackID = 2;break;
				case INO_e_German:  BINK_AudioTrackID = 3;break;
				case INO_e_Italian: BINK_AudioTrackID = 4;break;
				default: BINK_AudioTrackID = 0;
			}
		}
		else
			BINK_AudioTrackID = 0;
			
		GC_PlayBink(sName);
	}
};

#endif // NO_BINK
#endif // _GAMECUBE
