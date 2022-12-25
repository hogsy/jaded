/*$T xbINOsaving.c GC 1.138 04/08/05 15:05:33 */


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
#include "INOut/xbox/xbINOsaving.h"
#include "INOut/INOjoystick.h"
#include "BIGfiles/BIGspecial.h"
#include "INOut/INO.h"
#include "ENGine/Sources/text/text.h"

#define ENCRYPTSAVE


//NO_tden_SavUserMessage xbINO_ge_SavMsg = INO_e_SavMsg_None;
#define xbINO_Cte_SavFileName	"KingKong.sav"
#define xbINO_Cte_SavLatency		100

const DWORD BLOCK_SIZE = 16 * 1024; // TCR C4-12 Space Display
const DWORD REQUIRED_SAVED_BLOCKS = 5;   //Number of block required for saving on XBOX disk
static int sloslo=0;


// -- WARNING Bricolage XBOX pour nombre de blocks sauvegarde --
// dans STRparce.c si dans les textes /m1/* alors remplace * par le numero du blocks sauvegarde
short BlocksSizeNeeded=0;



#define MAX_FILE_SIZE (BLOCK_SIZE*REQUIRED_SAVED_BLOCKS)


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
int xbINO_gi_CardId = -1;
INO_tden_SavUserMessage xbINO_ge_SavMsg = INO_e_SavMsg_None;
//
int						xbINO_gi_EmulLatency = 0;
int						xbINO_gi_IsWorking= 0;

int						xbINO_gi_ConfirmOnSuccess = 1;
int						xbINO_gi_ConfirmOverWrite = 0;
INO_tden_SavUserMessage xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_None;
//
INO_tden_SavUserMessage xbINO_ge_EmulError = INO_e_SavMsg_None;

extern int AI_EvalFunc_IoButtonJustPressed_C(int _i_Button);

/*
 =======================================================================================================================
 =======================================================================================================================
 */

int CalculateSignature(XCALCSIG_SIGNATURE *xsig, BYTE *pSaveInfo, int SaveSize)
{
	HANDLE hSig;

	// calc the signature.
    hSig = XCalculateSignatureBegin( XCALCSIG_FLAG_SAVE_GAME );
    if( INVALID_HANDLE_VALUE != hSig ) 
	{
        XCalculateSignatureUpdate( hSig, pSaveInfo, SaveSize );
		XCalculateSignatureEnd( hSig, xsig );

		return 1;
    } 
	else 
	{
		memset(xsig,0,sizeof(XCALCSIG_SIGNATURE));
        return -1;
    }               
}

int xbINO_i_TestFreeSpace(int size)//int _i_Slot
{
	/*~~~~~*/
	int next;
	/*~~~~~*/
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		CHAR			strRootPath[4] = "U:\\";
		DWORD			dwBlocks = XGetDisplayBlocks(strRootPath);
		int				qwTotalBytes = 0;
		int				qwFreeBytes = 0;
		int				qwUsedBytes = 0;
		int				qwFreeBlock = 0;
		ULARGE_INTEGER	lFreeBytesAvailable;
		ULARGE_INTEGER	lTotalNumberOfBytes;
		ULARGE_INTEGER	lTotalNumberOfFreeBytes;
		BOOL			bSuccess = GetDiskFreeSpaceEx
			(
				strRootPath,
				&lFreeBytesAvailable,
				&lTotalNumberOfBytes,
				&lTotalNumberOfFreeBytes
			);
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(!bSuccess) return -1;//FALSE;

		qwTotalBytes = lTotalNumberOfBytes.QuadPart;
		qwFreeBytes = lFreeBytesAvailable.QuadPart;
		qwUsedBytes = qwTotalBytes - qwFreeBytes;
		qwFreeBlock = qwFreeBytes / BLOCK_SIZE;
		if (qwFreeBytes<MAX_FILE_SIZE) 
		{
			if (qwFreeBlock<REQUIRED_SAVED_BLOCKS)
			// -- WARNING Bricolage XBOX pour nombre de blocks sauvegarde --
			// dans STRparce.c si dans les textes /m1/* alors remplace * par le numero du blocks sauvegarde
			BlocksSizeNeeded = REQUIRED_SAVED_BLOCKS-qwFreeBlock;
			else BlocksSizeNeeded = 0;
			return -1;//FALSE;
		}
/*		INO_gi_FreeSpace = dwBlocks * BLOCK_SIZE;
		INO_gi_FreeSpace = qwFreeBytes;*/
	}

	return 0;
}

