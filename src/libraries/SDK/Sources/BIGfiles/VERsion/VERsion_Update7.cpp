/*$T VERsion_Update7.cpp GC!1.52 11/30/99 17:51:52 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/VERsion/VERsion_Update.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "EDIpaths.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGkey.h"

/*
 ===================================================================================================
    Aim:    Update bigfile from version 5 to Version 6
 ===================================================================================================
 */
void VERsion_UpdateOneFileVersion7(char *_pc_Buf, ULONG _ul_Len, char *_psz_Path, char *_psz_File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    *psz_ext;
	ULONG	i, j, ul_Num, ul_Num1, ul_Buf;
	USHORT	uw;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!L_strncmp(_psz_Path, EDI_Csz_Path_EditData, L_strlen(EDI_Csz_Path_EditData)))
		return;

    psz_ext = strrchr(_psz_File, '.');
    if(psz_ext && (!strcmp(psz_ext, EDI_Csz_ExtNetObject)))
    {
		SAV_Begin(_psz_Path, _psz_File);

		/* Number of lists */
		ul_Num = *(ULONG *) _pc_Buf;
		SAV_Buffer(&ul_Num, 4);
		_pc_Buf += 4;

		for(i = 0; i < ul_Num; i++)
		{
			ul_Buf = *(ULONG *) _pc_Buf;
			SAV_Buffer(&ul_Buf, 4);
			_pc_Buf += 4;

			ul_Num1 = *(ULONG *) _pc_Buf;
			SAV_Buffer(&ul_Num1, 4);
			_pc_Buf += 4;

			for(j = 0; j < ul_Num1; j++)
			{
				uw = *(USHORT *) _pc_Buf;
				SAV_Buffer(&uw, 2);
				_pc_Buf += 2;
				ul_Buf = *(ULONG *) _pc_Buf;
				SAV_Buffer(&ul_Buf, 4);
				_pc_Buf += 4;
				ul_Buf = 0;
				SAV_Buffer(&ul_Buf, 3);	/* Add */
			}
		}

		SAV_ul_End();
    }
}

#endif
