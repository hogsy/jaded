/*$T GC_File.c GC 1.138 10/30/03 14:06:26 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "GameCube/GC_File.h"
#include "GameCube/GC_Stream.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "INOut/INOfile.h"
#include "SDK/Sources/GameCube/GC_arammng.h"
#include "SDK/Sources/GameCube/GC_aramheap.h"
#include "GXI_GC/GXI_init.h"

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

BOOL	IsMediaError(void);
void	CheckMediaError(void);
void	SafeCheckMediaError(void);

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

#ifdef _DEBUG
unsigned int	GC_gui_StreamAllocSize = 0;
unsigned int	GC_gui_StreamMaxAllocSize = 0;
#endif
unsigned int	GC_gui_LastErrorCode;

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 
//OSMutex GC_DVDGetDriveStatusMutex;
s32 GC_SafeGetDriveStatus()
{
	s32 result;
//	OSLockMutex(&GC_DVDGetDriveStatusMutex);
	result = DVDGetDriveStatus();
//	OSUnlockMutex(&GC_DVDGetDriveStatusMutex);
	return result;
}
 
 
BOOL IsMediaError(void)
{
	s32 State = GC_SafeGetDriveStatus();
	
	switch(State)
	{
	case DVD_STATE_BUSY:
	case DVD_STATE_PAUSING:
	case DVD_STATE_MOTOR_STOPPED:
	case DVD_STATE_END:
		return FALSE;

	default:
		return TRUE;
	}
}

extern void SND_TrackPauseAll(BOOL b_Stop);
extern int	TEXT_gi_ChangedLang;
extern int	INO_i_PifPafEnable(int _i_Enable);

BOOL GC_bMediaError = FALSE;

/*
 =======================================================================================================================
 =======================================================================================================================
 */

#ifndef NO_BINK
extern OSThread* g_pBinkThread;
#endif // NO_BINK

void CheckMediaError(void)
{
#ifndef NO_BINK
	if (g_pBinkThread && !OSIsThreadTerminated(g_pBinkThread))
	{
		while (IsMediaError())
		{
			if (OSIsThreadSuspended(g_pBinkThread))
				OSResumeThread(g_pBinkThread);		
		}
	}
	else
#endif // NO_BINK
		SafeCheckMediaError();
}


