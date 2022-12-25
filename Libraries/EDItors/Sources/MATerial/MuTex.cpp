/*$T MuTex.cpp GC! 1.080 03/23/00 11:32:42 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include <windows.h>
#include <stdio.h>
#define MUTEX_C
#include "Res/res.h"
#include "Mutex.h"
#include "EDImainframe.h"
#include "EDImsg.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "DIAlogs/DIACOLOR_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "GraphicDK/Sources/TEXture/TEXstruct.h"

extern void EDI_Tooltip_DisplayMessage(char *,ULONG ulSpeed = 200);
extern float MAtClippoardProp;
#ifdef JADEFUSION
extern BOOL		b_AutoSetNormalMap;
extern BOOL		b_ShowCLPBRDMT;
#endif
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

static HINSTANCE	MyHinstance;
#define VSIZECLPBRD					15L
#define CLIENTBORDERSIZE			1L
#define NumberOFCheckFlags			2
#define MAD_RESCALE_SPINN_FACTOR	2.0f
static LONG			VSIZEOPEN;
#define VSIZEHALFOPEN	(58 + VSIZE)
#define VSIZETIERCEOPEN (105 + VSIZE)
static LONG			HSIZEREAL;
static LONG			HSIZE;
static LONG			HSIZEM1;
static LONG			HSIZEM2;
static HCURSOR		HandCurs;
static HCURSOR		DragCurs;
static HCURSOR		DragTexCurs;
static HCURSOR		DragTexCpyCurs;
static HCURSOR		DragCopyCurs;

static HCURSOR		ForbCurs;
static HCURSOR		PreviousCurs;
static HCURSOR		BeginCurs;
static LONG			SCROLL_LENGHT = 12;
#define SCROLL_SPEED			1
#define BUTTON_COLOR_BORDERY	6
#define BUTTON_COLOR_BORDERX	6
#define MUTEX_DETECTDRAG		4

static ULONG		BIG_REFRESH_LOCK = 0;

static ULONG		Group1[] =
{
	IDC_STATICColor,
	IDC_STATICTransparency,
	IDC_STATICUV,
	IDC_CHECK9,
	IDC_TransparencyType,
	IDC_BlendingType,
	IDC_SourceType,
	IDC_CHECK7,
	IDC_CHECK8,
	0xffffffff
};

static ULONG		Group2[] =
{
	IDC_CHECKU,
	IDC_CHECK2,
	IDC_CHECK3,
	IDC_CHECK4,
	IDC_CHECK10,
	IDC_CHECK11,
	IDC_CHECK12,
	IDC_CHECK5,
	IDC_BUTTONSETAT,
	IDC_BUTTONSETLA,
	0xffffffff
};

static ULONG		Group3[] =
{
	IDC_SCU,
	IDC_SCV,
	IDC_SCUV,
	IDC_EDITU,
	IDC_EDIT2,
	IDC_EDITROTATION,
	IDC_EDIT4,
	IDC_EDIT5,
	IDC_PosType,
	IDC_STATICROTATION,
	IDC_PSU,
	IDC_PSV,
	IDC_PSUV,
	IDC_PSUV00,
	IDC_SCUV11,
	IDC_BUTTONSETSCU,
	IDC_BUTTONSETSCV,
	IDC_BUTTONSETU,
	IDC_BUTTONSETV,
	IDC_CHECKLINK,
	IDC_MULAYER,
	0xffffffff
};

static ULONG		Group4[] =
{
	IDC_COMBOMATRIXFROM,
	IDC_EDIT6,
	IDC_MTX_RADIO1,
	IDC_MTX_RADIO2,
	IDC_MTX_RADIO3,
	IDC_MTX_RADIO4,
	IDC_CHECKNEGATIV,
	IDC_CHECKUSESCALE,
	IDC_CHECKUSEPOSITION,
	IDC_CHECKDEDUCTALPHA,
	IDC_CHECKSYMETRIC,
	IDC_CHECK13,
	0xffffffff
};
#ifdef JADEFUSION
// SC: Xenon UI controls group
static ULONG XeGroup[] =
{
    IDC_XECHECKEXTENDED,

    // Ambient
    IDC_XEAMBIENT_COLOR_TITLE,
    IDC_XEAMBIENT,

    // Diffuse source
    IDC_XEDIFFUSE_COLORSOURCE_TITLE,
    IDC_XEDIFFUSESOURCE,

    // Specular source
    IDC_XESPECULAR_COLORSOURCE_TITLE,
    IDC_XESPECULARSOURCE,
        
    // Diffuse
    IDC_XEDIFFUSE_COLOR_TITLE,
    IDC_XEDIFFUSE_COLOR,

    // Specular
    IDC_XESPECULAR_COLOR_TITLE,
    IDC_XESPECULAR_COLOR,
    IDC_XESPECULAR_EXP_TITLE,
    IDC_XESPECULAR_EXP,
    IDC_XESBIASTITLE,
    IDC_XESBIAS,

	//OFFSET
	IDC_OFFSETMAP,

    // Mip Mip LOD Bias
    IDC_XEMAP_LODBIAS_TITLE,
    IDC_XEDMAP_MIPBIAS_TITLE,
    IDC_XENMAP_MIPBIAS_TITLE,
    IDC_XEDMAP_LODBIAS,
    IDC_XENMAP_LODBIAS,

    IDC_XEALPHA_RANGE_TITLE,
    IDC_XEALPHA_START,
    IDC_XEALPHA_END,

    // Two sided checkbox
    IDC_XE_TWOSIDED,
    IDC_GLOW_ENABLE_CHK,
    IDC_XE_REFLECT_ON_WATER,

    // Normal map
    IDC_XENMAP_DISABLE,
    IDC_XENMAP,
    IDC_XECLEARNMAP,
    IDC_XENMAP_SCALEU,
    IDC_XENMAP_SCALEU_TITLE,
    IDC_XENMAP_SCALEV,
    IDC_XENMAP_SCALEV_TITLE,
    IDC_XENMAP_ANGLE,
    IDC_XENMAP_ANGLE_TITLE,
    IDC_XENMAP_SPEEDU,
    IDC_XENMAP_SPEEDU_TITLE,
    IDC_XENMAP_SPEEDV,
    IDC_XENMAP_SPEEDV_TITLE,
    IDC_XENMAP_ROLL,
    IDC_XENMAP_ROLL_TITLE,
    IDC_XENMAP_TRANSFORM,
    IDC_XENMAP_ABSOLUTE,
    IDC_XENMAP_SCROLLU,
    IDC_XENMAP_SCROLLV,
  
    // Specular map
    IDC_XESMAP_DISABLE,
    IDC_XESPECULARMAP,
    IDC_XECLEARSPECULARMAP,
    IDC_XESMAP_CHANNEL,
    IDC_XESMAP_SCALEU,
    IDC_XESMAP_SCALEU_TITLE,
    IDC_XESMAP_SCALEV,
    IDC_XESMAP_SCALEV_TITLE,
    IDC_XESMAP_ANGLE,
    IDC_XESMAP_ANGLE_TITLE,
    IDC_XESMAP_SPEEDU,
    IDC_XESMAP_SPEEDU_TITLE,
    IDC_XESMAP_SPEEDV,
    IDC_XESMAP_SPEEDV_TITLE,
    IDC_XESMAP_ROLL,
    IDC_XESMAP_ROLL_TITLE,
    IDC_XESMAP_TRANSFORM,
    IDC_XESMAP_ABSOLUTE,
    IDC_XESMAP_SCROLLU,
    IDC_XESMAP_SCROLLV,
   
    // Environment map
    IDC_XEEMAP_DISABLE,
    IDC_XEEMAP_MAP,
    IDC_XEEMAP_MAP_CLEAR,
    IDC_XEEMAP_COLOR_TITLE,
    IDC_XEEMAP_COLOR,

    // Environment map
    IDC_XEMOSSMAP_DISABLE,
    IDC_XEMOSSMAP_MAP,
    IDC_XEMOSSMAP_MAP_CLEAR,
    IDC_XEMOSSMAP_COLOR_TITLE,
    IDC_XEMOSSMAP_COLOR,
    IDC_XE_MOSSMAP_INVERT,
    IDC_XEMOSS_SPECULAR_TITLE,
    IDC_XEMOSS_SPECULAR,

    // Detail normal map
    IDC_XEDNMAP_DISABLE,
    IDC_XEDNMAP,
    IDC_XECLEARDNMAP,
    IDC_XEDNMAP_SCALEU,
    IDC_XEDNMAP_SCALEU_TITLE,
    IDC_XEDNMAP_SCALEV,
    IDC_XEDNMAP_SCALEV_TITLE,
    IDC_XEDNMAP_SPEEDU,
    IDC_XEDNMAP_SPEEDU_TITLE,
    IDC_XEDNMAP_SPEEDV,
    IDC_XEDNMAP_SPEEDV_TITLE,
    IDC_XEDNMAP_TRANSFORM,
    IDC_XEDNMAP_SCROLLU,
    IDC_XEDNMAP_SCROLLV,
 /*   IDC_XEDNMAP_LOD_TITLE,*///POPOWARNING a terminer
    IDC_XEDNMAP_LODSTART_TITLE,
    IDC_XEDNMAP_LODSTART,
    IDC_XEDNMAP_LODFULL_TITLE,
    IDC_XEDNMAP_LODFULL,
    IDC_XEDNMAP_STRN_TITLE,
    IDC_XEDNMAP_STRN,
    IDC_XEDNMAP_STRN_RANGE,

    // Rim Light
    IDC_XERIMLIGHT_CHK,              
    IDC_XERIM_WIDTH_MIN_TXT,         
    IDC_XERIM_WIDTH_MAX_TXT,         
    IDC_XERIM_WIDTH_MAX_EDT,         
    IDC_XERIM_INTENSITY_EDT,         
    IDC_XERIM_NMAP_RATIO_EDT,       
    IDC_XERIM_NNMAP_RATIO_TXT,       
    IDC_XERIM_INTENSITY_TITLE,       
    IDC_XERIM_WIDTH_MIN_EDT,         
    IDC_XERIM_SMAP_ATTENUATION_CHK,  

    // Xenon Mesh Processing
    IDC_XE_XMP_TITLE,
    IDC_XE_XMP_TESSELLATE,
    IDC_XE_XMP_TESS_AREA_TITLE,
    IDC_XE_XMP_TESS_AREA,
    IDC_XE_XMP_DISPLACE,
    IDC_XE_XMP_DISP_OFFSET_TITLE,
    IDC_XE_XMP_DISP_OFFSET,
    IDC_XE_XMP_DISP_HEIGHT_TITLE,
    IDC_XE_XMP_DISP_HEIGHT,
    IDC_XE_XMP_SMOOTH_TITLE,
    IDC_XE_XMP_SMOOTH,
    IDC_XE_XMP_CHAMFER,
    IDC_XE_XMP_CMF_LEN_TITLE,
    IDC_XE_XMP_CMF_LEN,
    IDC_XE_XMP_CMF_THRESH_TITLE,
    IDC_XE_XMP_CMF_THRESH,
    IDC_XE_XMP_CMF_WELD_TITLE,
    IDC_XE_XMP_CMF_WELD,

    0xffffffff
};

static ULONG XeGroupRight[] =
{
    // Environment map
    IDC_XEEMAP_DISABLE,
    IDC_XEEMAP_MAP,
    IDC_XEEMAP_MAP_CLEAR,
    IDC_XEEMAP_COLOR_TITLE,
    IDC_XEEMAP_COLOR,

    // Moss map
    IDC_XEMOSSMAP_DISABLE,
    IDC_XEMOSSMAP_MAP,
    IDC_XEMOSSMAP_MAP_CLEAR,
    IDC_XEMOSSMAP_COLOR_TITLE,
    IDC_XEMOSSMAP_COLOR,
    IDC_XE_MOSSMAP_INVERT,
    IDC_XEMOSS_SPECULAR_TITLE,
    IDC_XEMOSS_SPECULAR,

    // Rim Light
    IDC_XERIMLIGHT_CHK,              
    IDC_XERIM_WIDTH_MIN_TXT,         
    IDC_XERIM_WIDTH_MAX_TXT,         
    IDC_XERIM_WIDTH_MAX_EDT,         
    IDC_XERIM_INTENSITY_EDT,         
    IDC_XERIM_NMAP_RATIO_EDT,       
    IDC_XERIM_NNMAP_RATIO_TXT,       
    IDC_XERIM_INTENSITY_TITLE,       
    IDC_XERIM_WIDTH_MIN_EDT,         
    IDC_XERIM_SMAP_ATTENUATION_CHK,  

    // Xenon Mesh Processing
    IDC_XE_XMP_TITLE,
    IDC_XE_XMP_TESSELLATE,
    IDC_XE_XMP_TESS_AREA_TITLE,
    IDC_XE_XMP_TESS_AREA,
    IDC_XE_XMP_DISPLACE,
    IDC_XE_XMP_DISP_OFFSET_TITLE,
    IDC_XE_XMP_DISP_OFFSET,
    IDC_XE_XMP_DISP_HEIGHT_TITLE,
    IDC_XE_XMP_DISP_HEIGHT,
    IDC_XE_XMP_SMOOTH_TITLE,
    IDC_XE_XMP_SMOOTH,
    IDC_XE_XMP_CHAMFER,
    IDC_XE_XMP_CMF_LEN_TITLE,
    IDC_XE_XMP_CMF_LEN,
    IDC_XE_XMP_CMF_THRESH_TITLE,
    IDC_XE_XMP_CMF_THRESH,
    IDC_XE_XMP_CMF_WELD_TITLE,
    IDC_XE_XMP_CMF_WELD,

    0xffffffff
};
#endif

// SC: Top and bottom control of the Xenon stuff - Used to measure the size of the window
#define MUTEX_XE_CONTROL_TOP         IDC_XEAMBIENT_COLOR_TITLE
#define MUTEX_XE_CONTROL_BOTTOM      IDC_XE_XMP_DISPLACE
#define MUTEX_XE_CONTROL_HALF_BOTTOM IDC_XESMAP_SCALEV
#define MUTEX_XE_CONTROL_HALF_REF_1  IDC_XENMAP_DISABLE
#define MUTEX_XE_CONTROL_HALF_REF_2  IDC_XEEMAP_DISABLE

// Size of the Xenon stuff - Determined at runtime with the previous constants
static LONG s_l_XeVerticalSize                  = 0;
static LONG s_l_XeHalfModeVerticalSize          = 0;
static LONG s_l_XeHalfModeVerticalDisplacement  = 0;
static LONG s_l_XeVerticalDisplacement          = 97;
static LONG s_l_XeExtendedVerticalSize          = 18;

#define DRAWCOLOR(ITEM, BRUSH, BRUSHRECT, TEXT, TEXTLENGHT) \
	{ \
		HBRUSH	brh; \
		GetWindowRect(GetDlgItem(hwndDlg, ITEM), &LocalRect); \
		ppt.x = LocalRect.left; \
		ppt.y = LocalRect.top; \
		ScreenToClient(hwndDlg, &ppt); \
		LocalRect.left = ppt.x; \
		LocalRect.top = ppt.y; \
		ppt.x = LocalRect.right; \
		ppt.y = LocalRect.bottom; \
		ScreenToClient(hwndDlg, &ppt); \
		LocalRect.right = ppt.x; \
		LocalRect.bottom = ppt.y; \
		brh = CreateSolidBrush(0x00ffffff & BRUSH); \
		FillRect(ps.hdc, &LocalRect, brh); \
		DeleteObject(brh); \
		brh = CreateSolidBrush(0x00ffffff & BRUSHRECT); \
		FrameRect(ps.hdc, &LocalRect, brh); \
		SetBkColor(ps.hdc, 0x00ffffff & BRUSH); \
		LocalRect.top += 5; \
		SetTextColor(ps.hdc, (((BRUSH & 0xfefefe) + 0x808080)) & 0xfefefe); \
		SelectObject(ps.hdc, M_MF()->mo_Fnt); \
		DrawText(ps.hdc, TEXT, TEXTLENGHT, &LocalRect, DT_CENTER | DT_VCENTER); \
		DeleteObject(brh); \
	}

#define DRAWCOLOR_Disabled(ITEM, TEXT, TEXTLENGHT) \
	DRAWCOLOR \
	( \
		ITEM, \
		GetSysColor(COLOR_BTNFACE), \
		0x404040, \
		TEXT, \
		TEXTLENGHT \
	);

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*$off*/
typedef struct			MUTEX_HinstanceInfo_
{
	HWND			hMaterialWindow[MAX_LEVEL_MT];
	ULONG	bDoubled[MAX_LEVEL_MT];
	ULONG	ulHWNDOFFSET;
	MUTEX_Material	*MutexMat;
	HWND			Title;
	HWND			VoidHWND2;
	HWND			MMHWND;
	LONG			YOFFSET;
	LONG			XOFFSET;
	LONG			TotalHeight;
	LONG			XMOUSE;
	LONG			YMOUSE;
	LONG			RightScale;
	LONG			BottomScale;
	LONG			NumberOfSubTextures;
	void			*UserParam;
	int(__stdcall * RefreshCLBK) (void *User, BOOL);
	int				IsClipBoard;
}
MUTEX_HinstanceInfo;

#define MAX_MUTEX_HINST 20

static ULONG	NumberOfHinstances = 0;
MUTEX_HinstanceInfo		AllHinInf[MAX_MUTEX_HINST];
HWND					AllMotherWindw[MAX_MUTEX_HINST];
/*$on*/

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define CHACCES_DETECT(Windw) \
	ULONG ThisHinstance; \
	MUTEX_HinstanceInfo				*Hi; \
	ThisHinstance = 0; \
	for(ThisHinstance = 0; ThisHinstance < MAX_MUTEX_HINST; ThisHinstance++) \
	{ \
		if(AllMotherWindw[ThisHinstance] == Windw) break; \
	} \
	Hi = &AllHinInf[ThisHinstance]

#define SEND_INFORMATION_NO_UNDO() \
	if((!BIG_REFRESH_LOCK) && (CHACCES(RefreshCLBK) != NULL)) CHACCES(RefreshCLBK(CHACCES(UserParam), TRUE));

#define SEND_INFORMATION() \
	if((!BIG_REFRESH_LOCK) && (CHACCES(RefreshCLBK) != NULL)) CHACCES(RefreshCLBK(CHACCES(UserParam), FALSE));

#define SEND_INFORMATION_OTHHINST(a) \
	if((!BIG_REFRESH_LOCK) && (AllHinInf[a].RefreshCLBK != NULL)) \
		AllHinInf[a].RefreshCLBK(AllHinInf[a].UserParam, FALSE);

#define CHACCES(a)				Hi->a

#define MUTEXT_UD_WinIsOpen		0x80000000
#define MUTEXT_UD_WinIsOpenFull 0x40000000

#define KEEPONSCREENHEIGHT		(52L - 18L)

/* #define CLOSELOOSEFOCUS */

/*
 =======================================================================================================================
    __declspec(dllexport) void MUTEX_AdaptToRect(HWND Host); __declspec(dllexport) void MUTEX_Create(HWND Host);
 =======================================================================================================================
 */
#define ADDWINSTYLE(HWND, TYPE, FLAG) \
	{ \
		DWORD	Style; \
		Style = GetWindowLong(HWND, TYPE); \
		Style |= FLAG; \
		SetWindowLong(HWND, TYPE, Style); \
	}

#define SUBWINSTYLE(HWND, TYPE, FLAG) \
	{ \
		DWORD	Style; \
		Style = GetWindowLong(HWND, TYPE); \
		Style &= ~(FLAG); \
		SetWindowLong(HWND, TYPE, Style); \
	}

#define SWITCHWINSTYLE(HWND, TYPE, FLAG) \
	{ \
		DWORD	Style; \
		Style = GetWindowLong(HWND, TYPE); \
		Style ^= FLAG; \
		SetWindowLong(HWND, TYPE, Style); \
	}

#define REDRAWTHISWINDOW(Window) \
	{ \
		InvalidateRect(Window, NULL, TRUE); \
		UpdateWindow(Window); \
	}

#define REDRAWTHISWINDOW2(Window) \
	{ \
		InvalidateRect(Window, NULL, FALSE); \
		UpdateWindow(Window); \
	}
#ifdef JADEFUSION
/*
=======================================================================================================================
=======================================================================================================================
*/

void TextureLineToWindow(HWND hwndDlg, MUTEX_TextureLine *Line);
void TextureLineToWindow_Xenon(HWND _hwndDlg, const MUTEX_XenonTextureLine* _pst_Line);

// ------------------------------------------------------------------------------------------------
// Name   : MUTEX_GetXenonTextureType
// Params : _hTargetWindow : Target window
//          _hParentWindow : Parent dialog handle
// RetVal : Xenon texture type
// Descr. : Get a Xenon texture type based on a selected window
// ------------------------------------------------------------------------------------------------
long MUTEX_GetXenonTextureType(HWND _hTargetWindow, HWND _hParentWindow)
{
    if (_hTargetWindow == GetDlgItem(_hParentWindow, IDC_ChooseTexture))
    {
        return MUTEX_GSBR_XE_BASEMAP;
    }
    else if (_hTargetWindow == GetDlgItem(_hParentWindow, IDC_XENMAP))
    {
        return MUTEX_GSBR_XE_NORMALMAP;
    }
    else if (_hTargetWindow == GetDlgItem(_hParentWindow, IDC_XESPECULARMAP))
    {
        return MUTEX_GSBR_XE_SPECULARMAP;
    }
    else if (_hTargetWindow == GetDlgItem(_hParentWindow, IDC_XEDNMAP))
    {
        return MUTEX_GSBR_XE_DETAILNMAP;
    }
    else if (_hTargetWindow == GetDlgItem(_hParentWindow, IDC_XEMOSSMAP_MAP))
    {
        return MUTEX_GSBR_XE_MOSSMAP;
    }

    return MUTEX_GSBR_XE_NONE;
}

// ------------------------------------------------------------------------------------------------
// Name   : MUTEX_GetTextureId
// Params : _hTargetWindow : Target window
//          _hParentWindow : Parent dialog handle
//          _pLine         : Texture line
// RetVal : Texture Id
// Descr. : Get a texture Id based on a selected window
// ------------------------------------------------------------------------------------------------
long MUTEX_GetTextureId(HWND _hTargetWindow, HWND _hParentWindow, MUTEX_TextureLine* _pLine)
{
    if (_hTargetWindow == GetDlgItem(_hParentWindow, IDC_XENMAP))
    {
        return _pLine->st_XeInfo.l_NMapId;
    }
    else if (_hTargetWindow == GetDlgItem(_hParentWindow, IDC_XESPECULARMAP))
    {
        return _pLine->st_XeInfo.l_SpecularMapId;
    }
    else if (_hTargetWindow == GetDlgItem(_hParentWindow, IDC_XEDNMAP))
    {
        return _pLine->st_XeInfo.l_DNMapId;
    }

    return _pLine->TEXTURE_ID;
}

// ------------------------------------------------------------------------------------------------
// Name   : MUTEX_GetTextureNamePointer
// Params : _hTargetWindow : Target window
//          _hParentWindow : Parent dialog handle
//          _pLine         : Texture line
// RetVal : Texture name pointer
// Descr. : Get a texture name's pointer
// ------------------------------------------------------------------------------------------------
const char* MUTEX_GetTextureNamePointer(HWND _hTargetWindow, HWND _hParentWindow, MUTEX_TextureLine* _pLine)
{
    if (_hTargetWindow == GetDlgItem(_hParentWindow, IDC_XENMAP))
    {
        return _pLine->st_XeInfo.sz_NMapName;
    }
    else if (_hTargetWindow == GetDlgItem(_hParentWindow, IDC_XESPECULARMAP))
    {
        return _pLine->st_XeInfo.sz_SpecularMapName;
    }
    else if (_hTargetWindow == GetDlgItem(_hParentWindow, IDC_XEDNMAP))
    {
        return _pLine->st_XeInfo.sz_DNMapName;
    }

    return _pLine->TextureName;
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL APIENTRY DllMain(HANDLE hInstDLL, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	MyHinstance = (HINSTANCE) hInstDLL;
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		HandCurs = LoadCursor(MyHinstance, MAKEINTRESOURCE(IDC_CURSOR1));
		DragTexCurs = LoadCursor(MyHinstance, MAKEINTRESOURCE(IDC_CURSOR3));
		DragTexCpyCurs = LoadCursor(MyHinstance, MAKEINTRESOURCE(IDC_CURSOR7));
		DragCurs = LoadCursor(MyHinstance, MAKEINTRESOURCE(IDC_CURSOR4));
		DragCopyCurs = LoadCursor(MyHinstance, MAKEINTRESOURCE(IDC_CURSOR6));
		ForbCurs = LoadCursor(MyHinstance, MAKEINTRESOURCE(IDC_CURSOR2));
		BeginCurs = LoadCursor(MyHinstance, MAKEINTRESOURCE(MAIN_IDC_DRAGCOPY));
		memset(AllMotherWindw, 0, sizeof(AllMotherWindw[0]) * MAX_LEVEL_MT);
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}

	return TRUE;
}

