/*$T TEXframe.h GC!1.52 10/15/99 14:28:09 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASE/CLIbrary/CLIwin.h"
#include "BIGfiles/BIGdefs.h"
#include "EDIbaseframe.h"
#include "Res/Res.h"
#include "TEXframe_act.h"


/*$4
 ***************************************************************************************************
 Show flags
 ***************************************************************************************************
 */
#define ETEX_C_ShowName                 0x00000001

#define ETEX_C_TexShowRawPal			0x00000100
#define ETEX_C_TexShowTga				0x00000200
#define ETEX_C_TexShowAllSlot			0x00000400
#define ETEX_C_TexUseTMSlotOrder		0x00000800

#define ETEX_C_TexShowSlotPS2			0x00001000
#define ETEX_C_TexShowSlotGC			0x00002000
#define ETEX_C_TexShowSlotXBOX			0x00004000
#define ETEX_C_TexShowSlotPC			0x00008000
#define ETEX_C_TexShowSlotMask			0x0000F000
#define ETEX_C_TexShowSlotShift			12 

#define ETEX_C_SlotNb					4
extern	char	*ETEX_gsz_SlotName[ ETEX_C_SlotNb ];

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class   ETEX_cl_InsideScroll;
class ETEX_cl_Frame : public EDI_cl_BaseFrame
{
    DECLARE_DYNCREATE(ETEX_cl_Frame)

/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    ETEX_cl_Frame (void);
    ~ETEX_cl_Frame(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
	CSplitterWnd									*mpo_Splitter;
    ETEX_cl_InsideScroll							*mpo_ScrollView;
	EVAV_cl_View									*mpo_DataView;
	EVAV_tdst_VarsViewStruct                        mst_DataView;
    CList<EVAV_cl_ViewItem *, EVAV_cl_ViewItem *>   mo_ListItems;
    BIG_INDEX               mul_DirIndex;
    BIG_INDEX               mul_FileIndex;
    
    struct
    {
        int     i_Res;
        BOOL    b_AlphaShow;
        LONG    l_ShowFlag;
        BOOL    b_ForceRes;
    } mst_Ini;

    CMapStringToPtr         mo_TextureCheck;

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
    void    DeleteList(void);

    //void    ConvertRaw(BIG_INDEX, UCHAR *, UCHAR *, short, short, UCHAR, int, BOOL = FALSE);
	//void	ConvertJPG(BIG_INDEX, UCHAR *,ULONG);
    //void    ConvertTGA(ULONG, UCHAR *);
    //void    ConvertBitmap(ULONG, UCHAR *);
    //void    ConvertPalette(ULONG, UCHAR *);
    //void    ConvertRawFile(ULONG, UCHAR *);
    void    ConvertTex(ULONG, UCHAR *, ULONG, struct MAIEDITEX_tdst_BitmapDes_ *);    

    BOOL    b_UpdateTex( CPoint * );

    void    Browse(BOOL = TRUE);

/*$2
 ---------------------------------------------------------------------------------------------------
    INTERFACE.
 ---------------------------------------------------------------------------------------------------
 */

public:
    BOOL    b_CanBeLinkedToEngine(void) { return FALSE; };
    void    CloseProject(void);
    BOOL    b_CanActivate(void);
    char    *psz_OnActionGetBase(void) { return ETEX_asz_ActionBase; };
    void    Refresh(void);
    void    OnAction(ULONG);
    BOOL    b_OnActionValidate(ULONG, BOOL _b_Disp = TRUE);
    int     i_OnMessage(ULONG, ULONG, ULONG);
    UINT    ui_OnActionState(ULONG);
    void    ReinitIni(void);
    void    LoadIni(void);
    void    TreatIni(void);
    void    SaveIni(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    Check
 ---------------------------------------------------------------------------------------------------
 */
    void    ParseFile( BIG_INDEX, void (*) (void *, ULONG) );
    void    Check_DoubleName();
    void    Check_Loaded();
    void    Check_User();
    void    Check_TexFile();
    void    Check_Palette();
    void    Check_BadParams();
    void    Check_Font();
	void	Check_DuplicateRaw();

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

public:
    afx_msg int     OnCreate(LPCREATESTRUCT);
    afx_msg void    OnSize(UINT, int, int);
    afx_msg void    OnAlpha(void);
    afx_msg void    OnClose(void);
    afx_msg void    OnChooseResolution(int);
    afx_msg void    OnForceRes(void);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
