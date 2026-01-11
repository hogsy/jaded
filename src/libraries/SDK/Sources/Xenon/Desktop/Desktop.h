// ****************************************
// Visual Desktop Windowing Manager
//
// Used for King Kong Xenon Debugging
//
// By Alexandre David (January 2005)
// ****************************************

#ifndef __Desktop_h__
#define __Desktop_h__

// ***********************************************************************************************************************
//    Externally Used Types
// ***********************************************************************************************************************

#include "Desktop_ConstantTypes.h"

// ***********************************************************************************************************************

#if defined(DESKTOP_ENABLE)

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "Desktop_Native.h"

namespace Desktop
{

// ***********************************************************************************************************************
//    Forward Declarations
// ***********************************************************************************************************************

class CWidget;

// ***********************************************************************************************************************
//    Classes
// ***********************************************************************************************************************

class CText
{
private:
	TPWString		m_pBuffer;
	TInt volatile	m_iCurrentLength;
	TInt volatile	m_iMaximumLength;

public:
    M_DeclareOperatorNewAndDelete();

	CText();
	CText(TPWString Param_pString);
	CText(TPString Param_pString);
	CText(CText* Param_pText);
	~CText();

	void		Clear(TBool Param_bPreserveBuffer = FALSE);
	void		Resize(TInt Param_iSize, TBool Param_bCopyPrevious = TRUE);
	void		SetTo(TPWString Param_pString);
	void		SetTo(TPString Param_pString);
	void		SetTo(CText* Param_pText);
	void		Append(TPWString Param_pString);
	void		Append(TPString Param_pString);
	void		Append(CText* Param_pText);
	TBool		IsEmpty();
	TPWString	GetString();
	TInt		GetCurrentLength();
	TInt		GetMaximumLength();
};

// ***********************************************************************************************************************

class CFloat
{
private:
	TFloat	m_fTarget;
	TFloat	m_fCurrent;
	TFloat	m_fMSecValueDelta;
	TInt	m_iInterpolateSign;

public:
    M_DeclareOperatorNewAndDelete();

	CFloat();
	CFloat(TFloat Param_fCurrent);
	~CFloat();

	TBool	IsChanging();
	void	Set(TFloat Param_fNewCurrent);
	void	InterpolateTo(TFloat Param_fNewTarget, TFloat Param_fMSecValueDelta);
	TFloat	Get();
	void	Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged);
};

// ***********************************************************************************************************************

class CInteger
{
private:
	TInt	m_iTarget;
	TInt	m_iCurrent;
	TInt	m_iMSecValueDelta;

public:
    M_DeclareOperatorNewAndDelete();

	CInteger();
	CInteger(TInt Param_iCurrent);
	~CInteger();

	TBool	IsChanging();
	void	Set(TInt Param_iNewCurrent);
	void	InterpolateTo(TInt Param_iNewTarget, TInt Param_iMSecValueDelta);
	TInt	Get();
	void	Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged);
};

// ***********************************************************************************************************************

class CBoolean
{
private:
	TBool	m_bCurrent;

public:
    M_DeclareOperatorNewAndDelete();

	CBoolean();
	CBoolean(TBool Param_bCurrent);
	~CBoolean();

	void	Set(TBool Param_bNewCurrent);
	TBool	Get();
};

// ***********************************************************************************************************************

class CPoint
{
private:
	CFloat	m_X;
	CFloat	m_Y;

public:
    M_DeclareOperatorNewAndDelete();

	CPoint();
	CPoint(TFloat Param_fCurrentX, TFloat Param_fCurrentY);
	~CPoint();

	void	Set(TFloat Param_fNewCurrentX, TFloat Param_fNewCurrentY);
	void	MoveTo(TFloat Param_fNewTargetX, TFloat Param_fNewTargetY, TFloat Param_fMSecValueDelta);
	CFloat*	Get_X();
	CFloat*	Get_Y();
	TFloat	GetValue_X();
	TFloat	GetValue_Y();
	void	Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged);
};

// ***********************************************************************************************************************

class CColor
{
private:
	CFloat	m_Alpha;
	CFloat	m_Red;
	CFloat	m_Green;
	CFloat	m_Blue;

public:
    M_DeclareOperatorNewAndDelete();

	CColor();
	CColor(TFloat Param_fCurrentAlpha, TFloat Param_fCurrentRed, TFloat Param_fCurrentGreen, TFloat Param_fCurrentBlue);
	CColor(TARGBColor Param_iCurrentARGB);
	~CColor();

