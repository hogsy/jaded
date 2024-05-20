/*$T AIfunctions_wor.c GC! 1.097 07/05/01 08:42:00 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "LINks/LINKmsg.h"
#include "LINks/LINKtoed.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/AItools.h"
#include "ENGine/Sources/WORld/WORuniverse.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/ENGinit.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGio.h"
#include "BIGfiles/BIGcomp.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "SDK/Sources/TIMer/TIMdefs.h"
#include "BASe/MEMory/MEM.h"
#include "ENGine/Sources/TEXT/TEXT.h"

#if defined(_XENON) && !defined(_GX8)
#include "XenonGraphics/XeGDInterface.h"
#include "XenonGraphics/XeRenderer.h"
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

extern BOOL LOA_gb_SpeedMode;
extern BOOL ENG_gb_InPause;
#ifdef JADEFUSION
#define MAIN_MENU_KEY 0x3D00C456    

int		g_iReinitPauseDelay		= 0;
BOOL	g_iReinitPauseScheduled	= FALSE;
#endif
/*
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef PSX2_TARGET

#include "IOP/RPC_Manager.h"
#include "IOP/CDV_Manager.h"
#include "BIGfiles/BIGopen.h"

int			eeRPC_PreloadTextRq=-1;
int			gi_SpecialHandler = -1;

#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

int						gi_LastES = -1;				/* Last tested ES */
int						gi_NumES = 0;				/* Number of registered ES */
OBJ_tdst_GameObject		*gap_ES[100];				/* Registered ES */
ULONG					gau_ES[100];				/* All world keys */

char					*gp_LastPointer = NULL;		/* Pointer from end of block to free zone */
char					*gap_FirstPointer[20];		/* First loaded pointer per bin */
int						gai_CurSize[20];			/* Number of loaded blocks */
int						gi_CurBin = -1;				/* Bin currently loaded */
int						gi_TestBin = -1;
int						gi_PreloadReadSize = -1;
char					*MEM_gp_MaxBlock = NULL;	/* Max allocated block */
int						gai_CurBinFileSize[20] ;				/* Bin currently loaded */
unsigned int			gaui_CurBinFilePos[20] ;				/* Bin currently loaded */

#define PRELOADTEXSIZE		(3 * 1024 * 1024)
#define PRELOADTEXGRAN		(40 * 1024)
#define PRELOADMAXBIN		2							/* Max number of bin to load */
#define PRELOADMOVEB		(1*1024*1024 + 512 * 1024 + 32 * 1024)    //(4 * 1024 * 1024)
#define PRELOADFREENEEDED 	(250 * 1024)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int PreLoadTex_Finished(int *finish, int refresh)
{
#ifdef PSX2_TARGET
    int rcode;
    
    if((gi_SpecialHandler < 0))
    {
        *finish=1;
        return 0;
    }
    
    *finish = eeRPC_i_GetAsyncStatus(eeRPC_PreloadTextRq);
    
    if(*finish) eeRPC_PreloadTextRq = -1;
    if(*finish == -1) 
        rcode= 0;
    else
    	rcode =1;

    if((!*finish) && refresh) eeRPC_RefreshAsyncStatus();
    return rcode;

#else    
	*finish = 1;
	return 0;

#endif
}

