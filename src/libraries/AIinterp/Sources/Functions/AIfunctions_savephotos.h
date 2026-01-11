/*$T AIfunctions_save.c GC 1.138 07/22/03 16:16:39 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AItools.h"
#include "AIinterp/Sources/AIstack.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/BIGio.h"
#include <time.h>
#include "INOut/INOsaving.h"


#ifdef _GAMECUBE
#include "SDK/Sources/GameCube/GC_arammng.h"
#include "SDK/Sources/GameCube/GC_aramheap.h"
#endif


#define SAV_PhotoFirstInit()
#define SAV_PhotoInit()
#define SAV_PhotoReinit()
#define SAV_PhotoClose()
#define SAV_PhotoLastClose()
#define SAV_PhotoSet(a,b,c)
#define SAV_PhotoGet(a,b)	NULL

