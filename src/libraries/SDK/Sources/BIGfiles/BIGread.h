/*$T BIGread.h GC! 1.097 05/01/02 14:34:51 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once

#include "BASe/BAStypes.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGio.h"

#if defined( __cplusplus ) && !defined( JADEFUSION )
extern "C"
{
#endif

	/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef ACTIVE_EDITORS
	extern void *BIG_gp_GlobalSaveBuffer;
#endif

	/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

	ULONG BIG_ul_GetLengthFile( ULONG );
	void  BIG_Read( ULONG, void *, ULONG );

	/**
	 * A replacement for BIG_ul_ReadFile with bound checking.
	 * It uses the index, rather than an explicit position into the file.
	 *
	 * @param index			Index of the file.
	 * @param dstBuffer		Buffer the file is being copied into.
	 * @param bufferSize	Size of the destination buffer.
	 * @return				0 on failure, otherwise the size of the read.
	 */
	size_t BIG_ReadFileToDst( BIG_INDEX index, void *dstBuffer, size_t bufferSize );

	/**
	 * A replacement for BIG_pc_ReadFileTmp, which takes the index rather than pos.
	 *
	 * @param index		Index of the file.
	 * @param size		Returned size.
	 * @return			Pointer to the temporary buffer.
	 */
	void *BIG_ReadFileToTmp( BIG_INDEX index, ULONG *size );

	/**
	 * A replacement for BIG_pc_ReadFileTmpMustFree, which takes the index rather than pos.
	 *
	 * @param index		Index of the file.
	 * @param size		Returned size.
	 * @return			Pointer to a newly allocated buffer.
	 */
	void *BIG_ReadFile( BIG_INDEX index, ULONG *size );

	char *BIG_pc_ReadFileTmp( ULONG, ULONG * );
	void *BIG_p_RequestBuffer( int );
#ifdef ACTIVE_EDITORS
	void *BIG_p_RequestSaveBuffer( int );
#endif
	void BIG_FreeGlobalBuffer( void );

#if defined( __cplusplus ) && !defined( JADEFUSION )
}
#endif

ULONG BIG_ul_EditorGetSizeOfFile( ULONG ul_FileKey );