	void		Set(TFloat Param_fNewCurrentAlpha, TFloat Param_fNewCurrentRed, TFloat Param_fNewCurrentGreen, TFloat Param_fNewCurrentBlue);
	void		Set(TFloat Param_fNewCurrentRed, TFloat Param_fNewCurrentGreen, TFloat Param_fNewCurrentBlue);
	void		Set(TFloat Param_fNewCurrentAlpha);
	void		Set(TARGBColor Param_iNewCurrentARGB);
	void		FadeTo(TFloat Param_fNewTargetAlpha, TFloat Param_fNewTargetRed, TFloat Param_fNewTargetGreen, TFloat Param_fNewTargetBlue, TFloat Param_fMSecValueDelta);
	void		FadeTo(TFloat Param_fNewTargetRed, TFloat Param_fNewTargetGreen, TFloat Param_fNewTargetBlue, TFloat Param_fMSecValueDelta);
	void		FadeTo(TFloat Param_fNewTargetAlpha, TFloat Param_fMSecValueDelta);
	void		FadeTo(TARGBColor Param_iNewTargetARGB, TFloat Param_fMSecValueDelta);
	CFloat*		Get_Alpha();
	CFloat*		Get_Red();
	CFloat*		Get_Green();
	CFloat*		Get_Blue();
	TFloat		GetValue_Alpha();
	TFloat		GetValue_Red();
	TFloat		GetValue_Green();
	TFloat		GetValue_Blue();
	TARGBColor	Get_ARGB();
	void		Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged);

	static TARGBColor FloatToARGB(TFloat Param_fAlpha, TFloat Param_fRed, TFloat Param_fGreen, TFloat Param_fBlue);
};

// ***********************************************************************************************************************

class CFontManager
{
private:
	stFont*	m_pstElements;
	TInt	m_iCount;

public:
    M_DeclareOperatorNewAndDelete();

	CFontManager();
	~CFontManager();

	void	Initialize();
	void	Shutdown();
	stFont*	GetFont(TFontID Param_iFontID, TWChar Param_cCharacter = 0);
	void	CreateFont(TFontID Param_iFontID, TEngineResID Param_iDescriptorID, TEngineResID Param_iTextureID, TFloat Param_fScaleX, TFloat Param_fScaleY);
	void	Clear();
	void	InvalidateTextures();
};

// ***********************************************************************************************************************

class CMessageMaker
{
private:
	CText			m_Parameters;
	CText			m_LastMessage;
	TBool volatile	m_bIsLocked;
	TFloat			m_fSecTimeStamp;

public:
    M_DeclareOperatorNewAndDelete();

	CMessageMaker();
	~CMessageMaker();

	TBool		IsLocked();
	void		Tick(TFloat Param_fSecTimeDelta, TBool Param_bWorldHasChanged);
	TPWString	Make(TPWString Param_pString, TPWString Param_pModuleName = NULL);
	void		Clear();
	void		AddParameter(TPWString Param_pValue, TPWString Param_pName = NULL);
	void		AddParameter(TInt Param_iValue, TPWString Param_pName = NULL, TBool Param_bDisplayAsHex = FALSE);
	void		AddParameter(TFloat Param_fValue, TPWString Param_pName = NULL);
	void		AddParameter(TBool Param_bValue, TPWString Param_pName = NULL);
};

// ***********************************************************************************************************************

class CConfiguration
{
private:
	TBool*	m_pOptions;
	TInt	m_iOptionCount;

public:
    M_DeclareOperatorNewAndDelete();

	CConfiguration();
	~CConfiguration();

	void	SetCount(TInt Param_iListCount);
	void	SetOption(TListID Param_iListID, TBool Param_bValue);
	TBool	GetOption(TListID Param_iListID);
	TBool*	GetOptionPointer(TListID Param_iListID);
};

// ***********************************************************************************************************************

class CNavLinks
{
private:
	TWidgetID* m_aLinks;

public:
    M_DeclareOperatorNewAndDelete();

	CNavLinks();
	~CNavLinks();

	TWidgetID	GetLink(eNavLink Param_eNavLink);
	void		SetLink(eNavLink Param_eNavLink, TWidgetID Param_WID_LinkedWidget);
};

// ***********************************************************************************************************************

class CWidgetList
{
private:
	CWidget**	m_pElements;
	TInt		m_iCount;
	TWidgetID	m_WID_CurrentElement;

public:
    M_DeclareOperatorNewAndDelete();

