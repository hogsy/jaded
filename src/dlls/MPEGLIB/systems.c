/* systems.c, systems-specific routines                                 */

/* Copyright (C) 1996, MPEG Software Simulation Group. All Rights Reserved. */

#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "global.h"

/* initialize buffer, call once before first getbits or showbits */

/* parse system layer, ignore everything we don't need */
void Next_Packet()
{
  unsigned int code;
  int l;

  for(;;)
  {
    code = Get_Long();

    /* remove system layer byte stuffing */
    while ((code & 0xffffff00) != 0x100)
      code = (code<<8) | Get_Byte();

    switch(code)
    {
    case PACK_START_CODE: /* pack header */
      /* skip pack header (system_clock_reference and mux_rate) */
      gst_AllGloballs.ld->Rdptr += 8;
      break;
    case VIDEO_ELEMENTARY_STREAM:   
      code = Get_Word();             /* packet_length */
      gst_AllGloballs.ld->Rdmax = gst_AllGloballs.ld->Rdptr + code;

      code = Get_Byte();

      if((code>>6)==0x02)
      {
        gst_AllGloballs.ld->Rdptr++;
        code=Get_Byte();  /* parse PES_header_data_length */
        gst_AllGloballs.ld->Rdptr+=code;    /* advance pointer by PES_header_data_length */
        return;
      }
      else if(code==0xff)
      {
        /* parse MPEG-1 packet header */
        while((code=Get_Byte())== 0xFF);
      }
       
      /* stuffing bytes */
      if(code>=0x40)
      {
        if(code>=0x80)
        {
          Error("Error in packet header\n");
        }
        /* skip STD_buffer_scale */
        gst_AllGloballs.ld->Rdptr++;
        code = Get_Byte();
      }

      if(code>=0x30)
      {
        if(code>=0x40)
        {
          Error("Error in packet header\n");
        }
        /* skip presentation and decoding time stamps */
        gst_AllGloballs.ld->Rdptr += 9;
      }
      else if(code>=0x20)
      {
        /* skip presentation time stamps */
        gst_AllGloballs.ld->Rdptr += 4;
      }
      else if(code!=0x0f)
      {
        Error("Error in packet header\n");
      }
      return;
    case ISO_END_CODE: /* end */
      /* simulate a buffer full of sequence end codes */
      l = 0;
      while (l<2048)
      {
        gst_AllGloballs.ld->Rdbfr[l++] = SEQUENCE_END_CODE>>24;
        gst_AllGloballs.ld->Rdbfr[l++] = SEQUENCE_END_CODE>>16;
        gst_AllGloballs.ld->Rdbfr[l++] = SEQUENCE_END_CODE>>8;
        gst_AllGloballs.ld->Rdbfr[l++] = SEQUENCE_END_CODE&0xff;
      }
      gst_AllGloballs.ld->Rdptr = gst_AllGloballs.ld->Rdbfr;
      gst_AllGloballs.ld->Rdmax = gst_AllGloballs.ld->Rdbfr + 2048;
      return;
    default:
      if(code>=SYSTEM_START_CODE)
      {
        /* skip system headers and non-video packets*/
        code = Get_Word();
        gst_AllGloballs.ld->Rdptr += code;
      }
      else
      {
        Error("Unexpected startcode %08x in system layer\n");
      }
      break;
    }
  }
}



void Flush_Buffer32()
{
  int Incnt;

  gst_AllGloballs.ld->Bfr = 0;

  Incnt = gst_AllGloballs.ld->Incnt;
  Incnt -= 32;

  if (gst_AllGloballs.System_Stream_Flag && (gst_AllGloballs.ld->Rdptr >= gst_AllGloballs.ld->Rdmax-4))
  {
    while (Incnt <= 24)
    {
      if (gst_AllGloballs.ld->Rdptr >= gst_AllGloballs.ld->Rdmax)
        Next_Packet();
      gst_AllGloballs.ld->Bfr |= Get_Byte() << (24 - Incnt);
      Incnt += 8;
    }
  }
  else
  {
    while (Incnt <= 24)
    {
      if (gst_AllGloballs.ld->Rdptr >= gst_AllGloballs.ld->Rdbfr+2048)
        Fill_Buffer();
      gst_AllGloballs.ld->Bfr |= *gst_AllGloballs.ld->Rdptr++ << (24 - Incnt);
      Incnt += 8;
    }
  }
  gst_AllGloballs.ld->Incnt = Incnt;

}


unsigned int Get_Bits32()
{
  unsigned int l;

  l = Show_Bits(32);
  Flush_Buffer32();

  return l;
}


int Get_Long()
{
  int i;

  i = Get_Word();
  return (i<<16) | Get_Word();
}


