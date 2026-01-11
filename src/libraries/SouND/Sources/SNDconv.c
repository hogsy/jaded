/*$T SNDconv.c GC! 1.097 01/23/02 17:45:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"


/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
#include <math.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "BASe/CLIbrary/CLImem.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"

#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

#include "LINks/LINKmsg.h"
#include "../Main/WinEditors/Sources/EDIpaths.h"

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconv.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SND.h"

/*$4
 ***********************************************************************************************************************
    GLOBAL
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL SND_b_WAV_ParseMemory
(
	LPVOID			lpChunkOfMemory,							/* Points to raw ram */
	LPWAVEFORMATEX	*lplpWaveHeader,							/* Points to pointer to header */
	BYTE			**lplpWaveSamples,							/* Points to pointer to samples */
	LPDWORD			lpcbWaveSize, /* Points to size */
    LPDWORD			lpWaveHeaderSize
)																
{
	/*~~~~~~~~~~~~~*/
	LPDWORD pdw;
	LPDWORD pdwEnd;
	DWORD	dwRiff;
	DWORD	dwType;
	DWORD	dwLength;
	DWORD	numSamples;
	/*~~~~~~~~~~~~~*/

	/* Set defaults to NULL or zero */
	if(lplpWaveHeader) *lplpWaveHeader = NULL;
	if(lplpWaveSamples) *lplpWaveSamples = NULL;
	if(lpcbWaveSize) *lpcbWaveSize = 0;
    if(lpWaveHeaderSize) *lpWaveHeaderSize = 0;

	/* Set up DWORD pointers to the start of the chunk of memory. */
	pdw = (DWORD *) lpChunkOfMemory;

	/* Get the type and length of the chunk of memory */
	dwRiff = *pdw++;
	dwLength = *pdw++;
	dwType = *pdw++;

	if(dwRiff != mmioFOURCC('R', 'I', 'F', 'F')) return FALSE;	/* Not even RIFF */
	if(dwType != mmioFOURCC('W', 'A', 'V', 'E')) return FALSE;	/* Not a WAV */

	/* Find the pointer to the end of the chunk of memory */
	pdwEnd = (DWORD *) ((BYTE *) pdw + dwLength - 4);

	/* Run through the bytes looking for the tags */
	while(pdw < pdwEnd)
	{
		dwType = *pdw++;
		dwLength = *pdw++;

		switch(dwType)
		{
		/* Found the format part */
		case mmioFOURCC('f', 'm', 't', ' '):
			if(lplpWaveHeader && !*lplpWaveHeader)
			{
				if(dwLength < sizeof(WAVEFORMAT)) return FALSE; /* Something's wrong! Not a WAV */
                if(lpWaveHeaderSize) *lpWaveHeaderSize = dwLength;

				/* Set the lplpWaveHeader to point to this part of the memory chunk */
				*lplpWaveHeader = (LPWAVEFORMATEX) pdw;

				/*
				 * Check to see if the other two items have been filled out yet (the bits and the
				 * size of the bits). If so, then this chunk of memory has been parsed out and we
				 * can exit
				 */
				if((!lplpWaveSamples || *lplpWaveSamples) && (!lpcbWaveSize || *lpcbWaveSize))
				{
					return TRUE;
				}
			}
			break;

		/* Found the samples */
		case mmioFOURCC('d', 'a', 't', 'a'):
			if((lplpWaveSamples && !*lplpWaveSamples) || (lpcbWaveSize && !*lpcbWaveSize))
			{
				/* Point the samples pointer to this part of the chunk of memory. */
				if(lplpWaveSamples) *lplpWaveSamples = (BYTE *) pdw;

				/* Set the size of the wave */
				if(lpcbWaveSize) *lpcbWaveSize = dwLength;

				/* Make sure we have our header pointer set up. If we do, we can exit */
				if(!lplpWaveHeader || *lplpWaveHeader) return TRUE;
			}
			break;
		case mmioFOURCC('f','a','c','t'):
			numSamples = (DWORD) (*pdw);
			break;
		}														/* End case */

		/* Move the pointer through the chunk of memory */
		pdw = (DWORD *) ((BYTE *) pdw + ((dwLength + 1) &~1));
	}

	/* Failed! If we made it here, we did not get all the pieces of the wave */
	return FALSE;
}



#endif
