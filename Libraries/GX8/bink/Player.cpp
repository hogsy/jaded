#include <xtl.h>
#include <xgraphics.h>
extern  "C"
{
#include "ENGine/Sources/DEModisk/DEModisk.h"
};

#include "Gx8/bink/player.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/BENch/Bench.h"
#include "GX8\Gx8init.h"
#include "GDInterface/GDInterface.h"
#include "INOut/INO.h"
#include "inout/INOjoystick.h"
#include "Gx8/bink/bink.h"
#include "rad3d.h"
#include "gx8/Gx8FileError.h"

#define BINKO
#ifdef BINKO
// --------- BINK ----------
#define WIDTH  640
#define HEIGHT 480

#define VideoLoader

extern "C" void BINK_SUBOpen(char *TXTFile);
extern "C" void BINK_SUBClose();
extern "C" void BINK_DrawSUB(HBINK bink,u32 *Buffer,u32 XSize,u32 YSize);
extern "C" LONG	INO_l_Joystick_Enable;

u32 volatile GLOB = 1;
extern "C" volatile float BIG_gf_DispBinProgress ;
extern "C" float AttractdwTimeout;
extern "C" void SND_MuteAll(u32);
extern "C" void SND_TrackPauseAll(u32);
extern "C" void SND_Update(void *);
extern "C" unsigned int WOR_gul_WorldKey;
extern "C" void SPG2_Free();
extern "C" STOPVIDEO=0;
extern "C" BOOL Bink_PlayVideo;
extern "C" ULONG h_SaveWorldKey;
extern bool DontPlayVideoWhileError;
extern "C" int  TEXT_gi_ChangedLang;
extern "C" int  AI_EvalFunc_IoConsLangGet_C(void);
extern "C" int  TEXT_i_GetLang(void);


static float BIG_gf_DispBinProgressSave = 0.0f;
static bool Start=0;
static HBINK Bink = 0;
static NOPAD;
static u32 IsInit = 1;
static HRAD3DIMAGE ImageR = 0;
static bool DisplayProgressiveBar = false;
u32 Piste;
char Globalfilename[ MAX_PATH ];
char Tfilename[ MAX_PATH ];
static int VolumeGeneral = 32768/2;
static int VolumeFade = 0;
HANDLE hThread;
FILE *P_File;

long	    i_size;
PBYTE memo;
u32 current_image = 0;

//void *SPG2_SpaceBaseSave = NULL;
//void *SPG2_SpaceBase = NULL;
//extern "C" u32 SPG2_Allocate(u32 *pTR);
extern "C" int AI_EvalFunc_IoButtonJustPressed_C(int _i_Button);
//static u32 Loading = 0;
//u32 SPG2_SpaceSize = 0;

/*		void PTR4* RADLINK BINK_Mem_p_Alloc(U32 Size)
		{
					void PTR4 *ret;
					Size+=1024*512;
					if (Size < SPG2_SpaceSize)
					{
								ret = SPG2_SpaceBase;
								*(u32 *)&SPG2_SpaceBase += Size;
								SPG2_SpaceSize -= Size;
								L_memset(ret , 0  ,Size);
								return ret;
					}
					ret = MEM_p_Alloc(Size);
					return ret;
		}
		void RADLINK BINK_Mem_Free(void PTR4 *Pointer)
		{
					if ((Pointer >= SPG2_SpaceBaseSave) && (Pointer <= SPG2_SpaceBase))
								return; // SPG2 allocation
					MEM_Free(Pointer);
		}

*/


struct OVERLAYINFO
{
    D3DTexture *texture;
    D3DSurface *surface;
} Image[2];

static U32 Play_fast = 0;
static S32 Loop_current = 0;
static S32 Paused = 0;
static F32 Width_scale = 1.0f;
static F32 Height_scale = 1.0f;
static F32 X_adjust = 0.0f;
static F32 Y_adjust = 0.0f;

IDirect3DDevice8 *D3D = 0;
//IDirectSound8 *ds = NULL;
static D3DVIEWPORT8 vp = { 0, 0, WIDTH, HEIGHT, 0.0f, 1.0f };

HANDLE Game_pad[4] = {0,0,0,0};
static D3DPRESENT_PARAMETERS Presentation = { 0 };

