/*$T EDIapp_help.cpp GC! 1.100 06/15/01 10:29:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmerge.h"
#include "EDIapp.h"
#include "EDIpaths.h"
#include "EDImainframe.h"
#include "Res/Res.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDI_cl_App::SearchHelp(char *_psz_Name, BOOL)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	char	*sz_Path;
	char	asz_Name[1048];
	/*~~~~~~~~~~~~~~~~~~~*/

	sz_Path = M_MF()->mst_ExternIni.asz_PathHelp;
	if(!(*sz_Path)) L_strcpy(sz_Path, "Help/");
	if ( ( sz_Path[ strlen( sz_Path ) - 1 ] != '/' ) && ( sz_Path[ strlen( sz_Path ) - 1 ] != '\\' ) )
		L_strcat( sz_Path, "/" );

	sprintf(asz_Name, "%s%s", sz_Path, _psz_Name);
	strcat(asz_Name, ".htm");
	if(!ShellExecute(NULL, "open", asz_Name, NULL, NULL, SW_SHOWMAXIMIZED))
	{
		M_MF()->MessageBox(asz_Name, "Unable to find help file", MB_OK);
	}
	return TRUE;
}

#endif /* ACTIVE_EDITORS */
