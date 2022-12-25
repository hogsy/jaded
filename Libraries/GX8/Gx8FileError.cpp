#include "Gx8FileError.h"
#include "Gx8FontC.h"
#include "Gx8Init.h"
#include "GDInterface/GDInterface.h"
#include "INOut/INOJoystick.h"
#include "INOut/INO.h"

//extern "C"
//{
#include "ENGine/Sources/text/text.h"
//};

typedef enum
{
    GX8_FILEERROR_NULL,
    GX8_FILEERROR,
}Gx8FileErrorWriting;


struct ErrorSentence
{
    int language;
    Gx8FileErrorWriting sentence;
    short x;
    short y; 
    WCHAR* text;
};

static Gx8FileErrorWriting _message = GX8_FILEERROR;

static CRITICAL_SECTION	_CS;

volatile HANDLE gSNDThreadHandler = 0;

bool DontPlayVideoWhileError = false;

static ErrorSentence sentences[] =
{
//    { XC_LANGUAGE_ENGLISH, GX8_NO_PAD_RECONNECT, 180, 200, L"Please reconnect the contoller" },
/*    { INO_e_English, GX8_FILEERROR, 100, 100, L"There's a problem with the disc you're using.\nIt may be dirty or damaged.\nPress \u00b3 to continue." },//old font\u0100
    { INO_e_French, GX8_FILEERROR, 110, 100, L"Le disque utilisé présente une anomalie.\nIl est peut-être sale ou endommagé.\nAppuyer sur \u00b3 pour continuer." },
	{ INO_e_German, GX8_FILEERROR, 50, 100, L"Bei der von Ihnen benutzten CD \nist ein Problem aufgetreten.\nMöglicherweise ist sie verschmutzt oder beschädigt.\nWeiter mit \u00b3-Taste." },
	{ INO_e_Spanish, GX8_FILEERROR, 80, 100, L"Hay un problema con el disco que está usando.\nPuede estar sucio o dañado.\nPresione \u00b3 para continuar." },
	{ INO_e_Italian, GX8_FILEERROR, 100, 100, L"Il disco in uso ha qualche problema.\nPotrebbe essere sporco o danneggiato.\nPremere \u00b3 per continuare." },
	{ INO_e_Dutch, GX8_FILEERROR, 180, 100, L"Er is een probleem met de disc die u gebruikt.\nDe disc is mogelijk vuil of beschadigd.\nDruk op \u00b3 om verder te gaan." },

	{ INO_e_Swedish, GX8_FILEERROR, 180, 100, L"Spelskivan du använder har ett fel. Den kan vara smutsig eller skadad. \nTryck \u00b3 för att fortsätta." },
	{ INO_e_Danish, GX8_FILEERROR, 180, 100, L"Der er et problem med den disk, du bruger. Den er muligvis snavset eller beskadiget. \nTryk på \u00b3 for at fortsætte." },
	{ INO_e_Finnish, GX8_FILEERROR, 180, 100, L"Käytettävässä levyssä on ongelma. Levy voi olla likainen tai vahingoittunut. \nJatka painamalla \u00b3:ta." },
	{ INO_e_Norwegian, GX8_FILEERROR, 180, 100, L"Det er problemer med CDen du bruker. Den kan være skitten eller skadet. \nTrykk \u00b3 for å fortsette." },*/

    { INO_e_English, GX8_FILEERROR, 100, 100, L"There's a problem with the disc you're using.\nIt may be dirty or damaged." },//old font\u0100
    { INO_e_French, GX8_FILEERROR, 110, 100, L"Le disque utilisé présente une anomalie.\nIl est peut-être sale ou endommagé." },
	{ INO_e_German, GX8_FILEERROR, 50, 100, L"Bei der von Ihnen benutzten CD \nist ein Problem aufgetreten.\nMöglicherweise ist sie verschmutzt oder beschädigt." },
	{ INO_e_Spanish, GX8_FILEERROR, 80, 100, L"Hay un problema con el disco que está usando.\nPuede estar sucio o dañado." },
	{ INO_e_Italian, GX8_FILEERROR, 100, 100, L"Il disco in uso ha qualche problema.\nPotrebbe essere sporco o danneggiato." },
	{ INO_e_Dutch, GX8_FILEERROR, 90, 100, L"Er is een probleem met de disc die u gebruikt.\nDe disc is mogelijk vuil of beschadigd." },

	{ INO_e_Swedish, GX8_FILEERROR, 140, 100, L"Spelskivan du använder har ett fel.\nDen kan vara smutsig eller skadad." },
	{ INO_e_Danish, GX8_FILEERROR, 100, 100, L"Der er et problem med den disk, du bruger.\nDen er muligvis snavset eller beskadiget." },
	{ INO_e_Finnish, GX8_FILEERROR, 100, 100, L"Käytettävässä levyssä on ongelma.\nLevy voi olla likainen tai vahingoittunut." },
	{ INO_e_Norwegian, GX8_FILEERROR, 120, 100, L"Det er problemer med CD en du bruker.\nDen kan være skitten eller skadet." },

};

#define SENTENCE_COUNT  (sizeof( sentences )/sizeof( sentences[0] ))


