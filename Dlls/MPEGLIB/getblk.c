/* getblk.c, DCT block decoding                                             */

#include <stdio.h>

#include "config.h"
#include "global.h"


/* defined in getvlc.h */
typedef struct {
  char run, level, len;
} DCTtab;

extern DCTtab DCTtabfirst[],DCTtabnext[],DCTtab0[],DCTtab1[];
extern DCTtab DCTtab2[],DCTtab3[],DCTtab4[],DCTtab5[],DCTtab6[];
extern DCTtab DCTtab0a[],DCTtab1a[];


/* decode one intra coded MPEG-1 block */

#ifdef JADEFUSION
void Decode_MPEG1_Intra_Block(int comp, int dc_dct_pred[])
#else
void Decode_MPEG1_Intra_Block(comp,dc_dct_pred)
int comp;
int dc_dct_pred[];
#endif
{
  int val, i, j, sign;
  unsigned int code;
  DCTtab *tab;
  short *bp;

  bp = gst_AllGloballs.ld->block[comp];

  /* ISO/IEC 11172-2 section 2.4.3.7: Block layer. */
  /* decode DC coefficients */
  if (comp<4)
    bp[0] = (dc_dct_pred[0]+=Get_Luma_DC_dct_diff()) << 3;
  else if (comp==4)
    bp[0] = (dc_dct_pred[1]+=Get_Chroma_DC_dct_diff()) << 3;
  else
    bp[0] = (dc_dct_pred[2]+=Get_Chroma_DC_dct_diff()) << 3;

  if (gst_AllGloballs.Fault_Flag) return;

  /* D-pictures do not contain AC coefficients */
  if(gst_AllGloballs.picture_coding_type == D_TYPE)
    return;

  /* decode AC coefficients */
  for (i=1; ; i++)
  {
    code = Show_Bits(16);
    if (code>=16384)
      tab = &DCTtabnext[(code>>12)-4];
    else if (code>=1024)
      tab = &DCTtab0[(code>>8)-4];
    else if (code>=512)
      tab = &DCTtab1[(code>>6)-8];
    else if (code>=256)
      tab = &DCTtab2[(code>>4)-16];
    else if (code>=128)
      tab = &DCTtab3[(code>>3)-16];
    else if (code>=64)
      tab = &DCTtab4[(code>>2)-16];
    else if (code>=32)
      tab = &DCTtab5[(code>>1)-16];
    else if (code>=16)
      tab = &DCTtab6[code-16];
    else
    {
      return;
    }

    Flush_Buffer(tab->len);

    if (tab->run==64) /* end_of_block */
      return;

    if (tab->run==65) /* escape */
    {
      i+= Get_Bits(6);

      val = Get_Bits(8);
      if (val==0)
        val = Get_Bits(8);
      else if (val==128)
        val = Get_Bits(8) - 256;
      else if (val>128)
        val -= 256;

      if((sign = (val<0)))
        val = -val;
    }
    else
    {
      i+= tab->run;
      val = tab->level;
      sign = Get_Bits(1);
    }

    if (i>=64)
    {
      return;
    }

    j = scan[ZIG_ZAG][i];
    val = (val*gst_AllGloballs.ld->quantizer_scale*gst_AllGloballs.ld->intra_quantizer_matrix[j]) >> 3;

    /* mismatch control ('oddification') */
    if (val!=0) /* should always be true, but it's not guaranteed */
      val = (val-1) | 1; /* equivalent to: if ((val&1)==0) val = val - 1; */

    /* saturation */
    if (!sign)
      bp[j] = (val>2047) ?  2047 :  val; /* positive */
    else
      bp[j] = (val>2048) ? -2048 : -val; /* negative */
  }
}


