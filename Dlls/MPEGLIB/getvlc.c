/* getvlc.c, variable length decoding                                       */

/* Copyright (C) 1996, MPEG Software Simulation Group. All Rights Reserved. */

#include <stdio.h>

#include "config.h"
#include "global.h"
#include "getvlc.h"

/* private prototypes */
/* generic picture macroblock type processing functions */
static int Get_I_macroblock_type (void);
static int Get_P_macroblock_type (void);
static int Get_B_macroblock_type (void);
static int Get_D_macroblock_type (void);

/* spatial picture macroblock type processing functions */
static int Get_I_Spatial_macroblock_type (void);
static int Get_P_Spatial_macroblock_type (void);
static int Get_B_Spatial_macroblock_type (void);
static int Get_SNR_macroblock_type (void);

int Get_macroblock_type()
{
  int macroblock_type = 0;

  if (gst_AllGloballs.ld->scalable_mode==SC_SNR)
    macroblock_type = Get_SNR_macroblock_type();
  else
  {
    switch (gst_AllGloballs.picture_coding_type)
    {
    case I_TYPE:
      macroblock_type = gst_AllGloballs.ld->pict_scal ? Get_I_Spatial_macroblock_type() : Get_I_macroblock_type();
      break;
    case P_TYPE:
      macroblock_type = gst_AllGloballs.ld->pict_scal ? Get_P_Spatial_macroblock_type() : Get_P_macroblock_type();
      break;
    case B_TYPE:
      macroblock_type = gst_AllGloballs.ld->pict_scal ? Get_B_Spatial_macroblock_type() : Get_B_macroblock_type();
      break;
    case D_TYPE:
      macroblock_type = Get_D_macroblock_type();
      break;
    default:
      break;
    }
  }

  return macroblock_type;
}

static int Get_I_macroblock_type()
{
  if (Get_Bits1())
  {
    return 1;
  }

  if (!Get_Bits1())
  {
    gst_AllGloballs.Fault_Flag = 1;
  }

  return 17;
}

static int Get_P_macroblock_type()
{
  int code;

  if ((code = Show_Bits(6))>=8)
  {
    code >>= 3;
    Flush_Buffer(PMBtab0[code].len);
    return PMBtab0[code].val;
  }

  if (code==0)
  {
    gst_AllGloballs.Fault_Flag = 1;
    return 0;
  }
  Flush_Buffer(PMBtab1[code].len);


  return PMBtab1[code].val;
}

static int Get_B_macroblock_type()
{
  int code;

  if ((code = Show_Bits(6))>=8)
  {
    code >>= 2;
    Flush_Buffer(BMBtab0[code].len);

    return BMBtab0[code].val;
  }

  if (code==0)
  {
    gst_AllGloballs.Fault_Flag = 1;
    return 0;
  }

  Flush_Buffer(BMBtab1[code].len);

  return BMBtab1[code].val;
}

static int Get_D_macroblock_type()
{
  if (!Get_Bits1())
  {
    gst_AllGloballs.Fault_Flag=1;
  }

  return 1;
}

/* macroblock_type for pictures with spatial scalability */
static int Get_I_Spatial_macroblock_type()
{
  int code;

  code = Show_Bits(4);

  if (code==0)
  {
    gst_AllGloballs.Fault_Flag = 1;
    return 0;
  }

  Flush_Buffer(spIMBtab[code].len);
  return spIMBtab[code].val;
}

static int Get_P_Spatial_macroblock_type()
{
  int code;

  code = Show_Bits(7);

  if (code<2)
  {
    gst_AllGloballs.Fault_Flag = 1;
    return 0;
  }

  if (code>=16)
  {
    code >>= 3;
    Flush_Buffer(spPMBtab0[code].len);

    return spPMBtab0[code].val;
  }

  Flush_Buffer(spPMBtab1[code].len);

  return spPMBtab1[code].val;
}

static int Get_B_Spatial_macroblock_type()
{
  int code;
  VLCtab *p;

  code = Show_Bits(9);

  if (code>=64)
    p = &spBMBtab0[(code>>5)-2];
  else if (code>=16)
    p = &spBMBtab1[(code>>2)-4];
  else if (code>=8)
    p = &spBMBtab2[code-8];
  else
  {
    gst_AllGloballs.Fault_Flag = 1;
    return 0;
  }

  Flush_Buffer(p->len);

  return p->val;
}

static int Get_SNR_macroblock_type()
{
  int code;

  code = Show_Bits(3);

  if (code==0)
  {
    gst_AllGloballs.Fault_Flag = 1;
    return 0;
  }

  Flush_Buffer(SNRMBtab[code].len);

  return SNRMBtab[code].val;
}

