/*$T win32INOsaving.c GC 1.138 04/12/05 15:49:20 */


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
#include "BASe/MEMory/MEM.h"
#include "INOut/INOsaving.h"
#include "INOut/win32/win32INOsaving.h"
#include "INOut/INOjoystick.h"
#include "BIGfiles/BIGspecial.h"



/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define win32INO_Cte_SavFileName	"KingKong.sav"
#define win32INO_Cte_SavLatency		100

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
int win32INO_gi_CardId = -1;
INO_tden_SavUserMessage win32INO_ge_SavMsg = INO_e_SavMsg_None;
//
int						win32INO_gi_slot = 0;
int						win32INO_gi_EmulLatency = 0;
int						win32INO_gi_IsWorking= 0;
int						win32INO_gi_ConfirmOnSuccess = 1;
INO_tden_SavUserMessage win32INO_ge_SavEmulLatencyMsg = INO_e_SavMsg_None;
//
INO_tden_SavUserMessage win32INO_ge_EmulError = INO_e_SavMsg_None;

extern int AI_EvalFunc_IoButtonJustPressed_C(int _i_Button);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32INO_SavModuleInit(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32INO_SavModuleClose(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int win32INO_i_ReadFile(char *pBuff, int iToRead, int *piRead)
{
	/*~~~~~~~~~~~~*/
	HANDLE	hFile;
	BOOL	bResult;
    DWORD   dwSize;
    /*~~~~~~~~~~~~*/

	if(piRead) *piRead = 0;

	hFile = CreateFile(win32INO_Cte_SavFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

	/* Read just 4 BYTES to determine if buffer is crypted */
    bResult = ReadFile(hFile, pBuff, 4, &dwSize, NULL);

	if(BIG_b_IsBufferCrypted(pBuff))
	{
		char	*pTemp;

		pTemp = (char *)MEM_p_Alloc(iToRead + ENCRYPT_ADD_SIZE);

		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		bResult = ReadFile(hFile, pTemp, iToRead + ENCRYPT_ADD_SIZE, &dwSize, NULL);
		BIG_special_Decryption(&pBuff, &pTemp, NULL, iToRead + ENCRYPT_ADD_SIZE, INO_gst_SavManager.ul_CRC);

		BIG_ComputeCRC(&INO_gst_SavManager.ul_CRC[6], pBuff, iToRead);
		
		if(INO_gst_SavManager.ul_CRC[0] != INO_gst_SavManager.ul_CRC[6])
		{
			ERR_X_Assert(0);
		}

		MEM_Free(pTemp);
	}
	else
	{
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		bResult = ReadFile(hFile, pBuff, iToRead, &dwSize, NULL);
	}

	CloseHandle(hFile);

    if(piRead) *piRead = dwSize;

	if(bResult)
		return 0;
	else
		return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int win32INO_i_WriteFile(char *pBuff, int iToWrite, int *piWritten)
{
	/*~~~~~~~~~~~~*/
	HANDLE	hFile;
	BOOL	bResult;
    DWORD   dwSize;
	char	ac_Key[ENCRYPT_KEY_SIZE + ENCRYPT_CRC_SIZE];
	char	*pTemp;
	/*~~~~~~~~~~~~*/

	if(piWritten) *piWritten = 0;

	hFile = CreateFile(win32INO_Cte_SavFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	
	// Compute Encrypting Key ...
	BIG_ComputeKey(ac_Key, pBuff, iToWrite);

	// Encryption ...
	pTemp = (char *)MEM_p_Alloc(iToWrite + ENCRYPT_ADD_SIZE);
	BIG_special_Encryption(&pTemp, &pBuff, ac_Key, iToWrite, TRUE);
	bResult = WriteFile(hFile, pTemp, iToWrite + ENCRYPT_ADD_SIZE, &dwSize, NULL);
	CloseHandle(hFile);

	MEM_Free(pTemp);

    if(piWritten) *piWritten = dwSize;

	if(bResult)
		return 0;
	else
		return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32INO_i_Save()
{
	int result;

	LINK_PrintStatusMsg( "SAVE :         CP (save)" );

	result = win32INO_i_ReadFile(INO_gst_SavManager.p_Temp, INO_gst_SavManager.i_TempSize, NULL);
	if(result)
	{
		// no existing save
        L_memset(INO_gst_SavManager.p_Temp, 0, INO_gst_SavManager.i_TempSize);
	}
			
	/* copy the header */
	L_memcpy
	(
		INO_gst_SavManager.p_Temp + win32INO_gi_slot * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize),
		&INO_gst_SavManager.ast_SlotDesc[win32INO_gi_slot],
		INO_Cte_SavHeaderSize
	);

	/* copy data */
	L_memcpy
	(
		INO_gst_SavManager.p_Temp + win32INO_gi_slot * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize) + INO_Cte_SavHeaderSize,
		INO_gst_SavManager.ac_CurrSlotBuffer,
		INO_Cte_SavOneSlotMaxSize
	);

	result = win32INO_i_WriteFile(INO_gst_SavManager.p_Temp, INO_gst_SavManager.i_TempSize, NULL);
	if(result)
	{
		win32INO_ge_SavEmulLatencyMsg = INO_e_SavMsg_ErrorWhileSaving;
	}
	else
	{
		/* ok */
		win32INO_ge_SavEmulLatencyMsg = INO_e_SavMsg_SuccessWhileSaving;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32INO_SavUpdate(void)
{
	if(win32INO_gi_EmulLatency)
	{
		win32INO_gi_EmulLatency--;
		if(!win32INO_gi_EmulLatency)
		{
			win32INO_ge_SavMsg = win32INO_ge_SavEmulLatencyMsg;
			win32INO_ge_SavEmulLatencyMsg = INO_e_SavMsg_None;

            if(win32INO_ge_EmulError != INO_e_SavMsg_None)
            {
                if((win32INO_ge_EmulError == INO_e_SavMsg_ErrorWhileLoading) && (win32INO_ge_SavMsg == INO_e_SavMsg_SuccessWhileSaving))
                    win32INO_ge_EmulError = INO_e_SavMsg_ErrorWhileSaving;

                win32INO_ge_SavMsg = win32INO_ge_EmulError;
                win32INO_ge_EmulError = INO_e_SavMsg_None;
            }
            
		}
	}
	else
	{
            switch(win32INO_ge_SavMsg)
            {
			case	INO_e_SavMsg_None	:				
				win32INO_gi_IsWorking=0;
				break;

			case	INO_e_SavMsg_Accessing				:
				break;

			case INO_e_SavMsg_ConfirmSave:
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Triangle))
					win32INO_ge_SavMsg = INO_e_SavMsg_CancelSave;
				else if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
				{
					win32INO_i_Save();
					win32INO_gi_EmulLatency = win32INO_Cte_SavLatency;
					win32INO_ge_SavMsg = INO_e_SavMsg_Accessing;
				}
				break;

			case INO_e_SavMsg_CancelSave:
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Triangle))
				{
					win32INO_ge_SavMsg = INO_e_SavMsg_Retrying;
					win32INO_gi_IsWorking = 0;
				}
				else if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
					win32INO_gi_IsWorking = 0;
				break;

			
			case	INO_e_SavMsg_NoCard					:
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Triangle))
				{
					win32INO_ge_SavMsg = INO_e_SavMsg_Retrying;
					win32INO_gi_IsWorking = 0;
				}
				else if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
					win32INO_gi_IsWorking=0;
				break;
				
			case	INO_e_SavMsg_UnformattedCard		:
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Triangle))
				{
					win32INO_ge_SavMsg = INO_e_SavMsg_Retrying;
					win32INO_gi_IsWorking = 0;
				}
				else if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
				{
					win32INO_gi_IsWorking=1;
					win32INO_ge_SavMsg = INO_e_SavMsg_Formating;
					win32INO_ge_SavEmulLatencyMsg = INO_e_SavMsg_SuccessWhileFormating;
					win32INO_gi_EmulLatency = win32INO_Cte_SavLatency;
					if(win32INO_ge_EmulError == INO_e_SavMsg_ErrorWhileFormating)
					{
						win32INO_ge_SavEmulLatencyMsg = INO_e_SavMsg_SuccessWhileFormating;
						win32INO_ge_SavEmulLatencyMsg = INO_e_SavMsg_None;
					}
				}
				break;
				
			case	INO_e_SavMsg_NoSaving				:
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Triangle))
				{
					win32INO_gi_IsWorking = 0;
					win32INO_ge_SavMsg = INO_e_SavMsg_Retrying;
				}
				else if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
					win32INO_gi_IsWorking = 0;
				break;
				
			case	INO_e_SavMsg_NoMoreSpace			:
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
					win32INO_gi_IsWorking=0;
				break;
				
			case	INO_e_SavMsg_ErrorWhileSaving		:
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Triangle))
				{
					win32INO_ge_SavMsg = INO_e_SavMsg_Retrying;
					win32INO_gi_IsWorking = 0;
				}
				else if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
					win32INO_gi_IsWorking=0;
				break;
				
			case	INO_e_SavMsg_ErrorWhileLoading		:
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Triangle))
				{
					win32INO_ge_SavMsg = INO_e_SavMsg_Retrying;
					win32INO_gi_IsWorking = 0;
				}
				else if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
					win32INO_gi_IsWorking=0;
				break;
				
			case	INO_e_SavMsg_ErrorWhileFormating	:
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Triangle))
				{
					win32INO_ge_SavMsg = INO_e_SavMsg_Retrying;
					win32INO_gi_IsWorking = 0;
				}
				else if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
					win32INO_gi_IsWorking=0;
				break;
				
			case	INO_e_SavMsg_SuccessWhileSaving		:
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
					win32INO_gi_IsWorking=0;
				break;
				
			case	INO_e_SavMsg_SuccessWhileLoading	:
				if (win32INO_gi_ConfirmOnSuccess)
				{
					if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
						win32INO_gi_IsWorking=0;
				}
				else
				{
					win32INO_gi_IsWorking = 0;
				}
				break;
				
			case	INO_e_SavMsg_SuccessWhileFormating	:
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
					win32INO_gi_IsWorking=0;
				break;
				
			case 	INO_e_SavMsg_Retrying:
			case 	INO_e_SavMsg_Formating	:
				break;
 
			
			default:
				win32INO_gi_IsWorking=0;
				break;
            }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int win32INO_i_SavSeti(int rq, int val)
{
	/*~~~~~~~*/
	int result;
	int i;
	/*~~~~~~~*/

	switch(rq)
	{
	case INO_e_SavRq_ClearMessageId:
		win32INO_ge_SavMsg = INO_e_SavMsg_None;
		win32INO_ge_SavEmulLatencyMsg = INO_e_SavMsg_None;
		win32INO_gi_EmulLatency = 0;
		break;

	case INO_e_SavRq_ReadAllHeaders:
		win32INO_gi_IsWorking = 1;
		win32INO_gi_ConfirmOnSuccess = 0;
		
		/* VL : clear header */
		for(i = 0; i < INO_Cte_SavSlotNbMax; i++)
			L_memset( &INO_gst_SavManager.ast_SlotDesc[i],0 , INO_Cte_SavHeaderSize );

		result = win32INO_i_ReadFile(INO_gst_SavManager.p_Temp, INO_gst_SavManager.i_TempSize, NULL);
		if(result)
		{
			win32INO_ge_SavEmulLatencyMsg = INO_e_SavMsg_NoSaving;
		}
		else
		{
			for(i = 0; i < INO_Cte_SavSlotNbMax; i++)
			{
				L_memcpy
				(
					&INO_gst_SavManager.ast_SlotDesc[i],
					INO_gst_SavManager.p_Temp + i * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize),
					INO_Cte_SavHeaderSize
				);
			}

			win32INO_ge_SavEmulLatencyMsg = INO_e_SavMsg_SuccessWhileLoading;
		}

		win32INO_gi_EmulLatency = win32INO_Cte_SavLatency;
		win32INO_ge_SavMsg = INO_e_SavMsg_Accessing;
		break;

	case INO_e_SavRq_ReadOneProfile:
		win32INO_gi_IsWorking = 1;
		win32INO_gi_ConfirmOnSuccess = 1;
		/* read all profiles */
		result = win32INO_i_ReadFile(INO_gst_SavManager.p_Temp, INO_gst_SavManager.i_TempSize, NULL);
		if(result)
		{
			win32INO_ge_SavEmulLatencyMsg = INO_e_SavMsg_ErrorWhileLoading;
		}
		else
		{
			LINK_PrintStatusMsg( "SAVE :         CP (read)" );

			/* copy the header */
			L_memcpy
			(
				&INO_gst_SavManager.ast_SlotDesc[val],
				INO_gst_SavManager.p_Temp + val * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize),
				INO_Cte_SavHeaderSize
			);

			/* the data */
			L_memcpy
			(
				INO_gst_SavManager.ac_CurrSlotBuffer,
				INO_gst_SavManager.p_Temp +
					val *
					(INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize) +
					INO_Cte_SavHeaderSize,
				INO_Cte_SavOneSlotMaxSize
			);

			/* ok */
			win32INO_ge_SavEmulLatencyMsg = INO_e_SavMsg_SuccessWhileLoading;
		}

		win32INO_gi_EmulLatency = win32INO_Cte_SavLatency;
		win32INO_ge_SavMsg = INO_e_SavMsg_Accessing;
		break;

	case INO_e_SavRq_WriteOneProfile:
		win32INO_gi_IsWorking = 1;
		win32INO_gi_ConfirmOnSuccess = 1;
		win32INO_gi_slot = val;
		win32INO_ge_SavMsg = INO_e_SavMsg_ConfirmSave;
		//win32INO_i_Save();
		//win32INO_gi_EmulLatency = win32INO_Cte_SavLatency;
		//win32INO_ge_SavMsg = INO_e_SavMsg_Accessing;
		break;

	case INO_e_SavRq_Status:
		return win32INO_gi_IsWorking; /* work is finished */

	case INO_e_SavRq_CardId:
		if((val == 0) || (val == 1))
			win32INO_gi_CardId = val;
		else
			win32INO_gi_CardId = -1;
		return win32INO_gi_CardId;

	case INO_e_SavRq_UserMessageId:
		return win32INO_ge_SavMsg;

	case INO_e_SavRq_Space:
		return (1024 * 1024);

	case INO_e_SavRq_TestMemCard:
		win32INO_ge_SavMsg = INO_e_SavMsg_None;
		win32INO_gi_IsWorking = 0;
		break;

    default:
		return -10;
	}

	return 0;
}

