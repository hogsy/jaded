//-----------------------------------------------------------------------------
// ---------------------
// File ....: interfce.h
// ---------------------
// Author...: Gus J Grubba
// Date ....: October 1995
// Descr....: C Interface to JPEG library
//
// History .: Oct, 27 1995 - Started
//
//-----------------------------------------------------------------------------

#ifdef PSX2_TARGET
#	pragma pack( 1 )
#endif
typedef struct _jpegdata
{
	unsigned char *ptr;
	int width;
	int height;
	void *output_file;
	int aritcoding;
	int CCIR601sampling;
	int smoothingfactor;
	int quality;
	void *hWnd;
	int status;
	int components;
} JPEGDATA;
#ifdef PSX2_TARGET
#	pragma pack( 0 )
#endif

/* JADE specific: 
		ucJADE_JPEG_INPOUT_FILE is the address of the file buffer.
		lJADE_JPEG_INPOUT_FILE_LENGHT is the lenght of the file.
		you have an example in texfile & in texframe.
*/

#if defined( __cplusplus )
extern "C"
{
#endif

	extern unsigned char *ucJADE_JPEG_INPOUT_FILE;
#ifdef JADEFUSION
	extern int lJADE_JPEG_INPOUT_FILE_LENGHT;
#else
extern LONG lJADE_JPEG_INPOUT_FILE_LENGHT;
#endif
	void JpegWrite( JPEGDATA *data );
	void JpegInfo( JPEGDATA *data );
	void JpegRead( JPEGDATA *data );

#if defined( __cplusplus )
};
#endif

//-- interfce.c ---------------------------------------------------------------
