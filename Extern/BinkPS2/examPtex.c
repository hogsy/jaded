//############################################################################
//##                                                                        ##
//##  Example of playing a Bink movie with textures.                        ##
//##                                                                        ##
//############################################################################

#include <shellCore.h>
#include <libdma.h>
#include <malloc.h>
#include <assert.h>

#include "bink.h"
#include "rad_iop.h" 

#define HOST_ROOT "/devel/libs/ps2/usr/local/sce/"

// don't forget the ";1" at the end of the file for cdrom files!
//static char const filename[] = "cdrom0:\\USR\\LOCAL\\48BEN.BIK;1";
static char const filename[] = "host0:d:\\media\\videos\\attract.bik";

// dma chain buffer for uploading textures and rendering
static sceDmaTag dma_chain[ 384 ];

#define OSRoundUp128B( _x ) ( ( (U32) ( _x ) + 128 - 1 ) & ~ ( 128 - 1 ) )

#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   448

#define PIXEL_SIZE        4    // for SCE_GS_PSMCT32 
#define PIXEL_SIZE_SHIFT  2    // bits to shift for PIXEL_SIZE
#define Z_BUFFER_SIZE     2    // for SCE_GS_PSMZ16

#define TEXTURE_ADDRESS_BIT_ALIGN 6
#define TEXTURE_MIN_HEIGHT_BITS   5

static void load_modules( void )
{
  int id;

  id = sceSifLoadModule( "host0:" HOST_ROOT "iop/modules/libsd.irx", 0, NULL );
  if ( id < 0 )
  {
    printf("  Couldn't load libsd.irx: %d\n", id );
    exit(-1);
  }
}

#define MAX_TILES_WIDE 3
#define MAX_TILES_HIGH 3

// structure we use to contain the info for titled textures

typedef struct TEXTURE_TILE_SET
{
  U32 num_wide;
  U32 num_high;

  U32 width;
  U32 height;
  
  U32 tile_width;
  U32 tile_height;

  U32 x[ MAX_TILES_WIDE ];
  U32 y[ MAX_TILES_HIGH ];
  U32 w[ MAX_TILES_WIDE ];
  U32 h[ MAX_TILES_HIGH ];

  U32 col_bits[ MAX_TILES_WIDE ];
  U32 row_bits[ MAX_TILES_HIGH ];
} TEXTURE_TILE_SET;


// this function finds the next power-of-2 bit level to use for tiled textures.
//   it's more complicated than it would seem to need to be as it optimizes the number
//   of total bit levels, while minimizing the both the number of textures and
//   the total wasted space.
static U32 get_next_bit_level( U32 val, U32 bit_levels_left, U32 minimum_bit_level )
{
  #define count_leading_zeros(count, x) \
    __asm__ ("plzcw %0,%1"              \
       : "=r" (count)                   \
       : "r" (x))

  U32 highest_bit, single_bit_value, multiple_bits_value, temp;
  
  // is our value already smaller than the minimum? if so, just return the minimum
  if ( val <= ( 1 << minimum_bit_level ) )
    return( minimum_bit_level );
  
  // find the top bit in the value
  highest_bit = val >> 1;
  count_leading_zeros( highest_bit, highest_bit );
  highest_bit = 31- highest_bit;
  
  // now we need to figure out how many bits we can really use - this is the minimum
  //   of the bit_levels_left, highest_bit, and the difference between the highest
  //   bit and the minimum_bit_level (we can ask for 3 bit_levels, but there many
  //   only be two bit levels between the highest in the number and the minimum
  //   that we can use.
  temp = highest_bit + 1;
  if ( bit_levels_left > temp )
    bit_levels_left = temp;

  temp = highest_bit - minimum_bit_level + 1;
  if ( bit_levels_left > temp )
    bit_levels_left = temp;
  
  // now calculate the value that 2* the highest_bit would be
  single_bit_value = 1 << ( highest_bit + 1 );

  // now calculate the value that if we had all of the bit levels 
  //  available, how big it could be 
  multiple_bits_value = ( ( 1 << bit_levels_left ) - 1 ) << ( highest_bit + 1 - bit_levels_left );
  
  // now, which is a better match - the single 2*the highest bit value,
  //   or the multiple bit values.  (one detail here - the multiple_bits_value
  //   could be smaller than val, so you'd get a negative number and then
  //   this comparison would fail. however, we are using unsigned ints
  //   so the negative number wraps around to a large number and the 
  //   comparison therefore just works.  
  if ( ( single_bit_value - val ) <= ( multiple_bits_value - val ) )
    ++highest_bit;

  return( highest_bit );
}


