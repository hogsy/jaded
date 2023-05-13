/*$T WORuniverse.c GC! 1.100 09/04/01 16:05:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Initialisations du module World */
#include "Precomp.h"
#include "LINks/LINKtoed.h"
#include "TABles/TABles.h"
#include "ENGine/Sources/WORld/WORupdate.h"
#include "ENGine/Sources/WORld/WORuniverse.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/WORld/WORvars.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/EOT/EOTmain.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/GRP/GRPmain.h"
#include "ENGine/Sources/TEXT/TEXTload.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/BAStypes.h"
#include "AIinterp/Sources/AIload.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIBench.h"
#include "AIinterp/Sources/Functions/AIfunctions_savephotos.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "EDIpaths.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "TEXture/TEXmemory.h"
#include "STRing/STRstruct.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"
#include "INOut/INO.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SNDstream.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SND.h"

#include "MoDiFier/MDFmodifier_SPG2.h"
#ifdef JADEFUSION
#include "TEXture/TEXcubemap.h"
#include "Light/LIGHTrejection.h"
#endif

#ifdef _GAMECUBE
#include "SDK/Sources/GameCube/GC_arammng.h"
#endif

#if defined(_PC_RETAIL)
#include "Dx9/Dx9buffer.h"
#endif	// defined(_PC_RETAIL)

#if defined(_XBOX)
#include "Gx8/Gx8buffer.h"
#include "Gx8/Gx8VertexBuffer.h"
#endif	// defined(_XBOX)

#if defined(ACTIVE_EDITORS) && defined(_XENON_RENDER)
#include "XenonGraphics/XeGDInterface.h"
#endif

#if defined(_XENON)
#include "XenonGraphics/XeUtils.h"
#include "XenonGraphics/XeRenderer.h"
#include "XenonGraphics/XeBufferMgr.h"
#include "INOut/Xenon/XeINOjoystick.h"
#endif

extern int					BIG_gi_ReadMode;
extern BOOL					ENG_gb_ForceAttach;
extern BOOL					LOA_gb_SpeedMode;
extern void				SOFT_ZList_Clear(void);
extern void                AI_GeneratedObject_Reinit( void );
BOOL						WOR_gb_FirstTrameAfterLoad = FALSE;

/* désolé */
extern void		TEXT_FreeAll(void);
extern void     GEO_DebugObject_AddTexture( void );
extern void     SOFT_BackgroundImage_AddTexture( void );
extern void     GEO_ResetUncacheObjectList(void);

#ifndef _FINAL_
extern void		ACT_ul_ComputeWanted();
#endif

#if PLAN_B_MEMORY_LEAK_FIX
extern bool g_bJustRebooted;
#endif
/*
 =======================================================================================================================
    vraiment, vraiment
 =======================================================================================================================
 */
WOR_tdst_World *WOR_pst_Universe_MergeWorldList(WOR_tdst_World *_pst_Dest, BIG_KEY _ul_FileKey, char _c_ForEditor)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_pst_Universe_AddWorldList(_pst_Dest, _ul_FileKey, _c_ForEditor);
	return pst_World;
}

/*
 =======================================================================================================================
    Aim:    Add a world list to universe
 =======================================================================================================================
 */
extern int				gi_CurSize; 
extern void				MEM_Defrag(int);
extern void				GFX_End(void);
extern void				SDW_Destroy(void);
extern void				MAT_SpriteGen_Close(void);


extern void				GFX_NeedGeomEx(ULONG, ULONG, ULONG, ULONG, BOOL);
extern void				GFX_NeedGeomExElem(ULONG, ULONG);
extern void				ResetPreloadTexAll(void);
extern BOOL				WOR_gb_FinalWorld;
extern BOOL				WOR_gb_RealAllocWorld;
extern void				GEO_DebugObject_Destroy(void);

#ifdef PSX2_TARGET
extern void GSP_BeginWorldLoad();
extern void GSP_EndWorldLoad();
#endif

#ifdef _XBOX
extern void  Gx8_BeginWorldLoad();
extern void Gx8_EndWorldLoad();
ULONG h_SaveWorldKey;
#endif