	CWidgetList();
	~CWidgetList();

	TWidgetID	AddElement(CWidget* Param_pWidgetHandle);
	CWidget*	GetElement(TWidgetID Param_iWidgetID);
	CWidget*	GetCurrentElement();
	void		RemoveAllElements();
	void		SetCurrentElement(TWidgetID Param_iWidgetID, eNavLink Param_eNavLink = eNavLink_None);
	void		Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged);
	void		Render(stRenderBase Param_stRenderBase);
};

// ***********************************************************************************************************************

class CWidget
{
protected:
	TFlags		m_iFlags_Widget;
	eBlending	m_eBlending;
	CWidgetList	m_SubWidgets;
	CNavLinks	m_NavLinks;
	CFloat		m_TextSize;
	CColor		m_Color;
	CPoint		m_Position;
	CPoint		m_Size;
	CPoint		m_Clipping;
	CPoint		m_ScrollPosition;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget();
	virtual ~CWidget();

	virtual void	Draw(stRenderBase* Param_pstRenderBase);
	virtual void	Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged);
	virtual void	LoseFocus(eNavLink Param_eNavLink = eNavLink_None);
	virtual void	GainFocus(eNavLink Param_eNavLink = eNavLink_None);
	virtual TBool	IsVisible(stRenderBase* Param_pstRenderBase = NULL);
	virtual stBox	GetBoundingBox(stRenderBase Param_stRenderBase);
	virtual void	Signal(eNavLink Param_eNavLink = eNavLink_None);
	virtual void	Render(stRenderBase Param_stRenderBase);

	TWidgetID		AddSubWidget(CWidget* Param_pWidgetHandle);
	CWidget*		GetSubWidget(TWidgetID Param_iWidgetID);
	CWidget*		GetActiveWidget();
	void			SetActiveWidget(TWidgetID Param_iWidgetID, eNavLink Param_eNavLink = eNavLink_None);
	TFlags			GetFlags();
	eBlending		GetBlending();
	void			SetBlending(eBlending Param_eBlending);
	void			SetNavLink(eNavLink Param_eNavLink, TWidgetID Param_WID_LinkedWidget);
	TWidgetID		GetNavLink(eNavLink Param_eNavLink);
	CFloat*			Get_TextSize();
	CColor*			Get_Color();
	CPoint*			Get_Position();
	CPoint*			Get_Size();
	CPoint*			Get_Clipping();
	CPoint*			Get_ScrollPosition();
	stRenderBase	AdjustRenderBaseToString(TPWString Param_pString, stRenderBase Param_stRenderBase);
	void			Navigate(eNavLink Param_eNavLink);
	void			Scroll(eNavLink Param_eNavLink);
};

// ***********************************************************************************************************************

class CWidget_Base_ColorQuad : public CWidget
{
public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_ColorQuad(TFloat Param_fAlpha, TFloat Param_fRed, TFloat Param_fGreen, TFloat Param_fBlue);
	CWidget_Base_ColorQuad(TARGBColor Param_iColor);
	virtual ~CWidget_Base_ColorQuad();

	virtual void Draw(stRenderBase* Param_pstRenderBase);
};

// ***********************************************************************************************************************

class CWidget_Base_TexturedQuad : public CWidget
{
private:
	TInt m_iTextureID;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_TexturedQuad();
	CWidget_Base_TexturedQuad(TInt Param_iTextureID);
	virtual ~CWidget_Base_TexturedQuad();

	virtual void Draw(stRenderBase* Param_pstRenderBase);

	TInt GetTextureID();
	void SetTextureID(TInt Param_iTextureID);
};

// ***********************************************************************************************************************

class CWidget_Base_Text : public CWidget
{
private:
	CText m_Text;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_Text();
	CWidget_Base_Text(TPWString Param_pString);
	virtual ~CWidget_Base_Text();

	virtual void	Draw(stRenderBase* Param_pstRenderBase);
	virtual TBool	IsVisible(stRenderBase* Param_pstRenderBase = NULL);

	void		SetText(TPWString Param_pString);
	TPWString	GetText();
};

// ***********************************************************************************************************************

class CWidget_Base_Float : public CWidget
{
private:
	CFloat m_Value;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_Float();
	CWidget_Base_Float(TFloat Param_fValue);
	virtual ~CWidget_Base_Float();

	virtual void Draw(stRenderBase* Param_pstRenderBase);
	virtual void Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged);

	TFloat	GetValue();
	void	SetValue(TFloat Param_fValue);
	void	UpdateValue(TFloat Param_fValue);
};

