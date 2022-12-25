/*$T gcSND_Debug.c GC! 1.097 05/31/02 17:45:35 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef _GAMECUBE

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */
#include <stdio.h>
#include "SouND/Sources/gc/gcSND_Debug.h"
#include "SouND/Sources/gc/gcSND_Stream.h"
#include "SouND/Sources/SNDfx.h"
#include "SouND/Sources/gc/gcSNDfx.h"
#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */
#ifdef __gcSND_Dbg_Err__ 
void gcSND_PrintAssert(const char *_szFormat, ...)
{
    va_list         Marker;
    char            Buffer[4096];
    
    // Processing the Variable Arguments
    va_start(Marker, _szFormat);
    sprintf(Buffer, _szFormat, Marker);
    va_end(Marker);
    Buffer[4095] = '\0';

    OSReport(Buffer);
}
#endif

#ifdef GAMECUBE_SNDSPY
unsigned int	gcSND_gui_StreamSpyVal[] = { 0, 1, 2, 3 };

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */
 
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_StreamSpy(unsigned int _ui_Val, char *func, char *file, int line)
{
	/*~~*/
	int i;
	/*~~*/

	_ui_Val = _ui_Val - (unsigned int) gcSND_gax_StreamList;
	_ui_Val = _ui_Val / sizeof(gcSND_tdst_Stream);
	for(i = 0; i < (sizeof(gcSND_gui_StreamSpyVal) / sizeof(int)); i++)
	{
		if(_ui_Val == gcSND_gui_StreamSpyVal[i])
		{
			OSReport("[SPY](%x) : %s(%d):[%s]\n", gcSND_gui_StreamSpyVal[i], file, line, func);
		}
	}
}

#endif /* GAMECUBE_SNDSPY */

/*$4
 ***********************************************************************************************************************
    debug menu
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_GetCurrentFxModeName(char *pz, int aux)
{
	if(!pz) return;
	if(aux == 0)
	{
		switch(gcSND_i_FxGetMode(NULL))
		{
		case SND_Cte_FxMode_Off:				sprintf(pz, "%s", "Off"); break;
		case SND_Cte_FxMode_Room:				sprintf(pz, "%s", "Room"); break;
		case SND_Cte_FxMode_StudioA:			sprintf(pz, "%s", "StdA"); break;
		case SND_Cte_FxMode_StudioB:			sprintf(pz, "%s", "StdB"); break;
		case SND_Cte_FxMode_StudioC:			sprintf(pz, "%s", "StdC"); break;
		case SND_Cte_FxMode_Hall:				sprintf(pz, "%s", "Hall"); break;
		case SND_Cte_FxMode_Space:				sprintf(pz, "%s", "Space"); break;
		case SND_Cte_FxMode_Echo:				sprintf(pz, "%s", "Echo"); break;
		case SND_Cte_FxMode_Delay:				sprintf(pz, "%s", "Delay"); break;
		case SND_Cte_FxMode_Pipe:				sprintf(pz, "%s", "Pipe"); break;
		default:								sprintf(pz, "%s", "Bad"); break;
		}
	}
}

void gcSND_UpdateCurrentFxMode(int aux, int inc)
{
    if(aux == 0)
    {
        if(inc >0)
        {
    		switch(gcSND_i_FxGetMode(NULL))
    		{
    		case SND_Cte_FxMode_Off:				gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_Room); break;
    		case SND_Cte_FxMode_Room:				gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_StudioA); break;
    		case SND_Cte_FxMode_StudioA:			gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_StudioB); break;
    		case SND_Cte_FxMode_StudioB:			gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_StudioC); break;
    		case SND_Cte_FxMode_StudioC:			gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_Hall); break;
    		case SND_Cte_FxMode_Hall:				gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_Space); break;
    		case SND_Cte_FxMode_Space:				gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_Echo); break;
    		case SND_Cte_FxMode_Echo:				gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_Delay); break;
    		case SND_Cte_FxMode_Delay:				gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_Pipe); break;
    		case SND_Cte_FxMode_Pipe:				gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_Off); break;

    		default:								gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_Room); break;
    		}
    		gcSND_i_FxEnable(NULL);
    	}
    	else
    	{
    		switch(gcSND_i_FxGetMode(NULL))
    		{
    		case SND_Cte_FxMode_Off:				gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_Pipe);break;
    		case SND_Cte_FxMode_Room:				gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_Off); break;
    		case SND_Cte_FxMode_StudioA:			gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_Room); break;
    		case SND_Cte_FxMode_StudioB:			gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_StudioA);break;
    		case SND_Cte_FxMode_StudioC:			gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_StudioB);break;
    		case SND_Cte_FxMode_Hall:				gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_StudioC); break;
    		case SND_Cte_FxMode_Space:				gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_Hall);break;
    		case SND_Cte_FxMode_Echo:				gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_Space); break;
    		case SND_Cte_FxMode_Delay:				gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_Echo); break;
    		case SND_Cte_FxMode_Pipe:				gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_Delay); break;

    		default:								gcSND_i_FxSetMode(NULL, SND_Cte_FxMode_Off); break;
    		}
    		gcSND_i_FxEnable(NULL);
    	}
    }
}
/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* _GAMECUBE */