int xbINO_i_ReadFile(char *pBuff, int iToRead, int *piRead,int val)
{
	/*~~~~~~~~~~~~*/
	HANDLE	hFile;
	BOOL	bResult;
    DWORD   dwSize;
    /*~~~~~~~~~~~~*/

	WCHAR	GameName[128];
	char	RootPathName[4] = "U:\\";
	char	PathBuffer[256];
	DWORD	dwSuccess;
	int counter;

	HANDLE hFind;
	XCALCSIG_SIGNATURE xsig, SavedSig;
	XGAME_FIND_DATA xgfd;
	BY_HANDLE_FILE_INFORMATION  fileInfo;
	hFind = XFindFirstSaveGame( RootPathName, &xgfd );

	if( INVALID_HANDLE_VALUE == hFind ) 
	{
		//XFindClose( hFind );	
		return -1;
	}

	if (val>0)
	for( counter=0;counter<val;counter++)
	{
//		int slip;
		if (!XFindNextSaveGame( hFind, &xgfd )) return -1;
		swprintf(GameName,L"%S",INO_gst_SavManager.ast_SlotDesc[val].asz_Name);
//		slip = wcscmp  (xgfd.szSaveGameName,GameName,sizeof(GameName));
//		if ( !strncmp(xgfd.szSaveGameName,GameName,10)&& counter+1!=val )
		if ( !wcscmp  (xgfd.szSaveGameName,GameName,sizeof(GameName)) && counter+1!=val )

		//if ( (WCHAR)xgfd.szSaveGameName == GameName )//&& counter!=val)
		
		{
		
			XFindClose( hFind );	
			
			if (xbINO_gi_ConfirmOverWrite==1)
			XDeleteSaveGame(RootPathName,xgfd.szSaveGameName);
			return -3;
		}
	}
	
	if( INVALID_HANDLE_VALUE == hFind ) 
	{
		XFindClose( hFind );	
		return -1;
	}

	dwSuccess = XCreateSaveGame(RootPathName, xgfd.szSaveGameName, OPEN_EXISTING, 0, PathBuffer, 256);
	//CloseHandle(hFile);

	if(piRead) *piRead = 0;

	strcat(PathBuffer,xbINO_Cte_SavFileName);
	hFile = CreateFile(PathBuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		XFindClose( hFind );
		CloseHandle(hFile);
		return -2;
	}

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
 

		GetFileInformationByHandle(hFile,&fileInfo);
		if (fileInfo.nFileSizeHigh>0 || fileInfo.nFileSizeLow>MAX_FILE_SIZE) 
		{
			//wrong signature
			CloseHandle(hFile);
			XFindClose( hFind );
			return -2;
		}
#ifdef ENCRYPTSAVE

	/* Read just 4 BYTES to determine if buffer is crypted */
    bResult = ReadFile(hFile, pBuff, 4, &dwSize, NULL);

	if( BIG_b_IsBufferCrypted(pBuff))
	{
		char	*pTemp;

		pTemp = (char *)MEM_p_Alloc(iToRead + ENCRYPT_ADD_SIZE);

		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		bResult = ReadFile(hFile, pTemp, iToRead + ENCRYPT_ADD_SIZE, &dwSize, NULL);

		if ( !bResult )
		{
			//wrong signature
			CloseHandle(hFile);
			XFindClose( hFind );
			return -2;
		}

		BIG_special_Decryption(&pBuff, &pTemp, NULL, iToRead + ENCRYPT_ADD_SIZE, INO_gst_SavManager.ul_CRC);

		BIG_ComputeCRC(&INO_gst_SavManager.ul_CRC[6], pBuff, iToRead);
		
		if(INO_gst_SavManager.ul_CRC[0] != INO_gst_SavManager.ul_CRC[6])
		{
			MEM_Free(pTemp);
			CloseHandle(hFile);
			XFindClose( hFind );
			return -2;
		}
		CalculateSignature(&xsig,pTemp, dwSize);
		MEM_Free(pTemp);		
	}
	else
	{
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		bResult = ReadFile(hFile, pBuff, iToRead, &dwSize, NULL);

		if ( !bResult )
		{
			//wrong signature
			CloseHandle(hFile);
			XFindClose( hFind );
			return -2;
		}

		CalculateSignature(&xsig,pBuff, dwSize);
	}	
		
#else
	bResult = ReadFile(hFile, pBuff, iToRead, &dwSize, NULL);
	//bResult = L_fread (pBuff,1,&dwSize,hFile);
		if ( !bResult )
		{
			//wrong signature
			CloseHandle(hFile);
			XFindClose( hFind );
			return -2;
		}

		CalculateSignature(&xsig,pBuff, dwSize);
#endif
		
		L_fread(&SavedSig, 1, XCALCSIG_SIGNATURE_SIZE, hFile);
		if(memcmp(&xsig,&SavedSig,XCALCSIG_SIGNATURE_SIZE) != 0)
		{
			//wrong signature
			CloseHandle(hFile);
			XFindClose( hFind );
			return -2;
		}
	CloseHandle(hFile);



    if(piRead) *piRead = dwSize;

	/*if(bResult)
		return 0;
	else
		return -1;*/
	
	XFindClose( hFind );
	if(bResult)
		return 0;
	else
		return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

int xbINO_i_WriteFile(char *pBuff, int iToWrite, int *piWritten,int val)
{
	WCHAR	GameName[32];
	//char	GameName[32];
	char	RootPathName[4] = "U:\\";
	char	PathBuffer[256];
	DWORD	dwSuccess;
	char	ac_Key[ENCRYPT_KEY_SIZE + ENCRYPT_CRC_SIZE];
	char	*pTemp;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	HANDLE	hFile;
	BOOL	bResult;
    DWORD   dwSize;
	int counter;
	XCALCSIG_SIGNATURE xsig, SavedSig;

	//lstrcatW(GameName,INO_gst_SavManager.ast_SlotDesc[val].asz_Name);
	//L_strcpy(GameName,INO_gst_SavManager.ast_SlotDesc[val].asz_Name);
	//wcscpy(GameName,INO_gst_SavManager.ast_SlotDesc[val].asz_Name);
	
	
/*	// -- Specific XBOX Language Saving --
	{
	    INO_tden_LanguageId language = (INO_tden_LanguageId)TEXT_gst_Global.i_CurrentLanguage;//INO_getGameLanguage();//INO_e_English;//INO_getGameLanguage();
		INO_gst_SavManager.ast_SlotDesc[val].i_Language = language;
	}*/
	// -----------------------------------

	swprintf(GameName,L"%S",INO_gst_SavManager.ast_SlotDesc[val].asz_Name);
	dwSuccess = XCreateSaveGame(RootPathName, GameName, OPEN_ALWAYS, 0, PathBuffer, 256);

	if(piWritten) *piWritten = 0;

	//lstrcpyW(PathBuffer,xbINO_Cte_SavFileName);
	strcat(PathBuffer,xbINO_Cte_SavFileName);
	
	hFile = CreateFile(PathBuffer, GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		return -1;
	}

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

#ifdef ENCRYPTSAVE

	// Compute Encrypting Key ...
	BIG_ComputeKey(ac_Key, pBuff, iToWrite);

	// Encryption ...
	pTemp = (char *)MEM_p_Alloc(iToWrite + ENCRYPT_ADD_SIZE);
	BIG_special_Encryption(&pTemp, &pBuff, ac_Key, iToWrite, TRUE);
	bResult = WriteFile(hFile, pTemp, iToWrite + ENCRYPT_ADD_SIZE, &dwSize, NULL);
	//CloseHandle(hFile);
	CalculateSignature(&xsig,pTemp, dwSize);
	MEM_Free(pTemp);
#else
	bResult = WriteFile(hFile, pBuff, iToWrite, &dwSize, NULL);
	CalculateSignature(&xsig,pBuff, dwSize);
#endif

	//CalculateSignature(&xsig,pBuff, dwSize);
	//WriteFile(hFile, &xsig, iToWrite, XCALCSIG_SIGNATURE_SIZE, NULL);
	L_fwrite(&xsig, 1, XCALCSIG_SIGNATURE_SIZE, hFile);

	CloseHandle(hFile);

    if(piWritten) *piWritten = dwSize;

	/*if(bResult)
		return 0;
	else
		return -1;*/

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbINO_SavModuleClose(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbINO_SavModuleInit(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbINO_SavUpdate(void)
{
	if(xbINO_gi_EmulLatency)
	{
		xbINO_gi_EmulLatency--;
		if(!xbINO_gi_EmulLatency)
		{
			xbINO_ge_SavMsg = xbINO_ge_SavEmulLatencyMsg;
			xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_None;

            if(xbINO_ge_EmulError != INO_e_SavMsg_None)
            {
                if((xbINO_ge_EmulError == INO_e_SavMsg_ErrorWhileLoading) && (xbINO_ge_SavMsg == INO_e_SavMsg_SuccessWhileSaving))
                    xbINO_ge_EmulError = INO_e_SavMsg_ErrorWhileSaving;

 				xbINO_ge_SavMsg = xbINO_ge_EmulError;
                xbINO_ge_EmulError = INO_e_SavMsg_None;
            }
            
		}
	}
	else
	{
            switch(xbINO_ge_SavMsg)
            {
			case	INO_e_SavMsg_None	:				
				xbINO_gi_IsWorking=0;
				break;

			case	INO_e_SavMsg_Accessing				:
				break;
			
			
			case INO_e_SavMsg_BadFileOnMc:
				//INO_e_SavMsg_None;
				xbINO_ge_SavMsg = INO_e_SavMsg_BadFileOnMc;
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
				{
					xbINO_ge_SavMsg=xbINO_gi_IsWorking = 0;
					xbINO_ge_SavMsg = INO_e_SavMsg_NoSaving;
					//xbINO_gi_EmulLatency = xbINO_Cte_SavLatency;
				}
				break;
			
			case	INO_e_SavMsg_NoCard					:
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Triangle))
				{
					xbINO_ge_SavMsg = INO_e_SavMsg_Retrying;
					xbINO_gi_IsWorking = 0;
				}
				else if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
					xbINO_gi_IsWorking=0;
				break;
				
			case	INO_e_SavMsg_UnformattedCard		:
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Triangle))
				{
					xbINO_ge_SavMsg = INO_e_SavMsg_Retrying;
					xbINO_gi_IsWorking = 0;
				}
				else if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
				{
					xbINO_gi_IsWorking=1;
					xbINO_ge_SavMsg = INO_e_SavMsg_Formating;
					xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_SuccessWhileFormating;
					xbINO_gi_EmulLatency = xbINO_Cte_SavLatency;
					if(xbINO_ge_EmulError == INO_e_SavMsg_ErrorWhileFormating)
					{
						xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_SuccessWhileFormating;
						xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_None;
					}
				}
				break;
				
			case	INO_e_SavMsg_NoSaving				:
/*				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Triangle))
				{
					xbINO_gi_IsWorking = 0;
					xbINO_ge_SavMsg = INO_e_SavMsg_Retrying;
				}
				else if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))*/
					xbINO_gi_IsWorking = 0;
				break;
				
			case	INO_e_SavMsg_NoMoreSpace			:
				
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross)) 
				xbINO_gi_IsWorking=0;
				else if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Triangle))
				{	
					LD_LAUNCH_DASHBOARD LaunchDash;
					LaunchDash.dwReason = XLD_LAUNCH_DASHBOARD_MEMORY;
					LaunchDash.dwContext = 0;
					LaunchDash.dwParameter1 = (DWORD)('U');
					LaunchDash.dwParameter2 = BlocksSizeNeeded;
					XLaunchNewImage( NULL, (PLAUNCH_DATA)(&LaunchDash) );
					xbINO_gi_IsWorking=0;
				}

				
				break;
				
			case	INO_e_SavMsg_ErrorWhileSaving		:
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Triangle))
				{
					xbINO_ge_SavMsg = INO_e_SavMsg_Retrying;
					xbINO_gi_IsWorking = 0;
				}
				else if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
					xbINO_gi_IsWorking=0;
				break;
				
			case	INO_e_SavMsg_ErrorWhileLoading		://mauvaise sauvegarde
				/*if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Triangle))
				{
					xbINO_ge_SavMsg = INO_e_SavMsg_Retrying;
					xbINO_gi_IsWorking = 0;
				}
				else*/
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
				{	xbINO_gi_IsWorking=0;
				xbINO_ge_SavMsg = INO_e_SavMsg_NoSaving;}
				break;
				
			case	INO_e_SavMsg_ErrorWhileFormating	:
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Triangle))
				{
					xbINO_ge_SavMsg = INO_e_SavMsg_Retrying;
					xbINO_gi_IsWorking = 0;
				}
				else if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
					xbINO_gi_IsWorking=0;
				break;
				
			case	INO_e_SavMsg_ConfirmOverwriting		:
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Triangle))
				{
					//xbINO_ge_SavMsg = INO_e_SavMsg_Retrying;
					xbINO_gi_IsWorking = 0;
					xbINO_gi_ConfirmOverWrite = 0;
					xbINO_ge_SavMsg = INO_e_SavMsg_CancelSave;
				}
				else if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
				{
					xbINO_gi_IsWorking=0;
					xbINO_ge_SavMsg = INO_e_SavMsg_Retrying;
				}

				break;
			case	INO_e_SavMsg_SuccessWhileSaving		:
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
					xbINO_gi_IsWorking=0;
				break;
				
			case	INO_e_SavMsg_SuccessWhileLoading	:
				if (xbINO_gi_ConfirmOnSuccess)
				{
					if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
						xbINO_gi_IsWorking=0;
				}
				else
				{
					xbINO_gi_IsWorking = 0;
				}
				break;
				
			case	INO_e_SavMsg_SuccessWhileFormating	:
				if(AI_EvalFunc_IoButtonJustPressed_C(eBtn_Cross))
					xbINO_gi_IsWorking=0;
				break;
				
			case 	INO_e_SavMsg_Retrying:
			case 	INO_e_SavMsg_Formating	:
				break;
 
			
			default:
				xbINO_gi_IsWorking=0;
				break;
            }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_SavGeti(int i)
{
    return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
//int xbINO_SavSeti(int i, int ii)
void SetLangForDamagedGame(char* text)
{
	INO_tden_LanguageId language = (INO_tden_LanguageId)TEXT_gst_Global.i_CurrentLanguage;
	
	switch(language)
	{
		case INO_e_English:	
		sprintf(text,"..Damaged game..");return;break;
		case INO_e_French:	
		sprintf(text,"..Partie endommagée..");return;break;
		case INO_e_Spanish:	
		sprintf(text,"..Juego dañado..");return;break;
		case INO_e_German:	
		sprintf(text,"..Spiel beschädigt..");return;break;
		case INO_e_Dutch:	
		sprintf(text,"..Beschadigde game..");return;break;
		case INO_e_Italian:	
		sprintf(text,"..Gioco danneggiato..");return;break;
		case INO_e_Swedish:	
		sprintf(text,"..Skadat spel..");return;break;
		case INO_e_Danish:	
		sprintf(text,"..Beskadiget spil..");return;break;
		case INO_e_Finnish:
		sprintf(text,"..Vaurioitunut peli..");return;break;
		case INO_e_Norwegian:	
		sprintf(text,"..Skadet spill..");return;break;
	}
		sprintf(text,"..Damaged game..");
}

int xbINO_SavSeti(int rq, int val)
{
	/*~~~~~~~*/
	int result;
	int i;
	/*~~~~~~~*/

	switch(rq)
	{
	case INO_e_SavRq_ClearMessageId:
		xbINO_ge_SavMsg = INO_e_SavMsg_None;
		xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_None;
		xbINO_gi_EmulLatency = 0;
		break;

	case INO_e_SavRq_ReadAllHeaders:
		xbINO_gi_IsWorking = 1;
		xbINO_gi_ConfirmOnSuccess = 0;
		
		/* VL : clear header */
		for(i = 0; i < INO_Cte_SavSlotNbMax; i++)
		{			L_memset( &INO_gst_SavManager.ast_SlotDesc[i],0 , INO_Cte_SavHeaderSize );

		result = xbINO_i_ReadFile(INO_gst_SavManager.p_Temp, INO_gst_SavManager.i_TempSize, NULL,i);
		if(result==-2)
		{
		//data corrupt

			//xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_ErrorWhileLoading;//INO_e_SavMsg_BadFileOnMc;
			SetLangForDamagedGame(INO_gst_SavManager.ast_SlotDesc[i].asz_Name);
			INO_gst_SavManager.ast_SlotDesc[i].i_Slot = i;
			INO_gst_SavManager.ast_SlotDesc[i].i_Map=101;
			INO_gst_SavManager.ast_SlotDesc[i].ul_GameTime=1;
			INO_gst_SavManager.ast_SlotDesc[i].i_WP=1;
			INO_gst_SavManager.ast_SlotDesc[i].i_Time=1;
			//L_memset(INO_gst_SavManager.p_Temp, 0, INO_gst_SavManager.i_TempSize);
			//xbINO_gi_EmulLatency = xbINO_Cte_SavLatency;
			//xbINO_ge_SavMsg = INO_e_SavMsg_BadFileOnMc;
			//xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_SuccessWhileLoading;
			//break;
		
		}
		else if(result)
		{
			xbINO_gi_IsWorking = 0;//set to OFF: no saving... create ??..
			xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_NoSaving;//INO_e_SavMsg_None;
			//xbINO_ge_SavMsg = INO_e_SavMsg_NoSaving;
			//break;
		//xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_SuccessWhileLoading;
		}
		else
		{
			//for(i = 0; i < INO_Cte_SavSlotNbMax; i++)
			{
				int cucuslot=0;//INO_gst_SavManager.p_Temp[44];//INO_gst_SavManager.p_Temp[0+sizeof(INO_gst_SavManager.ast_SlotDesc->asz_Name)+sizeof(INO_gst_SavManager.ast_SlotDesc->ul_GameTime)];//44
				
				L_memcpy
				(
					
					&INO_gst_SavManager.ast_SlotDesc[i],
					INO_gst_SavManager.p_Temp + (cucuslot) * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize),
					INO_Cte_SavHeaderSize
				);
				INO_gst_SavManager.ast_SlotDesc[i].i_Slot = i;
			}

			xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_SuccessWhileLoading;
		}

		xbINO_gi_EmulLatency = 0;//xbINO_Cte_SavLatency;
		xbINO_ge_SavMsg = INO_e_SavMsg_BeforeAccessing;
}
		break;

	case INO_e_SavRq_FreeRessource:
	break;
	case INO_e_SavRq_ReadOneProfile:
		xbINO_gi_IsWorking = 1;
		xbINO_gi_ConfirmOnSuccess = 1;
		/* read all profiles */
		result = xbINO_i_ReadFile(INO_gst_SavManager.p_Temp, INO_gst_SavManager.i_TempSize, NULL,val);
		if (result==-2)
		{
			
			
			xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_BadFileOnMc;//INO_e_SavMsg_ErrorWhileLoading;
			//xbINO_gi_IsWorking =0;
			//xbINO_ge_SavMsg = INO_e_SavMsg_BadFileOnMc;
			//L_memset(INO_gst_SavManager.p_Temp, 0, INO_gst_SavManager.i_TempSize);
			xbINO_gi_EmulLatency = xbINO_Cte_SavLatency;
			xbINO_ge_SavMsg = INO_e_SavMsg_BadFileOnMc;//INO_e_SavMsg_ErrorWhileLoading;//INO_e_SavMsg_BadFileOnMc;

			break;
		}
		else if(result)
		{
			xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_ErrorWhileLoading;
		}

		else
		{
			/* copy the header */
			L_memcpy
			(
				&INO_gst_SavManager.ast_SlotDesc[val],
				INO_gst_SavManager.p_Temp + 0 * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize),
				INO_Cte_SavHeaderSize
			);

			/* the data */
			L_memcpy
			(
				INO_gst_SavManager.ac_CurrSlotBuffer,
				INO_gst_SavManager.p_Temp +
					0* //*val
					(INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize) +
					INO_Cte_SavHeaderSize,
				INO_Cte_SavOneSlotMaxSize
			);
			INO_gst_SavManager.ast_SlotDesc[val].i_Slot = val;

			// -- Specific XBOX Language Saving --Pas de save c'est tout
			{
			/*	INO_tden_LanguageId language = INO_gst_SavManager.ast_SlotDesc[val].i_Language;
				(INO_tden_LanguageId)TEXT_gst_Global.i_CurrentLanguage = language;*/
			}
			// -----------------------------------

			/* ok */
			xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_SuccessWhileLoading;
		}

		xbINO_gi_EmulLatency = xbINO_Cte_SavLatency;
		xbINO_ge_SavMsg = INO_e_SavMsg_Accessing;
		break;


		break;
	case INO_e_SavRq_WriteOneProfile:
		xbINO_gi_IsWorking = 1;
		xbINO_gi_ConfirmOnSuccess = 1;
		
		result = xbINO_i_ReadFile(INO_gst_SavManager.p_Temp, INO_gst_SavManager.i_TempSize, NULL,val);
		if(result==-2)
		{
            // corrupt save = meme nom
            L_memset(INO_gst_SavManager.p_Temp, 0, INO_gst_SavManager.i_TempSize);
			/*xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_BadFileOnMc;//INO_e_SavMsg_ErrorWhileLoading;
			xbINO_gi_IsWorking =0;
			xbINO_gi_EmulLatency = 0;//xbINO_Cte_SavLatency;
			xbINO_ge_SavMsg = INO_e_SavMsg_BadFileOnMc;//INO_e_SavMsg_ErrorWhileLoading;//INO_e_SavMsg_BadFileOnMc;
			break;*/

		}
		else if (result==-3 )
		{
			if ( xbINO_gi_ConfirmOverWrite ==0 )
			{
				xbINO_gi_ConfirmOverWrite =1;
				xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_ConfirmOverwriting;
				xbINO_ge_SavMsg = INO_e_SavMsg_ConfirmOverwriting;//INO_e_SavMsg_Accessing;
				break;
			}
			/*else
			{
			
			
			}*/
			//meme nom
			/*	L_memset(INO_gst_SavManager.p_Temp, 0, INO_gst_SavManager.i_TempSize);
				xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_ErrorWhileSaving;
				xbINO_ge_SavMsg = INO_e_SavMsg_ErrorWhileSaving;
				
			for(i = 0; i < INO_Cte_SavSlotNbMax; i++)
			{
				L_memset( &INO_gst_SavManager.ast_SlotDesc[i],0 , INO_Cte_SavHeaderSize );

				result = xbINO_i_ReadFile(INO_gst_SavManager.p_Temp, INO_gst_SavManager.i_TempSize, NULL,i);

				if(!result)
				{
					int cucuslot=0;//INO_gst_SavManager.p_Temp[44];//INO_gst_SavManager.p_Temp[0+sizeof(INO_gst_SavManager.ast_SlotDesc->asz_Name)+sizeof(INO_gst_SavManager.ast_SlotDesc->ul_GameTime)];//44
				
					L_memcpy
					(
						
						&INO_gst_SavManager.ast_SlotDesc[i],
						INO_gst_SavManager.p_Temp + (cucuslot) * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize),
						INO_Cte_SavHeaderSize
					);
					INO_gst_SavManager.ast_SlotDesc[i].i_Slot = i;
				}
				else if(result==-2)
				{
					sprintf(INO_gst_SavManager.ast_SlotDesc[i].asz_Name,"..????");
					INO_gst_SavManager.ast_SlotDesc[i].i_Slot = i;
					INO_gst_SavManager.ast_SlotDesc[i].i_Map=101;
					INO_gst_SavManager.ast_SlotDesc[i].ul_GameTime=1;
					INO_gst_SavManager.ast_SlotDesc[i].i_WP=1;
					INO_gst_SavManager.ast_SlotDesc[i].i_Time=1;
				}
			}*/
			/*xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_BadFileOnMc;//INO_e_SavMsg_ErrorWhileLoading;
			xbINO_gi_IsWorking =0;
			xbINO_gi_EmulLatency = 0;//xbINO_Cte_SavLatency;
			xbINO_ge_SavMsg = INO_e_SavMsg_BadFileOnMc;//INO_e_SavMsg_ErrorWhileLoading;//INO_e_SavMsg_BadFileOnMc;
				*/
				
				//break;
		}
		else if (result)
		{
            // no existing save
            L_memset(INO_gst_SavManager.p_Temp, 0, INO_gst_SavManager.i_TempSize);
		}
	
		else if (xbINO_gi_ConfirmOverWrite ==0 && INO_gst_SavManager.ast_SlotDesc[val].i_Map!=0)//comfirm overwrite
		{
				//xbINO_gi_EmulLatency = xbINO_Cte_SavLatency;
				xbINO_gi_ConfirmOverWrite =1;
				xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_ConfirmOverwriting;
				xbINO_ge_SavMsg = INO_e_SavMsg_ConfirmOverwriting;//INO_e_SavMsg_Accessing;

			break;
		}
		
		xbINO_gi_ConfirmOverWrite =0;
		/* copy the header */
			L_memcpy
			(
				//val
				INO_gst_SavManager.p_Temp + 0 * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize),
				&INO_gst_SavManager.ast_SlotDesc[val],
				INO_Cte_SavHeaderSize
			);

			/* copy data */
			L_memcpy
			(
				INO_gst_SavManager.p_Temp +
					0* //*val
					(INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize) +
					INO_Cte_SavHeaderSize,
				INO_gst_SavManager.ac_CurrSlotBuffer,
				INO_Cte_SavOneSlotMaxSize
			);

			if(result && INO_gst_SavManager.ast_SlotDesc[val].i_Map!=0)//pas de test pour deleter
			{
				result=xbINO_i_TestFreeSpace(INO_gst_SavManager.i_TempSize);
				if (result)
				{
					//L_memset(INO_gst_SavManager.p_Temp, 0, INO_gst_SavManager.i_TempSize);
					L_memset( &INO_gst_SavManager.ast_SlotDesc[val],0 , INO_Cte_SavHeaderSize );

				/*	L_memcpy
				(
					INO_gst_SavManager.p_Temp + val * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize),
					&INO_gst_SavManager.ast_SlotDesc[val],
					INO_Cte_SavHeaderSize
				);*/


					xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_NoMoreSpace;
//							xbINO_gi_EmulLatency = xbINO_Cte_SavLatency;
					xbINO_ge_SavMsg = INO_e_SavMsg_NoMoreSpace;

					break;
					//xbINO_ge_SavMsg = INO_e_SavMsg_NoMoreSpace;
				}
			}

			// ====================== Deleting Files ======================
			if ( INO_gst_SavManager.ast_SlotDesc[val].i_Map==0 )
			{
				XGAME_FIND_DATA xgfd;
				HANDLE hFind;
				char	RootPathName[4] = "U:\\";
				int counter;
				WCHAR	GameName[32];

				hFind = XFindFirstSaveGame( RootPathName, &xgfd );

				//if (val>0)
		/*		for( counter=0;counter<5;counter++)
				{
					

					swprintf(GameName,L"%S",INO_gst_SavManager.ast_SlotDesc[val].asz_Name);
					
					if ( xgfd.szSaveGameName==GameName )
					{
	
						
						XDeleteSaveGame(RootPathName,xgfd.szSaveGameName);
						xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_SuccessWhileSaving;
				
						xbINO_gi_EmulLatency = xbINO_Cte_SavLatency;
						xbINO_ge_SavMsg = INO_e_SavMsg_Saving;//INO_e_SavMsg_Accessing;
						break;
					}
				if (!XFindNextSaveGame( hFind, &xgfd )) return -1;


				}
				return -1;//n'a pas fontionné*/

				if (val>0)
				for( counter=0;counter<val;counter++)
				{
					if (!XFindNextSaveGame( hFind, &xgfd )) return -1;
				}
			
				XDeleteSaveGame(RootPathName,xgfd.szSaveGameName);

// ------------

		/* VL : clear header */
		for(i = 0; i < INO_Cte_SavSlotNbMax; i++)
		{
			L_memset( &INO_gst_SavManager.ast_SlotDesc[i],0 , INO_Cte_SavHeaderSize );

			result = xbINO_i_ReadFile(INO_gst_SavManager.p_Temp, INO_gst_SavManager.i_TempSize, NULL,i);

			if(!result)
			{
				int cucuslot=0;//INO_gst_SavManager.p_Temp[44];//INO_gst_SavManager.p_Temp[0+sizeof(INO_gst_SavManager.ast_SlotDesc->asz_Name)+sizeof(INO_gst_SavManager.ast_SlotDesc->ul_GameTime)];//44
			
				L_memcpy
				(
					
					&INO_gst_SavManager.ast_SlotDesc[i],
					INO_gst_SavManager.p_Temp + (cucuslot) * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize),
					INO_Cte_SavHeaderSize
				);
				INO_gst_SavManager.ast_SlotDesc[i].i_Slot = i;
			}
			else if(result==-2)
			{
				SetLangForDamagedGame(INO_gst_SavManager.ast_SlotDesc[i].asz_Name);
				INO_gst_SavManager.ast_SlotDesc[i].i_Slot = i;
				INO_gst_SavManager.ast_SlotDesc[i].i_Map=101;
				INO_gst_SavManager.ast_SlotDesc[i].ul_GameTime=1;
				INO_gst_SavManager.ast_SlotDesc[i].i_WP=1;
				INO_gst_SavManager.ast_SlotDesc[i].i_Time=1;
			}
		}