int Get_motion_code()
{
  int code;

  if (Get_Bits1())
  {
    return 0;
  }

  if ((code = Show_Bits(9))>=64)
  {
    code >>= 6;
    Flush_Buffer(MVtab0[code].len);

    return Get_Bits1()?-MVtab0[code].val:MVtab0[code].val;
  }

  if (code>=24)
  {
    code >>= 3;
    Flush_Buffer(MVtab1[code].len);

    return Get_Bits1()?-MVtab1[code].val:MVtab1[code].val;
  }

  if ((code-=12)<0)
  {
    gst_AllGloballs.Fault_Flag=1;
    return 0;
  }

  Flush_Buffer(MVtab2[code].len);

  return Get_Bits1() ? -MVtab2[code].val : MVtab2[code].val;
}

/* get differential motion vector (for dual prime prediction) */
int Get_dmvector()
{
  if (Get_Bits(1))
  {
    return Get_Bits(1) ? -1 : 1;
  }
  else
  {
    return 0;
  }
}

int Get_coded_block_pattern()
{
  int code;

  if ((code = Show_Bits(9))>=128)
  {
    code >>= 4;
    Flush_Buffer(CBPtab0[code].len);

    return CBPtab0[code].val;
  }

  if (code>=8)
  {
    code >>= 1;
    Flush_Buffer(CBPtab1[code].len);

    return CBPtab1[code].val;
  }

  if (code<1)
  {
    gst_AllGloballs.Fault_Flag = 1;
    return 0;
  }

  Flush_Buffer(CBPtab2[code].len);

  return CBPtab2[code].val;
}

int Get_macroblock_address_increment()
{
  int code, val;

  val = 0;

  while ((code = Show_Bits(11))<24)
  {
    if (code!=15) /* if not macroblock_stuffing */
    {
      if (code==8) /* if macroblock_escape */
      {
        val+= 33;
      }
      else
      {
        gst_AllGloballs.Fault_Flag = 1;
        return 1;
      }
    }

    Flush_Buffer(11);
  }

  /* macroblock_address_increment == 1 */
  /* ('1' is in the MSB position of the lookahead) */
  if (code>=1024)
  {
    Flush_Buffer(1);
    return val + 1;
  }

  /* codes 00010 ... 011xx */
  if (code>=128)
  {
    /* remove leading zeros */
    code >>= 6;
    Flush_Buffer(MBAtab1[code].len);

    return val + MBAtab1[code].val;
  }
  
  /* codes 00000011000 ... 0000111xxxx */
  code-= 24; /* remove common base */
  Flush_Buffer(MBAtab2[code].len);

  return val + MBAtab2[code].val;
}

/* combined MPEG-1 and MPEG-2 stage. parse VLC and 
   perform dct_diff arithmetic.

   MPEG-1:  ISO/IEC 11172-2 section
   MPEG-2:  ISO/IEC 13818-2 section 7.2.1 
   
   Note: the arithmetic here is presented more elegantly than
   the spec, yet the results, dct_diff, are the same.
*/

int Get_Luma_DC_dct_diff()
{
  int code, size, dct_diff;

  /* decode length */
  code = Show_Bits(5);

  if (code<31)
  {
    size = DClumtab0[code].val;
    Flush_Buffer(DClumtab0[code].len);
  }
  else
  {
    code = Show_Bits(9) - 0x1f0;
    size = DClumtab1[code].val;
    Flush_Buffer(DClumtab1[code].len);
  }

  if (size==0)
    dct_diff = 0;
  else
  {
    dct_diff = Get_Bits(size);
    if ((dct_diff & (1<<(size-1)))==0)
      dct_diff-= (1<<size) - 1;
  }

  return dct_diff;
}


int Get_Chroma_DC_dct_diff()
{
  int code, size, dct_diff;

  /* decode length */
  code = Show_Bits(5);

  if (code<31)
  {
    size = DCchromtab0[code].val;
    Flush_Buffer(DCchromtab0[code].len);

  }
  else
  {
    code = Show_Bits(10) - 0x3e0;
    size = DCchromtab1[code].val;
    Flush_Buffer(DCchromtab1[code].len);

  }

  if (size==0)
    dct_diff = 0;
  else
  {
    dct_diff = Get_Bits(size);
    if ((dct_diff & (1<<(size-1)))==0)
      dct_diff-= (1<<size) - 1;
  }
  return dct_diff;
}