void PreloadCancel(void)
{
#ifdef PSX2_TARGET
    if(gi_SpecialHandler<0) return;
    if(eeRPC_PreloadTextRq<0) return;
    eeRPC_i_CancelAsyncRq(eeRPC_PreloadTextRq);
    eeRPC_PreloadTextRq = -1;
    
#ifndef _FINAL_ 
	if(gi_CurBin || gai_CurSize[0])
		printf("PreLoad Stop... Bin (%i): %iKo/%iKo were preloaded before stop.\n", gi_CurBin, gai_CurSize[gi_CurBin] / 1024, gai_CurBinFileSize[gi_CurBin] / 1024);
	else
		printf("PreLoad Stop... Nothing preloaded.\n");
#endif

#endif
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PreLoadTex_Load(ULONG _ul_Pos, ULONG _ul_Size, void *p_Buf)
{
#ifdef PSX2_TARGET
    extern u_int NoPreload;
    
    if(gi_SpecialHandler < 0) return;
    
#ifndef _FINAL_    
    if(NoPreload)
        eeRPC_PreloadTextRq = -1;
    else
#endif    
        eeRPC_PreloadTextRq = eeRPC_i_AsyncRead(gi_SpecialHandler, _ul_Pos, p_Buf, _ul_Size, 0);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PreLoadTexReset(void)
{	
	gp_LastPointer = (char *) MEM_gst_MemoryInfo.pv_DynamicBloc + MEM_gst_MemoryInfo.ul_DynamicMaxSize - 4; // 4 because those bytes are used to store the DynamicLastFree pointer
	L_memset(gai_CurSize, 0, sizeof(gai_CurSize));
	gi_CurBin = 0;
	gap_FirstPointer[gi_CurBin] = gp_LastPointer;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ResetPreloadTexAll(void)
{
	gi_NumES = 0;
	gi_LastES = -1;

    L_memset(gap_ES, 0, 100*sizeof(OBJ_tdst_GameObject*));
    L_memset(gau_ES, 0, 100*sizeof(ULONG));

#ifdef PSX2_TARGET
	eeRPC_PreloadTextRq=-1;
#endif
	PreLoadTexReset();
	L_memset(gai_CurBinFileSize, 0, 20*sizeof(int));
	L_memset(gaui_CurBinFilePos, 0, 20*sizeof(int));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PreLoadMoveBlocks(ULONG key)
{
	/*~~~~~~~~~~~~~~*/
	char	*p, *p1;
	int		i, total;
	/*~~~~~~~~~~~~~~*/

	/* Cancel cause bad map */
	if((gi_LastES == -1) || key != gau_ES[gi_LastES])
	{
		gi_CurBin = -1;
#ifndef _FINAL_ 
		if(gau_ES[gi_LastES])
			printf("All PreLoad Cancel... Bad Map.\n");
#endif		
		return;
	}

	/* Begin of dest block */
	p = p1 = (char *) MEM_gst_MemoryInfo.pv_DynamicBloc + MEM_gst_MemoryInfo.ul_DynamicMaxSize - 4; // 4 = new memory system
	p -= PRELOADMOVEB;
	
	/* Impossible de faire un MoveBloc .. Annulation du preload */
	if(p < MEM_gst_MemoryInfo.pv_DynamicNextFree)
	{
		ULONG ul_Size;
		
		ul_Size = (int) ((char *)MEM_gst_MemoryInfo.pv_DynamicBloc + MEM_gst_MemoryInfo.ul_DynamicMaxSize) - (int) MEM_gst_MemoryInfo.pv_DynamicNextFree;		
	
#ifndef _FINAL_ 
		printf("All PreLoad Cancel. (MoveBlock). Memory Left during moveblock = %u Ko.\n", ul_Size / 1024);
#endif		
	
		gi_CurBin = -1;
		return;
	}

	total = 0;
	for(i = 0; i <= gi_CurBin; i++)
	{
		if(p - gai_CurSize[i] > (char *) MEM_gst_MemoryInfo.pv_DynamicNextFree)
		{			
			p -= gai_CurSize[i];
			p1 -= gai_CurSize[i];
			total += gai_CurSize[i];
			gap_FirstPointer[i] -= PRELOADMOVEB;
		}
		else
		{
			ERR_X_Assert(p > MEM_gst_MemoryInfo.pv_DynamicNextFree);
			
			gai_CurSize[i] = max(0, (int)p - (int)(MEM_gst_MemoryInfo.pv_DynamicNextFree));
			gai_CurSize[i] /= PRELOADTEXGRAN;
			gai_CurSize[i] *= PRELOADTEXGRAN;
			
			if(gai_CurSize[i])
			{
				p -= gai_CurSize[i];
				p1 -= gai_CurSize[i];
				total += gai_CurSize[i];
				gap_FirstPointer[i] -= PRELOADMOVEB;
				gi_CurBin = i;			
				
#ifndef _FINAL_ 
				printf("PreLoad (%u) truncated (MoveBlock would overwrite Memory).. Size Left = %iKo\n", i, gai_CurSize[i] / 1024);
#endif		
				
			}
			else
			{
				gi_CurBin = i - 1;
#ifndef _FINAL_ 
				printf("PreLoad limited to (%u) (MoveBlock would overwrite Memory)..\n", gi_CurBin);
#endif		
				
			}			
		}
	}

	/* Move block */
	L_memmove(p, p1, total);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_WORPreLoadTex_C(OBJ_tdst_GameObject *_pt_Ref)
{
#ifdef PSX2_TARGET
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i, ibest, res;
	MATH_tdst_Vector	st_Tmp;
	float				dist, fbest;
	ULONG				ul_Pos;
	int					i_Finished;
	ULONG				key;
	int					size;
	int			gran;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#endif
#ifndef PSX2_TARGET
	return;
#else // PSX2_TARGET

    if(!LOA_gb_SpeedMode) return;
	if(!gi_NumES) return;			/* No registered ES */

	/* Last load not finished */
	ibest = -1;
	res = PreLoadTex_Finished(&i_Finished, 0);
	if(!i_Finished) return;
	if(res)
	{
		gai_CurSize[gi_CurBin] += PRELOADTEXGRAN;
		gp_LastPointer -= PRELOADTEXGRAN;
	}
	
    /* Search the best ES */
    if(_pt_Ref)
    {
    	fbest = Cf_Infinit;
    	for(i = 0; i < gi_NumES; i++)
    	{
    		MATH_SubVector(&st_Tmp, OBJ_pst_GetAbsolutePosition(gap_ES[i]), OBJ_pst_GetAbsolutePosition(_pt_Ref));
    		dist = MATH_f_SqrNormVector(&st_Tmp);
    		if(dist < fbest)
    		{
    			fbest = dist;
    			ibest = i;
    			key = gau_ES[i];
    		}
    	}
    }
    else
    {
        i = ibest = gi_LastES;
        key = gau_ES[i];
    }

	if(ibest == -1) return;

	/* If ES has changed, reset loaded information */
	gran = PRELOADTEXGRAN;
	if(ibest != gi_LastES)
	{
#ifndef _FINAL_
		printf("New Best ES found. PreLoad Reset... \n");
#endif	
		PreLoadTexReset();
	}
	else
	{

		/* Already everything loaded. Change bin */
		size = 0;
		if(gai_CurSize[gi_CurBin])
		{		
			size = *(int *) (gap_FirstPointer[gi_CurBin] - PRELOADTEXGRAN);
#ifdef PSX2_TARGET
			if(gi_CurBin == 4) size = 20*1024;
#endif
		    gai_CurBinFileSize[gi_CurBin] = size;			
		}

		if(size)
		{
    		if(gai_CurSize[gi_CurBin] + PRELOADTEXGRAN > PRELOADTEXSIZE)
    		{
next:

    			gi_CurBin++;
    			gai_CurSize[gi_CurBin] = 0;
    			gap_FirstPointer[gi_CurBin] = gp_LastPointer;
    			

#ifndef _FINAL_				
				{
					int		Key;
								
					Key = key;
					switch(gi_CurBin)
					{
						case 0:	Key &= 0x0007FFFF;Key |= 0xFF000000; break;
						case 1:	Key &= 0x0007FFFF;Key |= 0xFF800000; break;
						case 2:	Key &= 0x0007FFFF;Key |= 0xFD000000 | ((TEXT_gst_Global.i_CurrentLanguage + 1) << 19); break;
						case 3:	Key &= 0x0007FFFF;Key |= 0xFE000000 | ((TEXT_gst_Global.i_CurrentSndLanguage + 1) << 19); break;
						case 4:	Key &= 0x0007FFFF;Key |= 0xFF400000; break;
						default: 				
							printf("Bin (%i) preloaded = %iKo/%iKo ... PreLoad Finished.\n", gi_CurBin - 1, gai_CurSize[gi_CurBin - 1] / 1024, gai_CurBinFileSize[gi_CurBin - 1] / 1024);				
							return;
					}
					
					printf("Bin (%i) preloaded = %iKo/%iKo ... \n",gi_CurBin - 1, gai_CurSize[gi_CurBin - 1] / 1024, gai_CurBinFileSize[gi_CurBin - 1] / 1024);
				}
#endif				

    		}
    		else if(gai_CurSize[gi_CurBin] + PRELOADTEXGRAN > size)
			{
				if(gai_CurSize[gi_CurBin] >= size) 
					goto next;				

				gran = size - gai_CurSize[gi_CurBin];
			}
    	}
	}

	gi_LastES = ibest;

	/* Request a load */
	switch(gi_CurBin)
	{
	case 0:	key &= 0x0007FFFF;key |= 0xFF000000; break;
	case 1:	key &= 0x0007FFFF;key |= 0xFF800000; break;
	case 2:	key &= 0x0007FFFF;key |= 0xFD000000 | ((TEXT_gst_Global.i_CurrentLanguage + 1) << 19); break;
	case 3:	key &= 0x0007FFFF;key |= 0xFE000000 | ((TEXT_gst_Global.i_CurrentSndLanguage + 1) << 19); break;
	case 4:	key &= 0x0007FFFF;key |= 0xFF400000; break;
	
	default: return;
	}

	/* J'arrive à la limite de mémoire pour préloader */
	if(((char *)MEM_gst_MemoryInfo.pv_DynamicNextFree) + PRELOADFREENEEDED > gap_FirstPointer[gi_CurBin] - gai_CurSize[gi_CurBin])
	{
		int	SizeLeft, tmp, tmp2;
		/* J'ai plus assez de place pour continuer le preload */
		
		SizeLeft = (gap_FirstPointer[gi_CurBin] - gai_CurSize[gi_CurBin]) - ((char *)MEM_gst_MemoryInfo.pv_DynamicNextFree);
		
		while((SizeLeft < PRELOADFREENEEDED) && (gi_CurBin != -1))
		{
			/* Je réduis la taille à conserver préloadée du bin courant */
			/* Si la taille préloadée du bin courant ne suffit pas à elle seule à regagner la mémoire min, je
			   je vire complétement et je passe à celui d'avant */
			if(SizeLeft + gai_CurSize[gi_CurBin] < PRELOADFREENEEDED)
			{
				SizeLeft += gai_CurSize[gi_CurBin];			
				gp_LastPointer += gai_CurSize[gi_CurBin];			
				gap_FirstPointer[gi_CurBin] = 0;
				gai_CurBinFileSize[gi_CurBin] = 0;
				gaui_CurBinFilePos[gi_CurBin] = 0;
				
				gai_CurSize[gi_CurBin] = 0;
				gi_CurBin --;				
				
//#ifndef _FINAL_
//				printf("Preload (%i) totally canceled ... Classical Alloc too Close \n", gi_CurBin + 1);
//#endif				
				
			}
			else
			{
				tmp = ((PRELOADFREENEEDED - SizeLeft)/ PRELOADTEXGRAN);
				tmp2 = 	gai_CurSize[gi_CurBin];
				gai_CurSize[gi_CurBin] -= (tmp + 1) * PRELOADTEXGRAN;				
				gai_CurSize[gi_CurBin] /= PRELOADTEXGRAN;
				gai_CurSize[gi_CurBin] *= PRELOADTEXGRAN;				
				tmp2 -= gai_CurSize[gi_CurBin];				
				
			 	SizeLeft += tmp2;
				gp_LastPointer += tmp2;						 	
				
//#ifndef _FINAL_
//				printf("Preload (%i): %i block(s) removed (Alloc too Close). Already Preloaded = %iKo\n", gi_CurBin, tmp + 1, gai_CurSize[gi_CurBin] / 1024);
//#endif				
				
					if(gai_CurSize[gi_CurBin] <= 0)
					{
						gai_CurBinFileSize[gi_CurBin] = 0;
						gaui_CurBinFilePos[gi_CurBin] = 0;
					
						gap_FirstPointer[gi_CurBin] = 0;
						gai_CurSize[gi_CurBin] = 0;				
						gi_CurBin --;						
//#ifndef _FINAL_
//					printf("Preload (%i) totally canceled ... Data overwriten by classical Alloc\n", i);
//#endif				
						
					}
				
			}			
		}
				
		return;
	}

	ul_Pos = BIG_ul_SearchKeyToPos(key);
	
	if( (ul_Pos == -1) || (ul_Pos == key) ) return;
	
	if(gai_CurSize[gi_CurBin])
	{
	    gai_CurBinFileSize[gi_CurBin] = *(int *) (gap_FirstPointer[gi_CurBin] - PRELOADTEXGRAN);
	    gaui_CurBinFilePos[gi_CurBin] = ul_Pos;
	    if(gi_CurBin == 4) gai_CurBinFileSize[gi_CurBin] =  20*1024;
	}
	else
	{
	    gai_CurBinFileSize[gi_CurBin] = 0;
	    gaui_CurBinFilePos[gi_CurBin] = 0;
	}
	
	PreLoadTex_Load(ul_Pos + gai_CurSize[gi_CurBin], gran, gp_LastPointer - PRELOADTEXGRAN);
#endif // PSX2_TARGET
}


int AllPreloadDone(void)
{
    int i;    

    if(gi_CurBin != 5) return 0;    

    for(i=0; i<(gi_CurBin-1); i++)
    {
        if(gai_CurSize[i]< *(int *) (gap_FirstPointer[i] - PRELOADTEXGRAN)) 
        {
            return 0;
        }
    }
    
    return 1;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_WORRegisterES_C(int key, OBJ_tdst_GameObject *pst_GO)
{	
	return;
	
	if(gi_NumES >= 100)
	{
		ERR_X_Warning(gi_NumES <= 100, "No more space for registering E/S",NULL);
		return;
	}

	gap_ES[gi_NumES] = pst_GO;
	gau_ES[gi_NumES++] = key;
}
/**/
AI_tdst_Node *AI_EvalFunc_WORRegisterES(AI_tdst_Node *_pst_Node)
{
	OBJ_tdst_GameObject *pst_GO;
	int					key;

	pst_GO = AI_PopGameObject();
	AI_Check(pst_GO != NULL, "Object is null");
	key = AI_PopInt();
	AI_EvalFunc_WORRegisterES_C(key, pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int WOR_gi_IsLoading=0;
int WOR_gi_IsLoadingFix=0;

#ifndef _FINAL_	
extern int	MEM_gi_MinDelta;
#endif	

int AI_EvalFunc_WORLoadList_C(int i)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
    extern BOOL INO_gb_SavingScreen;
	int	j;
	extern BAS_tdst_barray WOR_gst_LoadedKeyForCurrentFix;
#ifdef JADEFUSION
#ifndef ACTIVE_EDITORS
    extern BOOL ENG_gb_SlashL;
    extern ULONG ENG_ulCustomLevel;
#endif
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

    LOA_InitLoadRaster();


#ifndef _FINAL_
	/* Reset du Min Delta */
	MEM_gi_MinDelta = 0x7FFFFFFF;
#endif	


#ifdef ACTIVE_EDITORS
	AI_Check(BIG_ul_SearchKeyToFat(i) != -1, "Bad key");
#endif
#if defined(_XENON) && !defined(_GX8)
    Xe_InvalidateRenderLists();
#endif // defined(_XENON) && !defined(_GX8)

	if(WOR_gi_IsLoadingFix==0)
	{
		//
		// which one we want to keep, which one we want to destroy
		//
		WOR_gi_IsLoadingFix = 1;
		WOR_ResolveFixWorldRequest();
		BAS_binit(&WOR_gst_LoadedKeyForCurrentFix, 500);
		for(j=0; j<WOR_Cte_MaxFixWorldNb; j++)
		{
			// load all wanted fix
			if(!WOR_gul_FixWorldToLoad[j]) continue;
			AI_EvalFunc_WORLoadList_C(WOR_gul_FixWorldKeys[j]);
			WOR_gul_FixWorldLoaded[j] = 1;
			WOR_gul_FixWorldToLoad[j] = 0;
		}
		BAS_bfree(&WOR_gst_LoadedKeyForCurrentFix);
		WOR_gi_IsLoadingFix = 0;
	}

#ifdef _GAMECUBE
    INO_gb_SavingScreen = FALSE; 
#endif

	WOR_gi_IsLoading = 1;

	LOA_StartLoadRaster(LOA_Cte_LDI_AddWorldList);

#ifdef JADEFUSION
#ifndef ACTIVE_EDITORS
    // Loading hack : Swap the key of main menu with the one give in parameter
    if (ENG_gb_SlashL && i == MAIN_MENU_KEY)
    {
        i = ENG_ulCustomLevel;
        ENG_gb_SlashL = FALSE;
    }
#endif
#endif

	pst_World = WOR_pst_Universe_AddWorldList(NULL, i, 0);
	WOR_l_Universe_GetWorldIndex(pst_World);
	LOA_StopLoadRaster(LOA_Cte_LDI_AddWorldList);

	WOR_gi_IsLoading = 0;
	
	return WOR_l_Universe_GetWorldIndex(pst_World);
}
/**/
AI_tdst_Node *AI_EvalFunc_WORLoadList(AI_tdst_Node *_pst_Node)
{
	PROPS2_StartRaster(&PROPS2_gst_AI_EvalFunc_WORLoadList);
	AI_PushInt(AI_EvalFunc_WORLoadList_C(AI_PopInt()));
	PROPS2_StopRaster(&PROPS2_gst_AI_EvalFunc_WORLoadList);

#ifdef ACTIVE_EDITORS
	LINK_UpdatePointers();
#endif
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_WORRequestFix(AI_tdst_Node *_pst_Node)
{
	WOR_RequestFixWorld(AI_PopInt());
	return ++_pst_Node;
}
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_WORLoaded_C(void)
{
	return 1;
}
/**/
AI_tdst_Node *AI_EvalFunc_WORLoaded(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(1);
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_WORUniverseAdd_C(int i)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	AI_Check(BIG_ul_SearchKeyToFat(i) != -1, "Bad key");
#endif
	pst_World = WOR_pst_Universe_AddWorld(i, 0, TRUE);
	return (int) WOR_l_Universe_GetWorldIndex(pst_World);
}
/**/
AI_tdst_Node *AI_EvalFunc_WORUniverseAdd(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_WORUniverseAdd_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_WORUniverseDestroy_C(int _i_World, int _i_Fix)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_pst_Universe_GetWorldPointer(_i_World);
	AI_Check(pst_World && !TAB_b_IsAHole(pst_World), "World is not correct (not loaded ?)");
	WOR_Universe_DeleteWorld(pst_World, 0);
}
/**/
AI_tdst_Node *AI_EvalFunc_WORUniverseDestroy(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~*/
	int i_Dest, i_Flags;
	/*~~~~~~~~~~~~~~~~*/

	i_Flags = AI_PopInt();
	i_Dest = AI_PopInt();
	AI_EvalFunc_WORUniverseDestroy_C(i_Dest, i_Flags);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_WORActive_C(int i)
{
	PROPS2_StartRaster(&PROPS2_gst_AI_EvalFunc_WORActive);

	WOR_Universe_SetWorldActive(i);
	MAI_gst_MainHandles.pst_World = WOR_pst_Universe_GetActiveWorld(i);

	PROPS2_StopRaster(&PROPS2_gst_AI_EvalFunc_WORActive);
}
/**/
AI_tdst_Node *AI_EvalFunc_WORActive(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_WORActive_C(AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_WORDesactive(AI_tdst_Node *_pst_Node)
{
	WOR_Universe_SetWorldInactive(AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_WORGetObject_C(int world, int object)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_pst_Universe_GetWorldPointer(world);
	if(TAB_b_IsAHole(pst_World->st_AllWorldObjects.p_Table[object].p_Pointer)) return 0;
	return (OBJ_tdst_GameObject *) pst_World->st_AllWorldObjects.p_Table[object].p_Pointer;
}
/**/
AI_tdst_Node *AI_EvalFunc_WORGetObject(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~*/
	int object;
	/*~~~~~~~*/

	object = AI_PopInt();
	AI_PushGameObject(AI_EvalFunc_WORGetObject_C(AI_PopInt(), object));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_WORGetNumObject_C(int world)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_pst_Universe_GetWorldPointer(world);
	if ( pst_World == NULL ) // more horrible hacks ~hogsy
		return 0;

    return pst_World->st_AllWorldObjects.ul_NbElems + pst_World->st_AllWorldObjects.ul_NbHoles;
}
/**/
AI_tdst_Node *AI_EvalFunc_WORGetNumObject(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_WORGetNumObject_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_WORRefreshVisible_C(int world)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	ULONG			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_pst_Universe_GetWorldPointer(world);

	/*
	 * Force the the next WOR_World_MakeObjectsVisible to recompute all the visibility
	 * tables. We dont want an update BUT a total reconstruction !!!
	 */
	for(i = 0; i < pst_World->ul_NbViews; i++) pst_World->pst_View[i].uc_Flags |= WOR_Cuc_View_RecomputeTables;

	WOR_World_MakeObjectsVisible(pst_World);
}
/**/
AI_tdst_Node *AI_EvalFunc_WORRefreshVisible(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_WORRefreshVisible_C(AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_WORRefreshVisibleOfActor_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	ULONG			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	for(i = 0; i < pst_World->ul_NbViews; i++) pst_World->pst_View[i].uc_Flags |= WOR_Cuc_View_RecomputeTables;
	WOR_World_MakeObjectsVisible(pst_World);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_WORRefreshVisibleOfActor(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_WORRefreshVisibleOfActor_C(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_WORReinit_C(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	extern BOOL ENG_gb_NeedToReinit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ENG_gb_NeedToReinit = TRUE;
#ifdef JADEFUSION
	g_iReinitPauseDelay = 30;
#endif
	return AI_CR_StopAI;
}
/**/
AI_tdst_Node *AI_EvalFunc_WORReinit(AI_tdst_Node *_pst_Node)
{
	return (AI_tdst_Node *) AI_EvalFunc_WORReinit_C();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_WORPause_C(int world, ULONG ul_ID)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	TAB_tdst_PFelem *pst_Elem;
	TAB_tdst_PFelem *pst_LastElem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef JADEFUSION
	if (g_iReinitPauseDelay > 0)
	{
		g_iReinitPauseScheduled	= TRUE;
		return;
	}
#endif
	pst_World = WOR_pst_Universe_GetWorldPointer(world);
	ENG_gb_InPause = TRUE;

	/* Set pause mode for all objects */
	pst_Elem = pst_World->st_AllWorldObjects.p_Table;
	pst_LastElem = pst_World->st_AllWorldObjects.p_NextElem;
	for(; pst_Elem < pst_LastElem; pst_Elem++)
	{
		if(TAB_b_IsAHole(pst_Elem->p_Pointer)) continue;
		if(!OBJ_b_TestIdentityFlag((OBJ_tdst_GameObject *) (pst_Elem->p_Pointer), ul_ID)) continue;
		if(!OBJ_b_TestControlFlag((OBJ_tdst_GameObject *) (pst_Elem->p_Pointer), OBJ_C_ControlFlag_AlwaysActive))
			OBJ_SetControlFlag((OBJ_tdst_GameObject *) (pst_Elem->p_Pointer), OBJ_C_ControlFlag_InPause);
	}

	/* Recompute active objects, SnP and Views next frame */
	pst_World->b_ForceActivationRefresh = TRUE;
	pst_World->b_ForceVisibilityRefresh = TRUE;
	pst_World->b_ForceBVRefresh = TRUE;
}
/**/
AI_tdst_Node *AI_EvalFunc_WORPause(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~*/
	int		world;
	ULONG	ul_ID;
	/*~~~~~~~~~~*/

	ul_ID = AI_PopInt();
	world = AI_PopInt();
	AI_EvalFunc_WORPause_C(world, ul_ID);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_WORColorAmbiantSet_C(int world, int i_Col)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_pst_Universe_GetWorldPointer(world);
	pst_World->ul_AmbientColor = i_Col;
}
/**/
AI_tdst_Node *AI_EvalFunc_WORColorAmbiantSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~*/
	int world;
	int i_Col;
	/*~~~~~~*/

	i_Col = AI_PopInt();
	world = AI_PopInt();
	AI_EvalFunc_WORColorAmbiantSet_C(world, i_Col);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_WORColorAmbiantGet_C(int world)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_pst_Universe_GetWorldPointer(world);
	return pst_World->ul_AmbientColor;
}
/**/
AI_tdst_Node *AI_EvalFunc_WORColorAmbiantGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~*/
	int world;
	/*~~~~~~*/

	world = AI_PopInt();
	AI_PushInt(AI_EvalFunc_WORColorAmbiantGet_C(world));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_WORColorAmbiant2Set_C(int world, int i_Col)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_pst_Universe_GetWorldPointer(world);
	pst_World->ul_AmbientColor2 = i_Col;
}
/**/
AI_tdst_Node *AI_EvalFunc_WORColorAmbiant2Set(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~*/
	int world;
	int i_Col;
	/*~~~~~~*/

	i_Col = AI_PopInt();
	world = AI_PopInt();
	AI_EvalFunc_WORColorAmbiant2Set_C(world, i_Col);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_WORColorAmbiant2Get_C(int world)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_pst_Universe_GetWorldPointer(world);
	return pst_World->ul_AmbientColor2;
}
/**/
AI_tdst_Node *AI_EvalFunc_WORColorAmbiant2Get(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~*/
	int world;
	/*~~~~~~*/

	world = AI_PopInt();
	AI_PushInt(AI_EvalFunc_WORColorAmbiant2Get_C(world));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_WORColorBackSet_C(int world, int i_Col)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_pst_Universe_GetWorldPointer(world);
	pst_World->ul_BackgroundColor = i_Col;
}
/**/
AI_tdst_Node *AI_EvalFunc_WORColorBackSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~*/
	int world;
	int i_Col;
	/*~~~~~~*/

	i_Col = AI_PopInt();
	world = AI_PopInt();
	AI_EvalFunc_WORColorBackSet_C(world, i_Col);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_WORColorBackGet_C(int world)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_pst_Universe_GetWorldPointer(world);
	return pst_World->ul_BackgroundColor;
}
/**/
AI_tdst_Node *AI_EvalFunc_WORColorBackGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~*/
	int world;
	/*~~~~~~*/

	world = AI_PopInt();
	AI_PushInt(AI_EvalFunc_WORColorBackGet_C(world));
	return ++_pst_Node;
}

extern BOOL ENG_gb_ForceUnPause;
extern UINT ENG_gui_UnpauseMask;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_WORUnPause_C(int world, ULONG ul_ID)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_pst_Universe_GetWorldPointer(world);

	/* Recompute active objects, SnP and Views next frame */
	ENG_gb_InPause = FALSE;
	ENG_gui_UnpauseMask = ul_ID;
	ENG_gb_ForceUnPause = TRUE;
	pst_World->b_ForceActivationRefresh = TRUE;
	pst_World->b_ForceVisibilityRefresh = TRUE;
	pst_World->b_ForceBVRefresh = TRUE;

}
/**/
AI_tdst_Node *AI_EvalFunc_WORUnPause(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~*/
	int		world;
	ULONG	ul_ID;
	/*~~~~~~~~~~*/

	ul_ID = AI_PopInt();
	world = AI_PopInt();
	AI_EvalFunc_WORUnPause_C(world, ul_ID);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_WORCurIndexGet_C(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem *pst_Elem;
	TAB_tdst_PFelem *pst_LastElem;
	int				i_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Elem = WOR_gst_Universe.st_WorldsTable.p_Table;
	pst_LastElem = WOR_gst_Universe.st_WorldsTable.p_NextElem;
	i_Res = 0;
	for(; pst_Elem < pst_LastElem; pst_Elem++)
	{
		if((WOR_tdst_World *) pst_Elem->p_Pointer == WOR_gpst_CurrentWorld) break;
		i_Res++;
	}

	return i_Res;
}
/**/
AI_tdst_Node *AI_EvalFunc_WORCurIndexGet(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_WORCurIndexGet_C());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_WORGetKey_C(OBJ_tdst_GameObject *pst_GO)
{
	WOR_tdst_World	*pst_World;
	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	return pst_World->h_WorldKey;
}
/**/
AI_tdst_Node *AI_EvalFunc_WORGetKey(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_WORGetKey_C(pst_GO));
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_WORCheckSetOrder_C(OBJ_tdst_GameObject *_pst_GO1, OBJ_tdst_GameObject *_pst_GO2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_Elem, *pst_LastElem;
	int					i_Rank1, i_Rank2, i_Rank;
	OBJ_tdst_GameObject *pst_GO;
	WOR_tdst_World		*pst_World;
	ULONG				ul_SaveFlags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO1  || !_pst_GO2 || (_pst_GO1 == _pst_GO2)) return;

	pst_World = WOR_World_GetWorldOfObject(_pst_GO1);

	i_Rank1 = -1;
	i_Rank2 = -1;
	i_Rank = 0;

	pst_Elem = TAB_pst_PFtable_GetFirstElem(&pst_World->st_AllWorldObjects);
	pst_LastElem = TAB_pst_PFtable_GetLastElem(&pst_World->st_AllWorldObjects);
	for(; pst_Elem <= pst_LastElem; pst_Elem++, i_Rank ++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;

		if(pst_GO == _pst_GO1)
		{
			/* If GO1 is before GO2 in World Object Table, it is what we finally want -> return */
			if(i_Rank2 == -1) return;
			i_Rank1 = i_Rank;
		}

		if(pst_GO == _pst_GO2)
			i_Rank2 = i_Rank;

		
	}

	/* Swap GO in order to obtain 1 Before 2 in World Object Table */
	if((i_Rank1 != -1) && (i_Rank2 != -1))
	{
		ul_SaveFlags = (pst_World->st_AllWorldObjects.p_Table + i_Rank1)->ul_Flags;

		(pst_World->st_AllWorldObjects.p_Table + i_Rank1)->p_Pointer = (pst_World->st_AllWorldObjects.p_Table + i_Rank2)->p_Pointer;
		(pst_World->st_AllWorldObjects.p_Table + i_Rank1)->ul_Flags = (pst_World->st_AllWorldObjects.p_Table + i_Rank2)->ul_Flags;

		(pst_World->st_AllWorldObjects.p_Table + i_Rank2)->p_Pointer = _pst_GO1;
		(pst_World->st_AllWorldObjects.p_Table + i_Rank2)->ul_Flags = ul_SaveFlags;
	}

	pst_World->b_ForceVisibilityRefresh = TRUE;
}

/**/
AI_tdst_Node *AI_EvalFunc_WORCheckSetOrder(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO1, *pst_GO2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO1);

	pst_GO2 = AI_PopGameObject();
	pst_GO1 = AI_PopGameObject();

	AI_EvalFunc_WORCheckSetOrder_C(pst_GO1, pst_GO2);
	return ++_pst_Node;
}


#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
