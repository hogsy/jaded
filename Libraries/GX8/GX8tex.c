/*$T Gx8tex.c GC! 1.081 01/14/02 11:02:39 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/* Aim: Load texture in memory to be used by OpenGL RC. */

#include "Gx8tex.h"
#include <d3dx8tex.h>

#include <xgraphics.h>
#include <D3D8.h>
//#include <D3D8-Xbox.h>


#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/BAStypes.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "TEXture/TEXconvert.h"
#include "TEXture/TEXmemory.h"

#include "Gx8init.h"
#include "Gx8color.h"
#include <assert.h>

extern Gx8_tdst_SpecificData    *p_gGx8SpecificData;
extern BOOL Antialias_E3;

#define MAX_PAL 512
D3DPalette *g_aPal[MAX_PAL];

extern int xb_NumberOfPalettes;
extern BOOL Normalmap;

//#define rnd()  ((rand() ) / RAND_MAX)

/*
#define PALETTE_SIZE	1024

UINT PaletteContiguousMemory;
bool PaletteFree[MAX_PAL];
*/

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
	PALETTE BUFFER MANAGMENT FUNCTIONS
 =======================================================================================================================
 */

/*
UINT Gx8_AllocPaletteMemory(void)
{
	int i;
	UINT addressOffset;

	for(i=0;i<MAX_PAL;i++)
	{
		if (PaletteFree[i])
		{
			PaletteFree[i]=false;
			addressOffset=i*PALETTE_SIZE;
			return PaletteContiguousMemory+addressOffset;
		}
	}

	// palette memory full!
	assert(false);

	return 0;
}

void Gx8_FreePaletteMemory( UINT MemoryFreed )
{
	int Offset;

	Offset=(MemoryFreed-PaletteContiguousMemory)/PALETTE_SIZE;

	assert((Offset>=0)&&(Offset<MAX_PAL));

	PaletteFree[Offset]=true;
}

void Gx8_InitPaletteMemory(void)
{
	int i;

	for(i=0;i<MAX_PAL;i++)
	{
		PaletteFree[i]=true;
	}

	PaletteContiguousMemory = (UINT)D3D_AllocContiguousMemory( PALETTE_SIZE*MAX_PAL, D3DPALETTE_ALIGNMENT  );
}

*/
/*
 =======================================================================================================================
 END PALETTE BUFFER SPECIFIC FUNCTIONS
 =======================================================================================================================
 */

/*
 =======================================================================================================================
    Aim:    Init specific data for texture creation
 =======================================================================================================================
 */
