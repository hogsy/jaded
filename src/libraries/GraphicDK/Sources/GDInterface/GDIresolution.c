/*$T GDIresolution.c GC 1.129 09/21/01 09:47:19 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/MEMory/MEM.h"
#include "GDInterface/GDIresolution.h"

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

LONG				GDI_Resolution_Number;
GDI_tdst_Resolution *GDI_gdst_Resolution;
char				GDI_gsz_Resolution_Desc[32];
extern float		TIM_gf_SynchroFrequency;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GDI_Resolution_Init(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DEVMODE				st_DevMode;
	GDI_tdst_Resolution *pst_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GDI_Resolution_Number = 0;
	while(EnumDisplaySettings(NULL, GDI_Resolution_Number, &st_DevMode)) GDI_Resolution_Number++;

	GDI_gdst_Resolution = (GDI_tdst_Resolution*)MEM_p_Alloc(GDI_Resolution_Number * sizeof(GDI_tdst_Resolution));

	GDI_Resolution_Number = 0;
	pst_Res = GDI_gdst_Resolution;
	while(EnumDisplaySettings(NULL, GDI_Resolution_Number, &st_DevMode))
	{
		pst_Res->w = (LONG) st_DevMode.dmPelsWidth;
		pst_Res->h = (LONG) st_DevMode.dmPelsHeight;
		pst_Res->bpp = (LONG) st_DevMode.dmBitsPerPel;
		pst_Res->freq = (LONG) st_DevMode.dmDisplayFrequency;

		GDI_Resolution_Number++;
		pst_Res++;
	}

	TIM_gf_SynchroFrequency = GDI_f_ResolutionGetFreq();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GDI_Resolution_Close(void)
{
	MEM_Free(GDI_gdst_Resolution);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GDI_Resolution_Change(int _i_Res)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG				i;
	DEVMODE				devMode;
	GDI_tdst_Resolution *pst_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_Res < 0) return;
	if(_i_Res >= GDI_Resolution_Number) return;

	pst_Res = &GDI_gdst_Resolution[_i_Res];

	for(i = 0;; i++)
	{
		if(!EnumDisplaySettings(NULL, i, &devMode)) break;
		if
		(
			((LONG) devMode.dmBitsPerPel == pst_Res->bpp)
		&&	((LONG) devMode.dmPelsWidth == pst_Res->w)
		&&	((LONG) devMode.dmPelsHeight == pst_Res->h)
		)
		{
			ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
			return;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *GDI_sz_Resolution_Desc(int _i_Res)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GDI_tdst_Resolution *pst_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_Res == -1)
		strcpy(GDI_gsz_Resolution_Desc, "Current");
	else if((_i_Res < 0) || (_i_Res >= GDI_Resolution_Number))
		strcpy(GDI_gsz_Resolution_Desc, "Unknown");
	else
	{
		pst_Res = &GDI_gdst_Resolution[_i_Res];
		sprintf(GDI_gsz_Resolution_Desc, "%d x %d x %d", pst_Res->w, pst_Res->h, pst_Res->bpp);
	}

	return GDI_gsz_Resolution_Desc;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GDI_i_Resolution_GetIndex(GDI_tdst_Resolution *_pst_Res)
{
	/*~~~~~~*/
	int i_Res;
	/*~~~~~~*/

	for(i_Res = 0; i_Res < GDI_Resolution_Number; i_Res++)
	{
		if
		(
			(_pst_Res->w == GDI_gdst_Resolution[i_Res].w)
		&&	(_pst_Res->h == GDI_gdst_Resolution[i_Res].h)
		&&	(_pst_Res->bpp == GDI_gdst_Resolution[i_Res].bpp)
		&&	(_pst_Res->freq == GDI_gdst_Resolution[i_Res].freq)
		) return i_Res;
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GDI_tdst_Resolution *GDI_pst_Resolution_Get(int _i_Res)
{
	if((_i_Res < 0) || (_i_Res >= GDI_Resolution_Number))
		return NULL;
	else
		return &GDI_gdst_Resolution[_i_Res];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float GDI_f_ResolutionGetFreq(void)
{
	/*~~~~~~~~~*/
	DEVMODE mode;
	/*~~~~~~~~~*/

	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &mode);
	return (float) mode.dmDisplayFrequency;
}
#endif /* ACTIVE_EDITORS */
