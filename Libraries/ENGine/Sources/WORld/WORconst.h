/*$T WORconst.h GC! 1.081 04/12/00 10:09:53 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Constants of the world module */
#ifndef PSX2_TARGET
#pragma once
#endif
#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$F
The following figure explains the activation distances:
P is the activator
The object is somewhere between P and M. If it's distance from activator is greater than 
A+B+D, the object is tested every "WOR_Cf_DefaultTimeC" seconds.
if distance is greater than A+D, object is tested every WOR_Cf_DefaultTimeB seconds
if distance is greater than D, object is tested every WOR_Cf_DefaultTimeA seconds
See chapter "Système de visibilité automatique" from the document "MERLIN-Description de l'univers" 
for more information 



      activation distance (D)           A distance                 B distance
    P<-----------------------><--------------------------><------------------------->M

*/

/*$4
 ***********************************************************************************************************************
    Constants about the activation
 ***********************************************************************************************************************
 */

/* Activation And Visibility modes */
#define WOR_C_MaskActivBit	((UCHAR) 0x7F)

/* Default values for activation */
#define WOR_C_MaxNbWorlds				((ULONG) 4)		/* Max number of worlds */
#define WOR_C_MaxNbActObjects           16384           /* Maximum number of activ objects */
#define WOR_C_MaxNbVisObjects			16384			/* Maximum number of visible objects */
#define WOR_C_MaxNbLights				2000			/* Maximum number of lights */
#define WOR_Cf_DefaultBVRefreshTime		0.1f			/* Time in seconds between 2 refresh of the BV structure */

/* Flag for view (store in one byte) */
#define WOR_Cuc_View_Activ				0x01
#define WOR_Cuc_View_RecomputeTables	0x02
#ifdef ACTIVE_EDITORS
#define WOR_Cuc_View_SplitView			0x04
#endif


#define MAX_DEL_OBJ						400

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