#ifdef _GAMECUBE
extern void  GXI_StopFlip();
extern void  GXI_StartFlip();
#ifndef MEM_OPT
#define USE_HOLE_OPTIM 
#endif // MEM_OPT
#ifdef USE_HOLE_OPTIM
void MEM_ConcatHolesAfterDefrag();
#endif
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#if 0
void WOR_UpdateAllPrefabs(WOR_tdst_World *pst_World, TAB_tdst_PFtable *_pst_Table)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	BIG_INDEX			ul_Index, ul_IndexPrefab, ul_IndexPrefabObj;
	ULONG				ul_Addr;
	char				asz_Msg[200];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_Table);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_Table);

	for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		ul_Index = BIG_ul_SearchKeyToFat(pst_GO->ul_MyKey);
		ul_IndexPrefab = BIG_ul_SearchKeyToFat(pst_GO->ul_PrefabKey);
		ul_IndexPrefabObj = BIG_ul_SearchKeyToFat(pst_GO->ul_PrefabObjKey);

		if (ul_Index == BIG_C_InvalidIndex)
		{
			sprintf(asz_Msg, "Couldn't update prefab because key doesn't exist (%x)", pst_GO->ul_MyKey);
			ERR_X_Warning(0, asz_Msg, NULL);
			continue;
		}

		if (ul_IndexPrefab == BIG_C_InvalidIndex)
		{
			sprintf(asz_Msg, "Couldn't update prefab because object is in prefab table and doesn't have an associated prefab : ");
			ERR_X_Warning(0, asz_Msg, pst_GO->sz_Name);
			continue;
		}

		if (ul_IndexPrefabObj == BIG_C_InvalidIndex)
		{
			sprintf(asz_Msg, "Couldn't update prefab because following object is not in a prefab");
			ERR_X_Warning(0, asz_Msg, pst_GO->sz_Name);
			continue;
		}

		if (TAB_b_IsAHole(pst_GO)) continue;
		if (pst_GO->ul_PrefabKey && pst_GO->ul_PrefabKey != BIG_C_InvalidIndex &&
			pst_GO->ul_PrefabObjKey && pst_GO->ul_PrefabObjKey != BIG_C_InvalidIndex)
			OBJ_UpdateGaoGao(pst_World, pst_GO, pst_GO->ul_PrefabObjKey, 0xfff7ffff);
	}
}
#endif

unsigned int WOR_gul_WorldKey= 0xFFFFFFFF;

WOR_tdst_World *WOR_pst_Universe_AddWorldList(WOR_tdst_World *_pst_Dest, BIG_KEY _ul_FileKey, char _c_ForEditor)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CHAR					*pc_Buf;
	BIG_tdst_GroupElem		*pst_Grp;
	ULONG					i, ul_Pos, ul_Size;
	WOR_tdst_World			*pst_World;
	ULONG					ul_NbWorlds;
#ifdef ACTIVE_EDITORS
	char					asz_Msg[256];
	extern BAS_tdst_barray	WOR_ListAllKeys;
#endif
	TAB_tdst_PFelem			*pst_CurrentElem;
	TAB_tdst_PFelem			*pst_EndElem;
    GDI_tdst_DisplayData    *pst_DD;
	BIG_tdst_GroupElem		ast_Group[400];
	ULONG					firstindex;
	ULONG					lastindex;
	extern BOOL				EDI_gb_ComputeMap;
	extern int				ps2SND_i_ChangePriority(int);
    extern void            SND_Reinit(void);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    
    WOR_gul_WorldKey = _ul_FileKey;
    
#ifdef MEM_SPY
#ifdef MEM_OPT
	{
		// Start leak tracking if a game map has been loaded (at least 15Mo data).
		extern BOOL MEM_bStartLeakTracking;
#ifdef _GAMECUBE
		MEM_bStartLeakTracking = MEM_uGetAllocatedSize() > 15000000;
#else //_GAMCUBE
		MEM_bStartLeakTracking = MEM_uGetAllocatedSizeBasic() > 15000000;
#endif // _GAMCUBE
	}        
#endif // MEM_OPT
#endif // MEM_SPY
        
    
    {
    	extern void SPG2_CloseWorldHook();
    	SPG2_CloseWorldHook(); // Release computed GRID
    }
    
#ifdef BENCH_IA_TEST
	AIBnch_BeginWorldLoad(_ul_FileKey);
#endif
#ifdef _GAMECUBE
	GXI_StopFlip(_ul_FileKey);
