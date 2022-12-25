/**
 * @file BIGcompositeFile.h
 * @author Massimiliano Pagani
 *
 * The purpouse of this module is to handle the big file split over several
 * files.
 */

#if !defined( XB_COMPOSITE_FILE_H )
#  define XB_COMPOSITE_FILE_H

#if !defined( _XBOX )
#  error this is an XBox only file. Go away you misbeliever
#endif

#include <xtl.h>
#include <winbase.h>

struct XBCompositeFile;
typedef struct XBCompositeFile XBCompositeFile;

typedef void (XBCompositeFile_ErrorHandler)( void );

/**
 * This method open one composite file. The semantic is identical to the
 * Microsoft CreateFile where GENERIC_READ, FILE_SHARE_READ and OPEN_EXISTING
 * flags have been specified.
 * 
 * @param fileName this is the fileName stem. Up to ten files are opened named
 *                 fileName0, fileName1, ... fileName9. The alphabetical order
 *                 is taken as the ordering in the composite file.
 * @param flagsAndAttributes here you have to specify the flags and attributes
 *                           for opening the file as specified in the XBox
 *                           documentation. Mainly you would like to specify
 *                           FILE_FLAG_OVERLAPPED. (see
 *                           ms-help://MS.VSCC/MS.XboxSDK/XboxSDK/html/filesio_7wmd.htm)
 * @param errorHandler is the function that is called when a read fails. See
 *                     #XBCompositeFile_Read.
 * @return a pointer to the XBCompositeFile. This is always a valid pointer.
 *
 * @contract at least one file (named fileName0) must exist.
 * @ensure the pointer returned is always valid.
 */

XBCompositeFile*
XBCompositeFile_Open( const char* fileName,
                      DWORD flagsAndAttributes,
                      XBCompositeFile_ErrorHandler* errorHandler );

/**
 * Close the file opened by #XBCompositeFile_Open.
 *
 * @param compositeFile a pointer to a XBCompositeFile as returned by 
 *                      #XBCompositeFile_Open.
 *
 * @contract compositeFile must not be NULL.
 */

void
XBCompositeFile_Close( XBCompositeFile* compositeFile );

/**
 * This function reads a chunk of data from a file and stores it in the given
 * buffer.
 * The semantic for this call is the same of Microsoft FileRead.
 *
 * @param file a valid file pointer.
 * @param buffer a pointer to a buffer where read data will be stored. This
 *               buffer has to be large enough to accomodate 'bytesToRead'
 *               bytes.
 * @param bytesToRead the number of bytes to read.
 * @param bytesRead a pointer to a DWORD which will be filled with the number
 *                  of effectively read bytes.
 * @param overlapped if the file has been opened with the FILE_FLAG_OVERLAPPED
 *                   flag then this argument can point to an OVERLAPPED
 *                   structure, and the read will be performed asynchronously.
 * @contract if the read fails, then the error handler is called. When the 
 *           error handler returns the read is reattempted. If the error is
 *           permanent then this function never returns.
 * @return TRUE if the read was succesful, FALSE otherwise.
 * 
 * @see ms-help://MS.VSCC/MS.XboxSDK/XboxSDK/html/filesio_39id.htm for details
 *      about this function semantic.
 */
BOOL
XBCompositeFile_Read( XBCompositeFile* file,
                      void* buffer,
                      DWORD bytesToRead,
                      DWORD* bytesRead,
                      OVERLAPPED* overlapped );

/**
 * Change the reading position for the given file.
 * This has the same semantic of SetFilePointerEx from Microsoft library.
 *
 * @param file a composite file pointer as returned by #XBCompositeFile_Open.
 * @param offset the required offset.
 * @param newPosition if not NULL then it will be filled with the new
 *                    absolute position of the reading position for the
 *                    file.
 * @param moveMethod how to consider the offset. Can be FILE_BEGIN,
 *                   FILE_CURRENT or FILE_END.
 *
 * @return TRUE if the operation has succeeded, FALSE otherwise.
 */

BOOL
XBCompositeFile_SetFilePointerEx( XBCompositeFile* file,
                                  LARGE_INTEGER offset,
                                  LARGE_INTEGER* newPosition,
                                  DWORD moveMethod );

#endif
