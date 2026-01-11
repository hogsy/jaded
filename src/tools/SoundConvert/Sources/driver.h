/*$T driver.h GC 1.138 05/12/04 16:45:47 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
void	ConvertOneFile(char *szInput, char *szOutput);
void	vScanADirectory(char *_szInputDirectory, char *_szOutputDirectory);
HRESULT WriteCompressedFile
		(
			const char			*szInput,
			const char			*szOutput,
			DWORD				dwOriginalLength,
			BYTE				*pbEncodedData,
			DWORD				dwNewLength,
			IMAADPCMWAVEFORMAT	*pwfx,
			BOOL				bCopyAllChunks
		);

void	AlignMarker(char *szInput, char *szOutput);