// ***********************************************************************************************************************

class CWidget_Base_Integer : public CWidget
{
private:
	CInteger	m_Value;
	TInt		m_iDigitCount;
	TBool		m_bDisplayAsPercent;

	CText		m_Cache_FormattedText;
	TBool		m_Cache_IsValid;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_Integer();
	CWidget_Base_Integer(TInt Param_iValue);
	virtual ~CWidget_Base_Integer();

	virtual void Draw(stRenderBase* Param_pstRenderBase);
	virtual void Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged);

	TInt GetValue();
	void SetValue(TInt Param_iValue);
	void UpdateValue(TInt Param_iValue);
	void SetDigitCount(TInt Param_iValue);
	void SetDisplayAsPercent(TBool Param_bDisplayAsPercent);
	void UpdateCache();
};

// ***********************************************************************************************************************

class CWidget_Base_Boolean : public CWidget
{
private:
	CBoolean m_Value;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_Boolean();
	CWidget_Base_Boolean(TBool Param_bValue);
	virtual ~CWidget_Base_Boolean();

	virtual void Draw(stRenderBase* Param_pstRenderBase);
	virtual void Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged);

	TBool	GetValue();
	void	SetValue(TBool Param_bValue);
	void	UpdateValue(TBool Param_bValue);
};

// ***********************************************************************************************************************

class CWidget_Base_FontGlyph : public CWidget
{
private:
	TFontGlyph m_acGlyph[2];

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_FontGlyph();
	CWidget_Base_FontGlyph(TFontGlyph Param_cGlyph);
	virtual ~CWidget_Base_FontGlyph();

	virtual void Draw(stRenderBase* Param_pstRenderBase);

	TFontGlyph	GetGlyph();
	void		SetGlyph(TFontGlyph Param_cGlyph);
};

// ***********************************************************************************************************************

class CWidget_Base_ValueTracker : public CWidget
{
private:
	stTrackerType	m_stType;
	TWidgetID		m_WID_Name;
	TWidgetID		m_WID_Value_Native;
	TWidgetID		m_WID_Value_BackGlyph;
	TWidgetID		m_WID_Value_ForeGlyph;
	TWidgetID		m_WID_Value_Text;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_ValueTracker();
	CWidget_Base_ValueTracker(TPWString Param_pName, stTrackerType Param_stType);
	CWidget_Base_ValueTracker(stTrackerType Param_stType);
	virtual ~CWidget_Base_ValueTracker();

	void Initialize(TPWString Param_pName, stTrackerType Param_stType);
	void SetName(TPWString Param_pName);
	void SetValue(TFloat Param_fValue);
	void SetValue(TInt Param_iValue);
	void SetValue(TBool Param_bValue);
	void UpdateValue(TFloat Param_fValue);
	void UpdateValue(TInt Param_iValue);
	void UpdateValue(TBool Param_bValue);
	void SetPositions(TFloat Param_fPosition_X_Name, TFloat Param_fPosition_X_Value, TFloat Param_fPosition_Y);
	void SetTextSize(TFloat Param_fSize);
	void SetGlyphSize(TFloat Param_fSize_X, TFloat Param_fSize_Y);
};

// ***********************************************************************************************************************

class CWidget_Base_ListHeader : public CWidget
{
private:
	TWidgetID m_WID_TopLine;
	TWidgetID m_WID_BottomLine;
	TWidgetID m_WID_CentralBand;
	TWidgetID m_WID_Text;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_ListHeader(TPWString Param_pString);
	virtual ~CWidget_Base_ListHeader();

	void SetBarSize(TFloat Param_fSize_X, TFloat Param_fSize_Y);
	void SetTextSize(TFloat Param_fSize);
};

// ***********************************************************************************************************************

class CWidget_Base_Frame : public CWidget
{
private:
	TWidgetID	m_WID_Background;
	TWidgetID	m_WID_Title;
	TWidgetID	m_WID_Top_Left;
	TWidgetID	m_WID_Top;
	TWidgetID	m_WID_Top_Right;
	TWidgetID	m_WID_Left;
	TWidgetID	m_WID_Right;
	TWidgetID	m_WID_Bottom_Left;
	TWidgetID	m_WID_Bottom;
	TWidgetID	m_WID_Bottom_Right;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_Frame();
	CWidget_Base_Frame(TPWString Param_pTitle);
	virtual ~CWidget_Base_Frame();