/* decode one non-intra coded MPEG-1 block */
#ifdef JADEFUSION
void Decode_MPEG1_Non_Intra_Block(int comp)
#else
void Decode_MPEG1_Non_Intra_Block(comp)
int comp;
#endif
{
  int val, i, j, sign;
  unsigned int code;
  DCTtab *tab;
  short *bp;

  bp = gst_AllGloballs.ld->block[comp];

  /* decode AC coefficients */
  for (i=0; ; i++)
  {
    code = Show_Bits(16);
    if (code>=16384)
    {
      if (i==0)
        tab = &DCTtabfirst[(code>>12)-4];
      else
        tab = &DCTtabnext[(code>>12)-4];
    }
    else if (code>=1024)
      tab = &DCTtab0[(code>>8)-4];
    else if (code>=512)
      tab = &DCTtab1[(code>>6)-8];
    else if (code>=256)
      tab = &DCTtab2[(code>>4)-16];
    else if (code>=128)
      tab = &DCTtab3[(code>>3)-16];
    else if (code>=64)
      tab = &DCTtab4[(code>>2)-16];
    else if (code>=32)
      tab = &DCTtab5[(code>>1)-16];
    else if (code>=16)
      tab = &DCTtab6[code-16];
    else
    {
      return;
    }

    Flush_Buffer(tab->len);

    if (tab->run==64) /* end_of_block */
      return;

    if (tab->run==65) /* escape */
    {
      i+= Get_Bits(6);

      val = Get_Bits(8);
      if (val==0)
        val = Get_Bits(8);
      else if (val==128)
        val = Get_Bits(8) - 256;
      else if (val>128)
        val -= 256;

      if((sign = (val<0)))
        val = -val;
    }
    else
    {
      i+= tab->run;
      val = tab->level;
      sign = Get_Bits(1);
    }

    if (i>=64)
    {
      return;
    }

    j = scan[ZIG_ZAG][i];
    val = (((val<<1)+1)*gst_AllGloballs.ld->quantizer_scale*gst_AllGloballs.ld->non_intra_quantizer_matrix[j]) >> 4;

    /* mismatch control ('oddification') */
    if (val!=0) /* should always be true, but it's not guaranteed */
      val = (val-1) | 1; /* equivalent to: if ((val&1)==0) val = val - 1; */

    /* saturation */
    if (!sign)
      bp[j] = (val>2047) ?  2047 :  val; /* positive */
    else
      bp[j] = (val>2048) ? -2048 : -val; /* negative */
  }
}


/* decode one intra coded MPEG-2 block */
#ifdef JADEFUSION
void Decode_MPEG2_Intra_Block(int comp, int dc_dct_pred[])
#else
void Decode_MPEG2_Intra_Block(comp,dc_dct_pred)
int comp;
int dc_dct_pred[];
#endif
{
  int val, i, j, sign, nc, cc, run;
  unsigned int code;
  DCTtab *tab;
  short *bp;
  int *qmat;
  struct layer_data *ld1;

  /* with data partitioning, data always goes to gst_AllGloballs.base layer */
  ld1 = (gst_AllGloballs.ld->scalable_mode==SC_DP) ? &gst_AllGloballs.base : gst_AllGloballs.ld;
  bp = ld1->block[comp];

  if (gst_AllGloballs.base.scalable_mode==SC_DP)
    if (gst_AllGloballs.base.priority_breakpoint<64)
      gst_AllGloballs.ld = &gst_AllGloballs.enhan;
    else
      gst_AllGloballs.ld = &gst_AllGloballs.base;

  cc = (comp<4) ? 0 : (comp&1)+1;

  qmat = (comp<4 || gst_AllGloballs.chroma_format==CHROMA420)
         ? ld1->intra_quantizer_matrix
         : ld1->chroma_intra_quantizer_matrix;

  /* ISO/IEC 13818-2 section 7.2.1: decode DC coefficients */
  if (cc==0)
    val = (dc_dct_pred[0]+= Get_Luma_DC_dct_diff());
  else if (cc==1)
    val = (dc_dct_pred[1]+= Get_Chroma_DC_dct_diff());
  else
    val = (dc_dct_pred[2]+= Get_Chroma_DC_dct_diff());

  if (gst_AllGloballs.Fault_Flag) return;

  bp[0] = val << (3-gst_AllGloballs.intra_dc_precision);

  nc=0;

  /* decode AC coefficients */
  for (i=1; ; i++)
  {
    code = Show_Bits(16);
    if (code>=16384 && !gst_AllGloballs.intra_vlc_format)
      tab = &DCTtabnext[(code>>12)-4];
    else if (code>=1024)
    {
      if (gst_AllGloballs.intra_vlc_format)
        tab = &DCTtab0a[(code>>8)-4];
      else
        tab = &DCTtab0[(code>>8)-4];
    }
    else if (code>=512)
    {
      if (gst_AllGloballs.intra_vlc_format)
        tab = &DCTtab1a[(code>>6)-8];
      else
        tab = &DCTtab1[(code>>6)-8];
    }
    else if (code>=256)
      tab = &DCTtab2[(code>>4)-16];
    else if (code>=128)
      tab = &DCTtab3[(code>>3)-16];
    else if (code>=64)
      tab = &DCTtab4[(code>>2)-16];
    else if (code>=32)
      tab = &DCTtab5[(code>>1)-16];
    else if (code>=16)
      tab = &DCTtab6[code-16];
    else
    {
      return;
    }

    Flush_Buffer(tab->len);


    if (tab->run==64) /* end_of_block */
    {
      return;
    }

    if (tab->run==65) /* escape */
    {
      i+= run = Get_Bits(6);

      val = Get_Bits(12);
      if ((val&2047)==0)
      {
        return;
      }
      if((sign = (val>=2048)))
        val = 4096 - val;
    }
    else
    {
      i+= run = tab->run;
      val = tab->level;
      sign = Get_Bits(1);

    }

    if (i>=64)
    {
      return;
    }

    j = scan[ld1->alternate_scan][i];
    val = (val * ld1->quantizer_scale * qmat[j]) >> 4;
    bp[j] = sign ? -val : val;
    nc++;

    if (gst_AllGloballs.base.scalable_mode==SC_DP && nc==gst_AllGloballs.base.priority_breakpoint-63)
      gst_AllGloballs.ld = &gst_AllGloballs.enhan;
  }
}