static int (__stdcall *Defaultfunc) (void *, unsigned int, unsigned int, LONG);

ULONG			DragResult;

/* Edit float functions */
static ULONG	MUTEX_BigLocker2 = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DISABLEGROUP(HWND Wndw, ULONG *Group, ULONG Disable)
{
	while(*Group != 0xffffffff)
	{
		if(Disable)
		{
			ADDWINSTYLE(GetDlgItem(Wndw, *Group), GWL_STYLE, WS_DISABLED);
		}
		else
		{
			SUBWINSTYLE(GetDlgItem(Wndw, *Group), GWL_STYLE, WS_DISABLED);
		}

		Group++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ValidateAlphaCombo(HWND hwndDlg, MUTEX_TextureLine *Line)
{
	/*~~~~~~~~~~~~*/
	ULONG	Disable;
	/*~~~~~~~~~~~~*/

	if((Line->UVSource == 6) || (Line->UVSource == 4))
		DISABLEGROUP(hwndDlg, Group4, 0);
	else
		DISABLEGROUP(hwndDlg, Group4, 1);

	Disable = 1;
	if(Line->TextureFlags & 16) Disable = 0;
	if(Disable)
	{
		ADDWINSTYLE(GetDlgItem(hwndDlg, IDC_BUTTONSETAT), GWL_STYLE, WS_DISABLED);
	}
	else
	{
		SUBWINSTYLE(GetDlgItem(hwndDlg, IDC_BUTTONSETAT), GWL_STYLE, WS_DISABLED);
	}

	if(Line->TextureTransparency == 1) Disable = 0;
	if(Line->TextureTransparency == 2) Disable = 0;
	if(Line->TextureTransparency == 3) Disable = 0;
	if(Line->TextureTransparency == 4) Disable = 0;
	if(Disable)
	{
		ADDWINSTYLE(GetDlgItem(hwndDlg, IDC_CHECK9), GWL_STYLE, WS_DISABLED);

		/* ADDWINSTYLE(GetDlgItem(hwndDlg, IDC_CHECK12), GWL_STYLE, WS_DISABLED); */
	}
	else
	{
		SUBWINSTYLE(GetDlgItem(hwndDlg, IDC_CHECK9), GWL_STYLE, WS_DISABLED);

		/* SUBWINSTYLE(GetDlgItem(hwndDlg, IDC_CHECK12), GWL_STYLE, WS_DISABLED); */
	}

	if(!(Line->TextureFlags & 1024))
	{
		ADDWINSTYLE(GetDlgItem(hwndDlg, IDC_BUTTONSETLA), GWL_STYLE, WS_DISABLED);
	}
	else
	{
		SUBWINSTYLE(GetDlgItem(hwndDlg, IDC_BUTTONSETLA), GWL_STYLE, WS_DISABLED);
	}

	REDRAWTHISWINDOW(hwndDlg);
	RedrawWindow(hwndDlg, NULL, NULL, RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_FRAME | RDW_INVALIDATE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG VAlidItemFloat(HWND hwndDlg, ULONG ul_Item)
{
	if(MUTEX_BigLocker2) return 0;
	return 1;
}
/*
=======================================================================================================================
=======================================================================================================================
*/
#ifdef JADEFUSION
void InvalidateDlgItem(HWND hwndDlg, ULONG ul_Item)
{
    RECT rc;

    GetWindowRect(GetDlgItem(hwndDlg, ul_Item), &rc);
    ScreenToClient(hwndDlg, (POINT*)&rc.left);
    ScreenToClient(hwndDlg, (POINT*)&rc.right);
    InvalidateRect(hwndDlg, &rc, TRUE);
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef JADEFUSION
void SetDlgItemFloat(HWND hwndDlg, ULONG ul_Item, float Value, bool _bLowPrecision = false)
#else
void SetDlgItemFloat(HWND hwndDlg, ULONG ul_Item, float Value)
#endif
{
	/*~~~~~~~~~~~~*/
	char	TXT[64];
	/*~~~~~~~~~~~~*/

	if(MUTEX_BigLocker2) return;
	MUTEX_BigLocker2 = 1;
#ifdef JADEFUSION
	sprintf(TXT, _bLowPrecision ? "%.2f" : "%.4f", Value);
#else
	sprintf(TXT, "%.4f", Value);
#endif
	SetDlgItemText(hwndDlg, ul_Item, TXT);
	MUTEX_BigLocker2 = 0;
}
#ifdef JADEFUSION
// ------------------------------------------------------------------------------------------------
// Name   : SetDlgItemLong
// Params : _hwndDlg : Dialog
//          _ul_Item : Item in the dialog
//          _l_Value : Value to set
// RetVal : None
// Descr. : Set the text of a control
// ------------------------------------------------------------------------------------------------
void SetDlgItemLong(HWND _hwndDlg, ULONG _ul_Item, LONG _l_Value)
{
    CHAR sz_Txt[32];

    if (MUTEX_BigLocker2) return;
    MUTEX_BigLocker2 = 1;
    sprintf(sz_Txt, "%d", _l_Value);
    SetDlgItemText(_hwndDlg, _ul_Item, sz_Txt);
    MUTEX_BigLocker2 = 0;
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef JADEFUSION
ULONG NumberValid(char *ptr, bool _b_AllowFloat)
#else
ULONG NumberValid(char *ptr)
#endif
{
	while(*ptr != 0)
	{
#ifdef JADEFUSION
		if(((*ptr == '.') && _b_AllowFloat) || ((*ptr >= '0') && (*ptr <= '9')) || (*ptr == '-'))
#else
		if((*ptr == '.') || ((*ptr >= '0') && (*ptr <= '9')) || (*ptr == '-'))
#endif
		{
			ptr++;
		}
		else
			return 0;
	}

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float GetDlgItemFloat(HWND hwndDlg, ULONG ul_Item)
{
	/*~~~~~~~~~~~~~~~*/
	float	fScanned;
	char	uc_TXT[64];
	/*~~~~~~~~~~~~~~~*/

	MUTEX_BigLocker2 = 1;
	GetDlgItemText(hwndDlg, ul_Item, uc_TXT, 64);
#ifdef JADEFUSION
	if(!NumberValid(uc_TXT, true))
#else
	if(!NumberValid(uc_TXT))
#endif
	{
		fScanned = 0.0f;
		sprintf(uc_TXT, "%.2f", fScanned);
		SetDlgItemText(hwndDlg, ul_Item, uc_TXT);
	}
	else
		sscanf(uc_TXT, "%f", &fScanned);

	MUTEX_BigLocker2 = 0;
	return fScanned;
}
#ifdef JADEFUSION
// ------------------------------------------------------------------------------------------------
// Name   : GetDlgItemLong
// Params : _hwndDlg : Dialog handle
//          _ul_Item : Item identifier
// RetVal : Value
// Descr. : Dialog item text to long
// ------------------------------------------------------------------------------------------------
LONG GetDlgItemLong(HWND _hwndDlg, ULONG _ul_Item)
{
    CHAR sz_Text[64];
    LONG l_Value = 0;

    MUTEX_BigLocker2 = 1;

    GetDlgItemText(_hwndDlg, _ul_Item, sz_Text, 63);
    if (!NumberValid(sz_Text, false))
    {
        SetDlgItemText(_hwndDlg, _ul_Item, "0");
    }
    else
        sscanf(sz_Text, "%d", &l_Value);

    MUTEX_BigLocker2 = 0;

    return l_Value;
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CALLBACK MAD_MUTEX_SwapCopyMove(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		POINT MousePoint;

		/*~~~~~~~~~~~~~*/
		RECT	ThisRect;
		/*~~~~~~~~~~~~~*/

		GetCursorPos(&MousePoint);
		GetWindowRect(hwndDlg, &ThisRect);
		MoveWindow
		(
			hwndDlg,
			MousePoint.x - ((ThisRect.right - ThisRect.left) >> 1),
			MousePoint.y - ((ThisRect.bottom - ThisRect.top) >> 1),
			ThisRect.right - ThisRect.left,
			ThisRect.bottom - ThisRect.top,
			TRUE
		);
		return TRUE;
		break;
	case WM_CLOSE:
		DragResult = 0;
		break;
	case WM_COMMAND:
		if(HIWORD(wParam) == BN_CLICKED)
		{
			switch(LOWORD(wParam))
			{
			case IDC_ButtonSWAP:
				DragResult = IDC_ButtonSWAP;
				break;
			case IDC_ButtonMove:
				DragResult = IDC_ButtonMove;
				break;
			case IDC_ButtonCopy:
				DragResult = IDC_ButtonCopy;
				break;
			}

			EndDialog(hwndDlg, 1);
		}
		break;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MUTEX_DeleteLine(MUTEX_Material *MutexMat, ULONG LineNumber)
{
	/*~~~~~~~~~~~~*/
	ULONG	Counter;
	/*~~~~~~~~~~~~*/

	for(Counter = LineNumber; Counter < MutexMat->NumberOfSubTextures; Counter++)
		MutexMat->AllLine[Counter] = MutexMat->AllLine[Counter + 1];
	MutexMat->NumberOfSubTextures--;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MUTEX_MakeHolle(MUTEX_Material *MutexMat, ULONG LineNumber)
{
	/*~~~~~~~~~~~~*/
	ULONG	Counter;
	/*~~~~~~~~~~~~*/

	for(Counter = MutexMat->NumberOfSubTextures; Counter > LineNumber; Counter--)
		MutexMat->AllLine[Counter] = MutexMat->AllLine[Counter - 1];
	MutexMat->NumberOfSubTextures++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG u4_Interpol2PackedColor(ULONG ulP1, ULONG ulP2, float fZClipLocalCoef)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	RetValue, Interpoler;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(fZClipLocalCoef >= 0.98f) return ulP2;
	if(fZClipLocalCoef <= 0.02f) return ulP1;
	*((float *) &Interpoler) = fZClipLocalCoef + 32768.0f + 16384.0f;
	RetValue = (Interpoler & 128) ? (ulP2 & 0xfefefefe) >> 1 : (ulP1 & 0xfefefefe) >> 1;
	RetValue += (Interpoler & 64) ? (ulP2 & 0xfcfcfcfc) >> 2 : (ulP1 & 0xfcfcfcfc) >> 2;
	RetValue += (Interpoler & 32) ? (ulP2 & 0xf8f8f8f8) >> 3 : (ulP1 & 0xf8f8f8f8) >> 3;
	RetValue += (Interpoler & 16) ? (ulP2 & 0xf0f0f0f0) >> 4 : (ulP1 & 0xf0f0f0f0) >> 4;
	RetValue += (Interpoler & 8) ? (ulP2 & 0xe0e0e0e0) >> 5 : (ulP1 & 0xe0e0e0e0) >> 5;
	RetValue += (Interpoler & 4) ? (ulP2 & 0xc0c0c0c0) >> 6 : (ulP1 & 0xc0c0c0c0) >> 6;
	RetValue += (Interpoler & 2) ? (ulP2 & 0x80808080) >> 7 : (ulP1 & 0x80808080) >> 7;
	return RetValue;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CALLBACK MAD_MUTEX_CheckCLBK(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static ULONG	IsDraging = 0;
	static ULONG	Lenght = 0;
	static ULONG	TextureDetected;
	static ULONG	HOTD;
	static ULONG	ThisTexture;
	static HBRUSH	BackBrush = 0;
	static COLORREF color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CHACCES_DETECT(GetParent(GetParent(GetParent(hwndDlg))));
	switch(uMsg)
	{
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORLISTBOX:
		SetBkColor((HDC) wParam, color);
		return (int) BackBrush;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case WM_CTLCOLORDLG:
	case WM_CTLCOLORSTATIC:
		if(BackBrush == 0)
		{
			color = GetSysColor(COLOR_BTNFACE);
			color = u4_Interpol2PackedColor(0, color, 0.85f);
			BackBrush = CreateSolidBrush(color);
		}

		SetBkColor((HDC) wParam, color);
		SetTextColor((HDC) wParam, color);
		return (int) BackBrush;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case WM_LBUTTONUP:
		if(IsDraging)
		{
			ReleaseCapture();
			Lenght = 0;
			if(TextureDetected == -1)
			{
				MUTEX_DeleteLine(CHACCES(MutexMat), ThisTexture);
				MUTEX_SetMat(AllMotherWindw[ThisHinstance], CHACCES(MutexMat));
				SEND_INFORMATION();
			}

			if(IsDraging == 2)
			{
				/*~~~~~~~~~~~~~~~~~~~~~*/
				MUTEX_TextureLine	Swap;
				/*~~~~~~~~~~~~~~~~~~~~~*/

				if(TextureDetected == AllHinInf[HOTD].MutexMat->NumberOfSubTextures)
				{
					AllHinInf[HOTD].MutexMat->AllLine[TextureDetected] = CHACCES(MutexMat)->AllLine[ThisTexture];
					if(GetAsyncKeyState(VK_CONTROL)) MUTEX_DeleteLine(CHACCES(MutexMat), ThisTexture);
					AllHinInf[HOTD].MutexMat->NumberOfSubTextures++;
				}
				else
				{
					if(!GetAsyncKeyState(VK_CONTROL))	/* Duplicate */
					{
						Swap = CHACCES(MutexMat)->AllLine[ThisTexture];
						MUTEX_MakeHolle(AllHinInf[HOTD].MutexMat, TextureDetected);
						AllHinInf[HOTD].MutexMat->AllLine[TextureDetected] = Swap;
					}
					else								/* Move */
					{
						Swap = CHACCES(MutexMat)->AllLine[ThisTexture];
						MUTEX_MakeHolle(AllHinInf[HOTD].MutexMat, TextureDetected);
						AllHinInf[HOTD].MutexMat->AllLine[TextureDetected] = Swap;
						if((HOTD == ThisHinstance) && (TextureDetected < ThisTexture))
							MUTEX_DeleteLine(CHACCES(MutexMat), ThisTexture + 1);
						else
							MUTEX_DeleteLine(CHACCES(MutexMat), ThisTexture);
					}
				}

				MUTEX_SetMat(AllMotherWindw[ThisHinstance], CHACCES(MutexMat));
				SEND_INFORMATION();
				MUTEX_SetMat(AllMotherWindw[HOTD], AllHinInf[HOTD].MutexMat);
				SEND_INFORMATION_OTHHINST(HOTD);
			}

			IsDraging = 0;
		}
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
		if(!IsDraging) break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case WM_MOUSEMOVE:
		if(!IsDraging)
		{
			EDI_Tooltip_DisplayMessage("Click here to open - close,\nor Drag & Drop to clippord or another place",1000);
		}

		if(!(wParam & MK_LBUTTON))
		{
			if(GetCapture() == hwndDlg) ReleaseCapture();
			IsDraging = 0;
            Lenght = 0;
			return TRUE;
		}
		if(IsDraging)
		{
			/*~~~~~~~~~~~~~~~~~~*/
			POINT	MousePoint;
			HWND	WhoIsIt;
			LONG	TextureNumber;
			/*~~~~~~~~~~~~~~~~~~*/

			GetCursorPos(&MousePoint);
			WhoIsIt = WindowFromPoint(MousePoint);
			TextureDetected = -1;
			ThisTexture = 0;
			IsDraging = 1;
			HOTD = ThisHinstance;
			for(TextureNumber = 0; TextureNumber < MAX_LEVEL_MT; TextureNumber++)
			{
				if
				(
					(WhoIsIt == CHACCES(hMaterialWindow[TextureNumber]))
				||	(GetParent(WhoIsIt) == CHACCES(hMaterialWindow[TextureNumber]))
				) TextureDetected = TextureNumber;
				if(GetParent(hwndDlg) == CHACCES(hMaterialWindow[TextureNumber])) ThisTexture = TextureNumber;
			}

			if(TextureDetected == -1)
			{
				/*~~~~~~~~~~~~~*/
				ULONG	HinsCoun;
				/*~~~~~~~~~~~~~*/

				for(HinsCoun = 0; HinsCoun < MAX_MUTEX_HINST; HinsCoun++)
				{
					if(AllMotherWindw[HinsCoun] != 0)
					{
						for(TextureNumber = 0; TextureNumber < MAX_LEVEL_MT; TextureNumber++)
						{
							if
							(
								(WhoIsIt == AllHinInf[HinsCoun].hMaterialWindow[TextureNumber])
							||	(GetParent(WhoIsIt) == AllHinInf[HinsCoun].hMaterialWindow[TextureNumber])
							)
							{
								TextureDetected = TextureNumber;
								HOTD = HinsCoun;
							}
						}

						if((TextureDetected == -1) && (WhoIsIt == AllHinInf[HinsCoun].VoidHWND2))
						{
							TextureDetected = AllHinInf[HinsCoun].MutexMat->NumberOfSubTextures;
							HOTD = HinsCoun;
						}
					}
				}
			}

			if(TextureDetected != -1)
			{
				if((ThisTexture == TextureDetected) && (HOTD == ThisHinstance))
					SetCursor(BeginCurs);
				else
				{
					IsDraging = 2;
					if(!GetAsyncKeyState(VK_CONTROL))
						SetCursor(DragCurs);
					else
						SetCursor(DragCopyCurs);
				}
			}
			else
			{
				SetCursor(ForbCurs);
			}

			return TRUE;
		}

		if(wParam & MK_LBUTTON)
		{
			Lenght++;
			PreviousCurs = GetCursor();
			if(Lenght > MUTEX_DETECTDRAG)
			{
				SetCapture(hwndDlg);
				IsDraging = 1;
				return TRUE;
			}
		}
		break;
	}

	return Defaultfunc(hwndDlg, uMsg, wParam, lParam);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CALLBACK MAD_MUTEX_TextCLBK(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static ULONG		IsDraging = 0;
	static ULONG		Lenght = 0;
	static ULONG		TextureDetected;
	static ULONG		HOTD;
	static ULONG		ThisTexture;
#ifdef JADEFUSION
	static HWND         hCaptureWindow = NULL;
    static long         lXenonTextureSource = MUTEX_GSBR_XE_NONE;
    static long         lXenonDragTarget    = MUTEX_GSBR_XE_NONE;
#endif
	MUTEX_TextureLine	Swap;
	POINT				MousePoint;
	HWND				WhoIsIt;
	LONG				TextureNumber;
	ULONG				HinsCoun;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CHACCES_DETECT(GetParent(GetParent(GetParent(hwndDlg))));
	switch(uMsg)
	{

    /*$2--------------------------------------------------------------------------------------------------------------*/
    case WM_RBUTTONDOWN:
        {
            void TextureLineToWindow_MORE(HWND hwndDlg, MUTEX_TextureLine *Line);
            EMEN_cl_SubMenu     o_Menu(FALSE);
            int                 i_Res;
            EBRO_cl_Frame       *po_Browser;
            ULONG               ul_Key, ul_Index;

            GetCursorPos( &MousePoint );
			WhoIsIt = WindowFromPoint( MousePoint );
			TextureDetected = -1;
			ThisTexture = 0;
			IsDraging = 0;
			HOTD = ThisHinstance;
			for(TextureNumber = 0; TextureNumber < MAX_LEVEL_MT; TextureNumber++)
			{
				if
				(
					(WhoIsIt == CHACCES(hMaterialWindow[TextureNumber]))
				||	(GetParent(WhoIsIt) == CHACCES(hMaterialWindow[TextureNumber]))
				) TextureDetected = TextureNumber;
				if(GetParent(hwndDlg) == CHACCES(hMaterialWindow[TextureNumber])) ThisTexture = TextureNumber;
			}

            if (ThisTexture == -1) 
                break;

            M_MF()->InitPopupMenuAction(NULL, &o_Menu);
            M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Show in browser", -1);
		    i_Res = M_MF()->TrackPopupMenuAction(NULL, MousePoint, &o_Menu);
            switch( i_Res )
            {
            case 1:
                SetFocus(hwndDlg);
                ul_Key = TEX_ul_List_FindTexture(&TEX_gst_GlobalList, (short) CHACCES(MutexMat)->AllLine[ThisTexture].TEXTURE_ID );
                if(ul_Key != BIG_C_InvalidKey)
                {
                    ul_Index = BIG_ul_SearchKeyToFat( ul_Key );
                    if  (ul_Index != BIG_C_InvalidIndex)
                    {
                        po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
                        po_Browser->i_OnMessage( EDI_MESSAGE_SELFILE, BIG_ParentFile( ul_Index ), ul_Index );
                    }
                }
                break;
            }

        }
        break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case WM_LBUTTONUP:
		if(IsDraging)
		{
			ReleaseCapture();
			Lenght = 0;
			if(TextureDetected == -1)
			{
				/*~~~~~~~~~~~~~~~*/
				POINT	MousePoint;
				int		id;
				BIG_KEY ul_Key;
				/*~~~~~~~~~~~~~~~*/

				if(EDI_gst_DragDrop.b_CanDragDrop)
				{
#ifdef JADEFUSION
					id = MUTEX_GetTextureId(hCaptureWindow, 
                                            CHACCES(hMaterialWindow)[ThisTexture], 
                                            &CHACCES(MutexMat)->AllLine[ThisTexture]);
#else
					id = CHACCES(MutexMat->AllLine[ThisTexture]).TEXTURE_ID;
#endif
					ul_Key = TEX_ul_List_FindTexture(&TEX_gst_GlobalList, id);
					if(ul_Key != BIG_C_InvalidKey)
					{
						EDI_gst_DragDrop.ul_FatFile = BIG_ul_SearchKeyToFat(ul_Key);
						EDI_gst_DragDrop.ul_FatDir = BIG_ParentFile(EDI_gst_DragDrop.ul_FatFile);
						EDI_gst_DragDrop.i_Type = EDI_DD_File;
						EDI_gst_DragDrop.po_CaptureWnd = NULL;
						GetCursorPos(&MousePoint);
						EDI_gst_DragDrop.o_Pt = MousePoint;
						EDI_gst_DragDrop.b_BeginDragDrop = TRUE;
						EDI_gst_DragDrop.o_OutRect.SetRectEmpty();
						M_MF()->EndDragDrop(CPoint(MousePoint.x, MousePoint.y));
						EDI_gst_DragDrop.b_BeginDragDrop = FALSE;
						break;
					}
				}

				CHACCES(MutexMat)->AllLine[ThisTexture].TextureName[0] = 0;
				CHACCES(MutexMat)->AllLine[ThisTexture].TEXTURE_ID = -1;
				MUTEX_SetMat(AllMotherWindw[ThisHinstance], CHACCES(MutexMat));
				SEND_INFORMATION();
			}
			else if(IsDraging == 2)
			{
				if(GetAsyncKeyState(VK_CONTROL))	/* Duplicate */
				{
					AllHinInf[HOTD].MutexMat->AllLine[TextureDetected].TEXTURE_ID = CHACCES(MutexMat)->AllLine[ThisTexture].TEXTURE_ID;
					strcpy
					(
						AllHinInf[HOTD].MutexMat->AllLine[TextureDetected].TextureName,
						CHACCES(MutexMat)->AllLine[ThisTexture].TextureName
					);
				}
				else								/* Swap */
				{
					Swap = AllHinInf[HOTD].MutexMat->AllLine[TextureDetected];
					AllHinInf[HOTD].MutexMat->AllLine[TextureDetected].TEXTURE_ID = CHACCES(MutexMat)->AllLine[ThisTexture].TEXTURE_ID;
					strcpy
					(
						AllHinInf[HOTD].MutexMat->AllLine[TextureDetected].TextureName,
						CHACCES(MutexMat)->AllLine[ThisTexture].TextureName
					);
					CHACCES(MutexMat)->AllLine[ThisTexture].TEXTURE_ID = Swap.TEXTURE_ID;
					strcpy(CHACCES(MutexMat)->AllLine[ThisTexture].TextureName, Swap.TextureName);
				}

				MUTEX_SetMat(AllMotherWindw[ThisHinstance], CHACCES(MutexMat));
				SEND_INFORMATION();
				MUTEX_SetMat(AllMotherWindw[HOTD], AllHinInf[HOTD].MutexMat);
				SEND_INFORMATION_OTHHINST(HOTD);
			}
#ifdef JADEFUSION
			else if (IsDraging == 3)
            {
                const char* szTexNamePtr  = MUTEX_GetTextureNamePointer(hCaptureWindow, GetParent(hCaptureWindow), &CHACCES(MutexMat)->AllLine[ThisTexture]);
                long        lSrcTextureId = MUTEX_GetTextureId(hCaptureWindow, GetParent(hCaptureWindow), &CHACCES(MutexMat)->AllLine[ThisTexture]);

                if ((lXenonDragTarget != lXenonTextureSource) || 
                    (HOTD             != ThisHinstance)       || 
                    (ThisTexture      != TextureDetected))
                {
                    switch (lXenonDragTarget)
                    {
                        case MUTEX_GSBR_XE_BASEMAP:
                            AllHinInf[HOTD].MutexMat->AllLine[TextureDetected].TEXTURE_ID = lSrcTextureId;
                            strcpy(AllHinInf[HOTD].MutexMat->AllLine[TextureDetected].TextureName, szTexNamePtr);
                            break;

                        case MUTEX_GSBR_XE_NORMALMAP:
                            AllHinInf[HOTD].MutexMat->AllLine[TextureDetected].st_XeInfo.l_NMapId = lSrcTextureId;
                            strcpy(AllHinInf[HOTD].MutexMat->AllLine[TextureDetected].st_XeInfo.sz_NMapName, szTexNamePtr);
                            break;

                        case MUTEX_GSBR_XE_SPECULARMAP:
                            AllHinInf[HOTD].MutexMat->AllLine[TextureDetected].st_XeInfo.l_SpecularMapId = lSrcTextureId;
                            strcpy(AllHinInf[HOTD].MutexMat->AllLine[TextureDetected].st_XeInfo.sz_SpecularMapName, szTexNamePtr);
                            break;

                        case MUTEX_GSBR_XE_DETAILNMAP:
                            AllHinInf[HOTD].MutexMat->AllLine[TextureDetected].st_XeInfo.l_DNMapId = lSrcTextureId;
                            strcpy(AllHinInf[HOTD].MutexMat->AllLine[TextureDetected].st_XeInfo.sz_DNMapName, szTexNamePtr);
                            break;
                        case MUTEX_GSBR_XE_MOSSMAP:
                            AllHinInf[HOTD].MutexMat->AllLine[TextureDetected].st_XeInfo.l_MossMapId = lSrcTextureId;
                            strcpy(AllHinInf[HOTD].MutexMat->AllLine[TextureDetected].st_XeInfo.sz_MossMapName, szTexNamePtr);
                            break;
                    }

                    TextureLineToWindow(AllMotherWindw[HOTD], &CHACCES(MutexMat)->AllLine[TextureDetected]);
                    MUTEX_SetMat(AllMotherWindw[HOTD], AllHinInf[HOTD].MutexMat);
                    SEND_INFORMATION_OTHHINST(HOTD);
                }
            }
            hCaptureWindow = NULL;
#endif
			IsDraging = 0;
		}
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case WM_KEYDOWN:
	case WM_KEYUP:
		if(!IsDraging) break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case WM_MOUSEMOVE:
		if(!IsDraging)
		{
			EDI_Tooltip_DisplayMessage("Click here to change the texture",1000);
		}

		if(!(wParam & MK_LBUTTON))
		{
			ReleaseCapture();
			IsDraging = 0;
#ifdef JADEFUSION
			hCaptureWindow = NULL;
#endif
			return TRUE;
		}

		if(IsDraging)
		{
			GetCursorPos(&MousePoint);
			WhoIsIt = WindowFromPoint(MousePoint);
			TextureDetected = -1;
			ThisTexture = 0;
			IsDraging = 1;
			HOTD = ThisHinstance;
			for(TextureNumber = 0; TextureNumber < MAX_LEVEL_MT; TextureNumber++)
			{
				if
				(
					(WhoIsIt == CHACCES(hMaterialWindow[TextureNumber]))
				||	(GetParent(WhoIsIt) == CHACCES(hMaterialWindow[TextureNumber]))
				) TextureDetected = TextureNumber;
				if(GetParent(hwndDlg) == CHACCES(hMaterialWindow[TextureNumber])) ThisTexture = TextureNumber;
			}

			if(TextureDetected == -1)
			{
				for(HinsCoun = 0; HinsCoun < MAX_MUTEX_HINST; HinsCoun++)
				{
					if(AllMotherWindw[HinsCoun] != 0)
					{
						for(TextureNumber = 0; TextureNumber < MAX_LEVEL_MT; TextureNumber++)
						{
							if
							(
								(WhoIsIt == AllHinInf[HinsCoun].hMaterialWindow[TextureNumber])
							||	(GetParent(WhoIsIt) == AllHinInf[HinsCoun].hMaterialWindow[TextureNumber])
							)
							{
								TextureDetected = TextureNumber;
								HOTD = HinsCoun;
							}
						}
					}
				}
			}

			if(TextureDetected != -1)
			{
#ifdef JADEFUSION
				if (lXenonTextureSource != MUTEX_GSBR_XE_BASEMAP)
                {
                    lXenonDragTarget = MUTEX_GetXenonTextureType(WhoIsIt, GetParent(WhoIsIt));

                    if ((lXenonDragTarget == MUTEX_GSBR_XE_NONE) || 
                        ((lXenonDragTarget == lXenonTextureSource) && (HOTD == ThisHinstance)))
                    {
                        SetCursor(BeginCurs);
                    }
                    else
                    {
                        IsDraging = 3;
                        SetCursor(DragTexCpyCurs);
                    }
                }
                else
#endif
                {
				    if((ThisTexture == TextureDetected) && (HOTD == ThisHinstance))
					    SetCursor(BeginCurs);
				    else
				    {
					    IsDraging = 2;
					    if(GetAsyncKeyState(VK_CONTROL))
						    SetCursor(DragTexCpyCurs);
					    else
						    SetCursor(DragTexCurs);
				    }
                }
			}
			else
			{
				/*~~~~~~~~~~~~~~~*/
				POINT	MousePoint;
				int		id;
				BIG_KEY ul_Key;
				/*~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
                id = MUTEX_GetTextureId(hCaptureWindow,
                                        CHACCES(hMaterialWindow)[ThisTexture],
                                        &CHACCES(MutexMat)->AllLine[ThisTexture]);
#else
				id = CHACCES(MutexMat->AllLine[ThisTexture]).TEXTURE_ID;
#endif
				ul_Key = TEX_ul_List_FindTexture(&TEX_gst_GlobalList, id);
				if(ul_Key != BIG_C_InvalidKey)
				{
					EDI_gst_DragDrop.ul_FatFile = BIG_ul_SearchKeyToFat(ul_Key);
                    if(EDI_gst_DragDrop.ul_FatFile!=-1)
                    {
					    EDI_gst_DragDrop.ul_FatDir = BIG_ParentFile(EDI_gst_DragDrop.ul_FatFile);
					    EDI_gst_DragDrop.i_Type = EDI_DD_File;
					    EDI_gst_DragDrop.po_CaptureWnd = NULL;
					    EDI_gst_DragDrop.po_SourceEditor = (EDI_cl_BaseFrame *) M_MF()->FromHandle(GetParent(GetParent(GetParent(hwndDlg))));
					    GetCursorPos(&MousePoint);
					    EDI_gst_DragDrop.o_Pt = MousePoint;
					    EDI_gst_DragDrop.b_BeginDragDrop = TRUE;
					    EDI_gst_DragDrop.o_OutRect.SetRectEmpty();
					    M_MF()->b_MoveDragDrop(CPoint(MousePoint.x, MousePoint.y));
					    EDI_gst_DragDrop.b_BeginDragDrop = FALSE;
                    }
				}
			}

			return TRUE;
		}

		if(wParam & MK_LBUTTON)
		{
			Lenght++;
			PreviousCurs = GetCursor();
			if(Lenght > MUTEX_DETECTDRAG)
			{
				SetCapture(hwndDlg);
				IsDraging = 1;
#ifdef JADEFUSION
				if (hCaptureWindow == NULL)
                {
                    GetCursorPos(&MousePoint);
                    hCaptureWindow = WindowFromPoint(MousePoint);
                }
#endif
				return TRUE;
			}
		}
		break;
#ifdef JADEFUSION
		case WM_LBUTTONDOWN:
            if (!IsDraging)
            {
                GetCursorPos(&MousePoint);
                hCaptureWindow      = WindowFromPoint(MousePoint);
                lXenonTextureSource = MUTEX_GetXenonTextureType(hCaptureWindow, GetParent(hCaptureWindow));
                lXenonDragTarget    = MUTEX_GSBR_XE_NONE;
            }
            break;
#endif

	}

	return Defaultfunc(hwndDlg, uMsg, wParam, lParam);
}
#ifdef JADEFUSION
// ------------------------------------------------------------------------------------------------
// Name   : MUTEX_PackTextureInfo
// Params : _ul_LayerId     : Layer Id (Index of the MT level)
//          _ul_XeTextureId : Xenon texture identifier (see MUTEX_GSBR_XE_*)
// RetVal : Packed texture information
// Descr. : Pack the MT level and Xenon texture information
// ------------------------------------------------------------------------------------------------
int MUTEX_PackTextureInfo(ULONG _ul_LayerId, ULONG _ul_XeTextureId)
{
    return (int)((_ul_LayerId & MUTEX_GSBR_LAYER_MASK) | 
        ((_ul_XeTextureId << MUTEX_GSBR_XELAYER_SHIFT) & MUTEX_GSBR_XELAYER_MASK));
}

// ------------------------------------------------------------------------------------------------
// Name   : MUTEX_ExtractLayer
// Params : _i_TexInfo : Packed texture information
// RetVal : MT level index
// Descr. : Get the index of the packed MT level
// ------------------------------------------------------------------------------------------------
int MUTEX_ExtractLayer(int _i_TexInfo)
{
    if (_i_TexInfo == -1)
        return -1;

    return (_i_TexInfo & MUTEX_GSBR_LAYER_MASK);
}

// ------------------------------------------------------------------------------------------------
// Name   : MUTEX_ExtractXeTexture
// Params : _i_TexInfo : Packed texture information
// RetVal : Xenon texture identifier
// Descr. : Get the identifier of the Xenon texture (see MUTEX_GSBR_XE_*)
// ------------------------------------------------------------------------------------------------
int MUTEX_ExtractXeTexture(int _i_TexInfo)
{
    return ((_i_TexInfo & MUTEX_GSBR_XELAYER_MASK) >> MUTEX_GSBR_XELAYER_SHIFT);
}

// ------------------------------------------------------------------------------------------------
// Name   : TextureLineToWindow_Xenon
// Params : _hwndDlg  : Dialog handle
//          _pst_Line : Source line
// RetVal : None
// Descr. : Fill the dialog controls with information from the Xenon texture line
// ------------------------------------------------------------------------------------------------
void TextureLineToWindow_Xenon(HWND _hwndDlg, const MUTEX_XenonTextureLine* _pst_Line)
{
    ERR_X_Assert(_pst_Line != NULL);

    BOOL bHasNMap    = _pst_Line->l_NMapId != MAT_Xe_InvalidTextureId;
    BOOL bHasSMap    = _pst_Line->l_SpecularMapId != MAT_Xe_InvalidTextureId;
    BOOL bHasDNMap   = _pst_Line->l_DNMapId != MAT_Xe_InvalidTextureId;
    BOOL bHasEMap    = _pst_Line->l_EnvMapId != MAT_Xe_InvalidTextureId;
    BOOL bHasMossMap = _pst_Line->l_MossMapId != MAT_Xe_InvalidTextureId;

    // Flags
    // -----
    // Normal map transform
    CheckDlgButton(_hwndDlg, IDC_XENMAP_TRANSFORM,  _pst_Line->b_NMapTransform  ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(_hwndDlg, IDC_XENMAP_ABSOLUTE,   _pst_Line->b_NMapAbsolute   ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(_hwndDlg, IDC_XENMAP_SCROLLU,    _pst_Line->b_NMapScrollU    ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(_hwndDlg, IDC_XENMAP_SCROLLV,    _pst_Line->b_NMapScrollV    ? BST_CHECKED : BST_UNCHECKED);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XENMAP_TRANSFORM), bHasNMap);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XENMAP_ABSOLUTE),  bHasNMap && _pst_Line->b_NMapTransform);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XENMAP_SCROLLU),   bHasNMap && _pst_Line->b_NMapTransform);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XENMAP_SCROLLV),   bHasNMap && _pst_Line->b_NMapTransform);

    // Specular map transform
    CheckDlgButton(_hwndDlg, IDC_XESMAP_TRANSFORM,  _pst_Line->b_SMapTransform  ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(_hwndDlg, IDC_XESMAP_ABSOLUTE,   _pst_Line->b_SMapAbsolute   ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(_hwndDlg, IDC_XESMAP_SCROLLU,    _pst_Line->b_SMapScrollU    ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(_hwndDlg, IDC_XESMAP_SCROLLV,    _pst_Line->b_SMapScrollV    ? BST_CHECKED : BST_UNCHECKED);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XESMAP_TRANSFORM), bHasSMap);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XESMAP_ABSOLUTE),  bHasSMap && _pst_Line->b_SMapTransform);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XESMAP_SCROLLU),   bHasSMap && _pst_Line->b_SMapTransform);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XESMAP_SCROLLV),   bHasSMap && _pst_Line->b_SMapTransform);

    // Detail normal map transform
    CheckDlgButton(_hwndDlg, IDC_XEDNMAP_TRANSFORM, _pst_Line->b_DNMapTransform ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(_hwndDlg, IDC_XEDNMAP_SCROLLU,   _pst_Line->b_DNMapScrollU   ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(_hwndDlg, IDC_XEDNMAP_SCROLLV,   _pst_Line->b_DNMapScrollV   ? BST_CHECKED : BST_UNCHECKED);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XEDNMAP_TRANSFORM), bHasDNMap);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XEDNMAP_SCROLLU),   bHasDNMap && _pst_Line->b_DNMapTransform);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XEDNMAP_SCROLLV),   bHasDNMap && _pst_Line->b_DNMapTransform);

    // Specular
    // --------
    SetDlgItemFloat(_hwndDlg, IDC_XESPECULAR_EXP, _pst_Line->f_SpecularExp, true);

    // Mipmap LOD bias
    // ---------------
    SetDlgItemFloat(_hwndDlg, IDC_XEDMAP_LODBIAS, _pst_Line->f_BaseMipMapLODBias, true);
    SetDlgItemFloat(_hwndDlg, IDC_XENMAP_LODBIAS, _pst_Line->f_NormalMipMapLODBias, true);

    // Alpha range remapping
    SetDlgItemFloat(_hwndDlg, IDC_XEALPHA_START, _pst_Line->f_AlphaStart, true);
    SetDlgItemFloat(_hwndDlg, IDC_XEALPHA_END, _pst_Line->f_AlphaEnd, true);

    // Two sided
    CheckDlgButton(_hwndDlg, IDC_XE_TWOSIDED, _pst_Line->b_TwoSided ? BST_CHECKED : BST_UNCHECKED);

    // Glow
    CheckDlgButton(_hwndDlg, IDC_GLOW_ENABLE_CHK, _pst_Line->b_GlowEnable ? BST_CHECKED : BST_UNCHECKED);

    // Reflect on water
    CheckDlgButton(_hwndDlg, IDC_XE_REFLECT_ON_WATER, _pst_Line->b_ReflectOnWater ? BST_CHECKED : BST_UNCHECKED);

    // Normal map
    // ----------
    // Name
    SetDlgItemText(_hwndDlg, IDC_XENMAP, _pst_Line->sz_NMapName);

    // Offset MAP
    CheckDlgButton(_hwndDlg, IDC_OFFSETMAP, _pst_Line->b_OffsetMap ? BST_CHECKED : BST_UNCHECKED);

    // Enable|Disable
    CheckDlgButton(_hwndDlg, IDC_XENMAP_DISABLE, (bHasNMap && _pst_Line->b_NMapDisabled) ? BST_CHECKED : BST_UNCHECKED);

    // Transform
    SetDlgItemText(_hwndDlg, IDC_XENMAP_SPEEDU_TITLE,  _pst_Line->b_NMapScrollU  ? "SpeedU:" : "StartU:");
    SetDlgItemText(_hwndDlg, IDC_XENMAP_SPEEDV_TITLE,  _pst_Line->b_NMapScrollV  ? "SpeedV:" : "StartV:");
    SetDlgItemFloat(_hwndDlg, IDC_XENMAP_SCALEU, _pst_Line->st_NMapTransform.f_ScaleU);
    SetDlgItemFloat(_hwndDlg, IDC_XENMAP_SCALEV, _pst_Line->st_NMapTransform.f_ScaleV);
    SetDlgItemFloat(_hwndDlg, IDC_XENMAP_ANGLE,  _pst_Line->st_NMapTransform.f_Angle);
    SetDlgItemFloat(_hwndDlg, IDC_XENMAP_SPEEDU, _pst_Line->st_NMapTransform.f_SpeedU);
    SetDlgItemFloat(_hwndDlg, IDC_XENMAP_SPEEDV, _pst_Line->st_NMapTransform.f_SpeedV);
    SetDlgItemFloat(_hwndDlg, IDC_XENMAP_ROLL,   _pst_Line->st_NMapTransform.f_RollSpeed);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XENMAP_SCALEU), bHasNMap && _pst_Line->b_NMapTransform);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XENMAP_SCALEV), bHasNMap && _pst_Line->b_NMapTransform);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XENMAP_SPEEDU), bHasNMap && _pst_Line->b_NMapTransform);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XENMAP_SPEEDV), bHasNMap && _pst_Line->b_NMapTransform);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XENMAP_ANGLE),  bHasNMap && _pst_Line->b_NMapTransform);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XENMAP_ROLL),   bHasNMap && _pst_Line->b_NMapTransform);

    // Specular map
    // ------------
    // Name
    SetDlgItemText(_hwndDlg, IDC_XESPECULARMAP, _pst_Line->sz_SpecularMapName);

    // Enable|Disable
    CheckDlgButton(_hwndDlg, IDC_XESMAP_DISABLE, (bHasSMap && _pst_Line->b_SMapDisabled) ? BST_CHECKED : BST_UNCHECKED);

    // Channel
    SendMessage(GetDlgItem(_hwndDlg, IDC_XESMAP_CHANNEL), CB_RESETCONTENT, 0, 0);
    SendMessage(GetDlgItem(_hwndDlg, IDC_XESMAP_CHANNEL), CB_ADDSTRING, 0, (LONG)"A");
    SendMessage(GetDlgItem(_hwndDlg, IDC_XESMAP_CHANNEL), CB_ADDSTRING, 0, (LONG)"R");
    SendMessage(GetDlgItem(_hwndDlg, IDC_XESMAP_CHANNEL), CB_ADDSTRING, 0, (LONG)"G");
    SendMessage(GetDlgItem(_hwndDlg, IDC_XESMAP_CHANNEL), CB_ADDSTRING, 0, (LONG)"B");
    SendMessage(GetDlgItem(_hwndDlg, IDC_XESMAP_CHANNEL), CB_SETCURSEL, _pst_Line->ul_SpecularMapChannel, 0);

    // Bias
    SetDlgItemFloat(_hwndDlg, IDC_XESBIAS, _pst_Line->f_SpecularBias, true);

    // Transform
    SetDlgItemText(_hwndDlg, IDC_XESMAP_SPEEDU_TITLE,  _pst_Line->b_SMapScrollU  ? "SpeedU:" : "StartU:");
    SetDlgItemText(_hwndDlg, IDC_XESMAP_SPEEDV_TITLE,  _pst_Line->b_SMapScrollV  ? "SpeedV:" : "StartV:");
    SetDlgItemFloat(_hwndDlg, IDC_XESMAP_SCALEU, _pst_Line->st_SMapTransform.f_ScaleU);
    SetDlgItemFloat(_hwndDlg, IDC_XESMAP_SCALEV, _pst_Line->st_SMapTransform.f_ScaleV);
    SetDlgItemFloat(_hwndDlg, IDC_XESMAP_ANGLE,  _pst_Line->st_SMapTransform.f_Angle);
    SetDlgItemFloat(_hwndDlg, IDC_XESMAP_SPEEDU, _pst_Line->st_SMapTransform.f_SpeedU);
    SetDlgItemFloat(_hwndDlg, IDC_XESMAP_SPEEDV, _pst_Line->st_SMapTransform.f_SpeedV);
    SetDlgItemFloat(_hwndDlg, IDC_XESMAP_ROLL,   _pst_Line->st_SMapTransform.f_RollSpeed);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XESMAP_SCALEU), bHasSMap && _pst_Line->b_SMapTransform);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XESMAP_SCALEV), bHasSMap && _pst_Line->b_SMapTransform);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XESMAP_SPEEDU), bHasSMap && _pst_Line->b_SMapTransform);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XESMAP_SPEEDV), bHasSMap && _pst_Line->b_SMapTransform);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XESMAP_ANGLE),  bHasSMap && _pst_Line->b_SMapTransform);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XESMAP_ROLL),   bHasSMap && _pst_Line->b_SMapTransform);

    // Environment map
    // ---------------
    // Name
    SetDlgItemText(_hwndDlg, IDC_XEEMAP_MAP, _pst_Line->sz_EnvMapName);

    // Enable|Disable
    CheckDlgButton(_hwndDlg, IDC_XEEMAP_DISABLE, (bHasEMap && _pst_Line->b_EMapDisabled) ? BST_CHECKED : BST_UNCHECKED);

    // Moss map
    // ---------------
    // Name
    SetDlgItemText(_hwndDlg, IDC_XEMOSSMAP_MAP, _pst_Line->sz_MossMapName);

    // Enable|Disable
    CheckDlgButton(_hwndDlg, IDC_XEMOSSMAP_DISABLE, (bHasMossMap && _pst_Line->b_MossMapDisabled) ? BST_CHECKED : BST_UNCHECKED);

    // invert
    CheckDlgButton(_hwndDlg, IDC_XE_MOSSMAP_INVERT, _pst_Line->b_InvertMoss ? BST_CHECKED : BST_UNCHECKED);

    // specular factor
    SetDlgItemFloat(_hwndDlg, IDC_XEMOSS_SPECULAR, _pst_Line->f_MossSpecularFactor, false);


    // Detail normal map
    // -----------------
    // Name
    SetDlgItemText(_hwndDlg, IDC_XEDNMAP, _pst_Line->sz_DNMapName);

    // Enable|Disable
    CheckDlgButton(_hwndDlg, IDC_XEDNMAP_DISABLE, (bHasDNMap && _pst_Line->b_DNMapDisabled) ? BST_CHECKED : BST_UNCHECKED);

    // OFFSET Enable|Disable
    //CheckDlgButton(_hwndDlg, IDC_XEDNMAP_DISABLE, (bHasDNMap && _pst_Line->b_DNMapDisabled) ? BST_CHECKED : BST_UNCHECKED);

    // Transform
    SetDlgItemText(_hwndDlg, IDC_XEDNMAP_SPEEDU_TITLE, _pst_Line->b_DNMapScrollU ? "SpeedU:" : "StartU:");
    SetDlgItemText(_hwndDlg, IDC_XEDNMAP_SPEEDV_TITLE, _pst_Line->b_DNMapScrollV ? "SpeedV:" : "StartV:");
    SetDlgItemFloat(_hwndDlg, IDC_XEDNMAP_SCALEU, _pst_Line->st_DNMapTransform.f_ScaleU);
    SetDlgItemFloat(_hwndDlg, IDC_XEDNMAP_SCALEV, _pst_Line->st_DNMapTransform.f_ScaleV);
    SetDlgItemFloat(_hwndDlg, IDC_XEDNMAP_SPEEDU, _pst_Line->st_DNMapTransform.f_SpeedU);
    SetDlgItemFloat(_hwndDlg, IDC_XEDNMAP_SPEEDV, _pst_Line->st_DNMapTransform.f_SpeedV);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XEDNMAP_SCALEU), bHasDNMap && _pst_Line->b_DNMapTransform);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XEDNMAP_SCALEV), bHasDNMap && _pst_Line->b_DNMapTransform);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XEDNMAP_SPEEDU), bHasDNMap && _pst_Line->b_DNMapTransform);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XEDNMAP_SPEEDV), bHasDNMap && _pst_Line->b_DNMapTransform);

    // Level of detail
    SetDlgItemLong(_hwndDlg, IDC_XEDNMAP_LODSTART, (LONG)_pst_Line->ul_DNMapLODStart);
    SetDlgItemLong(_hwndDlg, IDC_XEDNMAP_LODFULL,  (LONG)_pst_Line->ul_DNMapLODFull);
    SetDlgItemFloat(_hwndDlg, IDC_XEDNMAP_STRN, _pst_Line->f_DNMapStrength, true);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XEDNMAP_LODSTART), bHasDNMap);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XEDNMAP_LODFULL),  bHasDNMap);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XEDNMAP_STRN),     bHasDNMap);

    // Rim Light
    CheckDlgButton(_hwndDlg, IDC_XERIMLIGHT_CHK, _pst_Line->b_RimLightEnable ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(_hwndDlg, IDC_XERIM_SMAP_ATTENUATION_CHK, _pst_Line->b_RimLightSMapAttenuationEnabled ? BST_CHECKED : BST_UNCHECKED);
	SetDlgItemFloat(_hwndDlg, IDC_XERIM_WIDTH_MIN_EDT,  _pst_Line->f_RimLightWidthMin, true);
    SetDlgItemFloat(_hwndDlg, IDC_XERIM_WIDTH_MAX_EDT,  _pst_Line->f_RimLightWidthMax, true);
    SetDlgItemFloat(_hwndDlg, IDC_XERIM_INTENSITY_EDT,  _pst_Line->f_RimLightIntensity, true);
    SetDlgItemFloat(_hwndDlg, IDC_XERIM_NMAP_RATIO_EDT, _pst_Line->f_RimLightNormalMapRatio, true);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XERIM_WIDTH_MIN_EDT),  _pst_Line->b_RimLightEnable);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XERIM_WIDTH_MAX_EDT),  _pst_Line->b_RimLightEnable);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XERIM_INTENSITY_EDT),  _pst_Line->b_RimLightEnable);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XERIM_NMAP_RATIO_EDT), _pst_Line->b_RimLightEnable);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XERIM_SMAP_ATTENUATION_CHK), _pst_Line->b_RimLightEnable);
	
    // Xenon Mesh Processing
    CheckDlgButton(_hwndDlg,  IDC_XE_XMP_CHAMFER,     _pst_Line->b_AllowChamfer      ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(_hwndDlg,  IDC_XE_XMP_TESSELLATE,  _pst_Line->b_AllowTessellation ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(_hwndDlg,  IDC_XE_XMP_DISPLACE,    _pst_Line->b_AllowDisplacement ? BST_CHECKED : BST_UNCHECKED);
    SetDlgItemFloat(_hwndDlg, IDC_XE_XMP_TESS_AREA,   _pst_Line->f_TessellationArea);
    SetDlgItemFloat(_hwndDlg, IDC_XE_XMP_DISP_OFFSET, _pst_Line->f_DisplacementOffset);
    SetDlgItemFloat(_hwndDlg, IDC_XE_XMP_DISP_HEIGHT, _pst_Line->f_DisplacementHeight);
    SetDlgItemFloat(_hwndDlg, IDC_XE_XMP_SMOOTH,      _pst_Line->f_SmoothThreshold);
    SetDlgItemFloat(_hwndDlg, IDC_XE_XMP_CMF_LEN,     _pst_Line->f_ChamferLength);
    SetDlgItemFloat(_hwndDlg, IDC_XE_XMP_CMF_THRESH,  _pst_Line->f_ChamferThreshold);
    SetDlgItemFloat(_hwndDlg, IDC_XE_XMP_CMF_WELD,    _pst_Line->f_ChamferWeldThreshold);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XE_XMP_TESS_AREA),   _pst_Line->b_AllowTessellation);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XE_XMP_DISP_OFFSET), _pst_Line->b_AllowDisplacement);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XE_XMP_DISP_HEIGHT), _pst_Line->b_AllowDisplacement);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XE_XMP_SMOOTH),      TRUE);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XE_XMP_CMF_LEN),     _pst_Line->b_AllowChamfer);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XE_XMP_CMF_THRESH),  _pst_Line->b_AllowChamfer);
    EnableWindow(GetDlgItem(_hwndDlg, IDC_XE_XMP_CMF_WELD),    _pst_Line->b_AllowChamfer);

    // Ambient
    SendMessage(GetDlgItem(_hwndDlg, IDC_XEAMBIENT), CB_RESETCONTENT, 0, 0);
    SendMessage(GetDlgItem(_hwndDlg, IDC_XEAMBIENT), CB_ADDSTRING, 0, (LONG)"Object");
    SendMessage(GetDlgItem(_hwndDlg, IDC_XEAMBIENT), CB_ADDSTRING, 0, (LONG)"Ambient1");
    SendMessage(GetDlgItem(_hwndDlg, IDC_XEAMBIENT), CB_ADDSTRING, 0, (LONG)"Ambient2");
    SendMessage(GetDlgItem(_hwndDlg, IDC_XEAMBIENT), CB_SETCURSEL, _pst_Line->ul_AmbientSel, 0);

    // Diffuse source
    SendMessage(GetDlgItem(_hwndDlg, IDC_XEDIFFUSESOURCE), CB_RESETCONTENT, 0, 0);
    SendMessage(GetDlgItem(_hwndDlg, IDC_XEDIFFUSESOURCE), CB_ADDSTRING, 0, (LONG)"Material");
    SendMessage(GetDlgItem(_hwndDlg, IDC_XEDIFFUSESOURCE), CB_ADDSTRING, 0, (LONG)"World");
    SendMessage(GetDlgItem(_hwndDlg, IDC_XEDIFFUSESOURCE), CB_SETCURSEL, _pst_Line->ul_DiffuseSourceSel, 0);

    // Specular source
    SendMessage(GetDlgItem(_hwndDlg, IDC_XESPECULARSOURCE), CB_RESETCONTENT, 0, 0);
    SendMessage(GetDlgItem(_hwndDlg, IDC_XESPECULARSOURCE), CB_ADDSTRING, 0, (LONG)"Material");
    SendMessage(GetDlgItem(_hwndDlg, IDC_XESPECULARSOURCE), CB_ADDSTRING, 0, (LONG)"World");
    SendMessage(GetDlgItem(_hwndDlg, IDC_XESPECULARSOURCE), CB_SETCURSEL, _pst_Line->ul_SpecularSourceSel, 0);
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TextureLineToWindow_MORE(HWND hwndDlg, MUTEX_TextureLine *Line)
{
	/*~~~~~~~~~~~~~~~~~~*/
	char	LineName[256];
	/*~~~~~~~~~~~~~~~~~~*/

	BIG_REFRESH_LOCK = 1;
	if(Line->bUIsASpeed)
		SetDlgItemText(hwndDlg, IDC_PSU, "Speed U");
	else
		SetDlgItemText(hwndDlg, IDC_PSU, "Pos U");
	if(Line->bVIsASpeed)
		SetDlgItemText(hwndDlg, IDC_PSV, "Speed V");
	else
		SetDlgItemText(hwndDlg, IDC_PSV, "Pos V");
	SendMessage(GetDlgItem(hwndDlg, IDC_SourceType), CB_SETCURSEL, Line->UVSource, 0);
	SendMessage(GetDlgItem(hwndDlg, IDC_BlendingType), CB_SETCURSEL, Line->TextureBlending, 0);
	SendMessage(GetDlgItem(hwndDlg, IDC_COMBOMATRIXFROM), CB_SETCURSEL, Line->GizmoFrom, 0);

	/*
	 * SendMessage(GetDlgItem(hwndDlg, IDC_AlpahsType), CB_SETCURSEL,
	 * Line->AlphaSource, 0);
	 */
	SendMessage(GetDlgItem(hwndDlg, IDC_PosType), CB_SETCURSEL, (Line->bVIsASpeed << 1) | Line->bUIsASpeed, 0);
	SendMessage(GetDlgItem(hwndDlg, IDC_TransparencyType), CB_SETCURSEL, Line->TextureTransparency, 0);

	/*
	 * SendDlgItemMessage(hwndDlg, IDC_COMBOUVPREDEF, CB_SETCURSEL, PRESETUVCHOOSE_i,
	 * 0);
	 */

	/* SetDlgItemInt(hwndDlg, IDC_EDITROTATION, (LONG) (Line->Rotation), TRUE); */
	SetDlgItemFloat(hwndDlg, IDC_EDITROTATION, (Line->Rotation) * 360.0f);

	SetDlgItemFloat(hwndDlg, IDC_EDITU, (Line->UScale));
	SetDlgItemFloat(hwndDlg, IDC_EDIT2, (Line->VScale));
	SetDlgItemFloat(hwndDlg, IDC_EDIT4, (Line->UPos * 100.0f));
	SetDlgItemFloat(hwndDlg, IDC_EDIT5, (Line->VPos * 100.0f));

	

	if(Line->TextureName[0] == 0)
	{
		SetDlgItemText(hwndDlg, IDC_ChooseTexture, "No Texture");
	}
	else
	{
		SetDlgItemText(hwndDlg, IDC_ChooseTexture, Line->TextureName);
	}

	sprintf
	(
		LineName,
		" %s %s (UV Source %s)",
		MUTEX_TransparencyTypes[Line->TextureTransparency],
		MUTEX_BlendingTypes[Line->TextureBlending],
		MUTEX_UVSources[Line->UVSource]
	);
	SetDlgItemText(hwndDlg, IDC_CHECKOPENTEX, LineName);
	SUBWINSTYLE(hwndDlg, GWL_USERDATA, (MUTEXT_UD_WinIsOpen | MUTEXT_UD_WinIsOpenFull));
	if(Line->IsOpen == 0) CheckDlgButton(hwndDlg, IDC_CHECKOPENTEX, BST_UNCHECKED);
	if(Line->IsOpen == 1)
	{
		ADDWINSTYLE(hwndDlg, GWL_USERDATA, (MUTEXT_UD_WinIsOpen));
		CheckDlgButton(hwndDlg, IDC_CHECKOPENTEX, BST_CHECKED);
	}

	if(Line->IsOpen == 2)
	{
		ADDWINSTYLE(hwndDlg, GWL_USERDATA, (MUTEXT_UD_WinIsOpen | MUTEXT_UD_WinIsOpenFull));
		CheckDlgButton(hwndDlg, IDC_CHECKOPENTEX, BST_INDETERMINATE);
	}

	if(Line->bInactive)
		CheckDlgButton(hwndDlg, IDC_CHECKDesactivate, BST_CHECKED);
	else
		CheckDlgButton(hwndDlg, IDC_CHECKDesactivate, BST_UNCHECKED);

	if(Line->ShiftFace){
		CheckDlgButton(hwndDlg, IDC_MULAYER, BST_CHECKED);
	}
	else{
		CheckDlgButton(hwndDlg, IDC_MULAYER, BST_UNCHECKED);
	}
#ifdef JADEFUSION
	// SC: Xenon properties
    // --------------------
    CheckDlgButton(hwndDlg, IDC_XECHECKEXTENDED, Line->b_XeUseExtendedProperties ? BST_CHECKED : BST_UNCHECKED);
    if (Line->b_XeUseExtendedProperties)
    {
        TextureLineToWindow_Xenon(hwndDlg, &Line->st_XeInfo);
    }
    // --------------------
#endif

	ValidateAlphaCombo(hwndDlg, Line);
	BIG_REFRESH_LOCK = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TextureLineToWindow(HWND hwndDlg, MUTEX_TextureLine *Line)
{
	/*~~~~~~~~*/
	LONG	Add;
	/*~~~~~~~~*/

	BIG_REFRESH_LOCK = 1;

	Add = 0;
	SendMessage(GetDlgItem(hwndDlg, IDC_SourceType), CB_RESETCONTENT, 0, 0);
	while(MUTEX_UVSources[Add][0] != 0)
		SendMessage(GetDlgItem(hwndDlg, IDC_SourceType), CB_ADDSTRING, 0, (LONG) MUTEX_UVSources[Add++]);

	SendMessage(GetDlgItem(hwndDlg, IDC_PosType), CB_RESETCONTENT, 0, 0);
	SendMessage(GetDlgItem(hwndDlg, IDC_PosType), CB_ADDSTRING, 0, (LONG) "No Speed");
	SendMessage(GetDlgItem(hwndDlg, IDC_PosType), CB_ADDSTRING, 0, (LONG) "Speed U");
	SendMessage(GetDlgItem(hwndDlg, IDC_PosType), CB_ADDSTRING, 0, (LONG) "Speed V");
	SendMessage(GetDlgItem(hwndDlg, IDC_PosType), CB_ADDSTRING, 0, (LONG) "Speed UV");

	SendMessage(GetDlgItem(hwndDlg, IDC_COMBOMATRIXFROM), CB_RESETCONTENT, 0, 0);
	Add = 0;
	while(MUTEX_GizmoFrom[Add][0] != 0)
		SendMessage(GetDlgItem(hwndDlg, IDC_COMBOMATRIXFROM), CB_ADDSTRING, 0, (LONG) MUTEX_GizmoFrom[Add++]);

	/*
	 * SendMessage(GetDlgItem(hwndDlg, IDC_AlpahsType), CB_RESETCONTENT, 0, 0);
	 * while(MUTEX_AlphaSources[Add][0] != 0) SendMessage ( GetDlgItem(hwndDlg,
	 * IDC_AlpahsType), CB_ADDSTRING, 0, (LONG) MUTEX_AlphaSources[Add++] ); //
	 * ADDWINSTYLE(GetDlgItem(hwndDlg, IDC_AlpahsType), GWL_STYLE , WS_DISABLED);
	 */

	/*
	 * Add = 0; SendMessage(GetDlgItem(hwndDlg, IDC_COMBOUVPREDEF), CB_RESETCONTENT,
	 * 0, 0); while(MUTEX_PrdefinedUV[Add][0] != 0) SendMessage ( GetDlgItem(hwndDlg,
	 * IDC_COMBOUVPREDEF), CB_ADDSTRING, 0, (LONG) MUTEX_PrdefinedUV[Add++] );
	 */
	SendMessage(GetDlgItem(hwndDlg, IDC_BlendingType), CB_RESETCONTENT, 0, 0);
	Add = 0;
	while(MUTEX_BlendingTypes[Add][0] != 0)
		SendMessage(GetDlgItem(hwndDlg, IDC_BlendingType), CB_ADDSTRING, 0, (LONG) MUTEX_BlendingTypes[Add++]);

	SendMessage(GetDlgItem(hwndDlg, IDC_TransparencyType), CB_RESETCONTENT, 0, 0);
	Add = 0;
	while(MUTEX_TransparencyTypes[Add][0] != 0)
	{
		SendMessage(GetDlgItem(hwndDlg, IDC_TransparencyType), CB_ADDSTRING, 0, (LONG) MUTEX_TransparencyTypes[Add++]);
	}

	if(Line->IsSclLinked)
		CheckDlgButton(hwndDlg, IDC_CHECKLINK, BST_CHECKED);
	else
		CheckDlgButton(hwndDlg, IDC_CHECKLINK, BST_UNCHECKED);

	if(Line->Negativ)
		CheckDlgButton(hwndDlg, IDC_CHECKNEGATIV, BST_CHECKED);
	else
		CheckDlgButton(hwndDlg, IDC_CHECKNEGATIV, BST_UNCHECKED);
	if(Line->Symetric)
		CheckDlgButton(hwndDlg, IDC_CHECKSYMETRIC, BST_CHECKED);
	else
		CheckDlgButton(hwndDlg, IDC_CHECKSYMETRIC, BST_UNCHECKED);
	if(Line->UseScale)
		CheckDlgButton(hwndDlg, IDC_CHECKUSESCALE, BST_CHECKED);
	else
		CheckDlgButton(hwndDlg, IDC_CHECKUSESCALE, BST_UNCHECKED);
	if(Line->DeductAlpha)
		CheckDlgButton(hwndDlg, IDC_CHECKDEDUCTALPHA, BST_CHECKED);
	else
		CheckDlgButton(hwndDlg, IDC_CHECKDEDUCTALPHA, BST_UNCHECKED);

	switch(Line->XYZ)
	{
	case 0:
		CheckRadioButton(hwndDlg, IDC_MTX_RADIO1, IDC_MTX_RADIO4, IDC_MTX_RADIO1);
		break;
	case 1:
		CheckRadioButton(hwndDlg, IDC_MTX_RADIO1, IDC_MTX_RADIO4, IDC_MTX_RADIO2);
		break;
	case 2:
		CheckRadioButton(hwndDlg, IDC_MTX_RADIO1, IDC_MTX_RADIO4, IDC_MTX_RADIO3);
		break;
	case 3:
		CheckRadioButton(hwndDlg, IDC_MTX_RADIO1, IDC_MTX_RADIO4, IDC_MTX_RADIO4);
		break;
	}

	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECKU), SW_HIDE);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK2), SW_HIDE);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK3), SW_HIDE);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK4), SW_HIDE);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK5), SW_HIDE);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK7), SW_HIDE);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK8), SW_HIDE);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK9), SW_HIDE);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK10), SW_HIDE);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK11), SW_HIDE);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK12), SW_HIDE);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK13), SW_HIDE);
	ShowWindow(GetDlgItem(hwndDlg, IDC_MULAYER), SW_HIDE);
	CheckDlgButton(hwndDlg, IDC_CHECKU, BST_UNCHECKED);
	CheckDlgButton(hwndDlg, IDC_CHECK2, BST_UNCHECKED);
	CheckDlgButton(hwndDlg, IDC_CHECK3, BST_UNCHECKED);
	CheckDlgButton(hwndDlg, IDC_CHECK4, BST_UNCHECKED);
	CheckDlgButton(hwndDlg, IDC_CHECK5, BST_UNCHECKED);
	CheckDlgButton(hwndDlg, IDC_CHECK7, BST_UNCHECKED);
	CheckDlgButton(hwndDlg, IDC_CHECK8, BST_UNCHECKED);
	CheckDlgButton(hwndDlg, IDC_CHECK9, BST_UNCHECKED);
	CheckDlgButton(hwndDlg, IDC_CHECK10, BST_UNCHECKED);
	CheckDlgButton(hwndDlg, IDC_CHECK11, BST_UNCHECKED);
	CheckDlgButton(hwndDlg, IDC_CHECK12, BST_UNCHECKED);
	CheckDlgButton(hwndDlg, IDC_CHECK13, BST_UNCHECKED);
	CheckDlgButton(hwndDlg, IDC_MULAYER, BST_UNCHECKED);

	if(Line->TextureFlags & 1) CheckDlgButton(hwndDlg, IDC_CHECKU, BST_CHECKED);
	if(Line->TextureFlags & 2) CheckDlgButton(hwndDlg, IDC_CHECK2, BST_CHECKED);
	if(Line->TextureFlags & 4) CheckDlgButton(hwndDlg, IDC_CHECK3, BST_CHECKED);
	if(Line->TextureFlags & 8) CheckDlgButton(hwndDlg, IDC_CHECK4, BST_CHECKED);
	if(Line->TextureFlags & 16) CheckDlgButton(hwndDlg, IDC_CHECK5, BST_CHECKED);
	if(Line->TextureFlags & 32) CheckDlgButton(hwndDlg, IDC_CHECK7, BST_CHECKED);
	if(Line->TextureFlags & 64) CheckDlgButton(hwndDlg, IDC_CHECK8, BST_CHECKED);
	if(Line->TextureFlags & 128) CheckDlgButton(hwndDlg, IDC_CHECK9, BST_CHECKED);
	if(Line->TextureFlags & 256) CheckDlgButton(hwndDlg, IDC_CHECK10, BST_CHECKED);
	if(Line->TextureFlags & 512) CheckDlgButton(hwndDlg, IDC_CHECK11, BST_CHECKED);
	if(Line->TextureFlags & 1024) CheckDlgButton(hwndDlg, IDC_CHECK12, BST_CHECKED);
	if(Line->CullUV) CheckDlgButton(hwndDlg, IDC_CHECK13, BST_CHECKED);
	if(Line->ShiftFace) CheckDlgButton(hwndDlg, IDC_MULAYER, BST_CHECKED);

	Add = 0;
	if(MUTEX_Flags[Add][0] == 0) goto EndCheck;
	SetDlgItemText(hwndDlg, IDC_CHECKU, MUTEX_Flags[Add++]);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECKU), SW_SHOW);

	if(MUTEX_Flags[Add][0] == 0) goto EndCheck;
	SetDlgItemText(hwndDlg, IDC_CHECK2, MUTEX_Flags[Add++]);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK2), SW_SHOW);

	if(MUTEX_Flags[Add][0] == 0) goto EndCheck;
	SetDlgItemText(hwndDlg, IDC_CHECK3, MUTEX_Flags[Add++]);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK3), SW_SHOW);

	if(MUTEX_Flags[Add][0] == 0) goto EndCheck;
	SetDlgItemText(hwndDlg, IDC_CHECK4, MUTEX_Flags[Add++]);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK4), SW_SHOW);

	if(MUTEX_Flags[Add][0] == 0) goto EndCheck;
	SetDlgItemText(hwndDlg, IDC_CHECK5, MUTEX_Flags[Add++]);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK5), SW_SHOW);

	if(MUTEX_Flags[Add][0] == 0) goto EndCheck;
	SetDlgItemText(hwndDlg, IDC_CHECK7, MUTEX_Flags[Add++]);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK7), SW_SHOW);

	if(MUTEX_Flags[Add][0] == 0) goto EndCheck;
	SetDlgItemText(hwndDlg, IDC_CHECK8, MUTEX_Flags[Add++]);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK8), SW_SHOW);

	if(MUTEX_Flags[Add][0] == 0) goto EndCheck;
	SetDlgItemText(hwndDlg, IDC_CHECK9, MUTEX_Flags[Add++]);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK9), SW_SHOW);

	if(MUTEX_Flags[Add][0] == 0) goto EndCheck;
	SetDlgItemText(hwndDlg, IDC_CHECK10, MUTEX_Flags[Add++]);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK10), SW_SHOW);

	if(MUTEX_Flags[Add][0] == 0) goto EndCheck;
	SetDlgItemText(hwndDlg, IDC_CHECK11, MUTEX_Flags[Add++]);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK11), SW_SHOW);

	if(MUTEX_Flags[Add][0] == 0) goto EndCheck;
	SetDlgItemText(hwndDlg, IDC_CHECK12, MUTEX_Flags[Add++]);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK12), SW_SHOW);

	if(MUTEX_Flags[Add][0] == 0) goto EndCheck;
	SetDlgItemText(hwndDlg, IDC_CHECK13, MUTEX_Flags[Add++]);
	ShowWindow(GetDlgItem(hwndDlg, IDC_CHECK13), SW_SHOW);

	if(MUTEX_Flags[Add][0] == 0) goto EndCheck;
	SetDlgItemText(hwndDlg, IDC_MULAYER, MUTEX_Flags[Add++]);
	ShowWindow(GetDlgItem(hwndDlg, IDC_MULAYER), SW_SHOW);

