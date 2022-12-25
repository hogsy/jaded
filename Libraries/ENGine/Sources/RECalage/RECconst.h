/*$T RECconst.h GC!1.52 11/30/99 15:45:29 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Constants of the REC module
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/* Constants for the Impulse (new speed) Type */

/* N is the "pseudo-normal" and D the new direction vector (null except in case REC_C_Straight) */
#define REC_C_Point     0   /* The New speed is colinear to N */
#define REC_C_Straight  1   /* The new speed must be in the plane defined by vectors N and D */
#define REC_C_Plan      2   /* The new speed is anywhere in the half space of N */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