void SafeCheckMediaError(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				error_msg[3][64];
	int					b_display = 0;
	extern void			GXI_ErrBegin(void);
	extern void			GXI_ErrEnd(void);
	extern void			GXI_ErrPrint(char *str);
	extern void			GXI_BeforeDisplay(void);
	extern int			WOR_gi_IsLoading;
	extern BOOL 		SND_gb_PauseAll;
	BOOL 				bIsAudioMuteForFile = FALSE;
	BOOL				Error;
	int					Lang;
	s32					error;

	ULONG				ul_Size = 0;
	BOOL				FrameBufferInARAM = FALSE;
	extern char			*g_pstFrameBuffer1;
	extern tdstARAMHeap *g_pARAMHeaps[eARAM_NumberOfARAMBlocks];
	extern int			TEXT_i_GetLang(void);
	int					i_PifPaf;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Error = FALSE;
	FrameBufferInARAM = FALSE;

	Lang = (TEXT_gi_ChangedLang > 1) ? TEXT_i_GetLang() : AI_EvalFunc_IoConsLangGet_C();
	i_PifPaf = INO_i_PifPafEnable(-1);

	do
	{
		error = GC_SafeGetDriveStatus();
		switch(error)
		{
		case DVD_STATE_PAUSING:
		case DVD_STATE_END:
		case DVD_STATE_BUSY:
		case DVD_STATE_MOTOR_STOPPED:
			if(b_display)
			{
				// Si on est en busy, on a pas eu le temps de vérifier si l'erreur a changée
				if (error != DVD_STATE_BUSY)
				{
					b_display = 0;
					GXI_vUseTwoBuffers();
					GXI_ErrBegin();
					GXI_ErrEnd();
					GXI_ErrBegin();
					GXI_ErrEnd();
				}
			}
			else
				GC_bMediaError = FALSE;
				
			if (!b_display)
				INO_gi_DiscError = INO_Cte_DiscErrorNone;

			break;
			

		case DVD_STATE_WRONG_DISK:
		case DVD_STATE_NO_DISK:
			switch(Lang)
			{
			case 0: /* France */
				sprintf(error_msg[0], "Veuillez insérer le Disque King Kong.");
				error_msg[1][0] = 0;
				error_msg[2][0] = 0;
				break;

			case 1: /* US */
				sprintf(error_msg[0], "Please insert the King Kong Game Disc.");
				error_msg[1][0] = 0;
				error_msg[2][0] = 0;
				break;
				
			case 2: /* Danish */
				sprintf(error_msg[0], "Indsæt venligst King Kong-spildisken.");
				error_msg[1][0] = 0;
				error_msg[2][0] = 0;
				break;
				
			case 3: /* Dutch (Netherlands) */
				sprintf(error_msg[0], "Plaats de spel-Disc met King Kong.");
				error_msg[1][0] = 0;
				error_msg[2][0] = 0;
				break;
				
			case 4: /* Finnish */
				sprintf(error_msg[0], "Aseta King Kong -pelilevy levyasemaan.");
				error_msg[1][0] = 0;
				error_msg[2][0] = 0;
				break;
				
			case 5: /* Germany */
				sprintf(error_msg[0], "Bitte legen Sie die King Kong-Disc ein.");
				error_msg[1][0] = 0;
				error_msg[2][0] = 0;
				break;

			case 6: /* Italy */
				sprintf(error_msg[0], "Inserisci il Disco di gioco King Kong.");
				error_msg[1][0] = 0;
				error_msg[2][0] = 0;
				break;
				
			case 7: /* Spain */
				sprintf(error_msg[0], "Coloca el Disco de King Kong.");
				error_msg[1][0] = 0;
				error_msg[2][0] = 0;
				break;

			case 9: /* Swedish */
				sprintf(error_msg[0], "Var vänlig stoppa i cd-skivan till");
				L_strcpy(error_msg[1], "King Kong.");
				error_msg[2][0] = 0;
				break;
				
			case 20: /* Norwegian */
				sprintf(error_msg[0], "Sett inn King Kong-platen.");
				error_msg[1][0] = 0;
				error_msg[2][0] = 0;
				break;
			}

			b_display++;
			if (error == DVD_STATE_WRONG_DISK)
				INO_gi_DiscError = INO_Cte_DiscErrorWrongDisc;
			else
				INO_gi_DiscError = INO_Cte_DiscErrorNoDisc;
			break;

		case DVD_STATE_COVER_OPEN:
			switch(Lang)
			{
			case 0: /* France */
				sprintf(error_msg[0], "Le Couvercle est ouvert. Pour continuer à jouer,");
				L_strcpy(error_msg[1], "veuillez fermer le Couvercle.");
				error_msg[2][0] = 0;
				break;

			case 1: /* US */
				L_strcpy(error_msg[0], "The Disc Cover is open. If you want to continue");
				L_strcpy(error_msg[1], "the game, please close the Disc Cover.");
				error_msg[2][0] = 0;
				break;

			case 2: /* Danish */
				sprintf(error_msg[0], "Disc-dæksel er åbent. Hvis du vil fortsætte");
				L_strcpy(error_msg[1], "spillet, skal du lukke for Disc-dæksel.");
				error_msg[2][0] = 0;
				break;
				
			case 3: /* Dutch (Netherlands) */
				sprintf(error_msg[0], "De Disc-deksel is open. Sluit de Disc-deksel als");
				L_strcpy(error_msg[1], "je verder wilt spelen.");
				error_msg[2][0] = 0;
				break;
				
			case 4: /* Finnish */
				sprintf(error_msg[0], "Levykkeen Suoja on auki. Jos haluat jatkaa");
				L_strcpy(error_msg[1], "peliä, sulje Levykkeen Suoja.");
				error_msg[2][0] = 0;
				break;
				
			case 5: /* Germany */
				sprintf(error_msg[0], "Der Disc-Deckel ist geöffnet. Bitte den Disc-Deckel");
				L_strcpy(error_msg[1], "schließen, um mit dem Spiel fortzufahren.");
				error_msg[2][0] = 0;
				break;

			case 6: /* Italy */
				sprintf(error_msg[0], "Il coperchio del Disco è aperto. Se vuoi proseguire");
				L_strcpy(error_msg[1], "nel gioco, chiudi il coperchio del Disco.");
				error_msg[2][0] = 0;
				break;
			
			case 7: /* Spain */
				sprintf(error_msg[0], "La Tapa está abierta. Si quieres seguir jugando,");
				L_strcpy(error_msg[1], "debes cerrar la Tapa.");
				error_msg[2][0] = 0;
				break;

			case 9: /* Swedish */
				sprintf(error_msg[0], "Skivlucka är öppen. Var snäll och stäng Skivlucka,");
				L_strcpy(error_msg[1], "om du vill fortsätta spelet.");
				error_msg[2][0] = 0;
				break;
				
			case 20: /* Norwegian */
				sprintf(error_msg[0], "Disc deksel er åpent. Hvis du vil");
				L_strcpy(error_msg[1], "fortsette å spille, må du lukke dette.");
				error_msg[2][0] = 0;
				break;
			}

			b_display++;
			INO_gi_DiscError = INO_Cte_DiscErrorCoverIsOpened;
			break;


		case DVD_STATE_RETRY:
			switch(Lang)
			{
			case 0: /* France */
				sprintf(error_msg[0], "La lecture du Disque a échoué. Veuillez vous référer");
				L_strcpy(error_msg[1], "au manuel d'instructions NINTENDO GAMECUBE pour de");
				L_strcpy(error_msg[2], "plus amples informations.");
				break;

			case 1: /* US */
				sprintf(error_msg[0], "The Game Disc could not be read. Please read the");
				L_strcpy(error_msg[1], "Nintendo GameCube Instruction Booklet for further");
				L_strcpy(error_msg[2], "informations.");
				break;

			case 2: /* Danish */
				sprintf(error_msg[0], "Det var ikke muligt at læse spildisc'en.");
				L_strcpy(error_msg[1], "Læs venligst instruktionsbogen til");
				L_strcpy(error_msg[2], "din NINTENDO GAMECUBE for yderligere information.");
				break;

			case 3: /* Netherlands (Dutch) */
				sprintf(error_msg[0], "De spel-Disc kon niet worden gelezen. Lees de");
				L_strcpy(error_msg[1], "handleiding van de NINTENDO GAMECUBE voor meer");
				L_strcpy(error_msg[2], "informatie.");
				break;

			case 4: /* Finnish */
				sprintf(error_msg[0], "Pelilevyn lukeminen ei onnistunut.");
				L_strcpy(error_msg[1], "Katso lisäohjeita NINTENDO GAMECUBE ohjekirjasta.");
				error_msg[2][0] = 0;
				break;
				
			case 5: /* Germany */
				sprintf(error_msg[0], "Diese Game Disc kann nicht gelesen werden. Bitte lesen");
				L_strcpy(error_msg[1], "Sie die Bedienungsanleitung des Nintendo GameCube,");
				L_strcpy(error_msg[2], "um weitere Informationen zu erhalten.");
				break;

			case 6: /* Italy */
				sprintf(error_msg[0], "Impossibile leggere il Disco di gioco.");
				L_strcpy(error_msg[1], "Per ulteriori indicazioni consulta il manuale");
				L_strcpy(error_msg[2], "di istruzioni del Nintendo GameCube.");
				break;
				
			case 7: /* Spain */
				sprintf(error_msg[0], "No se puede leer el Disco. Consulta el manual de ");
				L_strcpy(error_msg[1], "instrucciones de NINTENDO GAMECUBE para obtener");
				L_strcpy(error_msg[2], "más información.");
				break;

			case 9: /* Swedish */
				sprintf(error_msg[0], "CD-skivan kunde inte läsas. Var vänlig");
				L_strcpy(error_msg[1], "läs instruktionshäftet till din Nintendo GameCube");
				L_strcpy(error_msg[2], "för mer information.");
				break;

			case 20: /* Norwegian */
				sprintf(error_msg[0], "Platen kunne ikke leses. Sjekk i instruksjonsboken");
				L_strcpy(error_msg[1], "til NINTENDO GAMECUBE for å få mer informasjon.");
				error_msg[2][0] = 0;
				break;

			}

			b_display++;
			INO_gi_DiscError = INO_Cte_DiscErrorRetry;
			break;

		default:


		case DVD_STATE_FATAL_ERROR:
			switch(Lang)
			{
			case 0: /* France */
				sprintf(error_msg[0], "Une erreur est survenue. Eteignez la console et");
				L_strcpy(error_msg[1], "référez-vous au manuel d'instructions");
				L_strcpy(error_msg[2], "NINTENDO GAMECUBE pour de plus amples informations.");
				break;

			case 1: /* US */
				sprintf(error_msg[0], "An error has occurred. Turn the power off and refer");
				L_strcpy(error_msg[1], "to the Nintendo GameCube Instruction Booklet");
				L_strcpy(error_msg[2], "for further informations.");
				break;

			case 2: /* Danish */
				sprintf(error_msg[0], "Der er opstået en fejl. Sluk for strømmen");
				L_strcpy(error_msg[1], "og se i instruktionsbogen til din NINTENDO GAMECUBE");
				L_strcpy(error_msg[2], "for yderligere instrukser.");
				break;
				
			case 3: /* Netherlands (Dutch) */
				sprintf(error_msg[0], "Er is een fout opgetreden. Zet de NINTENDO GAMECUBE");
				L_strcpy(error_msg[1], "uit en raadpleeg de handleiding van de");
				L_strcpy(error_msg[2], "Nintendo GameCube voor nadere instructies.");
				break;				

			case 4: /* Finnish */
				sprintf(error_msg[0], "Tietojen luvussa on tapahtunut virhe.");
				L_strcpy(error_msg[1], "Sammuta laite ja katso lisäohjeita NINTENDO GAMECUBE");
				L_strcpy(error_msg[2], "ohjekirjasta.");
				break;
				
			case 5: /* Germany */
				sprintf(error_msg[0], "Ein Fehler ist aufgetreten. Bitte schalten Sie den");
				L_strcpy(error_msg[1], "NINTENDO GAMECUBE aus und lesen Sie die Bedienungsanleitung,");
				L_strcpy(error_msg[2], "um weitere Informationen zu erhalten.");
				break;

			case 6: /* Italy */
				sprintf(error_msg[0], "Si è verificato un errore. Spegni (OFF) e consulta");
				L_strcpy(error_msg[1], "il manuale d'istruzioni del NINTENDO GAMECUBE");
				L_strcpy(error_msg[2], "per ulteriori indicazioni.");
				break;

			case 7: /* Spain */
				sprintf(error_msg[0], "Se ha producido un error. Apaga la consola y consulta");
				L_strcpy(error_msg[1], "el manual de instrucciones de NINTENDO GAMECUBE");
				L_strcpy(error_msg[2], "para obtener más información.");
				break;
				
			case 9: /* Swedish */
				sprintf(error_msg[0], "Ett fel har uppstått. Stäng av enheten och");
				L_strcpy(error_msg[1], "läs instruktionshäftet till din Nintendo GameCube");
				L_strcpy(error_msg[2], "för vidare instruktioner.");
				break;

			case 20: /* Norwegian */
				sprintf(error_msg[0], "Det skjedde en feil. Skru av strømmen");
				L_strcpy(error_msg[1], "og se i instruksjonsboken til NINTENDO GAMECUBE");
				L_strcpy(error_msg[2], "for å få mer informasjon.");
				break;
			}

			b_display++;
			INO_gi_DiscError = INO_Cte_DiscErrorFatal;
			break;
		}

		if(b_display >= 50) b_display = 1;

		/* if we have a dialog just show it. */
		if(b_display == 1)
		{
			extern u64 gcTIM_gu64_currentTime;
			
			GXI_vUseOneBuffer();
			
			Error = TRUE;
			GC_bMediaError = TRUE;
						
			gcTIM_gu64_currentTime = OSTicksToMicroseconds(OSGetTime());
			
			// If mute is not active (by bink or here), activate it.
			if(!SND_gb_PauseAll && !bIsAudioMuteForFile)
			{
				bIsAudioMuteForFile = TRUE;
				SND_MuteAll(TRUE);
				SND_TrackPauseAll(TRUE);
				SND_Update(NULL);
			}

			INO_i_PifPafEnable(FALSE);

			// Display error message.
			GXI_ErrBegin();
			GXI_ErrPrint(error_msg[0]);
			if(error_msg[1][0]) GXI_ErrPrint(error_msg[1]);
			if(error_msg[2][0]) GXI_ErrPrint(error_msg[2]);
			GXI_ErrEnd();
			
			GXI_ErrBegin();
			GXI_ErrPrint(error_msg[0]);
			if(error_msg[1][0]) GXI_ErrPrint(error_msg[1]);
			if(error_msg[2][0]) GXI_ErrPrint(error_msg[2]);
			GXI_ErrEnd();
		}

		// Enable reset if message is not "fatal error".
		if(INO_gi_DiscError != INO_Cte_DiscErrorFatal)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			extern void GC_s_CheckResetRequest(void);
			extern BOOL ENG_gb_ExitApplication;
			extern void GC_s_EngineCheat(void);
			extern void	INO_Joystick_Update(void);
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			
			if(b_display || WOR_gi_IsLoading)
			{
				INO_Joystick_Update();
				GC_s_EngineCheat();
			}
			GC_s_CheckResetRequest();
			if(ENG_gb_ExitApplication) break;
		}
	} while(b_display);

	// Set engine state back to normal.
	if(Error)
	{
		if (bIsAudioMuteForFile)
		{
			bIsAudioMuteForFile = FALSE;
			SND_MuteAll(FALSE);
			SND_TrackPauseAll(FALSE);
			SND_Update(NULL);
		}

		INO_i_PifPafEnable(i_PifPaf);
	}
	
}

