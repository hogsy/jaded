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
//    Global Variables
// ***********************************************************************************************************************

CFontManager g_FontManager;

// ***********************************************************************************************************************
//    Class Methods : CText
// ***********************************************************************************************************************

CText::CText()
{
	m_pBuffer			= NULL;
	m_iCurrentLength	= 0;
	m_iMaximumLength	= 0;
}

// ***********************************************************************************************************************

CText::CText(TPWString Param_pString)
{
	m_pBuffer			= NULL;
	m_iCurrentLength	= 0;
	m_iMaximumLength	= 0;

	SetTo(Param_pString);
}

// ***********************************************************************************************************************

CText::CText(TPString Param_pString)
{
	m_pBuffer			= NULL;
	m_iCurrentLength	= 0;
	m_iMaximumLength	= 0;

	SetTo(Param_pString);
}

// ***********************************************************************************************************************

CText::CText(CText* Param_pText)
{
	m_pBuffer			= NULL;
	m_iCurrentLength	= 0;
	m_iMaximumLength	= 0;

	SetTo(Param_pText);
}

// ***********************************************************************************************************************

CText::~CText()
{
	Clear(FALSE);
}

// ***********************************************************************************************************************

void CText::Clear(TBool Param_bPreserveBuffer)
{
	if (!Param_bPreserveBuffer)
	{
		if (m_pBuffer != NULL)
		{
			Memory_Free(m_pBuffer);
			m_pBuffer = NULL;
		}

		m_iMaximumLength = 0;
	}
	else
	{
		if (m_pBuffer != NULL)
		{
			m_pBuffer[0] = 0;
		}
	}

	m_iCurrentLength = 0;
}

// ***********************************************************************************************************************

void CText::Resize(TInt Param_iSize, TBool Param_bCopyPrevious)
{
	// Clear On Size 0

	if (Param_iSize <= 0)
	{
		Clear();
		return;
	}

	// Resize Only If Requested Size Is Larger Than Current Maximum

	if (Param_iSize <= m_iMaximumLength)
	{
		return;
	}

	// Find New Size (Power of 2)

	if (m_iMaximumLength <= 0)
	{
		m_iMaximumLength = 8;
	}

	while (m_iMaximumLength < Param_iSize)
	{
		m_iMaximumLength *= 2;
	}

	// Backup Old Buffer

	TPWString pOldBuffer = m_pBuffer;

	// Allocate New Buffer

	m_pBuffer = (TPWString)Memory_Allocate((m_iMaximumLength + 1) * sizeof(TWChar));

	// Copy Previous Buffer Including '\0' (If Applicable)

	if (pOldBuffer != NULL)
	{
		if (Param_bCopyPrevious)
		{
			for (TInt Loop = 0; Loop < m_iCurrentLength; Loop++)
			{
				m_pBuffer[Loop] = pOldBuffer[Loop];
			}
		}

		Memory_Free(pOldBuffer);
	}

	// Insert Zero Character

	m_pBuffer[m_iCurrentLength]	= 0;
}

// ***********************************************************************************************************************

void CText::SetTo(TPWString Param_pString)
{
	if (Param_pString != NULL)
	{
		TInt Length = wcslen(Param_pString);

		if (Length > 0)
		{
			Resize(Length, FALSE);

			for (TInt Loop = 0; Loop < Length; Loop++)
			{
				m_pBuffer[Loop] = Param_pString[Loop];
			}

			m_iCurrentLength			= Length;
			m_pBuffer[m_iCurrentLength]	= 0;
		}
	}
	else
	{
		Clear();
	}
}

// ***********************************************************************************************************************

void CText::SetTo(TPString Param_pString)
{
	if (Param_pString != NULL)
	{
		TInt Length = strlen(Param_pString);

		Resize(Length, FALSE);

		for (TInt Loop = 0; Loop < Length; Loop++)
		{
			m_pBuffer[Loop] = Param_pString[Loop];
		}

		m_iCurrentLength			= Length;
		m_pBuffer[m_iCurrentLength]	= 0;
	}
	else
	{
		Clear();
	}
}

// ***********************************************************************************************************************

void CText::SetTo(CText* Param_pText)
{
	if (Param_pText == NULL)
	{
		SetTo((TPWString)NULL);
	}
	else
	{
		SetTo(Param_pText->GetString());
	}
}

// ***********************************************************************************************************************

void CText::Append(TPWString Param_pString)
{
	if (Param_pString != NULL)
	{
		TInt AppendLength = wcslen(Param_pString);

		Resize(m_iCurrentLength + AppendLength, TRUE);

		// Append New Buffer

		for (TInt Loop = 0; Loop < AppendLength; Loop++)
		{
			m_pBuffer[m_iCurrentLength + Loop] = Param_pString[Loop];
		}

		// Adjust Fields

		m_iCurrentLength += AppendLength;
		m_pBuffer[m_iCurrentLength] = 0;
	}
}

// ***********************************************************************************************************************

void CText::Append(TPString Param_pString)
{
	if (Param_pString != NULL)
	{
		TInt AppendLength = strlen(Param_pString);

		Resize(m_iCurrentLength + AppendLength, TRUE);

		// Append New Buffer

		for (TInt Loop = 0; Loop < AppendLength; Loop++)
		{
			m_pBuffer[m_iCurrentLength + Loop] = Param_pString[Loop];
		}

		// Adjust Fields

		m_iCurrentLength += AppendLength;
		m_pBuffer[m_iCurrentLength] = 0;
	}
}

// ***********************************************************************************************************************

void CText::Append(CText* Param_pText)
{
	if (Param_pText != NULL)
	{
		Append(Param_pText->GetString());
	}
}

// ***********************************************************************************************************************

inline TBool CText::IsEmpty()
{
	return (m_iCurrentLength == 0);
}

// ***********************************************************************************************************************

inline TPWString CText::GetString()
{
	return m_pBuffer;
}

// ***********************************************************************************************************************

inline TInt CText::GetCurrentLength()
{
	return m_iCurrentLength;
}

// ***********************************************************************************************************************

inline TInt CText::GetMaximumLength()
{
	return m_iMaximumLength;
}

// ***********************************************************************************************************************
//    Class Methods : CFloat
// ***********************************************************************************************************************

CFloat::CFloat()
{
	Set(0.0f);
}

// ***********************************************************************************************************************

CFloat::CFloat(TFloat Param_fCurrent)
{
	Set(Param_fCurrent);
}

// ***********************************************************************************************************************

CFloat::~CFloat()
{
}

// ***********************************************************************************************************************

TBool CFloat::IsChanging()
{
	return (m_iInterpolateSign != 0);
}

// ***********************************************************************************************************************

void CFloat::Set(TFloat Param_fNewCurrent)
{
	m_iInterpolateSign	= 0;
	m_fCurrent			= Param_fNewCurrent;
	m_fTarget			= Param_fNewCurrent;
	m_fMSecValueDelta	= 0.0f;
}

// ***********************************************************************************************************************

void CFloat::InterpolateTo(TFloat Param_fNewTarget, TFloat Param_fMSecValueDelta)
{
	m_iInterpolateSign	= (int)((Param_fNewTarget - m_fCurrent) * 1000.0f);
	m_fTarget			= Param_fNewTarget;
	m_fMSecValueDelta	= Param_fMSecValueDelta;
}

// ***********************************************************************************************************************

inline TFloat CFloat::Get()
{
	return m_fCurrent;
}

// ***********************************************************************************************************************

void CFloat::Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged)
{
	if (m_iInterpolateSign != 0)
	{
		if (m_iInterpolateSign > 0)
		{
			m_fCurrent += (m_fMSecValueDelta * Param_fMSecTimeDelta);

			if (m_fCurrent >= m_fTarget)
			{
				m_fCurrent			= m_fTarget;
				m_iInterpolateSign	= 0;
			}
		}
		else
		{
			m_fCurrent -= (m_fMSecValueDelta * Param_fMSecTimeDelta);

			if (m_fCurrent <= m_fTarget)
			{
				m_fCurrent			= m_fTarget;
				m_iInterpolateSign	= 0;
			}
		}
	}
}

// ***********************************************************************************************************************
//    Class Methods : CInteger
// ***********************************************************************************************************************

CInteger::CInteger()
{
	Set(0);
}

// ***********************************************************************************************************************

CInteger::CInteger(TInt Param_iCurrent)
{
	Set(Param_iCurrent);
}

// ***********************************************************************************************************************

CInteger::~CInteger()
{
}

// ***********************************************************************************************************************

TBool CInteger::IsChanging()
{
	return (m_iCurrent != m_iTarget);
}

// ***********************************************************************************************************************

void CInteger::Set(TInt Param_iNewCurrent)
{
	m_iCurrent			= Param_iNewCurrent;
	m_iTarget			= Param_iNewCurrent;
	m_iMSecValueDelta	= 0;
}

// ***********************************************************************************************************************

void CInteger::InterpolateTo(TInt Param_iNewTarget, TInt Param_iMSecValueDelta)
{
	m_iTarget			= Param_iNewTarget;
	m_iMSecValueDelta	= Param_iMSecValueDelta;
}

// ***********************************************************************************************************************

inline TInt CInteger::Get()
{
	return m_iCurrent;
}

// ***********************************************************************************************************************

void CInteger::Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged)
{
	if (m_iCurrent != m_iTarget)
	{
		// Calculate Interpolation Speed (Minimum 1)

		TInt Speed = (TInt)(m_iMSecValueDelta * Param_fMSecTimeDelta);

		if (Speed <= 0)
		{
			Speed = 1;
		}

		// Update Value

		if (m_iCurrent < m_iTarget)
		{
			m_iCurrent += Speed;

			if (m_iCurrent > m_iTarget)
			{
				m_iCurrent = m_iTarget;
			}
		}
		else
		{
			m_iCurrent -= Speed;

			if (m_iCurrent < m_iTarget)
			{
				m_iCurrent = m_iTarget;
			}
		}
	}
}

// ***********************************************************************************************************************
//    Class Methods : CBoolean
// ***********************************************************************************************************************

CBoolean::CBoolean()
{
	Set(FALSE);
}

// ***********************************************************************************************************************

CBoolean::CBoolean(TBool Param_bCurrent)
{
	Set(Param_bCurrent);
}

// ***********************************************************************************************************************

CBoolean::~CBoolean()
{
}

// ***********************************************************************************************************************

inline void CBoolean::Set(TBool Param_bNewCurrent)
{
	m_bCurrent = Param_bNewCurrent;
}

// ***********************************************************************************************************************

inline TBool CBoolean::Get()
{
	return m_bCurrent;
}

// ***********************************************************************************************************************
//    Class Methods : CPoint
// ***********************************************************************************************************************

CPoint::CPoint()
{
}

// ***********************************************************************************************************************

CPoint::CPoint(TFloat Param_fCurrentX, TFloat Param_fCurrentY)
{
	Set(Param_fCurrentX, Param_fCurrentY);
}

// ***********************************************************************************************************************

CPoint::~CPoint()
{
}

// ***********************************************************************************************************************

void CPoint::Set(TFloat Param_fNewCurrentX, TFloat Param_fNewCurrentY)
{
	m_X.Set(Param_fNewCurrentX);
	m_Y.Set(Param_fNewCurrentY);
}

// ***********************************************************************************************************************

void CPoint::MoveTo(TFloat Param_fNewTargetX, TFloat Param_fNewTargetY, TFloat Param_fMSecValueDelta)
{
	m_X.InterpolateTo(Param_fNewTargetX, Param_fMSecValueDelta);
	m_Y.InterpolateTo(Param_fNewTargetY, Param_fMSecValueDelta);
}

// ***********************************************************************************************************************

inline CFloat* CPoint::Get_X()
{
	return &m_X;
}

// ***********************************************************************************************************************

inline CFloat* CPoint::Get_Y()
{
	return &m_Y;
}

// ***********************************************************************************************************************

inline TFloat CPoint::GetValue_X()
{
	return m_X.Get();
}

// ***********************************************************************************************************************

inline TFloat CPoint::GetValue_Y()
{
	return m_Y.Get();
}

// ***********************************************************************************************************************

void CPoint::Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged)
{
	m_X.Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);
	m_Y.Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);
}

// ***********************************************************************************************************************
//    Class Methods : CColor
// ***********************************************************************************************************************

CColor::CColor()
{
}

// ***********************************************************************************************************************

CColor::CColor(TFloat Param_fCurrentAlpha, TFloat Param_fCurrentRed, TFloat Param_fCurrentGreen, TFloat Param_fCurrentBlue)
{
	Set(Param_fCurrentAlpha, Param_fCurrentRed, Param_fCurrentGreen, Param_fCurrentBlue);
}

// ***********************************************************************************************************************

CColor::CColor(TARGBColor Param_iCurrentARGB)
{
	Set(Param_iCurrentARGB);
}

// ***********************************************************************************************************************

CColor::~CColor()
{
}

// ***********************************************************************************************************************

void CColor::Set(TFloat Param_fNewCurrentAlpha, TFloat Param_fNewCurrentRed, TFloat Param_fNewCurrentGreen, TFloat Param_fNewCurrentBlue)
{
	m_Alpha.Set(Param_fNewCurrentAlpha);
	m_Red.Set  (Param_fNewCurrentRed);
	m_Green.Set(Param_fNewCurrentGreen);
	m_Blue.Set (Param_fNewCurrentBlue);
}

// ***********************************************************************************************************************

void CColor::Set(TFloat Param_fNewCurrentRed, TFloat Param_fNewCurrentGreen, TFloat Param_fNewCurrentBlue)
{
	m_Red.Set  (Param_fNewCurrentRed);
	m_Green.Set(Param_fNewCurrentGreen);
	m_Blue.Set (Param_fNewCurrentBlue);
}

// ***********************************************************************************************************************

void CColor::Set(TFloat Param_fNewCurrentAlpha)
{
	m_Alpha.Set(Param_fNewCurrentAlpha);
}

// ***********************************************************************************************************************

void CColor::Set(TARGBColor Param_iNewCurrentARGB)
{
	TInt Component;

	Component = (Param_iNewCurrentARGB >> 24) & 0xFF;
	m_Alpha.Set((TFloat)Component / 255.0f);

	Component = (Param_iNewCurrentARGB >> 16) & 0xFF;
	m_Red.Set((TFloat)Component / 255.0f);

	Component = (Param_iNewCurrentARGB >> 8) & 0xFF;
	m_Green.Set((TFloat)Component / 255.0f);

	Component = Param_iNewCurrentARGB & 0xFF;
	m_Blue.Set((TFloat)Component / 255.0f);
}

// ***********************************************************************************************************************

void CColor::FadeTo(TFloat Param_fNewTargetAlpha, TFloat Param_fNewTargetRed, TFloat Param_fNewTargetGreen, TFloat Param_fNewTargetBlue, TFloat Param_fMSecValueDelta)
{
	m_Alpha.InterpolateTo(Param_fNewTargetAlpha, Param_fMSecValueDelta);
	m_Red.InterpolateTo  (Param_fNewTargetRed  , Param_fMSecValueDelta);
	m_Green.InterpolateTo(Param_fNewTargetGreen, Param_fMSecValueDelta);
	m_Blue.InterpolateTo (Param_fNewTargetBlue , Param_fMSecValueDelta);
}

// ***********************************************************************************************************************

void CColor::FadeTo(TFloat Param_fNewTargetRed, TFloat Param_fNewTargetGreen, TFloat Param_fNewTargetBlue, TFloat Param_fMSecValueDelta)
{
	m_Red.InterpolateTo  (Param_fNewTargetRed  , Param_fMSecValueDelta);
	m_Green.InterpolateTo(Param_fNewTargetGreen, Param_fMSecValueDelta);
	m_Blue.InterpolateTo (Param_fNewTargetBlue , Param_fMSecValueDelta);
}

// ***********************************************************************************************************************

void CColor::FadeTo(TFloat Param_fNewTargetAlpha, TFloat Param_fMSecValueDelta)
{
	m_Alpha.InterpolateTo(Param_fNewTargetAlpha, Param_fMSecValueDelta);
}

// ***********************************************************************************************************************

void CColor::FadeTo(TARGBColor Param_iNewTargetARGB, TFloat Param_fMSecValueDelta)
{
	TInt Component;

	Component = (Param_iNewTargetARGB >> 24) & 0xFF;
	m_Alpha.InterpolateTo((TFloat)Component / 255.0f, Param_fMSecValueDelta);

	Component = (Param_iNewTargetARGB >> 16) & 0xFF;
	m_Red.InterpolateTo((TFloat)Component / 255.0f, Param_fMSecValueDelta);

	Component = (Param_iNewTargetARGB >> 8) & 0xFF;
	m_Green.InterpolateTo((TFloat)Component / 255.0f, Param_fMSecValueDelta);

	Component = Param_iNewTargetARGB & 0xFF;
	m_Blue.InterpolateTo((TFloat)Component / 255.0f, Param_fMSecValueDelta);
}

// ***********************************************************************************************************************

inline CFloat* CColor::Get_Alpha()
{
	return &m_Alpha;
}

// ***********************************************************************************************************************

inline CFloat* CColor::Get_Red()
{
	return &m_Red;
}

// ***********************************************************************************************************************

inline CFloat* CColor::Get_Green()
{
	return &m_Green;
}

// ***********************************************************************************************************************

inline CFloat* CColor::Get_Blue()
{
	return &m_Blue;
}

// ***********************************************************************************************************************

inline TFloat CColor::GetValue_Alpha()
{
	return m_Alpha.Get();
}

// ***********************************************************************************************************************

inline TFloat CColor::GetValue_Red()
{
	return m_Red.Get();
}

// ***********************************************************************************************************************

inline TFloat CColor::GetValue_Green()
{
	return m_Green.Get();
}

// ***********************************************************************************************************************

inline TFloat CColor::GetValue_Blue()
{
	return m_Blue.Get();
}

// ***********************************************************************************************************************

TARGBColor CColor::Get_ARGB()
{
	return FloatToARGB(GetValue_Alpha(), GetValue_Red(), GetValue_Green(), GetValue_Blue());
}

// ***********************************************************************************************************************

void CColor::Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged)
{
	m_Alpha.Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);
	m_Red.Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);
	m_Green.Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);
	m_Blue.Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);
}
// ***********************************************************************************************************************

TARGBColor CColor::FloatToARGB(TFloat Param_fAlpha, TFloat Param_fRed, TFloat Param_fGreen, TFloat Param_fBlue)
{
	return(((TInt)(Param_fAlpha * 0xFF) << 24) | ((TInt)(Param_fRed * 0xFF) << 16) | ((TInt)(Param_fGreen * 0xFF) << 8) | (TInt)(Param_fBlue * 0xFF));
}

// ***********************************************************************************************************************
//    Class Methods : CFontManager
// ***********************************************************************************************************************

CFontManager::CFontManager()
{
	m_pstElements	= NULL;
	m_iCount		= 0;
}

// ***********************************************************************************************************************

CFontManager::~CFontManager()
{
	Shutdown();
}

// ***********************************************************************************************************************

void CFontManager::Initialize()
{
	// Ensure Non-Initialization

	if (m_pstElements != NULL)
	{
		return;
	}

	// Create Basic Fonts

	CreateFont(kiFontID_Native        , kiEngineResID_Texture_Native       , kiEngineResID_Texture_Native       , kfFont_Scale_X_Native       , kfFont_Scale_Y_Native       );
	CreateFont(kiFontID_Desktop_Text  , kiEngineResID_Texture_DesktopText  , kiEngineResID_Texture_DesktopText  , kfFont_Scale_X_DesktopText  , kfFont_Scale_Y_DesktopText  );
	CreateFont(kiFontID_Desktop_Glyphs, kiEngineResID_Texture_DesktopGlyphs, kiEngineResID_Texture_DesktopGlyphs, kfFont_Scale_X_DesktopGlyphs, kfFont_Scale_Y_DesktopGlyphs);
}

// ***********************************************************************************************************************

void CFontManager::Shutdown()
{
	Clear();
}

// ***********************************************************************************************************************

stFont* CFontManager::GetFont(TFontID Param_iFontID, TWChar Param_cCharacter)
{
	// Error Checking

	if (m_pstElements == NULL)
	{
		return NULL;
	}

	// Handling Dynamic FontID

	if (Param_iFontID == kiFontID_Dynamic)
	{
		Param_iFontID = (Param_cCharacter >> 8);
	}

	// Scan All Fonts

	for (TInt iLoop = 0; iLoop < m_iCount; iLoop++)
	{
		if (m_pstElements[iLoop].iUniqueID == Param_iFontID)
		{
			// Update Engine-Specific Data (If Applicable)

			if (m_pstElements[iLoop].iTextureID == kiTextureID_None)
			{
				// Fetch Texture Index

				m_pstElements[iLoop].iTextureID = Texture_GetIndexFromID(m_pstElements[iLoop].iResID_Texture);

				// Populate Font Descriptor

				Rendering_PopulateFontDescriptor(&m_pstElements[iLoop].stDescriptor, m_pstElements[iLoop].iResID_Descriptor);

				// Return Native Fallback Font When Not Loaded (If Possible)

				if ((m_pstElements[iLoop].iTextureID == kiTextureID_None) || !m_pstElements[iLoop].stDescriptor.bIsPopulated)
				{
					if (Param_iFontID != kiFontID_Native)
					{
						return GetFont(kiFontID_Native);
					}
				}
			}

			return &m_pstElements[iLoop];
		}
	}

	return NULL;
}

// ***********************************************************************************************************************

void CFontManager::CreateFont(TFontID Param_iFontID, TEngineResID Param_iDescriptorID, TEngineResID Param_iTextureID, TFloat Param_fScaleX, TFloat Param_fScaleY)
{
	stFont*	pstNewElement;
	stFont*	pstOldList;

	// Return If Font Already Exists

	if (GetFont(Param_iFontID) != NULL)
	{
		return;
	}

	// Backup Previous List

	pstOldList = m_pstElements;

	// Allocate New List

	m_pstElements = (stFont*)Memory_Allocate((m_iCount + 1) * sizeof(stFont));

	// Transfer Old List To New List (If Applicable)

	if (pstOldList != NULL)
	{
		// Copy Pointers

		for (TInt iLoop = 0; iLoop < m_iCount; iLoop++)
		{
			m_pstElements[iLoop] = pstOldList[iLoop];
		}

		// Free Old List

		Memory_Free(pstOldList);
	}

	// Add New Element To List

	pstNewElement = &m_pstElements[m_iCount];
	m_iCount++;

	pstNewElement->iUniqueID					= Param_iFontID;
	pstNewElement->iResID_Descriptor			= Param_iDescriptorID;
	pstNewElement->iResID_Texture				= Param_iTextureID;
	pstNewElement->iTextureID					= kiTextureID_None;
	pstNewElement->fScale_X 					= Param_fScaleX;
	pstNewElement->fScale_Y 					= Param_fScaleY;
	pstNewElement->stDescriptor.bIsPopulated	= FALSE;
}

// ***********************************************************************************************************************

void CFontManager::Clear()
{
	if (m_pstElements != NULL)
	{
		Memory_Free(m_pstElements);

		m_pstElements = NULL;
	}

	m_iCount = 0;
}

// ***********************************************************************************************************************

void CFontManager::InvalidateTextures()
{
	if (m_pstElements == NULL)
	{
		return;
	}

	for (TInt iLoop = 0; iLoop < m_iCount; iLoop++)
	{
		m_pstElements[iLoop].iTextureID = kiTextureID_None;
	}
}

