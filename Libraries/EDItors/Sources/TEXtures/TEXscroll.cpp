/*$T TEXscroll.cpp GC!1.71 01/24/00 14:20:00 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGread.h"

#include "TEXutil.h"
#include "TEXscroll.h"
#include "TEXframe.h"
#include "TEXture/TEXconvert.h"
#include "STRing/STRstruct.h"
#include "EDImainframe.h"
#include "EDIapp.h"
#include "EDImsg.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "ENGine/Sources/ENGinit.h"
#include "DIAlogs/DIACOLOR_dlg.h"
#include "DIAlogs/DIApalette_dlg.h"
#include "DIAlogs/DIAtexture_dlg.h"
#include "DIAlogs/DIAfontdescriptor_dlg.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "EDIpaths.h"
#include "LINKs/LINKstruct_reg.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKmsg.h"
//#include "DIAlogs\DIASetTextureFilter_dlg.h"

#ifdef JADEFUSION
#include "TEXture/TEXxenon.h"
#include "DIAlogs/DIAmessage_dlg.h"
#include "GraphicDK/Sources/GDInterface/GDInterface.h"

//extern "C" {
extern ULONG    EDI_OUT_gl_ForceSetMode;
//};

#if defined(_XENON_RENDER)
static char XE_TEXTURE_SIZE[] = "   8\n8\n"    \
                                "  16\n16\n"   \
                                "  32\n32\n"   \
                                "  64\n64\n"   \
                                " 128\n128\n"  \
                                " 256\n256\n"  \
                                " 512\n512\n"  \
                                "1024\n1024\n" \
                                "2048\n2048\n";

static char XE_TEXTURE_FORMAT[] = "Auto\n0\n"         \
                                  "DXT1\n1\n"         \
                                  "DXT5\n2\n"         \
                                  "A8\n3\n"           \
                                  "A8R8G8B8\n4\n"     \
                                  "DXN\n5\n"          \
                                  "CTX1\n6\n";

static char XE_TEXTURE_EXT[]        = "*.tga,*.jpg,*.bmp";
static char XE_TEXTURE_NATIVE_EXT[] = "*.dds";

static char XE_TEXTURE_MIPMAPS[512];

static ULONG s_ul_Unused = 0;

void ETEX_BuildMipMapString(ULONG _ul_MaxMipMaps)
{
    CHAR  szTemp[32];
    ULONG i;

    strcpy(XE_TEXTURE_MIPMAPS, "All\n0\nNone\n1\n");

    for (i = 2; i <= _ul_MaxMipMaps; ++i)
    {
        sprintf(szTemp, "%s%u\n%u\n", (i < 10) ? " " : "", i, i);
        strcat(XE_TEXTURE_MIPMAPS, szTemp);
    }
}

static void Xe_ChangeSourceTexture(void* _p_Owner, void* _p_Item, void* _p_Data, long _l_Old);
static void Xe_ChangeNativeTexture(void* _p_Owner, void* _p_Item, void* _p_Data, long _l_Old);
static void Xe_OnGenerate(void* _p_Owner, void* _p_Item, void* _p_Data, long _l_Old);
static void Xe_ChangeFlags(void* _p_Owner, void* _p_Item, void* _p_Data, long _l_Old);
static void Xe_ChangeOutputWidth(void* _p_Owner, void* _p_Item, void* _p_Data, long _l_Old);
static void Xe_ChangeOutputHeight(void* _p_Owner, void* _p_Item, void* _p_Data, long _l_Old);
static void Xe_ChangeOutputFormat(void* _p_Owner, void* _p_Item, void* _p_Data, long _l_Old);
static void Xe_ChangeOutputMipMaps(void* _p_Owner, void* _p_Item, void* _p_Data, long _l_Old);

#endif //_XENON_RENDER
#else //not JADEFUSION
extern "C" {extern ULONG    EDI_OUT_gl_ForceSetMode;};
#endif //JADEFUSION

char	*ETEX_gsz_SlotName[4] = { "PS2", "GC", "XBOX", "PC" };
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(ETEX_cl_InsideScroll, CScrollView)
BEGIN_MESSAGE_MAP(ETEX_cl_InsideScroll, CScrollView)
    ON_WM_RBUTTONDOWN()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_CREATE()
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ETEX_cl_InsideScroll::ETEX_cl_InsideScroll(void)
{
    mb_AlphaShow = FALSE;
    mb_ForceRes = FALSE;

    mh_MipmapOnOff[0] = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(EDITEX_IDR_MIPMAPOFF));
    mh_MipmapOnOff[1] = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(EDITEX_IDR_MIPMAPON));
    mh_MipmapColorOnOff[0] = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(EDITEX_IDR_MIPMAPCOLOROFF));
    mh_MipmapColorOnOff[1] = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(EDITEX_IDR_MIPMAPCOLORON));
    mh_MipmapAlphaOnOff[0] = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(EDITEX_IDR_MIPMAPALPHAOFF));
    mh_MipmapAlphaOnOff[1] = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(EDITEX_IDR_MIPMAPALPHAON));
    mh_MipmapKeepBorderOnOff[0] = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(EDITEX_IDR_MIPMAPKEEPBORDEROFF));
    mh_MipmapKeepBorderOnOff[1] = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(EDITEX_IDR_MIPMAPKEEPBORDERON));
    mh_FontOnOff[0] = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(EDITEX_IDR_FONTOFF));
    mh_FontOnOff[1] = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(EDITEX_IDR_FONTON));
    mh_UpdatablePal[0] = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(EDITEX_IDR_UPDATABLEPALOFF));
    mh_UpdatablePal[1] = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(EDITEX_IDR_UPDATABLEPALON));
    mh_InterfaceOnOff[0] = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(EDITEX_IDR_INTERFACEOFF));
    mh_InterfaceOnOff[1] = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(EDITEX_IDR_INTERFACEON));
    mh_BumpMap[0] = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(EDITEX_IDR_BUMPOFF));
    mh_BumpMap[1] = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(EDITEX_IDR_BUMPON));

    mpo_SelBrush = new CBrush((COLORREF) 0xFF);
    mpo_UnselBrush = new CBrush((COLORREF) 0xFFFFFF);
    mpo_ColorBoderBrush = new CBrush((COLORREF) 0);
    mpo_BackBrush = new CBrush(GetSysColor(COLOR_BTNFACE));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ETEX_cl_InsideScroll::~ETEX_cl_InsideScroll(void)
{
    DeleteObject(mh_MipmapOnOff[0]);
    DeleteObject(mh_MipmapOnOff[1]);
    DeleteObject(mh_MipmapColorOnOff[0]);
    DeleteObject(mh_MipmapColorOnOff[1]);
    DeleteObject(mh_MipmapAlphaOnOff[0]);
    DeleteObject(mh_MipmapAlphaOnOff[1]);
    DeleteObject(mh_MipmapKeepBorderOnOff[0]);
    DeleteObject(mh_MipmapKeepBorderOnOff[1]);
    DeleteObject(mh_FontOnOff[0]);
    DeleteObject(mh_FontOnOff[1]);
    DeleteObject(mh_UpdatablePal[0] );
    DeleteObject(mh_UpdatablePal[1] );
    DeleteObject(mh_InterfaceOnOff[0] );
    DeleteObject(mh_InterfaceOnOff[1] );
    DeleteObject(mh_BumpMap[0] );
    DeleteObject(mh_BumpMap[1] );


    delete mpo_SelBrush;
    delete mpo_ColorBoderBrush;
    delete mpo_BackBrush;
    delete mpo_UnselBrush;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_InsideScroll::OnLButtonUp(UINT, CPoint pt)
{
    M_MF()->EndDragDrop(pt);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_InsideScroll::OnRButtonDown(UINT nId, CPoint pt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION                    pos, x_SelPos;
    MAIEDITEX_tdst_BitmapDes    *pdes;
    CRect                       o_Rect;
    int                         i_Res, i_Slot; //, i, j, k;
    char                        sz_Text[ 256 ];
	EMEN_cl_SubMenu				o_Menu(FALSE);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(EDI_gst_DragDrop.b_BeginDragDrop) 
    {
        M_MF()->CancelDragDrop();
        return;
    }

    OnLButtonDown( -1, pt );

    pos = mo_BmpList.GetHeadPosition();
    while(pos)
    {
		x_SelPos = pos;
        pdes = mo_BmpList.GetNext(pos);
        if(pdes->ul_FatFile == mul_Selected)
			break;
	}
	if (!x_SelPos) return;
	if (!pdes) return;
			
	M_MF()->InitPopupMenuAction(NULL, &o_Menu);

    if ( pdes->i_Type == MAIEDITEX_C_TGA )
    {
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 100, TRUE, "Edit tga", -1);
		if ( mst_CurDes.uw_Flags & TEX_FP_ThisIsAFont )
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 101, TRUE, "Edit font descriptor", -1);
		//M_MF()->AddPopupMenuAction(NULL, &o_Menu, 102, TRUE, "Change size", -1);
		//M_MF()->AddPopupMenuAction(NULL, &o_Menu, 103, TRUE, "Generate raw + pal", -1);
		//if ( pdes->st_Bmp.st_Header.biBitCount == 24 )
		//	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 104, TRUE, "Add alpha channel", -1);
		//else if ( pdes->st_Bmp.st_Header.biBitCount == 32 )
		//	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 105, TRUE, "Del alpha channel", -1);
	}
	else if (pdes->i_Type == MAIEDITEX_C_PALETTE )
	{
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 200, TRUE, "Edit palette", -1);
		//M_MF()->AddPopupMenuAction(NULL, &o_Menu, 201, TRUE, "List raw associated with", -1);
	}
	else if (pdes->i_Type == MAIEDITEX_C_RAW )
	{
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 300, TRUE, "Edit raw", -1);
		if ( mst_CurDes.uw_Flags & TEX_FP_ThisIsAFont )
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 301, TRUE, "Edit font descriptor", -1);
		//M_MF()->AddPopupMenuAction(NULL, &o_Menu, 302, TRUE, "Change size", -1);
		//M_MF()->AddPopupMenuAction(NULL, &o_Menu, 303, TRUE, "Change color depth", -1);
		//M_MF()->AddPopupMenuAction(NULL, &o_Menu, 304, TRUE, "Create TGA", -1);
		//M_MF()->AddPopupMenuAction(NULL, &o_Menu, 305, TRUE, "List pal associated with", -1);
	}
	else if (pdes->i_Type == MAIEDITEX_C_TEX )
	{
		i_Res = 0;
		for (i_Slot = 0; i_Slot < 4; i_Slot++)
		{
			if (( pdes->st_Tex.ast_Slot[ i_Slot ].ul_Pal != BIG_C_InvalidKey ) && (pdes->st_Tex.ast_Slot[ i_Slot ].ul_Raw != BIG_C_InvalidKey))
			{
				sprintf( sz_Text, "[%s] Edit raw", ETEX_gsz_SlotName[ i_Slot ]);
				M_MF()->AddPopupMenuAction(NULL, &o_Menu, 410 + i_Slot, TRUE, sz_Text, -1);
				sprintf( sz_Text, "[%s] Create TGA", ETEX_gsz_SlotName[ i_Slot ]);
				M_MF()->AddPopupMenuAction(NULL, &o_Menu, 400 + (i_Slot*2), TRUE, sz_Text, -1);
				strcat( sz_Text, " (palette)" );
				M_MF()->AddPopupMenuAction(NULL, &o_Menu, 401 + (i_Slot*2), TRUE, sz_Text, -1);
				sprintf( sz_Text, "[%s] import alpha mask", ETEX_gsz_SlotName[ i_Slot ]);
				M_MF()->AddPopupMenuAction(NULL, &o_Menu, 420 + i_Slot, TRUE, sz_Text, -1);
				i_Res++;
			}
		}
		if (i_Res > 1)
		{
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 408, TRUE, "Create TGA for all", -1);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 409, TRUE, "Create TGA (palette) for all", -1);
		}
	}
	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Show in browser", -1);
	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Remove from editor", -1);
	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, "Check User", -1);
	//M_MF()->AddPopupMenuAction(NULL, &o_Menu, 4, TRUE, "Set Id Filter", -1);

	GetCursorPos( &pt );
	i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);

	switch (i_Res )
	{
	case 1:
        if  (pdes->ul_FatFile != BIG_C_InvalidIndex)
		{
			EBRO_cl_Frame   *po_Browser;
			po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
            po_Browser->i_OnMessage( EDI_MESSAGE_SELFILE, BIG_ParentFile( pdes->ul_FatFile ), pdes->ul_FatFile );
		}
		break;
	case 2:
		mo_BmpList.RemoveAt( x_SelPos );
		mpo_Parent->Refresh();
		break;
	case 3:
		mpo_Parent->Check_User();
		break;
	// definit le canal id d'une texture
	case 4:
		{
			//EDIA_cl_SetTextureFilterDialogCombo dlg_SetTextureFilterDialogCombo( "Set ID Texture" );
			//dlg_SetTextureFilterDialogCombo.DoModal();
			break;
		}
	case 100:
		{
			EDIA_cl_Texture dlg_Texture( pdes->ul_FatFile, -1 );
			dlg_Texture.DoModal();
			mpo_Parent->Browse();
			break;
		}
	case 101:
	case 301:
		{
			EDIA_cl_FontDescriptorDialog dlg_Font( pdes->ul_FatFile );
            dlg_Font.DoModal();
			break;
        }
	case 200:
		{
			EDIA_cl_Palette dlg_Palette( pdes->ul_FatFile );
            dlg_Palette.DoModal();
            mpo_Parent->Browse();
			break;
		}
	case 300:
		{
			EDIA_cl_Texture dlg_Texture( pdes->ul_FatFile, -1 );
            dlg_Texture.DoModal();
            mpo_Parent->Browse();
			break;
		}
	case 400:
	case 401:
	case 402:
	case 403:
	case 404:
	case 405:
	case 406:
	case 407:
		CreateTga( pdes, (i_Res - 400) >> 1, i_Res & 1 );
		break;
	case 408:
	case 409:
		CreateTga( pdes, -1, i_Res & 1 );
		break;
	case 410:
	case 411:
	case 412:
	case 413:
		{
			EDIA_cl_Texture dlg_Texture( pdes->ul_FatFile, i_Res - 410 );
            dlg_Texture.DoModal();
            mpo_Parent->Browse();
			break;
		}
	case 420:
	case 421:
	case 422:
	case 423:
		ImportAlphaMask( pdes, i_Res - 420 );
		mpo_Parent->Browse();
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_InsideScroll::OnMouseMove(UINT, CPoint pt)
{
    M_MF()->b_MoveDragDrop(pt);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_InsideScroll::OnLButtonDown(UINT nID, CPoint pt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION                    pos;
    MAIEDITEX_tdst_BitmapDes    *pdes, *pdes1 = NULL;
    CRect                       o_Rect;
    int                         x, y;
    CPoint                      pt1;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    x = GetScrollPos(SB_HORZ);
    y = GetScrollPos(SB_VERT);
    pt1 = pt;
    pt.x += x;
    pt.y += y;

    /* Search in bmp list the one that correspond the the pt */
    pos = mo_BmpList.GetHeadPosition();
    while(pos)
    {
        pdes = mo_BmpList.GetNext(pos);
        if(pdes->ul_FatFile == mul_Selected) pdes1 = pdes;

        if(pdes->o_Rect.PtInRect(pt))
        {
            o_Rect = pdes->o_Rect;
            o_Rect -= CPoint(x, y);
            InvalidateRect(&o_Rect);
            o_Rect.DeflateRect(2, 2);
            o_Rect.bottom -= mi_InfoHeight;

            if(mul_Selected != pdes->ul_FatFile)
            {
                if(mul_Selected != BIG_C_InvalidIndex)
                {
                    /* Search the previous selected bmp */
                    if(pdes1 == NULL)
                    {
                        while(pos)
                        {
                            pdes1 = mo_BmpList.GetNext(pos);
                            if(pdes1->ul_FatFile == mul_Selected) break;
                        }
                    }

                    /* Invalidate (to redraw) current selected bitmap */
                    o_Rect = pdes1->o_Rect;
                    o_Rect -= CPoint(x, y);
                    InvalidateRect(&o_Rect);
                    o_Rect.DeflateRect(2, 2);
                    o_Rect.bottom -= mi_InfoHeight;
                }

                mul_Selected = pdes->ul_FatFile;
				if (mul_Selected != mst_CurDes.ul_FatFile)
				{
					L_memcpy( &mst_CurDes, pdes, sizeof( MAIEDITEX_tdst_BitmapDes ) );
					UpdateInfo();
				}

                /* Send message to link editors */
                if(nID != -1)
                    M_MF()->SendMessageToLinks( mpo_Parent, EDI_MESSAGE_SELFILE, mpo_Parent->mul_DirIndex, pdes->ul_FatFile );
            }

            /*
             * For drag & drop. OnLButtonDown can be called from outside to force selection.
             * In that case, nID must be set to -1 to tell the we don't want a drag&drop
             * operation.
             */
            if(nID != -1)
            {
                EDI_gst_DragDrop.ul_FatDir = mul_DirIndex;
                EDI_gst_DragDrop.ul_FatFile = pdes->ul_FatFile;
                M_MF()->BeginDragDrop(pt1, this, mpo_Parent);
            }
            return;
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_InsideScroll::OnLButtonDblClk(UINT nFlags, CPoint pt)
{
	return;
#ifdef OLDEDITOR
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION                    pos;
    MAIEDITEX_tdst_BitmapDes    *pdes;
    CRect                       o_Rect;
    //int                         Line;
    TEX_tdst_File_Params        st_Tex;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    pos = mo_BmpList.GetHeadPosition();
    while(pos)
    {
        pdes = mo_BmpList.GetNext(pos);
        if(pdes->ul_FatFile == mul_Selected)
        {
            if ( b_ClickOnInfo(pdes, pt, 1) )
            {
                o_Rect = pdes->o_Rect + CPoint(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
                InvalidateRect(&o_Rect);
            }
            else
            {
                o_Rect = pdes->o_Rect - CPoint(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
                if (o_Rect.PtInRect( pt ) )
                {
                    if (pdes->i_Type == MAIEDITEX_C_PALETTE)
                    {
                        EDIA_cl_Palette dlg_Palette( pdes->ul_FatFile );
                        dlg_Palette.DoModal();
                        mpo_Parent->Browse();
                    }
#ifdef JADEFUSION
                    else if ( (pdes->i_Type == MAIEDITEX_C_TGA) || (pdes->i_Type == MAIEDITEX_C_RAW) || (pdes->i_Type == MAIEDITEX_C_DDS) )
#else                    
					else if ( (pdes->i_Type == MAIEDITEX_C_TGA) || (pdes->i_Type == MAIEDITEX_C_RAW) )
#endif                    
					{
                        TEX_File_GetParams( &st_Tex, pdes->ul_FatFile);
                        if (st_Tex.uw_Flags & TEX_FP_ThisIsAFont)
                        {
                            EDIA_cl_FontDescriptorDialog dlg_Font( pdes->ul_FatFile );
                            dlg_Font.DoModal();
                        }
                    }
                    /*
					else if (pdes->i_Type == MAIEDITEX_C_TEX)
                    {
                        Line = (pt.y - o_Rect.top - 1 - pdes->st_Tex.i_FileStart) / 16;
                        if ( (Line >= 0)  && (Line <= 2) )
                        {
                            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
                    /*        EDIA_cl_FileDialog  o_File("", 3, TRUE, TRUE, EDI_Csz_Path_Textures);
                            char        *psz_Temp;
                            CString     o_Temp;
                            BIG_KEY     ul_Key;
                            BIG_INDEX   ul_Index;
                            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

                     /*       if (Line == 0) o_File.mo_Title = "Choose raw";
                            else if (Line == 1) o_File.mo_Title = "Choose pal";
                            else if (Line == 2) o_File.mo_Title = "Choose tga";

                            if(o_File.DoModal() != IDOK) return;
                            o_File.GetItem(o_File.mo_File, 1, o_Temp);
                            psz_Temp = (char *) (LPCSTR) o_Temp;
                            if(BIG_b_CheckName(psz_Temp) == FALSE) return;
                            if(!TEX_l_File_IsFormatSupported(psz_Temp, Line)) return;
                            ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, psz_Temp);
                            if(ul_Index == BIG_C_InvalidIndex) return;
                            ul_Key = BIG_FileKey(ul_Index);

                            if (Line == 0)
                            {
                                if (pdes->st_Tex.ul_Raw4or8 != ul_Key )
                                {
                                    pdes->st_Tex.ul_Raw4or8 = ul_Key;
                                    SaveTex( pdes );
                                }
                            }
                            else if (Line == 1)
                            {
                                if (pdes->st_Tex.ul_Palette != ul_Key )
                                {
                                    pdes->st_Tex.ul_Palette = ul_Key;
                                    SaveTex( pdes );
                                }
                            }
                            else if (Line == 2)
                            {
                                if (pdes->st_Tex.ul_Raw24or32 != ul_Key )
                                {
                                    pdes->st_Tex.ul_Raw24or32 = ul_Key;
                                    SaveTex( pdes );
                                }
                            }
                        }
                    }
					*/
                }
            }
            return;
        }
    }
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_InsideScroll::OnKeyDown( UINT nChar, UINT nRep, UINT nFlags )
{
	if (nChar == VK_DELETE)
    {
        CPoint      o_Pt;
        POSITION	pos;

        GetCursorPos( &o_Pt );
        pos = GetPosUnderPoint( &o_Pt );
		if ( pos )
		{
			mo_BmpList.RemoveAt( pos );
			mpo_Parent->Refresh();
		}
    }
    CScrollView::OnKeyDown(nChar, nRep, nFlags);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_InsideScroll::SaveTex( MAIEDITEX_tdst_BitmapDes *pDes )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    sz_Path[ BIG_C_MaxLenPath ];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    BIG_ComputeFullName( BIG_ParentFile( pDes->ul_FatFile ), sz_Path );
    TEX_ul_File_CreatePaletteRawTexture
        ( 
            sz_Path, 
            BIG_NameFile( pDes->ul_FatFile ), 
            pDes->st_Tex.ast_Slot[0].ul_Raw, 
            pDes->st_Tex.ast_Slot[0].ul_Pal, 
            pDes->st_Tex.ast_Slot[0].ul_TC
        );

    mpo_Parent->ConvertTex( 0, 0, 0, pDes );

    EDI_OUT_gl_ForceSetMode = 1;
    LINK_Refresh();
    EDI_OUT_gl_ForceSetMode = 0;

    mpo_Parent->Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
POSITION ETEX_cl_InsideScroll::GetPosUnderPoint( CPoint *_po_Pt )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION                    pos, x_PrevPos;
    MAIEDITEX_tdst_BitmapDes    *pdes;
    CRect                       o_Rect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ScreenToClient( _po_Pt );

    x_PrevPos = pos = mo_BmpList.GetHeadPosition();
    while(pos)
    {
		x_PrevPos = pos;
        pdes = mo_BmpList.GetNext(pos);
        o_Rect = pdes->o_Rect - CPoint(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
        if (o_Rect.PtInRect( *(POINT *) _po_Pt ) )
            return x_PrevPos;
    }
    return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAIEDITEX_tdst_BitmapDes *ETEX_cl_InsideScroll::GetDesUnderPoint( CPoint *_po_Pt )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION                    pos;
    MAIEDITEX_tdst_BitmapDes    *pdes;
    CRect                       o_Rect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ScreenToClient( _po_Pt );

    pos = mo_BmpList.GetHeadPosition();
    while(pos)
    {
        pdes = mo_BmpList.GetNext(pos);
        o_Rect = pdes->o_Rect - CPoint(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
        if (o_Rect.PtInRect( *(POINT *) _po_Pt ) )
            return pdes;
    }
    return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ETEX_cl_InsideScroll::OnCreate(LPCREATESTRUCT _pst_CS)
{
    SetClassLong(GetSafeHwnd(), GCL_HBRBACKGROUND, (LONG) mpo_BackBrush->GetSafeHandle());
    return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ETEX_cl_InsideScroll::mi_ResToWidth(void)
{
    switch(mi_Res)
    {
    case 0:
        return -1;
    case 1:
        return 32;
    case 2:
        return 64;
    case 3:
        return 128;
    case 4:
        return 256;
    }
    return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_InsideScroll::OnDraw(CDC *pdc)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CDC                         dc;
    POSITION                    pos;
    MAIEDITEX_tdst_BitmapDes    *pdes;
    int                         x, y, maxy, sizex, sizey;
    int                         i_Width, i_Height, i_Sel;
    CRect                       o_Rect, o_InfoRect;
    CBitmap                     *po_Old, *po_Tmp;
    BOOL                        b_First;
    CFont                       *oldFnt;
    CBrush                      *po_RectBrush;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    maxy = 0;
    po_Old = NULL;
    b_First = FALSE;

    dc.CreateCompatibleDC(pdc);

    GetClientRect(&o_Rect);

    sizex = OFFSET / 2;
    sizey = OFFSET / 2;
    x = OFFSET / 2;
    y = OFFSET / 2;
	i_Sel = 0;

    mi_InfoHeight = i_GetInfoHeight();
    oldFnt = pdc->SelectObject(&M_MF()->mo_Fnt);

    pos = mo_BmpList.GetHeadPosition();
    while(pos)
    {
        pdes = mo_BmpList.GetNext(pos);

        /* Get display size */
        i_Width = i_Height = mi_ResToWidth();
        
        if(i_Width == -1)
        {
            i_Width = pdes->st_Bmp.st_Header.biWidth;
            i_Height = pdes->st_Bmp.st_Header.biHeight;
        }

        if ( !mb_ForceRes )
        {
            if(i_Width > pdes->st_Bmp.st_Header.biWidth) i_Width = pdes->st_Bmp.st_Header.biWidth;
            if(i_Height > pdes->st_Bmp.st_Header.biHeight) i_Height = pdes->st_Bmp.st_Header.biHeight;
        }

        /* Compute position */
        if(x + i_Width > o_Rect.right)
        {
            if(x)
            {
                x = OFFSET / 2;
                y += maxy + OFFSET;
                sizey += (maxy + OFFSET);
                maxy = 0;
            }
            else
                continue;
        }
        
        /* Remember coordinates for selection */
        pdes->o_Rect.left = x - OFFSET / 2;
        pdes->o_Rect.top = y - OFFSET / 2;
        pdes->o_Rect.right = x + i_Width + OFFSET / 2;
        pdes->o_Rect.bottom = y + i_Height + mi_InfoHeight + OFFSET / 2;

        /* Is the bitmap selected ? */
        po_RectBrush = mpo_UnselBrush;
        if(pdes->ul_FatFile == mul_Selected)
        {
			i_Sel = 1;
            po_RectBrush = mpo_SelBrush;
			if ( mul_Selected != mst_CurDes.ul_FatFile )
			{
				L_memcpy( &mst_CurDes, pdes, sizeof( MAIEDITEX_tdst_BitmapDes ) );
				UpdateInfo();
			}
        }

        pdc->FrameRect(&pdes->o_Rect, po_RectBrush);
        pdes->o_Rect.DeflateRect(1, 1);
        pdc->FrameRect(&pdes->o_Rect, po_RectBrush);
        pdes->o_Rect.InflateRect(1, 1);

        /* Select bitmap or alpha bitmap depending on current mode */
        if(!mb_AlphaShow || pdes->st_Bmp.h_BitmapAlpha == NULL)
            po_Tmp = (CBitmap *) dc.SelectObject(pdes->st_Bmp.h_Bitmap);
        else
            po_Tmp = (CBitmap *) dc.SelectObject(pdes->st_Bmp.h_BitmapAlpha);

        if(b_First) po_Old = po_Tmp;

        b_First = FALSE;

        /* Draw bitmap */
        if((i_Width == pdes->st_Bmp.st_Header.biWidth) && (i_Height == pdes->st_Bmp.st_Header.biHeight))
        {
            pdc->BitBlt(x, y, i_Width, i_Height, &dc, 0, 0, SRCCOPY);
        }
        else
        {
            pdc->SetStretchBltMode(HALFTONE);
            pdc->StretchBlt
                (
                    x,
                    y,
                    i_Width,
                    i_Height,
                    &dc,
                    0,
                    0,
                    pdes->st_Bmp.st_Header.biWidth,
                    pdes->st_Bmp.st_Header.biHeight,
                    SRCCOPY
                );
        }

        /* Display info */
        o_InfoRect = pdes->o_Rect;
        o_InfoRect.top += i_Height;
        DisplayInfo(pdc, (RECT *) &o_InfoRect, pdes);

        /* Next position */
        x += i_Width + OFFSET;
        if(x > sizex) sizex = x;

        if(i_Height + mi_InfoHeight > maxy) maxy = i_Height + mi_InfoHeight;
    }

    pdc->SelectObject(oldFnt);

    /* Last bitmap of dc */
    if(po_Old) dc.SelectObject(po_Old);

    /* For scroll capabilities */
    if(maxy) sizey += maxy + OFFSET;
    mo_Size = CSize(sizex, sizey);

	if ( !i_Sel )
	{
		mul_Selected = BIG_C_InvalidIndex;
		if (mul_Selected != mst_CurDes.ul_FatFile)
		{
			mst_CurDes.ul_FatFile = BIG_C_InvalidIndex;
			UpdateInfo();
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ETEX_cl_InsideScroll::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll)
{
    CScrollView::OnScroll(nScrollCode, nPos, bDoScroll);
    return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ETEX_cl_InsideScroll::i_GetInfoHeight(void)
{
    if(mpo_Parent->mst_Ini.l_ShowFlag & ETEX_C_ShowName)
        return (16 + (OFFSET / 2));
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_InsideScroll::DisplayInfo(CDC *o_DC, RECT *pst_InRect, MAIEDITEX_tdst_BitmapDes *pst_BD)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG                    l_Show;
    RECT                    st_InRect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    l_Show = mpo_Parent->mst_Ini.l_ShowFlag;
    if(l_Show & ETEX_C_ShowName)
	{
	    st_InRect.left = pst_InRect->left + OFFSET;
		st_InRect.top = pst_InRect->top + OFFSET;
		st_InRect.right = pst_InRect->right - OFFSET;
		st_InRect.bottom = st_InRect.top + 16;

		o_DC->SetTextColor(0);
		o_DC->SetBkMode(TRANSPARENT);

		st_InRect.top += 1;
		st_InRect.left += 4;
		o_DC->DrawText(BIG_NameFile(pst_BD->ul_FatFile), -1, &st_InRect, DT_SINGLELINE);
		st_InRect.left -= 4;
		st_InRect.top = st_InRect.bottom;
		st_InRect.bottom = st_InRect.top + 16;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ChangeQuality(void *_p_Owner, void *_p_Item, void *_p_Data, long _l_Old )
{
	ETEX_cl_InsideScroll	*po_Editor;
	EVAV_cl_ViewItem		*po_Item;
	TEX_tdst_File_Params    st_Tex;
	
	po_Item = (EVAV_cl_ViewItem	* ) _p_Item;
	po_Editor = ((ETEX_cl_Frame *) _p_Owner)->mpo_ScrollView;

	TEX_File_GetParams(&st_Tex, po_Editor->mst_CurDes.ul_FatFile);
	st_Tex.uw_Flags &= ~( TEX_FP_QualityMask );
	st_Tex.uw_Flags |= (*(long *) _p_Data) & TEX_FP_QualityMask;
	TEX_File_SetParams(&st_Tex, po_Editor->mst_CurDes.ul_FatFile, FALSE);

	po_Editor->UpdateInfo();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ChangeFlags(void *_p_Owner, void *_p_Item, void *_p_Data, long _l_Old )
{
	ETEX_cl_InsideScroll	*po_Editor;
	EVAV_cl_ViewItem		*po_Item;
	TEX_tdst_File_Params    st_Tex;
	char					sz_Name[ 260 ], *sz_Ext;
	ULONG					ul_FontDesc;
		
	po_Item = (EVAV_cl_ViewItem	* ) _p_Item;
	po_Editor = ((ETEX_cl_Frame *) _p_Owner)->mpo_ScrollView;
	TEX_File_GetParams(&st_Tex, po_Editor->mst_CurDes.ul_FatFile);
	st_Tex.uw_Flags &= ~(1 << po_Item->mi_Param1);
	st_Tex.uw_Flags |= (*(long *) _p_Data) & (1 << po_Item->mi_Param1);
	
	if ( ((1 << po_Item->mi_Param1) == TEX_FP_ThisIsAFont) && (st_Tex.uw_Flags & st_Tex.uw_Flags) )
	{
		L_strcpy(sz_Name, BIG_NameFile( po_Editor->mst_CurDes.ul_FatFile ));
		sz_Ext = L_strrchr(sz_Name, '.' );
		if(sz_Ext)
			L_strcpy(sz_Ext, ".fod");

		ul_FontDesc = STR_ul_CreateFontDescriptor( BIG_ParentFile( po_Editor->mst_CurDes.ul_FatFile ), sz_Name, BIG_FileKey(po_Editor->mst_CurDes.ul_FatFile) );
		po_Editor->mst_CurDes.ul_FontDescKey = st_Tex.ul_Params[ 0 ] = BIG_FileKey( ul_FontDesc );
	}
	
	TEX_File_SetParams(&st_Tex, po_Editor->mst_CurDes.ul_FatFile, FALSE);
	po_Editor->UpdateInfo();
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ChangeColor(void *_p_Owner, void *_p_Item, void *_p_Data, long _l_Old )
{
	ETEX_cl_InsideScroll	*po_Editor;
	EVAV_cl_ViewItem		*po_Item;
	TEX_tdst_File_Params    st_Tex;
	EDIA_cl_ColorDialog		*po_Dial;
	
	po_Item = (EVAV_cl_ViewItem	* ) _p_Item;
	po_Editor = ((ETEX_cl_Frame *) _p_Owner)->mpo_ScrollView;
	TEX_File_GetParams(&st_Tex, po_Editor->mst_CurDes.ul_FatFile);
	
	po_Dial = new EDIA_cl_ColorDialog("Mipmap color", &st_Tex.ul_Color, NULL, 0, 0);
    if(po_Dial->DoModal())
    {
		TEX_File_SetParams(&st_Tex, po_Editor->mst_CurDes.ul_FatFile, FALSE);
		po_Editor->UpdateInfo();
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ChangeFontDesc(void *_p_Owner, void *_p_Item, void *_p_Data, long _l_Old )
{
	ETEX_cl_InsideScroll	*po_Editor;
	EVAV_cl_ViewItem		*po_Item;
	TEX_tdst_File_Params    st_Tex;
	
	po_Item = (EVAV_cl_ViewItem	* ) _p_Item;
	po_Editor = ((ETEX_cl_Frame *) _p_Owner)->mpo_ScrollView;
	TEX_File_GetParams(&st_Tex, po_Editor->mst_CurDes.ul_FatFile);
	st_Tex.ul_Params[ 0 ] = *(long *) _p_Data;
	TEX_File_SetParams(&st_Tex, po_Editor->mst_CurDes.ul_FatFile, FALSE);
	po_Editor->UpdateInfo();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ChangeTexKey(void *_p_Owner, void *_p_Item, void *_p_Data, long _l_Old )
{
	ETEX_cl_InsideScroll	*po_Editor;
	EVAV_cl_ViewItem		*po_Item;

	if (_l_Old == 0) return;

	po_Item = (EVAV_cl_ViewItem	* ) _p_Item;
	po_Editor = ((ETEX_cl_Frame *) _p_Owner)->mpo_ScrollView;
	TEX_ul_File_SaveTexWithIndex( po_Editor->mst_CurDes.ul_FatFile, &po_Editor->mst_CurDes.st_Tex );
	po_Editor->mpo_Parent->Browse();
}

static const char *StringForImageType( int type )
{
	switch ( type )
	{
		default:
			return "Unknown";
		case MAIEDITEX_C_TGA:
			return "TGA";
		case MAIEDITEX_C_PNG:
			return "PNG";
		case MAIEDITEX_C_PSD:
			return "PSD";
		case MAIEDITEX_C_GIF:
			return "GIF";
		case MAIEDITEX_C_RAW:
			return "RAW";
		case MAIEDITEX_C_DDS:
			return "DDS";
		case MAIEDITEX_C_TEX:
	        return "TEX";
		case MAIEDITEX_C_PALETTE:
			return "Palette";
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_InsideScroll::UpdateInfo( ) 
{
	TEX_tdst_File_Params    st_Tex;
	EVAV_cl_ViewItem		*po_Item;
	char                    asz_Infos[ BIG_C_MaxLenPath ];
	int						i_Slot;

	/* empty content of var view */
	mpo_Parent->mpo_DataView->ResetList();
	if ( mst_CurDes.ul_FatFile == BIG_C_InvalidIndex )
	{
		mpo_DialogBar->GetDlgItem(IDC_FILE)->SetWindowText( "" );
		mpo_DialogBar->GetDlgItem(IDC_PATH)->SetWindowText( "" );
		mpo_DialogBar->GetDlgItem(IDC_INFOS)->SetWindowText( "" );
		return;
	}
	
	/* get parameters */
	TEX_File_GetParams(&st_Tex, mst_CurDes.ul_FatFile);
	mst_CurDes.c_Quality = st_Tex.uw_Flags & TEX_FP_QualityMask;
	mst_CurDes.uw_Flags = st_Tex.uw_Flags;
	mst_CurDes.ul_Color = st_Tex.ul_Color;

	/* add name */
	mpo_Parent->mpo_DataView->AddItem("Name", EVAV_EVVIT_String, BIG_NameFile( mst_CurDes.ul_FatFile ), EVAV_ReadOnly );

	const char *typeLabel = StringForImageType( mst_CurDes.i_Type );
	mpo_Parent->mpo_DataView->AddItem( "Type", EVAV_EVVIT_String, ( void * ) typeLabel, EVAV_ReadOnly );
	
	switch( mst_CurDes.i_Type )
	{
	case MAIEDITEX_C_PALETTE: 
		mpo_Parent->mpo_DataView->AddItem("Number of Colors", EVAV_EVVIT_String, (mst_CurDes.st_Tex.ast_Slot[0].ul_Pal & TEX_uc_Palette16) ? "16" : "256", EVAV_ReadOnly );
		mpo_Parent->mpo_DataView->AddItem("Alpha Channel", EVAV_EVVIT_String, (mst_CurDes.st_Tex.ast_Slot[0].ul_Pal & TEX_uc_AlphaPalette) ? "Yes" : "No", EVAV_ReadOnly );
		break;
	case MAIEDITEX_C_TGA:
	case MAIEDITEX_C_PNG:
	case MAIEDITEX_C_PSD:
	case MAIEDITEX_C_GIF:
	case MAIEDITEX_C_RAW:
		mpo_Parent->mpo_DataView->AddItem( "Width", EVAV_EVVIT_Int, &mst_CurDes.st_Bmp.st_Header.biWidth, EVAV_ReadOnly );
		mpo_Parent->mpo_DataView->AddItem( "Height", EVAV_EVVIT_Int, &mst_CurDes.st_Bmp.st_Header.biHeight, EVAV_ReadOnly );
		mpo_Parent->mpo_DataView->AddItem( "Bit per pixel", EVAV_EVVIT_Int, &mst_CurDes.st_Bmp.st_Header.biBitCount, EVAV_ReadOnly );
		mpo_Parent->mpo_DataView->AddItem( "Compressed", EVAV_EVVIT_String, mst_CurDes.st_Bmp.b_Compress ? "Yes" : "No", EVAV_ReadOnly );

		mpo_Parent->mpo_DataView->AddItem( "Quality", EVAV_EVVIT_ConstInt, &mst_CurDes.c_Quality, 0, 0, 0, 1, 0, (int) "0 - Very low\n0\n1 - Low\n1\n2 - Medium\n2\n3 - High\n3\n4 - Very high\n4\n" );
		po_Item = mpo_Parent->mo_ListItems.GetTail();
		po_Item->mpfn_CB = ChangeQuality;
				
		mpo_Parent->mpo_DataView->AddItem( "Mipmap", EVAV_EVVIT_Flags, &mst_CurDes.uw_Flags, 0, 3, 2 );
		po_Item = mpo_Parent->mo_ListItems.GetTail();
		po_Item->mpfn_CB = ChangeFlags;
		mpo_Parent->mpo_DataView->AddItem( "Mipmap keep border", EVAV_EVVIT_Flags, &mst_CurDes.uw_Flags, 0, 7, 2 );
		po_Item = mpo_Parent->mo_ListItems.GetTail();
		po_Item->mpfn_CB = ChangeFlags;
		mpo_Parent->mpo_DataView->AddItem( "Mipmap use alpha", EVAV_EVVIT_Flags, &mst_CurDes.uw_Flags, 0, 4, 2 );
		po_Item = mpo_Parent->mo_ListItems.GetTail();
		po_Item->mpfn_CB = ChangeFlags;
		mpo_Parent->mpo_DataView->AddItem( "Mipmap use color", EVAV_EVVIT_Flags, &mst_CurDes.uw_Flags, 0, 5, 2 );
		po_Item = mpo_Parent->mo_ListItems.GetTail();
		po_Item->mpfn_CB = ChangeFlags;
		mpo_Parent->mpo_DataView->AddItem( "Mipmap color",  EVAV_EVVIT_Color, &mst_CurDes.ul_Color );
		po_Item = mpo_Parent->mo_ListItems.GetTail();
		po_Item->mpfn_CB = ChangeColor;

		mpo_Parent->mpo_DataView->AddItem( "Interface texture", EVAV_EVVIT_Flags, &mst_CurDes.uw_Flags, 0, 9, 2 );
		po_Item = mpo_Parent->mo_ListItems.GetTail();
		po_Item->mpfn_CB = ChangeFlags;
		if (mst_CurDes.uw_Flags & TEX_FP_Interface )
			po_Item->mx_Color = 0xFF00;
		mpo_Parent->mpo_DataView->AddItem( "Has a font descriptor", EVAV_EVVIT_Flags, &mst_CurDes.uw_Flags, 0, 6, 2 );
		po_Item = mpo_Parent->mo_ListItems.GetTail();
		po_Item->mpfn_CB = ChangeFlags;
		if ( mst_CurDes.uw_Flags & TEX_FP_ThisIsAFont )
		{
			po_Item->mx_Color = 0xFF00;
			mst_CurDes.ul_FontDescKey = st_Tex.ul_Params[ 0 ];
			mpo_Parent->mpo_DataView->AddItem( "Font descriptor" , EVAV_EVVIT_Key, &mst_CurDes.ul_FontDescKey );
			po_Item = mpo_Parent->mo_ListItems.GetTail();
			po_Item->mpfn_CB = ChangeFontDesc;
		}
		mpo_Parent->mpo_DataView->AddItem( "Is normal map", EVAV_EVVIT_Flags, &mst_CurDes.uw_Flags, 0, 0xb, 2 );
		po_Item = mpo_Parent->mo_ListItems.GetTail();
		po_Item->mpfn_CB = ChangeFlags;

		break;

    case MAIEDITEX_C_DDS:
        mpo_Parent->mpo_DataView->AddItem( "Width",  EVAV_EVVIT_Int, &mst_CurDes.st_Bmp.st_Header.biWidth,  EVAV_ReadOnly );
        mpo_Parent->mpo_DataView->AddItem( "Height", EVAV_EVVIT_Int, &mst_CurDes.st_Bmp.st_Header.biHeight, EVAV_ReadOnly );
        mpo_Parent->mpo_DataView->AddItem( "Has a font descriptor", EVAV_EVVIT_Flags, &mst_CurDes.uw_Flags, 0, 6, 2 );
        po_Item = mpo_Parent->mo_ListItems.GetTail();
        po_Item->mpfn_CB = ChangeFlags;
        if ( mst_CurDes.uw_Flags & TEX_FP_ThisIsAFont )
        {
            po_Item->mx_Color = 0xFF00;
            mst_CurDes.ul_FontDescKey = st_Tex.ul_Params[ 0 ];
            mpo_Parent->mpo_DataView->AddItem( "Font descriptor" , EVAV_EVVIT_Key, &mst_CurDes.ul_FontDescKey );
            po_Item = mpo_Parent->mo_ListItems.GetTail();
            po_Item->mpfn_CB = ChangeFontDesc;
        }
        break;

	case MAIEDITEX_C_TEX:
#if defined(_XENON_RENDER)
        mpo_Parent->mpo_DataView->AddItem("Basic Information", EVAV_EVVIT_Separator, NULL, EVAV_ReadOnly);
#endif
		for (i_Slot = 0; i_Slot < 4; i_Slot++)
		{
			sprintf( asz_Infos, "[%s] - Indexed",  ETEX_gsz_SlotName[ i_Slot ] );
			mpo_Parent->mpo_DataView->AddItem(asz_Infos, EVAV_EVVIT_Key, &mst_CurDes.st_Tex.ast_Slot[ i_Slot ].ul_Raw );
			po_Item = mpo_Parent->mo_ListItems.GetTail();
			po_Item->mpfn_CB = ChangeTexKey;
			sprintf( asz_Infos, "[%s] - Palette",  ETEX_gsz_SlotName[ i_Slot ] );
			mpo_Parent->mpo_DataView->AddItem( asz_Infos, EVAV_EVVIT_Key, &mst_CurDes.st_Tex.ast_Slot[ i_Slot ].ul_Pal );
			po_Item = mpo_Parent->mo_ListItems.GetTail();
			po_Item->mpfn_CB = ChangeTexKey;
			sprintf( asz_Infos, "[%s] - True Color",  ETEX_gsz_SlotName[ i_Slot ] );
			mpo_Parent->mpo_DataView->AddItem(asz_Infos, EVAV_EVVIT_Key, &mst_CurDes.st_Tex.ast_Slot[ i_Slot ].ul_TC );
			po_Item = mpo_Parent->mo_ListItems.GetTail();
			po_Item->mpfn_CB = ChangeTexKey;
		}
#if defined(_XENON_RENDER)
        // Xenon extended information
        {
            TEX_tdst_XenonFileTex* pst_XeProperties = &mst_CurDes.st_Tex.st_XeProperties;

            mpo_Parent->mpo_DataView->AddItem("Xenon Information", EVAV_EVVIT_Separator, NULL, EVAV_ReadOnly);

            // Original texture key
            mpo_Parent->mpo_DataView->AddItem("Source Texture", EVAV_EVVIT_Key, 
                                              &pst_XeProperties->ul_OriginalTexture, 0, 0, (int)XE_TEXTURE_EXT);
            po_Item = mpo_Parent->mo_ListItems.GetTail();
            po_Item->mpfn_CB = Xe_ChangeSourceTexture;

            // Native texture
            mpo_Parent->mpo_DataView->AddItem("Native Texture", EVAV_EVVIT_Key, 
                                              &pst_XeProperties->ul_NativeTexture, 0, 0, (int)XE_TEXTURE_NATIVE_EXT);
            po_Item = mpo_Parent->mo_ListItems.GetTail();
            po_Item->mpfn_CB = Xe_ChangeNativeTexture;

            // Output options
            if ((mst_CurDes.st_Tex.st_XeProperties.ul_NativeTexture == 0) || 
                (mst_CurDes.st_Tex.st_XeProperties.ul_NativeTexture == BIG_C_InvalidKey))
            {
                // Generate button
                mpo_Parent->mpo_DataView->AddItem("Generate", EVAV_EVVIT_Custom, &s_ul_Unused, EVAV_None, 0, 0, 4);
                po_Item = mpo_Parent->mo_ListItems.GetTail();
                po_Item->mpfn_CB = Xe_OnGenerate;

                // Flags
                {
                    // Is a normal map
                    mpo_Parent->mpo_DataView->AddItem("Normal Map", EVAV_EVVIT_Flags, &pst_XeProperties->ul_Flags, 0, 0, 4 );
                    po_Item = mpo_Parent->mo_ListItems.GetTail();
                    po_Item->mpfn_CB = Xe_ChangeFlags;
                }

                // Output width
                mpo_Parent->mpo_DataView->AddItem("Output Width", EVAV_EVVIT_ConstInt,
                                                  &pst_XeProperties->ul_OutputWidth, 0, 0, 0, 4, NULL, 
                                                  (int)XE_TEXTURE_SIZE);
                po_Item = mpo_Parent->mo_ListItems.GetTail();
                po_Item->mpfn_CB = Xe_ChangeOutputWidth;

                // Output height
                mpo_Parent->mpo_DataView->AddItem("Output Height", EVAV_EVVIT_ConstInt,
                                                  &pst_XeProperties->ul_OutputHeight, 0, 0, 0, 4, NULL, 
                                                  (int)XE_TEXTURE_SIZE);
                po_Item = mpo_Parent->mo_ListItems.GetTail();
                po_Item->mpfn_CB = Xe_ChangeOutputHeight;

                // Output format
                mpo_Parent->mpo_DataView->AddItem("Output Format", EVAV_EVVIT_ConstInt,
                                                  &pst_XeProperties->ul_OutputFormat, 0, 0, 0, 4, NULL, 
                                                  (int)XE_TEXTURE_FORMAT);
                po_Item = mpo_Parent->mo_ListItems.GetTail();
                po_Item->mpfn_CB = Xe_ChangeOutputFormat;

                // Number of mip maps
                ULONG ul_MaxMipMaps = TEX_ul_XeGetMaxMipMapsCount(pst_XeProperties->ul_OutputWidth,
                                                                  pst_XeProperties->ul_OutputHeight);
                ETEX_BuildMipMapString(ul_MaxMipMaps);
                mpo_Parent->mpo_DataView->AddItem("Mip Maps", EVAV_EVVIT_ConstInt,
                                                  &pst_XeProperties->ul_NbLevels, 0, 0, 0, 4, NULL, 
                                                  (int)XE_TEXTURE_MIPMAPS);
                po_Item = mpo_Parent->mo_ListItems.GetTail();
                po_Item->mpfn_CB = Xe_ChangeOutputMipMaps;
            }
            else
            {
                // Output format
                mpo_Parent->mpo_DataView->AddItem("Output Format", EVAV_EVVIT_ConstInt,
                                                  &pst_XeProperties->ul_OutputFormat, EVAV_ReadOnly, 0, 0, 4, NULL, 
                                                  (int)XE_TEXTURE_FORMAT);
                po_Item = mpo_Parent->mo_ListItems.GetTail();
                po_Item->mpfn_CB = Xe_ChangeOutputFormat;
            }
        }
#endif
		break;
	}

	mpo_Parent->mpo_DataView->SetItemList( &mpo_Parent->mo_ListItems );

	/* dialog bar info */
	/* path & name */
    BIG_ComputeFullName( BIG_ParentFile( mst_CurDes.ul_FatFile ), asz_Infos );
    mpo_DialogBar->GetDlgItem(IDC_FILE)->SetWindowText(BIG_NameFile(mst_CurDes.ul_FatFile));
    mpo_DialogBar->GetDlgItem(IDC_PATH)->SetWindowText( asz_Infos );

    /* Infos */
    if (mst_CurDes.i_Type == MAIEDITEX_C_PALETTE)
    {
		sprintf( asz_Infos, "Palette %d colors, %salpha", (mst_CurDes.st_Tex.ast_Slot[0].ul_Pal & TEX_uc_Palette16) ? 16 : 256,  (mst_CurDes.st_Tex.ast_Slot[0].ul_Pal & TEX_uc_AlphaPalette) ? "" : "no " );
	}
    else if (mst_CurDes.i_Type == MAIEDITEX_C_TEX)
    {
		asz_Infos[0] = 0;
	}
    else
    {
		sprintf( asz_Infos, "%d x %d x %d %s", mst_CurDes.st_Bmp.st_Header.biWidth, mst_CurDes.st_Bmp.st_Header.biHeight, mst_CurDes.st_Bmp.st_Header.biBitCount, mst_CurDes.st_Bmp.b_Compress ? "(Compressed)" : "" );
	}
    if(mst_CurDes.st_Bmp.h_BitmapAlpha && mb_AlphaShow) 
		sprintf(asz_Infos, "%s (Alpha Channel)", asz_Infos);
    mpo_DialogBar->GetDlgItem(IDC_INFOS)->SetWindowText(asz_Infos);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_InsideScroll::CreateTga( MAIEDITEX_tdst_BitmapDes *_pst_BD, int _i_Slot, int _i_Palette )
{
	TEX_tdst_Palette    st_Pal;
	int					i_Slot, i_SlotFirst, i_SlotLast;
	ULONG               W, H, ul_Size, *pul_Palette, *pul_OldColor;
    TEX_tdst_File_Desc  st_Desc, st_TgaDesc;
    int                 i, j, k;
    char                sz_Name[ BIG_C_MaxLenName ], sz_Path[ BIG_C_MaxLenPath ];
	char				sz_Text[ 256 ];
    UCHAR               *puc_Bmp;

	if (_i_Slot == -1 )
	{
		i_SlotFirst = 0;
		i_SlotLast = 4;
	}
	else
	{
		i_SlotFirst = _i_Slot;
		i_SlotLast = _i_Slot+1;
	}

	BIG_ComputeFullName( BIG_ParentFile( _pst_BD->ul_FatFile ), sz_Path );

	for (i_Slot = i_SlotFirst; i_Slot < i_SlotLast; i_Slot++)
	{
		/* load palette */
		L_memset( &st_Pal, 0, sizeof( TEX_tdst_Palette ) );
		st_Pal.ul_Key = _pst_BD->st_Tex.ast_Slot[ i_Slot ].ul_Pal;
		if ( st_Pal.ul_Key == BIG_C_InvalidKey ) continue;
		TEX_File_LoadPalette( &st_Pal, FALSE );
		if (st_Pal.uc_Flags & TEX_uc_InvalidPalette) continue;
		pul_Palette = 0;

	    /* check name */
	    strcpy( sz_Name, BIG_NameFile( _pst_BD->ul_FatFile ) );
		if ( strchr( sz_Name, '.')  ) *strchr( sz_Name, '.') = 0;
		if (i_Slot != 0)
			strcat( sz_Name, ETEX_gsz_SlotName[ i_Slot ] );
	    if ( _i_Palette )
			sprintf(sz_Name + strlen( sz_Name ), "_%dbits.tga", (st_Pal.uc_Flags & TEX_uc_Palette16) ? 4 : 8 );
		else
			strcat( sz_Name, ".tga" );
                    
		if ( BIG_ul_SearchFile( BIG_ParentFile(_pst_BD->ul_FatFile), sz_Name ) != BIG_C_InvalidKey)
		{
			sprintf( sz_Name, "File (%s) already exist", sz_Name );
			ERR_X_Warning( 0, sz_Text, NULL );
			TEX_M_File_Free( st_Pal.pul_Color ); 
			continue;
		}	

		/* load raw */
		TEX_l_File_GetInfoAndContent( _pst_BD->st_Tex.ast_Slot[ i_Slot ].ul_Raw, &st_Desc );
		W = st_Desc.st_Params.uw_Width;
        H = st_Desc.st_Params.uw_Height;
        ul_Size = W * H;

        /* prepare tga descriptor */
        L_memset( &st_TgaDesc, 0, sizeof( TEX_tdst_File_Desc ) );
        TEX_File_InitParams( &st_TgaDesc.st_Params );
        st_TgaDesc.uw_Width = st_Desc.st_Params.uw_Width;
        st_TgaDesc.uw_Height = st_Desc.st_Params.uw_Height;

        st_TgaDesc.p_Bitmap = MEM_p_Alloc( ul_Size * 4);

		if ( _i_Palette )
		{
			if (st_Pal.uc_Flags & TEX_uc_Palette16)
			{
				TEX_Convert_4To8( (UCHAR *) st_TgaDesc.p_Bitmap, (UCHAR *) st_Desc.p_Bitmap, W, H );
                i = W * H;
                puc_Bmp = (UCHAR *) st_TgaDesc.p_Bitmap;
                while ( i-- )
                {
					*puc_Bmp = 255 - *puc_Bmp; 
                    puc_Bmp++;
				}
                pul_Palette = (ULONG *) L_malloc( 256 * 4 );
                for (i=0; i < 16; i++)
					pul_Palette[ 255 - i ] = st_Pal.pul_Color[ i ];
                L_memset( pul_Palette, 0, 240 * 4 );
                pul_OldColor = st_Pal.pul_Color;
                st_Pal.pul_Color = pul_Palette;
			}
            else
            {
				L_memcpy( (UCHAR *) st_TgaDesc.p_Bitmap, (UCHAR *) st_Desc.p_Bitmap, ul_Size );
			}
			i = 256;
            j = k = 0;
            while( i-- )
            {
				((char *) st_Pal.pul_Color)[j++] = ((char *) st_Pal.pul_Color)[k++];
                ((char *) st_Pal.pul_Color)[j++] = ((char *) st_Pal.pul_Color)[k++];
                ((char *) st_Pal.pul_Color)[j++] = ((char *) st_Pal.pul_Color)[k++];
                k++;
			}
            st_TgaDesc.uc_BPP = 8;
            st_TgaDesc.p_Palette = st_Pal.pul_Color;
            st_TgaDesc.uc_PaletteBPC = 24;
            st_TgaDesc.uw_PaletteLength = 256;
		}
		else
		{
			if (st_Pal.uc_Flags & TEX_uc_AlphaPalette)
            {
				st_TgaDesc.uc_BPP = 32;
                if (st_Desc.uc_BPP == 4)
					TEX_Convert_4To32( (ULONG *) st_TgaDesc.p_Bitmap, (UCHAR *) st_Desc.p_Bitmap, st_Pal.pul_Color, ul_Size );
                else
					TEX_Convert_8To32( (ULONG *) st_TgaDesc.p_Bitmap, (UCHAR *) st_Desc.p_Bitmap, st_Pal.pul_Color, ul_Size );
			}
            else
            {
				st_TgaDesc.uc_BPP = 24;
                if (st_Desc.uc_BPP == 4)
					TEX_Convert_4To24( (UCHAR *) st_TgaDesc.p_Bitmap, (UCHAR *) st_Desc.p_Bitmap, st_Pal.pul_Color, W, H, 0);
                else
					TEX_Convert_8To24( (UCHAR *) st_TgaDesc.p_Bitmap, (UCHAR *) st_Desc.p_Bitmap, st_Pal.pul_Color, W, H, 0);
			}
		}
        
		ul_Size = TEX_ul_File_SaveTgaInBF( sz_Path, sz_Name, &st_TgaDesc );

        sprintf( sz_Text, "[%08X] %s has been created in directory", BIG_FileKey( ul_Size ), sz_Name );
        LINK_PrintStatusMsg( sz_Text );
        sprintf( sz_Text, ".            %s", sz_Path );
        LINK_PrintStatusMsg( sz_Text );
                    
        // Free
        if ( pul_Palette ) 
        {
			L_free( pul_Palette );
            st_Pal.pul_Color = pul_OldColor;
		}
        MEM_Free( st_TgaDesc.p_Bitmap );
        TEX_M_File_Free( st_Pal.pul_Color ); 
        TEX_File_FreeDescription( &st_Desc );

    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEX_cl_InsideScroll::ImportAlphaMask( MAIEDITEX_tdst_BitmapDes *_pst_BD, int _i_Slot )
{
	TEX_tdst_Palette    st_Pal;
	ULONG               W, H, ul_Size, ul_Key, ul_Index;
    TEX_tdst_File_Desc  st_RawDesc, st_TgaDesc;
    int                 i, j, k, i_Already8;
    char                sz_Path[ BIG_C_MaxLenPath ], sz_Text[ 256 ];
    EDIA_cl_FileDialog	*po_FileDlg;
    FILE				*hp_File;
    CString				o_FileName;
    char				*sz_FileName, *sz_Ext;
    char				c_UsedColor[ 256 ];
    unsigned long		ul_Alpha[ 16 ];
    ULONG				*pul_Color,  *pul_OldColor;
    UCHAR				*puc_Buffer, *puc_OldRaw, *puc_NewRaw, *puc_CurMask, *puc_CurNew;

	if (_i_Slot == -1 ) return;
	
	/* init var */
	i_Already8 = 0;
	
	/* init allocated pointer for deallocating */
	*sz_Text = 0;
	st_Pal.pul_Color = NULL;
	L_memset( &st_RawDesc, 0, sizeof( TEX_tdst_File_Desc ) );
	puc_Buffer = NULL;
	

	/* load palette */
	L_memset( &st_Pal, 0, sizeof( TEX_tdst_Palette ) );
	st_Pal.ul_Key = _pst_BD->st_Tex.ast_Slot[ _i_Slot ].ul_Pal;
	if ( st_Pal.ul_Key == BIG_C_InvalidKey ) 
	{
		sprintf( sz_Text, "No palette found" );	
		goto ImportAlphaMask_EndError;
	}
	TEX_File_LoadPalette( &st_Pal, FALSE );
		
	/* check compatibility of palette */
	if (st_Pal.uc_Flags & TEX_uc_InvalidPalette)
	{
		sprintf( sz_Text, "Palette [%08X] is invalid", st_Pal.ul_Key );
		goto ImportAlphaMask_EndError;
	}
	
	/* if palette with alpha and 8 bit, it's perhaps importation of a mask */
	if ( !(st_Pal.uc_Flags & TEX_uc_Palette16) && (st_Pal.uc_Flags & TEX_uc_AlphaPalette) )
	{
		sprintf( sz_Text, "Palette [%08X] is already an alpha - 8bit - palette\n"\
							"click ok to continue and assume that this palette\n"\
							"was created when previously importing a mask", st_Pal.ul_Key );
		if ( M_MF()->MessageBox( sz_Text, "Warning", MB_ICONQUESTION | MB_YESNO ) == IDNO )
			goto ImportAlphaMask_End;
		i_Already8 = 1;
	}
	else if ( !st_Pal.uc_Flags & TEX_uc_Palette16 )
	{
		sprintf( sz_Text, "Operation can only be made with 4bit palette" );
		goto ImportAlphaMask_EndError;
	}
	else if (st_Pal.uc_Flags & TEX_uc_AlphaPalette)
	{
		sprintf( sz_Text, "Operation can only be made with palette with no alpha" );
		goto ImportAlphaMask_EndError;
	}
		
	/* load raw */
	if ( !TEX_l_File_GetInfoAndContent( _pst_BD->st_Tex.ast_Slot[ _i_Slot ].ul_Raw, &st_RawDesc ) )
	{
		sprintf( sz_Text, "Can not load raw data [%08X]", _pst_BD->st_Tex.ast_Slot[ _i_Slot ].ul_Raw );
		goto ImportAlphaMask_EndError;
	}
	
	W = st_RawDesc.st_Params.uw_Width;
    H = st_RawDesc.st_Params.uw_Height;
    
    /* choose file to import as mask */
    po_FileDlg = new EDIA_cl_FileDialog("Choose texture mask", 3, TRUE, FALSE, NULL);
    if( po_FileDlg->DoModal() != IDOK)
		goto ImportAlphaMask_End;
		
	/* get filename */
	po_FileDlg->GetItem(po_FileDlg->mo_File, 1, o_FileName);
	sz_FileName = (char *) (LPCTSTR) o_FileName;
	sprintf( sz_Path, "%s/%s", po_FileDlg->masz_FullPath, sz_FileName );	
	delete po_FileDlg;
	
	/* check name */
	if( !BIG_b_CheckName( sz_FileName ) )
	{
		sprintf( sz_Text, "(%s) is a bad name", (char *) (LPCTSTR) o_FileName );
		goto ImportAlphaMask_EndError;
	}
	
	/* check extension */
	sz_Ext = strrchr( sz_FileName, '.' );
	if (!sz_Ext || L_stricmp( sz_Ext + 1, "tga" ) )
	{
		sprintf( sz_Text, "mask file has to be a tga file (with palette and using only 16 levels of grey)" );
		goto ImportAlphaMask_EndError;
	}
	
	/* open file */
	hp_File = L_fopen( sz_Path, "rb" );
	if ( !hp_File ) 
	{
		sprintf( sz_Text, "error opening file (%s)", sz_Path );
		goto ImportAlphaMask_EndError;
	}
	
	/* get file size */
	L_fseek(  hp_File, 0, SEEK_END );
	ul_Size = L_ftell( hp_File );
	L_fseek(  hp_File, 0, SEEK_SET );
	if (ul_Size == 0)
	{
		sprintf( sz_Text, "file is empty or access error (%s)", sz_Path );
		goto ImportAlphaMask_EndError;
	}
	
	/* get content of file */
	puc_Buffer = (unsigned char *) L_malloc( ul_Size );
	L_fread( puc_Buffer, ul_Size, 1, hp_File );
	L_fclose(hp_File);
	L_memset(&st_TgaDesc, 0, sizeof(TEX_tdst_File_Desc));
	st_TgaDesc.uw_DescFlags = TEX_Cuw_DF_Info;
	if ( !TEX_l_File_LoadTga( (char *) puc_Buffer, &st_TgaDesc) )
	{
		sprintf( sz_Text, "error loading tga data" );
		goto ImportAlphaMask_EndError;
	}
		
	/* check size */
	if ( (st_TgaDesc.uw_Height != H ) || (st_TgaDesc.uw_Width != W ) )
	{
		sprintf( sz_Text, "original raw (%d,%d) and mask (%d,%d) does have the same size" , W, H, st_TgaDesc.uw_Width, st_TgaDesc.uw_Height);
		goto ImportAlphaMask_EndError;
	}
		
	/* check for a palette */
	if ( st_TgaDesc.uc_BPP != 8 )
	{
		sprintf( sz_Text, "no palette found in mask tga file" );
		goto ImportAlphaMask_EndError;
	}
	
	/* check number of color */
	L_memset( c_UsedColor, 0, 256 );
	puc_NewRaw = (unsigned char *) st_TgaDesc.p_TmpBitmap;
	ul_Size = W * H;
	while( ul_Size-- )
		c_UsedColor[ *puc_NewRaw++ ] = 1;
		
	j = 0;
	for (i = 255; i >= 0; i-- )
	{
		if ( c_UsedColor[ i ] )
		{
			if (j == 16)
			{
				sprintf( sz_Text, "more than 16 color used in mask tga file" );
				goto ImportAlphaMask_EndError;
			}
			c_UsedColor[ i ] = j;
			ul_Alpha[ j ] = i;
			j++;
		} 
	}
	
	/* get alpha level */
	puc_NewRaw = (unsigned char *) st_TgaDesc.p_TmpPalette;
	for (i = 0; i < j; i++)
		ul_Alpha[ i ] = puc_NewRaw[ ul_Alpha[ i ] * 3 ] << 24;
	for ( ; j < 16; j++)
		ul_Alpha[ j ] = 0;
	
	/* build new palette */
	pul_Color = (ULONG *) L_malloc( 256 * sizeof( ULONG ) );
	k = 0;
	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 16; j++)
		{
			pul_Color[ k ] = (st_Pal.pul_Color[ j ] & 0xFFFFFF) | ul_Alpha[ i ];
			k++;
		}
	}
	
	/* save palette */
	pul_OldColor = st_Pal.pul_Color;
	st_Pal.pul_Color = pul_Color;
	st_Pal.uc_BitPerColor = 32;
	st_Pal.uc_Flags = TEX_uc_AlphaPalette;
	TEX_ul_File_SavePaletteBF( &st_Pal, 0 );
	st_Pal.pul_Color = pul_OldColor;
	L_free( pul_Color );
	
	// convert old raw from 4 to 8 bits
	ul_Size = W * H;
	if ( i_Already8 )
	{
		puc_OldRaw = (UCHAR*) st_RawDesc.p_Bitmap;
		while ( ul_Size-- )
			*puc_OldRaw++ &= 0xF;
		ul_Size = W * H;
	}
	else
	{
		TEX_M_File_Alloc( puc_OldRaw, ul_Size, UCHAR )
		TEX_Convert_4To8( puc_OldRaw, (UCHAR*) st_RawDesc.p_Bitmap, W, H );
		TEX_M_File_Free( st_RawDesc.p_Bitmap );
		st_RawDesc.p_Bitmap = puc_OldRaw;
	}
	
	// build new raw
	puc_OldRaw = (UCHAR*) st_RawDesc.p_Bitmap;
	puc_CurMask = (UCHAR *) st_TgaDesc.p_TmpBitmap;
	TEX_M_File_Alloc( puc_NewRaw, ul_Size, UCHAR );
	puc_CurNew = puc_NewRaw;
	while( ul_Size-- )
	{
		*puc_CurNew = *puc_OldRaw++;
		*puc_CurNew |= c_UsedColor[ *puc_CurMask++ ] << 4;
		puc_CurNew++;
	}
	
	// save raw
	ul_Key = _pst_BD->st_Tex.ast_Slot[ _i_Slot ].ul_Raw;
	ul_Index = BIG_ul_SearchKeyToFat( ul_Key );
	
	puc_OldRaw = (UCHAR*) st_RawDesc.p_Bitmap;
	st_RawDesc.p_Bitmap = puc_NewRaw;
	st_RawDesc.uc_BPP = 8;
	
	BIG_ComputeFullName( BIG_ParentFile( ul_Index  ), sz_Path );
	TEX_ul_File_SaveRawInBF( sz_Path, BIG_NameFile( ul_Index ), &st_RawDesc, 1 );
	
	TEX_M_File_Free( puc_NewRaw );
	st_RawDesc.p_Bitmap = puc_OldRaw;
	goto ImportAlphaMask_End;
	
	
	
ImportAlphaMask_EndError:
	/* display message */
	M_MF()->MessageBox( sz_Text, "Error", MB_OK | MB_ICONSTOP);
	
ImportAlphaMask_End:

	/* deallocating */
	if ( st_Pal.pul_Color )		TEX_M_File_Free( st_Pal.pul_Color );
	if ( st_RawDesc.p_Bitmap)	TEX_File_FreeDescription( &st_RawDesc );
	if ( puc_Buffer )			L_free( puc_Buffer );
}
#if defined(_XENON_RENDER)

static void Xe_ChangeSourceTexture(void* _p_Owner, void* _p_Item, void* _p_Data, long _l_Old)
{
    ETEX_cl_InsideScroll* po_Editor;
    EVAV_cl_ViewItem*     po_Item;

    po_Item   = (EVAV_cl_ViewItem*)_p_Item;
    po_Editor = ((ETEX_cl_Frame*)_p_Owner)->mpo_ScrollView;

    if ((_l_Old == 0) || (_l_Old == *(long*)po_Item->mp_Data))
        return;

    TEX_ul_File_SaveTexWithIndex(po_Editor->mst_CurDes.ul_FatFile, &po_Editor->mst_CurDes.st_Tex);
    po_Editor->mpo_Parent->Browse();
}

static void Xe_ChangeNativeTexture(void* _p_Owner, void* _p_Item, void* _p_Data, long _l_Old)
{
    ETEX_cl_InsideScroll* po_Editor;
    EVAV_cl_ViewItem*     po_Item;

    po_Item   = (EVAV_cl_ViewItem*)_p_Item;
    po_Editor = ((ETEX_cl_Frame*)_p_Owner)->mpo_ScrollView;

    if ((_l_Old == 0) || (_l_Old == *(long*)po_Item->mp_Data))
        return;

    TEX_ul_File_SaveTexWithIndex(po_Editor->mst_CurDes.ul_FatFile, &po_Editor->mst_CurDes.st_Tex);
    po_Editor->mpo_Parent->Browse();

    po_Editor->UpdateInfo();
}

static void Xe_ChangeFlags(void* _p_Owner, void* _p_Item, void* _p_Data, long _l_Old)
{
    ETEX_cl_InsideScroll* po_Editor;
    EVAV_cl_ViewItem*     po_Item;

    po_Item   = (EVAV_cl_ViewItem*)_p_Item;
    po_Editor = ((ETEX_cl_Frame*)_p_Owner)->mpo_ScrollView;

    TEX_ul_File_SaveTexWithIndex(po_Editor->mst_CurDes.ul_FatFile, &po_Editor->mst_CurDes.st_Tex);
    po_Editor->mpo_Parent->Browse();
}

static void Xe_OnGenerate(void* _p_Owner, void* _p_Item, void* _p_Data, long _l_Old)
{
    ETEX_cl_InsideScroll* po_Editor;
    EVAV_cl_ViewItem*     po_Item;
    ULONG ulFileIndex;
    CHAR  szNewFileName[256];
    CHAR  szDirName[L_MAX_PATH];
    CHAR* pcPos;
    long  lRes;

    if (!GDI_b_IsXenonGraphics())
        return;

    po_Item   = (EVAV_cl_ViewItem*)_p_Item;
    po_Editor = ((ETEX_cl_Frame*)_p_Owner)->mpo_ScrollView;

    strcpy(szNewFileName, BIG_NameFile(po_Editor->mst_CurDes.ul_FatFile));

    pcPos = strrchr(szNewFileName, '.');
    if (pcPos != NULL)
        *pcPos = '\0';

    strcat(szNewFileName, "_Xenon.dds");

    // Make sure the file does not exist, or prompt for overwrite
    ulFileIndex = BIG_ul_SearchFile(po_Editor->mul_DirIndex, szNewFileName);
    if (ulFileIndex != BIG_C_InvalidIndex)
    {
        CHAR szMessage[256];
        sprintf(szMessage, "File '%s' already exists. Do you want to overwrite it?", szNewFileName);
        EDIA_cl_MessageDialog oMsgBox(szMessage, "Xenon Texture Generation", MB_YESNO | MB_TASKMODAL | MB_ICONQUESTION);
        lRes = oMsgBox.DoModal();
        if (lRes != IDYES)
            return;
    }
    else
    {
        CHAR szMessage[256];
        sprintf(szMessage, "Generate %s?", szNewFileName);
        EDIA_cl_MessageDialog oMsgBox(szMessage, "Xenon Texture Generation", MB_YESNO | MB_TASKMODAL | MB_ICONQUESTION);
        lRes = oMsgBox.DoModal();
        if (lRes != IDYES)
            return;
    }

    BIG_ComputeFullName(po_Editor->mul_DirIndex, szDirName);

    if (TEX_XeGenerateDDS(&po_Editor->mst_CurDes.st_Tex.st_XeProperties, szDirName, szNewFileName))
    {
        TEX_ul_File_SaveTexWithIndex(po_Editor->mst_CurDes.ul_FatFile, &po_Editor->mst_CurDes.st_Tex);
        po_Editor->mpo_Parent->Browse();
        po_Editor->UpdateInfo();
    }
}

static void Xe_ChangeOutputWidth(void* _p_Owner, void* _p_Item, void* _p_Data, long _l_Old)
{
    ETEX_cl_InsideScroll* po_Editor;
    EVAV_cl_ViewItem*     po_Item;

    po_Item   = (EVAV_cl_ViewItem*)_p_Item;
    po_Editor = ((ETEX_cl_Frame*)_p_Owner)->mpo_ScrollView;

    TEX_ul_File_SaveTexWithIndex(po_Editor->mst_CurDes.ul_FatFile, &po_Editor->mst_CurDes.st_Tex);
    po_Editor->mpo_Parent->Browse();

    po_Editor->UpdateInfo();
}

static void Xe_ChangeOutputHeight(void* _p_Owner, void* _p_Item, void* _p_Data, long _l_Old)
{
    ETEX_cl_InsideScroll* po_Editor;
    EVAV_cl_ViewItem*     po_Item;

    po_Item   = (EVAV_cl_ViewItem*)_p_Item;
    po_Editor = ((ETEX_cl_Frame*)_p_Owner)->mpo_ScrollView;

    TEX_ul_File_SaveTexWithIndex(po_Editor->mst_CurDes.ul_FatFile, &po_Editor->mst_CurDes.st_Tex);
    po_Editor->mpo_Parent->Browse();

    po_Editor->UpdateInfo();
}

static void Xe_ChangeOutputFormat(void* _p_Owner, void* _p_Item, void* _p_Data, long _l_Old)
{
    ETEX_cl_InsideScroll* po_Editor;
    EVAV_cl_ViewItem*     po_Item;

    po_Item   = (EVAV_cl_ViewItem*)_p_Item;
    po_Editor = ((ETEX_cl_Frame*)_p_Owner)->mpo_ScrollView;

    TEX_ul_File_SaveTexWithIndex(po_Editor->mst_CurDes.ul_FatFile, &po_Editor->mst_CurDes.st_Tex);
    po_Editor->mpo_Parent->Browse();

    po_Editor->UpdateInfo();
}

static void Xe_ChangeOutputMipMaps(void* _p_Owner, void* _p_Item, void* _p_Data, long _l_Old)
{
    ETEX_cl_InsideScroll* po_Editor;
    EVAV_cl_ViewItem*     po_Item;

    po_Item   = (EVAV_cl_ViewItem*)_p_Item;
    po_Editor = ((ETEX_cl_Frame*)_p_Owner)->mpo_ScrollView;

    TEX_ul_File_SaveTexWithIndex(po_Editor->mst_CurDes.ul_FatFile, &po_Editor->mst_CurDes.st_Tex);
    po_Editor->mpo_Parent->Browse();

    po_Editor->UpdateInfo();
}

#endif
#endif /* ACTIVE_EDITORS */




 

