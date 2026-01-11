/* display.c, X11 interface                                                 */

 /* the Xlib interface is closely modeled after
  * mpeg_play 2.0 by the Berkeley Plateau Research Group
  */

#include <stdio.h>
#include <stdlib.h>

//#include <X11/Xlib.h>
//#include <X11/Xutil.h>

#include "config.h"
#include "global.h"

#define Pxl_32 unsigned long

/* private prototypes */
static void Display_Image (Pxl_32 *Dithered_Image);
static void Dither_Frame (unsigned char **src);
static void Dither_Top_Field (unsigned char *src[], Pxl_32 *dst);
static void Dither_Bottom_Field (unsigned char *src[], Pxl_32 *dst);
static void Dither_Top_Field420 (unsigned char *src[],
                                      Pxl_32 *dst);
static void Dither_Bottom_Field420 (unsigned char *src[],
                                      Pxl_32 *dst);

/* local data */

static unsigned char Y_Table[256+16];
static unsigned char Cb_Table[128+16];
static unsigned char Cr_Table[128+16];

static Pxl_32 *Dithered_Image, *Dithered_Image2;
/* X11 related variables */
//static Display *Display_Ptr;
//static Window Window_Instance;
//static GC GC_Instance;
//static XImage *Ximage_Ptr, *Ximage_Ptr2;
static Pxl_32 Pixel[256];

/* connect to server, create and map window,
 * allocate colors and (shared) memory
 */
void Initialize_Display_Process()
{
  int crv, cbu, cgu, cgv;
  long Y, Cb, Cr, R, G, B , Loc;
  int i;


  /* matrix coefficients */
  crv = Inverse_Table_6_9[gst_AllGloballs.matrix_coefficients][0];
  cbu = Inverse_Table_6_9[gst_AllGloballs.matrix_coefficients][1];
  cgu = Inverse_Table_6_9[gst_AllGloballs.matrix_coefficients][2];
  cgv = Inverse_Table_6_9[gst_AllGloballs.matrix_coefficients][3];

  /* allocate colors */

  /* color allocation:
   * i is the (internal) 8 bit color number, it consists of separate
   * bit fields for Y, U and V: i = (yyyyuuvv), we don't use yyyy=0000
   * and yyyy=1111, this leaves 32 colors for other applications
   *
   * the allocated colors correspond to the following Y, U and V values:
   * Y:   24, 40, 56, 72, 88, 104, 120, 136, 152, 168, 184, 200, 216, 232
   * U,V: -48, -16, 16, 48
   *
   * U and V values span only about half the color space; this gives
   * usually much better quality, although highly saturated colors can
   * not be displayed properly
   *
   * translation to R,G,B is implicitly done by the color look-up table
   */
  memset(Pixel , 0 , sizeof(Pxl_32)* 256);
  for (i=16; i<240; i++)
  {
    /* color space conversion */
	  Y  = 16L*((i>>4L)&15L) + 8L;
    Cb = 32L*((i>>2L)&3L)  - 48L;
    Cr = 32L*(i&3L)       - 48L;

    Y = 76309L * (Y - 16L); /* (255/219)*65536 */


    R = gst_AllGloballs.Clip[(Y + crv*Cr + 32768L)>>16];
    G = gst_AllGloballs.Clip[(Y - cgu*Cb - cgv*Cr + 32768L)>>16];
    B = gst_AllGloballs.Clip[(Y + cbu*Cb + 32786L)>>16];
	Pixel[i] = (R<<16)|(G<<8)|(B<<0);
    if (gst_AllGloballs.stSystem->ulFlags & MPEG_Mode_SwapRedBlue)
        Pixel[i] = (R<<0)|(G<<8)|(B<<16);
	
  }
  Loc = gst_AllGloballs.horizontal_size * gst_AllGloballs.vertical_size * 4L + gst_AllGloballs.horizontal_size * 4L * 32L;
#ifdef JADEFUSION
  Dithered_Image = (unsigned long*)MPG_MALLOC(Loc);
  Dithered_Image2 = (unsigned long*)MPG_MALLOC(Loc);
#else
  Dithered_Image = MPG_MALLOC(Loc);
  Dithered_Image2 = MPG_MALLOC(Loc);
#endif
}

void Terminate_Display_Process()
{
  free(Dithered_Image) ;
  free(Dithered_Image2);
  Dithered_Image = NULL;
  Dithered_Image2 =NULL;
}

static void Display_Image(Pxl_32 *Dithered_ImageX)
{
	gst_AllGloballs.stSystem->ulBufferResult = Dithered_ImageX;
}

void Display_Second_Field()
{
  Display_Image(Dithered_Image2);
}

/* 4x4 ordered dither
 *
 * threshold pattern:
 *   0  8  2 10
 *  12  4 14  6
 *   3 11  1  9
 *  15  7 13  5
 */

void Initialize_Dither_Matrix()
{
  int i, v;
  memset( Y_Table , 0 , 256+16);
  memset(Cb_Table , 0 , 128+16);
  memset(Cr_Table , 0 , 128+16);

  for (i=-8; i<256+8; i++)
  {
    v = i>>4;
    if (v<1)
      v = 1;
    else if (v>14)
      v = 14;
    Y_Table[i+8] = v<<4;
  }

  for (i=0; i<128+16; i++)
  {
    v = (i-40)>>4;
    if (v<0)
      v = 0;
    else if (v>3)
      v = 3;
    Cb_Table[i] = v<<2;
    Cr_Table[i] = v;
  }
}