LONG Gx8_l_Texture_Init(GDI_tdst_DisplayData *_pst_DD, ULONG _ul_NbTextures)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Gx8_tdst_SpecificData   *pst_SD;
    LONG                    l_Size;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_SD = (Gx8_tdst_SpecificData *) _pst_DD->pv_SpecificData;

    if(_ul_NbTextures == 0) _ul_NbTextures = 1;
    pst_SD->l_NumberOfTextures = _ul_NbTextures;

    l_Size = (_ul_NbTextures + 1) * sizeof(D3DTexture*);
    pst_SD->dul_Texture = MEM_p_Alloc(l_Size);

	L_memset(pst_SD->dul_Texture, 0, l_Size);

    return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void Gx8_CreateAfterFXTextures(Gx8_tdst_SpecificData *_pst_SD)
{
    HRESULT                 hr;
    IDirect3DSurface8* pBackBufferSurface;
    IDirect3DSurface8* pZBufferSurface;
    D3DSURFACE_DESC desc;
   // int i;

    // creation of the back buffer texture
    // PC_TODO: creation of the back buffer texture
    IDirect3DDevice8_GetBackBuffer( _pst_SD->mp_D3DDevice, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBufferSurface );
    IDirect3DSurface8_GetDesc( pBackBufferSurface, &desc );
    ZeroMemory( _pst_SD->pBackBufferTexture, sizeof(IDirect3DTexture8) );
    XGSetTextureHeader( desc.Width, desc.Height, 1, 0, desc.Format, 0, _pst_SD->pBackBufferTexture, //desc.Height
                        pBackBufferSurface->Data, desc.Width * XGBytesPerPixelFromFormat(desc.Format) );
    IDirect3DSurface8_Release(pBackBufferSurface);

/*if ( Antialias_E3 )
{
    ZeroMemory( _pst_SD->pBackBufferTextureTemp, sizeof(IDirect3DTexture8) );
    XGSetTextureHeader( desc.Width, desc.Height, 1, 0, desc.Format, 0, _pst_SD->pBackBufferTextureTemp, 
                        pBackBufferSurface->Data, desc.Width * XGBytesPerPixelFromFormat(desc.Format) );
    IDirect3DSurface8_Release(pBackBufferSurface);
}*/
	
/*	
	// set AfterFx Textures sizes
    _pst_SD->AfterFXTexture_Width = desc.Width;
    _pst_SD->AfterFXTexture_Height = desc.Height;
*/

    // creation of the Z buffer texture
    // PC_TODO: creation of the Z buffer texture
    IDirect3DDevice8_GetDepthStencilSurface( _pst_SD->mp_D3DDevice, &pZBufferSurface );
    IDirect3DSurface8_GetDesc( pZBufferSurface, &desc );
	ZeroMemory( _pst_SD->pZBufferTexture, sizeof(IDirect3DTexture8) );
    XGSetTextureHeader( desc.Width, desc.Height, 1, 0, desc.Format, 0, _pst_SD->pZBufferTexture, 
    pZBufferSurface->Data, desc.Width * XGBytesPerPixelFromFormat(desc.Format) );
    IDirect3DSurface8_Release(pZBufferSurface);

		// creation of the texture where everything is drawn temporally...
		
		if (_pst_SD->pTmpFrameBufferTexture != NULL ) D3DTexture_Release(_pst_SD->pTmpFrameBufferTexture);
		hr = IDirect3DDevice8_CreateTexture
        (
            _pst_SD->mp_D3DDevice,
            640,//desc.Width,
            480,
            1,//Warning maybe -> 1
            D3DUSAGE_RENDERTARGET,
            D3DFMT_LIN_A8R8G8B8,
            D3DPOOL_DEFAULT,
            &_pst_SD->pTmpFrameBufferTexture
        );
//if ( Antialias_E3 )
{
		// Yoann extension
		if (_pst_SD->pSaveBufferTexture != NULL ) D3DTexture_Release(_pst_SD->pSaveBufferTexture);
		hr = IDirect3DDevice8_CreateTexture
        //int val = 640;
		(
            _pst_SD->mp_D3DDevice,
            640,//desc.Width,
            480,
            0,
            D3DUSAGE_RENDERTARGET,
            D3DFMT_LIN_A8R8G8B8,
            D3DPOOL_DEFAULT, // Test, maybe change to DEFAULT.
            &_pst_SD->pSaveBufferTexture
        );
}
		
/*
	// creation of the texture for BW effect ...
    hr = IDirect3DDevice8_CreateTexture
        (
            _pst_SD->mp_D3DDevice,
            640,
            480,
            1,
            D3DUSAGE_RENDERTARGET,
            D3DFMT_LIN_L8,
            D3DPOOL_DEFAULT,
            &_pst_SD->pBWBufferTexture
        );
*/

}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_l_Texture_CreateShadowTextures(Gx8_tdst_SpecificData *_pst_SD)
{
    /*~~~~~~~~~~~~~~~*/
    ULONG   TexCounter;
    /*~~~~~~~~~~~~~~~*/
    TexCounter = MaxShadowTexture;
    while(TexCounter--)
    {
        IDirect3DDevice8_CreateTexture
            (
                p_gGx8SpecificData->mp_D3DDevice,
                ShadowTextureSize,
                ShadowTextureSize,
                1,
                D3DUSAGE_RENDERTARGET,
                D3DFMT_A8R8G8B8 /* | D3DFMT_R8G8B8 */ ,
                D3DPOOL_DEFAULT /* | D3DPOOL_MANAGED | D3DPOOL_SYSTEMMEM */ ,
                &_pst_SD->dul_SDW_Texture[TexCounter]
            );
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_l_Texture_DestroyShadowTextures(Gx8_tdst_SpecificData *_pst_SD)
{
    /*~~~~~~~~~~~~~~~*/
    ULONG   TexCounter;
    /*~~~~~~~~~~~~~~~*/

    TexCounter = MaxShadowTexture;
    
    while(TexCounter--)
    {
        D3DTexture_Release (_pst_SD->dul_SDW_Texture[TexCounter]);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void Gx8_Palette_Load(GDI_tdst_DisplayData *_pst_DD, TEX_tdst_Palette *pst_Pal, ULONG Index)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Gx8_tdst_SpecificData   *pst_SD;
    ULONG                   *pul_Color;
    int i;
    ULONG myColors[256];
    IDirect3DPalette8 *pPalette;
    HRESULT                 hr;
    D3DCOLOR *pColors;
	//void *pPaletteBuffer;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    if (Index == 0xFFFFFFFF) return;

    pst_SD = GX8_M_SD( _pst_DD );

	// THIS CODE WOULD BE USED FOR AVOIDING MEMORY WASTE
	// THE PALETTE IS MANUALLY GENERATED

/*	
	pPalette = MEM_p_Alloc( sizeof (*pPalette));
	memset( pPalette, 0, sizeof( *pPalette ));
	XGSetPaletteHeader( D3DPALETTE_256, pPalette, 0);

//	pPaletteBuffer= (void *)Gx8_AllocPaletteMemory();
//	IDirect3DPalette8_Register( pPalette, pPaletteBuffer );
*/
	

    hr = IDirect3DDevice8_CreatePalette(pst_SD->mp_D3DDevice, D3DPALETTE_256, &pPalette);
    ERR_X_Assert(hr==D3D_OK);

//	pPalette = 0;

	if(!pPalette)
	{
		//TiZ: this will cause an exception
		char *pInt = (char *)0xAAAABBBB;
		*pInt = 0x1234;
	}

    pul_Color = pst_Pal->pul_Color;
    if (pst_Pal->uc_Flags==TEX_uc_Palette16)
    {
        ULONG* firstUnusedColor=&myColors[16];
        ULONG Palette16Size = 16*sizeof( ULONG );

        memcpy( myColors, pul_Color, Palette16Size );
        memset( firstUnusedColor, 0, sizeof(myColors)-Palette16Size );
        pul_Color = myColors;
    }

    hr = IDirect3DPalette8_Lock( pPalette, &pColors, 0);
    ERR_X_Assert(hr==D3D_OK);
    for (i=0;i<256;i++)
    {
        ULONG value = Gx8_M_ConvertColor(pul_Color[i]);
        pColors[i]= Gx8_ConvertChrominancePixel( value );
    }
    hr = IDirect3DPalette8_Unlock(pPalette);
    ERR_X_Assert(hr==D3D_OK);

    // add the pal in the global array
    ERR_X_Assert(Index<MAX_PAL);
    g_aPal[Index] = pPalette;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_ReloadPalette(GDI_tdst_DisplayData *_pst_DD, ULONG ulPaletteIndex)
{
ULONG                   *pul_Color;
TEX_tdst_Palette        *pst_Pal;
Gx8_tdst_SpecificData   *pst_SD;

    pst_SD = GX8_M_SD( _pst_DD );

    pst_Pal = TEX_gst_GlobalList.dst_Palette + ulPaletteIndex;
    pul_Color = pst_Pal->pul_Color;

    {
        IDirect3DPalette8 *pPalette;
        HRESULT                 hr;
        D3DCOLOR *pColors;

        pPalette = g_aPal[ulPaletteIndex];
        if (pst_Pal->uc_Flags==TEX_uc_Palette16)
        {
            int i;
            ULONG MesCouleurs[256];
            memset(MesCouleurs,0,256*4);
            memcpy(MesCouleurs,pul_Color,16*4);
            hr = IDirect3DPalette8_Lock(pPalette,&pColors,D3DLOCK_READONLY);
            for (i=0;i<256;i++)
            {
                pColors[i]=Gx8_M_ConvertColor(MesCouleurs[i]);
            }
        }
        else
        {
            int i;
            hr = IDirect3DPalette8_Lock(pPalette,&pColors,D3DLOCK_READONLY);
            for (i=0;i<256;i++)
            {
                pColors[i]=Gx8_M_ConvertColor(pul_Color[i]);
            }
        }
        ERR_X_Assert(hr==D3D_OK);
        hr = IDirect3DPalette8_Unlock(pPalette);
        ERR_X_Assert(hr==D3D_OK);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_Set_Texture_Palette(GDI_tdst_DisplayData *_pst_DD, ULONG _ulTexNum, ULONG IdexTex, ULONG IndexPal)
{
}

/*
 =======================================================================================================================
    Stocke la texture en RAM pour chargement différé en VRAM £
    retourne 1 si la texture est stockée et ne doit pas être stocké immédiatement en VRAM retourne 0 sinon
 =======================================================================================================================
 */
LONG Gx8_l_Texture_Store
(
    GDI_tdst_DisplayData    *_pst_DD,
    TEX_tdst_Data           *_pst_TexData,
    TEX_tdst_File_Desc      *_pst_Tex,
    ULONG                   _ul_Texture
)
{
    return 0;
}

static void
_convertChrominance32( ULONG* buffer, int width, int height )
{
    const ULONG* end;

    for( end = buffer + width*height; buffer != end; buffer++ )
    {
        *buffer = Gx8_ConvertChrominancePixel( *buffer );
    }
}
void 
WritePixel (ULONG *buffer, ULONG pix, int width, int x, int y)
{
 	*(buffer+(width*y)+x) = pix;
}


void 
ReadPixel (ULONG *buffer, ULONG *pix, int width ,int x, int y)
{
	*pix = *(buffer+(width*y)+x);
}



static void
_convertToDot3( ULONG* buffer, int width, int height )
{
	typedef struct _pixel
	{
	BYTE red;
	BYTE blue;
	BYTE green;
	BYTE alpha;
	} pixel;

	ULONG* dstImage=NULL;
	ULONG* srcImage=NULL;
	int x,y;
	float dX, dY, nX, nY, nZ, oolen;
//	dstImage = (ULONG *) malloc (sizeof (ULONG) * height * width);
	dstImage = MEM_p_Alloc( sizeof(ULONG) * height * width );
	srcImage = MEM_p_Alloc( sizeof(ULONG) * height * width );

	for (y=0;y<height;y++)
	{
		for (x=0;x<width;x++)
		{
			ULONG red;
			ULONG green;
			ULONG blue;
			ULONG alpha;
			ULONG val;

			//ReadPixel(buffer,&val,width,x-1,height-y-2);//*(buffer+x+y*width);height-y-1
			ReadPixel(buffer,&val,width,x,y);

			alpha = (val & 0xFF000000) >>24;
			red = (val & 0x00FF0000) >>16;
			green = (val & 0x0000FF00) >>8;
			blue = (val & 0x000000FF);

			val= (alpha<<24) | (red<<16) | (green<<8) | blue;

			WritePixel(srcImage,val,width,x,y);

		}
	}


	for (y=0;y<height;y++)
	{
		for (x=0;x<width;x++)
		{
			ULONG red;
			ULONG green;
			ULONG blue;
			ULONG alpha;
			ULONG val;
			
			// Y Sobel
			ReadPixel(srcImage,&val,width,(x-1+width) % width, (y+1) % height);//*(buffer+x+y*width);
			red = (val & 0x00FF0000) >>16;
			dY  = ((float) red) / 255.0f * -1.0f;

            ReadPixel(srcImage, &val,width,   x   % width, (y+1) % height);
			red = (val & 0x00FF0000) >>16;
            dY += ((float) red) / 255.0f * -2.0f;
            
            ReadPixel(srcImage, &val,width, (x+1) % width, (y+1) % height);
			red = (val & 0x00FF0000) >>16;
            dY += ((float) red) / 255.0f * -1.0f;
            
            ReadPixel(srcImage, &val,width, (x-1+width) % width, (y-1+height) % height);
			red = (val & 0x00FF0000) >>16;
            dY += ((float) red) / 255.0f *  1.0f;
            
            ReadPixel(srcImage, &val,width,   x   % width, (y-1+height) % height);
			red = (val & 0x00FF0000) >>16;
            dY += ((float) red) / 255.0f *  2.0f;
            
            ReadPixel(srcImage, &val,width, (x+1) % width, (y-1+height) %height);
			red = (val & 0x00FF0000) >>16;
            dY += ((float) red) / 255.0f *  1.0f;
 
            // X Sobel
            ReadPixel(srcImage, &val,width, (x-1+width) % width, (y-1+height) % height);
            red = (val & 0x00FF0000) >>16;
			dX  = ((float) red) / 255.0f * -1.0f;
            
            ReadPixel(srcImage, &val,width, (x-1+width) % width,   y   % height);
            red = (val & 0x00FF0000) >>16;
			dX += ((float) red) / 255.0f * -2.0f;
            
            ReadPixel(srcImage, &val,width, (x-1+width) % width, (y+1) % height);
            red = (val & 0x00FF0000) >>16;
			dX += ((float) red) / 255.0f * -1.0f;
            
            ReadPixel(srcImage, &val,width, (x+1) % width, (y-1+height) % height);
            red = (val & 0x00FF0000) >>16;
			dX += ((float) red) / 255.0f *  1.0f;
            
            ReadPixel(srcImage, &val,width, (x+1) % width,   y   % height);
            red = (val & 0x00FF0000) >>16;
			dX += ((float) red) / 255.0f *  2.0f;
            
            ReadPixel(srcImage, &val,width, (x+1) % width, (y+1) % height);
            red = (val & 0x00FF0000) >>16;
			dX += ((float) red) / 255.0f *  1.0f;
 			
            // Cross Product
            nX = -dX;
            nY = -dY;//Inversion Ici ??
            nZ = 1;
 
            // Normalize
            oolen = 1.0f/((float) sqrt(nX*nX + nY*nY + nZ*nZ));
            nX *= oolen;
            nY *= oolen;
            nZ *= oolen;

			red = (ULONG)((nX+1.f)/2.f* 255.0f);
			green = (ULONG)((nY+1.f)/2.f* 255.0f);
			blue = (ULONG)((nZ+1.f)/2.f* 255.0f);

/* 
			alpha = (val & 0xFF000000) >>24;
			red = (val & 0x00FF0000) >>16;
			green = (val & 0x0000FF00) >>8;
			blue = (val & 0x000000FF);
*/
//			val= (alpha<<24) | (red<<16) | (green<<8) | blue;
			val= (alpha<<24) | (red<<16) | (green<<8) | blue;
			WritePixel(dstImage,val,width,x,y);
		}
	}
	memcpy( buffer, dstImage,sizeof (ULONG) * height * width  );
	MEM_Free( dstImage );
	MEM_Free( srcImage );
	//free( dstImage );


}

void
_MakeSpecularExponent32b( ULONG* buffer, int width, int height ,int exp)
{
	typedef struct _pixel
	{
	BYTE red;
	BYTE blue;
	BYTE green;
	BYTE alpha;
	} pixel;

	ULONG red;
	ULONG green;
	ULONG blue;
	ULONG alpha;
	ULONG val;
	
	ULONG* dstImage=NULL;
	int x,y;
//	dstImage = (ULONG *) malloc (sizeof (ULONG) * height * width);
	dstImage = MEM_p_Alloc( sizeof(ULONG) * height * width );

		for(y = 0; y < height; y++)
         {
			float NH = (float)pow( (height-1-y)/(float)(height-1), exp);
			for(x = 0; x < width; x++)
            {
				float NL = (float)x/(float)(width-1);
            
				red=(ULONG)(NL*255.f);
				green=(ULONG)(NL*255.f);
				blue=(ULONG)(NL*255.f);
				alpha=(ULONG)(NH*255.f);
//red = 255;
				val= (alpha<<24) | (red<<16) | (green<<8) | blue;
				//val= (alpha<<24) | (alpha<<16) | (alpha<<8) | alpha;
	
				WritePixel(dstImage,val,width,x,height-1-y);//height-1- si chargé de l'exterieur
				//WritePixel(dstImage,val,width,x,height-1-y);
           }
         }

	memcpy( buffer, dstImage,sizeof (ULONG) * height * width  );
	MEM_Free( dstImage );
	//free( dstImage );


}
/* NOISE
_MakeNoise32b( ULONG* buffer, int width, int height ,int exp)
{

	typedef struct _pixel
	{
	BYTE red;
	BYTE blue;
	BYTE green;
	BYTE alpha;
	} pixel;

	ULONG red;
	ULONG green;
	ULONG blue;
	ULONG alpha;
	ULONG val;
	
	ULONG* dstImage=NULL;
	int x,y;
//	dstImage = (ULONG *) malloc (sizeof (ULONG) * height * width);
	dstImage = MEM_p_Alloc( sizeof(ULONG) * height * width );

		for(y = 0; y < height; y++)
         {
			for(x = 0; x < width; x++)
            {
       
				red=(ULONG)255;//rnd(0,255);
				green=blue=alpha=red;

				val= (alpha<<24) | (red<<16) | (green<<8) | blue;
	
				//WritePixel(dstImage,val,width,x,height-1-y);//height-1- si chargé de l'exterieur
				WritePixel(dstImage,val,width,x,height-1);
           }
         }

	memcpy( buffer, dstImage,sizeof (ULONG) * height * width  );
	MEM_Free( dstImage );
	//free( dstImage );
}
*/

static IDirect3DTexture8*
_loadTexture32( GDI_tdst_DisplayData* _pst_DD, TEX_tdst_Data *_pst_TexData, TEX_tdst_File_Desc* _pst_Tex )
{
    IDirect3DTexture8* Gx8_TextureHard;
    HRESULT hr;
    DWORD dwFilter = D3DX_FILTER_LINEAR;
    Gx8_tdst_SpecificData* pst_SD = (Gx8_tdst_SpecificData*) _pst_DD->pv_SpecificData;
    int TX = _pst_TexData->w_Width;
    int TY = _pst_TexData->w_Height;
    void* pul_ConvertBuffer = _pst_Tex->p_Bitmap;
    IDirect3DSurface8* Gx8_Surface;
    RECT stSrcRect;
    ULONG l_MipmapFlag = _pst_Tex->st_Params.uw_Flags & (TEX_FP_MipmapUseAlpha | TEX_FP_MipmapUseColor | TEX_FP_MipmapKeepBorder);

    hr = IDirect3DDevice8_CreateTexture( pst_SD->mp_D3DDevice, TX, TY, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &Gx8_TextureHard );
    ERR_X_Assert( SUCCEEDED( hr ));

	TEX_Convert_32SwapRB(pul_ConvertBuffer, TX * TY);

/*	if ( _pst_Tex->st_Params.uw_Flags & TEX_FP_NormaleMap1 )
	{
		if ( Normalmap )
		_convertToDot3( pul_ConvertBuffer, TX, TY );
	}
	else*/
	{
		//TEX_Convert_32SwapRB(pul_ConvertBuffer, TX * TY);
	    _convertChrominance32( pul_ConvertBuffer, TX, TY );
	}


    IDirect3DTexture8_GetSurfaceLevel(Gx8_TextureHard, 0, &Gx8_Surface);
    stSrcRect.left=stSrcRect.top=0;
    stSrcRect.right = TX;
    stSrcRect.bottom = TY;
    D3DXLoadSurfaceFromMemory(Gx8_Surface,NULL,NULL,pul_ConvertBuffer,D3DFMT_LIN_A8R8G8B8,TX*4,NULL,&stSrcRect,
        D3DX_FILTER_NONE,0);
    // mip mapping
    if (l_MipmapFlag & TEX_FP_MipmapKeepBorder)
        dwFilter |= D3DX_FILTER_MIRROR;
    D3DXFilterTexture(Gx8_TextureHard,NULL,0,dwFilter);

    while((TX > 0) && (TY > 0))
    {
        _pst_DD->st_TexManager.l_MemoryTakenByLoading += TX * TY * 4L;
        TX >>= 1;
        TY >>= 1;
    }
	
	//FREE SURFACE!!!!!!!!
	if(Gx8_Surface)
	{
		IDirect3DSurface8_Release(Gx8_Surface);
		Gx8_Surface=0;
	}



    return Gx8_TextureHard;
}


static IDirect3DTexture8*
_loadTexture24( GDI_tdst_DisplayData* _pst_DD, TEX_tdst_Data *_pst_TexData, TEX_tdst_File_Desc* _pst_Tex, int i_MipmapLevel )
{
    IDirect3DTexture8* Gx8_TextureHard;
    DWORD dwFilter = D3DX_FILTER_LINEAR;
    HRESULT hr;
    Gx8_tdst_SpecificData* pst_SD = (Gx8_tdst_SpecificData*) _pst_DD->pv_SpecificData;
    int TX = _pst_TexData->w_Width;
    int TY = _pst_TexData->w_Height;
    void* pul_ConvertBuffer = _pst_Tex->p_Bitmap;
    RECT stSrcRect;
    ULONG l_MipmapFlag = _pst_Tex->st_Params.uw_Flags & (TEX_FP_MipmapUseAlpha | TEX_FP_MipmapUseColor | TEX_FP_MipmapKeepBorder);
    IDirect3DSurface8* Gx8_Surface;

    hr = IDirect3DDevice8_CreateTexture( pst_SD->mp_D3DDevice, TX, TY, i_MipmapLevel, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &Gx8_TextureHard );

    TEX_Convert_24To32SwapRB(pul_ConvertBuffer, TX * TY);

    IDirect3DTexture8_GetSurfaceLevel(Gx8_TextureHard, 0, &Gx8_Surface);
    stSrcRect.left=stSrcRect.top=0;
    stSrcRect.right = TX;
    stSrcRect.bottom = TY;
    D3DXLoadSurfaceFromMemory(Gx8_Surface,NULL,NULL,pul_ConvertBuffer,D3DFMT_LIN_A8R8G8B8,TX*4,NULL,&stSrcRect,
        D3DX_FILTER_NONE,0);
    // mip mapping
    if (l_MipmapFlag & TEX_FP_MipmapKeepBorder)
        dwFilter |= D3DX_FILTER_MIRROR;
    D3DXFilterTexture(Gx8_TextureHard,NULL,0,dwFilter);

    while((TX > 0) && (TY > 0))
    {
        _pst_DD->st_TexManager.l_MemoryTakenByLoading += TX * TY * 4L;
        TX >>= 1;
        TY >>= 1;
    }
	//FREE SURFACE!!!!!!!!
	if(Gx8_Surface)
	{
		IDirect3DSurface8_Release(Gx8_Surface);
		Gx8_Surface=0;
	}




    return Gx8_TextureHard;
}

static __inline unsigned int 
_rowPitch( int TX, D3DFORMAT format )
{
    switch( format )
    {
        case D3DFMT_DXT1:
            return 8*(TX/4);
            break;
        case D3DFMT_DXT2:       // same of DXT3
            return 16*(TX/4);
            break;
        case D3DFMT_DXT4:       // same of DXT5
            return 16*(TX/4);
            break;
        default:
            assert( false );
            return 0;
    }
}

static IDirect3DTexture8*
_loadTextureDtc( GDI_tdst_DisplayData* _pst_DD,
                 TEX_tdst_Data* _pst_TexData,
                 TEX_tdst_File_Desc* _pst_Tex,
                 D3DFORMAT format,
                 int i_MipmapLevel )
{
    IDirect3DTexture8* Gx8_TextureHard;
    DWORD dwFilter = D3DX_FILTER_LINEAR;
    HRESULT hr;
    Gx8_tdst_SpecificData* pst_SD = (Gx8_tdst_SpecificData*) _pst_DD->pv_SpecificData;
    int TX = _pst_TexData->w_Width;
    int TY = _pst_TexData->w_Height;
    void* pul_ConvertBuffer = _pst_Tex->p_Bitmap;
    RECT stSrcRect;
    ULONG l_MipmapFlag = _pst_Tex->st_Params.uw_Flags & (TEX_FP_MipmapUseAlpha | TEX_FP_MipmapUseColor | TEX_FP_MipmapKeepBorder);
    IDirect3DSurface8* Gx8_Surface;

    hr = IDirect3DDevice8_CreateTexture( pst_SD->mp_D3DDevice,
                                         TX, TY,
                                         i_MipmapLevel,
                                         0,
                                         format,
                                         D3DPOOL_MANAGED,
                                         &Gx8_TextureHard );
    ERR_X_Assert( SUCCEEDED( hr ));

    IDirect3DTexture8_GetSurfaceLevel(Gx8_TextureHard, 0, &Gx8_Surface);
    stSrcRect.left=stSrcRect.top=0;
    stSrcRect.right = TX;
    stSrcRect.bottom = TY;
    hr = D3DXLoadSurfaceFromMemory( Gx8_Surface,
                                    NULL,
                                    NULL,
                                    pul_ConvertBuffer,
                                    format,
                                    _rowPitch( TX, format ),
                                    NULL,
                                    &stSrcRect,
                                    D3DX_FILTER_NONE,
                                    0 );
    ERR_X_Assert( SUCCEEDED( hr ));
    // mip mapping
    if (l_MipmapFlag & TEX_FP_MipmapKeepBorder)
    {
        dwFilter |= D3DX_FILTER_MIRROR;
    }
    D3DXFilterTexture(Gx8_TextureHard,NULL,0,dwFilter);

    while((TX > 0) && (TY > 0))
    {
        _pst_DD->st_TexManager.l_MemoryTakenByLoading += TX * TY * 4L;
        TX >>= 1;
        TY >>= 1;
    }
	//FREE SURFACE!!!!!!!!
	if(Gx8_Surface)
	{
		hr = IDirect3DSurface8_Release(Gx8_Surface);
        ERR_X_Assert( SUCCEEDED( hr ));
		Gx8_Surface=0;
	}

    return Gx8_TextureHard;
}

static IDirect3DTexture8*
_loadTexturePalettized( GDI_tdst_DisplayData* _pst_DD, TEX_tdst_Data *_pst_TexData, TEX_tdst_File_Desc* _pst_Tex )
{
    IDirect3DTexture8* Gx8_TextureHard;
    HRESULT hr;
    Gx8_tdst_SpecificData* pst_SD = (Gx8_tdst_SpecificData*) _pst_DD->pv_SpecificData;
    int TX = _pst_TexData->w_Width;
    int TY = _pst_TexData->w_Height;
    void* pul_ConvertBuffer = _pst_Tex->p_Bitmap;
    RECT stSrcRect;
    IDirect3DSurface8* Gx8_Surface;
    void* p_Buf = NULL;

    if(_pst_Tex->uc_FinalBPP == 4)
    {
        p_Buf = MEM_p_Alloc( TX * TY );
        TEX_Convert_4To8( p_Buf, pul_ConvertBuffer, TX, TY );
        pul_ConvertBuffer = p_Buf;
    }

    hr = IDirect3DDevice8_CreateTexture( pst_SD->mp_D3DDevice, TX, TY, 1, 0, D3DFMT_P8, D3DPOOL_MANAGED, &Gx8_TextureHard );

    ERR_X_Assert( SUCCEEDED( hr ));

	IDirect3DTexture8_GetSurfaceLevel(Gx8_TextureHard, 0, &Gx8_Surface);
    stSrcRect.left=stSrcRect.top=0;
    stSrcRect.right = TX;
    stSrcRect.bottom = TY;
    

    D3DXLoadSurfaceFromMemory( Gx8_Surface,             // pDestSurface
                               NULL,                    // pDestPalette
                               NULL,                    // pDestRect
                               pul_ConvertBuffer,       // pSrcMemory
                               D3DFMT_P8,               // SrcFormat
                               TX,                      // SrcPitch
                               NULL,                    // pSrcPalette
                               &stSrcRect,              // pSrcRect
                               D3DX_FILTER_NONE,        // Filter
                               0);                      // ColorKey

    _pst_DD->st_TexManager.l_MemoryTakenByLoading += TX * TY;
    if (_pst_Tex->uc_FinalBPP == 4) MEM_Free( p_Buf );


	//FREE SURFACE!!!!!!!!
	if(Gx8_Surface)
	{
		IDirect3DSurface8_Release(Gx8_Surface);
	}
	
    return Gx8_TextureHard;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_Texture_Load
(
    GDI_tdst_DisplayData    *_pst_DD,
    TEX_tdst_Data           *_pst_TexData,
    TEX_tdst_File_Desc      *_pst_Tex,
    ULONG                   _ul_Texture
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                     c;
    int                     i_MipmapLevel;
    Gx8_tdst_SpecificData   *pst_SD;
    IDirect3DTexture8* Gx8_TextureHard;
	
    int TX = _pst_TexData->w_Width;
    int TY = _pst_TexData->w_Height;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	


    pst_SD = (Gx8_tdst_SpecificData *) _pst_DD->pv_SpecificData;

    if(_pst_Tex->st_Params.uw_Flags & TEX_FP_MipmapOn)//par la <----------
    {
        _pst_Tex->uw_FileFlags |= TEX_uw_Mipmap;
        c = (TX > TY) ? TX : TY;
        i_MipmapLevel = 0;
        while(c)
        {
            i_MipmapLevel++;
            c >>= 1;
        }
    }
    else
    {
        i_MipmapLevel = 1;
        _pst_Tex->uw_FileFlags &= ~TEX_uw_Mipmap;
    }

    _pst_TexData->uw_Flags = _pst_Tex->uw_FileFlags;

    c = _pst_Tex->st_Params.ul_Color;


    Gx8_TextureHard = NULL;

    switch( _pst_Tex->uc_FinalBPP )
    {
        case 32:
            Gx8_TextureHard = _loadTexture32(_pst_DD, _pst_TexData, _pst_Tex );
            break;
        case 24:
            Gx8_TextureHard = _loadTexture24(_pst_DD, _pst_TexData, _pst_Tex, i_MipmapLevel );
            break;

        case 0x81:  // Dxt1
            Gx8_TextureHard = _loadTextureDtc( _pst_DD, _pst_TexData, _pst_Tex, D3DFMT_DXT1, i_MipmapLevel );
            break;

        case 0x82:  // Dxt2
            Gx8_TextureHard = _loadTextureDtc( _pst_DD, _pst_TexData, _pst_Tex, D3DFMT_DXT2, i_MipmapLevel );
            break;

        case 0x83:  // Dxt3
            Gx8_TextureHard = _loadTextureDtc( _pst_DD, _pst_TexData, _pst_Tex, D3DFMT_DXT3, i_MipmapLevel );
            break;

        case 0x84:  // Dxt4
            Gx8_TextureHard = _loadTextureDtc( _pst_DD, _pst_TexData, _pst_Tex, D3DFMT_DXT4, i_MipmapLevel );
            break;

        case 0x85:  // Dxt1
            Gx8_TextureHard = _loadTextureDtc( _pst_DD, _pst_TexData, _pst_Tex, D3DFMT_DXT5, i_MipmapLevel );
            break;

        case 8:
            // fall through
        case 4:
            Gx8_TextureHard = _loadTexturePalettized(_pst_DD, _pst_TexData, _pst_Tex );
            break;
        default:
            // unknown texture depth.
            break;
    }



    {
        (IDirect3DTexture8 *) pst_SD->dul_Texture[_ul_Texture] = Gx8_TextureHard;
        //THIS WAS A BUG hr=IDirect3DTexture8_Release(Gx8_TextureHard);
    }
}

/*
 =======================================================================================================================
    Aim:    unload all texture from hardware memory
 =======================================================================================================================
 */
void Gx8_Texture_Unload(GDI_tdst_DisplayData *_pst_DD)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG                   *pul_Texture, *pul_LastTexture;
    Gx8_tdst_SpecificData   *pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_SD = (Gx8_tdst_SpecificData *) _pst_DD->pv_SpecificData;

    pul_Texture = (ULONG *) pst_SD->dul_Texture;
    pul_LastTexture = pul_Texture + pst_SD->l_NumberOfTextures;

	
    for(; pul_Texture < pul_LastTexture; pul_Texture++)
    {
        if(*pul_Texture)
        {
            IDirect3DTexture8_Release(((IDirect3DTexture8 *) * pul_Texture));
        }
    }

    if(pst_SD->dul_Texture)
    {
        MEM_Free(pst_SD->dul_Texture);
        pst_SD->dul_Texture = NULL;
    }

    /*
     * if(pst_SD->dul_TextureDeltaBlend) { MEM_Free(pst_SD->dul_TextureDeltaBlend);
     * pst_SD->dul_TextureDeltaBlend = NULL; }
     */
    pst_SD->l_NumberOfTextures = 0;


	//Release the PALETTES
	Gx8_ReleasePalette();

}



void Gx8_ReleasePalette(void)
{
	int i=0;
	//for( i = 0; i < TEX_gst_GlobalList.l_NumberOfPalettes; i++)
	for( i = 0; i < xb_NumberOfPalettes; i++)
	{
		if(g_aPal[i])
		{
			//D3DCOLOR *pPaletteBuffer;

			IDirect3DPalette8_Release(g_aPal[i]);

			// CODE FOR MANUAL PALETTE RELEASE (AVOID MEMORY WASTE)

			/*
			IDirect3DPalette8_Lock( g_aPal[i], &pPaletteBuffer, 0);
			Gx8_FreePaletteMemory((UINT)pPaletteBuffer);
			IDirect3DPalette8_Unlock( g_aPal[i] );
		
			IDirect3DResource8_BlockUntilNotBusy((D3DResource*)g_aPal[i]);
			MEM_Free(g_aPal[i]);
			*/

			g_aPal[i]=0;
		}
	}
	xb_NumberOfPalettes = 0;
}