#endif
	
    
    
#ifdef PSX2_TARGET
    ps2SND_i_ChangePriority(-3);
    if(!SND_gc_NoSound) SND_StreamStopAll();
	GSP_BeginWorldLoad();
#else
    if(!SND_gc_NoSound) SND_StreamStopAll();
#endif




#ifdef _XBOX
	Gx8_BeginWorldLoad();
#endif


#ifdef ACTIVE_EDITORS
    GEO_ResetUncacheObjectList();
#endif
    

    
	{
		extern void WTR_BeginLoadWorldHook();
		WTR_BeginLoadWorldHook();
	}

#ifdef ACTIVE_EDITORS
    if (!_c_ForEditor)
	    BAS_binit(&WOR_ListAllKeys, 100);
#endif
	WOR_gb_FirstTrameAfterLoad = TRUE;

#ifdef ANIMS_USE_ARAM
	ARAM_FreeEntireHeap(eARAM_Game, eARAM_Level);
	ARAM_TotalAlloc_Level = 0;
	
	if(ENG_gb_ExitApplication)
	{
		ARAM_FreeEntireHeap(eARAM_Game, eARAM_Fix);
		ARAM_TotalAlloc_Fix = 0;
	}
	
#endif	

#ifdef JADEFUSION
	LRL_Clean();
#endif

	/* begin : destroy any link with other word (words must be independant) */
	LOA_StartLoadRaster(LOA_Cte_LDI_res3);
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
	if(pst_CurrentElem)
	{
		for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
		{
			pst_World = (WOR_tdst_World *) pst_CurrentElem->p_Pointer;
			if(!TAB_b_IsAHole(pst_World))
			{
				WOR_Universe_DeleteWorld(pst_World, _c_ForEditor);
			}
		}
	}
	_pst_Dest = NULL;
	LOA_StopLoadRaster(LOA_Cte_LDI_res3);


	/* end */
	if(MAI_gst_MainHandles.pst_DisplayData) 
	{
		GDI_l_DetachWorld(MAI_gst_MainHandles.pst_DisplayData);
		MAI_gst_MainHandles.pst_DisplayData->f_RLIScale = 1.0f;
		MAI_gst_MainHandles.pst_DisplayData->ul_RLIColorDest = 0;
	}
	GFX_End();
#if defined( _XBOX )	// E3 a-bug - shame on me for this code! MP
	{
		extern u_int gAE_Status;
		const u_int GSP_Status_AE_DB =  0x00000008;
		const u_int GSP_Status_AE_DBN = 0x00010000;

		gAE_Status &= ~(GSP_Status_AE_DB | GSP_Status_AE_DBN );
	}
#endif
	SDW_Destroy();
	MAT_SpriteGen_Close();
	MSG_GlobalReinit();
	STR_3DStringList_Clear();
	SOFT_ZList_Clear();
	pst_World = _pst_Dest;
_Try_

	/* We loop thru all the worlds and merge them. */
	TEXT_FreeAll();
    SND_Reinit();
	TEX_List_Free(&TEX_gst_GlobalList);
#if defined(TEX_USE_CUBEMAPS)
    TEX_CubeMap_Free(FALSE);
#endif
	TEX_Memory_Close();
	STR_ReinitAll();
	INO_Reinit();
	SAV_PhotoClose();
    AI_GeneratedObject_Reinit();
	SPG2_LoadWorldHook();
	// HACK - Reboot after playing a map to prevent memory leaks
#ifdef _XENON

#if PLAN_B_MEMORY_LEAK_FIX
	static bool bRebootOnNextLoad = false;

	if( bRebootOnNextLoad )
	{
		int nCode = 0x00ABCDEF;
		XSetLaunchData( &nCode, sizeof( nCode ) );
		XLaunchNewImage( "Jade_XeD.exe", 0 );
	}

	if( _ul_FileKey != 0x4902ea59 &&
		_ul_FileKey != 0x3d00c452 &&
		_ul_FileKey != 0x3d00c456 &&
		_ul_FileKey != 0x3d00c45a )
	{
		bRebootOnNextLoad = true;

//		HANDLE hFile = CreateFile( "d:\\Mem.txt", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
//
//		SetFilePointer( hFile, 0, 0, FILE_END );
//
//		MEMORYSTATUS stMemStatus;
//		GlobalMemoryStatus(&stMemStatus);
//
//		char szMem[ 64 ];
//		sprintf( szMem, "Free mem: %d\r\n", stMemStatus.dwAvailPhys/1024 );
//
//		DWORD dwWritten;
//		WriteFile( hFile, szMem, strlen( szMem ), &dwWritten, NULL );
//
//		CloseHandle( hFile );
	}