void
Gx8_FileError_WriteString(void)
{
    INO_tden_LanguageId language = (INO_tden_LanguageId)TEXT_gst_Global.i_CurrentLanguage;//INO_e_English;//INO_getGameLanguage();
    

    for( int i=0; i<SENTENCE_COUNT; ++i )
    {
        if( sentences[i].language == language && 
            sentences[i].sentence == _message )
        {
            WriteString( sentences[i].text,
                         float(sentences[i].x),
                         float(sentences[i].y),
                         0xFFFFFFFF ); // white
        }
    }
}


void Gx8_FileError(void)
{
		//IDirect3DSurface8* pBackBuffer;
		//IDirect3DSurface8* ppSurfaceLevel;
        DWORD  DeviceSettings;
		int FrameTimer=0;

		EnterCriticalSection(&_CS);

        Gx8_tdst_SpecificData* pst_SD = (Gx8_tdst_SpecificData*)GDI_gpst_CurDD->pv_SpecificData;

		IDirect3DDevice8_CreateStateBlock(pst_SD->mp_D3DDevice,D3DSBT_ALL,&DeviceSettings);
		IDirect3DDevice8_CaptureStateBlock(pst_SD->mp_D3DDevice,DeviceSettings);

		IDirect3DDevice8_EnableOverlay( pst_SD->mp_D3DDevice,FALSE );
		DontPlayVideoWhileError = true;


		//Save the current front buffer and use the After Effect tmp buffer to store it
		/*IDirect3DDevice8_GetBackBuffer(pst_SD->mp_D3DDevice, -1, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
		IDirect3DTexture8_GetSurfaceLevel
		(
			pst_SD->pTmpFrameBufferTexture,
			0,
			&ppSurfaceLevel
		);
		IDirect3DDevice8_CopyRects( pst_SD->mp_D3DDevice, pBackBuffer, NULL, 0, ppSurfaceLevel, NULL );
		IDirect3DSurface8_Release(pBackBuffer);*/
	
		//INO_Joystick_Update();
		//IDirect3DDevice8_Present(pst_SD->mp_D3DDevice, NULL,NULL,NULL,NULL);
		//Enter the error loop. Exit when A is pressed
		while(!INO_b_MultiJoystick_IsButtonJustUp(e_ButtonA))
		{
			IDirect3DDevice8_Clear(pst_SD->mp_D3DDevice, 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,  0x00000000, 1.0f, 0);

			//IDirect3DDevice8_Swap(pst_SD->mp_D3DDevice, D3DSWAP_COPY);
			//IDirect3DDevice8_Present(pst_SD->mp_D3DDevice, NULL,NULL,NULL,NULL);
			

			//IDirect3DDevice8_GetBackBuffer(pst_SD->mp_D3DDevice, -1, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
			//IDirect3DDevice8_CopyRects( pst_SD->mp_D3DDevice, ppSurfaceLevel,  NULL, 0, pBackBuffer, NULL );
			//IDirect3DSurface8_Release(pBackBuffer);

//IDirect3DDevice8_Swap(pst_SD->mp_D3DDevice, D3DSWAP_DEFAULT);

			//Display the blinking string
			if(FrameTimer<30)
			{
				//Write a string
				Gx8_FileError_WriteString();
			}
			else
			{
				if(FrameTimer>60)
					FrameTimer=0;
			}
		
			FrameTimer++;

			//IDirect3DDevice8_Swap(pst_SD->mp_D3DDevice, D3DSWAP_FINISH);
			IDirect3DDevice8_Present(pst_SD->mp_D3DDevice, NULL,NULL,NULL,NULL);

			//update the pad
			INO_Joystick_Update();

		}


		//Copy back the initial screen
/*		IDirect3DDevice8_Swap(pst_SD->mp_D3DDevice, D3DSWAP_COPY);
		IDirect3DDevice8_GetBackBuffer(pst_SD->mp_D3DDevice, -1, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
		IDirect3DDevice8_CopyRects( pst_SD->mp_D3DDevice, ppSurfaceLevel,  NULL, 0, pBackBuffer, NULL );
		IDirect3DSurface8_Release(pBackBuffer);
		IDirect3DDevice8_Swap(pst_SD->mp_D3DDevice, D3DSWAP_FINISH);


		IDirect3DSurface8_Release(ppSurfaceLevel);*/
	
		IDirect3DDevice8_ApplyStateBlock(pst_SD->mp_D3DDevice,DeviceSettings);
		IDirect3DDevice8_DeleteStateBlock(pst_SD->mp_D3DDevice,DeviceSettings);

		LeaveCriticalSection(&_CS);
		DontPlayVideoWhileError = false;
}


void Gx8_FileErrorSound(void)
{
        DWORD suspendCount;
        HANDLE tempHandler;

        DuplicateHandle( 0, GetCurrentThread(), 0, &tempHandler, 0,FALSE,0 );
        ERR_X_Assert(tempHandler != 0);
        gSNDThreadHandler = tempHandler;

		suspendCount = SuspendThread(gSNDThreadHandler);
        ERR_X_Assert( suspendCount != -1 );
        gSNDThreadHandler = 0;
}

void 
Gx8_FileError_CheckSound( void )
{
    if( gSNDThreadHandler == 0 )
    {
        return;
    }
    Gx8_FileError();

    ResumeThread( gSNDThreadHandler );
}


void
Gx8_FileError_Init( void )
{
    InitializeCriticalSection(&_CS);
    gSNDThreadHandler = 0;
}
