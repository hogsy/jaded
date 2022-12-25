//############################################################################
//##                                                                        ##
//##  D3DRAD3D.CPP                                                          ##
//##                                                                        ##
//##  API to use Direct3D for video textures                                ##
//##                                                                        ##
//##  Author: Jeff Roberts                                                  ##
//##                                                                        ##
//############################################################################
//##                                                                        ##
//##  Copyright (C) RAD Game Tools, Inc.                                    ##
//##                                                                        ##
//##  For technical support, contact RAD Game Tools at 425 - 893 - 4300.    ##
//##                                                                        ##
//############################################################################

#include <xtl.h>
#include <xgraphics.h>

#ifndef _XBOX
#error This file is only for Xbox.
#endif

#include "rad3d.h"

//############################################################################
//##                                                                        ##
//## Setup the array to convert from 3D image formats to D3D formats.       ##
//##                                                                        ##
//############################################################################

static S32 Built_tables = 0;
static D3DFORMAT D3D_surface_type [ RAD3DSURFACECOUNT ];
static U32 Pixel_info [ RAD3DSURFACECOUNT ];

static void Setup_surface_array( void )
{
  if ( !Built_tables )
  {

    D3D_surface_type[ RAD3DSURFACE32 ] = D3DFMT_LIN_X8R8G8B8;
    D3D_surface_type[ RAD3DSURFACE32A ] = D3DFMT_LIN_A8R8G8B8;
    D3D_surface_type[ RAD3DSURFACE555 ] = D3DFMT_LIN_R6G5B5;
    D3D_surface_type[ RAD3DSURFACE565 ] = D3DFMT_LIN_R5G6B5;
    D3D_surface_type[ RAD3DSURFACE5551 ] = D3DFMT_LIN_A1R5G5B5;
    D3D_surface_type[ RAD3DSURFACE4444 ] = D3DFMT_LIN_A4R4G4B4;
    D3D_surface_type[ RAD3DSURFACEYUY2 ] = D3DFMT_YUY2;

    Pixel_info[ RAD3DSURFACE32 ] = 4;
    Pixel_info[ RAD3DSURFACE32A ] = 0x80000000|4;
    Pixel_info[ RAD3DSURFACE555 ] = 2;
    Pixel_info[ RAD3DSURFACE565 ] = 2;
    Pixel_info[ RAD3DSURFACE5551 ] = 0x80000000|2;
    Pixel_info[ RAD3DSURFACE4444 ] = 0x80000000|2;
    Pixel_info[ RAD3DSURFACEYUY2 ] = 2;

    Built_tables = 1;
  }
}


//############################################################################
//##                                                                        ##
//## Begin a D3D frame.                                                     ##
//##                                                                        ##
//############################################################################

RADCFUNC void Start_RAD_3D_frame( HRAD3D rad_3d )
{
  if ( rad_3d )
  {
    //
    // Clear the screen.
    //

    rad_3d->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);

    //
    // Start the scene
    //

    rad_3d->BeginScene();


    // possible states that you may have to reset depending on the state of the renderer in your game

/*    rad_3d->SetPixelShader( NULL );
    rad_3d->SetRenderState( D3DRS_ZENABLE,          FALSE );
    rad_3d->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    rad_3d->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
    rad_3d->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );
    rad_3d->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
    rad_3d->SetRenderState( D3DRS_ALPHAREF,         0x08 );
    rad_3d->SetRenderState( D3DRS_ALPHAFUNC,        D3DCMP_GREATEREQUAL );
    rad_3d->SetRenderState( D3DRS_FILLMODE,         D3DFILL_SOLID );
    rad_3d->SetRenderState( D3DRS_CULLMODE,         D3DCULL_CCW );
    rad_3d->SetRenderState( D3DRS_FOGENABLE,        FALSE );
    rad_3d->SetRenderState( D3DRS_STENCILENABLE,    FALSE );

    rad_3d->SetRenderState( D3DRS_EDGEANTIALIAS,    FALSE );
    rad_3d->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
    rad_3d->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
    rad_3d->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    rad_3d->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    rad_3d->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    rad_3d->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    rad_3d->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
    rad_3d->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    rad_3d->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    rad_3d->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    rad_3d->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );*/


  }
}


