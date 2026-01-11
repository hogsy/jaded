/* idct.c, inverse fast discrete cosine transform                           */

/**********************************************************/
/* inverse two dimensional DCT, Chen-Wang algorithm       */
/* (cf. IEEE ASSP-32, pp. 803-816, Aug. 1984)             */
/* 32-bit integer arithmetic (8 bit coefficients)         */
/* 11 mults, 29 adds per DCT                              */
/*                                      sE, 18.8.91       */
/**********************************************************/
/* coefficients extended to 12 bit for IEEE1180-1990      */
/* compliance                           sE,  2.1.94       */
/**********************************************************/

/* this code assumes >> to be a two's-complement arithmetic */
/* right shift: (-2)>>1 == -1 , (-3)>>1 == -2               */

#include "config.h"
#include "global.h"
#include <math.h>

#include "config.h"

#ifndef PI
# ifdef M_PI
#  define PI M_PI
# else
#  define PI 3.14159265358979323846
# endif
#endif

/* global declarations */
void Initialize_Fast_IDCTref (void);
void Reference_IDCT (short *block);

/* private data */

/* cosine transform matrix for 8x1 IDCT */
static float c[8][8];
static double   MATH_gd_Decal ;
/* initialize DCT coefficient matrix */

void Initialize_Reference_IDCT()
{
	int freq, time;
	float scale;
	
	for (freq=0; freq < 8; freq++)
	{
		scale = (freq == 0) ? (float)sqrt(0.125) : 0.5f;
		for (time=0; time<8; time++)
			c[freq][time] = scale*(float)cos((PI/8.0)*freq*(time + 0.5));
	}
#ifdef JADEFUSION
	MATH_gd_Decal = 3.0F * pow(2.0f, 51.0f);
#else
	MATH_gd_Decal = 3.0F * pow(2, 51);
#endif
}

/* perform IDCT matrix multiply for 8x8 coefficient block */
__inline int lFloatToLongOpt(float a)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    double  b;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    b = a + MATH_gd_Decal;
    return (*((int *) &(b)));
}

void Reference_IDCT(short *block)
{
	int i, j;
	float partial_product;
	float tmp[64];
	float Localblock[8];
	float *LocalTmp;

	for (i=0; i<8; i++)
	{
		Localblock[0] = (float)block[8*i + 0];
		Localblock[1] = (float)block[8*i + 1];
		Localblock[2] = (float)block[8*i + 2];
		Localblock[3] = (float)block[8*i + 3];
		Localblock[4] = (float)block[8*i + 4];
		Localblock[5] = (float)block[8*i + 5];
		Localblock[6] = (float)block[8*i + 6];
		Localblock[7] = (float)block[8*i + 7];//*/
		for (j=0; j<8; j++)
		{
			partial_product  = c[0][j]*Localblock[0];
			partial_product += c[1][j]*Localblock[1];
			partial_product += c[2][j]*Localblock[2];
			partial_product += c[3][j]*Localblock[3];
			partial_product += c[4][j]*Localblock[4];
			partial_product += c[5][j]*Localblock[5];
			partial_product += c[6][j]*Localblock[6];
			partial_product += c[7][j]*Localblock[7];//*/
			tmp[8*i+j] = partial_product;
		}
	}
	/* Transpose operation is integrated into address mapping by switching 
	loop order of i and j */

	for (j=0; j<8; j++)
	{
		LocalTmp = &tmp[j];
		for (i=0; i<8; i++)
		{
			partial_product  = c[0][i]*LocalTmp[8*0];
			partial_product += c[1][i]*LocalTmp[8*1];
			partial_product += c[2][i]*LocalTmp[8*2];
			partial_product += c[3][i]*LocalTmp[8*3];
			partial_product += c[4][i]*LocalTmp[8*4];
			partial_product += c[5][i]*LocalTmp[8*5];
			partial_product += c[6][i]*LocalTmp[8*6];
			partial_product += c[7][i]*LocalTmp[8*7];//*/
			block[8*i+j] = lFloatToLongOpt(partial_product);
		}
	}
/*	for (j=0; j<8; j++)
	{
		for (i=0; i<8; i++)
		{
			if ((i == 0) || (j == 0))
			 block[8*i+j] = 128;
		}
	}//*/
}


