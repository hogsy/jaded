// ************************************************
// Video Subtitles Manager
//
// Used for King Kong Xenon
// By Alexandre David (October 2005)
// ************************************************

#include "Precomp.h"

#if defined(_XENON)

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "VideoSubtitles.h"

#include "SDK/Sources/INOut/INO.h"
#include "XenonGraphics/XeRenderer.h"
#include "XenonGraphics/XeTextureManager.h"

// ***********************************************************************************************************************
//    Constants
// ***********************************************************************************************************************

const char* kacLanguageCode[] =
{
	"FR", // French
	"US", // English
	"DA", // Danish
	"DU", // Dutch
	"FI", // Finnish
	"GE", // German
	"IT", // Italian
	"SP", // Spanish
	"US", // Portuguese
	"SW", // Swedish
	"PO", // Polish
	"RU", // Russian
	"US", // Japanese
	"US", // Chinese
	"US", // Albanian
	"US", // Arabic
	"US", // Bulgarian
	"US", // Byelorussian
	"US", // Greek
	"US", // Korean
	"NO", // Norwegian
	"US", // Romanian
	"US", // Serbian
	"US", // Slovak
	"US", // Slovenian
	"US", // Turkish
	"US", // Czech
	"US", // Hungarian
	"US"  // Taiwanese
};

// ***********************************************************************************************************************
//    Externs
// ***********************************************************************************************************************

extern int IO_gi_SubtitleOn;
extern int TEXT_i_GetLang(void);

// ***********************************************************************************************************************
//    Functions
// ***********************************************************************************************************************

CVideoSubtitles::CVideoSubtitles()
{
	m_iFileSize		= 0;
	m_pFileBuffer	= NULL;
	m_iFont_ID		= MAT_Xe_InvalidTextureId;
	m_pFont_Texture	= NULL;
	m_uiStartTime	= 0;
}

// ***********************************************************************************************************************

CVideoSubtitles::~CVideoSubtitles()
{
	Close();
}

// ***********************************************************************************************************************

char* CVideoSubtitles::GetNextString(char* Param_pString)
{
	while ((*Param_pString != 0) && (*Param_pString != kcText_LineFeed))
	{
		Param_pString++;
	}

	return Param_pString;
}

// ***********************************************************************************************************************

int CVideoSubtitles::GetStringLength(char* Param_pTextBuffer)
{
	int iLength = 0;

	while (*Param_pTextBuffer != 0)
	{
		iLength++;
		Param_pTextBuffer++;
	}

	return iLength;
}

// ***********************************************************************************************************************

int CVideoSubtitles::GetStringWidth(char* Param_pString)
{
	int iWidth = 0;

	while ((*Param_pString != 0) && (*Param_pString != kcText_LineFeed))
	{
		iWidth++;
		Param_pString++;
	}

	return iWidth;
}

// ***********************************************************************************************************************

int CVideoSubtitles::GetStringHeight(char* Param_pString)
{
	int iHeight = 0;

	while (*Param_pString != 0)
	{
		if (*Param_pString == kcText_LineFeed)
		{
			iHeight++; 
		}

		Param_pString++;
	}

	return iHeight;
}

// ***********************************************************************************************************************

char* CVideoSubtitles::GetNextLine(char* Param_pTextBuffer, char* Param_pTextBufferEnd)
{
	while (Param_pTextBuffer < Param_pTextBufferEnd)
	{
		if (*Param_pTextBuffer == 0)
		{
			return Param_pTextBuffer + 2;
		}

		Param_pTextBuffer++;
	}

	return NULL;
}

// ***********************************************************************************************************************

float CVideoSubtitles::GetTimeCode(char* Param_pString)
{
	float TimeCode_InSeconds;

	// Handle Non-Digit Characters

	if ((Param_pString[0] < '0') || (Param_pString[0] > '9'))
	{
		return 1000000.0f;
	}

	// Calculate Seconds From String

	TimeCode_InSeconds =  (Param_pString[0] - '0') * 600.0f;
	TimeCode_InSeconds += (Param_pString[1] - '0') * 60.0f;
	TimeCode_InSeconds += (Param_pString[2] - '0') * 10.0f;
	TimeCode_InSeconds += (Param_pString[3] - '0');

	return TimeCode_InSeconds;
}

// ***********************************************************************************************************************

