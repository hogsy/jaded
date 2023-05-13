/*$T MEMpro.c GC! 1.081 07/22/02 14:13:58 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#if !defined(_XENON)
#define __MEMPRO_C__
#endif
#include "MEM.h"
#ifdef _GAMECUBE
#include "GC_arammng.h"
#endif
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEMpro.h"

#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/AImsg.h"
#include "AIinterp/Sources/AIsave.h"
#include "AIinterp/Sources/Events/EVEconst.h"
#include "AIinterp/Sources/Events/EVEplay.h"

#ifndef MEM_OPT

#ifdef PSX2_TARGET
static struct mallinfo	MEMpro_sst_CurrentState;
#endif
extern void MEM_PrintHistory(void);
/*$4
 ***********************************************************************************************************************
    private macros
 ***********************************************************************************************************************
 */

#ifdef MEMpro_M_ActiveRasters
#define M_AddrOf(_id)	&MEMpro_sa_RasterList[_id]

/* constants */
#define M_MainMemorySizeOnPS2Kit	(128 * 1024 * 1024)
#define M_MainMemorySizeOnPS2		(32 * 1024 * 1024)
#define M_MainMemorySize			M_MainMemorySizeOnPS2Kit
#define M_VU0MemSize				(4 * 1024)
#define M_VU1MemSize				(16 * 1024)
#define M_VU0MicroMemSize			(4 * 1024)
#define M_VU1MicroMemSize			(16 * 1024)
#define M_RasterStackDepth			16
#define M_RasterNameSize			64

/*$4
 ***********************************************************************************************************************
    private types
 ***********************************************************************************************************************
 */

typedef struct	MEMpro_tdst_MemStat_
{
	int i_SpaceAllocated;
	int i_MaxSpaceAllocated;
	int i_MinSpaceAllocated;
} MEMpro_tdst_MemStat;

typedef struct	MEMpro_tdst_MemRaster_
{
	MEMpro_tdst_MemStat				st_RTLib;
	MEMpro_tdst_MemStat				st_SDK;
	unsigned int					ui_Nb;
	struct MEMpro_tdst_MemRaster_	*pst_NextRaster;
	struct MEMpro_tdst_MemRaster_	*pst_NextGroup;
	char							asz_Name[M_RasterNameSize];
} MEMpro_tdst_MemRaster;

typedef enum						MEMpro_tden_MemRasterDisplay_
{
	MEMpro_en_DisplayRasters,
	MEMpro_en_DisplayMapping,
	MEMpro_en_DisplayHeapManaging
} MEMpro_tden_MemRasterDisplay;

/*$4
 ***********************************************************************************************************************
    external variables
 ***********************************************************************************************************************
 */

static void							MEMpro_PrintMemState(void);

/*$4
 ***********************************************************************************************************************
    private variables
 ***********************************************************************************************************************
 */

/* lists */
static MEMpro_tdst_MemRaster		MEMpro_sa_RasterList[MEMpro_M_Number0fRasters];

/* stack */
static MEMpro_tdst_MemManager		MEMpro_sa_RasterStack[M_RasterStackDepth];
static unsigned int					MEMpro_sui_CurrentDepth = 0;

/* display */
static MEMpro_tden_MemRasterDisplay MEMpro_sen_DisplayCommand = MEMpro_en_DisplayRasters;
static char							MEMpro_sasz_Prompt[2 * M_RasterStackDepth + 1] = "";
static MEMpro_tdst_MemRaster		*MEMpro_sa_RasterChain[MEMpro_M_Number0fRasters][3] =
/*$off*/
{
	/* main */
	{ M_AddrOf(MEMpro_Id_BIG_Open),							M_AddrOf(MEMpro_Id_LOA_ResolveArray),				NULL },
	{ M_AddrOf(MEMpro_Id_LOA_ResolveArray),					M_AddrOf(MEMpro_Id_ANI_Load),						NULL },

	{ M_AddrOf(MEMpro_Id_ANI_Load),							M_AddrOf(MEMpro_Id_EVE_ListEvents),					NULL },
	{ M_AddrOf(MEMpro_Id_EVE_ListEvents),					M_AddrOf(MEMpro_Id_AI_Models),						NULL },

	{ M_AddrOf(MEMpro_Id_AI_Models),						M_AddrOf(MEMpro_Id_AI_Functions),					NULL },
	{ M_AddrOf(MEMpro_Id_AI_Functions),						M_AddrOf(MEMpro_Id_AI_Vars),						NULL },
	{ M_AddrOf(MEMpro_Id_AI_Vars),							M_AddrOf(MEMpro_Id_GDK_ObjectCallback),				NULL },

	{ M_AddrOf(MEMpro_Id_GDK_ObjectCallback),				M_AddrOf(MEMpro_Id_GDI_fnpst_CreateDisplayData_1),	NULL },
	{ M_AddrOf(MEMpro_Id_GDI_fnpst_CreateDisplayData_1),	M_AddrOf(MEMpro_Id_OGL_ul_Texture_Create),			NULL },
	{ M_AddrOf(MEMpro_Id_OGL_ul_Texture_Create),			M_AddrOf(MEMpro_Id_GDK_MultiTexture),				NULL },
	{ M_AddrOf(MEMpro_Id_GDK_MultiTexture),					M_AddrOf(MEMpro_Id_GDK_SingleTexture),				NULL },
	{ M_AddrOf(MEMpro_Id_GDK_SingleTexture),				M_AddrOf(MEMpro_Id_Bigfread),						NULL },
	{ M_AddrOf(MEMpro_Id_Bigfread),							M_AddrOf(MEMpro_Id_BeginSpeedMode),					NULL },
	{ M_AddrOf(MEMpro_Id_BeginSpeedMode),								M_AddrOf(MEMpro_Id_EndSpeedMode),					NULL },
	{ M_AddrOf(MEMpro_Id_EndSpeedMode),						M_AddrOf(MEMpro_Id_GSP),							NULL },
	{ M_AddrOf(MEMpro_Id_GSP),														NULL,												NULL },
};
/*$on*/