/* decode one non-intra coded MPEG-2 block */
#ifdef JADEFUSION
void Decode_MPEG2_Non_Intra_Block(int comp)
#else
void Decode_MPEG2_Non_Intra_Block(comp)
int comp;
#endif
{
  int val, i, j, sign, nc, run;
  unsigned int code;
  DCTtab *tab;
  short *bp;
  int *qmat;
  struct layer_data *ld1;

  /* with data partitioning, data always goes to gst_AllGloballs.base layer */
  ld1 = (gst_AllGloballs.ld->scalable_mode==SC_DP) ? &gst_AllGloballs.base : gst_AllGloballs.ld;
  bp = ld1->block[comp];

  if (gst_AllGloballs.base.scalable_mode==SC_DP)
    if (gst_AllGloballs.base.priority_breakpoint<64)
      gst_AllGloballs.ld = &gst_AllGloballs.enhan;
    else
      gst_AllGloballs.ld = &gst_AllGloballs.base;

  qmat = (comp<4 || gst_AllGloballs.chroma_format==CHROMA420)
         ? ld1->non_intra_quantizer_matrix
         : ld1->chroma_non_intra_quantizer_matrix;

  nc = 0;

  /* decode AC coefficients */
  for (i=0; ; i++)
  {
    code = Show_Bits(16);
    if (code>=16384)
    {
      if (i==0)
        tab = &DCTtabfirst[(code>>12)-4];
      else
        tab = &DCTtabnext[(code>>12)-4];
    }
    else if (code>=1024)
      tab = &DCTtab0[(code>>8)-4];
    else if (code>=512)
      tab = &DCTtab1[(code>>6)-8];
    else if (code>=256)
      tab = &DCTtab2[(code>>4)-16];
    else if (code>=128)
      tab = &DCTtab3[(code>>3)-16];
    else if (code>=64)
      tab = &DCTtab4[(code>>2)-16];
    else if (code>=32)
      tab = &DCTtab5[(code>>1)-16];
    else if (code>=16)
      tab = &DCTtab6[code-16];
    else
    {
      return;
    }

    Flush_Buffer(tab->len);

    if (tab->run==64) /* end_of_block */
    {
      return;
    }

    if (tab->run==65) /* escape */
    {
      i+= run = Get_Bits(6);

      val = Get_Bits(12);
      if ((val&2047)==0)
      {
        return;
      }
      if((sign = (val>=2048)))
        val = 4096 - val;
    }
    else
    {
      i+= run = tab->run;
      val = tab->level;
      sign = Get_Bits(1);

    }

    if (i>=64)
    {
      return;
    }

    j = scan[ld1->alternate_scan][i];
    val = (((val<<1)+1) * ld1->quantizer_scale * qmat[j]) >> 5;
    bp[j] = sign ? -val : val;
    nc++;

    if (gst_AllGloballs.base.scalable_mode==SC_DP && nc==gst_AllGloballs.base.priority_breakpoint-63)
      gst_AllGloballs.ld = &gst_AllGloballs.enhan;
  }
}
