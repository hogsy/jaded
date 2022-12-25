#include "Gx8HandleNoPad.h"
#include "Gx8FontC.h"
#include "Gx8Init.h"
#include "GDInterface/GDInterface.h"
#include "INOut/INO.h"
#include "text/text.h"

extern "C" int INO_gi_CurrentFirstPadId;
//extern "C" void AE_TITLESAFEAREA(Gx8_tdst_SpecificData *pst_SD);

struct Sentence
{
    int language;
    Gx8NoPadWriting sentence;
    short x;
    short y;
    WCHAR text[128];
};

WCHAR* No=L"1";

static Gx8NoPadWriting _message = GX8_NO_PAD_NULL;

int no=1;

static Sentence sentences[] = //Old font start = 108/109
{
    { INO_e_English, GX8_NO_PAD_RECONNECT, 180, 200, L"Please reconnect the controller to controller port @ " },
	{ INO_e_English, GX8_NO_PAD_RECONNECT, 186, 226, L"and press \u00b1\u00b2 to continue" },
    { INO_e_French, GX8_NO_PAD_RECONNECT, 173, 200, L"Pour continuer, reconnecter la manette dans le port @ " },
    { INO_e_French, GX8_NO_PAD_RECONNECT, 165, 226, L"et appuyer sur \u00b1\u00b2" },
    { INO_e_Spanish, GX8_NO_PAD_RECONNECT, 155, 200, L"Volver a conectar el controlador,en el puerto"},
    { INO_e_Spanish, GX8_NO_PAD_RECONNECT, 171,226, L" de controlador @, y presionar \u00b1\u00b2 para continuar" },
    { INO_e_German, GX8_NO_PAD_RECONNECT, 180, 200, L"Bitte den Controller @ wieder" },
    { INO_e_German, GX8_NO_PAD_RECONNECT, 180, 226, L" anschließen und \u00b1\u00b2 drücken." },
    { INO_e_Dutch, GX8_NO_PAD_RECONNECT, 180, 200, L"Sluit de controller @ opnieuw aan en" },
    { INO_e_Dutch, GX8_NO_PAD_RECONNECT, 180, 226, L" druk op \u00b1\u00b2 om verder te gaan." },
    { INO_e_Italian, GX8_NO_PAD_RECONNECT, 200, 200, L"Ricollegare il controller alla porta del controller @ e" },
    { INO_e_Italian, GX8_NO_PAD_RECONNECT, 180, 226, L" premere \u00b1\u00b2 per continuare." },
    { INO_e_Swedish, GX8_NO_PAD_RECONNECT, 200, 200, L"Anslut handkontrollen @ och "},
    { INO_e_Swedish, GX8_NO_PAD_RECONNECT, 180, 226, L"tryck \u00b1\u00b2 för att fortsätta." },
	{ INO_e_Danish, GX8_NO_PAD_RECONNECT, 200, 200, L"Tilslut controlleren @ igen, og tryk " },
    { INO_e_Danish, GX8_NO_PAD_RECONNECT, 180, 226, L"på \u00b1\u00b2 for at fortsætte." },
	{ INO_e_Finnish, GX8_NO_PAD_RECONNECT, 200, 200, L"Kytke ohjain takaisin @." },
    { INO_e_Finnish, GX8_NO_PAD_RECONNECT, 180, 226, L"Jatka painamalla \u00b1\u00b2 -näppäintä" },
	{ INO_e_Norwegian, GX8_NO_PAD_RECONNECT, 200, 200, L"Koble kontrolleren @ til igjen, og" },
    { INO_e_Norwegian, GX8_NO_PAD_RECONNECT, 180, 226, L"trykk \u00b1\u00b2 for å fortsette." },


    { INO_e_English, GX8_NO_PAD_PRESS_START, 180, 226, L"Press \u00b1\u00b2 to continue" },
    { INO_e_French, GX8_NO_PAD_PRESS_START, 150, 200, L"Pour continuer appuyer sur \u00b1\u00b2" },
    { INO_e_Spanish, GX8_NO_PAD_PRESS_START, 171,226, L"Presione \u00b1\u00b2 para continuar" },
    { INO_e_German, GX8_NO_PAD_PRESS_START, 180, 226, L"\u00b1\u00b2 drücken"},
    { INO_e_Dutch, GX8_NO_PAD_PRESS_START, 180, 226, L"Druk op \u00b1\u00b2 om verder te gaan."},
    { INO_e_Italian, GX8_NO_PAD_PRESS_START, 180, 226, L"Premi \u00b1\u00b2 per continuare"},

    { INO_e_Swedish, GX8_NO_PAD_PRESS_START, 180, 226, L"Tryck \u00b1\u00b2 för att börja" },
    { INO_e_Danish, GX8_NO_PAD_PRESS_START, 180, 226, L"Tryk på \u00b1\u00b2 for at begynde." },
    { INO_e_Finnish, GX8_NO_PAD_PRESS_START, 180, 226, L"Aloita painamalla \u00b1\u00b2-näppäintä." },
    { INO_e_Norwegian, GX8_NO_PAD_PRESS_START, 180, 226, L"Trykk \u00b1\u00b2 for å starte." },


};

