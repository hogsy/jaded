/*$T BIGread.c GC! 1.097 05/01/02 14:25:05 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGspecial.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "LOAding/LOAdefs.h"
#include "LINKs/LINKmsg.h"

/*$4
 ***********************************************************************************************************************
    Global variables
 ***********************************************************************************************************************
 */

void		*BIG_gp_GlobalBuffer = NULL;	/* Address of temporary global buffer for bigfiles */
int			BIG_gi_GlobalBufferSize = 0;	/* The actual length of that buffer */
extern char *BIG_gp_ReadBuffer;				/* Temps buffer for special read/write mode */
extern int	BIG_gi_ReadMode;				/* 0 normal, 1 write, 2 read buffer */
extern int	BIG_gi_ReadSeek;				/* Current seek */
#ifdef ACTIVE_EDITORS
void		*BIG_gp_GlobalSaveBuffer = NULL;
int			BIG_gi_GlobalSaveBufferSize = 0;
#endif
extern void *LOA_FetchFile(ULONG *_pul_Length);
extern BOOL LOA_gb_SpeedMode;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

BOOL		BIG_gb_CanCache = FALSE;

/* #define BIG_CACHE */
#ifdef BIG_CACHE
#define SIZE_CACHE	1 * 1024 * 1024
char		*BIG_gp_Cache = NULL;
ULONG		BIG_gul_SeekCache = 0;
ULONG		BIG_gul_SeekCur = 0;
ULONG		BIG_gul_Miss = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static ULONG 
SeekSeek(BIGFileHandle h, ULONG pos, ULONG what)
{
	BIG_gul_SeekCur = pos;

	if(what != L_SEEK_SET || !BIG_gb_CanCache) return BIG_fseek(h, pos, what);
	if(!BIG_gp_Cache)
	{
		BIG_gp_Cache = (char *) L_malloc(SIZE_CACHE);
		BIG_fseek(h, pos, what);
		BIG_fread(BIG_gp_Cache, SIZE_CACHE, h);
		BIG_gul_SeekCache = pos;
	}

	if(pos - BIG_gul_SeekCache < SIZE_CACHE) return 0;

	BIG_gul_Miss++;
	BIG_fseek(h, pos, what);
	BIG_fread(BIG_gp_Cache, SIZE_CACHE, h);
	BIG_gul_SeekCache = pos;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static ULONG 
ReadRead(char *p, ULONG size, BIGFileHandle h)
{
	if((!BIG_gp_Cache) || (!BIG_gb_CanCache) || (size > SIZE_CACHE) || (BIG_gul_SeekCur < BIG_gul_SeekCache))
	{
		BIG_gul_SeekCur += size;
		return BIG_fread(p, size, h);
	}

	if(SIZE_CACHE - (BIG_gul_SeekCur - BIG_gul_SeekCache) > size)
	{
		L_memcpy(p, BIG_gp_Cache + (BIG_gul_SeekCur - BIG_gul_SeekCache), size);
		BIG_gul_SeekCur += size;
		return 1;
	}

	BIG_gul_Miss++;
	BIG_fseek(h, BIG_gul_SeekCur, L_SEEK_SET);
	BIG_fread(BIG_gp_Cache, SIZE_CACHE, h);
	BIG_gul_SeekCache = BIG_gul_SeekCur;
	L_memcpy(p, BIG_gp_Cache, size);
	BIG_gul_SeekCur += size;
	return 1;
}

#else
#define SeekSeek(a, b, c)	L_fseek(a, b, c)
#define ReadRead(a, b, c)	BIG_fread(a, b, c)
#endif

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Retreive the length of a file. The length of each file is saved at the beginning of the file content. It's
            not in FAT cause it is used for loading, but is not needed after (so this will avoid to loose 4 bytes per
            file in FAT).

    Note:   Be carreful cause that function read some bytes on drive (cause length of file is never in memory). In:
            Position in bigfile (not in FAT !) of the file.

    Out:    Length of file.
 =======================================================================================================================
 */
ULONG BIG_ul_GetLengthFile(ULONG _ul_Pos)
{
	/*~~~~~~~~~~~~~~*/
	ULONG	ul_Length,r;
	/*~~~~~~~~~~~~~~*/

	/* Seek to the beginning of the file */
	if(BIG_gi_ReadMode != 2)
	{
		r=SeekSeek(BIG_Handle(), _ul_Pos, L_SEEK_SET);
		ERR_X_Error(r == 0, L_ERR_Csz_FSeek, NULL);
	}

	/*
	 * Read length of file. The length of the file is saved at the beginning of the
	 * file buffer.
	 */
	if(!LOA_IsBinaryData())
	{
		r=ReadRead((UCHAR *) &ul_Length, sizeof(ULONG), BIG_Handle());
		ERR_X_Error(r == 1, L_ERR_Csz_FRead, NULL);
		if(LOA_IsSwapperActive()) SwapDWord(&ul_Length);
	}

#ifdef ACTIVE_EDITORS
	if(LOA_IsBinarizing())
	{
		/*~~~~~~~~~~~~*/

		/* If we're binarizing, we save the length of the file for binary load */
		CHAR	*pc_Buf;
		/*~~~~~~~~~~~~*/

		pc_Buf = (CHAR *) &ul_Length;

		LOA_FetchBuffer(0); /* Used to tell the LOA system to "change" files. i.e. the previous file is finished. */
		LOA_ReadULong(&pc_Buf);
	}

#endif /* #ifdef ACTIVE_EDITORS */
	if(LOA_IsBinaryData())
	{
		/*~~~~~~~~~~~~*/

		/*
		 * In the binary data, the original length of the file has been saved for binary
		 * load
		 */
		CHAR	*pc_Buf;
		/*~~~~~~~~~~~~*/

		pc_Buf = LOA_FetchBuffer(sizeof(ULONG));

		ul_Length = LOA_ReadULong(&pc_Buf);
	}


	return ul_Length;
}

/*
 =======================================================================================================================
    Aim:    Read a file in a given buffer. Be carreful cause the buffer must be big enough to get the while file. In:
            _ul_Pos Position of the file in bigfile (not in FAT). _p_Buffer Buffer where the file will be loaded.

    Out:    Return the length of the file readen.
 =======================================================================================================================
 */
void BIG_Read(ULONG _ul_Pos, void *_p_Buffer, ULONG _ul_Length)
{
	int r;
	if(!_ul_Length) return;

	/* Seek to the beginning of the file */
	r=SeekSeek(BIG_Handle(), _ul_Pos, L_SEEK_SET);
	ERR_X_Error(r == 0, L_ERR_Csz_FSeek, NULL);

	/* Read */
	r=ReadRead((UCHAR *) _p_Buffer, _ul_Length, BIG_Handle());
	ERR_X_Error(r == 1, L_ERR_Csz_FRead, NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_ReadNoSeek(ULONG _ul_Pos, void *_p_Buffer, ULONG _ul_Length)
{
	int r;
	if(!_ul_Length) return;

	/* Seek to the beginning of the file */
	if(BIG_gi_ReadMode != 2)
	{
		r=SeekSeek(BIG_Handle(), _ul_Pos, L_SEEK_SET);
		ERR_X_Error(r == 0, L_ERR_Csz_FSeek, NULL);
	}

	/* Read */
	r=ReadRead((UCHAR *) _p_Buffer, _ul_Length, BIG_Handle());
	ERR_X_Error(r == 1, L_ERR_Csz_FRead, NULL);
}

/*
 =======================================================================================================================
    Aim:    Read a file in a given buffer. Be carreful cause the buffer must be big enough to get the while file. In:
            _ul_Pos Position of the file in bigfile (not in FAT). _p_Buffer Buffer where the file will be loaded.

    Out:    Return the length of the file readen.
 =======================================================================================================================
 */
ULONG BIG_ul_ReadFile(ULONG _ul_Pos, void *_p_Buffer)
{
	/*~~~~~~~~~~~~~~*/
	ULONG	ul_Length,r;
	/*~~~~~~~~~~~~~~*/

	/* Seek to the beginning of the file */
	if(BIG_gi_ReadMode != 2)
	{
		r=SeekSeek(BIG_Handle(), _ul_Pos, L_SEEK_SET);
		ERR_X_Error(r == 0, L_ERR_Csz_FSeek, NULL);
	}

	/*
	 * Read length of file. The length of the file is saved at the beginning of the
	 * file buffer.
	 */
	r=ReadRead((UCHAR *) &ul_Length, sizeof(ULONG), BIG_Handle());
	ERR_X_Error(r == 1, L_ERR_Csz_FRead, NULL);
	if(LOA_IsSwapperActive()) SwapDWord(&ul_Length);

	if(ul_Length)
	{
		/* Read file */
		r=ReadRead((UCHAR *) _p_Buffer, ul_Length & 0x7FFFFFFF, BIG_Handle());
		ERR_X_Error(r == 1, L_ERR_Csz_FRead, NULL);
	}

	return(ul_Length);
}

/*
 * Aim: To read a file in a temporary buffer. The buffer is never freeded until
 * BIG_FreeGlobalBuffer() function is called. The buffer is BIG_gp_GlobalBuffer
 * global var and can be used to load are even save some files without care of
 * allocation. In: _ul_Pos Position of the file to load in the bigfile (not in
 * fat). _pul_Length To receive the length of the file. Can be NULL to ignore.
 * Out: Return the address of the temporary buffer. Always return the value of
 * BIG_gp_GlobalBuffer.
 */
extern int	BIG_SpeedMode_fread(void **_p_Buffer, int _i_Size, BIGFileHandle _h_Handle);
static int	truc_nul = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#if !defined(XML_CONV_TOOL)
char *BIG_pc_ReadFileTmp(ULONG _ul_Pos, ULONG *_pul_Length)
{
	/*~~~~~~~~~~~~~~*/
	void	*p_Buffer;
	ULONG	ul_Length,r;
	/*~~~~~~~~~~~~~~*/


#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~*/
		BIG_INDEX	ul_Fat;
		/*~~~~~~~~~~~~~~~*/

		ul_Fat = BAS_bsearch(_ul_Pos, &BIG_gst.st_PosTableToFat);

        // Set the "loaded" flag
		if(ul_Fat != BIG_C_InvalidIndex) 
            BIG_FileChanged(ul_Fat) |= EDI_FHC_Loaded;
	}
#endif



	if(BIG_gi_ReadMode == 2)
	{
		/*~~~~~~~~~~~~~*/
		void	*pNewBuf;
		/*~~~~~~~~~~~~~*/

		pNewBuf = NULL;

		ul_Length = 0;
		pNewBuf = LOA_FetchFile(_pul_Length);
		if(pNewBuf) return (char *) pNewBuf;
	}
	else
	{
		/* Seek to the beginning of the file */
		r=SeekSeek(BIG_Handle(), _ul_Pos, L_SEEK_SET);
		ERR_X_Error(r == 0, L_ERR_Csz_FSeek, NULL);

		/*
		 * Read length of file. The length of the file is saved at the beginning of the
		 * file buffer.
		 */
		r=ReadRead((UCHAR *) &ul_Length, sizeof(ULONG), BIG_Handle());
		ERR_X_Error(r == 1, L_ERR_Csz_FRead, NULL);
		if(LOA_IsSwapperActive()) SwapDWord(&ul_Length);

		/* Eventually save size for caller */
		if(_pul_Length) *_pul_Length = ul_Length & 0x7FFFFFFF;

		LOA_FetchFile(_pul_Length);
		/* Allocate temporary buffer to receive file. We store a 0 at the end of the file. */
		if(ul_Length)
		{
			p_Buffer = BIG_p_RequestBuffer(ul_Length + 1);
			ERR_X_Error(p_Buffer != NULL, L_ERR_Csz_NotEnoughMemory, NULL);

			/* Read content of file */
			r=ReadRead((UCHAR *) p_Buffer, ul_Length, BIG_Handle());
			ERR_X_Error(r == 1, L_ERR_Csz_FRead, NULL);
			((UCHAR *) p_Buffer)[ul_Length] = 0;

#ifdef ACTIVE_EDITORS
			p_Buffer = BIG_special_LZOload((char*)p_Buffer, &ul_Length);
#endif
		}
        else
        {
            p_Buffer = (void *) &truc_nul;
        }
	}

	return (char *) p_Buffer;
}
#endif // XML_CONV_TOOL

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *BIG_pc_ReadPartOfFileTmp(ULONG _ul_Pos, ULONG ul_ReadSize)
{
	/*~~~~~~~~~~~~~~*/
	void	*p_Buffer;
	ULONG	ul_Length,r;
	/*~~~~~~~~~~~~~~*/

	p_Buffer = NULL;

#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~*/
		BIG_INDEX	ul_Fat;
		/*~~~~~~~~~~~~~~~*/

		ul_Fat = BAS_bsearch(_ul_Pos, &BIG_gst.st_PosTableToFat);
		if(ul_Fat != BIG_C_InvalidIndex) BIG_FileChanged(ul_Fat) |= EDI_FHC_Loaded;
	}

#endif
	if(BIG_gi_ReadMode == 2)
	{
		/*~~~~~~~~~~~~~*/
		void	*pNewBuf;
		/*~~~~~~~~~~~~~*/

		pNewBuf = NULL;

		ul_Length = 0;
		pNewBuf = LOA_FetchFile(&ul_Length);
		if(pNewBuf) return (char *) pNewBuf;
	}
	else
	{
		/* Seek to the beginning of the file */
		r=SeekSeek(BIG_Handle(), _ul_Pos, L_SEEK_SET);
		ERR_X_Error(r == 0, L_ERR_Csz_FSeek, NULL);

		LOA_FetchFile(&ul_ReadSize);

		/* Allocate temporary buffer to receive file. We store a 0 at the end of the file. */
		p_Buffer = BIG_p_RequestBuffer(ul_ReadSize + 1);
		ERR_X_Error(p_Buffer != NULL, L_ERR_Csz_NotEnoughMemory, NULL);

		if(ul_ReadSize)
		{
			/* Read content of file */
			r=ReadRead((UCHAR *) p_Buffer, ul_ReadSize, BIG_Handle());
			ERR_X_Error(r == 1, L_ERR_Csz_FRead, NULL);
			((UCHAR *) p_Buffer)[ul_ReadSize] = 0;
		}
	}

	return (char *) p_Buffer;
}

/*
 =======================================================================================================================
    Aim:    The same as previous function. But the return pointer must be free by caller cause it is not shared with
            other read and write operations.
 =======================================================================================================================
 */
char *BIG_pc_ReadFileTmpMustFree(ULONG _ul_Pos, ULONG *_pul_Length)
{
	/*~~~~~~~~~~~~~~*/
	void	*p_Buffer;
	ULONG	ul_Length,r;
	/*~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~*/
		BIG_INDEX	ul_Fat;
		/*~~~~~~~~~~~~~~~*/

		ul_Fat = BAS_bsearch(_ul_Pos, &BIG_gst.st_PosTableToFat);
		if(ul_Fat != BIG_C_InvalidIndex) BIG_FileChanged(ul_Fat) |= EDI_FHC_Loaded;
	}

#endif
	/* Seek to the beginning of the file */
	r=SeekSeek(BIG_Handle(), _ul_Pos, L_SEEK_SET);
	ERR_X_Error(r == 0, L_ERR_Csz_FSeek, NULL);

	/*
	 * Read length of file. The length of the file is saved at the beginning of the
	 * file buffer.
	 */
	r=ReadRead((UCHAR *) &ul_Length, sizeof(ULONG), BIG_Handle());
	ERR_X_Error(r == 1, L_ERR_Csz_FRead, NULL);
	if(LOA_IsSwapperActive()) SwapDWord(&ul_Length);

	/* Eventually save size for caller */
	if(_pul_Length) *_pul_Length = ul_Length;

	/* Allocate temporary buffer to receive file. We store a 0 at the end of the file. */
	p_Buffer = NULL;
	if(ul_Length)
	{
		p_Buffer = (void *) L_malloc(ul_Length + 1);
		ERR_X_Error(p_Buffer != NULL, L_ERR_Csz_NotEnoughMemory, NULL);

		/* Read content of file */
		r=ReadRead((UCHAR *) p_Buffer, ul_Length, BIG_Handle());
		ERR_X_Error(r == 1, L_ERR_Csz_FRead, NULL);
		((UCHAR *) p_Buffer)[ul_Length] = 0;
	}

	return (char *) p_Buffer;
}

