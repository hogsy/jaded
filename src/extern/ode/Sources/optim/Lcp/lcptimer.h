#ifndef _LCP_TIMER_H_
#define _LCP_TIMER_H_

#include <stdlib.h>
#include <stdio.h>

/* stop watch objects */
//#ifndef JADEFUSION
typedef struct dStopwatch {
  double time;			/* total clock count */
  ULONG cc[2];		/* clock count since last `start' */
} dStopwatch;
//#endif

void dStopwatchReset (dStopwatch *);
void dStopwatchStart (dStopwatch *);
void dStopwatchStop  (dStopwatch *);
double dStopwatchTime (dStopwatch *);	/* returns total time in secs */


/* code timers */

void dTimerStart (const char *description);	/* pass a static string here */
void dTimerNow (const char *description);	/* pass a static string here */
void dTimerEnd();

/* print out a timer report. if `average' is nonzero, print out the average
 * time for each slot (this is only meaningful if the same start-now-end
 * calls are being made repeatedly.
 */
void dTimerReport (FILE *fout, int average);


/* resolution */

/* returns the timer ticks per second implied by the timing hardware or API.
 * the actual timer resolution may not be this great.
 */
double dTimerTicksPerSecond();

/* returns an estimate of the actual timer resolution, in seconds. this may
 * be greater than 1/ticks_per_second.
 */
double dTimerResolution();


#endif
