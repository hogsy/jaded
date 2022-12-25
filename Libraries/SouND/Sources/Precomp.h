
#ifndef UBI_INCLUDED_PRECOMP_H__E367CE0A_386F_44F2_B62B_40DCC489C3E4
#define UBI_INCLUDED_PRECOMP_H__E367CE0A_386F_44F2_B62B_40DCC489C3E4

#if defined (JADEFUSION)

#pragma message("*******************************************************************************")
#pragma message("*** Sound Precompiled Header *************************************************")

#include "ProjectConfig.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"

#ifdef ACTIVE_EDITORS

#include "BASe/MEMory/MEM.h"
#include "BASe/ERRors/ERR.h"

#include "BIGfiles/BIGio.h"
#include "BIGfiles/LOAding/LOAread.h"

#include "ENGine/Sources/OBJects/OBJaccess.h"

#include "ENGine/Sources/ENGvars.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDspecific.h"
#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDdialog.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDfx.h"
#include "SouND/Sources/SNDrasters.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SNDstream.h"
#include "SouND/Sources/edi/ediSNDdebug.h"
#include "SouND/Sources/SNDconv_xboxadpcm.h"
#include "EDItors/Sources/SOuNd/SONutil.h"
#include "SouND/Sources/SNDconv_pcretailadpcm.h"

#ifdef USE_EAX
#include "../../Extern/EAX30/include/eax.h"
#endif

#endif

// Audio Console is currently in the Xenon folder, but can be safely included by any target
#include "SouND/Sources/Xenon/xeSND_AudioConsole.h"
#endif	//JADEFUSION

//#else*/*/

#if !defined (_XBOX) && !defined (_XENON)
#include <dsound.h>
#endif

#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/CLIbrary/CLIxxx.h"

#endif	//UBI_INCLUDED_PRECOMP_H__E367CE0A_386F_44F2_B62B_40DCC489C3E4