#endif // PLAN_B_MEMORY_LEAK_FIX

#endif // _XENON
#ifdef ACTIVE_EDITORS
	GEO_DebugObject_Destroy();
#endif


#ifdef _GAMECUBE
    {
        void AI2C_UnloadDll(void);
        AI2C_UnloadDll();
    }
#endif
 
#ifdef MEM_OPT 
    MEM_vManageSnapShot();
#endif // MEM_OPT
	
	
	MEM_Defrag(0);
#ifdef USE_HOLE_OPTIM	
	MEM_ConcatHolesAfterDefrag();
#endif	
	
#if defined(_XENON)
	if( g_oXeSimpleRenderer.IsActive( ) )
	{
		g_oXeSimpleRenderer.RequestReleaseDeviceOwnership( );

		while( g_oXeSimpleRenderer.OwnsDevice( ) )
		{
			// just wait
			Sleep( 0 );
		}
	}

#if defined(_XENON_RENDERER_USETHREAD)
	g_oXeRenderer.AcquireThreadOwnership( );
#endif // defined(_XENON_RENDERER_USETHREAD)

	g_XeBufferMgr.GarbageCollect(true);

#if defined(_XENON_RENDERER_USETHREAD)
	g_oXeRenderer.ReleaseThreadOwnership( );
#endif // defined(_XENON_RENDERER_USETHREAD)

	if( g_oXeSimpleRenderer.IsActive( ) )
		g_oXeSimpleRenderer.SignalDeviceOwnershipAvailable();
#endif
#ifndef ACTIVE_EDITORS

#ifdef _DEBUG
	{//c est mieux que de le faire a la main !!
	    void MEM_dbg_FindLastAllocatedCluster(void);
	    MEM_dbg_FindLastAllocatedCluster();
	}
#endif
	
	GFX_NeedGeomEx(1500, 1500, 4, 300, TRUE);
	GFX_NeedGeomExElem(0, 300);
	GFX_NeedGeomExElem(1, 300);
	GFX_NeedGeomExElem(2, 300);
	GFX_NeedGeomExElem(3, 300);
	SAV_PhotoInit();


	{ 
	    int			i_Finished;
	    extern int	PreLoadTex_Finished(int*, int);
		extern void PreLoadMoveBlocks(ULONG);
		extern void PreloadCancel(void);

    	i_Finished = 0;
   	    PreLoadTex_Finished(&i_Finished, 1);
   	    if(!i_Finished) PreloadCancel();

		PreLoadMoveBlocks(_ul_FileKey);
		
#ifdef PSX2_TARGET
		if(!i_Finished)
        {
            extern void eeRPC_AsyncReadReset(void);
            eeRPC_AsyncReadReset();
        }
#endif		
	}
#endif

#ifdef ACTIVE_EDITORS
    if (!LOA_gb_SpeedMode && !EDI_gb_ComputeMap)
    {
        SOFT_BackgroundImage_AddTexture();
        GEO_DebugObject_AddTexture();
    }
#endif

	LOA_StartLoadRaster(LOA_Cte_LDI_SpeedMode_Engine);	

#ifdef _GAMECUBE
    {	
        void AI2C_LoadAIForWorld(ULONG);
        AI2C_LoadAIForWorld(_ul_FileKey);
    }
