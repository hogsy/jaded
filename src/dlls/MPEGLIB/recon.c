/* Predict.c, motion compensation routines                                    */

/* Copyright (C) 1996, MPEG Software Simulation Group. All Rights Reserved. */


#include <stdio.h>

#include "config.h"
#include "global.h"

/* private prototypes */
static void form_prediction (unsigned char *src[], int sfield,
  unsigned char *dst[], int dfield,
  int lx, int lx2, int w, int h, int x, int y, int dx, int dy,
  int average_flag);

static void form_component_prediction (unsigned char *src, unsigned char *dst,
  int lx, int lx2, int w, int h, int x, int y, int dx, int dy, int average_flag);

#ifdef JADEFUSION
void form_predictions(int bx, int by, int macroblock_type, int motion_type, int PMV[2][2][2], int motion_vertical_field_select[2][2], int dmvector[2], int stwtype)
#else
void form_predictions(bx,by,macroblock_type,motion_type,PMV,motion_vertical_field_select,dmvector,stwtype)
int bx, by;
int macroblock_type;
int motion_type;
int PMV[2][2][2], motion_vertical_field_select[2][2], dmvector[2];
int stwtype;
#endif
{
  int currentfield;
  unsigned char **predframe;
  int DMV[2][2];
  int stwtop, stwbot;

  stwtop = stwtype%3; /* 0:temporal, 1:(spat+temp)/2, 2:spatial */
  stwbot = stwtype/3;

  if ((macroblock_type & MACROBLOCK_MOTION_FORWARD) 
   || (gst_AllGloballs.picture_coding_type==P_TYPE))
  {
    if (gst_AllGloballs.picture_structure==FRAME_PICTURE)
    {
      if ((motion_type==MC_FRAME) 
        || !(macroblock_type & MACROBLOCK_MOTION_FORWARD))
      {
        /* frame-based prediction (broken into top and bottom halves
             for spatial scalability prediction purposes) */
        if (stwtop<2)
          form_prediction(gst_AllGloballs.forward_reference_frame,0,gst_AllGloballs.current_frame,0,
            gst_AllGloballs.Coded_Picture_Width,gst_AllGloballs.Coded_Picture_Width<<1,16,8,bx,by,
            PMV[0][0][0],PMV[0][0][1],stwtop);

        if (stwbot<2)
          form_prediction(gst_AllGloballs.forward_reference_frame,1,gst_AllGloballs.current_frame,1,
            gst_AllGloballs.Coded_Picture_Width,gst_AllGloballs.Coded_Picture_Width<<1,16,8,bx,by,
            PMV[0][0][0],PMV[0][0][1],stwbot);
      }
      else if (motion_type==MC_FIELD) /* field-based prediction */
      {
        /* top field prediction */
        if (stwtop<2)
          form_prediction(gst_AllGloballs.forward_reference_frame,motion_vertical_field_select[0][0],
            gst_AllGloballs.current_frame,0,gst_AllGloballs.Coded_Picture_Width<<1,gst_AllGloballs.Coded_Picture_Width<<1,16,8,
            bx,by>>1,PMV[0][0][0],PMV[0][0][1]>>1,stwtop);

        /* bottom field prediction */
        if (stwbot<2)
          form_prediction(gst_AllGloballs.forward_reference_frame,motion_vertical_field_select[1][0],
            gst_AllGloballs.current_frame,1,gst_AllGloballs.Coded_Picture_Width<<1,gst_AllGloballs.Coded_Picture_Width<<1,16,8,
            bx,by>>1,PMV[1][0][0],PMV[1][0][1]>>1,stwbot);
      }
      else if (motion_type==MC_DMV) /* dual prime prediction */
      {
        /* calculate derived motion vectors */
        Dual_Prime_Arithmetic(DMV,dmvector,PMV[0][0][0],PMV[0][0][1]>>1);

        if (stwtop<2)
        {
          /* predict top field from top field */
          form_prediction(gst_AllGloballs.forward_reference_frame,0,gst_AllGloballs.current_frame,0,
            gst_AllGloballs.Coded_Picture_Width<<1,gst_AllGloballs.Coded_Picture_Width<<1,16,8,bx,by>>1,
            PMV[0][0][0],PMV[0][0][1]>>1,0);

          /* predict and add to top field from bottom field */
          form_prediction(gst_AllGloballs.forward_reference_frame,1,gst_AllGloballs.current_frame,0,
            gst_AllGloballs.Coded_Picture_Width<<1,gst_AllGloballs.Coded_Picture_Width<<1,16,8,bx,by>>1,
            DMV[0][0],DMV[0][1],1);
        }

        if (stwbot<2)
        {
          /* predict bottom field from bottom field */
          form_prediction(gst_AllGloballs.forward_reference_frame,1,gst_AllGloballs.current_frame,1,
            gst_AllGloballs.Coded_Picture_Width<<1,gst_AllGloballs.Coded_Picture_Width<<1,16,8,bx,by>>1,
            PMV[0][0][0],PMV[0][0][1]>>1,0);

          /* predict and add to bottom field from top field */
          form_prediction(gst_AllGloballs.forward_reference_frame,0,gst_AllGloballs.current_frame,1,
            gst_AllGloballs.Coded_Picture_Width<<1,gst_AllGloballs.Coded_Picture_Width<<1,16,8,bx,by>>1,
            DMV[1][0],DMV[1][1],1);
        }
      }
    }
    else /* TOP_FIELD or BOTTOM_FIELD */
    {
      /* field picture */
      currentfield = (gst_AllGloballs.picture_structure==BOTTOM_FIELD);

      /* determine which frame to use for prediction */
      if ((gst_AllGloballs.picture_coding_type==P_TYPE) && gst_AllGloballs.Second_Field
         && (currentfield!=motion_vertical_field_select[0][0]))
        predframe = gst_AllGloballs.backward_reference_frame; /* same frame */
      else
        predframe = gst_AllGloballs.forward_reference_frame; /* previous frame */

      if ((motion_type==MC_FIELD)
        || !(macroblock_type & MACROBLOCK_MOTION_FORWARD))
      {
        /* field-based prediction */
        if (stwtop<2)
          form_prediction(predframe,motion_vertical_field_select[0][0],gst_AllGloballs.current_frame,0,
            gst_AllGloballs.Coded_Picture_Width<<1,gst_AllGloballs.Coded_Picture_Width<<1,16,16,bx,by,
            PMV[0][0][0],PMV[0][0][1],stwtop);
      }
      else if (motion_type==MC_16X8)
      {
        if (stwtop<2)
        {
          form_prediction(predframe,motion_vertical_field_select[0][0],gst_AllGloballs.current_frame,0,
            gst_AllGloballs.Coded_Picture_Width<<1,gst_AllGloballs.Coded_Picture_Width<<1,16,8,bx,by,
            PMV[0][0][0],PMV[0][0][1],stwtop);

          /* determine which frame to use for lower half prediction */
          if ((gst_AllGloballs.picture_coding_type==P_TYPE) && gst_AllGloballs.Second_Field
             && (currentfield!=motion_vertical_field_select[1][0]))
            predframe = gst_AllGloballs.backward_reference_frame; /* same frame */
          else
            predframe = gst_AllGloballs.forward_reference_frame; /* previous frame */

          form_prediction(predframe,motion_vertical_field_select[1][0],gst_AllGloballs.current_frame,0,
            gst_AllGloballs.Coded_Picture_Width<<1,gst_AllGloballs.Coded_Picture_Width<<1,16,8,bx,by+8,
            PMV[1][0][0],PMV[1][0][1],stwtop);
        }
      }
      else if (motion_type==MC_DMV) /* dual prime prediction */
      {
        if (gst_AllGloballs.Second_Field)
          predframe = gst_AllGloballs.backward_reference_frame; /* same frame */
        else
          predframe = gst_AllGloballs.forward_reference_frame; /* previous frame */

        /* calculate derived motion vectors */
        Dual_Prime_Arithmetic(DMV,dmvector,PMV[0][0][0],PMV[0][0][1]);

        /* predict from field of same parity */
        form_prediction(gst_AllGloballs.forward_reference_frame,currentfield,gst_AllGloballs.current_frame,0,
          gst_AllGloballs.Coded_Picture_Width<<1,gst_AllGloballs.Coded_Picture_Width<<1,16,16,bx,by,
          PMV[0][0][0],PMV[0][0][1],0);

        /* predict from field of opposite parity */
        form_prediction(predframe,!currentfield,gst_AllGloballs.current_frame,0,
          gst_AllGloballs.Coded_Picture_Width<<1,gst_AllGloballs.Coded_Picture_Width<<1,16,16,bx,by,
          DMV[0][0],DMV[0][1],1);
      }
    }
    stwtop = stwbot = 1;
  }

  if (macroblock_type & MACROBLOCK_MOTION_BACKWARD)
  {
    if (gst_AllGloballs.picture_structure==FRAME_PICTURE)
    {
      if (motion_type==MC_FRAME)
      {
        /* frame-based prediction */
        if (stwtop<2)
          form_prediction(gst_AllGloballs.backward_reference_frame,0,gst_AllGloballs.current_frame,0,
            gst_AllGloballs.Coded_Picture_Width,gst_AllGloballs.Coded_Picture_Width<<1,16,8,bx,by,
            PMV[0][1][0],PMV[0][1][1],stwtop);

        if (stwbot<2)
          form_prediction(gst_AllGloballs.backward_reference_frame,1,gst_AllGloballs.current_frame,1,
            gst_AllGloballs.Coded_Picture_Width,gst_AllGloballs.Coded_Picture_Width<<1,16,8,bx,by,
            PMV[0][1][0],PMV[0][1][1],stwbot);
      }
      else /* field-based prediction */
      {
        /* top field prediction */
        if (stwtop<2)
          form_prediction(gst_AllGloballs.backward_reference_frame,motion_vertical_field_select[0][1],
            gst_AllGloballs.current_frame,0,gst_AllGloballs.Coded_Picture_Width<<1,gst_AllGloballs.Coded_Picture_Width<<1,16,8,
            bx,by>>1,PMV[0][1][0],PMV[0][1][1]>>1,stwtop);

        /* bottom field prediction */
        if (stwbot<2)
          form_prediction(gst_AllGloballs.backward_reference_frame,motion_vertical_field_select[1][1],
            gst_AllGloballs.current_frame,1,gst_AllGloballs.Coded_Picture_Width<<1,gst_AllGloballs.Coded_Picture_Width<<1,16,8,
            bx,by>>1,PMV[1][1][0],PMV[1][1][1]>>1,stwbot);
      }
    }
    else /* TOP_FIELD or BOTTOM_FIELD */
    {
      /* field picture */
      if (motion_type==MC_FIELD)
      {
        /* field-based prediction */
        form_prediction(gst_AllGloballs.backward_reference_frame,motion_vertical_field_select[0][1],
          gst_AllGloballs.current_frame,0,gst_AllGloballs.Coded_Picture_Width<<1,gst_AllGloballs.Coded_Picture_Width<<1,16,16,
          bx,by,PMV[0][1][0],PMV[0][1][1],stwtop);
      }
      else if (motion_type==MC_16X8)
      {
        form_prediction(gst_AllGloballs.backward_reference_frame,motion_vertical_field_select[0][1],
          gst_AllGloballs.current_frame,0,gst_AllGloballs.Coded_Picture_Width<<1,gst_AllGloballs.Coded_Picture_Width<<1,16,8,
          bx,by,PMV[0][1][0],PMV[0][1][1],stwtop);

        form_prediction(gst_AllGloballs.backward_reference_frame,motion_vertical_field_select[1][1],
          gst_AllGloballs.current_frame,0,gst_AllGloballs.Coded_Picture_Width<<1,gst_AllGloballs.Coded_Picture_Width<<1,16,8,
          bx,by+8,PMV[1][1][0],PMV[1][1][1],stwtop);
      }
    }
  }
}