	void		Initialize(TPWString Param_pTitle = NULL);
	void		SetTitle(TPWString Param_pTitle);
	TPWString	GetTitle();
	void		SetBackgroundColor(TFloat Param_fAlpha, TFloat Param_fRed, TFloat Param_fGreen, TFloat Param_fBlue);
	void		SetTopBarHeight(TFloat Param_fHeight);
	void		SetTitleWidth(TFloat Param_fWidth);
};

// ***********************************************************************************************************************

class CWidget_Base_Panel : public CWidget
{
private:
	TWidgetID m_WID_Frame;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_Panel();
	CWidget_Base_Panel(TPWString Param_pTitle);
	virtual ~CWidget_Base_Panel();

	virtual void Signal(eNavLink Param_eNavLink = eNavLink_None);

	void Initialize(TPWString Param_pTitle = NULL);
	void SetTitle(TPWString Param_pTitle);
	void SetBackgroundColor(TFloat Param_fAlpha, TFloat Param_fRed, TFloat Param_fGreen, TFloat Param_fBlue);
	void SetTopBarHeight(TFloat Param_fHeight);
};

// ***********************************************************************************************************************

class CWidget_Base_ScrollBar : public CWidget
{
private:
	TWidgetID m_WID_Background;
	TWidgetID m_WID_Slider;

	TInt m_iStartEntry;
	TInt m_iTotalEntryCount;
	TInt m_iDisplayedEntryCount;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_ScrollBar();
	virtual ~CWidget_Base_ScrollBar();

	virtual void Draw(stRenderBase* Param_pstRenderBase);

	void UpdateValues(TInt Param_iStartEntry, TInt Param_iTotalEntryCount, TInt Param_iDisplayedEntryCount);
};

// ***********************************************************************************************************************

class CWidget_Base_StringList : public CWidget
{
private:
	TWidgetID			m_WID_ScrollBar;

	stStringList_Entry*	m_pstList;
	TInt				m_iListSize_Maximum;
	TInt				m_iListSize_Current;
	TInt				m_iListStartIndex;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_StringList();
	CWidget_Base_StringList(TInt Param_iMaximumSize);
	virtual ~CWidget_Base_StringList();

	virtual void Draw(stRenderBase* Param_pstRenderBase);

	void				Initialize(TInt Param_iMaximumSize);
	void				ResetMaximumListSize(TInt Param_iMaximumSize);
	void				Clear();
	void				AddString(TPWString Param_pString, stStringList_EntryType Param_stEntryType);
	void				AddString(TPWString Param_pString);
	stStringList_Entry*	GetEntry(TInt Param_iCircularEntryIndex);
	void				SetEntry(TInt Param_iCircularEntryIndex, stStringList_Entry* Param_pstEntry);
};

// ***********************************************************************************************************************

class CWidget_Base_OptionList : public CWidget
{
private:
	TWidgetID			m_WID_ScrollBar;
	TWidgetID			m_WID_SelectionBar;

	stOptionList_Entry*	m_pstList;
	TInt				m_iListSize;
	TListID				m_iCurrentSelected;
	TFloat				m_fPosition_Value_X;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_OptionList(TFloat Param_fPosition_Value_X);
	virtual ~CWidget_Base_OptionList();

	virtual void Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged);
	virtual void Signal(eNavLink Param_eNavLink = eNavLink_None);

	void				SlideOptionValue(eNavLink Param_eDirection);
	TListID				AddOption(TPWString Param_pName, stTrackerType Param_stType, void* Param_pLinkedVariable);
	stOptionList_Entry*	GetOption(TListID Param_iListID);
	stOptionList_Entry*	GetSelectedOption();
	TBool				SelectOption(TListID Param_iListID);
	void				SetLimits(TListID Param_iListID, TFloat Param_fMinimum, TFloat Param_fMaximum, TFloat Param_fDelta);
	void				SetLimits(TListID Param_iListID, TInt Param_iMinimum, TInt Param_iMaximum, TInt Param_iDelta);
	void				SetNotifyCallback(TListID Param_iListID, void* Param_fnNotifyCallback);
	void				SetLinkedVariable(TListID Param_iListID, void* Param_pLinkedVariable);
};

// ***********************************************************************************************************************

class CWidget_Base_PanelList : public CWidget
{
private:
	TWidgetID				m_WID_ScrollBar;
	TWidgetID				m_WID_SelectionBar;