// function to figure out how to break up the video into a set of 
//   power-of-2 texture tiles.
static void setup_texture_info( TEXTURE_TILE_SET * textures, U32 width, U32 height )
{
  U32 bits, remaining, i, ofs;
  
  textures->width = width;
  textures->height = height;
  
  // calculate the sizes of the columns
  remaining = width;
  ofs = 0;
  textures->tile_width = 0;
  for ( i = 0 ; i <= MAX_TILES_WIDE ; )
  {
    bits = get_next_bit_level( remaining, MAX_TILES_WIDE - i, TEXTURE_ADDRESS_BIT_ALIGN );  
    
    textures->col_bits[ i ] = bits;
    textures->x[ i ] = ofs;
    bits = 1 << bits;
    textures->w[ i ] = ( remaining < bits ) ? remaining : bits;
    textures->tile_width += bits;

    ++i;
    if ( remaining <= bits )
      break;
    remaining -= bits;
    ofs += bits;
  }
  textures->num_wide = i;
  
  // calculate the sizes of the rows
  remaining = height;
  ofs = 0;
  textures->tile_height = 0;
  for ( i = 0 ; i <= MAX_TILES_HIGH ; )
  {
    bits = get_next_bit_level( remaining, MAX_TILES_HIGH - i, TEXTURE_MIN_HEIGHT_BITS ); 
    
    textures->row_bits[ i ] = bits;
    textures->y[ i ] = ofs;
    bits = 1 << bits;
    textures->h[ i ] = ( remaining < bits ) ? remaining : bits;
    textures->tile_height += bits;
    
    ++i;
    if ( remaining <= bits )
      break;
    remaining -= bits;
    ofs += bits;
  }
  textures->num_high = i;
}


//###########################################################################
// bunch of ps2 goofyness to upload memory rapidly to the GS and then draw it

#define UC_ADDR( _x ) ( (void *) ( ( (U32) ( _x ) ) | 0x20000000 ) )

// Address/Data helper
static inline void setGsAD( void *dest, u_int addr, u_long data )
{
  u_long * packet = (u_long*) dest;

  *packet++ = data;
  *packet++ = (u_long) addr;
}


