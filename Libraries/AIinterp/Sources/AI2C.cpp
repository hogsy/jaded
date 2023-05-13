#include "Precomp.h"

#ifdef _GAMECUBE
#include <dolphin.h>
#else
#include <vector>
#endif

#include "AI2C_fctheader.h"
#include "AI2C_mdlheader.h"

#ifdef JADEFUSION
extern WOR_tdst_World	*WOR_pst_Universe_GetActiveWorld(int);
#else
extern "C" WOR_tdst_World	*WOR_pst_Universe_GetActiveWorld(int);
#endif


int testtesttrigger(message &);

int (* adrtesttesttrigger)(message &) = &testtesttrigger;

#ifdef JADEFUSION
extern void AI_ResolveMsg(char *, AI_tdst_Message *);
#else
extern "C" void AI_ResolveMsg(char *, AI_tdst_Message *);
#endif

// Définition of AI2C_pfi_XXX for functions and triggers.
#ifdef GAMECUBE_USE_AI2C_DLL
#define AI2C_FCTDEF(a, b)			int (*AI2C_pfi_##b) (void);
#ifdef __cplusplus
class message;
#define AI2C_FCTDEFTRIGGER(a, b, c)	int (*AI2C_pfi_##c) (message &);
#else // __cplusplus
#define AI2C_FCTDEFTRIGGER(a, b, c)	
#endif // __cplusplus

#include "AI2C_fctdefs.h"

#undef AI2C_FCTDEF
#undef AI2C_FCTDEFTRIGGER
#endif // GAMECUBE_USE_AI2C_DLL

extern "C"
{
	const UCHAR *GETCOMPILATIONTIME()
	{
#ifdef AI2C_DATE_GEN		
		return (const UCHAR *)AI2C_DATE_GEN;//,AI2C_TIME_GEN);
#else
		return 0;
#endif 		
	}
	const UCHAR *GETCOMPILATIONDATE()
	{
#ifdef AI2C_DATE_GEN		
		return (const UCHAR *)AI2C_TIME_GEN;
#else
		return 0;
#endif 		
	}
}


#define AI2C_FCTDEF(a, b)
#ifdef GAMECUBE_USE_AI2C_DLL
#define AI2C_FCTDEFTRIGGER(a, b, c) {a, b, &AI2C_pfi_##c, #c},
#else // GAMECUBE_USE_AI2C_DLL
#define AI2C_FCTDEFTRIGGER(a, b, c) {a, b, c, #c},
#endif // GAMECUBE_USE_AI2C_DLL