#define W1 2841 /* 2048*sqrt(2)*cos(1*pi/16) */
#define W2 2676 /* 2048*sqrt(2)*cos(2*pi/16) */
#define W3 2408 /* 2048*sqrt(2)*cos(3*pi/16) */
#define W5 1609 /* 2048*sqrt(2)*cos(5*pi/16) */
#define W6 1108 /* 2048*sqrt(2)*cos(6*pi/16) */
#define W7 565  /* 2048*sqrt(2)*cos(7*pi/16) */

/* global declarations */
void Initialize_Fast_IDCT (void);
void Fast_IDCT (short *block);

/* private data */
static short iclip[1024]; /* clipping table */
static short *iclp;

/* private prototypes */
static void idctrow (short *blk);
static void idctcol (short *blk);

/* row (horizontal) IDCT
 *
 *           7                       pi         1
 * dst[k] = sum c[l] * src[l] * cos( -- * ( k + - ) * l )
 *          l=0                      8          2
 *
 * where: c[0]    = 128
 *        c[1..7] = 128*sqrt(2)
 */

static void idctrow(short *blk)
{
  int x0, x1, x2, x3, x4, x5, x6, x7, x8;

  /* shortcut */
  if (!((x1 = blk[4]<<11) | (x2 = blk[6]) | (x3 = blk[2]) |
        (x4 = blk[1]) | (x5 = blk[7]) | (x6 = blk[5]) | (x7 = blk[3])))//*/
  {
    blk[0]=blk[1]=blk[2]=blk[3]=blk[4]=blk[5]=blk[6]=blk[7]=blk[0]<<3;
    return;//*/
  }

  x0 = (blk[0]<<11) + 128; /* for proper rounding in the fourth stage */

  /* first stage */
  x8 = W7*(x4+x5);
  x4 = x8 + (W1-W7)*x4;
  x5 = x8 - (W1+W7)*x5;
  x8 = W3*(x6+x7);
  x6 = x8 - (W3-W5)*x6;
  x7 = x8 - (W3+W5)*x7;
  
  /* second stage */
  x8 = x0 + x1;
  x0 -= x1;
  x1 = W6*(x3+x2);
  x2 = x1 - (W2+W6)*x2;
  x3 = x1 + (W2-W6)*x3;
  x1 = x4 + x6;
  x4 -= x6;
  x6 = x5 + x7;
  x5 -= x7;
  
  /* third stage */
  x7 = x8 + x3;
  x8 -= x3;
  x3 = x0 + x2;
  x0 -= x2;
  x2 = (181*(x4+x5)+128)>>8;
  x4 = (181*(x4-x5)+128)>>8;
  
  /* fourth stage */
  blk[0] = (x7+x1)>>8;
  blk[1] = (x3+x2)>>8;
  blk[2] = (x0+x4)>>8;
  blk[3] = (x8+x6)>>8;
  blk[4] = (x8-x6)>>8;
  blk[5] = (x0-x4)>>8;
  blk[6] = (x3-x2)>>8;
  blk[7] = (x7-x1)>>8;
}

static void idctrow_FAST(short *blk)
{
   blk[0]=blk[1]=blk[2]=blk[3]=blk[4]=blk[5]=blk[6]=blk[7]=blk[0]<<3;
//   blk[0]=blk[1]=blk[2]=blk[3]=blk[4]=blk[5]=blk[6]=blk[7]=blk[0]=0xffff;
}
static void idctcol_FAST(short *blk)
{
   blk[8*0]=blk[8*1]=blk[8*2]=blk[8*3]=blk[8*4]=blk[8*5]=blk[8*6]=blk[8*7]=iclp[(blk[8*0]+32)>>6];
//   blk[8*0]=blk[8*1]=blk[8*2]=blk[8*3]=blk[8*4]=blk[8*5]=blk[8*6]=blk[8*7]=0xffff;
}

/* column (vertical) IDCT
 *
 *             7                         pi         1
 * dst[8*k] = sum c[l] * src[8*l] * cos( -- * ( k + - ) * l )
 *            l=0                        8          2
 *
 * where: c[0]    = 1/1024
 *        c[1..7] = (1/1024)*sqrt(2)
 */
