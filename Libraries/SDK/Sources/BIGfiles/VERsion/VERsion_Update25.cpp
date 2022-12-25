/*$T VERsion_Update25.cpp GC! 1.081 05/25/00 17:05:23 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGmdfy_file.h"

/*
 =======================================================================================================================
    Aim:    Update actions. Adding a null Shape.
 =======================================================================================================================
 */
void VERsion_UpdateOneFileVersion25(char *_pc_Buf, ULONG _ul_Len, char *_pz_Path, char *_pz_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	*pc_dstBuff, *b;
	char	*psz_ext;
	char	c_NbOfActionsItems, i;
	ULONG	ul_Rest;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Find extension of file */
	psz_ext = strrchr(_pz_File, '.');

	if(psz_ext && !strcmp(psz_ext, EDI_Csz_ExtAction))
	{
		c_NbOfActionsItems = *(char *) _pc_Buf;
		ul_Rest = _ul_Len;

		/* allocate a buffer with the new size */
		b = pc_dstBuff = (char *) L_malloc(_ul_Len + c_NbOfActionsItems * sizeof(ULONG));

		L_memcpy(b, _pc_Buf, 4);
		b += 4;
		_pc_Buf += 4;
		ul_Rest -= 4;

		/* Loop thru all the Action Items */
		for(i = 0; i < c_NbOfActionsItems; i++)
		{
			/* Pass the pst_ListTracks and pst_Transitions */
			L_memcpy(b, _pc_Buf, 2 * sizeof(ULONG));
			b += 2 * sizeof(ULONG);
			_pc_Buf += 2 * sizeof(ULONG);
			ul_Rest -= 2 * sizeof(ULONG);

			/* Add the pointer (NULL) */
			*(ULONG *) b = BIG_C_InvalidIndex;
			b += sizeof(ULONG);

			L_memcpy(b, _pc_Buf, 16 - 2 * sizeof(ULONG));
			b += 16 - 2 * sizeof(ULONG);
			_pc_Buf += 16 - 2 * sizeof(ULONG);
			ul_Rest -= 16 - 2 * sizeof(ULONG);
		}

		L_memcpy(b, _pc_Buf, ul_Rest);

		/* update the file size */
		_ul_Len += c_NbOfActionsItems * sizeof(ULONG);

		/* Finally, we replace the file with the destination buffer */
		BIG_UpdateFileFromBuffer(_pz_Path, _pz_File, pc_dstBuff, _ul_Len);

		/* free the buffer */
		L_free(pc_dstBuff);
	}
}

#endif /* ACTIVE_EDITORS */