static S32 Decompress_frameTex( HBINK bink, HRAD3DIMAGE image, U32 flags )
{
  void* pixels;
  U32 pixel_pitch;
  U32 pixel_format;
  S32 skipped = 1;

  //
  // Decompress the Bink frame.
  //

  BinkDoFrame( bink );

  //
  // Lock the 3D image so that we can copy the decompressed frame into it.
  //

  if ( Lock_RAD_3D_image( image, &pixels, &pixel_pitch, &pixel_format ) )
  {
    //
    // Copy the decompressed frame into the 3D image.
    //

    skipped = BinkCopyToBuffer( bink,
                                pixels,
                                pixel_pitch,
                                bink->Height,
                                0,0,
                                flags );

    //
    // Unlock the 3D image.
    //

    Unlock_RAD_3D_image( image );
  }
  
  return( skipped );
}

static S32 Decompress_frame( HBINK bink, OVERLAYINFO * oi, U32 flags )
{
  S32 skipped;
  D3DLOCKED_RECT lock_rect;

  // Decompresse frame.

 	 BinkDoFrame( bink );
 
  // Lock 3D 

  oi->texture->LockRect( 0, &lock_rect, 0, 0 );

  // Copy decompresse frame dans image 3D image.

  skipped = BinkCopyToBuffer( bink,
                              lock_rect.pBits,
                              lock_rect.Pitch,
                              bink->Height,
                              0,0,
                              flags );

  BINK_DrawSUB(bink,(u32*)lock_rect.pBits,bink->Width,bink->Height);
  // Unlock the 3D image.

  oi->texture->UnlockRect( 0 );

  return( skipped );
}

static void Show_frame( HBINK bink, OVERLAYINFO * oi )
{
  // dessine l'image sur l'ecran...

  RECT dst_rect;
  RECT src_rect = { 0, 0, bink->Width, bink->Height };
  RECT overlay_rect = { 0, 0, WIDTH, HEIGHT };

  Width_scale=10;//<<<<<<<<<<<<<<<<<<<<<<<<<<<< FORCE
  dst_rect.left = (u32) ( ( X_adjust * Width_scale * ( F32 ) Bink->Width ) + ( WIDTH - ( Width_scale * Bink->Width ) ) / 2 );
  dst_rect.top = (u32) ( ( Y_adjust * Height_scale * ( F32 ) Bink->Height ) + ( HEIGHT - ( Height_scale * Bink->Height ) ) / 2 );
  dst_rect.right = dst_rect.left + (u32)( bink->Width * Width_scale );
  dst_rect.bottom = dst_rect.top + (u32)( bink->Height * Height_scale );

   // Clip rectangle vers taille overlay .
 
  IntersectRect( &dst_rect, &dst_rect, &overlay_rect );

   // Update this bugger.
 
  D3D->UpdateOverlay( oi->surface, &src_rect, &dst_rect, FALSE, 0 );
}


void Show_frameTex()
{


//
  // Begin a 3D frame.
  //

  Start_RAD_3D_frame ( D3D );

  //
  // Draw the image on the screen (centered)...
  //

	F32 x,y;
	Width_scale=(float)640/Bink->Width;
	Height_scale=(float)480/Bink->Height;

  x = ( X_adjust * Width_scale * ( F32 ) Bink->Width ) + ( WIDTH - ( Width_scale * Bink->Width ) ) / 2 ;
  y = ( Y_adjust * Height_scale * ( F32 ) Bink->Height ) + ( HEIGHT - ( Height_scale * Bink->Height ) ) / 2;

  Blit_RAD_3D_image( ImageR,
                     x, y,
                     Width_scale, Height_scale,
                     1.0F );

  //
  // End a 3D frame.
  //
  {
	
		float x;
		float y=420;
		float factor=140;
		float hsize=10;
		float taille;
		static int CligneColor = 0xffff0000;
		D3D->SetRenderState( D3DRS_YUVENABLE,FALSE);
		BIG_gf_DispBinProgressSave = fMax(BIG_gf_DispBinProgress,BIG_gf_DispBinProgressSave);
		if (!DisplayProgressiveBar) CligneColor = 0xff;

		if (DisplayProgressiveBar && BIG_gf_DispBinProgressSave>0 && Bink->playedframes>6 )
		{
				
			taille=BIG_gf_DispBinProgressSave;
		
			x=60;//(640-factor)/2;
			ULONG color;
			if (taille>1.f) taille=1.f;
			
			D3D->SetTexture(0,NULL);
			if (GLOB>1)
			{	
				color =0xff000000+(CligneColor<<8);
				CligneColor-=32;
				if ( CligneColor<0 )CligneColor=0xff;
			}
			else
			color =0xffffffff;//(ULONG)(taille*255);
			
			Gx8_DrawRectangle(D3D, x-1, y-1, factor+2, hsize+2, 0xffffffff);
			Gx8_DrawRectangle(D3D, x, y, factor, hsize, 0x0);
			Gx8_DrawRectangle2(D3D, x+1, y+1, (taille*factor)-2, hsize-2, 0xff000000+color,0xff000000+color);
			
			//D3D->SetRenderState( D3DRS_YUVENABLE,FALSE);
			//D3D->SetRenderState( D3DRS_YUVENABLE,TRUE);
			//pBackBuffer->UnlockRect();
		}
  }
	End_RAD_3D_frame ( D3D );
}

