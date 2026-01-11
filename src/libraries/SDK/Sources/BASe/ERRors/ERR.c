/*$T ERR.c GC! 1.081 01/23/03 09:26:12 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/ERRors/ERRdefs.h"

void	ERR_InitModule(void);
void	ERR_CloseModule(void);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ERR_InitModule(void)
{
	L_unlink(ERR_Csz_AssertLogName);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ERR_CloseModule(void)
{
}

#ifdef _GAMECUBE
#include <stdio.h>
#include <stdarg.h>

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GC_OutputDebugString(const char *_szFormat, ...)
{
	/*~~~~~~~~~~~~~~~~~*/
#ifdef ENABLE_ERR_MSG
	va_list Marker;
	char	Buffer[4096];
	/*~~~~~~~~~~~~~~~~~*/

	/* Processing the Variable Arguments */
	va_start(Marker, _szFormat);
	vsprintf(Buffer, _szFormat, Marker);
	va_end(Marker);
	Buffer[4095] = '\0';

	OSReport(Buffer);
#else
#pragma unused(_szFormat)
#endif /* ENABLE_ERR_MSG */
}


#include "GXI_GC/GXI_Init.h"
/*
	Display error on screen.
*/
void GC_DisplayError(const char *_szMsg)
{
	char tmp[256];
	int i;

	strcpy(tmp,_szMsg);
	
	//GXI_DrawText(8L,320, tmp, 0xffeeffee, 0xff202020, 0, 6);
	
	for(i = 0; i < 64; ++i)
	{
		GXI_ErrBegin();
		GXI_ErrPrint(tmp);
		GXI_ErrEnd();
	}
}

#endif /* _GAMECUBE */
#if defined(_XBOX) || defined(_XENON)

/*
 =======================================================================================================================
 allow to break into the caller
 =======================================================================================================================
 */
void ERR_CallerBreak(void)
{
	DebugBreak();
}

#endif 
 