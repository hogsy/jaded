//-----------------------------------------------------------------------------
// ---------------------
// File ....: interfce.c
// ---------------------
// Author...: Gus J Grubba
// Date ....: October 1995
// Descr....: Interface to JPEG library
//
// History .: Oct, 27 1995 - Started
//            
//-----------------------------------------------------------------------------
        
#define  STRICT
#if (defined(PSX2_TARGET) || defined(_GAMECUBE) || defined(_XBOX) || defined(_XENON))
#include "JINCLUDE.H"
#else
#include <windows.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "INTERFCE.H"
#include "JPEGLIB.H"
#include "JERROR.H"
#include <setjmp.h>


#ifdef PSX2_TARGET
extern void *ps2memset(char *dst, unsigned char c, int size);
extern void *ps2memcpy(char *dst, char*src, int size);
#define L_memcpy    ps2memcpy
#define L_memset    ps2memset
#else
#define L_memcpy    memcpy
#define L_memset    memset
#endif

unsigned char *ucJADE_JPEG_INPOUT_FILE;
#ifdef JADEFUSION
int lJADE_JPEG_INPOUT_FILE_LENGHT;
#else
LONG lJADE_JPEG_INPOUT_FILE_LENGHT;
#endif
//-----------------------------------------------------------------------------
// Error handling
//

struct my_error_mgr {
  struct Jade_jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF void my_error_exit (j_common_ptr cinfo) {
  my_error_ptr myerr = (my_error_ptr) cinfo->err;
  longjmp(myerr->setjmp_buffer, 1);
}

//-----------------------------------------------------------------------------
// *> Info()
//

void JpegInfo( JPEGDATA *data ) {

  struct Jade_jpeg_decompress_struct cinfo;
  struct my_error_mgr           jerr;

  cinfo.err           = Jade_jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;

  if (setjmp(jerr.setjmp_buffer)) {
    Jade_jpeg_destroy_decompress(&cinfo);
    data->status = 1;
    return;
  }

  Jade_jpeg_create_decompress(&cinfo);
  Jade_jpeg_stdio_src(&cinfo);

  (void) Jade_jpeg_read_header(&cinfo, TRUE);
  (void) Jade_jpeg_start_decompress(&cinfo);

  data->width      = cinfo.output_width;
  data->height     = cinfo.output_height;
  data->components = cinfo.num_components;

  //(void) Jade_jpeg_finish_decompress(&cinfo);
  
  Jade_jpeg_destroy_decompress(&cinfo);

  data->status = 0;
  
}

//-----------------------------------------------------------------------------
// *> Write()
//
extern void *jpgHightPointer;
void JpegWrite( JPEGDATA *data ) {

     JSAMPROW row_pointer[1];
     int      row_stride;
     struct   Jade_jpeg_compress_struct cinfo;
     struct   Jade_jpeg_error_mgr       jerr;
     
     //-- Error Handling ------------------------------------------------------
     
     jmp_buf env;
     jpgHightPointer = NULL;
    
     if (setjmp(env)) {
     
        //-- Define Error
        
        
        data->status = 1;
        Jade_jpeg_destroy_compress(&cinfo);
        return;
        
     }
   
     //-- Allocate and Initialize Jpeg Structures -----------------------------
   
     memset(&cinfo,0,sizeof(struct Jade_jpeg_compress_struct));
     memset(&jerr, 0,sizeof(struct Jade_jpeg_error_mgr));
     
     //-- Initialize the JPEG compression object with default error handling --
   
     cinfo.err       = Jade_jpeg_std_error(&jerr);
     jerr.error_exit = my_error_exit;
   
     Jade_jpeg_create_compress(&cinfo);

     //-- Specify data destination for compression ----------------------------

     Jade_jpeg_stdio_dest(&cinfo, (FILE*)data->output_file);

     //-- Initialize JPEG parameters ------------------------------------------

     cinfo.in_color_space   = JCS_RGB;
     cinfo.image_width      = data->width;
     cinfo.image_height     = data->height;
     cinfo.input_components = 3;

     Jade_jpeg_set_defaults(&cinfo);

     cinfo.data_precision   = 8;
     cinfo.arith_code       = data->aritcoding;
     cinfo.optimize_coding  = TRUE;
     cinfo.CCIR601_sampling = data->CCIR601sampling;
     cinfo.smoothing_factor = data->smoothingfactor;
  	 if (data->width > 320)
	 {
		cinfo.dct_method = JDCT_IFAST; 
	 }
	 if (data->width > 2000)
	 {
		cinfo.dct_method = JDCT_ISLOW; 
	 }
   
     Jade_jpeg_set_quality(&cinfo, data->quality, TRUE);
     Jade_jpeg_default_colorspace(&cinfo);

     //-- Start compressor

     Jade_jpeg_start_compress(&cinfo, TRUE);

     //-- Process data

     row_stride = data->width * 3;

     while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &data->ptr[cinfo.next_scanline * row_stride];
        (void) Jade_jpeg_write_scanlines(&cinfo, row_pointer, 1);
     }

     //-- Finish compression and release memory
     
     Jade_jpeg_finish_compress(&cinfo);
     Jade_jpeg_destroy_compress(&cinfo);

     //-- Status
     
     data->status = 0;

}

//-----------------------------------------------------------------------------
// *> Read()
//

void JpegRead( JPEGDATA *data ) {

     struct Jade_jpeg_decompress_struct cinfo;
     struct my_error_mgr           jerr;

     BYTE      *bf;
     JSAMPARRAY buffer;
     int        row_stride,y;

     cinfo.err           = Jade_jpeg_std_error(&jerr.pub);
     jerr.pub.error_exit = my_error_exit;

     if (setjmp(jerr.setjmp_buffer)) {


        data->status = 1;
        Jade_jpeg_destroy_decompress(&cinfo);
        return;
     }

     Jade_jpeg_create_decompress(&cinfo);
     Jade_jpeg_stdio_src(&cinfo);

     (void) Jade_jpeg_read_header(&cinfo, TRUE);
     (void) Jade_jpeg_start_decompress(&cinfo);

     data->components = cinfo.num_components;

     row_stride = cinfo.output_width * cinfo.output_components;

     buffer = (*cinfo.mem->alloc_sarray)
     		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

     bf = data->ptr;
     y  = 0;
     
     while (cinfo.output_scanline < cinfo.output_height) {
        (void) Jade_jpeg_read_scanlines(&cinfo, buffer, 1);
        L_memcpy(bf,buffer[0],row_stride);
        bf += row_stride;
     }

     (void) Jade_jpeg_finish_decompress(&cinfo);
     Jade_jpeg_destroy_decompress(&cinfo);

     //-- Status
     
     data->status = 0;

}

//-- interfce.c ---------------------------------------------------------------