EndCheck:

	/*
	 * SUBWINSTYLE( hwndDlg , GWL_USERDATA, MUTEXT_UD_WinIsOpen); SetDlgItemInt(
	 * hwndDlg , IDC_CHECKOPENTEX , GetWindowLong(hwndDlg, GWL_USERDATA),FALSE);
	 * CheckDlgButton(hwndDlg, IDC_CHECKOPENTEX, BST_UNCHECKED);
	 */
	TextureLineToWindow_MORE(hwndDlg, Line);

	BIG_REFRESH_LOCK = 0;
}

HWND		ORIGIN_H;
ULONG		*ColorOr;
CHOOSECOLOR *CCLORI;
COLORREF	gpx_ColVal;
BOOL		gb_CanColor;
BOOL		gb_bFirst;
HWND		gh_Static;

BOOL		CallSetfloat = FALSE;
HWND		SetFloatWin;
ULONG		ul_ItemSetFloat;
ULONG		ul_ItemSetFloat2;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void __cdecl BackCall(ULONG User1, ULONG User2)
{
	CHACCES_DETECT(GetParent((HWND) User2));
	REDRAWTHISWINDOW2((HWND) User2);
	SEND_INFORMATION_NO_UNDO();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CDLG_USR_GetColor(ULONG *Color, char *TEXT, HWND MUM)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_ColorDialog *Padjaklj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Padjaklj = new EDIA_cl_ColorDialog(TEXT, Color, BackCall, 0, (ULONG) MUM);
	Padjaklj->DoModal();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CALLBACK MAD_MUTEX_OneLineVOIDCLBK(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static HBRUSH	BackBrush = 0;
	COLORREF		color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(uMsg)
	{
	case WM_CTLCOLORDLG:
		return (int) BackBrush;

	case WM_INITDIALOG:
		if(BackBrush == 0)
		{
			color = GetSysColor(COLOR_BTNFACE);
			color = u4_Interpol2PackedColor(0, color, 0.85f);
			BackBrush = CreateSolidBrush(color);
		}

		return TRUE;
	}

	return FALSE;
};

static ULONG	*Alpha01;
static float	*Scalr01;
static float	*Scalr02;
static float	SavedScalr01;
static float	SavedScalr02;
static float	Factor01;
static float	Factor0X;
static ULONG	HightLimit;
static ULONG	LowLimit;
static ULONG	MAPUV = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CALLBACK MAD_MUTEX_EditScalar(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POINT			MousePointss;
	static POINT	OriginalMousePointss;
	RECT			TheRect;
	static ULONG	CALMOS;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CHACCES_DETECT(GetParent(ORIGIN_H));

	switch(uMsg)
	{
	case WM_INITDIALOG:
		POINT MousePoint;

		/*~~~~~~~~~~~~~*/
		RECT	ThisRect;
		RECT	DeskRect;
		LONG	Height;
		/*~~~~~~~~~~~~~*/

		CALMOS = 3;

		/*
		 * If(HightLimit) { if(*Scalr01 > Factor0X) *Scalr01 = Factor0X; } if(*Scalr01 <
		 * 0.0f) Scalr01 = 0.0f;
		 */
		GetCursorPos(&MousePoint);
		GetCursorPos(&OriginalMousePointss);
		GetWindowRect(hwndDlg, &ThisRect);
		GetWindowRect(GetDesktopWindow(), &DeskRect);
		Height = ThisRect.bottom - ThisRect.top;
		Height -= Height >> 2;
		if(MousePoint.y + Height > DeskRect.bottom) MousePoint.y = DeskRect.bottom - Height;
		if(MousePoint.y < Height) MousePoint.y = Height;

		/* ThisRect.top = ThisRect.left = 0; ThisRect.bottom = 100; ThisRect.right = 25; */
		MoveWindow
		(
			hwndDlg,
			MousePoint.x - ((ThisRect.right - ThisRect.left) >> 1),
			MousePoint.y - ((ThisRect.bottom - ThisRect.top) >> 1),
			ThisRect.right - ThisRect.left,
			ThisRect.bottom - ThisRect.top,
			TRUE
		);
		GetWindowRect(hwndDlg, &TheRect);
		MousePointss.x = TheRect.left + ((TheRect.right - TheRect.left) >> 1);
		MousePointss.y = TheRect.top + (int) ((float) (TheRect.bottom - TheRect.top) * (1.0f - (*Scalr01 * (1.0f / Factor0X))));
		SetCursorPos(MousePointss.x, MousePointss.y);
	case WM_ERASEBKGND:
		return TRUE;
	case WM_CLOSE:
		EndDialog(hwndDlg, 1);
		break;
	case WM_KEYDOWN:
		if((GetAsyncKeyState(VK_SHIFT) < 0) || (GetAsyncKeyState(VK_CONTROL) < 0)) return 0;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		*Scalr01 = SavedScalr01;
		if(Scalr02) *Scalr02 = SavedScalr02;
		if(CallSetfloat)
		{
			SetDlgItemFloat(SetFloatWin, ul_ItemSetFloat, *Scalr01 * Factor01);
			if(ul_ItemSetFloat2 != 0xffffffff) SetDlgItemFloat(SetFloatWin, ul_ItemSetFloat2, *Scalr02 * Factor01);
		}

		SEND_INFORMATION_NO_UNDO();
		REDRAWTHISWINDOW2(hwndDlg);
		ReleaseCapture();
		EndDialog(hwndDlg, 1);

	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
		ReleaseCapture();
		EndDialog(hwndDlg, 1);

		/* SetCursorPos(OriginalMousePointss.x,OriginalMousePointss.y); */
		return FALSE;
	case WM_LBUTTONDOWN:
		GetCursorPos(&MousePointss);
		GetWindowRect(hwndDlg, &TheRect);
		if
		(
			(MousePointss.y < TheRect.top)
		||	(MousePointss.y > TheRect.bottom)
		||	(MousePointss.x < TheRect.left)
		||	(MousePointss.x > TheRect.right)
		)
		{
			ReleaseCapture();
			EndDialog(hwndDlg, 1);
			return FALSE;
		}

		wParam |= MK_LBUTTON;
	case WM_MOUSEMOVE:


		if(CALMOS)
		{
			REDRAWTHISWINDOW2(hwndDlg);
			CALMOS--;
			return TRUE;
		}

		if(wParam & MK_LBUTTON)
		{
			/*~~~~~~~~~~~~~~~*/
			ULONG	AlphaLocal;
			/*~~~~~~~~~~~~~~~*/

			GetCursorPos(&MousePointss);
			GetWindowRect(hwndDlg, &TheRect);
			*Scalr01 = (float) (TheRect.top - MousePointss.y) / (float) (TheRect.top - TheRect.bottom);
			if(LowLimit)
				if(*Scalr01 > 1.0f) *Scalr01 = 1.0f;
			if(HightLimit)
				if(*Scalr01 < 0.0f) *Scalr01 = 0.0f;
			*Scalr01 = 1.0f - *Scalr01;
			*Scalr01 *= Factor0X;
			if(Scalr02)
			{
				if(MAPUV)
				{
					*Scalr02 = (float) (MousePointss.x - TheRect.left) / (float) (TheRect.bottom - TheRect.top);
					if(HightLimit)
						if(*Scalr02 > 1.0f) *Scalr02 = 1.0f;
					if(LowLimit)
						if(*Scalr02 < 0.0f) *Scalr02 = 0.0f;
					*Scalr02 *= Factor0X;
				}
				else
					*Scalr02 = *Scalr01;
			}

			AlphaLocal = (ULONG) (255.f **Scalr01);
			*Alpha01 = (*Alpha01 & 0x00ffffff) | (AlphaLocal << 24);
			if(CallSetfloat)
			{
				SetDlgItemFloat(SetFloatWin, ul_ItemSetFloat, *Scalr01 * Factor01);
				if(ul_ItemSetFloat2 != 0xffffffff)
					SetDlgItemFloat(SetFloatWin, ul_ItemSetFloat2, *Scalr02 * Factor01);
			}

			SEND_INFORMATION_NO_UNDO();
			REDRAWTHISWINDOW2(hwndDlg);
		}

		return TRUE;

		break;
	case WM_PAINT:
		RECT LocalRect;

		/*~~~~~~~~~~~~*/
		PAINTSTRUCT ps;
		HBRUSH		brh;
		/*~~~~~~~~~~~~*/

		SetCapture(hwndDlg);

		/* Paint data for Begin/EndPaint */
		BeginPaint(hwndDlg, &ps);
		GetClientRect(hwndDlg, &LocalRect);
		TheRect = LocalRect;
		*Scalr01 *= 1.0f / Factor0X;
		TheRect.bottom = LocalRect.top = LocalRect.top + (int) ((float) (LocalRect.bottom - LocalRect.top) * (1.0f - *Scalr01));
		*Scalr01 *= Factor0X;
		brh = CreateSolidBrush(0x00F0F0F0);
		FillRect(ps.hdc, &LocalRect, brh);
		DeleteObject(brh);
		brh = CreateSolidBrush(0x000F0F0F);
		FillRect(ps.hdc, &TheRect, brh);
		DeleteObject(brh);
		EndPaint(hwndDlg, &ps);
		break;
	}

	return FALSE;
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MUTEX_EditScalar01(HWND Mother, float *Value, float *Value2)
{
	SavedScalr01 = *Value;
	Scalr01 = Value;
	if(Value2) SavedScalr02 = *Value2;
	Scalr02 = Value2;
	ORIGIN_H = Mother;
	DialogBox
	(
		MyHinstance,
		MAKEINTRESOURCE(IDD_SCALARADJUST),
		Mother,
		/* (int(__stdcall *) (void)) &MAD_MUTEX_EditScalar */
			(int(__stdcall *) (struct HWND__ *, unsigned int, unsigned int, LONG)) MAD_MUTEX_EditScalar
	);
	CallSetfloat = FALSE;
	MAPUV = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CALLBACK MAD_MUTEX_OneLineCLBK(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static ULONG	IsMoving = 0;
	static LONG		XOFFSET;
	static LONG		YOFFSET;
	static HWND		FOCUSWINDOW;
	static POINT	MousePoint;
	static BOOL		bLocalLock;
	static HBRUSH	BackBrush = 0;
	static COLORREF color;
	static HBRUSH	BackBrushEdit;
	static COLORREF colorEdit;
	LONG			Add, Style;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CHACCES_DETECT(GetParent(GetParent(hwndDlg)));
	switch(uMsg)
	{
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORLISTBOX:
		SetBkColor((HDC) wParam, colorEdit);
		return (int) BackBrushEdit;
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORSTATIC:
		Style = GetWindowLong((HWND) lParam, GWL_EXSTYLE);
		if(!(Style & WS_EX_DLGMODALFRAME))
		{
			SetBkColor((HDC) wParam, color);
			return (int) BackBrush;
		}
		break;

	case WM_INITDIALOG:
		if(BackBrush == 0)
		{
			color = GetSysColor(COLOR_BTNFACE);
			color = u4_Interpol2PackedColor(0, color, 0.85f);
			BackBrush = CreateSolidBrush(color);
			colorEdit = 0xe8ffe8;
			BackBrushEdit = CreateSolidBrush(colorEdit);
		}

		Add = GetWindowLong(GetDlgItem(hwndDlg, IDC_CHECKOPENTEX), GWL_WNDPROC);
		*(LONG *) &Defaultfunc = Add;
		Add = (LONG) MAD_MUTEX_CheckCLBK;
		SetWindowLong(GetDlgItem(hwndDlg, IDC_CHECKOPENTEX), GWL_WNDPROC, Add);

		Add = GetWindowLong(GetDlgItem(hwndDlg, IDC_ChooseTexture), GWL_WNDPROC);
		*(LONG *) &Defaultfunc = Add;
		Add = (LONG) MAD_MUTEX_TextCLBK;
		SetWindowLong(GetDlgItem(hwndDlg, IDC_ChooseTexture), GWL_WNDPROC, Add);

#ifdef JADEFUSION
        // Drag and drop callback
        Add = GetWindowLong(GetDlgItem(hwndDlg, IDC_XENMAP), GWL_WNDPROC);
        *(LONG *) &Defaultfunc = Add;
        Add = (LONG)MAD_MUTEX_TextCLBK;
        SetWindowLong(GetDlgItem(hwndDlg, IDC_XENMAP), GWL_WNDPROC, Add);

        // Drag and drop callback
        Add = GetWindowLong(GetDlgItem(hwndDlg, IDC_XESPECULARMAP), GWL_WNDPROC);
        *(LONG *) &Defaultfunc = Add;
        Add = (LONG)MAD_MUTEX_TextCLBK;
        SetWindowLong(GetDlgItem(hwndDlg, IDC_XESPECULARMAP), GWL_WNDPROC, Add);

        // Drag and drop callback
        Add = GetWindowLong(GetDlgItem(hwndDlg, IDC_XEDNMAP), GWL_WNDPROC);
        *(LONG *) &Defaultfunc = Add;
        Add = (LONG)MAD_MUTEX_TextCLBK;
        SetWindowLong(GetDlgItem(hwndDlg, IDC_XEDNMAP), GWL_WNDPROC, Add);
#endif

		SetWindowLong(hwndDlg, GWL_USERDATA, lParam);
		SetDlgItemInt(hwndDlg, IDC_CHECKOPENTEX, GetWindowLong(hwndDlg, GWL_USERDATA), FALSE);
		bLocalLock = FALSE;
		return TRUE;
		break;
	case WM_PAINT:
		if(CHACCES(MutexMat))
		{
			/*~~~~~~~~~~~~~~~~~~*/
			POINT		ppt;
			RECT		LocalRect;
			PAINTSTRUCT ps;
			float		IPOS;
			/*~~~~~~~~~~~~~~~~~~*/

			BeginPaint(hwndDlg, &ps);

			if(!(WS_DISABLED & GetWindowLong(GetDlgItem(hwndDlg, IDC_BUTTONSETAT), GWL_STYLE)))
			{
				DRAWCOLOR(IDC_BUTTONSETAT, 0, 0, "Thresh", 6);
				GetWindowRect(GetDlgItem(hwndDlg, IDC_BUTTONSETAT), &LocalRect);
				ppt.x = LocalRect.left;
				ppt.y = LocalRect.top;
				ScreenToClient(hwndDlg, &ppt);
				LocalRect.left = ppt.x + 2;
				LocalRect.top = ppt.y + 2;
				ppt.x = LocalRect.right;
				ppt.y = LocalRect.bottom;
				ScreenToClient(hwndDlg, &ppt);
				LocalRect.right = ppt.x - 2;
				LocalRect.bottom = ppt.y - 2;
				IPOS = CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).AlphaTestTreshold / 256.0f;
				LocalRect.right = LocalRect.left + (int) ((float) (LocalRect.right - LocalRect.left) * IPOS);
				InvertRect(ps.hdc, &LocalRect);
			}

			if(!(WS_DISABLED & GetWindowLong(GetDlgItem(hwndDlg, IDC_BUTTONSETLA), GWL_STYLE)))
			{
				DRAWCOLOR(IDC_BUTTONSETLA, 0, 0, "Value", 5);
				GetWindowRect(GetDlgItem(hwndDlg, IDC_BUTTONSETLA), &LocalRect);
				ppt.x = LocalRect.left;
				ppt.y = LocalRect.top;
				ScreenToClient(hwndDlg, &ppt);
				LocalRect.left = ppt.x + 2;
				LocalRect.top = ppt.y + 2;
				ppt.x = LocalRect.right;
				ppt.y = LocalRect.bottom;
				ScreenToClient(hwndDlg, &ppt);
				LocalRect.right = ppt.x - 2;
				LocalRect.bottom = ppt.y - 2;
				IPOS = CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).LocalAlpha;
				LocalRect.right = LocalRect.left + (int) ((float) (LocalRect.right - LocalRect.left) * IPOS);
				InvertRect(ps.hdc, &LocalRect);
			}

#ifdef JADEFUSION
            // Diffuse color
            if (CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).b_XeUseExtendedProperties)
            {
                DRAWCOLOR(IDC_XEDIFFUSE_COLOR, (CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.ul_DiffuseColor), 0, "", 0);
            }

            // Specular color
            if (CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).b_XeUseExtendedProperties)
            {
                DRAWCOLOR(IDC_XESPECULAR_COLOR, (CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.ul_SpecularColor), 0, "", 0);
            }

            // Environment map color
            if (CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).b_XeUseExtendedProperties)
            {
                DRAWCOLOR(IDC_XEEMAP_COLOR, (CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.ul_EnvMapColor), 0, "", 0);
            }

            // Moss map color
            if (CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).b_XeUseExtendedProperties)
            {
                DRAWCOLOR(IDC_XEMOSSMAP_COLOR, (CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.ul_MossMapColor), 0, "", 0);
            }