// ------------
				xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_SuccessWhileSaving;
				
				xbINO_gi_EmulLatency = xbINO_Cte_SavLatency;
				xbINO_ge_SavMsg = INO_e_SavMsg_Saving;//INO_e_SavMsg_Accessing;
				break;


			}
			// =============================================================

			
			result = xbINO_i_WriteFile(INO_gst_SavManager.p_Temp, INO_gst_SavManager.i_TempSize, NULL,val);
			if(result)
			{
				xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_ErrorWhileSaving;
			}
			else
			{
				/* ok */
				xbINO_ge_SavEmulLatencyMsg = INO_e_SavMsg_SuccessWhileSaving;
			}

		xbINO_gi_EmulLatency = 150;//TRC 3 secondes xbINO_Cte_SavLatency;
		xbINO_ge_SavMsg = INO_e_SavMsg_Saving;//INO_e_SavMsg_Accessing;
		break;

	case INO_e_SavRq_Status:
		return xbINO_gi_IsWorking; /* work is finished */

	case INO_e_SavRq_CardId:
		if((val == 0) || (val == 1))
			xbINO_gi_CardId = val;
		else
			xbINO_gi_CardId = -1;
		return xbINO_gi_CardId;

	case INO_e_SavRq_UserMessageId:
		{
			//static int popo=1;	
			return xbINO_ge_SavMsg;
		}
	case INO_e_SavRq_Space:
		return (1024 * 1024);

	case INO_e_SavRq_TestMemCard://Test inutile car pour presence de memory card
		xbINO_ge_SavMsg = INO_e_SavMsg_None;
		xbINO_gi_IsWorking = 0;
		break;

    default:
		return -10;
	} 
	return 0;
}