void dither(unsigned char **src)
{
  /* should this test only the display flag, not progressive_sequence ? --CF */
  /* CHANGE 95/05/13: progressive_sequence -> progressive_frame */
  if( gst_AllGloballs.progressive_frame || gst_AllGloballs.Display_Progressive_Flag)
    Dither_Frame(src);
  else
  {
    if ((gst_AllGloballs.picture_structure==FRAME_PICTURE && gst_AllGloballs.top_field_first) || gst_AllGloballs.picture_structure==BOTTOM_FIELD)
    {
		Dither_Top_Field(src,Dithered_Image);
		Dither_Bottom_Field(src,Dithered_Image2);
    }
    else
    {
      /* bottom field first */
		Dither_Bottom_Field(src,Dithered_Image);
		Dither_Top_Field(src,Dithered_Image2);
    }
  }

  Display_Image(Dithered_Image);
}

static void Dither_Frame(unsigned char **src)
{
  int i,j;
  int y,u,v,DeVf;
  unsigned char *py,*pu,*pv;
  Pxl_32 *dst;

  py = src[0];
  pu = src[1];
  pv = src[2];
  dst = (Pxl_32 *)Dithered_Image;
  DeVf = 0;
  if (gst_AllGloballs.ulFLAGS & MPEG_Mode_VerticalFlip)
  {
	  dst += (long)(gst_AllGloballs.Coded_Picture_Height - 1L) * (long)gst_AllGloballs.Coded_Picture_Width;
	  DeVf = gst_AllGloballs.Coded_Picture_Width * 2L;
  }

  if (gst_AllGloballs.ulFLAGS & MPEG_Mode_BlackAndWhite)
  {
	  for (j=0; j<gst_AllGloballs.Coded_Picture_Height; j++)
	  {
		  for (i=0; i<gst_AllGloballs.Coded_Picture_Width; i++)
		  {
			  *dst++ = *py|(*py<<16)|(*py<<8);
			  py++;
		  }
		  dst -= DeVf;
	  }
	  return;
  }


  for (j=0; j<gst_AllGloballs.Coded_Picture_Height; j+=4)
  {
    /* line j + 0 */
    for (i=0; i<gst_AllGloballs.Coded_Picture_Width; i+=4)
    {
      y = *py++;
      u = *pu++ >> 1;
      v = *pv++ >> 1;
      *dst++ = Pixel[Y_Table[y]|Cb_Table[u]|Cr_Table[v]];
      y = *py++;
      if (gst_AllGloballs.chroma_format==CHROMA444)
      {
        u = *pu++ >> 1;
        v = *pv++ >> 1;
      }
      *dst++ = Pixel[Y_Table[y+8]|Cb_Table[u+8]|Cr_Table[v+8]];
      y = *py++;
      u = *pu++ >> 1;
      v = *pv++ >> 1;
      *dst++ = Pixel[Y_Table[y+2]|Cb_Table[u+2]|Cr_Table[v+2]];
      y = *py++;
      if (gst_AllGloballs.chroma_format==CHROMA444)
      {
        u = *pu++ >> 1;
        v = *pv++ >> 1;
      }
      *dst++ = Pixel[Y_Table[y+10]|Cb_Table[u+10]|Cr_Table[v+10]];
    }
	dst -= DeVf;

    if (gst_AllGloballs.chroma_format==CHROMA420)
    {
      pu -= gst_AllGloballs.Chroma_Width;
      pv -= gst_AllGloballs.Chroma_Width;
    }

    /* line j + 1 */
    for (i=0; i<gst_AllGloballs.Coded_Picture_Width; i+=4)
    {
      y = *py++;
      u = *pu++ >> 1;
      v = *pv++ >> 1;
      *dst++ = Pixel[Y_Table[y+12]|Cb_Table[u+12]|Cr_Table[v+12]];
      y = *py++;
      if (gst_AllGloballs.chroma_format==CHROMA444)
      {
        u = *pu++ >> 1;
        v = *pv++ >> 1;
      }
      *dst++ = Pixel[Y_Table[y+4]|Cb_Table[u+4]|Cr_Table[v+4]];
      y = *py++;
      u = *pu++ >> 1;
      v = *pv++ >> 1;
      *dst++ = Pixel[Y_Table[y+14]|Cb_Table[u+14]|Cr_Table[v+14]];
      y = *py++;
      if (gst_AllGloballs.chroma_format==CHROMA444)
      {
        u = *pu++ >> 1;
        v = *pv++ >> 1;
      }
      *dst++ = Pixel[Y_Table[y+6]|Cb_Table[u+6]|Cr_Table[v+6]];
    }
	dst -= DeVf;

    /* line j + 2 */
    for (i=0; i<gst_AllGloballs.Coded_Picture_Width; i+=4)
    {
      y = *py++;
      u = *pu++ >> 1;
      v = *pv++ >> 1;
      *dst++ = Pixel[Y_Table[y+3]|Cb_Table[u+3]|Cr_Table[v+3]];
      y = *py++;
      if (gst_AllGloballs.chroma_format==CHROMA444)
      {
        u = *pu++ >> 1;
        v = *pv++ >> 1;
      }
      *dst++ = Pixel[Y_Table[y+11]|Cb_Table[u+11]|Cr_Table[v+11]];
      y = *py++;
      u = *pu++ >> 1;
      v = *pv++ >> 1;
      *dst++ = Pixel[Y_Table[y+1]|Cb_Table[u+1]|Cr_Table[v+1]];
      y = *py++;
      if (gst_AllGloballs.chroma_format==CHROMA444)
      {
        u = *pu++ >> 1;
        v = *pv++ >> 1;
      }
      *dst++ = Pixel[Y_Table[y+9]|Cb_Table[u+9]|Cr_Table[v+9]];
    }

    if (gst_AllGloballs.chroma_format==CHROMA420)
    {
      pu -= gst_AllGloballs.Chroma_Width;
      pv -= gst_AllGloballs.Chroma_Width;
    }
	dst -= DeVf;
    /* line j + 3 */
    for (i=0; i<gst_AllGloballs.Coded_Picture_Width; i+=4)
    {
      y = *py++;
      u = *pu++ >> 1;
      v = *pv++ >> 1;
      *dst++ = Pixel[Y_Table[y+15]|Cb_Table[u+15]|Cr_Table[v+15]];
      y = *py++;
      if (gst_AllGloballs.chroma_format==CHROMA444)
      {
        u = *pu++ >> 1;
        v = *pv++ >> 1;
      }
      *dst++ = Pixel[Y_Table[y+7]|Cb_Table[u+7]|Cr_Table[v+7]];
      y = *py++;
      u = *pu++ >> 1;
      v = *pv++ >> 1;
      *dst++ = Pixel[Y_Table[y+13]|Cb_Table[u+13]|Cr_Table[v+13]];
      y = *py++;
      if (gst_AllGloballs.chroma_format==CHROMA444)
      {
        u = *pu++ >> 1;
        v = *pv++ >> 1;
      }
      *dst++ = Pixel[Y_Table[y+5]|Cb_Table[u+5]|Cr_Table[v+5]];
    }
	dst -= DeVf;
  }

}
#ifdef JADEFUSION
static void Dither_Top_Field(unsigned char *src[], Pxl_32 *dst)
#else
static void Dither_Top_Field(src,dst)
unsigned char *src[];
Pxl_32 *dst;
#endif
{
  int i,j;

  int y,Y2,u,v;
  unsigned char *py,*Y2_ptr,*pu,*pv;
  Pxl_32 *dst2;

  py = src[0];
  Y2_ptr = src[0] + (gst_AllGloballs.Coded_Picture_Width<<1);
  pu = src[1];
  pv = src[2];
  dst2 = dst + gst_AllGloballs.Coded_Picture_Width;



  if (gst_AllGloballs.ulFLAGS & MPEG_Mode_BlackAndWhite)
  {
	  for (j=0; j<gst_AllGloballs.Coded_Picture_Height; j++)
	  {
		  for (i=0; i<gst_AllGloballs.Coded_Picture_Width; i++)
		  {
			  *dst++ = *py|(*py<<16)|(*py<<8);
			  py++;
		  }
	  }
		  return;
  }


  for (j=0; j<gst_AllGloballs.Coded_Picture_Height; j+=4)
  {
    /* line j + 0, j + 1 */
    for (i=0; i<gst_AllGloballs.Coded_Picture_Width; i+=4)
    {
      y = *py++;
      Y2 = *Y2_ptr++;
      u = *pu++ >> 1;	
      v = *pv++ >> 1;
      *dst++  = Pixel[Y_Table[y]|Cb_Table[u]|Cr_Table[v]];
      *dst2++ = Pixel[Y_Table[((y+Y2)>>1)+12]|Cb_Table[u+12]|Cr_Table[v+12]];

      y = *py++;
      Y2 = *Y2_ptr++;
      if (gst_AllGloballs.chroma_format==CHROMA444)
      {
        u = *pu++ >> 1;
        v = *pv++ >> 1;
      }
      *dst++  = Pixel[Y_Table[y+8]|Cb_Table[u+8]|Cr_Table[v+8]];
      *dst2++ = Pixel[Y_Table[((y+Y2)>>1)+4]|Cb_Table[u+4]|Cr_Table[v+4]];

      y = *py++;
      Y2 = *Y2_ptr++;
      u = *pu++ >> 1;
      v = *pv++ >> 1;
      *dst++  = Pixel[Y_Table[y+2]|Cb_Table[u+2]|Cr_Table[v+2]];
      *dst2++ = Pixel[Y_Table[((y+Y2)>>1)+14]|Cb_Table[u+14]|Cr_Table[v+14]];

      y = *py++;
      Y2 = *Y2_ptr++;
      if (gst_AllGloballs.chroma_format==CHROMA444)
      {
        u = *pu++ >> 1;
        v = *pv++ >> 1;
      }
      *dst++  = Pixel[Y_Table[y+10]|Cb_Table[u+10]|Cr_Table[v+10]];
      *dst2++ = Pixel[Y_Table[((y+Y2)>>1)+6]|Cb_Table[u+6]|Cr_Table[v+6]];
    }

    py += gst_AllGloballs.Coded_Picture_Width;

    if (j!=(gst_AllGloballs.Coded_Picture_Height-4))
      Y2_ptr += gst_AllGloballs.Coded_Picture_Width;
    else
      Y2_ptr -= gst_AllGloballs.Coded_Picture_Width;

    dst += gst_AllGloballs.Coded_Picture_Width;
    dst2 += gst_AllGloballs.Coded_Picture_Width;

    if (gst_AllGloballs.chroma_format==CHROMA420)
    {
      pu -= gst_AllGloballs.Chroma_Width;
      pv -= gst_AllGloballs.Chroma_Width;
    }
    else
    {
      pu += gst_AllGloballs.Chroma_Width;
      pv += gst_AllGloballs.Chroma_Width;
    }

    /* line j + 2, j + 3 */
    for (i=0; i<gst_AllGloballs.Coded_Picture_Width; i+=4)
    {
      y = *py++;
      Y2 = *Y2_ptr++;
      u = *pu++ >> 1;
      v = *pv++ >> 1;
      *dst++  = Pixel[Y_Table[y+3]|Cb_Table[u+3]|Cr_Table[v+3]];
      *dst2++ = Pixel[Y_Table[((y+Y2)>>1)+15]|Cb_Table[u+15]|Cr_Table[v+15]];

      y = *py++;
      Y2 = *Y2_ptr++;
      if (gst_AllGloballs.chroma_format==CHROMA444)
      {
        u = *pu++ >> 1;
        v = *pv++ >> 1;
      }
      *dst++  = Pixel[Y_Table[y+11]|Cb_Table[u+11]|Cr_Table[v+11]];
      *dst2++ = Pixel[Y_Table[((y+Y2)>>1)+7]|Cb_Table[u+7]|Cr_Table[v+7]];

      y = *py++;
      Y2 = *Y2_ptr++;
      u = *pu++ >> 1;
      v = *pv++ >> 1;
      *dst++  = Pixel[Y_Table[y+1]|Cb_Table[u+1]|Cr_Table[v+1]];
      *dst2++ = Pixel[Y_Table[((y+Y2)>>1)+13]|Cb_Table[u+13]|Cr_Table[v+13]];

      y = *py++;
      Y2 = *Y2_ptr++;
      if (gst_AllGloballs.chroma_format==CHROMA444)
      {
        u = *pu++ >> 1;
        v = *pv++ >> 1;
      }
      *dst++  = Pixel[Y_Table[y+9]|Cb_Table[u+9]|Cr_Table[v+9]];
      *dst2++ = Pixel[Y_Table[((y+Y2)>>1)+5]|Cb_Table[u+5]|Cr_Table[v+5]];
    }

    py += gst_AllGloballs.Coded_Picture_Width;
    Y2_ptr += gst_AllGloballs.Coded_Picture_Width;
    dst += gst_AllGloballs.Coded_Picture_Width;
    dst2 += gst_AllGloballs.Coded_Picture_Width;
    pu += gst_AllGloballs.Chroma_Width;
    pv += gst_AllGloballs.Chroma_Width;
  }
}