#endif
			EndPaint(hwndDlg, &ps);
			return FALSE;
		}

	case WM_SIZE:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			HWND	MainCheck;
			RECT	Pos, PosMC, PosParent, PosDesacte;
			ULONG	DeltaX, VerticalSize;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			VerticalSize = VSIZE;
			if(CHACCES(IsClipBoard)) VerticalSize = VSIZECLPBRD;
			GetWindowRect(GetDlgItem(hwndDlg, IDC_CHECKDesactivate), &PosDesacte);
			HSIZEM1 = PosDesacte.right - PosDesacte.left;
			GetWindowRect(GetDlgItem(hwndDlg, IDC_ChooseTexture), &PosDesacte);
			HSIZEM2 = PosDesacte.right - PosDesacte.left;
			GetWindowRect(GetParent(hwndDlg), &PosParent);
			GetWindowRect(hwndDlg, &Pos);
			MainCheck = GetDlgItem(hwndDlg, IDC_CHECKOPENTEX);
			GetWindowRect(MainCheck, &PosMC);
			DeltaX = PosParent.left - Pos.left;
			if(DeltaX < 0) DeltaX = 0;
			DeltaX += CLIENTBORDERSIZE;
			MoveWindow(GetDlgItem(hwndDlg, IDC_CHECKDesactivate), DeltaX, 0, HSIZEM1, VerticalSize + 1, TRUE);
			MoveWindow(GetDlgItem(hwndDlg, IDC_ChooseTexture), DeltaX + HSIZEM1, 0, HSIZEM2, VerticalSize + 1, TRUE);
			MoveWindow
			(
				MainCheck,
				DeltaX + HSIZEM1 + HSIZEM2,
				0,
				(PosParent.right - PosParent.left) - HSIZEM1 - HSIZEM2 - 1,
				VerticalSize + 1,
				TRUE
			);
		}
		break;

	case WM_LBUTTONUP:
		ReleaseCapture();
		break;
	case WM_LBUTTONDOWN:
		{
			/*~~~~~~~~~~~~~*/
			RECT	THERECTE;
			POINT	MC;
			/*~~~~~~~~~~~~~*/

			GetWindowRect(GetDlgItem(hwndDlg, IDC_SCU), &THERECTE);
			GetCursorPos(&MC);
			if((MC.x < THERECTE.right) && (MC.x > THERECTE.left) && (MC.y > THERECTE.top) && (MC.y < THERECTE.bottom))
			{
				SendMessage(hwndDlg, WM_COMMAND, (BN_CLICKED << 16) | IDC_BUTTONSETSCU, 0);
				return FALSE;
			}

			GetWindowRect(GetDlgItem(hwndDlg, IDC_SCV), &THERECTE);
			GetCursorPos(&MC);
			if((MC.x < THERECTE.right) && (MC.x > THERECTE.left) && (MC.y > THERECTE.top) && (MC.y < THERECTE.bottom))
			{
				SendMessage(hwndDlg, WM_COMMAND, (BN_CLICKED << 16) | IDC_BUTTONSETSCV, 0);
				return FALSE;
			}

			GetWindowRect(GetDlgItem(hwndDlg, IDC_SCUV), &THERECTE);
			GetCursorPos(&MC);
			if((MC.x < THERECTE.right) && (MC.x > THERECTE.left) && (MC.y > THERECTE.top) && (MC.y < THERECTE.bottom))
			{
				SendMessage(hwndDlg, WM_COMMAND, (BN_CLICKED << 16) | IDC_BUTTONSETSCUV, 0);
				return FALSE;
			}

			GetWindowRect(GetDlgItem(hwndDlg, IDC_PSUV00), &THERECTE);
			GetCursorPos(&MC);
			if((MC.x < THERECTE.right) && (MC.x > THERECTE.left) && (MC.y > THERECTE.top) && (MC.y < THERECTE.bottom))
			{
				SendMessage(hwndDlg, WM_COMMAND, (BN_CLICKED << 16) | IDC_PSUV00, 0);
				return FALSE;
			}

			GetWindowRect(GetDlgItem(hwndDlg, IDC_SCUV11), &THERECTE);
			GetCursorPos(&MC);
			if((MC.x < THERECTE.right) && (MC.x > THERECTE.left) && (MC.y > THERECTE.top) && (MC.y < THERECTE.bottom))
			{
				SendMessage(hwndDlg, WM_COMMAND, (BN_CLICKED << 16) | IDC_SCUV11, 0);
				return FALSE;
			}

			GetWindowRect(GetDlgItem(hwndDlg, IDC_PSU), &THERECTE);
			GetCursorPos(&MC);
			if((MC.x < THERECTE.right) && (MC.x > THERECTE.left) && (MC.y > THERECTE.top) && (MC.y < THERECTE.bottom))
			{
				SendMessage(hwndDlg, WM_COMMAND, (BN_CLICKED << 16) | IDC_BUTTONSETU, 0);
				return FALSE;
			}

			GetWindowRect(GetDlgItem(hwndDlg, IDC_PSV), &THERECTE);
			GetCursorPos(&MC);
			if((MC.x < THERECTE.right) && (MC.x > THERECTE.left) && (MC.y > THERECTE.top) && (MC.y < THERECTE.bottom))
			{
				SendMessage(hwndDlg, WM_COMMAND, (BN_CLICKED << 16) | IDC_BUTTONSETV, 0);
				return FALSE;
			}

			GetWindowRect(GetDlgItem(hwndDlg, IDC_STATICROTATION), &THERECTE);
			GetCursorPos(&MC);
			if((MC.x < THERECTE.right) && (MC.x > THERECTE.left) && (MC.y > THERECTE.top) && (MC.y < THERECTE.bottom))
			{
				SendMessage(hwndDlg, WM_COMMAND, (BN_CLICKED << 16) | IDC_STATICROTATION, 0);
				return FALSE;
			}

			GetWindowRect(GetDlgItem(hwndDlg, IDC_PSUV), &THERECTE);
			GetCursorPos(&MC);
			if((MC.x < THERECTE.right) && (MC.x > THERECTE.left) && (MC.y > THERECTE.top) && (MC.y < THERECTE.bottom))
			{
				SendMessage(hwndDlg, WM_COMMAND, (BN_CLICKED << 16) | IDC_BUTTONSETUV, 0);
				return FALSE;
			}

			if(!(WS_DISABLED & GetWindowLong(GetDlgItem(hwndDlg, IDC_BUTTONSETAT), GWL_STYLE)))
			{
				GetWindowRect(GetDlgItem(hwndDlg, IDC_BUTTONSETAT), &THERECTE);
				GetCursorPos(&MC);
				if
				(
					(MC.x < THERECTE.right)
				&&	(MC.x > THERECTE.left)
				&&	(MC.y > THERECTE.top)
				&&	(MC.y < THERECTE.bottom)
				)
				{
					SendMessage(hwndDlg, WM_COMMAND, (BN_CLICKED << 16) | IDC_BUTTONSETAT, 0);
					return FALSE;
				}
			}

			if(!(WS_DISABLED & GetWindowLong(GetDlgItem(hwndDlg, IDC_BUTTONSETLA), GWL_STYLE)))
			{
				GetWindowRect(GetDlgItem(hwndDlg, IDC_BUTTONSETLA), &THERECTE);
				GetCursorPos(&MC);
				if
				(
					(MC.x < THERECTE.right)
				&&	(MC.x > THERECTE.left)
				&&	(MC.y > THERECTE.top)
				&&	(MC.y < THERECTE.bottom)
				)
				{
					SendMessage(hwndDlg, WM_COMMAND, (BN_CLICKED << 16) | IDC_BUTTONSETLA, 0);
					return FALSE;
				}
			}
#ifdef JADEFUSION
            GetWindowRect(GetDlgItem(hwndDlg, IDC_XEDIFFUSE_COLOR), &THERECTE);
            GetCursorPos(&MC);
            if((MC.x < THERECTE.right) && (MC.x > THERECTE.left) && (MC.y > THERECTE.top) && (MC.y < THERECTE.bottom))
            {
                SendMessage(hwndDlg, WM_COMMAND, (BN_CLICKED << 16) | IDC_XEDIFFUSE_COLOR, 0);
                return FALSE;
            }

            GetWindowRect(GetDlgItem(hwndDlg, IDC_XESPECULAR_COLOR), &THERECTE);
            GetCursorPos(&MC);
            if((MC.x < THERECTE.right) && (MC.x > THERECTE.left) && (MC.y > THERECTE.top) && (MC.y < THERECTE.bottom))
            {
                SendMessage(hwndDlg, WM_COMMAND, (BN_CLICKED << 16) | IDC_XESPECULAR_COLOR, 0);
                return FALSE;
            }

            GetWindowRect(GetDlgItem(hwndDlg, IDC_XEEMAP_COLOR), &THERECTE);
            GetCursorPos(&MC);
            if((MC.x < THERECTE.right) && (MC.x > THERECTE.left) && (MC.y > THERECTE.top) && (MC.y < THERECTE.bottom))
            {
                SendMessage(hwndDlg, WM_COMMAND, (BN_CLICKED << 16) | IDC_XEEMAP_COLOR, 0);
                return FALSE;
            }

            GetWindowRect(GetDlgItem(hwndDlg, IDC_XEMOSSMAP_COLOR), &THERECTE);
            GetCursorPos(&MC);
            if((MC.x < THERECTE.right) && (MC.x > THERECTE.left) && (MC.y > THERECTE.top) && (MC.y < THERECTE.bottom))
            {
                SendMessage(hwndDlg, WM_COMMAND, (BN_CLICKED << 16) | IDC_XEMOSSMAP_COLOR, 0);
                return FALSE;
            }
#endif

		}

		FOCUSWINDOW = hwndDlg;
		XOFFSET = CHACCES(XOFFSET);
		YOFFSET = CHACCES(YOFFSET);
		GetCursorPos(&MousePoint);
		break;
	case WM_MOUSEMOVE:
		if(!(wParam & MK_LBUTTON))
		{
			EDI_Tooltip_DisplayMessage("Double clic for close\nDrag for scroll",1000);
		}

		if(FOCUSWINDOW == hwndDlg)
		{
			if(wParam & MK_LBUTTON)
			{
				/*~~~~~~~~~~~~~~~~*/
				POINT	MousePoint2;
				/*~~~~~~~~~~~~~~~~*/

				GetCursorPos(&MousePoint2);
				SetCursor(HandCurs);
				SetCapture(hwndDlg);
				IsMoving++;
				if((IsMoving & 1) == 0)
				{
					if((MousePoint2.x != MousePoint.x) && (MousePoint2.y != MousePoint.y))
					{
						CHACCES(XOFFSET) = XOFFSET + MousePoint.x - MousePoint2.x;
						CHACCES(YOFFSET) = YOFFSET + MousePoint.y - MousePoint2.y;
						MUTEX_AdaptToRect(GetParent(GetParent(hwndDlg)));
					}
				}

				return TRUE;
			}
		}
		break;
	case WM_LBUTTONDBLCLK:
		CheckDlgButton(hwndDlg, IDC_CHECKOPENTEX, BST_UNCHECKED);
		SUBWINSTYLE(hwndDlg, GWL_USERDATA, (MUTEXT_UD_WinIsOpen | MUTEXT_UD_WinIsOpenFull));
		CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).IsOpen = 0;
		MUTEX_AdaptToRect(GetParent(GetParent(hwndDlg)));
		break;
	case WM_COMMAND:

		/* If (HIWORD(wParam) == CBN_EDITCHANGE) */
		if(HIWORD(wParam) == EN_CHANGE)
		{
			/*~~~~~~~~~*/
			BOOL	Used;
			/*~~~~~~~~~*/

			Used = TRUE;
			if(!bLocalLock)
			{
				switch(LOWORD(wParam))
				{
				case IDC_EDITU: /* Scale U */
					if(VAlidItemFloat(hwndDlg, IDC_EDITU))
					{
						CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).UScale = GetDlgItemFloat
							(
								hwndDlg,
								IDC_EDITU
							);
						if(CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).IsSclLinked)
						{
							SetDlgItemFloat
							(
								hwndDlg,
								IDC_EDIT2,
								CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).UScale
							);
							CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).VScale = CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).UScale;
						}
					}

					SEND_INFORMATION();
					break;
				case IDC_EDIT2: /* Scale V */
					if(VAlidItemFloat(hwndDlg, IDC_EDIT2))
					{
						CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).VScale = GetDlgItemFloat
							(
								hwndDlg,
								IDC_EDIT2
							);
						if(CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).IsSclLinked)
						{
							SetDlgItemFloat
							(
								hwndDlg,
								IDC_EDITU,
								CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).VScale
							);
							CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).UScale = CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).VScale;
						}
					}

					SEND_INFORMATION();
					break;
				case IDC_EDITROTATION:	/* Rotation */
					if(VAlidItemFloat(hwndDlg, IDC_EDITROTATION))
					{
						CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).Rotation = GetDlgItemFloat
								(
									hwndDlg,
									IDC_EDITROTATION
								) /
							360.0f;
					}

					SEND_INFORMATION();
					break;
				case IDC_EDIT4: /* Pos U */
					if(VAlidItemFloat(hwndDlg, IDC_EDIT4))
					{
						CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).UPos = 0.01f * GetDlgItemFloat(hwndDlg, IDC_EDIT4);
					}

					SEND_INFORMATION();
					break;
				case IDC_EDIT5: /* Pos V */
					if(VAlidItemFloat(hwndDlg, IDC_EDIT5))
					{
						CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).VPos = 0.01f * GetDlgItemFloat(hwndDlg, IDC_EDIT5);
					}

					SEND_INFORMATION();
					break;