static void idctcol(short *blk)
{
  int x0, x1, x2, x3, x4, x5, x6, x7, x8;

  /* shortcut */
  if (!((x1 = (blk[8*4]<<8)) | (x2 = blk[8*6]) | (x3 = blk[8*2]) |
        (x4 = blk[8*1]) | (x5 = blk[8*7]) | (x6 = blk[8*5]) | (x7 = blk[8*3])))//*/
  {
    blk[8*0]=blk[8*1]=blk[8*2]=blk[8*3]=blk[8*4]=blk[8*5]=blk[8*6]=blk[8*7]=
      iclp[(blk[8*0]+32)>>6];
    return;//*/
  }

  x0 = (blk[8*0]<<8) + 8192;

  /* first stage */
  x8 = W7*(x4+x5) + 4;
  x4 = (x8+(W1-W7)*x4)>>3;
  x5 = (x8-(W1+W7)*x5)>>3;
  x8 = W3*(x6+x7) + 4;
  x6 = (x8-(W3-W5)*x6)>>3;
  x7 = (x8-(W3+W5)*x7)>>3;
  
  /* second stage */
  x8 = x0 + x1;
  x0 -= x1;
  x1 = W6*(x3+x2) + 4;
  x2 = (x1-(W2+W6)*x2)>>3;
  x3 = (x1+(W2-W6)*x3)>>3;
  x1 = x4 + x6;
  x4 -= x6;
  x6 = x5 + x7;
  x5 -= x7;
  
  /* third stage */
  x7 = x8 + x3;
  x8 -= x3;
  x3 = x0 + x2;
  x0 -= x2;
  x2 = (181*(x4+x5)+128)>>8;
  x4 = (181*(x4-x5)+128)>>8;
  
  /* fourth stage */
/*  blk[8*0] = iclp[(x7+x1)>>14];
  blk[8*1] = iclp[(x3+x2)>>14];
  blk[8*2] = iclp[(x0+x4)>>14];
  blk[8*3] = iclp[(x8+x6)>>14];
  blk[8*4] = iclp[(x8-x6)>>14];
  blk[8*5] = iclp[(x0-x4)>>14];
  blk[8*6] = iclp[(x3-x2)>>14];
  blk[8*7] = iclp[(x7-x1)>>14];//*/
/*  blk[8*7] = (x7+x1)>>14;
  blk[8*6] = (x3+x2)>>14;
  blk[8*5] = (x0+x4)>>14;
  blk[8*4] = (x8+x6)>>14;
  blk[8*3] = (x8-x6)>>14;
  blk[8*2] = (x0-x4)>>14;
  blk[8*1] = (x3-x2)>>14;
  blk[8*0] = (x7-x1)>>14;//*/
  blk[8*0] = (x7+x1)>>14;
  blk[8*1] = (x3+x2)>>14;
  blk[8*2] = (x0+x4)>>14;
  blk[8*3] = (x8+x6)>>14;
  blk[8*4] = (x8-x6)>>14;
  blk[8*5] = (x0-x4)>>14;
  blk[8*6] = (x3-x2)>>14;
  blk[8*7] = (x7-x1)>>14;//*/
}

/* two dimensional inverse discrete cosine transform */
#ifdef JADEFUSION
void Fast_IDCT(short *block)
#else
void Fast_IDCT(block)
short *block;
#endif
{
  int i;
  if (gst_AllGloballs.ulFLAGS & MPEG_Mode_BigDots)
  {
	  for (i=0; i<8; i++)
		  idctrow_FAST(block+8*i);
	  
	  for (i=0; i<8; i++)
		  idctcol_FAST(block+i);
	  return;
  }
  for (i=0; i<8; i++)
	idctrow(block+8*i);

  for (i=0; i<8; i++)
	  idctcol(block+i);
}

void Initialize_Fast_IDCT()
{
  int i;

  iclp = iclip+512;
  for (i= -512; i<512; i++)
    iclp[i] = (i<-256) ? -256 : ((i>255) ? 255 : i);
}