static S32 Update_input( void )
{
	STOPVIDEO=0;
	//INO_Update();
	if (INO_b_MultiJoystick_IsButtonJustUp(e_ButtonA) || INO_b_MultiJoystick_IsButtonJustUp(e_Start)) 

	//if  (STOPVIDEO)
	{
		STOPVIDEO=0;
		return ( 1 );
	}
	return( 0 );
}
void AI_VideoLauncher(ULONG VideoIds)
{
	Mem_CreateForBink();
	SND_MuteAll(TRUE);
	SND_TrackPauseAll(TRUE);
	SND_Update(NULL);

	BinkPlayer(VideoIds);

	Mem_RestoreForBink();
	SND_MuteAll(FALSE);
	SND_TrackPauseAll(FALSE);
	SND_Update(NULL);

}
void BinkOpenFile(ULONG VideoIds)
{
	char *p = DEM_GetLaunchPath();
	char DefaultPath[] = "D:\\";
	char folder[ MAX_PATH ];
	u32 TID=0,Language;


	if (!p) p = DefaultPath;
	sprintf( folder, "%smedia\\video\\", p);
	BinkSetSoundTrack(1,&TID); //par defaut

		// Video bonus, sequence called by AI
		if (VideoIds<2000)
		{
			switch(VideoIds)
			{
				case 0:
				sprintf( Globalfilename, "%sNEWGAME.BIK", folder);
				{
					BOOL INO_b_LanguageIsPresent(int i);
					Language = (TEXT_gi_ChangedLang>1) ? TEXT_i_GetLang() : AI_EvalFunc_IoConsLangGet_C();
					TID = 0;
                    switch (Language)
                    {
						case INO_e_English:   TID = 0;break;
						case INO_e_French:    TID = 1;break;
						case INO_e_Spanish:  TID = 2;break;
						case INO_e_German:  TID = 3;break;
						case INO_e_Italian:     TID = 4;break;
					}
					BinkSetSoundTrack(1,&TID);
				}
				break;
				case 1:sprintf(Globalfilename,"%sBONUS_0.BIK", folder);break;
				case 2:sprintf(Globalfilename,"%sBONUS_1.BIK", folder);break;
				case 3:sprintf(Globalfilename,"%sBONUS_2.BIK", folder);break;
				case 4:
				sprintf( Globalfilename, "%sBONUS_3.BIK", folder);
				{
					BOOL INO_b_LanguageIsPresent(int i);
					Language = (TEXT_gi_ChangedLang>1) ? TEXT_i_GetLang() : AI_EvalFunc_IoConsLangGet_C();
					TID = 0;
                    switch (Language)
                    {
						case INO_e_English:   TID = 0;break;
						case INO_e_French:    TID = 1;break;
						case INO_e_Spanish:  TID = 2;break;
						case INO_e_German:  TID = 3;break;
						case INO_e_Italian:     TID = 4;break;
					}
					BinkSetSoundTrack(1,&TID);
				}
				break;
				//	sprintf(Globalfilename,"%sBONUS_3.BIK", folder);break;
				case 5:sprintf(Globalfilename,"%sBONUS_4.BIK", folder);break;
				case 6:sprintf(Globalfilename,"%sBONUS_5.BIK", folder);break;
				case 7:sprintf(Globalfilename,"%sBONUS_6.BIK", folder);break;
				case 8:sprintf(Globalfilename,"%sBONUS_7.BIK", folder);break;
			}
/*			if (VideoIds==0)
			sprintf( filename, "%sNEWGAME.BIK", folder);
			else
			{
				sprintf( filename, "%sbonus_", folder);
				sprintf( folder, "%d", VideoIds-1);
				sprintf( folder,"%s.bik", folder);
				strcat(filename,folder);
			}*/

		}
		// Intro/Demo Ending/Logo...
		else
		{
			if (VideoIds==2000)
			sprintf( Globalfilename, "%sintro.bik", folder);
			else if (VideoIds==2001)
			//sprintf( Globalfilename, "%skk.bik", folder);
			sprintf( Globalfilename, "%sATTRACT.bik", folder);
			else if (VideoIds==2002)
			sprintf( Globalfilename, "%skke.bik", folder);
			else if (VideoIds==2003)
			sprintf( Globalfilename, "%suniversl.bik", folder);
			else if (VideoIds==2004)
			sprintf( Globalfilename, "%sweta.bik", folder);
		}
		#ifdef BINK51
	    //
		// 5.1 tracks
		//

		U32 TrackIDsToPlay[ 4 ] = { 0, 1, 2, 3 };

		BinkSetSoundTrack( 4, TrackIDsToPlay );
		Bink = BinkOpen( Globalfilename, BINKSNDTRACK );
 
		//if (!Bink) Gx8_FileError();

		U32 bins[ 2 ];

		bins[ 0 ] = DSMIXBIN_FRONT_LEFT;
		bins[ 1 ] = DSMIXBIN_FRONT_RIGHT;
		BinkSetMixBins( Bink, 0, bins, 2 );
		bins[ 0 ] = DSMIXBIN_FRONT_CENTER;
		BinkSetMixBins( Bink, 1, bins, 1 );
		bins[ 0 ] = DSMIXBIN_LOW_FREQUENCY;
		BinkSetMixBins( Bink, 2, bins, 1 );
		bins[ 0 ] = DSMIXBIN_BACK_LEFT;
		bins[ 1 ] = DSMIXBIN_BACK_RIGHT;
		BinkSetMixBins( Bink, 3, bins, 2 );

#else

		BINK_SUBOpen(Globalfilename);
		
		do {
			Bink = BinkOpen( Globalfilename, BINKSNDTRACK | BINKIOSIZE );
			if (!Bink)
				Gx8_FileError();//Disc Error
		}while (!Bink);
#endif
		Piste = TID;

}
void BinkPlayer(ULONG VideoIds)
//void BinkPlayer(IDirect3DDevice8 *D3D,int VideoId)
{

	//D3DVIEWPORT8 vp = { 0, 0, WIDTH, HEIGHT, 0.0f, 1.0f };
	D3D->SetViewport( &vp );

	D3D->Clear( 0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0 );
	D3D->Present( 0, 0, 0, 0 );
  
	BinkUnloadConverter( BINKCONVERTERSALL );
	BinkLoadConverter( BINKSURFACEYUY2 );

	BinkOpenFile(VideoIds);

	if ( Bink )
	{
		u32 TID=2;
		current_image = 0;

		//
		// create YUV textures .
		//
		D3D->CreateTexture( Bink->Width, Bink->Height, 1, 0,
		D3DFMT_YUY2, 0, &Image[0].texture );
		D3D->CreateTexture( Bink->Width, Bink->Height, 1, 0,
		D3DFMT_YUY2, 0, &Image[1].texture );

		VolumeFade = 0;

		if ( Image[0].texture && Image[1].texture )
		{
			//
			// Enable overlays
			//
			D3D->EnableOverlay( TRUE );
	        Image[0].texture->GetSurfaceLevel( 0, &Image[0].surface );
			Image[1].texture->GetSurfaceLevel( 0, &Image[1].surface );

			while (1)
			{
	
				//static bool slip=0;
				if ( ( !BinkWait( Bink ) ) || ( Play_fast ) )
				{
				int SaveFrameNum;	
					//HBINK error;
					//D3D->EnableOverlay( TRUE );
					Bink_PlayVideo = TRUE;
					
					if (VolumeFade<VolumeGeneral) VolumeFade+=1000;
					else VolumeFade=VolumeGeneral;
					BinkSetVolume(Bink,Piste,VolumeFade);//32768

					if ( Decompress_frame( Bink,&Image[ current_image ^ 1 ],BINKCOPYALL | BINKSURFACEYUY2 ) == 0 )
					{
						current_image ^= 1;
						// next frame.
						Show_frame( Bink, &Image[current_image] );
					}

					if ( ! Loop_current )
					{
						if ( Bink->FrameNum == Bink->Frames )
						{
							break;
						}
					}
						BinkNextFrame( Bink );
						SaveFrameNum = Bink->FrameNum+1;
						
						if (Bink->ReadError) 
						//if ( Bink->FrameNum>70 && !slip)
						{
							//slip=1;
							BinkClose( Bink );
							Gx8_FileError();
							do
							{
								Bink = BinkOpen( Globalfilename, 0 );
								if (!Bink) Gx8_FileError();
								D3D->EnableOverlay( TRUE );

							}while(!Bink);
							
							BinkGoto(Bink,SaveFrameNum,0);
						}
				}
				else
				{
					if ( Paused )
					{
						Show_frame( Bink, &Image[current_image] );
					}
				}

				if ( Update_input( ) == 1 && VideoIds!=2002 && VideoIds!=0 )//2 demo end not skippable
				{
					if ( Bink->playedframes >30*3 )//if > 3 seconde = skip/break
					{
						//Sleep( 600 );
						break;
					}
				}
			}

                D3D->BlockUntilVerticalBlank();
				D3D->EnableOverlay( FALSE );
		 
				Image[0].surface->Release();
				Image[1].surface->Release();
			}

		if( Image[0].texture )
		{
			Image[0].texture->Release();
			Image[0].texture = NULL;
		}

		if( Image[1].texture )
		{
			Image[1].texture->Release();
			Image[1].texture = NULL;
		}
	 
		BinkClose( Bink );
		Bink = 0;
		Bink_PlayVideo = FALSE;

    }

  BINK_SUBClose();
}
#endif

