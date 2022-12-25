// ****************************************
// Visual Desktop Windowing Manager
//
// Used for King Kong Xenon Debugging
//
// By Alexandre David (January 2005)
// ****************************************

#ifndef __Desktop_ConstantTypes_h__
#define __Desktop_ConstantTypes_h__

// ***********************************************************************************************************************

namespace Desktop
{

// ***********************************************************************************************************************
//    Macros
// ***********************************************************************************************************************

#define MakeFontGlyph(Param_iIndex, Param_iFontID) ((Param_iFontID << 8) | Param_iIndex)

// ***********************************************************************************************************************

#if !defined(CALLBACK)
#define CALLBACK __stdcall
#endif

// ***********************************************************************************************************************
//    Forward Declarations
// ***********************************************************************************************************************

class CText;

// ***********************************************************************************************************************
//    Typedefs
// ***********************************************************************************************************************

typedef unsigned long	TBool; // Choosen as a 32-bit value compiled to a different type overload from "int"
typedef int				TInt;
typedef float			TFloat;
typedef wchar_t			TWChar;
typedef char			TChar;
typedef unsigned char	TByte;

typedef TWChar*			TPWString;
typedef TChar*			TPString;

typedef TInt			TWidgetID;
typedef TInt			TListID;
typedef TInt			TFlags;
typedef TInt			TVisibility;
typedef TInt			TTrackerType;
typedef TInt			TARGBColor;
typedef TWChar			TFontGlyph;
typedef TByte			TFontID;
typedef TInt			TEngineResID;

typedef void			(CALLBACK* fnOptionNotify      )(void*   Param_pLinkedVariable, TTrackerType Param_iType, TListID Param_iListID);
typedef void			(CALLBACK* fnOptionNotify_Bool )(TBool*  Param_pLinkedVariable, TListID Param_iListID);
typedef void			(CALLBACK* fnOptionNotify_Int  )(TInt*   Param_pLinkedVariable, TInt Param_iOldValue, TListID Param_iListID);
typedef void			(CALLBACK* fnOptionNotify_Float)(TFloat* Param_pLinkedVariable, TFloat Param_fOldValue, TListID Param_iListID);

// ***********************************************************************************************************************
//    Font Information
// ***********************************************************************************************************************

const TFloat		kfFont_AspectRatio	= 3.125f;
const TInt			kiTextureID_None	= -1;
const TEngineResID	kiEngineResID_None	= -1;

// ***********************************************************************************************************************

const TFontID kiFontID_Dynamic			= -1;
const TFontID kiFontID_Native			= 0;
const TFontID kiFontID_Desktop_Text		= 1;
const TFontID kiFontID_Desktop_Glyphs	= 2;

// ***********************************************************************************************************************

typedef struct
{
	TFloat fU0;
	TFloat fV0;
	TFloat fU1;
	TFloat fV1;
}
stFontLetter;

// ***********************************************************************************************************************

typedef struct
{
	TBool			bIsPopulated;
	stFontLetter	astLetter[256];
}
stFontDescriptor;

// ***********************************************************************************************************************

typedef struct
{
	TFontID				iUniqueID;
	TEngineResID		iResID_Descriptor;
	TEngineResID		iResID_Texture;
	stFontDescriptor	stDescriptor;
	TInt				iTextureID;
	TFloat				fScale_X;
	TFloat				fScale_Y;
}
stFont;

// ***********************************************************************************************************************

const TFontID kiFontID_UsedForText		= kiFontID_Desktop_Text;
const TFontID kiFontID_UsedForGlyphs	= kiFontID_Dynamic;

// ***********************************************************************************************************************
//    Enums
// ***********************************************************************************************************************

typedef enum
{
	eNavLink_None = -1,
	eNavLink_Left,
	eNavLink_Right,
	eNavLink_Up,
	eNavLink_Down,
	eNavLink_Forward,
	eNavLink_Back,
	eNavLink_Skip_Forward,
	eNavLink_Skip_Back,

	eNavLink_Count
}
eNavLink;

// ***********************************************************************************************************************

typedef enum
{
	eBlending_Copy,
	eBlending_Alpha,
	eBlending_Add,
	eBlending_AlphaAdd,

	eBlending_Count
}
eBlending;

// ***********************************************************************************************************************

typedef enum
{
	eButton_None,
	eButton_A,
	eButton_B,
	eButton_X,
	eButton_Y,
	eButton_White,
	eButton_Black,
	eButton_Trigger_Left,
	eButton_Trigger_Right,
	eButton_Back,
	eButton_Start,
	eButton_Thumb_Right,
	eButton_Thumb_Left,
	eButton_DPad_Up,
	eButton_DPad_Right,
	eButton_DPad_Down,
	eButton_DPad_Left
}
eButton;

// ***********************************************************************************************************************
//    Constants
// ***********************************************************************************************************************

const TFloat		kfSafeFrame_Left					= 0.06f;
const TFloat		kfSafeFrame_Right					= 0.05f;
const TFloat		kfSafeFrame_Top						= 0.05f;
const TFloat		kfSafeFrame_Bottom					= 0.06f;

const TFloat		kfFrame_ScrollBar_X					= 0.25f;
const TFloat		kfFrame_ScrollBar_Y					= 0.01f;

const TFloat		kfFont_Size_DesktopName				= 0.20f;
const TFloat		kfFont_Size_WindowTitle				= 0.20f;

const TFloat		kfIcon_Size_X						= 0.125f;
const TFloat		kfIcon_Size_Y						= 0.125f;
const TFloat		kfIcon_Spacing_Y					= 0.06f;
const TFloat		kfIcon_DefaultStartPosition_Y		= 0.94f;

const TFloat		kfWindow_Offset_Y					= 0.05f;
const TFloat		kfWindow_TopBarHeight				= 0.1f;

const TFloat		kfInput_ScrollThreshold				= 0.50f;

const TFloat		kfOptionList_Offset_Y				= 0.01f;
const TFloat		kfOptionList_Scroll_Threshold_Y		= 0.05f;
const TFloat		kfOptionList_Scroll_Threshold_X		= 0.05f;

const TFloat		kfOption_Text_Size					= 0.20f;
const TFloat		kfOption_Spacing_Y					= 0.05f;

const TFloat		kfPanelList_Offset_Y				= 0.01f;
const TFloat		kfPanelList_Scroll_Threshold_Y		= 0.05f;
const TFloat		kfPanelList_Scroll_Threshold_X		= 0.05f;

const TFloat		kfMSecValueDelta_IconMove			= 0.0010f;
const TFloat		kfMSecValueDelta_IconFade			= 0.0020f;
const TFloat		kfMSecValueDelta_Scroll				= 0.0002f;
const TFloat		kfMSecValueDelta_ValueUpdateFactor	= 0.0100f;
const TFloat		kfMSecValueDelta_Clipping			= 0.0010f;
const TFloat		kfMSecValueDelta_DesktopVisibility	= 0.0010f;
const TFloat		kfMSecValueDelta_WindowMove			= 0.0010f;
const TFloat		kfMSecValueDelta_WindowFadeIn		= 0.0010f;
const TFloat		kfMSecValueDelta_WindowFadeOut		= 0.0010f;
const TFloat		kfMSecValueDelta_SelectionBarMove	= 0.0005f;
const TFloat		kfMSecValueDelta_PanelMove			= 0.0020f;
const TFloat		kfMSecValueDelta_SelectionBarFade	= 0.0010f;
const TFloat		kfMSecValueDelta_Instant			= 99999.0f;

const TWidgetID		kiWidgetID_Invalid					= -1;
const TWidgetID		kiWidgetID_Signal					= -2;

const TListID		kiListID_Invalid					= -1;
const TListID		kiListID_NoChange					= -2;
const TListID		kiListID_FirstEntry					= -3;
const TListID		kiListID_LastEntry					= -4;
const TListID		kiListID_NextEntry					= -5;
const TListID		kiListID_PreviousEntry				= -6;
const TListID		kiListID_NextTitle					= -7;
const TListID		kiListID_PreviousTitle				= -8;
const TListID		kiListID_NextPage					= -9;
const TListID		kiListID_PreviousPage				= -10;

const TVisibility	kiVisibility_Lower					= -2;
const TVisibility	kiVisibility_Raise					= -1;
const TVisibility	kiVisibility_None					= 0;
const TVisibility	kiVisibility_VeryLow				= 1;
const TVisibility	kiVisibility_Low					= 2;
const TVisibility	kiVisibility_Medium					= 3;
const TVisibility	kiVisibility_High					= 4;
const TVisibility	kiVisibility_VeryHigh				= 5;

const TARGBColor	kiColor_Desktop_Theme_Blue			= 0x00C0C0FF;
const TARGBColor	kiColor_Desktop_Theme_Cyan			= 0x00C0FFFF;
const TARGBColor	kiColor_Desktop_Theme_Green			= 0x00C0FFC0;
const TARGBColor	kiColor_Desktop_Theme_Yellow		= 0x00FFFFC0;
const TARGBColor	kiColor_Desktop_Theme_Red			= 0x00FFC0C0;
const TARGBColor	kiColor_Desktop_Theme_Purple		= 0x00FFC0FF;
const TARGBColor	kiColor_Desktop_Theme_Gray			= 0x00FFFFFF;

const TARGBColor	kiColor_Desktop_Background			= 0x00000000;
const TARGBColor	kiColor_Desktop_Name				= 0xFFFFFFFF;
const TARGBColor	kiColor_Window_Foreground			= 0x00FFFFFF;
const TARGBColor	kiColor_Window_Background			= 0x80404040;
const TARGBColor	kiColor_Window_Title				= 0xFFFFFFFF;
const TARGBColor	kiColor_ScrollBar_Background		= 0x80808080;
const TARGBColor	kiColor_ScrollBar_Slider			= 0xFFA0A0A0;
const TARGBColor	kiColor_OptionList_ItemEnum			= 0xFFFFA0FF;
const TARGBColor	kiColor_OptionList_ItemInt			= 0xFFA0A0FF;
const TARGBColor	kiColor_OptionList_ItemFloat		= 0xFFA0FFFF;
const TARGBColor	kiColor_OptionList_ItemDefault		= 0xFFFFFFFF;
const TARGBColor	kiColor_OptionList_SelectionBar		= 0xA0808080;
const TARGBColor	kiColor_PanelList_SelectionBar		= 0xC0A0A0A0;
const TARGBColor	kiColor_ListHeader_Lines			= 0xA0A0A0A0;
const TARGBColor	kiColor_ListHeader_CentralBand		= 0x80606060;

const TFontGlyph	kcFontGlyph_None					= MakeFontGlyph(  0, 0);
const TFontGlyph	kcFontGlyph_Window_TopLeft			= MakeFontGlyph( 65, kiFontID_Desktop_Glyphs);
const TFontGlyph	kcFontGlyph_Window_Top				= MakeFontGlyph( 66, kiFontID_Desktop_Glyphs);
const TFontGlyph	kcFontGlyph_Window_TopRight			= MakeFontGlyph( 67, kiFontID_Desktop_Glyphs);
const TFontGlyph	kcFontGlyph_Window_Left				= MakeFontGlyph( 68, kiFontID_Desktop_Glyphs);
const TFontGlyph	kcFontGlyph_Window_Right			= MakeFontGlyph( 69, kiFontID_Desktop_Glyphs);
const TFontGlyph	kcFontGlyph_Window_BottomLeft		= MakeFontGlyph( 70, kiFontID_Desktop_Glyphs);
const TFontGlyph	kcFontGlyph_Window_BottomRight		= MakeFontGlyph( 71, kiFontID_Desktop_Glyphs);
const TFontGlyph	kcFontGlyph_Window_Bottom			= MakeFontGlyph( 72, kiFontID_Desktop_Glyphs);
const TFontGlyph	kcFontGlyph_Desktop_TopLeft			= MakeFontGlyph( 73, kiFontID_Desktop_Glyphs);
const TFontGlyph	kcFontGlyph_Desktop_Top				= MakeFontGlyph( 74, kiFontID_Desktop_Glyphs);
const TFontGlyph	kcFontGlyph_Desktop_TopRight		= MakeFontGlyph( 75, kiFontID_Desktop_Glyphs);
const TFontGlyph	kcFontGlyph_Bar_Smooth_Fore			= MakeFontGlyph( 76, kiFontID_Desktop_Glyphs);
const TFontGlyph	kcFontGlyph_Bar_Smooth_Back			= MakeFontGlyph( 77, kiFontID_Desktop_Glyphs);
const TFontGlyph	kcFontGlyph_Bar_Dotted_Fore			= MakeFontGlyph( 78, kiFontID_Desktop_Glyphs);
const TFontGlyph	kcFontGlyph_Bar_Dotted_Back			= MakeFontGlyph( 79, kiFontID_Desktop_Glyphs);
const TFontGlyph	kcFontGlyph_BevelSquare				= MakeFontGlyph( 80, kiFontID_Desktop_Glyphs);
const TFontGlyph	kcFontGlyph_Bar_Size_Fore			= MakeFontGlyph( 81, kiFontID_Desktop_Glyphs);
const TFontGlyph	kcFontGlyph_Bar_Size_Back			= MakeFontGlyph( 82, kiFontID_Desktop_Glyphs);

const TFlags		kiFlags_Widget_None						= 0x00;
const TFlags		kiFlags_Widget_IsAWindow				= 0x01;
const TFlags		kiFlags_Widget_IgnoreParentAlpha		= 0x02;
const TFlags		kiFlags_Widget_IgnoreRenderVisibility	= 0x04;
const TFlags		kiFlags_Widget_AcquireFocusOnCreate		= 0x08;

const TFlags		kiFlags_StringList_None					= 0x00;
const TFlags		kiFlags_StringList_IsAnError			= 0x01;

// ***********************************************************************************************************************
//    Value Tracker Types
// ***********************************************************************************************************************

typedef union
{
	TFloat	AsFloat;
	TInt	AsInt;
}
TFloatInt;

// ***********************************************************************************************************************

typedef TInt TTracker_Storage;

// ***********************************************************************************************************************

const TTracker_Storage	kiTrackerStorage_None		= -1;
const TTracker_Storage	kiTrackerStorage_Integer	= 0;
const TTracker_Storage	kiTrackerStorage_Float		= 1;
const TTracker_Storage	kiTrackerStorage_Boolean	= 2;

// ***********************************************************************************************************************

typedef struct
{
	TInt				iFirstIndex;
	TInt				iCount;
	const TPWString*	aStrings;
}
stTrackerEnum;

// ***********************************************************************************************************************

typedef struct
{
	TTracker_Storage		iStorage;
	TFontGlyph				cGlyph_Background;
	TFontGlyph				cGlyph_Foreground;
	const stTrackerEnum*	pstEnum;
}
stTrackerType;

// ***********************************************************************************************************************

const stTrackerType kstTrackerType_None			= {kiTrackerStorage_None   , kcFontGlyph_None           , kcFontGlyph_None           , NULL};
const stTrackerType kstTrackerType_Integer		= {kiTrackerStorage_Integer, kcFontGlyph_None           , kcFontGlyph_None           , NULL};
const stTrackerType kstTrackerType_Float		= {kiTrackerStorage_Float  , kcFontGlyph_None           , kcFontGlyph_None           , NULL};
const stTrackerType kstTrackerType_Boolean		= {kiTrackerStorage_Boolean, kcFontGlyph_None           , kcFontGlyph_None           , NULL};
const stTrackerType kstTrackerType_Bar_Smooth	= {kiTrackerStorage_Float  , kcFontGlyph_Bar_Smooth_Back, kcFontGlyph_Bar_Smooth_Fore, NULL};
const stTrackerType kstTrackerType_Bar_Dotted	= {kiTrackerStorage_Float  , kcFontGlyph_Bar_Dotted_Back, kcFontGlyph_Bar_Dotted_Fore, NULL};
const stTrackerType kstTrackerType_Bar_Size		= {kiTrackerStorage_Float  , kcFontGlyph_Bar_Size_Back  , kcFontGlyph_Bar_Size_Fore  , NULL};

// ***********************************************************************************************************************
//    StringList Entries
// ***********************************************************************************************************************

typedef struct
{
	TARGBColor	iColor;
	TFlags		iFlags_StringList;
}
stStringList_EntryType;

// ***********************************************************************************************************************

typedef struct
{
	stStringList_EntryType	stType;
	CText*					pText;
}
stStringList_Entry;

// ***********************************************************************************************************************

const stStringList_EntryType kstStringList_EntryType_Basic = {0xFFFFFFFF, kiFlags_StringList_None};

// ***********************************************************************************************************************
//    OptionList Entries
// ***********************************************************************************************************************

typedef struct
{
	TTracker_Storage	iStorageType;
	TWidgetID			iWidgetID;
	void*				pLinkedVariable;
	TFloatInt			fiValue_Minimum;
	TFloatInt			fiValue_Maximum;
	TFloatInt			fiValue_Delta;
	void*				fnNotifyCallback;
}
stOptionList_Entry;

// ***********************************************************************************************************************
//    PanelList Entries
// ***********************************************************************************************************************

typedef struct
{
	TWidgetID iWidgetID;
}
stPanelList_Entry;

// ***********************************************************************************************************************
//    Structs
// ***********************************************************************************************************************

typedef struct
{
	TPWString	pName;
	TBool		bDefaultValue;
	TBool		bIsTitle;
}
stConfigurationEntry;

// ***********************************************************************************************************************

typedef struct
{
	TFloat	fPosition_X;
	TFloat	fPosition_Y;
	TFloat	fSize_X;
	TFloat	fSize_Y;
}
stBox;

// ***********************************************************************************************************************

typedef struct
{
	stBox	stCollisionBox;
	TFloat	fColor_Alpha;
	TFloat	fColor_Red;
	TFloat	fColor_Green;
	TFloat	fColor_Blue;
}
stRenderBase;

// ***********************************************************************************************************************

const stRenderBase kstRenderBase_Identity = { {0.0f, 0.0f, 1.0f, 1.0f}, 1.0f, 1.0f, 1.0f, 1.0f};

// ***********************************************************************************************************************

} // Namespace

using namespace Desktop;

// ***********************************************************************************************************************

#endif // __Desktop_ConstantTypes_h__
