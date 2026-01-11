// PERCheckTreeListHeaderCtrl.cpp : implementation file
//

#include "precomp.h"
#include "PERCheckTreeListHeaderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTLItem

CTLItem::CTLItem()
{
	m_cEnding = '¶';
	m_itemString = "";
	m_Bold = FALSE;
	m_Color = ::GetSysColor(COLOR_WINDOWTEXT);
	m_bIsEditable=FALSE;
	m_bHasCombo=FALSE;
	m_bIsFolder = FALSE;
}

CTLItem::CTLItem(CTLItem &copyItem)
{
	m_cEnding = copyItem.m_cEnding;
	m_itemString = copyItem.GetItemString();
	m_Bold = copyItem.m_Bold;
	m_Color = copyItem.m_Color;
	itemData = copyItem.itemData;
	m_bIsFolder = copyItem.m_bIsFolder;
}

CString CTLItem::GetSubstring(int m_nSub)
{
	CString m_tmpStr("");
	int i=0, nHits=0;
	int length = m_itemString.GetLength();

	while((i<length) && (nHits<=m_nSub))
	{
		if(m_itemString[i]==m_cEnding)
		{
			nHits++;
		}
		else
			if(nHits==m_nSub)
				m_tmpStr+=m_itemString[i];

		i++;
	}

	if((i>=length) && (nHits<m_nSub))
		return "";
	else
		return m_tmpStr;
}

void CTLItem::SetSubstring(int m_nSub, CString m_sText)
{	
	CString m_tmpStr("");
	int i=0, nHits=0, first=0;
	int length = m_itemString.GetLength();

	while((i<length) && (nHits<=m_nSub))
	{
		if(m_itemString[i]==m_cEnding)
		{
			if(nHits!=m_nSub)
				first = i;
			nHits++;
		}

		i++;
	}

	CString m_newStr("");
	if((nHits>m_nSub) || ((nHits==m_nSub) && (i>=length)))
	{
		// insert in the middle
		if(first!=0)
		{
			m_newStr = m_itemString.Left(first);
			m_newStr += m_cEnding; 
		}
		m_newStr += m_sText;
		if(i<length)
		{
			m_newStr += m_cEnding;
			m_newStr += m_itemString.Right(m_itemString.GetLength()-i);
		}

		m_itemString=m_newStr;
	}
	else
	{
		// insert at the end
		for(i=nHits;i<m_nSub;i++)
			m_itemString+=m_cEnding;
		m_itemString+=m_sText;
	}
}

void CTLItem::SetEditable(BOOL bEditable)
{
	m_bIsEditable=bEditable;
	if (m_bIsEditable)
	{
		m_bHasCombo=FALSE;
	}
}