#ifdef VideoLoader
DWORD __stdcall MoviePlayerThreadBink( void* NONO  )
{

	if ( Bink )
	{
	    if ( ImageR )
		{
 
			while (1)
			{
				if ( Bink && GLOB>0 )
				{
					if ( (( !BinkWait( Bink ) ) || ( Play_fast ) ) && GLOB>0 && !DontPlayVideoWhileError)
					{

						Bink_PlayVideo = TRUE;
						if (VolumeFade<VolumeGeneral) VolumeFade+=1000;
						else VolumeFade=VolumeGeneral;
						BinkSetVolume(Bink,0,VolumeFade);//32768

						if ( Decompress_frameTex( Bink, ImageR, BINKSURFACEYUY2 ) == 0 )
						{
						//
						// Draw the next frame.
						//

						Show_frameTex( );
						}
						if (Bink->playedframes <30)
						//if ( Bink->FrameNum < 10) 
						{
							BIG_gf_DispBinProgress = 0.0f;
						}
						/*if (Bink->playedframes <6 && GLOB==2 ) 
						{
							GLOB=3;break;
						}*/

						if ( ! Loop_current )
						{
							if ( Bink->FrameNum == Bink->Frames )
							{
								if (GLOB==2) {GLOB=3;break;}
							}
						}

						do 
						{
							BinkNextFrame( Bink );
							if (Bink->ReadError) 
							Gx8_FileError();
						}while(Bink->ReadError );//|| !Bink);

						
					}//Bink Wait 
					else 
					{
						if ( Update_input( ) == 1 )
						{
							DisplayProgressiveBar = true;
							if (GLOB==2) {GLOB=3;break;}
						}
						Sleep(10);
					}

					//if (GLOB==2) {GLOB=3;break;}
				} //Bink && GLOB>0
			
			}//While
			if (GLOB==2) {GLOB=3;}
		
		}//ImageR
	} // Bink

	//	D3D->BlockUntilVerticalBlank();
	//	D3D->EnableOverlay( FALSE );
	Bink_PlayVideo = FALSE;
	return (0);		
}
#endif

