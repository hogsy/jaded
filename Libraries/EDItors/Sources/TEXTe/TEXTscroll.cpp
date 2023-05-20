/*$T TEXTscroll.cpp GC 1.138 09/07/04 17:19:54 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "TEXTframe.h"
#include "TEXTscroll.h"
#include "EDImainframe.h"
#include "EDIpaths.h"
#include "ENGine/Sources/TEXT/TEXTstruct.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"
#include "DIAlogs/DIAname_dlg.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "EDItors/Sources/SOuNd/SONframe.h"
#include "EDIeditors_infos.h"
#include "EDImsg.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "SouND/Sources/SNDstruct.h"

#define OFFSET				5
#define OFFSET1				25

#define CAPTURE_SIZECOL		1
#define CAPTURE_PICKHEADER	2
#define CAPTURE_MOVEHEADER	3
#define CAPTURE_TIRETTE		4
#define CAPTURE_PICKRANK	5
#define CAPTURE_MOVERANK	6

/*
 =======================================================================================================================
    little function to count number of lines into a text
 =======================================================================================================================
 */
int ETEXT_CountLine(char *_sz_Text)
{
	/*~~~~~~~~~~*/
	int num_lines;
	/*~~~~~~~~~~*/

	num_lines = 1;

	while(1)
	{
		_sz_Text = strchr(_sz_Text, '\n');
		if(!_sz_Text) return num_lines;
		num_lines++;
		_sz_Text++;
	}
}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(ETEXT_cl_Scroll, CScrollView)
BEGIN_MESSAGE_MAP(ETEXT_cl_Scroll, CScrollView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_EN_CHANGE(10, OnEditChange)
	ON_EN_KILLFOCUS(10, OnLooseFocus)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEWHEEL()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ETEXT_cl_Scroll::ETEXT_cl_Scroll(void)
{
	/*~~~~~~~~~*/
	ULONG	C, i;
	/*~~~~~~~~~*/

	mpo_Parent = NULL;
	mb_EditContent = FALSE;
	mb_InMenu = FALSE;
	mi_HitTest = 0;
	mi_Capture = 0;
	Column_Nb = 0;
	mi_NumSel = -1;
	mi_OrderColumn = 0;
	mi_OrderLesserToGreater = TRUE;
	mi_LastEntrySel = -1;
	mb_RankMoving = FALSE;
	mi_HitTest_Line = -1;

	mul_BackColor = GetSysColor(COLOR_WINDOW);

	/*
	 * ul_Color[ 2 ][ 6 ] = { { 0x9098A8, 0xC8D0D8, 0, 0xC0C0C0, 0xA0A0A0, 0xFF8080},
	 * { 0x505868, 0,0xC8D0D8, 0x808080, 0x606060, 0xBF4040} };
	 */
	maul_Color[0][ETEXT_COLOR_RankBack] = GetSysColor(COLOR_3DFACE);
	maul_Color[0][ETEXT_COLOR_RankLight] = GetSysColor(COLOR_3DHILIGHT);
	maul_Color[0][ETEXT_COLOR_RankDark] = GetSysColor(COLOR_3DSHADOW);
	maul_Color[0][ETEXT_COLOR_Col0] = M_MF()->u4_Interpol2PackedColor(GetSysColor(COLOR_WINDOW), 0, 0.03f);
	maul_Color[0][ETEXT_COLOR_Col1] = M_MF()->u4_Interpol2PackedColor(GetSysColor(COLOR_WINDOW), 0xFFFFFF, 0.1f);
	maul_Color[0][ETEXT_COLOR_ColSort0] = M_MF()->u4_Interpol2PackedColor(maul_Color[0][ETEXT_COLOR_Col0], 0, 0.1f);
	maul_Color[0][ETEXT_COLOR_ColSort1] = M_MF()->u4_Interpol2PackedColor(maul_Color[0][ETEXT_COLOR_Col1], 0, 0.1f);
	maul_Color[0][ETEXT_COLOR_ColFind0] = M_MF()->u4_Interpol2PackedColor
		(
			maul_Color[0][ETEXT_COLOR_Col0],
			GetSysColor(COLOR_ACTIVECAPTION),
			0.3f
		);
	maul_Color[0][ETEXT_COLOR_ColFind1] = M_MF()->u4_Interpol2PackedColor
		(
			maul_Color[0][ETEXT_COLOR_Col1],
			GetSysColor(COLOR_ACTIVECAPTION),
			0.3f
		);
	maul_Color[0][ETEXT_COLOR_ColFindCur] = M_MF()->u4_Interpol2PackedColor
		(
			GetSysColor(COLOR_3DHILIGHT),
			GetSysColor(COLOR_ACTIVECAPTION),
			0.5f
		);

	for(i = 0; i < ETEXT_COLOR_Nb; i++)
	{
		maul_Color[1][i] = M_MF()->u4_Interpol2PackedColor(maul_Color[0][i], GetSysColor(COLOR_HIGHLIGHT), 0.1f);
	}

	C = maul_Color[1][ETEXT_COLOR_RankDark];
	maul_Color[1][ETEXT_COLOR_RankDark] = maul_Color[1][ETEXT_COLOR_RankLight];
	maul_Color[1][ETEXT_COLOR_RankLight] = C;

	mo_MoveHeaderPen.CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
	mo_DarkPen.CreatePen(PS_SOLID, 1, maul_Color[0][ETEXT_COLOR_RankDark]);
	mo_LightPen.CreatePen(PS_SOLID, 1, maul_Color[0][ETEXT_COLOR_RankLight]);

	mo_SelBrush.CreateSolidBrush(GetSysColor(COLOR_ACTIVECAPTION));

	/* arrow for sorting down */
	mao_Arrow[0] = CPoint(4, 4);
	mao_Arrow[1] = CPoint(12, 4);
	mao_Arrow[2] = CPoint(8, 12);

	/* arrow for sorting up */
	mao_Arrow[3] = CPoint(8, 4);
	mao_Arrow[4] = CPoint(12, 12);
	mao_Arrow[5] = CPoint(4, 12);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ETEXT_cl_Scroll::~ETEXT_cl_Scroll(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ETEXT_cl_Scroll::OnCreate(LPCREATESTRUCT _pst_CS)
{
	mo_Edit.Create
		(
			WS_BORDER | WS_CHILD | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN,
			CRect(0, 0, 0, 0),
			this,
			10
		);
	mo_Edit.SetFont(&M_MF()->mo_Fnt);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::OnKillFocus(CWnd *_po_Wnd)
{
	if(mi_Capture) ReleaseCapture();
	mi_Capture = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::OnDraw(CDC *pdc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect	o_Rect, o_CelRect;
	CString o_Id, o_Content, o_C;
	CPoint	o_Pos, po_Poly;
	CPen	*poldpen;
	int		i_Size, i_Num, iii, index, i_Odd;
	int		i_ColumnType, X, Y, YText, Column_ColorOffset;
	int		sel, searchres, Start, End;
	char	*sz_Name, *sz_CR;
	ULONG	C;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetClientRect(&o_Rect);
	if(mpo_Parent->mpt_EditText == NULL)
	{
		pdc->FillSolidRect(o_Rect, GetSysColor(COLOR_BTNFACE));
		EnableWindow(FALSE);
		return;
	}

	EnableWindow(TRUE);
	o_Pos = GetDeviceScrollPosition();

	pdc->SelectObject(&M_MF()->mo_Fnt);
	pdc->SetBkMode(TRANSPARENT);
	pdc->SetTextColor(0);

	/* computing column displayed */
	Start = o_Pos.x;
	End = o_Pos.x + o_Rect.Width();
	X = 0;
	Column_Nb = 0;
	Column_ColorOffset = 0;

	for(iii = 0; iii < COLMAX; iii++)
	{
		i_ColumnType = mpo_Parent->mst_Ini.ai_ColOrder[iii];
		if((i_Size = mpo_Parent->mst_Ini.ai_ColSize[i_ColumnType]) <= 0) continue;
		//if(X + i_Size > Start)
		{
			Column_X[Column_Nb] = X;
			Column[Column_Nb++] = iii;
		}

		X += i_Size;
		//if(X >= End) break;
		if(Column_Nb == 0) Column_ColorOffset++;
	}

	Column_X[Column_Nb] = X;

	/* display column header */
	pdc->FillSolidRect(o_Pos.x, o_Pos.y, o_Rect.Width(), 16, maul_Color[0][ETEXT_COLOR_RankBack]);
	pdc->Draw3dRect
		(
			Column_X[0],
			o_Pos.y,
			Column_X[Column_Nb],
			16,
			maul_Color[0][ETEXT_COLOR_RankLight],
			maul_Color[0][ETEXT_COLOR_RankDark]
		);
	pdc->SetTextAlign(TA_CENTER);

	o_CelRect.top = o_Pos.y;
	o_CelRect.bottom = o_Pos.y + 16;
	for(iii = 0; iii < Column_Nb; iii++)
	{
		i_ColumnType = mpo_Parent->mst_Ini.ai_ColOrder[Column[iii]];
		sz_Name = mpo_Parent->mst_Ini.asz_ColName[i_ColumnType];

		/* header */
		o_CelRect.left = Column_X[iii];
		o_CelRect.right = Column_X[iii + 1];

		if(iii)
		{
			poldpen = pdc->SelectObject(&mo_DarkPen);
			pdc->MoveTo(Column_X[iii] - 1, o_Pos.y + 3);
			pdc->LineTo(Column_X[iii] - 1, o_Pos.y + 13);
			pdc->SelectObject(&mo_LightPen);
			pdc->MoveTo(Column_X[iii], o_Pos.y + 3);
			pdc->LineTo(Column_X[iii], o_Pos.y + 13);
			pdc->SelectObject(poldpen);
		}

		pdc->ExtTextOut
			(
				Column_X[iii] + o_CelRect.Width() / 2,
				o_Pos.y + 2,
				ETO_CLIPPED,
				o_CelRect,
				sz_Name,
				strlen(sz_Name),
				NULL
			);

		/* sort arrow */
		if(mi_OrderColumn == i_ColumnType)
		{
			for(i_Num = 0; i_Num < 6; i_Num++) mao_Arrow[i_Num].Offset(Column_X[iii], o_Pos.y);
			pdc->Polygon(mao_Arrow + (mi_OrderLesserToGreater ? 0 : 3), 3);
			for(i_Num = 0; i_Num < 6; i_Num++) mao_Arrow[i_Num].Offset(-Column_X[iii], -o_Pos.y);
		}
	}

	/* display moving column header */
	if(mi_Capture == CAPTURE_MOVEHEADER)
	{
		/*~~~~~~~~~~~~~~~~~~~~~*/
		int dist, bestdist, best;
		/*~~~~~~~~~~~~~~~~~~~~~*/

		i_ColumnType = mpo_Parent->mst_Ini.ai_ColOrder[mi_MoveHeader_Column];
		sz_Name = mpo_Parent->mst_Ini.asz_ColName[i_ColumnType];

		o_CelRect.top += 2;
		o_CelRect.bottom -= 2;
		o_CelRect.left = mo_CapturePt.x - mi_MoveHeader_Offset;
		o_CelRect.right = o_CelRect.left + mi_MoveHeader_Size;

		pdc->Rectangle(&o_CelRect);
		pdc->ExtTextOut(o_CelRect.CenterPoint().x, o_Pos.y + 2, ETO_CLIPPED, o_CelRect, sz_Name, strlen(sz_Name), NULL);

		bestdist = 10000;
		best = -1;
		for(index = 0; index <= Column_Nb; index++)
		{
			dist = abs(o_CelRect.CenterPoint().x - Column_X[index]);
			if(dist < bestdist)
			{
				bestdist = dist;
				best = index;
			}
		}

		mi_MoveHeader_Drop = best;

		poldpen = pdc->SelectObject(&mo_MoveHeaderPen);
		if(best != -1)
		{
			pdc->MoveTo(Column_X[best] - 1, o_Pos.y);
			pdc->LineTo(Column_X[best] - 1, o_Pos.y + 16);
		}

		pdc->SelectObject(poldpen);
	}

	/* fill vertical part unused */
	X = Column_X[iii] - o_Pos.x;
	if(X < o_Rect.Width())
		pdc->FillSolidRect(Column_X[iii], o_Pos.y, o_Rect.Width() - X, o_Rect.Height() - 16, mul_BackColor);

	/* display all line */
	pdc->SetTextAlign(TA_LEFT);
	Start = o_Pos.y;
	End = o_Pos.y + o_Rect.Height() - 16;

	for(iii = 0; iii < (int) mpo_Parent->mpt_EditText->ul_Num; iii++)
	{
		Y = mpo_Parent->mpt_EditText->pst_Ids[iii].bottom;

		if(Y > Start)
		{
			o_CelRect.bottom = mpo_Parent->mpt_EditText->pst_Ids[iii].bottom + 16;
			o_CelRect.top = mpo_Parent->mpt_EditText->pst_Ids[iii].top + 16;
			if(o_CelRect.top - o_Pos.y < 16) o_CelRect.top = o_Pos.y + 16;
			index = mpo_Parent->mpt_EditText->pst_Ids[iii].order;
			sel = mpo_Parent->mpt_EditText->pst_Ids[index].sel ? 1 : 0;

			for(i_Num = 0; i_Num < Column_Nb; i_Num++)
			{
				YText = mpo_Parent->mpt_EditText->pst_Ids[iii].top + 18;
				o_CelRect.left = Column_X[i_Num];
				o_CelRect.right = Column_X[i_Num + 1];

				i_ColumnType = mpo_Parent->mst_Ini.ai_ColOrder[Column[i_Num]] + 1;
				sz_Name = mpo_Parent->CEL_GetText(index, i_ColumnType);

				if(i_ColumnType == mpo_Parent->en_Rank)
				{
					pdc->FillSolidRect(&o_CelRect, maul_Color[sel][ETEXT_COLOR_RankBack]);
				}
				else
				{
					i_Odd = ((Column_ColorOffset + i_Num) & 1);

					if((mi_OrderColumn + 1) == i_ColumnType)
						C = maul_Color[sel][ETEXT_COLOR_ColSort0 + i_Odd];
					else
						C = maul_Color[sel][ETEXT_COLOR_Col0 + i_Odd];

					if((mpo_Parent->mi_FindLine == index) && (mpo_Parent->mi_FindCol == i_ColumnType))
						C = maul_Color[sel][ETEXT_COLOR_ColFindCur];
					else if(searchres = mpo_Parent->mpt_EditText->pst_Ids[index].searchres)
					{
						if
						(
							((i_ColumnType == mpo_Parent->en_ID) && (searchres & TEXT_SearchResult_Name))
						||	((i_ColumnType == mpo_Parent->en_Preview) && (searchres & TEXT_SearchResult_Content))
                        ||	((i_ColumnType == mpo_Parent->en_IDKey) && (searchres & TEXT_SearchResult_EntryKey))
						) C = maul_Color[sel][ETEXT_COLOR_ColFind0 + i_Odd];
					}

					pdc->FillSolidRect(&o_CelRect, C);
				}

				while(1)
				{
					sz_CR = strchr(sz_Name, '\n');
					if(sz_CR) *sz_CR = 0;
					pdc->ExtTextOut(Column_X[i_Num] + 2, YText, ETO_CLIPPED, o_CelRect, sz_Name, strlen(sz_Name), NULL);
					if(!sz_CR) break;
					*sz_CR = '\n';
					sz_Name = sz_CR + 1;
					YText += mo_SizeFnt.cy;
				}
			}

			if(Y > 16)
			{
				poldpen = pdc->SelectObject(&mo_DarkPen);
				pdc->MoveTo(o_Pos.x, Y + 15);
				pdc->LineTo(Column_X[Column_Nb], Y + 15);
				pdc->SelectObject(poldpen);
			}
		}

		if(Y > End) break;
	}

	/* erase unused part of scroll vieuw */
	if(Y < End)
	{
		for(iii = 0; iii < Column_Nb; iii++)
		{
			i_ColumnType = mpo_Parent->mst_Ini.ai_ColOrder[Column[iii]];
			if(i_ColumnType + 1 == mpo_Parent->en_Rank)
				C = ETEXT_COLOR_RankBack;
			else
			{
				if(i_ColumnType == mi_OrderColumn)
					C = ETEXT_COLOR_ColSort0 + ((Column_ColorOffset + iii) & 1);
				else
					C = ETEXT_COLOR_Col0 + ((Column_ColorOffset + iii) & 1);
			}

			pdc->FillSolidRect
				(
					Column_X[iii],
					o_Pos.y + Y + 16,
					Column_X[iii + 1] - Column_X[iii],
					o_Rect.Height() - Y - 16,
					maul_Color[0][C]
				);
		}
	}

	/* Draw rect while in tirette capture mode */
	if(mi_Capture == CAPTURE_TIRETTE)
	{
		if(Tirette_EndLine < Tirette_Line)
		{
			o_Rect.top = mpo_Parent->mpt_EditText->pst_Ids[Tirette_EndLine].top + 16;
			o_Rect.bottom = mpo_Parent->mpt_EditText->pst_Ids[Tirette_Line].bottom + 16;
		}
		else
		{
			o_Rect.top = mpo_Parent->mpt_EditText->pst_Ids[Tirette_StartLine].top + 16;
			o_Rect.bottom = mpo_Parent->mpt_EditText->pst_Ids[Tirette_EndLine].bottom + 16;
		}

		for(iii = 0; iii < Column_Nb; iii++)
		{
			if(mpo_Parent->mst_Ini.ai_ColOrder[Column[iii]] + 1 == Tirette_ColumnType)
			{
				o_Rect.left = Column_X[iii];
				o_Rect.right = Column_X[iii + 1];
			}
		}

		pdc->DrawEdge(&o_Rect, EDGE_BUMP, BF_RECT);
	}

	if(mb_RankMoving && (mi_HitTest_Line != -1))
	{
		o_Rect.top = mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].top + 16;
		o_Rect.bottom = mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].bottom + 16;

		for(iii = 0; iii < Column_Nb; iii++)
		{
			if(mpo_Parent->mst_Ini.ai_ColOrder[Column[iii]] + 1 & mpo_Parent->en_Masked) continue;
			if(mpo_Parent->mst_Ini.ai_ColOrder[Column[iii]] + 1 != mpo_Parent->en_Empty)
			{
				o_Rect.left = min(o_Rect.left, Column_X[iii]);
				o_Rect.right = max(o_Rect.right, Column_X[iii + 1]);
			}
		}

		pdc->DrawEdge(&o_Rect, EDGE_BUMP, BF_RECT);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::OnMouseMove(UINT n, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect	o_Rect;
	int		i, i_ColumnType, i_PosY, i_PosX, i_MinX, i_MaxX, dX;
	CPoint	savept;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(EDI_gst_DragDrop.b_BeginDragDrop)
	{
		M_MF()->b_MoveDragDrop(pt);
		return;
	}

	if(mi_Capture)
	{
		savept = pt;
		if(pt.x > mo_CaptureRect.right) pt.x = mo_CaptureRect.right;
		if(pt.x < mo_CaptureRect.left) pt.x = mo_CaptureRect.left;
		if(pt.y < mo_CaptureRect.top) pt.y = mo_CaptureRect.top;
		if(pt.y > mo_CaptureRect.bottom) pt.y = mo_CaptureRect.bottom;
		if(savept != pt)
		{
			ClientToScreen((LPPOINT) & pt);
			SetCursorPos(pt.x, pt.y);
			return;
		}
	}

	if(mi_Capture == CAPTURE_SIZECOL)
	{
		if(pt.x != mo_CapturePt.x)
		{
			i_ColumnType = mpo_Parent->mst_Ini.ai_ColOrder[Column[mi_HitTest_SizeColumn]];
			mpo_Parent->mst_Ini.ai_ColSize[i_ColumnType] += (pt.x - mo_CapturePt.x);
			if(mpo_Parent->mst_Ini.ai_ColSize[i_ColumnType] < 10) mpo_Parent->mst_Ini.ai_ColSize[i_ColumnType] = 10;
			Invalidate();
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
		}

		mo_CapturePt = pt;
		return;
	}

	if(mi_Capture == CAPTURE_PICKHEADER)
	{
		pt -= mo_CapturePt;
		if(pt.x * pt.x + pt.y * pt.y > 16)
		{
			mi_Capture = CAPTURE_MOVEHEADER;
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
		}

		return;
	}

	if(mi_Capture == CAPTURE_MOVEHEADER)
	{
		mo_CapturePt = pt;
		i_PosX = GetScrollPos(SB_HORZ);
		GetScrollRange(SB_HORZ, &i_MinX, &i_MaxX);
		if(pt.x == mo_CaptureRect.right)
		{
			dX = i_MaxX - i_PosX;
			if(dX)
			{
				if(dX > 5) dX = 5;
				i_PosX = GetScrollPos(SB_HORZ);
				SetScrollPos(SB_HORZ, i_PosX + dX);
				if(i_PosX != GetScrollPos(SB_HORZ)) mi_MoveHeader_Offset -= dX;
			}
		}
		else if(pt.x == mo_CaptureRect.left)
		{
			dX = i_PosX - i_MinX;
			if(dX)
			{
				if(dX > 5) dX = 5;
				i_PosX = GetScrollPos(SB_HORZ);
				SetScrollPos(SB_HORZ, i_PosX - dX);
				mi_MoveHeader_Offset += dX;
			}
		}

		Invalidate();
		return;
	}

	if(mi_Capture == CAPTURE_TIRETTE)
	{
		i_PosY = GetScrollPos(SB_VERT);
		i_PosY += pt.y;

		Tirette_EndLine = -1;

		if(i_PosY < mpo_Parent->mpt_EditText->pst_Ids[Tirette_StartLine].top + 16)
		{
			Tirette_EndLine = 0;
			for(i = Tirette_StartLine - 1; i >= 0; i--)
			{
				if(i_PosY >= mpo_Parent->mpt_EditText->pst_Ids[i].top + 16)
				{
					Tirette_EndLine = i;
					break;
				}
			}
		}
		else if(i_PosY <= mpo_Parent->mpt_EditText->pst_Ids[Tirette_Line].bottom + 16)
			Tirette_EndLine = Tirette_Line;
		else
		{
			for(i = Tirette_Line + 1; i < (int) mpo_Parent->mpt_EditText->ul_Num - 1; i++)
			{
				if(i_PosY <= mpo_Parent->mpt_EditText->pst_Ids[i].bottom + 16)
				{
					Tirette_EndLine = i;
					break;
				}
			}

			if(Tirette_EndLine == -1) Tirette_EndLine = i;
		}

		Invalidate();
		return;
	}

	if(mi_Capture == CAPTURE_PICKRANK)
	{
		if(mi_CaptureState == 0)
		{
			HitTest(pt);
			if((mi_HitTest_Line != Sel_DownLine) || (mi_HitTest_Column != Sel_DownColumn))
			{
				if(!MigratePickRank()) mi_CaptureState = 1;
			}
		}
		else
			::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR2));
		return;
	}

	if(HitTest(pt) && (mi_HitTest_SizeColumn != -1)) ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));

	CScrollView::OnMouseMove(n, pt);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::OnLButtonDown(UINT n, CPoint pt)
{
	/*~~~~~~~~~~~~*/
	CRect	o_Rect;
	CString content;
	CPoint	o_Pos;
	/*~~~~~~~~~~~~*/

	mb_RankMoving = FALSE;
	HitTest(pt);
	if(!mi_HitTest)
	{
		if(mb_EditContent) OnLooseFocus();
		if(UpdateSel(-1, 0, 0, -1)) Invalidate();
		return;
	}

	if(mi_HitTest_SizeColumn != -1)
	{
		mo_Edit.ShowWindow(SW_HIDE);
		o_Pos = GetDeviceScrollPosition();
		SetCapture();
		mi_Capture = CAPTURE_SIZECOL;
		mo_CapturePt = pt;
		GetClientRect(mo_CaptureRect);
		mo_CaptureRect.left = Column_X[mi_HitTest_SizeColumn] - o_Pos.x + 10;
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
		return;
	}

	if(mb_EditContent)
	{
		OnLooseFocus();
	}

	if(mi_HitTest_Line == -1)
	{
		SetCapture();
		mi_Capture = CAPTURE_PICKHEADER;
		mo_CapturePt = pt;
		GetClientRect(mo_CaptureRect);

		mi_Edit_Column = mi_HitTest_Column;
		mi_MoveHeader_Column = Column[mi_Edit_Column];
		mi_MoveHeader_Offset = pt.x - Column_X[mi_Edit_Column];
		mi_MoveHeader_Size = Column_X[mi_Edit_Column + 1] - Column_X[mi_Edit_Column];
		return;
	}

	if(UpdateSel(mi_HitTest_Line, n & MK_SHIFT, n & MK_CONTROL, TRUE)) Invalidate();

	/* set capture to be sure that we get the button up */
	SetCapture();
	mi_Capture = CAPTURE_PICKRANK;
	mo_CapturePt = pt;
	mi_CaptureState = 0;
	GetClientRect(mo_CaptureRect);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::MoveRank(int iOldFrom, int iNewFrom, int iSize)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Id	*pTemp;
	int				i, j;
	/*~~~~~~~~~~~~~~~~~~~~~*/


	if(iOldFrom == iNewFrom) return;
    if(iSize == 0) return;

	pTemp = new TEXT_tdst_Id[mpo_Parent->mpt_EditText->ul_Num];
	L_memcpy(pTemp, mpo_Parent->mpt_EditText->pst_Ids, mpo_Parent->mpt_EditText->ul_Num * sizeof(TEXT_tdst_Id));

	for(j = 0; j < iSize; j++)
	{
		L_memcpy(&mpo_Parent->mpt_EditText->pst_Ids[iNewFrom + j], &pTemp[iOldFrom + j], sizeof(TEXT_tdst_Id));
	}

	if(iOldFrom > iNewFrom)
	{
		for(i = iNewFrom, j = 0; i < iOldFrom; i++, j++)
		{
			L_memcpy
			(
				&mpo_Parent->mpt_EditText->pst_Ids[iNewFrom + iSize + j],
				&pTemp[iNewFrom + j],
				sizeof(TEXT_tdst_Id)
			);
		}
	}
	else
	{
		for(i = iOldFrom, j = 0; i < iNewFrom; i++, j++)
		{
			L_memcpy
			(
				&mpo_Parent->mpt_EditText->pst_Ids[iOldFrom + j],
				&pTemp[iOldFrom + iSize + j],
				sizeof(TEXT_tdst_Id)
			);
		}
	}

	delete[] pTemp;

	mpo_Parent->SetFileModified(TRUE);
	mpo_Parent->mb_NeedSync = TRUE;
	
    //mpo_Parent->OnFileChange();
    TextChange();

	Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::OnLButtonUp(UINT n, CPoint pt)
{
	/*~~~~~~~*/
	int i, col;
	/*~~~~~~~*/

	if(mb_RankMoving)
	{
		MoveRank
		(
			mpo_Parent->mpt_EditText->pst_Ids[Sel_BlockStart].order,
			mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order,
			Sel_BlockEnd - Sel_BlockStart + 1
		);
	}

	mb_RankMoving = FALSE;

	if(EDI_gst_DragDrop.b_BeginDragDrop)
	{
		M_MF()->EndDragDrop(pt);
		return;
	}

	/* resizing column */
	if(mi_Capture == CAPTURE_SIZECOL)
	{
		ComputeSizes();
		goto lbuttondown_release;
	}

	/* pick header : changing line sort method */
	if(mi_Capture == CAPTURE_PICKHEADER)
	{
		if(mpo_Parent->mst_Ini.ai_ColOrder[Column[mi_HitTest_Column]] == mi_OrderColumn)
			mi_OrderLesserToGreater = !mi_OrderLesserToGreater;
		else
		{
			mi_OrderColumn = mpo_Parent->mst_Ini.ai_ColOrder[Column[mi_HitTest_Column]];
			mi_OrderLesserToGreater = TRUE;
		}

		ComputeOrder();
		ComputeSizes();
		Invalidate();
		goto lbuttondown_release;
	}

	/* move header : changing order of column */
	if(mi_Capture == CAPTURE_MOVEHEADER)
	{
		if(mi_MoveHeader_Drop != -1)
		{
			if(mi_MoveHeader_Drop == Column_Nb)
				mi_MoveHeader_Drop = Column[Column_Nb - 1] + 1;
			else
				mi_MoveHeader_Drop = Column[mi_MoveHeader_Drop];

			if((mi_MoveHeader_Drop != mi_MoveHeader_Column) && (mi_MoveHeader_Drop != mi_MoveHeader_Column + 1))
			{
				col = mpo_Parent->mst_Ini.ai_ColOrder[mi_MoveHeader_Column];
				if(mi_MoveHeader_Drop < mi_MoveHeader_Column)
				{
					for(i = mi_MoveHeader_Column; i > mi_MoveHeader_Drop; i--)
						mpo_Parent->mst_Ini.ai_ColOrder[i] = mpo_Parent->mst_Ini.ai_ColOrder[i - 1];
					mpo_Parent->mst_Ini.ai_ColOrder[mi_MoveHeader_Drop] = col;
				}
				else
				{
					for(i = mi_MoveHeader_Column; i < mi_MoveHeader_Drop - 1; i++)
						mpo_Parent->mst_Ini.ai_ColOrder[i] = mpo_Parent->mst_Ini.ai_ColOrder[i + 1];
					mpo_Parent->mst_Ini.ai_ColOrder[mi_MoveHeader_Drop - 1] = col;
				}
			}
		}

		Invalidate();
		goto lbuttondown_release;
	}

	/* tirette */
	if(mi_Capture == CAPTURE_TIRETTE)
	{
		if ((n & MK_SHIFT) && (n & MK_CONTROL))
			DoTirette();
		Invalidate();
		goto lbuttondown_release;
	}

	/* update/edit sel */
	if(mi_Capture == CAPTURE_PICKRANK)
	{
		if(mi_CaptureState) goto lbuttondown_release;
		if(UpdateSel(mi_HitTest_Line, n & MK_SHIFT, n & MK_CONTROL, FALSE)) Invalidate();

		/* if(mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].sel) EditCel(); */
		goto lbuttondown_release;
	}

lbuttondown_release:
	ReleaseCapture();
	mi_Capture = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::OnLButtonDblClk(UINT nFlags, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC		*pdc;
	char	*sz_Content, *sz_BOL, *sz_EOL;
	int		ColumnType, SizeMax, iii;
	CSize	o_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mi_HitTest) return;

	if(mi_HitTest_SizeColumn != -1)
	{
		pdc = GetDC();
		pdc->SelectObject(&M_MF()->mo_Fnt);

		ColumnType = mpo_Parent->mst_Ini.ai_ColOrder[Column[mi_HitTest_Column]];
		SizeMax = 10;

		for(iii = 0; iii < (int) mpo_Parent->mpt_EditText->ul_Num; iii++)
		{
			sz_Content = mpo_Parent->CEL_GetText(iii, ColumnType + 1);
			sz_BOL = sz_Content;

			do
			{
				sz_EOL = L_strchr(sz_BOL, '\n');
				if(sz_EOL) *sz_EOL = 0;

				o_Size = pdc->GetTextExtent(sz_BOL, strlen(sz_BOL));
				if(o_Size.cx > SizeMax) SizeMax = o_Size.cx;
				if(sz_EOL) *sz_EOL = '\n';
				sz_BOL = sz_EOL + 1;
			} while(sz_EOL);
		}

		mpo_Parent->mst_Ini.ai_ColSize[ColumnType] = SizeMax + 5;
		ComputeSizes();
		Invalidate();
		return;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::OnRButtonUp(UINT n, CPoint pt)
{
	/*
	 * update/edit sel £
	 * if(mi_Capture == CAPTURE_PICKRANK)
	 */
	{
		if(!mi_CaptureState)
		{
			if(UpdateSel(mi_HitTest_Line, n & MK_SHIFT, n & MK_CONTROL, FALSE)) Invalidate();
			if(mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].sel) EditCel();
		}
	}

	ReleaseCapture();
	mi_Capture = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::OnRButtonDown(UINT n, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int i, col, res, ColumnType;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mi_Capture) return;

	mo_Edit.ShowWindow(SW_HIDE);

	if(!mi_HitTest) return;
	if(mi_HitTest_SizeColumn != -1) return;

	if(mi_HitTest_Line == -1)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~*/
		CMenu	o_Menu;
		BOOL	b_UsedCol[COLMAX];
		char	sz_Name[ 128 ];
		/*~~~~~~~~~~~~~~~~~~~~~~*/

		mi_HitTest = 0;

		o_Menu.CreatePopupMenu();

		/* command */
		if(mi_HitTest_Column != -2)
		{
			ColumnType = mpo_Parent->mst_Ini.ai_ColOrder[Column[mi_HitTest_Column]];
			if(ColumnType + 1 >= mpo_Parent->en_UserData)
			{
				sprintf(sz_Name, "rename %s", mpo_Parent->mst_Ini.asz_ColName[ColumnType]);
				o_Menu.AppendMenu(MF_BYCOMMAND, 1, sz_Name);
				o_Menu.AppendMenu(MF_SEPARATOR, 0, "");
			}
		}

		/* column */
		mpo_Parent->Column_GetUsed( b_UsedCol, 3 );

		for(i = 0; i < COLMAX; i++)
		{
			col = mpo_Parent->mst_Ini.ai_ColOrder[i];
			if(!b_UsedCol[col]) continue;
			res = MF_BYCOMMAND | (mpo_Parent->mst_Ini.ai_ColSize[col] > 0) ? MF_CHECKED : 0;
			o_Menu.AppendMenu(res, i + 10, mpo_Parent->mst_Ini.asz_ColName[col]);
		}

		/* display menu */
		GetCursorPos(&pt);
		res = o_Menu.TrackPopupMenu
			(
				TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
				pt.x,
				pt.y,
				mpo_Parent
			);
		if(res == 1)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			EDIA_cl_NameDialog	o_Name("Enter the column name");
			char				sz_Temp[SIZEMAX];
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			o_Name.mo_Name = CString(mpo_Parent->mst_Ini.asz_ColName[ColumnType]);
			if((o_Name.DoModal() == IDOK) && o_Name.mo_Name.GetLength())
			{
				L_memcpy(sz_Temp, (char *) (LPCTSTR) o_Name.mo_Name, SIZEMAX);
				sz_Temp[SIZEMAX - 1] = 0;
				L_strcpy(mpo_Parent->mst_Ini.asz_ColName[ColumnType], sz_Temp);
				Invalidate();
			}
		}
		else if(res > 0)
		{
			res -= 10;
			col = mpo_Parent->mst_Ini.ai_ColOrder[res];
			mpo_Parent->mst_Ini.ai_ColSize[col] *= -1;
			if((mpo_Parent->mst_Ini.ai_ColSize[col] > 0) && (mpo_Parent->mst_Ini.ai_ColSize[col] < 10))
				mpo_Parent->mst_Ini.ai_ColSize[col] = 10;
			ComputeSizes();
			Invalidate();
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ETEXT_cl_Scroll::HitTest(CPoint pt)
{
	/*~~~~~~~~~~*/
	CPoint	o_Pos;
	int		i;
	/*~~~~~~~~~~*/

	mi_HitTest = 0;
	mi_HitTest_Line = -2;
	mi_HitTest_Column = -2;
	mi_HitTest_SizeColumn = -1;

	if(!mpo_Parent->mpt_EditText) return 0;

	o_Pos = GetDeviceScrollPosition();

	if(pt.y < 16)
	{
		mi_HitTest_Line = -1;
	}

	/* column */
	pt.x += o_Pos.x;
	for(i = 0; i < Column_Nb; i++)
	{
		if(pt.x < Column_X[i + 1])
		{
			if(abs(pt.x - Column_X[i + 1]) < 4) mi_HitTest_SizeColumn = i;
			mi_HitTest_Column = i;
			break;
		}
	}

	if(mi_HitTest_Column == -2)
	{
		if(mi_HitTest_Line == -1)
			return(mi_HitTest = 1);
		else
			return 0;
	}

	mi_HitTest_Line = -2;
	if(pt.y < 16)
		mi_HitTest_Line = -1;
	else
	{
		pt.y += o_Pos.y - 16;
		for(i = 0; i < (int) mpo_Parent->mpt_EditText->ul_Num; i++)
		{
			if(pt.y < mpo_Parent->mpt_EditText->pst_Ids[i].bottom)
			{
				mi_HitTest_Line = i;
				break;
			}
		}
	}

	if(mi_HitTest_Line == -2) return 0;

	mi_HitTest = 1;
	return mi_HitTest;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ETEXT_cl_Scroll::SetEditPosSize(CPoint pt, int _i_Num, BOOL _b_Content)
{
	/*~~~~~~~~~~*/
	CPoint	o_Pos;
	/*~~~~~~~~~~*/

	o_Pos = GetDeviceScrollPosition();
	pt.x += o_Pos.x;
	pt.y += o_Pos.y;

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::OnLooseFocus(void)
{
	OnEditChange();
	mb_EditContent = FALSE;
	mo_Edit.ShowWindow(SW_HIDE);
	ComputeSizes();
	Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::OnEditChange(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static int	si_Recurse = 0;
	CString		o_Text;
	CRect		o_Rect;
	int			ColumnType, i_Count;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_Parent->mpt_EditText) return;
	if(si_Recurse) return;
	si_Recurse = 1;

	mo_Edit.GetWindowText(o_Text);
	o_Text.Replace("\r\n", "\n");
	ColumnType = mpo_Parent->mst_Ini.ai_ColOrder[Column[mi_Edit_Column]] + 1;
	mpo_Parent->CEL_SetString
		(
			mpo_Parent->mpt_EditText->pst_Ids[mi_Edit_Line].order,
			ColumnType,
			(char *) (LPCTSTR) o_Text
		);

	i_Count = ETEXT_CountLine((char *) (LPCTSTR) o_Text);
	if(i_Count != mi_Edit_LineNumber)
	{
		mi_Edit_LineNumber = i_Count;
		ComputeSizes();
		Invalidate();
		ComputeCelRect(o_Rect, mi_Edit_Column, mi_Edit_Line, TRUE);
		mo_Edit.MoveWindow(o_Rect);
	}

	mpo_Parent->mb_FileIsModified = TRUE;

	si_Recurse = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ETEXT_cl_Scroll::OnScroll(UINT a, UINT b, BOOL bDoScroll)
{
	mo_Edit.ShowWindow(SW_HIDE);
	mb_EditContent = FALSE;
	Invalidate();
	return CScrollView::OnScroll(a, b, bDoScroll);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ETEXT_cl_Scroll::OnMouseWheel(UINT a, SHORT b, CPoint pt)
{
	/*~~~~~~~~*/
	BOOL	res;
	/*~~~~~~~~*/

	res = CScrollView::DoMouseWheel(a, b * 5, pt);

	if(res) Invalidate();
	return res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::UpdateScroll(void)
{
	SetScrollSizes(MM_TEXT, mo_Size);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::OnSize(UINT nType, int cx, int cy)
{
	mo_Edit.ShowWindow(SW_HIDE);
	CScrollView::OnSize(nType, cx, cy);

	if(mpo_Parent)
	{
		ComputeSizes();
		if(mb_EditContent) ShowEdit(FALSE);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ETEXT_cl_Scroll::OnEraseBkgnd(CDC *)
{
	/* return TRUE; */
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::DeleteCur(void)
{
#if 0
	mo_Edit.ShowWindow(SW_HIDE);
	mpo_Parent->mo_ListContent.RemoveAt(mpo_Parent->mo_ListContent.FindIndex(mi_NumSel));
	mpo_Parent->mo_ListIds.RemoveAt(mpo_Parent->mo_ListIds.FindIndex(mi_NumSel));
	if(mi_NumSel >= mpo_Parent->mo_ListContent.GetCount()) mi_NumSel--;
	Invalidate();
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ETEXT_cl_Scroll::PreTranslateMessage(MSG *msg)
{
	/*~~~~~~~~~~~~~~*/
	CString o_Content;
	/*~~~~~~~~~~~~~~*/

	if(msg->message == WM_MBUTTONDOWN) return TRUE;

	if(msg->message == WM_KEYDOWN)
	{
		switch(msg->wParam)
		{
		case VK_TAB:	EditNextPrevious(FALSE, (GetAsyncKeyState(VK_SHIFT) < 0) ? FALSE : TRUE); return TRUE;
//		case VK_HOME:	if (mb_EditContent) EditNextPrevious(FALSE, FALSE); return TRUE;
		case VK_PRIOR:	if (mb_EditContent) EditNextPrevious(TRUE, FALSE); return TRUE;
		case VK_NEXT:	if (mb_EditContent) EditNextPrevious(TRUE, TRUE); return TRUE;
		}
	}

	if(msg->hwnd == mo_Edit.GetSafeHwnd())
	{
		if(msg->message == WM_KEYDOWN)
		{
			if((msg->wParam == VK_RETURN) && !mi_Edit_Multiline && (GetAsyncKeyState(VK_SHIFT) >= 0)) return TRUE;
			if(msg->wParam == VK_TAB) return TRUE;
		}
	}

	return CScrollView::PreTranslateMessage(msg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::TextChange(void)
{
	mo_Edit.ShowWindow(SW_HIDE);
	ComputeOrder();
	ComputeSizes();
	Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::ShowEdit(BOOL _b_Init)
{
	/*~~~~~~~~~~~~~~~*/
	int		ColumnType;
	CRect	o_Rect;
	CString content;
	/*~~~~~~~~~~~~~~~*/

	ColumnType = mpo_Parent->mst_Ini.ai_ColOrder[Column[mi_Edit_Column]] + 1;

	ComputeCelRect(o_Rect, mi_Edit_Column, mi_Edit_Line, TRUE);
	mo_Edit.MoveWindow(o_Rect);
	if(_b_Init)
	{
		content = mpo_Parent->CEL_GetText(mpo_Parent->mpt_EditText->pst_Ids[mi_Edit_Line].order, ColumnType);
		mi_Edit_LineNumber = ETEXT_CountLine((char *) (LPCTSTR) content);
		content.Replace("\n", "\r\n");
		mo_Edit.SetWindowText(content);
		mi_Edit_Multiline = (ColumnType == mpo_Parent->en_ID) ? FALSE : TRUE;
	}

	mo_Edit.ShowWindow(SW_SHOW);
	mo_Edit.SetFocus();
	mb_EditContent = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::EditNextPrevious(BOOL _b_KeepCol, BOOL _b_Next)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int i, i_Col;
	int ai_EditCol[128], i_NbEditCol, i_NbCol;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_b_KeepCol)
	{
		if(!mb_EditContent) return;
		if(_b_Next)
		{
			if(mi_Edit_Line + 1 >= (int) mpo_Parent->mpt_EditText->ul_Num) return;
			mi_Edit_Line++;
		}
		else
		{
			if(mi_Edit_Line == 0) return;
			mi_Edit_Line--;
		}
	}
	else
	{
		i_NbEditCol = i_NbCol = 0;
		for(i = 0; i < COLMAX; i++)
		{
			i_Col = mpo_Parent->mst_Ini.ai_ColOrder[i];
			if(mpo_Parent->mst_Ini.ai_ColSize[ i_Col ] <= 0) continue;
			i_Col++;
			if((i_Col == mpo_Parent->en_Preview) || (i_Col == mpo_Parent->en_ID) || (i_Col >= mpo_Parent->en_UserData))
				ai_EditCol[i_NbEditCol++] = i_NbCol;

			i_NbCol++;
		}

		if(!i_NbEditCol) return;

		if(_b_Next)
		{
			if(mb_EditContent)
			{
				for(i = 0; i < i_NbEditCol; i++)
				{
					if(ai_EditCol[i] > mi_Edit_Column) break;
				}

				if(i == i_NbEditCol)
				{
					if(mi_Edit_Line + 1 >= (int) mpo_Parent->mpt_EditText->ul_Num) return;
					mi_Edit_Line++;
					mi_Edit_Column = ai_EditCol[0];
				}
				else
					mi_Edit_Column = ai_EditCol[i];
			}
			else
			{
				mi_Edit_Column = ai_EditCol[0];
				if(mi_LastEntrySel != -1)
                {
                    for(i=0; i<(int)mpo_Parent->mpt_EditText->ul_Num; i++)
                    {
                        if(mpo_Parent->mpt_EditText->pst_Ids[i].order == mi_LastEntrySel) break;
                    }
					mi_Edit_Line = i;
                }
				else
					mi_Edit_Line = 0;
			}
		}
		else
		{
			if(mb_EditContent)
			{
				for(i = 0; i < i_NbEditCol; i++)
				{
					if(ai_EditCol[i] == mi_Edit_Column) break;
				}

				if(i == 0)
				{
					if(mi_Edit_Line == 0) return;
					mi_Edit_Line--;
					mi_Edit_Column = ai_EditCol[i_NbEditCol - 1];
				}
				else
					mi_Edit_Column = ai_EditCol[i - 1];
			}
			else
			{
				mi_Edit_Column = ai_EditCol[i_NbEditCol];
				if(mi_LastEntrySel != -1)
                {
                    for(i=0; i<(int)mpo_Parent->mpt_EditText->ul_Num; i++)
                    {
                        if(mpo_Parent->mpt_EditText->pst_Ids[i].order == mi_LastEntrySel) break;
                    }

					mi_Edit_Line = i;
                }
				else
					mi_Edit_Line = 0;
			}
		}
	}

	EnsureVisible(mi_Edit_Line, mi_Edit_Column);
	EnsureVisible(mi_Edit_Line, mpo_Parent->mst_Ini.ai_ColOrder[Column[mi_Edit_Column]] );
	ShowEdit(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::EditCel(void)
{
	/*~~~~~~~~~~~~~~~~*/
	int		ColumnType;
	CRect	o_Rect;
	CString content;
	CPoint	o_Pos, o_Pt;
	CMenu	o_Menu;
	int		i;
	/*~~~~~~~~~~~~~~~~*/

	ColumnType = mpo_Parent->mst_Ini.ai_ColOrder[Column[mi_HitTest_Column]] + 1;
	if
	(
		(ColumnType == mpo_Parent->en_Preview)
	||	(ColumnType == mpo_Parent->en_ID)
	||	(ColumnType >= mpo_Parent->en_UserData)
	)
	{
		mi_Edit_Column = mi_HitTest_Column;
		mi_Edit_Line = mi_HitTest_Line;
		ShowEdit(TRUE);
		return;
	}

    if(ColumnType == mpo_Parent->en_IDKey)
    {
		/*~~~~~~~~~~~~~~~~~~~~~*/
		ULONG	ul_Key;
		/*~~~~~~~~~~~~~~~~~~~~~*/
        
        ul_Key = mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].ul_IdKey;
        if((ul_Key != -1) && (ul_Key != 0)) return;

		o_Menu.CreatePopupMenu();
		o_Menu.AppendMenu(MF_BYCOMMAND, 1, "Correct entry key");
		GetCursorPos(&o_Pt);

		mb_InMenu = TRUE;
		i = o_Menu.TrackPopupMenu
			(
				TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
				o_Pt.x,
				o_Pt.y,
				mpo_Parent
			);
		mb_InMenu = FALSE;
		
        switch(i)
		{
		case 1:
            mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].ul_IdKey = BIG_ul_GetNewKey(BIG_C_InvalidKey);
            Invalidate();
			mpo_Parent->mb_FileIsModified = TRUE;
			break;

		default:
			break;
		}

		return;
    }

	if(ColumnType == mpo_Parent->en_Rank)
	{
		/*~~~~~~~~~~~*/
		ULONG	ulPrev;
		/*~~~~~~~~~~~*/

		o_Menu.CreatePopupMenu();
		o_Menu.AppendMenu(MF_BYCOMMAND, 1, "Insert hole");
		o_Menu.AppendMenu(MF_BYCOMMAND, 2, "Insert entry");
		o_Menu.AppendMenu(MF_BYCOMMAND, 3, "Delete entry");
		GetCursorPos(&o_Pt);

		mb_InMenu = TRUE;
		i = o_Menu.TrackPopupMenu
			(
				TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
				o_Pt.x,
				o_Pt.y,
				mpo_Parent
			);
		mb_InMenu = FALSE;
		switch(i)
		{
		case 1:
			i = mi_HitTest_Line;
			ulPrev = mpo_Parent->mpt_EditText->ul_Num;
			mpo_Parent->OnEntryNew(TRUE, TRUE);
			MoveRank
			(
				mpo_Parent->mpt_EditText->pst_Ids[ulPrev].order,
				mpo_Parent->mpt_EditText->pst_Ids[i].order,
				mpo_Parent->mpt_EditText->ul_Num - ulPrev
			);
			break;

		case 2:
			i = mi_HitTest_Line;
			ulPrev = mpo_Parent->mpt_EditText->ul_Num;
			mpo_Parent->OnEntryNew(TRUE, FALSE);
			MoveRank
			(
				mpo_Parent->mpt_EditText->pst_Ids[ulPrev].order,
				mpo_Parent->mpt_EditText->pst_Ids[i].order,
				mpo_Parent->mpt_EditText->ul_Num - ulPrev
			);
			break;

		case 3:
			mpo_Parent->OnAction(ETEXT_ACTION_ENTRY_DEL);
			break;

		default:
			break;
		}

		return;
	}

	if(ColumnType == mpo_Parent->en_SndFileKey)
	{
		/*~~~~~~~~~~~~~~~~~~~~~*/
		ULONG	ul_Index, ul_Key;
		/*~~~~~~~~~~~~~~~~~~~~~*/

        ul_Key = mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].ul_SoundKey ;

		o_Menu.CreatePopupMenu();
        ul_Index =  ((ul_Key!=0) && (ul_Key!=BIG_C_InvalidKey)) ? MF_ENABLED : (MF_DISABLED|MF_GRAYED) ;
        o_Menu.AppendMenu(MF_BYCOMMAND | ul_Index, 6, "Edit");
        o_Menu.AppendMenu(MF_BYCOMMAND | ul_Index, 7, "Show in Browser");
        o_Menu.AppendMenu(MF_BYCOMMAND | ul_Index, 4, "Copy");
        o_Menu.AppendMenu(MF_SEPARATOR, 0, "");
		o_Menu.AppendMenu(MF_BYCOMMAND, 1, "Muted");
		o_Menu.AppendMenu(MF_BYCOMMAND, 5, "None");
		o_Menu.AppendMenu(MF_BYCOMMAND, 2, "Browse");
		o_Menu.AppendMenu(MF_BYCOMMAND, 3, "Auto");

        if((ul_Key!=0) && (ul_Key!=BIG_C_InvalidKey))
        {
            o_Menu.AppendMenu(MF_SEPARATOR, 0, "");
		    o_Menu.AppendMenu(MF_BYCOMMAND, 6, "Edit");
		    o_Menu.AppendMenu(MF_BYCOMMAND, 8, "Force for all language");
            o_Menu.AppendMenu(MF_SEPARATOR, 0, "");
        }

		mpo_Parent->CheckSndFileHistory();
		if(mpo_Parent->mst_Ini.i_NbSndFileKey)
		{
			o_Menu.AppendMenu(MF_SEPARATOR, 0, "");
			for(i = 0; i < mpo_Parent->mst_Ini.i_NbSndFileKey; i++)
			{
				ul_Index = BIG_ul_SearchKeyToFat(mpo_Parent->mst_Ini.ai_SndFileKey[i]);
				o_Menu.AppendMenu(MF_BYCOMMAND, i + 10, BIG_NameFile(ul_Index));
			}
		}

		GetCursorPos(&o_Pt);
		mb_InMenu = TRUE;
		i = o_Menu.TrackPopupMenu
			(
				TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
				o_Pt.x,
				o_Pt.y,
				mpo_Parent
			);
		mb_InMenu = FALSE;
		if(i <= 0) return;

		if(i == 1)
			ul_Key = 0;
		else if(i == 2)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			EDIA_cl_FileDialog	o_DialogFile("Choose dialog file", 0, 0, 1, NULL, "*" EDI_Csz_ExtSoundDialog);
			CString				o;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			ul_Key = BIG_C_InvalidKey;
			if(o_DialogFile.DoModal() == IDOK)
			{
				o_DialogFile.GetItem(o_DialogFile.mo_File, 0, o);
				ul_Index = BIG_ul_SearchFileExt(o_DialogFile.masz_FullPath, (char *) (LPCSTR) o);
				if(ul_Index != BIG_C_InvalidIndex) ul_Key = BIG_FileKey(ul_Index);
			}
		}
		else if(i == 3)
		{
			int idx = mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order;
			ul_Key = BIG_C_InvalidKey;

			if(mpo_Parent->mpt_EditText)
			{
				// skip if any "<...>"
				if(mpo_Parent->mpt_EditText->pst_Ids[idx].i_Offset != -1) 
				{
					if(
						L_strcmp(mpo_Parent->mpt_EditText->psz_Text + mpo_Parent->mpt_EditText->pst_Ids[idx].i_Offset, "<hole>") && 
						L_strcmp(mpo_Parent->mpt_EditText->psz_Text + mpo_Parent->mpt_EditText->pst_Ids[idx].i_Offset, "<deleted>")
						)
					{
						ul_Key = mpo_Parent->GetSndFileAuto
							(
								mpo_Parent->mpt_EditText->pst_Ids[idx].asz_Name
							);
					}
				}
			}
		}
        else if(i == 4)
        {
			ul_Key = mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].ul_SoundKey ;
			if(ul_Key) mpo_Parent->AddSndFileInHistory(ul_Key);
            return;
        }
        else if( i== 5)
        {
            mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].ul_SoundKey = BIG_C_InvalidKey;
            Invalidate();
			mpo_Parent->mb_FileIsModified = TRUE;
            return;
        }
        else if( i== 6 )
        {
            ESON_cl_Frame *po_SoundEdi ;
            ULONG ul_fat;
            ul_fat = BIG_ul_SearchKeyToFat(mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].ul_SoundKey);
            if(ul_fat != BIG_C_InvalidIndex)
            {
                po_SoundEdi = (ESON_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_SOUND, 0);
			    po_SoundEdi->mpo_MyView->IWantToBeActive(po_SoundEdi);
                po_SoundEdi->i_OnMessage( EDI_MESSAGE_SELFILE, BIG_ParentFile( ul_fat), ul_fat);
            }
            return;
        }
        else if( i== 7 )
        {
            EBRO_cl_Frame *po_Browser ;
            ULONG ul_fat;
            ul_fat = BIG_ul_SearchKeyToFat(mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].ul_SoundKey);
            if(ul_fat != BIG_C_InvalidIndex)
            {
                po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
			    po_Browser->mpo_MyView->IWantToBeActive(po_Browser);
                po_Browser->i_OnMessage( EDI_MESSAGE_SELFILE, BIG_ParentFile( ul_fat), ul_fat);
            }
            return;
        }
		else if(i==8)
		{
			mpo_Parent->mi_ForceSynchroEntry = mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order;
			mpo_Parent->mst_ForceSynchroEntry.ul_IdKey = mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mi_ForceSynchroEntry].ul_IdKey;
			mpo_Parent->mst_ForceSynchroEntry.ul_SoundKey = mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mi_ForceSynchroEntry].ul_SoundKey ;
			mpo_Parent->OnSynchronize();
			mpo_Parent->mi_ForceSynchroEntry = -1;
			L_memset(&mpo_Parent->mst_ForceSynchroEntry, 0, sizeof(TEXT_tdst_Id));
		}
		else
			ul_Key = mpo_Parent->mst_Ini.ai_SndFileKey[i - 10];

		if(mpo_Parent->mpt_EditText)
		{
			if(ul_Key != BIG_C_InvalidKey)
			{
				mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].ul_SoundKey = ul_Key;
				if(ul_Key) mpo_Parent->AddSndFileInHistory(ul_Key);
				Invalidate();
				mpo_Parent->mb_FileIsModified = TRUE;
			}
		}

		return;
	}

	if(ColumnType == mpo_Parent->en_Priority)
	{
		i = (int)mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].us_Priority;

		o_Menu.CreatePopupMenu();
		o_Menu.AppendMenu(MF_BYCOMMAND | ((i == TEXT_e_PrioUltraHigh) ? MF_CHECKED : 0), 1, "Ultra high");
		o_Menu.AppendMenu(MF_BYCOMMAND | ((i == TEXT_e_PrioVeryHigh) ? MF_CHECKED : 0), 2, "Very high");
		o_Menu.AppendMenu(MF_BYCOMMAND | ((i == TEXT_e_PrioHigh) ? MF_CHECKED : 0), 3, "High");
		o_Menu.AppendMenu(MF_BYCOMMAND | ((i == TEXT_e_PrioNormal) ? MF_CHECKED : 0), 4, "Normal");
		o_Menu.AppendMenu(MF_BYCOMMAND | ((i == TEXT_e_PrioLow) ? MF_CHECKED : 0), 5, "Low");
		o_Menu.AppendMenu(MF_BYCOMMAND | ((i == TEXT_e_PrioVeryLow) ? MF_CHECKED : 0), 6, "Very low");
		o_Menu.AppendMenu(MF_BYCOMMAND | ((i == TEXT_e_PrioUltraLow) ? MF_CHECKED : 0), 7, "Ultra low");

		GetCursorPos(&o_Pt);
		mb_InMenu = TRUE;
		i = o_Menu.TrackPopupMenu
			(
				TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
				o_Pt.x,
				o_Pt.y,
				mpo_Parent
			);
		mb_InMenu = FALSE;

		switch(i)
		{
		case 1:		i = TEXT_e_PrioUltraHigh; break;
		case 2:		i = TEXT_e_PrioVeryHigh; break;
		case 3:		i = TEXT_e_PrioHigh; break;
		case 4:		i = TEXT_e_PrioNormal; break;
		case 5:		i = TEXT_e_PrioLow; break;
		case 6:		i = TEXT_e_PrioVeryLow; break;
		case 7:		i = TEXT_e_PrioUltraLow; break;
		default:	return;
		}

		if((int)mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].us_Priority != i)
		{
			mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].us_Priority = (unsigned short)(0xFFFF & i);
			Invalidate();
			mpo_Parent->mb_FileIsModified = TRUE;
		}
	}

	if(ColumnType == mpo_Parent->en_FacialExp)
	{
		i = (int)mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].c_FacialIdx;

		o_Menu.CreatePopupMenu();
		if(mpo_Parent->b_HasUserValue(mpo_Parent->en_FacialExp))
		{
			for(int ij=0; ij<mpo_Parent->i_GetUserValueNb(mpo_Parent->en_FacialExp); ij++)
			{
				ULONG ulVal = mpo_Parent->ul_GetUserValue(mpo_Parent->en_FacialExp, ij);
				o_Menu.AppendMenu(MF_BYCOMMAND | (((ULONG)i == ulVal) ? MF_CHECKED : 0), ulVal, mpo_Parent->p_GetUserName(mpo_Parent->en_FacialExp, ij));
			}
		}
		else
		{
			o_Menu.AppendMenu(MF_BYCOMMAND | ((i == TEXT_e_ExpAngry) ? MF_CHECKED : 0), TEXT_e_ExpAngry, "Angry");
			o_Menu.AppendMenu(MF_BYCOMMAND | ((i == TEXT_e_ExpFighting) ? MF_CHECKED : 0), TEXT_e_ExpFighting, "Fighting");
			o_Menu.AppendMenu(MF_BYCOMMAND | ((i == TEXT_e_ExpHappy) ? MF_CHECKED : 0), TEXT_e_ExpHappy, "Happy");
			o_Menu.AppendMenu(MF_BYCOMMAND | ((i == TEXT_e_ExpNormal) ? MF_CHECKED : 0), TEXT_e_ExpNormal, "Normal");
			o_Menu.AppendMenu(MF_BYCOMMAND | ((i == TEXT_e_ExpRelieved) ? MF_CHECKED : 0), TEXT_e_ExpRelieved, "Relieved");
			o_Menu.AppendMenu(MF_BYCOMMAND | ((i == TEXT_e_ExpSurprised) ? MF_CHECKED : 0), TEXT_e_ExpSurprised, "Surprised");
			o_Menu.AppendMenu(MF_BYCOMMAND | ((i == TEXT_e_ExpWorried) ? MF_CHECKED : 0), TEXT_e_ExpWorried, "Worried");
		}

		GetCursorPos(&o_Pt);
		mb_InMenu = TRUE;
		i = o_Menu.TrackPopupMenu
			(
				TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
				o_Pt.x,
				o_Pt.y,
				mpo_Parent
			);
		mb_InMenu = FALSE;

		if((int)mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].c_FacialIdx != i)
		{
			mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].c_FacialIdx = (char)(0xFF & i);
			Invalidate();
			mpo_Parent->mb_FileIsModified = TRUE;
		}
	}

	if(ColumnType == mpo_Parent->en_Lips)
	{
		i = (int)mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].c_LipsIdx;

		o_Menu.CreatePopupMenu();

		if(mpo_Parent->b_HasUserValue(mpo_Parent->en_Lips))
		{
			for(int ij=0; ij<mpo_Parent->i_GetUserValueNb(mpo_Parent->en_Lips); ij++)
			{
				ULONG ulVal = mpo_Parent->ul_GetUserValue(mpo_Parent->en_Lips, ij);
				o_Menu.AppendMenu(MF_BYCOMMAND | (((ULONG)i == ulVal) ? MF_CHECKED : 0), ulVal, mpo_Parent->p_GetUserName(mpo_Parent->en_Lips, ij));
			}
		}
		else
		{
			o_Menu.AppendMenu(MF_BYCOMMAND | ((i == TEXT_e_LipsNormal) ? MF_CHECKED : 0), TEXT_e_LipsNormal, "Normal");
			o_Menu.AppendMenu(MF_BYCOMMAND | ((i == TEXT_e_LipsShouted) ? MF_CHECKED : 0), TEXT_e_LipsShouted, "Shouted");
			o_Menu.AppendMenu(MF_BYCOMMAND | ((i == TEXT_e_LipsShouted) ? MF_CHECKED : 0), TEXT_e_LipsScreamed, "Screamed");
			o_Menu.AppendMenu(MF_BYCOMMAND | ((i == TEXT_e_LipsWhispered) ? MF_CHECKED : 0), TEXT_e_LipsWhispered, "Whispered");
		}

		GetCursorPos(&o_Pt);
		mb_InMenu = TRUE;
		i = o_Menu.TrackPopupMenu
			(
				TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
				o_Pt.x,
				o_Pt.y,
				mpo_Parent
			);
		mb_InMenu = FALSE;

		if((int)mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].c_LipsIdx != i)
		{
			mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].c_LipsIdx = (char)(0xFF & i);
			Invalidate();
			mpo_Parent->mb_FileIsModified = TRUE;
		}
	}

	if(ColumnType == mpo_Parent->en_Anims)
	{
		int ij;
		ULONG ulVal;
		char sz_Text[16];

		i = (int)mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].c_AnimIdx;

		o_Menu.CreatePopupMenu();
		if(mpo_Parent->b_HasUserValue(mpo_Parent->en_Anims))
		{
			for(ij=0; ij<mpo_Parent->i_GetUserValueNb(mpo_Parent->en_Anims); ij++)
			{
				ulVal = mpo_Parent->ul_GetUserValue(mpo_Parent->en_Anims, ij);
				o_Menu.AppendMenu(MF_BYCOMMAND | (((ULONG)i == ulVal) ? MF_CHECKED : 0), ulVal, mpo_Parent->p_GetUserName(mpo_Parent->en_Anims, ij));
			}
		}
		else
		{
			for(ij=0; ij<100; ij++)
			{
				L_itoa(ij, sz_Text, 10);
				o_Menu.AppendMenu(MF_BYCOMMAND | ((i == ij) ? MF_CHECKED : 0), ij, sz_Text);
			}
		}

		GetCursorPos(&o_Pt);
		mb_InMenu = TRUE;
		i = o_Menu.TrackPopupMenu
			(
				TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
				o_Pt.x,
				o_Pt.y,
				mpo_Parent
			);
		mb_InMenu = FALSE;

		if((int)mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].c_AnimIdx != i)
		{
			mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].c_AnimIdx = (char)(0xFF & i);
			Invalidate();
			mpo_Parent->mb_FileIsModified = TRUE;
		}
	}

	if(ColumnType == mpo_Parent->en_Character)
	{
		/*~~~~~~~~~~~~~~~~~~~~~*/
		ULONG	ul_Index, ul_Key;
		/*~~~~~~~~~~~~~~~~~~~~~*/

        ul_Key = mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].ul_ObjKey ;

		o_Menu.CreatePopupMenu();
        o_Menu.AppendMenu(MF_BYCOMMAND | ( ((ul_Key!=0) && (ul_Key!=BIG_C_InvalidKey)) ? MF_ENABLED : (MF_DISABLED|MF_GRAYED)) , 3, "Copy");
        o_Menu.AppendMenu(MF_SEPARATOR, 0, "");
		o_Menu.AppendMenu(MF_BYCOMMAND, 1, "Nobody");
		o_Menu.AppendMenu(MF_BYCOMMAND, 2, "Browse");

		mpo_Parent->CheckCharacterHistory();
		if(mpo_Parent->mst_Ini.i_NbCharacterKey)
		{
			o_Menu.AppendMenu(MF_SEPARATOR, 0, "");
			for(i = 0; i < mpo_Parent->mst_Ini.i_NbCharacterKey; i++)
			{
				ul_Index = BIG_ul_SearchKeyToFat(mpo_Parent->mst_Ini.ai_CharacterKey[i]);
				o_Menu.AppendMenu(MF_BYCOMMAND, i + 10, BIG_NameFile(ul_Index));
			}
		}

		GetCursorPos(&o_Pt);
		mb_InMenu = TRUE;
		i = o_Menu.TrackPopupMenu
			(
				TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
				o_Pt.x,
				o_Pt.y,
				mpo_Parent
			);
		mb_InMenu = FALSE;
		if(i <= 0) return;
		if(i == 1)
			ul_Key = 0;
		else if(i == 2)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			EDIA_cl_FileDialog	o_DialogFile("Choose File", 0, 0, 1, NULL, "*" EDI_Csz_ExtGameObject);
			CString				o;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			ul_Key = BIG_C_InvalidKey;
			if(o_DialogFile.DoModal() == IDOK)
			{
				o_DialogFile.GetItem(o_DialogFile.mo_File, 0, o);
				ul_Index = BIG_ul_SearchFileExt(o_DialogFile.masz_FullPath, (char *) (LPCSTR) o);
				if(ul_Index != BIG_C_InvalidIndex) ul_Key = BIG_FileKey(ul_Index);
			}
		}
        else if(i== 3)
        {
            ul_Key = mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].ul_ObjKey;
            if(ul_Key) mpo_Parent->AddCharacterInHistory(ul_Key);
            return;
        }
		else
			ul_Key = mpo_Parent->mst_Ini.ai_CharacterKey[i - 10];

		if(mpo_Parent->mpt_EditText)
		{
			if(ul_Key != BIG_C_InvalidKey)
			{
				mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].ul_ObjKey = ul_Key;
                mpo_Parent->mpt_EditText->pst_Ids[mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order].pv_Obj = NULL;
				if(ul_Key) mpo_Parent->AddCharacterInHistory(ul_Key);
				Invalidate();
				mpo_Parent->mb_FileIsModified = TRUE;
			}
		}

		return;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::ComputeCelRect(CRect &_o_Rect, int _i_Column, int _i_Line, BOOL _b_Client)
{
	_o_Rect.left = Column_X[_i_Column];
	_o_Rect.right = Column_X[_i_Column + 1];
	_o_Rect.top = mpo_Parent->mpt_EditText->pst_Ids[_i_Line].top;
	_o_Rect.bottom = mpo_Parent->mpt_EditText->pst_Ids[_i_Line].bottom;

	if(_b_Client) _o_Rect -= GetDeviceScrollPosition() - CPoint(0, 16);
}

