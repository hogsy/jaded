/*$T PROPS2.c GC! 1.081 06/30/00 18:28:44 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    special rasters, using PS2 perf counter
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#define __PROPS2_C__
#include "PROPS2.h"
#include "BASe/BASsys.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/CLIbrary/CLIstr.h"

#ifdef PROPS2_M_ActiveRasters

#ifdef __cplusplus
	#define EXTERN extern "C"
#else
	#define EXTERN extern
#endif


EXTERN float __floatdisf(unsigned long long ul);

EXTERN float _f_ulltof(unsigned long long ul) {
	signed long long sl = (signed long long) ul;
	
	if (sl > 0)	
		return __floatdisf(ul);
	else {
		float f = __floatdisf((ul>>1)|(ul&1));
		return f+f;
	}
}

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "BASe/BAStypes.h"
#include <libpc.h>

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C" {
#endif
/*$4
 ***********************************************************************************************************************
    private macros
 ***********************************************************************************************************************
 */

#define M_RasterStackDepth	50

#define M_DisplayOnlyUser 
#ifndef M_DisplayOnlyUser
#define M_PC_Config (int) \
		( \
			SCE_PC_EXL1 | SCE_PC_K1 | SCE_PC_S1 | SCE_PC_U1 | SCE_PC_U0 | SCE_PC_CTE | SCE_PC0_CPU_CYCLE | \
				SCE_PC1_CPU_CYCLE \
		)
#else
#define M_PC_Config (int) \
		(SCE_PC_EXL1 | SCE_PC_K1 | SCE_PC_S1 | SCE_PC_U1 | SCE_PC_U0  | SCE_PC0_CPU_CYCLE | SCE_PC_CTE | SCE_PC1_NO_EVENT)
#endif
#define M_CPUFrequency	0.2949120f	/* on kit 294.912MHz, on PS2 150MHz */
#define M_Correction	0			/* 70 /* cost of a start & stop calls in clock unit */

/*$4
 ***********************************************************************************************************************
    private types
 ***********************************************************************************************************************
 */


typedef struct	PROPS2_tdst_Stack_
{
	u_long64	ul_pc0;
	u_long64	ul_pc1;
} PROPS2_tdst_Stack;

typedef enum						PROPS2_tden_DisplayCommand_
{
	e_DisplayRasters,
	e_DisplaySortedByMax,
	e_DisplaySortedByMin,
	e_DisplaySortedByAve,
	e_DisplaySortedByVal,
	e_DisplaySortedByNb
} PROPS2_tden_DisplayCommand;

typedef enum						PROPS2_tden_SortCommand_
{
	e_Init,
	e_pc0Max,
	e_pc0Min,
	e_pc0Val,
	e_pc0Ave,
	e_pc1Max,
	e_pc1Min,
	e_pc1Val,
	e_pc1Ave,
	e_Nb
} PROPS2_tden_SortCommand;

/*$4
 ***********************************************************************************************************************
    private variables
 ***********************************************************************************************************************
 */

/* static */
static PROPS2_tdst_Stack			PROPS2_sa_RasterStack[M_RasterStackDepth];
static PROPS2_tdst_Stack			*PROPS2_spst_StackPointer;
static BOOL							PROPS2_sb_StackOverflow;
static unsigned int					PROPS2_sui_StackOverflowDepth;
static char							PROPS2_sasz_Prompt[2 * M_RasterStackDepth + 1] = "";
static PROPS2_tden_DisplayCommand	PROPS2_sen_DisplayCommand;
static PROPS2_tden_SortCommand		PROPS2_sen_LastSort;

/*$4
 ***********************************************************************************************************************
    private proptotypes
 ***********************************************************************************************************************
 */

static void							s_SortChain(PROPS2_tden_SortCommand _e_type);
static PROPS2_tdst_Raster			*s_RecursiveSearch
									(
										PROPS2_tdst_Raster *,
										PROPS2_tdst_Raster *,
										unsigned int _ui_length,
										u_long64 _ul_criterion
									);
static void							s_PrintNext(PROPS2_tdst_Raster *);
static void							s_PrintCalled(PROPS2_tdst_Raster *);
static void							s_Format(u_long64 _ul_data);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PROPS2_MakeLink
(
	PROPS2_tdst_Raster	*_pst_raster,
	PROPS2_tdst_Raster	*_pst_next,
	PROPS2_tdst_Raster	*_pst_called
)
{
	if(_pst_next != &PROPS2_gst_none)
		_pst_raster->pst_NextRaster = _pst_next;
	else
		_pst_raster->pst_NextRaster = NULL;

	if(_pst_called != &PROPS2_gst_none)
		_pst_raster->pst_NextCalled = _pst_called;
	else
		_pst_raster->pst_NextCalled = NULL;
}

