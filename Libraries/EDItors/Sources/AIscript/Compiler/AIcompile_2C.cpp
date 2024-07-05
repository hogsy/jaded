/*$T AIcompile_2C.cpp GC 1.139 04/13/04 11:49:01 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "LINKs/LINKmsg.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGread.h"
#include "../AIerrid.h"
#include "../AIframe.h"
#include "AIinterp/Sources/AIengine.h"
#include "EDImainframe.h"
#include "BASe/MEMory/MEM.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "EDIpaths.h"
#include "process.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

static char							*gpz_GlobalBuf = NULL;
static int							gpi_GlobalSize = 0;
static int							gpi_GlobalMaxSize = 0;
static FILE							*AI2C_gf_Model;
static int							AI2C_gi_FileNum = 0;
static int							AI2C_gi_GlInsert = 0;
static int							AI2C_gi_GlInsertMode = 0;
char								*AI_PATH = "../../../Libraries/AiInterp/Sources/";
static char							AI2C_gaz_InitFct[128];
static char							AI2C_gaz_GlobalRef[128];
static char							AI2C_gaz_GlobalName[512];

static CMapPtrToPtr					AI2C_go_MapRef;
static CMapPtrToPtr					AI2C_go_ProcListRef;
static CList<CString, CString>		AI2C_go_ProcUltraList;
static CList<BIG_INDEX, BIG_INDEX>	AI2C_go_IncludeRef;
static CList<BIG_INDEX, BIG_INDEX>	AI2C_go_LibIncludeRef;
CList<BIG_INDEX, BIG_INDEX>			AI2C_go_DoneFct;
static int							AI2C_gi_Label;
BAS_tdst_barray             AI2C_gst_FctList;
BOOL                        AI2C_gb_FctListInit = FALSE;


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define DEFINE_FUNCTION(a, b, c, d, e, f, g, h, i, j)	h,
char	*AI2C_gasz_Functions[] = {
#include "AIinterp/sources/Functions/AIdeffct.h"
};

#define DEFINE_FUNCTION(a, b, c, d, e, f, g, h, i, j)	c,
char	*AI2C_gasz_FunctionsInt[] = {
#include "AIinterp/sources/Functions/AIdeffct.h"
};

#define DEFINE_FUNCTION(a, b, c, d, e, f, g, h, i, j)	#j,
char	*AI2C_gasz_FunctionsCP[] = {
#include "AIinterp/sources/Functions/AIdeffct.h"
};

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_GenFctList(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	FILE	*f;
	char	path[1024];
	char	az_Tmp[128];
	int		i, j;
	char	*pz;
	int		cpt, max;
	char	cvec, cmsg, ctxt, cid;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_strcpy(path, AI_PATH);
	L_strcat(path, "AI2C_fctlist.h");
	if(_access(path, 2)) _chmod(path, _S_IWRITE);
	f = fopen(path, "w");
	if(!f) return;

	fputs("#define AI2C_DATE_GEN \"", f);
	fputs(_strdate(az_Tmp), f);
	fputs("\"\n", f);
	fputs("#define AI2C_TIME_GEN \"", f);
	fputs(_strtime(az_Tmp), f);
	fputs("\"\n", f);

	for(i = 0; i < sizeof(AI2C_gasz_Functions) / sizeof(void *); i++)
	{
		cvec = 0;
		cmsg = 0;
		ctxt = 0;
		cid = 0;

		if(EAI_gast_Functions[i].i_ReturnType == TYPE_VECTOR) cvec = 1;
		if(EAI_gast_Functions[i].i_ReturnType == TYPE_MESSAGE) cmsg = 1;
		if(EAI_gast_Functions[i].i_ReturnType == TYPE_TEXT) ctxt = 1;
		if(EAI_gast_Functions[i].i_ReturnType == TYPE_MESSAGEID) cid = 1;

		if(cvec || cmsg || ctxt || cid)
		{
			if(cvec)
				fputs("_inline_ vector ", f);
			else if(cmsg)
				fputs("_inline_ message ", f);
			else if(ctxt)
				fputs("_inline_ text ", f);
			else if(cid)
				fputs("_inline_ messageid ", f);
			fputs(AI2C_gasz_Functions[i], f);
			fputs("_AI2C(", f);

			cpt = 0;
			pz = L_strchr(AI2C_gasz_FunctionsCP[i], '(') + 1;
			max = EAI_gast_Functions[i].i_NumPars;
			if(EAI_gast_Functions[i].c_IsUltra) max++;
			while(1)
			{
				if(cpt == max)
				{
					if(cpt == 0) fputs("void", f);
					if(cvec)
						fputs(")\n{\n\tvector out;\n", f);
					else if(cmsg)
						fputs(")\n{\n\tmessage out;\n", f);
					else if(ctxt)
						fputs(")\n{\n\ttext out;\n", f);
					else if(cid)
						fputs(")\n{\n\tmessageid out;\n", f);
					fputs("\t", f);

					fputs(AI2C_gasz_Functions[i], f);
					fputs("(", f);
					for(j = 0; j < max; j++)
					{
						sprintf(az_Tmp, "p%d, ", j);
						fputs(az_Tmp, f);
					}

					if(cvec)
						fputs("(PVEC) out);\n", f);
					else if(cmsg)
						fputs("(PPMSG) out);\n", f);
					else if(ctxt)
						fputs("(PTEXT) out);\n", f);
					else if(cid)
						fputs("(PMSGID) out);\n", f);
					fputs("\treturn out;\n}\n\n", f);
					break;
				}
				else
				{
					j = 0;
#ifdef JADEFUSION
					bool FoundSpace = false;
					bool FoundType  = false;
					while(*pz != ',' && *pz != ')') 
					{
						// remove parameter name (just keep the type)
						if (*pz != ' ')
							FoundType = true;
						if (*pz == ' ' && FoundType)
							FoundSpace = true;
						if (!FoundSpace || *pz == '*' || *pz == '&')
							az_Tmp[j++] = *pz;
						pz++;
					}
#else					
					while(*pz != ',' && *pz != ')') az_Tmp[j++] = *pz++;
#endif
					az_Tmp[j] = 0;
					fputs(az_Tmp, f);
					fputs(" ", f);
					sprintf(az_Tmp, "p%d", cpt);
					fputs(az_Tmp, f);
					pz++;
					cpt++;
					if(cpt != max) fputs(", ", f);
				}
			}
		}
	}

	fclose(f);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INSERTS(int cpt, char *pz)
{
	/*~~~~*/
	int len;
	/*~~~~*/

	len = strlen(pz);
	if(len + gpi_GlobalSize >= gpi_GlobalMaxSize)
	{
		gpi_GlobalMaxSize += (len + 1024);
		gpz_GlobalBuf = (char *) realloc(gpz_GlobalBuf, gpi_GlobalMaxSize);
	}

	L_memmove(gpz_GlobalBuf + cpt + len, gpz_GlobalBuf + cpt, gpi_GlobalSize - cpt);
	L_memcpy(gpz_GlobalBuf + cpt, pz, len);
	gpi_GlobalSize += len;
	gpz_GlobalBuf[gpi_GlobalSize] = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PUTS(char *pz)
{
	/*~~~~*/
	int len;
	/*~~~~*/

	len = strlen(pz);
	if(AI2C_gi_GlInsertMode)
	{
		INSERTS(AI2C_gi_GlInsert, pz);
		AI2C_gi_GlInsert += len;
		return;
	}

	if(len + gpi_GlobalSize >= gpi_GlobalMaxSize)
	{
		gpi_GlobalMaxSize += (len + 1024);
		if(gpz_GlobalBuf)
			gpz_GlobalBuf = (char *) realloc(gpz_GlobalBuf, gpi_GlobalMaxSize);
		else
			gpz_GlobalBuf = (char *) malloc(gpi_GlobalMaxSize);
	}

	strcpy(gpz_GlobalBuf + gpi_GlobalSize, pz);
	gpi_GlobalSize += len;
	gpz_GlobalBuf[gpi_GlobalSize] = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DELETES(int cpt, int len)
{
	L_memmove(gpz_GlobalBuf + cpt, gpz_GlobalBuf + cpt + len, gpi_GlobalSize - cpt - len);
	gpi_GlobalSize -= len;
	gpz_GlobalBuf[gpi_GlobalSize] = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_CheckName(char *az_Name)
{
	/*~~~~~~~~*/
	char	*pz;
	/*~~~~~~~~*/

	pz = az_Name;
	if(L_isdigit(*pz)) *pz = '_';
	while(*pz)
	{
		if(!L_isalnum(*pz) && (*pz != '_')) *pz = '_';
		if(*pz == '°') *pz = '_';
		if(*pz == 'é') *pz = '_';
		if(*pz == 'è') *pz = '_';
		if(*pz == 'à') *pz = '_';
		if(*pz == 'ä') *pz = '_';
		if(*pz == 'â') *pz = '_';
		if(*pz == 'ë') *pz = '_';
		if(*pz == 'ê') *pz = '_';
		pz++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_AddTriggerToDef(BIG_INDEX ul_File, char *name, int num)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	FILE	*file;
	char	az_Line[1024];
	char	az_Name[512];
	char	az_DefName[512];
	char	*pz;
	/*~~~~~~~~~~~~~~~~~~~~*/

	/* Engine func */
	strcpy(az_Name, BIG_NameFile(ul_File));
	*strrchr(az_Name, '.') = 0;
	L_strcat(az_Name, ".fce");
	ul_File = BIG_ul_SearchFile(BIG_ParentFile(ul_File), az_Name);

	/* Add file in deflist */
	strcpy(az_DefName, AI_PATH);
	strcat(az_DefName, "AI2C_fctdefs.h");

#ifdef JADEFUSION
	// make sure file exists.. create it otherwise
	file = fopen(az_DefName, "r");
	if (!file)
		file = fopen(az_DefName, "w");
	fclose(file);
#endif

	if(_access(az_DefName, 2)) _chmod(az_DefName, _S_IREAD | _S_IWRITE);

	int nCount = 0;
    while ((file = fopen(az_DefName, "r+")) == NULL)
	{
		fflush(NULL);
		Sleep(500);

		char sMsg[1024];
		char* errStr = strerror(errno);
		sprintf(sMsg,"Cannot open '%s' : %s\n",az_DefName,errStr);
		ERR_LogPrint(sMsg);

		if(++nCount>=10)
		{
			sprintf(sMsg,"Fatal error : Cannot open '%s' : %s\n",az_DefName,errStr);
			ERR_LogPrint(sMsg);
			return;
		}
	}

	pz = fgets(az_Line, 1024, file);
	while(pz)
	{
		pz = strrchr(az_Line, ',');
		pz++;
		while(L_isspace(*pz)) pz++;
		*strrchr(pz, ')') = 0;
		if(!_strcmpi(pz, name)) break;
		pz = fgets(az_Line, 1024, file);
	}

	if(!pz)
	{
		fseek(file, 0, SEEK_END);
		sprintf(az_Line, "AI2C_FCTDEFTRIGGER(0x%x, %d, %s)\n", BIG_FileKey(ul_File), num, name);
		fputs(az_Line, file);
	}

	fclose(file);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_AddToDef(BIG_INDEX ul_File)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	char	az_Name[512];
	char	az_DefName[512];
	FILE	*file;
	char	az_Line[1024];
	char	*pz;
	/*~~~~~~~~~~~~~~~~~~~~*/

	/* Engine func */
	strcpy(az_Name, BIG_NameFile(ul_File));
	pz = strrchr(az_Name, '.');
	if(!L_strcmpi(pz, ".fct"))
	{
		*strrchr(az_Name, '.') = 0;
		strcat(az_Name, ".ofc");
		ul_File = BIG_ul_SearchFile(BIG_ParentFile(ul_File), az_Name);
	}
	else if(!L_strcmpi(pz, ".mdl"))
	{
		*strrchr(az_Name, '.') = 0;
		L_strcat(az_Name, ".omd");
		BIG_ComputeFullName(BIG_ParentFile(ul_File), az_Line);
		L_strcat(az_Line, "/");
		L_strcat(az_Line, az_Name);
		*strrchr(az_Line, '.') = 0;
		ul_File = BIG_ul_SearchFileExt(az_Line, az_Name);
	}
	else if(!L_strcmpi(pz, ".fcl"))
	{
		*strrchr(az_Name, '.') = 0;
		strcat(az_Name, ".fce");
		ul_File = BIG_ul_SearchFile(BIG_ParentFile(ul_File), az_Name);
	}

	/* Add file in deflist */
	strcpy(az_Name, BIG_NameFile(ul_File));
	*strrchr(az_Name, '.') = 0;
	AI2C_CheckName(az_Name);
	strcpy(az_DefName, AI_PATH);
	strcat(az_DefName, "AI2C_fctdefs.h");

#ifdef JADEFUSION
	// make sure file exists.. create it otherwise
	file = fopen(az_DefName, "r");
	if (!file)
		file = fopen(az_DefName, "w");
	fclose(file);
#endif

	if(_access(az_DefName, 2)) _chmod(az_DefName, _S_IREAD | _S_IWRITE);
	int nCount = 0;
    while ((file = fopen(az_DefName, "r+")) == NULL)
	{
		fflush(NULL);
		Sleep(500);

		char sMsg[1024];
		char* errStr = strerror(errno);
		sprintf(sMsg,"Cannot open '%s' : %s\n",az_DefName,errStr);
		ERR_LogPrint(sMsg);

		if(++nCount>=10)
		{
			sprintf(sMsg,"Fatal error : Cannot open '%s' : %s\n",az_DefName,errStr);
			ERR_LogPrint(sMsg);
			return;
		}
	}

	pz = fgets(az_Line, 1024, file);
	while(pz)
	{
		pz = strchr(az_Line, ',');
		pz++;
		while(L_isspace(*pz)) pz++;
		*strrchr(pz, ')') = 0;
		if(!strcmp(pz, az_Name)) break;
		pz = fgets(az_Line, 1024, file);
	}

    if(!AI2C_gb_FctListInit)
    {
        AI2C_gb_FctListInit = TRUE;
        BAS_binit(&AI2C_gst_FctList, 32);
    }

	if(!pz)
	{
		fseek(file, 0, SEEK_END);
		sprintf(az_Line, "AI2C_FCTDEF(0x%x, %s)\n", BIG_FileKey(ul_File), az_Name);
		fputs(az_Line, file);
        BAS_binsert(BIG_FileKey(ul_File), BIG_FileKey(ul_File), &AI2C_gst_FctList);
	}
    else
    {
        char log[512];
        ULONG ul = BAS_bsearch(BIG_FileKey(ul_File), &AI2C_gst_FctList);

        sprintf(log, "two functions have the same name : %s [%x]", az_Line, BIG_FileKey(ul_File));
        ERR_X_Error(((ul != -1) && (ul == BIG_FileKey(ul_File))), log, "");
    }
	fclose(file);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_GenerateVars(BIG_INDEX ul_Model, BIG_INDEX ul_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	char	az_NameModel[64];
	char	az_Name[512];
	char	az_Dum[512];
	FILE	*f;
	ULONG	ul_Size;
	char	*pc_Buf;
	ULONG	i;
	char	az_Path[512];
	char	az_Tmp[1024];
	char	*pz, *pz1;
	int		i_Dummy;
	int		i_Empty;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	gpi_GlobalSize = 0;
	AI2C_gi_GlInsert = 0;
	AI2C_gi_GlInsertMode = 0;

	/* Global path */
	strcpy(az_Path, AI_PATH);

	strcat(az_Path, "AI2C_ofc");
	_mkdir(az_Path);
	strcat(az_Path, "/");

	/* Generation du nom */
	L_strcpy(az_NameModel, BIG_NameFile(ul_Model));
	*L_strrchr(az_NameModel, '.') = 0;
    AI2C_CheckName(az_NameModel);
	L_strcpy(az_Name, az_NameModel);
	
	L_strcat(az_Name, ".h");
	if(_access(az_Name, 2)) _chmod(az_Name, _S_IWRITE);
	L_strcat(az_Path, az_Name);

	f = fopen(az_Path, "w");
    if (!f)
    {
        char buffer[_MAX_PATH];

        /* Get the current working directory: */
        ERR_X_Assert( _getcwd( buffer, _MAX_PATH ) != NULL );

    	L_strcat(buffer, "/");
    	L_strcat(buffer, az_Path);
        sprintf( az_Path, "Cannot open file %s \n", buffer);
        ERR_LogPrint(az_Path);
        
        return;
    }

	L_strcpy(az_Name, BIG_NameFile(ul_File));
	L_strcat(az_Name, ".pp");
	ul_File = BIG_ul_SearchFile(BIG_ParentFile(ul_File), az_Name);
	pc_Buf = BIG_pc_ReadFileTmp(BIG_PosFile(ul_File), &ul_Size);

	/* Struct variables */
	sprintf(az_Tmp, "#ifndef __%s_H__\n#define __%s_H__\n", az_NameModel, az_NameModel);
	fputs(az_Tmp, f);
	sprintf(az_Tmp, "typedef struct %s_Vars_ {\n", az_NameModel);
	fputs(az_Tmp, f);

	/* On élimine certaines definitions */
	pz = pc_Buf;
	while(*pz && (*pz=='\n')||(*pz==' ')||(*pz=='\t')) pz++;
	while(*pz)
	{
		pz1 = pz;
		if(!strncmp(pz, "function", 8) && 
                (
                    (pz[8]==' ') || (pz[8]=='\n') || (pz[8]=='\t') || (pz[8]==0x0a)
                ) && 
                (
                    (pz[-1]==' ') || (pz[-1]=='\n') || (pz[-1]=='\t')  || (pz[-1]==0x0a)
                ) )
		{
			pz += 8;
			while(*pz && L_isspace(*pz)) pz++;
			if(!strncmp(pz, "track", 5))
			{
				while(*pz1 && *pz1 != '\n') *pz1++ = '\n';
				pz = pz1;
			}
		}
		else if(!strncmp(pz, "separator", 9) && 
                (
                    (pz[9]=='"') || (pz[9]==' ') || (pz[9]=='\n') || (pz[9]=='\t') || (pz[9]==0x0a)
                ) && 
                (
                    (pz[-1]==' ') || (pz[-1]=='\n') || (pz[-1]=='\t')  || (pz[-1]==0x0a)
                ) )
		{
			while(*pz && *pz != '\n') *pz++ = '\n';
		}
		else if
            (
                !strncmp(pz, "vector", 6) && 
                (
                    (pz[6]==' ') || (pz[6]=='\n') || (pz[6]=='\t') || (pz[6]==0x0a)
                ) && 
                (
                    (pz[-1]==' ') || (pz[-1]=='\n') || (pz[-1]=='\t')  || (pz[-1]==0x0a)
                ) 
            )
		{
			pz[4] = '_';
			pz[5] = 'h';
		}

		pz++;
	}

	pz = pc_Buf;
	while(*pz && (*pz=='\n')||(*pz==' ')||(*pz=='\t')) pz++;
	while(*pz)
	{
		if(*pz == '=')
		{
			while(*pz && *pz != '\n') *pz++ = '\n';
		}
		else
		{
			pz++;
		}
	}  

	i_Dummy = 0;
	while(*pc_Buf)
	{
		/* Get one line */
		i = 0;
		i_Empty = TRUE;
		while(*pc_Buf && *pc_Buf != '\n')
		{
			if(!L_isspace(*pc_Buf)) i_Empty = FALSE;
			az_Tmp[i++] = *pc_Buf++;
		}

		if(*pc_Buf) pc_Buf++;
		az_Tmp[i++] = '\n';
		az_Tmp[i++] = 0;

		if(i_Empty) continue;

		/* Array. One dummy int before the array per dimension */
		pz = L_strchr(az_Tmp, '[');
		while(pz)
		{
			sprintf(az_Dum, "\tint\tDUMMY_ARRAY%d;\n", i_Dummy++);
			pz++;
			pz = L_strchr(pz, '[');
			fputs(az_Dum, f);
		}

		if(AI_gb_GenOptim2)
		{
			pz = az_Tmp;
			while(*pz && *pz != 'o') pz++;
			if(!strncmp(pz, "optim", 5))
			{
				pz += 5;
				while(!L_isalpha(*pz)) pz++;
				while(L_isalpha(*pz)) pz++;
				pz1 = pz;
				while(*pz1 && *pz1 != '\n' && *pz1 != '[') pz1++;
				if(*pz1 == '[')
				{
					while(*pz1 != ']') *pz1++ = ' ';
					*pz1++ = ' ';
					*pz = '*';
				}
			}
		}

		/* Static init */
		pz = L_strchr(az_Tmp, '=');
		if(pz)
		{
			pz[0] = '/';
			pz[1] = '/';
		}

		/* Add ';' */
		pz = az_Tmp + strlen(az_Tmp) - 1;
		while(L_isspace(*pz)) pz--;
		pz[1] = ';';
		pz[2] = '\n';
		pz[3] = 0;

		/* Output */
		fputs("\t", f);
		fputs(az_Tmp, f);
	}


    AI2C_CheckName(az_NameModel);
	sprintf(az_Tmp, "} %s_Vars;\n\n", az_NameModel);
	fputs(az_Tmp, f);
	fputs("\n\n", f);

	sprintf(az_Tmp, "#endif /* __%s_H__ */\n", az_NameModel);
	fputs(az_Tmp, f);

	fclose(f);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_AddToFctListName(char *az_Name)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	char	az_DefName[512];
	FILE	*file;
	char	az_Precomp[1024];
	char	az_Arg[1024];
	char	*pz;
	int		found,nCount;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	/* Num file */
	AI2C_gi_FileNum = 0;
	pz = az_Name;
	while(*pz)
	{
		AI2C_gi_FileNum += *pz;
		pz++;
	}

	AI2C_gi_FileNum = AI2C_gi_FileNum % 10;

	/* Add file in include list */
	sprintf(az_DefName, "%s%s%d.cpp", AI_PATH, "AI2C_fctlist", AI2C_gi_FileNum);

#ifdef JADEFUSION
	// make sure file exists.. create it otherwise
	file = fopen(az_DefName, "r");
	if (!file)
		file = fopen(az_DefName, "w");
	fclose(file);
#endif

	if(_access(az_DefName, 2)) _chmod(az_DefName, _S_IWRITE);
	nCount=0;
    while ((file = fopen(az_DefName, "r+")) == NULL)
	{
		fflush(NULL);
		Sleep(500);

		char sMsg[1024];
		char* errStr = strerror(errno);
		sprintf(sMsg,"Cannot open '%s' : %s\n",az_DefName,errStr);
		ERR_LogPrint(sMsg);

		if(++nCount>=10)
		{
			sprintf(sMsg,"Fatal error : Cannot open '%s' : %s\n",az_DefName,errStr);
			ERR_LogPrint(sMsg);
			return;
		}
	}

	found = 0;
	while(fscanf(file, "%s %s", az_Precomp, az_Arg) != -1)
	{
		if(_strcmpi(az_Precomp, "#include")) continue;
		pz = strrchr(az_Arg, '/');
		if(pz)
		{
			pz++;
			*strrchr(pz, '"') = 0;
			if(!_strcmpi(pz, az_Name))
			{
				found = 1;
				break;
			}
		}
	}

	if(!found)
	{
		fseek(file, 0, SEEK_END);
		fprintf(file, "#include \"AI2C_ofc/%s\"\n", az_Name);
	}

	fclose(file);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_AddToFctList(BIG_INDEX ul_Model)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	char	az_Name[512];
	char	az_Path[512];
	char	az_NameModel[512];
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	/* Global path */
	strcpy(az_Path, AI_PATH);
	strcat(az_Path, "AI2C_ofc");
	_mkdir(az_Path);
	strcat(az_Path, "/");

	/* Generate file */
	L_strcpy(az_NameModel, BIG_NameFile(ul_Model));
	*L_strrchr(az_NameModel, '.') = 0;
    
	L_strcpy(az_Name, az_NameModel);
	AI2C_CheckName(az_Name);
	L_strcat(az_Name, ".cpp");
	if(_access(az_Name, 2)) _chmod(az_Name, _S_IWRITE);
	L_strcat(az_Path, az_Name);

	L_strcpy(AI2C_gaz_GlobalName, az_Path);
	AI2C_gf_Model = fopen(az_Path, "w");

	/* Include */
	AI2C_AddToFctListName(az_Name);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_ReplaceFctAI(char *in)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	char	output[1024];
	char	az_Name[128];
	int		i;
	char	*pzo, *pzt, *pz;
	int		ok;
	/*~~~~~~~~~~~~~~~~~~~~*/

	pzo = output;
	pz = in;
	while(*pz)
	{
		while(*pz && !L_isalpha((UCHAR) * pz) && *pz != '_')
		{
			if(*pz == '"')
			{
				*pzo++ = *pz++;
				while(*pz != '"') *pzo++ = *pz++;
			}

			*pzo++ = *pz++;
		}

		if(!*pz) break;

		i = 0;
		az_Name[i++] = *pz++;
		while(L_isalnum(*pz) || *pz == '_') az_Name[i++] = *pz++;
		az_Name[i] = 0;

		ok = 0;
		for(i = 0; i < sizeof(AI2C_gasz_FunctionsInt) / sizeof(void *); i++)
		{
			if(!L_strcmp(AI2C_gasz_FunctionsInt[i], az_Name))
			{
				L_strcpy(pzo, AI2C_gasz_Functions[i]);
				pzo += L_strlen(AI2C_gasz_Functions[i]);

				/* Surcharge */
				if
				(
					(EAI_gast_Functions[i].i_ReturnType == TYPE_VECTOR)
				||	(EAI_gast_Functions[i].i_ReturnType == TYPE_MESSAGE)
				||	(EAI_gast_Functions[i].i_ReturnType == TYPE_TEXT)
				||	(EAI_gast_Functions[i].i_ReturnType == TYPE_MESSAGEID)
				)
				{
					L_strcat(pzo, "_AI2C");
					pzo += 5;
				}

				ok = 1;
				break;
			}
		}

		if(!ok)
		{
			pzt = az_Name;
			while(*pzt) *pzo++ = *pzt++;
		}
	}

	*pzo = 0;
	L_strcpy(in, output);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_FirstScanFile(char *pc_Buf, BOOL proclist)
{
	/*~~~~~~~~~~~~~~~~~*/
	char	*pz;
	char	az_Tmp[1024];
	int		i;
	/*~~~~~~~~~~~~~~~~~*/

	while(*pc_Buf)
	{
		/* Get one line (eliminate double spaces) */
		i = 0;
		while(*pc_Buf && *pc_Buf != '\n')
		{
			if(*pc_Buf == '\r') *pc_Buf = ' ';
			az_Tmp[i++] = *pc_Buf++;
		}

		if(*pc_Buf == '\n') pc_Buf++;
		az_Tmp[i++] = '\n';
		az_Tmp[i++] = 0;

		AI2C_ReplaceFctAI(az_Tmp);

		/* Empty ? */
		pz = az_Tmp;
		if(gpz_GlobalBuf && gpz_GlobalBuf[gpi_GlobalSize - 1] == '\n')
		{
			pz = az_Tmp;
			while(*pz)
			{
				if(!L_isspace(*pz)) break;
				pz++;
			}
		}

		/* Output */
		if(*pz)
		{
			if(!proclist) PUTS("\t");
			PUTS(az_Tmp);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_GenFunc(BIG_INDEX ul_Model, BIG_INDEX ul_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	az_Cast[128];
	char	az_Tmp[1024];
	char	az_Name[128];
	char	*pc_Buf;
	ULONG	ul_Size;
	char	az_Model[64];
	int		firstlabel;
	int		i, cpt, cpt2, first, count;
	int		cpt1, param;
	CString oname;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_strcpy(az_Cast, BIG_NameFile(ul_Model));
	*L_strrchr(az_Cast, '.') = 0;
    AI2C_CheckName(az_Cast);

	/* Decl */
	PUTS("//////////////////////////////////////////////////////////\n");
	PUTS("//////////////////////////////////////////////////////////\n");
	L_strcpy(az_Name, BIG_NameFile(ul_File));
	*L_strrchr(az_Name, '.') = 0;
    AI2C_CheckName(az_Name);
	sprintf(az_Tmp, "int %s(void) {\n", az_Name);
	PUTS(az_Tmp);
	sprintf(az_Tmp, "\t%s_Vars\t*DV = (%s_Vars *) (AI_gpst_CurrentInstance->pc_VarsBuffer);\n", az_Cast, az_Cast);
	PUTS(az_Tmp);
	sprintf(az_Tmp, "\tOBJ_tdst_GameObject\t*__who = AI_gpst_CurrentGameObject;\n");
	PUTS(az_Tmp);
	PUTS("\tint\t__resexec__;\n");
	PUTS("\n");
	first = gpi_GlobalSize;

	/* Get PP file */
	L_strcpy(az_Name, BIG_NameFile(ul_File));
	L_strcat(az_Name, ".pp");
	ul_File = BIG_ul_SearchFile(BIG_ParentFile(ul_File), az_Name);
	pc_Buf = BIG_pc_ReadFileTmp(BIG_PosFile(ul_File), &ul_Size);

	AI2C_FirstScanFile(pc_Buf, FALSE);
	PUTS(";\n");
	PUTS("\n\tRETURN(0);\n");
	PUTS("}\n\n");

	/* Recherche premier code */
	first = 0;
	while(gpz_GlobalBuf[first])
	{
		if(!strncmp(&gpz_GlobalBuf[first], "/*FIRST_CODE*/", 14))
		{
			DELETES(first, 14);
			break;
		}

		first++;
	}

	/* Metas */
	L_strcpy(az_Model, BIG_NameFile(ul_Model));
	*L_strrchr(az_Model, '.') = 0;
	AI2C_CheckName(az_Model);
	cpt = first;
	firstlabel = AI2C_gi_Label;
	while(gpz_GlobalBuf[cpt])
	{
		if(!L_strncmp(&gpz_GlobalBuf[cpt], "procedure_ultra", strlen("procedure_ultra")))
		{
			while(gpz_GlobalBuf[cpt] && gpz_GlobalBuf[cpt] != '(') cpt++;

			// Get name of procedure
			oname.Empty();
			cpt2 = cpt;
			while(!L_isspace(gpz_GlobalBuf[cpt2 - 1])) cpt2--;
			while(cpt2 != cpt && !L_isspace(gpz_GlobalBuf[cpt2])) oname += gpz_GlobalBuf[cpt2++];
			AI2C_go_ProcUltraList.AddTail(oname);

			cpt++;
			INSERTS(cpt, "OBJ_tdst_GameObject *");
			cpt += 21;

			cpt1 = cpt;
			param = 0;
			while(gpz_GlobalBuf[cpt1] && gpz_GlobalBuf[cpt1] != ')') 
			{
				if(!L_isspace(gpz_GlobalBuf[cpt1]) && gpz_GlobalBuf[cpt1] != ')' && gpz_GlobalBuf[cpt1] != '(')
					param = 1;
				cpt1++;
			}

			if(param)
			{
				INSERTS(cpt, ",");
			}
		}
		else if(!L_strncmp(&gpz_GlobalBuf[cpt], "TIME_Wait_C", strlen("TIME_Wait_C")))
		{
			while(gpz_GlobalBuf[cpt] != ';') cpt++;
			cpt++;
			sprintf
			(
				az_Tmp,
				" *(int *) AI_gppst_CurrentJumpNode = %d; RETURN(0);\n%sLBL_%d:\n\t*(int *) AI_gppst_CurrentJumpNode = 0;\n",
				AI2C_gi_Label,
				az_Model,
				AI2C_gi_Label
			);
			AI2C_gi_Label++;
			INSERTS(cpt, az_Tmp);
		}
		else if(!L_strncmp(&gpz_GlobalBuf[cpt], "meta", strlen("meta")))
		{
			sprintf(az_Tmp, "%sLBL_%d:\n\t*(int *) AI_gppst_CurrentJumpNode = 0;\n", az_Model, AI2C_gi_Label);
			INSERTS(cpt, az_Tmp);

			while(gpz_GlobalBuf[cpt] != '{') cpt++;
			cpt++;
			count = 1;
			while(count)
			{
				if(gpz_GlobalBuf[cpt] == '{') count++;
				if(gpz_GlobalBuf[cpt] == '}') count--;
				if(!count) break;
				cpt++;
			}

			sprintf(az_Tmp, "*(int *) AI_gppst_CurrentJumpNode = %d; RETURN(0);\n\t", AI2C_gi_Label);
			AI2C_gi_Label++;
			INSERTS(cpt, az_Tmp);
		}

		cpt++;
	}

	/* Switch for all metas */
	if(firstlabel != AI2C_gi_Label)
	{
		sprintf(az_Tmp, "\tswitch(*(int *) AI_gppst_CurrentJumpNode) {\n");
		INSERTS(first, az_Tmp);
		first += L_strlen(az_Tmp);
		for(i = firstlabel; i < AI2C_gi_Label; i++)
		{
			sprintf(az_Tmp, "\t\tcase %d: goto %sLBL_%d;\n", i, az_Model, i);
			INSERTS(first, az_Tmp);
			first += L_strlen(az_Tmp);
		}

		INSERTS(first, "\t}\n\n");
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_GetCWord(int &cpt, char *word)
{
	/*~~*/
	int i;
	/*~~*/

	*word = 0;
	while(gpz_GlobalBuf[cpt] && L_isspace(gpz_GlobalBuf[cpt])) cpt++;
	if(!gpz_GlobalBuf[cpt]) return;

	if(gpz_GlobalBuf[cpt] == '"')
	{
		cpt++;
		while(gpz_GlobalBuf[cpt] != '"') cpt++;
		cpt++;
		return;
	}

	if(!L_isalpha((UCHAR) gpz_GlobalBuf[cpt]) && gpz_GlobalBuf[cpt] != '_')
	{
		cpt++;
		return;
	}

	i = 0;
	word[i++] = gpz_GlobalBuf[cpt++];
	while(L_isalnum(gpz_GlobalBuf[cpt]) || gpz_GlobalBuf[cpt] == '_') word[i++] = gpz_GlobalBuf[cpt++];
	word[i] = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_ProcessUltra(EAI_cl_Compiler *po_Compiler)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		az_Name[128];
	char		az_Ultra[128];
	int			i, len;
	int			cpt, cptu, cptf;
	POSITION	pos;
	void		*po_Var;
	CString		o_Name;
	int			ref;
	ULONG		key, fat;
	int			cpt_arr, cpt2;
	int			param, cptp;
	int			autoultra;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	cpt = 0;
	while(gpz_GlobalBuf[cpt])
	{
		while(gpz_GlobalBuf[cpt] && L_isspace(gpz_GlobalBuf[cpt])) cpt++;

		/* Insert ';' before '}' */
		if(gpz_GlobalBuf[cpt] == '}')
		{
			INSERTS(cpt, ";");
			cpt++;
		}

		/* Get ultra operator */
		ref = 0;
		az_Ultra[0] = 0;
		if(gpz_GlobalBuf[cpt] == '@')
		{
			cptu = cpt++;
			while(gpz_GlobalBuf[cpt] && L_isspace(gpz_GlobalBuf[cpt])) cpt++;

			if(gpz_GlobalBuf[cpt] == '"')
			{
				/* Get ref */
				len = 0;
				az_Ultra[len++] = gpz_GlobalBuf[cpt++];
				while(gpz_GlobalBuf[cpt] != '"') az_Ultra[len++] = gpz_GlobalBuf[cpt++];
				az_Ultra[len++] = gpz_GlobalBuf[cpt++];

				/* Comment just after (ref mark) */
				while(gpz_GlobalBuf[cpt] && L_isspace(gpz_GlobalBuf[cpt])) cpt++;
				if(gpz_GlobalBuf[cpt] == '/' && gpz_GlobalBuf[cpt + 1] == '*' && gpz_GlobalBuf[cpt + 2] == '0')
				{
					/* @gao var => no DV */
					sscanf(&gpz_GlobalBuf[cpt], "/*0x%x*/", &key);
					fat = key;
					key = BIG_ul_SearchKeyToFat(key);
					if(BIG_b_IsFileExtension(key, ".gao")) ref = 1;
					if(BIG_b_IsFileExtension(key, ".oin")) ref = 1;

					az_Ultra[len++] = gpz_GlobalBuf[cpt++];
					while(gpz_GlobalBuf[cpt] != '/') az_Ultra[len++] = gpz_GlobalBuf[cpt++];
					az_Ultra[len++] = gpz_GlobalBuf[cpt++];
				}

				az_Ultra[len] = 0;
			}
			else
			{
				AI2C_GetCWord(cpt, az_Ultra);
				len = strlen(az_Ultra);
				while(gpz_GlobalBuf[cpt] && L_isspace(gpz_GlobalBuf[cpt])) cpt++;
				while(gpz_GlobalBuf[cpt] == '[')
				{
					cpt_arr = 1;
					while(cpt_arr)
					{
						az_Ultra[len++] = gpz_GlobalBuf[cpt++];
						if(gpz_GlobalBuf[cpt] == '[') cpt_arr++;
						if(gpz_GlobalBuf[cpt] == ']') cpt_arr--;
					}
					az_Ultra[len++] = gpz_GlobalBuf[cpt++];
					az_Ultra[len] = 0;
					while(gpz_GlobalBuf[cpt] && L_isspace(gpz_GlobalBuf[cpt])) cpt++;
				}
			}
		}

		/* Get function */
		while(gpz_GlobalBuf[cpt] && L_isspace(gpz_GlobalBuf[cpt])) cpt++;
		cptf = cpt;
		AI2C_GetCWord(cpt, az_Name);
		if(az_Name[0])
		{
			for(i = 0; i < sizeof(AI2C_gasz_Functions) / sizeof(void *); i++)
			{
				if(!strncmp(az_Name, AI2C_gasz_Functions[i], strlen(AI2C_gasz_Functions[i])))
				{
					if(EAI_gast_Functions[i].c_IsUltra)
					{
						while(gpz_GlobalBuf[cpt] != '(') cpt++;
						cpt++;

						if(az_Ultra[0])
						{
							INSERTS(cpt, az_Ultra);
							cpt += strlen(az_Ultra);
							if(EAI_gast_Functions[i].i_NumPars) INSERTS(cpt, ", ");
							DELETES(cptu, cptf - cptu);
							cpt -= (cptf - cptu);
							cpt -= strlen(az_Ultra);
						}
						else
						{
							if(EAI_gast_Functions[i].i_NumPars == 0)
								INSERTS(cpt, "__who");
							else
								INSERTS(cpt, "__who, ");
						}
					}
					break;
				}
			}

			/* Ultra procedure ? */
			autoultra = 0;
			if(!az_Ultra[0] && az_Name[0] && i == sizeof(AI2C_gasz_Functions) / sizeof(void *))
			{
				if(AI2C_go_ProcUltraList.Find(az_Name) != NULL)
				{
					// Be sure it's a call, and not a definition
					cpt2 = cpt;
					while(gpz_GlobalBuf[cpt2] != '(') cpt2++;
					cpt2++;
					while(L_isspace(gpz_GlobalBuf[cpt2])) cpt2++;
					if(L_strncmp(&gpz_GlobalBuf[cpt2], "OBJ_tdst_GameObject", 19))
					{
						autoultra = 1;
						L_strcpy(az_Ultra, "AI_gpst_CurrentGameObject");
					}
				}
			}

			if(az_Ultra[0] && i == sizeof(AI2C_gasz_Functions) / sizeof(void *))
			{
				if(az_Name[0] && AI2C_go_ProcUltraList.Find(az_Name) != NULL)
				{
					if(az_Ultra[0] == '"')
					{
						DELETES(cptu, cptf - cptu);
						cpt -= (cptf - cptu);
						autoultra = 1;
						AI2C_go_MapRef.SetAt((void *) fat, (void *) fat);
						sprintf(az_Ultra, "%sREF_%x", AI2C_gaz_GlobalRef, fat);
					}

					while(gpz_GlobalBuf[cpt] != '(') cpt++;
					cpt++;

					cpt2 = cpt;
					param = 0;
					cptp = 1;
					while(1)
					{
						if(gpz_GlobalBuf[cpt2] == ')') cptp--;
						if(!cptp) break;
						if(gpz_GlobalBuf[cpt2] == '(') cptp++;
						if(cptp == 1 && !L_isspace(gpz_GlobalBuf[cpt2]) && gpz_GlobalBuf[cpt2] != ')' && gpz_GlobalBuf[cpt2] != '(') param = 1;
						cpt2++;
					}

					if(param) L_strcat(az_Ultra, ", ");
					INSERTS(cpt, az_Ultra);
					cpt += strlen(az_Ultra);
					if(!autoultra)
					{
						DELETES(cptu, cptf - cptu);
						cpt -= (cptf - cptu);
						cpt -= strlen(az_Ultra);
					}
					continue;
				}
			}

			/* A name that is not a function (put DV->) */
			if(!ref && po_Compiler && i == sizeof(AI2C_gasz_Functions) / sizeof(void *))
			{
				pos = po_Compiler->mst_GlobalVars.o_Vars.GetStartPosition();
				while(pos)
				{
					po_Compiler->mst_GlobalVars.o_Vars.GetNextAssoc(pos, o_Name, (void * &) po_Var);
					if(!L_strcmp(az_Name, (char *) (LPCSTR) o_Name))
					{
						INSERTS(cptf, "DV->");
						cpt += 4;
						break;
					}
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_GetVar(int &cpt, char *out)
{
	/*~~*/
	int i;
	/*~~*/

	i = 0;
recom:
	while(gpz_GlobalBuf[cpt] == '_' || L_isalnum(gpz_GlobalBuf[cpt])) out[i++] = gpz_GlobalBuf[cpt++];
	if(gpz_GlobalBuf[cpt] == '-' && gpz_GlobalBuf[cpt + 1] == '>')
	{
		out[i++] = gpz_GlobalBuf[cpt++];
		out[i++] = gpz_GlobalBuf[cpt++];
		goto recom;
	}

	while(gpz_GlobalBuf[cpt] && L_isspace(gpz_GlobalBuf[cpt])) cpt++;
	while(gpz_GlobalBuf[cpt] == '[')
	{
		out[i++] = gpz_GlobalBuf[cpt++];
		while(gpz_GlobalBuf[cpt] && gpz_GlobalBuf[cpt] != ']') out[i++] = gpz_GlobalBuf[cpt++];
		out[i++] = gpz_GlobalBuf[cpt++];
		while(gpz_GlobalBuf[cpt] && L_isspace(gpz_GlobalBuf[cpt])) cpt++;
	}

	out[i] = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_MakeRefs(BIG_INDEX ul_File, BOOL proclist)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				az_Model[1024];
	char				az_Var[128];
	char				az_Tmp[1024];
	char				az_Tmp1[1024];
	ULONG				key;
	POSITION			pos;
	int					p, cpt, cpt1, cpt2;
	BIG_INDEX			ul_Ref, ul_Model;
	char				*pz;
	BIG_INDEX			file;
	BOOL				ultra;
	OBJ_tdst_GameObject *pst_GAO;
	int					len;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI2C_gi_GlInsertMode = 1;

	cpt = 0;
	while(gpz_GlobalBuf[cpt])
	{
		/* AI_Execute retourne une valeur a prendre en compte */
		if(!proclist && gpz_GlobalBuf[cpt - 1] != '=' && !L_strncmp(&gpz_GlobalBuf[cpt], "AI_EvalFunc_AIExecute_C", 23))
		{
			INSERTS(cpt, "{ if(__resexec__ =");
			cpt1 = cpt;
			while(gpz_GlobalBuf[cpt1] != ';') cpt1++;
			INSERTS(cpt1 + 1, "}");
			INSERTS(cpt1, ") RETURN(__resexec__)");
		}

		if(gpz_GlobalBuf[cpt] == '/' && gpz_GlobalBuf[cpt + 1] == '*' && gpz_GlobalBuf[cpt + 2] == '0')
		{
			/* Get key */
			sscanf(&gpz_GlobalBuf[cpt], "/*0x%x*/", &key);
			ul_Ref = BIG_ul_SearchKeyToFat(key);
			pz = L_strrchr(BIG_NameFile(ul_Ref), '.');

			/* Del comment mark and ref string */
			len = 1;
			cpt1 = cpt + 1;
			while(gpz_GlobalBuf[cpt1] != '/') cpt1++, len++;
			DELETES(cpt, len + 1);

//			DELETES(cpt, 14);
			cpt1 = cpt;
			while(gpz_GlobalBuf[cpt1] != '"') cpt1--;
			cpt1--;
			while(gpz_GlobalBuf[cpt1] != '"') cpt1--;
			DELETES(cpt1, cpt - cpt1);
			cpt = cpt1;

			/* Ultra restant (gao var) ? */
			ultra = FALSE;
			cpt1--;
			while(cpt1 && L_isspace(gpz_GlobalBuf[cpt1])) cpt1--;
			if(gpz_GlobalBuf[cpt1] == '@') ultra = TRUE;

			/* Replace by var */
			if(!L_strcmpi(pz, ".ofc"))
			{
				/*
				 * Si on fait un execute d'une fonction constante, on peut appeler directement la
				 * fonction
				 */
				cpt2 = cpt - 1;
				while(cpt2 && L_isspace(gpz_GlobalBuf[cpt2])) cpt2--;
				if(gpz_GlobalBuf[cpt2] == '(')
				{
					cpt2--;
					while(cpt2 && L_isspace(gpz_GlobalBuf[cpt2])) cpt2--;
					while(cpt2 && !L_isspace(gpz_GlobalBuf[cpt2])) cpt2--;
					cpt2++;
					if(gpz_GlobalBuf[cpt2] == '=') cpt2++;
					if(!L_strncmp(&gpz_GlobalBuf[cpt2], "AI_EvalFunc_AIExecute_C", 23))
					{
						DELETES(cpt2, 23);
						L_strcpy(az_Model, BIG_NameFile(ul_Ref));
						*L_strrchr(az_Model, '.') = 0;
						INSERTS(cpt2, az_Model);
						cpt = cpt2;
						continue;
					}
				}

				AI2C_go_MapRef.SetAt((void *) key, (void *) key);
				sprintf(az_Model, "%sREF_%x", AI2C_gaz_GlobalRef, key);
				INSERTS(cpt, az_Model);
			}
			else if(!L_strcmpi(pz, ".gao"))
			{
				az_Tmp[0] = 0;
				if(ultra)
				{
					DELETES(cpt1, 1);
					cpt = cpt1;

					pst_GAO = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(ul_Ref));
					ul_Model = LOA_ul_SearchKeyWithAddress((ULONG) pst_GAO->pst_Extended->pst_Ai->pst_Model);
					ul_Model = BIG_ul_SearchKeyToFat(ul_Model);
					L_strcpy(az_Model, BIG_NameFile(ul_Model));
					*L_strrchr(az_Model, '.') = 0;
					L_strcat(az_Model, "_Vars");

					file = LOA_ul_SearchKeyWithAddress((ULONG) pst_GAO->pst_Extended->pst_Ai);
					sprintf(az_Tmp, "((%s *) %sREF_%x)->", az_Model, AI2C_gaz_GlobalRef, file);
					AI2C_go_MapRef.SetAt((void *) file, (void *) file);

					/* Include model (insert mode is active, so at the beginning) */
					if(!AI2C_go_IncludeRef.Find(ul_Model))
					{
						AI2C_go_IncludeRef.AddTail(ul_Model);
						L_strcpy(az_Model, BIG_NameFile(ul_Model));
						*L_strchr(az_Model, '.') = 0;
						L_strcat(az_Model, ".h");
						sprintf(az_Tmp1, "#include \"AI2C_ofc/%s\"\n", az_Model);
						PUTS(az_Tmp1);
						cpt += L_strlen(az_Tmp1);
					}
				}
				else
				{
					AI2C_go_MapRef.SetAt((void *) key, (void *) key);
					sprintf(az_Model, "%sREF_%x", AI2C_gaz_GlobalRef, key);
					L_strcat(az_Tmp, az_Model);
				}

				INSERTS(cpt, az_Tmp);
			}
			else if(!L_strcmpi(pz, ".mdl"))
			{
				if(ultra)
				{
					cpt1 = cpt;
					while(gpz_GlobalBuf[cpt1] != '@') cpt1--;
					while(L_isspace(gpz_GlobalBuf[cpt])) cpt++;
					AI2C_GetVar(cpt, az_Var);
					DELETES(cpt1, cpt - cpt1);

					L_strcpy(az_Model, BIG_NameFile(ul_Ref));
					*L_strrchr(az_Model, '.') = 0;
					L_strcat(az_Model, "_Vars");

					sprintf(az_Tmp, "((%s *) ((%s)->pst_Extended->pst_Ai->pc_VarsBuffer))->", az_Model, az_Var);
					INSERTS(cpt1, az_Tmp);
					cpt = cpt1;

					/* Include model (insert mode is active, so at the beginning) */
					if(!AI2C_go_IncludeRef.Find(ul_Ref))
					{
						AI2C_go_IncludeRef.AddTail(ul_Ref);
						L_strcpy(az_Model, BIG_NameFile(ul_Ref));
						*L_strchr(az_Model, '.') = 0;
						L_strcat(az_Model, ".h");
						sprintf(az_Tmp, "#include \"AI2C_ofc/%s\"\n", az_Model);
						PUTS(az_Tmp);
					}
				}
				else
				{
					/* Get engine model */
					L_strcpy(az_Model, BIG_NameFile(ul_Ref));
					*L_strrchr(az_Model, '.') = 0;
					L_strcat(az_Model, ".omd");
					BIG_ComputeFullName(BIG_ParentFile(ul_Ref), az_Tmp1);
					L_strcat(az_Tmp1, "/");
					L_strcat(az_Tmp1, az_Model);
					*strrchr(az_Tmp1, '.') = 0;
					key = BIG_ul_SearchFileExt(az_Tmp1, az_Model);
					key = BIG_FileKey(key);

					AI2C_go_MapRef.SetAt((void *) key, (void *) key);
					sprintf(az_Model, "%sREF_%x", AI2C_gaz_GlobalRef, key);
					INSERTS(cpt, az_Model);
				}
			}
			else if(!L_strcmpi(pz, ".oin")) /* Univ */
			{
				cpt1 = cpt;
				while(gpz_GlobalBuf[cpt1] != '@') cpt1--;
				while(L_isspace(gpz_GlobalBuf[cpt])) cpt++;
				DELETES(cpt1, cpt - cpt1);

				L_strcpy(az_Model, BIG_NameFile(ul_Ref));
				*L_strrchr(az_Model, '.') = 0;
				L_strcat(az_Model, "_Vars");

				sprintf(az_Tmp, "((%s *) %sREF_%x)->", az_Model, AI2C_gaz_GlobalRef, key);
				INSERTS(cpt1, az_Tmp);
				cpt = cpt1;

				AI2C_go_MapRef.SetAt((void *) key, (void *) key);

				/* Include model (insert mode is active, so at the beginning) */
				if(!AI2C_go_IncludeRef.Find(ul_Ref))
				{
					AI2C_go_IncludeRef.AddTail(ul_Ref);
					L_strcpy(az_Model, BIG_NameFile(ul_Ref));
					*L_strchr(az_Model, '.') = 0;
					L_strcat(az_Model, ".h");
					sprintf(az_Tmp, "#include \"AI2C_ofc/%s\"\n", az_Model);
					PUTS(az_Tmp);
				}
			}
			else
			{
				AI2C_go_MapRef.SetAt((void *) key, (void *) key);
				sprintf(az_Model, "%sREF_%x", AI2C_gaz_GlobalRef, key);
				INSERTS(cpt, az_Model);
			}
		}

		cpt++;
	}

	PUTS("\n");
	PUTS("//////////////////////////////////////////////////////////\n");
	PUTS("//////////////////////////////////////////////////////////\n");

	pos = AI2C_go_MapRef.GetStartPosition();
	while(pos)
	{
		AI2C_go_MapRef.GetNextAssoc(pos, (void * &) key, (void * &) p);
		file = BIG_ul_SearchKeyToFat(key);
		if(BIG_b_IsFileExtension(file, ".gao"))
			sprintf(az_Model, "static OBJ_tdst_GameObject\t*%sREF_%x;\n", AI2C_gaz_GlobalRef, key);
		else if(BIG_b_IsFileExtension(file, ".ofc"))
			sprintf(az_Model, "static AI_tdst_Function\t\t*%sREF_%x;\n", AI2C_gaz_GlobalRef, key);
		else if(BIG_b_IsFileExtension(file, ".omd"))
			sprintf(az_Model, "static AI_tdst_Model\t\t\t*%sREF_%x;\n", AI2C_gaz_GlobalRef, key);
		else
			/* Instance vars */
			sprintf(az_Model, "static char\t\t\t\t\t*%sREF_%x;\n", AI2C_gaz_GlobalRef, key);

		PUTS(az_Model);
	}

	PUTS("\nint ");
	PUTS(AI2C_gaz_InitFct);
	PUTS("(void) {\n");

	pos = AI2C_go_MapRef.GetStartPosition();
	while(pos)
	{
		AI2C_go_MapRef.GetNextAssoc(pos, (void * &) key, (void * &) p);
		file = BIG_ul_SearchKeyToFat(key);
		if(BIG_b_IsFileExtension(file, ".gao"))
		{
			sprintf
			(
				az_Model,
				"\t%sREF_%x = (OBJ_tdst_GameObject *) AI_ul_RealResolveOneRefNoType(0x%x);\n",
				AI2C_gaz_GlobalRef,
				key,
				key
			);
		}
		else if(BIG_b_IsFileExtension(file, ".ofc"))
		{
			sprintf
			(
				az_Model,
				"\t%sREF_%x = (AI_tdst_Function *) AI_ul_RealResolveOneRefNoType(0x%x);\n",
				AI2C_gaz_GlobalRef,
				key,
				key
			);
		}
		else if(BIG_b_IsFileExtension(file, ".omd"))
		{
			sprintf
			(
				az_Model,
				"\t%sREF_%x = (AI_tdst_Model *) AI_ul_RealResolveOneRefNoType(0x%x);\n",
				AI2C_gaz_GlobalRef,
				key,
				key
			);
		}
		else
		{	/* Instance vars */
			sprintf
			(
				az_Model,
				"\t%sREF_%x = ((AI_tdst_Instance *) AI_ul_RealResolveOneRefNoType(0x%x))->pc_VarsBuffer;\n",
				AI2C_gaz_GlobalRef,
				key,
				key
			);
		}

		PUTS(az_Model);
	}

	PUTS("\tRETURN(0);\n");
	PUTS("}\n\n");

	AI2C_gi_GlInsertMode = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_PostProcess(BOOL proclist)
{
	/*~~~~~~~~~~~~~~*/
	int		cpt, cpt1;
	int		countpar;
	int		len;
	int		val;
	char	az[64];
	/*~~~~~~~~~~~~~~*/

	cpt = 0;
	countpar = 0;
	while(gpz_GlobalBuf[cpt])
	{
		if(gpz_GlobalBuf[cpt] == '(') countpar++;
		if(gpz_GlobalBuf[cpt] == ')') countpar--;
		if(gpz_GlobalBuf[cpt] == '{') countpar = 0;
		if(gpz_GlobalBuf[cpt] == '}') countpar = 0;

		/* Zap strings */
		if(gpz_GlobalBuf[cpt] == '"')
		{
			cpt++;
			while(gpz_GlobalBuf[cpt] != '"')
			{
				/*
				 * Les chaines contiennent des \ pour le format STR => ajout d'un autre pour ne
				 * pas perturber le compilo C
				 */
				if((gpz_GlobalBuf[cpt] == '\\') && (gpz_GlobalBuf[cpt + 1] != '\\') && (gpz_GlobalBuf[cpt - 1] != '\\'))
				{
					INSERTS(cpt, "\\");
					cpt++;
				}

				cpt++;
			}

			cpt++;
			continue;
		}

		/* Eliminate ->DV-> */
		if((gpz_GlobalBuf[cpt] == '-') && (gpz_GlobalBuf[cpt + 1] == '>'))
		{
			cpt += 2;
			while(gpz_GlobalBuf[cpt] && L_isspace(gpz_GlobalBuf[cpt])) cpt++;
			if((gpz_GlobalBuf[cpt] == 'D') && (gpz_GlobalBuf[cpt + 1] == 'V') && (gpz_GlobalBuf[cpt + 2] == '-'))
			{
				gpz_GlobalBuf[cpt] = ' ';
				gpz_GlobalBuf[cpt + 1] = ' ';
				gpz_GlobalBuf[cpt + 2] = ' ';
				gpz_GlobalBuf[cpt + 3] = ' ';
			}
		}


		if(!L_strncmp(&gpz_GlobalBuf[cpt], ".des_flags", strlen(".des_flags")))
		{
			DELETES(cpt, strlen(".des_flags"));
			INSERTS(cpt, "->pst_Extended->pst_Design->flags");
		}

		if(!L_strncmp(&gpz_GlobalBuf[cpt], ".des_int1", strlen(".des_int1")))
		{
			DELETES(cpt, strlen(".des_int1"));
			INSERTS(cpt, "->pst_Extended->pst_Design->i1");
		}

		if(!L_strncmp(&gpz_GlobalBuf[cpt], ".des_int2", strlen(".des_int2")))
		{
			DELETES(cpt, strlen(".des_int2"));
			INSERTS(cpt, "->pst_Extended->pst_Design->i2");
		}

		if(!L_strncmp(&gpz_GlobalBuf[cpt], ".des_float1", strlen(".des_float1")))
		{
			DELETES(cpt, strlen(".des_float1"));
			INSERTS(cpt, "->pst_Extended->pst_Design->f1");
		}

		if(!L_strncmp(&gpz_GlobalBuf[cpt], ".des_float2", strlen(".des_float2")))
		{
			DELETES(cpt, strlen(".des_float2"));
			INSERTS(cpt, "->pst_Extended->pst_Design->f2");
		}

		if(!L_strncmp(&gpz_GlobalBuf[cpt], ".des_vec1", strlen(".des_vec1")))
		{
			DELETES(cpt, strlen(".des_vec1"));
			INSERTS(cpt, "->pst_Extended->pst_Design->st_Vec1");
		}
		if(!L_strncmp(&gpz_GlobalBuf[cpt], ".des_vec2", strlen(".des_vec2")))
		{
			DELETES(cpt, strlen(".des_vec2"));
			INSERTS(cpt, "->pst_Extended->pst_Design->st_Vec2");
		}
		if(!L_strncmp(&gpz_GlobalBuf[cpt], ".des_object1", strlen(".des_object1")))
		{
			DELETES(cpt, strlen(".des_object1"));
			INSERTS(cpt, "->pst_Extended->pst_Design->pst_Perso1");
		}
		if(!L_strncmp(&gpz_GlobalBuf[cpt], ".des_object2", strlen(".des_object2")))
		{
			DELETES(cpt, strlen(".des_object2"));
			INSERTS(cpt, "->pst_Extended->pst_Design->pst_Perso2");
		}
		if(!L_strncmp(&gpz_GlobalBuf[cpt], ".des_net1", strlen(".des_net1")))
		{
			DELETES(cpt, strlen(".des_net1"));
			INSERTS(cpt, "->pst_Extended->pst_Design->pst_Net1");
		}
		if(!L_strncmp(&gpz_GlobalBuf[cpt], ".des_net2", strlen(".des_net2")))
		{
			DELETES(cpt, strlen(".des_net2"));
			INSERTS(cpt, "->pst_Extended->pst_Design->pst_Net2");
		}
		if(!L_strncmp(&gpz_GlobalBuf[cpt], ".des_text1", strlen(".des_text1")))
		{
			DELETES(cpt, strlen(".des_text1"));
			INSERTS(cpt, "->pst_Extended->pst_Design->st_Text1");
		}
		if(!L_strncmp(&gpz_GlobalBuf[cpt], ".des_text2", strlen(".des_text2")))
		{
			DELETES(cpt, strlen(".des_text2"));
			INSERTS(cpt, "->pst_Extended->pst_Design->st_Text2");
		}
		if(!L_strncmp(&gpz_GlobalBuf[cpt], ".des_int3", strlen(".des_int3")))
		{
			DELETES(cpt, strlen(".des_int3"));
			INSERTS(cpt, "->pst_Extended->pst_Design->i3");
		}
		if(!L_strncmp(&gpz_GlobalBuf[cpt], ".des_desflags", strlen(".des_desflags")))
		{
			DELETES(cpt, strlen(".des_desflags"));
			INSERTS(cpt, "->pst_Extended->pst_Design->ui_DesignFlags");
		}


		/* Conversion de nombres */
		if(gpz_GlobalBuf[cpt] == '0')
		{
			if(!isalpha(gpz_GlobalBuf[cpt - 1]) && !L_isdigit(gpz_GlobalBuf[cpt - 1]) && gpz_GlobalBuf[cpt - 1] != '_')
			{
				if(L_toupper(gpz_GlobalBuf[cpt + 1]) == 'B')
				{
					val = 0;
					cpt1 = cpt + 2;
					while(gpz_GlobalBuf[cpt1] == '0' || gpz_GlobalBuf[cpt1] == '1')
					{
						val <<= 1;
						if(gpz_GlobalBuf[cpt1] == '1') val++;
						cpt1++;
					}

					DELETES(cpt, cpt1 - cpt);
					sprintf(az, "0x%x", val);
					INSERTS(cpt, az);
					cpt += strlen(az);
					continue;
				}
				else if(L_toupper(gpz_GlobalBuf[cpt + 1]) == 'X')
				{
					cpt += 2;
					while(isxdigit(gpz_GlobalBuf[cpt])) cpt++;
					continue;
				}
				else
				{
					while(gpz_GlobalBuf[cpt] == '0' && L_isdigit(gpz_GlobalBuf[cpt + 1])) gpz_GlobalBuf[cpt++] = ' ';
				}
			}
		}

		/* Add 'f' */
		if(L_isdigit(gpz_GlobalBuf[cpt]))
		{
			if(!isalpha(gpz_GlobalBuf[cpt - 1]) && !L_isdigit(gpz_GlobalBuf[cpt - 1]) && gpz_GlobalBuf[cpt - 1] != '_')
			{
				while(L_isdigit(gpz_GlobalBuf[cpt])) cpt++;
				if(gpz_GlobalBuf[cpt] == '.' && L_isdigit(gpz_GlobalBuf[cpt + 1]))
				{
					cpt++;
					while(L_isdigit(gpz_GlobalBuf[cpt])) cpt++;
					INSERTS(cpt, "f");
					if(!gpz_GlobalBuf[cpt]) break;
				}
			}
		}

		/* Ajout d'un return apres le curchangenow */
		else if(!L_strncmp(&gpz_GlobalBuf[cpt], "AI_EvalFunc_AITrackCurChangeNow_C", 33))
		{
			INSERTS(cpt, "{ ");
			while(gpz_GlobalBuf[cpt] != ';') cpt++;
			INSERTS(cpt, "; RETURN(AI_CR_CurChangeNow); }");
		}

		/* Ajout d'un return apres le objdestroy */
		else if(!proclist && gpz_GlobalBuf[cpt - 1] != '=' && !L_strncmp(&gpz_GlobalBuf[cpt], "AI_EvalFunc_OBJDestroy_C", 24))
		{
			INSERTS(cpt, "{ if(__resexec__ =");
			cpt1 = cpt;
			while(gpz_GlobalBuf[cpt1] != ';') cpt1++;
			INSERTS(cpt1, ") RETURN(__resexec__); }");
		}
		else if(!proclist && !L_strncmp(&gpz_GlobalBuf[cpt], "return", 6))
		{
			if(!L_isalnum(gpz_GlobalBuf[cpt + 6]))
			{
				cpt += 6;
				INSERTS(cpt, " 0");
			}
		}
		else if(!L_strncmp(&gpz_GlobalBuf[cpt], "byrefarr", 8))
		{
			DELETES(cpt, 8);
			while(L_isspace(gpz_GlobalBuf[cpt])) cpt++;	/* Goto type */
			while(!L_isspace(gpz_GlobalBuf[cpt])) cpt++;	/* Zap type */
			while(L_isspace(gpz_GlobalBuf[cpt])) cpt++;	/* Goto type */
			INSERTS(cpt, "*");
		}
		else if(!L_strncmp(&gpz_GlobalBuf[cpt], "byref", 5))
		{
			DELETES(cpt, 5);
			while(L_isspace(gpz_GlobalBuf[cpt])) cpt++;	/* Goto type */
			while(!L_isspace(gpz_GlobalBuf[cpt])) cpt++;	/* Zap type */
			while(L_isspace(gpz_GlobalBuf[cpt])) cpt++;	/* Goto type */
			INSERTS(cpt, "&");
		}

		/* By reference */
		else if(proclist && countpar)
		{
			len = 0;
			if(!L_strncmp(&gpz_GlobalBuf[cpt], "messageid", 9))
				len = 9;
			else if(!L_strncmp(&gpz_GlobalBuf[cpt], "text", 4))
				len = 4;
#if 0
			else if(!L_strncmp(&gpz_GlobalBuf[cpt], "message", 7))
				len = 7;
			else if(!L_strncmp(&gpz_GlobalBuf[cpt], "vector", 6))
				len = 6;
#endif
			if(len && !L_isalnum(gpz_GlobalBuf[cpt - 1]) && !L_isalnum(gpz_GlobalBuf[cpt + len]))
			{
				cpt1 = cpt;
				cpt1 += len;
				while(L_isspace(gpz_GlobalBuf[cpt1])) cpt1++;
				if(gpz_GlobalBuf[cpt1] != '&')
				{
					INSERTS(cpt1, "& ");
					INSERTS(cpt, "const ");
					cpt = cpt1;
				}
			}
		}

		cpt++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_PassGC(void)
{
	/*~~~~~~~~~~~~~~~~~*/
//	char	az_Tmp[1024];
	/*~~~~~~~~~~~~~~~~~*/

//	sprintf(az_Tmp, "-file-\"%s\"", AI2C_gaz_GlobalName);
//	ShellExecute(NULL, "open", "d:/tools/gc.exe", az_Tmp, NULL, SW_HIDE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_EndCompute(EAI_cl_Compiler *po_Compiler, BIG_INDEX ul_Model, BOOL proclist)
{
	/*~~~~~~~~~~~~~~*/
	char	*pz, *pz1;
	int		begline;
	/*~~~~~~~~~~~~~~*/

	/* Process */
	L_strcpy(AI2C_gaz_GlobalRef, BIG_NameFile(ul_Model));
	*L_strrchr(AI2C_gaz_GlobalRef, '.') = 0;
	AI2C_CheckName(AI2C_gaz_GlobalRef);
	AI2C_go_MapRef.RemoveAll();

	AI2C_ProcessUltra(po_Compiler);
	AI2C_MakeRefs(ul_Model, proclist);

	/* Move ';' */
	pz = gpz_GlobalBuf;
	while(*pz)
	{
		if(*pz == ';')
		{
			pz1 = pz - 1;
			if(*pz1 == ' ' || *pz1 == '\t')
			{
				while(pz1 != gpz_GlobalBuf && (*pz1 == ' ' || *pz1 == '\t')) pz1--;
				pz1++;
				*pz = *pz1;
				*pz1 = ';';
				pz = pz1;
				continue;
			}

			if(*pz1 == '\n')
			{
				while(pz1 != gpz_GlobalBuf && *pz1 == '\n') pz1--;
				pz1++;
				*pz1 = ';';
				*pz = '\n';
				pz = pz1;
				continue;
			}
		}

		pz++;
	}

	AI2C_PostProcess(proclist);

	/* Output */
	pz = gpz_GlobalBuf;
	begline = 1;
	while(*pz)
	{
		if((pz[0] == ' ' || pz[0] == '\t') && (pz[1] == ' ' || pz[1] == '\t') && !begline)
		{
			pz++;
			continue;
		}

		if(*pz == '}' && pz[1] == ';') pz[1] = ' '; /* Eliminate }; */
		if(*pz == ';' && pz[1] == ';') pz[1] = ' '; /* Eliminate ;; */

		if(*pz == '\n')
			begline = 1;
		else if(pz[0] != ' ' && pz[0] != '\t')
			begline = 0;
		else if(!begline)
			*pz = ' ';
		fputc(*pz, AI2C_gf_Model);
		pz++;
	}

	fclose(AI2C_gf_Model);
	AI2C_PassGC();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_GenerateLib(EAI_cl_Compiler *po_Compiler, BIG_INDEX ul_Model, BIG_INDEX ul_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		*pc_Buf, *pz, *pz1;
	ULONG		ul_Size;
	char		az_Path[1024];
	char		az_Name[1024];
	char		az_NameModel[1024];
	char		az_Tmp[1024];
	char		az_Cast[1024];
	BIG_INDEX	ul_File1;
	POSITION	pos;
	int			numtrigger;
	char		az_NameProc[128];
	int			cpt, cpt1, cpt2, local;
	int			param;
	CString		oname;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	numtrigger = 0;
	gpi_GlobalSize = 0;
	AI2C_gi_GlInsert = 0;
	AI2C_gi_GlInsertMode = 0;
	AI2C_go_IncludeRef.RemoveAll();

	/* Global path */
	strcpy(az_Path, AI_PATH);
	strcat(az_Path, "AI2C_ofc");
	_mkdir(az_Path);
	strcat(az_Path, "/");

	/* Generate file */
	L_strcpy(az_NameModel, BIG_NameFile(ul_File));
	*L_strrchr(az_NameModel, '.') = 0;
    AI2C_CheckName(az_NameModel);
	sprintf(az_Name, "%s.cpp", az_NameModel);

	if(_access(az_Name, 2)) _chmod(az_Name, _S_IWRITE);
	L_strcat(az_Path, az_Name);
	AI2C_AddToFctListName(az_Name);

	/* Get PP file */
	L_strcpy(az_Name, BIG_NameFile(ul_File));
	L_strcat(az_Name, ".pp");
	ul_File1 = BIG_ul_SearchFile(BIG_ParentFile(ul_File), az_Name);
	pc_Buf = BIG_pc_ReadFileTmpMustFree(BIG_PosFile(ul_File1), &ul_Size);

	AI2C_FirstScanFile(pc_Buf, TRUE);
	free(pc_Buf);

	/* Procedures locales */
	L_strcpy(az_Cast, BIG_NameFile(ul_Model));
	*L_strrchr(az_Cast, '.') = 0;
    AI2C_CheckName(az_Cast);
	cpt = 0;
	local = 0;
	while(gpz_GlobalBuf[cpt])
	{
		if(!strncmp(&gpz_GlobalBuf[cpt], "procedure_local", 15))
		{
			while(gpz_GlobalBuf[cpt] != '{') 
			{
				if(gpz_GlobalBuf[cpt] == ';') goto lbreak;
				cpt++;
			}

			cpt++;
			sprintf(az_Tmp, "\t%s_Vars\t*DV = (%s_Vars *) (AI_gpst_CurrentInstance->pc_VarsBuffer);\n", az_Cast, az_Cast);
			INSERTS(cpt, az_Tmp);
			cpt += strlen(az_Tmp);
			sprintf(az_Tmp, "\tOBJ_tdst_GameObject\t*__who = AI_gpst_CurrentGameObject;\n");
			INSERTS(cpt, az_Tmp);
			local = 1;
		}
		else if(!strncmp(&gpz_GlobalBuf[cpt], "procedure_ultra", 15))
		{
			while(gpz_GlobalBuf[cpt] != '(') 
			{
				if(gpz_GlobalBuf[cpt] == ';') goto lbreak;
				cpt++;
			}

			// Get name of procedure
			oname.Empty();
			cpt2 = cpt;
			while(!L_isspace(gpz_GlobalBuf[cpt2 - 1])) cpt2--;
			while(cpt2 != cpt && !L_isspace(gpz_GlobalBuf[cpt2])) oname += gpz_GlobalBuf[cpt2++];
			AI2C_go_ProcUltraList.AddTail(oname);

			cpt++;
			INSERTS(cpt, "OBJ_tdst_GameObject *__who");
			cpt1 = cpt + 26;
			cpt2 = cpt + 26;

			param = 0;
			while(gpz_GlobalBuf[cpt2] != ')') 
			{
				if(!L_isspace(gpz_GlobalBuf[cpt2]) && gpz_GlobalBuf[cpt2] != ')' && gpz_GlobalBuf[cpt2] != '(')
				{
					param = 1;
					break;
				}
                cpt2++;
			}

			if(param) 
			{
				INSERTS(cpt1, ",");
				cpt++;
			}

			while(gpz_GlobalBuf[cpt] != '{' && gpz_GlobalBuf[cpt] != ';') 
				cpt++;
			if(gpz_GlobalBuf[cpt] == ';') goto lbreak;

			cpt++;
			sprintf(az_Tmp, "\t%s_Vars\t*DV = (%s_Vars *) (__who->pst_Extended->pst_Ai->pc_VarsBuffer);\n", az_Cast, az_Cast);
			INSERTS(cpt, az_Tmp);
			sprintf(az_Tmp, "\t_StoreContext_ _Cxt_(__who);\n");
			INSERTS(cpt, az_Tmp);
			local = 1;
		}
		else if(!strncmp(&gpz_GlobalBuf[cpt], "procedure", 9))
		{
			while(gpz_GlobalBuf[cpt] != '{') 
			{
				if(gpz_GlobalBuf[cpt] == ';') goto lbreak;
				cpt++;
			}

			cpt++;
			sprintf(az_Tmp, "\tOBJ_tdst_GameObject\t*__who = AI_gpst_CurrentGameObject;\n");
			INSERTS(cpt, az_Tmp);
		}

lbreak:
		cpt++;
	}

	/* Proto init */
	L_strcpy(AI2C_gaz_InitFct, BIG_NameFile(ul_File));
	*L_strrchr(AI2C_gaz_InitFct, '.') = 0;
    AI2C_CheckName(AI2C_gaz_InitFct);

	L_strcpy(AI2C_gaz_GlobalName, az_Path);
	AI2C_gf_Model = fopen(az_Path, "w");
	while(!AI2C_gf_Model) AI2C_gf_Model = fopen(az_Path, "w");

	/* Include current model */
	if(local)
	{
	    sprintf(az_Tmp, "#include \"AI2C_ofc/%s.h\"\n", az_Cast);
		fputs(az_Tmp, AI2C_gf_Model);
	}

	/* Include other libs */
	pos = AI2C_go_LibIncludeRef.GetHeadPosition();
	while(pos)
	{
		ul_File1 = AI2C_go_LibIncludeRef.GetNext(pos);
		L_strcpy(az_NameModel, BIG_NameFile(ul_File1));
		*L_strrchr(az_NameModel, '.') = 0;
        AI2C_CheckName(az_NameModel);
        sprintf(az_Tmp, "#include \"AI2C_ofc/%s.h\"\n", az_NameModel);
		fputs(az_Tmp, AI2C_gf_Model);
	}

	AI2C_go_LibIncludeRef.AddTail(ul_File);

	/* Scan */
	AI2C_EndCompute(local ? po_Compiler : NULL, ul_File, TRUE);

	/* Generate .h */
	*L_strrchr(az_Path, '.') = 0;
	L_strcat(az_Path, ".h");
	if(_access(az_Path, 2)) _chmod(az_Path, _S_IWRITE);

	L_strcpy(AI2C_gaz_GlobalName, az_Path);
	AI2C_gf_Model = fopen(az_Path, "w");

	/* Include mark */
	L_strcpy(az_Name, BIG_NameFile(ul_File));
	*L_strrchr(az_Name, '.') = 0;
	sprintf(az_Path, "#ifndef __%s_H__\n#define __%s_H__\n\n", az_Name, az_Name);
	fputs(az_Path, AI2C_gf_Model);

	/* Write */
	pc_Buf = gpz_GlobalBuf;
	while(*pc_Buf)
	{
		if(!strncmp(pc_Buf, "procedure_trigger", 17))
		{
			pz = pc_Buf + 17;
			while(L_isspace(*pz)) pz++;
			while(!L_isspace(*pz)) pz++;	/* return type */
			while(L_isspace(*pz)) pz++;

			pz1 = az_NameProc;
			while(!L_isspace(*pz) && *pz != '(') *pz1++ = *pz++;
			*pz1 = 0;
			AI2C_AddTriggerToDef(ul_File, az_NameProc, numtrigger++);
		}
		else if(!strncmp(pc_Buf, "procedure", 9))
		{
			pz = pc_Buf;
			while(*pz != ')') pz++;
			pz++;
			fputs("extern ", AI2C_gf_Model);
			fwrite(pc_Buf, pz - pc_Buf, 1, AI2C_gf_Model);
			fputs(";\n", AI2C_gf_Model);
			pc_Buf = pz;
		}
		else if(!strncmp(pc_Buf, "procedure_local", 15))
		{
			pz = pc_Buf;
			while(*pz != ')') pz++;
			pz++;
			fputs("extern ", AI2C_gf_Model);
			fwrite(pc_Buf, pz - pc_Buf, 1, AI2C_gf_Model);
			fputs(";\n", AI2C_gf_Model);
			pc_Buf = pz;
		}

		pc_Buf++;
	}

	/* End include mark */
	sprintf(az_Path, "\n#endif /* __%s_H__ */\n", az_Name);
	fputs(az_Path, AI2C_gf_Model);

	fclose(AI2C_gf_Model);
	AI2C_PassGC();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_Reinit(void)
{
	AI2C_go_DoneFct.RemoveAll();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI2C_ModelToC(EAI_cl_Compiler *po_Compiler, BIG_INDEX ul_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	char		*pc_Buf;
	ULONG		i, ul_Size;
	ULONG		*pul_Buf;
	ULONG		ul_FileExt;
	BIG_INDEX	ul_Var;
	char		az_Tmp[1024];
	char		az_H[128];
	int			p;
	POSITION	pos;
	static int	i_first = 1;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	AI2C_go_IncludeRef.RemoveAll();
	AI2C_go_MapRef.RemoveAll();
	AI2C_go_ProcListRef.RemoveAll();
	AI2C_go_ProcUltraList.RemoveAll();
	AI2C_go_LibIncludeRef.RemoveAll();
	gpi_GlobalSize = 0;
	AI2C_gi_Label = 1;

	/* Generate all C++ functions */
	if(i_first)
	{
		AI2C_GenFctList();
		i_first = 0;
	}

	/* Init model */
	AI2C_AddToDef(ul_File);

	/* Fctdefs */
	pc_Buf = BIG_pc_ReadFileTmpMustFree(BIG_PosFile(ul_File), &ul_Size);
	pul_Buf = (ULONG *) pc_Buf;
	for(i = 0; i < (ul_Size >> 3); ++i)
	{
		ul_Var = *pul_Buf++;
		ul_FileExt = *pul_Buf++;
		if(ul_FileExt == *((ULONG *) ".fct"))
		{
			ul_Var = BIG_ul_SearchKeyToFat(ul_Var);
			AI2C_AddToDef(ul_Var);
		}
		else if(ul_FileExt == *((ULONG *) ".fcl"))
		{
			ul_Var = BIG_ul_SearchKeyToFat(ul_Var);
			AI2C_AddToDef(ul_Var);
			AI2C_go_ProcListRef.SetAt((void *) ul_Var, (void *) ul_Var);
			AI2C_GenerateLib(po_Compiler, ul_File, ul_Var);
		}
	}

	AI2C_go_IncludeRef.RemoveAll();
	free(pc_Buf);

	/* Proto init */
	L_strcpy(AI2C_gaz_InitFct, BIG_NameFile(ul_File));
	*L_strrchr(AI2C_gaz_InitFct, '.') = 0;
    AI2C_CheckName(AI2C_gaz_InitFct);

	/* Generation du cpp */
	AI2C_AddToFctList(ul_File);

	/* Generation fichier cpp complet */
	gpi_GlobalSize = 0;
	L_strcpy(az_H, BIG_NameFile(ul_File));
	*L_strchr(az_H, '.') = 0;
	AI2C_CheckName(az_H);
	L_strcat(az_H, ".h");
	sprintf(az_Tmp, "#include \"AI2C_ofc/%s\"\n", az_H);
	PUTS(az_Tmp);

	/* Include global lib */
	pos = AI2C_go_ProcListRef.GetStartPosition();
	while(pos)
	{
		AI2C_go_ProcListRef.GetNextAssoc(pos, (void * &) ul_Var, (void * &) p);
		L_strcpy(az_H, BIG_NameFile(ul_Var));
		*L_strrchr(az_H, '.') = 0;
		AI2C_CheckName(az_H);
		sprintf(az_Tmp, "#include \"AI2C_ofc/%s.h\"\n", az_H);
		PUTS(az_Tmp);
	}

	AI2C_gi_GlInsert = gpi_GlobalSize;

	pc_Buf = BIG_pc_ReadFileTmpMustFree(BIG_PosFile(ul_File), &ul_Size);
	pul_Buf = (ULONG *) pc_Buf;
	for(i = 0; i < (ul_Size >> 3); ++i)
	{
		ul_Var = *pul_Buf++;
		ul_FileExt = *pul_Buf++;
		if(ul_FileExt == *((ULONG *) ".fct"))
		{
			ul_Var = BIG_ul_SearchKeyToFat(ul_Var);
			if(!AI2C_go_DoneFct.Find(ul_Var))
			{
				AI2C_GenFunc(ul_File, ul_Var);
				AI2C_go_DoneFct.AddTail(ul_Var);
			}
		}
	}

	free(pc_Buf);
	AI2C_EndCompute(po_Compiler, ul_File, FALSE);
}
#endif /* ACTIVE_EDITORS */