#ifdef JADEFUSION
static void form_prediction(
unsigned char *src[], /* prediction source buffer */
int sfield,           /* prediction source field number (0 or 1) */
unsigned char *dst[], /* prediction destination buffer */
int dfield,           /* prediction destination field number (0 or 1)*/
int lx, int lx2,      /* line strides */
int w, int h,         /* prediction block/sub-block width, height */
int x, int y,         /* pixel co-ordinates of top-left sample in current MB */
int dx, int dy,       /* horizontal, vertical prediction address */
int average_flag)     /* add prediction error to prediction ? */
#else
static void form_prediction(src,sfield,dst,dfield,lx,lx2,w,h,x,y,dx,dy,average_flag)
unsigned char *src[]; /* prediction source buffer */
int sfield;           /* prediction source field number (0 or 1) */
unsigned char *dst[]; /* prediction destination buffer */
int dfield;           /* prediction destination field number (0 or 1)*/
int lx,lx2;           /* line strides */
int w,h;              /* prediction block/sub-block width, height */
int x,y;              /* pixel co-ordinates of top-left sample in current MB */
int dx,dy;            /* horizontal, vertical prediction address */
int average_flag;     /* add prediction error to prediction ? */
#endif
{
  /* Y */
  /*MAX_SPD*/
  
  form_component_prediction(src[0]+(sfield?lx2>>1:0),dst[0]+(dfield?lx2>>1:0),
    lx,lx2,w,h,x,y,dx,dy,average_flag);

  if (gst_AllGloballs.ulFLAGS & MPEG_Mode_BlackAndWhite) return;

  if (gst_AllGloballs.chroma_format!=CHROMA444)
  {
    lx>>=1; lx2>>=1; w>>=1; x>>=1; dx/=2;
  }

  if (gst_AllGloballs.chroma_format==CHROMA420)
  {
    h>>=1; y>>=1; dy/=2;
  }

  /* Cb */
  form_component_prediction(src[1]+(sfield?lx2>>1:0),dst[1]+(dfield?lx2>>1:0),
    lx,lx2,w,h,x,y,dx,dy,average_flag);

  /* Cr */
  form_component_prediction(src[2]+(sfield?lx2>>1:0),dst[2]+(dfield?lx2>>1:0),
    lx,lx2,w,h,x,y,dx,dy,average_flag);//*/
}