char* CVideoSubtitles::FindLanguageCode(char* Param_pTextBuffer, char* Param_pTextBufferEnd)
{
	while (Param_pTextBuffer < Param_pTextBufferEnd)
	{
		// Find Possible Start Of "LANG."

		while (*Param_pTextBuffer != 'L') 
		{
			Param_pTextBuffer++;
		};

		// Ensure Subsequent Characters Do Form "LANG."

		if ((Param_pTextBuffer[1] == 'A') &&
			(Param_pTextBuffer[2] == 'N') &&
			(Param_pTextBuffer[3] == 'G') &&
			(Param_pTextBuffer[4] == '.'))
		{
			// Found - Return Language Code Pointer

			return &Param_pTextBuffer[5];
		};

		// Skip "L" Character And Keep Looking

		Param_pTextBuffer++;
	}

	return NULL;
}

// ***********************************************************************************************************************

void CVideoSubtitles::RemoveLineFeeds(char* Param_pTextBuffer, int Param_iSize)
{
	while (Param_iSize > 0)
	{
		// Zero-Out Carriage Returns Or LineFeeds

		if ((*Param_pTextBuffer == '\n') || (*Param_pTextBuffer == '\r'))
		{
			*Param_pTextBuffer = 0;
		}

		// Tick Counters

		Param_pTextBuffer++;
		Param_iSize--;
	}
}

// ***********************************************************************************************************************

void CVideoSubtitles::FormatString(char* Param_pString_Source, char* Param_pString_Dest, int Param_iMaxLineLength)
{
	char	cPlaceHolder;
	char*	pCutPoint			= (char*)&cPlaceHolder;
	int		iCounter			= 0;
	int		iCutPointCounter	= 0;

	while (*Param_pString_Source != 0)
	{
		iCounter++;

		// Cut String If Too Long

		if (iCounter > Param_iMaxLineLength)
		{
			*pCutPoint	=  kcText_LineFeed;
			iCounter	-= iCutPointCounter;
		}

		// Reset Counter If Cut Point Seen

		if (*Param_pString_Source == kcText_LineFeed)
		{
			iCounter = 0;
		}

		// Copy Source Character To Dest String

		*Param_pString_Dest = *Param_pString_Source;

		// Update Cut Point With Last Space

		if (*Param_pString_Dest == ' ')
		{
			pCutPoint			= Param_pString_Dest;
			iCutPointCounter	= iCounter;
		}

		// Increment Position & Size Counters

		Param_pString_Dest++;
		Param_pString_Source++;
	}

	// Add Null Termination Character

	*Param_pString_Dest = 0;
}

// ***********************************************************************************************************************

void CVideoSubtitles::DrawCharacter(float Param_fPositionX, float Param_fPositionY, unsigned char Param_ucCharacter)
{
	// Error Checking

	if ((m_iFont_ID == MAT_Xe_InvalidTextureId) || (Param_ucCharacter < 32))
	{
		return;
	}

	// Font Starts At Char #32 (Space)

	int iFontIndex = (Param_ucCharacter - 32);

	// Compute Letter Position In Font

	float fU = (iFontIndex % kiFont_Letter_Columns) * kfFont_Letter_Width;
	float fV = (iFontIndex / kiFont_Letter_Columns) * kfFont_Letter_Height;

	// Send Letter To Render List

	DrawRectangleEx(Param_fPositionX, Param_fPositionY, Param_fPositionX + kfSubtitle_Letter_Width, Param_fPositionY + kfSubtitle_Letter_Height,
					fU, fV, fU + kfFont_Letter_Width, fV + kfFont_Letter_Height,
					0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0.0f, MAT_Cc_Op_Alpha, m_iFont_ID);
}

// ***********************************************************************************************************************

char* CVideoSubtitles::DrawString(char* Param_pString, float Param_fPositionX, float Param_fPositionY)
{
	while ((*Param_pString != 0) && (*Param_pString != kcText_LineFeed))
	{
		DrawCharacter(Param_fPositionX, Param_fPositionY, *Param_pString);

		Param_fPositionX += kfSubtitle_Letter_Width;

		Param_pString++;
	}

	return Param_pString;
}

// ***********************************************************************************************************************

