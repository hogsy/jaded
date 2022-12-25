//#################################################################################
//
// Public IOP based APIs - async IO, sound control, and timer callbacks.
//
// Copyright (c) 2003-2004 RAD Game Tools, Inc. 
//
//#################################################################################


#include "radbase.h"

RADDEFSTART

//#################################################################################
// Initialization routines
//
// This function starts the RAD_IOP library, it can be called multiple times. 
//   Only the first call sets the command handler to use, though.  You have
//   have to shut it down as many times as you start it up.
//
// The IOP library handles async reading, interrupt driven looping sound
//   playback and interrupt timer services.
//
// The cd/dvd system should be setup (sceCdInit) and the media type set before 
//   using these routines (if you are using the cd/dvd).
//
//#################################################################################


#define RAD_IOP_NEED_CORE0      1 
#define RAD_IOP_NEED_CORE1      2
#define RAD_IOP_NO_INIT_LIBSD  16

// You tell us how many file caches to use at startup time.  This can be zero
//   (if you don't call any IO routines) up to 16.  Each file cache is 4K.
//   You also have to tell us what command handler and the address of the iop
//   memory to use.  You usually allocate IOP memory with the sceSifAllocIopHeap
//   function.  Use RAD_IOPMemoryAmount() to get how much memory to allocate.
//   You can use negative numbers for the command_handler_num to have us init
//   the handler array first.  
S32 RAD_IOPStartUp( U32 num_file_caches, S32 command_handler_num, void * iop_memory, U32 sound_core_flags ); 

// This function returns how much memory to allocate for the RAD_IOPStartUp
//   call.  Bink only uses one sound buffer no matter how many sound buffers
//   are playing.  If your Binks don't have sound, you can pass zero for
//   this function.
U32 RAD_IOPMemoryAmount( U32 sound_core_flags );

// Closes the RAD library.
S32 RAD_IOPShutDown( void );



//#################################################################################
// Async IO API
//   This simple API lets you queue up async reads. 
//#################################################################################


#define RAD_IOP_STATUS_NONE       0
#define RAD_IOP_STATUS_QUEUED     1
#define RAD_IOP_STATUS_RUNNING    2

#define RAD_IOP_STATUS_COMPLETE   3  // when >= complete, then we're done in some fashion

#define RAD_IOP_STATUS_CANCELLED  3
#define RAD_IOP_STATUS_ERROR      4
#define RAD_IOP_STATUS_DONE       5


// Return the last drive status. (SCECdErNO, ... on a CD, otherwise err_no)...
S32 RAD_IOPGetIOStatus( void );



struct RAD_IOP_ASYNC_OPEN_DATA;
typedef void (RAD_IOPOPENCALLBACK)( struct RAD_IOP_ASYNC_OPEN_DATA * data );
typedef struct RAD_IOP_ASYNC_OPEN_DATA
{
  // set by RAD_IOP
  S32 volatile status;    // one of the RAD_IOP values
  S32 file_handle;
  U32 size;

  // you fill these out
  char const * name;
  U32 group;    // optional id that you can use to refer a set of asyncs at once
  RAD_IOPOPENCALLBACK * callback;  // can be zero for no callback (watch status field)

  // internally used
  struct RAD_IOP_ASYNC_OPEN_DATA * volatile prev;
  struct RAD_IOP_ASYNC_OPEN_DATA * volatile next;
} RAD_IOP_ASYNC_OPEN_DATA;


// Return file info for a filename asychronously - just put the name in the data structure
//  and then wait for the callback or the status to change.  Open files are highest priority,
//  they occur as soon as the current read/open is complete (or immediately if nothing is
//  happening).  If you already know the LSN of the file that you are reading off the
//  CD, then you don't need to open it - you can convert a LSN to a file handle with the
//  LSNtoFileHandle macro instead.
S32 RAD_IOPQueueAsyncOpenFile( RAD_IOP_ASYNC_OPEN_DATA * data );

#define LSNtoFileHandle( LSN ) ( (int) ( LSN ) + ( (int) 0x80000000 ) )


struct RAD_IOP_ASYNC_READ_DATA;
typedef void (RAD_IOPREADCALLBACK)( struct RAD_IOP_ASYNC_READ_DATA * data );
typedef struct RAD_IOP_ASYNC_READ_DATA
{
  // set by RAD_IOP
  S32 volatile status;    // one of the RAD_IOP values
  U32 volatile bytes_read;

  // you fill these out
  U32 offset;             // offset into the file (in bytes)
  U32 bytes_to_read;      // length in bytes (has to be divisible by 16)
  void * dest;            // ee address (has to be on a 16-byte aligned address)
  U32 group;              // optional id that you can use to cancel a set of asyncs at once
  S32 file_handle;        // file handle to read from (get from open)
  RAD_IOPREADCALLBACK * callback;  // can be zero for no callback (watch status field)

  // used internally by RAD_IOP  
  struct RAD_IOP_ASYNC_READ_DATA * volatile prev;
  struct RAD_IOP_ASYNC_READ_DATA * volatile next;
} RAD_IOP_ASYNC_READ_DATA;

