/*$T INO.c GC 1.138 03/16/04 09:49:58 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "INO.h"
#include "INOfile.h"
#include "INOkeyboard.h"
#include "INOjoystick.h"
#include "INOsaving.h"
#include "BIGfiles/LOAding/LOAread.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

extern void INO_SimulateReset(void);
extern void INO_SavReinit(void);

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

extern int				INO_gai_PresentLanguage[INO_e_MaxLangNb];
int						INO_gi_DiscError = INO_Cte_DiscErrorNone;
INO_tden_MasterType		INO_ge_MasterType = e_Master_None;
INO_tdst_LanguageName	INO_dst_LanguageName[INO_e_MaxLangNb] =
{
	{ "fr", "French" },
	{ "en", "English" },
	{ "da", "Danish" },
	{ "nl", "Dutch" },
	{ "fi", "Finnish" },
	{ "de", "German" },
	{ "it", "Italian" },
	{ "es", "Spanish" },
	{ "pt", "Portuguese" },
	{ "sv", "Swedish" },
	{ "pl", "Polish" },
	{ "ru", "Russian" },
	{ "ja", "Japanese" },
	{ "zh", "Chinese" },
	{ "sq", "Albanian" },
	{ "ar", "Arabic" },
	{ "bg", "Bulgarian" },
	{ "be", "Byelorussian" },
	{ "el", "Greek" },
	{ "ko", "Korean" },
	{ "no", "Norwegian" },
	{ "ro", "Romanian" },
	{ "sr", "Serbian" },
	{ "sk", "Slovak" },
	{ "sl", "Slovenian" },
	{ "tr", "Turkish" },
	{ "cs", "Czech"},
	{ "hu", "Hungarian"},
	{ "tw", "Taiwanese"}
};

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_InitModule(void)
{
	INO_gi_DiscError = INO_Cte_DiscErrorNone;
	INO_KeyboardInit();
	INO_SavInitModule();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_CloseModule(void)
{
	INO_KeyboardClose();
	INO_SavCloseModule();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_Update(void)
{
	INO_Joystick_Update();
	INO_Keyboard_Update();
	INO_Save_Update();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_Reinit(void)
{
	INO_gi_DiscError = INO_Cte_DiscErrorNone;

	INO_l_JoystickMask = INO_JoyMask_All;
	INO_l_JoystickDir = 0;

	INO_SimulateReset();

	INO_PifSet(0);
	INO_PafSet(0, 0);
	INO_SavReinit();
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *INO_p_StoreLanguage(char *psz_Scan)
{
	/*~~*/
	int i;
	/*~~*/

	if(!psz_Scan) return NULL;

	L_memset(INO_gai_PresentLanguage, 0, INO_e_MaxLangNb * sizeof(int));

	while(*psz_Scan && (*psz_Scan != ']'))
	{
		for(i = 0; i < INO_e_MaxLangNb; i++)
		{
			if(L_strnicmp(psz_Scan, INO_dst_LanguageName[i].az_ShortName, 2) == 0) INO_gai_PresentLanguage[i] = 1;
		}

		while(*psz_Scan != ',')
		{
			if(*psz_Scan == ']') return ++psz_Scan;
			psz_Scan++;
		}

		psz_Scan++;
	}

	return psz_Scan;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_SaveLanguageFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	FILE		*f;
	int			i;
	char		asz_Path[1024];
	time_t		now;
	struct tm	when;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	strcpy(asz_Path, "INOlang.c");

	f = fopen(asz_Path, "wt");
	if(f)
	{
		time(&now);
		when = *localtime(&now);

		fprintf(f, "/*\n");
		fprintf
		(
			f,
			" +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
		);
		fprintf(f, "    !! This file is generated by Jade editor !!\n");
		fprintf
		(
			f,
			"    %s : %d/%d/%d - %02d:%02d\n",
			asz_Path,
			1900 + when.tm_year,
			when.tm_mon,
			when.tm_mday,
			when.tm_hour,
			when.tm_min
		);
		fprintf
		(
			f,
			" +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
		);
		fprintf(f, "*/\n\n\n");
		fprintf(f, "#include \"Precomp.h\"\n");
		fprintf(f, "#include \"INO.h\"\n\n");
		fprintf(f, "int INO_gai_PresentLanguage[INO_e_MaxLangNb] =\n");
		fprintf(f, "{\n");
		for(i = 0; i < (INO_e_MaxLangNb - 1); i++)
		{
			if(INO_gai_PresentLanguage[i])
				fprintf(f, " 1, /* %s */\n", INO_dst_LanguageName[i].az_LongName);
			else
				fprintf(f, " 0, /* %s */\n", INO_dst_LanguageName[i].az_LongName);
		}

		if(INO_gai_PresentLanguage[i])
			fprintf(f, " 1  /* %s */\n", INO_dst_LanguageName[i].az_LongName);
		else
			fprintf(f, " 0  /* %s */\n", INO_dst_LanguageName[i].az_LongName);
		fprintf(f, "};\n\n");

		fprintf(f, "/*\n");
		fprintf
		(
			f,
			" ***********************************************************************************************************************\n"
		);
		fprintf(f, "    EOF\n");
		fprintf
		(
			f,
			" ***********************************************************************************************************************\n"
		);
		fprintf(f, "*/\n");

		fclose(f);
	}
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef JADEFUSION
BOOL INO_b_LanguageIsPresent(int i, BOOL bSound)
#else
BOOL INO_b_LanguageIsPresent(int i)
#endif
{
	/* check idx */
	if(i >= INO_e_MaxLangNb) return FALSE;
	if(i < 0) return FALSE;

#ifdef ACTIVE_EDITORS
	/* special case french is always present during Binarization */
	if(LOA_IsBinarizing() && (i == INO_e_French)) return TRUE;
#endif

	/* else return the true state */
/*
    not used today
	switch(INO_ge_MasterType)
	{
	case e_MasterPS2_SCEE:
		if(INO_gai_PresentLanguage[i]) return TRUE;
		break;

	case e_MasterPS2_SCEA:
		switch(i)
		{
		case INO_e_English:
		case INO_e_French:
		case INO_e_Spanish:
			if(INO_gai_PresentLanguage[i]) return TRUE;
			break;

		default:
			return FALSE;
		}
		break;

	default:
		if(INO_gai_PresentLanguage[i]) return TRUE;
		break;
	}
	*/
#if defined(_XENON)
	if (bSound)
	{
		if (INO_gai_PresentLanguage[INO_e_Japanese] == 0)
		{
			// US / Europe Version

			return ((i == INO_e_English) ||
					(i == INO_e_French) ||
					(i == INO_e_German) ||
					(i == INO_e_Italian) ||
					(i == INO_e_Russian) ||
					(i == INO_e_Spanish));
		}
		else
		{
			// Japanese Version

			return (i == INO_e_English);
			//return ((i == INO_e_English) ||
					//(i == INO_e_Japanese));
		}
	}
#endif
    if(INO_gai_PresentLanguage[i]) return TRUE;
	return FALSE;
}


#ifdef _XBOX
//
// ---- run mode
//

static INO_tden_RunMode _runMode = INO_tden_RunMode_Undefined;

void
INO_setRunMode( INO_tden_RunMode mode )
{
    ERR_X_Assert( mode != INO_tden_RunMode_Undefined );
    _runMode = mode;
}

BOOL
INO_isRunMode_Demo( void )
{
    ERR_X_Assert( _runMode != INO_tden_RunMode_Undefined );

    return _runMode == INO_tden_RunMode_Demo;
}

INO_tden_RunMode
INO_getRunMode( void )
{
    ERR_X_Assert( _runMode != INO_tden_RunMode_Undefined );

    return _runMode;
}
#endif
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
