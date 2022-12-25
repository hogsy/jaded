/*$T AI2C_fctheader.h GC! 1.100 03/21/01 10:59:17 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __AI2C_FCTHEADER_H__
#define __AI2C_FCTHEADER_H__

#include "BASe/ERRors/ERRasser.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGfat.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/TEXT/TEXTstruct.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/AImsg.h"
#include "AIinterp/Sources/AIsave.h"
#include "AIinterp/Sources/Events/EVEconst.h"
#include "AIinterp/Sources/Events/EVEplay.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"
#include "AIdebug.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "TIMer/TIMdefs.h"
#include "NETwork/sources/NET.h"

#ifdef AI2C


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#define AI2C_FCTDEF(a, b)			extern int	  b(void);

#ifdef __cplusplus
class message;
#define AI2C_FCTDEFTRIGGER(a, b, c)	extern int	  c(message &);
#else // __cplusplus
#define AI2C_FCTDEFTRIGGER(a, b, c)	
#endif // __cplusplus

#include "AI2C_fctdefs.h"
#undef AI2C_FCTDEF
#undef AI2C_FCTDEFTRIGGER

#define DEFINE_FUNCTION(a, b, c, d, e, f, g, h, i, j)	extern	  j;
#include "Functions/AIdeffct.h"

extern ULONG				AI_ul_RealResolveOneRefNoType(ULONG);
extern void					AI2C_PushInt(int);
extern int					AI2C_PopInt(void);
extern void					AI2C_PushFloat(float);
extern float				AI2C_PopFloat(void);
extern void					AI2C_PushVector(MATH_tdst_Vector *);
extern void					AI2C_PopVector(MATH_tdst_Vector *);
extern void					AI2C_PushMessageId(AI_tdst_GlobalMessageId *);
extern void					AI2C_PopMessageId(AI_tdst_GlobalMessageId *);
extern void					AI2C_PushGameObject(OBJ_tdst_GameObject *);
extern OBJ_tdst_GameObject	*AI2C_PopGameObject(void);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#if !defined(PSX2_TARGET) && !defined(_GAMECUBE)
#pragma warning(disable : 4244)
#pragma warning(disable : 4146)
#pragma warning(disable : 4305)
#pragma warning(disable : 4101)
#endif
#if defined(PSX2_TARGET) || defined(_GAMECUBE)
#pragma warn_unusedvar off 
#pragma warn_possunwant off 
#endif
#endif /* AI2C */

#if defined(GAMECUBE_USE_AI2C_DLL)
#include "MATHs/MATHvector_aligned.h"

typedef void (*voidfunctionptr) (void);

__declspec(section ".init") extern voidfunctionptr	_ctors[];
__declspec(section ".init") extern voidfunctionptr	_dtors[];

#define AI2C_FCTDEF(a, b)			extern int (*AI2C_pfi_##b) (void);

#ifdef __cplusplus
class message;
#define AI2C_FCTDEFTRIGGER(a, b, c)	extern int (*AI2C_pfi_##c) (message &);
#else // __cplusplus
#define AI2C_FCTDEFTRIGGER(a, b, c)	
#endif // __cplusplus

#include "AI2C_fctdefs.h"
#undef AI2C_FCTDEF
#undef AI2C_FCTDEFTRIGGER

#ifdef GAMECUBE_DLL_BODY
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

//  _prolog, _epilog and _unresolved must have a C signature, so that OSLink finds them.
#ifdef __cplusplus
extern "C"
{
#endif

void	_prolog(void);
void	_epilog(void);
void	_unresolved(void);
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void _unresolved(void)
{
#ifdef _DEBUG
    /*~~~*/
	u32 i;
	u32 *p;
	/*~~~*/

	OSReport("\nError: "__FILE__ "  called an unlinked function.\n");
	OSReport("Address:      Back Chain    LR Save\n");
	for
	(
		i = 0, p = (u32 *) OSGetStackPointer(); /* get current sp */
		p && (u32) p != 0xffffffff && i++ < 16;
		p = (u32 *) * p
	)/* get caller sp */
	{
		OSReport("0x%08x:   0x%08x    0x%08x\n", p, p[0], p[1]);
	}

	OSReport("\n");
#endif
}

#ifdef __cplusplus
}
#endif


#endif GAMECUBE_DLL_BODY
#endif GAMECUBE_USE_AI2C_DLL

#endif /* __AI2C_FCTHEADER_H__ */