#define SENTENCE_COUNT  (sizeof( sentences )/sizeof( sentences[0] ))


void
Gx8_HandleNoPad_WriteString(void)
{
	
	static	unsigned int Alpha=0;
	float	RealWidth = 0, RealHeight = 0;
	float	PosX = 99999, PosY = 99999;
    INO_tden_LanguageId language = (INO_tden_LanguageId)TEXT_gst_Global.i_CurrentLanguage;//INO_getGameLanguage();//INO_e_English;//INO_getGameLanguage();
/*
	 WCHAR *String;
	//Attenuate background.
	//if ( _message ==1 ) 
	{
		//WCHAR nouveau;	
		String = (WCHAR* )sentences[0].text;
		//swprintf( sentences[0].text, L"1" );
		//wcscpy((wchar_t*)sentences[0].text,(wchar_t*)No);

		while (*String)
		{
			//*String = *String;
			if (*String == '@')
			{
				*String = L'1' + 0;
			}
			String += 2;
		}



		//swprintf( sentences[1].text, L"caca" );
	}*/



    for( int i=0; i<SENTENCE_COUNT; ++i )
    {
        if( sentences[i].language == language && 
            sentences[i].sentence == _message )
        {
			float	Width,Height;
			if (sentences[i].sentence == GX8_NO_PAD_RECONNECT)
			{
				WCHAR *String;
				String = (WCHAR* )sentences[i].text;

				while (*String)
				{
					//*String = *String;
					if (*String == '@')
					{
						//*String = L'1' + 0;
						*String = (INO_gi_CurrentFirstPadId+1) + 48;//nb of ASCII character
					}
					String += 1;
				}
			}



			GetTextExtent(	sentences[i].text,
							&Width,
							&Height,
							false);

			if(Width>RealWidth)
				RealWidth = Width;

            float x = (640.0f - Width)/2.0f;
			if( x < PosX )
				PosX = x;

			if(sentences[i].y<PosY)
				PosY = sentences[i].y;

			RealHeight+=Height;
		}
	}

	Gx8_Draw2DQuad(PosX-10, PosY-10, RealWidth+20, RealHeight+20, 0xCC000000);

    for( int i=0; i<SENTENCE_COUNT; ++i )
    {
        if( sentences[i].language == language && 
            sentences[i].sentence == _message )
        {
			unsigned int CurrentAlpha = Alpha;

			if(CurrentAlpha>255)
				CurrentAlpha = 512 - CurrentAlpha;

            float width;
            float height;

			GetTextExtent(	sentences[i].text,
							&width,
							&height,
							true );

            float x = (640.0f - width)/2.0f;
            
/*			if (_message ==1 && i==0)
			{
			WCHAR popo[50];//Alors la attention bordel>>>
			switch (language)
			{
				case INO_e_English:
				{				
					switch (INO_gi_CurrentFirstPadId+1)
					{
						case 1:
						swprintf (popo,L"Please reconnect the controller in port 1");break;
						case 2:
						swprintf (popo,L"Please reconnect the controller in port 2");break;
						case 3:
						swprintf (popo,L"Please reconnect the controller in port 3");break;
						case 4:
						swprintf (popo,L"Please reconnect the controller in port 4");break;
					}
				}
				case INO_e_French:
				{				
					switch (INO_gi_CurrentFirstPadId+1)
					{
						case 1:
						swprintf (popo,L"Pour continuer, reconnecter la manette dans le port 1");break;
						case 2:
						swprintf (popo,L"Pour continuer, reconnecter la manette dans le port 2");break;
						case 3:
						swprintf (popo,L"Pour continuer, reconnecter la manette dans le port 3");break;
						case 4:
						swprintf (popo,L"Pour continuer, reconnecter la manette dans le port 4");break;
					}			
				}
				case INO_e_Spanish:
				{				
					switch (INO_gi_CurrentFirstPadId+1)
					{
					case 1:
					swprintf (popo,L"Vuelva a conectar el controlador 1 y");break;
					case 2:
					swprintf (popo,L"Vuelva a conectar el controlador 2 y");break;
					case 3:
					swprintf (popo,L"Vuelva a conectar el controlador 3 y");break;
					case 4:
					swprintf (popo,L"Vuelva a conectar el controlador 4 y");break;
				}
				}
				case INO_e_German:
				{				
					switch (INO_gi_CurrentFirstPadId+1)
					{
					case 1:
					swprintf (popo,L"Bitte den Controller 1 wieder");break;
					case 2:
					swprintf (popo,L"Bitte den Controller 2 wieder");break;
					case 3:
					swprintf (popo,L"Bitte den Controller 3 wieder");break;
					case 4:
					swprintf (popo,L"Bitte den Controller 4 wieder");break;
				}				
				}
				case INO_e_Dutch:
				{				
					switch (INO_gi_CurrentFirstPadId+1)
					{
					case 1:
					swprintf (popo,L"Sluit de controller 1 opnieuw aan en");break;
					case 2:
					swprintf (popo,L"Sluit de controller 2 opnieuw aan en");break;
					case 3:
					swprintf (popo,L"Sluit de controller 3 opnieuw aan en");break;
					case 4:
					swprintf (popo,L"Sluit de controller 4 opnieuw aan en");break;
				}		
				}
				case INO_e_Italian:
				{				
					switch (INO_gi_CurrentFirstPadId+1)
					{
					case 1:
					swprintf (popo,L"Ricollega il controller @ e");break;
					case 2:
					swprintf (popo,L"Ricollega il controller @ e");break;
					case 3:
					swprintf (popo,L"Ricollega il controller @ e");break;
					case 4:
					swprintf (popo,L"Ricollega il controller @ e");break;
				}	
				}

		
			
			WriteString( popo,
                         x+2.0f,
                         float(sentences[i].y+2),
                         CurrentAlpha<<24 ); // Black
            WriteString( popo,
                         x,
                         float(sentences[i].y),
                         (0x00FFFFFF|(CurrentAlpha << 24)) ); // white
			}
			else */
			{
			WriteString( sentences[i].text,
                         x+2.0f,
                         float(sentences[i].y+2),
                         CurrentAlpha<<24 ); // Black
            WriteString( sentences[i].text,
                         x,
                         float(sentences[i].y),
                         (0x00FFFFFF|(CurrentAlpha << 24)) ); // white
			}
				
			
			
			/*if ( _message ==1 )  
			{
				WriteString( No,
								x+2.0f,
								float(sentences[i].y+2),
								CurrentAlpha<<24 ); // Black
			
				WriteString( No,
								x+2.0f,
								float(sentences[i].y+2),
								(0x00FFFFFF|(CurrentAlpha<<24 ))); // Black
			
			}*/		
		}
    }


	Alpha+=10;
	if(Alpha > 512)
		Alpha = 0;
}