/* ISO/IEC 13818-2 section 7.6.4: Forming predictions */
/* NOTE: the arithmetic below produces numerically equivalent results
 *  to 7.6.4, yet is more elegant. It differs in the following ways:
 *
 *   1. the vectors (dx, dy) are based on cartesian frame 
 *      coordiantes along a half-pel grid (always positive numbers)
 *      In contrast, vector[r][s][t] are differential (with positive and 
 *      negative values). As a result, deriving the integer vectors 
 *      (int_vec[t]) from dx, dy is accomplished by a simple right shift.
 *
 *   2. Half pel flags (xh, yh) are equivalent to the LSB (Least
 *      Significant Bit) of the half-pel coordinates (dx,dy).
 * 
 *
 *  NOTE: the work of combining predictions (ISO/IEC 13818-2 section 7.6.7)
 *  is distributed among several other stages.  This is accomplished by 
 *  folding line offsets into the source and destination (src,dst)
 *  addresses (note the call arguments to form_prediction() in Predict()),
 *  line stride variables lx and lx2, the block dimension variables (w,h), 
 *  average_flag, and by the very order in which Predict() is called.  
 *  This implementation design (implicitly different than the spec) 
 *  was chosen for its elegance.
*/

#ifdef JADEFUSION
static void form_component_prediction(
unsigned char *src,
unsigned char *dst,
int lx,          /* raster line increment */ 
int lx2,
int w, int h,
int x, int y,
int dx, int dy,
int average_flag)      
#else
static void form_component_prediction(src,dst,lx,lx2,w,h,x,y,dx,dy,average_flag)
unsigned char *src;
unsigned char *dst;
int lx;          /* raster line increment */ 
int lx2;
int w,h;
int x,y;
int dx,dy;
int average_flag;      /* flag that signals bi-directional or Dual-Prime 
                          averaging (7.6.7.1 and 7.6.7.4). if average_flag==1,
                          a previously formed prediction has been stored in 
                          pel_pred[] */