	stPanelList_Entry*		m_pstList;
	TInt					m_iListSize;
	TListID					m_iCurrentSelected;
	TFloat					m_fPanelHeight;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_PanelList(TFloat Param_fPanelHeight);
	virtual ~CWidget_Base_PanelList();

	virtual void Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged);
	virtual void Signal(eNavLink Param_eNavLink = eNavLink_None);

	void				Initialize();
	void				Destroy();
	void				Reset();
	TListID				AddPanel(CWidget_Base_Panel* Param_pPanel);
	stPanelList_Entry*	GetPanel(TListID Param_iListID);
	stPanelList_Entry*	GetSelectedPanel();
	TListID				GetSelectedID();
	TBool				SelectPanel(TListID Param_iListID);
	TInt				GetCount();
};

// ***********************************************************************************************************************

class CWidget_Base_Chart : public CWidget
{
private:
	TFloat*	m_afList;
	TInt	m_iListSize;
	TInt	m_iListCurrentIndex;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_Chart(TInt Param_iSize);
	virtual ~CWidget_Base_Chart();

	virtual void Draw(stRenderBase* Param_pstRenderBase);

	void RecordValue(TFloat Param_fValue);
};

// ***********************************************************************************************************************

class CWidget_Base_NotifyIcon : public CWidget
{
private:
	TWidgetID	m_WID_IconGlyph;

	TBool		m_bIsActive;
	TFloat		m_fCurrentSecTimeToLive;
	TFloat		m_fCurrentSecTimeToBlink;
	TFloat		m_fMaximumSecTimeToLive;
	TFloat		m_fMaximumSecTimeToBlink;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_NotifyIcon();
	CWidget_Base_NotifyIcon(TFontGlyph Param_iGlyph, TFloat Param_fSecTimeToLive = 10.0f, TFloat Param_fSecTimeToBlink = 1.0f);
	virtual ~CWidget_Base_NotifyIcon();

	virtual void Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged);

	void Initialize(TFontGlyph Param_iGlyph, TFloat Param_fSecTimeToLive, TFloat Param_fSecTimeToBlink);
	void SetGlyph(TFontGlyph Param_iGlyph);
	void SetTimes(TFloat Param_fSecTimeToLive, TFloat Param_fSecTimeToBlink);
	void Activate();
	void Deactivate();
};

// ***********************************************************************************************************************

class CWidget_Base_Window : public CWidget
{
private:
	TInt		m_iPageID;
	TWidgetID	m_WID_Frame;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_Window();
	CWidget_Base_Window(TPWString Param_pTitle);
	virtual ~CWidget_Base_Window();

	virtual void LoseFocus(eNavLink Param_eNavLink = eNavLink_None);
	virtual void GainFocus(eNavLink Param_eNavLink = eNavLink_None);
	virtual void Tick(TFloat Param_fMSecTimeDelta, TBool Param_bWorldHasChanged);
	virtual void Render(stRenderBase Param_stRenderBase);

	void		Initialize(TPWString Param_pTitle);
	void		SetTitle(TPWString Param_pTitle);
	TPWString	GetTitle();
	void		SetBackgroundColor(TFloat Param_fAlpha, TFloat Param_fRed, TFloat Param_fGreen, TFloat Param_fBlue);
	TInt		GetPageID();
	void		SetPageID(TInt Param_iPageID);
	void		UpdateTrackerValue(TWidgetID Param_WID_Tracker, TFloat Param_fValue);
	void		UpdateTrackerValue(TWidgetID Param_WID_Tracker, TInt Param_iValue);
	void		UpdateTrackerValue(TWidgetID Param_WID_Tracker, TBool Param_bValue);
};

// ***********************************************************************************************************************

class CWidget_Base_Window_Message : public CWidget_Base_Window
{
private:
	TWidgetID m_WID_StringList;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_Window_Message(TPWString Param_pTitle, TInt Param_iMaxListSize);
	virtual ~CWidget_Base_Window_Message();

	void Clear();
	void AddMessage(TPWString Param_pString, stStringList_EntryType Param_stEntryType);
};

// ***********************************************************************************************************************

class CWidget_Base_Window_Options : public CWidget_Base_Window
{
protected:
	TWidgetID m_WID_OptionList;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_Window_Options(TPWString Param_pTitle, TFloat Param_fPosition_Value_X = 0.75f);
	virtual ~CWidget_Base_Window_Options();