AI2C_fctdeftrigger AI2C_gat_fctdefstrigger[] =
{
#include "AI2C_fctdefs.h"
#ifdef GAMECUBE_USE_AI2C_DLL
	{BIG_C_InvalidKey, BIG_C_InvalidKey, &adrtesttesttrigger, NULL}
#else //GAMECUBE_USE_AI2C_DLL
	{BIG_C_InvalidKey, BIG_C_InvalidKey, testtesttrigger, NULL}
#endif // GAMECUBE_USE_AI2C_DLL
};
#undef AI2C_FCTDEF
#undef AI2C_FCTDEFTRIGGER


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI2C_i_SizeofArrayTrigger(void)
{
	return sizeof(AI2C_gat_fctdefstrigger) / sizeof(AI2C_fctdeftrigger);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int testtesttrigger(message &)
{
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS

#ifdef JADEFUSION
extern OBJ_tdst_GameObject *AI_gp_ResolveGO;
#else
extern "C"
{
    extern OBJ_tdst_GameObject *AI_gp_ResolveGO;
}
#endif

class AI_tdUser
{
public :
    AI_tdUser() {h_User = 0; s_CallBackName = 0;}
    BIG_KEY h_User;
    char *s_CallBackName;
    bool operator == (const AI_tdUser _sUser) {return (_sUser.h_User == h_User) && (0 == strcmp(_sUser.s_CallBackName,s_CallBackName));}
};


template <class TUsed, class TUser>
struct AI_tdDependency
{   
    TUsed h_Used;
    std::vector<TUser> a_User;
};

typedef  AI_tdDependency<SCR_tt_Trigger *,OBJ_tdst_GameObject *> AI_tdTriggerDependency;
std::vector<AI_tdTriggerDependency>  AI_aTriggerDependencies;

typedef AI_tdDependency<BIG_KEY,AI_tdUser> AI_tdKeyDependency ;
std::vector<AI_tdKeyDependency>  AI_aKeyDependencies;

template <class TUsed, class TUser>
void AI_AddDependency(TUsed h_Used,TUser h_User,std::vector<AI_tdDependency<TUsed,TUser> > &a_Dependencies)
{
    AI_tdDependency<TUsed,TUser> *pDependency = NULL;
    int i;
	for(i = 0; i < (int)a_Dependencies.size(); i++)
    {
        if (a_Dependencies[i].h_Used == h_Used)
        {
            pDependency = &(a_Dependencies[i]);
            break;
        }
    }

    if (!pDependency)
    {
        int iSize = a_Dependencies.size();
        a_Dependencies.resize(iSize + 1);
        pDependency = &(a_Dependencies[iSize]);
        pDependency->h_Used = h_Used;
        pDependency->a_User.resize(0);
    }
    
    for (i=0; i<(int)pDependency->a_User.size(); i++)
    {
        if (pDependency->a_User[i] == h_User)
            break;
    }

    if (i>=(int)pDependency->a_User.size())
    {
        int iSize = pDependency->a_User.size();
        pDependency->a_User.resize(iSize+1);
        pDependency->a_User[iSize] = h_User;
    }

    /*char sMsg[256];
    sprintf(sMsg,"Dependency of %x\n:",pDependency->h_Used);
    OutputDebugString(sMsg);
    for (i=0; i<pDependency->a_User.size(); i++)
    {
        sprintf(sMsg,"%x\n:",pDependency->a_User[i]);
        OutputDebugString(sMsg);
    }*/
}

void AI_vAddKeyDependency(BIG_KEY h_Used,BIG_KEY h_User,char *_s_CallBackName)
{
    AI_tdUser stUser;
    stUser.h_User = h_User;
    stUser.s_CallBackName = _s_CallBackName;

    AI_AddDependency<BIG_KEY,AI_tdUser>(h_Used,stUser,AI_aKeyDependencies);
}

void AI_PrintTriggerDependencies(const char *s_TriggerName)
{
    char sMsg[256];
    LINK_gul_ColorTxt = 0x00FF0000;
	snprintf( sMsg, sizeof(sMsg), "Trigger %s is used by:", s_TriggerName );
    LINK_PrintStatusMsgCanal(sMsg,2);

    int i;
    for (i=0; i<(int)AI_aTriggerDependencies.size(); i++)
    {
        AI_tdTriggerDependency *pTriggerDep = &(AI_aTriggerDependencies[i]);
        char *sTriggerName = pTriggerDep->h_Used->az_Name+1;
        while (*sTriggerName >= '0' && *sTriggerName <= '9')
            sTriggerName++;

        if (L_strstr(s_TriggerName,sTriggerName))
        {
            for (i=0; i<(int)pTriggerDep->a_User.size(); i++)
            {
				snprintf( sMsg, sizeof(sMsg), "%s (%x)", pTriggerDep->a_User[ i ]->sz_Name, pTriggerDep->a_User[ i ]->ul_MyKey );
                LINK_PrintStatusMsgCanal(sMsg,2);
            }
            break;
        }
    }
    LINK_gul_ColorTxt = 0;
}
#ifdef JADEFUSION
extern BIG_KEY g_ul_GroupKey;
#else
extern "C" BIG_KEY g_ul_GroupKey;
#endif
void AI_PrintUnusedGAO()
{
    // Print list of unused GAO (GAO that are loaded but not referenced by IAs or by triggers, 
    // and that are not visible, not active and not duplicates (pst_CurrentGO->ul_MyKey != 0))

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    WOR_tdst_World *pst_World = WOR_pst_Universe_GetActiveWorld(0);
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_CurrentGO;
    char sMsg[256];
    char sWorldPath[256];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    sprintf(sMsg,"GAOs that are loaded but not visible, active or referenced :");
    LINK_PrintStatusMsgCanal(sMsg,0);

    BIG_ComputeFullName(BIG_ParentFile(BIG_ul_SearchKeyToFat(pst_World->h_WorldKey)), sWorldPath);
    
    pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&pst_World->st_AllWorldObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&pst_World->st_AllWorldObjects);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_CurrentGO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if (TAB_b_IsAHole(pst_CurrentGO)) continue;

		if (pst_CurrentGO->ul_MyKey &&
            !(pst_CurrentGO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Active) && 
            !(pst_CurrentGO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Visible))
        {
            // Is the object in the world directory ?
            ULONG ulFat = BIG_ul_SearchKeyToFat(pst_CurrentGO->ul_MyKey);

            if(ulFat != BIG_C_InvalidIndex)
            {
                char asz_Path[256];
                BIG_ComputeFullName(BIG_ParentFile(ulFat), asz_Path);
                if (!strstr(asz_Path,sWorldPath))
                    continue;
            }

            // Is the object referenced ?
            BOOL bIsObjectReferenced = FALSE;
            int i;
            for (i=0; i<(int)AI_aKeyDependencies.size(); i++)
            {
                AI_tdKeyDependency *pKeyDep = &(AI_aKeyDependencies[i]);
                if (pKeyDep->h_Used == pst_CurrentGO->ul_MyKey)
                {
                    if (pKeyDep->a_User.size() == 1)
                        bIsObjectReferenced = (pKeyDep->a_User[0].h_User != g_ul_GroupKey);
                    else
                        bIsObjectReferenced = (pKeyDep->a_User.size() > 0);

                    break;
                }
            }
            if (!bIsObjectReferenced)
            {
                if(ulFat != BIG_C_InvalidIndex)
                {
                    char asz_Path[256];
                    BIG_ComputeFullName(BIG_ParentFile(ulFat), asz_Path);
					snprintf( sMsg, sizeof(sMsg), "%s/%s (key %x)", asz_Path, pst_CurrentGO->sz_Name, pst_CurrentGO->ul_MyKey );
                }
                else
					snprintf( sMsg, sizeof(sMsg), "%s (key %x)", pst_CurrentGO->sz_Name, pst_CurrentGO->ul_MyKey );

                LINK_PrintStatusMsgCanal(sMsg,0);
            }
        }
	}
}