#ifdef JADEFUSION
static void Dither_Bottom_Field(unsigned char *src[], Pxl_32 *dst)
#else
static void Dither_Bottom_Field(src,dst)
unsigned char *src[];
Pxl_32 *dst;
#endif
{
  int i,j;
  int y,Y2,u,v;
  unsigned char *py,*Y2_ptr,*pu,*pv;
  Pxl_32 *dst2;

  py = src[0] + gst_AllGloballs.Coded_Picture_Width;
  Y2_ptr = py;
  pu = src[1] + gst_AllGloballs.Chroma_Width;
  pv = src[2] + gst_AllGloballs.Chroma_Width;
  dst2 = dst + gst_AllGloballs.Coded_Picture_Width;

  if (gst_AllGloballs.ulFLAGS & MPEG_Mode_BlackAndWhite)
  {
	  for (j=0; j<gst_AllGloballs.Coded_Picture_Height; j++)
		  for (i=0; i<gst_AllGloballs.Coded_Picture_Width; i++)
		  {
			  *dst++ = *py|(*py<<16)|(*py<<8);
			  py++;
		  }
		  return;
  }

  for (j=0; j<gst_AllGloballs.Coded_Picture_Height; j+=4)
  {
    /* line j + 0, j + 1 */
    for (i=0; i<gst_AllGloballs.Coded_Picture_Width; i+=4)
    {
      y = *py++;
      Y2 = *Y2_ptr++;
      u = *pu++ >> 1;
      v = *pv++ >> 1;
      *dst++  = Pixel[Y_Table[((y+Y2)>>1)]|Cb_Table[u]|Cr_Table[v]];
      *dst2++ = Pixel[Y_Table[Y2+12]|Cb_Table[u+12]|Cr_Table[v+12]];

      y = *py++;
      Y2 = *Y2_ptr++;
      if (gst_AllGloballs.chroma_format==CHROMA444)
      {
        u = *pu++ >> 1;
        v = *pv++ >> 1;
      }
      *dst++  = Pixel[Y_Table[((y+Y2)>>1)+8]|Cb_Table[u+8]|Cr_Table[v+8]];
      *dst2++ = Pixel[Y_Table[Y2+4]|Cb_Table[u+4]|Cr_Table[v+4]];

      y = *py++;
      Y2 = *Y2_ptr++;
      u = *pu++ >> 1;
      v = *pv++ >> 1;
      *dst++  = Pixel[Y_Table[((y+Y2)>>1)+2]|Cb_Table[u+2]|Cr_Table[v+2]];
      *dst2++ = Pixel[Y_Table[Y2+14]|Cb_Table[u+14]|Cr_Table[v+14]];

      y = *py++;
      Y2 = *Y2_ptr++;
      if (gst_AllGloballs.chroma_format==CHROMA444)
      {
        u = *pu++ >> 1;
        v = *pv++ >> 1;
      }
      *dst++  = Pixel[Y_Table[((y+Y2)>>1)+10]|Cb_Table[u+10]|Cr_Table[v+10]];
      *dst2++ = Pixel[Y_Table[Y2+6]|Cb_Table[u+6]|Cr_Table[v+6]];
    }

    if (j==0)
      py -= gst_AllGloballs.Coded_Picture_Width;
    else
      py += gst_AllGloballs.Coded_Picture_Width;

    Y2_ptr += gst_AllGloballs.Coded_Picture_Width;
    dst += gst_AllGloballs.Coded_Picture_Width;
    dst2 += gst_AllGloballs.Coded_Picture_Width;

    if (gst_AllGloballs.chroma_format==CHROMA420)
    {
      pu -= gst_AllGloballs.Chroma_Width;
      pv -= gst_AllGloballs.Chroma_Width;
    }
    else
    {
      pu += gst_AllGloballs.Chroma_Width;
      pv += gst_AllGloballs.Chroma_Width;
    }

    /* line j + 2. j + 3 */
    for (i=0; i<gst_AllGloballs.Coded_Picture_Width; i+=4)
    {
      y = *py++;
      Y2 = *Y2_ptr++;
      u = *pu++ >> 1;
      v = *pv++ >> 1;
      *dst++  = Pixel[Y_Table[((y+Y2)>>1)+3]|Cb_Table[u+3]|Cr_Table[v+3]];
      *dst2++ = Pixel[Y_Table[Y2+15]|Cb_Table[u+15]|Cr_Table[v+15]];

      y = *py++;
      Y2 = *Y2_ptr++;
      if (gst_AllGloballs.chroma_format==CHROMA444)
      {
        u = *pu++ >> 1;
        v = *pv++ >> 1;
      }
      *dst++  = Pixel[Y_Table[((y+Y2)>>1)+11]|Cb_Table[u+11]|Cr_Table[v+11]];
      *dst2++ = Pixel[Y_Table[Y2+7]|Cb_Table[u+7]|Cr_Table[v+7]];

      y = *py++;
      Y2 = *Y2_ptr++;
      u = *pu++ >> 1;
      v = *pv++ >> 1;
      *dst++  = Pixel[Y_Table[((y+Y2)>>1)+1]|Cb_Table[u+1]|Cr_Table[v+1]];
      *dst2++ = Pixel[Y_Table[Y2+13]|Cb_Table[u+13]|Cr_Table[v+13]];

      y = *py++;
      Y2 = *Y2_ptr++;
      if (gst_AllGloballs.chroma_format==CHROMA444)
      {
        u = *pu++ >> 1;
        v = *pv++ >> 1;
      }
      *dst++  = Pixel[Y_Table[((y+Y2)>>1)+9]|Cb_Table[u+9]|Cr_Table[v+9]];
      *dst2++ = Pixel[Y_Table[Y2+5]|Cb_Table[u+5]|Cr_Table[v+5]];
    }

    py += gst_AllGloballs.Coded_Picture_Width;
    Y2_ptr += gst_AllGloballs.Coded_Picture_Width;
    dst += gst_AllGloballs.Coded_Picture_Width;
    dst2 += gst_AllGloballs.Coded_Picture_Width;
    pu += gst_AllGloballs.Chroma_Width;
    pv += gst_AllGloballs.Chroma_Width;
  }
}

