/*$T VERsion_Update23.cpp GC! 1.081 05/05/00 10:29:56 */


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
    Aim:    Update geometric objet Adding a null ulStripFlag inside
 =======================================================================================================================
 */
void VERsion_UpdateOneFileVersion32(char *_pc_Buf, ULONG _ul_Len, char *_pz_Path, char *_pz_File)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	char	*pc_dstBuff, *b;
	char	*psz_ext;
	LONG	l_Type;
	/*~~~~~~~~~~~~~~~~~~~~*/

	/* Find extension of file */
	psz_ext = strrchr(_pz_File, '.');

	if(psz_ext && !strcmp(psz_ext, EDI_Csz_ExtGraphicObject))
	{
		l_Type = *(LONG *) _pc_Buf;
		if(l_Type != 1) return;

		/* allocate a buffer with the new size */
		b = pc_dstBuff = (char *) L_malloc(_ul_Len + sizeof(ULONG));

		/* copy the old file in the new buffer */
		L_memcpy(b, _pc_Buf, _ul_Len);
		b += _ul_Len;

		/* add the flag (=0) */
		L_memset(b, 0, sizeof(ULONG));

		/* update the file size */
		_ul_Len += sizeof(ULONG);

		/* Finally, we replace the file with the destination buffer */
		BIG_UpdateFileFromBuffer(_pz_Path, _pz_File, pc_dstBuff, _ul_Len);

		/* free the buffer */
		L_free(pc_dstBuff);
	}
}

#endif /* ACTIVE_EDITORS */
