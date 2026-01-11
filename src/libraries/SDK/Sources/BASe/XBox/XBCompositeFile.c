/**
 * @file BIGcompositeFile.c
 * @author Massimiliano Pagani
 *
 * The purpouse of this module is to handle the big file split over several
 * files.
 */

#include "Precomp.h"

#if !defined( _XBOX )
#  error this is an XBox only file. Go away you misbeliever
#endif

#include "XBCompositeFile.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/MEMory/MEM.h"

#define MAX_SPLITS  1

// to enable read error injection uncomment the following line
// #define ENABLE_ERROR_INJECTION

// to enable read logging uncomment the following line
// #define ENABLE_READ_LOGGING

typedef struct
{
    HANDLE handle;
    __int64 offset;     // just in case this is growing larger than 4G
    DWORD size;
} SingleSplit;

struct XBCompositeFile
{
    int currentSplit;
    SingleSplit splits[MAX_SPLITS];
    XBCompositeFile_ErrorHandler* errorHandler;
};

struct XBStatus
{
    int currentSplit;
    LARGE_INTEGER offset;
};

static void
XBCompositeFile_getStatus( XBCompositeFile* file, struct XBStatus* status )
{
    static const LARGE_INTEGER zero = {0};        // this is all zero by ANSI C.
    ERR_X_Assert( status != NULL );

    status->currentSplit = file->currentSplit;

    SetFilePointerEx( file->splits[file->currentSplit].handle,
                      zero,
                      &status->offset,
                      FILE_CURRENT );
}

static BOOL
XBCompositeFile_setStatus( XBCompositeFile* file, const struct XBStatus* status )
{
    ERR_X_Assert( status != NULL );

    file->currentSplit = status->currentSplit;
    return SetFilePointerEx( file->splits[file->currentSplit].handle,
                             status->offset,
                             NULL,
                             FILE_BEGIN );
}

XBCompositeFile*
XBCompositeFile_Open( const char* fileName,
                      DWORD flagsAndAttributes,
                      XBCompositeFile_ErrorHandler* errorHandler )
{
    int i;
    char splitName[260];
    XBCompositeFile* file;
    int lastChar;
    __int64 offset;
        
    ERR_X_Assert( fileName != NULL );
    
    file = MEM_p_Alloc( sizeof( XBCompositeFile ));
    
    strcpy( splitName, fileName );
    lastChar = strlen( splitName ) - 1;
    
    offset = 0;
    for( i=0; i<MAX_SPLITS; ++i )
    {
		do
		{    
			file->splits[i].handle = CreateFile( splitName,
												GENERIC_READ,
												FILE_SHARE_READ,
												NULL,
												OPEN_EXISTING,
												flagsAndAttributes,
												NULL );
			if( file->splits[i].handle == INVALID_HANDLE_VALUE )
			{
				//break;
				Gx8_FileError();
			}
		}while(file->splits[i].handle == INVALID_HANDLE_VALUE);


        file->splits[i].offset = offset;
        file->splits[i].size = GetFileSize( file->splits[i].handle, NULL );
        ERR_X_Assert( file->splits[i].size != -1 );
        offset += file->splits[i].size;
        
        splitName[lastChar]++;
    }
    file->currentSplit = 0;
    file->errorHandler = errorHandler;
#if defined( ENABLE_READ_LOGGING )
    {
        char buffer[256];
        sprintf( buffer, "XBCompositeFile_Open -> %08x\n", file );
        OutputDebugString( buffer );
    }
#endif
    return file;
}

void
XBCompositeFile_Close( XBCompositeFile* compositeFile )
{
    int i;
    
    ERR_X_Assert( compositeFile != NULL );

    for( i=0; i<MAX_SPLITS; ++i )
    {
        if( compositeFile->splits[i].handle == INVALID_HANDLE_VALUE )
        {
            break;
        }
        CloseHandle( compositeFile->splits[i].handle );
    }
    MEM_Free( compositeFile );
}

static int
_getFirstSplit( XBCompositeFile* file,
                DWORD bytesToRead,
                OVERLAPPED* overlapped )
{
    int i;
    BOOL found;
    
    SingleSplit* split = &file->splits[ 0 ];
    found = FALSE;
    // find first split
    for( i=0; i<MAX_SPLITS; ++i )
    {
        LARGE_INTEGER offset;
        
        ERR_X_Assert( split->handle != INVALID_HANDLE_VALUE );
        
        offset.LowPart = overlapped->Offset;
        offset.HighPart = overlapped->OffsetHigh;
        
        found = offset.QuadPart >= split->offset &&
                offset.QuadPart < split->offset + split->size;
        if( found )
        {
            break;
        }
        split++;
    }
    ERR_X_Assert( found );  // if ! found, then attempt to read after end
    return i;
}