#if 0
extern int	INO_ge_SavErrorId;

const DWORD BLOCK_SIZE = 16 * 1024; /* TCR C4-12 Space Display */

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbINO_SavModuleInit(void)
{

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbINO_SavModuleClose(void)
{
	if(INO_gst_SavFileDesc.x_File) L_fclose(INO_gst_SavFileDesc.x_File);
	INO_gst_SavFileDesc.x_File = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL xbINO_b_AsyncInProgress(int _i_Slot)
{
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_i_TestPresence(int _i_Slot)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	static	generateerror = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(generateerror)
	{
		INO_ge_SavErrorId = INO_SavErr_NoMemoryCard;
		INO_M_SavBtn_Assign(INO_Btn_Cross, INO_Id_Retry, Sav_e_TestPresence);
		INO_M_SavBtn_Assign(INO_Btn_Square, INO_Id_Cancel, Sav_e_CancelAll);
		INO_M_SavBtn_Assign(INO_Btn_Circle, INO_Id_None, Sav_e_None);
		INO_M_SavBtn_Assign(INO_Btn_Triangle, INO_Id_None, Sav_e_None);
		return(INO_ge_SavState = Sav_e_WaitingForUser);
	}

	INO_ge_SavErrorId = Sav_e_None;
	return Sav_e_MountCard;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_i_MountCard(int _i_Slot)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	static	generateerror = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(generateerror)
	{
		if(generateerror == 1)
			INO_ge_SavErrorId = INO_SavErr_WrongDevice;
		else
			INO_ge_SavErrorId = INO_SavErr_Encoding;
		INO_M_SavBtn_Assign(INO_Btn_Cross, INO_Id_Retry, Sav_e_TestPresence);
		INO_M_SavBtn_Assign(INO_Btn_Square, INO_Id_Cancel, Sav_e_CancelAll);
		INO_M_SavBtn_Assign(INO_Btn_Circle, INO_Id_Format, Sav_e_FormatCard);
		INO_M_SavBtn_Assign(INO_Btn_Triangle, INO_Id_None, Sav_e_None);
		return(INO_ge_SavState = Sav_e_WaitingForUser);
	}

	return Sav_e_CheckCard;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_i_CheckCard(int _i_Slot)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	static	generateerror = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(generateerror)
	{
		INO_ge_SavErrorId = INO_SavErr_CheckFailed;
		INO_M_SavBtn_Assign(INO_Btn_Cross, INO_Id_Retry, Sav_e_TestPresence);
		INO_M_SavBtn_Assign(INO_Btn_Square, INO_Id_Cancel, Sav_e_CancelAll);
		INO_M_SavBtn_Assign(INO_Btn_Circle, INO_Id_None, Sav_e_None);
		INO_M_SavBtn_Assign(INO_Btn_Triangle, INO_Id_None, Sav_e_None);
		return(INO_ge_SavState = Sav_e_WaitingForUser);
	}

	return Sav_e_TestFreeSpace;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_i_UnmountCard(int _i_Slot)
{
	return Sav_e_None;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_i_FormatCard(int _i_Slot)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	static	generateerror = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(generateerror)
	{
		INO_ge_SavErrorId = INO_SavErr_FormatFailed;
		INO_M_SavBtn_Assign(INO_Btn_Cross, INO_Id_Retry, Sav_e_TestPresence);
		INO_M_SavBtn_Assign(INO_Btn_Square, INO_Id_Cancel, Sav_e_CancelAll);
		INO_M_SavBtn_Assign(INO_Btn_Circle, INO_Id_Format, Sav_e_FormatCard);
		INO_M_SavBtn_Assign(INO_Btn_Triangle, INO_Id_None, Sav_e_None);
		return(INO_ge_SavState = Sav_e_WaitingForUser);
	}

	INO_ge_SavErrorId = INO_SavErr_FormatSuccessFull;
	INO_M_SavBtn_Assign(INO_Btn_Cross, INO_Id_Continue, Sav_e_TestPresence);
	INO_M_SavBtn_Assign(INO_Btn_Square, INO_Id_None, Sav_e_None);
	INO_M_SavBtn_Assign(INO_Btn_Circle, INO_Id_None, Sav_e_None);
	INO_M_SavBtn_Assign(INO_Btn_Triangle, INO_Id_None, Sav_e_None);
	return Sav_e_WaitingForUser;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_i_Retry(int _i_Slot)
{
	return INO_ge_SavCommand;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_i_Continue(int _i_Slot)
{
	return Sav_e_None;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_i_ActiveConsole(int _i_Slot)
{
	return Sav_e_None;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_i_Open(int _i_Slot)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int		next;
	WCHAR	GameName[16];
	char	RootPathName[4] = "U:\\";
	char	PathBuffer[256];
	DWORD	dwSuccess;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	lstrcpyW(GameName, L"Kong saved game");
	dwSuccess = XCreateSaveGame(RootPathName, GameName, OPEN_ALWAYS, 0, PathBuffer, 256);

	strcat(PathBuffer, INO_gsz_SavFileName);

	INO_gst_SavFileDesc.x_File = CreateFile
		(
			PathBuffer,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_ALWAYS,
			0,
			NULL
		);
	
	if(INO_gst_SavFileDesc.x_File == INVALID_HANDLE_VALUE)
	{
		if(INO_ge_SavCommand == Sav_e_WriteSlot_A)
		{
			if(INO_gi_FreeSpace < SAV_OneSlotMaxSize)
			{
				INO_ge_SavErrorId = INO_SavErr_NoSufficientSpace;
				INO_M_SavBtn_Assign(INO_Btn_Cross, INO_Id_Retry, Sav_e_TestPresence);
				INO_M_SavBtn_Assign(INO_Btn_Square, INO_Id_Cancel, Sav_e_CancelAll);
				INO_M_SavBtn_Assign(INO_Btn_Circle, INO_Id_None, Sav_e_None);
				INO_M_SavBtn_Assign(INO_Btn_Triangle, INO_Id_None, Sav_e_None);
				INO_ge_SavSetNextState = INO_ge_SavCommand;
				return Sav_e_WaitingForUser;
			}
		}
		else if(INO_ge_SavCommand == Sav_e_WriteIndex_0)
		{
			if(INO_gi_FreeSpace < SAV_OneIdxMaxSize)
			{
				INO_ge_SavErrorId = INO_SavErr_NoSufficientSpace;
				INO_M_SavBtn_Assign(INO_Btn_Cross, INO_Id_Retry, Sav_e_TestPresence);
				INO_M_SavBtn_Assign(INO_Btn_Square, INO_Id_Cancel, Sav_e_CancelAll);
				INO_M_SavBtn_Assign(INO_Btn_Circle, INO_Id_None, Sav_e_None);
				INO_M_SavBtn_Assign(INO_Btn_Triangle, INO_Id_None, Sav_e_None);
				INO_ge_SavSetNextState = INO_ge_SavCommand;
				return Sav_e_WaitingForUser;
			}
		}

		INO_gst_SavFileDesc.x_File = CreateFile
			(
				PathBuffer,
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_ALWAYS,
				0,
				NULL
			);
		if(INO_gst_SavFileDesc.x_File == INVALID_HANDLE_VALUE)
		{
			if((INO_ge_SavCommand == Sav_e_WriteIndex_0) || (INO_ge_SavCommand == Sav_e_WriteSlot_A))
				INO_ge_SavErrorId = INO_SavErr_SaveFailed;
			else
				INO_ge_SavErrorId = INO_SavErr_LoadFailed;
			INO_M_SavBtn_Assign(INO_Btn_Cross, INO_Id_Retry, Sav_e_TestPresence);
			INO_M_SavBtn_Assign(INO_Btn_Square, INO_Id_Cancel, Sav_e_CancelAll);
			INO_M_SavBtn_Assign(INO_Btn_Circle, INO_Id_None, Sav_e_None);
			INO_M_SavBtn_Assign(INO_Btn_Triangle, INO_Id_None, Sav_e_None);
			INO_ge_SavSetNextState = INO_ge_SavCommand;
			return Sav_e_WaitingForUser;
		}
	}

	next = (INO_ge_SavSetNextState != -1) ? INO_ge_SavSetNextState : Sav_e_None;
	INO_ge_SavSetNextState = -1;

	return next;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_i_Close(int _i_Slot)
{
	/*~~~~~*/
	int next;
	/*~~~~~*/

	if(INO_gst_SavFileDesc.x_File) L_fclose(INO_gst_SavFileDesc.x_File);
	INO_gst_SavFileDesc.x_File = NULL;

	next = (INO_ge_SavSetNextState != -1) ? INO_ge_SavSetNextState : Sav_e_None;
	INO_ge_SavSetNextState = -1;

	return next;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_i_ReadIndex_1(int _i_Slot)
{
	if(INO_gst_SavFileDesc.x_File)
	{
		L_fread(INO_gst_SavFileDesc.p_Buffer, 1, INO_gst_SavFileDesc.i_Size, INO_gst_SavFileDesc.x_File);
	}

	return Sav_e_ReadIndex_2;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_i_WriteIndex(int _i_Slot)
{
	if(INO_gst_SavFileDesc.x_File)
	{
		L_fwrite(INO_gst_SavFileDesc.p_Buffer, 1, INO_gst_SavFileDesc.i_Size, INO_gst_SavFileDesc.x_File);
	}

	return Sav_e_Close;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_i_ReadSlot_1(int _i_Slot)
{
	if(INO_gst_SavFileDesc.x_File)
	{
		L_fread(INO_gst_SavFileDesc.p_Buffer, 1, INO_gst_SavFileDesc.i_Size, INO_gst_SavFileDesc.x_File);
	}

	return Sav_e_ReadSlot_2;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_i_ReadSlot_2(int _i_Slot)
{
	if(INO_gst_SavFileDesc.x_File)
	{
		L_fread(INO_gst_SavFileDesc.p_Buffer, 1, INO_gst_SavFileDesc.i_Size, INO_gst_SavFileDesc.x_File);
	}

	return Sav_e_ReadSlot_3;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_i_ReadSlot_3(int _i_Slot)
{
	if(INO_gst_SavFileDesc.x_File)
	{
		L_fread(INO_gst_SavFileDesc.p_Buffer, 1, INO_gst_SavFileDesc.i_Size, INO_gst_SavFileDesc.x_File);
	}

	return Sav_e_Close;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_i_WriteSlot_C(int _i_Slot)
{
	if(INO_gst_SavFileDesc.x_File)
	{
		L_fwrite(INO_gst_SavFileDesc.p_Buffer, 1, INO_gst_SavFileDesc.i_Size, INO_gst_SavFileDesc.x_File);
	}

	return Sav_e_WriteSlot_D;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_i_WriteSlot_E(int _i_Slot)
{
	if(INO_gst_SavFileDesc.x_File)
	{
		L_fwrite(INO_gst_SavFileDesc.p_Buffer, 1, INO_gst_SavFileDesc.i_Size, INO_gst_SavFileDesc.x_File);
	}

	return Sav_e_WriteSlot_F;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_i_WriteSlot_F(int _i_Slot)
{
	if(INO_gst_SavFileDesc.x_File)
	{
		L_fwrite(INO_gst_SavFileDesc.p_Buffer, 1, INO_gst_SavFileDesc.i_Size, INO_gst_SavFileDesc.x_File);
	}

	return Sav_e_Close;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_i_FileGetSize(char *_sz_Name)
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

#endif