	TListID	AddOption(TPWString Param_pName, stTrackerType Param_stType, void* Param_pLinkedVariable);
	void	SetLimits(TListID Param_iListID, TFloat Param_fMinimum, TFloat Param_fMaximum, TFloat Param_fDelta);
	void	SetLimits(TListID Param_iListID, TInt Param_iMinimum, TInt Param_iMaximum, TInt Param_iDelta);
	void	SetNotifyCallback(TListID Param_iListID, void* Param_fnNotifyCallback);
	void	SetLinkedVariable(TListID Param_iListID, void* Param_pLinkedVariable);
};

// ***********************************************************************************************************************

class CWidget_Base_Window_Configuration : public CWidget_Base_Window_Options
{
private:
	CConfiguration m_Configuration;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_Window_Configuration(TInt Param_iOptionCount, stConfigurationEntry* pstConfiguration);
	virtual ~CWidget_Base_Window_Configuration();

	void	AddTitle(TPWString Param_pName);
	void	AddOption(TPWString Param_pName, TListID Param_iListID, TBool Param_bDefaultValue = FALSE);
	TBool	GetOption(TListID Param_iListID);
	void	SetOption(TListID Param_iListID, TBool Param_bValue);
};

// ***********************************************************************************************************************

class CWidget_Base_Window_Panels : public CWidget_Base_Window
{
protected:
	TWidgetID m_WID_PanelList;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_Window_Panels(TPWString Param_pTitle, TFloat Param_fPanelHeight);
	virtual ~CWidget_Base_Window_Panels();

	TListID				AddPanel(CWidget_Base_Panel* Param_pPanel);
	stPanelList_Entry*	GetPanel(TListID Param_iListID);
};

// ***********************************************************************************************************************

class CWidget_Base_Window_Chart : public CWidget_Base_Window
{
protected:
	TWidgetID m_WID_Chart;

public:
    M_DeclareOperatorNewAndDelete();

	CWidget_Base_Window_Chart(TPWString Param_pTitle, TInt Param_iChartListSize);
	virtual ~CWidget_Base_Window_Chart();

	void RecordValue(TFloat Param_fValue);
};

// ***********************************************************************************************************************

class CWidget_Base_Desktop : public CWidget
{
public:
	TWidgetID m_WID_Background;
	TWidgetID m_WID_Name;
	TWidgetID m_WID_Page_Text;
	TWidgetID m_WID_Page_Current;
	TWidgetID m_WID_Page_Total;
	TWidgetID m_WID_Visibility_Text;
	TWidgetID m_WID_Visibility_Value;
	TWidgetID m_WID_Bar_TopLeft;
	TWidgetID m_WID_Bar_Top;
	TWidgetID m_WID_Bar_TopRight;
	TWidgetID m_WID_NotifyIcon_Input;
	TWidgetID m_WID_Window_Configuration;

    M_DeclareOperatorNewAndDelete();

	CWidget_Base_Desktop();
	CWidget_Base_Desktop(TPWString Param_pName);
	virtual ~CWidget_Base_Desktop();

	void Initialize(TPWString Param_pName);
	void SetVisibility(TVisibility Param_iVisibility);
	void SetName(TPWString Param_pName);
};

// ***********************************************************************************************************************

class CDesktop
{
private:
	CWidget_Base_Desktop	m_DesktopRootWidget;
	CMessageMaker			m_MessageMaker;
	TVisibility				m_iCurrentVisibility;
	TBool					m_bIsMonitoringInput;
	eButton					m_eMonitorToggleButton_Hold;
	eButton					m_eMonitorToggleButton_Tap;
	TFloat					m_fIconPositionSlot_Y;
	TEngineResID			m_iCurrentWorld;

public:
    M_DeclareOperatorNewAndDelete();

	CDesktop();
	CDesktop(TPWString Param_pName, TARGBColor Param_iColorTheme);
	~CDesktop();

