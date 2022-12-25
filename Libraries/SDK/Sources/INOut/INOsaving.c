/*$T INOsaving.c GC 1.138 04/06/05 10:20:13 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "Precomp.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/ERRors/ERRasser.h"
#include "INOut/INOsaving.h"

#ifdef _GAMECUBE
#include "INOut/gc/gcINOsaving.h"
#elif defined(PSX2_TARGET)
#include "INOut/ps2/ps2INOsaving.h"
#elif defined(_XBOX)
#include "INOut/xbox/xbINOsaving.h"
#elif defined(_XENON)
#include "INOut/Xenon/XeINOsaving.h"
#else
#include "INOut/win32/win32INOsaving.h"
#endif
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AItools.h"
#include "AIinterp/Sources/AIstack.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/BIGio.h"
#include "BIGfiles/BIGspecial.h"
#include <time.h>
#include "INOut/INOsaving.h"

#ifdef _XBOX
#include "INOut/INO.h"
#include "ENGine/Sources/text/text.h"
#endif
/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

INO_tdst_SavManager INO_gst_SavManager;

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_SavReinit(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_SavInitModule(void)
{
	L_memset(&INO_gst_SavManager, 0, sizeof(INO_tdst_SavManager));

#ifdef PSX2_TARGET
	INO_gst_SavManager.pfv_ModuleClose = ps2INO_SavModuleClose;
	INO_gst_SavManager.pfv_ModuleInit = ps2INO_SavModuleInit;
	INO_gst_SavManager.pfv_Update = ps2INO_SavUpdate;
	INO_gst_SavManager.pfi_Seti = ps2INO_i_SavSeti;
	INO_gst_SavManager.pfi_DbgSeti = ps2INO_i_SavDbgSeti;
#elif defined(_GAMECUBE)
	INO_gst_SavManager.pfv_ModuleClose = gcINO_SavModuleClose;
	INO_gst_SavManager.pfv_ModuleInit = gcINO_SavModuleInit;
	INO_gst_SavManager.pfv_Update = gcINO_SavUpdate;
	INO_gst_SavManager.pfi_Seti = gcINO_SavSeti;
//todo	INO_gst_SavManager.pfi_DbgSeti = gcINO_SavDbgSeti;
#elif defined(_XBOX)
	INO_gst_SavManager.pfv_ModuleClose = xbINO_SavModuleClose;
	INO_gst_SavManager.pfv_ModuleInit = xbINO_SavModuleInit;
	INO_gst_SavManager.pfv_Update = xbINO_SavUpdate;
	INO_gst_SavManager.pfi_Seti = xbINO_SavSeti;
//todo	INO_gst_SavManager.pfi_DbgSeti = xbINO_SavDbgSeti;
#elif defined(_XENON)
	INO_gst_SavManager.pfv_ModuleClose = xeINO_SavModuleClose;
	INO_gst_SavManager.pfv_ModuleInit = xeINO_SavModuleInit;
	INO_gst_SavManager.pfv_Update = xeINO_SavUpdate;
	INO_gst_SavManager.pfi_Seti = xeINO_SavSeti;
	//todo	INO_gst_SavManager.pfi_DbgSeti = xeINO_SavDbgSeti;
#else
	INO_gst_SavManager.pfv_ModuleClose = win32INO_SavModuleClose;
	INO_gst_SavManager.pfv_ModuleInit = win32INO_SavModuleInit;
	INO_gst_SavManager.pfv_Update = win32INO_SavUpdate;
	INO_gst_SavManager.pfi_Seti = win32INO_i_SavSeti;
    INO_gst_SavManager.pfi_DbgSeti = win32INO_i_SavDbgSeti;
#endif

	INO_gst_SavManager.pfv_ModuleInit();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_SavCloseModule(void)
{
    INO_i_SavRequest(INO_e_SavRq_FreeRessource, 0);
	INO_gst_SavManager.pfv_ModuleClose();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_Save_Update(void)
{
	INO_gst_SavManager.pfv_Update();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#define RoundUp(x,RoundUpValue) (((u32)(x) + (RoundUpValue) - 1) & ~((RoundUpValue) - 1)) 
 
void INO_i_SavCreateTempBuffer()
{
    if(!INO_gst_SavManager.p_Temp)
    {
#ifdef _GAMECUBE
		// Read are done with multiples of CARD_READ_SIZE.
		// Writes are done with larger size, but we write garbage after end of real data.
        INO_gst_SavManager.i_TempSize = RoundUp(INO_Cte_SavFileSize,CARD_READ_SIZE);
        INO_gst_SavManager.p_Temp = MEM_p_AllocAlign(INO_gst_SavManager.i_TempSize,32);
#else // _GAMECUBE
        INO_gst_SavManager.i_TempSize = INO_Cte_SavFileSize;
        INO_gst_SavManager.p_Temp = (char* )MEM_p_Alloc(INO_gst_SavManager.i_TempSize);
#endif // _GAMECUBE
    }
    L_memset(INO_gst_SavManager.p_Temp, 0, INO_gst_SavManager.i_TempSize);
 }
 
/*
 =======================================================================================================================
 =======================================================================================================================
 */
 