static BOOL
_isLastSplit( const SingleSplit* file,
              DWORD bytesToRead,
              const OVERLAPPED* overlapped )
{
    LARGE_INTEGER offset;

    offset.LowPart = overlapped->Offset;
    offset.HighPart = overlapped->OffsetHigh;

    return offset.QuadPart + bytesToRead <= file->offset+file->size;
}

static void
_computeOverlapped( SingleSplit* split,
                    const OVERLAPPED* virtual,
                    LARGE_INTEGER absoluteOffset,
                    OVERLAPPED* physical )
{
    LARGE_INTEGER physicalOffset;
    
    *physical = *virtual;
    
    physicalOffset.LowPart = physical->Offset;
    physicalOffset.HighPart = physical->OffsetHigh;

    physicalOffset.QuadPart = absoluteOffset.QuadPart - split->offset;

    physical->Offset = physicalOffset.LowPart;
    physical->OffsetHigh = physicalOffset.HighPart;
}

    
static BOOL
_asynchRead( XBCompositeFile* file,
             void* buffer,
             DWORD bytesToRead,
             DWORD* bytesRead,
             OVERLAPPED* overlapped )
{
    int i;
    BOOL ok = TRUE;
    DWORD savedBytesToRead = bytesToRead;
    BOOL firstReadPending = false;
    LARGE_INTEGER absoluteOffset;
    
    absoluteOffset.LowPart = overlapped->Offset;
    absoluteOffset.HighPart = overlapped->OffsetHigh;
    
    ERR_X_Assert( overlapped->OffsetHigh == 0 );
    
    i = _getFirstSplit( file, bytesToRead, overlapped );
    
    do
    {
        if( _isLastSplit( &file->splits[i], bytesToRead, overlapped ))
        {
            OVERLAPPED thisOverlapped;

            _computeOverlapped( &file->splits[i], overlapped, absoluteOffset, &thisOverlapped );
            ok = ReadFile( file->splits[i].handle,
                           buffer,
                           bytesToRead,
                           bytesRead,
                           &thisOverlapped );
            break;
        }
        else
        {
            OVERLAPPED thisOverlapped;
            DWORD thisBytesToRead;
            
            _computeOverlapped( &file->splits[i], overlapped, absoluteOffset, &thisOverlapped );
            ERR_X_Assert( file->splits[i].size > thisOverlapped.Offset );
            thisBytesToRead = file->splits[i].size - thisOverlapped.Offset;
            thisOverlapped.hEvent = 0;
            ok = ReadFile( file->splits[i].handle,
                           buffer,
                           thisBytesToRead,
                           bytesRead,
                           &thisOverlapped );
            if( !ok )
            {
                firstReadPending = true;
            }
            buffer = ((char*)buffer)+thisBytesToRead;
            bytesToRead -= thisBytesToRead;
            absoluteOffset.QuadPart += thisBytesToRead;
        }
        i++;
    }
    while( ok || GetLastError() == ERROR_IO_PENDING );
    if( ok )
    {
        *bytesRead = savedBytesToRead;
    }
    // we assume that if the last read has been completed then all the
    // previous read had been completed. The following assertion
    // enforces this assumption.
    ERR_X_Assert( !(ok && firstReadPending) );

    return ok;
}

static BOOL
_synchRead( XBCompositeFile* file,
            void* buffer,
            DWORD bytesToRead,
            DWORD* bytesRead )
{
    BOOL ok;
    
    *bytesRead = 0;
    
    do
    {
        DWORD thisRoundReadBytes;
        SingleSplit* split = &file->splits[ file->currentSplit ];
            
        ERR_X_Assert( split->handle != INVALID_HANDLE_VALUE );
        ok = ReadFile( split->handle,
                       buffer,
                       bytesToRead,
                       &thisRoundReadBytes,
                       NULL );
        if( !ok )
        {
            break;
        }
        if( thisRoundReadBytes != bytesToRead )
        {
            // EOF encountered
            file->currentSplit++;
        }
        bytesToRead -= thisRoundReadBytes;
        *bytesRead += thisRoundReadBytes;
        buffer = ((char*)buffer) + thisRoundReadBytes;
    }
    while( bytesToRead > 0 );
    return ok;
}