/*$4
 ***********************************************************************************************************************
    public functions
 ***********************************************************************************************************************
 */

void PROPS2_PrintChain(PROPS2_tdst_Raster *_pRaster)
{
	printf("-- %s : ", _pRaster->asz_Name);
	s_Format(_pRaster->st_pc0.ul_Value);
#ifndef M_DisplayOnlyUser
	printf(" + ");
	s_Format(_pRaster->st_pc1.ul_Value);
#endif
	printf(" [");
	s_Format(_pRaster->st_pc0.ul_Min);
	printf("/");
	s_Format(_pRaster->st_pc0.ul_Ave);
	printf("/");
	s_Format(_pRaster->st_pc0.ul_Max);
#ifndef M_DisplayOnlyUser
	printf("]+[");
	s_Format(_pRaster->st_pc1.ul_Min);
	printf("/");
	s_Format(_pRaster->st_pc1.ul_Ave);
	printf("/");
	s_Format(_pRaster->st_pc1.ul_Max);
#endif
	printf("] (%d) -", _pRaster->ul_Nb);
	s_Format(_pRaster->st_pc0.ul_Ave * _pRaster->ul_Nb);
	printf("-\n");

	if(_pRaster->pst_NextCell) PROPS2_PrintChain(_pRaster->pst_NextCell);
}

void PROPS2_Print(PROPS2_tdst_Raster *_pRaster)
{
	printf("-- %s : ", _pRaster->asz_Name);
	s_Format(_pRaster->st_pc0.ul_Value);
#ifndef M_DisplayOnlyUser
	printf(" + ");
	s_Format(_pRaster->st_pc1.ul_Value);
#endif
	printf(" [");
	s_Format(_pRaster->st_pc0.ul_Min);
	printf("/");
	s_Format(_pRaster->st_pc0.ul_Ave);
	printf("/");
	s_Format(_pRaster->st_pc0.ul_Max);
#ifndef M_DisplayOnlyUser
	printf("]+[");
	s_Format(_pRaster->st_pc1.ul_Min);
	printf("/");
	s_Format(_pRaster->st_pc1.ul_Ave);
	printf("/");
	s_Format(_pRaster->st_pc1.ul_Max);
#endif
	printf("] (%d) -", _pRaster->ul_Nb);
	s_Format(_pRaster->st_pc0.ul_Ave * _pRaster->ul_Nb);
	printf("-\n");

}


