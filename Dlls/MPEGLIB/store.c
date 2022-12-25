/* store.c, picture output routines                                         */

/* Copyright (C) 1996, MPEG Software Simulation Group. All Rights Reserved. */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "config.h"
#include "global.h"

/* private prototypes */
static void store_one (char *outname, unsigned char *src[],
  int offset, int incr, int height);

#define OBFRSIZE 4096
static unsigned char obfr[OBFRSIZE];
static unsigned char *optr;
static int outfile;

/*
 * store a picture as either one frame or two fields
 */
void Write_Frame(unsigned char *src[],int frame)
{
  /*MAX_SPD*/
  dither(src);
}
