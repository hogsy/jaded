#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "global.h"

/* initialize buffer, call once before first getbits or showbits */

void Initialize_Buffer()
{
  gst_AllGloballs.ld->Incnt = 0;
  gst_AllGloballs.ld->Rdptr = gst_AllGloballs.ld->Rdbfr + 2048;
  gst_AllGloballs.ld->Rdmax = gst_AllGloballs.ld->Rdptr;


  gst_AllGloballs.ld->Bfr = 0;
  Flush_Buffer(0); /* fills valid data into bfr */
}

void Fill_Buffer()
{
  int Buffer_Level;

  Buffer_Level = gst_AllGloballs.stSystem->Read2048((int)gst_AllGloballs.ld->Rdbfr);
  gst_AllGloballs.ld->Rdptr = gst_AllGloballs.ld->Rdbfr;

  if (gst_AllGloballs.System_Stream_Flag)
    gst_AllGloballs.ld->Rdmax -= 2048;

  
  /* end of the bitstream file */
  if (Buffer_Level < 2048)
  {
    /* just to be safe */
    if (Buffer_Level < 0)
      Buffer_Level = 0;

    /* pad until the next to the next 32-bit word boundary */
    while (Buffer_Level & 3)
      gst_AllGloballs.ld->Rdbfr[Buffer_Level++] = 0;

	/* pad the buffer with sequence end codes */
    while (Buffer_Level < 2048)
    {
      gst_AllGloballs.ld->Rdbfr[Buffer_Level++] = SEQUENCE_END_CODE>>24;
      gst_AllGloballs.ld->Rdbfr[Buffer_Level++] = SEQUENCE_END_CODE>>16;
      gst_AllGloballs.ld->Rdbfr[Buffer_Level++] = SEQUENCE_END_CODE>>8;
      gst_AllGloballs.ld->Rdbfr[Buffer_Level++] = SEQUENCE_END_CODE&0xff;
    }
  }
}


/* MPEG-1 system layer demultiplexer */

int Get_Byte()
{
  while(gst_AllGloballs.ld->Rdptr >= gst_AllGloballs.ld->Rdbfr+2048)
  {
	gst_AllGloballs.stSystem->Read2048((int)gst_AllGloballs.ld->Rdbfr);
    gst_AllGloballs.ld->Rdptr -= 2048;
    gst_AllGloballs.ld->Rdmax -= 2048;
  }
  return *gst_AllGloballs.ld->Rdptr++;
}

/* extract a 16-bit word from the bitstream buffer */
int Get_Word()
{
  int Val;

  Val = Get_Byte();
  return (Val<<8) | Get_Byte();
}



/* return next bit (could be made faster than Get_Bits(1)) */

unsigned int Get_Bits1()
{
  return Get_Bits(1);
}



void Flush_Buffer(int N)
{
  int Incnt;

  gst_AllGloballs.ld->Bfr <<= N;

  Incnt = gst_AllGloballs.ld->Incnt -= N;

  if (Incnt <= 24)
  {
    if (gst_AllGloballs.System_Stream_Flag && (gst_AllGloballs.ld->Rdptr >= gst_AllGloballs.ld->Rdmax-4))
    {
      do
      {
        if (gst_AllGloballs.ld->Rdptr >= gst_AllGloballs.ld->Rdmax)
          Next_Packet();
        gst_AllGloballs.ld->Bfr |= Get_Byte() << (24 - Incnt);
        Incnt += 8;
      }
      while (Incnt <= 24);
    }
    else if (gst_AllGloballs.ld->Rdptr < gst_AllGloballs.ld->Rdbfr+2044)
    {
      do
      {
        gst_AllGloballs.ld->Bfr |= *gst_AllGloballs.ld->Rdptr++ << (24 - Incnt);
        Incnt += 8;
      }
      while (Incnt <= 24);
    }
    else
    {
      do
      {
        if (gst_AllGloballs.ld->Rdptr >= gst_AllGloballs.ld->Rdbfr+2048)
          Fill_Buffer();
        gst_AllGloballs.ld->Bfr |= *gst_AllGloballs.ld->Rdptr++ << (24 - Incnt);
        Incnt += 8;
      }
      while (Incnt <= 24);
    }
    gst_AllGloballs.ld->Incnt = Incnt;
  }
}


/* return next n bits (right adjusted) */

#ifdef JADEFUSION
unsigned int Get_Bits(int N)
#else
unsigned int Get_Bits(N)
int N;
#endif
{
  unsigned int Val;

  Val = Show_Bits(N);
  Flush_Buffer(N);

  return Val;
}