/*
 =======================================================================================================================
    fast open file
 =======================================================================================================================
 */
tdstGC_File *GC_fOpen(u8 *_filename, u8 *_mode)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	tdstGC_File *fileHandle;
	s32			fileEntryNum;
	/*~~~~~~~~~~~~~~~~~~~~~*/

#pragma unused(_mode)
	fileEntryNum = DVDConvertPathToEntrynum(_filename);
	if(fileEntryNum == -1)
	{
#ifdef ENABLE_ERR_MSG
		GC_OutputDebugString("%s l%d File not found : %s\n", __FILE__, __LINE__, _filename);
#else
		OSReport("File not found");
#endif
		return NULL;
	}

	fileHandle = (tdstGC_File *) MEM_p_Alloc(sizeof(tdstGC_File));
	L_memset(fileHandle, 0, sizeof(tdstGC_File));

	if(!fileHandle)
	{
		OSReport("Could not allocate memory to open the file\n");
		return NULL;
	}

	if(!DVDFastOpen(fileEntryNum, &fileHandle->stFileInfo))
	{
		OSReport("Could not open the file\n");
		GC_fClose(fileHandle);
		return NULL;
	}

	fileHandle->pReadBuffer = (u8 *) MEM_p_AllocAlign(CDVD_BUFFER_SIZE, 32);

	if(!fileHandle->pReadBuffer)
	{
		OSReport("Could not allocate memory for the file read buffer\n");
		GC_fClose(fileHandle);
		return NULL;
	}

	fileHandle->iFileSize = DVDGetLength(&fileHandle->stFileInfo);

	fileHandle->u32ReadBufferStart = 0x0FFFFFFF;	/* faudrait max u32 DJ_TEMP !!! */
	fileHandle->u32ReadBufferEnd = 0;
	fileHandle->iFileEntryNum = fileEntryNum;

	return fileHandle;
}

