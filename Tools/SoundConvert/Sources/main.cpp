/*$T main.cpp GC 1.138 05/25/04 15:00:19 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "stdafx.h"
#include <direct.h>
#include <stdio.h>
#include "main.h"
#include "driver.h"
#include "imaadpcm.h"
#include "wavparse.h"
#include "MSADPCM.h"


#define M_VERSION  "2.05"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

MAIN_tdst_Command	MAIN_gst_Command;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Init(void)
{
	L_memset(&MAIN_gst_Command, 0, sizeof(MAIN_tdst_Command));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ParseCommandLine(int argc, char *argv[])
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	char	*pz1, *pz2, *pz;
	/*~~~~~~~~~~~~~~~~~~~~*/

	for(int i = 1; i < argc; i++)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
		const char	*szArg = argv[i];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(szArg[0] == '-' || szArg[0] == '/')
		{
			switch(szArg[1])
			{

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			case 'a':
			case 'A':
				if(!stricmp(szArg + 1, "align") || !stricmp(szArg + 1, "am")) MAIN_gst_Command.b_AlignMarker = true;
				break;

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			case 'c':
			case 'C':
				if(!stricmp(szArg + 1, "c") || !stricmp(szArg + 1, "cc")) MAIN_gst_Command.b_CopyAllChunk = true;
				break;

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			case 'd':
			case 'D':
				if(!_strnicmp(szArg + 1, "dll", strlen("dll")))
				{
					pz = (char *) szArg + 1;
					pz1 = strchr(szArg + 1, '(');
					if(pz1)
					{
						pz1++;
						pz2 = strchr(pz, ')');
						*pz2 = 0;
						if(strlen(pz1) > M_STR_SIZE)
						{
							printf("** too long path\n");
							ExitProcess(-1);
						}

						strcpy(MAIN_gst_Command.asz_DllPath, pz1);
					}

					pz1 = strchr(MAIN_gst_Command.asz_DllPath, '/');
					while(pz1)
					{
						*pz1 = '\\';
						pz1 = strchr(MAIN_gst_Command.asz_DllPath, '/');
					}

					pz = pz2 + 1;
				}
				break;

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			case 'f':
			case 'F':
				if(!stricmp(szArg + 1, "file") || !stricmp(szArg + 1, "fi"))
					MAIN_gst_Command.b_OnlyOneFileTargeted = true;
				break;

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			case 'g':
			case 'G':
				if(!stricmp(szArg + 1, "gamecube") || !stricmp(szArg + 1, "gc")) MAIN_gst_Command.e_Format = FMT_e_GC;
				break;

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			case 'm':
			case 'M':
				if(!stricmp(szArg + 1, "mux")) MAIN_gst_Command.b_MuxStereo = true;
				break;

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			case 'o':
			case 'O':
				if(!stricmp(szArg + 1, "overwrite") || !stricmp(szArg + 1, "ov"))
					MAIN_gst_Command.b_ReplaceSource = true;
				break;

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			case 'p':
			case 'P':
				if(!stricmp(szArg + 1, "ps2") || !stricmp(szArg + 1, "playstation2"))
					MAIN_gst_Command.e_Format = FMT_e_PS2;
				else if(!stricmp(szArg + 1, "pc"))
					MAIN_gst_Command.e_Format = FMT_e_PCRETAIL;
				break;

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			case 'q':
			case 'Q':
				if(!stricmp(szArg + 1, "quiet") || !stricmp(szArg + 1, "q")) MAIN_gst_Command.b_Silent = true;
				break;

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			case 's':
			case 'S':
				if(!stricmp(szArg + 1, "silent") || !stricmp(szArg + 1, "si")) MAIN_gst_Command.b_Silent = true;
				break;

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			case 'x':
			case 'X':
				if(!stricmp(szArg + 1, "xbox") || !stricmp(szArg + 1, "xb")) MAIN_gst_Command.e_Format = FMT_e_XBOX;
				break;

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			default:
				printf("Unrecognized option %c%c.\n", szArg[0], szArg[1]);
				return;
			}
		}
		else if(!strlen(MAIN_gst_Command.asz_Input))
			strcpy(MAIN_gst_Command.asz_Input, szArg);
		else if(!strlen(MAIN_gst_Command.asz_Output))
			strcpy(MAIN_gst_Command.asz_Output, szArg);
	}

	if(MAIN_gst_Command.b_ReplaceSource) strcpy(MAIN_gst_Command.asz_Output, MAIN_gst_Command.asz_Input);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MessageUsage(void)
{
#ifdef DEBUG
	printf("\n\nCONVERT " M_VERSION "d\n");
#else
	printf("\n\nCONVERT " M_VERSION "r\n");
#endif
	printf("\n\nCONVERT [/Q|SILENT] [/FILE] [/CC] [/PS2|GC|XB|PC] [/DLL(<path>)] [/AM] [/OV] <in> <out>\n");
	printf("\n");
	printf("\t/FI\t\tSpecify that input and output point to a file\n\t\t\t(folder path by default).\n");
	printf("\t/CC\t\tCopy all chunks to encoded file.\n");
	printf("\t/PS2|GC|XB|PC\tConvert to Playstaion2 or Gamecube or xbox or MS ADPCM\n\t\t\t(choose only one format).\n");
	printf("\t/DLL(<path>)\tSpecify the folder for dll.\n");
	printf("\t/AM\t\tAlign all markers.\n");
	printf("\t/OV\t\tOverwrite the input (output = input).\n");
	printf("\t/Q or SILENT\t\tSilent mode.\n");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int CheckCommand(void)
{
    if(MAIN_gst_Command.b_Silent)
    {
	    switch(MAIN_gst_Command.e_Format)
	    {
	    case FMT_e_PS2:
	    case FMT_e_GC:
	    case FMT_e_XBOX:
	    case FMT_e_PCRETAIL:
	    case FMT_e_Native:
            return 0;
	    default:
		    return -1;
	    }
    }
    else
    {
	    switch(MAIN_gst_Command.e_Format)
	    {
	    case FMT_e_PS2:
		    printf("Playstation2 codec.");
		    break;

	    case FMT_e_GC:
		    printf("Gamecube codec.");
		    break;

	    case FMT_e_XBOX:
		    printf("Xbox codec.");
		    break;

	    case FMT_e_PCRETAIL:
		    printf("PC (MS ADPCM) codec.");
		    break;

	    case FMT_e_Native:
		    printf("No convertion.");
		    break;

	    default:
		    printf("Unknown codec.");
		    return -1;
	    }

	    return 0;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void mon_main(int argc, char *argv[])
{
	/*~~~~~~~~~~~~~~*/
	DWORD	TickCount; 
	/*~~~~~~~~~~~~~~*/

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 -------------------------------------------------------------------------------------------------------------------
	 */

	Init();

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 -------------------------------------------------------------------------------------------------------------------
	 */

	ParseCommandLine(argc, argv);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Check to make sure we had valid arguments
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(!strlen(MAIN_gst_Command.asz_Input) || !strlen(MAIN_gst_Command.asz_Output))
	{
		MessageUsage();
		return;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(CheckCommand()) return;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(MAIN_gst_Command.b_OnlyOneFileTargeted)
	{
        if(!MAIN_gst_Command.b_Silent) 
            printf("Converting one wave file from %s to %s...\n", MAIN_gst_Command.asz_Input, MAIN_gst_Command.asz_Output);
		TickCount = GetTickCount();

		ConvertOneFile(MAIN_gst_Command.asz_Input, MAIN_gst_Command.asz_Output);

		TickCount = GetTickCount() - TickCount;
		if(!MAIN_gst_Command.b_Silent) 
            printf("Time = %.3f\n", (float) TickCount / 1000.0f);
	}
	else
	{
		if(!MAIN_gst_Command.b_Silent)
            printf
		(
			"Converting wave files from directory %s to directory %s...\n",
			MAIN_gst_Command.asz_Input,
			MAIN_gst_Command.asz_Output
		);
		TickCount = GetTickCount();

		if(MAIN_gst_Command.asz_Input[strlen(MAIN_gst_Command.asz_Input - 1)] != '\\')
			strcat(MAIN_gst_Command.asz_Input, "\\");
		if(MAIN_gst_Command.asz_Output[strlen(MAIN_gst_Command.asz_Output - 1)] != '\\')
			strcat(MAIN_gst_Command.asz_Output, "\\");

		CreateDirectory(MAIN_gst_Command.asz_Output, NULL);
		vScanADirectory(MAIN_gst_Command.asz_Input, MAIN_gst_Command.asz_Output);

		TickCount = GetTickCount() - TickCount;
		if(!MAIN_gst_Command.b_Silent)
            printf("Time = %.3f\n", (float) TickCount / 1000.0f);
	}
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