#ifdef JADEFUSION
static void Dither_Top_Field420(unsigned char *src[], Pxl_32 *dst)
#else
static void Dither_Top_Field420(src,dst)
unsigned char *src[];
Pxl_32 *dst;
#endif
{
  int i,j;
  int Y1,Cb1,Cr1,Y2,Cb2,Cr2;
  unsigned char *Y1_ptr,*Cb1_ptr,*Cr1_ptr,*Y2_ptr,*Cb2_ptr,*Cr2_ptr;
	  Pxl_32 *dst2;

  Y1_ptr = src[0];
  Cb1_ptr = src[1];
  Cr1_ptr = src[2];

  Y2_ptr = Y1_ptr + (gst_AllGloballs.Coded_Picture_Width<<1);
  Cb2_ptr = Cb1_ptr + (gst_AllGloballs.Chroma_Width<<1);
  Cr2_ptr = Cr1_ptr + (gst_AllGloballs.Chroma_Width<<1);

  dst2 = dst + gst_AllGloballs.Coded_Picture_Width;

  for (j=0; j<gst_AllGloballs.Coded_Picture_Height; j+=4)
  {
    /* line j + 0, j + 1 */
    for (i=0; i<gst_AllGloballs.Coded_Picture_Width; i+=4)
    {
      Y1 = *Y1_ptr++;
      Y2 = *Y2_ptr++;
      Cb1 = *Cb1_ptr++ >> 1;
      Cr1 = *Cr1_ptr++ >> 1;
      Cb2 = *Cb2_ptr++ >> 1;
      Cr2 = *Cr2_ptr++ >> 1;
      *dst++  = Pixel[Y_Table[((3*Y1+Y2)>>2)]|Cb_Table[Cb1]|Cr_Table[Cr1]];
      *dst2++ = Pixel[Y_Table[((Y1+3*Y2)>>2)+12]|Cb_Table[((3*Cb1+Cb2)>>2)+12]
                                             |Cr_Table[((3*Cr1+Cr2)>>2)+12]];

      Y1 = *Y1_ptr++;
      Y2 = *Y2_ptr++;
      *dst++  = Pixel[Y_Table[((3*Y1+Y2)>>2)+8]|Cb_Table[Cb1+8]|Cr_Table[Cr1+8]];
      *dst2++ = Pixel[Y_Table[((Y1+3*Y2)>>2)+4]|Cb_Table[((3*Cb1+Cb2)>>2)+4]
                                            |Cr_Table[((3*Cr1+Cr2)>>2)+4]];

      Y1 = *Y1_ptr++;
      Y2 = *Y2_ptr++;
      Cb1 = *Cb1_ptr++ >> 1;
      Cr1 = *Cr1_ptr++ >> 1;
      Cb2 = *Cb2_ptr++ >> 1;
      Cr2 = *Cr2_ptr++ >> 1;
      *dst++  = Pixel[Y_Table[((3*Y1+Y2)>>2)+2]|Cb_Table[Cb1+2]|Cr_Table[Cr1+2]];
      *dst2++ = Pixel[Y_Table[((Y1+3*Y2)>>2)+14]|Cb_Table[((3*Cb1+Cb2)>>2)+14]
                                             |Cr_Table[((3*Cr1+Cr2)>>2)+14]];

      Y1 = *Y1_ptr++;
      Y2 = *Y2_ptr++;
      *dst++  = Pixel[Y_Table[((3*Y1+Y2)>>2)+10]|Cb_Table[Cb1+10]|Cr_Table[Cr1+10]];
      *dst2++ = Pixel[Y_Table[((Y1+3*Y2)>>2)+6]|Cb_Table[((3*Cb1+Cb2)>>2)+6]
                                            |Cr_Table[((3*Cr1+Cr2)>>2)+6]];
    }

    Y1_ptr += gst_AllGloballs.Coded_Picture_Width;

    if (j!=(gst_AllGloballs.Coded_Picture_Height-4))
      Y2_ptr += gst_AllGloballs.Coded_Picture_Width;
    else
      Y2_ptr -= gst_AllGloballs.Coded_Picture_Width;

    Cb1_ptr -= gst_AllGloballs.Chroma_Width;
    Cr1_ptr -= gst_AllGloballs.Chroma_Width;
    Cb2_ptr -= gst_AllGloballs.Chroma_Width;
    Cr2_ptr -= gst_AllGloballs.Chroma_Width;

    dst  += gst_AllGloballs.Coded_Picture_Width;
    dst2 += gst_AllGloballs.Coded_Picture_Width;

    /* line j + 2, j + 3 */
    for (i=0; i<gst_AllGloballs.Coded_Picture_Width; i+=4)
    {
      Y1 = *Y1_ptr++;
      Y2 = *Y2_ptr++;
      Cb1 = *Cb1_ptr++ >> 1;
      Cr1 = *Cr1_ptr++ >> 1;
      Cb2 = *Cb2_ptr++ >> 1;
      Cr2 = *Cr2_ptr++ >> 1;
      *dst++  = Pixel[Y_Table[((3*Y1+Y2)>>2)+3]|Cb_Table[((Cb1+Cb2)>>1)+3]
                                            |Cr_Table[((Cr1+Cr2)>>1)+3]];
      *dst2++ = Pixel[Y_Table[((Y1+3*Y2)>>2)+15]|Cb_Table[((Cb1+3*Cb2)>>2)+15]
                                             |Cr_Table[((Cr1+3*Cr2)>>2)+15]];

      Y1 = *Y1_ptr++;
      Y2 = *Y2_ptr++;
      *dst++  = Pixel[Y_Table[((3*Y1+Y2)>>2)+11]|Cb_Table[((Cb1+Cb2)>>1)+11]
                                             |Cr_Table[((Cr1+Cr2)>>1)+11]];
      *dst2++ = Pixel[Y_Table[((Y1+3*Y2)>>2)+7]|Cb_Table[((Cb1+3*Cb2)>>2)+7]
                                            |Cr_Table[((Cr1+3*Cr2)>>2)+7]];

      Y1 = *Y1_ptr++;
      Y2 = *Y2_ptr++;
      Cb1 = *Cb1_ptr++ >> 1;
      Cr1 = *Cr1_ptr++ >> 1;
      Cb2 = *Cb2_ptr++ >> 1;
      Cr2 = *Cr2_ptr++ >> 1;
      *dst++  = Pixel[Y_Table[((3*Y1+Y2)>>2)+1]|Cb_Table[((Cb1+Cb2)>>1)+1]
                                            |Cr_Table[((Cr1+Cr2)>>1)+1]];
      *dst2++ = Pixel[Y_Table[((Y1+3*Y2)>>2)+13]|Cb_Table[((Cb1+3*Cb2)>>2)+13]
                                             |Cr_Table[((Cr1+3*Cr2)>>2)+13]];

      Y1 = *Y1_ptr++;
      Y2 = *Y2_ptr++;
      *dst++  = Pixel[Y_Table[((3*Y1+Y2)>>2)+9]|Cb_Table[((Cb1+Cb2)>>1)+9]
                                            |Cr_Table[((Cr1+Cr2)>>1)+9]];
      *dst2++ = Pixel[Y_Table[((Y1+3*Y2)>>2)+5]|Cb_Table[((Cb1+3*Cb2)>>2)+5]
                                            |Cr_Table[((Cr1+3*Cr2)>>2)+5]];
    }

    Y1_ptr += gst_AllGloballs.Coded_Picture_Width;
    Y2_ptr += gst_AllGloballs.Coded_Picture_Width;
    Cb1_ptr += gst_AllGloballs.Chroma_Width;
    Cr1_ptr += gst_AllGloballs.Chroma_Width;
    if (j!=(gst_AllGloballs.Coded_Picture_Height-8))
    {
      Cb2_ptr += gst_AllGloballs.Chroma_Width;
      Cr2_ptr += gst_AllGloballs.Chroma_Width;
    }
    else
    {
      Cb2_ptr -= gst_AllGloballs.Chroma_Width;
      Cr2_ptr -= gst_AllGloballs.Chroma_Width;
    }
    dst += gst_AllGloballs.Coded_Picture_Width;
    dst2+= gst_AllGloballs.Coded_Picture_Width;
  }
}