//############################################################################
//##                                                                        ##
//## End a D3D frame.                                                       ##
//##                                                                        ##
//############################################################################

RADCFUNC void End_RAD_3D_frame( HRAD3D rad_3d )
{
  if ( rad_3d )
  {
    //
    // End the rendering.
    //

    rad_3d->EndScene();

    rad_3d->Present(0, 0, 0, 0);
  }
}


//############################################################################
//##                                                                        ##
//## Create a new D3D texture.                                              ##
//##                                                                        ##
//############################################################################

static LPDIRECT3DTEXTURE8 Create_texture  ( LPDIRECT3DDEVICE8 d3d_device,
                                            U32 format,
                                            U32 width,
                                            U32 height )
{
  LPDIRECT3DTEXTURE8 texture_surface;

  //
  // Create a surface for our texture with the DirectDraw handle.
  //

  if ( SUCCEEDED( d3d_device->CreateTexture( width, height, 1, 0,
                                             D3D_surface_type[ format ],
                                             0, &texture_surface ) ) )
  {
    return( texture_surface );
  }

  return( 0 );
}


//############################################################################
//##                                                                        ##
//## Structure to contain a RAD 3D image.                                   ##
//##                                                                        ##
//############################################################################

typedef struct RAD3DIMAGE
{
  U32 width;
  U32 height;
  S32 alpha_pixels;
  U32 pixel_size;
  U32 rad_surface_type;
  LPDIRECT3DTEXTURE8 texture;
  LPDIRECT3DDEVICE8 direct_3d_device;
} RAD3DIMAGE;


//############################################################################
//##                                                                        ##
//## Open a RAD 3D image (a data structure to blit an image through D3D).   ##
//##                                                                        ##
//############################################################################

RADCFUNC HRAD3DIMAGE Open_RAD_3D_image( LPDIRECT3DDEVICE8 d3d_device,
                                        U32 width,
                                        U32 height,
                                        U32 rad3d_surface_format )
{
  HRAD3DIMAGE rad_image;
  U32 pixel_size;

  Setup_surface_array();

  //
  // Calculate the pixel size
  //

  pixel_size = Pixel_info[ rad3d_surface_format ] & 255;

  //
  // Allocate enough memory for a RAD image, a list of textures and a buffer.
  //

  rad_image = ( HRAD3DIMAGE ) malloc( sizeof( RAD3DIMAGE ) );

  if ( rad_image == 0 )
  {
    return( 0 );
  }

  //
  // Set all the variables in our new structure.
  //

  rad_image->direct_3d_device = d3d_device;
  rad_image->width = width;
  rad_image->height = height;
  rad_image->alpha_pixels = (Pixel_info[rad3d_surface_format]&0x80000000)?1:0;
  rad_image->pixel_size = pixel_size;
  rad_image->rad_surface_type = rad3d_surface_format;

  rad_image->texture = Create_texture( rad_image->direct_3d_device,
                                       rad_image->rad_surface_type,
                                       rad_image->width,
                                       rad_image->height );

  if ( rad_image->texture == 0 )
  {
    free( rad_image );
    return( 0 );
  }

  return( rad_image );
}



//############################################################################
//##                                                                        ##
//## Closes a RAD 3D image (frees textures and memory).                     ##
//##                                                                        ##
//############################################################################

RADCFUNC void Close_RAD_3D_image( HRAD3DIMAGE rad_image )
{
  if ( rad_image )
  {
    if ( rad_image->texture )
    {
      rad_image->texture->Release( );
      rad_image->texture = 0;
    }
    
    //
    // Free our memory.
    //

	//rad_image->direct_3d_device->SetRenderState( D3DRS_YUVENABLE,FALSE);

    free( rad_image );
  }
}


//############################################################################
//##                                                                        ##
//## Lock a RAD 3D image and return the buffer address and pitch.           ##
//##                                                                        ##
//############################################################################