#endif    
	
	LOA_BeginSpeedMode(_ul_FileKey);

	/* On autorise la memoire temporaire */
	MEM_gb_EnableTmp = TRUE;
	BIG_FreeGlobalBuffer();
	MEM_ResetTmpMemory();

	if (_ul_FileKey == BIG_C_InvalidKey )
		return NULL;

	// wol loading begin --
	ul_Pos = BIG_ul_SearchKeyToPos(_ul_FileKey);
	pc_Buf = BIG_pc_ReadFileTmp(ul_Pos, &ul_Size);
	pst_Grp = ast_Group;
	for(i = 0; i < ul_Size >> 3; ++i)
	{
		pst_Grp[i].ul_Key = LOA_ReadULong(&pc_Buf);
		pst_Grp[i].ul_Type = LOA_ReadULong(&pc_Buf);
	}

	ul_NbWorlds = (ul_Size >> 3);
	// wol loading end --
	
	/* Get index of last world (the final one) */
	firstindex = lastindex = 0xFFFFFFFF;
	for(i = 0; i < ul_NbWorlds; i++)
	{
		if(!pst_Grp[i].ul_Key) continue;
		if(pst_Grp[i].ul_Key == 0xFFFFFFFF) continue;
		if(firstindex == 0xFFFFFFFF) firstindex = i;
		lastindex = i;
	}

	for(i = 0; i < ul_NbWorlds; i++)
	{
		if(!pst_Grp[i].ul_Key) continue;
		if(pst_Grp[i].ul_Key == (ULONG) - 1) continue;
#ifdef ACTIVE_EDITORS
		ul_Pos = BIG_ul_SearchKeyToFat(pst_Grp[i].ul_Key);
		if(ul_Pos == 0xFFFFFFFF || !BIG_b_IsFileExtension(ul_Pos, EDI_Csz_ExtWorld))
		{
			sprintf(asz_Msg, "World file contains a bad file (%x)", pst_Grp[i].ul_Key);
			ERR_X_Warning(0, asz_Msg, NULL);
			continue;
		}
#endif
		WOR_gb_RealAllocWorld = (i == firstindex);
		WOR_gb_FinalWorld = (i == lastindex);
		pst_World = WOR_pst_Universe_MergeWorld(pst_World, pst_Grp[i].ul_Key, _c_ForEditor, FALSE);
	}

	
_Catch_
	pst_World = NULL;
_End_

	WOR_gb_RealAllocWorld = TRUE;
	WOR_gb_FinalWorld = TRUE;

    
	LOA_EndSpeedMode();
	LOA_StopLoadRaster(LOA_Cte_LDI_SpeedMode_Engine);	

	TEXT_LoadWorldText();
    SND_LoadAllMapSounds();

	if(!pst_World)
	{
#ifdef ACTIVE_EDITORS
        if (!_c_ForEditor)
		    BAS_bfree(&WOR_ListAllKeys);
#endif
		
#ifdef PSX2_TARGET
		GSP_EndWorldLoad();
#endif	
#ifdef _XBOX
		Gx8_EndWorldLoad();
#endif	

		MEM_gb_EnableTmp = FALSE;
		return NULL;
	}

	/* Set key of wol */
	pst_World->h_WorldKey = _ul_FileKey;

	/* DRL: Added group & father checking from final game here */
	WOR_World_AddRefBeforeCheckGroup( pst_World );
	WOR_World_CheckFathersOfWorld( pst_World );
	WOR_World_CheckGroupOfWorld( pst_World );

	/* Check all loaded objects */
#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
		OBJ_tdst_GameObject *pst_GO;
		BIG_KEY				ul_GOKey;
		BIG_INDEX			ul_File;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_PFElem = TAB_pst_PFtable_GetFirstElem(&pst_World->st_AllWorldObjects);
		pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&pst_World->st_AllWorldObjects);
		for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
			if(TAB_b_IsAHole(pst_GO)) continue;
			if(!pst_GO) continue;
			ul_GOKey = LOA_ul_SearchKeyWithAddress((ULONG) pst_GO);
			BAS_bdelete(ul_GOKey, &WOR_ListAllKeys);
		}

		while(WOR_ListAllKeys.num)
		{
			ul_GOKey = WOR_ListAllKeys.base[--WOR_ListAllKeys.num].ul_Key;
			if(ul_GOKey != BIG_C_InvalidKey)
			{
				ul_File = BIG_ul_SearchKeyToFat(ul_GOKey);
				pst_GO = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(ul_File));
				if((int) pst_GO == -1) continue;
				if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone) continue;
				sprintf(asz_Msg, "Object 0x%x has been loaded outside world", ul_GOKey);
				ERR_X_Warning(0, asz_Msg, NULL);
			}
		}

        if (!_c_ForEditor)
		    BAS_bfree(&WOR_ListAllKeys);
	}