void Gx8_ComputeVideoName(char *FileName)
{
            char HEXAConv[] = "0123456789ABCDEF";
            sprintf(FileName,"BK_xxxx.BIK");
            FileName[3] = HEXAConv[((WOR_gul_WorldKey >> 12) & 0xf)];
            FileName[4] = HEXAConv[((WOR_gul_WorldKey >> 8) & 0xf)];
            FileName[5] = HEXAConv[((WOR_gul_WorldKey >> 4) & 0xf)];
            FileName[6] = HEXAConv[((WOR_gul_WorldKey >> 0) & 0xf)];
}


void  Gx8_BeginWorldLoad()
{
#ifndef _XBOXDEMO
#ifdef VideoLoader
	
	Gx8_tdst_SpecificData	*pst_SD;
	pst_SD = (Gx8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData;
	D3D=pst_SD->mp_D3DDevice;
	BIG_gf_DispBinProgressSave  = 0.0f;
	DisplayProgressiveBar = false;
	static int IsInit2 = 4;

	// Pour éviter d'en avoir avant la premiere map.
	if ( IsInit2-->0 ) return;

	// ====== To be clear if needed: temporaly detect the world for not display video before intro logo =====
	if ( h_SaveWorldKey==0 || h_SaveWorldKey==1224927833 || h_SaveWorldKey==1023460434) return;
	// ======================================================================================================
	
	// No Thread return;
	if ( hThread ) return;

	//SPG2_SpaceBaseSave = SPG2_SpaceBase = (void *)SPG2_Allocate(&SPG2_SpaceSize);	
	//BinkSetMemory(BINK_Mem_p_Alloc,BINK_Mem_Free);
	Mem_CreateForBink();

	if (IsInit)
	{
	
		char *p = DEM_GetLaunchPath();
		char DefaultPath[] = "D:\\";
		//char filename[ MAX_PATH ];
		char folder[ MAX_PATH ];

		if (!p) p = DefaultPath;
		wsprintf( folder, "%smedia\\video\\", p);

		Gx8_ComputeVideoName(Globalfilename);

		//strcpy(Tfilename,folder);
		//strcat(Tfilename,Globalfilename);
		//strcpy(Globalfilename,p);

		//Bink = BinkOpen( Tfilename, 0 );

		//wsprintf( Tfilename, "%s3e15.bik", p);
		//P_File=fopen(Tfilename, "rb")			;//L_fopen(_file, L_fopen_RB)

		//1er repertoire
		wsprintf( folder, "%smedia\\video\\OVR\\", p);
		strcpy(Tfilename,folder);
		strcat(Tfilename,Globalfilename);
		P_File=fopen(Tfilename, "rb");
		
		if (!P_File)
		{
			//2eme repertoire
			wsprintf( folder, "%smedia\\video\\OVR2\\", p);
			strcpy(Tfilename,folder);
			strcat(Tfilename,Globalfilename);
			P_File=fopen(Tfilename, "rb");
		}
		
		if (!P_File) 
		{
			wsprintf( folder, "%smedia\\video\\", p);
			wsprintf( Tfilename, "%sDEFAULT.bik", folder);
			//wsprintf( Tfilename, "%sintro.bik", folder);
		
			do {	
					P_File=fopen(Tfilename, "rb");
					if (!P_File) Gx8_FileError(); // Disc Error 
				}while (!P_File);
		
		}


		if (P_File)
		{
			//SPG2_SpaceBaseSave = SPG2_SpaceBase = (void *)SPG2_Allocate(&SPG2_SpaceSize);	
		
			int ret;
			{
				long savepos;//, i_size;
				savepos = ftell(P_File); 
				fseek(P_File, 0, SEEK_END); 
				i_size = ftell(P_File); 
				fseek(P_File, savepos, SEEK_SET); 
			}
			
			memo = (PBYTE)malloc( i_size );
			
			//fread(SPG2_SpaceBaseSave, 1,i_size, popo);//(sizeof)85cdc
			
			do {
				ret = fread(memo, 1,i_size, P_File);//(sizeof)85cdc
				if (!ret) Gx8_FileError();
			}while(!ret);

			fclose(P_File);
			
//			Bink = BinkOpen( (char*)SPG2_SpaceBaseSave, BINKFROMMEMORY );

			Bink = BinkOpen( (char*)memo, BINKFROMMEMORY );

		if (Bink) 
		{	

			BinkUnloadConverter( BINKCONVERTERSALL );
			BinkLoadConverter( BINKSURFACEYUY2 );

			SND_MuteAll(TRUE);
			SND_TrackPauseAll(TRUE);
			SND_Update(NULL);

			
			ImageR = Open_RAD_3D_image( D3D,
                                 Bink->Width,
                                 Bink->Height,
                                 RAD3DSURFACEYUY2 );
 
			D3DVIEWPORT8 vp = { 0, 0, WIDTH, HEIGHT, 0.0f, 1.0f };
			D3D->SetViewport( &vp );
			
			VolumeFade =0;
			BinkSetVolume(Bink,0,VolumeFade);

			if (!hThread)
			hThread = CreateThread( 0, 0, &MoviePlayerThreadBink, 0, 0, 0 );
			IsInit= 0;
			IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_CULLMODE, D3DCULL_NONE );
			/*IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ZWRITEENABLE,FALSE);
			IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ZENABLE,FALSE);
			IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ZFUNC,D3DCMP_ALWAYS);
			IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_ALPHATESTENABLE,FALSE);*/

		}
		
		
		}
		//else fclose(popo);
		
	}
	

#endif
#endif
}
void Gx8_EndWorldLoad()
{
#ifdef VideoLoader
#ifndef _XBOXDEMO
	Gx8_tdst_SpecificData	*pst_SD;
	pst_SD = (Gx8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData;
	D3D=pst_SD->mp_D3DDevice;

	// Wait for the thread to terminate.
	if ( hThread )
	{
		GLOB = 2;
		
		//Wait for the thread to terminate.
		while (GLOB==2) 
		{
		}

		// Clean up thread handles to free all thread resources.
		// This has to be done after we finish waiting on the handle.
	
		CloseHandle( hThread );
		hThread=NULL;

		if (Bink)
		{
			//if ( memo ) 
			free( memo );
				
			BinkClose( Bink );
			Bink = 0;
			
			//D3D->BlockUntilVerticalBlank();
			//D3D->EnableOverlay( FALSE );
			//if (ds) ds->Release();
			//if (D3D) D3D->Release();
			Close_RAD_3D_image( ImageR );
			ImageR = 0;
			DisplayProgressiveBar = false;

			SND_MuteAll(FALSE);
			SND_TrackPauseAll(FALSE);
			SND_Update(NULL);
			IsInit= 1;
		}
	}
	
	Mem_RestoreForBink();
	/*SPG2_Free();	
	SPG2_SpaceBase = NULL; 
	SPG2_SpaceSize = 0;*/
#endif
#endif
	//INO_Joystick_Update();//pour etre sur que le debranchement soit pris en compte
	GLOB = 1;// C'est comme les globulos .. lol...
}
void Mem_CreateForBink()
{
	Gx8_tdst_SpecificData	*pst_SD;
	pst_SD = (Gx8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData;

	if (pst_SD->pSaveBufferTexture!=NULL)
	{
		D3DTexture_Release(pst_SD->pSaveBufferTexture);
		pst_SD->pSaveBufferTexture=NULL;
	}
	
	if (pst_SD->pZBufferTexture!=NULL)
	{
		MEM_Free( pst_SD->pZBufferTexture );
		pst_SD->pZBufferTexture=NULL;
	}
}

void Mem_RestoreForBink()
{
	HRESULT hr;
	Gx8_tdst_SpecificData	*pst_SD;
	pst_SD = (Gx8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData;

	if (pst_SD->pSaveBufferTexture==NULL)
	{
		hr = IDirect3DDevice8_CreateTexture
		(
			pst_SD->mp_D3DDevice,
			640,//desc.Width,
			480,
			0,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_LIN_A8R8G8B8,
			D3DPOOL_DEFAULT,
			&pst_SD->pSaveBufferTexture
		);
		ERR_X_Assert( SUCCEEDED( hr ));
	}

	if (pst_SD->pZBufferTexture==NULL)
	pst_SD->pZBufferTexture = (IDirect3DTexture8*)MEM_p_Alloc( sizeof(IDirect3DTexture8) );
}