#ifdef JADEFUSION
                case IDC_XESPECULAR_EXP:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.f_SpecularExp = GetDlgItemFloat(hwndDlg, IDC_XESPECULAR_EXP);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XEDMAP_LODBIAS:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.f_BaseMipMapLODBias = GetDlgItemFloat(hwndDlg, IDC_XEDMAP_LODBIAS);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XENMAP_LODBIAS:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.f_NormalMipMapLODBias = GetDlgItemFloat(hwndDlg, IDC_XENMAP_LODBIAS);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XESBIAS:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.f_SpecularBias = GetDlgItemFloat(hwndDlg, IDC_XESBIAS);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XEMOSS_SPECULAR:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.f_MossSpecularFactor = GetDlgItemFloat(hwndDlg, IDC_XEMOSS_SPECULAR);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XEALPHA_START:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.f_AlphaStart = GetDlgItemFloat(hwndDlg, IDC_XEALPHA_START);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XEALPHA_END:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.f_AlphaEnd = GetDlgItemFloat(hwndDlg, IDC_XEALPHA_END);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XENMAP_SCALEU:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.st_NMapTransform.f_ScaleU = GetDlgItemFloat(hwndDlg, IDC_XENMAP_SCALEU);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XENMAP_SCALEV:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.st_NMapTransform.f_ScaleV = GetDlgItemFloat(hwndDlg, IDC_XENMAP_SCALEV);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XENMAP_SPEEDU:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.st_NMapTransform.f_SpeedU = GetDlgItemFloat(hwndDlg, IDC_XENMAP_SPEEDU);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XENMAP_SPEEDV:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.st_NMapTransform.f_SpeedV = GetDlgItemFloat(hwndDlg, IDC_XENMAP_SPEEDV);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XENMAP_ANGLE:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.st_NMapTransform.f_Angle = GetDlgItemFloat(hwndDlg, IDC_XENMAP_ANGLE);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XENMAP_ROLL:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.st_NMapTransform.f_RollSpeed = GetDlgItemFloat(hwndDlg, IDC_XENMAP_ROLL);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XESMAP_SCALEU:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.st_SMapTransform.f_ScaleU = GetDlgItemFloat(hwndDlg, IDC_XESMAP_SCALEU);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XESMAP_SCALEV:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.st_SMapTransform.f_ScaleV = GetDlgItemFloat(hwndDlg, IDC_XESMAP_SCALEV);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XESMAP_SPEEDU:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.st_SMapTransform.f_SpeedU = GetDlgItemFloat(hwndDlg, IDC_XESMAP_SPEEDU);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XESMAP_SPEEDV:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.st_SMapTransform.f_SpeedV = GetDlgItemFloat(hwndDlg, IDC_XESMAP_SPEEDV);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XESMAP_ANGLE:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.st_SMapTransform.f_Angle = GetDlgItemFloat(hwndDlg, IDC_XESMAP_ANGLE);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XESMAP_ROLL:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.st_SMapTransform.f_RollSpeed = GetDlgItemFloat(hwndDlg, IDC_XESMAP_ROLL);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XEDNMAP_SCALEU:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.st_DNMapTransform.f_ScaleU = GetDlgItemFloat(hwndDlg, IDC_XEDNMAP_SCALEU);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XEDNMAP_SCALEV:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.st_DNMapTransform.f_ScaleV = GetDlgItemFloat(hwndDlg, IDC_XEDNMAP_SCALEV);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XEDNMAP_SPEEDU:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.st_DNMapTransform.f_SpeedU = GetDlgItemFloat(hwndDlg, IDC_XEDNMAP_SPEEDU);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XEDNMAP_SPEEDV:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.st_DNMapTransform.f_SpeedV = GetDlgItemFloat(hwndDlg, IDC_XEDNMAP_SPEEDV);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XEDNMAP_LODSTART:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.ul_DNMapLODStart = (ULONG)GetDlgItemLong(hwndDlg, IDC_XEDNMAP_LODSTART);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XEDNMAP_LODFULL:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.ul_DNMapLODFull = (ULONG)GetDlgItemLong(hwndDlg, IDC_XEDNMAP_LODFULL);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XEDNMAP_STRN:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.f_DNMapStrength = GetDlgItemFloat(hwndDlg, IDC_XEDNMAP_STRN);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XERIM_WIDTH_MIN_EDT: 
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.f_RimLightWidthMin = GetDlgItemFloat(hwndDlg, IDC_XERIM_WIDTH_MIN_EDT);
                        SEND_INFORMATION();
                }

                case IDC_XERIM_WIDTH_MAX_EDT:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.f_RimLightWidthMax = GetDlgItemFloat(hwndDlg, IDC_XERIM_WIDTH_MAX_EDT);
                        SEND_INFORMATION();
                    }

                case IDC_XERIM_INTENSITY_EDT: 
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.f_RimLightIntensity = GetDlgItemFloat(hwndDlg, IDC_XERIM_INTENSITY_EDT);
                        SEND_INFORMATION();
                    }
                case IDC_XERIM_NMAP_RATIO_EDT: 
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.f_RimLightNormalMapRatio = GetDlgItemFloat(hwndDlg, IDC_XERIM_NMAP_RATIO_EDT);
                        SEND_INFORMATION();
                    }

                case IDC_XE_XMP_TESS_AREA:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.f_TessellationArea = GetDlgItemFloat(hwndDlg, IDC_XE_XMP_TESS_AREA);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XE_XMP_DISP_OFFSET:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.f_DisplacementOffset = GetDlgItemFloat(hwndDlg, IDC_XE_XMP_DISP_OFFSET);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XE_XMP_DISP_HEIGHT:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.f_DisplacementHeight = GetDlgItemFloat(hwndDlg, IDC_XE_XMP_DISP_HEIGHT);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XE_XMP_SMOOTH:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.f_SmoothThreshold = GetDlgItemFloat(hwndDlg, IDC_XE_XMP_SMOOTH);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XE_XMP_CMF_LEN:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.f_ChamferLength = GetDlgItemFloat(hwndDlg, IDC_XE_XMP_CMF_LEN);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XE_XMP_CMF_THRESH:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.f_ChamferThreshold = GetDlgItemFloat(hwndDlg, IDC_XE_XMP_CMF_THRESH);
                        SEND_INFORMATION();
                    }
                    break;

                case IDC_XE_XMP_CMF_WELD:
                    if (!MUTEX_BigLocker2)
                    {
                        CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.f_ChamferWeldThreshold = GetDlgItemFloat(hwndDlg, IDC_XE_XMP_CMF_WELD);
                        SEND_INFORMATION();
                    }
                    break;
