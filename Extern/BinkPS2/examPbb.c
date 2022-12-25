//############################################################################
//##                                                                        ##
//##  Example of playing a Bink movie directly onto the back buffer.        ##
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

static sceDmaTag dma_chain[ 64 ];

#define OSRoundUp128B( _x ) ( ( (U32) ( _x ) + 128 - 1 ) & ~ ( 128 - 1 ) )

#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   448


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


//######################################################################
// bunch of ps2 goofyness to upload memory rapidly to the back buffer

#define UC_ADDR(_x)             ((void *)(((u32)(_x)) | 0x20000000))

// Address/Data helper
static inline void setGsAD( void *dest, u_int addr, u_long data )
{
  u_long * packet = (u_long*) dest;

  *packet++ = data;
  *packet++ = (u_long) addr;
}

// Upload the rgb32_buffer to the GS
static void setup_backbuffer_upload( sceDmaTag ** dma_tag, int dest_GS_addr, U32 * rgb32_buffer, U32 buffer_width, U32 buffer_height )
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
  y = ( SCREEN_HEIGHT - buffer_height ) / 2;
  if ( x < 0 )
    x = 0;

  // Initialize our blt_data structure
  blt_data = (BITBLTBUFDATA*) sceDmaAddCont( dma_tag, sizeof(BITBLTBUFDATA) / sizeof(u_long128) );
  SCE_GIF_CLEAR_TAG( &blt_data->giftag0 );
  blt_data->giftag0.NLOOP = 4;
  blt_data->giftag0.NREG = 1;
  blt_data->giftag0.REGS0 = SCE_GIF_PACKED_AD;

  setGsAD( &blt_data->bitbltbuf, SCE_GS_BITBLTBUF, 
           SCE_GS_SET_BITBLTBUF( 0, 0, 0, dest_GS_addr, SCREEN_WIDTH / 64, SCE_GS_PSMCT32 ) );
  setGsAD( &blt_data->trxpos, SCE_GS_TRXPOS, SCE_GS_SET_TRXPOS( 0, 0, x, y, 0 ) );
  setGsAD( &blt_data->trxreg, SCE_GS_TRXREG, SCE_GS_SET_TRXREG( buffer_width, buffer_height ) );
  setGsAD( &blt_data->trxdir, SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR( 0 ) );


  // Break the image upload into chunks so we don't break the dma limit
  image = (u_long128*) rgb32_buffer;
  size = ( buffer_width * buffer_height ) >> 2;
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
//######################################################################



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
  int buffer = 0;
  U32 * rgb32_buffer = 0;
  HBINK bink;

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
   
    // allocate some memory to hold the rgb pixels
    rgb32_buffer = (U32*) shellMemoryAllocate( 128, OSRoundUp128B( bink->Width * bink->Height * 4 ) );

    // Play and show the movie.
    while ( 1 )
    {
      int bink_wait;

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
        if ( BinkCopyToBuffer( bink, UC_ADDR( rgb32_buffer ),
                               bink->Width * 4, bink->Height, 0, 0, BINKSURFACE32 ) == 0 )
        {
          sceDmaTag * dma_tag;

          // now wait until the page has been flipped (you'll never wait here unless
          //   you have a really fast movie (like great than 90 fps), but we do it 
          //   just to be safe.
          while ( do_flip ) {;}

          // point to the start of our dma chain
          dma_tag = UC_ADDR( dma_chain );

          // start uploading this buffer to the backbuffer - this happens in 
          //   the background, so you should make sure it finished before the 
          //   next call to BinkCopyToBuffer.
          
          // note that we only upload the buffer and page flip, if the BinkCopyToBuffer
          //   didn't skip a frame!
          setup_backbuffer_upload( &dma_tag, buffer ? 0 : ( ( SCREEN_WIDTH * SCREEN_HEIGHT ) / 64 ), 
                                             rgb32_buffer, bink->Width, bink->Height );

          // mark the end of the dma chain
          sceDmaAddEnd( &dma_tag, 0, 0 );

          // make sure we didn't overflow our buffer
          assert( ( ( (char*) dma_tag ) - ( (char*) UC_ADDR( dma_chain ) ) ) <= sizeof( dma_chain ) );

          // start the dma up!
          sceDmaSend( sceDmaGetChan( SCE_DMA_GIF ), dma_chain );

          buffer = !buffer;
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