#ifdef JADEFUSION
static void Dither_Bottom_Field420(unsigned char *src[], Pxl_32 *dst)
#else
static void Dither_Bottom_Field420(src,dst)
unsigned char *src[];
Pxl_32 *dst;
#endif
{
  int i,j;
  int Y1,Cb1,Cr1,Y2,Cb2,Cr2;
  unsigned char *Y1_ptr,*Cb1_ptr,*Cr1_ptr,*Y2_ptr,*Cb2_ptr,*Cr2_ptr;
	  Pxl_32 *dst2;

  Y2_ptr = Y1_ptr = src[0] + gst_AllGloballs.Coded_Picture_Width;
  Cb2_ptr = Cb1_ptr = src[1] + gst_AllGloballs.Chroma_Width;
  Cr2_ptr = Cr1_ptr = src[2] + gst_AllGloballs.Chroma_Width;

  dst2 = dst;

  for (j=0; j<gst_AllGloballs.Coded_Picture_Height; j+=4)
  {
    /* line j + 0, j + 1 */
    for (i=0; i<gst_AllGloballs.Coded_Picture_Width; i+=4)
    {
      Y1 = *Y1_ptr++;
      Y2 = *Y2_ptr++;
      Cb1 = *Cb1_ptr++ >> 1;
      Cr1 = *Cr1_ptr++ >> 1;
      Cb2 = *Cb2_ptr++ >> 1;
      Cr2 = *Cr2_ptr++ >> 1;
      *dst++  = Pixel[Y_Table[((3*Y1+Y2)>>2)+15]|Cb_Table[((3*Cb1+Cb2)>>2)+15]
                                             |Cr_Table[((3*Cr1+Cr2)>>2)+15]];
      *dst2++ = Pixel[Y_Table[((Y1+3*Y2)>>2)]|Cb_Table[((Cb1+Cb2)>>1)]
                                          |Cr_Table[((Cr1+Cr2)>>1)]];

      Y1 = *Y1_ptr++;
      Y2 = *Y2_ptr++;
      *dst++  = Pixel[Y_Table[((3*Y1+Y2)>>2)+7]|Cb_Table[((3*Cb1+Cb2)>>2)+7]
                                            |Cr_Table[((3*Cr1+Cr2)>>2)+7]];
      *dst2++ = Pixel[Y_Table[((Y1+3*Y2)>>2)+8]|Cb_Table[((Cb1+Cb2)>>1)+8]
                                            |Cr_Table[((Cr1+Cr2)>>1)+8]];

      Y1 = *Y1_ptr++;
      Y2 = *Y2_ptr++;
      Cb1 = *Cb1_ptr++ >> 1;
      Cr1 = *Cr1_ptr++ >> 1;
      Cb2 = *Cb2_ptr++ >> 1;
      Cr2 = *Cr2_ptr++ >> 1;
      *dst++  = Pixel[Y_Table[((3*Y1+Y2)>>2)+13]|Cb_Table[((3*Cb1+Cb2)>>2)+13]
                                             |Cr_Table[((3*Cr1+Cr2)>>2)+13]];
      *dst2++ = Pixel[Y_Table[((Y1+3*Y2)>>2)+2]|Cb_Table[((Cb1+Cb2)>>1)+2]
                                            |Cr_Table[((Cr1+Cr2)>>1)+2]];

      Y1 = *Y1_ptr++;
      Y2 = *Y2_ptr++;
      *dst++  = Pixel[Y_Table[((3*Y1+Y2)>>2)+5]|Cb_Table[((3*Cb1+Cb2)>>2)+5]
                                            |Cr_Table[((3*Cr1+Cr2)>>2)+5]];
      *dst2++ = Pixel[Y_Table[((Y1+3*Y2)>>2)+10]|Cb_Table[((Cb1+Cb2)>>1)+10]
                                             |Cr_Table[((Cr1+Cr2)>>1)+10]];
    }

    if (j!=0)
      Y1_ptr += gst_AllGloballs.Coded_Picture_Width;
    else
      Y1_ptr -= gst_AllGloballs.Coded_Picture_Width;

    Y2_ptr += gst_AllGloballs.Coded_Picture_Width;

    Cb1_ptr -= gst_AllGloballs.Chroma_Width;
    Cr1_ptr -= gst_AllGloballs.Chroma_Width;
    Cb2_ptr -= gst_AllGloballs.Chroma_Width;
    Cr2_ptr -= gst_AllGloballs.Chroma_Width;

    if (j!=0)
      dst  += gst_AllGloballs.Coded_Picture_Width;

    dst2 += gst_AllGloballs.Coded_Picture_Width;

    /* line j + 2, j + 3 */
    for (i=0; i<gst_AllGloballs.Coded_Picture_Width; i+=4)
    {
      Y1 = *Y1_ptr++;
      Y2 = *Y2_ptr++;
      Cb1 = *Cb1_ptr++ >> 1;
      Cr1 = *Cr1_ptr++ >> 1;
      Cb2 = *Cb2_ptr++ >> 1;
      Cr2 = *Cr2_ptr++ >> 1;
      *dst++  = Pixel[Y_Table[((3*Y1+Y2)>>2)+12]|Cb_Table[((Cb1+3*Cb2)>>2)+12]
                                             |Cr_Table[((Cr1+3*Cr2)>>2)+12]];
      *dst2++ = Pixel[Y_Table[((Y1+3*Y2)>>2)+3]|Cb_Table[Cb2+3]
                                            |Cr_Table[Cr2+3]];

      Y1 = *Y1_ptr++;
      Y2 = *Y2_ptr++;
      *dst++  = Pixel[Y_Table[((3*Y1+Y2)>>2)+4]|Cb_Table[((Cb1+3*Cb2)>>2)+4]
                                            |Cr_Table[((Cr1+3*Cr2)>>2)+4]];
      *dst2++ = Pixel[Y_Table[((Y1+3*Y2)>>2)+11]|Cb_Table[Cb2+11]
                                             |Cr_Table[Cr2+11]];

      Y1 = *Y1_ptr++;
      Y2 = *Y2_ptr++;
      Cb1 = *Cb1_ptr++ >> 1;
      Cr1 = *Cr1_ptr++ >> 1;
      Cb2 = *Cb2_ptr++ >> 1;
      Cr2 = *Cr2_ptr++ >> 1;
      *dst++  = Pixel[Y_Table[((3*Y1+Y2)>>2)+14]|Cb_Table[((Cb1+3*Cb2)>>2)+14]
                                             |Cr_Table[((Cr1+3*Cr2)>>2)+14]];
      *dst2++ = Pixel[Y_Table[((Y1+3*Y2)>>2)+1]|Cb_Table[Cb2+1]
                                            |Cr_Table[Cr2+1]];

      Y1 = *Y1_ptr++;
      Y2 = *Y2_ptr++;
      *dst++  = Pixel[Y_Table[((3*Y1+Y2)>>2)+6]|Cb_Table[((Cb1+3*Cb2)>>2)+6]
                                            |Cr_Table[((Cr1+3*Cr2)>>2)+6]];
      *dst2++ = Pixel[Y_Table[((Y1+3*Y2)>>2)+9]|Cb_Table[Cb2+9]
                                            |Cr_Table[Cr2+9]];
    }

    Y1_ptr += gst_AllGloballs.Coded_Picture_Width;
    Y2_ptr += gst_AllGloballs.Coded_Picture_Width;

    if (j!=0)
    {
      Cb1_ptr += gst_AllGloballs.Chroma_Width;
      Cr1_ptr += gst_AllGloballs.Chroma_Width;
    }
    else
    {
      Cb1_ptr -= gst_AllGloballs.Chroma_Width;
      Cr1_ptr -= gst_AllGloballs.Chroma_Width;
    }

    Cb2_ptr += gst_AllGloballs.Chroma_Width;
    Cr2_ptr += gst_AllGloballs.Chroma_Width;

    dst += gst_AllGloballs.Coded_Picture_Width;
    dst2+= gst_AllGloballs.Coded_Picture_Width;
  }

  Y2_ptr -= (gst_AllGloballs.Coded_Picture_Width<<1);
  Cb2_ptr -= (gst_AllGloballs.Chroma_Width<<1);
  Cr2_ptr -= (gst_AllGloballs.Chroma_Width<<1);

  /* dither last line */
  for (i=0; i<gst_AllGloballs.Coded_Picture_Width; i+=4)
  {
    Y1 = *Y1_ptr++;
    Y2 = *Y2_ptr++;
    Cb1 = *Cb1_ptr++ >> 1;
    Cr1 = *Cr1_ptr++ >> 1;
    Cb2 = *Cb2_ptr++ >> 1;
    Cr2 = *Cr2_ptr++ >> 1;
    *dst++  = Pixel[Y_Table[((3*Y1+Y2)>>2)+15]|Cb_Table[((3*Cb1+Cb2)>>2)+15]
                                           |Cr_Table[((3*Cr1+Cr2)>>2)+15]];

    Y1 = *Y1_ptr++;
    Y2 = *Y2_ptr++;
    *dst++  = Pixel[Y_Table[((3*Y1+Y2)>>2)+7]|Cb_Table[((3*Cb1+Cb2)>>2)+7]
                                          |Cr_Table[((3*Cr1+Cr2)>>2)+7]];

    Y1 = *Y1_ptr++;
    Y2 = *Y2_ptr++;
    Cb1 = *Cb1_ptr++ >> 1;
    Cr1 = *Cr1_ptr++ >> 1;
    Cb2 = *Cb2_ptr++ >> 1;
    Cr2 = *Cr2_ptr++ >> 1;
    *dst++  = Pixel[Y_Table[((3*Y1+Y2)>>2)+13]|Cb_Table[((3*Cb1+Cb2)>>2)+13]
                                           |Cr_Table[((3*Cr1+Cr2)>>2)+13]];

    Y1 = *Y1_ptr++;
    Y2 = *Y2_ptr++;
    *dst++  = Pixel[Y_Table[((3*Y1+Y2)>>2)+5]|Cb_Table[((3*Cb1+Cb2)>>2)+5]
                                          |Cr_Table[((3*Cr1+Cr2)>>2)+5]];
    }

}
