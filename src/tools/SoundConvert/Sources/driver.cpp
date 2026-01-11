/*$T driver.cpp GC 1.138 10/05/04 08:21:54 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "stdafx.h"
#include <direct.h>
#include <stdio.h>
#include "imaadpcm.h"
#include "wavparse.h"
#include "MSADPCM.h"
#include "main.h"
#include "driver.h"
#include "CWaveFile.h"
#include "ps2adpcm.h"
#include "gcadpcm.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

unsigned	CalculateCodedBytes
			(
				unsigned	numSamples,
				unsigned	samplesPerBlock,
				unsigned	nBytesPerBlock,
				unsigned	&numBlocks,
				unsigned	&extraSamples
			);

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*$F 
    we round wav size on 448 samples by this way it suits to all targets
    PS2 = 28 samples 
    GC  = 14 samples
    XB  = 64 samples
*/
int gai_PaddingSize[FMT_e_Nb] = { 448 /* none */, 0 /* ps2 */, 0 /* gc */, 64 /* xbox */, 0 /* pcretail */ };

/*
 =======================================================================================================================
 =======================================================================================================================
 */
HRESULT GetSourceInfo
(
	const char		*szInput,
	BYTE			**ppbSampleData,
	DWORD			*pdwDuration,
	DWORD			*pdwPadding,
	WAVEFORMATEX	*pwfx
)
{
	/*~~~~~~~~~~~~~~~~~~*/
	CWaveParser wfSrc;	/* Source file */
	DWORD		dwSamples;
	/*~~~~~~~~~~~~~~~~~~*/

	*ppbSampleData = NULL;
	*pdwDuration = 0;
	*pdwPadding = 0;
	ZeroMemory(pwfx, sizeof(WAVEFORMATEX));

	/* Open the source wav file */
	if(FAILED(wfSrc.Open(szInput)))
	{
		printf("Couldn't open %s.\n", szInput);
		return E_FAIL;
	}

	/* Get the format and size of the source data */
	wfSrc.GetFormat(pwfx, sizeof(WAVEFORMATEX));
	wfSrc.GetDuration(pdwDuration);

	/*
	 * Don't compress 8-bit sounds - the codec can't reliably do it, and £
	 * it would sound bad, anyway.
	 */
	if(pwfx->wBitsPerSample != 16)
	{
		printf("You can only compress 16-bit sources.\n");
		return E_FAIL;
	}

	dwSamples = *pdwDuration / pwfx->nBlockAlign;

	/* See if we need to pad the end to a multiple of "gai_PaddingSize" samples */
	if(gai_PaddingSize[MAIN_gst_Command.e_Format] && (dwSamples % gai_PaddingSize[MAIN_gst_Command.e_Format]))
	{
		*pdwPadding =
			(
				gai_PaddingSize[MAIN_gst_Command.e_Format] -
				(dwSamples % gai_PaddingSize[MAIN_gst_Command.e_Format])
			) *
			pwfx->nBlockAlign;
	}

	/* and allocate a buffer for it */
	*ppbSampleData = new BYTE[*pdwDuration +*pdwPadding];
	if(!*ppbSampleData)
	{
		printf("Couldn't allocate %d bytes to read sample data.n", *pdwDuration +*pdwPadding);
		return E_FAIL;
	}

	/* Read the source data */
	wfSrc.ReadSample(0, *ppbSampleData, *pdwDuration, pdwDuration);
	wfSrc.Close();

	/* Fill remainder with silence */
	ZeroMemory(*ppbSampleData +*pdwDuration, *pdwPadding);

	return S_OK;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
HRESULT WriteCompressedFile
(
	const char			*szInput,
	const char			*szOutput,
	DWORD				dwOriginalLength,
	BYTE				*pbEncodedData,
	DWORD				dwNewLength,
	IMAADPCMWAVEFORMAT	*pwfx,
	BOOL				bCopyAllChunks
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HANDLE		hSrc = INVALID_HANDLE_VALUE;
	HANDLE		hDest = INVALID_HANDLE_VALUE;
	RIFFHEADER	rh;
	RIFFHEADER	rh_wave;
	HRESULT		hr = S_OK;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Open the source file */
	hSrc = CreateFile(szInput, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0L, NULL);
	if(INVALID_HANDLE_VALUE == hSrc)
	{
		printf("Couldn't open %s", szInput);
		hr = E_FAIL;
		goto Error;
	}

	/* Create the destination file */
	hDest = CreateFile(szOutput, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if(INVALID_HANDLE_VALUE == hDest)
	{
		printf("Couldn't open output file %s\n", szOutput);
		hr = E_FAIL;
		goto Error;
	}

	for(;;)
	{
		/*~~~~~~~~~~~~*/
		BOOL	fReadOK;
		DWORD	cb;
		/*~~~~~~~~~~~~*/

		/* Read the chunk header */
		fReadOK = ReadFile(hSrc, &rh, sizeof(RIFFHEADER), &cb, NULL);
		if(!fReadOK)
		{
			printf("Error reading from %s.\n", szInput);
			hr = E_FAIL;
			goto Error;
		}

		/* Check to see if we hit the end of the file */
		if(cb == 0) break;

		switch(rh.fccChunkId)
		{
		case FOURCC_RIFF:
			/* Correct the data size to account for compression */
			rh_wave = rh;
			rh_wave.dwDataSize = 0;

			if(bCopyAllChunks)
			{
				/*
				 * If we're copying everything over, just subtract the £
				 * compression amount, and account for the difference in £
				 * wave format structures
				 */
				if(MAIN_gst_Command.e_Format == FMT_e_XBOX)
					rh.dwDataSize -= (dwOriginalLength - dwNewLength) + sizeof(WAVEFORMATEX) - sizeof(IMAADPCMWAVEFORMAT);
				else
					rh.dwDataSize -= (dwOriginalLength - dwNewLength);
			}
			else
			{
				/*
				 * If we're not copying everything over, then just £
				 * calculate the new data size
				 */
				if(MAIN_gst_Command.e_Format == FMT_e_XBOX)
					rh.dwDataSize = 2 * sizeof(RIFFHEADER) + sizeof(FOURCC) + sizeof(IMAADPCMWAVEFORMAT) + dwNewLength;
				else
					rh.dwDataSize = 2 * sizeof(RIFFHEADER) + sizeof(FOURCC) + sizeof(WAVEFORMATEX) + dwNewLength;
			}

			/* Write out the correct RIFF header */
			WriteFile(hDest, &rh, sizeof(RIFFHEADER), &cb, NULL);
			//rh_wave.dwDataSize += sizeof(RIFFHEADER);

			/*~~~~~~~~~~~~~~~*/
			/* Copy the form type over */
			DWORD	dwFormType;
			/*~~~~~~~~~~~~~~~*/

			ReadFile(hSrc, &dwFormType, sizeof(DWORD), &cb, NULL);
			WriteFile(hDest, &dwFormType, sizeof(DWORD), &cb, NULL);
			rh_wave.dwDataSize += sizeof(DWORD);

			break;

		case FOURCC_FORMAT:
			/* Skip past the original format */
			SetFilePointer(hSrc, rh.dwDataSize, NULL, FILE_CURRENT);

			/* Correct the data size for the format struct */
			if(MAIN_gst_Command.e_Format == FMT_e_XBOX)
				rh.dwDataSize = sizeof(IMAADPCMWAVEFORMAT);
			else
				rh.dwDataSize = sizeof(WAVEFORMATEX);

			/* Write out the correct FMT header */
			WriteFile(hDest, &rh, sizeof(RIFFHEADER), &cb, NULL);
			rh_wave.dwDataSize += sizeof(RIFFHEADER);

			/* Write out the new format struct */
			pwfx->wfx.cbSize = 0;
			if(MAIN_gst_Command.e_Format == FMT_e_XBOX)
			{
				pwfx->wfx.cbSize = 2;
				WriteFile(hDest, pwfx, sizeof(IMAADPCMWAVEFORMAT), &cb, NULL);
				rh_wave.dwDataSize += sizeof(IMAADPCMWAVEFORMAT);
			}
			else
			{
				WriteFile(hDest, pwfx, sizeof(WAVEFORMATEX), &cb, NULL);
				rh_wave.dwDataSize += sizeof(WAVEFORMATEX);
			}

			break;

		case FOURCC_DATA:
			/* Skip past the original data */
			SetFilePointer(hSrc, rh.dwDataSize, NULL, FILE_CURRENT);

			/* Correct the data size for compressed sample data */
			rh.dwDataSize = dwNewLength;

			/* Write out the correct DATA header */
			WriteFile(hDest, &rh, sizeof(RIFFHEADER), &cb, NULL);
			rh_wave.dwDataSize += sizeof(RIFFHEADER);

			/* Write out the compressed sample data */
			WriteFile(hDest, pbEncodedData, dwNewLength, &cb, NULL);
			rh_wave.dwDataSize += dwNewLength;

			break;


		case FOURCC_CUE:
		case FOURCC_PLST:
		case FOURCC_LABL:
		case FOURCC_NOTE:
		case FOURCC_LTXT:
		case FOURCC_LIST:
		case FOURCC_INFO:
			if(bCopyAllChunks)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				/* Allocate a block of memory for the chunk data */
				BYTE	*pbChunkData = new BYTE[rh.dwDataSize];
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				if(!pbChunkData)
				{
					printf("Couldn't allocate enough memory to copy data.\n");
					hr = E_FAIL;
					goto Error;
				}

				/* Copy the chunk header over */
				WriteFile(hDest, &rh, sizeof(RIFFHEADER), &cb, NULL);
				rh_wave.dwDataSize += sizeof(RIFFHEADER);

				/* Copy the chunk data over */
				ReadFile(hSrc, pbChunkData, rh.dwDataSize, &cb, NULL);
				WriteFile(hDest, pbChunkData, rh.dwDataSize, &cb, NULL);
				rh_wave.dwDataSize+=rh.dwDataSize;

				/* Release the memory */
				delete[] pbChunkData;
			}
			else
			{
				/* Seek to the next chunk */
				SetFilePointer(hSrc, rh.dwDataSize, NULL, FILE_CURRENT);
			}
			break;




		default:
			if(0)//bCopyAllChunks)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				/* Allocate a block of memory for the chunk data */
				BYTE	*pbChunkData = new BYTE[rh.dwDataSize];
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				if(!pbChunkData)
				{
					printf("Couldn't allocate enough memory to copy data.\n");
					hr = E_FAIL;
					goto Error;
				}

				/* Copy the chunk header over */
				WriteFile(hDest, &rh, sizeof(RIFFHEADER), &cb, NULL);
				rh_wave.dwDataSize += sizeof(RIFFHEADER);

				/* Copy the chunk data over */
				ReadFile(hSrc, pbChunkData, rh.dwDataSize, &cb, NULL);
				WriteFile(hDest, pbChunkData, rh.dwDataSize, &cb, NULL);
				rh_wave.dwDataSize+=rh.dwDataSize;

				/* Release the memory */
				delete[] pbChunkData;
			}
			else
			{
				/* Seek to the next chunk */
				SetFilePointer(hSrc, rh.dwDataSize, NULL, FILE_CURRENT);
			}
			break;
		}
	}

	DWORD	cb;
	SetFilePointer(hDest, 0, NULL, FILE_BEGIN);
	WriteFile(hDest, &rh_wave, sizeof(RIFFHEADER), &cb, NULL);
Error:
	/* Clean up */
	if(INVALID_HANDLE_VALUE != hSrc) CloseHandle(hSrc);
	if(INVALID_HANDLE_VALUE != hDest) CloseHandle(hDest);

	return hr;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void vConvertWavFile(char *_szInput, char *_szOutput)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DWORD				dwDuration;
	DWORD				dwPadding;
	BYTE				*pbSampleData = NULL;
	BYTE				*pbEncodedData = NULL;
	WAVEFORMATEX		wfx;
	IMAADPCMWAVEFORMAT	wfxEncode;
	CImaAdpcmCodec		xboxcodec;
	CPS2AdpcmCodec		ps2codec;
	CGCAdpcmCodec		GCcodec;
	BOOL				b_Result;
	DWORD				dwDestBlocks;
	DWORD				dwDestLength;
	short				*pAlignedSampleData;
	unsigned			numSamples;
	unsigned			nBytesPerBlock;
	unsigned			extraSamples;
	unsigned			totalSamples;
	unsigned			numBlocks;
	unsigned			codedBytes;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	if(!MAIN_gst_Command.b_Silent) printf("%s ...\n", _szInput);

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    Get the source data and wave format
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	if(FAILED(GetSourceInfo(_szInput, &pbSampleData, &dwDuration, &dwPadding, &wfx))) return;

	if(wfx.wFormatTag != WAVE_FORMAT_PCM || wfx.nChannels > 2)
	{
		printf("%s : invalid source format.\nSource must be mono or stereo 16-bit PCM.\n", _szInput);
		return;
	}

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    Create an APDCM format structure based off the source format
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	switch(MAIN_gst_Command.e_Format)
	{
	default:
		printf("*** internal error\n");
		return;

	case FMT_e_PCRETAIL:
		xboxcodec.CreateImaAdpcmFormat(wfx.nChannels, wfx.nSamplesPerSec, XBOX_ADPCM_SAMPLES_PER_BLOCK, &wfxEncode);
		numSamples = dwDuration / (wfx.nChannels * sizeof(unsigned short));
		nBytesPerBlock = wfx.nChannels * C_MSADPCM_MONO_BLOCKSIZE;
		extraSamples = numSamples % C_MSADPCM_SAMPLESPERBLOCK;
		totalSamples = C_MSADPCM_SAMPLESPERBLOCK * ((numSamples / C_MSADPCM_SAMPLESPERBLOCK) + 1);

		pAlignedSampleData = new short[totalSamples * sizeof(short) * wfx.nChannels];

		/* Copy the first samples */
		memcpy(pAlignedSampleData, pbSampleData, sizeof(short) * wfx.nChannels * numSamples);
		if(extraSamples)
		{
			/* Fill with zeroes samples */
			memset
			(
				pAlignedSampleData + (numSamples * sizeof(short) * wfx.nChannels),
				0,
				(C_MSADPCM_SAMPLESPERBLOCK - extraSamples) * sizeof(short) * wfx.nChannels
			);
		}

		codedBytes = CalculateCodedBytes
			(
				numSamples,
				C_MSADPCM_SAMPLESPERBLOCK,
				nBytesPerBlock,
				numBlocks,
				extraSamples
			);

		/* Allocate a buffer for encoded data */
		pbEncodedData = new BYTE[codedBytes];
		memset(pbEncodedData, 0, codedBytes);
		dwDestLength = numSamples;

		CompressADPCM
		(
			codedBytes,
			numSamples,
			(short *) pAlignedSampleData,
			pbEncodedData,
			numBlocks,
			extraSamples,
			wfx.nChannels
		);

		/* Write the encoded data to output */
		WriteADPCMWave(_szOutput, codedBytes, pbEncodedData, wfx.nChannels, numSamples, wfx.nSamplesPerSec);

		/* Clean up */
		delete[] pAlignedSampleData;
		break;

	case FMT_e_XBOX:
		xboxcodec.CreateImaAdpcmFormat(wfx.nChannels, wfx.nSamplesPerSec, XBOX_ADPCM_SAMPLES_PER_BLOCK, &wfxEncode);

		/* Calculate number of ADPCM blocks and length of ADPCM data */
		dwDestBlocks = (dwDuration + dwPadding) / wfx.nBlockAlign / XBOX_ADPCM_SAMPLES_PER_BLOCK;
		dwDestLength = dwDestBlocks * wfxEncode.wfx.nBlockAlign;

		/* Allocate a buffer for encoded data */
		pbEncodedData = new BYTE[dwDestLength];

		/* Initialize the codec */
		b_Result = xboxcodec.Initialize(&wfxEncode, TRUE);

		if(FALSE == b_Result)
		{
			printf("Couldn't initialize codec.\n");
			return;
		}

		/* Convert the data */
		b_Result = xboxcodec.Convert(pbSampleData, pbEncodedData, dwDestBlocks);
		if(FALSE == b_Result)
		{
			printf("Codec failed.\n");
			return;
		}

		/* Write out the encoded file */
		WriteCompressedFile
		(
			_szInput,
			_szOutput,
			dwDuration,
			pbEncodedData,
			dwDestLength,
			&wfxEncode,
			MAIN_gst_Command.b_CopyAllChunk
		);
		break;

	case FMT_e_PS2:
		dwDuration += dwPadding;
		ps2codec.CreateFormat(wfx.nChannels, wfx.nSamplesPerSec, (WAVEFORMATEX *) &wfxEncode);

		dwDestLength = ps2codec.uiGetEncodedFileSize(dwDuration / wfx.nChannels);
		dwDestLength *= wfx.nChannels;
		pbEncodedData = new BYTE[dwDestLength];

		/* Initialize the codec */
		b_Result = ps2codec.Initialize((WAVEFORMATEX *) &wfxEncode, MAIN_gst_Command.b_MuxStereo);

		if(FALSE == b_Result)
		{
			printf("Couldn't initialize codec.\n");
			return;
		}

		/* Convert the data */
		b_Result = ps2codec.Convert(pbSampleData, pbEncodedData, dwDuration);
		if(FALSE == b_Result)
		{
			printf("Codec failed.\n");
			return;
		}

		/* Write out the encoded file */
		WriteCompressedFile
		(
			_szInput,
			_szOutput,
			dwDuration,
			pbEncodedData,
			dwDestLength,
			&wfxEncode,
			MAIN_gst_Command.b_CopyAllChunk
		);
		break;

	case FMT_e_GC:
		dwDuration += dwPadding;
		GCcodec.CreateFormat(wfx.nChannels, wfx.nSamplesPerSec, (WAVEFORMATEX *) &wfxEncode);

		dwDestLength = GCcodec.uiGetEncodedFileSize(dwDuration / wfx.nChannels);
		dwDestLength *= wfx.nChannels;
		pbEncodedData = new BYTE[dwDestLength];

		/* Initialize the codec */
		b_Result = GCcodec.Initialize((WAVEFORMATEX *) &wfxEncode, MAIN_gst_Command.b_MuxStereo);

		if(FALSE == b_Result)
		{
			printf("Couldn't initialize codec.\n");
			return;
		}

		/* Convert the data */
		b_Result = GCcodec.Convert(pbSampleData, pbEncodedData, dwDuration);
		if(FALSE == b_Result)
		{
			printf("Codec failed.\n");
			return;
		}

		/* Write out the encoded file */
		WriteCompressedFile
		(
			_szInput,
			_szOutput,
			dwDuration,
			pbEncodedData,
			dwDestLength,
			&wfxEncode,
			MAIN_gst_Command.b_CopyAllChunk
		);
		break;
	}

	delete[] pbSampleData;
	delete[] pbEncodedData;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned CalculateCodedBytes
(
	unsigned	numSamples,
	unsigned	samplesPerBlock,
	unsigned	nBytesPerBlock,
	unsigned	&numBlocks,
	unsigned	&extraSamples
)
{
	/*~~~~~~~~~~~~~~*/
	unsigned	coded;
	int			j;
	/*~~~~~~~~~~~~~~*/

	numBlocks = numSamples / samplesPerBlock;
	extraSamples = numSamples % samplesPerBlock;
	if(extraSamples)
	{
		/* There is a block more */
		numBlocks++;
		if(extraSamples == 1) j = 0;
	}

	coded = numBlocks * nBytesPerBlock;

	return coded;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void vScanADirectory(char *_szInputDirectory, char *_szOutputDirectory)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WIN32_FIND_DATA stWFD;
	HANDLE			hFD;
	BOOL			bContinue;
	char			szCurrentPath[MAX_PATH];
	char			szInputSubDirectory[MAX_PATH];
	char			szOutputSubDirectory[MAX_PATH];
	char			szInput[MAX_PATH];	/* Source file name */
	char			szOutput[MAX_PATH]; /* Destination file name */
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sprintf(szCurrentPath, "%s*.*", _szInputDirectory);
	hFD = FindFirstFile(szCurrentPath, &stWFD);
	bContinue = (hFD != INVALID_HANDLE_VALUE);

	while(bContinue)
	{
		if(stWFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			/* this is a subdirectory : create output directory and convert its wave files */
			if(stWFD.cFileName[0] != '.')
			{
				/* create new directory in output */
				SetCurrentDirectory(_szOutputDirectory);
				_mkdir(stWFD.cFileName);

				/* scan the input directory */
				sprintf(szInputSubDirectory, "%s%s\\", _szInputDirectory, stWFD.cFileName);
				sprintf(szOutputSubDirectory, "%s%s\\", _szOutputDirectory, stWFD.cFileName);
				vScanADirectory(szInputSubDirectory, szOutputSubDirectory);
			}
		}
		else
		{
			/* this is a file */
			SetCurrentDirectory(_szInputDirectory);
			strcpy(szInput, stWFD.cFileName);
			strcpy(szOutput, _szOutputDirectory);
			strcat(szOutput, szInput);

			ConvertOneFile(szInput, szOutput);
		}

		bContinue = FindNextFile(hFD, &stWFD);
	}

	FindClose(hFD);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ConvertOneFile(char *szInput, char *szOutput)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	char	*pcPoint;
	char	asz_TempDir[1024];
	char	asz_TempFile[2048];
	HANDLE	hTempFile;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pcPoint = strrchr(szInput, '.');
	if(!pcPoint)
	{
		CopyFile(szInput, szOutput, FALSE);
	}
	else
	{
		if(_strnicmp(pcPoint, ".wa", 3))
		{
			CopyFile(szInput, szOutput, FALSE);
		}
		else
		{
			if(MAIN_gst_Command.b_AlignMarker)
			{
				GetTempPath(1024, (LPTSTR) asz_TempDir);
				GetTempFileName(asz_TempDir, "CVT", 0, asz_TempFile);

				hTempFile = CreateFile
					(
						(LPTSTR) asz_TempFile,
						GENERIC_READ | GENERIC_WRITE,
						0,
						NULL,
						CREATE_ALWAYS,
						FILE_ATTRIBUTE_TEMPORARY,
						NULL
					); 
				CloseHandle(hTempFile);

				AlignMarker(szInput, asz_TempFile);
				strcpy(szInput, asz_TempFile);
			}

			switch(MAIN_gst_Command.e_Format)
			{
			case FMT_e_PS2:
			case FMT_e_GC:
			case FMT_e_XBOX:
			case FMT_e_PCRETAIL:
				vConvertWavFile(szInput, szOutput);
				break;

			case FMT_e_Native:
                CopyFile(szInput, szOutput, FALSE);
				break;

			default:
				break;
			}

			if(MAIN_gst_Command.b_AlignMarker)
			{
				DeleteFile(asz_TempFile);
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AlignMarker(char *szInput, char *szOutput)
{
	/*~~~~~~~~~~~~*/
	CRIFFForm	oRIFF;
	/*~~~~~~~~~~~~*/

	if(!MAIN_gst_Command.b_AlignMarker) return;

	oRIFF.Open(szInput);
    oRIFF.Load();
	oRIFF.Close();

	oRIFF.AlignMarkers(448);
	
    oRIFF.Open(szOutput, false);
	oRIFF.Save();
	oRIFF.Close();
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