void CVideoSubtitles::DrawText(char* Param_pString_Source, float Param_pTimeProgress, int Param_iMaxLineCount)
{
	char acStringTempBuffer[2048];

	// Error Checking

	if ((Param_iMaxLineCount <= 0) || (Param_pString_Source == NULL))
	{
		return;
	}

	// Cut Text Into Lines

	char* pString_Temp = acStringTempBuffer;

	FormatString(Param_pString_Source, pString_Temp, 45);

	if (*pString_Temp == 0)
	{
		return;
	}

	// Fast-Forward Through Expired Lines (If Applicable)

	int iTextHeight = GetStringHeight(pString_Temp);

	int iLineOffset = 0;

	while (Param_pTimeProgress  > (float)(iLineOffset + Param_iMaxLineCount) / (float)(iTextHeight + 1))
	{
		int iCounter = Param_iMaxLineCount;

		while (iCounter > 0) 
		{
			pString_Temp = GetNextString(pString_Temp);

			if (*pString_Temp != 0)
			{
				pString_Temp++;
			}

			if (*pString_Temp == 0)
			{
				return;
			}

			iCounter--;
		}

		iLineOffset += Param_iMaxLineCount;
	}

	// Calculate Display Height

	if (iTextHeight + 1 > Param_iMaxLineCount)
	{
		iTextHeight = lMin(GetStringHeight(pString_Temp) + 1, Param_iMaxLineCount);
	}

	float Y = 0.82f;

	// Draw String Lines

	while (*pString_Temp != 0)
	{
		pString_Temp = DrawString(pString_Temp, (1.0f - (kfSubtitle_Letter_Width * GetStringWidth(pString_Temp))) / 2.0f, Y);

		if (*pString_Temp != 0)
		{
			pString_Temp++;
		}

		Param_iMaxLineCount--;

		if (Param_iMaxLineCount == 0)
		{
			return;
		}

		Y += kfSubtitle_Letter_Height;
	}
}

// ***********************************************************************************************************************

void CVideoSubtitles::Draw()
{
	// Error Checking

	if (m_pFileBuffer == NULL)
	{
		return;
	}

	// Fetch Language

	int Language = TEXT_i_GetLang();

	if (!INO_b_LanguageIsPresent(Language))
	{
		Language = INO_e_English;
	}

	// Find Localised Text

	char* pLocalisedText = m_pFileBuffer;

	while ((pLocalisedText[0] != kacLanguageCode[Language][0]) || (pLocalisedText[1] != kacLanguageCode[Language][1]))
	{
		pLocalisedText = FindLanguageCode(pLocalisedText, m_pFileBuffer + m_iFileSize);

		if (pLocalisedText == NULL)
		{
			return;
		}
	}

	// Fetch TimeCode

	char* pDrawLine	= GetNextLine(pLocalisedText, m_pFileBuffer + m_iFileSize);
	pLocalisedText	= GetNextLine(pDrawLine     , m_pFileBuffer + m_iFileSize);
		
	float fCurrentTime	= (float)(GetTickCount() - m_uiStartTime) / (float)1000.0f;
	float fMinimalTime	= 0;

	BOOL bBackgroundShown = FALSE;

	while (pLocalisedText != NULL)
	{
		float fLateTime;

		float fCurrentLineTime	= GetTimeCode(pLocalisedText);
		float fLastLineTime		= GetTimeCode(pDrawLine);

		fMinimalTime	= fMax(fLastLineTime, fMinimalTime);
		fLateTime		= fMinimalTime - fLastLineTime;

		fLastLineTime	+= fLateTime;
		fMinimalTime	+= (float)GetStringLength(pDrawLine + 6) * 0.03f + 0.3f;
		fLateTime		= fMax(fCurrentLineTime, fMinimalTime) - fCurrentLineTime;

		fCurrentLineTime += fLateTime;

		// Investigate Latest Line

		if (fCurrentLineTime > fCurrentTime)
		{
			float fLocalDelta;
			float fMaxTime;

			fLocalDelta	= fCurrentTime - fLastLineTime;
			fMaxTime	= (float)GetStringLength(pDrawLine + 6) * 0.08f + 0.5f;

			// Display If Not Expired

			if (fLocalDelta < fMaxTime)
			{
				// Display Dark Background Bar (If Applicable)

				if (!bBackgroundShown)
				{
					DrawRectangleEx(0.0f, 0.81f, 1.0f, 0.91f, 0.0f, 0.0f, 1.0f, 1.0f, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0.0f, MAT_Cc_Op_Alpha, -1);

					bBackgroundShown = TRUE;
				}

				// Draw Text Lines

				fMaxTime = fMin(fMaxTime, (fCurrentLineTime - fLastLineTime));

				DrawText(pDrawLine + 6, (fCurrentTime - fLastLineTime) / fMaxTime, 2);
			}

			return;
		}

		pDrawLine		= pLocalisedText;
		pLocalisedText	= GetNextLine(pLocalisedText, m_pFileBuffer + m_iFileSize);
	}
}

// ***********************************************************************************************************************