BOOL bPrintIfComesAnotherWorld(ULONG _ulFat,const char *_sName, const char *_sWorldPath,ULONG _ulFatGAO)
{
    char sMsg[256];
    if (_ulFat == BIG_C_InvalidIndex)
        return FALSE;

    char asz_Path[256];
    BIG_ComputeFullName(BIG_ParentFile(_ulFat), asz_Path);

    // If the object is not in the levels directory, it's OK.
    if (!strstr(asz_Path,"ROOT/EngineDatas/06 Levels"))
        return FALSE;

    // If path starts by "ROOT/EngineDatas/06 Levels/_", it's OK
    if (strstr(asz_Path,"ROOT/EngineDatas/06 Levels/_"))
        return FALSE;

    // If path starts by the world path, it's OK
    if (strstr(asz_Path,_sWorldPath))
        return FALSE;

    if (_ulFatGAO == BIG_C_InvalidIndex)
    {
		snprintf( sMsg, sizeof(sMsg), "%s %s/%s comes from another world(dir)", _sName, asz_Path, BIG_NameFile( _ulFat ) );
        LINK_PrintStatusMsgCanal(sMsg,0);
    }
    else
    {
		snprintf( sMsg, sizeof(sMsg), "%s %s/%s used by %s comes from another world(dir)", _sName, asz_Path, BIG_NameFile( _ulFat ), BIG_NameFile( _ulFatGAO ) );
        LINK_PrintStatusMsgCanal(sMsg,0);
    }

    return TRUE;
}