static BOOL
_Read( XBCompositeFile* file,
       void* buffer,
       DWORD bytesToRead,
       DWORD* bytesRead,
       OVERLAPPED* overlapped )
{
    BOOL ok;

    ERR_X_Assert( file != NULL );
    ERR_X_Assert( bytesToRead != 0 );
    if( bytesToRead == 0 )
    {
        *bytesRead = 0;
        return TRUE;
    }
    if( overlapped != NULL )
    {
        // handle Asynch io
        ok = _asynchRead( file, buffer, bytesToRead, bytesRead, overlapped );
    }
    else
    {
        // synch read.
        ok = _synchRead( file, buffer, bytesToRead, bytesRead );
    }
#if defined( ENABLE_ERROR_INJECTION )
    if( rand() < 100 )
    {
        OutputDebugString( "read failed\n" );
        ok = FALSE;
    }
#endif
    return ok;
}

#if defined( ENABLE_READ_LOGGING )
static void
_logRead( XBCompositeFile* file,
          void* buffer,
          DWORD bytesToRead,
          OVERLAPPED* overlapped )
{
    char text[512];

    sprintf( text,
             "%8x: Read( %8x, %8x, %d, %s )\n",
             GetCurrentThreadId(),
             file,
             buffer,
             bytesToRead,
             overlapped != NULL ? "asynch" : "synch" );
    OutputDebugString( text );
}
#else
static __inline void
_logRead( XBCompositeFile* file,
          void* buffer,
          DWORD bytesToRead,
          OVERLAPPED* overlapped )
{
    (void) file;
    (void) buffer;
    (void) bytesToRead;
    (void) overlapped;
}

#endif

BOOL
XBCompositeFile_Read( XBCompositeFile* file,
                      void* buffer,
                      DWORD bytesToRead,
                      DWORD* bytesRead,
                      OVERLAPPED* overlapped )
{
    struct XBStatus status;
    BOOL ok = TRUE;

    ERR_X_Assert( file != NULL );

    _logRead( file, buffer, bytesToRead, overlapped );

    XBCompositeFile_getStatus( file, &status );

    for( ;; )
    {
        ok = _Read( file, buffer, bytesToRead, bytesRead, overlapped );
        if( ok )
        {
            return ok;
        }
        switch( GetLastError() )
        {
            case ERROR_HANDLE_EOF:
                return FALSE;

            case ERROR_IO_PENDING:
                return FALSE;

            default:
                break;
        }
        if( file->errorHandler == NULL )
        {
            // no error handling callback
            // let the user run his own
            return FALSE;
        }
        do
        {
            (file->errorHandler)();
            ok = XBCompositeFile_setStatus( file, &status );
        }
        while( !ok );
    }
}


static BOOL
_seekFromStart( XBCompositeFile* file,
                LARGE_INTEGER offset,
                LARGE_INTEGER* newPosition )
{
    int i;
    SingleSplit* split;
    
    split = &file->splits[0];
    for( i=0; i<MAX_SPLITS; ++i )
    {
        if( split->handle == INVALID_HANDLE_VALUE )
        {
            break;
        }
        if( offset.QuadPart < split->offset + split->size )
        {
            LARGE_INTEGER splitPosition;

            if( newPosition != NULL )
            {
                *newPosition = offset;
            }
            splitPosition.QuadPart = offset.QuadPart - split->offset;
            file->currentSplit = i;
            return SetFilePointerEx( split->handle,
                                     splitPosition,
                                     NULL,
                                     FILE_BEGIN );
        }
        split++;
    }
    return FALSE;
}

static BOOL
_seekFromCurrent( XBCompositeFile* file,
                  LARGE_INTEGER offset,
                  LARGE_INTEGER* newPosition )
{
    LARGE_INTEGER fromStart;

    fromStart.QuadPart = file->splits[file->currentSplit].offset +
                         offset.QuadPart;
    return _seekFromStart( file,
                           fromStart,
                           newPosition );
}

static BOOL
_seekFromEnd( XBCompositeFile* file,
              LARGE_INTEGER offset,
              LARGE_INTEGER* newPosition )
{
    ERR_X_ForceError( "don't want to implement this", "" );
    return FALSE;
}


BOOL
XBCompositeFile_SetFilePointerEx( XBCompositeFile* file,
                                  LARGE_INTEGER offset,
                                  LARGE_INTEGER* newPosition,
                                  DWORD moveMethod )
{
    ERR_X_Assert( file != NULL );
    switch( moveMethod )
    {
        case FILE_BEGIN:
            return _seekFromStart( file, offset, newPosition );
            
        case FILE_CURRENT:
            return _seekFromCurrent( file, offset, newPosition );
            
        case FILE_END:
            return _seekFromEnd( file, offset, newPosition );
            
        default:
            ERR_X_Assert( moveMethod != FILE_BEGIN && 
                          moveMethod != FILE_CURRENT && 
                          moveMethod != FILE_END );
            return FALSE;
    }
}
                                 