#endif
{
  int xint;      /* horizontal integer sample vector: analogous to int_vec[0] */
  int yint;      /* vertical integer sample vectors: analogous to int_vec[1] */
  int xh;        /* horizontal half sample flag: analogous to half_flag[0]  */
  int yh;        /* vertical half sample flag: analogous to half_flag[1]  */
  int i, j, v;
  unsigned char *s;    /* source pointer: analogous to pel_ref[][]   */
  unsigned char *d;    /* destination pointer:  analogous to pel_pred[][]  */

  /* half pel scaling for integer vectors */
  xint = dx>>1;
  yint = dy>>1;

  /* derive half pel flags */
  xh = dx & 1;
  yh = dy & 1;

  /* compute the linear address of pel_ref[][] and pel_pred[][] 
     based on cartesian/raster cordinates provided */
  s = src + lx*(y+yint) + x + xint;
  d = dst + lx*y + x;

  if (!xh && !yh) /* no horizontal nor vertical half-pel */
  {
    if (average_flag)
    {
      for (j=0; j<h; j++)
      {
        for (i=0; i<w; i++)
        {
          v = d[i]+s[i];
          d[i] = (v+(v>=0?1:0))>>1;
        }
      
        s+= lx2;
        d+= lx2;
      }
    }
    else
    {
      for (j=0; j<h; j++)
      {
        for (i=0; i<w; i++)
        {
          d[i] = s[i];
        }
        
        s+= lx2;
        d+= lx2;
      }
    }
  }
  else if (!xh && yh) /* no horizontal but vertical half-pel */
  {
    if (average_flag)
    {
      for (j=0; j<h; j++)
      {
        for (i=0; i<w; i++)
        {
          v = d[i] + ((unsigned int)(s[i]+s[i+lx]+1)>>1);
          d[i]=(v+(v>=0?1:0))>>1;
        }
     
        s+= lx2;
        d+= lx2;
      }
    }
    else
    {
      for (j=0; j<h; j++)
      {
        for (i=0; i<w; i++)
        {
          d[i] = (unsigned int)(s[i]+s[i+lx]+1)>>1;
        }

        s+= lx2;
        d+= lx2;
      }
    }
  }
  else if (xh && !yh) /* horizontal but no vertical half-pel */
  {
    if (average_flag)
    {
      for (j=0; j<h; j++)
      {
        for (i=0; i<w; i++)
        {
          v = d[i] + ((unsigned int)(s[i]+s[i+1]+1)>>1);
          d[i] = (v+(v>=0?1:0))>>1;
        }
     
        s+= lx2;
        d+= lx2;
      }
    }
    else
    {
      for (j=0; j<h; j++)
      {
        for (i=0; i<w; i++)
        {
          d[i] = (unsigned int)(s[i]+s[i+1]+1)>>1;
        }

        s+= lx2;
        d+= lx2;
      }
    }
  }
  else /* if (xh && yh) horizontal and vertical half-pel */
  {
    if (average_flag)
    {
      for (j=0; j<h; j++)
      {
        for (i=0; i<w; i++)
        {
          v = d[i] + ((unsigned int)(s[i]+s[i+1]+s[i+lx]+s[i+lx+1]+2)>>2);
          d[i] = (v+(v>=0?1:0))>>1;
        }
     
        s+= lx2;
        d+= lx2;
      }
    }
    else
    {
      for (j=0; j<h; j++)
      {
        for (i=0; i<w; i++)
        {
          d[i] = (unsigned int)(s[i]+s[i+1]+s[i+lx]+s[i+lx+1]+2)>>2;
        }

        s+= lx2;
        d+= lx2;
      }
    }
  }
}
