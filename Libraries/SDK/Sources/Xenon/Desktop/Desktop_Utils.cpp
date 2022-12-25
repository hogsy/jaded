// ****************************************
// Visual Desktop Windowing Manager
//
// Used for King Kong Xenon Debugging
//
// By Alexandre David (January 2005)
// ****************************************

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "Precomp.h"

#include "Desktop.h"

// ***********************************************************************************************************************

#if defined(DESKTOP_ENABLE)

namespace Desktop
{

// ***********************************************************************************************************************
//    General Purpose
// ***********************************************************************************************************************

void GetStringSize(TFloat* Param_pReturnSizeX, TFloat* Param_pReturnSizeY, const TPWString Param_szText, TFloat Param_fScaleX, TFloat Param_fScaleY, TFontID Param_iFontID, TFloat Param_fClipX, TFloat Param_fClipY)
{
	stFont*	pstFont				= NULL;
	TFloat	fSubString_Width	= 0.0f;
	TFloat	fSubString_Height	= 0.0f;
	TFloat	fString_Width		= 0.0f;
	TFloat	fString_Height		= 0.0f;

	if (Param_pReturnSizeX != NULL)
	{
		*Param_pReturnSizeX = 0.0f;
	}

	if (Param_pReturnSizeY != NULL)
	{
		*Param_pReturnSizeY = 0.0f;
	}

	// Verify Input

	if (Param_szText == NULL)
	{
		return;
	}

	// Scan Through String Characters

	for(TInt iLoop = 0; Param_szText[iLoop] != '\0'; iLoop++)
	{
		TWChar	cCharacterToDisplay	= Param_szText[iLoop    ] & 0xFF;
		TWChar	cNextCharacter		= Param_szText[iLoop + 1] & 0xFF;
		TBool	bNewLine			= FALSE;

		// New Line - "\n"

		if ((cCharacterToDisplay == '\\') && (cNextCharacter == 'n'))
		{
			bNewLine = TRUE;
			iLoop++;
		}

		// New Line - '\n' #10 or #13

		if ((cCharacterToDisplay == 10) || (cCharacterToDisplay == 13))
		{
			bNewLine = TRUE;
		}

		if (bNewLine)
		{
			// Update Largest Sizes

			if (fSubString_Width > fString_Width)
			{
				fString_Width = fSubString_Width;
			}

			if (fSubString_Height > fString_Height)
			{
				fString_Height = fSubString_Height;
			}

			// Increment / Reset Size Counters

			if (Param_pReturnSizeY != NULL)
			{
				*Param_pReturnSizeY += fString_Height;
			}

			fString_Height		= 0.0f;
			fSubString_Width	= 0.0f;
			fSubString_Height	= 0.0f;
		}
		else
		{
			// Fetch Appropriate Font (If Applicable)

			if ((pstFont == NULL) || (Param_iFontID == kiFontID_Dynamic))
			{
				// Fetch Font Structure

				pstFont = g_FontManager.GetFont(Param_iFontID, Param_szText[iLoop]);

				if (pstFont == NULL)
				{
					break;
				}
			}

			// Fetch Texture Coordinates

			TFloat fU0 = pstFont->stDescriptor.astLetter[cCharacterToDisplay].fU0;
			TFloat fU1 = pstFont->stDescriptor.astLetter[cCharacterToDisplay].fU1;
			TFloat fV0 = pstFont->stDescriptor.astLetter[cCharacterToDisplay].fV0;
			TFloat fV1 = pstFont->stDescriptor.astLetter[cCharacterToDisplay].fV1;

			// Handle Clipping - X

			if (fU0 < fU1)
			{
				fU1 = fU0 + ((fU1 - fU0) * Param_fClipX);
			}
			else
			{
				fU0 = fU1 + ((fU0 - fU1) * Param_fClipX);
			}

			// Handle Clipping - Y

			if (fV0 < fV1)
			{
				fV1 = fV0 + ((fV1 - fV0) * Param_fClipY);
			}
			else
			{
				fV0 = fV1 + ((fV0 - fV1) * Param_fClipY);
			}

			// Compute Width & Height (in normalized coordinates)

			TFloat fWidth  = (fabs(fU1 - fU0) * (Param_fScaleX * pstFont->fScale_X));
			TFloat fHeight = (fabs(fV1 - fV0) * (Param_fScaleY * pstFont->fScale_Y));

			// Update Counters

			fSubString_Width += fWidth;

			if (fHeight > fSubString_Height)
			{
				fSubString_Height = fHeight;
			}
		}
	}

	// Update Largest Sizes

	if (fSubString_Width > fString_Width)
	{
		fString_Width = fSubString_Width;
	}

	if (fSubString_Height > fString_Height)
	{
		fString_Height = fSubString_Height;
	}

	// Return Values

	if (Param_pReturnSizeX != NULL)
	{
		*Param_pReturnSizeX += fString_Width;
	}

	if (Param_pReturnSizeY != NULL)
	{
		*Param_pReturnSizeY += fString_Height;
	}
}

// ***********************************************************************************************************************

void DrawString(const TPWString Param_szText, TFloat Param_fXPos, TFloat Param_fYPos, TFloat Param_fScaleX, TFloat Param_fScaleY, TInt Param_iColor, eBlending Param_eBlending, TFontID Param_iFontID, TFloat Param_fClipX, TFloat Param_fClipY)
{
	stFont* pstFont = NULL;

	// Verify Input

	if (Param_szText == NULL)
	{
		return;
	}

	// Position Text

	TFloat fCurrentPosition_X	= Param_fXPos;
	TFloat fCurrentPosition_Y	= Param_fYPos;
	TFloat fHighestCharacter	= 0.0f;

	for(TInt iLoop = 0; Param_szText[iLoop] != '\0'; iLoop++)
	{
		TWChar	cCharacterToDisplay	= Param_szText[iLoop    ] & 0xFF;
		TWChar	cNextCharacter		= Param_szText[iLoop + 1] & 0xFF;
		TBool	bNewLine			= FALSE;

		// New Line - "\n"

		if ((cCharacterToDisplay == '\\') && (cNextCharacter == 'n'))
		{
			bNewLine = TRUE;
			iLoop++;
		}

		// New Line - '\n' #10 or #13

		if ((cCharacterToDisplay == 10) || (cCharacterToDisplay == 13))
		{
			bNewLine = TRUE;
		}

		if (bNewLine)
		{
			fCurrentPosition_X	= Param_fXPos;
			fCurrentPosition_Y	+= fHighestCharacter;
			fHighestCharacter	= 0.0f;
		}
		else
		{
			// Fetch Appropriate Font (If Applicable)

			if ((pstFont == NULL) || (Param_iFontID == kiFontID_Dynamic))
			{
				// Fetch Font Structure

				pstFont = g_FontManager.GetFont(Param_iFontID, Param_szText[iLoop]);

				if (pstFont == NULL)
				{
					break;
				}
			}

			// Fetch Texture Coordinates

			TFloat fU0 = pstFont->stDescriptor.astLetter[cCharacterToDisplay].fU0;
			TFloat fU1 = pstFont->stDescriptor.astLetter[cCharacterToDisplay].fU1;
			TFloat fV0 = pstFont->stDescriptor.astLetter[cCharacterToDisplay].fV0;
			TFloat fV1 = pstFont->stDescriptor.astLetter[cCharacterToDisplay].fV1;

			// Handle Clipping - X

			if (fU0 < fU1)
			{
				fU1 = fU0 + ((fU1 - fU0) * Param_fClipX);
			}
			else
			{
				fU0 = fU1 + ((fU0 - fU1) * Param_fClipX);
			}

			// Handle Clipping - Y

			if (fV0 < fV1)
			{
				fV1 = fV0 + ((fV1 - fV0) * Param_fClipY);
			}
			else
			{
				fV0 = fV1 + ((fV0 - fV1) * Param_fClipY);
			}

			// Compute Width & Height (in normalized coordinates)

			TFloat fWidth  = (fabs(fU1 - fU0) * (Param_fScaleX * pstFont->fScale_X));
			TFloat fHeight = (fabs(fV1 - fV0) * (Param_fScaleY * pstFont->fScale_Y));

			// Submit Quad To Rendering

			Rendering_DrawRectangle(fCurrentPosition_X, fCurrentPosition_Y, fWidth, fHeight, fU0, fV0, fU1, fV1, Param_iColor, Param_eBlending, pstFont->iTextureID);

			fCurrentPosition_X += fWidth;

			if (fHeight > fHighestCharacter)
			{
				fHighestCharacter = fHeight;
			}
		}
	}
}

// ***********************************************************************************************************************

void IntegerToString(TInt Param_iValue, TPWString Param_pDestBuffer, TBool Param_bAddCommas, TBool Param_bDisplayAsPercent, TInt Param_iDigitCount)
{
	TWChar	acBuffer_Original[32];
	TWChar	acBuffer_WithCommas[32];
	TWChar	acControlString[32];
	TWChar*	pcCopyPointer;

	// Error Checking

	if (Param_pDestBuffer == NULL)
	{
		return;
	}

	// Create Control String

	if (Param_bDisplayAsPercent)
	{
		swprintf(acControlString, L"%%0%ii%%%%", Param_iDigitCount);
	}
	else
	{
		swprintf(acControlString, L"%%0%ii", Param_iDigitCount);
	}

	// Apply Control String

	swprintf(acBuffer_Original, acControlString, Param_iValue);

	pcCopyPointer = acBuffer_Original;

	// Add Commas (If Applicable)

	if (Param_bAddCommas)
	{
		TInt iLength_Original	= wcslen(acBuffer_Original);
		TInt iLength_WithCommas	= iLength_Original + ((iLength_Original - ((Param_iValue < 0) ? 2 : 1) - (Param_bDisplayAsPercent ? 1 : 0)) / 3);

		acBuffer_WithCommas[iLength_WithCommas] = 0;

		TInt iSizeCounter = Param_bDisplayAsPercent ? -1 : 0;

		while (iLength_Original > 0)
		{
			iSizeCounter++;

			if ((iSizeCounter > 3) && ((iLength_Original > 1) || (Param_iValue >= 0)))
			{
				iLength_WithCommas--;
				acBuffer_WithCommas[iLength_WithCommas] = ',';
				iSizeCounter -= 3;
			}

			iLength_Original--;
			iLength_WithCommas--;

			acBuffer_WithCommas[iLength_WithCommas] = acBuffer_Original[iLength_Original];
		}

		pcCopyPointer = acBuffer_WithCommas;
	}

	// Copy To Destination Buffer

	wcscpy(Param_pDestBuffer, pcCopyPointer);
}

// ***********************************************************************************************************************

} // Namespace

// ***********************************************************************************************************************

#endif // DESKTOP_ENABLE
