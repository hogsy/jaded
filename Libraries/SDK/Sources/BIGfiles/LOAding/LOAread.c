/*$T LOAread.c GC! 1.081 07/02/02 16:27:28 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "LOAread.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BIGfiles/BIGfat.h"

#include "LOAdefs.h"

extern int	BIG_SpeedMode_fread(void **, int _i_Size, L_FILE _h_Handle);

/*$F
 *
 * The LOA_BINARIZATION define activates the implicit saving code in the reads for
 * binarization Binarization is only possible in editor version
 *
 */
#define LOA_DEFAULT_LONG	0x00000000
#define LOA_DEFAULT_SHORT	0x0000
#define LOA_DEFAULT_CHAR	0x00
#define LOA_DEFAULT_FLOAT	0.0f

#define LOA_SIZE_LONG		sizeof(LONG)
#define LOA_SIZE_SHORT		sizeof(SHORT)
#define LOA_SIZE_CHAR		sizeof(CHAR)
#define LOA_SIZE_FLOAT		sizeof(float)

/*$F
 *
 * LOA_gb_SwapperActive indicates whether longs and shorts must be swapped because
 * the disk data and the platform are in incompatible format (big endian (68x, GC)
 * vs little endian (x86, PS2)) e.g. used when reading PC data on GameCube
 *
 */
BOOL				LOA_gb_SwapperActive = FALSE;

/*$F
 *
 * LOA_gb_SwapOnWrite indicates if the data (longs & shorts) should be swapped
 * before writing to disk When this is set, the data on disk will be written
 * swapped, but the resulting data used by the engine will remain untouched. e..g
 * Used when creating native binary data for GameCube on PC.
 *
 */
BOOL				LOA_gb_SwapOnWrite = FALSE;

#ifdef LOA_BINARIZATION
static CHAR			*LOA_gspc_BinaryBufferBegin = NULL;
static CHAR			*LOA_gspc_BinaryBuffer = NULL;	/* contains the binarized data. */
static LONG			LOA_gsl_BinaryBufferSize = 0;
static LONG			LOA_gsl_BinaryBufferGranularity = 0;
static unsigned int LOA_gui_BinCheckSum = 0;
static unsigned int LOA_gui_BinCheckSumActivity = 0;
static CHAR			*LOA_gspc_BinaryBufferBegin_Old = NULL;
static CHAR			*LOA_gspc_BinaryBuffer_Old = NULL;
#endif /* LOA_BINARIZATION */

static LONG			gl_LastLengthPos = -1;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *LOA_FetchFile(ULONG *_pul_Length)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LOA_tdstBinFileHeader	stBinFileHeader;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef PSX2_TARGET
	/* Philippe : Read PAD during loading */
	/* */
	{
		static u32 LastVBlankCounter = 0;
		extern void INO_Update();
		u_int volatile VBlankCounter;
		extern u32 BinkVideoWith;
		 // Avoid to call it each time.
		if ((BinkVideoWith) &&(LastVBlankCounter != VBlankCounter))
		{
			BOOL SaveLoading;
			extern BOOL LOA_gb_Loading;
			SaveLoading = LOA_gb_Loading;
			LOA_gb_Loading = FALSE;
			INO_Update();
			LOA_gb_Loading = SaveLoading;
			LastVBlankCounter = VBlankCounter;
		}
	}//*/
#endif	