// ***********************************************************************************************************************
//    Class Methods : CMessageMaker
// ***********************************************************************************************************************

CMessageMaker::CMessageMaker()
{
	m_bIsLocked		= FALSE;
	m_fSecTimeStamp	= 0.0f;
}

// ***********************************************************************************************************************

CMessageMaker::~CMessageMaker()
{
}

// ***********************************************************************************************************************

TBool CMessageMaker::IsLocked()
{
	return m_bIsLocked;
}

// ***********************************************************************************************************************

void CMessageMaker::Tick(TFloat Param_fSecTimeDelta, TBool Param_bWorldHasChanged)
{
	m_fSecTimeStamp += Param_fSecTimeDelta;
}

// ***********************************************************************************************************************

TPWString CMessageMaker::Make(TPWString Param_pString, TPWString Param_pModuleName)
{
	m_bIsLocked = TRUE;

	m_LastMessage.Clear(TRUE);

	// Append TimeStamp

	TWChar TimeBuffer[16];

	TInt iMinutes	= (TInt)(m_fSecTimeStamp / 60.0f);
	TInt iSeconds	= (TInt)(m_fSecTimeStamp - (iMinutes * 60.0f));
	TInt iTenths	= (TInt)m_fSecTimeStamp;
	iTenths			= (TInt)((m_fSecTimeStamp - (TFloat)iTenths) * 10.0f);

	swprintf(TimeBuffer, L"%i:%02i.%i ", iMinutes, iSeconds, iTenths);

	m_LastMessage.Append(TimeBuffer);

	// Append Module Name (If Applicable)

	if (Param_pModuleName != NULL)
	{
		m_LastMessage.Append(Param_pModuleName);
		m_LastMessage.Append(L"> ");
	}

	// Append Message

	m_LastMessage.Append(Param_pString);

	// Append Parameters (If Applicable)

	if (!m_Parameters.IsEmpty())
	{
		m_LastMessage.Append(L" (");
		m_LastMessage.Append(&m_Parameters);
		m_LastMessage.Append(L")");
	}

	// Clear Parameter List

	m_Parameters.Clear(TRUE);

	m_bIsLocked = FALSE;

	return m_LastMessage.GetString();
}

// ***********************************************************************************************************************

void CMessageMaker::Clear()
{
	m_LastMessage.Clear();
	m_Parameters.Clear();
}

// ***********************************************************************************************************************

void CMessageMaker::AddParameter(TPWString Param_pValue, TPWString Param_pName)
{
	m_bIsLocked = TRUE;

	// Concatenate ", " to List (If Applicable)

	if (!m_Parameters.IsEmpty())
	{
		m_Parameters.Append(L", ");
	}

	// Concatenate Parameter Name to List (If Applicable)

	if (Param_pName != NULL)
	{
		m_Parameters.Append(Param_pName);
		m_Parameters.Append(L"=");
	}

	// Concatenate Parameter Value to List (If Applicable)

	m_Parameters.Append(L"\"");

	if (Param_pValue != NULL)
	{
		m_Parameters.Append(Param_pValue);
	}

	m_Parameters.Append(L"\"");
}

// ***********************************************************************************************************************

void CMessageMaker::AddParameter(TInt Param_iValue, TPWString Param_pName, TBool Param_bDisplayAsHex)
{
	m_bIsLocked = TRUE;

	// Concatenate ", " to List (If Applicable)

	if (!m_Parameters.IsEmpty())
	{
		m_Parameters.Append(L", ");
	}

	// Concatenate Parameter Name to List (If Applicable)

	if (Param_pName != NULL)
	{
		m_Parameters.Append(Param_pName);
		m_Parameters.Append(L"=");
	}

	// Concatenate Parameter Value to List

	TWChar IntBuffer[16];

	if (Param_bDisplayAsHex)
	{
		swprintf(IntBuffer, L"x%08x", Param_iValue);
	}
	else
	{
		swprintf(IntBuffer, L"%li", Param_iValue);
	}

	m_Parameters.Append(IntBuffer);
}

// ***********************************************************************************************************************

void CMessageMaker::AddParameter(TFloat Param_fValue, TPWString Param_pName)
{
	m_bIsLocked = TRUE;

	// Concatenate ", " to List (If Applicable)

	if (!m_Parameters.IsEmpty())
	{
		m_Parameters.Append(L", ");
	}

	// Concatenate Parameter Name to List (If Applicable)

	if (Param_pName != NULL)
	{
		m_Parameters.Append(Param_pName);
		m_Parameters.Append(L"=");
	}

	// Concatenate Parameter Value to List

	TWChar FloatBuffer[16];

	swprintf(FloatBuffer, L"%.2f", Param_fValue);

	m_Parameters.Append(FloatBuffer);
}

// ***********************************************************************************************************************

void CMessageMaker::AddParameter(TBool Param_bValue, TPWString Param_pName)
{
	m_bIsLocked = TRUE;

	// Concatenate ", " to List (If Applicable)

	if (!m_Parameters.IsEmpty())
	{
		m_Parameters.Append(L", ");
	}

	// Concatenate Parameter Name to List (If Applicable)

	if (Param_pName != NULL)
	{
		m_Parameters.Append(Param_pName);
		m_Parameters.Append(L"=");
	}

	// Concatenate Parameter Value to List

	if (Param_bValue)
	{
		m_Parameters.Append(L"True");
	}
	else
	{
		m_Parameters.Append(L"False");
	}
}

// ***********************************************************************************************************************
//    Class Methods : CConfiguration
// ***********************************************************************************************************************

CConfiguration::CConfiguration()
{
	m_iOptionCount	= 0;
	m_pOptions		= NULL;
}

// ***********************************************************************************************************************

CConfiguration::~CConfiguration()
{
	if (m_pOptions != NULL)
	{
		Memory_Free(m_pOptions);
	}
}

// ***********************************************************************************************************************

void CConfiguration::SetCount(TInt Param_iListCount)
{
	if ((m_pOptions != NULL) || (Param_iListCount <= 0))
	{
		return;
	}

	// Allocate Option List

	m_iOptionCount	= Param_iListCount;
	m_pOptions		= (TBool*)Memory_Allocate(m_iOptionCount * sizeof(TBool));

	// Clear Option List (Set Everything To "False")

	Memory_Fill(m_pOptions, m_iOptionCount * sizeof(TBool));
}

// ***********************************************************************************************************************

void CConfiguration::SetOption(TListID Param_iListID, TBool Param_bValue)
{
	if ((Param_iListID < 0) || (Param_iListID >= m_iOptionCount) || (m_pOptions == NULL))
	{
		return;
	}

	m_pOptions[Param_iListID] = Param_bValue;
}

// ***********************************************************************************************************************

TBool CConfiguration::GetOption(TListID Param_iListID)
{
	if ((Param_iListID < 0) || (Param_iListID >= m_iOptionCount) || (m_pOptions == NULL))
	{
		return FALSE;
	}

	return m_pOptions[Param_iListID];
}

// ***********************************************************************************************************************

TBool* CConfiguration::GetOptionPointer(TListID Param_iListID)
{
	if ((Param_iListID < 0) || (Param_iListID >= m_iOptionCount) || (m_pOptions == NULL))
	{
		return NULL;
	}

	return &m_pOptions[Param_iListID];
}

// ***********************************************************************************************************************
//    Class Methods : CNavLinks
// ***********************************************************************************************************************

CNavLinks::CNavLinks()
{
	m_aLinks = NULL;
}

// ***********************************************************************************************************************

CNavLinks::~CNavLinks()
{
	if (m_aLinks != NULL)
	{
		Memory_Free(m_aLinks);
	}
}

// ***********************************************************************************************************************

TWidgetID CNavLinks::GetLink(eNavLink Param_eNavLink)
{
	// Error Checking

	if ((m_aLinks == NULL) || (Param_eNavLink <= eNavLink_None) || (Param_eNavLink >= eNavLink_Count))
	{
		return kiWidgetID_Invalid;
	}

	// Return Appropriate Link Entry

	return m_aLinks[Param_eNavLink];
}

// ***********************************************************************************************************************

void CNavLinks::SetLink(eNavLink Param_eNavLink, TWidgetID Param_WID_LinkedWidget)
{
	// Error Checking

	if ((Param_eNavLink <= eNavLink_None) || (Param_eNavLink >= eNavLink_Count))
	{
		return;
	}

	// Allocate Entry Array (If Applicable)

	if ((m_aLinks == NULL) && (Param_WID_LinkedWidget != kiWidgetID_Invalid))
	{
		m_aLinks = (TWidgetID*)Memory_Allocate(eNavLink_Count * sizeof(TWidgetID));

		// Initialize Array

		for (TInt Loop = 0; Loop < eNavLink_Count; Loop++)
		{
			m_aLinks[Loop] = kiWidgetID_Invalid;
		}
	}

	// Set Appropriate Link Entry

	m_aLinks[Param_eNavLink] = Param_WID_LinkedWidget;
}

// ***********************************************************************************************************************
//    Class Methods : CWidgetList
// ***********************************************************************************************************************

CWidgetList::CWidgetList()
{
	m_pElements				= NULL;
	m_iCount				= 0;
	m_WID_CurrentElement	= kiWidgetID_Invalid;
}

// ***********************************************************************************************************************

CWidgetList::~CWidgetList()
{
	RemoveAllElements();
}

// ***********************************************************************************************************************

TWidgetID CWidgetList::AddElement(CWidget* Param_pWidgetHandle)
{
	TWidgetID	WID_NewElement;
	CWidget**	pOldList;
	TInt		Loop;

	// Ensure New Widget is Valid

	if (Param_pWidgetHandle == NULL)
	{
		return kiWidgetID_Invalid;
	}

	// Backup Previous List

	pOldList = m_pElements;

	// Allocate New List

	m_pElements = (CWidget**)Memory_Allocate((m_iCount + 1) * sizeof(CWidget*));

	// Transfer Old List To New List (If Applicable)

	if (pOldList != NULL)
	{
		// Copy Pointers

		for (Loop = 0; Loop < m_iCount; Loop++)
		{
			m_pElements[Loop] = pOldList[Loop];
		}

		// Free Old List

		Memory_Free(pOldList);
	}

	// Add New Element To List

	WID_NewElement = m_iCount;

	m_pElements[WID_NewElement] = Param_pWidgetHandle;
	m_iCount++;

	// Make Widget Active (If Applicable)

	if (Param_pWidgetHandle->GetFlags() & kiFlags_Widget_AcquireFocusOnCreate)
	{
		SetCurrentElement(WID_NewElement, eNavLink_None);
	}

	return (WID_NewElement);
}

// ***********************************************************************************************************************

CWidget* CWidgetList::GetElement(TWidgetID Param_iWidgetID)
{
	// Error Checking

	if ((Param_iWidgetID < 0) || (Param_iWidgetID >= m_iCount))
	{
		return NULL;
	}

	if (m_pElements == NULL)
	{
		return NULL;
	}

	// Return Indexed Element

	return m_pElements[Param_iWidgetID];
}

// ***********************************************************************************************************************

CWidget* CWidgetList::GetCurrentElement()
{
	return GetElement(m_WID_CurrentElement);
}

// ***********************************************************************************************************************

void CWidgetList::RemoveAllElements()
{
	if (m_pElements != NULL)
	{
		while (m_iCount > 0)
		{
			m_iCount--;
			delete m_pElements[m_iCount];
		}

		Memory_Free(m_pElements);
	}

	m_pElements	= NULL;
	m_iCount	= 0;
}

// ***********************************************************************************************************************

void CWidgetList::SetCurrentElement(TWidgetID Param_iWidgetID, eNavLink Param_eNavLink)
{
	// Error Checking

	if ((Param_iWidgetID < 0) || (Param_iWidgetID >= m_iCount))
	{
		return;
	}

	// Return If Element Is Already Current

	if (m_WID_CurrentElement == Param_iWidgetID)
	{
		return;
	}

	// Notify Element Losing Focus

	if (m_WID_CurrentElement != kiWidgetID_Invalid)
	{
		m_pElements[m_WID_CurrentElement]->LoseFocus(Param_eNavLink);
	}

	// Update Current Element Field

	m_WID_CurrentElement = Param_iWidgetID;

	// Notify Element Gaining Focus

	if (m_WID_CurrentElement != kiWidgetID_Invalid)
	{
		m_pElements[m_WID_CurrentElement]->GainFocus(Param_eNavLink);
	}
}

// ***********************************************************************************************************************

void CWidgetList::Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged)
{
	// Error Checking

	if (m_pElements == NULL)
	{
		return;
	}

	// Recursively Tick All Sub-Elements

	for (TInt Loop = 0; Loop < m_iCount; Loop++)
	{
		m_pElements[Loop]->Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);
	}
}

// ***********************************************************************************************************************

void CWidgetList::Render(stRenderBase Param_stRenderBase)
{
	// Error Checking

	if (m_pElements == NULL)
	{
		return;
	}

	// Recursively Render All Sub-Elements

	for (TInt Loop = 0; Loop < m_iCount; Loop++)
	{
		m_pElements[Loop]->Render(Param_stRenderBase);
	}
}

// ***********************************************************************************************************************
//    Class Methods : CWidget
// ***********************************************************************************************************************

CWidget::CWidget()
{
	m_iFlags_Widget = kiFlags_Widget_None;

	m_TextSize.Set(1.0f);
	m_Color.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_Position.Set(0.0f, 0.0f);
	m_Size.Set(1.0f, 1.0f);
	m_Clipping.Set(1.0f, 1.0f);
	m_ScrollPosition.Set(0.0f, 0.0f);

	m_eBlending	= eBlending_Alpha;
}

// ***********************************************************************************************************************

CWidget::~CWidget()
{
}

// ***********************************************************************************************************************

void CWidget::Draw(stRenderBase* Param_pstRenderBase)
{
	// Nothing Required For Base Class So Far
}

// ***********************************************************************************************************************

TWidgetID CWidget::AddSubWidget(CWidget* Param_pWidget)
{
	return m_SubWidgets.AddElement(Param_pWidget);
}

// ***********************************************************************************************************************

CWidget* CWidget::GetSubWidget(TWidgetID Param_iWidgetID)
{
	return m_SubWidgets.GetElement(Param_iWidgetID);
}

// ***********************************************************************************************************************

CWidget* CWidget::GetActiveWidget()
{
	return m_SubWidgets.GetCurrentElement();
}

// ***********************************************************************************************************************

void CWidget::SetActiveWidget(TWidgetID Param_iWidgetID, eNavLink Param_eNavLink)
{
	m_SubWidgets.SetCurrentElement(Param_iWidgetID, Param_eNavLink);
}

// ***********************************************************************************************************************

inline TFlags CWidget::GetFlags()
{
	return m_iFlags_Widget;
}

// ***********************************************************************************************************************

inline eBlending CWidget::GetBlending()
{
	return m_eBlending;
}

// ***********************************************************************************************************************

inline void CWidget::SetBlending(eBlending Param_eBlending)
{
	m_eBlending = Param_eBlending;
}

// ***********************************************************************************************************************
void CWidget::SetNavLink(eNavLink Param_eNavLink, TWidgetID Param_WID_LinkedWidget)
{
	m_NavLinks.SetLink(Param_eNavLink, Param_WID_LinkedWidget);
}

// ***********************************************************************************************************************

TWidgetID CWidget::GetNavLink(eNavLink Param_eNavLink)
{
	return m_NavLinks.GetLink(Param_eNavLink);
}

// ***********************************************************************************************************************

inline CFloat* CWidget::Get_TextSize()
{
	return &m_TextSize;
}

// ***********************************************************************************************************************

inline CColor* CWidget::Get_Color()
{
	return &m_Color;
}

// ***********************************************************************************************************************

inline CPoint* CWidget::Get_Position()
{
	return &m_Position;
}

// ***********************************************************************************************************************

inline CPoint* CWidget::Get_Size()
{
	return &m_Size;
}

// ***********************************************************************************************************************

inline CPoint* CWidget::Get_Clipping()
{
	return &m_Clipping;
}

// ***********************************************************************************************************************

inline CPoint* CWidget::Get_ScrollPosition()
{
	return &m_ScrollPosition;
}

// ***********************************************************************************************************************

TBool CWidget::IsVisible(stRenderBase* Param_pstRenderBase)
{
	if (Param_pstRenderBase == NULL)
	{
		// No Rendering Context

		// - Rejection : Alpha

		if (m_Color.GetValue_Alpha() <= 0.0f)
		{
			return FALSE;
		}

		// - Rejection : Out of Bounds +X

		if (m_Position.GetValue_X() >= 1.0f)
		{
			return FALSE;
		}

		// - Rejection : Out of Bounds +Y

		if (m_Position.GetValue_Y() >= 1.0f)
		{
			return FALSE;
		}

		// - Rejection : Out of Bounds -X

		//if ((m_Position.Get_X() + m_Size.Get_X()) <= 0.0f)
		//{
			//return FALSE;
		//}

		// - Rejection : Out of Bounds -Y

		//if ((m_Position.Get_Y() + m_Size.Get_Y()) <= 0.0f)
		//{
			//return FALSE;
		//}
	}
	else
	{
		// Using Rendering Context

		// - Rejection : Alpha

		if (Param_pstRenderBase->fColor_Alpha <= 0.0f)
		{
			return FALSE;
		}

		// - Rejection : Out of Bounds +X

		if (Param_pstRenderBase->stCollisionBox.fPosition_X >= 1.0f)
		{
			return FALSE;
		}

		// - Rejection : Out of Bounds +Y

		if (Param_pstRenderBase->stCollisionBox.fPosition_Y >= 1.0f)
		{
			return FALSE;
		}

		// - Rejection : Out of Bounds -X

		//if ((Param_pRenderBase->SCollisionBox.fPosition_X + Param_pRenderBase->SCollisionBox.fSize_X) <= 0.0f)
		//{
			//return FALSE;
		//}

		// - Rejection : Out of Bounds -Y

		//if ((Param_pRenderBase->SCollisionBox.fPosition_Y + Param_pRenderBase->SCollisionBox.fSize_Y) <= 0.0f)
		//{
			//return FALSE;
		//}
	}

	return TRUE;
}

// ***********************************************************************************************************************

stRenderBase CWidget::AdjustRenderBaseToString(TPWString Param_pString, stRenderBase Param_stRenderBase)
{
	//TFloat Size_X;
	//TFloat Size_Y;

	//GetStringScreenSize(&Size_X, &Size_Y, Param_pString, Param_stRenderBase.stCollisionBox.fSize_X, Param_RenderBase.stCollisionBox.fSize_Y, Param_stRenderBase.iScreen_Width, Param_stRenderBase.iScreen_Height, eFont_Dynamic, m_Clipping.Get_X(), m_Clipping.Get_Y());
	//Param_stRenderBase.stCollisionBox.fSize_X = Size_X / Param_stRenderBase.iScreen_Width;
	//Param_stRenderBase.stCollisionBox.fSize_Y = Size_Y / Param_stRenderBase.iScreen_Height;

	return Param_stRenderBase;
}

// ***********************************************************************************************************************

stBox CWidget::GetBoundingBox(stRenderBase Param_stRenderBase)
{
	stBox stCollisionBox = {0};

	//stCollisionBox.fPosition_X	= m_Position.Get_X();
	//stCollisionBox.fPosition_Y	= m_Position.Get_Y();
	//stCollisionBox.fSize_X		= m_Size.Get_X();
	//stCollisionBox.fSize_Y		= m_Size.Get_Y();

	return stCollisionBox;
}

// ***********************************************************************************************************************

void CWidget::Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged)
{
	m_Position.Tick      (Param_fMSecTimeDelta, Param_bWorldHasChanged);
	m_Size.Tick          (Param_fMSecTimeDelta, Param_bWorldHasChanged);
	m_Clipping.Tick      (Param_fMSecTimeDelta, Param_bWorldHasChanged);
	m_Color.Tick         (Param_fMSecTimeDelta, Param_bWorldHasChanged);
	m_TextSize.Tick      (Param_fMSecTimeDelta, Param_bWorldHasChanged);
	m_ScrollPosition.Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);
	m_SubWidgets.Tick    (Param_fMSecTimeDelta, Param_bWorldHasChanged);
}

// ***********************************************************************************************************************

void CWidget::Render(stRenderBase Param_stRenderBase)
{
	// Propagate Inherited Properties from Parent

	Param_stRenderBase.stCollisionBox.fPosition_X	+= (m_Position.GetValue_X() * Param_stRenderBase.stCollisionBox.fSize_X);
	Param_stRenderBase.stCollisionBox.fPosition_Y	+= (m_Position.GetValue_Y() * Param_stRenderBase.stCollisionBox.fSize_Y);
	Param_stRenderBase.stCollisionBox.fSize_X		*= m_Size.GetValue_X();
	Param_stRenderBase.stCollisionBox.fSize_Y		*= m_Size.GetValue_Y();

	if (m_iFlags_Widget & kiFlags_Widget_IgnoreParentAlpha)
	{
		Param_stRenderBase.fColor_Alpha = m_Color.GetValue_Alpha();
	}
	else
	{
		Param_stRenderBase.fColor_Alpha *= m_Color.GetValue_Alpha();
	}

	Param_stRenderBase.fColor_Red	*= m_Color.GetValue_Red();
	Param_stRenderBase.fColor_Green	*= m_Color.GetValue_Green();
	Param_stRenderBase.fColor_Blue	*= m_Color.GetValue_Blue();

	// Check Visiblity

	TBool bIsVisible = IsVisible(&Param_stRenderBase);

	// Draw Current Widget (If Applicable)

	if (bIsVisible)
	{
		Draw(&Param_stRenderBase);
	}

	// Render Sub Widgets With Propagated Properties

	if (bIsVisible || (m_iFlags_Widget & kiFlags_Widget_IgnoreRenderVisibility))
	{
		m_SubWidgets.Render(Param_stRenderBase);
	}
}

// ***********************************************************************************************************************

void CWidget::LoseFocus(eNavLink Param_eNavLink)
{
	// Nothing Required For Base Class So Far
}

// ***********************************************************************************************************************

void CWidget::GainFocus(eNavLink Param_eNavLink)
{
	// Nothing Required For Base Class So Far
}

// ***********************************************************************************************************************

void CWidget::Signal(eNavLink Param_eNavLink)
{
	// Nothing Required For Base Class So Far
}

// ***********************************************************************************************************************

void CWidget::Navigate(eNavLink Param_eNavLink)
{
	TWidgetID WID_LinkedWidget;

	// Fetch Active Widget

	CWidget* pWidget = GetActiveWidget();

	if (pWidget == NULL)
	{
		return;
	}

	// Fetch Linked WidgetID

	WID_LinkedWidget = pWidget->GetNavLink(Param_eNavLink);

	// Forward Nagivation If WidgetID is Valid

	if (WID_LinkedWidget == kiWidgetID_Invalid)
	{
		pWidget->Navigate(Param_eNavLink);
		return;
	}

	if (WID_LinkedWidget == kiWidgetID_Signal)
	{
		// Signal Input To Widget

		pWidget->Signal(Param_eNavLink);
	}
	else
	{
		// Switch Focus To Linked Widget

		m_SubWidgets.SetCurrentElement(WID_LinkedWidget, Param_eNavLink);
	}
}

// ***********************************************************************************************************************