int qsort_xor;
int qsort_factor;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int __cdecl qsort_ID(const void *a, const void *b)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Id	*A, *B = (TEXT_tdst_Id *) * (ULONG *) b;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	A = (TEXT_tdst_Id *) * (ULONG *) a;

	return qsort_factor * L_stricmp(A->asz_Name, B->asz_Name);
}
/**/
char	*qsort_text;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int __cdecl qsort_preview(const void *a, const void *b)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Id	*A = (TEXT_tdst_Id *) * (ULONG *) a;
	TEXT_tdst_Id	*B = (TEXT_tdst_Id *) * (ULONG *) b;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if((A->i_Offset == -1) && (B->i_Offset == -1))
		return 0;
	else if(A->i_Offset == -1)
		return -qsort_factor;
	else if(B->i_Offset == -1)
		return qsort_factor;
	else
		return qsort_factor * L_strcmp(qsort_text + A->i_Offset, qsort_text + B->i_Offset);
}
/**/
int __cdecl qsort_idkey(const void *a, const void *b)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Id	*A = (TEXT_tdst_Id *) * (ULONG *) a;
	TEXT_tdst_Id	*B = (TEXT_tdst_Id *) * (ULONG *) b;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	return qsort_xor ^ ((A->ul_IdKey > B->ul_IdKey) ? 1 : 0);
}
/**/
int __cdecl qsort_character(const void *a, const void *b)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Id	*A = (TEXT_tdst_Id *) * (ULONG *) a;
	TEXT_tdst_Id	*B = (TEXT_tdst_Id *) * (ULONG *) b;
	char			*sz_A, *sz_B;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(A->ul_ObjKey == 0) sz_A = "nobody";
	else if(A->ul_ObjKey == BIG_C_InvalidKey) sz_A = "none";
	else sz_A = BIG_NameFile(BIG_ul_SearchKeyToFat( A->ul_ObjKey ));
	
	if(B->ul_ObjKey == 0) sz_B = "nobody";
	else if(B->ul_ObjKey == BIG_C_InvalidKey) sz_B = "none";
	else sz_B = BIG_NameFile(BIG_ul_SearchKeyToFat( B->ul_ObjKey ));

	return qsort_factor * L_strcmp(sz_A, sz_B);
}
/**/
int __cdecl qsort_sndfile(const void *a, const void *b)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Id	*A = (TEXT_tdst_Id *) * (ULONG *) a;
	TEXT_tdst_Id	*B = (TEXT_tdst_Id *) * (ULONG *) b;
	char			*sz_A, *sz_B;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(A->ul_SoundKey == 0) sz_A = "muted";
	else if(A->ul_SoundKey == BIG_C_InvalidKey) sz_A = "none";
	else sz_A = BIG_NameFile(BIG_ul_SearchKeyToFat( A->ul_SoundKey ));
	
	if(B->ul_SoundKey == 0) sz_B = "muted";
	else if(B->ul_SoundKey == BIG_C_InvalidKey) sz_B = "none";
	else sz_B = BIG_NameFile(BIG_ul_SearchKeyToFat( B->ul_SoundKey ));

	return qsort_factor * L_strcmp(sz_A, sz_B);
}
/**/int __cdecl qsort_prio(const void *a, const void *b)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Id	*A = (TEXT_tdst_Id *) * (ULONG *) a;
	TEXT_tdst_Id	*B = (TEXT_tdst_Id *) * (ULONG *) b;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    if(A->us_Priority == B->us_Priority) return 0;
    return qsort_factor * ((A->us_Priority > B->us_Priority) ? 1 : -1);
}
/**/
int __cdecl qsort_userdata(const void *a, const void *b)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Id	*A = (TEXT_tdst_Id *) * (ULONG *) a;
	TEXT_tdst_Id	*B = (TEXT_tdst_Id *) * (ULONG *) b;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	return qsort_factor * L_strcmp(A->asz_4Sort, B->asz_4Sort);
}
/**/
void ETEXT_cl_Scroll::ComputeOrder(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	ULONG	**mpul_Adr;
	int		i, ColumnType;
	/*~~~~~~~~~~~~~~~~~~*/

	if(!mpo_Parent->mpt_EditText) return;

	ColumnType = mi_OrderColumn + 1;

	if(ColumnType == mpo_Parent->en_Rank)
	{
		if(mi_OrderLesserToGreater)
		{
			for(i = 0; i < (int) mpo_Parent->mpt_EditText->ul_Num; i++)
				mpo_Parent->mpt_EditText->pst_Ids[i].order = i;
		}
		else
		{
			for(i = 0; i < (int) mpo_Parent->mpt_EditText->ul_Num; i++)
				mpo_Parent->mpt_EditText->pst_Ids[i].order = mpo_Parent->mpt_EditText->ul_Num - 1 - i;
		}

		return;
	}

	mpul_Adr = (ULONG **) L_malloc(sizeof(ULONG *) * mpo_Parent->mpt_EditText->ul_Num);
	for(i = 0; i < (int) mpo_Parent->mpt_EditText->ul_Num; i++)
		mpul_Adr[i] = (ULONG *) &mpo_Parent->mpt_EditText->pst_Ids[i];

	qsort_xor = mi_OrderLesserToGreater ? 1 : 0;
	qsort_factor = mi_OrderLesserToGreater ? 1 : -1;

	if(ColumnType >= mpo_Parent->en_UserData)
	{
		for(i = 0; i < (int) mpo_Parent->mpt_EditText->ul_Num; i++)
			mpo_Parent->mpt_EditText->pst_Ids[i].asz_4Sort = mpo_Parent->CEL_GetText(i, ColumnType);
		qsort(mpul_Adr, mpo_Parent->mpt_EditText->ul_Num, sizeof(ULONG), qsort_userdata);
	}
	else
	{
		switch(ColumnType)
		{
		case mpo_Parent->en_ID:
			qsort(mpul_Adr, mpo_Parent->mpt_EditText->ul_Num, sizeof(ULONG), qsort_ID);
			break;

		case mpo_Parent->en_Preview:
			qsort_text = mpo_Parent->mpt_EditText->psz_Text;
			qsort(mpul_Adr, mpo_Parent->mpt_EditText->ul_Num, sizeof(ULONG), qsort_preview);
			break;

		case mpo_Parent->en_IDKey:
		    qsort(mpul_Adr, mpo_Parent->mpt_EditText->ul_Num, sizeof(ULONG), qsort_idkey);
			break;
			
		case mpo_Parent->en_Character:
			qsort( mpul_Adr, mpo_Parent->mpt_EditText->ul_Num, sizeof(ULONG), qsort_character);
			break;
		
        case mpo_Parent->en_Priority:
			qsort(mpul_Adr, mpo_Parent->mpt_EditText->ul_Num, sizeof(USHORT), qsort_prio);
			break;

		case mpo_Parent->en_SndFileKey:
			qsort(mpul_Adr, mpo_Parent->mpt_EditText->ul_Num, sizeof(ULONG), qsort_sndfile);
			break;
			
		} 
	}

	for(i = 0; i < (int) mpo_Parent->mpt_EditText->ul_Num; i++)
	{
		mpo_Parent->mpt_EditText->pst_Ids[i].order = ((TEXT_tdst_Id *) mpul_Adr[i]) - mpo_Parent->mpt_EditText->pst_Ids;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::ComputeSizes(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int iii, j, index, X, Y, lines, num_lines;
	CDC *pdc;
	int colnb, coltype, col[100];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_Parent->mpt_EditText) return;

	pdc = GetDC();
	pdc->SelectObject(&M_MF()->mo_Fnt);
	mo_SizeFnt = pdc->GetTextExtent("A");

	/* COLUMN */
	X = 0;
	colnb = 0;
	mpo_Parent->GetEntryColumnInfo();
	for(iii = 0; iii < COLMAX; iii++)
	{
		coltype = mpo_Parent->mst_Ini.ai_ColOrder[iii];
		if(mpo_Parent->mst_Ini.ai_ColSize[coltype] <= 0) continue;
		X += mpo_Parent->mst_Ini.ai_ColSize[coltype];

		coltype++;
		if((coltype == mpo_Parent->en_Preview) || (coltype >= mpo_Parent->en_UserData)) col[colnb++] = coltype;
	}

	/* LINES */
	Y = 0;
	for(iii = 0; iii < (int) mpo_Parent->mpt_EditText->ul_Num; iii++)
	{
		index = mpo_Parent->mpt_EditText->pst_Ids[iii].order;
		num_lines = 1;
		for(j = 0; j < colnb; j++)
		{
			lines = ETEXT_CountLine(mpo_Parent->CEL_GetText(index, col[j]));
			if(lines > num_lines) num_lines = lines;
		}

		mpo_Parent->mpt_EditText->pst_Ids[iii].top = Y;
		mpo_Parent->mpt_EditText->pst_Ids[iii].bottom = Y + 4 + (num_lines * mo_SizeFnt.cy);
		Y = mpo_Parent->mpt_EditText->pst_Ids[iii].bottom;
	}

	mo_Size.cx = X;
	mo_Size.cy = Y + 16;
	SetScrollSizes(MM_TEXT, mo_Size);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::EnsureVisible(int i_Line, int i_Col)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int		iii, x, y, left, right;
	CPoint	o_Pos;
	CRect	o_Rect;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_Parent->mpt_EditText) return;

	o_Pos = GetDeviceScrollPosition();
	GetClientRect(&o_Rect);

	/* LINES */
	y = -1;
    iii = i_Line;
	//for(iii = 0; iii < (int) mpo_Parent->mpt_EditText->ul_Num; iii++)
	{
//		if(mpo_Parent->mpt_EditText->pst_Ids[iii].order != i_Line) continue;

		if(mpo_Parent->mpt_EditText->pst_Ids[iii].bottom > o_Pos.y + o_Rect.Height() - 16)
			y = mpo_Parent->mpt_EditText->pst_Ids[iii].bottom - o_Rect.Height() + 16;

		if(mpo_Parent->mpt_EditText->pst_Ids[iii].top < o_Pos.y + 16) y = mpo_Parent->mpt_EditText->pst_Ids[iii].top;
		//break;
	}

	/* COLUMN */
	right = 0;
	mpo_Parent->GetEntryColumnInfo();
	for(iii = 0; iii < COLMAX; iii++)
	{
		x = mpo_Parent->mst_Ini.ai_ColOrder[iii];
		if(mpo_Parent->mst_Ini.ai_ColSize[ x ] <= 0) continue;
		left = right;
		right += mpo_Parent->mst_Ini.ai_ColSize[x];
		if(x + 1 == i_Col) break;
	}

	x = -1;
	if(right > o_Pos.x + o_Rect.Width()) x = right - o_Rect.Width();
	if(left < o_Pos.x) x = left;

	if((x != -1) || (y != -1))
	{
		if(y != -1) o_Pos.y = y;
		if(x != -1) o_Pos.x = x;
		ScrollToPosition(o_Pos);
		Invalidate();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ETEXT_cl_Scroll::UpdateSel(int _i_Line, int _i_Shift, int _i_Ctrl, int _i_Down)
{
	/*~~~~~~~~~~~~~~~~~~*/
	int		i, j, i_Entry;
	BOOL	b_Change;
	/*~~~~~~~~~~~~~~~~~~*/

	b_Change = FALSE;

	if((_i_Down == 0) && (Sel_Down || (_i_Line != Sel_DownLine))) return FALSE;

	if(_i_Shift && (mi_LastEntrySel != -1))
	{
		for(i = 0; i < (int) mpo_Parent->mpt_EditText->ul_Num; i++)
			if(mpo_Parent->mpt_EditText->pst_Ids[i].order == mi_LastEntrySel) break;
		if(i > _i_Line)
		{
			j = i;
			i = _i_Line;
			_i_Line = j;
		}

		for(j = 0; j < (int) mpo_Parent->mpt_EditText->ul_Num; j++)
		{
			i_Entry = mpo_Parent->mpt_EditText->pst_Ids[j].order;
			if((j >= i) && (j <= _i_Line))
			{
				if(!mpo_Parent->mpt_EditText->pst_Ids[i_Entry].sel)
				{
					b_Change = TRUE;
					mpo_Parent->mpt_EditText->pst_Ids[i_Entry].sel = 1;
				}
			}
			else if(mpo_Parent->mpt_EditText->pst_Ids[i_Entry].sel)
			{
				b_Change = TRUE;
				mpo_Parent->mpt_EditText->pst_Ids[i_Entry].sel = 0;
			}
		}
	}
	else if(_i_Ctrl)
	{
		i_Entry = mpo_Parent->mpt_EditText->pst_Ids[mi_HitTest_Line].order;
		mi_LastEntrySel = i_Entry;
		if(!mpo_Parent->mpt_EditText->pst_Ids[i_Entry].sel)
		{
			mpo_Parent->mpt_EditText->pst_Ids[i_Entry].sel = 1;
			b_Change = TRUE;
		}
		else
		{
			if(_i_Down == 0)
			{
				mpo_Parent->mpt_EditText->pst_Ids[i_Entry].sel = 0;
				b_Change = TRUE;
			}
		}
	}
	else
	{
		if(_i_Line >= 0)
			i_Entry = mpo_Parent->mpt_EditText->pst_Ids[_i_Line].order;
		else
			i_Entry = -1;
		mi_LastEntrySel = i_Entry;

		if((_i_Down != 1) || !mpo_Parent->mpt_EditText->pst_Ids[i_Entry].sel)
		{
			for(i = 0; i < (int) mpo_Parent->mpt_EditText->ul_Num; i++)
			{
				if(i == i_Entry)
				{
					if(!mpo_Parent->mpt_EditText->pst_Ids[i].sel) b_Change = TRUE;
					mpo_Parent->mpt_EditText->pst_Ids[i].sel = 1;
				}
				else
				{
					if(mpo_Parent->mpt_EditText->pst_Ids[i].sel) b_Change = TRUE;
					mpo_Parent->mpt_EditText->pst_Ids[i].sel = 0;
				}
			}
		}
	}

	if(mi_LastEntrySel != -1)
	{
		mpo_Parent->mi_FindLine = mi_LastEntrySel;
		mpo_Parent->mi_FindCol = -1;
	}

	if(_i_Down == 1)
	{
		Sel_Down = b_Change;
		Sel_DownLine = _i_Line;
		Sel_DownColumn = mi_HitTest_Column;
	}

	Sel_BlockStart = Sel_BlockEnd = -1;
	for(i = 0; i < (int) mpo_Parent->mpt_EditText->ul_Num; i++)
	{
		j = mpo_Parent->mpt_EditText->pst_Ids[i].order;
		if(mpo_Parent->mpt_EditText->pst_Ids[j].sel)
		{
			if(Sel_BlockStart == -1)
				Sel_BlockStart = i;
			else if(Sel_BlockEnd != -1)
			{
				Sel_BlockStart = -1;
				break;
			}
		}
		else
		{
			if((Sel_BlockStart != -1) && (Sel_BlockEnd == -1)) Sel_BlockEnd = i - 1;
		}
	}

	return b_Change;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ETEXT_cl_Scroll::MigratePickRank(void)
{
	/*~~~~~~~~~~~*/
	int ColumnType;
	/*~~~~~~~~~~~*/

	ColumnType = mpo_Parent->mst_Ini.ai_ColOrder[Column[mi_HitTest_Column]] + 1;

	if(ColumnType == mpo_Parent->en_IDKey) return FALSE;
	if(ColumnType == mpo_Parent->en_TxgFileKey) return FALSE;
	if(ColumnType == mpo_Parent->en_TxlFileKey) return FALSE;
	if(ColumnType == mpo_Parent->en_LabelAuto) return FALSE;

	/* pas la meme colonne : déplace le contenu de la sélection */
	if(Sel_DownColumn != mi_HitTest_Column) return FALSE;

	/* rank : déplace / copie des lignes */
	if(ColumnType == mpo_Parent->en_Rank)
	{
		if(mi_OrderColumn + 1 == mpo_Parent->en_Rank)
		{
			Invalidate();

			if((Sel_BlockStart >= 0) && (mi_HitTest_Line >= 0) && (Sel_BlockEnd >= 0))
			{
				mb_RankMoving = TRUE;
				return TRUE;
			}
		}

		return FALSE;
	}

	/* tirette */
	if((Sel_BlockStart != -1) && (Sel_BlockEnd == Sel_DownLine) && (Sel_DownLine != mi_HitTest_Line))	/* &&
																										 * (mi_HitTest_Line
																										 * ==
																										 * Sel_DownLine
																										 * 1)) */
	{
		mi_Capture = CAPTURE_TIRETTE;
		Tirette_StartLine = Sel_BlockStart;
		Tirette_Line = Tirette_EndLine = Sel_BlockEnd;
		Tirette_ColumnType = ColumnType;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ETEXT_cl_Scroll::b_CanCloseWithKey(void)
{
	if(mi_Capture) return FALSE;
	if(mb_InMenu) return FALSE;

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Scroll::DoTirette(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString			o_Content;
	int				i, j, Tirette_Increment, linetgt, linesrc;
	ULONG			ul_Value;
	char			*sz_Value, *sz_Ref, *sz_End;
	int				i_Type, i_Start, i_Next, i_DecDone, i_Dec, i_NbChar;
	unsigned int	ui_MinDigit;
	char			sz_Format[64], *sz_Text;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* just to have type of data of tirette column */
	i_Type = mpo_Parent->CEL_GetContent(Tirette_StartLine, Tirette_ColumnType, &ul_Value);

	if(Tirette_EndLine < Tirette_Line)
	{
		i = Tirette_StartLine;
		Tirette_StartLine = Tirette_Line;
		Tirette_Line = i;
		Tirette_Increment = -1;
	}
	else
	{
		Tirette_Increment = 1;
	}

	if(i_Type == ETEXT_Content_None) return;

	if(i_Type == ETEXT_Content_Int)
	{
		j = Tirette_StartLine;
		for(i = Tirette_Line + Tirette_Increment; i != Tirette_EndLine + Tirette_Increment; i += Tirette_Increment)
		{
			linetgt = mpo_Parent->mpt_EditText->pst_Ids[i].order;
			linesrc = mpo_Parent->mpt_EditText->pst_Ids[j].order;
			if(j == Tirette_Line)
				j = Tirette_StartLine;
			else
				j += Tirette_Increment;

			mpo_Parent->CEL_GetContent(linesrc, Tirette_ColumnType, &ul_Value);
			mpo_Parent->CEL_SetInt(linetgt, Tirette_ColumnType, ul_Value);
		}
	}

	if(i_Type == ETEXT_Content_String)
	{
		sz_Ref = NULL;
		for(i = Tirette_StartLine; i != Tirette_Line + Tirette_Increment; i += Tirette_Increment)
		{
			linesrc = mpo_Parent->mpt_EditText->pst_Ids[i].order;
			sz_Value = mpo_Parent->CEL_GetText(linesrc, Tirette_ColumnType);

			if(sz_Ref == NULL)
			{
				sz_Ref = sz_Value;
				sz_End = sz_Ref + strlen(sz_Ref) - 1;
				while(isdigit(0xFF & *sz_End) && (sz_End >= sz_Ref)) sz_End--;
				sz_End++;
				sz_Ref = sz_Value;
				i_NbChar = sz_End - sz_Ref;
				ui_MinDigit = strlen(sz_Ref) - i_NbChar;
				i_Start = atoi(sz_End);
				i_DecDone = 0;
			}
			else
			{
				for(j = 0; j < i_NbChar; j++)
					if(sz_Value[j] != sz_Ref[j]) break;
				if(j != i_NbChar) break;
				if(strlen(sz_Value) - i_NbChar < ui_MinDigit) ui_MinDigit = strlen(sz_Value) - i_NbChar;
				i_Next = 0;
				while(isdigit(0xFF & sz_Value[j]))
				{
					i_Next = (i_Next * 10) + (sz_Value[j] - '0');
					j++;
				}

				if(sz_Value[j] != 0) break;

				if(i_DecDone == 0)
				{
					i_DecDone = 1;
					i_Dec = i_Next - i_Start;
					i_Start = i_Next;
				}
				else
				{
					if(i_Next - i_Start != i_Dec) break;
					i_Start = i_Next;
				}
			}
		}

		if((i == Tirette_Line + Tirette_Increment) && i_DecDone && i_NbChar)
		{
			sz_Text = (char *) malloc(i_NbChar + 100);
			L_memcpy(sz_Text, sz_Ref, i_NbChar);
			sz_End = sz_Text + i_NbChar;
			sprintf(sz_Format, "%%0%dd", ui_MinDigit);

			i_Next = i_Start;
			for(i = Tirette_Line + Tirette_Increment; i != Tirette_EndLine + Tirette_Increment; i += Tirette_Increment)
			{
				linetgt = mpo_Parent->mpt_EditText->pst_Ids[i].order;
				i_Next += i_Dec;
				sprintf(sz_End, sz_Format, (i_Next < 0) ? -i_Next : i_Next);
				mpo_Parent->CEL_SetString(linetgt, Tirette_ColumnType, sz_Text);
			}

			L_free(sz_Text);
		}
		else
		{
			j = Tirette_StartLine;
			for(i = Tirette_Line + Tirette_Increment; i != Tirette_EndLine + Tirette_Increment; i += Tirette_Increment)
			{
				linetgt = mpo_Parent->mpt_EditText->pst_Ids[i].order;
				linesrc = mpo_Parent->mpt_EditText->pst_Ids[j].order;
				if(j == Tirette_Line)
					j = Tirette_StartLine;
				else
					j += Tirette_Increment;

				sz_Value = mpo_Parent->CEL_GetText(linesrc, Tirette_ColumnType);
				mpo_Parent->CEL_SetString(linetgt, Tirette_ColumnType, sz_Value);
			}
		}
	}
	mpo_Parent->mb_FileIsModified = TRUE;
}
#endif /* ACTIVE_EDITORS */
