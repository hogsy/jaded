/*$T LOA.c GC! 1.098 10/26/00 12:08:52 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "LOAdefs.h"
#include "BASe/MEMory/MEM.h"

#ifdef PSX2_TARGET	/* avoid C++ error */
#include "LOA.h"
#endif

extern void LOA_UnloadExFileKey(void);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_InitModule(void)
{
	LOA_BeforeLoading();
	LOA_InitLoadRaster();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_CloseModule(void)
{
	LOA_AfterLoading();

#ifdef ACTIVE_EDITORS

	/* Destroy references array */
	if(LOA_gi_InitRef)
	{
		/*~~*/
		int i;
		/*~~*/

		for(i = 0; i < LOA_gst_RefArray.num; i++)
		{
			BAS_bfree((BAS_tdst_barray *) LOA_gst_RefArray.base[i].ul_Val);
			L_free((void *) LOA_gst_RefArray.base[i].ul_Val);
		}

		BAS_bfree(&LOA_gst_RefArray);
	}

	LOA_UnloadExFileKey();
#endif
}