void CWidget::Scroll(eNavLink Param_eNavLink)
{
	// Propagate Scroll To Active Child (If Applicable)

	CWidget* pWidget = GetActiveWidget();

	if (pWidget != NULL)
	{
		pWidget->Scroll(Param_eNavLink);
		return;
	}

	// Perform Local Scroll

	switch (Param_eNavLink)
	{
		case eNavLink_Left:
		{
			if (m_ScrollPosition.GetValue_X() >= kfOptionList_Scroll_Threshold_X)
			{
				m_ScrollPosition.Get_X()->InterpolateTo(m_ScrollPosition.GetValue_X() - kfOptionList_Scroll_Threshold_X, kfMSecValueDelta_Scroll);
			}
			else
			{
				m_ScrollPosition.Get_X()->InterpolateTo(0.0f, kfMSecValueDelta_Scroll);
			}
			break;
		}

		case eNavLink_Right:
		{
			if (m_ScrollPosition.GetValue_X() <= (1.0f - kfOptionList_Scroll_Threshold_X))
			{
				m_ScrollPosition.Get_X()->InterpolateTo(m_ScrollPosition.GetValue_X() + kfOptionList_Scroll_Threshold_X, kfMSecValueDelta_Scroll);
			}
			else
			{
				m_ScrollPosition.Get_X()->InterpolateTo(1.0f, kfMSecValueDelta_Scroll);
			}
			break;
		}

		case eNavLink_Up:
		{
			if (m_ScrollPosition.GetValue_Y() >= kfOptionList_Scroll_Threshold_Y)
			{
				m_ScrollPosition.Get_Y()->InterpolateTo(m_ScrollPosition.GetValue_Y() - kfOptionList_Scroll_Threshold_Y, kfMSecValueDelta_Scroll);
			}
			else
			{
				m_ScrollPosition.Get_Y()->InterpolateTo(0.0f, kfMSecValueDelta_Scroll);
			}
			break;
		}

		case eNavLink_Down:
		{
			if (m_ScrollPosition.GetValue_Y() <= (1.0f - kfOptionList_Scroll_Threshold_Y))
			{
				m_ScrollPosition.Get_Y()->InterpolateTo(m_ScrollPosition.GetValue_Y() + kfOptionList_Scroll_Threshold_Y, kfMSecValueDelta_Scroll);
			}
			else
			{
				m_ScrollPosition.Get_Y()->InterpolateTo(1.0f, kfMSecValueDelta_Scroll);
			}
			break;
		}
	}
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_ColorQuad
// ***********************************************************************************************************************

CWidget_Base_ColorQuad::CWidget_Base_ColorQuad(TFloat Param_fAlpha, TFloat Param_fRed, TFloat Param_fGreen, TFloat Param_fBlue)
{
	m_Color.Set(Param_fAlpha, Param_fRed, Param_fGreen, Param_fBlue);
}

// ***********************************************************************************************************************

CWidget_Base_ColorQuad::CWidget_Base_ColorQuad(TARGBColor Param_iColor)
{
	m_Color.Set(Param_iColor);
}

// ***********************************************************************************************************************

CWidget_Base_ColorQuad::~CWidget_Base_ColorQuad()
{
}

// ***********************************************************************************************************************

void CWidget_Base_ColorQuad::Draw(stRenderBase* Param_pstRenderBase)
{
	// Parent Draw Operations

	__super::Draw(Param_pstRenderBase);

	// Widget-Specific

	TARGBColor Color_ARGB;

	Color_ARGB = CColor::FloatToARGB(Param_pstRenderBase->fColor_Alpha, Param_pstRenderBase->fColor_Red, Param_pstRenderBase->fColor_Green, Param_pstRenderBase->fColor_Blue);

	Rendering_DrawRectangle(
		Param_pstRenderBase->stCollisionBox.fPosition_X,
		Param_pstRenderBase->stCollisionBox.fPosition_Y,
		Param_pstRenderBase->stCollisionBox.fSize_X,
		Param_pstRenderBase->stCollisionBox.fSize_Y,
		0.0f, 0.0f, 1.0f, 1.0f, Color_ARGB, eBlending_Alpha, kiTextureID_None);
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_TexturedQuad
// ***********************************************************************************************************************

CWidget_Base_TexturedQuad::CWidget_Base_TexturedQuad()
{
	m_iTextureID = kiTextureID_None;
}

// ***********************************************************************************************************************

CWidget_Base_TexturedQuad::CWidget_Base_TexturedQuad(TInt Param_iTextureID)
{
	m_iTextureID = Param_iTextureID;
}

// ***********************************************************************************************************************

CWidget_Base_TexturedQuad::~CWidget_Base_TexturedQuad()
{
}

// ***********************************************************************************************************************

TInt CWidget_Base_TexturedQuad::GetTextureID()
{
	return m_iTextureID;
}

// ***********************************************************************************************************************

void CWidget_Base_TexturedQuad::SetTextureID(TInt Param_iTextureID)
{
	m_iTextureID = Param_iTextureID;
}

// ***********************************************************************************************************************

void CWidget_Base_TexturedQuad::Draw(stRenderBase* Param_pstRenderBase)
{
	// Parent Draw Operations

	__super::Draw(Param_pstRenderBase);

	// Widget-Specific

	TARGBColor Color_ARGB;

	Color_ARGB = CColor::FloatToARGB(Param_pstRenderBase->fColor_Alpha, Param_pstRenderBase->fColor_Red, Param_pstRenderBase->fColor_Green, Param_pstRenderBase->fColor_Blue);

	Rendering_DrawRectangle(
		Param_pstRenderBase->stCollisionBox.fPosition_X,
		Param_pstRenderBase->stCollisionBox.fPosition_Y,
		Param_pstRenderBase->stCollisionBox.fSize_X,
		Param_pstRenderBase->stCollisionBox.fSize_Y,
		0.0f, 0.0f, 1.0f, 1.0f, Color_ARGB, eBlending_Alpha, m_iTextureID);
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_Text
// ***********************************************************************************************************************

CWidget_Base_Text::CWidget_Base_Text()
{
	SetBlending(eBlending_AlphaAdd);
}

// ***********************************************************************************************************************

CWidget_Base_Text::CWidget_Base_Text(TPWString Param_pString)
{
	SetBlending(eBlending_AlphaAdd);

	SetText(Param_pString);
}

// ***********************************************************************************************************************

CWidget_Base_Text::~CWidget_Base_Text()
{
}

// ***********************************************************************************************************************

void CWidget_Base_Text::Draw(stRenderBase* Param_pstRenderBase)
{
	// Check Content

	if (m_Text.IsEmpty() || (m_Text.GetString() == NULL))
	{
		return;
	}

	// Parent Draw Operations

	__super::Draw(Param_pstRenderBase);

	// Widget-Specific Drawing

	TARGBColor Color_ARGB;

	Color_ARGB = CColor::FloatToARGB(Param_pstRenderBase->fColor_Alpha, Param_pstRenderBase->fColor_Red, Param_pstRenderBase->fColor_Green, Param_pstRenderBase->fColor_Blue);

	DrawString(m_Text.GetString(), Param_pstRenderBase->stCollisionBox.fPosition_X, Param_pstRenderBase->stCollisionBox.fPosition_Y, Param_pstRenderBase->stCollisionBox.fSize_X * m_TextSize.Get(), Param_pstRenderBase->stCollisionBox.fSize_Y * m_TextSize.Get(), Color_ARGB, GetBlending(), kiFontID_UsedForText, m_Clipping.GetValue_X(), m_Clipping.GetValue_Y());
}

// ***********************************************************************************************************************

TBool CWidget_Base_Text::IsVisible(stRenderBase* Param_pstRenderBase)
{
	//stRenderBase stAdjustedBase = AdjustRenderBaseToString(m_Text.GetString(), *Param_pstRenderBase);

	//return __super::IsVisible(&stAdjustedBase);
	return __super::IsVisible(Param_pstRenderBase);
}

// ***********************************************************************************************************************

void CWidget_Base_Text::SetText(TPWString Param_pString)
{
	m_Text.SetTo(Param_pString);
}

// ***********************************************************************************************************************

TPWString CWidget_Base_Text::GetText()
{
	return m_Text.GetString();
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_Float
// ***********************************************************************************************************************

CWidget_Base_Float::CWidget_Base_Float()
{
	SetBlending(eBlending_AlphaAdd);

	SetValue(0.0f);
}

// ***********************************************************************************************************************

CWidget_Base_Float::CWidget_Base_Float(TFloat Param_fValue)
{
	SetBlending(eBlending_AlphaAdd);

	SetValue(Param_fValue);
}

// ***********************************************************************************************************************

CWidget_Base_Float::~CWidget_Base_Float()
{
}

// ***********************************************************************************************************************

void CWidget_Base_Float::Draw(stRenderBase* Param_pstRenderBase)
{
	TWChar Buffer[32];

	// Parent Draw Operations

	__super::Draw(Param_pstRenderBase);

	// Widget-Specific Drawing

	TARGBColor Color_ARGB;

	Color_ARGB = CColor::FloatToARGB(Param_pstRenderBase->fColor_Alpha, Param_pstRenderBase->fColor_Red, Param_pstRenderBase->fColor_Green, Param_pstRenderBase->fColor_Blue);

	swprintf(Buffer, L"%.2f", m_Value.Get());

	DrawString(Buffer, Param_pstRenderBase->stCollisionBox.fPosition_X, Param_pstRenderBase->stCollisionBox.fPosition_Y, Param_pstRenderBase->stCollisionBox.fSize_X * m_TextSize.Get(), Param_pstRenderBase->stCollisionBox.fSize_Y * m_TextSize.Get(), Color_ARGB, GetBlending(), kiFontID_UsedForText, m_Clipping.GetValue_X(), m_Clipping.GetValue_Y());
}

// ***********************************************************************************************************************

TFloat CWidget_Base_Float::GetValue()
{
	return m_Value.Get();
}

// ***********************************************************************************************************************

void CWidget_Base_Float::SetValue(TFloat Param_fValue)
{
	m_Value.Set(Param_fValue);
}

// ***********************************************************************************************************************

void CWidget_Base_Float::UpdateValue(TFloat Param_fValue)
{
	TFloat fSpeed;

	fSpeed = fabs(m_Value.Get() - Param_fValue) * kfMSecValueDelta_ValueUpdateFactor;

	m_Value.InterpolateTo(Param_fValue, fSpeed);
}

// ***********************************************************************************************************************

void CWidget_Base_Float::Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged)
{
	// Parent Tick

	__super::Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);

	// Widget-Specific

	m_Value.Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_Integer
// ***********************************************************************************************************************

CWidget_Base_Integer::CWidget_Base_Integer()
{
	SetBlending(eBlending_AlphaAdd);
	m_Cache_IsValid	= FALSE;

	SetValue(0);
	SetDigitCount(1);
	SetDisplayAsPercent(FALSE);
}

// ***********************************************************************************************************************

CWidget_Base_Integer::CWidget_Base_Integer(TInt Param_iValue)
{
	SetBlending(eBlending_AlphaAdd);
	m_Cache_IsValid	= FALSE;

	SetValue(Param_iValue);
	SetDigitCount(1);
	SetDisplayAsPercent(FALSE);
}

// ***********************************************************************************************************************

CWidget_Base_Integer::~CWidget_Base_Integer()
{
}

// ***********************************************************************************************************************

void CWidget_Base_Integer::SetDigitCount(TInt Param_iValue)
{
	// Clamp Value

	if (Param_iValue <= 0)
	{
		Param_iValue = 1;
	}

	// Update Value (If Applicable)

	if (m_iDigitCount != Param_iValue)
	{
		m_iDigitCount = Param_iValue;

		// Invalidate Cache

		m_Cache_IsValid	= FALSE;
	}
}

// ***********************************************************************************************************************

void CWidget_Base_Integer::SetDisplayAsPercent(TBool Param_bDisplayAsPercent)
{
	if (m_bDisplayAsPercent != Param_bDisplayAsPercent)
	{
		m_bDisplayAsPercent = Param_bDisplayAsPercent;

		// Invalidate Cache

		m_Cache_IsValid	= FALSE;
	}
}

// ***********************************************************************************************************************

void CWidget_Base_Integer::Draw(stRenderBase* Param_pstRenderBase)
{
	// Parent Draw Operations

	__super::Draw(Param_pstRenderBase);

	// Widget-Specific Drawing

	TARGBColor Color_ARGB;

	Color_ARGB = CColor::FloatToARGB(Param_pstRenderBase->fColor_Alpha, Param_pstRenderBase->fColor_Red, Param_pstRenderBase->fColor_Green, Param_pstRenderBase->fColor_Blue);

	// Update Formatted Text Cache

	UpdateCache();

	// Draw Formatted Text

	DrawString(m_Cache_FormattedText.GetString(), Param_pstRenderBase->stCollisionBox.fPosition_X, Param_pstRenderBase->stCollisionBox.fPosition_Y, Param_pstRenderBase->stCollisionBox.fSize_X * m_TextSize.Get(), Param_pstRenderBase->stCollisionBox.fSize_Y * m_TextSize.Get(), Color_ARGB, GetBlending(), kiFontID_UsedForText, m_Clipping.GetValue_X(), m_Clipping.GetValue_Y());
}

// ***********************************************************************************************************************

TInt CWidget_Base_Integer::GetValue()
{
	return m_Value.Get();
}

// ***********************************************************************************************************************

void CWidget_Base_Integer::SetValue(TInt Param_iValue)
{
	if (m_Value.Get() != Param_iValue)
	{
		m_Value.Set(Param_iValue);

		// Invalidate Cache

		m_Cache_IsValid	= FALSE;
	}
}

// ***********************************************************************************************************************

void CWidget_Base_Integer::UpdateValue(TInt Param_iValue)
{
	TFloat fSpeed;

	fSpeed = (TFloat)(abs(m_Value.Get() - Param_iValue)) * kfMSecValueDelta_ValueUpdateFactor;

	if (fSpeed < 1.0f)
	{ 
		fSpeed = 1.0f;
	}

	m_Value.InterpolateTo(Param_iValue, (TInt)fSpeed);
}

// ***********************************************************************************************************************

void CWidget_Base_Integer::Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged)
{
	// Parent Tick

	__super::Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);

	// Invalidate Cache (If Applicable)

	if (m_Value.IsChanging())
	{
		m_Cache_IsValid	= FALSE;
	}

	// Widget-Specific

	m_Value.Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);
}

// ***********************************************************************************************************************

