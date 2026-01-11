/*$T SNDspecific.h GC! 1.081 12/17/02 09:13:11 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef _SNDSPECIFIC_H_
#define _SNDSPECIFIC_H_

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$2- GAMECUBE interface ---------------------------------------------------------------------------------------------*/

#ifdef _GAMECUBE
#include "gc/gcSND.h"
#include "gc/gcSND_stream.h"
#include "gc/gcSNDfx.h"
#include "gc/gcSND_Prefetch.h"

/*$2- XENON interface -------------------------------------------------------------------------------------------------*/

#elif defined(_XENON)

#include "Xenon/xeSND_Interface.h"

/*$2- XBOX interface -------------------------------------------------------------------------------------------------*/

#elif defined(_XBOX)
#include "xbox/xbSND.h"
#include "xbox/xbSND_stream.h"

/*$2- PS2 interface --------------------------------------------------------------------------------------------------*/

#elif defined(PSX2_TARGET)
#include "ps2/ps2SND.h"

/*$2- EDITOR interface ------------------------------------------------------------------------------------------------*/
#elif defined(ACTIVE_EDITORS)

#include "edi/ediSND.h"
#include "edi/ediSND_Stream.h"

/*$2- win32 interface ------------------------------------------------------------------------------------------------*/

#elif defined(PCWIN_TOOL) 
#include "win32/win32SND.h"
#include "win32/win32SNDfx.h"
#include "win32/win32SND_Stream.h"

/*$2- PC retail interface --------------------------------------------------------------------------------------------*/
#elif defined(_PC_RETAIL)
#include "win32/win32SND.h"
#include "win32/win32SND_Stream.h"
#include "pcretail/pcretailSND.h"
#include "pcretail/pcretailSNDfx.h"
/*$2- UNKNOWN --------------------------------------------------------------------------------------------------------*/

#else
#pragma message(__FILE__ ">> : error unknown target <<")
#endif

/*$2------------------------------------------------------------------------------------------------------------------*/

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* _SNDSPE_H_ */