#endif
	

	/* Load new textures if needed */
	if(WOR_gb_HasAddedMaterial)
	{
#ifdef ACTIVE_EDITORS
		GDI_l_AttachWorld((GDI_tdst_DisplayData *) pst_World->pst_View->st_DisplayInfo.pst_DisplayDatas, pst_World);
#endif
		WOR_gb_HasAddedMaterial = FALSE;
	}

    // retire de la VRam les textures encore en VRAM et qui ne seront pas utilisées par le monde
    // ce système ne fonctionne que si le flag TEX_Manager_FixVRam est défini dans le Texture Manager
    pst_DD = ((GDI_tdst_DisplayData *) pst_World->pst_View->st_DisplayInfo.pst_DisplayDatas);
#ifndef PSX2_TARGET
    if (pst_DD)
        pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_TextureUnloadCompare, 0 );
#endif        

	ENG_gb_ForceAttach = TRUE;

	/* Resolve All AI references. */
	WOR_World_ResolveAIRef(pst_World);

	L_memset(AI_gpst_MainActors, 0, sizeof(AI_gpst_MainActors));
	for(i = 0; i < AI_CXT_Max; i++) AI_gai_Differed[i] = 0;

	/* Rebuild all tables */
	WOR_World_ActivateObjects(pst_World);
	EOT_SetOfEOT_Build(&pst_World->st_EOT, &pst_World->st_ActivObjects);

#ifdef ACTIVE_EDITORS
	OBJ_ReComputeAllBVs(&pst_World->st_AllWorldObjects);
#endif
//	MEM_Free(pst_Grp);

	/* Free loading buffers */
	BIG_FreeGlobalBuffer();

#ifdef _GAMECUBE
{
	extern int mem_available;
	mem_available = MEM_uGetLastBlockSize(&MEM_gst_MemoryInfo) - ((3 * 1024 + 512) * 1024);	
}
#endif

	/* Load textures */
#ifndef ACTIVE_EDITORS
	{
		extern void DisplayAttach(GDI_tdst_DisplayData *);
		MAI_gst_MainHandles.pst_World = pst_World;
		DisplayAttach(MAI_gst_MainHandles.pst_DisplayData);
	}
#endif

	/* Finit la memoire temporaire */
	MEM_gb_EnableTmp = FALSE;
	MEM_ResetTmpMemory();

#ifndef ACTIVE_EDITORS
	ResetPreloadTexAll();
#endif

#ifdef PSX2_TARGET
	GSP_EndWorldLoad();
#endif
#ifdef _XBOX
		h_SaveWorldKey = pst_World->h_WorldKey;
		Gx8_EndWorldLoad();
#endif	
#ifdef _GAMECUBE
	GXI_StartFlip();
#endif

/*
#ifdef _DX8
    WOR_vComputeDX8FriendlyData(pst_World);
#endif
*/

#if defined(_PC_RETAIL)
	Dx9_PrepareVB( pst_World );
#endif	// defined(_PC_RETAIL)

#if defined(_XBOX) || (defined(_XENON) && defined(_GX8))
    Gx8_VertexBuffer_Clean();
	Gx8_PrepareVB( pst_World );
#endif	// defined(_XBOX)

#ifdef JADEFUSION
	void SPG2Holder_Modifier_Prepare();
    SPG2Holder_Modifier_Prepare();
#endif

#ifndef _FINAL_
//	ACT_ul_ComputeWanted();
#endif

#ifdef _XENON
	xeINO_ResetInactivityTimer( TimerExitDemo );
#endif


#if 0


#ifdef ACTIVE_EDITORS
	if (pst_World)
		if (MessageBox(MAI_gh_MainWindow, "Update all prefabs ?", "Appuie sur le bouton", MB_YESNO| MB_ICONQUESTION) == IDYES)
			WOR_UpdateAllPrefabs(pst_World, &pst_World->st_ActivObjects);
#endif


#endif

	return pst_World;
}

/*
 =======================================================================================================================
    Aim:    Add a world to universe. This load can be the first of a merge. If it is the case, we must not resolve the
            references that could be in another world.
 =======================================================================================================================
 */
