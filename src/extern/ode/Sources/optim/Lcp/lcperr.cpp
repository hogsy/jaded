
//#include "Precomp.h"
#ifdef JADEFUSION
#include "projectconfig.h"
#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/CLIbrary/CLIxxx.h"
#endif

#include "lcperr.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static void printMessage (int num, const char *msg1, const char *msg2,
			  va_list ap)
{
  fflush (stderr);
  fflush (stdout);
  if (num) fprintf (stderr,"\n%s %d: ",msg1,num);
  else fprintf (stderr,"\n%s: ",msg1);
  vfprintf (stderr,msg2,ap);
  fprintf (stderr,"\n");
  fflush (stderr);
}

void dMessage (int num, const char *msg, ...)
{
#if 0
  va_list ap;
  va_start (ap,msg);
  printMessage (num,"LCP Message",msg,ap);
#endif

}

void dDebug (int num, const char *msg, ...)
{
  va_list ap;
  va_start (ap,msg);
  printMessage (num,"LCP INTERNAL ERROR",msg,ap);
  // *((char *)0) = 0;   ... commit SEGVicide
  abort();

}