RADCFUNC S32 Lock_RAD_3D_image( HRAD3DIMAGE rad_image,
                                void* out_pixel_buffer,
                                U32* out_buffer_pitch,
                                U32* out_surface_type )
{
  if ( rad_image == 0 )
  {
    return( 0 );
  }

  //
  // Lock the surface
  //

  D3DLOCKED_RECT lock_rect;

  if ( ! SUCCEEDED( rad_image->texture->LockRect( 0,
                                                  &lock_rect,
                                                  0,
                                                  0 ) ) )
  {
    return( 0 );
  }

  //
  // Fill the variables that were requested.
  //

  if ( out_pixel_buffer )
  {
    *( void** )out_pixel_buffer = lock_rect.pBits;
  }

  if ( out_buffer_pitch )
  {
    *out_buffer_pitch = lock_rect.Pitch;
  }

  if ( out_surface_type )
  {
    *out_surface_type = rad_image->rad_surface_type;
  }

  return( 1 );
}


//############################################################################
//##                                                                        ##
//## Unlock a RAD 3D image                                                  ##
//##                                                                        ##
//############################################################################

RADCFUNC void Unlock_RAD_3D_image( HRAD3DIMAGE rad_image )
{
  if ( rad_image == 0)
  {
    return;
  }

  //
  // Unlock the DirectX texture.
  //

  rad_image->texture->UnlockRect( 0 );
}


//
// Structure to contain the default vertex shader that we use.
//

typedef struct RADVERTEX
{
  F32 sx,sy,sz;
  F32 rhw;
  U32 color;
  F32 tu,tv;
} RADVERTEX;


//############################################################################
//##                                                                        ##
//## Submit the vertices for one texture.                                   ##
//##                                                                        ##
//############################################################################

static void Submit_vertices( LPDIRECT3DDEVICE8 d3d_device,
                             F32 dest_x,
                             F32 dest_y,
                             F32 scale_x,
                             F32 scale_y,
                             S32 width,
                             S32 height,
                             F32 alpha_level )
{
  RADVERTEX vertices[ 4 ];

  //
  // Setup up the vertices.
  //

  vertices[ 0 ].sx = dest_x;
  vertices[ 0 ].sy = dest_y;
  vertices[ 0 ].sz = 0.0F;
  vertices[ 0 ].rhw = 0.0F;
  vertices[ 0 ].color = ( ( S32 ) ( ( alpha_level * 255.0F ) ) << 24 ) | 0xffffff;
  vertices[ 0 ].tu = -0.5F;
  vertices[ 0 ].tv = -0.5F;

  vertices[ 1 ] = vertices[ 0 ];

  vertices[ 1 ].sx = dest_x + ( ( ( F32 ) width ) * scale_x );
  vertices[ 1 ].tu = ( ( F32 ) width) - 0.5F;

  vertices[ 2 ] = vertices[0];

  vertices[ 2 ].sy = dest_y + ( ( ( F32 ) height ) * scale_y );
  vertices[ 2 ].tv = ( ( F32 ) height) - 0.5F;

  vertices[ 3 ] = vertices[ 1 ];

  vertices[ 3 ].sy = vertices[ 2 ].sy;
  vertices[ 3 ].tv = ( ( F32 ) height) - 0.5F;

  //
  // Draw the vertices.
  //

  d3d_device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP,
                               2,
                               vertices,
                               sizeof( RADVERTEX ) );
}


//############################################################################
//##                                                                        ##
//## Submit the lines to outline a texture.                                 ##
//##                                                                        ##
//############################################################################