WOR_tdst_World *WOR_pst_Universe_AddWorld(BIG_KEY _ul_FileKey, char _c_ForEditor, BOOL _b_ResolveAllRef)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Load */
	pst_World = WOR_pst_World_Load(NULL, _ul_FileKey, _b_ResolveAllRef);	
	if(pst_World == NULL) return NULL;

	/* Init */
	ENG_InitOneWorld(pst_World);	

	/* Rebuild all tables */
	if(_b_ResolveAllRef)
	{
		WOR_World_ActivateObjects(pst_World);
		EOT_SetOfEOT_Build(&pst_World->st_EOT, &pst_World->st_ActivObjects);
#ifdef ACTIVE_EDITORS
		OBJ_ReComputeAllBVs(&pst_World->st_ActivObjects);
#endif
	}

	if(TAB_ul_PFtable_GetElemIndexWithPointer(&WOR_gst_Universe.st_WorldsTable, pst_World) == TAB_Cul_BadIndex)
	{
		TAB_PFtable_AddElemWithDataAndResize(&WOR_gst_Universe.st_WorldsTable, pst_World, 0);
#ifdef ACTIVE_EDITORS
		pst_World->c_EditorReferences = (_c_ForEditor) ? 1 : 0;
#endif
	}

#ifdef ACTIVE_EDITORS
	else
	{
		if(pst_World->c_EditorReferences != 0)
		{
			if(!_c_ForEditor)
				pst_World->c_EditorReferences = 0;
			else
				pst_World->c_EditorReferences++;
		}
	}

	LINK_RefreshMenu();

#endif
	return pst_World;
}

/*
 =======================================================================================================================
    Aim:    Merge a world. There is a difference between a single merge and a multiple one. We do not build the EOT
            tables if it is a Mutliple merge. In that case, the EOT build will be done after the last merge has been
            completed.
 =======================================================================================================================
 */
WOR_tdst_World *WOR_pst_Universe_MergeWorld
(
	WOR_tdst_World	*_pst_World,
	BIG_KEY			_ul_FileKey,
	char			_c_ForEditor,
	BOOL			_b_SingleMerge
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*_pst_ret;
	int				ret;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_World)
	{
		_pst_ret = WOR_pst_Universe_AddWorld(_ul_FileKey, _c_ForEditor, _b_SingleMerge);
		return _pst_ret;
	}

	/* Load */
	ret = (int) WOR_pst_World_Load(_pst_World, _ul_FileKey, _b_SingleMerge);
	
	if(!ret) 
	{
	    return NULL;
	}

	/* Load new textures */
	if(_b_SingleMerge && WOR_gb_HasAddedMaterial)
	{
		if(_pst_World->pst_View)
#ifdef PSX2_TARGET
			GDI_l_AttachWorld(GDI_gpst_CurDD, _pst_World);
#else
		GDI_l_AttachWorld((GDI_tdst_DisplayData *) _pst_World->pst_View->st_DisplayInfo.pst_DisplayDatas, _pst_World);
#endif
		WOR_gb_HasAddedMaterial = FALSE;
	}

    
	/* Init */
	ENG_InitOneWorld(_pst_World);

	/* Rebuild all tables only if there is just a single merge */
	if(_b_SingleMerge)
	{
		WOR_World_ActivateObjects(_pst_World);
		EOT_SetOfEOT_Build(&_pst_World->st_EOT, &_pst_World->st_ActivObjects);
	}

#ifdef ACTIVE_EDITORS
	LINK_RefreshMenu();
#endif

	return _pst_World;
}

/*
 =======================================================================================================================
    Aim:    Delete a world in world universe list
 =======================================================================================================================
 */