void
Gx8_HandleNoPad_setMessage( Gx8NoPadWriting sentence )
{
    _message = sentence;
}

bool
Gx8_HandleNoPad_isMessageToDisplay( void )
{
    return _message != GX8_NO_PAD_NULL;
}

void
Gx8_HandleNoPad(void)
{
	if( Gx8_HandleNoPad_isMessageToDisplay() )
	{
        DWORD  DeviceSettings;
        Gx8_tdst_SpecificData* pst_SD = (Gx8_tdst_SpecificData*)GDI_gpst_CurDD->pv_SpecificData;

		IDirect3DDevice8_CreateStateBlock(pst_SD->mp_D3DDevice,D3DSBT_ALL,&DeviceSettings);
		IDirect3DDevice8_CaptureStateBlock(pst_SD->mp_D3DDevice,DeviceSettings);
	
		//Write a string
		Gx8_HandleNoPad_WriteString();

		//AE_TITLESAFEAREA( (Gx8_tdst_SpecificData*)pst_SD->mp_D3DDevice);

		IDirect3DDevice8_ApplyStateBlock(pst_SD->mp_D3DDevice,DeviceSettings);
		IDirect3DDevice8_DeleteStateBlock(pst_SD->mp_D3DDevice,DeviceSettings);
	}
}
void Gx8_HandleNoPadPresent()
{
			Gx8_tdst_SpecificData   *pst_SD;
			pst_SD = (Gx8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData;
			IDirect3DDevice8_EnableOverlay( pst_SD->mp_D3DDevice,FALSE );

			IDirect3DDevice8_Clear(pst_SD->mp_D3DDevice, 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,  0x00000000, 1.0f, 0);
			//IDirect3DDevice8_Swap(pst_SD->mp_D3DDevice, D3DSWAP_COPY);
			//Gx8_HandleNoPad_setMessage(GX8_NO_PAD_RECONNECT);//1=GX8_NO_PAD_RECONNECT
			Gx8_HandleNoPad();
			IDirect3DDevice8_Present(pst_SD->mp_D3DDevice, NULL,NULL,NULL,NULL);
}