#endif
				}
			}
		}

		if(HIWORD(wParam) == CBN_SELCHANGE)
		{
			/*~~~~~~~~~~*/
			ULONG	Local;
			/*~~~~~~~~~~*/

			switch(LOWORD(wParam))
			{
			case IDC_PosType:
				Local = SendDlgItemMessage(hwndDlg, IDC_PosType, CB_GETCURSEL, 0, 0);
				if(Local & 1)
					CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).bUIsASpeed = 1;
				else
					CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).bUIsASpeed = 0;
				if(Local & 2)
					CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).bVIsASpeed = 1;
				else
					CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).bVIsASpeed = 0;
				TextureLineToWindow_MORE(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]));
				SEND_INFORMATION();
				break;
#ifdef JADEFUSION
            case IDC_XESMAP_CHANNEL:
                if (!MUTEX_BigLocker2)
                {
                    CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.ul_SpecularMapChannel = SendDlgItemMessage(hwndDlg, IDC_XESMAP_CHANNEL, CB_GETCURSEL, 0, 0);
                    TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo);
                    SEND_INFORMATION();
                }
                break;

            case IDC_XEAMBIENT:
                if (!MUTEX_BigLocker2)
                {
                    CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.ul_AmbientSel = SendDlgItemMessage(hwndDlg, IDC_XEAMBIENT, CB_GETCURSEL, 0, 0);
                    TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo);
                    SEND_INFORMATION();
                }
                break;

            case IDC_XEDIFFUSESOURCE:
                if (!MUTEX_BigLocker2)
                {
                    CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.ul_DiffuseSourceSel = SendDlgItemMessage(hwndDlg, IDC_XEDIFFUSESOURCE, CB_GETCURSEL, 0, 0);
                    TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo);
                    SEND_INFORMATION();
                }
                break;

            case IDC_XESPECULARSOURCE:
                if (!MUTEX_BigLocker2)
                {
                    CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.ul_SpecularSourceSel = SendDlgItemMessage(hwndDlg, IDC_XESPECULARSOURCE, CB_GETCURSEL, 0, 0);
                    TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo);
                    SEND_INFORMATION();
                }
                break;
#endif
			case IDC_COMBOMATRIXFROM:
			case IDC_BlendingType:
			case IDC_TransparencyType:
			case IDC_SourceType:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).TextureBlending = SendDlgItemMessage
					(
						hwndDlg,
						IDC_BlendingType,
						CB_GETCURSEL,
						0,
						0
					);
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).GizmoFrom = SendDlgItemMessage
					(
						hwndDlg,
						IDC_COMBOMATRIXFROM,
						CB_GETCURSEL,
						0,
						0
					);
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).TextureTransparency = SendDlgItemMessage(hwndDlg, IDC_TransparencyType, CB_GETCURSEL, 0, 0);
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).UVSource = SendDlgItemMessage
					(
						hwndDlg,
						IDC_SourceType,
						CB_GETCURSEL,
						0,
						0
					);
				SEND_INFORMATION();
				break;
			}
			{
				/*~~~~~~~~~~~~~~~~~~*/
				char	LineName[256];
				/*~~~~~~~~~~~~~~~~~~*/

				sprintf
				(
					LineName,
					" %s %s %s (UV Source %s) *",
					CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).TextureName,
					MUTEX_TransparencyTypes[CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).TextureTransparency],
					MUTEX_BlendingTypes[CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).TextureBlending],
					MUTEX_UVSources[CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).UVSource]
				);
				SetDlgItemText(hwndDlg, IDC_CHECKOPENTEX, LineName);
				ValidateAlphaCombo(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]));
			}
		}

		if(HIWORD(wParam) == BN_CLICKED)
		{
			switch(LOWORD(wParam))
			{
			case IDC_MULAYER:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).ShiftFace ^= 1;
				SEND_INFORMATION();
				break;
			case IDC_ChangeFlags:
				break;
			case IDC_ChooseUV:
				break;
			case IDC_CHECKOPENTEX:
				SetFocus(hwndDlg);
				if(GetAsyncKeyState(VK_RBUTTON))
				{
					if(CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).IsOpen != 0)
						CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).IsOpen--;
				}
				else
				{
					CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).IsOpen++;
					CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).IsOpen %= 3;
				}

				SUBWINSTYLE(hwndDlg, GWL_USERDATA, (MUTEXT_UD_WinIsOpen | MUTEXT_UD_WinIsOpenFull));
				switch(CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).IsOpen)
				{
				case 1:
					ADDWINSTYLE(hwndDlg, GWL_USERDATA, (MUTEXT_UD_WinIsOpen));
					CheckDlgButton(hwndDlg, IDC_CHECKOPENTEX, BST_CHECKED);
					break;
				case 2:
					ADDWINSTYLE(hwndDlg, GWL_USERDATA, (MUTEXT_UD_WinIsOpen | MUTEXT_UD_WinIsOpenFull));
					CheckDlgButton(hwndDlg, IDC_CHECKOPENTEX, BST_INDETERMINATE);
					break;
				case 0:
					CheckDlgButton(hwndDlg, IDC_CHECKOPENTEX, BST_UNCHECKED);
					break;
				}

				MUTEX_AdaptToRect(GetParent(GetParent(hwndDlg)));
				break;
			case IDC_CHECKDesactivate:
				SetFocus(hwndDlg);
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).bInactive ^= 1;
				SEND_INFORMATION();
				break;
			case IDC_ChooseTexture:
				SetFocus(hwndDlg);
				if((!BIG_REFRESH_LOCK) && (CHACCES(RefreshCLBK) != NULL))
				{
					CHACCES
					(
						RefreshCLBK
						(
							CHACCES(UserParam),
							(int) (&CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]))
						)
					);
				}

				TextureLineToWindow_MORE(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]));
				CheckDlgButton(hwndDlg, IDC_ChooseTexture, BST_UNCHECKED);
				break;
			case IDC_CHECKU:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).TextureFlags ^= 1;
				SEND_INFORMATION();
				break;
			case IDC_CHECK2:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).TextureFlags ^= 2;
				SEND_INFORMATION();
				break;
			case IDC_CHECK3:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).TextureFlags ^= 4;
				SEND_INFORMATION();
				break;
			case IDC_CHECK4:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).TextureFlags ^= 8;
				SEND_INFORMATION();
				break;
			case IDC_CHECK5:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).TextureFlags ^= 16;
				ValidateAlphaCombo(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]));
				SEND_INFORMATION();
				break;
			case IDC_CHECK7:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).TextureFlags ^= 32;
				SEND_INFORMATION();
				break;
			case IDC_CHECK8:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).TextureFlags ^= 64;
				SEND_INFORMATION();
				break;
			case IDC_CHECK9:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).TextureFlags ^= 128;
				SEND_INFORMATION();
				break;
			case IDC_CHECK10:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).TextureFlags ^= 256;
				SEND_INFORMATION();
				break;
			case IDC_CHECK11:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).TextureFlags ^= 512;
				SEND_INFORMATION();
				break;
			case IDC_CHECK12:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).TextureFlags ^= 1024;
				ValidateAlphaCombo(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]));
				SEND_INFORMATION();
				break;
			case IDC_CHECK13:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).CullUV ^= 1;
				SEND_INFORMATION();
				break;
			case IDC_MTX_RADIO1:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).XYZ = 0;
				SEND_INFORMATION();
				break;
			case IDC_MTX_RADIO2:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).XYZ = 1;
				SEND_INFORMATION();
				break;
			case IDC_MTX_RADIO3:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).XYZ = 2;
				SEND_INFORMATION();
				break;
			case IDC_MTX_RADIO4:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).XYZ = 3;
				SEND_INFORMATION();
				break;
			case IDC_CHECKNEGATIV:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).Negativ ^= 1;
				SEND_INFORMATION();
				break;
			case IDC_CHECKSYMETRIC:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).Symetric ^= 1;
				SEND_INFORMATION();
				break;
			case IDC_CHECKUSESCALE:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).UseScale ^= 1;
				SEND_INFORMATION();
				break;
			case IDC_CHECKDEDUCTALPHA:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).DeductAlpha ^= 1;
				SEND_INFORMATION();
				break;
			case IDC_BUTTONSETLA:
				{
					/*~~~~~~~~~~*/
					ULONG	Bidon;
					/*~~~~~~~~~~*/

					Alpha01 = &Bidon;
					Factor01 = 1.0f;
					Factor0X = 1.0f;
					CallSetfloat = FALSE;
					SetFloatWin = hwndDlg;
					ul_ItemSetFloat = 0;
					ul_ItemSetFloat2 = 0xffffffff;
					bLocalLock = TRUE;
					HightLimit = 1;
					LowLimit = 1;
					MUTEX_EditScalar01
					(
						GetParent(hwndDlg),
						&CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).LocalAlpha,
						NULL
					);
					bLocalLock = FALSE;
					REDRAWTHISWINDOW(hwndDlg);
					SEND_INFORMATION();
				}
				break;

			case IDC_BUTTONSETAT:
				{
					/*~~~~~~~~~~*/
					ULONG	Bidon;
					/*~~~~~~~~~~*/

					Alpha01 = &Bidon;
					Factor01 = 1.0f;
					Factor0X = 255.0f;
					CallSetfloat = FALSE;
					SetFloatWin = hwndDlg;
					ul_ItemSetFloat = 0;
					ul_ItemSetFloat2 = 0xffffffff;
					bLocalLock = TRUE;
					HightLimit = 1;
					LowLimit = 1;
					MUTEX_EditScalar01
					(
						GetParent(hwndDlg),
						&CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).AlphaTestTreshold,
						NULL
					);
					bLocalLock = FALSE;
					REDRAWTHISWINDOW(hwndDlg);
					SEND_INFORMATION();
				}
				break;
			case IDC_CHECKLINK:
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).IsSclLinked ^= 1;
				break;

			/* If (BST_CHECKED ==IsDlgButtonChecked(hwndDlg,IDC_CHECKLINK) */
			case IDC_BUTTONSETU:
				{
					/*~~~~~~~~~~*/
					ULONG	Bidon;
					/*~~~~~~~~~~*/

					Alpha01 = &Bidon;
					Factor01 = 100.0f;
					Factor0X = CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).UPos * MAD_RESCALE_SPINN_FACTOR;
					if(Factor0X == 0.0f) Factor0X = 1.0f;
					if(Factor0X < 0.0f) Factor0X = -Factor0X;
					CallSetfloat = TRUE;
					SetFloatWin = hwndDlg;
					ul_ItemSetFloat = IDC_EDIT4;
					ul_ItemSetFloat2 = 0xffffffff;
					bLocalLock = TRUE;
					HightLimit = 0;
					LowLimit = 0;
					MUTEX_EditScalar01
					(
						GetParent(hwndDlg),
						&CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).UPos,
						NULL
					);
					bLocalLock = FALSE;
					SEND_INFORMATION();
				}
				break;
			case IDC_BUTTONSETUV:
				MAPUV = 1;
			case IDC_BUTTONSETV:
				{
					/*~~~~~~~~~~*/
					ULONG	Bidon;
					float	*SC2;
					/*~~~~~~~~~~*/

					Alpha01 = &Bidon;
					Factor01 = 100.0f;
					Factor0X = CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).VPos * MAD_RESCALE_SPINN_FACTOR;
					if(Factor0X == 0.0f) Factor0X = 1.0f;
					if(Factor0X < 0.0f) Factor0X = -Factor0X;
					CallSetfloat = TRUE;
					SetFloatWin = hwndDlg;
					ul_ItemSetFloat = IDC_EDIT5;
					ul_ItemSetFloat2 = 0xffffffff;
					SC2 = NULL;
					if(MAPUV)
					{
						ul_ItemSetFloat2 = IDC_EDIT4;
						SC2 = &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).UPos;
					}

					bLocalLock = TRUE;
					HightLimit = 0;
					LowLimit = 0;
					MUTEX_EditScalar01
					(
						GetParent(hwndDlg),
						&CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).VPos,
						SC2
					);
					bLocalLock = FALSE;
					SEND_INFORMATION();
				}
				break;
			case IDC_STATICROTATION:
				{
					/*~~~~~~~~~~*/
					ULONG	Bidon;
					/*~~~~~~~~~~*/

					Alpha01 = &Bidon;
					Factor01 = 360.f;
					Factor0X = 1.0f;
					CallSetfloat = TRUE;
					SetFloatWin = hwndDlg;
					ul_ItemSetFloat = IDC_EDITROTATION;
					ul_ItemSetFloat2 = 0xffffffff;
					bLocalLock = TRUE;
					HightLimit = 0;
					LowLimit = 0;
					MUTEX_EditScalar01
					(
						GetParent(hwndDlg),
						&CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).Rotation,
						NULL
					);
					bLocalLock = FALSE;
					SEND_INFORMATION();
				}
				break;
			case IDC_BUTTONSETSCU:
				{
					/*~~~~~~~~~~*/
					ULONG	Bidon;
					float	*SC2;
					/*~~~~~~~~~~*/

					Alpha01 = &Bidon;
					Factor01 = 1.0f;
					Factor0X = CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).UScale * MAD_RESCALE_SPINN_FACTOR;
					if(Factor0X == 0.0f) Factor0X = 1.0f;
					CallSetfloat = TRUE;
					SetFloatWin = hwndDlg;
					ul_ItemSetFloat = IDC_EDITU;
					ul_ItemSetFloat2 = 0xffffffff;
					SC2 = NULL;
					if(CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).IsSclLinked)
					{
						ul_ItemSetFloat2 = IDC_EDIT2;
						SC2 = &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).VScale;
					}

					bLocalLock = TRUE;
					HightLimit = 0;
					LowLimit = 0;
					MUTEX_EditScalar01
					(
						GetParent(hwndDlg),
						&CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).UScale,
						SC2
					);
					bLocalLock = FALSE;
					SEND_INFORMATION();
				}
				break;
			case IDC_SCUV11:
				bLocalLock = TRUE;
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).UScale = 1.0f;
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).VScale = 1.0f;
				SetDlgItemFloat
				(
					hwndDlg,
					IDC_EDITU,
					CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).UScale
				);
				SetDlgItemFloat
				(
					hwndDlg,
					IDC_EDIT2,
					CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).VScale
				);
				bLocalLock = FALSE;
				SEND_INFORMATION();
				break;
			case IDC_PSUV00:
				bLocalLock = TRUE;
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).UPos = 0.0f;
				CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).VPos = 0.0f;
				SetDlgItemFloat
				(
					hwndDlg,
					IDC_EDIT4,
					CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).UPos
				);
				SetDlgItemFloat
				(
					hwndDlg,
					IDC_EDIT5,
					CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).VPos
				);
				bLocalLock = FALSE;
				SEND_INFORMATION();
				break;
			case IDC_BUTTONSETSCUV:
				MAPUV = 1;
			case IDC_BUTTONSETSCV:
				{
					/*~~~~~~~~~~*/
					ULONG	Bidon;
					float	*SC2;
					/*~~~~~~~~~~*/

					Alpha01 = &Bidon;
					Factor01 = 1.0f;
					Factor0X = CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).VScale * MAD_RESCALE_SPINN_FACTOR;
					if(Factor0X == 0.0f) Factor0X = 1.0f;
					CallSetfloat = TRUE;
					SetFloatWin = hwndDlg;
					ul_ItemSetFloat = IDC_EDIT2;
					ul_ItemSetFloat2 = 0xffffffff;
					SC2 = NULL;
					if((CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).IsSclLinked) || MAPUV)
					{
						ul_ItemSetFloat2 = IDC_EDITU;
						SC2 = &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).UScale;
					}

					bLocalLock = TRUE;
					HightLimit = 0;
					LowLimit = 0;
					MUTEX_EditScalar01
					(
						GetParent(hwndDlg),
						&CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).VScale,
						SC2
					);
					bLocalLock = FALSE;
					SEND_INFORMATION();
				}
				break;