static void Submit_lines( LPDIRECT3DDEVICE8 d3d_device,
                          F32 dest_x,
                          F32 dest_y,
                          F32 scale_x,
                          F32 scale_y,
                          S32 width,
                          S32 height )
{
  RADVERTEX points[ 12 ];

  //
  // Setup the line coordinates.
  //

  points[ 0 ].sx = dest_x;
  points[ 0 ].sy = dest_y;
  points[ 0 ].sz = 0.0F;
  points[ 0 ].rhw = 1.0F;
  points[ 0 ].color = 0xffffffff;
  points[ 0 ].tu = 0.0F;
  points[ 0 ].tv = 0.0F;

  points[ 1 ] = points[ 0 ];

  points[ 1 ].sx = dest_x + ( ( ( F32 ) width ) * scale_x );
  points[ 1 ].sy = dest_y;

  points[ 2 ] = points[ 1 ];
  points[ 3 ] = points[ 1 ];

  points[ 3 ].sy = dest_y + ( ( ( F32 ) height ) * scale_y );

  points[ 4 ] = points[ 3 ];
  points[ 5 ] = points[ 3 ];

  points[ 5 ].sx = dest_x;

  points[ 6 ] = points[ 5 ];
  points[ 7 ] = points[ 0 ];

  points[ 8 ] = points[ 0 ];
  points[ 9 ] = points[ 3 ];

  points[ 10 ] = points[ 2 ];
  points[ 11 ] = points[ 6 ];

  points[ 8 ].color = 0xffff0000;
  points[ 9 ].color = 0xffff0000;
  points[ 10 ].color = 0xffff0000;
  points[ 11 ].color = 0xffff0000;

  //
  // Turn off texturing.
  //

  d3d_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
  d3d_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );

  //
  // Draw the lines.
  //

  d3d_device->DrawPrimitiveUP( D3DPT_LINELIST,
                               6,
                               points,
                               sizeof( RADVERTEX ) );
}

//############################################################################
//##                                                                        ##
//## Blit a 3D image onto the render target.                                ##
//##                                                                        ##
//############################################################################

RADCFUNC void Blit_RAD_3D_image( HRAD3DIMAGE rad_image,
                                 F32 x_offset,
                                 F32 y_offset,
                                 F32 x_scale,
                                 F32 y_scale,
                                 F32 alpha_level )
{
  if ( rad_image == 0 )
  {
    return;
  }

  //
  // If alpha is disabled and there is no texture alpha, turn alpha off.
  //

  if ( ( alpha_level >= (1.0F-0.0001) ) && ( ! rad_image->alpha_pixels ) )
  {
    rad_image->direct_3d_device->SetRenderState( D3DRS_ALPHABLENDENABLE, 0 );
    rad_image->direct_3d_device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
    rad_image->direct_3d_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    rad_image->direct_3d_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
  }
  else
  {
    rad_image->direct_3d_device->SetRenderState( D3DRS_ALPHABLENDENABLE, 1 );
    rad_image->direct_3d_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    rad_image->direct_3d_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    rad_image->direct_3d_device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
    rad_image->direct_3d_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    rad_image->direct_3d_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    rad_image->direct_3d_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
  }

  //
  // Handle RGB vs. YUV textures.
  //

  rad_image->direct_3d_device->SetRenderState( D3DRS_YUVENABLE, 
                                 ( rad_image->rad_surface_type == RAD3DSURFACEYUY2 ) ? 1 : 0 );

  //
  // Turn on clamping so that the linear textures work
  //

  rad_image->direct_3d_device->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
  rad_image->direct_3d_device->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

  //
  // Turn on texture filtering when scaling...
  //

  if ( ( x_scale < 1.001F ) && ( x_scale > 0.999F ) &&
       ( y_scale < 1.001F ) && ( y_scale > 0.999F ) )
  {
    rad_image->direct_3d_device->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
    rad_image->direct_3d_device->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
  }
  else
  {
    rad_image->direct_3d_device->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    rad_image->direct_3d_device->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
  }

  //
  // Use a default vertex shader
  //

  rad_image->direct_3d_device->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );

  //
  // Select the texture.
  //

  rad_image->direct_3d_device->SetTexture( 0, rad_image->texture );

  //
  // Submit the vertices.
  //

  Submit_vertices( rad_image->direct_3d_device,
                   x_offset,
                   y_offset,
                   x_scale,
                   y_scale,
                   rad_image->width,
                   rad_image->height,
                   alpha_level );
}


//############################################################################
//##                                                                        ##
//## Draw the edges of each texture for debugging purposes.                 ##
//##                                                                        ##
//############################################################################

RADCFUNC void Draw_lines_RAD_3D_image( HRAD3DIMAGE rad_image,
                                       F32 x_offset,
                                       F32 y_offset,
                                       F32 x_scale,
                                       F32 y_scale )
{
  if ( rad_image == 0 )
  {
    return;
  }

  //
  // Submit the lines.
  //

  Submit_lines( rad_image->direct_3d_device,
                x_offset,
                y_offset,
                x_scale,
                y_scale,
                rad_image->width,
                rad_image->height );
}
