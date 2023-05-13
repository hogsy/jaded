/*$T AI2C_dll.c GC! 1.081 02/12/03 14:35:13 */


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
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BASe/MEMory/MEM.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define AI2C_Cte_DllMaxNb	(100)
#define AI2C_Cte_ConsoleId	(2)

/*$2- name in bigfile ------------------------------------------------------------------------------------------------*/

#define AI2C_Csz_FixMdlListName			"Fix Models.mdl"
#define AI2C_Csz_FixMdlListPath			"ROOT/EngineDatas/04 Technical Bank/Model Settings"

#define AI2C_Csz_DisableCMdlListName	"Disable C code.mdl"
#define AI2C_Csz_DisableCMdlListPath	"ROOT/EngineDatas/04 Technical Bank/Model Settings"

/*$2- name of generated source files ---------------------------------------------------------------------------------*/

#define AI2C_Csz_FixSourceName	"AI2C_FixFct.cpp"
#define AI2C_Csz_XRefSourceName "AI2C_CrossReference.c"
#define AI2C_Csz_DllSourceName	"AIdll%03d.cpp"

/*$2- name of build exec files ---------------------------------------------------------------------------------------*/

#ifdef _DEBUG
#define AI2C_Csz_StringFileName "/rel-debug/jadegc_ia2cd.str"
#define AI2C_Csz_DllFileName	"/rel-debug/AIgc_dll%03dd.rel"
#else
#define AI2C_Csz_StringFileName "/rel-release/jadegc_ia2cr.str"
#define AI2C_Csz_DllFileName	"/rel-release/AIgc_dll%03dr.rel"
#endif
#define AI2C_Csz_LoadedModelList	"jade.mdl"

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

void					AI2C_AddModelInDllContents(void);
void					AI2C_LoadFixModelList(void);
void					AI2C_MakeMapFixList(void);
void					AI2C_UnloadFixModelList(void);
int						AI2C_MakeModelList(int);
int						AI2C_UpdateModelCrossReference(void);
int						AI2C_FindNextEmptyRef(void);
static BOOL				AI2C_b_ModelIsFix(ULONG ul_key);

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern BOOL				EDI_gb_ComputeMap;
extern int				WOR_gi_CurrentConsole;
extern unsigned int		WOR_gul_WorldKey;

/*$2------------------------------------------------------------------------------------------------------------------*/

static BAS_tdst_barray	AI2C_gst_FixModelsList;
static BOOL				AI2C_gb_FixModelsListLoaded = FALSE;

static FILE				*AI2C_gp_TempFileMdl = NULL;
static FILE				*AI2C_gp_TempFileFct = NULL;

/*$2------------------------------------------------------------------------------------------------------------------*/

static BAS_tdst_barray	AI2C_gst_ModelsList;
static BOOL				AI2C_gb_ModelsListLoaded = FALSE;
static BOOL				AI2C_gb_ModelsListFileExists = FALSE;
ULONG					AI2C_gul_MinInstanceNbForGeneration = 0;

/*$2------------------------------------------------------------------------------------------------------------------*/

static BAS_tdst_barray	AI2C_gst_DisableCModelsList;
static BOOL				AI2C_gb_DisableCModelsListLoaded = FALSE;

