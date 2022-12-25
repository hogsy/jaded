/*$T ANIsave.c GC! 1.081 04/21/00 16:12:37 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS
#include "BASe/ERRors/ERRasser.h"
#include "TABles/TABles.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOading/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "EDIpaths.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG ANI_ul_SaveShape(ANI_tdst_Shape *_pst_Shape, BIG_KEY _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			i;
	BIG_INDEX	ul_Index;
	char		az_Path[BIG_C_MaxLenPath];
	char		c_NbCanal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ul_Key == BIG_C_InvalidIndex)
		ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Shape);
	else
		ul_Index = BIG_ul_SearchKeyToFat(_ul_Key);

	if(ul_Index == BIG_C_InvalidIndex)
	{
		ERR_X_Warning(ul_Index != BIG_C_InvalidIndex, "Cannot find Shape file", NULL);
		return BIG_C_InvalidIndex;
	}

	/* Compute the file name and the path name and open the file */
	BIG_ComputeFullName(BIG_ParentFile(ul_Index), az_Path);
	SAV_Begin(az_Path, BIG_NameFile(ul_Index));

	c_NbCanal = (char) _pst_Shape->uw_LastCanal; 

	SAV_Buffer(&c_NbCanal, 1);
	for(i = 0; i < _pst_Shape->uw_LastCanal; i ++)
	{
		SAV_Buffer(_pst_Shape->auc_AI_Canal + i, 1);
		SAV_Buffer(_pst_Shape->auc_Visu + i, 1);
	}

	return SAV_ul_End();
}

#endif /* ACTIVE_EDITORS */

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