	void					Initialize();
	void					SetName(TPWString Param_pName);
	void					SetVisibility(TVisibility Param_Visibility);
	void					SetupIconBar(TFloat Param_fIconPositionSlot_Y, TFontGlyph Param_iGlyph_InputIcon);
	void					PrepareConfiguration(TInt Param_iOptionCount);
	TWidgetID				AddWindow(CWidget_Base_Window* Param_pWindow);
	void					AddConfigurationWindow(CWidget_Base_Window* Param_pWindow);
	TWidgetID				AddNotifyIcon(CWidget_Base_NotifyIcon* Param_pNotifyIcon);
	void					SetActiveWindow(TWidgetID Param_WID_NewActiveWindow, eNavLink Param_eNavLink = eNavLink_None);
	CWidget_Base_Window*	GetWindow(TWidgetID Param_WID_Window);
	CWidget_Base_Window*	GetActiveWindow();
	TBool					GetConfiguration(TListID Param_iConfigurationOption);
	void					SetConfiguration(TListID Param_iConfigurationOption, TBool Param_bValue);
	CMessageMaker*			GetMessageMaker();
	void					ActivateNotifyIcon(TWidgetID Param_WID_Icon);
	void					DeactivateNotifyIcon(TWidgetID Param_WID_Icon);
	void					Tick(TFloat Param_fSecTimeDelta);
	void					Render();
	void					SetInputMonitoringToggleButtons(eButton Param_eMonitorToggleButton_Hold, eButton Param_eMonitorToggleButton_Tap);
	void					SetInputMonitoring(TBool Param_bIsMonitoring, TWidgetID Param_WID_NotifyIcon = kiWidgetID_Invalid);
	TBool					MonitorInput_Toggle();
	void					MonitorInput_Navigation();
	void					WaitForMessageExclusion();
};

// ***********************************************************************************************************************
//    Prototypes - Native
// ***********************************************************************************************************************

void*			Memory_Allocate(TInt Param_iSize);
void			Memory_Free(void* Param_pHeapBlock);
void			Memory_Fill(void* Param_pMemory, TInt Param_iSize, TByte Param_byValue = 0);

TBool			Input_IsLocked();
void			Input_SetLock(TBool Param_bIsInputLocked);
TFloat			Input_GetMotion_X();
TFloat			Input_GetMotion_Y();
TBool			Input_IsButtonCurrentlyPressed(eButton Param_eButton);
TBool			Input_IsButtonRecentlyPressed(eButton Param_eButton);

TInt			Texture_GetCount();
TInt			Texture_GetIndexFromID(TEngineResID Param_iEngineResourceID);
TInt			Texture_GetIDFromIndex(TInt Param_iTextureIndex);
TInt			Texture_GetWidth(TInt Param_iTextureIndex);
TInt			Texture_GetHeight(TInt Param_iTextureIndex);
TInt			Texture_GetSize(TInt Param_iTextureIndex);
TPWString		Texture_GetFormat(TInt Param_iTextureIndex);
TBool           Texture_IsValid(TInt Param_iTextureIndex);
TBool			Texture_GetTotalMemoryUsed();

TInt			AudioBuffers_GetCount();
TInt			AudioBuffers_GetUsedMemory();

void			Rendering_PopulateFontDescriptor(stFontDescriptor* Param_pDescriptor, TEngineResID Param_iEngineResourceID);
void			Rendering_DrawRectangle(TFloat Param_fX, TFloat Param_fY, TFloat Param_fWidth, TFloat Param_fHeight, TFloat Param_fU0, TFloat Param_fV0, TFloat Param_fU1, TFloat Param_fV1, TARGBColor Param_iColor, eBlending Param_eBlending, TInt Param_iTextureIndex);

TEngineResID	World_GetID();

void			Profile_Begin(const TPString Param_pString);
void			Profile_Begin(const TPString Param_pFormatString, int Param_iArgument);
void			Profile_End();

// ***********************************************************************************************************************
//    Prototypes - Utils
// ***********************************************************************************************************************

void GetStringSize(TFloat* Param_pReturnSizeX, TFloat* Param_pReturnSizeY, const TPWString Param_szText, TFloat Param_fScaleX, TFloat Param_fScaleY, TFontID Param_iFontID = kiFontID_Desktop_Text, TFloat Param_fClipX = 1.0f, TFloat Param_fClipY = 1.0f);
void DrawString(const TPWString Param_szText, TFloat Param_fXPos, TFloat Param_fYPos, TFloat Param_fScaleX, TFloat Param_fScaleY, TInt Param_iColor, eBlending = eBlending_Alpha, TFontID Param_iFontID = kiFontID_Desktop_Text, TFloat Param_fClipX = 1.0f, TFloat Param_fClipY = 1.0f);
void IntegerToString(TInt Param_iValue, TPWString Param_pDestBuffer, TBool Param_bAddCommas = FALSE, TBool Param_bDisplayAsPercent = FALSE, TInt Param_iDigitCount = 0);

// ***********************************************************************************************************************
//    Global Variables
// ***********************************************************************************************************************

extern CFontManager g_FontManager;

// ***********************************************************************************************************************

} // Namespace

// ***********************************************************************************************************************

#endif //DESKTOP_ENABLE

#endif // __Desktop_h__