#ifdef JADEFUSION
                // SC: Xenon properties
                // --------------------
            case IDC_XECHECKEXTENDED:
                SetFocus(hwndDlg);
                CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).b_XeUseExtendedProperties ^= 1;
                SEND_INFORMATION();
				if (CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).b_XeUseExtendedProperties)
				{
					TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
					//set variable to autoset normal map when Extended layer is checked
					b_AutoSetNormalMap = TRUE;
				}
                
				MUTEX_AdaptToRect(GetParent(GetParent(hwndDlg)));

				if(b_AutoSetNormalMap)
				{
					EMAT_BrowseForTextureCLBK(CHACCES(UserParam), &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.l_NMapId), 
											  CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.sz_NMapName),
										      CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].TextureName), EMAT_BT_BrowseTexture,
											  CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].TEXTURE_ID));
					TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
					CheckDlgButton(hwndDlg, IDC_XENMAP, BST_UNCHECKED);
					b_AutoSetNormalMap = FALSE;
				}

				break;

                // Normal map disable
            case IDC_XENMAP_DISABLE:
                SetFocus(hwndDlg);
                CHACCES(MutexMat)->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.b_NMapDisabled ^= 1;
                SEND_INFORMATION();
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                break;

                // Browse for a normal map
            case IDC_XENMAP:
                SetFocus(hwndDlg);
                EMAT_BrowseForTextureCLBK(CHACCES(UserParam), &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.l_NMapId), 
                                                               CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.sz_NMapName),
															   CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].TextureName), EMAT_BT_BrowseTexture,
															   CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].TEXTURE_ID));
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                CheckDlgButton(hwndDlg, IDC_XENMAP, BST_UNCHECKED);
                break;

                // Clear the normal map
            case IDC_XECLEARNMAP:
                SetFocus(hwndDlg);
                EMAT_BrowseForTextureCLBK(CHACCES(UserParam), &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.l_NMapId), 
                                                               CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.sz_NMapName),
															   CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].TextureName), EMAT_BT_Clear);
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

                // Browse for a specular map
            case IDC_XESPECULARMAP:
                SetFocus(hwndDlg);
                EMAT_BrowseForTextureCLBK(CHACCES(UserParam), &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.l_SpecularMapId), 
                                                               CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.sz_SpecularMapName),
															   CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].TextureName), EMAT_BT_BrowseTexture,
															   CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].TEXTURE_ID));
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                CheckDlgButton(hwndDlg, IDC_XESPECULARMAP, BST_UNCHECKED);
                break;

                // Specular map disable
            case IDC_XESMAP_DISABLE:
                SetFocus(hwndDlg);
                CHACCES(MutexMat)->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.b_SMapDisabled ^= 1;
                SEND_INFORMATION();
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                break;

                // Clear the specular map
            case IDC_XECLEARSPECULARMAP:
                SetFocus(hwndDlg);
                EMAT_BrowseForTextureCLBK(CHACCES(UserParam), &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.l_SpecularMapId), 
                                                               CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.sz_SpecularMapName),
															   CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].TextureName), EMAT_BT_Clear);
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

                // Browse for a detail normal map
            case IDC_XEDNMAP:
                SetFocus(hwndDlg);
                EMAT_BrowseForTextureCLBK(CHACCES(UserParam), &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.l_DNMapId), 
                                                               CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.sz_DNMapName),
															   CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].TextureName), EMAT_BT_BrowseTexture,
															   CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].TEXTURE_ID));
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                CheckDlgButton(hwndDlg, IDC_XEDNMAP, BST_UNCHECKED);
                break;

                // Detail normal map disable
            case IDC_XEDNMAP_DISABLE:
                SetFocus(hwndDlg);
                CHACCES(MutexMat)->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.b_DNMapDisabled ^= 1;
                SEND_INFORMATION();
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                break;
			
				//OFFSET
            case IDC_OFFSETMAP:
                SetFocus(hwndDlg);
                CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_OffsetMap ^= 1;
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;


                // Clear the detail normal map
            case IDC_XECLEARDNMAP:
                SetFocus(hwndDlg);
                EMAT_BrowseForTextureCLBK(CHACCES(UserParam), &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.l_DNMapId), 
                                                               CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.sz_DNMapName),
															   CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].TextureName), EMAT_BT_Clear);
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

                // Environment map
            case IDC_XEEMAP_MAP:
                SetFocus(hwndDlg);
                EMAT_BrowseForTextureCLBK(CHACCES(UserParam), &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.l_EnvMapId), 
                                                               CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.sz_EnvMapName),
															   CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].TextureName), EMAT_BT_BrowseCubeMap,
															   CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].TEXTURE_ID));
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                CheckDlgButton(hwndDlg, IDC_XEEMAP_MAP, BST_UNCHECKED);
                break;

                // Environment map disable
            case IDC_XEEMAP_DISABLE:
                SetFocus(hwndDlg);
                CHACCES(MutexMat)->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.b_EMapDisabled ^= 1;
                SEND_INFORMATION();
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                break;

                // Environment map - Clear
            case IDC_XEEMAP_MAP_CLEAR:
                SetFocus(hwndDlg);
                EMAT_BrowseForTextureCLBK(CHACCES(UserParam), &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.l_EnvMapId), 
                                                               CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.sz_EnvMapName),
															   CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].TextureName), EMAT_BT_ClearCubeMap);
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

                // Moss map
            case IDC_XEMOSSMAP_MAP:
                SetFocus(hwndDlg);
                EMAT_BrowseForTextureCLBK(CHACCES(UserParam), &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.l_MossMapId), 
                    CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.sz_MossMapName),
                    CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].TextureName), EMAT_BT_BrowseTexture);
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                CheckDlgButton(hwndDlg, IDC_XEMOSSMAP_MAP, BST_UNCHECKED);
                break;

                // Moss map disable
            case IDC_XEMOSSMAP_DISABLE:
                SetFocus(hwndDlg);
                CHACCES(MutexMat)->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.b_MossMapDisabled ^= 1;
                SEND_INFORMATION();
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                break;

                // Environment map - Clear
            case IDC_XEMOSSMAP_MAP_CLEAR:
                SetFocus(hwndDlg);
                EMAT_BrowseForTextureCLBK(CHACCES(UserParam), &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.l_MossMapId), 
                    CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.sz_MossMapName),
                    CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].TextureName), EMAT_BT_Clear);
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

                // Enable|Disable two sided
            case IDC_XE_TWOSIDED:
                SetFocus(hwndDlg);
                CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_TwoSided ^= 1;
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

                // Enable|Disable two sided
            case IDC_XE_REFLECT_ON_WATER:
                SetFocus(hwndDlg);
                CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_ReflectOnWater ^= 1;
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

                // Enable|Disable normal map texture coordinates transform
            case IDC_XENMAP_TRANSFORM:
                SetFocus(hwndDlg);
                CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_NMapTransform ^= 1;
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

                // Enable|Disable normal map texture coordinates hierarchical transform
            case IDC_XENMAP_ABSOLUTE:
                SetFocus(hwndDlg);
                CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_NMapAbsolute ^= 1;
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

                // Enable|Disable normal map texture coordinates scroll U
            case IDC_XENMAP_SCROLLU:
                SetFocus(hwndDlg);
                CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_NMapScrollU ^= 1;
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

                // Enable|Disable normal map texture coordinates scroll V
            case IDC_XENMAP_SCROLLV:
                SetFocus(hwndDlg);
                CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_NMapScrollV ^= 1;
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

                // Enable|Disable specular map texture coordinates transform
            case IDC_XESMAP_TRANSFORM:
                SetFocus(hwndDlg);
                CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_SMapTransform ^= 1;
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

                // Enable|Disable specular map texture coordinates hierarchical transform
            case IDC_XESMAP_ABSOLUTE:
                SetFocus(hwndDlg);
                CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_SMapAbsolute ^= 1;
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

                // Enable|Disable specular map texture coordinates scroll U
            case IDC_XESMAP_SCROLLU:
                SetFocus(hwndDlg);
                CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_SMapScrollU ^= 1;
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

                // Enable|Disable specular map texture coordinates scroll V
            case IDC_XESMAP_SCROLLV:
                SetFocus(hwndDlg);
                CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_SMapScrollV ^= 1;
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

                // Enable|Disable detail normal map texture coordinates transform
            case IDC_XEDNMAP_TRANSFORM:
                SetFocus(hwndDlg);
                CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_DNMapTransform ^= 1;
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

                // Enable|Disable detail normal map texture coordinates scroll U
            case IDC_XEDNMAP_SCROLLU:
                SetFocus(hwndDlg);
                CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_DNMapScrollU ^= 1;
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

                // Enable|Disable detail normal map texture coordinates scroll V
            case IDC_XEDNMAP_SCROLLV:
                SetFocus(hwndDlg);
                CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_DNMapScrollV ^= 1;
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

            case IDC_XEDIFFUSE_COLOR:
                {
                    CHAR szText[512];
                    sprintf(szText, "%s Diffuse Color", CHACCES(MutexMat->MaterialName));
                    CDLG_USR_GetColor(&CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.ul_DiffuseColor), szText, GetParent(hwndDlg));
                    InvalidateDlgItem(hwndDlg, IDC_XEDIFFUSE_COLOR);
                    SEND_INFORMATION();
                }
                break;

            case IDC_XESPECULAR_COLOR:
                {
                    CHAR szText[512];
                    sprintf(szText, "%s Specular Color", CHACCES(MutexMat->MaterialName));
                    CDLG_USR_GetColor(&CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.ul_SpecularColor), szText, GetParent(hwndDlg));
                    InvalidateDlgItem(hwndDlg, IDC_XESPECULAR_COLOR);
                    SEND_INFORMATION();
                }
                break;

            case IDC_XEEMAP_COLOR:
                {
                    CHAR szText[512];
                    sprintf(szText, "%s Environment Map Color", CHACCES(MutexMat->MaterialName));
                    CDLG_USR_GetColor(&CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.ul_EnvMapColor), szText, GetParent(hwndDlg));
                    InvalidateDlgItem(hwndDlg, IDC_XEEMAP_COLOR);
                    SEND_INFORMATION();
                }
                break;

            case IDC_XEMOSSMAP_COLOR:
                {
                    CHAR szText[512];
                    sprintf(szText, "%s Moss Map Color", CHACCES(MutexMat->MaterialName));
                    CDLG_USR_GetColor(&CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo.ul_MossMapColor), szText, GetParent(hwndDlg));
                    InvalidateDlgItem(hwndDlg, IDC_XEMOSSMAP_COLOR);
                    SEND_INFORMATION();
                }
                break;

            case IDC_XE_MOSSMAP_INVERT:
                SetFocus(hwndDlg);
                CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_InvertMoss ^= 1;
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

            case IDC_XERIMLIGHT_CHK:
                {
                    SetFocus(hwndDlg);
                    CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_RimLightEnable ^= 1;
                    TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                    SEND_INFORMATION();
                }
                break;

            case IDC_XERIM_SMAP_ATTENUATION_CHK:
                {
                    SetFocus(hwndDlg);
                    CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_RimLightSMapAttenuationEnabled ^= 1;
                    TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                    SEND_INFORMATION();
                }
                break;

            case IDC_XE_XMP_CHAMFER:
                SetFocus(hwndDlg);
                CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_AllowChamfer ^= 1;
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

            case IDC_XE_XMP_TESSELLATE:
                SetFocus(hwndDlg);
                CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_AllowTessellation ^= 1;
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

            case IDC_XE_XMP_DISPLACE:
                SetFocus(hwndDlg);
                CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_AllowDisplacement ^= 1;
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

            case IDC_GLOW_ENABLE_CHK:
                SetFocus(hwndDlg);
                CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)]).st_XeInfo.b_GlowEnable ^= 1;
                TextureLineToWindow_Xenon(hwndDlg, &CHACCES(MutexMat->AllLine[GetWindowLong(hwndDlg, GWL_USERDATA)].st_XeInfo));
                SEND_INFORMATION();
                break;

                // --------------------