int	win32INO_i_SavDbgSeti(int rq,int val)
{
    switch(rq)
    {
    case INO_e_SavDbgRq_NoCardInserted:win32INO_ge_EmulError = INO_e_SavMsg_NoCard;break;
    case INO_e_SavDbgRq_NoSpace		:win32INO_ge_EmulError = INO_e_SavMsg_NoMoreSpace;break;
    case INO_e_SavDbgRq_NoSave		:win32INO_ge_EmulError = INO_e_SavMsg_NoSaving;break;
    case INO_e_SavDbgRq_Unformatted	:win32INO_ge_EmulError = INO_e_SavMsg_UnformattedCard;break;
    case INO_e_SavDbgRq_AccessError	:win32INO_ge_EmulError = INO_e_SavMsg_ErrorWhileLoading;break;
    case INO_e_SavDbgRq_ErrorWhileFormating:win32INO_ge_EmulError = INO_e_SavMsg_ErrorWhileFormating	;break;
    default:return -1;
    }

    return 0;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int win32INO_i_FileGetSize(char *_sz_Name)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	L_FILE		x_File;
	LONG		l_Size;
	char		sz[L_MAX_PATH];
	extern int	BIG_fseek(L_FILE _hfile, unsigned int _offset, int _origin);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sprintf(sz, "%s", _sz_Name);
	x_File = L_fopen(sz, L_fopen_RB);
	if(CLI_FileOpen(x_File))
	{
		L_fseek(x_File, 0, SEEK_END);
		l_Size = ftell(x_File);
		L_fclose(x_File);
	}

	return l_Size;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
