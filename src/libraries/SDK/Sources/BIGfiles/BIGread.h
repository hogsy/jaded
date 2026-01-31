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

	char *BIG_pc_ReadFileTmp( ULONG, ULONG * );
	char *BIG_pc_ReadFileTmpMustFree( ULONG, ULONG * );
	void *BIG_p_RequestBuffer( int );
#ifdef ACTIVE_EDITORS
	void *BIG_p_RequestSaveBuffer( int );
#endif
	void BIG_FreeGlobalBuffer( void );

#if defined( __cplusplus ) && !defined( JADEFUSION )
}
#endif

ULONG BIG_ul_EditorGetSizeOfFile( ULONG ul_FileKey );
