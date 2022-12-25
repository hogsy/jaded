/*$T EOT.h GC! 1.081 04/12/00 10:11:54 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __EOT_H__
#define __EOT_H__

#include "ENGine/Sources/EOT/EOTstruct.h"
#include "ENGine/Sources/EOT/EOTmain.h"

static const double OETWarningTreshold = 0.75;
static const int OETVisuSize = 2000; // Old 1000
static const int OETAnimsSize = 500;
static const int OETDynaSize = 500;
static const int OETAISize = 750; // Old 500
static const int OETColMapSize = 2000; // Old 500 -> 1000 
static const int OETZDMSize = 750; // Old 500
static const int OETZDESize = 750; // Old 500
static const int OETEventsSize = 500;
static const int OETHierarchySize = 750; // Old 500
static const int OETODESize = 750; // Old 500


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/* Initialisation and close of the module */
extern void EOT_InitModule(void);
extern void EOT_CloseModule(void);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __EOT_H__ */
