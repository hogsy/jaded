/*$T AIcompile_ref.cpp GC!1.71 01/24/00 09:41:49 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "../AIframe.h"
#include "../AIerrid.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGfat.h"
#include "EDIpaths.h"
#include "ENGine/Sources/TEXT/TEXT.h"
#include "ENGine/Sources/TEXT/TEXTstruct.h"
#include "ENGine/Sources/TEXT/TEXTload.h"
#include "BIGfiles/LOAding/LOAdefs.h"

extern CMapPtrToPtr	go_PPAllRef;
extern int gi_PPAllPVBeforeNext;
extern int gi_PPAllPV;
#define MGP() if(mb_GenPP) go_PPAllRef.SetAt((void *) (gi_PPAllPV - 1), (void *) e.i_Value); /* Marquage ref */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::GetReference(void)
{
    /*~~~~~~~~~~~~~~~~~~*/
    BOOL    b_Res;
    char    *psz_Temp;
	ULONG	eng;
    /*~~~~~~~~~~~~~~~~~~*/

    /*$2
     -------------------------------------------------------------------------------------------------------------------
        Is it a reference depending on requested type ?
     -------------------------------------------------------------------------------------------------------------------
     */

    switch(mi_RequestedType)
    {
    case TYPE_FUNCTIONREF:
    case TYPE_GAMEOBJECT:
        ERR_X_Error(mpst_CurrentModel != NULL, ERR_COMPILER_Csz_RefNotLoaded, (char *) (LPCSTR) e.o_Value);
        break;
    }

    /*$2
     -------------------------------------------------------------------------------------------------------------------
        Treat all references
     -------------------------------------------------------------------------------------------------------------------
     */

    switch(mi_RequestedType)
    {
    case TYPE_FUNCTIONREF:
        psz_Temp = (char *) (LPCSTR) e.o_Value;
        psz_Temp = L_strrchr(psz_Temp, '.');
        if(psz_Temp)
        {
            b_Res = b_GetAIFunction(NULL);
        }
        else
        {
            b_Res = b_GetAIFunction(EDI_Csz_ExtAIEditorFct);
        }

        ERR_X_Error(b_Res, ERR_COMPILER_Csz_UnkownRef, (char *) (LPCSTR) e.o_Value);

		eng = ul_GetEngineFileForForFunction(e.i_Value);
		if(mb_GenPP) go_PPAllRef.SetAt((void *) (gi_PPAllPV - 1), (void *) eng); /* Marquage ref */
        break;

    case TYPE_NETWORK:
        b_Res = b_GetNetwork();
        ERR_X_Error(b_Res, ERR_COMPILER_Csz_UnkownRef, (char *) (LPCSTR) e.o_Value);
		MGP();
        break;

    case TYPE_STRING:
        break;

    case TYPE_GAMEOBJECT:
    default:
        b_Res = b_GetEngineObject();
        if(!b_Res)  /* Model ? */
        {
            b_Res = b_GetAIModel();
            if(b_Res) 
			{
				if(mb_GenPP) go_PPAllRef.SetAt((void *) (gi_PPAllPV - 1), (void *) (BIG_FileKey(e.i_Value))); /* Marquage ref */
				return;
			}
        }

        ERR_X_Error(b_Res, ERR_COMPILER_Csz_UnkownRef, (char *) (LPCSTR) e.o_Value);
		MGP();
        break;
    }

    /* Reset to default request type */
    mi_RequestedType = TYPE_GAMEOBJECT;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Compiler::b_GetAIModel(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_File;
    char        asz_Name[BIG_C_MaxLenPath];
    char        asz_Path[BIG_C_MaxLenPath];
    char        *psz_Temp;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    L_strcpy(asz_Name, (char *) (LPCSTR) e.o_Value);
    if(!L_strrchr(asz_Name, '.')) L_strcat(asz_Name, EDI_Csz_ExtAIEditorModel);

    L_strcpy(asz_Path, EDI_Csz_Path_AIModels);
    psz_Temp = L_strrchr(asz_Name, '/');
    if(psz_Temp)
    {
        *psz_Temp = 0;
        L_strcat(asz_Path, "/");
        L_strcat(asz_Path, asz_Name);
        psz_Temp++;
        L_strcpy(asz_Name, psz_Temp);
    }

    ul_File = BIG_ul_SearchFileExt(asz_Path, asz_Name);
    if(ul_File == BIG_C_InvalidIndex) return FALSE;

    e.c_Token = TOKEN_MODEL;
    e.i_Value = ul_File;

    return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Compiler::b_GetEngineObject(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char        *psz_Name, *psz_Temp;
    char        asz_Name[BIG_C_MaxLenPath];
    char        asz_Path[BIG_C_MaxLenPath];
    BIG_INDEX   ul_Index;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    psz_Name = (char *) (LPCSTR) e.o_Value;

    /* Is it for the universe ? */
    if(!L_stricmp(psz_Name, "univ"))
    {
        e.o_Value.Empty();
        e.c_Token = TOKEN_GAMEOBJECT;
        e.i_Value = BIG_gst.st_ToSave.ul_UniverseKey;
        if(e.i_Value == BIG_C_InvalidIndex) return FALSE;
        return TRUE;
    }

    /* Else search the instance in the given level */
    L_strcpy(asz_Path, EDI_Csz_Path_Levels);
    L_strcpy(asz_Name, psz_Name);
    psz_Temp = L_strrchr(asz_Name, '/');
    if(!psz_Temp) return FALSE;
    *psz_Temp = 0;
    L_strcat(asz_Path, "/");
    L_strcat(asz_Path, asz_Name);
    L_strcat(asz_Path, "/");
    L_strcat(asz_Path, EDI_Csz_Path_GameObject);
    psz_Temp++;
    L_strcat(psz_Temp, EDI_Csz_ExtGameObject);

    ul_Index = BIG_ul_SearchFileExt(asz_Path, psz_Temp);
    if(ul_Index == BIG_C_InvalidIndex) return FALSE;

    e.o_Value.Empty();
    e.c_Token = TOKEN_GAMEOBJECT;
    e.i_Value = BIG_FileKey(ul_Index);
    return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Compiler::b_GetNetwork(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char        *psz_Name, *psz_Temp;
    char        asz_Name[BIG_C_MaxLenPath];
    char        asz_Path[BIG_C_MaxLenPath];
    BIG_INDEX   ul_Index;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    psz_Name = (char *) (LPCSTR) e.o_Value;

    /* Else search the instance in the given level */
    L_strcpy(asz_Path, EDI_Csz_Path_Levels);
    L_strcpy(asz_Name, psz_Name);
    psz_Temp = L_strrchr(asz_Name, '/');
    if(!psz_Temp) return FALSE;
    *psz_Temp = 0;
    L_strcat(asz_Path, "/");
    L_strcat(asz_Path, asz_Name);
    L_strcat(asz_Path, "/");
    L_strcat(asz_Path, EDI_Csz_Path_Network);
    psz_Temp++;
    L_strcat(psz_Temp, EDI_Csz_ExtNetWay);

    ul_Index = BIG_ul_SearchFileExt(asz_Path, psz_Temp);
    if(ul_Index == BIG_C_InvalidIndex) return FALSE;

    e.o_Value.Empty();
    e.c_Token = TOKEN_NETWORK;
    e.i_Value = BIG_FileKey(ul_Index);
    return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Compiler::b_GetAIFunction(char *_psz_Ext)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int         i;
    BIG_INDEX   ul_Index;
    char        asz_Name[BIG_C_MaxLenPath];
    char        *psz_Ext1;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    psz_Ext1 = _psz_Ext;
    if(!_psz_Ext) psz_Ext1 = L_strrchr((char *) (LPCSTR) e.o_Value, '.');

    i = BIG_i_GetNextGrpType(mpst_CurrentModel, mi_SizeModel, -1, psz_Ext1);
    while(i != -1)
    {
        ul_Index = BIG_ul_SearchKeyToFat(mpst_CurrentModel[i].ul_Key);
		if(ul_Index == BIG_C_InvalidIndex)
		{
			ERR_X_ForceError("Model seams to be incorrect (bad reference to function or var)", BIG_NameFile(mul_CurrentModel));
			return FALSE;
		}

        /* Add extension */
        L_strcpy(asz_Name, (char *) (LPCSTR) e.o_Value);
        if(_psz_Ext) L_strcat(asz_Name, _psz_Ext);

        if(!L_strcmp(asz_Name, BIG_NameFile(ul_Index)))
        {
            e.o_Value.Empty();
            e.c_Token = TOKEN_AIFUNCTION;
            e.i_Value = mpst_CurrentModel[i].ul_Key;
            return TRUE;
        }

        i = BIG_i_GetNextGrpType(mpst_CurrentModel, mi_SizeModel, i, psz_Ext1);
    }

    return FALSE;
}