/*$4
 ***********************************************************************************************************************
    private functions
 ***********************************************************************************************************************
 */

static void							MEMpro_PrintNext(MEMpro_tdst_MemRaster *);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void MEMpro_PrintGroup(MEMpro_tdst_MemRaster *_pRaster)
{
	/*~~~~~~~~~~~~~~~~~*/
	static int	i = 0;
	char		az[2048];
	/*~~~~~~~~~~~~~~~~~*/

	MEMpro_sasz_Prompt[i++] = ' ';
	MEMpro_sasz_Prompt[i++] = ' ';
	MEMpro_sasz_Prompt[i] = '\0';

#ifdef ACTIVE_EDITORS
	sprintf
	(
		az,
		"--%s %s : %.1f Ko [min %.1f max %.1f] (%d)",
		MEMpro_sasz_Prompt,
		_pRaster->asz_Name,
		_pRaster->st_SDK.i_SpaceAllocated / 1024.0f,
		_pRaster->st_SDK.i_MinSpaceAllocated / 1024.0f,
		_pRaster->st_SDK.i_MaxSpaceAllocated / 1024.0f,
		_pRaster->ui_Nb
	);
	LINK_PrintStatusMsg(az);
#else
	sprintf
	(
		az,
		"--%s %s : %.1f + %.1f Ko [%.1f/%.1f]+[%.1f/%.1f] (%d)",
		MEMpro_sasz_Prompt,
		_pRaster->asz_Name,
		_pRaster->st_RTLib.i_SpaceAllocated / 1024.0f,
		_pRaster->st_SDK.i_SpaceAllocated / 1024.0f,
		_pRaster->st_RTLib.i_MinSpaceAllocated / 1024.0f,
		_pRaster->st_RTLib.i_MaxSpaceAllocated / 1024.0f,
		_pRaster->st_SDK.i_MinSpaceAllocated / 1024.0f,
		_pRaster->st_SDK.i_MaxSpaceAllocated / 1024.0f,
		_pRaster->ui_Nb
	);
	printf(az);
	printf("\n");
#endif
	if(_pRaster->pst_NextGroup) MEMpro_PrintGroup(_pRaster->pst_NextGroup);
	if(_pRaster->pst_NextRaster) MEMpro_PrintNext(_pRaster->pst_NextRaster);

	MEMpro_sasz_Prompt[i--] = '\0';
	MEMpro_sasz_Prompt[i--] = '\0';
	MEMpro_sasz_Prompt[i] = '\0';
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void MEMpro_PrintNext(MEMpro_tdst_MemRaster *_pRaster)
{
	/*~~~~~~~~~~~~~*/
	char	az[2048];
	/*~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	sprintf
	(
		az,
		"--%s %s : %.1f Ko [min %.1f max %.1f] (%d)",
		MEMpro_sasz_Prompt,
		_pRaster->asz_Name,
		_pRaster->st_SDK.i_SpaceAllocated / 1024.0f,
		_pRaster->st_SDK.i_MinSpaceAllocated / 1024.0f,
		_pRaster->st_SDK.i_MaxSpaceAllocated / 1024.0f,
		_pRaster->ui_Nb
	);
	LINK_PrintStatusMsg(az);
#else
	sprintf
	(
		az,
		"--%s %s : %.1f + %.1f Ko [%.1f/%.1f]+[%.1f/%.1f] (%d)",
		MEMpro_sasz_Prompt,
		_pRaster->asz_Name,
		_pRaster->st_RTLib.i_SpaceAllocated / 1024.0f,
		_pRaster->st_SDK.i_SpaceAllocated / 1024.0f,
		_pRaster->st_RTLib.i_MinSpaceAllocated / 1024.0f,
		_pRaster->st_RTLib.i_MaxSpaceAllocated / 1024.0f,
		_pRaster->st_SDK.i_MinSpaceAllocated / 1024.0f,
		_pRaster->st_SDK.i_MaxSpaceAllocated / 1024.0f,
		_pRaster->ui_Nb
	);
	printf(az);
	printf("\n");
#endif
	if(_pRaster->pst_NextGroup) MEMpro_PrintGroup(_pRaster->pst_NextGroup);
	if(_pRaster->pst_NextRaster) MEMpro_PrintNext(_pRaster->pst_NextRaster);
}
#ifdef JADEFUSION
extern __declspec(align(32)) MEM_tdst_MainStruct MEM_gst_MemoryInfo;
#else
extern MEM_tdst_MainStruct	MEM_gst_MemoryInfo;
#endif
extern void					*MEM_gpv_LastFree;
extern ULONG				MEM_gul_FromEndMaxAllocSize;
void						MEM_ComputeMemoryHoleInfo(ULONG *, ULONG *, BOOL _b_Log);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEMpro_PrintMemRaster(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifndef ACTIVE_EDITORS
	ULONG	ul_NumHoles, ul_HoleSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	printf("-- ------------------------------------------------------\n");
	printf("MEM_gst_MemoryInfo.pv_StaticBloc                : 0x%08x\n", (int) MEM_gst_MemoryInfo.pv_StaticBloc);
	printf("MEM_gst_MemoryInfo.pv_DynamicBloc               : 0x%08x\n", (int) MEM_gst_MemoryInfo.pv_DynamicBloc);
	printf("MEM_gst_MemoryInfo.pv_DynamicNextFree           : 0x%08x\n", (int) MEM_gst_MemoryInfo.pv_DynamicNextFree);
	printf("MEM_gst_MemoryInfo.ul_DynamicMaxSize            : %d oct\n", (int) MEM_gst_MemoryInfo.ul_DynamicMaxSize);
	printf("MEM_gst_MemoryInfo.ul_StaticMaxSize             : %d oct\n", (int) MEM_gst_MemoryInfo.ul_StaticMaxSize);
	printf
	(
		"MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated   : %d oct\n",
		(int) MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated
	);
	printf
	(
		"MEM_gst_MemoryInfo.ul_StaticCurrentAllocated    : %d oct\n",
		(int) MEM_gst_MemoryInfo.ul_StaticCurrentAllocated
	);
	printf
	(
		"MEM_gst_MemoryInfo.ul_TexturesCurrentAllocated  : %d oct\n",
		(int) MEM_gst_MemoryInfo.ul_TexturesCurrentAllocated
	);
	printf("MEM_gst_MemoryInfo.ul_HolesStatic               : %d oct\n", (int) MEM_gst_MemoryInfo.ul_HolesStatic);

	MEM_ComputeMemoryHoleInfo(&ul_NumHoles, &ul_HoleSize, FALSE);
	printf
	(
		"Engine %u ko -- Textures %u ko -- WH %u ko -- Gran %u ko -- Holes % u -- Size Holes %u ko\n",
		MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated / 1024,
		MEM_gst_MemoryInfo.ul_TexturesCurrentAllocated / 1024,
		((char *) MEM_gst_MemoryInfo.pv_DynamicNextFree - (char *) MEM_gst_MemoryInfo.pv_DynamicBloc) / 1024,
		MEM_gst_MemoryInfo.ul_HolesStatic / 1024,
		ul_NumHoles,
		ul_HoleSize / 1024
	);
	printf("-- ------------------------------------------------------\n");
	printf("MEM_gpv_LastFree                                : %08x oct\n", (int) MEM_gst_MemoryInfo.pv_LastFree);
	printf("MEM_gul_RealSize                                : %d oct\n", MEM_gst_MemoryInfo.ul_RealSize);
	printf("MEM_gul_FromEndMaxAllocSize                     : %d oct\n", MEM_gul_FromEndMaxAllocSize);
	printf("-- ------------------------------------------------------\n");
	MEMpro_PrintMemState();
	printf("-- Memory rasters : RTLib [Min/Max] SDK [Min/Max] Nb\n");
#endif
	MEMpro_PrintNext(MEMpro_sa_RasterList);

#ifndef ACTIVE_EDITORS
	printf("-- ------------------------------------------------------\n");
#endif
}

/*
 =======================================================================================================================
    print dynamic memory state
 =======================================================================================================================
 */
static void MEMpro_PrintMemState(void)
{
#ifndef ACTIVE_EDITORS
	printf("-- Dynamic memory state <malloc.h> ----------------------\n");
	printf("-- heap space              : %3.2f Ko\n", MEMpro_sst_CurrentState.arena / 1024.0f);
	printf("--      allocated          : %3.2f Ko\n", MEMpro_sst_CurrentState.uordblks / 1024.0f);
	printf("--      free               : %3.2f Ko\n", MEMpro_sst_CurrentState.fordblks / 1024.0f);
	printf("-- unused chunks           : %3.2f Ko\n", MEMpro_sst_CurrentState.ordblks / 1024.0f);
	printf("-- mapped regions          : %3.2f Ko\n", MEMpro_sst_CurrentState.hblks / 1024.0f);
	printf("-- space in mapped regions : %3.2f Ko\n", MEMpro_sst_CurrentState.hblkhd / 1024.0f);
	printf("-- top-most releasable     : %3.2f Ko\n", MEMpro_sst_CurrentState.keepcost / 1024.0f);
	printf("-- ------------------------------------------------------\n");
#endif
}

/*
 =======================================================================================================================
    update the dynamic memory data
 =======================================================================================================================
 */
static _inline_ void MEMpro_Update(void)
{
#ifndef ACTIVE_EDITORS
	MEMpro_sst_CurrentState = mallinfo();
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void MEMpro_CreateMemRaster(MEMpro_tden_MemRasterId _Id, const char *_Name)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MEMpro_tdst_MemRaster	*Raster;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_Id < MEMpro_M_Number0fRasters)
	{
		Raster = &MEMpro_sa_RasterList[_Id];
		Raster->st_RTLib.i_SpaceAllocated = 0;
		Raster->st_RTLib.i_MaxSpaceAllocated = 0;
		Raster->st_RTLib.i_MinSpaceAllocated = (UINT) - 1;
		Raster->st_SDK.i_SpaceAllocated = 0;
		Raster->st_SDK.i_MaxSpaceAllocated = 0;
		Raster->st_SDK.i_MinSpaceAllocated = (UINT) - 1;
		Raster->ui_Nb = 0;
		Raster->pst_NextGroup = NULL;
		Raster->pst_NextRaster = NULL;
		if(_Name) L_memcpy(Raster->asz_Name, (char *) _Name, M_RasterNameSize);
		Raster->asz_Name[M_RasterNameSize - 1] = '\0';
	}
	else
		ERR_X_Warning(0, "There is no more MemRaster !", NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static _inline_ void MEMpro_Push(void)
{
	if(++MEMpro_gp_StackPointer == &MEMpro_sa_RasterStack[M_RasterStackDepth])
	{
		ERR_X_Error(0, "** Not enougth stack depth for memory rasters", NULL);
		MEMpro_gb_StackOverflow = TRUE;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static _inline_ void MEMpro_Pop(void)
{
	if(MEMpro_gp_StackPointer-- == &MEMpro_sa_RasterStack[M_RasterStackDepth]) MEMpro_gb_StackOverflow = FALSE;
}

/*$4
 ***********************************************************************************************************************
    public functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    MemRasters init, called before all others functions !
 =======================================================================================================================
 */
void MEMpro_Init(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	MEMpro_tden_MemRasterId i;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	/* reset stack */
	L_memset(MEMpro_sa_RasterStack, 0, sizeof(MEMpro_tdst_MemManager) * M_RasterStackDepth);
	MEMpro_gp_StackPointer = MEMpro_sa_RasterStack;
	MEMpro_gb_StackOverflow = FALSE;

	/* reset list of rasters */
	L_memset(&MEMpro_sa_RasterList, 0, sizeof(MEMpro_tdst_MemRaster) * MEMpro_M_Number0fRasters);

	/* init rasters */
	MEMpro_CreateMemRaster(MEMpro_Id_BIG_Open, "BIG_Open");
	MEMpro_CreateMemRaster(MEMpro_Id_LOA_ResolveArray, "LOA_Resolve array");
	MEMpro_CreateMemRaster(MEMpro_Id_GDI_fnpst_CreateDisplayData_1, "GDI_tdst_DisplayData");
	MEMpro_CreateMemRaster(MEMpro_Id_OGL_ul_Texture_Create, "OGL_ul_Texture_Create");
	MEMpro_CreateMemRaster(MEMpro_Id_ANI_Load, "ANI_pst_Load");
	MEMpro_CreateMemRaster(MEMpro_Id_EVE_ListEvents, "EVE_LoadListEvents");
	MEMpro_CreateMemRaster(MEMpro_Id_AI_Models, "AI_ul_CallbackLoadModel");
	MEMpro_CreateMemRaster(MEMpro_Id_AI_Functions, "AI_ul_CallbackLoadFunction");
	MEMpro_CreateMemRaster(MEMpro_Id_AI_Vars, "AI_ul_CallbackLoadVars");
	MEMpro_CreateMemRaster(MEMpro_Id_GDK_ObjectCallback, "GEO_ul_Load_ObjectCallback");
	MEMpro_CreateMemRaster(MEMpro_Id_GDK_MultiTexture, "MAT_pst_CreateMultiTextureFromBuffer");
	MEMpro_CreateMemRaster(MEMpro_Id_GDK_SingleTexture, "MAT_pst_CreateSingleFromBuffer");
	MEMpro_CreateMemRaster(MEMpro_Id_Bigfread, "Big_fread");
	MEMpro_CreateMemRaster(MEMpro_Id_BeginSpeedMode, "BeginSpeedMode");
	MEMpro_CreateMemRaster(MEMpro_Id_EndSpeedMode, "EndSpeedMode");
	MEMpro_CreateMemRaster(MEMpro_Id_GSP, "GSP");

	/* read the chain */
	for(int loop = 0; loop < MEMpro_M_Number0fRasters; loop++)
	{
		MEMpro_sa_RasterChain[loop][0]->pst_NextRaster = MEMpro_sa_RasterChain[loop][1];
		MEMpro_sa_RasterChain[loop][0]->pst_NextGroup = MEMpro_sa_RasterChain[loop][2];
	}

	/* init of heap raster */
	MEMpro_Update();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEMpro_StartMemRaster(void)
{
	MEMpro_Push();

	if(MEMpro_gb_StackOverflow) return;

	MEMpro_gp_StackPointer->ui_Alloc = 0;
	MEMpro_gp_StackPointer->ui_Free = 0;

	MEMpro_Update();
#ifndef ACTIVE_EDITORS
	MEMpro_gp_StackPointer->ui_RTalloc = MEMpro_sst_CurrentState.uordblks;
#endif
	MEMpro_sui_CurrentDepth++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEMpro_StopMemRaster(MEMpro_tden_MemRasterId _RasterId)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						size;
	int						size1;
	MEMpro_tdst_MemRaster	*Raster;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Raster = &MEMpro_sa_RasterList[_RasterId];

	if(MEMpro_gb_StackOverflow)
	{
		L_memset(Raster, 0xFF, sizeof(MEMpro_tdst_MemRaster));
	}
	else
	{
		MEMpro_Update();
#ifndef ACTIVE_EDITORS
		size = MEMpro_sst_CurrentState.uordblks - MEMpro_gp_StackPointer->ui_RTalloc;
		if(size)
		{
			Raster->st_RTLib.i_SpaceAllocated += size;
			Raster->st_RTLib.i_MaxSpaceAllocated = max
				(
					Raster->st_RTLib.i_MaxSpaceAllocated,
					Raster->st_RTLib.i_SpaceAllocated
				);
			Raster->st_RTLib.i_MinSpaceAllocated = min
				(
					Raster->st_RTLib.i_MinSpaceAllocated,
					Raster->st_RTLib.i_SpaceAllocated
				);
		}

#endif
		/* ui_Nb of Raster calls */
		Raster->ui_Nb++;

		/*
		 * due to realloc operations, we allocate before to free memory so the max is
		 * calculate before substration
		 */
		size = MEMpro_gp_StackPointer->ui_Alloc;
		Raster->st_SDK.i_SpaceAllocated += size;
		Raster->st_SDK.i_MaxSpaceAllocated = max(Raster->st_SDK.i_MaxSpaceAllocated, Raster->st_SDK.i_SpaceAllocated);

		size1 = MEMpro_gp_StackPointer->ui_Free;
		Raster->st_SDK.i_SpaceAllocated -= size1;
		Raster->st_SDK.i_MinSpaceAllocated = min(Raster->st_SDK.i_MinSpaceAllocated, Raster->st_SDK.i_SpaceAllocated);
	}

	MEMpro_Pop();

	/* add allocations done in a subfunction */
	if(MEMpro_sui_CurrentDepth-- > 1)
	{
		MEMpro_gp_StackPointer->ui_Alloc += size;
		MEMpro_gp_StackPointer->ui_Free += size1;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEMpro_ChangeDisplay(void)
{
#ifdef JADEFUSION
	MEMpro_sen_DisplayCommand = (MEMpro_tden_MemRasterDisplay)((int)MEMpro_sen_DisplayCommand + 1);
	switch(MEMpro_sen_DisplayCommand)
#else
	switch(++MEMpro_sen_DisplayCommand)
#endif
	{
	case MEMpro_en_DisplayRasters:
		printf("-- New memory rasters display: rasters\n");
		printf("-- Rasters <<-> Mapping <<-> HeapManaging\n");
		break;
	case MEMpro_en_DisplayMapping:
		printf("-- New memory rasters display: mapping\n");
		printf("-- Rasters <<-> Mapping <<-> HeapManaging\n");
		break;
	case MEMpro_en_DisplayHeapManaging:
		printf("-- New memory rasters display: heap manager\n");
		printf("-- Rasters <<-> Mapping <<-> HeapManaging\n");
		break;
	default:
		MEMpro_sen_DisplayCommand = MEMpro_en_DisplayRasters;
		printf("-- New memory rasters display: rasters\n");
		printf("-- Rasters <<-> Mapping <<-> HeapManaging\n");
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEMpro_Print(void)
{
	switch(MEMpro_sen_DisplayCommand)
	{
	case MEMpro_en_DisplayRasters:
		MEMpro_PrintMemRaster();
		break;
	case MEMpro_en_DisplayHeapManaging:
		MEMpro_PrintMemState();
		break;
	default:
		printf("-- error\n");
		break;
	}
}
#ifdef JADEFUSION
ULONG	MEMpro_getAnimMemory(void)
{
	return MEMpro_sa_RasterList[MEMpro_Id_ANI_Load].st_SDK.i_SpaceAllocated;
}

int	MEMpro_GetAnimNumber(void)
{
	return MEMpro_sa_RasterList[MEMpro_Id_ANI_Load].ui_Nb;
}

ULONG	MEMpro_getAICBMemory(void)
{
	return MEMpro_sa_RasterList[MEMpro_Id_AI_Models].st_SDK.i_SpaceAllocated;
}

int		MEMpro_GetAICBNumber(void)
{
	return MEMpro_sa_RasterList[MEMpro_Id_AI_Models].ui_Nb;
}

ULONG	MEMpro_getAIFuncMemory(void)
{
	return MEMpro_sa_RasterList[MEMpro_Id_AI_Functions].st_SDK.i_SpaceAllocated;
}

int		MEMpro_GetAIFuncNumber(void)
{
	return MEMpro_sa_RasterList[MEMpro_Id_AI_Functions].ui_Nb;
}

ULONG	MEMpro_getAIVarsMemory(void)
{
	return MEMpro_sa_RasterList[MEMpro_Id_AI_Vars].st_SDK.i_SpaceAllocated;
}

int		MEMpro_GetAIVarsNumber(void)
{
	return MEMpro_sa_RasterList[MEMpro_Id_AI_Vars].ui_Nb;
}

ULONG	MEMpro_getObjMemory(void)
{
	return MEMpro_sa_RasterList[MEMpro_Id_GDK_ObjectCallback].st_SDK.i_SpaceAllocated;
}

int		MEMpro_GetObjNumber(void)
{
	return MEMpro_sa_RasterList[MEMpro_Id_GDK_ObjectCallback].ui_Nb;
}
#endif
#endif /* MEMpro_M_ActiveRasters */

#if defined(PSX2_TARGET) || defined(_GAMECUBE)

/*$4
 ***********************************************************************************************************************
    only PS2 fonct
 ***********************************************************************************************************************
 */

#ifdef PSX2_TARGET
#define _DrawGlobalRasters	GSP_DrawGlobalRasters
extern void GSP_DrawGlobalRasters(char *, float FrameNumb, float YPos, float ARS, u_int TheColor);
#else
#include <__ppc_eabi_linker.h>
#define _DrawGlobalRasters	GXI_DrawGlobalRasters
extern void GXI_DrawGlobalRasters(char *, float FrameNumb, float YPos, float ARS, unsigned int TheColor);
extern void GXI_DrawText(int X,int Y,char *P_String,long Color,long ColorBack,long TFront,long TBack);
extern int ARAM_Anims_RAM ;
extern int ARAM_Anims_ARAM ;
#endif

void		MEM_RasterSetRequest(int rq);
int			MEM_i_RasterGetStatus(void);
int			MEM_gi_RasterState = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int MEM_i_RasterGetStatus(void)
{
	return MEM_gi_RasterState;
}

extern int	GetStackSize(void);
extern int	GetHeapSize(void);

#define USE_HOLE_OPTIM

#ifdef USE_HOLE_OPTIM
extern ULONG MEM_ul_NumOldHoles;
extern ULONG MEM_ul_SizeOldHoles;
#endif

extern int					MEM_gi_MinDelta;
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_RasterSetRequest(int rq)
{
	/*~~~~~~~~~*/
	float	f, y;
	/*~~~~~~~~~*/

	switch(MEM_gi_RasterState)
	{

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    dynamic
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case 1:
	
#ifdef _GAMECUBE
    {
    	char 		asz_Log[512];

    	y = 26.0f;

    	sprintf(asz_Log,"Ani RAM %d - Ani ARAM %d", ARAM_Anims_RAM, ARAM_Anims_ARAM);		
    	GXI_DrawText(8L,(LONG)(0.75 * y), asz_Log, 0xffeeffee, 0xff202020, MAT_Cc_Op_Copy, MAT_Cc_Op_Sub);

#ifdef USE_HOLE_OPTIM
    	y += 24.0f;
    	sprintf(asz_Log,"Num Old Holes %d - Size Old Holes %d Bytes", MEM_ul_NumOldHoles, MEM_ul_SizeOldHoles);		
    	GXI_DrawText(8L,(LONG)(0.75 * y), asz_Log, 0xffeeffee, 0xff202020, MAT_Cc_Op_Copy, MAT_Cc_Op_Sub);
#endif    	

		y = 0.75f;
		f = (float)ARAM_TotalAlloc_Level/(float)g_u32ARAM_BlocksSize[eARAM_Game];
		_DrawGlobalRasters
		(
			"ARAMlevel",
			f /* value */ ,
			y /* ypos */ ,
			f * ((float)g_u32ARAM_BlocksSize[eARAM_Game]/(1024.0f*1024.0f)) /* f */ /* avgval */ ,
			0xf0808080		/* color */
		);
		y += 0.05f;
		f = (float)ARAM_TotalAlloc_Fix/(float)g_u32ARAM_BlocksSize[eARAM_Game];
		_DrawGlobalRasters
		(
			"ARAMFix",
			f /* value */ ,
			y /* ypos */ ,
			f * (g_u32ARAM_BlocksSize[eARAM_Game]/(1024.0f*1024.0f)) /* f */ /* avgval */ ,
			0xf0808080		/* color */
		);
		y += 0.05f;
	}
#endif
		y = 0.80f;
		f = fDiv
			(
				fLongToFloat(MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated),
				fLongToFloat(MEM_gst_MemoryInfo.ul_DynamicMaxSize)
			);
		_DrawGlobalRasters
		(
			"Delta",
			f /* value */ ,
			y /* ypos */ ,
			MEM_gi_MinDelta / (1024.0f*1024.0f) /* f */ /* avgval */ ,
			0xf0808080		/* color */
		);
		y += 0.05f;

		f = fDiv
			(
				fLongToFloat(MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated),
				fLongToFloat(MEM_gst_MemoryInfo.ul_DynamicMaxSize)
			);
		_DrawGlobalRasters
		(
			"Alloc",
			f /* value */ ,
			y /* ypos */ ,
			MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated / (1024.0f*1024.0f) /* f */ /* avgval */ ,
			0xf0808080		/* color */
		);
		y += 0.05f;

		f = fLongToFloat((int) MEM_gst_MemoryInfo.pv_DynamicNextFree - (int) MEM_gst_MemoryInfo.pv_DynamicBloc) /
			fLongToFloat(MEM_gst_MemoryInfo.ul_DynamicMaxSize);
			_DrawGlobalRasters
			(
				"Fragm",
				f /* value */ ,
				y /* ypos */ ,
				f * MEM_gst_MemoryInfo.ul_DynamicMaxSize / (1024.0f*1024.0f) /* f */ /* avgval */ ,
				0xf0808000	/* color */
			);
		y += 0.05f;
		
		f = fLongToFloat(MEM_gst_MemoryInfo.ul_RealSize) / fLongToFloat(MEM_gst_MemoryInfo.ul_DynamicMaxSize);
		_DrawGlobalRasters("Dispo", f /* value */ , y /* ypos */ , fLongToFloat(MEM_gst_MemoryInfo.ul_RealSize)/(1024.0f*1024.0f) /* avgval */ , 0xF0008000 /* color */ );
		y += 0.05f;
		break;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    static
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case 2:
#ifdef _GAMECUBE
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			float	exe, data, bss, mem, sbss, sdata, stack, total, other;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			y = 0.55f;

			exe = fLongToFloat((u32) _e_text - (u32) _f_text) / (24 * 1024 * 1024);
			_DrawGlobalRasters("code", exe /* value */ , y /* ypos */ , exe * 24 /* avgval */ , 0xF0000080 /* color */ );
			y += 0.05f;

			data = fLongToFloat((u32) _e_data - (u32) _f_data) / (24 * 1024 * 1024);
			_DrawGlobalRasters("data", data /* value */ , y /* ypos */ , data * 24 /* avgval */ , 0xF0000080 /* color */ );
			y += 0.05f;

			bss = fLongToFloat((u32) _e_bss - (u32) _f_bss) / (24 * 1024 * 1024);
			_DrawGlobalRasters("bss", bss /* value */ , y /* ypos */ , bss * 24 /* avgval */ , 0xF0000080 /* color */ );
			y += 0.05f;

			sdata = fLongToFloat((u32) _e_sdata - (u32) _f_sdata + (u32) _e_sdata2 - (u32) _f_sdata2) / (24 * 1024 * 1024);
			_DrawGlobalRasters("sdata", sdata /* value */ , y /* ypos */ , sdata * 24 /* avgval */ , 0xF0000080 /* color */ );
			y += 0.05f;

			sbss = fLongToFloat((u32) _e_sbss - (u32) _f_sbss + (u32) _e_sbss2 - (u32) _f_sbss2) / (24 * 1024 * 1024);
			_DrawGlobalRasters("sbss", sbss /* value */ , y /* ypos */ , sbss * 24 /* avgval */ , 0xF0000080 /* color */ );
			y += 0.05f;

			other = fLongToFloat((u32) _e_rodata - (u32) _f_rodata + (u32) _e_init - (u32) _f_init) / (24 * 1024 * 1024);
			_DrawGlobalRasters("other", other /* value */ , y /* ypos */ , other * 24 /* avgval */ , 0xF0000080 /* color */ );
			y += 0.05f;

			mem = fLongToFloat(MEM_gst_MemoryInfo.ul_RealSize) / (24 * 1024 * 1024);
			_DrawGlobalRasters("MEMdyn", mem /* value */ , y /* ypos */ , mem * 24 /* avgval */ , 0xF0008000 /* color */ );
			y += 0.05f;

			stack = fLongToFloat(_stack_addr - _stack_end) / (24 * 1024 * 1024);
			_DrawGlobalRasters("stck", stack /* value */ , y /* ypos */ , stack * 24 /* avgval */ , 0xF0800000 /* color */ );
			y += 0.05f;

			total = exe + data + bss + sdata + sbss + mem + other;
			_DrawGlobalRasters("Total", total /* value */ , y /* ypos */ , total * 24 /* avgval */ , 0xF0800000 /* color */ );
			y += 0.05f;
		}

#endif
#ifdef PSX2_TARGET
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			float	exe, mem, stack, fheap;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			MEMpro_sst_CurrentState = mallinfo();
			y = 0.75f;

			exe = fLongToFloat(GetHeapSize()) / (32 * 1024 * 1024);
			_DrawGlobalRasters("code", exe /* value */ , y /* ypos */ , exe * 32 /* avgval */ , 0xF0000080 /* color */ );
			y += 0.05f;

			mem = fLongToFloat(MEM_gst_MemoryInfo.ul_RealSize) / (32 * 1024 * 1024);
			_DrawGlobalRasters("MEMdyn", mem /* value */ , y /* ypos */ , mem * 32 /* avgval */ , 0xF0008000 /* color */ );
			y += 0.05f;

			fheap = fLongToFloat(MEMpro_sst_CurrentState.arena - MEM_gst_MemoryInfo.ul_RealSize) / (32 * 1024 * 1024);
			_DrawGlobalRasters("heap", fheap /* value */ , y /* ypos */ , fheap * 32 /* avgval */ , 0xF0008080);
			y += 0.05f;

			stack = fLongToFloat(GetStackSize()) / (32 * 1024 * 1024);
			_DrawGlobalRasters("stck", stack /* value */ , y /* ypos */ , stack * 32 /* avgval */ , 0xF0800000 /* color */ );
			y += 0.05f;

			f = exe + mem + fheap + stack;
			_DrawGlobalRasters("Total", f /* value */ , y /* ypos */ , f * 32 /* avgval */ , 0xF0800080 /* color */ );
			y += 0.05f;
		}

#endif
		break;

	case 3:
#ifdef MEMpro_M_ActiveRasters
		MEMpro_PrintMemRaster();
#endif
		rq = 0;
		break;
		
	case 4:
		MEM_PrintHistory();
		rq = 0;
		break;
		
	default: break;
	}

	MEM_gi_RasterState = rq;
}

#endif

#endif // MEM_OPT