void CTLItem::SetCombo(BOOL bCombo)
{
	m_bHasCombo=bCombo;
	if (m_bHasCombo)
	{
		m_bIsEditable=FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// EPERCheck_cl_TreeListHeaderCtrl

EPERCheck_cl_TreeListHeaderCtrl::EPERCheck_cl_TreeListHeaderCtrl()
{
	//initialise variables
	m_pImageList = NULL;
}

EPERCheck_cl_TreeListHeaderCtrl::~EPERCheck_cl_TreeListHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(EPERCheck_cl_TreeListHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(EPERCheck_cl_TreeListHeaderCtrl)
	ON_WM_PAINT()	
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EPERCheck_cl_TreeListHeaderCtrl message handlers

void EPERCheck_cl_TreeListHeaderCtrl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	CDC dc;

	dc.Attach( lpDrawItemStruct->hDC );	

	// Save DC
	int nSavedDC = dc.SaveDC();

	// Get the column rect	
	CRect rcLabel( lpDrawItemStruct->rcItem );

	// Set clipping region to limit drawing within column
	CRgn rgn;
	rgn.CreateRectRgnIndirect( &rcLabel );
	dc.SelectObject( &rgn );	
	rgn.DeleteObject();

	// Labels are offset by a certain amount  
	// This offset is related to the width of a space character
	int offset = dc.GetTextExtent(_T(" "), 1 ).cx*2;


	// Draw image from image list

	// Get the column text and format
	TCHAR buf[256];
	HD_ITEM hditem;
	
	hditem.mask = HDI_TEXT | HDI_FORMAT;
	hditem.pszText = buf;
	hditem.cchTextMax = 255;

	GetItem( lpDrawItemStruct->itemID, &hditem );

	CFont font;
	font.CreateFont(14,0,                         // nWidth
    0,                         // nEscapement
    0,                         // nOrientation
    FW_NORMAL,                 // nWeight
    FALSE,                     // bItalic
    FALSE,                     // bUnderline
    0,                         // cStrikeOut
    ANSI_CHARSET,              // nCharSet
    OUT_DEFAULT_PRECIS,        // nOutPrecision
    CLIP_DEFAULT_PRECIS,       // nClipPrecision
    DEFAULT_QUALITY,           // nQuality
    DEFAULT_PITCH,			   // nPitchAndFamily
    NULL);                     // lpszFacename

	dc.SelectObject(font);
	
	// Determine format for drawing column label
	UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP 
						| DT_VCENTER | DT_END_ELLIPSIS ;

	if( hditem.fmt & HDF_CENTER)
		uFormat |= DT_CENTER;
	else if( hditem.fmt & HDF_RIGHT)
		uFormat |= DT_RIGHT;
	else
		uFormat |= DT_LEFT;

	if(!(uFormat & DT_RIGHT))
	{
		// Adjust the rect if the mouse button is pressed on it
		if( lpDrawItemStruct->itemState == ODS_SELECTED )
		{
			rcLabel.left++;
			rcLabel.top += 2;
			rcLabel.right++;
		}

		rcLabel.left += offset;
		rcLabel.right -= offset;
	
	
		// Draw column label
		if( rcLabel.left < rcLabel.right )
			dc.DrawText(buf,-1,rcLabel, uFormat);
	}

	int imageIndex;
	if (m_pImageList && 
		m_mapImageIndex.Lookup( lpDrawItemStruct->itemID, imageIndex ) )
	{
		if( imageIndex != -1 )
		{
			if(uFormat & DT_RIGHT)
			// draw to the left of the label
				m_pImageList->Draw(&dc, imageIndex, 
						CPoint( rcLabel.left + offset,offset/3 ),
						ILD_TRANSPARENT );
			else
			// draw to the right
				m_pImageList->Draw(&dc, imageIndex, 
						CPoint( rcLabel.right - dc.GetTextExtent(_T(buf), 1 ).cx*2,offset/3 ),
						ILD_TRANSPARENT );


			// Now adjust the label rectangle
			IMAGEINFO imageinfo;
			if( m_pImageList->GetImageInfo( imageIndex, &imageinfo ) )
			{
				rcLabel.left += offset/2 + 
					imageinfo.rcImage.right - imageinfo.rcImage.left;
			}
		}
	}

	if(uFormat & DT_RIGHT)
	{
		// Adjust the rect if the mouse button is pressed on it
		if( lpDrawItemStruct->itemState == ODS_SELECTED )
		{
			rcLabel.left++;
			rcLabel.top += 2;
			rcLabel.right++;
		}

		rcLabel.left += offset;
		rcLabel.right -= offset;
		
		
		// Draw column label
		if( rcLabel.left < rcLabel.right )
			dc.DrawText(buf,-1,rcLabel, uFormat);
	}

	// Restore dc
	dc.RestoreDC( nSavedDC );

	// Detach the dc before returning
	dc.Detach();
}

CImageList* EPERCheck_cl_TreeListHeaderCtrl::SetImageList( CImageList* pImageList )
{
	CImageList *pPrevList = m_pImageList;
	m_pImageList = pImageList;
	return pPrevList;
}

int EPERCheck_cl_TreeListHeaderCtrl::GetItemImage( int nItem )
{
	int imageIndex;
	if( m_mapImageIndex.Lookup( nItem, imageIndex ) )
		return imageIndex;
	return -1;
}

void EPERCheck_cl_TreeListHeaderCtrl::SetItemImage( int nItem, int nImage )
{
	// Save the image index
	m_mapImageIndex[nItem] = nImage;

	// Change the item to ownder drawn
	HD_ITEM hditem;

	hditem.mask = HDI_FORMAT;
	GetItem( nItem, &hditem );
	hditem.fmt |= HDF_OWNERDRAW;
	SetItem( nItem, &hditem );

	// Invalidate header control so that it gets redrawn
	Invalidate();
}

void EPERCheck_cl_TreeListHeaderCtrl::Autofit(int nOverrideItemData /*= -1*/, int nOverrideWidth /*= 0*/)
{
	int nItemCount = GetItemCount();
	int nTotalWidthOfColumns = 0;
	int nDifferenceInWidht;
	int nItem;
	HD_ITEM hi;
	CRect rClient;

	if (!m_bAutofit)
		return;

	SetRedraw(FALSE);

	GetParent()->GetClientRect(&rClient);
	if (-1 != nOverrideItemData)
		rClient.right -= nOverrideWidth;

	// Get total width of all columns
	for (nItem = 0; nItem < nItemCount; nItem++)
	{
		if (nItem == nOverrideItemData)	// Don't mess with the item being resized by the user
			continue;

		hi.mask = HDI_WIDTH;
		GetItem(nItem, &hi);

		nTotalWidthOfColumns += hi.cxy;
	}

	if (nTotalWidthOfColumns != rClient.Width())
	{
		nDifferenceInWidht = abs(nTotalWidthOfColumns-rClient.Width());	// We need to shrink/expand all columns!
		
		// Shrink/expand all columns proportionally based on their current size
		for (nItem = 0; nItem < nItemCount; nItem++)
		{
			if (nItem == nOverrideItemData)	// Skip the overrride column if there is one!
				continue;
			
			hi.mask = HDI_WIDTH;
			GetItem(nItem, &hi);

			hi.mask = HDI_WIDTH;
			hi.cxy = (hi.cxy * rClient.Width()) / nTotalWidthOfColumns;

			SetItem(nItem, &hi);
		}
	}

	SetRedraw(TRUE);
	Invalidate();
}

void EPERCheck_cl_TreeListHeaderCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CWnd::DefWindowProc( WM_PAINT, (WPARAM)dc.m_hDC, 0 );	
}

void EPERCheck_cl_TreeListHeaderCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	CHeaderCtrl::OnLButtonDown(nFlags, point);
}