void WOR_Universe_DeleteWorld(WOR_tdst_World *_pst_World, char _c_ForEditor)
{
    {
    	extern void SPG2_CloseWorldHook();
    	SPG2_CloseWorldHook(); // Release computed GRID
    }

	if(!_pst_World) return;

#if defined(ACTIVE_EDITORS) && defined(_XENON_RENDER)
    Xe_InvalidateRenderLists();
#endif

#ifdef ACTIVE_EDITORS

	WORGos_DestroyGroup(_pst_World);

//	AI_InitBreakList();
	if(_c_ForEditor != 3)
	{
		if(_c_ForEditor)
		{
			if(!_pst_World->c_EditorReferences) return;
			_pst_World->c_EditorReferences--;
			if((_c_ForEditor == 2) && (_pst_World->c_EditorReferences)) return;
		}
		else
		{
			if(_pst_World->c_EditorReferences) return;
		}
	}

#endif
	if(TAB_ul_PFtable_GetElemIndexWithPointer(&WOR_gst_Universe.st_WorldsTable, _pst_World) != TAB_Cul_BadIndex)
	{
		if(!WOR_World_Destroy(_pst_World))
			TAB_PFtable_RemoveElemWithPointer(&WOR_gst_Universe.st_WorldsTable, _pst_World);
#ifdef ACTIVE_EDITORS
		else if(_c_ForEditor)
			_pst_World->c_EditorReferences++;
		if(!ENG_gb_ExitApplication) LINK_UpdatePointers();
#endif
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
WOR_tdst_World *WOR_pst_Universe_GetActiveWorld(int _i_Start)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			ul_CurrentWorld;
	WOR_tdst_World	*pst_World;
	TAB_tdst_PFelem *pst_CurrentElem;
	TAB_tdst_PFelem *pst_EndElem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
	pst_CurrentElem += _i_Start;
	ul_CurrentWorld = _i_Start;
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_World = (WOR_tdst_World *) pst_CurrentElem->p_Pointer;
		if(!TAB_b_IsAHole(pst_World))
		{
			if(WOR_b_Universe_IsWorldActive(ul_CurrentWorld))
				return WOR_pst_Universe_GetWorldPointer(ul_CurrentWorld);
		}

		ul_CurrentWorld++;
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG WOR_l_Universe_GetWorldIndex(WOR_tdst_World *_pst_World)
{
	return TAB_ul_PFtable_GetElemIndexWithPointer(&WOR_gst_Universe.st_WorldsTable, _pst_World);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_Universe_Update_GroPointerChange(void *_p_New, void *_p_Old)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	TAB_tdst_PFelem *pst_CurrentElem, *pst_EndElem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_World = (WOR_tdst_World *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_World)) continue;
		WOR_World_Update_GroPointerChange(pst_World, _p_New, _p_Old);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_Universe_Update_RLI(WOR_tdst_Update_RLI *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	TAB_tdst_PFelem *pst_CurrentElem, *pst_EndElem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_World = (WOR_tdst_World *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_World)) continue;
		WOR_World_Update_RLI(pst_World, _pst_Data);
	}
}

#define WOR_Cte_MaxFixWorldNb	8
ULONG	WOR_gul_FixWorldKeys[WOR_Cte_MaxFixWorldNb] = {0};

ULONG	WOR_gul_FixWorldLoaded[WOR_Cte_MaxFixWorldNb] = {0};
ULONG	WOR_gul_FixWorldToLoad[WOR_Cte_MaxFixWorldNb] = {0};
ULONG	WOR_gul_FixWorldToUnload[WOR_Cte_MaxFixWorldNb] = {0};
BAS_tdst_barray WOR_gst_LoadedKeyForCurrentFix;

void WOR_RegisterFixWorldKey(int _l_Index, ULONG _ul_Key)
{
	if(_l_Index < 0) return;
	if(_l_Index >= WOR_Cte_MaxFixWorldNb) return;

	WOR_gul_FixWorldKeys[_l_Index] = _ul_Key;
}

void WOR_RequestFixWorld(ULONG _ul_Key)
{
	int i;

	// if binarizing
	if(BIG_gi_ReadMode == 1) return;

	for(i=0; i<WOR_Cte_MaxFixWorldNb;i++)
	{
		// find the correct index
		if(WOR_gul_FixWorldKeys[i] != _ul_Key)	continue;	

		// yet loaded
		if(WOR_gul_FixWorldLoaded[i])			return;		

		// regsiter to be loaded it
		WOR_gul_FixWorldToLoad[i] = 1;
		return;
	}
}

void WOR_ResolveFixWorldRequest(void)
{
	int i;
	extern void LOA_MakeSpecialArrayAddrToDestroy(void);


	for(i=0; i<WOR_Cte_MaxFixWorldNb; i++)
	{
		// skip unloaded fix
		if(!WOR_gul_FixWorldLoaded[i]) continue;
		
		// now unload unwanted loaded fix
		if(WOR_gul_FixWorldToLoad[i])
			WOR_gul_FixWorldToUnload[i] = 0;
		else
			WOR_gul_FixWorldToUnload[i] = 1;
	}

	LOA_MakeSpecialArrayAddrToDestroy();
}


















