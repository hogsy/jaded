/*$T SAVdefs.c GC! 1.098 12/12/00 12:39:26 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGspecial.h"

/*$4
 ***********************************************************************************************************************
    Global variables
 ***********************************************************************************************************************
 */

static int	sgi_ActualSize = 0;
static char sgasz_Path[BIG_C_MaxLenPath];
static char sgasz_File[BIG_C_MaxLenName];

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    This function must be called before making a save operation.

    In:     _psz_Path   Complete path of file (on bigfile) to save.
            _psz_Name   Atomic part of the file name to save.
 =======================================================================================================================
 */
void SAV_Begin(char *_psz_Path, char *_psz_Name)
{
	sgi_ActualSize = 0;
	L_strcpy(sgasz_Path, _psz_Path);
	L_strcpy(sgasz_File, _psz_Name);
}

/*
 =======================================================================================================================
    Aim:    Call that function to save some bytes in a file.

    Note:   The buffer is not saved in the bigfile, but in a temporary buffer. When you have finished, you must called
            SAV_ul_End to really save the file.

    In:     _p_Buffer   Address of buffer that contains informations to save.
            _i_Size     The number of bytes to save.
 =======================================================================================================================
 */
void SAV_Buffer(void *_p_Buffer, int _i_Size)
{
	/*~~~~~~~~~~~~*/
	char	*pc_Buf;
	/*~~~~~~~~~~~~*/

	pc_Buf = (char *) BIG_p_RequestSaveBuffer(sgi_ActualSize + _i_Size);
	pc_Buf += sgi_ActualSize;
	L_memcpy(pc_Buf, _p_Buffer, _i_Size);
	sgi_ActualSize += _i_Size;
}

/*
 =======================================================================================================================
    Aim:    Call that function to really save the file on the bigfile. This is the last function that must be called.

    Out:    Returns the index (in fat of file) of the saved file.
 =======================================================================================================================
 */
BIG_INDEX SAV_ul_End(void)
{
	/*~~~~~~~~~~~~*/
	char	*pc_Buf;
	/*~~~~~~~~~~~~*/

_Try_
	BIG_ul_CreateDir(sgasz_Path);

	switch(BIG_specialmode)
	{
	case BIG_C_SpecialModeLZO:		pc_Buf = BIG_special_LZOsave((char*)BIG_gp_GlobalSaveBuffer, (ULONG*)&sgi_ActualSize); break;
	default:						pc_Buf = (char*)BIG_gp_GlobalSaveBuffer; break;
	}

	BIG_UpdateFileFromBuffer(sgasz_Path, sgasz_File, pc_Buf, sgi_ActualSize);
	sgi_ActualSize = 0;
_Catch_
_End_
	return BIG_ul_SearchFileExt(sgasz_Path, sgasz_File);
}

#endif /* ACTIVE_EDITORS */