// Upload the rgb32_buffer to the texture address on the GS
static void setup_texture_uploads( sceDmaTag ** dma_tag, TEXTURE_TILE_SET * textures, int GS_buffer, void * EE_buffer )
{
  U32 x, y;
  
  typedef struct BITBLTBUFDATA
  {
    sceGifTag       giftag0;
    sceGsBitbltbuf  bitbltbuf;
    long            bitbltbufaddr;
    sceGsTrxpos     trxpos;
    long            trxposaddr;
    sceGsTrxreg     trxreg;
    long            trxregaddr;
    sceGsTrxdir     trxdir;
    long            trxdiraddr;
  } BITBLTBUFDATA;

  // spin through all of our textures and upload each one
  for ( y = 0 ; y < textures->num_high; y++ )
  {
    for ( x = 0 ; x < textures->num_wide; x++ )
    {
      int size;
      u_long128 * image;
      BITBLTBUFDATA * blt_data;
      
      // Initialize our blt_data structure
      blt_data = (BITBLTBUFDATA*) sceDmaAddCont( dma_tag, sizeof(BITBLTBUFDATA) / sizeof(u_long128) );
      SCE_GIF_CLEAR_TAG( &blt_data->giftag0 );
      blt_data->giftag0.NLOOP = 4;
      blt_data->giftag0.NREG = 1;
      blt_data->giftag0.REGS0 = SCE_GIF_PACKED_AD;

      setGsAD( &blt_data->bitbltbuf, SCE_GS_BITBLTBUF, 
           SCE_GS_SET_BITBLTBUF( 0, 0, 0, GS_buffer, 
                                          1 << ( textures->col_bits[ x ] - TEXTURE_ADDRESS_BIT_ALIGN ), SCE_GS_PSMCT32 ) );
      setGsAD( &blt_data->trxpos, SCE_GS_TRXPOS,
           SCE_GS_SET_TRXPOS( 0, 0, 0 /* x */, 0 /* y */, 0 ) );
      setGsAD( &blt_data->trxreg, SCE_GS_TRXREG, SCE_GS_SET_TRXREG( 1 << textures->col_bits[ x ], 1 << textures->row_bits[ y ] ) );
      setGsAD( &blt_data->trxdir, SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR( 0 ) );

      // now send the bits of data themselves
      image = (u_long128 *) EE_buffer;
      size = 1 << ( textures->col_bits[ x ] + textures->row_bits[ y ] );
      GS_buffer += ( ( size + 63 ) >> TEXTURE_ADDRESS_BIT_ALIGN );
      EE_buffer = ( (char*) EE_buffer ) + ( size << PIXEL_SIZE_SHIFT );
      size >>= 2;

      // setup the actual pixel DMA uploads, being careful to 
      //   stay within the maximum 0x7fff U128 upload maximum.
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
  }
}


// Initialize our sprite data structure. We draw in swaths of ~32 wide
//  which is supposed to be a lot faster and all that.  This is pretty
//  easy since we draw in orthographic mode.
static void setup_texture_renders( sceDmaTag ** dma_tag, TEXTURE_TILE_SET * textures, int GS_buffer,
                                   S32 blit_x, S32 blit_y, U32 blit_width, U32 blit_height )
{
  typedef struct RENDERSPRITEPOINT
  {
    sceGsTex0       tex0;
    long            tex0addr;
    sceGsPrim       prim;
    long            primaddr;
  
    sceGsUv         uv0;
    long            uv0addr;
    sceGsXyz        xyz0;
    long            xyz0addr;
  
    sceGsUv         uv1;
    long            uv1addr;
    sceGsXyz        xyz1;
    long            xyz1addr;
  } RENDERSPRITEPOINT;
  
  typedef struct RENDERSPRITEHEADER
  {
    sceGifTag       giftag0;
  
    sceGsTexflush   texflush;
    long            texflushaddr;
    sceGsTex1       tex1;
    long            tex1addr;
  } RENDERSPRITEHEADER;

  RENDERSPRITEPOINT * sprite;
  RENDERSPRITEHEADER * sprite_hdr;
  U32 x, y;
  int poly_x, poly_y;
  int dma_length;

  // adjust the coordinates for the offset within the guard band
  poly_x = ( ( ( 4096 - SCREEN_WIDTH ) / 2 ) + blit_x ) * 16;
  poly_y = ( ( ( 4096 - SCREEN_HEIGHT ) / 2 ) + blit_y ) *16;

  dma_length = ( sizeof( RENDERSPRITEHEADER ) + 
                 sizeof( RENDERSPRITEPOINT ) * textures->num_wide * textures->num_high ) / sizeof( u_long128 );
                 
  // setup our start header
  sprite_hdr = (RENDERSPRITEHEADER*) sceDmaAddCont( dma_tag, dma_length );

  // Initialize our sprite header
  SCE_GIF_CLEAR_TAG( &sprite_hdr->giftag0 );
  sprite_hdr->giftag0.NLOOP = dma_length - 1;
  sprite_hdr->giftag0.NREG = 1;
  sprite_hdr->giftag0.REGS0 = SCE_GIF_PACKED_AD;
  sprite_hdr->giftag0.EOP = 1;

  setGsAD( &sprite_hdr->texflush, SCE_GS_TEXFLUSH, 0 );

  // set filter mode
  setGsAD( &sprite_hdr->tex1, SCE_GS_TEX1_1,
        SCE_GS_SET_TEX1(
            0,                  // LOD calculation method
            0,                  // Max MIP level
            0,                  // Filter when texture is expanded
            0,                  // Filter when texture is reduced
            0,                  // Base Address specification
            0,                  // LOD value L
            0 ) );              // LOD value K

  // sprite rects comes directly after the header (hence the + 1)
  sprite = (RENDERSPRITEPOINT *) ( sprite_hdr + 1 );

  // spin through all of our textures and create a sprite for each one
  for ( y = 0 ; y < textures->num_high; y++ )
  {
    for ( x = 0 ; x < textures->num_wide; x++ )
    {
      setGsAD( &sprite->tex0, SCE_GS_TEX0_1,
            SCE_GS_SET_TEX0(
                GS_buffer,                 // Base address of texture buffer
                1 << ( textures->col_bits[ x ] - TEXTURE_ADDRESS_BIT_ALIGN ), // Width of texture buffer
                SCE_GS_PSMCT32,            // Format in which texture pixels are saved
                textures->col_bits[ x ],   // log2(Texture Width)
                textures->row_bits[ y ],   // log2(Texture Height)
                0,                         // Texture color component
                1,                         // Texture function (0:modulate, 1:decal, ...)
                0,                         // CLUT base pointer
                0,                         // CLUT format
                0,                         // CLUT storage mode
                0,                         // CLUT entry offset
                0 ) );                     // CLUT buffer load control

      setGsAD( &sprite->prim, SCE_GS_PRIM,
          SCE_GS_SET_PRIM( SCE_GS_PRIM_SPRITE, 0, 1, 0, 0, 0, 1, 0, 0 ) );

      // send down the UVs and the screen coordinates
      setGsAD( &sprite->uv0, SCE_GS_UV, SCE_GS_SET_UV( 0, 0 ) );
      setGsAD( &sprite->xyz0, SCE_GS_XYZ2, SCE_GS_SET_XYZ2( poly_x + ( textures->x[ x ] * 16 * blit_width ) / textures->width, 
                                                            poly_y + ( textures->y[ y ] * 16 * blit_height ) / textures->height, 0 ) );

      setGsAD( &sprite->uv1, SCE_GS_UV, SCE_GS_SET_UV( textures->w[ x ] * 16, 
                                                       textures->h[ y ] * 16 ) );
      setGsAD( &sprite->xyz1, SCE_GS_XYZ2, SCE_GS_SET_XYZ2( poly_x + ( ( textures->x[ x ] + textures->w[ x ] ) * 16 * blit_width ) / textures->width, 
                                                            poly_y + ( ( textures->y[ y ] + textures->h[ y ] ) * 16 * blit_height ) / textures->height, 0 ) );
                                      
      GS_buffer += ( ( ( 1 << ( textures->col_bits[ x ] + textures->row_bits[ y ] ) ) + 63 ) >> TEXTURE_ADDRESS_BIT_ALIGN );
      ++sprite;
    }
  }
}

//######################################################################


// stupid function to just bounce a rectangle around the screen
static void adjust_texture_rect( int * xc, int * yc, int * wc, int * hc )
{
  static int ldir = 1;
  static int tdir = 1;
  static int rdir = 1;
  static int bdir = 1;
  int l, t, r, b;
  
  l = *xc;
  t = *yc;
  r = l + *wc;
  b = t + *hc;

  #define SPEED 5
  if ( ldir < 0 )
  { l -= SPEED; if ( l < -64 ) ldir = -ldir; }
  else
  { l += SPEED; if ( l > ( SCREEN_WIDTH - 64 ) ) ldir = -ldir; }
     
  if ( tdir < 0 )
  { t -= SPEED; if ( t < -64 ) tdir = -tdir; }
  else
  { t += SPEED; if ( t > ( SCREEN_HEIGHT - 64 ) ) tdir = -tdir; }

  r += (ldir + rdir ) * SPEED;
  if ( r < 64 ) 
  { rdir = -rdir; }
  else if ( r > ( SCREEN_WIDTH + 64 ) ) 
  { rdir = -rdir; }
     
  b += (tdir + bdir ) * SPEED;
  if ( b < 64 ) 
  {  bdir = -bdir; }
  else if ( b > ( SCREEN_WIDTH + 64 ) ) 
  {  bdir = -bdir; }
   
  if ( ( r - l ) < 32 ) 
  { ldir = -ldir; rdir = -rdir; } 
  
  if ( ( b - t ) < 32 ) 
  { tdir = -tdir; bdir = -bdir; } 
  
  *xc = l;
  *yc = t;
  *wc = r - l;
  *hc = b - t;
}

// this function copies over the current Bink frame from the internal YUV buffers into 
//   the rgb32 EE texture memory buffer.  later, the memory is copied from EE memory
//   into the GS for texturing.
static S32 do_texture_copies( HBINK bink, TEXTURE_TILE_SET * textures, void * EE_buffer )
{
  int x, y;

  // We copy the first texture separately to see if the blit is going to be skipped.
  //   If the blit is skipped, then we don't do any of the other subrects...
  if ( BinkCopyToBufferRect( bink, EE_buffer, 1 << ( textures->col_bits[ 0 ] + PIXEL_SIZE_SHIFT ), textures->h[ 0 ], 0, 0, 
                            textures->x[ 0 ], textures->y[ 0 ], textures->w[ 0 ], textures->h[ 0 ],
                            BINKSURFACE32 ) == 1 )
  {
    return( 1 );
  }
  
  EE_buffer = ( (char*) EE_buffer ) + ( 1 << ( textures->col_bits[ 0 ] + textures->row_bits[ 0 ] + PIXEL_SIZE_SHIFT ) );

  x = 1; 
  y = 0;

  while ( y < textures->num_high )
  {
    while ( x < textures->num_wide )
    {
      // copy the subrect - always use the BINKNOSKIP flag, because we have already 
      //   checked to see if this frame should be skipped in the first call to 
      //   BinkCopyToBufferRect above.
      BinkCopyToBufferRect( bink, EE_buffer, 1 << ( textures->col_bits[ x ] + PIXEL_SIZE_SHIFT ), textures->h[ y ], 0, 0, 
                            textures->x[ x ], textures->y[ y ], textures->w[ x ], textures->h[ y ],
                            BINKNOSKIP | BINKSURFACE32 );
      EE_buffer = ( (char*) EE_buffer ) + ( 1 << ( textures->col_bits[ x ] + textures->row_bits[ y ] + PIXEL_SIZE_SHIFT ) );
      ++x;
    }
    x = 0;
    ++y;
  }
  
  return( 0 );
}

static volatile int do_flip = 0;

// vblank callback - we only flip backbuffers when requested to with the
//   do_flip flag, and only once the dma is complete
static void vblank_callback( int data )
{
  if ( do_flip )
  {
    // has the dma completed?  Use 
    if ( ( *D2_CHCR & ( 1 << 8 ) )  == 0 )
    {
      --do_flip;
      shellDispSwapDblBuffer();
    }
  }
}


// displays frame playback information to the debug screen via printf
static void show_framerate( HBINK bink )
{
  U32 fps;
  BINKREALTIME rt;
  static U32 update = 0;

  BinkGetRealtime( bink, &rt, 0 );
  fps = rt.Frames * 10000 / rt.FramesTime;

  // display every 32 frames
  if ( ( ( update++ ) & 0x1f ) == 0 )
  {
    printf( "Fr: %4d, FPS: %d.%0d, Rate: %d, Video: %i%%, Audio %i%%, Blit: %i%%, IO: %i%%, Filled: %i%%\n",
             rt.FrameNum,
             fps / 10, fps % 10,
             rt.FramesDataRate,
             rt.FramesVideoDecompTime * 100 / rt.FramesTime,
             rt.FramesAudioDecompTime * 100 / rt.FramesTime,
             rt.FramesBlitTime * 100 / rt.FramesTime,
             rt.FramesReadTime * 100 / rt.FramesTime,
             ( ( rt.ReadBufferUsed + 1 ) * 100 ) / ( rt.ReadBufferSize + 1 )
           );
  }
}


// program entry point
int main( int argc, char *argv[ ] )
{
  void * iop_addr;
  U32 * rgb32_buffer = 0;
  HBINK bink;
  int texture_memory_base_offset;
  TEXTURE_TILE_SET textures;
  int blit_x, blit_y, blit_w, blit_h;

  printf( "Example started.\n" );

  sceSifInitRpc( 0 );

  /*
   * This function is required to be called for the PS2 Shell System
   * to operate. This function should be called before any other
   * memory allocation by the user. If the user wishes to leave some
   * memory for the default memory allocator, they can specify a
   * number of bytes with the reserve parameter. All other memory
   * will be taken over an managed by the core library.
   */
  shellMemoryInit( 16 * 1024 * 1024 );

  // Call this routine to initialise the DMA system.
  shellDmaInit();

  // Init file system
  shellFileInit( SHELLFILE_MEDIUM_HOST0,         // Where to serve files from: SHELLFILE_MEDIUM_HOST0 = host0.
                 1,                              // Flag to indicate an IOP reboot.
                 HOST_ROOT "iop/modules",        // String of directories to search for IOP modules, separated by semicolons.
                 NULL );                         // Parameter specific to choice of medium.

  // Initialise the display system and configure
  shellDispInit( SHELLDISP_INTERLACE_FIELD_AA,   // The interlace mode type.
                 SHELLDISP_NTSC,                 // Type of display.
                 SCREEN_WIDTH,                   // Width of the display buffer.
                 SCREEN_HEIGHT,                  // Height of the display buffer.
                 SCE_GS_PSMCT32,                 // Pixel format of the display buffer.
                 SCREEN_WIDTH,                   // Width of the draw buffer.
                 SCREEN_HEIGHT,                  // Height of the draw buffer.
                 SCE_GS_PSMCT32,                 // Pixel format of the draw buffer.
                 SCE_GS_PSMZ16 );                // Pixel format of the Z buffer

  shellDispSetClearColour( 0x3f, 0, 0x3f, 0 );
  shellDispAddVSyncCallback( vblank_callback, 0 );
  
  // Load the modules
  load_modules();

  // init all the systems Bink needs
  sceSifInitIopHeap();
  sceCdInit( SCECdINIT );
  sceCdMmode( ( sceCdGetDiskType() == SCECdPS2DVD ) ? SCECdDVD : SCECdCD );

  // allocate some IOP memory for Bink
  iop_addr = sceSifAllocIopHeap( RAD_IOPMemoryAmount( RAD_IOP_NEED_CORE1 ) );

  // start up the RAD reading and sound IOP module
  if ( RAD_IOPStartUp( 1, -1, iop_addr, RAD_IOP_NEED_CORE1 ) )
  {
        
    // use core1's pcm channel
    RAD_IOPHardwareVolumes( 1, 0x3fff, 0x3fff, 0x7fff, 0x7fff );
    BinkSoundUseRAD_IOP( 1 );

    bink = BinkOpen( filename, 0 );

    if ( !bink )
    {
      printf( "BinkOpen failed!\n" );
      return( -1 );
    }
   
    // setup the little array of textures info structures
    setup_texture_info( &textures, bink->Width, bink->Height );

    // setup initial texture sizes for animating
    blit_x = blit_y = 0;
    blit_w = bink->Width;
    blit_h = bink->Height;
    
    // calculate the address to use for our textures (first address is
    //   right after the 2 pixel buffers, and the z buffer)
    texture_memory_base_offset = ( SCREEN_WIDTH * SCREEN_HEIGHT * PIXEL_SIZE * 2 + SCREEN_WIDTH * SCREEN_HEIGHT * Z_BUFFER_SIZE ) >> ( TEXTURE_ADDRESS_BIT_ALIGN + PIXEL_SIZE_SHIFT );

    // allocate some EE memory to hold the rgb pixels
    rgb32_buffer = (U32*) shellMemoryAllocate( 128, OSRoundUp128B( textures.tile_width * textures.tile_height << PIXEL_SIZE_SHIFT ) );
    
    
    // Play and show the movie.
    while ( 1 )
    {
      int bink_wait;

      // has enough time passed since our last frame?
      bink_wait = BinkWait( bink );
    
      if ( !bink_wait )
      {
        // decompress a frame (this is compress is usually happening while 
        //   the previous frame is being uploaded)
        BinkDoFrame( bink );

        // wait until the previous dma is done.  Note that we don't wait for the 
        //   page flip, since we want to overlap the BinkCopyToBuffer
        //   with the waiting for vblank time (which will be from 0 to 16 ms).
        while ( *D2_CHCR & ( 1 << 8 ) ) {;}
        
        // Copy into our rgb buffer - we use the UC_ADDR macro to access our
        //   in uncached memory (so that we don't pollute our data cache
        //   with a bunch of pixels that are on the way out the door anyway).

        // note that we only update the textures, render and page flip, if the 
        //   BinkCopyToBuffer call didn't skip a frame!
        if ( do_texture_copies( bink, &textures, rgb32_buffer ) == 0 )
        {
          sceDmaTag * dma_tag;

          // now wait until the page has been flipped (you'll never wait here unless
          //   you have a really fast movie (like great than 90 fps), but we do it 
          //   just to be safe.
          while ( do_flip ) {;}

          // point to the start of our dma chain
          dma_tag = UC_ADDR( dma_chain );

          // add the dma commands to update each of our textures in GS memory
          setup_texture_uploads( &dma_tag, &textures, texture_memory_base_offset, rgb32_buffer );

          // add the dma commands to render our newly updated GS textures onto the screen
          setup_texture_renders( &dma_tag, &textures, texture_memory_base_offset, blit_x, blit_y, blit_w, blit_h );
          
          // mark the end of the dma chain
          sceDmaAddEnd( &dma_tag, 0, 0 );

          // make sure we didn't overflow our buffer
          assert( ( ( (char*) dma_tag ) - ( (char*) UC_ADDR( dma_chain ) ) ) <= sizeof( dma_chain ) );

          // start the dma up!
          sceDmaSend( sceDmaGetChan( SCE_DMA_GIF ), dma_chain );

          // change the blit rectangle each frame
          adjust_texture_rect( &blit_x, &blit_y, &blit_w, &blit_h );

          // request a page flip in the next VB (the VB watches for DMA completion first)
          ++do_flip;
        }

        // advance to the next frame 
        BinkNextFrame( bink );
        show_framerate( bink );
      }
      
      if ( bink->ReadError)
      {
        printf( "I/O error reading the Bink file!\n" );
        break;
      }
    }

    // never gets here, but this is how you shutdown...
    BinkClose( bink );

    RAD_IOPShutDown();
  }

  // free the memory we allocated previously
  sceSifFreeIopHeap( iop_addr );
 
  return( EXIT_SUCCESS );
}