void AI_PrintExternREF()
{
    // Print list of GAO that reference data not in world.

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    WOR_tdst_World *pst_World = WOR_pst_Universe_GetActiveWorld(0);
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_CurrentGO;
    char sWorldPath[256];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    BIG_ComputeFullName(BIG_ParentFile(BIG_ul_SearchKeyToFat(pst_World->h_WorldKey)), sWorldPath);
    
    pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&pst_World->st_AllWorldObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&pst_World->st_AllWorldObjects);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_CurrentGO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if (TAB_b_IsAHole(pst_CurrentGO)) continue;

        // If the GAO has no key, it's a duplicate...
		if (!pst_CurrentGO->ul_MyKey)
            continue;

        // Is the object in the world directory ?
        ULONG ulGAOFat = BIG_ul_SearchKeyToFat(pst_CurrentGO->ul_MyKey);

        if (ulGAOFat != BIG_C_InvalidIndex)
        {
            if (bPrintIfComesAnotherWorld(ulGAOFat,"GAO",sWorldPath,BIG_C_InvalidIndex))
                continue;

            // The GAO is in the world directory : check if gro, map and cob are in its directory.
            // GRO
            if (pst_CurrentGO && pst_CurrentGO->pst_Base && pst_CurrentGO->pst_Base->pst_Visu && pst_CurrentGO->pst_Base->pst_Visu->pst_Object)
            {
                BIG_KEY ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_CurrentGO->pst_Base->pst_Visu->pst_Object);
                if (ul_Key != BIG_C_InvalidIndex)
                {
                    ULONG ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
                    bPrintIfComesAnotherWorld(ul_Fat,"GRO",sWorldPath,ulGAOFat);
                }
            }

            // MAP
            if (pst_CurrentGO && pst_CurrentGO->pst_Extended && pst_CurrentGO->pst_Extended->pst_Col && ((COL_tdst_Base *) pst_CurrentGO->pst_Extended->pst_Col)->pst_ColMap)
            {
                COL_tdst_ColMap *pColMap = ((COL_tdst_Base *) pst_CurrentGO->pst_Extended->pst_Col)->pst_ColMap;
        		BIG_KEY ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pColMap);
                ULONG ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
                if (bPrintIfComesAnotherWorld(ul_Fat,"MAP",sWorldPath,ulGAOFat))
                    continue;

                // COB
                int i;
                for (i=0; i<pColMap->uc_NbOfCob; i++)
                {
                    BIG_KEY ul_Key = LOA_ul_SearchKeyWithAddress((ULONG)(pColMap->dpst_Cob[i]) ) ;
                    ULONG ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
                    bPrintIfComesAnotherWorld(ul_Fat,"COB",sWorldPath,ulGAOFat);
                }
            }
        }
	}
}


void AI_PrintKeyDependencies(BIG_KEY h_Key)
{
    char sMsg[256];
	ULONG ulFat = BIG_ul_SearchKeyToFat(h_Key);
    if(ulFat != BIG_C_InvalidIndex)
		snprintf( sMsg, sizeof(sMsg), "Key %x (%s) is referenced by:", h_Key, BIG_NameFile( ulFat ) );
    else
		snprintf( sMsg, sizeof(sMsg), "Key %x is referenced by:", h_Key );

    LINK_gul_ColorTxt = 0x00FF0000;
    LINK_PrintStatusMsgCanal(sMsg,2);

    int i;
    for (i=0; i<(int)AI_aKeyDependencies.size(); i++)
    {
        AI_tdKeyDependency *pKeyDep = &(AI_aKeyDependencies[i]);
        if (pKeyDep->h_Used == h_Key)
        {
            for (i=0; i<(int)pKeyDep->a_User.size(); i++)
            {
                ulFat = BIG_ul_SearchKeyToFat(pKeyDep->a_User[i].h_User);
                if(ulFat != BIG_C_InvalidIndex)
                {
                    if (pKeyDep->a_User[i].s_CallBackName)
						snprintf( sMsg, sizeof(sMsg), "%x (%s) with callback %s", pKeyDep->a_User[ i ].h_User, BIG_NameFile( ulFat ), pKeyDep->a_User[ i ].s_CallBackName );
                    else
						snprintf( sMsg, sizeof(sMsg), "%x (%s)", pKeyDep->a_User[ i ].h_User, BIG_NameFile( ulFat ) );
                }
                else
					snprintf( sMsg, sizeof(sMsg), "%x", pKeyDep->a_User[ i ].h_User );
                LINK_PrintStatusMsgCanal(sMsg,2);
            }
            break;
        }
    }
    
    LINK_gul_ColorTxt = 0;
}

