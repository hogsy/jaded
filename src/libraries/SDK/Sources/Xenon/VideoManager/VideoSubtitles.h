// ************************************************
// Video Subtitles Manager
//
// Used for King Kong Xenon
// By Alexandre David (October 2005)
// ************************************************

#ifndef __VideoSubtitles_h__
#define __VideoSubtitles_h__

#if defined(_XENON)

// ***********************************************************************************************************************
//    Constants
// ***********************************************************************************************************************

const char	kcText_LineFeed				= 10;

const int	kiFont_Letter_Columns		= 32;
const int	kiFont_Letter_Rows			= 7;
const int	kiFont_Texture_Width		= 1024;
const int	kiFont_Texture_Height		= 512;

const float	kfFont_Letter_Width			= (30.0f / (float)kiFont_Texture_Width);
const float	kfFont_Letter_Height		= (59.0f / (float)kiFont_Texture_Height);

const float	kfSubtitle_Letter_Width		= 0.015f;
const float	kfSubtitle_Letter_Height	= 0.04f;

// ***********************************************************************************************************************
//    Classes
// ***********************************************************************************************************************

class CVideoSubtitles
{
private:
	int					m_iFileSize;
	char*				m_pFileBuffer;
	int					m_iFont_ID;
	IDirect3DTexture9*	m_pFont_Texture;
	unsigned int		m_uiStartTime;

	char*	GetNextString(char* Param_pString);
	int		GetStringLength(char* Param_pTextBuffer);
	int		GetStringWidth(char* Param_pString);
	int		GetStringHeight(char* Param_pString);
	char*	GetNextLine(char* Param_pTextBuffer, char* Param_pTextBufferEnd);
	float	GetTimeCode(char* Param_pString);
	char*	FindLanguageCode(char* Param_pTextBuffer, char* Param_pTextBufferEnd);
	void	RemoveLineFeeds(char* Param_pTextBuffer, int Param_iSize);
	void	FormatString(char* Param_pString_Source, char* Param_pString_Dest, int Param_iMaxLineLength);
	void	DrawCharacter(float Param_fPositionX, float Param_fPositionY, unsigned char Param_ucCharacter);
	char*	DrawString(char* Param_pString, float Param_fPositionX, float Param_fPositionY);
	void	DrawText(char* Param_pString_Source, float Param_pTimeProgress, int Param_iMaxLineCount);
	void	LoadFont();

public:
	CVideoSubtitles();
	~CVideoSubtitles();

	void	Draw();
	void	Open(const char* Param_VideoFilename);
	void	Close();
};

// ***********************************************************************************************************************

#endif // _XENON

#endif // __VideoSubtitles_h__