/*$4
 ***********************************************************************************************************************
    EDITOR function bodies
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_LoadDisableCModelList(void)
{
	/*~~~~~~~~~~~~*/
	ULONG	ul_fat;
	ULONG	ul_size;
	char	*pc;
	/*~~~~~~~~~~~~*/

	if(AI2C_gb_DisableCModelsListLoaded) return;

	ul_fat = BIG_ul_SearchFileExt(AI2C_Csz_DisableCMdlListPath, AI2C_Csz_DisableCMdlListName);
	AI2C_gb_DisableCModelsListLoaded = FALSE;

	if(ul_fat != BIG_C_InvalidIndex)
	{
		pc = BIG_pc_ReadFileTmp(BIG_PosFile(ul_fat), &ul_size);
		ul_size = ul_size / 8;
		BAS_binit(&AI2C_gst_DisableCModelsList, 100);
		while(ul_size)
		{
			BAS_binsert(*(ULONG *) pc, 1, &AI2C_gst_DisableCModelsList);
			pc += 8;
			ul_size -= 1;
		}

		AI2C_gb_DisableCModelsListLoaded = TRUE;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_UnloadDisableCModelList(void)
{
	if(AI2C_gb_DisableCModelsListLoaded) BAS_bfree(&AI2C_gst_DisableCModelsList);
}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    loaded models during preprocess
 -----------------------------------------------------------------------------------------------------------------------
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI2C_i_LoadListModel(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	ULONG	ulkey, ulval;
	FILE	*f;
	/*~~~~~~~~~~~~~~~~~*/

	if(!AI2C_gb_ModelsListLoaded)
	{
		AI2C_gb_ModelsListLoaded = TRUE;
		BAS_binit(&AI2C_gst_ModelsList, 100);

		if(_access(AI2C_Csz_LoadedModelList, 2)) _chmod(AI2C_Csz_LoadedModelList, _S_IWRITE);
		f = fopen(AI2C_Csz_LoadedModelList, "r+t");
		if(f)
		{
			AI2C_gb_ModelsListFileExists = TRUE;
			fseek(f, 0, SEEK_SET);
			while(!feof(f))
			{
				fscanf(f, "%08x %08x\n", &ulkey, &ulval);
				BAS_binsert(ulkey, ulval, &AI2C_gst_ModelsList);
			}

			fclose(f);
		}
		else
		{
			AI2C_gb_ModelsListFileExists = FALSE;
		}
	}

	return(AI2C_gb_ModelsListFileExists ? 1 : 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_UnloadListModel(void)
{
	if(AI2C_gb_ModelsListLoaded) BAS_bfree(&AI2C_gst_ModelsList);
	AI2C_gb_ModelsListLoaded = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI2C_i_C_CodeGenerationIsEnabled(ULONG key)
{
	if(AI2C_gb_DisableCModelsListLoaded)
	{
		if(BAS_bsearch(key, &AI2C_gst_DisableCModelsList) != -1) return 0;
	}

	key = BAS_bsearch(key, &AI2C_gst_ModelsList);

	if(key == -1) return 0;
	if(key >= AI2C_gul_MinInstanceNbForGeneration) return 1;

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_RegisterCurrentModel(void)
{
	/*~~~~~~~~~~*/
	ULONG	ulval;
	/*~~~~~~~~~~*/

	/*$1- register models ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI2C_i_LoadListModel();

	ulval = BAS_bsearch(LOA_ul_GetCurrentKey(), &AI2C_gst_ModelsList);

	if(ulval == -1)
		BAS_binsert(LOA_ul_GetCurrentKey(), 1, &AI2C_gst_ModelsList);
	else
		BAS_binsert(LOA_ul_GetCurrentKey(), ulval + 1, &AI2C_gst_ModelsList);

	/*$1- GC Dll processing... ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI2C_AddModelInDllContents();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_SaveCurrentModelsList(void)
{
	/*~~~~~~~*/
	int		i;
	FILE	*f;
	/*~~~~~~~*/

    do{ f = fopen(AI2C_Csz_LoadedModelList, "wt"); } while(!f);

	if(f)
	{
		for(i = 0; i < AI2C_gst_ModelsList.num; i++)
		{
			fprintf(f, "%08x %08x\n", AI2C_gst_ModelsList.base[i].ul_Key, AI2C_gst_ModelsList.base[i].ul_Val);
		}

		fclose(f);
	}

	AI2C_UnloadListModel();
}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    DLL for Gamecube
 -----------------------------------------------------------------------------------------------------------------------
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
//#define DLL_DEBUG

BAS_tdst_barray st_Mdl;
BAS_tdst_barray st_Fct;
int gi_init=0;

// Return TRUE if model is added (the first time).
BOOL AI2C_bInsertModel(ULONG _ulFat)
{
	if(BAS_bsearch(_ulFat, &st_Mdl) == -1)
	{
    	char	asz_Ext[1024];
		BAS_binsert(_ulFat, _ulFat, &st_Mdl) ;

		L_strcpy(asz_Ext, BIG_NameFile(_ulFat));
        *L_strrchr(asz_Ext, '.') = 0;

		if (strcmp(asz_Ext,"Global Library"))
        {
		    fprintf(AI2C_gp_TempFileMdl, "%s\n", asz_Ext);
#ifdef DLL_DEBUG
            {
                unsigned char sMsg[256];
                sprintf(sMsg,"Add Model : %s\n",asz_Ext);
                OutputDebugString(sMsg);
            }
#endif //DLL_DEBUG
        }
        return TRUE;
    }
    return FALSE;
}

void AI2C_vInsertFunctionName(const char *_sFunctionName)
{
    fprintf(AI2C_gp_TempFileFct, "%s\n", _sFunctionName);
#ifdef DLL_DEBUG
    {
        unsigned char sMsg[256];
        sprintf(sMsg,"Add Function : %s\n",_sFunctionName);
        OutputDebugString(sMsg);
    }
#endif //DLL_DEBUG
}

void AI2C_vInsertFunction(ULONG _ulFat)
{
	if(BAS_bsearch(_ulFat, &st_Fct) == -1)
	{
        char asz_Ext[1024];
        char	*ppp;
		BAS_binsert(_ulFat, _ulFat, &st_Fct) ;
        L_strcpy(asz_Ext, BIG_NameFile(_ulFat));

        *L_strrchr(asz_Ext, '.') = 0;
        ppp = asz_Ext;
        while(*ppp)
        {
            if(!L_isalnum(*ppp) && (*ppp != '_')) *ppp = '_';
            if(*ppp == '°') *ppp = '_';
            if(*ppp == 'é') *ppp = '_';
            if(*ppp == 'è') *ppp = '_';
            if(*ppp == 'à') *ppp = '_';
            if(*ppp == 'ä') *ppp = '_';
            if(*ppp == 'â') *ppp = '_';
            if(*ppp == 'ë') *ppp = '_';
            if(*ppp == 'ê') *ppp = '_';
            ppp++;
        }

        AI2C_vInsertFunctionName(asz_Ext);
    }
}

void AI2C_vInsertFCLFunctions(ULONG _ulFat)
{
    // We parse the file 
    ULONG	size_fcl;
    ULONG	ofc_fat_fcl;
    char *buffer_fcl,*buffer_fcl_begin;
    char *comment,*procedureName;
    char *parenthesis;
    char asz_Dir[1024],asz_TriggerName[1024],asz_ReturnValue[1024];
    int read_values;
    char asz_Ext[1024];

    // Get buffer that contains the fcl file.
    L_strcpy(asz_Ext, BIG_NameFile(_ulFat));
    BIG_ComputeFullName(BIG_ParentFile(_ulFat),asz_Dir);
    asz_Ext[strlen(asz_Ext)-1] = 'l'; // .fce -> .fcl
    ofc_fat_fcl = BIG_ul_SearchFileExt(asz_Dir,asz_Ext);

#ifdef DLL_DEBUG
    {
        unsigned char sMsg[256];
        sprintf(sMsg,"Insert fcl functions of %s/%s \n",asz_Dir,asz_Ext);
        OutputDebugString(sMsg);
    }
#endif //DLL_DEBUG

    buffer_fcl_begin = buffer_fcl = BIG_pc_ReadFileTmpMustFree(BIG_PosFile(ofc_fat_fcl),&size_fcl);

    // parse the fcl file.
    procedureName = L_strstr(buffer_fcl,"procedure_trigger");
    comment = L_strstr(buffer_fcl,"//");
    while (procedureName != NULL)
    {
        // Found a comment before "procedure_trigger"
        if (comment && (comment < procedureName))
        {
            // Eat comment line (until end of line).
            buffer_fcl = L_strstr(comment,"\n") + 1;
        }
        else
        {
            // Get the trigger name.
            buffer_fcl = procedureName + L_strlen("procedure_trigger");
            read_values = sscanf(buffer_fcl,"%s %s",asz_ReturnValue,asz_TriggerName); 
            // read_values should be 2.

            // Get rid of '(' char if necessary
            parenthesis = L_strchr(asz_TriggerName,'(');
            if (parenthesis != NULL)
                *parenthesis = '\0';

            AI2C_vInsertFunctionName(asz_TriggerName);
        }
        procedureName = L_strstr(buffer_fcl,"procedure_trigger");
        comment = L_strstr(buffer_fcl,"//");
    }
    L_free(buffer_fcl_begin);
}

void AI2C_AddModelInDllContents(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	ULONG	key, fat, size;
	ULONG	ofc_fat;
	char	*pc,*pc_begin;
	/*~~~~~~~~~~~~~~~~~~~*/

	/*$2- activation condition = preprocessing for GC ----------------------------------------------------------------*/

	if(WOR_gi_CurrentConsole != AI2C_Cte_ConsoleId) return;
	if(!EDI_gb_ComputeMap) return;

	/*$2- load fix model list ----------------------------------------------------------------------------------------*/

	AI2C_LoadFixModelList();

	if(!gi_init)
	{
	    BAS_binit(&st_Mdl, 10);
	    BAS_binit(&st_Fct, 10);
	    gi_init ++;
	}

	/*$2- create a temp file -----------------------------------------------------------------------------------------*/

	if(!AI2C_gp_TempFileMdl) AI2C_gp_TempFileMdl = tmpfile();
	if(!AI2C_gp_TempFileFct) AI2C_gp_TempFileFct = tmpfile();

	/*$2- add all model functions into the map list ------------------------------------------------------------------*/

	if(AI2C_gp_TempFileMdl && AI2C_gp_TempFileFct)
	{
		key = LOA_ul_GetCurrentKey();
		if(AI2C_b_ModelIsFix(key)) return;

#ifdef DLL_DEBUG
        {
            unsigned char sMsg[256];
            sprintf(sMsg,"\nNew mdl with key %x\n",key);
            OutputDebugString(sMsg);
        }
#endif //DLL_DEBUG

		pc_begin = pc = BIG_pc_ReadFileTmpMustFree(BIG_ul_SearchKeyToPos(key), &size);
		fat = BIG_ul_SearchKeyToFat(key);

        if (AI2C_bInsertModel(fat))
        {
            // Insert init function of mdl (only first time that mdl is used)
            AI2C_vInsertFunction(fat);
        }

		size = size / 8;

		// parse all files of mdl
		while(size)
		{
			if(!L_strnicmp(pc + 4, ".ofc", 4) || !L_strnicmp(pc + 4, ".fce", 4))
			{
				ofc_fat = BIG_ul_SearchKeyToFat(*(ULONG *) pc);
				if(ofc_fat && (ofc_fat != -1))
				{
					if(!L_strnicmp(pc + 4, ".fce", 4))
					{
                        // case fce : add the cpp file of the fce (if not already added)
                        if (AI2C_bInsertModel(ofc_fat))
                        {
                            // Add the functions inside the fce (parse the fcl file which is the source file for the fce).
                            AI2C_vInsertFCLFunctions(ofc_fat);

                            // Insert init function of fcl (only first time that mdl is used)
                            AI2C_vInsertFunction(ofc_fat);
                        }
					}
					else
					{   
                        // case ofc : add the function (if not already added)
                        AI2C_vInsertFunction(ofc_fat);
					}
				}
			}

			/* update read ptr */
			size--;
			pc += 8;
		}

		L_free(pc_begin);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_GenDllSourceFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	int			id;
	char		asz_Path[1024];
	char		asz_Fct[1024];
	FILE		*f;
	time_t		now;
	struct tm	when;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(WOR_gi_CurrentConsole != AI2C_Cte_ConsoleId) return;
	if(!EDI_gb_ComputeMap) return;

	if(AI2C_gp_TempFileMdl && AI2C_gp_TempFileFct)
	{
		/* check temp file is not empty */
		if(ftell(AI2C_gp_TempFileMdl) > 2 && ftell(AI2C_gp_TempFileFct) > 2)
		{
			/* get dll id */
			id = AI2C_UpdateModelCrossReference();
			snprintf( asz_Path, sizeof(asz_Path), AI2C_Csz_DllSourceName, id );

			/* generate the file */
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
				fprintf(f, "*/\n\n");
				fprintf(f, "#define GAMECUBE_DLL_BODY\n");
				fprintf(f, "\n");
				fprintf(f, "#include \"Precomp.h\"\n");
				fprintf(f, "#include \"AI2C_fctheader.h\"\n");
				fprintf(f, "#include \"AI2C_mdlheader.h\"\n");
				fprintf(f, "\n");
				fprintf(f, "#ifdef GAMECUBE_USE_AI2C_DLL\n\n");
				fprintf(f, "static void AI2C_dll%03d_Open(void);\n", id);
				fprintf(f, "static void AI2C_dll%03d_Close(void);\n", id);
				fprintf(f, "\n");
				fprintf(f,"// _prolog  and _epilog must have a C signature, else they won't be seen by OSLink.\n");
				fprintf(f,"extern \"C\" {\n");
				fprintf(f, "\n");
				fprintf(f, "/*\n");
				fprintf
				(
					f,
					" =======================================================================================================================\n"
				);
				fprintf
				(
					f,
					" =======================================================================================================================\n"
				);
				fprintf(f, "*/\n");
				fprintf(f, "void _prolog(void)\n");
				fprintf(f, "{\n");
				fprintf(f, "\tvoidfunctionptr *constructor;\n\n");
				fprintf(f, "\tfor(constructor = _ctors; *constructor; constructor++)\n");
				fprintf(f, "\t{\n");
				fprintf(f, "\t\t(*constructor) ();\n");
				fprintf(f, "\t}\n");
				fprintf(f, "\tAI2C_dll%03d_Open();\n", id);
				fprintf(f, "}\n");
				fprintf(f, "\n");
				fprintf(f, "/*\n");
				fprintf
				(
					f,
					" =======================================================================================================================\n"
				);
				fprintf
				(
					f,
					" =======================================================================================================================\n"
				);
				fprintf(f, "*/\n");
				fprintf(f, "void _epilog(void)\n");
				fprintf(f, "{\n");
				fprintf(f, "\tvoidfunctionptr *destructor;\n");
				fprintf(f, "\n");
				fprintf(f, "\tfor(destructor = _dtors; *destructor; destructor++)\n");
				fprintf(f, "\t{\n");
				fprintf(f, "\t\t(*destructor) ();\n");
				fprintf(f, "\t}\n");
				fprintf(f, "\tAI2C_dll%03d_Close();\n", id);
				fprintf(f, "}\n");
				fprintf(f, "\n");
				fprintf(f, "/*\n");
				fprintf
				(
					f,
					" =======================================================================================================================\n"
				);
				fprintf
				(
					f,
					" =======================================================================================================================\n"
				);
				fprintf(f, "*/\n");
				fprintf(f, "static void AI2C_dll%03d_Open(void)\n", id);
				fprintf(f, "{\n");
				fseek(AI2C_gp_TempFileFct, 0, SEEK_SET);
				while(!feof(AI2C_gp_TempFileFct))
				{
					if(fscanf(AI2C_gp_TempFileFct, "%s", asz_Fct) != -1)
						fprintf(f, "\tAI2C_pfi_%s = %s;\n", asz_Fct, asz_Fct);
				}

				fprintf(f, "}\n");
				fprintf(f, "\n");
				fprintf(f, "/*\n");
				fprintf
				(
					f,
					" =======================================================================================================================\n"
				);
				fprintf
				(
					f,
					" =======================================================================================================================\n"
				);
				fprintf(f, "*/\n");
				fprintf(f, "static void AI2C_dll%03d_Close(void)\n", id);
				fprintf(f, "{\n");
				fseek(AI2C_gp_TempFileFct, 0, SEEK_SET);
				while(!feof(AI2C_gp_TempFileFct))
				{
					if(fscanf(AI2C_gp_TempFileFct, "%s", asz_Fct) != -1) fprintf(f, "\tAI2C_pfi_%s = NULL;\n", asz_Fct);
				}

				fprintf(f, "}\n");
				fprintf(f, "\n");

				fprintf(f, "} // extern \"C\"\n");
				fprintf(f, "\n");
				
				fprintf(f, "/*\n");
				fprintf
				(
					f,
					" ***********************************************************************************************************************\n"
				);
				fprintf(f, "    AI functions includes\n");
				fprintf
				(
					f,
					" ***********************************************************************************************************************\n"
				);
				fprintf(f, "*/\n");

                // Include .h
				fseek(AI2C_gp_TempFileMdl, 0, SEEK_SET);
				while(!feof(AI2C_gp_TempFileMdl))
				{
					if(fscanf(AI2C_gp_TempFileMdl, "%s", asz_Fct) != -1)
						fprintf(f, "#include \"AI2C_ofc/%s.h\"\n", asz_Fct);
				}

				fprintf(f, "\n");

                // Include .cpp
				fseek(AI2C_gp_TempFileMdl, 0, SEEK_SET);
				while(!feof(AI2C_gp_TempFileMdl))
				{
					if(fscanf(AI2C_gp_TempFileMdl, "%s", asz_Fct) != -1)
						fprintf(f, "#include \"AI2C_ofc/%s.cpp\"\n", asz_Fct);
				}

				fprintf(f, "\n");
				fprintf(f, "\n");

				fprintf(f, "#endif /* GAMECUBE_USE_AI2C_DLL */\n");

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
	}

	_rmtmp();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_LoadFixModelList(void)
{
	/*~~~~~~~~~~~~*/
	ULONG	ul_fat;
	ULONG	ul_size;
	char	*pc;
	/*~~~~~~~~~~~~*/

	if(AI2C_gb_FixModelsListLoaded) return;

	ul_fat = BIG_ul_SearchFileExt(AI2C_Csz_FixMdlListPath, AI2C_Csz_FixMdlListName);
	AI2C_gb_FixModelsListLoaded = FALSE;

	if(ul_fat != BIG_C_InvalidIndex)
	{
		pc = BIG_pc_ReadFileTmp(BIG_PosFile(ul_fat), &ul_size);
		ul_size = ul_size / 8;
		BAS_binit(&AI2C_gst_FixModelsList, 100);
		while(ul_size)
		{
			BAS_binsert(*(ULONG *) pc, 1, &AI2C_gst_FixModelsList);
			pc += 8;
			ul_size -= 1;
		}

		AI2C_gb_FixModelsListLoaded = TRUE;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_UnloadFixModelList(void)
{
	if(AI2C_gb_FixModelsListLoaded) BAS_bfree(&AI2C_gst_FixModelsList);
	AI2C_gb_FixModelsListLoaded = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static BOOL AI2C_b_ModelIsFix(ULONG ul_key)
{
	if(!AI2C_gb_FixModelsListLoaded) return FALSE;
	if(BAS_bsearch(ul_key, &AI2C_gst_FixModelsList) == -1) return FALSE;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void AI2C_MakeMapFixList(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	FILE		*f;
	ULONG		ul, ull;
	char		*pc;
	ULONG		lll;
	char		*ppp;
	int			i;
	char		asz_Path[1024];
	char		asz_Ext[1024];
	time_t		now;
	struct tm	when;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	strcpy(asz_Path, AI2C_Csz_FixSourceName);

	f = fopen(asz_Path, "wt");
	if(f)
	{
		BAS_tdst_barray st_IncludedFiles;
		BAS_binit(&st_IncludedFiles, 10);

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
		fprintf(f, "#include \"MATHs/MATHvector_aligned.h\"\n");
		fprintf(f, "#include \"AI2C_fctheader.h\"\n\n");
		fprintf(f, "#include \"AI2C_mdlheader.h\"\n\n");
		fprintf(f, "#ifdef GAMECUBE_USE_AI2C_DLL\n\n");

		fprintf(f, "#define AI2C_FCTDEFTRIGGER(a, b, c) \n");
		fprintf(f, "#define AI2C_FCTDEF(a, b)	extern int (*AI2C_pfi_##b) (void);\n");
		fprintf(f, "#include \"AI2C_fctdefs.h\"\n");
		fprintf(f, "#undef AI2C_FCTDEF\n\n");
		fprintf(f, "#undef AI2C_FCTDEFTRIGGER\n\n");

		fprintf(f, "#define AI2C_FCTDEFTRIGGER(a, b, c) \n");
		fprintf(f, "#define AI2C_FCTDEF(a, b)	extern int	  b(void);\n");
		fprintf(f, "#include \"AI2C_fctdefs.h\"\n");
		fprintf(f, "#undef AI2C_FCTDEF\n\n\n");
		fprintf(f, "#undef AI2C_FCTDEFTRIGGER\n\n");

		fprintf(f, "/*\n");
		fprintf
		(
			f,
			" ***********************************************************************************************************************\n"
		);
		fprintf(f, "    AI functions includes\n");
		fprintf
		(
			f,
			" ***********************************************************************************************************************\n"
		);
		fprintf(f, "*/\n");
		for(i = 0; i < AI2C_gst_FixModelsList.num; i++)
		{
			ul = AI2C_gst_FixModelsList.base[i].ul_Key;

			pc = BIG_pc_ReadFileTmp(BIG_ul_SearchKeyToPos(ul), &lll);
			ul = BIG_ul_SearchKeyToFat(ul);

			if(BIG_b_IsFileExtension(ul, ".omd"))
			{
				lll = lll / 8;
				while(lll)
				{
					if(!L_strnicmp(pc + 4, ".ofc", 4))
					{
						ull = BIG_ul_SearchKeyToFat(*(ULONG *) pc);
						if(ull && (ull != -1))
						{
							strcpy(asz_Ext, BIG_NameDir(BIG_ParentFile(ull)));
							// If file has been treated, don't do it again.
							if(BAS_bsearch(ul, &st_IncludedFiles) == -1)
							{
								BAS_binsert(ul, ul, &st_IncludedFiles) ;
								if(L_strcmp(asz_Ext, "Global Library"))
								{
									strcat(asz_Ext, ".cpp");
									fprintf(f, "#include \"AI2C_ofc/%s\"\n", asz_Ext);
								}
							}
						}
					}

					lll--;
					pc += 8;
				}
			}
			else
			{
				strcpy(asz_Ext, BIG_NameDir(BIG_ParentFile(ul)));
				strcat(asz_Ext, "_");
				strcat(asz_Ext, BIG_NameFile(ul));
				*L_strrchr(asz_Ext, '.') = 0;
				ppp = asz_Ext;
				while(*ppp)
				{
					if(!L_isalnum(*ppp) && (*ppp != '_')) *ppp = '_';
					if(*ppp == '°') *ppp = '_';
					if(*ppp == 'é') *ppp = '_';
					if(*ppp == 'è') *ppp = '_';
					if(*ppp == 'à') *ppp = '_';
					if(*ppp == 'ä') *ppp = '_';
					if(*ppp == 'â') *ppp = '_';
					if(*ppp == 'ë') *ppp = '_';
					if(*ppp == 'ê') *ppp = '_';
					ppp++;
				}

				strcat(asz_Ext, ".c");
				fprintf(f, "#include \"AI2C_ofc/%s\"\n", asz_Ext);
			}
		}

		fprintf(f, "\n");
		fprintf(f, "/*\n");
		fprintf
		(
			f,
			" =======================================================================================================================\n"
		);
		fprintf
		(
			f,
			" =======================================================================================================================\n"
		);
		fprintf(f, "*/\n\n");
		fprintf(f, "extern \"C\"\n{\n\n");
		fprintf(f, "void AI2C_InitFix(void)\n");
		fprintf(f, "{\n");
		for(i = 0; i < AI2C_gst_FixModelsList.num; i++)
		{
			int bHasWritenInitFunction = 0;
			ul = AI2C_gst_FixModelsList.base[i].ul_Key;

			pc = BIG_pc_ReadFileTmp(BIG_ul_SearchKeyToPos(ul), &lll);
			ul = BIG_ul_SearchKeyToFat(ul);

			if(BIG_b_IsFileExtension(ul, ".omd"))
			{
				lll = lll / 8;
				while(lll)
				{
					if(!L_strnicmp(pc + 4, ".ofc", 4))
					{
						ull = BIG_ul_SearchKeyToFat(*(ULONG *) pc);
						if(ull && (ull != -1))
						{
							// assign model init function
							strcpy(asz_Ext, BIG_NameDir(BIG_ParentFile(ull)));
							if(L_strcmp(asz_Ext, "Global Library"))
							{
								if (!bHasWritenInitFunction)
								{
									bHasWritenInitFunction = 1;
									ppp = asz_Ext;
									while(*ppp)
									{
										if(!L_isalnum(*ppp) && (*ppp != '_')) *ppp = '_';
										if(*ppp == '°') *ppp = '_';
										if(*ppp == 'é') *ppp = '_';
										if(*ppp == 'è') *ppp = '_';
										if(*ppp == 'à') *ppp = '_';
										if(*ppp == 'ä') *ppp = '_';
										if(*ppp == 'â') *ppp = '_';
										if(*ppp == 'ë') *ppp = '_';
										if(*ppp == 'ê') *ppp = '_';
										ppp++;
									}

									fprintf(f, "\tAI2C_pfi_%s = %s;\n", asz_Ext, asz_Ext);
								}

								// assign model functions
								strcpy(asz_Ext, BIG_NameFile(ull));
								*L_strrchr(asz_Ext, '.') = 0;
								ppp = asz_Ext;
								while(*ppp)
								{
									if(!L_isalnum(*ppp) && (*ppp != '_')) *ppp = '_';
									if(*ppp == '°') *ppp = '_';
									if(*ppp == 'é') *ppp = '_';
									if(*ppp == 'è') *ppp = '_';
									if(*ppp == 'à') *ppp = '_';
									if(*ppp == 'ä') *ppp = '_';
									if(*ppp == 'â') *ppp = '_';
									if(*ppp == 'ë') *ppp = '_';
									if(*ppp == 'ê') *ppp = '_';
									ppp++;
								}

								fprintf(f, "\tAI2C_pfi_%s = %s;\n", asz_Ext, asz_Ext);
							}
						}
					}

					lll--;
					pc += 8;
				}
			}
			else
			{
				strcpy(asz_Ext, BIG_NameDir(BIG_ParentFile(ul)));
				strcat(asz_Ext, "_");
				strcat(asz_Ext, BIG_NameFile(ul));
				*L_strrchr(asz_Ext, '.') = 0;
				ppp = asz_Ext;
				while(*ppp)
				{
					if(!L_isalnum(*ppp) && (*ppp != '_')) *ppp = '_';
					if(*ppp == '°') *ppp = '_';
					if(*ppp == 'é') *ppp = '_';
					if(*ppp == 'è') *ppp = '_';
					if(*ppp == 'à') *ppp = '_';
					if(*ppp == 'ä') *ppp = '_';
					if(*ppp == 'â') *ppp = '_';
					if(*ppp == 'ë') *ppp = '_';
					if(*ppp == 'ê') *ppp = '_';
					ppp++;
				}

				fprintf(f, "\tAI2C_pfi_%s = %s;\n", asz_Ext, asz_Ext);
			}
		}

		fprintf(f, "}\n\n");
		fprintf(f, "}// extern \"C\"\n\n");
		fprintf(f, "#endif /* GAMECUBE_USE_AI2C_DLL */\n");
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

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static char *sp_PadString(char *dst, char *src, int size)
{
	/*~~~~~~~*/
	char	*p;
	/*~~~~~~~*/

	p = dst;

	while(size && *src)
	{
		*dst++ = *src++;
		size--;
	}

	while(size)
	{
		*dst++ = '-';
		size--;
	}

	*dst = 0;
	return p;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI2C_UpdateModelCrossReference(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	FILE			*f, *x;
	char			line[1024];
	int				i, l, il;
	unsigned int	k;
	static int		updated = -1;
	time_t			now;
	struct tm		when;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(updated != -1) return updated;

	if(_access(AI2C_Csz_XRefSourceName, 2)) _chmod(AI2C_Csz_XRefSourceName, _S_IWRITE);
	f = fopen(AI2C_Csz_XRefSourceName, "r+t");

	if(f)
	{
		/* update Xref file */
		fseek(f, 0, SEEK_SET);
		while(!feof(f))
		{
			l = ftell(f);
			fscanf(f, "%s\n", line);
			if(!L_strncmp(line, "0x", 2)) break;
		}

		fseek(f, l, SEEK_SET);

		i = 0;
		for(i = 0; i < AI2C_Cte_DllMaxNb; i++)
		{
			l = ftell(f);
			fscanf(f, "0x%08x, /* #%03d - %s */\n", &k, &il, line);
			if(k == WOR_gul_WorldKey) break;
			if(!k) break;
		}

		if(k != WOR_gul_WorldKey)
		{
			fseek(f, l, SEEK_SET);
			sp_PadString(line, BIG_NameFile(BIG_ul_SearchKeyToFat(WOR_gul_WorldKey)), 128);
			fprintf(f, "0x%08x, /* #%03d - %s */\n", WOR_gul_WorldKey, il, line);
		}

		updated = i;
	}
	else
	{
		/* create all AIdllxxx.c */
		for(i = 0; i < AI2C_Cte_DllMaxNb; i++)
		{
			snprintf( line, sizeof(line), AI2C_Csz_DllSourceName, i );
			x = fopen(line, "wt");
			time(&now);
			when = *localtime(&now);
			fprintf(x, "/*\n");
			fprintf
			(
				x,
				" +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
			);
			fprintf(x, "    !! This file is generated by Jade editor !!\n");
			fprintf
			(
				x,
				"    %s : %d/%d/%d - %02d:%02d\n",
				line,
				1900 + when.tm_year,
				when.tm_mon,
				when.tm_mday,
				when.tm_hour,
				when.tm_min
			);
			fprintf
			(
				x,
				" +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
			);
			fprintf(x, "*/\n\n");
			fprintf(x, "#include \"Precomp.h\"\n");
			fprintf(x, "#define GAMECUBE_DLL_BODY\n");
			fprintf(x, "#include \"AI2C_fctheader.h\"\n");
			fprintf(x, "\n");
			fprintf(x, "#ifdef GAMECUBE_USE_AI2C_DLL\n\n");
			fprintf(x, "\n");
			fprintf(x,"// _prolog  and _epilog must have a C signature, else they won't be seen by OSLink.\n");
			fprintf(x,"extern \"C\" {\n");
			fprintf(x, "\n");
			fprintf(x, "/*\n");
			fprintf
			(
				x,
				" =======================================================================================================================\n"
			);
			fprintf
			(
				x,
				" =======================================================================================================================\n"
			);

			fprintf(x, "*/\n");
			fprintf(x, "void _prolog(void)\n");
			fprintf(x, "{\n");
			fprintf(x, "\tvoidfunctionptr *constructor;\n\n");
			fprintf(x, "\tfor(constructor = _ctors; *constructor; constructor++)\n");
			fprintf(x, "\t{\n");
			fprintf(x, "\t\t(*constructor) ();\n");
			fprintf(x, "\t}\n");
			fprintf(x, "}\n");
			fprintf(x, "\n");
			fprintf(x, "/*\n");
			fprintf
			(
				x,
				" =======================================================================================================================\n"
			);
			fprintf
			(
				x,
				" =======================================================================================================================\n"
			);
			fprintf(x, "*/\n");
			fprintf(x, "void _epilog(void)\n");
			fprintf(x, "{\n");
			fprintf(x, "\tvoidfunctionptr *destructor;\n");
			fprintf(x, "\n");
			fprintf(x, "\tfor(destructor = _dtors; *destructor; destructor++)\n");
			fprintf(x, "\t{\n");
			fprintf(x, "\t\t(*destructor) ();\n");
			fprintf(x, "\t}\n");
			fprintf(x, "}\n");
			fprintf(x, "\n");
			fprintf(x, "} // extern \"C\"\n");
			fprintf(x, "\n");
			fprintf(x, "#endif /* GAMECUBE_USE_AI2C_DLL */\n");
			fprintf(x, "/*\n");
			fprintf
			(
				x,
				" ***********************************************************************************************************************\n"
			);
			fprintf(x, "    EOF\n");
			fprintf
			(
				x,
				" ***********************************************************************************************************************\n"
			);
			fprintf(x, "*/\n");
			fclose(x);
		}

		/* create Xref file */
		f = fopen(AI2C_Csz_XRefSourceName, "wt");

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
			AI2C_Csz_XRefSourceName,
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
		fprintf(f, "#include \"AIinterp\\Sources\\AIstruct.h\"\n\n");
		fprintf(f, "#ifdef GAMECUBE_USE_AI2C_DLL\n");
		fprintf(f, "#include \"AIinterp\\Sources\\AI2C_dll\\AI2C_CrossReference.h\"\n\n");
		fprintf(f, "unsigned int AI2C_gx_CrossRef[]={\n");

		sp_PadString(line, BIG_NameFile(BIG_ul_SearchKeyToFat(WOR_gul_WorldKey)), 128);
		fprintf(f, "0x%08x, /* #000 - %s */\n", WOR_gul_WorldKey, line);

		sp_PadString(line, "none", 128);
		for(i = 1; i < AI2C_Cte_DllMaxNb - 1; i++)
		{
			fprintf(f, "0x00000000, /* #%03d - %s */\n", i, line);
		}

		fprintf(f, "0x00000000  /* #%03d - %s */\n", i, line);
		fprintf(f, "};\n");
		fprintf(f, "#endif\n");
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
		updated = 0;
	}

	fclose(f);

	return updated;
}

#endif

/*$4
 ***********************************************************************************************************************
    ENGINE
 ***********************************************************************************************************************
 */

#ifdef GAMECUBE_USE_AI2C_DLL

/*$2- headers --------------------------------------------------------------------------------------------------------*/

#include "AI2C_CrossReference.h"

/*$2- macros ---------------------------------------------------------------------------------------------------------*/

/*$2- proto ----------------------------------------------------------------------------------------------------------*/

void					AI2C_LoadDll(char *);

/*$2- variables ------------------------------------------------------------------------------------------------------*/

static void				*AI2C_gpv_bssptr = NULL;
static OSModuleHeader	*AI2C_gspst_DllHandler = NULL;

/*$2- functions ------------------------------------------------------------------------------------------------------*/

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_LoadAIForWorld(ULONG _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~*/
	int		i;
	char	asz_file[64];
	/*~~~~~~~~~~~~~~~~~*/

	if(!_ul_Key) return;
	if(_ul_Key == -1) return;

	for(i = 0; i < AI2C_Cte_DllMaxNb; i++)
	{
		if(AI2C_gx_CrossRef[i] == _ul_Key)
		{
			sprintf(asz_file, AI2C_Csz_DllFileName, i);
			AI2C_LoadDll(asz_file);
			return;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_UnloadDll(void)
{
	if(AI2C_gspst_DllHandler)
	{
		if(AI2C_gspst_DllHandler->epilog) ((u32(*) (void)) AI2C_gspst_DllHandler->epilog) ();

		OSUnlink(&AI2C_gspst_DllHandler->info);

		MEM_FreeAlign(AI2C_gpv_bssptr);
		AI2C_gpv_bssptr = NULL;
		MEM_FreeAlign(AI2C_gspst_DllHandler);
		AI2C_gspst_DllHandler = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_LoadDll(char *name)
{
	/*~~~~~~~~~~~~~~~~~*/
	u32			length;
	BOOL		result;
	tdstGC_File *pFile;
	/*~~~~~~~~~~~~~~~~~*/

#ifdef _DEBUG

	/*$2- load str file for debug symbols ----------------------------------------------------------------------------*/

	{
		/*~~~~~~~~~~~~~~~~~~~~*/
		DVDFileInfo fileInfo;
		static int	first = 1;
		void		*ptr;
		/*~~~~~~~~~~~~~~~~~~~~*/

		if(first)
		{
			first = 0;
			result = DVDOpen(AI2C_Csz_StringFileName, &fileInfo);
			if(result)
			{
				length = OSRoundUp32B(DVDGetLength(&fileInfo));
				ptr = MEM_p_AllocAlign(length, 32);
				result = DVDRead(&fileInfo, ptr, (s32) length, 0);
				if(!result)
				{
					MEM_FreeAlign(ptr);
				}
				else
				{
					OSSetStringTable(ptr);
				}
				DVDClose(&fileInfo);
			}
		}
	}

#endif

	/*$2- load the dll file into MRAM --------------------------------------------------------------------------------*/

	/* reset ptr */
	AI2C_gpv_bssptr = NULL;
	AI2C_gspst_DllHandler = NULL;

	/* read file */
	pFile = GC_fOpen(name, 0);
	if(!pFile) return;

	length = OSRoundUp32B(DVDGetLength(&pFile->stFileInfo));
	
	AI2C_gspst_DllHandler = (OSModuleHeader *) MEM_p_AllocAlign(length, 32);
	
	result = GC_fRead(pFile, AI2C_gspst_DllHandler, length);
	if(result != length)
	{
		GC_fClose(pFile);
		MEM_FreeAlign(AI2C_gspst_DllHandler);
		AI2C_gspst_DllHandler = NULL;
		return;
	}


	/* alloc space for bss section & do link */
	AI2C_gpv_bssptr = MEM_p_AllocAlign(AI2C_gspst_DllHandler->bssSize, 32); /* alloc bss area */
	OSLink(&AI2C_gspst_DllHandler->info, AI2C_gpv_bssptr);

	/* exec dll init */
	if(AI2C_gspst_DllHandler->prolog) ((u32(*) (void)) AI2C_gspst_DllHandler->prolog) ();
	
#ifdef _DEBUG
	OSReport("New Dll used : %s\n",name);
#endif // _DEBUG
	
	
	GC_fClose(pFile);
}

#else /* GAMECUBE_USE_AI2C_DLL */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_LoadAIForWorld(ULONG _ul_Key)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_UnloadDll(void)
{
}

#endif /* GAMECUBE_USE_AI2C_DLL */

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

