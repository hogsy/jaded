#ifndef _LCP_ERRORS_H_
#define _LPC_ERRORS_H_


/* error numbers */

enum {
  d_ERR_UNKNOWN = 0,		/* unknown error */
  d_ERR_IASSERT,		/* internal assertion failed */
  d_ERR_UASSERT,		/* user assertion failed */
  d_ERR_LCP			/* user assertion failed */
};

void dDebug (int num, const char *msg, ...);
void dMessage (int num, const char *msg, ...);

#endif