#endif
			default: break;
			}
		}
		break;
	}

	return FALSE;
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CALLBACK MAD_MUTEX_OneLineTITLECLBK(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CHACCES_DETECT(GetParent(hwndDlg));
	switch(uMsg)
	{
	case WM_INITDIALOG:
	case WM_ERASEBKGND:
		return TRUE;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case WM_VSCROLL:
		switch(LOWORD(wParam))
		{
		case SB_LINEUP:
			CHACCES(YOFFSET) -= SCROLL_SPEED * 50;
			break;
		case SB_LINEDOWN:
			CHACCES(YOFFSET) += SCROLL_SPEED * 50;
			break;
		case SB_PAGEUP:
			CHACCES(YOFFSET) -= SCROLL_SPEED * 100L;
			break;
		case SB_PAGEDOWN:
			CHACCES(YOFFSET) += SCROLL_SPEED * 100L;
			break;
		case SB_THUMBPOSITION:
			CHACCES(YOFFSET) = (short int) HIWORD(wParam);
			break;
		case SB_THUMBTRACK:
			CHACCES(YOFFSET) = (short int) HIWORD(wParam);
			break;
		case SB_TOP:
			return TRUE;
			break;
		case SB_BOTTOM:
			return TRUE;
			break;
		case SB_ENDSCROLL:
			return TRUE;
			break;
		}

		/* SetScrollPos(hwndDlg , SB_VERT , CHACCES(YOFFSET) , TRUE); */
		MUTEX_AdaptToRect(GetParent(hwndDlg));
		return FALSE;
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case WM_HSCROLL:
		switch(LOWORD(wParam))
		{
		case SB_LINEUP:
			CHACCES(XOFFSET) -= SCROLL_SPEED;
			break;
		case SB_LINEDOWN:
			CHACCES(XOFFSET) += SCROLL_SPEED;
			break;
		case SB_PAGEUP:
			CHACCES(XOFFSET) -= SCROLL_SPEED * 10L;
			break;
		case SB_PAGEDOWN:
			CHACCES(XOFFSET) += SCROLL_SPEED * 10L;
			break;
		case SB_THUMBPOSITION:
			CHACCES(XOFFSET) = (short int) HIWORD(wParam);
			break;
		case SB_THUMBTRACK:
			CHACCES(XOFFSET) = (short int) HIWORD(wParam);
			break;
		case SB_TOP:
			return TRUE;
			break;
		case SB_BOTTOM:
			return TRUE;
			break;
		case SB_ENDSCROLL:
			return TRUE;
			break;
		}

		/* SetScrollPos(hwndDlg , SB_HORZ , CHACCES(XOFFSET) , TRUE); */
		MUTEX_AdaptToRect(GetParent(hwndDlg));
		return FALSE;
		break;
	}

	return FALSE;
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CALLBACK MAD_MUTEX_CLIPBOARD(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	/*~~~~~~~~~*/

	/* Static HBRUSH BackBrush; static COLORREF color; */
	RECT	rect;
	/*~~~~~~~~~*/

	CHACCES_DETECT(GetParent(hwndDlg));
	switch(uMsg)
	{
	/*
	 * Case WM_CTLCOLORDLG: case WM_CTLCOLORSTATIC: SetBkColor((HDC) wParam, color);
	 * SetTextColor((HDC) wParam, 0xf0f0f0); return (int) BackBrush; case
	 * WM_INITDIALOG: if(BackBrush == 0) { color = 0x800000; BackBrush =
	 * CreateSolidBrush(color); } return TRUE;
	 */
	case WM_SIZE:
		rect.left = LOWORD(lParam) - 70;
		rect.top = 0;
		rect.bottom = HIWORD(lParam);
		rect.right = 70;
		MoveWindow(GetDlgItem(hwndDlg, IDC_BUTTONCLEANCLPB), rect.left, rect.top, rect.right, rect.bottom, TRUE);
		break;

	case WM_COMMAND:
		if(HIWORD(wParam) == BN_CLICKED)
		{
			switch(LOWORD(wParam))
			{
			case IDC_BUTTONCLEANCLPB:
				CHACCES(MutexMat->NumberOfSubTextures) = 0;
				MUTEX_AdaptToRect(GetParent(hwndDlg));
				break;
			}
		}
		break;
	}

	return FALSE;
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CALLBACK MAD_MUTEX_MainMaterialCLBK(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	/*~~~~~~~~~~~~~~~~~~*/
	ULONG		Add;
	RECT		LocalRect;
	PAINTSTRUCT ps;
	BOOL		Used;
	int			Counter;
	RECT		LocalR;
	/*~~~~~~~~~~~~~~~~~~*/

	CHACCES_DETECT(GetParent(hwndDlg));
	switch(uMsg)
	{

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case WM_INITDIALOG:
		Add = 0;

		/*
		 * SendMessage(GetDlgItem(hwndDlg, IDC_COMBODIFFALFROM), CB_RESETCONTENT, 0, 0);
		 * while(MUTEX_DiffuseAlphaFrom[Add][0] != 0) { SendMessage ( GetDlgItem(hwndDlg,
		 * IDC_COMBODIFFALFROM), CB_ADDSTRING, 0, (LONG) MUTEX_DiffuseAlphaFrom[Add++] );
		 * }
		 */
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case WM_PAINT:
		if(CHACCES(MutexMat))
		{
			/*~~~~~~~~*/
			POINT	ppt;
			/*~~~~~~~~*/

			BeginPaint(hwndDlg, &ps);
			DRAWCOLOR(IDC_DiffuseC, CHACCES(MutexMat->DColor), 0, "Diffuse", 7);
			DRAWCOLOR(IDC_SpecularC, CHACCES(MutexMat->SColor), 0, "Constant", 8);
			DRAWCOLOR(IDC_SelfillC, CHACCES(MutexMat->AColor), 0, "Ambiant", 7);

			/*
			 * DRAWCOLOR(IDC_Opacity, 0, 0, "Opacity", 7); GetWindowRect(GetDlgItem(hwndDlg,
			 * IDC_Opacity), &LocalRect);
			 */
			ppt.x = LocalRect.left;
			ppt.y = LocalRect.top;
			ScreenToClient(hwndDlg, &ppt);
			LocalRect.left = ppt.x + 2;
			LocalRect.top = ppt.y + 2;
			ppt.x = LocalRect.right;
			ppt.y = LocalRect.bottom;
			ScreenToClient(hwndDlg, &ppt);
			LocalRect.right = ppt.x - 2;
			LocalRect.bottom = ppt.y - 2;
			LocalRect.right = LocalRect.left + (int) ((float) (LocalRect.right - LocalRect.left) * CHACCES(MutexMat->Opacity));
			InvertRect(ps.hdc, &LocalRect);
			EndPaint(hwndDlg, &ps);
		}
		break;


	/*$2--------------------------------------------------------------------------------------------------------------*/

	case WM_LBUTTONDOWN:
		{
			/*~~~~~~~~~~~~~*/
			RECT	THERECTE;
			POINT	MC;
			/*~~~~~~~~~~~~~*/

			GetWindowRect(GetDlgItem(hwndDlg, IDC_DiffuseC), &THERECTE);
			GetCursorPos(&MC);
			if((MC.x < THERECTE.right) && (MC.x > THERECTE.left) && (MC.y > THERECTE.top) && (MC.y < THERECTE.bottom))
			{
				SendMessage(hwndDlg, WM_COMMAND, (BN_CLICKED << 16) | IDC_DiffuseC, 0);
				return FALSE;
			}

			GetWindowRect(GetDlgItem(hwndDlg, IDC_SpecularC), &THERECTE);
			GetCursorPos(&MC);
			if((MC.x < THERECTE.right) && (MC.x > THERECTE.left) && (MC.y > THERECTE.top) && (MC.y < THERECTE.bottom))
			{
				SendMessage(hwndDlg, WM_COMMAND, (BN_CLICKED << 16) | IDC_SpecularC, 0);
				return FALSE;
			}

			GetWindowRect(GetDlgItem(hwndDlg, IDC_SelfillC), &THERECTE);
			GetCursorPos(&MC);
			if((MC.x < THERECTE.right) && (MC.x > THERECTE.left) && (MC.y > THERECTE.top) && (MC.y < THERECTE.bottom))
			{
				SendMessage(hwndDlg, WM_COMMAND, (BN_CLICKED << 16) | IDC_SelfillC, 0);
				return FALSE;
			}
#ifdef JADEFUSION //POPOWARNING a TERMINER
/*			GetWindowRect(GetDlgItem(hwndDlg, IDC_XESPECULAR), &THERECTE);
            GetCursorPos(&MC);
            if((MC.x < THERECTE.right) && (MC.x > THERECTE.left) && (MC.y > THERECTE.top) && (MC.y < THERECTE.bottom))
            {
                SendMessage(hwndDlg, WM_COMMAND, (BN_CLICKED << 16) | IDC_XESPECULAR, 0);
                return FALSE;
            }*/
#endif
			/*
			 * GetWindowRect(GetDlgItem(hwndDlg, IDC_Opacity), &THERECTE); GetCursorPos(&MC);
			 * if ( (MC.x < THERECTE.right) && (MC.x > THERECTE.left) && (MC.y > THERECTE.top)
			 * && (MC.y < THERECTE.bottom) ) { SendMessage(hwndDlg, WM_COMMAND, (BN_CLICKED <<
			 * 16) | IDC_Opacity, 0); return FALSE; }
			 */
		}
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case WM_COMMAND:

		/*
		 * If(HIWORD(wParam) == CBN_SELCHANGE) { switch(LOWORD(wParam)) { case
		 * IDC_COMBODIFFALFROM: Local = SendDlgItemMessage(hwndDlg, IDC_COMBODIFFALFROM,
		 * CB_GETCURSEL, 0, 0); CHACCES(MutexMat)->MaterialFlag = Local;
		 * SEND_INFORMATION(); break; } }
		 */
		if(HIWORD(wParam) == EN_CHANGE)
		{
			Used = TRUE;
			switch(LOWORD(wParam))
			{
			case IDC_LAYER:
				CHACCES(MutexMat->NumberOfSubTextures) = GetDlgItemInt(hwndDlg, IDC_LAYER, &Used, Used);
				if(CHACCES(MutexMat->NumberOfSubTextures) > MAX_LEVEL_MT)
				{
					CHACCES(MutexMat->NumberOfSubTextures) = MAX_LEVEL_MT;
					SetDlgItemInt(hwndDlg, IDC_LAYER, MAX_LEVEL_MT, FALSE);
				}

				for(Counter = 0; Counter < MAX_LEVEL_MT; Counter++)
				{
					if(!CHACCES(hMaterialWindow)[Counter])
					{
						CHACCES(hMaterialWindow)[Counter] = CreateDialogParam
							(
								MyHinstance,
								MAKEINTRESOURCE(IDD_MUTEX_OneLine),
								CHACCES(Title),
								(int(__stdcall *) (struct HWND__ *, unsigned int, unsigned int, LONG)) MAD_MUTEX_OneLineCLBK,
								Counter
							);
						GetWindowRect(CHACCES(hMaterialWindow)[Counter], &LocalR);
						VSIZEOPEN = LocalR.bottom - LocalR.top - 1;
						HSIZEREAL = HSIZE = LocalR.right - LocalR.left;
	
//#ifdef JADEFUSION
                        // Size of the Xenon controls
                        if (s_l_XeVerticalSize == 0)
                        {
                            RECT rc_Top;
                            RECT rc_Bottom;
                            GetWindowRect(GetDlgItem(CHACCES(hMaterialWindow)[Counter], MUTEX_XE_CONTROL_TOP), &rc_Top);
                            GetWindowRect(GetDlgItem(CHACCES(hMaterialWindow)[Counter], MUTEX_XE_CONTROL_BOTTOM), &rc_Bottom);
#ifdef JADEFUSION
							s_l_XeVerticalSize = (rc_Bottom.bottom - rc_Top.top) + 25;
#else
							s_l_XeVerticalSize = (rc_Bottom.bottom - rc_Top.top) + 50;
#endif
						}
                        if (s_l_XeHalfModeVerticalSize == 0)
                        {
                            RECT rc_Top;
                            RECT rc_Bottom;
                            GetWindowRect(GetDlgItem(CHACCES(hMaterialWindow)[Counter], MUTEX_XE_CONTROL_TOP), &rc_Top);
                            GetWindowRect(GetDlgItem(CHACCES(hMaterialWindow)[Counter], MUTEX_XE_CONTROL_HALF_BOTTOM), &rc_Bottom);
                            s_l_XeHalfModeVerticalSize = (rc_Bottom.bottom - rc_Top.top) + 10;
                        }
                        if (s_l_XeHalfModeVerticalDisplacement == 0)
                        {
                            RECT rc_Top;
                            RECT rc_Bottom;
                            GetWindowRect(GetDlgItem(CHACCES(hMaterialWindow)[Counter], MUTEX_XE_CONTROL_HALF_REF_1), &rc_Top);
                            GetWindowRect(GetDlgItem(CHACCES(hMaterialWindow)[Counter], MUTEX_XE_CONTROL_HALF_REF_2), &rc_Bottom);
                            s_l_XeHalfModeVerticalDisplacement = (rc_Bottom.top - rc_Top.top);
                        }
//#endif
					
					}
				}

				for(Counter = 0; Counter < (int) CHACCES(MutexMat->NumberOfSubTextures); Counter++)
					TextureLineToWindow(CHACCES(hMaterialWindow[Counter]), &CHACCES(MutexMat->AllLine[Counter]));

				MUTEX_AdaptToRect(GetParent(hwndDlg));

				SEND_INFORMATION();
				break;
			}
		}

		if(HIWORD(wParam) == BN_CLICKED)
		{
			/*~~~~~~~~~~~~~~~*/
			char	*TEXT[256];
			/*~~~~~~~~~~~~~~~*/

			switch(LOWORD(wParam))
			{
			case IDC_DiffuseC:
				sprintf((char *) TEXT, "%s Diffuse Color", CHACCES(MutexMat->MaterialName));
				CDLG_USR_GetColor(&CHACCES(MutexMat->DColor), (char *) TEXT, hwndDlg);
				break;
			case IDC_SpecularC:
				sprintf((char *) TEXT, "%s Constant Color", CHACCES(MutexMat->MaterialName));
				CDLG_USR_GetColor(&CHACCES(MutexMat->SColor), (char *) TEXT, hwndDlg);
				break;
			case IDC_SelfillC:
				sprintf((char *) TEXT, "%s Ambiant Color", CHACCES(MutexMat->MaterialName));
				CDLG_USR_GetColor(&CHACCES(MutexMat->AColor), (char *) TEXT, hwndDlg);
				break;

				/*
				 * Case IDC_Opacity: { Alpha01 = &Bidon; Factor01 = 1.0f; Factor0X = 1.0f;
				 * HightLimit = 1; LowLimit = 1; MUTEX_EditScalar01(hwndDlg,
				 * &CHACCES(MutexMat->Opacity), NULL); } break;
				 */
			}

			REDRAWTHISWINDOW(hwndDlg);
			SEND_INFORMATION();
			return FALSE;
		}

		if(HIWORD(wParam) == CBN_SELCHANGE)
		{
			switch(LOWORD(wParam))
			{
			case IDC_SOUND_COMBO:
					CHACCES(MutexMat)->Sound = (UCHAR) SendDlgItemMessage
					(
						hwndDlg,
						IDC_SOUND_COMBO,
						CB_GETCURSEL,
						0,
						0
					);
				SEND_INFORMATION();
				break;
			}
		}

	}

	return FALSE;
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MUTEX_SetXYoffset(void *Strange, ULONG ulXOFFSET, ULONG ulYOFFSET)
{
	/*~~~~~~~~~*/
	HWND	Host;
	/*~~~~~~~~~*/

	*(void **) &Host = Strange;
	CHACCES_DETECT(Host);
	CHACCES(ulHWNDOFFSET) = ulXOFFSET;
	MUTEX_AdaptToRect(Host);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MOVEGROUP(HWND Wndw, ULONG *Group, LONG X, LONG Y)
{
	/*~~~~~~~~~~~~~~*/
	RECT	LocalRect;
	RECT	MAINZone;
	/*~~~~~~~~~~~~~~*/

	GetWindowRect(Wndw, &MAINZone);
	while(*Group != 0xffffffff)
	{
		GetWindowRect(GetDlgItem(Wndw, *Group), &LocalRect);
		MoveWindow
		(
			GetDlgItem(Wndw, *Group),
			LocalRect.left - MAINZone.left + X,
			LocalRect.top - MAINZone.top + Y,
			LocalRect.right - LocalRect.left,
			LocalRect.bottom - LocalRect.top,
			TRUE
		);

		Group++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int MUTEX_GetSubByRect(void *Strange, POINT pt)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	HWND			h_Text;
	RECT			rect;
	unsigned int	Counter;
	HWND			Host;
	/*~~~~~~~~~~~~~~~~~~~~*/

	*(void **) &Host = Strange;
	CHACCES_DETECT(Host);

	for(Counter = 0; Counter < CHACCES(MutexMat->NumberOfSubTextures); Counter++)
	{
		h_Text = GetDlgItem(CHACCES(hMaterialWindow)[Counter], IDC_ChooseTexture);
		GetWindowRect(h_Text, &rect);
#ifdef JADEFUSION
		if(PtInRect(&rect, pt)) return MUTEX_PackTextureInfo(Counter, MUTEX_GSBR_XE_NONE);

        // SC: Xenon extended layer information
        if (CHACCES(MutexMat->AllLine[Counter]).b_XeUseExtendedProperties)
        {
            // Normal map
            h_Text = GetDlgItem(CHACCES(hMaterialWindow)[Counter], IDC_XENMAP);
            GetWindowRect(h_Text, &rect);
            if (PtInRect(&rect, pt)) return MUTEX_PackTextureInfo(Counter, MUTEX_GSBR_XE_NORMALMAP);

            // Specular map
            h_Text = GetDlgItem(CHACCES(hMaterialWindow)[Counter], IDC_XESPECULARMAP);
            GetWindowRect(h_Text, &rect);
            if (PtInRect(&rect, pt)) return MUTEX_PackTextureInfo(Counter, MUTEX_GSBR_XE_SPECULARMAP);

            // EnvMap
            h_Text = GetDlgItem(CHACCES(hMaterialWindow)[Counter], IDC_XEEMAP_MAP);
            GetWindowRect(h_Text, &rect);
            if (PtInRect(&rect, pt)) return MUTEX_PackTextureInfo(Counter, MUTEX_GSBR_XE_ENVMAP);

            // MossMap
            h_Text = GetDlgItem(CHACCES(hMaterialWindow)[Counter], IDC_XEMOSSMAP_MAP);
            GetWindowRect(h_Text, &rect);
            if (PtInRect(&rect, pt)) return MUTEX_PackTextureInfo(Counter, MUTEX_GSBR_XE_MOSSMAP);

            // Detail normal map
            h_Text = GetDlgItem(CHACCES(hMaterialWindow)[Counter], IDC_XEDNMAP);
            GetWindowRect(h_Text, &rect);
            if (PtInRect(&rect, pt)) return MUTEX_PackTextureInfo(Counter, MUTEX_GSBR_XE_DETAILNMAP);
        }
#else
		if(PtInRect(&rect, pt)) return Counter;
#endif
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MUTEX_AdaptToRect(void *Strange)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG		Counter;
	DWORD		UserData;
	RECT		AdaptZone;
	RECT		MAINZone;
	ULONG		OneIsOpen;
	SCROLLINFO	si;
	HWND		Host;
	ULONG		bDoubled;
	ULONG		LocalHSizeFull;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	LocalHSizeFull = VSIZEOPEN;

	*(void **) &Host = Strange;

	CHACCES_DETECT(Host);
	if(CHACCES(MutexMat) == NULL)
	{
		ShowWindow(CHACCES(Title), SW_HIDE);
		ShowWindow(CHACCES(MMHWND), SW_HIDE);
		return;
	}
	else
	{
		ShowWindow(CHACCES(Title), SW_SHOW);
		ShowWindow(CHACCES(MMHWND), SW_SHOW);
	}

	GetClientRect(Host, &AdaptZone);
	AdaptZone.left += CHACCES(ulHWNDOFFSET);

#ifdef JADEFUSION
	if(!CHACCES(IsClipBoard) && b_ShowCLPBRDMT)
#else
	if(!CHACCES(IsClipBoard))
#endif
		AdaptZone.bottom = AdaptZone.top + (int) ((float) (AdaptZone.bottom - AdaptZone.top) * MAtClippoardProp);

	GetWindowRect(CHACCES(MMHWND), &MAINZone);
	MAINZone.bottom -= MAINZone.top;
	MAINZone.left -= MAINZone.right;
	MAINZone.top = 0;
	MAINZone.right = 0;
	AdaptZone.top += MAINZone.bottom;
	bDoubled = 0;
	if(HSIZE << 1 < (AdaptZone.right - AdaptZone.left))
	{
		bDoubled = 1;
		LocalHSizeFull = VSIZETIERCEOPEN;
	}

	for(Counter = 0; Counter < CHACCES(MutexMat->NumberOfSubTextures); Counter++)
	{
		if(CHACCES(bDoubled[Counter]) != bDoubled)
		{
			if(bDoubled)
			{
				MOVEGROUP(CHACCES(hMaterialWindow)[Counter], Group2, HSIZE, -VSIZEHALFOPEN + VSIZE + 10);
				MOVEGROUP(CHACCES(hMaterialWindow)[Counter], Group3, 0, VSIZEHALFOPEN - VSIZETIERCEOPEN + 2);
				MOVEGROUP
				(
					CHACCES(hMaterialWindow)[Counter],
					Group4,
					HSIZE,
					-VSIZEHALFOPEN + VSIZE + 10 + VSIZEHALFOPEN - VSIZETIERCEOPEN + 2
				);
#ifdef JADEFUSION
                MOVEGROUP(CHACCES(hMaterialWindow)[Counter], XeGroup, 0, -s_l_XeVerticalDisplacement);
                MOVEGROUP(CHACCES(hMaterialWindow)[Counter], XeGroupRight, HSIZE, -s_l_XeHalfModeVerticalDisplacement);
#endif
			}
			else
			{
				MOVEGROUP(CHACCES(hMaterialWindow)[Counter], Group2, -HSIZE, VSIZEHALFOPEN - VSIZE - 10);
				MOVEGROUP(CHACCES(hMaterialWindow)[Counter], Group3, 0, VSIZETIERCEOPEN - VSIZEHALFOPEN - 2);
				MOVEGROUP
				(
					CHACCES(hMaterialWindow)[Counter],
					Group4,
					-HSIZE,
					-(-VSIZEHALFOPEN + VSIZE + 10 + VSIZEHALFOPEN - VSIZETIERCEOPEN + 2)
				);
#ifdef JADEFUSION
                MOVEGROUP(CHACCES(hMaterialWindow)[Counter], XeGroup, 0, s_l_XeVerticalDisplacement);
                MOVEGROUP(CHACCES(hMaterialWindow)[Counter], XeGroupRight, -HSIZE, s_l_XeHalfModeVerticalDisplacement);
#endif
			}
		}

		CHACCES(bDoubled[Counter]) = bDoubled;
	}

	if(CHACCES(XOFFSET) < 0) CHACCES(XOFFSET) = 0;
	if(CHACCES(YOFFSET) < 0) CHACCES(YOFFSET) = 0;
	CHACCES(TotalHeight) = 0;
	OneIsOpen = 0;
	for(Counter = 0; Counter < CHACCES(MutexMat->NumberOfSubTextures); Counter++)
	{
		UserData = GetWindowLong(CHACCES(hMaterialWindow)[Counter], GWL_USERDATA);
		if((UserData & MUTEXT_UD_WinIsOpen) && !(CHACCES(IsClipBoard)))
		{
			if(UserData & MUTEXT_UD_WinIsOpenFull)
			{
				if (!CHACCES(bDoubled[Counter]) 
#ifdef JADEFUSION
					&& !CHACCES(MutexMat->AllLine[Counter].b_XeUseExtendedProperties)
#endif
					)
           
				{
                    CHACCES(TotalHeight) -= s_l_XeVerticalSize;
                }
#ifdef JADEFUSION
				else if (CHACCES(bDoubled[Counter]) && CHACCES(MutexMat->AllLine[Counter].b_XeUseExtendedProperties))
				{
                    CHACCES(TotalHeight) += s_l_XeHalfModeVerticalSize;
                }
#endif
                if (CHACCES(bDoubled[Counter]))
                {
                    CHACCES(TotalHeight) += s_l_XeExtendedVerticalSize;
                }
//#endif
				CHACCES(TotalHeight) += LocalHSizeFull;
			}
			else
				CHACCES(TotalHeight) += VSIZEHALFOPEN;
			OneIsOpen = 1;
		}
		else if(CHACCES(IsClipBoard))
			CHACCES(TotalHeight) += VSIZECLPBRD;
		else
			CHACCES(TotalHeight) += VSIZE;
	}

	MoveWindow
	(
		CHACCES(MMHWND),
		AdaptZone.left,
		0,
		AdaptZone.right - AdaptZone.left,
		MAINZone.bottom - MAINZone.top,
		TRUE
	);
	MoveWindow
	(
		CHACCES(Title),
		AdaptZone.left,
		AdaptZone.top,
		AdaptZone.right - AdaptZone.left,
		AdaptZone.bottom - AdaptZone.top,
		TRUE
	);
	if(CHACCES(TotalHeight) > AdaptZone.bottom - AdaptZone.top)
	{
		if(AdaptZone.bottom - AdaptZone.top + CHACCES(YOFFSET) > CHACCES(TotalHeight) + CHACCES(BottomScale))
		{
			CHACCES(YOFFSET) = CHACCES(TotalHeight) + CHACCES(BottomScale) - AdaptZone.bottom + AdaptZone.top;
			if(CHACCES(YOFFSET) < 0) CHACCES(YOFFSET) = 0;
		}

		si.cbSize = sizeof(si);
		si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		si.nMin = 0;
		si.nMax = CHACCES(TotalHeight) + CHACCES(BottomScale);
		si.nPage = (AdaptZone.bottom - AdaptZone.top);
		si.nPos = CHACCES(YOFFSET);
		SetScrollInfo(CHACCES(Title), SB_VERT, &si, FALSE);
		ShowScrollBar(CHACCES(Title), SB_VERT, FALSE);
		ShowScrollBar(CHACCES(Title), SB_VERT, TRUE);
		CHACCES(RightScale) = SCROLL_LENGHT;
	}
	else
	{
		CHACCES(YOFFSET) = 0;
		SetScrollPos(CHACCES(Title), SB_VERT, 0, TRUE);
		ShowScrollBar(CHACCES(Title), SB_VERT, FALSE);
		CHACCES(RightScale) = 0;
	}

	if((HSIZE > AdaptZone.right - AdaptZone.left) && (OneIsOpen))
	{
		if(AdaptZone.right - AdaptZone.left + CHACCES(XOFFSET) > HSIZE + CHACCES(RightScale))
		{
			CHACCES(XOFFSET) = HSIZE + CHACCES(RightScale) - AdaptZone.right + AdaptZone.left;
			if(CHACCES(XOFFSET) < 0) CHACCES(XOFFSET) = 0;
		}

		si.cbSize = sizeof(si);
		si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		si.nMin = 0;
		si.nMax = HSIZE + CHACCES(RightScale);
		si.nPage = (AdaptZone.right - AdaptZone.left);
		si.nPos = CHACCES(XOFFSET);
		SetScrollInfo(CHACCES(Title), SB_HORZ, &si, FALSE);
		ShowScrollBar(CHACCES(Title), SB_HORZ, FALSE);
		ShowScrollBar(CHACCES(Title), SB_HORZ, TRUE);
		CHACCES(BottomScale) = SCROLL_LENGHT;
	}
	else
	{
		CHACCES(XOFFSET) = 0;
		SetScrollPos(CHACCES(Title), SB_HORZ, 0, TRUE);
		ShowScrollBar(CHACCES(Title), SB_HORZ, FALSE);
		CHACCES(BottomScale) = 0;
	}

	CHACCES(TotalHeight) = -CHACCES(YOFFSET);
	for(Counter = 0; Counter < CHACCES(MutexMat->NumberOfSubTextures); Counter++)
	{
		UserData = GetWindowLong(CHACCES(hMaterialWindow)[Counter], GWL_USERDATA);
		if((UserData & MUTEXT_UD_WinIsOpen) && !CHACCES(IsClipBoard))
		{
			if(UserData & MUTEXT_UD_WinIsOpenFull)
			{
//#ifdef JADEFUSION
                LONG l_ExtSize = 0;

                if (CHACCES(bDoubled[Counter]))
                {
                    l_ExtSize += s_l_XeExtendedVerticalSize;
                }
#ifdef JADEFUSION
                if (CHACCES(bDoubled[Counter]) && CHACCES(MutexMat->AllLine[Counter].b_XeUseExtendedProperties))
				{
                    l_ExtSize += s_l_XeHalfModeVerticalSize;
                }
#endif
				else if (!CHACCES(bDoubled[Counter]) 
#ifdef JADEFUSION
					&& !CHACCES(MutexMat->AllLine[Counter].b_XeUseExtendedProperties)
#endif					
					)
				{
                    l_ExtSize += -s_l_XeVerticalSize;
                }
//#endif
				InvalidateRect(CHACCES(hMaterialWindow)[Counter], NULL, FALSE);
				MoveWindow
				(
					CHACCES(hMaterialWindow)[Counter],
					-CHACCES(XOFFSET),
					CHACCES(TotalHeight),
					CHACCES(XOFFSET) + AdaptZone.right - AdaptZone.left,
//#ifdef JADEFUSION
					LocalHSizeFull + l_ExtSize,
//#else
	//				LocalHSizeFull,
//#endif
					TRUE
				);
				UpdateWindow(CHACCES(hMaterialWindow)[Counter]);
//#ifdef JADEFUSION
				CHACCES(TotalHeight) += LocalHSizeFull + l_ExtSize;
//#else
//				CHACCES(TotalHeight) += LocalHSizeFull;
//#endif
			}
			else
			{
				InvalidateRect(CHACCES(hMaterialWindow)[Counter], NULL, FALSE);
				MoveWindow
				(
					CHACCES(hMaterialWindow)[Counter],
					-CHACCES(XOFFSET),
					CHACCES(TotalHeight),
					CHACCES(XOFFSET) + AdaptZone.right - AdaptZone.left,
					VSIZEHALFOPEN,
					TRUE
				);
				UpdateWindow(CHACCES(hMaterialWindow)[Counter]);
				CHACCES(TotalHeight) += VSIZEHALFOPEN;
			}
		}
		else
		{
			InvalidateRect(CHACCES(hMaterialWindow)[Counter], NULL, FALSE);
			if(CHACCES(IsClipBoard))
			{
				MoveWindow
				(
					CHACCES(hMaterialWindow)[Counter],
					-CHACCES(XOFFSET),
					CHACCES(TotalHeight),
					CHACCES(XOFFSET) + AdaptZone.right - AdaptZone.left,
					VSIZECLPBRD + 1,
					TRUE
				);
			}
			else
			{
				MoveWindow
				(
					CHACCES(hMaterialWindow)[Counter],
					-CHACCES(XOFFSET),
					CHACCES(TotalHeight),
					CHACCES(XOFFSET) + AdaptZone.right - AdaptZone.left,
					VSIZE + 1,
					TRUE
				);
			}

			UpdateWindow(CHACCES(hMaterialWindow)[Counter]);
			if(CHACCES(IsClipBoard))
				CHACCES(TotalHeight) += VSIZECLPBRD + 1;
			else
				CHACCES(TotalHeight) += VSIZE + 1;
		}

		ShowWindow(CHACCES(hMaterialWindow)[Counter], SW_SHOW);
	}

	for(; Counter < MAX_LEVEL_MT; Counter++)
	{
		ShowWindow(CHACCES(hMaterialWindow)[Counter], SW_HIDE);
	}

	MoveWindow
	(
		CHACCES(VoidHWND2),
		-CHACCES(XOFFSET),
		CHACCES(TotalHeight),
		CHACCES(XOFFSET) + AdaptZone.right - AdaptZone.left,
		1000L,
		TRUE
	);
	CHACCES(TotalHeight) += CHACCES(YOFFSET);
	ShowWindow(CHACCES(VoidHWND2), SW_SHOW);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MUTEX_Create(void *Strange, ULONG IsClipBoard)
{
	/*~~~~~~~~~~~~~~~~~~*/
	ULONG		Counter;
	SCROLLINFO	si;
	RECT		AdaptZone;
	RECT		MAINZone;
	HWND		Host;
	/*~~~~~~~~~~~~~~~~~~*/

	*(void **) &Host = Strange;

	CHACCES_DETECT((HWND) 0);
	AllMotherWindw[ThisHinstance] = Host;
	memset(&AllHinInf[ThisHinstance], 0, sizeof(AllHinInf[ThisHinstance]));
	GetClientRect(Host, &AdaptZone);
	CHACCES(IsClipBoard) = IsClipBoard;
	if(IsClipBoard)
	{
		CHACCES(MMHWND) = CreateDialog
			(
				MyHinstance,
				MAKEINTRESOURCE(IDD_CLIPBOARDM),
				Host,
				(int(__stdcall *) (struct HWND__ *, unsigned int, unsigned int, LONG)) MAD_MUTEX_CLIPBOARD
			);
	}
	else
	{
		CHACCES(MMHWND) = CreateDialog
			(
				MyHinstance,
				MAKEINTRESOURCE(IDD_MAINMATERIAL),
				Host,
				(int(__stdcall *) (struct HWND__ *, unsigned int, unsigned int, LONG)) MAD_MUTEX_MainMaterialCLBK
			);
	}

	GetWindowRect(CHACCES(MMHWND), &MAINZone);
	MoveWindow(CHACCES(MMHWND), 0, 0, AdaptZone.right - AdaptZone.left, MAINZone.bottom - MAINZone.top, TRUE);

	CHACCES(Title) = CreateDialog
		(
			MyHinstance,
			MAKEINTRESOURCE(IDD_OneLineTitle),
			Host,
			(int(__stdcall *) (struct HWND__ *, unsigned int, unsigned int, LONG)) MAD_MUTEX_OneLineTITLECLBK
		);

	EnableScrollBar(CHACCES(Title), SB_VERT, 0);
	EnableScrollBar(CHACCES(Title), SB_HORZ, 0);
	si.cbSize = sizeof(si);
	si.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
	si.nMin = 0;
	si.nMax = 100;
	si.nPage = 1;
	si.nPos = 0;
	SetScrollInfo(CHACCES(Title), SB_VERT, &si, FALSE);
	SetScrollInfo(CHACCES(Title), SB_HORZ, &si, FALSE);
	SCROLL_LENGHT = GetSystemMetrics(SM_CXHSCROLL);
	CHACCES(VoidHWND2) = CreateDialog
		(
			MyHinstance,
			MAKEINTRESOURCE(IDD_VOID0),
			CHACCES(Title),
			(int(__stdcall *) (struct HWND__ *, unsigned int, unsigned int, LONG)) MAD_MUTEX_OneLineVOIDCLBK
		);
	HSIZE = 4000;
	for(Counter = 0; Counter < MAX_LEVEL_MT; Counter++) CHACCES(hMaterialWindow)[Counter] = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MUTEX_SetMat(void *Strange, MUTEX_Material *New)
{
	/*~~~~~~~~~*/
	HWND	Host;
	/*~~~~~~~~~*/

	*(void **) &Host = Strange;
	CHACCES_DETECT(Host);
	CHACCES(MutexMat) = New;

	if(New)
	{
		/*~~~~~~~~~~~~~*/
		ULONG	TCounter;
		RECT	LocalR;
		LONG	Add;
		/*~~~~~~~~~~~~~*/

		BIG_REFRESH_LOCK = 1;

		if(!CHACCES(IsClipBoard))
		{
			SetDlgItemInt(CHACCES(MMHWND), IDC_LAYER, New->NumberOfSubTextures, FALSE);
		}

		Add = 0;
		SendMessage(GetDlgItem(CHACCES(MMHWND), IDC_SOUND_COMBO), CB_RESETCONTENT, 0, 0);
		while(MUTEX_SoundID[Add][0] != 0)
			SendMessage(GetDlgItem(CHACCES(MMHWND), IDC_SOUND_COMBO), CB_ADDSTRING, 0, (LONG) MUTEX_SoundID[Add++]);

		SendMessage(GetDlgItem(CHACCES(MMHWND), IDC_SOUND_COMBO), CB_SETCURSEL, New->Sound, 0);


		for(TCounter = 0; TCounter < New->NumberOfSubTextures; TCounter++)
		{
			if(!CHACCES(hMaterialWindow)[TCounter])
			{
				CHACCES(hMaterialWindow)[TCounter] = CreateDialogParam
					(
						MyHinstance,
						MAKEINTRESOURCE(IDD_MUTEX_OneLine),
						CHACCES(Title),
						(int(__stdcall *) (struct HWND__ *, unsigned int, unsigned int, LONG)) MAD_MUTEX_OneLineCLBK,
						TCounter
					);
				GetWindowRect(CHACCES(hMaterialWindow)[TCounter], &LocalR);
				VSIZEOPEN = LocalR.bottom - LocalR.top - 1;
				HSIZEREAL = HSIZE = LocalR.right - LocalR.left;
			}

			TextureLineToWindow(CHACCES(hMaterialWindow[TCounter]), &New->AllLine[TCounter]);
		}

		BIG_REFRESH_LOCK = 0;
	}

	MUTEX_AdaptToRect(Host);
	REDRAWTHISWINDOW(CHACCES(MMHWND));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MUTEX_SetRefrechCLBK(void *Strange, int (__stdcall *RefreshCLBK) (void *, BOOL), void *UserParam)
{
	/*~~~~~~~~~*/
	HWND	Host;
	/*~~~~~~~~~*/

	*(void **) &Host = Strange;
	CHACCES_DETECT(Host);
	CHACCES(RefreshCLBK) = RefreshCLBK;
	CHACCES(UserParam) = UserParam;
}