#endif //ACTIVE_EDITORS


void AI_ResolveTrigger(SCR_tt_Trigger *pt_Trigger, BOOL dyn)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	UINT					i;
	SCR_tt_ProcedureList	*pt_PL;
	ULONG					ul_Pos;
	int						num;
	char					*pz, *pz1;
	extern int	AI2C_i_SizeofArrayTrigger(void);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pt_Trigger->pt_Proc = NULL;
	if(!pt_Trigger->h_KeyFile) return;
	if(!pt_Trigger->az_Name[0]) return;

#ifdef ACTIVE_EDITORS
    // Store dependency to remember it afterwards.
    AI_AddDependency<SCR_tt_Trigger *,OBJ_tdst_GameObject *>(pt_Trigger,AI_gp_ResolveGO,AI_aTriggerDependencies);
#endif //ACTIVE_EDITORS


	/* Search if a corresponding C function exists */
	if(!dyn)
	{
		/* Get rank of trigger */
		num = 0;
		pz = pt_Trigger->az_Name + 1;
		while(L_isdigit(*pz))
		{
			num *= 10;
			num += *pz - '0';
			pz++;
		}

		for(i = 0; i < (UINT) AI2C_i_SizeofArrayTrigger(); i++)
		{
			if((pt_Trigger->h_KeyFile == AI2C_gat_fctdefstrigger[i].ul_File) && !L_strcmp(pz, AI2C_gat_fctdefstrigger[i].name))
			{
#ifdef GAMECUBE_USE_AI2C_DLL
				pt_Trigger->pfn_CFunc = (void *) *(AI2C_gat_fctdefstrigger[i].ppst_Func);
#else // GAMECUBE_USE_AI2C_DLL
				pt_Trigger->pfn_CFunc = (void *) AI2C_gat_fctdefstrigger[i].pst_Func;
#endif // GAMECUBE_USE_AI2C_DLL
				AI_ResolveMsg(pt_Trigger->az_Name, &pt_Trigger->t_Msg);
                return;
			}
		}
	}

	/* Else normal resolve */
	ul_Pos = BIG_ul_SearchKeyToPos(pt_Trigger->h_KeyFile);
	if(ul_Pos != -1)
	{
		ul_Pos = LOA_ul_SearchAddress(ul_Pos);
		if(ul_Pos != -1)
		{
			pt_PL = (SCR_tt_ProcedureList *) ul_Pos;
			for(i = 0; i < pt_PL->u16_Num; i++)
			{
				if(pt_PL->pt_All[i].pz_Name[0] != '@') continue;
				pz = pt_PL->pt_All[i].pz_Name + 1;
				pz1 = pt_Trigger->az_Name + 1;
				while(L_isdigit(*pz)) pz++;
				while(L_isdigit(*pz1)) pz1++;
				if(!L_strcmp(pz, pz1))
				{
					pt_Trigger->pt_Proc = pt_PL->pt_All + i;
					if(!dyn) AI_ResolveMsg(pt_Trigger->az_Name, &pt_Trigger->t_Msg);
					return;
				}
			}
		}
	}

	if(!dyn) AI_ResolveMsg(pt_Trigger->az_Name, &pt_Trigger->t_Msg);
}