void CVideoSubtitles::LoadFont()
{
	// Open Font File

	HANDLE hFile_Font = CreateFile("GAME:\\Video\\SubtitleFont.raw", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	// Create RAW Buffer

	int iSize_Font = kiFont_Texture_Width * kiFont_Texture_Height;

	void* pBuffer_Font = MEM_p_Alloc(iSize_Font);

	if (pBuffer_Font == NULL)
	{
		CloseHandle(hFile_Font);
		return;
	}

	// Load RAW Data

	int iBytesRead = 0;

	if (!ReadFile(hFile_Font, pBuffer_Font, iSize_Font, (LPDWORD)&iBytesRead, NULL))
	{
		CloseHandle(hFile_Font);
		MEM_Free(pBuffer_Font);
		return;
	}

	CloseHandle(hFile_Font);

	// Create Font Texture

	HRESULT hResult_Font = g_oXeRenderer.GetDevice()->CreateTexture(kiFont_Texture_Width, kiFont_Texture_Height, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_MANAGED, &m_pFont_Texture, NULL);

	if (FAILED(hResult_Font) || (m_pFont_Texture == NULL))
	{
		MEM_Free(pBuffer_Font);
		return;
	}

	// Lock Surface

	D3DLOCKED_RECT stRect_Font;

	hResult_Font = m_pFont_Texture->LockRect(0, &stRect_Font, NULL, 0);

	if (FAILED(hResult_Font))
	{
		MEM_Free(pBuffer_Font);
		return;
	}

	// Fill Pixel Surface

	char* pImage_Font = (char*)pBuffer_Font;

	for (int iLoopY = 0; iLoopY < kiFont_Texture_Height; iLoopY++)
	{
		int* pTextureLine = (int*)stRect_Font.pBits + ((iLoopY * stRect_Font.Pitch) >> 2);

		for (int iLoopX = 0; iLoopX < kiFont_Texture_Width; iLoopX++)
		{
			unsigned int uiColor = *(pImage_Font++);
			*(pTextureLine++) = (uiColor << 24) | (uiColor << 16) | (uiColor << 8) | uiColor;
		}
	}

	// Release Buffer

	MEM_Free(pBuffer_Font);

	// Unlock Surface

	m_pFont_Texture->UnlockRect(0);

	// Add / Update User Texture In Manager

	if (m_iFont_ID == MAT_Xe_InvalidTextureId)
	{
		m_iFont_ID = g_oXeTextureMgr.RegisterUserTexture(m_pFont_Texture);
	}
	else
	{
		g_oXeTextureMgr.UpdateUserTexture(m_iFont_ID, m_pFont_Texture);
	}
}

// ***********************************************************************************************************************

void CVideoSubtitles::Open(const char* Param_VideoFilename)
{
	char* pSubtitleFilename = NULL;

	// Error Checking

	if (Param_VideoFilename == NULL)
	{
		return;
	}

	// Language Check

	if (!IO_gi_SubtitleOn && (TEXT_i_GetLang() == INO_e_English))
	{
		return;
	}

	// Detect Video File

	if (strstr(Param_VideoFilename, "Intro") > 0)
	{
		pSubtitleFilename = "GAME:\\Video\\Intro.txt";
	}

	if (strstr(Param_VideoFilename, "Trailer") > 0)
	{
		pSubtitleFilename = "GAME:\\Video\\Intro.txt";
	}

	if (strstr(Param_VideoFilename, "IntPJ") > 0)
	{
		pSubtitleFilename = "GAME:\\Video\\IntPJ.txt";
	}

	if (strstr(Param_VideoFilename, "IntPB") > 0)
	{
		pSubtitleFilename = "GAME:\\Video\\IntPB.txt";
	}

	if (pSubtitleFilename == NULL)
	{
		return;
	}

	// Read Text File

	HANDLE hFile = CreateFile(pSubtitleFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}

	m_iFileSize		= GetFileSize(hFile, NULL);
	m_pFileBuffer	= (char*)MEM_p_Alloc(m_iFileSize);

	if (m_pFileBuffer == NULL)
	{
		return;
	}

	ReadFile(hFile, m_pFileBuffer, m_iFileSize, (LPDWORD)&m_iFileSize, NULL);

	CloseHandle(hFile);

	RemoveLineFeeds(m_pFileBuffer, m_iFileSize);

	// Load Font (If Applicable)

	LoadFont();

	// Grab Video Start Time

	m_uiStartTime = GetTickCount();
}

// ***********************************************************************************************************************

void CVideoSubtitles::Close()
{
	// Destroy Font Texture

	SAFE_RELEASE(m_pFont_Texture);

	if (m_iFont_ID != MAT_Xe_InvalidTextureId)
	{
		g_oXeTextureMgr.UpdateUserTexture(m_iFont_ID, NULL);
	}

	// Destroy Text Buffer

	if (m_pFileBuffer != NULL)
	{
		MEM_Free(m_pFileBuffer);

		m_pFileBuffer = NULL;
	}
}

// ***********************************************************************************************************************

#endif // _XENON