/*
 =======================================================================================================================
    Aim:    Call to request an allocation of the temporary buffer. If the buffer is already big enough, no allocation
            occurs. Else the buffer is reallocated and BIG_gi_GlobalBufferSize is set to the new size. To free
            temporary buffer, call BIG_FreeGlobalBuffer function. In: _i_Size Size to allocate.

    Out:    Return address of buffer (BIG_gp_GlobalBuffer).
 =======================================================================================================================
 */
void *BIG_p_RequestBuffer(int _i_Size)
{
	/* If buffer is too small, (re)alloc it */
	if(_i_Size > BIG_gi_GlobalBufferSize)
	{
		BIG_gi_GlobalBufferSize = _i_Size;
		if(BIG_gp_GlobalBuffer)
		{
#ifdef ACTIVE_EDITORS
			BIG_gp_GlobalBuffer = L_realloc(BIG_gp_GlobalBuffer, BIG_gi_GlobalBufferSize + 4096);
			ERR_X_Assert(BIG_gp_GlobalBuffer);
#else
			BIG_gp_GlobalBuffer = MEM_p_Realloc(BIG_gp_GlobalBuffer, BIG_gi_GlobalBufferSize);
			ERR_X_Assert(BIG_gp_GlobalBuffer);
#endif
		}
		else
		{
#ifdef ACTIVE_EDITORS
			BIG_gp_GlobalBuffer = L_malloc(BIG_gi_GlobalBufferSize);
#else
#ifdef MEM_OPT
		    BIG_gp_GlobalBuffer = MEM_p_Alloc(BIG_gi_GlobalBufferSize);
#else // MEM_OPT
			BIG_gp_GlobalBuffer = MEM_p_AllocTmp(BIG_gi_GlobalBufferSize);
#endif // MEM_OPT

#endif
		}
	}

	return BIG_gp_GlobalBuffer;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *BIG_p_RequestSaveBuffer(int _i_Size)
{
	/* If buffer is too small, (re)alloc it */
	if(_i_Size > BIG_gi_GlobalSaveBufferSize)
	{
		BIG_gi_GlobalSaveBufferSize = _i_Size;
		if(BIG_gp_GlobalSaveBuffer)
		{
			BIG_gp_GlobalSaveBuffer = L_realloc(BIG_gp_GlobalSaveBuffer, BIG_gi_GlobalSaveBufferSize);
		}
		else
		{
			BIG_gp_GlobalSaveBuffer = L_malloc(BIG_gi_GlobalSaveBufferSize);
		}
	}

	return BIG_gp_GlobalSaveBuffer;
}

#endif

/*
 =======================================================================================================================
    Aim:    To clear global temporary buffer of bigfile. The temporary buffer can be used for a lot of usage (when
            loading for example). It's never desalocated until that function is called (its size grew up after each
            call to BIG_p_RequestBuffer function).
 =======================================================================================================================
 */
void BIG_FreeGlobalBuffer(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	extern BOOL ENG_gb_ExitApplication;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef ACTIVE_EDITORS

	/* if(!ENG_gb_ExitApplication) return; */
#endif
	if(BIG_gp_GlobalBuffer)
	{
#ifdef ACTIVE_EDITORS
		L_free(BIG_gp_GlobalBuffer);
#else
#ifdef _GAMECUBE
		MEM_Free(BIG_gp_GlobalBuffer);
#else // _GAMECUBE
		if(MEM_gp_AllocTmpFirst) 
			MEM_Free(BIG_gp_GlobalBuffer);
#endif // _GAMECUBE
#endif
	}

	BIG_gp_GlobalBuffer = NULL;
	BIG_gi_GlobalBufferSize = 0;
	BIG_FreeSpecialBuffer();

#ifdef ACTIVE_EDITORS
	if(BIG_gp_GlobalSaveBuffer) L_free(BIG_gp_GlobalSaveBuffer);
	BIG_gp_GlobalSaveBuffer = NULL;
	BIG_gi_GlobalSaveBufferSize = 0;
#endif
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
    get size of file without any BIG_fread access (editor data only -> not in bin process)
 =======================================================================================================================
 */
ULONG BIG_ul_EditorGetSizeOfFile(ULONG ul_FileKey)
{
	/*~~~~~~~~~~~~~~*/
	ULONG	ul_Pos;
	ULONG	ul_Length;
 	fpos_t	pos;
 	fpos_t	pos2;


	if(ul_FileKey == -1) return 0;
	ul_Pos = BIG_ul_SearchKeyToPos(ul_FileKey);
	if(ul_Pos == -1) return 0;
	if(ul_Pos == 0) return 0;
	if(ul_Pos == ul_FileKey) return 0;


	fgetpos(BIG_Handle(), &pos);
	
	pos2 = (fpos_t)ul_Pos ;
	fsetpos(BIG_Handle(), &pos2);

	fread((UCHAR *) &ul_Length, sizeof(ULONG),1, BIG_Handle());

	fsetpos(BIG_Handle(), &pos);

	return ul_Length;
}

#endif 
 
 