#ifdef LOA_BINARIZATION
	if(LOA_IsBinarizing())
	{
		/*~~~~~~~~~~~~*/
		CHAR	*pc_Tmp;
		/*~~~~~~~~~~~~*/

		pc_Tmp = (CHAR *) &stBinFileHeader;

		/*$1- "Terminate" the previous file ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		LOA_FetchBuffer(0);

		/*$1- Write header to the binary data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		stBinFileHeader.ul_Size = _pul_Length ? *_pul_Length : 0;
		_LOA_ReadBinFileHeader(&pc_Tmp, NULL, LOA_eBinFileInfo);

		gl_LastLengthPos = (LONG)
			(
				LOA_GetCurBinaryBuffer() -
				LOA_GetBinaryBuffer() -
				sizeof(LOA_tdstBinFileHeader) +
				ubiGetFieldOffset(LOA_tdstBinFileHeader, ul_Size)
			);
	}
	else
#endif /* LOA_BINARIZATION */
		if(LOA_IsBinaryData())
		{
			/*~~~~~~~~~~~~~~~~~~*/
			CHAR	*pc_BinBuffer;
			/*~~~~~~~~~~~~~~~~~~*/

			pc_BinBuffer = NULL;

			/*$1- Fetch the necessary data (possibly uncompress as needed) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			BIG_SpeedMode_fread((void**)&pc_BinBuffer, sizeof(LOA_tdstBinFileHeader), BIG_Handle());

			/*$1- Write header to the binary data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			_LOA_ReadBinFileHeader((CHAR**)&pc_BinBuffer, &stBinFileHeader, LOA_eBinFileInfo);

			/*$1- restore the actual data size ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			*_pul_Length = stBinFileHeader.ul_Size;

			return LOA_FetchBuffer(stBinFileHeader.ul_Size);
		}

	return NULL;
}


/*$4
 ***********************************************************************************************************************
    Binarization related utilitary functions
 ***********************************************************************************************************************
 */

#ifdef LOA_BINARIZATION

/*
 =======================================================================================================================
    Aim:    Initializes the binary buffer used to store loaded data in order to save a file used for binary versions.

    Note:   Must be called before any binarisation operation.

    In:     _lBufferSize = Initial Buffer Size

    In:     _lGranularity = Granularity for buffer adjustment (i.e. buffer size incremented by this value when
            required)
 =======================================================================================================================
 */
void LOA_InitBinaryBuffer(LONG _lBufferSize, LONG _lGranularity)
{
	ERR_X_Assert(LOA_gspc_BinaryBuffer == NULL);		/* The binary buffer should be cleared before setting it again */
	ERR_X_Assert(LOA_gspc_BinaryBufferBegin == NULL);	/* The binary buffer should be cleared before setting it again */
	ERR_X_Assert(LOA_gsl_BinaryBufferSize == 0);		/* The binary buffer should be cleared before setting it again */
	ERR_X_Assert(_lBufferSize >= 0);					/* The binary buffer should have a positive initial value */
	ERR_X_Assert(_lGranularity > 0);					/* Granularity should be non null to grow the buffer as
														 * necessary */

	LOA_gspc_BinaryBufferBegin = LOA_gspc_BinaryBuffer = (CHAR *) L_malloc(_lBufferSize);
    L_memset(LOA_gspc_BinaryBufferBegin, 0, _lBufferSize);

	LOA_gsl_BinaryBufferSize = _lBufferSize;
	LOA_gsl_BinaryBufferGranularity = _lGranularity;

	LOA_gui_BinCheckSum = 0;
    LOA_gui_BinCheckSumActivity=0;
}

/*
 =======================================================================================================================
    Aim:    Frees the binary buffer. It must be reinitialized before any binarisation operation is called
 =======================================================================================================================
 */
void LOA_ClearBinaryBuffer(void)
{
	if(LOA_gspc_BinaryBufferBegin != NULL) L_free(LOA_gspc_BinaryBufferBegin);
	LOA_gspc_BinaryBuffer = NULL;
	LOA_gspc_BinaryBufferBegin = NULL;
	LOA_gsl_BinaryBufferSize = 0;
	LOA_gsl_BinaryBufferGranularity = 0;

	LOA_gui_BinCheckSum = 0;
    LOA_gui_BinCheckSumActivity=0;
}

/*
 =======================================================================================================================
    Aim:    Returns a pointer to the beginning of the binarisation buffer
 =======================================================================================================================
 */
CHAR *LOA_GetBinaryBuffer(void)
{
	return LOA_gspc_BinaryBufferBegin;
}

/*
 =======================================================================================================================
    Aim:    Returns a pointer to the current position in the binarisation buffer
 =======================================================================================================================
 */
CHAR *LOA_GetCurBinaryBuffer(void)
{
	return LOA_gspc_BinaryBuffer;
}

/*
 =======================================================================================================================
    Aim:    Activates Swap On Write mode (see declaration of the variable for details)
 =======================================================================================================================
 */
void LOA_SetSwapOnWrite(BOOL _bSwapOnWrite)
{
	LOA_gb_SwapOnWrite = _bSwapOnWrite;
}


/*
 =======================================================================================================================
    Aim:    Writes to the binarisation buffer!
 =======================================================================================================================
 */
void LOA_WriteToBinaryBuffer(CHAR *_pData, LONG _lLength)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	extern unsigned int BIG_GetChecksumFromBuff(char *, int len);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ERR_X_Assert(LOA_gspc_BinaryBuffer != NULL);	/* The binary buffer should be created with LOA_InitBinaryBuffer */

	/* Check if the buffer is big enough and make it grow if necessary. */
	if((LOA_gspc_BinaryBuffer - LOA_gspc_BinaryBufferBegin + _lLength) > LOA_gsl_BinaryBufferSize)
	{
		/*~~~~~~~~~~~~~*/
		LONG	lNewSize;
		LONG	lCurPos;
		LONG	lReqSize;
		/*~~~~~~~~~~~~~*/

		lNewSize = LOA_gsl_BinaryBufferSize;
		lCurPos = LOA_gspc_BinaryBuffer - LOA_gspc_BinaryBufferBegin;
		lReqSize = lCurPos + _lLength;

/*#ifdef ACTIVE_EDITORS
		
        if(0)//LOA_gui_BinCheckSum)
		{
			
			unsigned int	newchecksum;
			

            LOA_gui_BinCheckSumActivity++;
            if(LOA_gui_BinCheckSumActivity == 100)
            {
			    newchecksum = BIG_GetChecksumFromBuff
				    (
					    LOA_gspc_BinaryBufferBegin_Old,
					    LOA_gspc_BinaryBuffer_Old - LOA_gspc_BinaryBufferBegin_Old
				    );
			    ERR_X_Assert(newchecksum == LOA_gui_BinCheckSum);
                LOA_gui_BinCheckSumActivity=0;
            }
		}

#endif*/
		while(lNewSize < lReqSize)
		{
			lNewSize += LOA_gsl_BinaryBufferGranularity;
		}

		/* Evil realloc, but done only while binarizing, never in a final version. */
		LOA_gspc_BinaryBufferBegin = (CHAR*)L_realloc(LOA_gspc_BinaryBufferBegin, lNewSize);
		LOA_gspc_BinaryBuffer = LOA_gspc_BinaryBufferBegin + lCurPos;
        L_memset(LOA_gspc_BinaryBuffer , 0, lNewSize-LOA_gsl_BinaryBufferSize);
		LOA_gsl_BinaryBufferSize = lNewSize;


/*#ifdef ACTIVE_EDITORS
		
		LOA_gspc_BinaryBuffer_Old = LOA_gspc_BinaryBuffer;
		LOA_gspc_BinaryBufferBegin_Old = LOA_gspc_BinaryBufferBegin;
        LOA_gui_BinCheckSum = BIG_GetChecksumFromBuff
			(
				LOA_gspc_BinaryBufferBegin,
				LOA_gspc_BinaryBuffer - LOA_gspc_BinaryBufferBegin
			);
#endif*/

    }


	L_memcpy(LOA_gspc_BinaryBuffer, _pData, _lLength);
	LOA_gspc_BinaryBuffer += _lLength;

}

#endif /* LOA_BINARIZATION */

/*$4
 ***********************************************************************************************************************
    Loading configuration functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Activates the byte swapper (swaps longs and shorts) used to convert little endian disk data to big endian
            and vice versa
 =======================================================================================================================
 */
void LOA_SetIsSwapperActive(BOOL _bSwapperActive)
{
	LOA_gb_SwapperActive = _bSwapperActive;
}


/*
 =======================================================================================================================
    Aim:    Returns the pointer to the current binary buffer and increments the internal binary buffer by the given
            size.

    Note:   Used like BIG_pc_ReadFileTmp when it is not not appropriate to call BIG_pc_ReadFileTmp. For example, when
            the binarization process adds data that is specifically attached to a position in the bigfile. An example
            of use is in SND.c to know before hand the types of elements in the sound banks. When binarizing, this
            functions sets the actual size back in the header before the data and resets the offset for the data
            header.
 =======================================================================================================================
 */
CHAR *LOA_FetchBuffer(ULONG _ul_Length)
{
	if(LOA_IsBinaryData())
	{
		/*~~~~~~~~~~~~~~~~~~*/
		CHAR	*pc_BinBuffer;
		/*~~~~~~~~~~~~~~~~~~*/

		pc_BinBuffer = NULL;

		/* Fetch the necessary data (possibly uncompress as needed) */
		BIG_SpeedMode_fread((void**)&pc_BinBuffer, _ul_Length, BIG_Handle());

		return pc_BinBuffer;
	}

#ifdef LOA_BINARIZATION
	if(LOA_IsBinarizing())
	{
		/*$F
		 * The Actual Size in the binary data is the size written in the binary buffer
		 * (CurBinaryBuffer-BinaryBuffer) less the amount of data written prior to this
		 * file which is marked by the pointer to the Size in the header
		 * (gl_LastLengthPos) to which we must substract the rest of the size of the
		 * header (+ ubiGetFieldOffset(LOA_tdstBinFileHeader, ul_Size) -
		 * sizeof(LOA_tdstBinFileHeader)
		 */
		if(gl_LastLengthPos >= 0)
		{
			/*~~~~~~~~~~~~~~~*/
			ULONG	ulDiskSize;
			ULONG	*p;
			ULONG	ulNewSize;
			/*~~~~~~~~~~~~~~~*/

			ulDiskSize = *(LONG *) (LOA_GetBinaryBuffer() + gl_LastLengthPos);
			p = (ULONG *) (LOA_GetBinaryBuffer() + gl_LastLengthPos);
			ulNewSize = (ULONG)
				(
					LOA_GetCurBinaryBuffer() -
					LOA_GetBinaryBuffer() -
					gl_LastLengthPos -
					sizeof(LOA_tdstBinFileHeader) +
					ubiGetFieldOffset(LOA_tdstBinFileHeader, ul_Size)
				);

/*#ifdef ACTIVE_EDITORS
            if(0)//LOA_gui_BinCheckSum)
			{
				
				unsigned int	newchecksum;
				

                LOA_gui_BinCheckSumActivity++;
                if(LOA_gui_BinCheckSumActivity==100)
                {
                    LOA_gui_BinCheckSumActivity=0;
				    newchecksum = BIG_GetChecksumFromBuff
					    (
						    LOA_gspc_BinaryBufferBegin_Old,
						    LOA_gspc_BinaryBuffer_Old - LOA_gspc_BinaryBufferBegin_Old
					    );
				    ERR_X_Assert(newchecksum == LOA_gui_BinCheckSum);
                }
			}

#endif*/
			/* save the actual size back before the buffer */
			if(LOA_IsSwapperActive() || LOA_SwapOnWrite())
			{
				SwapDWord(&ulNewSize);
			}

			*p = ulNewSize;

			if(*p == 0 && (ulDiskSize != 0))
			{
				LOA_gspc_BinaryBuffer -= sizeof(LOA_tdstBinFileHeader);
			}

			gl_LastLengthPos = -1;	/* Reset the last length position */

/*#ifdef ACTIVE_EDITORS
	
	        LOA_gui_BinCheckSum = BIG_GetChecksumFromBuff
				(
					LOA_gspc_BinaryBufferBegin,
					LOA_gspc_BinaryBuffer - LOA_gspc_BinaryBufferBegin
				);
			LOA_gspc_BinaryBuffer_Old = LOA_gspc_BinaryBuffer;
			LOA_gspc_BinaryBufferBegin_Old = LOA_gspc_BinaryBufferBegin;
#endif*/
		}
		else
		{
			return (CHAR *) -1;
		}

		return NULL;
	}

#endif /* LOA_BINARIZATION */
	return NULL;
}


/*$4
 ***********************************************************************************************************************
    Basic Types Loading
 ***********************************************************************************************************************
 */

/*$F
 =======================================================================================================================
    Basic Type Load 
    - Load Phase 
        - Normal (eng data, editor data in Editor) 
        - Aligned Load (PS2)
        - Skip (Engine and LOA_IsBinaryData() == False and (_eBinFlags & LOA_eBinEditorData) == TRUE) 
        - Do nothing (LOA_IsBinaryData() == True and (_eBinFlags & LOA_eBinEditorData) == TRUE) 
    - Swap (if LOA_IsSwapperActive(), i.e. mismatched platform <=> data) 
    - Save for Bin (if LOA_IsBinarizing() == TRUE)
 =======================================================================================================================
 */

LONG _LOA_ReadLong(CHAR **_ppcBuffer, LONG *_Data, LOA_eBinFlags _eBinFlags)
{
	/*~~~~~~~~~~~~~~~*/
	LONG	Data;
	LONG	lIncrement;
	/*~~~~~~~~~~~~~~~*/

	Data = LOA_DEFAULT_LONG;
	lIncrement = 0;

	/*$F Load Phase 
     *  - Skip (Engine and LOA_IsBinaryData() == False and (_eBinFlags & LOA_eBinEditorData) == TRUE) 
     *  - Do nothing (LOA_IsBinaryData() == True and (_eBinFlags & LOA_eBinEditorData) == TRUE) 
	 */

	/*$1- Binary data doesn't contain editor data, so it won't be read. ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	lIncrement = ((_eBinFlags & LOA_eBinEditorData) && LOA_IsBinaryData()) ? 0 : LOA_SIZE_LONG;
	if(lIncrement)
	{
#ifdef LOA_ALIGNED_LOAD
		Data = ReadAlignedLong(*_ppcBuffer);
#else
		Data = *((LONG *) * _ppcBuffer);
#endif
		if(LOA_IsSwapperActive())
		{
			SwapDWord(&Data);
		}
	}

#ifdef LOA_BINARIZATION

	/*$1- Save for Bin (if LOA_IsBinarizing() == TRUE) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(LOA_IsBinarizing() && !(_eBinFlags & (LOA_eBinEditorData | LOA_eBinLookAheadData)))
	{
		if(LOA_SwapOnWrite()) SwapDWord(&Data);
		LOA_WriteToBinaryBuffer((CHAR *) &Data, LOA_SIZE_LONG);
		if(LOA_SwapOnWrite()) SwapDWord(&Data);
	}

#endif /* LOA_BINARIZATION */

	/*$1- Increment the buffer by the amount of data read ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	(*_ppcBuffer) += lIncrement;

	/*$1- copy data to the user's buffer ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_Data)
	{
#ifdef LOA_ALIGNED_LOAD
		((CHAR *) _Data)[0] = ((CHAR *) &Data)[0];
		((CHAR *) _Data)[1] = ((CHAR *) &Data)[1];
		((CHAR *) _Data)[2] = ((CHAR *) &Data)[2];
		((CHAR *) _Data)[3] = ((CHAR *) &Data)[3];
#else
		*_Data = Data;
#endif
	}

	return Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SHORT _LOA_ReadShort(CHAR **_ppcBuffer, SHORT *_Data, LOA_eBinFlags _eBinFlags)
{
	/*~~~~~~~~~~~~~~~*/
	SHORT	Data;
	LONG	lIncrement;
	/*~~~~~~~~~~~~~~~*/

	Data = LOA_DEFAULT_SHORT;
	lIncrement = 0;

	/*$F Load Phase 
     *  - Skip (Engine and LOA_IsBinaryData() == False and (_eBinFlags & LOA_eBinEditorData) == TRUE) 
     *  - Do nothing (LOA_IsBinaryData() == True and (_eBinFlags & LOA_eBinEditorData) == TRUE) 
	 */

	/*$1- Binary data doesn't contain editor data, so it won't be read. ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	lIncrement = ((_eBinFlags & LOA_eBinEditorData) && LOA_IsBinaryData()) ? 0 : LOA_SIZE_SHORT;
	if(lIncrement)
	{
#ifdef LOA_ALIGNED_LOAD
		Data = ReadAlignedShort(*_ppcBuffer);
#else
		Data = *((SHORT *) * _ppcBuffer);
#endif
		if(LOA_IsSwapperActive())
		{
			SwapWord(&Data);
		}
	}

#ifdef LOA_BINARIZATION

	/*$1- Save for Bin (if LOA_IsBinarizing() == TRUE) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(LOA_IsBinarizing() && !(_eBinFlags & (LOA_eBinEditorData | LOA_eBinLookAheadData)))
	{
		if(LOA_SwapOnWrite()) SwapWord(&Data);
		LOA_WriteToBinaryBuffer((CHAR *) &Data, LOA_SIZE_SHORT);
		if(LOA_SwapOnWrite()) SwapWord(&Data);
	}

#endif

	/*$1- Increment the buffer by the amount of data read ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	(*_ppcBuffer) += lIncrement;

	/*$1- copy data read ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_Data)
	{
#ifdef LOA_ALIGNED_LOAD
		((CHAR *) _Data)[0] = ((CHAR *) &Data)[0];
		((CHAR *) _Data)[1] = ((CHAR *) &Data)[1];
#else
		*_Data = Data;
#endif
	}

	return Data;
}

/*
 =======================================================================================================================
    Aim:    No aligned loading or swapping necessary for chars
 =======================================================================================================================
 */
CHAR _LOA_ReadChar(CHAR **_ppcBuffer, CHAR *_Data, LOA_eBinFlags _eBinFlags)
{
	/*~~~~~~~~~~~~~~~*/
	CHAR	Data;
	LONG	lIncrement;
	/*~~~~~~~~~~~~~~~*/

	Data = LOA_DEFAULT_CHAR;
	lIncrement = 0;

	/*$F Load Phase
     *  - Skip (Engine and LOA_IsBinaryData() == False and (_eBinFlags & LOA_eBinEditorData) == TRUE) 
     *  - Do nothing (LOA_IsBinaryData() == True and (_eBinFlags & LOA_eBinEditorData) == TRUE) 
     */

	/*$1- Binary data doesn't contain editor data, so it won't be read. ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	lIncrement = ((_eBinFlags & LOA_eBinEditorData) && LOA_IsBinaryData()) ? 0 : LOA_SIZE_CHAR;
	if(lIncrement)
	{
		Data = *((CHAR *) * _ppcBuffer);
	}

#ifdef LOA_BINARIZATION

	/*$1- Save for Bin (if LOA_IsBinarizing() == TRUE) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(LOA_IsBinarizing() && !(_eBinFlags & (LOA_eBinEditorData | LOA_eBinLookAheadData)))
	{
		LOA_WriteToBinaryBuffer(&Data, LOA_SIZE_CHAR);
	}

#endif

	/*$1- Increment the buffer by the amount of data read ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	(*_ppcBuffer) += lIncrement;

	/*$1- copy data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_Data) *_Data = Data;

	return Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float _LOA_ReadFloat(CHAR **_ppcBuffer, float *_Data, LOA_eBinFlags _eBinFlags)
{
	/*~~~~~~~~~~~~~~~*/
	float	Data;
	LONG	lIncrement;
	/*~~~~~~~~~~~~~~~*/

	Data = LOA_DEFAULT_LONG;
	lIncrement = 0;

	/*$F Load Phase 
     *  - Skip (Engine and LOA_IsBinaryData() == False and (_eBinFlags & LOA_eBinEditorData) == TRUE) 
     *  - Do nothing (LOA_IsBinaryData() == True and (_eBinFlags & LOA_eBinEditorData) == TRUE)     
	 */

	/*$1- Binary data doesn't contain editor data, so it won't be read. ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	lIncrement = ((_eBinFlags & LOA_eBinEditorData) && LOA_IsBinaryData()) ? 0 : LOA_SIZE_FLOAT;
	if(lIncrement)
	{
#ifdef LOA_ALIGNED_LOAD
		Data = ReadAlignedFloat(*_ppcBuffer);
#else
		Data = *((float *) * _ppcBuffer);
#endif
		if(LOA_IsSwapperActive())
		{
			SwapDWord((LONG *) &Data);
		}
	}

#ifdef LOA_BINARIZATION

	/*$1- Save for Bin (if LOA_IsBinarizing() == TRUE) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(LOA_IsBinarizing() && !(_eBinFlags & (LOA_eBinEditorData | LOA_eBinLookAheadData)))
	{
		if(LOA_SwapOnWrite()) SwapDWord((LONG *) &Data);
		LOA_WriteToBinaryBuffer((CHAR *) &Data, LOA_SIZE_FLOAT);
		if(LOA_SwapOnWrite()) SwapDWord((LONG *) &Data);
	}

#endif
	/*$-1 Increment the buffer by the amount of data read */
	(*_ppcBuffer) += lIncrement;

	/*$1- copy data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_Data)
	{
#ifdef LOA_ALIGNED_LOAD
		((CHAR *) _Data)[0] = ((CHAR *) &Data)[0];
		((CHAR *) _Data)[1] = ((CHAR *) &Data)[1];
		((CHAR *) _Data)[2] = ((CHAR *) &Data)[2];
		((CHAR *) _Data)[3] = ((CHAR *) &Data)[3];
#else
		*_Data = Data;
#endif
	}

	return Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void _LOA_ReadCharArray(CHAR **_ppcBuffer, CHAR *_pcDestArray, LONG _lArraySize, LOA_eBinFlags _eBinFlags)
{
	/*~~*/
	int i;
	/*~~*/

#ifdef ACTIVE_EDITORS
	for(i = 0; i < _lArraySize; ++i, _pcDestArray ? _pcDestArray++ : 0)
	{
		_LOA_ReadChar(_ppcBuffer, _pcDestArray, _eBinFlags);
	}

#else
	if(_pcDestArray) L_memcpy(_pcDestArray, *_ppcBuffer, _lArraySize);
	i = ((_eBinFlags & LOA_eBinEditorData) && LOA_IsBinaryData()) ? 0 : _lArraySize;
	*_ppcBuffer += i;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void _LOA_ReadString(CHAR **_ppcBuffer, CHAR *_szString, LONG _lStringLength, LOA_eBinFlags _eBinFlags)
{
	SKIP_IF_NULL_DATA(*_ppcBuffer, _szString, _lStringLength);
	*_szString = '\0';
	_LOA_ReadCharArray(_ppcBuffer, _szString, _lStringLength, _eBinFlags);
#if !defined(XML_CONV_TOOL)
	_szString[_lStringLength - 1] = 0;	/* Zero-terminate the string */
#endif
}

/*$4
 ***********************************************************************************************************************
    Complex Types Loading
 ***********************************************************************************************************************
 */

/*$F
 * Complex Type Load 
 *  - Load Phase 
 *      - Load each field of the structures and use the appropriate call for editor only data
 */

/*
 =======================================================================================================================
    Note:   INTERNAL LOA Binarization use
 =======================================================================================================================
 */
void _LOA_ReadBinFileHeader(CHAR **_ppcBuffer, LOA_tdstBinFileHeader *_Data, LOA_eBinFlags _eBinFlags)
{
#ifdef FORCE_OPTIMIZED_LOAD
	M_OPT_LOAD_MEMCPY(*_ppcBuffer, _Data, LOA_tdstBinFileHeader);
#endif /* #ifdef FORCE_OPTIMIZED_LOAD */
	_LOA_ReadLong(_ppcBuffer, (LONG*)(_Data ? &_Data->ul_Size : NULL), _eBinFlags);
}

/*
 =======================================================================================================================
    Note:   MATH
 =======================================================================================================================
 */
void _LOA_ReadVector(CHAR **_ppcBuffer, MATH_tdst_Vector *_Data, LOA_eBinFlags _eBinFlags)
{
	SKIP_IF_NULL_DATA(*_ppcBuffer, _Data, sizeof(MATH_tdst_Vector));
	M_OPT_LOAD_MEMCPY(*_ppcBuffer, _Data, MATH_tdst_Vector);
	_LOA_ReadFloat(_ppcBuffer, &_Data->x, _eBinFlags);
	_LOA_ReadFloat(_ppcBuffer, &_Data->y, _eBinFlags);
	_LOA_ReadFloat(_ppcBuffer, &_Data->z, _eBinFlags);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void _LOA_ReadMatrix(CHAR **_ppcBuffer, MATH_tdst_Matrix *_Data, LOA_eBinFlags _eBinFlags)
{
	SKIP_IF_NULL_DATA(*_ppcBuffer, _Data, sizeof(MATH_tdst_Matrix));
	M_OPT_LOAD_MEMCPY(*_ppcBuffer, _Data, MATH_tdst_Matrix);

	_LOA_ReadFloat(_ppcBuffer, &_Data->Ix, _eBinFlags);
	_LOA_ReadFloat(_ppcBuffer, &_Data->Iy, _eBinFlags);
	_LOA_ReadFloat(_ppcBuffer, &_Data->Iz, _eBinFlags);
	_LOA_ReadFloat(_ppcBuffer, &_Data->Sx, _eBinFlags);
	_LOA_ReadFloat(_ppcBuffer, &_Data->Jx, _eBinFlags);
	_LOA_ReadFloat(_ppcBuffer, &_Data->Jy, _eBinFlags);
	_LOA_ReadFloat(_ppcBuffer, &_Data->Jz, _eBinFlags);
	_LOA_ReadFloat(_ppcBuffer, &_Data->Sy, _eBinFlags);
	_LOA_ReadFloat(_ppcBuffer, &_Data->Kx, _eBinFlags);
	_LOA_ReadFloat(_ppcBuffer, &_Data->Ky, _eBinFlags);
	_LOA_ReadFloat(_ppcBuffer, &_Data->Kz, _eBinFlags);
	_LOA_ReadFloat(_ppcBuffer, &_Data->Sz, _eBinFlags);
	_LOA_ReadVector(_ppcBuffer, &_Data->T, _eBinFlags);
	_LOA_ReadFloat(_ppcBuffer, &_Data->w, _eBinFlags);
	_LOA_ReadLong(_ppcBuffer, &_Data->lType, _eBinFlags);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void _LOA_ReadQuaternion(CHAR **_ppcBuffer, MATH_tdst_Quaternion *_Data, LOA_eBinFlags _eBinFlags)
{
	SKIP_IF_NULL_DATA(*_ppcBuffer, _Data, sizeof(MATH_tdst_Quaternion));
	M_OPT_LOAD_MEMCPY(*_ppcBuffer, _Data, MATH_tdst_Quaternion);

	_LOA_ReadFloat(_ppcBuffer, &_Data->x, _eBinFlags);
	_LOA_ReadFloat(_ppcBuffer, &_Data->y, _eBinFlags);
	_LOA_ReadFloat(_ppcBuffer, &_Data->z, _eBinFlags);
	_LOA_ReadFloat(_ppcBuffer, &_Data->w, _eBinFlags);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void _LOA_ReadCompressedQuaternion(CHAR **_ppcBuffer, MATH_tdst_CompressedQuaternion *_Data, LOA_eBinFlags _eBinFlags)
{
	SKIP_IF_NULL_DATA(*_ppcBuffer, _Data, sizeof(MATH_tdst_CompressedQuaternion));
	M_OPT_LOAD_MEMCPY(*_ppcBuffer, _Data, MATH_tdst_CompressedQuaternion);

	_LOA_ReadShort(_ppcBuffer, &_Data->x, _eBinFlags);
	_LOA_ReadShort(_ppcBuffer, &_Data->y, _eBinFlags);
	_LOA_ReadShort(_ppcBuffer, &_Data->z, _eBinFlags);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void _LOA_ReadUltraCompressedQuaternion
(
	CHAR								**_ppcBuffer,
	MATH_tdst_UltraCompressedQuaternion *_Data,
	LOA_eBinFlags						_eBinFlags
)
{
	SKIP_IF_NULL_DATA(*_ppcBuffer, _Data, sizeof(MATH_tdst_UltraCompressedQuaternion));
	M_OPT_LOAD_MEMCPY(*_ppcBuffer, _Data, MATH_tdst_UltraCompressedQuaternion);

	_LOA_ReadShort(_ppcBuffer, &_Data->s, _eBinFlags);
	_LOA_ReadChar(_ppcBuffer, &_Data->c1, _eBinFlags);
	_LOA_ReadChar(_ppcBuffer, &_Data->c2, _eBinFlags);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void _LOA_ReadCompressedVector(CHAR **_ppcBuffer, MATH_tdst_CompressedVector *_Data, LOA_eBinFlags _eBinFlags)
{
	SKIP_IF_NULL_DATA(*_ppcBuffer, _Data, sizeof(MATH_tdst_CompressedVector));
	M_OPT_LOAD_MEMCPY(*_ppcBuffer, _Data, MATH_tdst_CompressedVector);

	_LOA_ReadShort(_ppcBuffer, &_Data->x, _eBinFlags);
	_LOA_ReadShort(_ppcBuffer, &_Data->y, _eBinFlags);
	_LOA_ReadShort(_ppcBuffer, &_Data->z, _eBinFlags);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void _LOA_ReadUltraCompressedVector(CHAR **_ppcBuffer, MATH_tdst_UltraCompressedVector *_Data, LOA_eBinFlags _eBinFlags)
{
	SKIP_IF_NULL_DATA(*_ppcBuffer, _Data, sizeof(MATH_tdst_UltraCompressedVector));
	M_OPT_LOAD_MEMCPY(*_ppcBuffer, _Data, MATH_tdst_UltraCompressedVector);

	_LOA_ReadShort(_ppcBuffer, &_Data->s, _eBinFlags);
	_LOA_ReadChar(_ppcBuffer, &_Data->c1, _eBinFlags);
	_LOA_ReadChar(_ppcBuffer, &_Data->c2, _eBinFlags);
}

/*
 =======================================================================================================================
    Note:   AI
 =======================================================================================================================
 */
void _LOA_ReadAINode(CHAR **_ppcBuffer, AI_tdst_Node *_Data, LOA_eBinFlags _eBinFlags)
{
	SKIP_IF_NULL_DATA(*_ppcBuffer, _Data, sizeof(AI_tdst_Node));
	M_OPT_LOAD_MEMCPY(*_ppcBuffer, _Data, AI_tdst_Node);

	_LOA_ReadLong(_ppcBuffer, &_Data->l_Param, _eBinFlags);
	_LOA_ReadShort(_ppcBuffer, &_Data->w_Param, _eBinFlags);
	_LOA_ReadChar(_ppcBuffer, &_Data->c_Flags, _eBinFlags);
	_LOA_ReadChar(_ppcBuffer, &_Data->c_Type, _eBinFlags);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void _LOA_ReadAILocal(CHAR **_ppcBuffer, AI_tdst_Local *_Data, LOA_eBinFlags _eBinFlags)
{
	SKIP_IF_NULL_DATA(*_ppcBuffer, _Data, sizeof(AI_tdst_Local));

	_LOA_ReadInt(_ppcBuffer, &_Data->i_Offset, _eBinFlags);
	_LOA_ReadInt(_ppcBuffer, &_Data->i_Type, _eBinFlags);
	_LOA_ReadString(_ppcBuffer, (CHAR *) &_Data->asz_Name, AI_C_MaxLenVar, _eBinFlags);
	LOA_ReadShort_Ed(_ppcBuffer, NULL); /* Skip 2 bytes because of struct padding on PC (2 ints + string[30] = 38 + 2
										 * on PC = 40 on disk for native data) */
}