// Schedule an asynchronous read at the front of the read queue (highest priority)
//   The data structure has to be available until the read completes.
S32 RAD_IOPQueueAsyncReadAtTop( RAD_IOP_ASYNC_READ_DATA * data );

// Schedule an asynchronous read at the end of the read queue (lowest priority)
//   The data structure has to be available until the read completes.
S32 RAD_IOPQueueAsyncReadAtBottom( RAD_IOP_ASYNC_READ_DATA * data );



// Cancel asynchronous reads (if data is already reading, we have to wait for that chunk to finish only)
//   all return the number of asyncs cancelled
S32 RAD_IOPCancelAsyncRead( RAD_IOP_ASYNC_READ_DATA * data ); 
S32 RAD_IOPCancelAsyncOpen( RAD_IOP_ASYNC_OPEN_DATA * data ); 
S32 RAD_IOPCancelGroupOfAsyncs( U32 group );
S32 RAD_IOPCancelFileAsyncs( S32 file_handle );
S32 RAD_IOPCancelAllAsyncs( void );

// Close an open file - stops all async IO for this file
S32 RAD_IOPCloseFile( S32 file_handle );


//#################################################################################
// Sound API
//   This API simply sets up a looping PCM buffer (there is one on each
//   sound chip core). It will then call your callback with an address 
//   and a length for you to stick or mix sound into.
//
// You should have loaded libsd.irx before using these calls!!
//#################################################################################


struct RAD_IOP_SOUND_DATA;

typedef S32 (RAD_IPOSOUNDCALLBACK)( struct RAD_IOP_SOUND_DATA * data );
  // if you return one, then you did the mixing, if you return zero, then
  //   you didn't do anything.
  
typedef struct RAD_IOP_SOUND_DATA
{
  // set by RAD_IOP
  S32 overwrite;     // if set, then the contents of mix_buf should be overwritten, otherwise
                     //   you must mix your audio into the contents of mix_addr
  void * address;
  U32 length;
  S32 volatile status;        // one of the RAD_IOP_STATUS
  
  // you fill these out
  U32 which_sound_core;
  RAD_IPOSOUNDCALLBACK * callback;

  // used internally by RAD_IOP  
  struct RAD_IOP_SOUND_DATA * volatile prev;
  struct RAD_IOP_SOUND_DATA * volatile next;
} RAD_IOP_SOUND_DATA;
 

// Start calling your callback at the mid and buffer end points
S32 RAD_IOPRegisterSoundCallback( RAD_IOP_SOUND_DATA * data );

// Stop calling your callback at the mid and buffer end points
S32 RAD_IOPUnregisterSoundCallback( RAD_IOP_SOUND_DATA * data );

// Start calling the sound callback.
S32 RAD_IOPStartSound( RAD_IOP_SOUND_DATA * data );

// Stop calling the sound callback.
S32 RAD_IOPPauseSound( RAD_IOP_SOUND_DATA * data );


// This function sets the volumes.  Remember that core 0 feeds into core 1, so
//   you need to set both core volumes if you are using core 0. Use -1 to not set
//   the volume for that channel.
S32 RAD_IOPHardwareVolumes( U32 which_core, S32 master_core_volume_L, S32 master_core_volume_R, S32 channel_volume_L, S32 channel_volume_R );



//#################################################################################
// Timer API
//   This API allows you create any number of timers that you want.  We create
//   a single IOP timer to drive all of the them and the resolution will be 
//   equal to the fastest timer instantiated.  These timers don't drift - if 
//   they fall behind due to system load, they will be called rapidly in a row
//   to catch back up.
//#################################################################################

// Simple function that returns a rolling ms count.
U32 RADTimerRead( void );


struct RAD_IOP_TIMER_DATA;
typedef void (RAD_IOPTIMERCALLBACK)( struct RAD_IOP_TIMER_DATA * timer );
typedef struct RAD_IOP_TIMER_DATA
{
  S32 volatile status;        // one of the RAD_IOP_STATUS

  // you fill this out
  RAD_IOPTIMERCALLBACK * callback;
  
  // used internally
  U32 period;
  U32 volatile last_time;  
  struct RAD_IOP_TIMER_DATA * volatile prev;
  struct RAD_IOP_TIMER_DATA * volatile next;
} RAD_IOP_TIMER_DATA;


// This function registers a timer callback.  The only field in the timer structure that
//   you need to fill in is the callback address.  Callbacks don't start until you call
//   RAD_IOPTimerStart.  The timer structure must remain until it is unregistered.
S32 RAD_IOPRegisterTimerCallback( RAD_IOP_TIMER_DATA * timer );

// This starts the timer.  The fastest timer determines resolution of the low level 
//   timer.  Returns the current timer resolution.
U32 RAD_IOPStartTimerCallback( RAD_IOP_TIMER_DATA * timer, U32 ms_between );

// Suspend the callbacks. Use RAD_IOPStartTimer to start again.
S32 RAD_IOPPauseTimer( RAD_IOP_TIMER_DATA * timer );

// Remove the timer from queue (returns new timer resolution)
U32 RAD_IOPUnregisterTimerCallback( RAD_IOP_TIMER_DATA * timer );

RADDEFEND

