/*$T DYNmain.h GC! 1.077 03/13/00 12:33:13 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Main functions of the dynamics module */
#ifndef _DYNMAIN_H_
#include "TABles/TABles.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Functions called by the engine loop
 ***********************************************************************************************************************
 */

extern void DYN_MainCall(struct WOR_tdst_World_ *);
extern void DYN_OneCall(struct OBJ_tdst_GameObject_ *);


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif 
 