void CWidget_Base_Integer::UpdateCache()
{
	if (m_Cache_IsValid)
	{
		return;
	}

	// Convert Value To String

	TWChar acBuffer[32];

	IntegerToString(m_Value.Get(), acBuffer, TRUE, m_bDisplayAsPercent, m_iDigitCount);

	// Update Cache Formatted Text

	m_Cache_FormattedText.SetTo(acBuffer);

	// Validate Cache

	m_Cache_IsValid = TRUE;
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_Boolean
// ***********************************************************************************************************************

CWidget_Base_Boolean::CWidget_Base_Boolean()
{
	SetBlending(eBlending_AlphaAdd);

	SetValue(FALSE);
}

// ***********************************************************************************************************************

CWidget_Base_Boolean::CWidget_Base_Boolean(TBool Param_bValue)
{
	SetBlending(eBlending_AlphaAdd);

	SetValue(Param_bValue);
}

// ***********************************************************************************************************************

CWidget_Base_Boolean::~CWidget_Base_Boolean()
{
}

// ***********************************************************************************************************************

void CWidget_Base_Boolean::Draw(stRenderBase* Param_pstRenderBase)
{
	TPWString pBuffer;

	// Parent Draw Operations

	__super::Draw(Param_pstRenderBase);

	// Widget-Specific Drawing

	TARGBColor iColor_ARGB;

	iColor_ARGB = CColor::FloatToARGB(Param_pstRenderBase->fColor_Alpha, Param_pstRenderBase->fColor_Red, Param_pstRenderBase->fColor_Green, Param_pstRenderBase->fColor_Blue);

	if (m_Value.Get())
	{
		pBuffer = L"True";
	}
	else
	{
		pBuffer = L"False";
	}

	DrawString(pBuffer, Param_pstRenderBase->stCollisionBox.fPosition_X, Param_pstRenderBase->stCollisionBox.fPosition_Y, Param_pstRenderBase->stCollisionBox.fSize_X * m_TextSize.Get(), Param_pstRenderBase->stCollisionBox.fSize_Y * m_TextSize.Get(), iColor_ARGB, GetBlending(), kiFontID_UsedForText, m_Clipping.GetValue_X(), m_Clipping.GetValue_Y());
}

// ***********************************************************************************************************************

TBool CWidget_Base_Boolean::GetValue()
{
	return m_Value.Get();
}

// ***********************************************************************************************************************

void CWidget_Base_Boolean::SetValue(TBool Param_bValue)
{
	m_Value.Set(Param_bValue);
}

// ***********************************************************************************************************************

void CWidget_Base_Boolean::UpdateValue(TBool Param_bValue)
{
	m_Value.Set(Param_bValue);
}

// ***********************************************************************************************************************

void CWidget_Base_Boolean::Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged)
{
	// Parent Tick

	__super::Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_FontGlyph
// ***********************************************************************************************************************

CWidget_Base_FontGlyph::CWidget_Base_FontGlyph()
{
	SetGlyph(kcFontGlyph_None);
}

// ***********************************************************************************************************************

CWidget_Base_FontGlyph::CWidget_Base_FontGlyph(TFontGlyph Param_cGlyph)
{
	SetGlyph(Param_cGlyph);
}

// ***********************************************************************************************************************

CWidget_Base_FontGlyph::~CWidget_Base_FontGlyph()
{
}

// ***********************************************************************************************************************

void CWidget_Base_FontGlyph::Draw(stRenderBase* Param_pstRenderBase)
{
	// Check Content

	if (GetGlyph() == kcFontGlyph_None)
	{
		return;
	}

	// Parent Draw Operations

	__super::Draw(Param_pstRenderBase);

	// Widget-Specific Drawing

	TARGBColor Color_ARGB;

	Color_ARGB = CColor::FloatToARGB(Param_pstRenderBase->fColor_Alpha, Param_pstRenderBase->fColor_Red, Param_pstRenderBase->fColor_Green, Param_pstRenderBase->fColor_Blue);

	DrawString(m_acGlyph, Param_pstRenderBase->stCollisionBox.fPosition_X, Param_pstRenderBase->stCollisionBox.fPosition_Y, Param_pstRenderBase->stCollisionBox.fSize_X * m_TextSize.Get(), Param_pstRenderBase->stCollisionBox.fSize_Y * m_TextSize.Get(), Color_ARGB, GetBlending(), kiFontID_UsedForGlyphs, m_Clipping.GetValue_X(), m_Clipping.GetValue_Y());
}

// ***********************************************************************************************************************

void CWidget_Base_FontGlyph::SetGlyph(TFontGlyph Param_cGlyph)
{
	m_acGlyph[0] = Param_cGlyph;
	m_acGlyph[1] = kcFontGlyph_None;
}

// ***********************************************************************************************************************

TFontGlyph CWidget_Base_FontGlyph::GetGlyph()
{
	return m_acGlyph[0];
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_ValueTracker
// ***********************************************************************************************************************

CWidget_Base_ValueTracker::CWidget_Base_ValueTracker()
{
	Initialize(NULL, kstTrackerType_Integer);
}

// ***********************************************************************************************************************

CWidget_Base_ValueTracker::CWidget_Base_ValueTracker(TPWString Param_pName, stTrackerType Param_stType)
{
	Initialize(Param_pName, Param_stType);
}

// ***********************************************************************************************************************

CWidget_Base_ValueTracker::CWidget_Base_ValueTracker(stTrackerType Param_stType)
{
	Initialize(NULL, Param_stType);
}

// ***********************************************************************************************************************

CWidget_Base_ValueTracker::~CWidget_Base_ValueTracker()
{
}

// ***********************************************************************************************************************

void CWidget_Base_ValueTracker::Initialize(TPWString Param_pName, stTrackerType Param_stType)
{
	// Initialize Fields

	m_stType = Param_stType;

	// Create SubWidget - Name

	CWidget_Base_Text* pWidget_Name = new CWidget_Base_Text(Param_pName);
	m_WID_Name = AddSubWidget(pWidget_Name);

	// Create SubWidget - Value

	switch(m_stType.iStorage)
	{
		case kiTrackerStorage_Float:
		{
			CWidget_Base_Float* pWidget_Value = new CWidget_Base_Float(0.0f);
			m_WID_Value_Native = AddSubWidget(pWidget_Value);

			// Make Value Invisible if "Glyph" or "Enum" Rendering is Used

			if ((m_stType.cGlyph_Foreground != kcFontGlyph_None) || (m_stType.pstEnum != NULL))
			{
				pWidget_Value->Get_Color()->Set(0.0f);
			}
			break;
		}

		case kiTrackerStorage_Integer:
		{
			CWidget_Base_Integer* pWidget_Value = new CWidget_Base_Integer(0);
			m_WID_Value_Native = AddSubWidget(pWidget_Value);

			// Make Value Invisible if "Glyph" or "Enum" Rendering is Used

			if ((m_stType.cGlyph_Foreground != kcFontGlyph_None) || (m_stType.pstEnum != NULL))
			{
				pWidget_Value->Get_Color()->Set(0.0f);
			}
			break;
		}

		case kiTrackerStorage_Boolean:
		{
			CWidget_Base_Boolean* pWidget_Value = new CWidget_Base_Boolean(FALSE);
			m_WID_Value_Native = AddSubWidget(pWidget_Value);

			// Make Value Invisible if "Glyph" or "Enum" Rendering is Used

			if ((m_stType.cGlyph_Foreground != kcFontGlyph_None) || (m_stType.pstEnum != NULL))
			{
				pWidget_Value->Get_Color()->Set(0.0f);
			}
			break;
		}
	}

	// Create SubWidget - Background Glyph (If Applicable)

	if (m_stType.cGlyph_Background != kcFontGlyph_None)
	{
		CWidget_Base_FontGlyph* pWidget_Glyph = new CWidget_Base_FontGlyph(m_stType.cGlyph_Background);
		m_WID_Value_BackGlyph = AddSubWidget(pWidget_Glyph);
	}
	else
	{
		m_WID_Value_BackGlyph = kiWidgetID_Invalid;
	}

	// Create SubWidget - Foreground Glyph (If Applicable)

	if (m_stType.cGlyph_Foreground != kcFontGlyph_None)
	{
		CWidget_Base_FontGlyph* pWidget_Glyph = new CWidget_Base_FontGlyph(m_stType.cGlyph_Foreground);
		m_WID_Value_ForeGlyph = AddSubWidget(pWidget_Glyph);

		pWidget_Glyph->Get_Clipping()->Set(0.0f, 1.0f);
	}
	else
	{
		m_WID_Value_ForeGlyph = kiWidgetID_Invalid;
	}

	// Create SubWidget - Text (If Applicable)

	if ((m_stType.pstEnum != NULL) && (m_stType.pstEnum->aStrings != NULL))
	{
		CWidget_Base_Text* pWidget_Text = new CWidget_Base_Text(m_stType.pstEnum->aStrings[0]);
		m_WID_Value_Text = AddSubWidget(pWidget_Text);
	}
	else
	{
		m_WID_Value_Text = kiWidgetID_Invalid;
	}
}
// ***********************************************************************************************************************

void CWidget_Base_ValueTracker::SetName(TPWString Param_pName)
{
	CWidget_Base_Text* pWidget_Name;

	pWidget_Name = (CWidget_Base_Text*)GetSubWidget(m_WID_Name);
	if (pWidget_Name != NULL)
	{
		pWidget_Name->SetText(Param_pName);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_ValueTracker::SetValue(TFloat Param_fValue)
{
	// Return on Value of Wrong Type

	if (m_stType.iStorage != kiTrackerStorage_Float)
	{
		return;
	}

	// Set New Value

	CWidget_Base_Float* pWidget_Value;

	pWidget_Value = (CWidget_Base_Float*)GetSubWidget(m_WID_Value_Native);
	if (pWidget_Value != NULL)
	{
		pWidget_Value->SetValue(Param_fValue);
	}

	// Update Foreground Glyph Clipping (If Applicable)

	CWidget_Base_FontGlyph* pWidget_Glyph;

	pWidget_Glyph = (CWidget_Base_FontGlyph*)GetSubWidget(m_WID_Value_ForeGlyph);
	if (pWidget_Glyph != NULL)
	{
		// Clamp Clipping Value To [0.0, 1.0] Range

		if (Param_fValue < 0.0f)
		{
			Param_fValue = 0.0f;
		}

		if (Param_fValue > 1.0f)
		{
			Param_fValue = 1.0f;
		}

		// Update Fore Glyph Clipping Value

		pWidget_Glyph->Get_Clipping()->Set(Param_fValue, 1.0f);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_ValueTracker::SetValue(TInt Param_iValue)
{
	// Return on Value of Wrong Type

	if (m_stType.iStorage != kiTrackerStorage_Integer)
	{
		return;
	}

	// Set New Value

	CWidget_Base_Integer* pWidget_Value;

	pWidget_Value = (CWidget_Base_Integer*)GetSubWidget(m_WID_Value_Native);
	if (pWidget_Value != NULL)
	{
		pWidget_Value->SetValue(Param_iValue);
	}

	// Update Enum Text (If Applicable)

	CWidget_Base_Text* pWidget_Text;

	pWidget_Text = (CWidget_Base_Text*)GetSubWidget(m_WID_Value_Text);
	if ((pWidget_Text != NULL) && (m_stType.pstEnum != NULL))
	{
		pWidget_Text->SetText(m_stType.pstEnum->aStrings[Param_iValue - m_stType.pstEnum->iFirstIndex]);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_ValueTracker::SetValue(TBool Param_bValue)
{
	// Return on Value of Wrong Type

	if (m_stType.iStorage != kiTrackerStorage_Boolean)
	{
		return;
	}

	// Set New Value

	CWidget_Base_Boolean* pWidget_Value;

	pWidget_Value = (CWidget_Base_Boolean*)GetSubWidget(m_WID_Value_Native);
	if (pWidget_Value != NULL)
	{
		pWidget_Value->SetValue(Param_bValue);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_ValueTracker::UpdateValue(TFloat Param_fValue)
{
	// Return on Value of Wrong Type

	if (m_stType.iStorage != kiTrackerStorage_Float)
	{
		return;
	}

	// Interpolate To New Value

	CWidget_Base_Float* pWidget_Value;

	pWidget_Value = (CWidget_Base_Float*)GetSubWidget(m_WID_Value_Native);
	if (pWidget_Value != NULL)
	{
		pWidget_Value->UpdateValue(Param_fValue);
	}

	// Update Foreground Glyph Clipping (If Applicable)

	CWidget_Base_FontGlyph* pWidget_Glyph;

	pWidget_Glyph = (CWidget_Base_FontGlyph*)GetSubWidget(m_WID_Value_ForeGlyph);
	if (pWidget_Glyph != NULL)
	{
		// Clamp Clipping Value To [0.0, 1.0] Range

		if (Param_fValue < 0.0f)
		{
			Param_fValue = 0.0f;
		}

		if (Param_fValue > 1.0f)
		{
			Param_fValue = 1.0f;
		}

		// Update Fore Glyph Clipping Value

		pWidget_Glyph->Get_Clipping()->MoveTo(Param_fValue, 1.0f, kfMSecValueDelta_Clipping);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_ValueTracker::UpdateValue(TInt Param_iValue)
{
	// Return on Value of Wrong Type

	if (m_stType.iStorage != kiTrackerStorage_Integer)
	{
		return;
	}

	// Interpolate To New Value

	CWidget_Base_Integer* pWidget_Value;

	pWidget_Value = (CWidget_Base_Integer*)GetSubWidget(m_WID_Value_Native);
	if (pWidget_Value != NULL)
	{
		pWidget_Value->UpdateValue(Param_iValue);
	}

	// Update Enum Text (If Applicable)

	CWidget_Base_Text* pWidget_Text;

	pWidget_Text = (CWidget_Base_Text*)GetSubWidget(m_WID_Value_Text);
	if ((pWidget_Text != NULL) && (m_stType.pstEnum != NULL))
	{
		pWidget_Text->SetText(m_stType.pstEnum->aStrings[Param_iValue - m_stType.pstEnum->iFirstIndex]);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_ValueTracker::UpdateValue(TBool Param_bValue)
{
	// Return on Value of Wrong Type

	if (m_stType.iStorage != kiTrackerStorage_Boolean)
	{
		return;
	}

	// Interpolate To New Value

	CWidget_Base_Boolean* pWidget_Value;

	pWidget_Value = (CWidget_Base_Boolean*)GetSubWidget(m_WID_Value_Native);
	if (pWidget_Value != NULL)
	{
		pWidget_Value->UpdateValue(Param_bValue);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_ValueTracker::SetPositions(TFloat Param_fPosition_X_Name, TFloat Param_fPosition_X_Value, TFloat Param_fPosition_Y)
{
	// Tracker "Parent" Position

	m_Position.Set(Param_fPosition_X_Name, Param_fPosition_Y);

	// Name "Child" Position

	CWidget* pWidget_Name = GetSubWidget(m_WID_Name);
	if (pWidget_Name != NULL)
	{
		pWidget_Name->Get_Position()->Set(0.0f, 0.0f);
	}

	// Value "Child" Position

	CWidget* pWidget_Value = GetSubWidget(m_WID_Value_Native);
	if (pWidget_Value != NULL)
	{
		pWidget_Value->Get_Position()->Set(Param_fPosition_X_Value - Param_fPosition_X_Name, 0.0f);
	}

	// Background Glyph "Child" Position

	CWidget* pWidget_BackGlyph = GetSubWidget(m_WID_Value_BackGlyph);
	if (pWidget_BackGlyph != NULL)
	{
		pWidget_BackGlyph->Get_Position()->Set(Param_fPosition_X_Value - Param_fPosition_X_Name, 0.0f);
	}

	// Foreground Glyph "Child" Position

	CWidget* pWidget_ForeGlyph = GetSubWidget(m_WID_Value_ForeGlyph);
	if (pWidget_ForeGlyph != NULL)
	{
		pWidget_ForeGlyph->Get_Position()->Set(Param_fPosition_X_Value - Param_fPosition_X_Name, 0.0f);
	}

	// Enum Text "Child" Position

	CWidget* pWidget_Text = GetSubWidget(m_WID_Value_Text);
	if (pWidget_Text != NULL)
	{
		pWidget_Text->Get_Position()->Set(Param_fPosition_X_Value - Param_fPosition_X_Name, 0.0f);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_ValueTracker::SetTextSize(TFloat Param_fSize)
{
	// Name

	CWidget* pWidget_Name = GetSubWidget(m_WID_Name);
	if (pWidget_Name != NULL)
	{
		pWidget_Name->Get_TextSize()->Set(Param_fSize);
	}

	// Value - Native

	CWidget* pWidget_Value = GetSubWidget(m_WID_Value_Native);
	if (pWidget_Value != NULL)
	{
		pWidget_Value->Get_TextSize()->Set(Param_fSize);
	}

	// Value - Enum Text

	CWidget* pWidget_Text = GetSubWidget(m_WID_Value_Text);
	if (pWidget_Text != NULL)
	{
		pWidget_Text->Get_TextSize()->Set(Param_fSize);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_ValueTracker::SetGlyphSize(TFloat Param_fSize_X, TFloat Param_fSize_Y)
{
	// Background Glyph

	CWidget* pWidget_BackGlyph = GetSubWidget(m_WID_Value_BackGlyph);
	if (pWidget_BackGlyph != NULL)
	{
		pWidget_BackGlyph->Get_Size()->Set(Param_fSize_X, Param_fSize_Y);
	}

	// Foreground Glyph

	CWidget* pWidget_ForeGlyph = GetSubWidget(m_WID_Value_ForeGlyph);
	if (pWidget_ForeGlyph != NULL)
	{
		pWidget_ForeGlyph->Get_Size()->Set(Param_fSize_X, Param_fSize_Y);
	}
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_ListHeader
// ***********************************************************************************************************************

CWidget_Base_ListHeader::CWidget_Base_ListHeader(TPWString Param_pString)
{
	// Create SubWidget - Top Line

	CWidget_Base_ColorQuad* pWidget_TopLine = new CWidget_Base_ColorQuad(kiColor_ListHeader_Lines);
	m_WID_TopLine = AddSubWidget(pWidget_TopLine);

	// Create SubWidget - Bottom Line

	CWidget_Base_ColorQuad* pWidget_BottomLine = new CWidget_Base_ColorQuad(kiColor_ListHeader_Lines);
	m_WID_BottomLine = AddSubWidget(pWidget_BottomLine);

	// Create SubWidget - Central Band

	CWidget_Base_ColorQuad* pWidget_CentralBand = new CWidget_Base_ColorQuad(kiColor_ListHeader_CentralBand);
	m_WID_CentralBand = AddSubWidget(pWidget_CentralBand);

	// Create SubWidget - Header Text

	CWidget_Base_Text* pWidget_Text = new CWidget_Base_Text(Param_pString);
	m_WID_Text = AddSubWidget(pWidget_Text);

	// Set Sizes & Positions

	SetBarSize(1.0f, 1.0f);
}

// ***********************************************************************************************************************

CWidget_Base_ListHeader::~CWidget_Base_ListHeader()
{
}

// ***********************************************************************************************************************

void CWidget_Base_ListHeader::SetBarSize(TFloat Param_fSize_X, TFloat Param_fSize_Y)
{
	CWidget_Base_ColorQuad* pWidget_ColorQuad;

	// Top Line

	pWidget_ColorQuad = (CWidget_Base_ColorQuad*)GetSubWidget(m_WID_TopLine);
	pWidget_ColorQuad->Get_Size()->Set(Param_fSize_X * 1.0f, Param_fSize_Y * 0.1f);

	// Bottom Line

	pWidget_ColorQuad = (CWidget_Base_ColorQuad*)GetSubWidget(m_WID_BottomLine);
	pWidget_ColorQuad->Get_Size()->Set(Param_fSize_X * 1.0f, Param_fSize_Y * 0.1f);
	pWidget_ColorQuad->Get_Position()->Set(0.0f, Param_fSize_Y * 0.9f);

	// Central Band

	pWidget_ColorQuad = (CWidget_Base_ColorQuad*)GetSubWidget(m_WID_CentralBand);
	pWidget_ColorQuad->Get_Size()->Set(Param_fSize_X * 1.0f, Param_fSize_Y * 0.6f);
	pWidget_ColorQuad->Get_Position()->Set(0.0f, Param_fSize_Y * 0.2f);

	// Header Text

	CWidget_Base_Text* pWidget_Text = (CWidget_Base_Text*)GetSubWidget(m_WID_Text);
	pWidget_Text->Get_Position()->Set(0.0f, Param_fSize_Y * 0.07f);
}

// ***********************************************************************************************************************

void CWidget_Base_ListHeader::SetTextSize(TFloat Param_fSize)
{
	CWidget* pWidget_Text = GetSubWidget(m_WID_Text);
	pWidget_Text->Get_TextSize()->Set(Param_fSize);
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_Frame
// ***********************************************************************************************************************

CWidget_Base_Frame::CWidget_Base_Frame()
{
	Initialize();
}

// ***********************************************************************************************************************

CWidget_Base_Frame::CWidget_Base_Frame(TPWString Param_pTitle)
{
	Initialize(Param_pTitle);
}

// ***********************************************************************************************************************

CWidget_Base_Frame::~CWidget_Base_Frame()
{
}

// ***********************************************************************************************************************

void CWidget_Base_Frame::Initialize(TPWString Param_pTitle)
{
	CWidget_Base_Text*		pWidget_Title;
	CWidget_Base_ColorQuad* pWidget_Background;
	CWidget_Base_FontGlyph*	pWidget_FontGlyph;

	// Create SubWidget - Background

	pWidget_Background = new CWidget_Base_ColorQuad(kiColor_Window_Background);

	m_WID_Background = AddSubWidget(pWidget_Background);

	// Create SubWidget - Top

	pWidget_FontGlyph = new CWidget_Base_FontGlyph(kcFontGlyph_Window_Top);

	pWidget_FontGlyph->Get_Size()->Set(7.82f, kfWindow_TopBarHeight);
	pWidget_FontGlyph->Get_Position()->Set(0.0125f, 0.0f);

	m_WID_Top = AddSubWidget(pWidget_FontGlyph);

	// Create SubWidget - Left

	pWidget_FontGlyph = new CWidget_Base_FontGlyph(kcFontGlyph_Window_Left);

	pWidget_FontGlyph->Get_Size()->Set(0.1f, 7.51f);
	pWidget_FontGlyph->Get_Position()->Set(0.0f, 0.05f);

	m_WID_Left = AddSubWidget(pWidget_FontGlyph);

	// Create SubWidget - Right

	pWidget_FontGlyph = new CWidget_Base_FontGlyph(kcFontGlyph_Window_Right);

	pWidget_FontGlyph->Get_Size()->Set(0.1f, 7.51f);
	pWidget_FontGlyph->Get_Position()->Set(0.996f, 0.05f);

	m_WID_Right = AddSubWidget(pWidget_FontGlyph);

	// Create SubWidget - Bottom

	pWidget_FontGlyph = new CWidget_Base_FontGlyph(kcFontGlyph_Window_Bottom);

	pWidget_FontGlyph->Get_Size()->Set(7.89f, 0.1f);
	pWidget_FontGlyph->Get_Position()->Set(0.011f, 0.9945f);

	m_WID_Bottom = AddSubWidget(pWidget_FontGlyph);

	// Create SubWidget - Top Left

	pWidget_FontGlyph = new CWidget_Base_FontGlyph(kcFontGlyph_Window_TopLeft);

	pWidget_FontGlyph->Get_Size()->Set(0.1f, kfWindow_TopBarHeight);
	pWidget_FontGlyph->Get_Position()->Set(0.0, 0.0f);

	m_WID_Top_Left = AddSubWidget(pWidget_FontGlyph);

	// Create SubWidget - Top Right

	pWidget_FontGlyph = new CWidget_Base_FontGlyph(kcFontGlyph_Window_TopRight);

	pWidget_FontGlyph->Get_Size()->Set(0.1f, kfWindow_TopBarHeight);
	pWidget_FontGlyph->Get_Position()->Set(0.99f, 0.0f);

	m_WID_Top_Right = AddSubWidget(pWidget_FontGlyph);

	// Create SubWidget - Bottom Left

	pWidget_FontGlyph = new CWidget_Base_FontGlyph(kcFontGlyph_Window_BottomLeft);

	pWidget_FontGlyph->Get_Size()->Set(0.1f, 0.1f);
	pWidget_FontGlyph->Get_Position()->Set(0.0, 0.9883f);

	m_WID_Bottom_Left = AddSubWidget(pWidget_FontGlyph);

	// Create SubWidget - Bottom Right

	pWidget_FontGlyph = new CWidget_Base_FontGlyph(kcFontGlyph_Window_BottomRight);

	pWidget_FontGlyph->Get_Size()->Set(0.1f, 0.1f);
	pWidget_FontGlyph->Get_Position()->Set(0.99f, 0.9883f);

	m_WID_Bottom_Right = AddSubWidget(pWidget_FontGlyph);

	// Create SubWidget - Title

	pWidget_Title = new CWidget_Base_Text(Param_pTitle);

	pWidget_Title->Get_Color()->Set(kiColor_Window_Title);
	pWidget_Title->Get_Position()->Set(0.02f, 0.003f);
	pWidget_Title->Get_Size()->Set(kfFont_Size_WindowTitle, kfFont_Size_WindowTitle);

	m_WID_Title = AddSubWidget(pWidget_Title);
}

// ***********************************************************************************************************************

void CWidget_Base_Frame::SetTitle(TPWString Param_pTitle)
{
	CWidget_Base_Text* pWidget_Title;

	pWidget_Title = (CWidget_Base_Text*)GetSubWidget(m_WID_Title);

	if (pWidget_Title != NULL)
	{
		pWidget_Title->SetText(Param_pTitle);
	}
}

// ***********************************************************************************************************************

TPWString CWidget_Base_Frame::GetTitle()
{
	CWidget_Base_Text* pWidget_Title;

	pWidget_Title = (CWidget_Base_Text*)GetSubWidget(m_WID_Title);

	if (pWidget_Title != NULL)
	{
		return pWidget_Title->GetText();
	}
	else
	{
		return NULL;
	}
}

// ***********************************************************************************************************************
void CWidget_Base_Frame::SetBackgroundColor(TFloat Param_fAlpha, TFloat Param_fRed, TFloat Param_fGreen, TFloat Param_fBlue)
{
	CWidget* pWidget_Background;

	pWidget_Background = GetSubWidget(m_WID_Background);
	if (pWidget_Background != NULL)
	{
		pWidget_Background->Get_Color()->FadeTo(Param_fAlpha, Param_fRed, Param_fGreen, Param_fBlue, kfMSecValueDelta_Instant);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_Frame::SetTopBarHeight(TFloat Param_fHeight)
{
	CWidget* pWidget;

	// Title

	pWidget = GetSubWidget(m_WID_Title);
	if (pWidget != NULL)
	{
		pWidget->Get_Size()->Get_Y()->Set(kfFont_Size_WindowTitle * Param_fHeight);
	}

	// Top

	pWidget = GetSubWidget(m_WID_Top);
	if (pWidget != NULL)
	{
		pWidget->Get_Size()->Get_Y()->Set(kfWindow_TopBarHeight * Param_fHeight);
	}

	// Top Left

	pWidget = GetSubWidget(m_WID_Top_Left);
	if (pWidget != NULL)
	{
		pWidget->Get_Size()->Get_Y()->Set(kfWindow_TopBarHeight * Param_fHeight);
	}

	// Top Right

	pWidget = GetSubWidget(m_WID_Top_Right);
	if (pWidget != NULL)
	{
		pWidget->Get_Size()->Get_Y()->Set(kfWindow_TopBarHeight * Param_fHeight);
	}

	// Bottom

	pWidget = GetSubWidget(m_WID_Bottom);
	if (pWidget != NULL)
	{
		pWidget->Get_Size()->Get_Y()->Set(kfWindow_TopBarHeight * Param_fHeight);
	}

	// Bottom Left

	pWidget = GetSubWidget(m_WID_Bottom_Left);
	if (pWidget != NULL)
	{
		pWidget->Get_Size()->Get_Y()->Set(kfWindow_TopBarHeight * Param_fHeight);
		pWidget->Get_Position()->Get_Y()->Set(0.9998f - (Param_fHeight * 0.0081f));
	}

	// Bottom Right

	pWidget = GetSubWidget(m_WID_Bottom_Right);
	if (pWidget != NULL)
	{
		pWidget->Get_Size()->Get_Y()->Set(kfWindow_TopBarHeight * Param_fHeight);
		pWidget->Get_Position()->Get_Y()->Set(0.9998f - (Param_fHeight * 0.0081f));
	}
}

// ***********************************************************************************************************************

void CWidget_Base_Frame::SetTitleWidth(TFloat Param_fWidth)
{
	CWidget* pWidget = GetSubWidget(m_WID_Title);

	if (pWidget != NULL)
	{
		pWidget->Get_Size()->Get_X()->Set(kfFont_Size_WindowTitle * Param_fWidth);
	}

}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_Panel
// ***********************************************************************************************************************

CWidget_Base_Panel::CWidget_Base_Panel()
{
	Initialize();
}

// ***********************************************************************************************************************

CWidget_Base_Panel::CWidget_Base_Panel(TPWString Param_pTitle)
{
	Initialize(Param_pTitle);
}

// ***********************************************************************************************************************

CWidget_Base_Panel::~CWidget_Base_Panel()
{
}

// ***********************************************************************************************************************

void CWidget_Base_Panel::Initialize(TPWString Param_pTitle)
{
	// Create SubWidget - Frame

	CWidget_Base_Frame* pWidget_Frame = new CWidget_Base_Frame(Param_pTitle);

	m_WID_Frame = AddSubWidget(pWidget_Frame);
}

// ***********************************************************************************************************************

void CWidget_Base_Panel::SetTitle(TPWString Param_pTitle)
{
	CWidget_Base_Frame* pWidget_Frame;

	pWidget_Frame = (CWidget_Base_Frame*)GetSubWidget(m_WID_Frame);
	if (pWidget_Frame != NULL)
	{
		pWidget_Frame->SetTitle(Param_pTitle);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_Panel::SetBackgroundColor(TFloat Param_fAlpha, TFloat Param_fRed, TFloat Param_fGreen, TFloat Param_fBlue)
{
	CWidget_Base_Frame* pWidget_Frame;

	pWidget_Frame = (CWidget_Base_Frame*)GetSubWidget(m_WID_Frame);
	if (pWidget_Frame != NULL)
	{
		pWidget_Frame->SetBackgroundColor(Param_fAlpha, Param_fRed, Param_fGreen, Param_fBlue);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_Panel::SetTopBarHeight(TFloat Param_fHeight)
{
	CWidget_Base_Frame* pWidget_Frame;

	pWidget_Frame = (CWidget_Base_Frame*)GetSubWidget(m_WID_Frame);
	if (pWidget_Frame != NULL)
	{
		pWidget_Frame->SetTopBarHeight(Param_fHeight);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_Panel::Signal(eNavLink Param_eNavLink)
{
	// Parent Signal

	__super::Signal(Param_eNavLink);
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_ScrollBar
// ***********************************************************************************************************************

CWidget_Base_ScrollBar::CWidget_Base_ScrollBar()
{
	UpdateValues(0, 0, 0);

	// Create SubWidget - Background

	CWidget_Base_ColorQuad* pWidget_Background = new CWidget_Base_ColorQuad(kiColor_ScrollBar_Background);

	m_WID_Background = AddSubWidget(pWidget_Background);
	
	// Create SubWidget - Foreground

	CWidget_Base_ColorQuad* pWidget_Slider = new CWidget_Base_ColorQuad(kiColor_ScrollBar_Slider);

	pWidget_Slider->Get_Position()->Set(kfFrame_ScrollBar_X, kfFrame_ScrollBar_Y);
	pWidget_Slider->Get_Size()->Set(1.0f - (2.0f * kfFrame_ScrollBar_X), 1.0f - (2.0f * kfFrame_ScrollBar_Y));

	m_WID_Slider = AddSubWidget(pWidget_Slider);
}

// ***********************************************************************************************************************

CWidget_Base_ScrollBar::~CWidget_Base_ScrollBar()
{
}

// ***********************************************************************************************************************

void CWidget_Base_ScrollBar::Draw(stRenderBase* Param_pstRenderBase)
{
	// Parent Draw Operations

	__super::Draw(Param_pstRenderBase);

	// Update Bar

	CWidget_Base_ColorQuad* pWidget_Background	= (CWidget_Base_ColorQuad*)GetSubWidget(m_WID_Background);
	CWidget_Base_ColorQuad* pWidget_Slider		= (CWidget_Base_ColorQuad*)GetSubWidget(m_WID_Slider);

	if ((pWidget_Background == NULL) || (pWidget_Slider == NULL))
	{
		return;
	}

	TFloat SizeRatio;
	TFloat PositionRatio;

	if (m_iDisplayedEntryCount < m_iTotalEntryCount)
	{
		SizeRatio		= ((TFloat)m_iDisplayedEntryCount / (TFloat)m_iTotalEntryCount);
		PositionRatio	= (1.0f - SizeRatio) * ((TFloat)m_iStartEntry / (TFloat)(m_iTotalEntryCount - m_iDisplayedEntryCount));
	}
	else
	{
		SizeRatio		= 1.0f;
		PositionRatio	= 0.0f;
	}

	TFloat Size_Y		= SizeRatio * (pWidget_Background->Get_Size()->GetValue_Y() - (2.0f * kfFrame_ScrollBar_Y));
	TFloat Position_Y	= PositionRatio * (pWidget_Background->Get_Size()->GetValue_Y() - (2.0f * kfFrame_ScrollBar_Y));

	pWidget_Slider->Get_Size()->Get_Y()->Set(Size_Y);
	pWidget_Slider->Get_Position()->Get_Y()->Set(pWidget_Background->Get_Position()->GetValue_Y() + kfFrame_ScrollBar_Y + Position_Y);
}

// ***********************************************************************************************************************

void CWidget_Base_ScrollBar::UpdateValues(TInt Param_iStartEntry, TInt Param_iTotalEntryCount, TInt Param_iDisplayedEntryCount)
{
	// Ensure Valid Values

	// - Negative

	if (Param_iTotalEntryCount < 0)
	{
		Param_iTotalEntryCount = 0;
	}

	if (Param_iDisplayedEntryCount < 0)
	{
		Param_iDisplayedEntryCount = 0;
	}

	if (Param_iStartEntry < 0)
	{
		Param_iStartEntry = 0;
	}

	// - Entry Counts

	if (Param_iDisplayedEntryCount > Param_iTotalEntryCount)
	{
		Param_iDisplayedEntryCount = Param_iTotalEntryCount;
	}

	if (Param_iStartEntry > Param_iTotalEntryCount)
	{
		Param_iStartEntry = Param_iTotalEntryCount;
	}

	// Update Values

	m_iStartEntry			= Param_iStartEntry;
	m_iTotalEntryCount		= Param_iTotalEntryCount;
	m_iDisplayedEntryCount	= Param_iDisplayedEntryCount;
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_StringList
// ***********************************************************************************************************************

CWidget_Base_StringList::CWidget_Base_StringList()
{
	Initialize(0);
}

// ***********************************************************************************************************************

CWidget_Base_StringList::CWidget_Base_StringList(TInt Param_iMaximumSize)
{
	Initialize(Param_iMaximumSize);
}

// ***********************************************************************************************************************

CWidget_Base_StringList::~CWidget_Base_StringList()
{
	ResetMaximumListSize(0);
}

// ***********************************************************************************************************************

void CWidget_Base_StringList::Draw(stRenderBase* Param_pstRenderBase)
{
	TInt				Loop;
	stStringList_Entry*	pstEntry;
	TFloat				StringSize_X;
	TFloat				StringSize_Y;
	TInt				DisplayedLines;

	TFloat				Offset_Y = 0.0f;

	// Parent Draw Operations

	__super::Draw(Param_pstRenderBase);

	// Find Scrolling List Start Position

	TInt StartPosition = (TInt)(m_ScrollPosition.GetValue_Y() * (TFloat)m_iListSize_Current);

	if (StartPosition < 0)
	{
		StartPosition = 0;
	}

	if (StartPosition >= m_iListSize_Current)
	{
		StartPosition = m_iListSize_Current - 1;
	}

	// Draw List

	DisplayedLines = 0;

	for (Loop = StartPosition; Loop < m_iListSize_Current; Loop++)
	{
		pstEntry = GetEntry(Loop);

		if (pstEntry != NULL)
		{
			if (pstEntry->pText != NULL)
			{
				// Get String Size

				GetStringSize(&StringSize_X, &StringSize_Y, pstEntry->pText->GetString(), Param_pstRenderBase->stCollisionBox.fSize_X * m_TextSize.Get(), Param_pstRenderBase->stCollisionBox.fSize_Y * m_TextSize.Get(), kiFontID_UsedForText, m_Clipping.GetValue_X(), m_Clipping.GetValue_Y());

				// Terminate Drawing Before Reaching Bottom of Widget

				if ((Offset_Y + StringSize_Y) >= (Param_pstRenderBase->stCollisionBox.fSize_Y))
				{
					Loop = m_iListSize_Current;
				}
				else
				{
					// Compute Color

					CColor Color_Entry(pstEntry->stType.iColor);

					Color_Entry.Set(Color_Entry.GetValue_Alpha() * Param_pstRenderBase->fColor_Alpha, Color_Entry.GetValue_Red() * Param_pstRenderBase->fColor_Red, Color_Entry.GetValue_Green() * Param_pstRenderBase->fColor_Green, Color_Entry.GetValue_Blue() * Param_pstRenderBase->fColor_Blue);

					// Draw String

					DrawString(pstEntry->pText->GetString(), Param_pstRenderBase->stCollisionBox.fPosition_X, Param_pstRenderBase->stCollisionBox.fPosition_Y + Offset_Y, Param_pstRenderBase->stCollisionBox.fSize_X * m_TextSize.Get(), Param_pstRenderBase->stCollisionBox.fSize_Y * m_TextSize.Get(), Color_Entry.Get_ARGB(), GetBlending(), kiFontID_UsedForText, m_Clipping.GetValue_X(), m_Clipping.GetValue_Y());

					// Increment Y Position by String Height

					Offset_Y += StringSize_Y;

					DisplayedLines++;
				}
			}
		}
	}

	// Update Scroll Bar

	CWidget_Base_ScrollBar* pWidget_ScrollBar = (CWidget_Base_ScrollBar*)GetSubWidget(m_WID_ScrollBar);

	if (pWidget_ScrollBar != NULL)
	{
		pWidget_ScrollBar->UpdateValues(StartPosition, m_iListSize_Current, DisplayedLines);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_StringList::Initialize(TInt Param_iMaximumSize)
{
	// Initialize Fields

	m_iListSize_Current	= 0;
	m_iListSize_Maximum	= 0;
	m_pstList			= NULL;
	m_iFlags_Widget		|= kiFlags_Widget_AcquireFocusOnCreate;

	ResetMaximumListSize(Param_iMaximumSize);

	// Create SubWidget - Scroll Bar

	CWidget_Base_ScrollBar* pWidget_ScrollBar = new CWidget_Base_ScrollBar();

	pWidget_ScrollBar->Get_Position()->Set(0.95f, 0.05f);
	pWidget_ScrollBar->Get_Size()->Set(0.04f, 0.9f);

	m_WID_ScrollBar = AddSubWidget(pWidget_ScrollBar);	
}

// ***********************************************************************************************************************

void CWidget_Base_StringList::ResetMaximumListSize(TInt Param_iMaximumSize)
{
	// Empty List

	Clear();

	// Free Old List (If Applicable)

	if ((m_iListSize_Maximum > 0) && (m_pstList != NULL))
	{
		Memory_Free(m_pstList);
		m_pstList = NULL;
	}

	// Assign New Size

	m_iListSize_Maximum	= Param_iMaximumSize;

	// Allocate New List (If Applicable)

	if ((m_iListSize_Maximum > 0) && (m_pstList == NULL))
	{
		m_pstList = (stStringList_Entry*)Memory_Allocate(m_iListSize_Maximum * sizeof(stStringList_Entry));
	}
}

// ***********************************************************************************************************************

void CWidget_Base_StringList::Clear()
{
	TInt				Loop;
	stStringList_Entry*	pstEntry;

	for (Loop = 0; Loop < m_iListSize_Current; Loop++)
	{
		pstEntry = GetEntry(Loop);

		if (pstEntry != NULL)
		{
			if (pstEntry->pText != NULL)
			{
				delete pstEntry->pText;
			}
		}
	}

	m_iListSize_Current	= 0;
	m_iListStartIndex	= 0;

	m_ScrollPosition.Set(0.0f, 0.0f);
}

// ***********************************************************************************************************************

void CWidget_Base_StringList::AddString(TPWString Param_pString, stStringList_EntryType Param_stEntryType)
{
	// Error Checking

	if ((m_iListSize_Maximum <= 0) || (m_pstList == NULL))
	{
		return;
	}

	// Add / Replace Element

	if (m_iListSize_Current < m_iListSize_Maximum)
	{
		// Add New

		stStringList_Entry stEntry;

		stEntry.pText	= new CText(Param_pString);
		stEntry.stType	= Param_stEntryType;

		m_iListSize_Current++;
		SetEntry(m_iListSize_Current - 1, &stEntry);
	}
	else
	{
		// Replace First

		stStringList_Entry* pstEntry = GetEntry(0);

		if (pstEntry != NULL)
		{
			// Update Type

			pstEntry->stType = Param_stEntryType;

			// Update Text

			if (pstEntry->pText != NULL)
			{
				pstEntry->pText->SetTo(Param_pString);
			}
		}

		m_iListStartIndex++;
	}
}

// ***********************************************************************************************************************

void CWidget_Base_StringList::AddString(TPWString Param_pString)
{
	AddString(Param_pString, kstStringList_EntryType_Basic);
}

// ***********************************************************************************************************************

stStringList_Entry* CWidget_Base_StringList::GetEntry(TInt Param_iCircularEntryIndex)
{
	// Error Checking

	if ((Param_iCircularEntryIndex < 0) || (Param_iCircularEntryIndex >= m_iListSize_Current))
	{
		return NULL;
	}

	// Wrap Around (If Applicable)

	while ((m_iListStartIndex + Param_iCircularEntryIndex) >= m_iListSize_Maximum)
	{
		Param_iCircularEntryIndex -= m_iListSize_Maximum;
	}

	// Straight Fetch

	return &m_pstList[m_iListStartIndex + Param_iCircularEntryIndex];
}

// ***********************************************************************************************************************

void CWidget_Base_StringList::SetEntry(TInt Param_iCircularEntryIndex, stStringList_Entry* Param_pstEntry)
{
	// Error Checking

	if ((Param_iCircularEntryIndex < 0) || (Param_iCircularEntryIndex >= m_iListSize_Current))
	{
		return;
	}

	// Wrap Around (If Applicable)

	while ((m_iListStartIndex + Param_iCircularEntryIndex) >= m_iListSize_Maximum)
	{
		Param_iCircularEntryIndex -= m_iListSize_Maximum;
	}

	// Straight Store

	m_pstList[m_iListStartIndex + Param_iCircularEntryIndex] = *Param_pstEntry;
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_OptionList
// ***********************************************************************************************************************

CWidget_Base_OptionList::CWidget_Base_OptionList(TFloat Param_fPosition_Value_X)
{
	// Initialize Fields

	m_iListSize			= 0;
	m_pstList			= NULL;
	m_iCurrentSelected	= kiListID_Invalid;
	m_fPosition_Value_X = Param_fPosition_Value_X;
	m_iFlags_Widget		|= kiFlags_Widget_AcquireFocusOnCreate;

	m_ScrollPosition.Set(0.5f, 0.5f);

	SetNavLink(eNavLink_Forward		, kiWidgetID_Signal);
	SetNavLink(eNavLink_Skip_Back	, kiWidgetID_Signal);
	SetNavLink(eNavLink_Skip_Forward, kiWidgetID_Signal);

	// Create SubWidget - Scroll Bar

	CWidget_Base_ScrollBar* pWidget_ScrollBar = new CWidget_Base_ScrollBar();

	pWidget_ScrollBar->Get_Position()->Set(0.95f, 0.05f);
	pWidget_ScrollBar->Get_Size()->Set(0.04f, 0.9f);

	m_WID_ScrollBar = AddSubWidget(pWidget_ScrollBar);

	// Create SubWidget - Selection Bar

	CWidget_Base_ColorQuad* pWidget_ColorQuad = new CWidget_Base_ColorQuad(kiColor_OptionList_SelectionBar);

	pWidget_ColorQuad->Get_Position()->Set(0.001f, -1.0f);
	pWidget_ColorQuad->Get_Size()->Set(0.92f, kfOption_Spacing_Y);

	m_WID_SelectionBar = AddSubWidget(pWidget_ColorQuad);
}

// ***********************************************************************************************************************

CWidget_Base_OptionList::~CWidget_Base_OptionList()
{
	if (m_pstList != NULL)
	{
		Memory_Free(m_pstList);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_OptionList::Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged)
{
	// Parent Tick

	__super::Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);

	// Reject Further Processing If Not Visible (Saves CPU Cycles)

	if (!IsVisible())
	{
		return;
	}

	// Update Tracker Values

	stOptionList_Entry* pstOption;

	for (TInt Loop = 0; Loop < m_iListSize; Loop++)
	{
		pstOption = &m_pstList[Loop];

		CWidget_Base_ValueTracker* pWidget = (CWidget_Base_ValueTracker*)GetSubWidget(pstOption->iWidgetID);

		if ((pWidget != NULL) && (pstOption->pLinkedVariable != NULL))
		{
			switch(pstOption->iStorageType)
			{
				case kiTrackerStorage_Integer:
				{
					pWidget->UpdateValue(*((TInt*)pstOption->pLinkedVariable));
					break;
				}

				case kiTrackerStorage_Float:
				{
					pWidget->UpdateValue(*((TFloat*)pstOption->pLinkedVariable));
					break;
				}

				case kiTrackerStorage_Boolean:
				{
					if (*((TBool*)pstOption->pLinkedVariable))
					{
						pWidget->Get_Color()->Get_Green()->Set(1.00f);
						pWidget->Get_Color()->Get_Red()->Set(0.70f);
						pWidget->Get_Color()->Get_Blue()->Set(0.70f);
					}
					else
					{
						pWidget->Get_Color()->Get_Green()->Set(0.70f);
						pWidget->Get_Color()->Get_Red()->Set(1.00f);
						pWidget->Get_Color()->Get_Blue()->Set(0.70f);
					}

					pWidget->UpdateValue(*((TBool*)pstOption->pLinkedVariable));
					break;
				}

				default:
				case kiTrackerStorage_None:
				{
					break;
				}
			}
		}
	}

	// Change Selected Option (If Vertical Scrolling Detected)

	while (m_ScrollPosition.GetValue_Y() >= (0.495f + kfOptionList_Scroll_Threshold_Y))
	{
		SelectOption(kiListID_NextEntry);

		m_ScrollPosition.Get_Y()->Set(0.500f);
	}

	while (m_ScrollPosition.GetValue_Y() <= (0.505f - kfOptionList_Scroll_Threshold_Y))
	{
		SelectOption(kiListID_PreviousEntry);

		m_ScrollPosition.Get_Y()->Set(0.500f);
	}

	// Change Option Value (If Horizontal Scrolling Detected)

	while (m_ScrollPosition.GetValue_X() >= (0.495f + kfOptionList_Scroll_Threshold_X))
	{
		SlideOptionValue(eNavLink_Right);

		m_ScrollPosition.Get_X()->Set(0.500f);
	}

	while (m_ScrollPosition.GetValue_X() <= (0.505f - kfOptionList_Scroll_Threshold_X))
	{
		SlideOptionValue(eNavLink_Left);

		m_ScrollPosition.Get_X()->Set(0.500f);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_OptionList::SlideOptionValue(eNavLink Param_eDirection)
{
	stOptionList_Entry* pstOption = GetSelectedOption();

	if (pstOption == NULL)
	{
		return;
	}

	CWidget_Base_ValueTracker* pWidget = (CWidget_Base_ValueTracker*)GetSubWidget(pstOption->iWidgetID);

	if ((pWidget != NULL) && (pstOption->pLinkedVariable != NULL))
	{
		switch(pstOption->iStorageType)
		{
			case kiTrackerStorage_Integer:
			{
				TInt*	piValue		= (TInt*)pstOption->pLinkedVariable;
				TInt	iValue_Old	= *piValue;

				switch (Param_eDirection)
				{
					case eNavLink_Left:
					{
						// Decrement Value By Delta

						*piValue -= pstOption->fiValue_Delta.AsInt;

						// Clamp (If Applicable)

						if (*piValue < pstOption->fiValue_Minimum.AsInt)
						{
							*piValue = pstOption->fiValue_Minimum.AsInt;
						}

						break;
					}

					case eNavLink_Right:
					{
						// Increment Value By Delta

						*piValue += pstOption->fiValue_Delta.AsInt;

						// Clamp (If Applicable)

						if (*piValue > pstOption->fiValue_Maximum.AsInt)
						{
							*piValue = pstOption->fiValue_Maximum.AsInt;
						}

						break;
					}

					case eNavLink_Forward:
					{
						// Wrap Around (If Applicable)

						if (*piValue >= pstOption->fiValue_Maximum.AsInt)
						{
							*piValue = pstOption->fiValue_Minimum.AsInt;
						}
						else
						{
							// Increment Value By Delta

							*piValue += pstOption->fiValue_Delta.AsInt;

							// Clamp (If Applicable)

							if (*piValue > pstOption->fiValue_Maximum.AsInt)
							{
								*piValue = pstOption->fiValue_Maximum.AsInt;
							}
						}

						break;
					}
				}

				// Trigger Notify Callback (If Applicable)

				if ((pstOption->fnNotifyCallback != NULL) && (*piValue != iValue_Old))
				{
					((fnOptionNotify_Int)pstOption->fnNotifyCallback)(piValue, iValue_Old, m_iCurrentSelected);
				}

				break;
			}

			case kiTrackerStorage_Float:
			{
				TFloat*	pfValue		= (TFloat*)pstOption->pLinkedVariable;
				TFloat	fValue_Old	= *pfValue;

				switch (Param_eDirection)
				{
					case eNavLink_Left:
					{
						// Decrement Value By Delta

						*pfValue -= pstOption->fiValue_Delta.AsFloat;

						// Clamp (If Applicable)

						if (*pfValue < pstOption->fiValue_Minimum.AsFloat)
						{
							*pfValue = pstOption->fiValue_Minimum.AsFloat;
						}

						break;
					}

					case eNavLink_Right:
					{
						// Increment Value By Delta

						*pfValue += pstOption->fiValue_Delta.AsFloat;

						// Clamp (If Applicable)

						if (*pfValue > pstOption->fiValue_Maximum.AsFloat)
						{
							*pfValue = pstOption->fiValue_Maximum.AsFloat;
						}

						break;
					}

					case eNavLink_Forward:
					{
						// Wrap Around (If Applicable)

						if (*pfValue >= pstOption->fiValue_Maximum.AsFloat)
						{
							*pfValue = pstOption->fiValue_Minimum.AsFloat;
						}
						else
						{
							// Increment Value By Delta

							*pfValue += pstOption->fiValue_Delta.AsFloat;

							// Clamp (If Applicable)

							if (*pfValue > pstOption->fiValue_Maximum.AsFloat)
							{
								*pfValue = pstOption->fiValue_Maximum.AsFloat;
							}
						}

						break;
					}
				}

				// Trigger Notify Callback (If Applicable)

				if ((pstOption->fnNotifyCallback != NULL) && (*pfValue != fValue_Old))
				{
					((fnOptionNotify_Float)pstOption->fnNotifyCallback)(pfValue, fValue_Old, m_iCurrentSelected);
				}

				break;
			}

			case kiTrackerStorage_Boolean:
			{
				TBool* pbValue = (TBool*)pstOption->pLinkedVariable;

				// Flip Current Value

				*pbValue = !*pbValue;

				// Trigger Notify Callback (If Applicable)

				if (pstOption->fnNotifyCallback != NULL)
				{
					((fnOptionNotify_Bool)pstOption->fnNotifyCallback)(pbValue, m_iCurrentSelected);
				}

				break;
			}

			default:
			case kiTrackerStorage_None:
			{
				break;
			}
		}
	}
}

// ***********************************************************************************************************************

void CWidget_Base_OptionList::Signal(eNavLink Param_eNavLink)
{
	switch(Param_eNavLink)
	{
		// Update Option

		case eNavLink_Forward:
		{
			SlideOptionValue(eNavLink_Forward);
			break;
		}

		// Scan To Previous Title

		case eNavLink_Skip_Back:
		{
			SelectOption(kiListID_PreviousTitle);
			break;
		}

		// Scan To Next Title

		case eNavLink_Skip_Forward:
		{
			SelectOption(kiListID_NextTitle);
			break;
		}
	}
}

// ***********************************************************************************************************************

TListID CWidget_Base_OptionList::AddOption(TPWString Param_pName, stTrackerType Param_stType, void* Param_pLinkedVariable)
{
	TListID				iListID;
	stOptionList_Entry	stNewOption;

	iListID = m_iListSize;

	// Backup Old List

	stOptionList_Entry* pstOldList = m_pstList;

	// Allocate New List

	m_pstList = (stOptionList_Entry*)Memory_Allocate((m_iListSize + 1) * sizeof(stOptionList_Entry));

	// Transfer From Old List (If Applicable)

	if (pstOldList != NULL)
	{
		for (TListID Loop = 0; Loop < m_iListSize; Loop++)
		{
			m_pstList[Loop] = pstOldList[Loop];
		}

		Memory_Free(pstOldList);
	}

	// Increase List Size

	m_iListSize++;

	// Create New ValueTracker Widget

	CWidget* pWidget;

	TFloat fPosition_Y = kfOptionList_Offset_Y + (iListID * kfOption_Spacing_Y);

	if (Param_pLinkedVariable == NULL)
	{
		// Title

		pWidget = new CWidget_Base_ListHeader(Param_pName);

		((CWidget_Base_ListHeader*)pWidget)->Get_Position()->Set(0.01f, fPosition_Y);
		((CWidget_Base_ListHeader*)pWidget)->SetBarSize(0.90f, kfOption_Spacing_Y);
		((CWidget_Base_ListHeader*)pWidget)->SetTextSize(kfOption_Text_Size);
	}
	else
	{
		// Option

		pWidget = new CWidget_Base_ValueTracker(Param_pName, Param_stType);

		((CWidget_Base_ValueTracker*)pWidget)->SetPositions(0.01f, m_fPosition_Value_X, fPosition_Y);
		((CWidget_Base_ValueTracker*)pWidget)->SetTextSize(kfOption_Text_Size);
		((CWidget_Base_ValueTracker*)pWidget)->SetGlyphSize(kfOption_Text_Size, kfOption_Text_Size);

		switch(Param_stType.iStorage)
		{
			case kiTrackerStorage_Integer:
			{
				if (Param_stType.pstEnum != NULL)
				{
					stNewOption.fiValue_Minimum.AsInt	= Param_stType.pstEnum->iFirstIndex;
					stNewOption.fiValue_Maximum.AsInt	= (Param_stType.pstEnum->iFirstIndex + (Param_stType.pstEnum->iCount - 1));
					stNewOption.fiValue_Delta.AsInt		= 1;

					pWidget->Get_Color()->Set(kiColor_OptionList_ItemEnum);
				}
				else
				{
					stNewOption.fiValue_Minimum.AsInt	= 0;
					stNewOption.fiValue_Maximum.AsInt	= 10;
					stNewOption.fiValue_Delta.AsInt		= 1;

					pWidget->Get_Color()->Set(kiColor_OptionList_ItemInt);
				}
				break;
			}

			case kiTrackerStorage_Float:
			{
				stNewOption.fiValue_Minimum.AsFloat	= 0.0f;
				stNewOption.fiValue_Maximum.AsFloat	= 1.0f;
				stNewOption.fiValue_Delta.AsFloat	= 0.1f;

				pWidget->Get_Color()->Set(kiColor_OptionList_ItemFloat);
				break;
			}

			default:
			case kiTrackerStorage_Boolean:
			case kiTrackerStorage_None:
			{
				pWidget->Get_Color()->Set(kiColor_OptionList_ItemDefault);
				break;
			}
		}
	}

	// Create As Invisible If Currently Outside Window

	if (fPosition_Y >= (1.0f - kfOption_Spacing_Y))
	{
		pWidget->Get_Color()->Get_Alpha()->Set(0.0f);
	}

	// Populate Entry Struct

	stNewOption.pLinkedVariable		= Param_pLinkedVariable;
	stNewOption.iStorageType		= Param_stType.iStorage;
	stNewOption.iWidgetID			= AddSubWidget(pWidget);
	stNewOption.fnNotifyCallback	= NULL;

	// Store New Entry

	m_pstList[iListID] = stNewOption;

	// Select First Option By Default

	if (GetSelectedOption() == NULL)
	{
		SelectOption(kiListID_FirstEntry);
	}

	return iListID;
}

// ***********************************************************************************************************************

stOptionList_Entry* CWidget_Base_OptionList::GetOption(TListID Param_iListID)
{
	// Error Checking

	if ((m_pstList == NULL) || (Param_iListID < 0) || (Param_iListID >= m_iListSize))
	{
		return NULL;
	}

	return &m_pstList[Param_iListID];
}

// ***********************************************************************************************************************

stOptionList_Entry* CWidget_Base_OptionList::GetSelectedOption()
{
	return GetOption(m_iCurrentSelected);
}

// ***********************************************************************************************************************

TBool CWidget_Base_OptionList::SelectOption(TListID Param_iListID)
{
	TListID				iListID;
	stOptionList_Entry*	pstOptionEntry;

	switch (Param_iListID)
	{
		case kiListID_FirstEntry:
		{
			iListID = -1;

			// Scan For Non-Title Entry (Forward)

			do
			{
				iListID++;
				pstOptionEntry = GetOption(iListID);
			}
			while ((pstOptionEntry != NULL) && (pstOptionEntry->pLinkedVariable == NULL));

			break;
		}

		case kiListID_LastEntry:
		{
			iListID = m_iListSize;

			// Scan For Non-Title Entry (Backward)

			do
			{
				iListID--;
				pstOptionEntry = GetOption(iListID);
			}
			while ((pstOptionEntry != NULL) && (pstOptionEntry->pLinkedVariable == NULL));

			break;
		}

		case kiListID_NextEntry:
		{
			iListID = m_iCurrentSelected;

			// Scan For Non-Title Entry (Forward)

			do
			{
				iListID++;
				pstOptionEntry = GetOption(iListID);
			}
			while ((pstOptionEntry != NULL) && (pstOptionEntry->pLinkedVariable == NULL));

			break;
		}

		case kiListID_PreviousEntry:
		{
			iListID = m_iCurrentSelected;

			// Scan For Non-Title Entry (Backward)

			do
			{
				iListID--;
				pstOptionEntry = GetOption(iListID);
			}
			while ((pstOptionEntry != NULL) && (pstOptionEntry->pLinkedVariable == NULL));

			break;
		}

		case kiListID_NextTitle:
		{
			iListID = m_iCurrentSelected;

			// Scan For Title Entry (Forward)

			do
			{
				iListID++;
				pstOptionEntry = GetOption(iListID);
			}
			while ((pstOptionEntry != NULL) && (pstOptionEntry->pLinkedVariable != NULL));

			// Scan For Next Non-Title Entry (Forward)

			m_iCurrentSelected = iListID;

			if (SelectOption(kiListID_NextEntry))
			{
				return TRUE;
			}
			else
			{
				return SelectOption(kiListID_LastEntry);
			}

			break;
		}

		case kiListID_PreviousTitle:
		{
			iListID = m_iCurrentSelected;

			// Scan For Title Entry (Backward)

			do
			{
				iListID--;
				pstOptionEntry = GetOption(iListID);
			}
			while ((pstOptionEntry != NULL) && (pstOptionEntry->pLinkedVariable != NULL));

			// Scan For Next Non-Title Entry (Backward)

			m_iCurrentSelected = iListID;

			if (SelectOption(kiListID_PreviousEntry))
			{
				return TRUE;
			}
			else
			{
				return SelectOption(kiListID_FirstEntry);
			}

			break;
		}

		default:
		{
			iListID			= Param_iListID;
			pstOptionEntry	= GetOption(iListID);

			// Check If Entry Is A Title

			if ((pstOptionEntry != NULL) && (pstOptionEntry->pLinkedVariable == NULL))
			{
				// Scan For Non-Title Entry (Forward)

				do
				{
					iListID++;
					pstOptionEntry = GetOption(iListID);
				}
				while ((pstOptionEntry != NULL) && (pstOptionEntry->pLinkedVariable == NULL));
			}

			break;
		}
	}

	// Update Selected Option ID

	if (GetOption(iListID) != NULL)
	{
		TFloat fOffset_Y;

		m_iCurrentSelected = iListID;

		// Calculate Vertical Offset (With Top / Bottom Scroll Clamping)

		if (((m_iCurrentSelected * kfOption_Spacing_Y) > 0.5f) && ((m_iListSize * kfOption_Spacing_Y) >= 1.0f - kfOption_Spacing_Y))
		{
			if (((m_iListSize - m_iCurrentSelected + 1) * kfOption_Spacing_Y) < 0.5f)
			{
				fOffset_Y = -(((m_iListSize + 1) * kfOption_Spacing_Y) - 1.0f);
			}
			else
			{
				fOffset_Y = 0.5f - (m_iCurrentSelected * kfOption_Spacing_Y);
			}
		}
		else
		{
			fOffset_Y = 0.0f;
		}

		// Update Option Positions

		for (TListID iLoop = 0; iLoop < m_iListSize; iLoop++)
		{
			CWidget_Base_ValueTracker* pWidget = (CWidget_Base_ValueTracker*)GetSubWidget(m_pstList[iLoop].iWidgetID);

			if (pWidget != NULL)
			{
				TFloat fPosition_Y = kfOptionList_Offset_Y + (iLoop * kfOption_Spacing_Y) + fOffset_Y;

				// Update Alpha

				if ((fPosition_Y < 0.0f) || (fPosition_Y >= (1.0f - kfOption_Spacing_Y)))
				{
					pWidget->Get_Color()->Get_Alpha()->Set(0.0f);
				}
				else
				{
					pWidget->Get_Color()->Get_Alpha()->InterpolateTo(1.0f, kfMSecValueDelta_SelectionBarFade);
				}

				// Update Position

				pWidget->Get_Position()->Get_Y()->InterpolateTo(fPosition_Y, kfMSecValueDelta_SelectionBarMove);
			}
		}

		// Update Selection Bar Position

		CWidget_Base_ColorQuad* pWidget_SelectionBar = (CWidget_Base_ColorQuad*)GetSubWidget(m_WID_SelectionBar);

		pWidget_SelectionBar->Get_Position()->Get_Y()->InterpolateTo(kfOptionList_Offset_Y + (m_iCurrentSelected * kfOption_Spacing_Y) + fOffset_Y, kfMSecValueDelta_SelectionBarMove);

		// Update Scroll Bar

		CWidget_Base_ScrollBar* pWidget_ScrollBar = (CWidget_Base_ScrollBar*)GetSubWidget(m_WID_ScrollBar);

		pWidget_ScrollBar->UpdateValues((TInt)(-fOffset_Y / kfOption_Spacing_Y), m_iListSize, (TInt)((1.0f - kfOptionList_Offset_Y) / kfOption_Spacing_Y));

		return TRUE;
	}

	return FALSE;
}

// ***********************************************************************************************************************

void CWidget_Base_OptionList::SetLimits(TListID Param_iListID, TFloat Param_fMinimum, TFloat Param_fMaximum, TFloat Param_fDelta)
{
	// Fetch Option

	stOptionList_Entry* pstOption = GetOption(Param_iListID);

	if (pstOption == NULL)
	{
		return;
	}

	// Ensure Proper Range

	if (Param_fMinimum > Param_fMaximum)
	{
		TFloat fTemp	= Param_fMinimum;
		Param_fMinimum	= Param_fMaximum;
		Param_fMaximum	= fTemp;
	}

	// Update Values

	pstOption->fiValue_Minimum.AsFloat	= Param_fMinimum;
	pstOption->fiValue_Maximum.AsFloat	= Param_fMaximum;
	pstOption->fiValue_Delta.AsFloat	= fabs(Param_fDelta);
}

// ***********************************************************************************************************************

void CWidget_Base_OptionList::SetLimits(TListID Param_iListID, TInt Param_iMinimum, TInt Param_iMaximum, TInt Param_iDelta)
{
	// Fetch Option

	stOptionList_Entry* pstOption = GetOption(Param_iListID);

	if (pstOption == NULL)
	{
		return;
	}

	// Ensure Proper Range

	if (Param_iMinimum > Param_iMaximum)
	{
		TInt iTemp		= Param_iMinimum;
		Param_iMinimum	= Param_iMaximum;
		Param_iMaximum	= iTemp;
	}

	// Update Values

	pstOption->fiValue_Minimum.AsInt	= Param_iMinimum;
	pstOption->fiValue_Maximum.AsInt	= Param_iMaximum;
	pstOption->fiValue_Delta.AsInt		= abs(Param_iDelta);
}

// ***********************************************************************************************************************

void CWidget_Base_OptionList::SetNotifyCallback(TListID Param_iListID, void* Param_fnNotifyCallback)
{
	// Fetch Option

	stOptionList_Entry* pstOption = GetOption(Param_iListID);

	if (pstOption == NULL)
	{
		return;
	}

	// Update Callback Pointer

	pstOption->fnNotifyCallback = Param_fnNotifyCallback;
}

// ***********************************************************************************************************************

void CWidget_Base_OptionList::SetLinkedVariable(TListID Param_iListID, void* Param_pLinkedVariable)
{
	// Error Checking

	if (Param_pLinkedVariable == NULL)
	{
		return;
	}

	// Fetch Option

	stOptionList_Entry* pstOption = GetOption(Param_iListID);

	if (pstOption == NULL)
	{
		return;
	}

	// Update Callback Pointer

	pstOption->pLinkedVariable = Param_pLinkedVariable;
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_PanelList
// ***********************************************************************************************************************

CWidget_Base_PanelList::CWidget_Base_PanelList(TFloat Param_fPanelHeight)
{
	// Initialize Fields

	m_iFlags_Widget	|= kiFlags_Widget_AcquireFocusOnCreate;
	m_fPanelHeight	= Param_fPanelHeight;

	SetNavLink(eNavLink_Forward		, kiWidgetID_Signal);
	SetNavLink(eNavLink_Skip_Back	, kiWidgetID_Signal);
	SetNavLink(eNavLink_Skip_Forward, kiWidgetID_Signal);

	// Initialize List & Widgets

	Initialize();
}

// ***********************************************************************************************************************

CWidget_Base_PanelList::~CWidget_Base_PanelList()
{
	Destroy();
}

// ***********************************************************************************************************************

void CWidget_Base_PanelList::Initialize()
{
	// Initialize Fields

	m_iListSize			= 0;
	m_pstList			= NULL;
	m_iCurrentSelected	= kiListID_Invalid;

	m_ScrollPosition.Set(0.5f, 0.5f);

	// Error Checking

	if ((m_fPanelHeight <= 0.0f) || (m_fPanelHeight >= 1.0f))
	{
		m_fPanelHeight = 0.1f;
	}

	// Create SubWidget - Scroll Bar

	CWidget_Base_ScrollBar* pWidget_ScrollBar = new CWidget_Base_ScrollBar();

	pWidget_ScrollBar->Get_Position()->Set(0.95f, 0.05f);
	pWidget_ScrollBar->Get_Size()->Set(0.04f, 0.9f);

	m_WID_ScrollBar = AddSubWidget(pWidget_ScrollBar);

	// Create SubWidget - Selection Bar

	CWidget_Base_ColorQuad* pWidget_ColorQuad = new CWidget_Base_ColorQuad(kiColor_PanelList_SelectionBar);

	pWidget_ColorQuad->Get_Position()->Set(0.001f, -1.0f);
	pWidget_ColorQuad->Get_Size()->Set(0.92f, m_fPanelHeight + 0.01f);

	m_WID_SelectionBar = AddSubWidget(pWidget_ColorQuad);
}

// ***********************************************************************************************************************

void CWidget_Base_PanelList::Destroy()
{
	// Flush All Widgets

	m_SubWidgets.RemoveAllElements();

	// Deallocate List

	if (m_pstList != NULL)
	{
		Memory_Free(m_pstList);
		m_pstList = NULL;
	}
}

// ***********************************************************************************************************************

void CWidget_Base_PanelList::Reset()
{
	Destroy();
	Initialize();
}

// ***********************************************************************************************************************

void CWidget_Base_PanelList::Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged)
{
	// Parent Tick

	__super::Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);

	// Reject Further Processing If Not Visible (Saves CPU Cycles)

	if (!IsVisible())
	{
		return;
	}

	// Change Selected Panel (If Vertical Scrolling Detected)

	while (m_ScrollPosition.GetValue_Y() >= (0.495f + kfPanelList_Scroll_Threshold_Y))
	{
		SelectPanel(kiListID_NextEntry);

		m_ScrollPosition.Get_Y()->Set(0.500f);
	}

	while (m_ScrollPosition.GetValue_Y() <= (0.505f - kfPanelList_Scroll_Threshold_Y))
	{
		SelectPanel(kiListID_PreviousEntry);

		m_ScrollPosition.Get_Y()->Set(0.500f);
	}

	// Notify Panel (If Horizontal Scrolling Detected)

	while (m_ScrollPosition.GetValue_X() >= (0.495f + kfPanelList_Scroll_Threshold_X))
	{
		stPanelList_Entry* pstEntry = GetSelectedPanel();

		if (pstEntry != NULL)
		{
			CWidget_Base_Panel* pWidget_Panel = (CWidget_Base_Panel*)GetSubWidget(pstEntry->iWidgetID);

			if (pWidget_Panel != NULL)
			{
				pWidget_Panel->Signal(eNavLink_Right);
			}
		}

		m_ScrollPosition.Get_X()->Set(0.500f);
	}

	while (m_ScrollPosition.GetValue_X() <= (0.505f - kfPanelList_Scroll_Threshold_X))
	{
		stPanelList_Entry* pstEntry = GetSelectedPanel();

		if (pstEntry != NULL)
		{
			CWidget_Base_Panel* pWidget_Panel = (CWidget_Base_Panel*)GetSubWidget(pstEntry->iWidgetID);

			if (pWidget_Panel != NULL)
			{
				pWidget_Panel->Signal(eNavLink_Left);
			}
		}

		m_ScrollPosition.Get_X()->Set(0.500f);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_PanelList::Signal(eNavLink Param_eNavLink)
{
	switch(Param_eNavLink)
	{
		// Update Panel

		case eNavLink_Forward:
		{
			if ((m_iCurrentSelected >= 0) && (m_iCurrentSelected < m_iListSize))
			{
				CWidget_Base_Panel* pPanel = (CWidget_Base_Panel*)GetSubWidget(m_pstList[m_iCurrentSelected].iWidgetID);

				if (pPanel != NULL)
				{
					pPanel->Signal(Param_eNavLink);
				}
			}
			break;
		}

		// Scan To Previous Title

		case eNavLink_Skip_Back:
		{
			SelectPanel(kiListID_PreviousPage);
			break;
		}

		// Scan To Next Title

		case eNavLink_Skip_Forward:
		{
			SelectPanel(kiListID_NextPage);
			break;
		}
	}
}

// ***********************************************************************************************************************

TListID CWidget_Base_PanelList::AddPanel(CWidget_Base_Panel* Param_pPanel)
{
	// Error Checking

	if (Param_pPanel == NULL)
	{
		return kiListID_Invalid;
	}

	// Backup Old List

	TListID iListID = m_iListSize;

	stPanelList_Entry* pstOldList = m_pstList;

	// Allocate New List

	m_pstList = (stPanelList_Entry*)Memory_Allocate((m_iListSize + 1) * sizeof(stPanelList_Entry));

	// Transfer From Old List (If Applicable)

	if (pstOldList != NULL)
	{
		for (TListID Loop = 0; Loop < m_iListSize; Loop++)
		{
			m_pstList[Loop] = pstOldList[Loop];
		}

		Memory_Free(pstOldList);
	}

	// Increase List Size

	m_iListSize++;

	// Store New Entry

	m_pstList[iListID].iWidgetID = AddSubWidget(Param_pPanel);

	// Calculate Position

	TFloat fPosition_Y = kfPanelList_Offset_Y + (iListID * m_fPanelHeight);

	Param_pPanel->Get_Position()->Set(0.01f, fPosition_Y);
	Param_pPanel->Get_Size()->Set(0.90f, m_fPanelHeight - 0.01f);
	Param_pPanel->SetTopBarHeight(0.75f / m_fPanelHeight);

	// Create As Invisible If Currently Outside Window

	if (fPosition_Y >= (1.0f - m_fPanelHeight))
	{
		Param_pPanel->Get_Color()->Get_Alpha()->Set(0.0f);
	}

	// Select First Panel By Default (If Applicable) & Update Scroll Bar

	if (GetSelectedPanel() == NULL)
	{
		SelectPanel(kiListID_FirstEntry);
	}
	else
	{
		SelectPanel(kiListID_NoChange);
	}

	return iListID;
}

// ***********************************************************************************************************************

stPanelList_Entry* CWidget_Base_PanelList::GetPanel(TListID Param_iListID)
{
	// Error Checking

	if ((m_pstList == NULL) || (Param_iListID < 0) || (Param_iListID >= m_iListSize))
	{
		return NULL;
	}

	return &m_pstList[Param_iListID];
}

// ***********************************************************************************************************************

stPanelList_Entry* CWidget_Base_PanelList::GetSelectedPanel()
{
	return GetPanel(m_iCurrentSelected);
}

// ***********************************************************************************************************************

TListID CWidget_Base_PanelList::GetSelectedID()
{
	return m_iCurrentSelected;
}

// ***********************************************************************************************************************

TBool CWidget_Base_PanelList::SelectPanel(TListID Param_iListID)
{
	TListID	iListID;
	TFloat	fScrollSpeed = kfMSecValueDelta_PanelMove;

	switch (Param_iListID)
	{
		case kiListID_NoChange:
		{
			iListID = m_iCurrentSelected;
			break;
		}

		case kiListID_FirstEntry:
		{
			iListID = 0;
			break;
		}

		case kiListID_LastEntry:
		{
			iListID = m_iListSize - 1;
			break;
		}

		case kiListID_NextEntry:
		{
			iListID = m_iCurrentSelected + 1;

			// Clamp - High

			if (iListID >= m_iListSize)
			{
				iListID = m_iListSize - 1;
			}
			break;
		}

		case kiListID_PreviousEntry:
		{
			iListID = m_iCurrentSelected - 1;

			// Clamp - Low

			if (iListID < 0)
			{
				iListID = 0;
			}
			break;
		}

		case kiListID_NextPage:
		{
			iListID			= m_iCurrentSelected + 5;
			fScrollSpeed	*= 5.0f;

			// Clamp - High

			if (iListID >= m_iListSize)
			{
				iListID = m_iListSize - 1;
			}
			break;
		}

		case kiListID_PreviousPage:
		{
			iListID			= m_iCurrentSelected - 5;
			fScrollSpeed	*= 5.0f;

			// Clamp - Low

			if (iListID < 0)
			{
				iListID = 0;
			}
			break;
		}

		default:
		{
			iListID = Param_iListID;

			// Clamp - Low

			if (iListID < 0)
			{
				iListID = 0;
			}

			// Clamp - High

			if (iListID >= m_iListSize)
			{
				iListID = m_iListSize - 1;
			}

			break;
		}
	}

	// Update Selected Panel ID

	if (GetPanel(iListID) != NULL)
	{
		TFloat fOffset_Y;

		m_iCurrentSelected = iListID;

		// Calculate Vertical Offset (With Top / Bottom Scroll Clamping)

		if (((m_iCurrentSelected * m_fPanelHeight) > 0.5f) && ((m_iListSize * m_fPanelHeight) >= 1.0f - m_fPanelHeight))
		{
			if (((m_iListSize - m_iCurrentSelected + 1) * m_fPanelHeight) < 0.5f)
			{
				fOffset_Y = -(((m_iListSize + 1) * m_fPanelHeight) - 1.0f);
			}
			else
			{
				fOffset_Y = 0.5f - (m_iCurrentSelected * m_fPanelHeight);
			}
		}
		else
		{
			fOffset_Y = 0.0f;
		}

		// Update Panel Positions

		for (TListID iLoop = 0; iLoop < m_iListSize; iLoop++)
		{
			CWidget_Base_Panel* pPanel = (CWidget_Base_Panel*)GetSubWidget(m_pstList[iLoop].iWidgetID);

			if (pPanel != NULL)
			{
				TFloat fPosition_Y = kfPanelList_Offset_Y + (iLoop * m_fPanelHeight) + fOffset_Y;

				// Update Alpha

				if ((fPosition_Y < 0.0f) || (fPosition_Y >= (1.0f - m_fPanelHeight)))
				{
					pPanel->Get_Color()->Get_Alpha()->Set(0.0f);
				}
				else
				{
					pPanel->Get_Color()->Get_Alpha()->InterpolateTo(1.0f, kfMSecValueDelta_SelectionBarFade);
				}

				// Update Position

				pPanel->Get_Position()->Get_Y()->InterpolateTo(fPosition_Y, fScrollSpeed);
			}
		}

		// Update Selection Bar Position

		CWidget_Base_ColorQuad* pWidget_SelectionBar = (CWidget_Base_ColorQuad*)GetSubWidget(m_WID_SelectionBar);

		pWidget_SelectionBar->Get_Position()->Get_Y()->InterpolateTo((kfPanelList_Offset_Y - 0.01f) + (m_iCurrentSelected * m_fPanelHeight) + fOffset_Y, fScrollSpeed);

		// Update Scroll Bar

		CWidget_Base_ScrollBar* pWidget_ScrollBar = (CWidget_Base_ScrollBar*)GetSubWidget(m_WID_ScrollBar);

		pWidget_ScrollBar->UpdateValues((TInt)(-fOffset_Y / m_fPanelHeight), m_iListSize, (TInt)((1.0f - kfPanelList_Offset_Y) / m_fPanelHeight));

		return TRUE;
	}

	return FALSE;
}

// ***********************************************************************************************************************

TInt CWidget_Base_PanelList::GetCount()
{
	return m_iListSize;
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_Chart
// ***********************************************************************************************************************

CWidget_Base_Chart::CWidget_Base_Chart(TInt Param_iSize)
{
	if (Param_iSize <= 0)
	{
		m_afList = NULL;
	}
	else
	{
		m_afList = (TFloat*)Memory_Allocate(Param_iSize * sizeof(TFloat));
	}

	m_iListSize			= Param_iSize;
	m_iListCurrentIndex	= 0;
}

// ***********************************************************************************************************************

CWidget_Base_Chart::~CWidget_Base_Chart()
{
	if (m_afList != NULL)
	{
		Memory_Free(m_afList);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_Chart::Draw(stRenderBase* Param_pstRenderBase)
{
	// Parent Draw Operations

	__super::Draw(Param_pstRenderBase);

	// Ensure Proper List Initialization

	if (m_afList == NULL)
	{
		return;
	}

	// Variables & Pre-Computed Values

	TFloat fValue_New;
	TFloat fBarLeft;
	TFloat fBarTop;
	TFloat fBarHeight;

	TFloat fBarWidth = Param_pstRenderBase->stCollisionBox.fSize_X * (1.0f / (TFloat)m_iListSize);
	TFloat fValue_Old = m_afList[m_iListSize - 1];

	TARGBColor iColor;
	TARGBColor iColor_Current	= CColor::FloatToARGB(Param_pstRenderBase->fColor_Alpha, Param_pstRenderBase->fColor_Red, Param_pstRenderBase->fColor_Green, Param_pstRenderBase->fColor_Blue);
	TARGBColor iColor_Old		= CColor::FloatToARGB(Param_pstRenderBase->fColor_Alpha * 0.75f, 0.0f, Param_pstRenderBase->fColor_Green * 0.75f, 0.0f);
	TARGBColor iColor_PreUpdate	= CColor::FloatToARGB(Param_pstRenderBase->fColor_Alpha * 0.25f, 0.0f, Param_pstRenderBase->fColor_Green * 0.25f, 0.0f);
	TARGBColor iColor_Recent	= CColor::FloatToARGB(Param_pstRenderBase->fColor_Alpha, 0.0f, Param_pstRenderBase->fColor_Green * 1.0f, 0.0f);

	for (TInt Loop = 0; Loop < m_iListSize; Loop++)
	{
		// Fetch Value From List

		fValue_New = m_afList[Loop];

		// Determine Color (Based on Position Relative To Cursor)

		if (Loop > m_iListCurrentIndex)
		{
			iColor = iColor_Old;
		}
		else
		{
			if (Loop < (m_iListCurrentIndex - 1))
			{
				iColor = iColor_Recent;
			}
			else
			{
				if (Loop == m_iListCurrentIndex)
				{
					iColor = iColor_PreUpdate;
				}
				else
				{
					iColor = iColor_Current;
				}
			}
		}

		// Determine Visual Bar Extents (Based On New/Old Delta)

		fBarLeft = ((TFloat)Loop / (TFloat)m_iListSize);

		if (fValue_Old < fValue_New)
		{
			fBarTop	= 1.0f - fValue_New;
		}
		else
		{
			fBarTop	= 1.0f - fValue_Old;
		}

		fBarHeight = fabs(fValue_Old - fValue_New);

		// Ensure Minimum Thickness

		if (fBarHeight < 0.005f)
		{
			fBarHeight = 0.005f;
		}

		// Draw Bar

		Rendering_DrawRectangle(
			Param_pstRenderBase->stCollisionBox.fPosition_X + (fBarLeft * Param_pstRenderBase->stCollisionBox.fSize_X),
			Param_pstRenderBase->stCollisionBox.fPosition_Y + (fBarTop * Param_pstRenderBase->stCollisionBox.fSize_Y),
			fBarWidth,
			Param_pstRenderBase->stCollisionBox.fSize_Y * fBarHeight,
			0.0f, 0.0f, 1.0f, 1.0f, iColor, eBlending_Alpha, kiTextureID_None);

		// Backup Value For Next Bar Delta

		fValue_Old = fValue_New;
	}
}

// ***********************************************************************************************************************

void CWidget_Base_Chart::RecordValue(TFloat Param_fValue)
{
	// Ensure Proper List Initialization

	if (m_afList == NULL)
	{
		return;
	}

	// Clamp & Store Value

	if (Param_fValue < 0.0f)
	{
		Param_fValue = 0.0f;
	}

	if (Param_fValue > 1.0f)
	{
		Param_fValue = 1.0f;
	}

	m_afList[m_iListCurrentIndex] = Param_fValue;

	// Increase Index Marker (Circular)

	m_iListCurrentIndex++;

	while (m_iListCurrentIndex >= m_iListSize)
	{
		m_iListCurrentIndex -= m_iListSize;
	}
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_NotifyIcon
// ***********************************************************************************************************************

CWidget_Base_NotifyIcon::CWidget_Base_NotifyIcon()
{
	Initialize(kcFontGlyph_None, 10.0f, 1.0f);
}

// ***********************************************************************************************************************

CWidget_Base_NotifyIcon::CWidget_Base_NotifyIcon(TFontGlyph Param_iGlyph, TFloat Param_fSecTimeToLive, TFloat Param_fSecTimeToBlink)
{
	Initialize(Param_iGlyph, Param_fSecTimeToLive, Param_fSecTimeToBlink);
}

// ***********************************************************************************************************************

CWidget_Base_NotifyIcon::~CWidget_Base_NotifyIcon()
{
}

// ***********************************************************************************************************************

void CWidget_Base_NotifyIcon::Initialize(TFontGlyph Param_iGlyph, TFloat Param_fSecTimeToLive, TFloat Param_fSecTimeToBlink)
{
	// Create SubWidget - Icon

	CWidget_Base_FontGlyph* pWidget_Icon = new CWidget_Base_FontGlyph(Param_iGlyph);

	m_WID_IconGlyph = AddSubWidget(pWidget_Icon);

	// Initialize Fields

	m_iFlags_Widget |= kiFlags_Widget_IgnoreParentAlpha;

	m_Position.Set(1.0f, 1.0f);
	m_Size.Set(kfIcon_Size_X, kfIcon_Size_Y);
	m_Color.Get_Alpha()->Set(0.0f);

	SetTimes(Param_fSecTimeToLive, Param_fSecTimeToBlink);
	Deactivate();
}

// ***********************************************************************************************************************

void CWidget_Base_NotifyIcon::SetGlyph(TFontGlyph Param_iGlyph)
{
	CWidget_Base_FontGlyph* pWidget_Icon = (CWidget_Base_FontGlyph*)GetSubWidget(m_WID_IconGlyph);

	if (pWidget_Icon == NULL)
	{
		return;
	}

	pWidget_Icon->SetGlyph(Param_iGlyph);
}

// ***********************************************************************************************************************

void CWidget_Base_NotifyIcon::SetTimes(TFloat Param_fSecTimeToLive, TFloat Param_fSecTimeToBlink)
{
	m_fMaximumSecTimeToLive		= Param_fSecTimeToLive;
	m_fMaximumSecTimeToBlink	= Param_fSecTimeToBlink;
}

// ***********************************************************************************************************************

void CWidget_Base_NotifyIcon::Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged)
{
	// Parent Tick

	__super::Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);

	if (!m_bIsActive)
	{
		return;
	}

	// Update Times

	m_fCurrentSecTimeToLive		-= (Param_fMSecTimeDelta / 1000.0f);
	m_fCurrentSecTimeToBlink	-= (Param_fMSecTimeDelta / 1000.0f);

	// Expire (If Applicable)

	if ((m_fMaximumSecTimeToLive > 0.0f) && (m_fCurrentSecTimeToLive <= 0.0f))
	{
		Deactivate();
		return;
	}

	// Blink (If Applicable)

	if ((m_fMaximumSecTimeToBlink > 0.0f) && (m_fCurrentSecTimeToBlink <= 0.0f))
	{
		if (m_Color.GetValue_Alpha() <= 0.0f)
		{
			m_Color.Get_Alpha()->InterpolateTo(1.0f, kfMSecValueDelta_IconFade);
			m_fCurrentSecTimeToBlink = m_fMaximumSecTimeToBlink;
		}
		else
		{
			m_Color.Get_Alpha()->InterpolateTo(0.0f, kfMSecValueDelta_IconFade);
		}
	}
}

// ***********************************************************************************************************************

void CWidget_Base_NotifyIcon::Activate()
{
	m_bIsActive = TRUE;

	m_fCurrentSecTimeToLive		= m_fMaximumSecTimeToLive;
	m_fCurrentSecTimeToBlink	= m_fMaximumSecTimeToBlink;

	m_Position.Get_X()->InterpolateTo(1.0f - kfIcon_Size_X, kfMSecValueDelta_IconMove);
	m_Color.Get_Alpha()->InterpolateTo(1.0f, kfMSecValueDelta_IconFade);
}

// ***********************************************************************************************************************

void CWidget_Base_NotifyIcon::Deactivate()
{
	m_bIsActive = FALSE;

	m_Position.Get_X()->InterpolateTo(1.0f, kfMSecValueDelta_IconMove);
	m_Color.Get_Alpha()->InterpolateTo(0.0f, kfMSecValueDelta_IconFade);
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_Window
// ***********************************************************************************************************************

CWidget_Base_Window::CWidget_Base_Window()
{
	Initialize(NULL);
}

// ***********************************************************************************************************************

CWidget_Base_Window::CWidget_Base_Window(TPWString Param_pTitle)
{
	Initialize(Param_pTitle);
}

// ***********************************************************************************************************************

CWidget_Base_Window::~CWidget_Base_Window()
{
}

// ***********************************************************************************************************************

void CWidget_Base_Window::Initialize(TPWString Param_pTitle)
{
	// Create SubWidget - Frame

	CWidget_Base_Frame* pWidget_Frame = new CWidget_Base_Frame(Param_pTitle);

	m_WID_Frame = AddSubWidget(pWidget_Frame);

	// Initialize Fields

	m_iFlags_Widget |= kiFlags_Widget_IsAWindow;

	m_Color.Set(kiColor_Window_Foreground);
	m_Position.Set(1.0f, kfWindow_Offset_Y);
	m_Size.Set(1.0f, 1.0f - kfWindow_Offset_Y);
	SetPageID(-1);
}

// ***********************************************************************************************************************

void CWidget_Base_Window::SetTitle(TPWString Param_pTitle)
{
	CWidget_Base_Frame* pWidget_Frame;

	pWidget_Frame = (CWidget_Base_Frame*)GetSubWidget(m_WID_Frame);

	if (pWidget_Frame != NULL)
	{
		pWidget_Frame->SetTitle(Param_pTitle);
	}
}

// ***********************************************************************************************************************

TPWString CWidget_Base_Window::GetTitle()
{
	CWidget_Base_Frame* pWidget_Frame;

	pWidget_Frame = (CWidget_Base_Frame*)GetSubWidget(m_WID_Frame);

	if (pWidget_Frame != NULL)
	{
		return pWidget_Frame->GetTitle();
	}
	else
	{
		return NULL;
	}
}

// ***********************************************************************************************************************

void CWidget_Base_Window::SetBackgroundColor(TFloat Param_fAlpha, TFloat Param_fRed, TFloat Param_fGreen, TFloat Param_fBlue)
{
	CWidget_Base_Frame* pWidget_Frame;

	pWidget_Frame = (CWidget_Base_Frame*)GetSubWidget(m_WID_Frame);
	if (pWidget_Frame != NULL)
	{
		pWidget_Frame->SetBackgroundColor(Param_fAlpha, Param_fRed, Param_fGreen, Param_fBlue);
	}
}

// ***********************************************************************************************************************

TInt CWidget_Base_Window::GetPageID()
{
	return m_iPageID;
}

// ***********************************************************************************************************************

void CWidget_Base_Window::SetPageID(TInt Param_iPageID)
{
	m_iPageID = Param_iPageID;
}

// ***********************************************************************************************************************

void CWidget_Base_Window::UpdateTrackerValue(TWidgetID Param_WID_Tracker, TFloat Param_fValue)
{
	CWidget_Base_ValueTracker* pWidget_Tracker;

	pWidget_Tracker = (CWidget_Base_ValueTracker*)GetSubWidget(Param_WID_Tracker);
	if (pWidget_Tracker != NULL)
	{
		pWidget_Tracker->UpdateValue(Param_fValue);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_Window::UpdateTrackerValue(TWidgetID Param_WID_Tracker, TInt Param_iValue)
{
	CWidget_Base_ValueTracker* pWidget_Tracker;

	pWidget_Tracker = (CWidget_Base_ValueTracker*)GetSubWidget(Param_WID_Tracker);
	if (pWidget_Tracker != NULL)
	{
		pWidget_Tracker->UpdateValue(Param_iValue);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_Window::UpdateTrackerValue(TWidgetID Param_WID_Tracker, TBool Param_bValue)
{
	CWidget_Base_ValueTracker* pWidget_Tracker;

	pWidget_Tracker = (CWidget_Base_ValueTracker*)GetSubWidget(Param_WID_Tracker);
	if (pWidget_Tracker != NULL)
	{
		pWidget_Tracker->UpdateValue(Param_bValue);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_Window::LoseFocus(eNavLink Param_eNavLink)
{
	// Parent Focus Change

	__super::LoseFocus(Param_eNavLink);

	// Widget-Specific

	m_Color.FadeTo(0.0f, kfMSecValueDelta_WindowFadeOut);

	if (Param_eNavLink == eNavLink_Right)
	{
		m_Position.MoveTo(-1.0f, kfWindow_Offset_Y, kfMSecValueDelta_WindowMove);
	}
	else
	{
		m_Position.MoveTo(1.0f, kfWindow_Offset_Y, kfMSecValueDelta_WindowMove);
	}
}

// ***********************************************************************************************************************

void CWidget_Base_Window::GainFocus(eNavLink Param_eNavLink)
{
	// Parent Focus Change

	__super::GainFocus(Param_eNavLink);

	// Widget-Specific

	m_Color.FadeTo(1.0f, kfMSecValueDelta_WindowFadeIn);
	m_Position.MoveTo(0.0f, kfWindow_Offset_Y, kfMSecValueDelta_WindowMove);
}

// ***********************************************************************************************************************

void CWidget_Base_Window::Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged)
{
	Profile_Begin("Window(%i).Tick", m_iPageID);

	// Parent Operation

	__super::Tick(Param_fMSecTimeDelta, Param_bWorldHasChanged);

	Profile_End();
}

// ***********************************************************************************************************************

void CWidget_Base_Window::Render(stRenderBase Param_stRenderBase)
{
	Profile_Begin("Window(%i).Render", m_iPageID);

	// Parent Operation

	__super::Render(Param_stRenderBase);

	Profile_End();
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_Window_Message
// ***********************************************************************************************************************

CWidget_Base_Window_Message::CWidget_Base_Window_Message(TPWString Param_pTitle, TInt Param_iMaxListSize) : CWidget_Base_Window(Param_pTitle)
{
	// Create SubWidget - String List

	CWidget_Base_StringList* pWidget_StringList = new CWidget_Base_StringList(Param_iMaxListSize);

	pWidget_StringList->Get_Size()->Set(0.98f, 0.95f);
	pWidget_StringList->Get_Position()->Set(0.02f, 0.05f);
	pWidget_StringList->Get_TextSize()->Set(0.175f);

	m_WID_StringList = AddSubWidget(pWidget_StringList);
}

// ***********************************************************************************************************************

CWidget_Base_Window_Message::~CWidget_Base_Window_Message()
{
}

// ***********************************************************************************************************************

void CWidget_Base_Window_Message::Clear()
{
	((CWidget_Base_StringList*)GetSubWidget(m_WID_StringList))->Clear();
}

// ***********************************************************************************************************************

void CWidget_Base_Window_Message::AddMessage(TPWString Param_pString, stStringList_EntryType Param_stEntryType)
{
	((CWidget_Base_StringList*)GetSubWidget(m_WID_StringList))->AddString(Param_pString, Param_stEntryType);
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_Window_Options
// ***********************************************************************************************************************

CWidget_Base_Window_Options::CWidget_Base_Window_Options(TPWString Param_pTitle, TFloat Param_fPosition_Value_X) : CWidget_Base_Window(Param_pTitle)
{
	// Create SubWidget - Option List

	CWidget_Base_OptionList* pWidget_OptionList = new CWidget_Base_OptionList(Param_fPosition_Value_X);

	pWidget_OptionList->Get_Size()->Set(0.98f, 0.95f);
	pWidget_OptionList->Get_Position()->Set(0.02f, 0.05f);
	pWidget_OptionList->Get_TextSize()->Set(0.2f);

	m_WID_OptionList = AddSubWidget(pWidget_OptionList);
}

// ***********************************************************************************************************************

CWidget_Base_Window_Options::~CWidget_Base_Window_Options()
{
}

// ***********************************************************************************************************************

TListID CWidget_Base_Window_Options::AddOption(TPWString Param_pName, stTrackerType Param_stType, void* Param_pLinkedVariable)
{
	return ((CWidget_Base_OptionList*)GetSubWidget(m_WID_OptionList))->AddOption(Param_pName, Param_stType, Param_pLinkedVariable);
}

// ***********************************************************************************************************************

void CWidget_Base_Window_Options::SetLimits(TListID Param_iListID, TFloat Param_fMinimum, TFloat Param_fMaximum, TFloat Param_fDelta)
{
	((CWidget_Base_OptionList*)GetSubWidget(m_WID_OptionList))->SetLimits(Param_iListID, Param_fMinimum, Param_fMaximum, Param_fDelta);
}

// ***********************************************************************************************************************

void CWidget_Base_Window_Options::SetLimits(TListID Param_iListID, TInt Param_iMinimum, TInt Param_iMaximum, TInt Param_iDelta)
{
	((CWidget_Base_OptionList*)GetSubWidget(m_WID_OptionList))->SetLimits(Param_iListID, Param_iMinimum, Param_iMaximum, Param_iDelta);
}

// ***********************************************************************************************************************

void CWidget_Base_Window_Options::SetNotifyCallback(TListID Param_iListID, void* Param_fnNotifyCallback)
{
	((CWidget_Base_OptionList*)GetSubWidget(m_WID_OptionList))->SetNotifyCallback(Param_iListID, Param_fnNotifyCallback);
}

// ***********************************************************************************************************************

void CWidget_Base_Window_Options::SetLinkedVariable(TListID Param_iListID, void* Param_pLinkedVariable)
{
	((CWidget_Base_OptionList*)GetSubWidget(m_WID_OptionList))->SetLinkedVariable(Param_iListID, Param_pLinkedVariable);
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_Window_Configuration
// ***********************************************************************************************************************

CWidget_Base_Window_Configuration::CWidget_Base_Window_Configuration(TInt Param_iOptionCount, stConfigurationEntry* pstConfiguration) : CWidget_Base_Window_Options(L"Configuration")
{
	// Create Array

	m_Configuration.SetCount(Param_iOptionCount);

	// Populate Array

	for (TInt Loop = 0; Loop < Param_iOptionCount; Loop++)
	{
		if (pstConfiguration[Loop].bIsTitle)
		{
			AddTitle(pstConfiguration[Loop].pName);
		}
		else
		{
			AddOption(pstConfiguration[Loop].pName, Loop, pstConfiguration[Loop].bDefaultValue);
		}
	}
}

// ***********************************************************************************************************************

CWidget_Base_Window_Configuration::~CWidget_Base_Window_Configuration()
{
}

// ***********************************************************************************************************************

void CWidget_Base_Window_Configuration::AddTitle(TPWString Param_pName)
{
	((CWidget_Base_OptionList*)GetSubWidget(m_WID_OptionList))->AddOption(Param_pName, kstTrackerType_None, NULL);
}

// ***********************************************************************************************************************

void CWidget_Base_Window_Configuration::AddOption(TPWString Param_pName, TListID Param_iListID, TBool Param_bDefaultValue)
{
	m_Configuration.SetOption(Param_iListID, Param_bDefaultValue);
	((CWidget_Base_OptionList*)GetSubWidget(m_WID_OptionList))->AddOption(Param_pName, kstTrackerType_Boolean, m_Configuration.GetOptionPointer(Param_iListID));
}

// ***********************************************************************************************************************

TBool CWidget_Base_Window_Configuration::GetOption(TListID Param_iListID)
{
	return m_Configuration.GetOption(Param_iListID);
}

// ***********************************************************************************************************************

void CWidget_Base_Window_Configuration::SetOption(TListID Param_iListID, TBool Param_bValue)
{
	m_Configuration.SetOption(Param_iListID, Param_bValue);
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_Window_Panels
// ***********************************************************************************************************************

CWidget_Base_Window_Panels::CWidget_Base_Window_Panels(TPWString Param_pTitle, TFloat Param_fPanelHeight) : CWidget_Base_Window(Param_pTitle)
{
	// Create SubWidget - Panel List

	CWidget_Base_PanelList* pWidget_PanelList = new CWidget_Base_PanelList(Param_fPanelHeight);

	pWidget_PanelList->Get_Size()->Set(0.98f, 0.95f);
	pWidget_PanelList->Get_Position()->Set(0.02f, 0.05f);
	pWidget_PanelList->Get_TextSize()->Set(0.2f);

	m_WID_PanelList = AddSubWidget(pWidget_PanelList);
}

// ***********************************************************************************************************************

CWidget_Base_Window_Panels::~CWidget_Base_Window_Panels()
{
}

// ***********************************************************************************************************************

TListID	CWidget_Base_Window_Panels::AddPanel(CWidget_Base_Panel* Param_pPanel)
{
	return ((CWidget_Base_PanelList*)GetSubWidget(m_WID_PanelList))->AddPanel(Param_pPanel);
}

// ***********************************************************************************************************************

stPanelList_Entry* CWidget_Base_Window_Panels::GetPanel(TListID Param_iListID)
{
	return ((CWidget_Base_PanelList*)GetSubWidget(m_WID_PanelList))->GetPanel(Param_iListID);
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_Window_Chart
// ***********************************************************************************************************************

CWidget_Base_Window_Chart::CWidget_Base_Window_Chart(TPWString Param_pTitle, TInt Param_iChartListSize) : CWidget_Base_Window(Param_pTitle)
{
	// Create SubWidget - Chart

	CWidget_Base_Chart* pWidget_Chart = new CWidget_Base_Chart(Param_iChartListSize);

	pWidget_Chart->Get_Position()->Set(0.005f, 0.045f);
	pWidget_Chart->Get_Size()->Set(0.992f, 0.95f);

	m_WID_Chart = AddSubWidget(pWidget_Chart);
}

// ***********************************************************************************************************************

CWidget_Base_Window_Chart::~CWidget_Base_Window_Chart()
{
}

// ***********************************************************************************************************************

void CWidget_Base_Window_Chart::RecordValue(TFloat Param_fValue)
{
	// Fetch Chart Widget & Record Value

	CWidget_Base_Chart* pWidget_Chart = (CWidget_Base_Chart*)GetSubWidget(m_WID_Chart);

	if (pWidget_Chart != NULL)
	{
		pWidget_Chart->RecordValue(Param_fValue);
	}
}

// ***********************************************************************************************************************
//    Class Methods : CWidget_Base_Destkop
// ***********************************************************************************************************************

CWidget_Base_Desktop::CWidget_Base_Desktop()
{
	Initialize(NULL);
}

// ***********************************************************************************************************************

CWidget_Base_Desktop::CWidget_Base_Desktop(TPWString Param_pName)
{
	Initialize(Param_pName);
}

// ***********************************************************************************************************************

CWidget_Base_Desktop::~CWidget_Base_Desktop()
{
}

// ***********************************************************************************************************************

void CWidget_Base_Desktop::Initialize(TPWString Param_pName)
{
	CWidget_Base_FontGlyph*	pWidget_FontGlyph;

	// Set Extents (Allowing for a safe-frame)

	m_Position.Set(kfSafeFrame_Left, kfSafeFrame_Top);
	m_Size.Set(1.0f - (kfSafeFrame_Left + kfSafeFrame_Right), 1.0f - (kfSafeFrame_Top + kfSafeFrame_Bottom));

	// Create SubWidget - Background

	CWidget_Base_ColorQuad* pWidget_Background = new CWidget_Base_ColorQuad(kiColor_Desktop_Background);

	m_WID_Background = AddSubWidget(pWidget_Background);

	// Create SubWidget - Bar - Top

	pWidget_FontGlyph = new CWidget_Base_FontGlyph(kcFontGlyph_Desktop_Top);

	pWidget_FontGlyph->Get_Size()->Set(7.82f, 0.1f);
	pWidget_FontGlyph->Get_Position()->Set(0.0125f, 0.0f);

	m_WID_Bar_Top = AddSubWidget(pWidget_FontGlyph);

	// Create SubWidget - Bar - Top Left

	pWidget_FontGlyph = new CWidget_Base_FontGlyph(kcFontGlyph_Desktop_TopLeft);

	pWidget_FontGlyph->Get_Size()->Set(0.1f, 0.1f);
	pWidget_FontGlyph->Get_Position()->Set(0.0, 0.0f);

	m_WID_Bar_TopLeft = AddSubWidget(pWidget_FontGlyph);

	// Create SubWidget - Bar - Top Right

	pWidget_FontGlyph = new CWidget_Base_FontGlyph(kcFontGlyph_Desktop_TopRight);

	pWidget_FontGlyph->Get_Size()->Set(0.1f, 0.1f);
	pWidget_FontGlyph->Get_Position()->Set(0.99f, 0.0f);

	m_WID_Bar_TopRight = AddSubWidget(pWidget_FontGlyph);

	// Create SubWidget - Name

	CWidget_Base_Text* pWidget_Name = new CWidget_Base_Text(Param_pName);

	pWidget_Name->Get_Color()->Set(kiColor_Desktop_Name);
	pWidget_Name->Get_Position()->Set(0.02f, 0.004f);
	pWidget_Name->Get_Size()->Set(kfFont_Size_DesktopName, kfFont_Size_DesktopName);

	m_WID_Name = AddSubWidget(pWidget_Name);

	// Create SubWidget - Page (Text)

	CWidget_Base_Text* pWidget_Page_Text = new CWidget_Base_Text(L"Page    /");

	pWidget_Page_Text->Get_Color()->Set(kiColor_Desktop_Name);
	pWidget_Page_Text->Get_Position()->Set(0.642f, 0.004f);
	pWidget_Page_Text->Get_Size()->Set(kfFont_Size_DesktopName, kfFont_Size_DesktopName);

	m_WID_Page_Text = AddSubWidget(pWidget_Page_Text);

	// Create SubWidget - Page (Current)

	CWidget_Base_Integer* pWidget_Page_Current = new CWidget_Base_Integer(0);

	pWidget_Page_Current->SetDigitCount(2);
	pWidget_Page_Current->Get_Color()->Set(kiColor_Desktop_Name);
	pWidget_Page_Current->Get_Position()->Set(0.735f, 0.004f);
	pWidget_Page_Current->Get_Size()->Set(kfFont_Size_DesktopName, kfFont_Size_DesktopName);

	m_WID_Page_Current = AddSubWidget(pWidget_Page_Current);

	// Create SubWidget - Page (Total)

	CWidget_Base_Integer* pWidget_Page_Total = new CWidget_Base_Integer(0);

	pWidget_Page_Total->SetDigitCount(2);
	pWidget_Page_Total->Get_Color()->Set(kiColor_Desktop_Name);
	pWidget_Page_Total->Get_Position()->Set(0.795f, 0.004f);
	pWidget_Page_Total->Get_Size()->Set(kfFont_Size_DesktopName, kfFont_Size_DesktopName);

	m_WID_Page_Total = AddSubWidget(pWidget_Page_Total);

	// Create SubWidget - Visibility (Text)

	CWidget_Base_Text* pWidget_Visibility_Text = new CWidget_Base_Text(L"Vis");

	pWidget_Visibility_Text->Get_Color()->Set(kiColor_Desktop_Name);
	pWidget_Visibility_Text->Get_Position()->Set(0.85f, 0.004f);
	pWidget_Visibility_Text->Get_Size()->Set(kfFont_Size_DesktopName, kfFont_Size_DesktopName);

	m_WID_Visibility_Text = AddSubWidget(pWidget_Visibility_Text);

	// Create SubWidget - Visibility (Value)

	CWidget_Base_Integer* pWidget_Visibility_Value = new CWidget_Base_Integer(0);

	pWidget_Visibility_Value->SetDisplayAsPercent(TRUE);
	pWidget_Visibility_Value->Get_Color()->Set(kiColor_Desktop_Name);
	pWidget_Visibility_Value->Get_Position()->Set(0.91f, 0.004f);
	pWidget_Visibility_Value->Get_Size()->Set(kfFont_Size_DesktopName, kfFont_Size_DesktopName);

	m_WID_Visibility_Value = AddSubWidget(pWidget_Visibility_Value);

	// Initialize SubWidget IDs

	// - Input Notification Icon

	m_WID_NotifyIcon_Input = kiWidgetID_Invalid;

	// - Configuration Window

	m_WID_Window_Configuration = kiWidgetID_Invalid;

	// Initialize Fields

	m_iFlags_Widget |= kiFlags_Widget_IgnoreRenderVisibility;
}

// ***********************************************************************************************************************

void CWidget_Base_Desktop::SetVisibility(TVisibility Param_Visibility)
{
	TFloat	fAlpha_Foreground;
	TFloat	fAlpha_Background;
	TInt	iVisibilityDisplay;

	switch (Param_Visibility)
	{
		case kiVisibility_None:
		{
			fAlpha_Foreground	= 0.00f;
			fAlpha_Background	= 0.00f;
			iVisibilityDisplay	= 0;
			break;
		}

		case kiVisibility_VeryLow:
		{
			fAlpha_Foreground	= 0.10f;
			fAlpha_Background	= 0.00f;
			iVisibilityDisplay	= 10;
			break;
		}

		case kiVisibility_Low:
		{
			fAlpha_Foreground	= 0.25f;
			fAlpha_Background	= 0.00f;
			iVisibilityDisplay	= 25;
			break;
		}

		case kiVisibility_Medium:
		{
			fAlpha_Foreground	= 0.50f;
			fAlpha_Background	= 0.00f;
			iVisibilityDisplay	= 50;
			break;
		}

		case kiVisibility_High:
		{
			fAlpha_Foreground	= 1.00f;
			fAlpha_Background	= 0.25f;
			iVisibilityDisplay	= 75;
			break;
		}

		case kiVisibility_VeryHigh:
		{
			fAlpha_Foreground	= 1.00f;
			fAlpha_Background	= 0.75f;
			iVisibilityDisplay	= 100;
			break;
		}
	}

	// Fade Foreground & Background Alphas

	m_Color.FadeTo(fAlpha_Foreground, kfMSecValueDelta_DesktopVisibility);

	CWidget* pWidget_Background = GetSubWidget(m_WID_Background);
	if (pWidget_Background != NULL)
	{
		pWidget_Background->Get_Color()->FadeTo(fAlpha_Background, kfMSecValueDelta_DesktopVisibility);
	}

	// Update Visibility Indicator

	CWidget_Base_Integer* pWidget_Visibility_Value = (CWidget_Base_Integer*)GetSubWidget(m_WID_Visibility_Value);
	if (pWidget_Visibility_Value != NULL)
	{
		pWidget_Visibility_Value->UpdateValue(iVisibilityDisplay);
	}
}
// ***********************************************************************************************************************

void CWidget_Base_Desktop::SetName(TPWString Param_pName)
{
	CWidget_Base_Text* pWidget_Name;

	pWidget_Name = (CWidget_Base_Text*)GetSubWidget(m_WID_Name);
	if (pWidget_Name != NULL)
	{
		pWidget_Name->SetText(Param_pName);
	}
}

// ***********************************************************************************************************************
//    Class Methods : CDesktop
// ***********************************************************************************************************************

CDesktop::CDesktop()
{
	Initialize();
}

// ***********************************************************************************************************************

CDesktop::CDesktop(TPWString Param_pName, TARGBColor Param_iColorTheme)
{
	Initialize();

	SetName(Param_pName);

	m_DesktopRootWidget.Get_Color()->Set(Param_iColorTheme);
}

// ***********************************************************************************************************************

CDesktop::~CDesktop()
{
}

// ***********************************************************************************************************************

void CDesktop::Initialize()
{
	// Initialize Font Manager

	g_FontManager.Initialize();

	// Initialize Fields

	m_fIconPositionSlot_Y = kfIcon_DefaultStartPosition_Y;

	SetInputMonitoringToggleButtons(eButton_None, eButton_White);
	SetInputMonitoring(FALSE, kiWidgetID_Invalid);

	m_iCurrentWorld = kiEngineResID_None;

	// Hide

	SetVisibility(kiVisibility_None);
}

// ***********************************************************************************************************************

void CDesktop::SetName(TPWString Param_pName)
{
	m_DesktopRootWidget.SetName(Param_pName);
}

// ***********************************************************************************************************************

void CDesktop::SetVisibility(TVisibility Param_iVisibility)
{
	// Update Visibility Property

	if (Param_iVisibility == kiVisibility_Raise)
	{
		// Raise

		if (m_iCurrentVisibility < kiVisibility_VeryHigh)
		{
			m_iCurrentVisibility++;
		}
	}
	else
	{
		if (Param_iVisibility == kiVisibility_Lower)
		{
			// Lower

			if (m_iCurrentVisibility > kiVisibility_None)
			{
				m_iCurrentVisibility--;
			}
		}
		else
		{
			// Set Directly To Something

			if ((Param_iVisibility >= kiVisibility_None) && (Param_iVisibility <= kiVisibility_VeryHigh))
			{
				m_iCurrentVisibility = Param_iVisibility;
			}
		}
	}

	// Fade Desktop To New Visibility

	m_DesktopRootWidget.SetVisibility(m_iCurrentVisibility);
}

// ***********************************************************************************************************************

void CDesktop::SetupIconBar(TFloat Param_fIconPositionSlot_Y, TFontGlyph Param_iGlyph_InputIcon)
{
	m_fIconPositionSlot_Y = Param_fIconPositionSlot_Y;

	// Create SubWidget - Input Notification Icon

	m_DesktopRootWidget.m_WID_NotifyIcon_Input = AddNotifyIcon(new CWidget_Base_NotifyIcon(Param_iGlyph_InputIcon, 0.0f, 2.0f));
}

// ***********************************************************************************************************************

TWidgetID CDesktop::AddWindow(CWidget_Base_Window* Param_pWindow)
{
	TWidgetID WID_Window;

	// Check Input

	if (Param_pWindow == NULL)
	{
		return kiWidgetID_Invalid;
	}

	// Add Window SubWidget To List

	WID_Window = m_DesktopRootWidget.AddSubWidget(Param_pWindow);

	// Make Window Active (If First Window Added)

	if (m_DesktopRootWidget.GetActiveWidget() == NULL)
	{
		SetActiveWindow(WID_Window);
		Param_pWindow->Get_Position()->Set(0.0f, kfWindow_Offset_Y);
	}

	// Automatically Link Window In Navigation Chain

	TWidgetID	WID_ScanID = WID_Window;
	CWidget*	pWidget;

	// - Find Previous Window In List

	while (WID_ScanID > 0)
	{
		WID_ScanID--;
		pWidget = m_DesktopRootWidget.GetSubWidget(WID_ScanID);
		if (pWidget != NULL)
		{
			if (pWidget->GetFlags() & kiFlags_Widget_IsAWindow)
			{
				// Cross Nav-Link The Two Windows

				Param_pWindow->SetNavLink(eNavLink_Left, WID_ScanID);
				pWidget->SetNavLink(eNavLink_Right, WID_Window);

				// Stop The Scan

				WID_ScanID = -1;
			}
		}
	}

	// Update Page IDs

	CWidget_Base_Integer* pWidget_Page_Total = (CWidget_Base_Integer*)m_DesktopRootWidget.GetSubWidget(m_DesktopRootWidget.m_WID_Page_Total);
	if (pWidget_Page_Total != NULL)
	{
		// Set Window ID

		Param_pWindow->SetPageID(pWidget_Page_Total->GetValue());

		// Update Page Total

		pWidget_Page_Total->SetValue(pWidget_Page_Total->GetValue() + 1);
	}

	return WID_Window;
}

// ***********************************************************************************************************************

void CDesktop::AddConfigurationWindow(CWidget_Base_Window* Param_pWindow)
{
	m_DesktopRootWidget.m_WID_Window_Configuration = AddWindow(Param_pWindow);
}

// ***********************************************************************************************************************

TWidgetID CDesktop::AddNotifyIcon(CWidget_Base_NotifyIcon* Param_pNotifyIcon)
{
	TWidgetID WID_Icon;

	// Check Input

	if (Param_pNotifyIcon == NULL)
	{
		return kiWidgetID_Invalid;
	}

	// Set Icon Position

	Param_pNotifyIcon->Get_Position()->Get_Y()->Set(m_fIconPositionSlot_Y);

	m_fIconPositionSlot_Y -= kfIcon_Spacing_Y;

	// Add NotifyIcon SubWidget To List

	WID_Icon = m_DesktopRootWidget.AddSubWidget(Param_pNotifyIcon);

	return WID_Icon;
}

// ***********************************************************************************************************************

void CDesktop::SetActiveWindow(TWidgetID Param_WID_NewActiveWindow, eNavLink Param_eNavLink)
{
	m_DesktopRootWidget.SetActiveWidget(Param_WID_NewActiveWindow, Param_eNavLink);
}

// ***********************************************************************************************************************

CWidget_Base_Window* CDesktop::GetWindow(TWidgetID Param_WID_Window)
{
	return (CWidget_Base_Window*)m_DesktopRootWidget.GetSubWidget(Param_WID_Window);
}

// ***********************************************************************************************************************

CWidget_Base_Window* CDesktop::GetActiveWindow()
{
	return (CWidget_Base_Window*)m_DesktopRootWidget.GetActiveWidget();
}

// ***********************************************************************************************************************

TBool CDesktop::GetConfiguration(TListID Param_iConfigurationOption)
{
	CWidget_Base_Window_Configuration* pWidget_Window = (CWidget_Base_Window_Configuration*)m_DesktopRootWidget.GetSubWidget(m_DesktopRootWidget.m_WID_Window_Configuration);

	if (pWidget_Window == NULL)
	{
		return FALSE;
	}
	else
	{
		return pWidget_Window->GetOption(Param_iConfigurationOption);
	}
}

// ***********************************************************************************************************************

void CDesktop::SetConfiguration(TListID Param_iConfigurationOption, TBool Param_bValue)
{
	CWidget_Base_Window_Configuration* pWidget_Window = (CWidget_Base_Window_Configuration*)m_DesktopRootWidget.GetSubWidget(m_DesktopRootWidget.m_WID_Window_Configuration);

	if (pWidget_Window != NULL)
	{
		pWidget_Window->SetOption(Param_iConfigurationOption, Param_bValue);
	}
}

// ***********************************************************************************************************************

CMessageMaker* CDesktop::GetMessageMaker()
{
	return &m_MessageMaker;
}

// ***********************************************************************************************************************

void CDesktop::ActivateNotifyIcon(TWidgetID Param_WID_Icon)
{
	CWidget_Base_NotifyIcon* pWidget_Icon = (CWidget_Base_NotifyIcon*)m_DesktopRootWidget.GetSubWidget(Param_WID_Icon);

	if (pWidget_Icon == NULL)
	{
		return;
	}

	pWidget_Icon->Activate();
}

// ***********************************************************************************************************************

void CDesktop::DeactivateNotifyIcon(TWidgetID Param_WID_Icon)
{
	CWidget_Base_NotifyIcon* pWidget_Icon = (CWidget_Base_NotifyIcon*)m_DesktopRootWidget.GetSubWidget(Param_WID_Icon);

	if (pWidget_Icon == NULL)
	{
		return;
	}

	pWidget_Icon->Deactivate();
}

// ***********************************************************************************************************************

void CDesktop::Tick(TFloat Param_fSecTimeDelta)
{
	TBool IsInputStateChanged = FALSE;

	// Display Current Page ID

	CWidget_Base_Integer*	pWidget_Page_Current	= (CWidget_Base_Integer*)m_DesktopRootWidget.GetSubWidget(m_DesktopRootWidget.m_WID_Page_Current);
	CWidget_Base_Window*	pWindow					= GetActiveWindow();

	if ((pWidget_Page_Current != NULL) && (pWindow != NULL))
	{
		pWidget_Page_Current->SetValue(pWindow->GetPageID() + 1);
	}

	// Check For World Change

	TBool bWorldHasChanged = FALSE;

	if (m_iCurrentWorld != World_GetID())
	{
		bWorldHasChanged = TRUE;

		// Update World ID

		m_iCurrentWorld = World_GetID();

		// Invalidate Font Textures

		g_FontManager.InvalidateTextures();
	}

	// Increment MessageMaker TimeStamp

	m_MessageMaker.Tick(Param_fSecTimeDelta, bWorldHasChanged);

	// Monitor Gamepad Input

	// - Check For Toggle Disable

	if (m_bIsMonitoringInput)
	{
		IsInputStateChanged = MonitorInput_Toggle();
	}

	// - Navigation

	if (m_bIsMonitoringInput)
	{
		MonitorInput_Navigation();
	}

	// - Check For Toggle Enable

	if (!m_bIsMonitoringInput && !Input_IsLocked() && !IsInputStateChanged)
	{
		MonitorInput_Toggle();
	}

	// Convert Seconds DT To Milliseconds

	TFloat MSecTimeDelta = Param_fSecTimeDelta * 1000.0f;

	// Clamp to Maximum of 1000ms (1 second)

	if (MSecTimeDelta > 1000.0f)
	{
		MSecTimeDelta = 1000.0f;
	}

	// Recursively Tick All Widgets

	m_DesktopRootWidget.Tick(MSecTimeDelta, bWorldHasChanged);
}

// ***********************************************************************************************************************

void CDesktop::Render()
{
	stRenderBase RenderBase_Identity = kstRenderBase_Identity;

	// Recursively Render All Widgets

	m_DesktopRootWidget.Render(RenderBase_Identity);
}

// ***********************************************************************************************************************

void CDesktop::SetInputMonitoringToggleButtons(eButton Param_eMonitorToggleButton_Hold, eButton Param_eMonitorToggleButton_Tap)
{
	m_eMonitorToggleButton_Hold	= Param_eMonitorToggleButton_Hold;
	m_eMonitorToggleButton_Tap	= Param_eMonitorToggleButton_Tap;
}

// ***********************************************************************************************************************

void CDesktop::SetInputMonitoring(TBool Param_bIsMonitoring, TWidgetID Param_WID_NotifyIcon)
{
	if (Param_bIsMonitoring != m_bIsMonitoringInput)
	{
		m_bIsMonitoringInput = Param_bIsMonitoring;

		if (Param_WID_NotifyIcon != kiWidgetID_Invalid)
		{
			if (m_bIsMonitoringInput)
			{
				ActivateNotifyIcon(Param_WID_NotifyIcon);
			}
			else
			{
				DeactivateNotifyIcon(Param_WID_NotifyIcon);
			}
		}
	}
}

// ***********************************************************************************************************************

TBool CDesktop::MonitorInput_Toggle()
{
	// Disable InputLock

	Input_SetLock(FALSE);

	// Check Button - Hold (Optional)

	if ((m_eMonitorToggleButton_Hold != eButton_None) && !Input_IsButtonCurrentlyPressed(m_eMonitorToggleButton_Hold))
	{
		Input_SetLock(m_bIsMonitoringInput);
		return FALSE;
	}

	// Check Button - Tap (Mandatory)

	if ((m_eMonitorToggleButton_Tap == eButton_None) || !Input_IsButtonRecentlyPressed(m_eMonitorToggleButton_Tap))
	{
		Input_SetLock(m_bIsMonitoringInput);
		return FALSE;
	}

	// Flip Monitoring State

	SetInputMonitoring(!m_bIsMonitoringInput, m_DesktopRootWidget.m_WID_NotifyIcon_Input);

	Input_SetLock(m_bIsMonitoringInput);
	return TRUE;
}

// ***********************************************************************************************************************

void CDesktop::MonitorInput_Navigation()
{
	Input_SetLock(FALSE);

	// Digital Pad

	if (Input_IsButtonRecentlyPressed(eButton_DPad_Up))
	{
		SetVisibility(kiVisibility_Raise);
	}

	if (Input_IsButtonRecentlyPressed(eButton_DPad_Down))
	{
		SetVisibility(kiVisibility_Lower);
	}

	if (Input_IsButtonRecentlyPressed(eButton_DPad_Left))
	{
		m_DesktopRootWidget.Navigate(eNavLink_Left);
	}

	if (Input_IsButtonRecentlyPressed(eButton_DPad_Right))
	{
		m_DesktopRootWidget.Navigate(eNavLink_Right);
	}

	if (Input_IsButtonRecentlyPressed(eButton_A))
	{
		m_DesktopRootWidget.Navigate(eNavLink_Forward);
	}

	if (Input_IsButtonRecentlyPressed(eButton_B))
	{
		m_DesktopRootWidget.Navigate(eNavLink_Back);
	}

	if (Input_IsButtonRecentlyPressed(eButton_Trigger_Left))
	{
		m_DesktopRootWidget.Navigate(eNavLink_Skip_Back);
	}

	if (Input_IsButtonRecentlyPressed(eButton_Trigger_Right))
	{
		m_DesktopRootWidget.Navigate(eNavLink_Skip_Forward);
	}

	// Analog Stick

	TFloat Motion_X = Input_GetMotion_X();
	TFloat Motion_Y = Input_GetMotion_Y();

	if (Motion_X < -kfInput_ScrollThreshold)
	{
		m_DesktopRootWidget.Scroll(eNavLink_Left);
	}

	if (Motion_X > kfInput_ScrollThreshold)
	{
		m_DesktopRootWidget.Scroll(eNavLink_Right);
	}

	if (Motion_Y < -kfInput_ScrollThreshold)
	{
		m_DesktopRootWidget.Scroll(eNavLink_Down);
	}

	if (Motion_Y > kfInput_ScrollThreshold)
	{
		m_DesktopRootWidget.Scroll(eNavLink_Up);
	}

	Input_SetLock(TRUE);
}

// ***********************************************************************************************************************

void CDesktop::WaitForMessageExclusion()
{
	Profile_Begin("CDesktop.WaitForMessageExclusion");

	while (m_MessageMaker.IsLocked())
	{
		// Wait
	}

	Profile_End();
}

// ***********************************************************************************************************************

} // Namespace

// ***********************************************************************************************************************

#endif // DESKTOP_ENABLE