/*
 =======================================================================================================================
    close file
 =======================================================================================================================
 */
u32 GC_fClose(tdstGC_File *_handle)
{
	/*~~~~*/
	u32 ret;
	/*~~~~*/

	ret = 1;

	ERR_X_Warning(_handle, "File Handle is NULL. Nothing to do...returning from GC_fClose...", NULL);

	if(!DVDClose(&_handle->stFileInfo))
	{
		OSReport("Close failed!!\n");
		ret = 0;
	}

	if(_handle->pReadBuffer) MEM_FreeAlign(_handle->pReadBuffer);

	MEM_Free(_handle);

	return ret;
};

/*
 =======================================================================================================================
    read at current pos in file
 =======================================================================================================================
 */
u32 GC_fRead(tdstGC_File *_handle, u8 *_buffer, u32 _size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	u8		*p_u8_Buffer;	/* buffer where we copy data in */
	u32		u32_BytesToRead;
	s32		s32BytesRead;
	s32		s32NumberOfBytesToRead;
	BOOL	bReadAsyncSucceds;
	u8		*ucReadBufferPos;
	u32		l_u32NumberOfBytesToRead;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	l_u32NumberOfBytesToRead = _size;

	ERR_X_Warning(_handle, "This File is closed.", NULL);
	ERR_X_Warning(_buffer, "Cannot read into a NULL buffer!!!", NULL);

	if(_size == 0)
	{
		OSReport("Requested 0 bytes to read. Returning immediatly.\n");
		return 0;
	}

	if(_handle->iCurrentFilePos >= _handle->iFileSize)
	{
		OSReport("EOF already reached. Cannot read more.\n");
		return 0;
	}

	p_u8_Buffer = _buffer;
	u32_BytesToRead = 0;
	s32BytesRead = 0;

	ucReadBufferPos = _handle->pReadBuffer + (_handle->iCurrentFilePos - _handle->u32ReadBufferStart);

	do
	{
		/* Check if the needed offset is in the buffers. */
		if
		(
			_handle->iCurrentFilePos < _handle->u32ReadBufferStart
		||	_handle->iCurrentFilePos >= _handle->u32ReadBufferEnd
		)
		{
			/* The data we need is not in the buffers */
			_handle->u32ReadBufferStart = OSRoundDown32B(_handle->iCurrentFilePos);
			_handle->u32ReadBufferEnd = _handle->u32ReadBufferStart + CDVD_BUFFER_SIZE;

			ucReadBufferPos = _handle->pReadBuffer + (_handle->iCurrentFilePos - _handle->u32ReadBufferStart);

			s32NumberOfBytesToRead = OSRoundUp32B(min((u32) CDVD_BUFFER_SIZE, _handle->iFileSize - _handle->u32ReadBufferStart));

			do
			{
				bReadAsyncSucceds = DVDReadAsync
					(
						&_handle->stFileInfo,
						_handle->pReadBuffer,
						s32NumberOfBytesToRead,
						_handle->u32ReadBufferStart,
						NULL
					);
				

				if(!bReadAsyncSucceds)
				{
					OSReport("\nFailed to read a file buffer");
					CheckMediaError();
				}

				
			} while(!bReadAsyncSucceds);

			while(DVDGetFileInfoStatus(&_handle->stFileInfo) != DVD_FILEINFO_READY)
			{
				CheckMediaError();
			}

			_handle->u32ReadBufferEnd = _handle->u32ReadBufferStart + s32NumberOfBytesToRead;
		}

		u32_BytesToRead = min(l_u32NumberOfBytesToRead, (_handle->u32ReadBufferEnd - _handle->iCurrentFilePos));

		L_memcpy(p_u8_Buffer, ucReadBufferPos, u32_BytesToRead);

		_handle->iCurrentFilePos += u32_BytesToRead;
		ucReadBufferPos += u32_BytesToRead;
		p_u8_Buffer += u32_BytesToRead;
		l_u32NumberOfBytesToRead -= u32_BytesToRead;

		if(_handle->iCurrentFilePos >= _handle->iFileSize)	/* We reached the end of the file */
			break;
	} while(l_u32NumberOfBytesToRead > 0);

	return _size;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
u32 GC_fSeek(tdstGC_File *_handle, u32 _pos, u8 _mode)
{
	ERR_X_Warning(_handle, "This File is closed.", NULL);

	switch(_mode)
	{
	case SEEK_SET:	_handle->iCurrentFilePos = _pos; break;
	case SEEK_CUR:	_handle->iCurrentFilePos += _pos; break;
	case SEEK_END:	_handle->iCurrentFilePos = _handle->iFileSize - _pos; break;
	default:		OSReport(0, "This origin is not defined."); return 1; break;
	}

	if(_handle->iCurrentFilePos >= _handle->iFileSize)
	{
		/* position at end of file */
		_handle->iCurrentFilePos = _handle->iFileSize;
	}
	else if(_handle->iCurrentFilePos < 0)
	{
		/* position at start of file */
		_handle->iCurrentFilePos = 0;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
u32 GC_fTell(tdstGC_File *_handle)
{
	ERR_X_Warning(_handle, "This File is closed.", NULL);

	return _handle->iCurrentFilePos;
}

/*$4
 ***********************************************************************************************************************
    streaming tools
 ***********************************************************************************************************************
 */

#ifdef _DEBUG
#define GC_M_Err(_fct, _str)	OSReport("["#_fct "] "_str "\n")
#else
#define GC_M_Err(_fct, _str)
#endif
void					GC_StreamCallBack(s32 result, DVDFileInfo *fileInfo);
GC_tdst_StreamHandler	GC_gax_StreamList[GC_Cte_StreamMaxNumber];

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GC_StreamInitModule(void)
{
	L_memset(GC_gax_StreamList, 0, GC_Cte_StreamMaxNumber * sizeof(GC_tdst_StreamHandler));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GC_StreamCloseModule(void)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < GC_Cte_StreamMaxNumber; i++)
	{
		if(GC_gax_StreamList[i].i_CurrentState != GC_Cte_StreamStateFree) GC_StreamClose(&GC_gax_StreamList[i]);
	}

	L_memset(GC_gax_StreamList, 0, GC_Cte_StreamMaxNumber * sizeof(GC_tdst_StreamHandler));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GC_tdst_StreamHandler *GC_pst_StreamOpen(tdstGC_File *_pst_SrcFileHandler, int _i_Priority)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GC_tdst_StreamHandler	*pst_StreamHandler;
	int						i;
	extern char				*gcSND_gp_BufferForLoading;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_SrcFileHandler)
	{
		GC_M_Err(GC_pst_StreamOpen, "Null file handler");
		return NULL;
	}

	if(_pst_SrcFileHandler->iFileEntryNum == -1)
	{
		GC_M_Err(GC_pst_StreamOpen, "Bad file entry number");
		return NULL;
	}

	pst_StreamHandler = GC_gax_StreamList;
	for(i = 0; i < GC_Cte_StreamMaxNumber; i++, pst_StreamHandler++)
	{
		if(pst_StreamHandler->i_CurrentState == GC_Cte_StreamStateFree) break;
	}

	if(i >= GC_Cte_StreamMaxNumber)
	{
		GC_M_Err(GC_pst_StreamOpen, "No more stream handler");
		return NULL;
	}

	L_memset(pst_StreamHandler, 0, sizeof(GC_tdst_StreamHandler));
	pst_StreamHandler->i_CurrentState = GC_Cte_StreamStateIdle;

	if(!DVDFastOpen(_pst_SrcFileHandler->iFileEntryNum, &pst_StreamHandler->st_FileInfo))
	{
		GC_M_Err(GC_pst_StreamOpen, "Could not open the file");
		GC_StreamClose(pst_StreamHandler);
		return NULL;
	}

	if(_i_Priority == 0x1002)
	{
		pst_StreamHandler->pc_FileBuffer = (u8 *) gcSND_gp_BufferForLoading;
	}
	else
	{
		pst_StreamHandler->pc_FileBuffer = (u8 *) MEM_p_AllocAlign(2 * CDVD_BUFFER_SIZE, 32);
	}

	pst_StreamHandler->ui_FileBufferSize = 2 * CDVD_BUFFER_SIZE;

#ifdef _DEBUG
	if(!MEM_IsTmpPointer(pst_StreamHandler->pc_FileBuffer))
	{
		GC_gui_StreamAllocSize += pst_StreamHandler->ui_FileBufferSize;
		if(GC_gui_StreamAllocSize > GC_gui_StreamMaxAllocSize) GC_gui_StreamMaxAllocSize = GC_gui_StreamAllocSize;
	}
#endif
	if(!pst_StreamHandler->pc_FileBuffer)
	{
		GC_M_Err(GC_pst_StreamOpen, "Could not allocate memory for the file read buffer");
		GC_StreamClose(pst_StreamHandler);
		return NULL;
	}

	switch(_i_Priority)
	{
	case 0:		/* highest priority */break;
	case 1:		/* default priority used by Nintendo audio stream */break;
	case 0x1002:	_i_Priority = 2;
	case 2:		/* default priority (Nintendo) */break;
	case 3:		/* lowest priority */break;
	case -1:	 /* ->used defaul Stream module priority */_i_Priority = 2; break;
	default:		GC_M_Err(GC_pst_StreamOpen, "Bad priority choice"); _i_Priority = 2; break;
	}

	pst_StreamHandler->i_UserPriority = _i_Priority;

	return pst_StreamHandler;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GC_StreamClose(GC_tdst_StreamHandler *_pst_Stream)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	extern char *gcSND_gp_BufferForLoading;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Stream) return;

	if(_pst_Stream->i_CurrentState != GC_Cte_StreamStateIdle)
	{
		if(DVDCancel((DVDCommandBlock *) &_pst_Stream->st_FileInfo) == -1)
		{
			GC_M_Err(GC_StreamClose, "Caution, cancel command failed when closing the stream");
		}
	}

#ifdef _DEBUG
	if(_pst_Stream->pc_FileBuffer)
	{
		if(!MEM_IsTmpPointer(_pst_Stream->pc_FileBuffer))
		{
			GC_gui_StreamAllocSize -= _pst_Stream->ui_FileBufferSize;
		}
	}
#endif
	DVDClose(&_pst_Stream->st_FileInfo);
	if(_pst_Stream->pc_FileBuffer)
	{
		if(_pst_Stream->pc_FileBuffer != gcSND_gp_BufferForLoading) MEM_FreeAlign(_pst_Stream->pc_FileBuffer);
	}

	_pst_Stream->i_CurrentState = GC_Cte_StreamStateFree;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GC_StreamCancel(GC_tdst_StreamHandler *_pst_Stream)
{
	if(!_pst_Stream) return;

	if(_pst_Stream->i_CurrentState != GC_Cte_StreamStateIdle)
	{
		if(DVDCancel((DVDCommandBlock *) &_pst_Stream->st_FileInfo) == -1)
		{
			GC_M_Err(GC_StreamCancel, "Caution, cancel command failed");
		}

		_pst_Stream->i_CurrentState = GC_Cte_StreamStateIdle;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GC_i_StreamRead
(
	GC_tdst_StreamHandler	*_pst_Stream,
	char					*_pc_Buff,
	unsigned int			_ui_Size,
	unsigned int			_ui_Position,
	GC_tdpfv_StreamCallBack _pfv_CallBack,
	unsigned int			_ui_Context
)
{
	/*~~~~~~~~~~*/
	BOOL	old;
	BOOL	b_Ret;
	/*~~~~~~~~~~*/

	if(!_pst_Stream)
	{
		GC_M_Err(GC_i_StreamRead, "Null stream handler");
		return -1;
	}

	if(!_pc_Buff)
	{
		GC_M_Err(GC_i_StreamRead, "Null user buffer adress");
		return -1;
	}

	if(!_ui_Size)
	{
		_pfv_CallBack(DVD_RESULT_GOOD, &_pst_Stream->st_FileInfo, _ui_Context);
		GC_M_Err(GC_i_StreamRead, "Null user size");
		return -1;
	}

	old = OSDisableInterrupts();
	if(_pst_Stream->i_CurrentState != GC_Cte_StreamStateIdle)
	{
		GC_M_Err(GC_i_StreamRead, "adding read command failed : stream is working");
		OSRestoreInterrupts(old);
		return -1;
	}

	_pst_Stream->pc_UserBuffer = _pc_Buff;
	_pst_Stream->ui_UserBufferSize = _ui_Size;
	_pst_Stream->ui_UserPosition = _ui_Position;
	_pst_Stream->pfv_UserCallBack = _pfv_CallBack;
	_pst_Stream->ui_UserContext = _ui_Context;

	_pst_Stream->ui_CurrentPosition = lRoundDown4B(_pst_Stream->ui_UserPosition);
	_pst_Stream->ui_CurrentDeltaAlign = _pst_Stream->ui_UserPosition - _pst_Stream->ui_CurrentPosition;

	if(_pst_Stream->ui_FileBufferSize < OSRoundUp32B(_pst_Stream->ui_UserBufferSize + _pst_Stream->ui_CurrentDeltaAlign))
	{
		_pst_Stream->ui_CurrentReadSize = _pst_Stream->ui_FileBufferSize;
		_pst_Stream->ui_CurrentWriteSize = _pst_Stream->ui_FileBufferSize - _pst_Stream->ui_CurrentDeltaAlign;
	}
	else
	{
		_pst_Stream->ui_CurrentReadSize = OSRoundUp32B(_pst_Stream->ui_UserBufferSize + _pst_Stream->ui_CurrentDeltaAlign);
		_pst_Stream->ui_CurrentWriteSize = _pst_Stream->ui_UserBufferSize;
	}

	_pst_Stream->pc_CurrentWritePtr = _pst_Stream->pc_UserBuffer;

	_pst_Stream->i_CurrentState = GC_Cte_StreamStateWorking;
	_pst_Stream->i_ErrorCode = GC_Cte_StreamErrorNone;

	OSRestoreInterrupts(old);

	do
	{
		b_Ret = DVDReadAsyncPrio
			(
				&_pst_Stream->st_FileInfo,
				_pst_Stream->pc_FileBuffer,
				_pst_Stream->ui_CurrentReadSize,
				_pst_Stream->ui_CurrentPosition,
				GC_StreamCallBack,
				_pst_Stream->i_UserPriority
			);

		if(!b_Ret)
		{
			CheckMediaError();
		}
		
	} while(!b_Ret);


	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GC_StreamCallBack(s32 result, DVDFileInfo *fileInfo)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	GC_tdst_StreamHandler	*pst_Stream;
	BOOL					old;
	BOOL					b_Finished, b_Ret;
	s32						i_Result;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0, pst_Stream = GC_gax_StreamList; i < GC_Cte_StreamMaxNumber; i++, pst_Stream++)
	{
		if(pst_Stream->i_CurrentState == GC_Cte_StreamStateFree) continue;
		if(&pst_Stream->st_FileInfo == fileInfo) break;
	}

	if(i >= GC_Cte_StreamMaxNumber)
	{
		GC_M_Err(GC_StreamCallBack, "Unknown stream handler");
	}
	else
	{
		b_Finished = TRUE;
		i_Result = DVD_RESULT_FATAL_ERROR;

		old = OSDisableInterrupts();
		switch(pst_Stream->i_CurrentState)
		{
		case GC_Cte_StreamStateWorking:
			switch(result)
			{
			case DVD_RESULT_FATAL_ERROR:
				GC_M_Err(GC_StreamCallBack, "Fatal error detected");
				pst_Stream->i_CurrentState = GC_Cte_StreamStateIdle;
				pst_Stream->i_ErrorCode = GC_Cte_StreamErrorFatal;
				break;

			case DVD_RESULT_CANCELED:
				/* GC_M_Err(GC_StreamCallBack, "Cancel transfert detected"); */
				pst_Stream->i_CurrentState = GC_Cte_StreamStateIdle;
				pst_Stream->i_ErrorCode = GC_Cte_StreamErrorFatal;
				i_Result = DVD_RESULT_CANCELED;
				break;

			default:
				if(result == pst_Stream->ui_CurrentReadSize)
				{
					i_Result = DVD_RESULT_GOOD;
					pst_Stream->i_CurrentState = GC_Cte_StreamStateIdle;

					/* copy data */
					L_memcpy
					(
						pst_Stream->pc_CurrentWritePtr,
						pst_Stream->pc_FileBuffer + pst_Stream->ui_CurrentDeltaAlign,
						pst_Stream->ui_CurrentWriteSize
					);
					pst_Stream->pc_CurrentWritePtr += pst_Stream->ui_CurrentWriteSize;
					pst_Stream->ui_UserBufferSize -= pst_Stream->ui_CurrentWriteSize;

					pst_Stream->ui_CurrentDeltaAlign = 0;
					pst_Stream->ui_CurrentPosition += pst_Stream->ui_CurrentReadSize;

					if(pst_Stream->ui_UserBufferSize)
					{
						pst_Stream->i_CurrentState = GC_Cte_StreamStateWorking;
						b_Finished = FALSE;
						if(pst_Stream->ui_FileBufferSize < OSRoundUp32B(pst_Stream->ui_UserBufferSize))
						{
							pst_Stream->ui_CurrentReadSize = pst_Stream->ui_FileBufferSize;
							pst_Stream->ui_CurrentWriteSize = pst_Stream->ui_FileBufferSize;
						}
						else
						{
							pst_Stream->ui_CurrentReadSize = OSRoundUp32B(pst_Stream->ui_UserBufferSize);
							pst_Stream->ui_CurrentWriteSize = pst_Stream->ui_UserBufferSize;
						}

						b_Ret = DVDReadAsyncPrio
							(
								&pst_Stream->st_FileInfo,
								pst_Stream->pc_FileBuffer,
								pst_Stream->ui_CurrentReadSize,
								pst_Stream->ui_CurrentPosition,
								GC_StreamCallBack,
								pst_Stream->i_UserPriority
							);

						if(!b_Ret)
						{
							GC_M_Err(GC_StreamCallBack, "Can not add a read command");
							pst_Stream->i_CurrentState = GC_Cte_StreamStateIdle;
							pst_Stream->i_ErrorCode = GC_Cte_StreamErrorFatal;
							b_Finished = TRUE;
							i_Result = DVD_RESULT_FATAL_ERROR;
						}
					}
				}
				else
				{
					GC_M_Err(GC_StreamCallBack, "Transfert error detected");
					pst_Stream->i_CurrentState = GC_Cte_StreamStateIdle;
					pst_Stream->i_ErrorCode = GC_Cte_StreamErrorFatal;
				}
				break;
			}
			break;

		case GC_Cte_StreamStateIdle:
			GC_M_Err(GC_StreamCallBack, "Bad stream state (idle)");
			pst_Stream->i_CurrentState = GC_Cte_StreamStateIdle;
			pst_Stream->i_ErrorCode = GC_Cte_StreamErrorFatal;
			break;

		default:
			GC_M_Err(GC_StreamCallBack, "Bad stream state");
			pst_Stream->i_CurrentState = GC_Cte_StreamStateIdle;
			pst_Stream->i_ErrorCode = GC_Cte_StreamErrorFatal;
			break;
		}

		OSRestoreInterrupts(old);

		if(b_Finished && pst_Stream->pfv_UserCallBack)
			pst_Stream->pfv_UserCallBack(i_Result, &pst_Stream->st_FileInfo, pst_Stream->ui_UserContext);
	}
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