int INO_i_SavRequest(int _i_RequestId, int _i_Value)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	int i;
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	
    switch(_i_RequestId)
    {
	case INO_e_SavRq_SaveSaveBuffer:
		LINK_PrintStatusMsg( "SAVE : Save <- CP       " );
		L_memcpy( INO_gst_SavManager.ac_SaveSlotBuffer, INO_gst_SavManager.ac_CurrSlotBuffer, INO_Cte_SavOneSlotMaxSize );
		L_memcpy( INO_gst_SavManager.ast_SaveSlotDesc, INO_gst_SavManager.ast_SlotDesc, sizeof( INO_gst_SavManager.ast_SlotDesc ) );
		return 0;

	case INO_e_SavRq_RestoreSaveBuffer:
		LINK_PrintStatusMsg( "SAVE : Save -> CP       " );
		L_memcpy( INO_gst_SavManager.ac_CurrSlotBuffer, INO_gst_SavManager.ac_SaveSlotBuffer, INO_Cte_SavOneSlotMaxSize );
		L_memcpy( INO_gst_SavManager.ast_SlotDesc, INO_gst_SavManager.ast_SaveSlotDesc, sizeof( INO_gst_SavManager.ast_SlotDesc ) );
		return 0;

    case INO_e_SavRq_FreeRessource:
        if(INO_gst_SavManager.p_Temp) MEM_Free(INO_gst_SavManager.p_Temp);
        INO_gst_SavManager.p_Temp = NULL;
        INO_gst_SavManager.i_TempSize = 0;
        INO_i_SavRequest(INO_e_SavRq_ClearMessageId, 0);
        break;


    case INO_e_SavRq_ReadAllHeaders:
       	/* clear old header befoer reading */
		for(i = 0; i < INO_Cte_SavSlotNbMax; i++)
			L_memset( &INO_gst_SavManager.ast_SlotDesc[i],0 , INO_Cte_SavHeaderSize );
			
		/* launch process to read all headers */
        INO_gst_SavManager.i_CurrentSlot = -1;
        INO_i_SavCreateTempBuffer();
        break;


    case INO_e_SavRq_ReadOneProfile:
        if(_i_Value < 0) return -1;
        if(_i_Value >= INO_Cte_SavSlotNbMax) return -1;

        INO_gst_SavManager.i_CurrentSlot = _i_Value;
        INO_i_SavCreateTempBuffer();
        L_memset(INO_gst_SavManager.ac_CurrSlotBuffer, 0, INO_Cte_SavOneSlotMaxSize);
        break;


    case INO_e_SavRq_WriteOneProfile:
        if(_i_Value < 0) return -1;
        if(_i_Value >= INO_Cte_SavSlotNbMax) return -1;

        INO_gst_SavManager.i_CurrentSlot = _i_Value;
#ifdef _XBOX
		// -- Specific XBOX Language Saving --
		{
			INO_tden_LanguageId language = (INO_tden_LanguageId)TEXT_gst_Global.i_CurrentLanguage;//INO_getGameLanguage();//INO_e_English;//INO_getGameLanguage();
			INO_gst_SavManager.ast_SlotDesc[_i_Value].i_Language = language;
		}
#endif
        INO_i_SavCreateTempBuffer();
        break;

    default:
        break;
    }

	return INO_gst_SavManager.pfi_Seti(_i_RequestId, _i_Value);
}

int INO_i_SavDbgRequest(int _i_RequestId, int _i_Value)
{
#ifdef _DEBUG
    return INO_gst_SavManager.pfi_DbgSeti(_i_RequestId, _i_Value);
#else
	return 0;    
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int INO_i_Sav_SlotIsEmpty( INO_tdst_SavSlotDesc *_pst_Slot )
{
	if ( _pst_Slot->i_Time == 0 )
		return 1;
	return 0;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
