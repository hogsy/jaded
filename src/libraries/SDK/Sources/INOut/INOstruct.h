/*$T INOstruct.h GC! 1.097 10/13/00 16:04:59 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __INOSTRUCT_H__
#define __INOSTRUCT_H__

#include "BASe/BAStypes.h"

#ifdef PSX2_TARGET
#include <libpad.h>
#endif

#include "BASe/ERRors/ERRasser.h"
#include "BASe/CLIbrary/CLIfile.h"


typedef enum	INO_tden_ButtonId_
{
	e_CROSS			= 0,
	e_CIRCLE		= 1,
	e_SQUARE		= 2,
	e_TRIANGLE		= 3,
	e_L1			= 4,
	e_R1			= 5,
	e_L2			= 6,
	e_R2			= 7,	
	e_SEL			= 8,
	e_START			= 9,	
	e_R3			= 10,
	e_L3			= 11,	
	e_UP			= 12,
	e_RIGHT			= 13,
	e_DOWN			= 14,
	e_LEFT			= 15,
	e_Status		= 16,
	e_LowerUnit		= 17,
	e_UpperUnit		= 18,	
	e_AnaStickRightX= 19,
	e_AnaStickRightY= 20,
	e_AnaStickLeftX = 21,
	e_AnaStickLeftY = 22,
	e_InfoR			= 23,
	e_InfoL			= 24,
	e_InfoU			= 25,
	e_InfoD			= 26,
	e_InfoT			= 27,
	e_InfoC			= 28,
	e_InfoX			= 29,
	e_InfoS			= 30,
	e_InfoL1		= 31,
	e_InfoR1		= 32,
	e_InfoL2		= 33,
	e_InfoR2		= 34,
	e_LastButtonId	= 35
} INO_tden_ButtonId;


#endif /* __INOSTRUCT_H__ */