void PROPS2_InitRaster(PROPS2_tdst_Raster *_pst_raster, char *_asz_name)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static PROPS2_tdst_Raster	*pst_last = &PROPS2_gst_FirstRaster;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(_pst_raster, 0, sizeof(PROPS2_tdst_Raster));
	_pst_raster->st_pc0.ul_Min = (u_long64) - 1;	/* valeur bidon */
	_pst_raster->st_pc1.ul_Min = (u_long64) - 1;	/* valeur bidon */
	if(_asz_name) L_memcpy(_pst_raster->asz_Name, _asz_name, PROPS2_M_RasterNameSize);
	_pst_raster->asz_Name[PROPS2_M_RasterNameSize - 1] = '\0';

	/* create a chain */
	pst_last->pst_NextCell = _pst_raster;
	pst_last = _pst_raster;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PROPS2_Init(int _i_ForceInit)
{
    static int si_Initialized = 0;
    
    if( _i_ForceInit || ! si_Initialized)
    {
    	L_memset(PROPS2_sa_RasterStack, 0, sizeof(PROPS2_tdst_Stack) * M_RasterStackDepth);
    	PROPS2_spst_StackPointer = PROPS2_sa_RasterStack;
    	PROPS2_sb_StackOverflow = FALSE;
    	PROPS2_sui_StackOverflowDepth = 0;
    	PROPS2_sen_LastSort = e_Init;

    /*$off*/
    /* raster init */
#define __M_Macro__(a, b, c)	PROPS2_InitRaster(&PROPS2_gst_##a, #a);
#include "TIMer/PROfiler/PROPS2_def.h"
#undef __M_Macro__

    	/* 1st raster = special init */
    	PROPS2_gst_FirstRaster.st_pc0.ul_Min = 0;
    	PROPS2_gst_FirstRaster.st_pc1.ul_Min = 0;

    /* rasters link */
#define __M_Macro__(a, b, c)	PROPS2_MakeLink(&PROPS2_gst_##a, &PROPS2_gst_##b, &PROPS2_gst_##c);
#include "TIMer/PROfiler/PROPS2_def.h"
#undef __M_Macro__
    /*$on*/
    si_Initialized = 1;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern void asm_break(void);
void _PROPS2_StartRaster(PROPS2_tdst_Raster *_pst_Raster)
{
	/*~~~~~~~~~*/
	int pc0, pc1;
	/*~~~~~~~~~*/
	
	pc0 = scePcGetCounter0();
	pc1 = scePcGetCounter1();

	/* if overflow, do nothing */
	if(PROPS2_sui_StackOverflowDepth)
	{
		PROPS2_sui_StackOverflowDepth++;
		_pst_Raster->l_StartCount = -1;
	}

	if(_pst_Raster->l_StartCount >= 0)
	{
		if(PROPS2_spst_StackPointer == &PROPS2_sa_RasterStack[M_RasterStackDepth - 1])
		{
			/* record the overflow */
			ERR_X_Error(0, "** PS2 rasters stack overflow", NULL);
			PROPS2_sui_StackOverflowDepth++;
			_pst_Raster->l_StartCount = -1;
		}
		else
		{
			/* save the elapsed time in the caller */
			PROPS2_spst_StackPointer->ul_pc0 += (u_long64)pc0;
			PROPS2_spst_StackPointer->ul_pc1 += (u_long64)pc1;

			/* start the new raster */
			(++PROPS2_spst_StackPointer)->ul_pc0 = 0;
			PROPS2_spst_StackPointer->ul_pc1 = 0;

			_pst_Raster->ul_Nb++;
			_pst_Raster->l_StartCount++;
			pc0 = pc1 = 0;
		}
	}
    scePcStart(M_PC_Config, pc0, pc1);
}

int gpc0, gpc1;
void _PROPS2_StopAll(void)
{
	gpc0 = scePcGetCounter0();
	gpc1 = scePcGetCounter1();
}

void PROPS2_StartAll(void)
{
    scePcStart(M_PC_Config, gpc0, gpc1);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void _PROPS2_StopRaster(PROPS2_tdst_Raster *_pst_Raster)
{
	/*~~~~~~~~~*/
	int pc0, pc1;
	/*~~~~~~~~~*/

	pc0 = scePcGetCounter0();
	pc1 = scePcGetCounter1();

	if(PROPS2_sui_StackOverflowDepth) PROPS2_sui_StackOverflowDepth--;

	if(_pst_Raster->l_StartCount-- > 0)
	{
		/* the result of current raster */
		_pst_Raster->st_pc0.ul_Value = (u_long64)pc0 + PROPS2_spst_StackPointer->ul_pc0 - M_Correction;
		_pst_Raster->st_pc1.ul_Value = (u_long64)pc1 + PROPS2_spst_StackPointer->ul_pc1 - M_Correction;

		/* process statistics */
		_pst_Raster->st_pc0.ul_Max = max(_pst_Raster->st_pc0.ul_Max, _pst_Raster->st_pc0.ul_Value);
		_pst_Raster->st_pc0.ul_Min = min(_pst_Raster->st_pc0.ul_Min, _pst_Raster->st_pc0.ul_Value);
		_pst_Raster->st_pc0.ul_Ave = (_pst_Raster->st_pc0.ul_Ave * (_pst_Raster->ul_Nb - 1) + _pst_Raster->st_pc0.ul_Value) / _pst_Raster->ul_Nb;
		_pst_Raster->st_pc1.ul_Max = max(_pst_Raster->st_pc1.ul_Max, _pst_Raster->st_pc1.ul_Value);
		_pst_Raster->st_pc1.ul_Min = min(_pst_Raster->st_pc1.ul_Min, _pst_Raster->st_pc1.ul_Value);
		_pst_Raster->st_pc1.ul_Ave = (_pst_Raster->st_pc1.ul_Ave * (_pst_Raster->ul_Nb - 1) + _pst_Raster->st_pc1.ul_Value) / _pst_Raster->ul_Nb;

		/* 1-add the duration to the caller one / 0-no add*/
#if 0		
		(--PROPS2_spst_StackPointer)->ul_pc0 += _pst_Raster->st_pc0.ul_Value;
		PROPS2_spst_StackPointer->ul_pc1 += _pst_Raster->st_pc1.ul_Value;
#else
        PROPS2_spst_StackPointer--;
#endif    
		/* if first level, we must not restart counter */
		if(PROPS2_spst_StackPointer == PROPS2_sa_RasterStack) return;
		pc0 = pc1 = 0;
	}

	scePcStart(M_PC_Config, pc0, pc1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PROPS2_PrintRaster(void)
{
	switch(PROPS2_sen_DisplayCommand)
	{
	case e_DisplaySortedByMax:
		printf("-- ----------------------------------------\n");
		printf("-- Rasters display: sorted by maximum value\n");
		printf("-- ----------------------------------------\n");
		PROPS2_PrintChain(&PROPS2_gst_FirstRaster);
		break;
	case e_DisplaySortedByMin:
		printf("-- ----------------------------------------\n");
		printf("-- Rasters display: sorted by minimum value\n");
		printf("-- ----------------------------------------\n");
		PROPS2_PrintChain(&PROPS2_gst_FirstRaster);
		break;
	case e_DisplaySortedByAve:
		printf("-- ----------------------------------------\n");
		printf("-- Rasters display: sorted by average value\n");
		printf("-- ----------------------------------------\n");
		PROPS2_PrintChain(&PROPS2_gst_FirstRaster);
		break;
	case e_DisplaySortedByVal:
		printf("-- ----------------------------------------\n");
		printf("-- Rasters display: sorted by value\n");
		printf("-- ----------------------------------------\n");
		PROPS2_PrintChain(&PROPS2_gst_FirstRaster);
		break;
	case e_DisplaySortedByNb:
		printf("-- ----------------------------------------\n");
		printf("-- Rasters display: sorted by activation count\n");
		printf("-- ----------------------------------------\n");
		PROPS2_PrintChain(&PROPS2_gst_FirstRaster);
		break;
	default:
	case e_DisplayRasters:
		printf("-- ----------------------------------------\n");
		printf("-- Rasters display: hierarchy\n");
		printf("-- ----------------------------------------\n");
		s_PrintNext(&PROPS2_gst_FirstRaster);
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PROPS2_ChangeDisplay(void)
{
#ifdef __GNUC__
    PROPS2_sen_DisplayCommand = (PROPS2_tden_DisplayCommand)(PROPS2_sen_DisplayCommand+1);
	switch(PROPS2_sen_DisplayCommand)
#else
	switch(++PROPS2_sen_DisplayCommand)
#endif
	{
	case e_DisplaySortedByMax:
		printf("-- ----------------------------------------\n");
		printf("-- Rasters display: sorted by maximum value\n");
		s_SortChain(e_pc0Max);
		break;
	case e_DisplaySortedByMin:
		printf("-- ----------------------------------------\n");
		printf("-- Rasters display: sorted by minimum value\n");
		s_SortChain(e_pc0Min);
		break;
	case e_DisplaySortedByAve:
		printf("-- ----------------------------------------\n");
		printf("-- Rasters display: sorted by average value\n");
		s_SortChain(e_pc0Ave);
		break;
	case e_DisplaySortedByVal:
		printf("-- ----------------------------------------\n");
		printf("-- Rasters display: sorted by value\n");
		s_SortChain(e_pc0Val);
		break;
	case e_DisplaySortedByNb:
		printf("-- ----------------------------------------\n");
		printf("-- Rasters display: sorted by activation count\n");
		s_SortChain(e_Nb);
		break;
	default: PROPS2_sen_DisplayCommand = e_DisplayRasters;
	case e_DisplayRasters:
		printf("-- ----------------------------------------\n");
		printf("-- Rasters display: hierarchy\n");
		break;
	}

	printf("-- hierarchy >> max >> min >> ave >> val >> nb\n");
	printf("-- ----------------------------------------\n");
}

/*$4
 ***********************************************************************************************************************
    private functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void s_Format(u_long64 _ul_data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float	f;
	char	*unit[] = { "ns", "us", "ms", "s" };
	int		i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i = 0;

	f = _ul_data / M_CPUFrequency;

	while(f >= 1000.0f)
	{
		f /= 1000.0f;
		if(++i == 3) break;
	}

	printf("%3.2f%s", f, unit[i]);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void s_PrintCalled(PROPS2_tdst_Raster *_pRaster)
{
	/*~~~~~~~~~~~~~~*/
	static int	i = 0;
	/*~~~~~~~~~~~~~~*/

	PROPS2_sasz_Prompt[i++] = ' ';
	PROPS2_sasz_Prompt[i++] = '>';
	PROPS2_sasz_Prompt[i] = '\0';

	if(_pRaster->ul_Nb)
	{
		printf("--%s %s : ", PROPS2_sasz_Prompt, _pRaster->asz_Name);
		s_Format(_pRaster->st_pc0.ul_Value);
#ifndef M_DisplayOnlyUser
		printf(" + ");
		s_Format(_pRaster->st_pc1.ul_Value);
#endif
		printf(" [");
		s_Format(_pRaster->st_pc0.ul_Min);
		printf("/");
		s_Format(_pRaster->st_pc0.ul_Ave);
		printf("/");
		s_Format(_pRaster->st_pc0.ul_Max);
#ifndef M_DisplayOnlyUser
		printf("]+[");
		s_Format(_pRaster->st_pc1.ul_Min);
		printf("/");
		s_Format(_pRaster->st_pc1.ul_Ave);
		printf("/");
		s_Format(_pRaster->st_pc1.ul_Max);
#endif
		printf("] (%d)\n", _pRaster->ul_Nb);
	}

	if(_pRaster->pst_NextCalled) s_PrintCalled(_pRaster->pst_NextCalled);
	if(_pRaster->pst_NextRaster) s_PrintNext(_pRaster->pst_NextRaster);

	PROPS2_sasz_Prompt[i--] = '\0';
	PROPS2_sasz_Prompt[i--] = '\0';
	PROPS2_sasz_Prompt[i] = '\0';
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void s_PrintNext(PROPS2_tdst_Raster *_pRaster)
{
	if(_pRaster->ul_Nb)
	{
		printf("--%s %s : ", PROPS2_sasz_Prompt, _pRaster->asz_Name);
		s_Format(_pRaster->st_pc0.ul_Value);
#ifndef M_DisplayOnlyUser
		printf(" + ");
		s_Format(_pRaster->st_pc1.ul_Value);
#endif
		printf(" [");
		s_Format(_pRaster->st_pc0.ul_Min);
		printf("/");
		s_Format(_pRaster->st_pc0.ul_Ave);
		printf("/");
		s_Format(_pRaster->st_pc0.ul_Max);
#ifndef M_DisplayOnlyUser
		printf("]+[");
		s_Format(_pRaster->st_pc1.ul_Min);
		printf("/");
		s_Format(_pRaster->st_pc1.ul_Ave);
		printf("/");
		s_Format(_pRaster->st_pc1.ul_Max);
#endif
		printf("] (%d)\n", _pRaster->ul_Nb);
	}

	if(_pRaster->pst_NextCalled) s_PrintCalled(_pRaster->pst_NextCalled);
	if(_pRaster->pst_NextRaster) s_PrintNext(_pRaster->pst_NextRaster);
}

/*
 =======================================================================================================================
    search the preceding element for the value:: _pst_start->val <= value < _pst_end->val
 =======================================================================================================================
 */
static PROPS2_tdst_Raster *s_RecursiveSearch
(
	PROPS2_tdst_Raster	*_pst_start,
	PROPS2_tdst_Raster	*_pst_end,
	unsigned int		_ui_length,
	u_long64		_ul_criterion
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	PROPS2_tdst_Raster	*pst_middle;
	unsigned int		i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ui_length <= 1) return _pst_start;

	pst_middle = _pst_start;
	for(i = 0; i < _ui_length / 2; i++) pst_middle = pst_middle->pst_NextCell;

	if(pst_middle->ul_Criterion <= _ul_criterion)
		return s_RecursiveSearch(pst_middle, _pst_end, (_ui_length + 1) / 2, _ul_criterion);
	else
		return s_RecursiveSearch(_pst_start, pst_middle, _ui_length / 2, _ul_criterion);
}

/*
 =======================================================================================================================
    sort the chain among the criterion
 =======================================================================================================================
 */
static void s_SortChain(PROPS2_tden_SortCommand _e_type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	PROPS2_tdst_Raster	*pst_first;
	PROPS2_tdst_Raster	*pst_last;
	PROPS2_tdst_Raster	*pst_current;
	PROPS2_tdst_Raster	*pst_middle;
	PROPS2_tdst_Raster	*pst_preceding;
	unsigned int		ui_length;
	unsigned int		i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(PROPS2_sen_LastSort == _e_type) return;
	switch(_e_type)
	{
	case e_pc0Max:
	case e_pc0Min:
	case e_pc0Val:
	case e_pc0Ave:
	case e_pc1Max:
	case e_pc1Min:
	case e_pc1Val:
	case e_pc1Ave:
	case e_Nb:
		PROPS2_sen_LastSort = _e_type;
		break;
	default:
		PROPS2_sen_LastSort = e_Nb;
		break;
	}

	pst_current = &PROPS2_gst_FirstRaster;

	switch(PROPS2_sen_LastSort)
	{
	case e_pc0Max:
		while(pst_current)
		{
			pst_current->ul_Criterion = pst_current->st_pc0.ul_Max;
			pst_current = pst_current->pst_NextCell;
		}
		break;
	case e_pc0Min:
		while(pst_current)
		{
			pst_current->ul_Criterion = pst_current->st_pc0.ul_Min;
			pst_current = pst_current->pst_NextCell;
		}
		break;
	case e_pc0Val:
		while(pst_current)
		{
			pst_current->ul_Criterion = pst_current->st_pc0.ul_Value;
			pst_current = pst_current->pst_NextCell;
		}
		break;
	case e_pc0Ave:
		while(pst_current)
		{
			pst_current->ul_Criterion = pst_current->st_pc0.ul_Ave;
			pst_current = pst_current->pst_NextCell;
		}
		break;
	case e_pc1Max:
		while(pst_current)
		{
			pst_current->ul_Criterion = pst_current->st_pc1.ul_Max;
			pst_current = pst_current->pst_NextCell;
		}
		break;
	case e_pc1Min:
		while(pst_current)
		{
			pst_current->ul_Criterion = pst_current->st_pc1.ul_Min;
			pst_current = pst_current->pst_NextCell;
		}
		break;
	case e_pc1Val:
		while(pst_current)
		{
			pst_current->ul_Criterion = pst_current->st_pc1.ul_Value;
			pst_current = pst_current->pst_NextCell;
		}
		break;
	case e_pc1Ave:
		while(pst_current)
		{
			pst_current->ul_Criterion = pst_current->st_pc1.ul_Ave;
			pst_current = pst_current->pst_NextCell;
		}
		break;
	case e_Nb:
		while(pst_current)
		{
			pst_current->ul_Criterion = pst_current->ul_Nb;
			pst_current = pst_current->pst_NextCell;
		}
		break;
	default:
	    break;
	}

	ui_length = 0;

	pst_first = pst_last = &PROPS2_gst_FirstRaster;
	pst_current = pst_first->pst_NextCell;
	pst_last->pst_NextCell = NULL;

	while(pst_current)
	{
		if(pst_current->ul_Criterion >= pst_last->ul_Criterion)
		{
			/* put it at the end */
			pst_last->pst_NextCell = pst_current;
			pst_current = pst_current->pst_NextCell;
			pst_last = pst_last->pst_NextCell;
			pst_last->pst_NextCell = NULL;
		}
		else if(pst_current->ul_Criterion == pst_first->ul_Criterion)
		{
			/* put it just after the first */
			pst_middle = pst_current;
			pst_current = pst_current->pst_NextCell;
			pst_middle->pst_NextCell = pst_first->pst_NextCell;
			pst_first->pst_NextCell = pst_middle;
		}
		else if(pst_current->ul_Criterion < pst_first->ul_Criterion)
		{
			/* put it at the begining */
			pst_preceding = pst_current;
			pst_current = pst_current->pst_NextCell;
			pst_preceding->pst_NextCell = pst_first;
			pst_first = pst_preceding;
		}
		else
		{
			/* put it between 1st & last */
			pst_middle = pst_first;
			for(i = 0; i < ui_length / 2; i++) pst_middle = pst_middle->pst_NextCell;
			if(pst_middle->ul_Criterion <= pst_current->ul_Criterion)
				pst_preceding = s_RecursiveSearch(pst_middle, pst_last, (ui_length + 1) / 2, pst_current->ul_Criterion);
			else
				pst_preceding = s_RecursiveSearch(pst_first, pst_middle, ui_length / 2, pst_current->ul_Criterion);

			pst_middle = pst_current;
			pst_current = pst_current->pst_NextCell;
			pst_middle->pst_NextCell = pst_preceding->pst_NextCell;
			pst_preceding->pst_NextCell = pst_middle;
		}

		ui_length++;
	}
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* PSX2_TARGET && RASTERS_ON */
