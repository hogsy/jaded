// ListCtrlStyled.cpp : implementation file
//

#include "precomp.h"
#include "ListCtrlStyled.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CListCtrlStyled::CListCtrlStyled()
{ 	columns.SetSize(0);
	columns.RemoveAll();

	mi_SubClassed = 0;
	m_Default_pCFont = NULL;

	// Set default HIGHLIGHT colors
	//
	m_highlighttext = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_highlight = ::GetSysColor(COLOR_HIGHLIGHT);
}

CListCtrlStyled::~CListCtrlStyled()
{ 	// Must free if it's not alreay do
	//
	int nCols = columns.GetSize();
	for(int nCol = 0; nCol < nCols;nCol++)
	{	if(columns[nCol] != NULL)
			this->Free_LS_item(columns[nCol]);
	}

	columns.RemoveAll();
	columns.SetSize(0);
}

BEGIN_MESSAGE_MAP(CListCtrlStyled, CListCtrl)
    //{{AFX_MSG_MAP(CListCtrlStyled)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomdraw)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CListCtrlStyled::OnColumnClick(NMHDR *pNotifyStruct2, LRESULT *result)
{
	NM_LISTVIEW *pNotifyStruct;
	pNotifyStruct = (NM_LISTVIEW *)pNotifyStruct2;
	m_wndFlatHeader.SetSortColumn(pNotifyStruct->iSubItem, FALSE);
}

void CListCtrlStyled::PreSubclassWindow(void) 
{
	CListCtrl::PreSubclassWindow();
}

int CListCtrlStyled::OnCreate(LPCREATESTRUCT lp)
{
	int res = CListCtrl::OnCreate(lp);
	return res;
}

void CListCtrlStyled::OnDestroy()
{	// Must free all internal objects for prevent memory leaks
	//
	this->DeleteAllItems();
	CListCtrl::OnDestroy();
}

// ****************
// ** InsertItem **
// ****************
//
int CListCtrlStyled::InsertItem( const LVITEM* pItem )
{	// So we need to construct our style object for this item, store
	// it into the lParam of pItem. But before this we must store it's lParam
	// for mask this usage for the programmer. lparam must be usable by the same
	// way that the original CListCtrl. then we store it into our style object

	// For let after to modify lParam (pItem is a const pointer)
	//
	LVITEM pItem2 = (LVITEM) *pItem;

	// Create a new LS_item for store data
	//
	LS_item * lpLS_item;
	lpLS_item = NULL;
	lpLS_item = new LS_item;

	Init_LS_item(lpLS_item);		// Init the structure

	// Store lParam from the pItem
	//
	if(pItem2.mask & LVIF_PARAM)
	{	lpLS_item->lParam = pItem2.lParam;
		lpLS_item->mParam = true;
	}

	// Set the LS_Item to the lParam of pItem
	//
	pItem2.mask = pItem2.mask | LVIF_PARAM;				// Add LVIF_PARAM option
	pItem2.lParam = (LPARAM)(void *)lpLS_item;			// Link our structure

	// Create structure for each subitems (with default style too)
	//
	int nSubItems = this->GetHeaderCtrl()->GetItemCount() - 1;

	LS_item * lpLS_subitem;
	for(int bcl = 0; bcl < nSubItems ;bcl++)
	{	lpLS_subitem = NULL;
		lpLS_subitem = new LS_item;

		Init_LS_item(lpLS_subitem,false);				// Init the structure

		lpLS_item->subitems.Add(lpLS_subitem);			// Add the structure of this subitem to the list.
	}

	return (CListCtrl::InsertItem( &pItem2 ));
}

int CListCtrlStyled::InsertItem( UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState, UINT nStateMask, int nImage, LPARAM lParam )
{
	// So we must create an LVItem and insert it
	LVITEM lvitem;
	lvitem.mask = nMask;
	lvitem.iItem = nItem;
	lvitem.iSubItem = 0;
	lvitem.pszText = (char*)lpszItem;
	lvitem.cchTextMax = 0;
	if(nMask & LVIF_TEXT)
		lvitem.cchTextMax = strlen(lvitem.pszText);
	lvitem.state = nState;
	lvitem.stateMask = nStateMask;
	lvitem.iImage = nImage;
	lvitem.lParam = lParam;

	// Insert it
	return (this->InsertItem(&lvitem));
}

int CListCtrlStyled::InsertItem( int nItem, LPCTSTR lpszItem )
{ return (this->InsertItem(LVIF_TEXT,nItem,lpszItem,0,0,0,0)); }

int CListCtrlStyled::InsertItem( int nItem, LPCTSTR lpszItem, int nImage )
{ return (this->InsertItem(LVIF_TEXT|LVIF_IMAGE,nItem,lpszItem,0,0,nImage,0)); }

// *************
// ** SetItem **
// *************

BOOL CListCtrlStyled::SetItem( const LVITEM* pItem )
{
	if(pItem->mask & LVIF_PARAM)
	{	// We must assume that lParam member is correctly managed in our derived class
		//
		LVITEM pItem2 = (LVITEM) *pItem;

		// Try to retrieve the root pItem
		//
		LVITEM pRootItem;
		InitLVITEM(pItem2.iItem,0,&pRootItem);

		LS_item * lpLS_item = NULL;
		lpLS_item = (LS_item *)pRootItem.lParam;

		// Management of lParam
		//
		lpLS_item->lParam = pItem2.lParam;
		lpLS_item->mParam = true;

		// No lParam for this subitem
		//
		pItem2.lParam = NULL;
		pItem2.mask = pItem2.mask & (0xFFFF - LVIF_PARAM);

		return (CListCtrl::SetItem( &pItem2 ));
	}

	return (CListCtrl::SetItem( pItem ));
}


BOOL CListCtrlStyled::SetItem(int nItem,int nSubItem,UINT nMask,LPCTSTR lpszItem,int nImage,UINT nState,UINT nStateMask,LPARAM lParam)
{	return (this->SetItem(nItem,nSubItem,nMask,lpszItem,nImage,nState,nStateMask,lParam,0)); }

BOOL CListCtrlStyled::SetItem(int nItem,int nSubItem,UINT nMask,LPCTSTR lpszItem,int nImage,UINT nState,UINT nStateMask,LPARAM lParam,int nIndent)
{
	// So we must create an LVItem and insert it
	LVITEM lvitem;
	lvitem.mask = nMask;
	lvitem.iItem = nItem;
	lvitem.iSubItem = nSubItem;
	lvitem.pszText = (char*)lpszItem;
	lvitem.cchTextMax = 0;
	if(nMask & LVIF_TEXT)
		lvitem.cchTextMax = strlen(lvitem.pszText);
	lvitem.state = nState;
	lvitem.stateMask = nStateMask;
	lvitem.iImage = nImage;
	lvitem.lParam = lParam;
	lvitem.iIndent = nIndent;

	// Insert it
	return (this->SetItem(&lvitem));
}

// ****************
// ** DeleteItem **
// ****************
BOOL CListCtrlStyled::DeleteItem( int nItem )
{
	// We must delete all LS_item before deleting this Item
	// So we must retrieve it for all columns
	//

	// retrieve the LS_item structure for this item
	//
	LVITEM pItem;
	InitLVITEM(nItem,0,&pItem);

	LS_item * lpLS_item = NULL;
	lpLS_item = (LS_item *)pItem.lParam;

	// Free his structure style
	//
	this->Free_LS_item(lpLS_item);

	return (CListCtrl::DeleteItem(nItem));
}

BOOL CListCtrlStyled::DeleteAllItems()
{
	// Get number of Rows
	//
	int nItems = CListCtrl::GetItemCount();

	// Delete Each row (One by One for managed our structure destruction)
	//
	for(int nItem = 0; nItem < nItems; nItem++)
		this->DeleteItem(0);

	// Call the base class DeleteAllItems (maybe some treatments must be do)
	//
	return (CListCtrl::DeleteAllItems());
}

// *********************
// ** Get/SetItemData **
// *********************
DWORD CListCtrlStyled::GetItemData( int nItem )
{	// retrieve the LS_item structure
	//
	LVITEM pItem;
	InitLVITEM(nItem,0,&pItem);

	LS_item * lpLS_item = NULL;
	lpLS_item = (LS_item *)pItem.lParam;

	if(lpLS_item->mParam)
		return (lpLS_item->lParam);
	else
		return NULL;	// No significant if the item mask hasn't LVIF_PARAM
}

BOOL CListCtrlStyled::SetItemData( int nItem, DWORD dwData )
{
	// Retrieve the LS_item structure
	//
	LVITEM pItem;
	InitLVITEM(nItem,0,&pItem);

	LS_item * lpLS_item = NULL;
	lpLS_item = (LS_item *)pItem.lParam;

	// set the new data
	//
	if(lpLS_item->mParam)
		lpLS_item->lParam = dwData;

	// Return false if the LVIF_PARAM isn't set for this item
	//
	return (lpLS_item->mParam);
}

// *************
// ** GetItem **
// *************
BOOL CListCtrlStyled::GetItem( LV_ITEM* pItem )
{
	/*
	typedef struct _LV_ITEM {
    UINT   mask;
    int    iItem;
    int    iSubItem;
    UINT   state;
    UINT   stateMask;
    LPSTR  pszText;
    int    cchTextMax;
    int    iImage;
    LPARAM lParam;       // 32-bit value to associate with item
	} LV_ITEM;
  */

	// Retrieve Information from Base class
	//
	BOOL Result = CListCtrl::GetItem(pItem);
	if(Result)
	{	// Replace the lParam value
		//
		LS_item * lpLS_item = NULL;
		lpLS_item = (LS_item *)pItem->lParam;

		if(lpLS_item->mParam)
		{	// Ok set the original lParam
			//
			pItem->lParam = lpLS_item->lParam;
		}
		else
		{	// No lParam member
			//
			pItem->lParam = NULL;
			pItem->mask = pItem->mask & (0xFFFF - LVIF_PARAM);
		}
	}

	return Result;
}

// *********************************
// ** InsertColumn / DeleteColumn **
// *********************************
int CListCtrlStyled::InsertColumn( int nCol, const LVCOLUMN* pColumn )
{
	int index = CListCtrl::InsertColumn(nCol,pColumn);
	if(index == -1)
		return index;

	{
		if(!mi_SubClassed)
		{
			m_wndFlatHeader.SubclassWindow(GetHeaderCtrl()->m_hWnd);
			mi_SubClassed = 1;
		}
	}

	// Adjust Size of columns Array
	//
	this->columns.SetSize( this->GetHeaderCtrl()->GetItemCount() );

	// Must update the "columns" array for hold a custom column style if needed
	//
	this->columns.InsertAt(index,NULL,1);

	// We must adjust all LS_item into the CListCtrl
	//
	int nRows = CListCtrl::GetItemCount();

	// Variables needed
	LVITEM pItem;
	LS_item * lpLS_item;
	LS_item * lpLS_subitem;

	for(int nItem = 0; nItem < 	nRows; nItem++)
	{	// Retrieve the LS_item for this item
		//
		InitLVITEM(nItem,0,&pItem);

		lpLS_item = NULL;
		lpLS_item = (LS_item *)pItem.lParam;

		// Create the style for the new subitem
		//
		lpLS_subitem = NULL;
		lpLS_subitem = new LS_item;

		this->Init_LS_item(lpLS_subitem,false);  // Init the structure

		// Insert it into his correct place (shift all element above)
		//
		if(index > 0)
			lpLS_item->subitems.InsertAt(index - 1,lpLS_subitem,1);
		else
			lpLS_item->subitems.InsertAt(index,lpLS_subitem,1);
	}

	return index;
}

int CListCtrlStyled::InsertColumn( int nCol, LPCTSTR lpszColumnHeading, int nFormat, int nWidth, int nSubItem)
{	// Create an LVCOLUMN item
	/*
	typedef struct _LVCOLUMN {
   	 UINT mask;
   	 int fmt;
   	 int cx;
   	 LPTSTR pszText;
   	 int cchTextMax;
   	 int iSubItem;
	#if (_WIN32_IE >= 0x0300)
   	 int iImage;
   	 int iOrder;
	#endif
	} LVCOLUMN, *LPLVCOLUMN;
	*/
	LVCOLUMN lvColumn;

	lvColumn.mask = LVCF_FMT | LVCF_TEXT;
	if(nWidth != -1)
		lvColumn.mask = lvColumn.mask | LVCF_WIDTH;
	if(nSubItem != -1)
		lvColumn.mask = lvColumn.mask | LVCF_SUBITEM;

	lvColumn.fmt = nFormat;
	lvColumn.cx = nWidth;
	lvColumn.pszText = (char*)lpszColumnHeading;
	lvColumn.cchTextMax  = strlen( lvColumn.pszText );
	lvColumn.iSubItem = nSubItem;

	return (this->InsertColumn(nCol,&lvColumn));
}

BOOL CListCtrlStyled::DeleteColumn( int nCol )
{
	if(nCol >= columns.GetSize()) return TRUE;

	// Start be delete the Column Style if needed
	//
	LS_item * lpLS_col = NULL;
	lpLS_col = this->columns[nCol];

	if(lpLS_col != NULL)
		this->Free_LS_item(lpLS_col);

	// Update the Array
	//
	columns.RemoveAt(nCol,1);

	// Some Subitems will be deleted, we must delete also theses structure style
	//
	int nItems = CListCtrl::GetItemCount();
	for(int nItem = 0;nItem < nItems;nItem++)
	{ 	LVITEM pItem;
		InitLVITEM(nItem,0,&pItem);

		LS_item * lpLS_root = NULL;
		LS_item * lpLS_item = NULL;
		lpLS_root = (LS_item*) pItem.lParam;

		if(nCol > 0)
		{	lpLS_item = lpLS_root->subitems[nCol - 1];
			lpLS_root->subitems.RemoveAt(nCol - 1,1);
		}
		else
		{	if( this->GetHeaderCtrl()->GetItemCount() > 1)
			{	lpLS_item = lpLS_root->subitems[0];
				lpLS_root->subitems.RemoveAt(0,1);
			}
			else
			{	this->DeleteAllItems();
				return (CListCtrl::DeleteColumn(nCol ));
			}
		}

		this->Free_LS_item(lpLS_item);
	}

	// We can delete the Column now
	//
	return (CListCtrl::DeleteColumn(nCol));
}

// *************************
// ** SetColumnOrderArray **
// *************************
BOOL CListCtrlStyled::SetColumnOrderArray( int iCount, LPINT piArray )
{	// Must rearrange the "columns" array
	//
	CArray<struct iLS_item * , struct iLS_item *> old;

	old.SetSize( columns.GetSize() );
	old.RemoveAll();
	old.Copy( this->columns );

	for( int nItem = 0; nItem < iCount; nItem++)
		this->columns.SetAt( nItem, old[ piArray[nItem] ] );

	old.RemoveAll();

	return (CListCtrl::SetColumnOrderArray(iCount,piArray));
}

// ***************
// ** SortItems **
// ***************

PFNLVCOMPARE LIS_CompFunc;															// Store the user-defined callback function

int CALLBACK LIS_CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{	// Must call the origine compare function
	//
	LPARAM lp1;					// the real lParam1
	LPARAM lp2;					// the real lParam2
	LS_item * lpLS_item;

	// Set the correct lParam1
	//
	lpLS_item = NULL;
	lpLS_item = (LS_item*) lParam1;
	lp1 = lpLS_item->lParam;

	// Set the correct lParam2
	//
	lpLS_item = NULL;
	lpLS_item = (LS_item*) lParam2;
	lp2 = lpLS_item->lParam;

	// Call now the real callback function defined by the user
	//
	return LIS_CompFunc(lp1,lp2,lParamSort);
}

BOOL CListCtrlStyled::SortItems( PFNLVCOMPARE pfnCompare, DWORD dwData )
{	LIS_CompFunc = pfnCompare;
	PFNLVCOMPARE callback_func = LIS_CompareFunc;
	return (CListCtrl::SortItems( callback_func, dwData ));
}

// **************
// ** FindItem **
// **************
int CListCtrlStyled::FindItem( LV_FINDINFO* pFindInfo, int nStart)
{	/*
	typedef struct _LV_FINDINFO {
		UINT flags;    //see below
		LPCSTR psz;    //see below
		LPARAM lParam; //see below
	} LV_FINDINFO;
	*/

	if(pFindInfo->flags & LVFI_PARAM)
	{	// We must implement this method ourselve because we can't access directly to the lParam member
		// When LVFI_PARAM is used, all other flag are ignored
		//

		int nItems = this->GetItemCount();
		for(int nItem = nStart + 1; nItem < nItems; nItem++)
		{	// Get lParam value for this item
			//
			LVITEM pItem;
			InitLVITEM(nItem,0,&pItem);

			LS_item * lpLS_item = NULL;
			lpLS_item = (LS_item *)pItem.lParam;

			if(lpLS_item->lParam == pFindInfo->lParam)
				return nItem;
		}
		return -1;
	}
	else
	{	// We can call the base class method
		//
		return CListCtrl::FindItem(pFindInfo,nStart);
	}
}

// ********************************
// ** New STYLE Methods on Items **
// ********************************

void CListCtrlStyled::SetItemTxtColor(int nItem,int nSubItem,COLORREF txtColor,bool redraw)
{	// We must retrieve the Style info structure of this item
	//
	LVITEM pItem;
	InitLVITEM(nItem,0,&pItem);

	LS_item * lpLS_item = NULL;
	lpLS_item = (LS_item*) pItem.lParam;

	if(nSubItem > 0)
		lpLS_item = lpLS_item->subitems[nSubItem - 1];

	// no we can update the style
	//
	lpLS_item->txtColor = txtColor;

	// Redraw it
	if(redraw)	CListCtrl::Update(nItem);
}

void CListCtrlStyled::SetItemBgColor(int nItem,int nSubItem,COLORREF txtBgColor,bool redraw)
{	// We must retrieve the Style info structure of this item
	//
	LVITEM pItem;
	InitLVITEM(nItem,0,&pItem);

	LS_item * lpLS_item = NULL;
	lpLS_item = (LS_item*) pItem.lParam;

	if(nSubItem > 0)
		lpLS_item = lpLS_item->subitems[nSubItem - 1];

	// no we can update the style
	//
	lpLS_item->bgColor = txtBgColor;

	// Redraw it
	if(redraw)	CListCtrl::Update(nItem);
}

void CListCtrlStyled::SetItemStyle(int nItem,int nSubItem,DWORD Style,bool redraw)
{
	// We must retrieve the Style info structure of this item
	//
	LVITEM pItem;
	InitLVITEM(nItem,0,&pItem);

	LS_item * lpLS_item = NULL;
	lpLS_item = (LS_item*) pItem.lParam;

	if(nSubItem > 0)
		lpLS_item = lpLS_item->subitems[ nSubItem - 1];

	// no we can update the style
	//
	lpLS_item->StyleFlag = Style;

	DWORD mask = LIS_BOLD | LIS_ITALIC | LIS_UNDERLINE| LIS_STROKE ;
	lpLS_item->in_use = (Style & mask) > 0;

	// if any font exist for this item then delete it
	//
	this->Free_LS_font(lpLS_item);

	// Redraw it
	if(redraw)	CListCtrl::Update(nItem);
}

void CListCtrlStyled::SetItemFont(int nItem,int nSubItem,CFont * pFont,bool redraw)
{
	// We must retrieve the Style info structure of this item
	//
	LVITEM pItem;
	InitLVITEM(nItem,0,&pItem);

	LS_item * lpLS_item = NULL;
	lpLS_item = (LS_item*) pItem.lParam;

	if(nSubItem > 0)
		lpLS_item = lpLS_item->subitems[ nSubItem - 1];

	// if any internal font exist for this item then delete it
	//
	Free_LS_font(lpLS_item);

	lpLS_item->cfont = pFont;
	lpLS_item->ifont = false;

	lpLS_item->in_use = true;

	// Redraw it
	if(redraw)	CListCtrl::Update(nItem);
}

// *****************************************
// ** New STYLE Methods on SELECTED Items **
// *****************************************
void CListCtrlStyled::SetItemSelectedStyle(int nItem,int nSubItem,DWORD Style,bool redraw)
{	// We must retrieve the Style info structure of this item
	//
	LVITEM pItem;
	InitLVITEM(nItem,0,&pItem);

	LS_item * lpLS_item = NULL;
	lpLS_item = (LS_item*) pItem.lParam;

	if(nSubItem > 0)
		lpLS_item = lpLS_item->subitems[ nSubItem - 1];

	// Take the selected style structure
	//
	if(lpLS_item->selected_style == NULL)
	{	// Create a structure style
		//
		lpLS_item->selected_style = new LS_item;
		this->Init_LS_item( lpLS_item->selected_style );
	}
	lpLS_item = lpLS_item->selected_style;

	// no we can update the style
	//
	lpLS_item->StyleFlag = Style;

	DWORD mask = LIS_BOLD | LIS_ITALIC | LIS_UNDERLINE| LIS_STROKE ;
	lpLS_item->in_use = (Style & mask) > 0;

	// if any font exist for this item then delete it
	//
	this->Free_LS_font(lpLS_item);

	// Redraw it
	if(redraw)	CListCtrl::Update(nItem);
}

void CListCtrlStyled::SetItemSelectedTxtColor(int nItem,int nSubItem,COLORREF txtColor,bool redraw)
{	// We must retrieve the Style info structure of this item
	//
	LVITEM pItem;
	InitLVITEM(nItem,0,&pItem);

	LS_item * lpLS_item = NULL;
	lpLS_item = (LS_item*) pItem.lParam;

	if(nSubItem > 0)
		lpLS_item = lpLS_item->subitems[nSubItem - 1];

	// Take the selected style structure
	//
	if(lpLS_item->selected_style == NULL)
	{	// Create a structure style
		//
		lpLS_item->selected_style = new LS_item;
		this->Init_LS_item( lpLS_item->selected_style );
	}
	lpLS_item = lpLS_item->selected_style;

	// no we can update the style
	//
	lpLS_item->txtColor = txtColor;

	// Redraw it
	if(redraw)	CListCtrl::Update(nItem);
}

void CListCtrlStyled::SetItemSelectedBgColor(int nItem,int nSubItem,COLORREF txtBgColor,bool redraw)
{	// We must retrieve the Style info structure of this item
	//
	LVITEM pItem;
	InitLVITEM(nItem,0,&pItem);

	LS_item * lpLS_item = NULL;
	lpLS_item = (LS_item*) pItem.lParam;

	if(nSubItem > 0)
		lpLS_item = lpLS_item->subitems[nSubItem - 1];

	// Take the selected style structure
	//
	if(lpLS_item->selected_style == NULL)
	{	// Create a structure style
		//
		lpLS_item->selected_style = new LS_item;
		this->Init_LS_item( lpLS_item->selected_style );
	}
	lpLS_item = lpLS_item->selected_style;

	// no we can update the style
	//
	lpLS_item->bgColor = txtBgColor;

	// Redraw it
	if(redraw)	CListCtrl::Update(nItem);
}

void CListCtrlStyled::SetItemSelectedFont(int nItem,int nSubItem,CFont * pFont,bool redraw)
{
	// We must retrieve the Style info structure of this item
	//
	LVITEM pItem;
	InitLVITEM(nItem,0,&pItem);

	LS_item * lpLS_item = NULL;
	lpLS_item = (LS_item*) pItem.lParam;

	if(nSubItem > 0)
		lpLS_item = lpLS_item->subitems[ nSubItem - 1];

	// Take the selected style structure
	//
	if(lpLS_item->selected_style == NULL)
	{	// Create a structure style
		//
		lpLS_item->selected_style = new LS_item;
		this->Init_LS_item( lpLS_item->selected_style );
	}
	lpLS_item = lpLS_item->selected_style;

	// if any internal font exist for this item then delete it
	//
	Free_LS_font(lpLS_item);

	lpLS_item->cfont = pFont;
	lpLS_item->ifont = false;

	lpLS_item->in_use = true;

	// Redraw it
	if(redraw)	CListCtrl::Update(nItem);
}

// *******************************
// ** New STYLE Methods on Rows **
// *******************************
void CListCtrlStyled::SetRowStyle(int nRow,DWORD Style,bool redraw)
{	// We must retrieve the Style info structure of this item
	//
	LVITEM pItem;
	InitLVITEM(nRow,0,&pItem);

	LS_item * lpLS_item = NULL;
	lpLS_item = (LS_item*) pItem.lParam;

	// Verify if a style for this Row already exist or not
	//
	LS_item * lpLS_row = NULL;
	lpLS_row = lpLS_item->row_style;

	if(lpLS_row == NULL)
	{	// We must create one
		//
		lpLS_row = new LS_item;
		this->Init_LS_item(lpLS_row,false);

		// attach to the item
		//
		lpLS_item->row_style = lpLS_row;
	}

	// no we can update the style
	//
	lpLS_row->StyleFlag = Style;

	DWORD mask = LIS_BOLD | LIS_ITALIC | LIS_UNDERLINE| LIS_STROKE ;
	lpLS_row->in_use = (Style & mask) > 0;

	// if any font exist for this item then delete it
	//
	this->Free_LS_font(lpLS_row);

	// Redraw it
	if(redraw)	CListCtrl::Update(nRow);
}

void CListCtrlStyled::SetRowTxtColor(int nRow,COLORREF txtColor,bool redraw)
{	// We must retrieve the Style info structure of this item
	//
	LVITEM pItem;
	InitLVITEM(nRow,0,&pItem);

	LS_item * lpLS_item = NULL;
	lpLS_item = (LS_item*) pItem.lParam;

	// Verify if a style for this Row already exist or not
	//
	LS_item * lpLS_row = NULL;
	lpLS_row = lpLS_item->row_style;

	if(lpLS_row == NULL)
	{	// We must create one
		//
		lpLS_row = new LS_item;
		this->Init_LS_item(lpLS_row,false);

		// attach to the item
		//
		lpLS_item->row_style = lpLS_row;
	}

	// no we can update the style
	//
	lpLS_row->txtColor = txtColor;

	// Redraw it
	if(redraw)	CListCtrl::Update(nRow);
}

void CListCtrlStyled::SetRowBgColor(int nRow,COLORREF txtBgColor,bool redraw)
{	// We must retrieve the Style info structure of this item
	//
	LVITEM pItem;
	InitLVITEM(nRow,0,&pItem);

	LS_item * lpLS_item = NULL;
	lpLS_item = (LS_item*) pItem.lParam;

	// Verify if a style for this Row already exist or not
	//
	LS_item * lpLS_row = NULL;
	lpLS_row = lpLS_item->row_style;

	if(lpLS_row == NULL)
	{	// We must create one
		//
		lpLS_row = new LS_item;
		this->Init_LS_item(lpLS_row,false);

		// attach to the item
		//
		lpLS_item->row_style = lpLS_row;
	}

	// no we can update the style
	//
	lpLS_row->bgColor = txtBgColor;

	// Redraw it
	if(redraw)	CListCtrl::Update(nRow);
}

void CListCtrlStyled::SetRowFont(int nRow,CFont * pFont,bool redraw)
{	// We must retrieve the Style info structure of this item
	//
	LVITEM pItem;
	InitLVITEM(nRow,0,&pItem);

	LS_item * lpLS_item = NULL;
	lpLS_item = (LS_item*) pItem.lParam;

	// Verify if a style for this Row already exist or not
	//
	LS_item * lpLS_row = NULL;
	lpLS_row = lpLS_item->row_style;

	if(lpLS_row == NULL)
	{	// We must create one
		//
		lpLS_row = new LS_item;
		this->Init_LS_item(lpLS_row,false);

		// attach to the item
		//
		lpLS_item->row_style = lpLS_row;
	}

	// if any internal font exist for this item then delete it
	//
	this->Free_LS_font(lpLS_row);

	// no we can update the style
	//
	lpLS_row->cfont = pFont;
	lpLS_row->ifont = false;

	lpLS_row->in_use = true;

	// Redraw it
	if(redraw)	CListCtrl::Update(nRow);
}

// ****************************************
// ** New STYLE Methods on SELECTED Rows **
// ****************************************
void CListCtrlStyled::SetRowSelectedStyle(int nRow,DWORD Style,bool redraw)
{	// We must retrieve the Style info structure of this item
	//
	LVITEM pItem;
	InitLVITEM(nRow,0,&pItem);

	LS_item * lpLS_item = NULL;
	lpLS_item = (LS_item*) pItem.lParam;

	// Verify if a style for this Row already exist or not
	//
	LS_item * lpLS_row = NULL;
	lpLS_row = lpLS_item->row_style;

	if(lpLS_row == NULL)
	{	// We must create one
		//
		lpLS_row = new LS_item;
		this->Init_LS_item(lpLS_row,false);

		// attach to the item
		//
		lpLS_item->row_style = lpLS_row;
	}

	// Take the selected style structure
	//
	if(lpLS_row->selected_style == NULL)
	{	// Create a structure style
		//
		lpLS_row->selected_style = new LS_item;
		this->Init_LS_item( lpLS_row->selected_style );
	}
	lpLS_row = lpLS_row->selected_style;

	// no we can update the style
	//
	lpLS_row->StyleFlag = Style;

	DWORD mask = LIS_BOLD | LIS_ITALIC | LIS_UNDERLINE| LIS_STROKE ;
	lpLS_row->in_use = (Style & mask) > 0;

	// if any font exist for this item then delete it
	//
	this->Free_LS_font(lpLS_row);

	// Redraw it
	if(redraw)	CListCtrl::Update(nRow);
}

void CListCtrlStyled::SetRowSelectedTxtColor(int nRow,COLORREF txtColor,bool redraw)
{	// We must retrieve the Style info structure of this item
	//
	LVITEM pItem;
	InitLVITEM(nRow,0,&pItem);

	LS_item * lpLS_item = NULL;
	lpLS_item = (LS_item*) pItem.lParam;

	// Verify if a style for this Row already exist or not
	//
	LS_item * lpLS_row = NULL;
	lpLS_row = lpLS_item->row_style;

	if(lpLS_row == NULL)
	{	// We must create one
		//
		lpLS_row = new LS_item;
		this->Init_LS_item(lpLS_row,false);

		// attach to the item
		//
		lpLS_item->row_style = lpLS_row;
	}

	// Take the selected style structure
	//
	if(lpLS_row->selected_style == NULL)
	{	// Create a structure style
		//
		lpLS_row->selected_style = new LS_item;
		this->Init_LS_item( lpLS_row->selected_style );
	}
	lpLS_row = lpLS_row->selected_style;

	// no we can update the style
	//
	lpLS_row->txtColor = txtColor;

	// Redraw it
	if(redraw)	CListCtrl::Update(nRow);
}

void CListCtrlStyled::SetRowSelectedBgColor(int nRow,COLORREF txtBgColor,bool redraw)
{	// We must retrieve the Style info structure of this item
	//
	LVITEM pItem;
	InitLVITEM(nRow,0,&pItem);

	LS_item * lpLS_item = NULL;
	lpLS_item = (LS_item*) pItem.lParam;

	// Verify if a style for this Row already exist or not
	//
	LS_item * lpLS_row = NULL;
	lpLS_row = lpLS_item->row_style;

	if(lpLS_row == NULL)
	{	// We must create one
		//
		lpLS_row = new LS_item;
		this->Init_LS_item(lpLS_row,false);

		// attach to the item
		//
		lpLS_item->row_style = lpLS_row;
	}

	// Take the selected style structure
	//
	if(lpLS_row->selected_style == NULL)
	{	// Create a structure style
		//
		lpLS_row->selected_style = new LS_item;
		this->Init_LS_item( lpLS_row->selected_style );
	}
	lpLS_row = lpLS_row->selected_style;

	// no we can update the style
	//
	lpLS_row->bgColor = txtBgColor;

	// Redraw it
	if(redraw)	CListCtrl::Update(nRow);
}


void CListCtrlStyled::SetRowSelectedFont(int nRow,CFont * pFont,bool redraw)
{	// We must retrieve the Style info structure of this item
	//
	LVITEM pItem;
	InitLVITEM(nRow,0,&pItem);

	LS_item * lpLS_item = NULL;
	lpLS_item = (LS_item*) pItem.lParam;

	// Verify if a style for this Row already exist or not
	//
	LS_item * lpLS_row = NULL;
	lpLS_row = lpLS_item->row_style;

	if(lpLS_row == NULL)
	{	// We must create one
		//
		lpLS_row = new LS_item;
		this->Init_LS_item(lpLS_row,false);

		// attach to the item
		//
		lpLS_item->row_style = lpLS_row;
	}

	// Take the selected style structure
	//
	if(lpLS_row->selected_style == NULL)
	{	// Create a structure style
		//
		lpLS_row->selected_style = new LS_item;
		this->Init_LS_item( lpLS_row->selected_style );
	}
	lpLS_row = lpLS_row->selected_style;

	// if any internal font exist for this item then delete it
	//
	this->Free_LS_font(lpLS_row);

	// no we can update the style
	//
	lpLS_row->cfont = pFont;
	lpLS_row->ifont = false;

	lpLS_row->in_use = true;

	// Redraw it
	if(redraw)	CListCtrl::Update(nRow);
}

// **********************************
// ** New STYLE Methods on Columns **
// **********************************
void CListCtrlStyled::SetColStyle(int nCol,DWORD Style,bool redraw)
{
	// Verify if a style for this Row already exist or not
	//
	LS_item * lpLS_col = NULL;
	lpLS_col = this->columns[nCol];

	if(lpLS_col == NULL)
	{	// We must create one
		//
		lpLS_col = new LS_item;
		this->Init_LS_item(lpLS_col,false);

		// attach to Array
		//
		this->columns.SetAt(nCol, lpLS_col);
	}

	// no we can update the style
	//
	lpLS_col->StyleFlag = Style;

	DWORD mask = LIS_BOLD | LIS_ITALIC | LIS_UNDERLINE| LIS_STROKE ;
	lpLS_col->in_use = (Style & mask) > 0;

	// if any font exist for this item then delete it
	//
	this->Free_LS_font(lpLS_col);

	// Redraw it
	if(redraw)
	{	this->RedrawItems(0, this->GetItemCount() );
		this->UpdateWindow();
	}
}

void CListCtrlStyled::SetColTxtColor(int nCol,COLORREF txtColor,bool redraw)
{
	// Verify if a style for this Row already exist or not
	//
	LS_item * lpLS_col = NULL;
	lpLS_col = this->columns[nCol];

	if(lpLS_col == NULL)
	{	// We must create one
		//
		lpLS_col = new LS_item;
		this->Init_LS_item(lpLS_col,false);

		// attach to Array
		//
		this->columns.SetAt(nCol, lpLS_col);
	}

	// no we can update the style
	//
	lpLS_col->txtColor = txtColor;

	// Redraw it
	if(redraw)
	{	this->RedrawItems(0, this->GetItemCount() );
		this->UpdateWindow();
	}
}

void CListCtrlStyled::SetColBgColor(int nCol,COLORREF txtBgColor,bool redraw)
{
	// Verify if a style for this Row already exist or not
	//
	LS_item * lpLS_col = NULL;
	lpLS_col = this->columns[nCol];

	if(lpLS_col == NULL)
	{	// We must create one
		//
		lpLS_col = new LS_item;
		this->Init_LS_item(lpLS_col,false);

		// attach to Array
		//
		this->columns.SetAt(nCol, lpLS_col);
	}

	// no we can update the style
	//
	lpLS_col->bgColor = txtBgColor;

	// Redraw it
	if(redraw)
	{	this->RedrawItems(0, this->GetItemCount() );
		this->UpdateWindow();
	}
}

void CListCtrlStyled::SetColFont(int nCol,CFont * pFont,bool redraw)
{
	// Verify if a style for this Row already exist or not
	//
	LS_item * lpLS_col = NULL;
	lpLS_col = this->columns[nCol];

	if(lpLS_col == NULL)
	{	// We must create one
		//
		lpLS_col = new LS_item;
		this->Init_LS_item(lpLS_col,false);

		// attach to Array
		//
		this->columns.SetAt(nCol, lpLS_col);
	}

	// if any internal font exist for this item then delete it
	//
	this->Free_LS_font(lpLS_col);

	// no we can update the style
	//
	lpLS_col->cfont = pFont;
	lpLS_col->ifont = false;

	lpLS_col->in_use = true;

	// Redraw it
	if(redraw)
	{	this->RedrawItems(0, this->GetItemCount() );
		this->UpdateWindow();
	}
}

// *******************************************
// ** New STYLE Methods on Columns SELECTED **
// *******************************************
void CListCtrlStyled::SetColSelectedStyle(int nCol,DWORD Style,bool redraw)
{
	// Verify if a style for this Col already exist or not
	//
	LS_item * lpLS_col = NULL;
	lpLS_col = this->columns[nCol];

	if(lpLS_col == NULL)
	{	// We must create one
		//
		lpLS_col = new LS_item;
		this->Init_LS_item(lpLS_col,false);

		// attach to Array
		//
		this->columns.SetAt(nCol, lpLS_col);
	}

	// Take the selected style structure
	//
	if(lpLS_col->selected_style == NULL)
	{	// Create a structure style
		//
		lpLS_col->selected_style = new LS_item;
		this->Init_LS_item( lpLS_col->selected_style );
	}
	lpLS_col = lpLS_col->selected_style;

	// no we can update the style
	//
	lpLS_col->StyleFlag = Style;

	DWORD mask = LIS_BOLD | LIS_ITALIC | LIS_UNDERLINE| LIS_STROKE ;
	lpLS_col->in_use = (Style & mask) > 0;

	// if any font exist for this item then delete it
	//
	this->Free_LS_font(lpLS_col);

	// Redraw it
	if(redraw)
	{	this->RedrawItems(0, this->GetItemCount() );
		this->UpdateWindow();
	}
}

void CListCtrlStyled::SetColSelectedTxtColor(int nCol,COLORREF txtColor,bool redraw)
{
	// Verify if a style for this Row already exist or not
	//
	LS_item * lpLS_col = NULL;
	lpLS_col = this->columns[nCol];

	if(lpLS_col == NULL)
	{	// We must create one
		//
		lpLS_col = new LS_item;
		this->Init_LS_item(lpLS_col,false);

		// attach to Array
		//
		this->columns.SetAt(nCol, lpLS_col);
	}

	// Take the selected style structure
	//
	if(lpLS_col->selected_style == NULL)
	{	// Create a structure style
		//
		lpLS_col->selected_style = new LS_item;
		this->Init_LS_item( lpLS_col->selected_style );
	}
	lpLS_col = lpLS_col->selected_style;

	// no we can update the style
	//
	lpLS_col->txtColor = txtColor;

	// Redraw it
	if(redraw)
	{	this->RedrawItems(0, this->GetItemCount() );
		this->UpdateWindow();
	}
}

void CListCtrlStyled::SetColSelectedBgColor(int nCol,COLORREF txtBgColor,bool redraw)
{
	// Verify if a style for this Row already exist or not
	//
	LS_item * lpLS_col = NULL;
	lpLS_col = this->columns[nCol];

	if(lpLS_col == NULL)
	{	// We must create one
		//
		lpLS_col = new LS_item;
		this->Init_LS_item(lpLS_col,false);

		// attach to Array
		//
		this->columns.SetAt(nCol, lpLS_col);
	}

	// Take the selected style structure
	//
	if(lpLS_col->selected_style == NULL)
	{	// Create a structure style
		//
		lpLS_col->selected_style = new LS_item;
		this->Init_LS_item( lpLS_col->selected_style );
	}
	lpLS_col = lpLS_col->selected_style;

	// no we can update the style
	//
	lpLS_col->bgColor = txtBgColor;

	// Redraw it
	if(redraw)
	{	this->RedrawItems(0, this->GetItemCount() );
		this->UpdateWindow();
	}
}

void CListCtrlStyled::SetColSelectedFont(int nCol,CFont * pFont,bool redraw)
{
	// Verify if a style for this Row already exist or not
	//
	LS_item * lpLS_col = NULL;
	lpLS_col = this->columns[nCol];

	if(lpLS_col == NULL)
	{	// We must create one
		//
		lpLS_col = new LS_item;
		this->Init_LS_item(lpLS_col,false);

		// attach to Array
		//
		this->columns.SetAt(nCol, lpLS_col);
	}

	// Take the selected style structure
	//
	if(lpLS_col->selected_style == NULL)
	{	// Create a structure style
		//
		lpLS_col->selected_style = new LS_item;
		this->Init_LS_item( lpLS_col->selected_style );
	}
	lpLS_col = lpLS_col->selected_style;

	// if any internal font exist for this item then delete it
	//
	this->Free_LS_font(lpLS_col);

	// no we can update the style
	//
	lpLS_col->cfont = pFont;
	lpLS_col->ifont = false;

	lpLS_col->in_use = true;

	// Redraw it
	if(redraw)
	{	this->RedrawItems(0, this->GetItemCount() );
		this->UpdateWindow();
	}
}

// *********************************
// ** New HIGHLIGHT COLOR Methods **
// *********************************
void CListCtrlStyled::SetHighlightTextColor(COLORREF Color)
{	this->m_highlighttext = Color; }

void CListCtrlStyled::SetHighlightColor(COLORREF Color)
{	this->m_highlight = Color; }

// ******************************************
// ** Internal Methods for Drawing Process **
// ******************************************
void CListCtrlStyled::OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult)
{
    // first, lets extract data from
    // the message for ease of use later
    NMLVCUSTOMDRAW* pNMLVCUSTOMDRAW = (NMLVCUSTOMDRAW*)pNMHDR;

    // we'll copy the device context into hdc
    // but wont convert it to a pDC* until (and if)
    // we need it as this requires a bit of work
    // internally for MFC to create temporary CDC
    // objects
    HDC hdc = pNMLVCUSTOMDRAW->nmcd.hdc;
    CDC* pDC = NULL;

    // here is the item info
    // note that we don't get the subitem
    // number here, as this may not be
    // valid data except when we are
    // handling a sub item notification
    // so we'll do that separately in
    // the appropriate case statements
    // below.
    int nItem = pNMLVCUSTOMDRAW->nmcd.dwItemSpec;
    UINT nState = pNMLVCUSTOMDRAW->nmcd.uItemState;
    LPARAM lParam = pNMLVCUSTOMDRAW->nmcd.lItemlParam;

    // next we set up flags that will control
    // the return value for *pResult
    bool bNotifyPostPaint = false;
    bool bNotifyItemDraw = false;
    bool bNotifySubItemDraw = false;
    bool bSkipDefault = false;
    bool bNewFont = false;

    // what we do next depends on the
    // drawing stage we are processing
    switch (pNMLVCUSTOMDRAW->nmcd.dwDrawStage) {
    case CDDS_PREPAINT:
        {
            // PrePaint
            m_pOldItemFont = NULL;
            m_pOldSubItemFont = NULL;
            bNotifyPostPaint = false;
            bNotifyItemDraw = true;
        }
        break;
    case CDDS_ITEMPREPAINT:
        {
            // Item PrePaint
			//
			// set up a different font to use, if any
			if (! pDC) pDC = CDC::FromHandle(hdc);

			 m_pOldItemFont = NULL;
            bNotifyPostPaint = false;
            bNotifySubItemDraw = true;

			// Store the first time, information about default font
			//
			if(m_Default_pCFont == NULL)
			{	m_Default_pCFont = pDC->GetCurrentFont();
				m_Default_pCFont->GetLogFont(&m_Default_LOGFONT);
			}

            m_item_selected = false;
            m_item_selected = this->GetItemState(nItem, LVIS_SELECTED) != 0;
			if(m_item_selected)
			{	this->SetItemState(nItem,NULL,LVIS_SELECTED);
				bNotifyPostPaint = true;
			}

            CFont* pNewFont = FontForItem(nItem,0,lParam,pNMLVCUSTOMDRAW);
            if (pNewFont) {
                m_pOldItemFont = pDC->SelectObject(pNewFont);
                bNotifyPostPaint = true;    // need to restore old font
				bNewFont = true;
            }
        }
        break;
    case CDDS_ITEMPREPAINT|CDDS_SUBITEM:
        {	// Sub Item PrePaint
            // set sub item number (data will be valid now)
            int nSubItem = pNMLVCUSTOMDRAW->iSubItem;
            m_pOldSubItemFont = NULL;
            bNotifyPostPaint = false;

            // set up a different font to use, if any
			if (! pDC) pDC = CDC::FromHandle(hdc);
            CFont* pNewFont = FontForItem(nItem,nSubItem,lParam,pNMLVCUSTOMDRAW);

			if (pNewFont) {
                m_pOldSubItemFont = pDC->SelectObject(pNewFont);
                bNotifyPostPaint = true;    // need to restore old font
				bNewFont = true;
            }
			else if(m_pOldItemFont)
			{	m_pOldSubItemFont = pDC->SelectObject(m_pOldItemFont);
                bNotifyPostPaint = true;    // need to restore old font
				bNewFont = true;
			}
        }
        break;
    case CDDS_ITEMPOSTPAINT|CDDS_SUBITEM:
        {
            // Sub Item PostPaint
            // set sub item number (data will be valid now)
            int nSubItem = pNMLVCUSTOMDRAW->iSubItem;
            // restore old font if any
			if (! pDC) pDC = CDC::FromHandle(hdc);
            if (m_pOldSubItemFont) {
                pDC->SelectObject(m_pOldSubItemFont);
                m_pOldSubItemFont = NULL;
            }
		 }
        break;
    case CDDS_ITEMPOSTPAINT:
        {
            // Item PostPaint
            // restore old font if any
			if (! pDC) pDC = CDC::FromHandle(hdc);
            if (m_pOldItemFont) {
                pDC->SelectObject(m_pOldItemFont);
                m_pOldItemFont = NULL;
            }

            if(m_item_selected)
			{	m_item_selected = false;
				this->SetItemState(nItem,LVIS_SELECTED,LVIS_SELECTED);
			}
		}
        break;
    }

    ASSERT(CDRF_DODEFAULT==0);
    *pResult = 0;
    if (bNotifyPostPaint) {
        *pResult |= CDRF_NOTIFYPOSTPAINT;
    }
    if (bNotifyItemDraw) {
        *pResult |= CDRF_NOTIFYITEMDRAW;
    }
    if (bNotifySubItemDraw) {
        *pResult |= CDRF_NOTIFYSUBITEMDRAW;
    }
    if (bNewFont) {
        *pResult |= CDRF_NEWFONT;
    }
    if (bSkipDefault) {
        *pResult |= CDRF_SKIPDEFAULT;
    }
    if (*pResult == 0) {
        // redundant as CDRF_DODEFAULT==0 anyway
        // but shouldn't depend on this in our code
        *pResult = CDRF_DODEFAULT;
    }

}

CFont * CListCtrlStyled::FontForItem(int nItem,int nSubItem,LPARAM lParam,NMLVCUSTOMDRAW* pNMLVCUSTOMDRAW)
{
	// Allow to create the correct font for an item of the CListCtrl
	//

	// Cast the lParam into an LS_item object
	LS_item * lpLS_root = NULL;
	lpLS_root = (LS_item*)lParam;

	LS_item * lpLS_item = NULL;
	LS_item * lpLS_row = NULL;
	LS_item * lpLS_col = NULL;

	// Set a mask value
	//
	DWORD mask = LIS_BOLD | LIS_ITALIC | LIS_UNDERLINE| LIS_STROKE ;

	// Take Corects Structure style that we need
	//
	lpLS_item = lpLS_root;
	lpLS_row = lpLS_root->row_style;
	lpLS_col = this->columns[nSubItem];

	if(nSubItem > 0)
		 lpLS_item = lpLS_root->subitems[nSubItem - 1];

	if(m_item_selected)
	{
		if(lpLS_item)
			lpLS_item = lpLS_item->selected_style;

		if(lpLS_row)
			lpLS_row = lpLS_row->selected_style;

		if(lpLS_col)
			lpLS_col = lpLS_col->selected_style;
	}

	// Set if style of columns & rows are enable or not
	//
	bool enable_col_style = true;
	bool enable_row_style = true;

	if(lpLS_item)
	{	enable_col_style = (lpLS_item->StyleFlag & LIS_NO_COL_STYLE) == 0;
		enable_row_style = (lpLS_item->StyleFlag & LIS_NO_ROW_STYLE) == 0;
	}

	if(lpLS_row && enable_col_style && enable_row_style)
		enable_col_style = (lpLS_row->StyleFlag & LIS_NO_COL_STYLE) == 0 ;

	if(lpLS_col && enable_row_style && enable_col_style)
		enable_row_style = (lpLS_col->StyleFlag & LIS_NO_ROW_STYLE) == 0;

	// ---------------------------
	// -- Text Color Management --
	// ---------------------------
	{	// Default
		//
		bool set = false;

		if(m_item_selected)
			pNMLVCUSTOMDRAW->clrText = m_highlighttext;
		else
			pNMLVCUSTOMDRAW->clrText = CListCtrl::GetTextColor();

		// Try ITEM color
		//
		if(lpLS_item)
		{	if(lpLS_item->StyleFlag & LIS_TXTCOLOR)
			{	pNMLVCUSTOMDRAW->clrText = lpLS_item->txtColor;
				set = true;
			}
		}

		// Try LINE color
		//
		if(lpLS_row && !set && enable_row_style)
		{	if(lpLS_row->StyleFlag & LIS_TXTCOLOR)
			{	pNMLVCUSTOMDRAW->clrText = lpLS_row->txtColor;
				set = true;
			}
		}

		// Try COLUMN color
		//
		if(lpLS_col && !set && enable_col_style)
		{	if(lpLS_col->StyleFlag & LIS_TXTCOLOR)
				pNMLVCUSTOMDRAW->clrText = lpLS_col->txtColor;
		}
	}

	// ---------------------------------
	// -- BackGround Color Management --
	// ---------------------------------
	{	// Default
		//
		bool set = false;
		if(!m_item_selected)
			pNMLVCUSTOMDRAW->clrTextBk = CListCtrl::GetTextBkColor();
		else
			pNMLVCUSTOMDRAW->clrTextBk = m_highlight;

		// Try ITEM color
		//
		if(lpLS_item)
		{	if(lpLS_item->StyleFlag & LIS_BGCOLOR)
			{	pNMLVCUSTOMDRAW->clrTextBk = lpLS_item->bgColor;
				set = true;
			}
		}

		// Try LINE color
		//
	   	if(lpLS_row && !set)
		{	if(lpLS_row->StyleFlag & LIS_BGCOLOR && enable_row_style)
			{	pNMLVCUSTOMDRAW->clrTextBk = lpLS_row->bgColor;
				set = true;
			}
		}

		// Try COLUMN color
		//
		if(lpLS_col	&& !set)
		{	if(lpLS_col->StyleFlag & LIS_BGCOLOR && enable_col_style)
				pNMLVCUSTOMDRAW->clrTextBk = lpLS_col->bgColor;
		}
	}

	// Adjusting LS_Structure item for take orignal style if no style is defined for the selected case
	//
	if(m_item_selected)
	{	bool back;

		back = true;
		if(lpLS_item)
			back = !lpLS_item->in_use;

		if(back)
		{	lpLS_item = lpLS_root;
			if(nSubItem > 0)
				lpLS_item = lpLS_root->subitems[nSubItem - 1];
		}

		back = true;
		if(lpLS_row)
			back = !lpLS_row->in_use;

		if(back)
			lpLS_row = lpLS_root->row_style;

		back = true;
		if(lpLS_col)
			back = !lpLS_col->in_use;

		if(back)
			lpLS_col = this->columns[nSubItem];

		// Recompute if style of columns & rows are enable or not
		//
		enable_col_style = true;
		enable_row_style = true;

		if(lpLS_item)
		{	enable_col_style = (lpLS_item->StyleFlag & LIS_NO_COL_STYLE) == 0;
			enable_row_style = (lpLS_item->StyleFlag & LIS_NO_ROW_STYLE) == 0;
		}

		if(lpLS_row && enable_col_style && enable_row_style)
			enable_col_style = (lpLS_row->StyleFlag & LIS_NO_COL_STYLE) == 0 ;

		if(lpLS_col && enable_row_style && enable_col_style)
			enable_row_style = (lpLS_col->StyleFlag & LIS_NO_ROW_STYLE) == 0;
	}

	// ---------------------------
	// -- Fonts Initialisations --
	// ---------------------------
	CFont * pCFontItem = NULL;			// Item Font
	CFont * pCFontRow = NULL;			// Row Font
	CFont * pCFontCol = NULL;			// Column Font

	// -- If a font is created, then the existing merged font can't be available
	// -- This flag prevent this use if it's not permitted
	// --
	bool must_erase_merged_font = false;


	// ---------------
	// -- Item Font --
	// ---------------
	if(lpLS_item)
	{
		if(lpLS_item->cfont != NULL)
			pCFontItem = lpLS_item->cfont;
		else
		{	// Create it if necessary
			//
			if((lpLS_item->StyleFlag & mask) != 0)
			{	pCFontItem = CreateFont(lpLS_item->StyleFlag);
				lpLS_item->cfont = pCFontItem;
				lpLS_item->ifont = true;
				must_erase_merged_font = true;
			}
		}
	}

	// --------------
	// -- Row Font --
	// --------------
	if(lpLS_row && enable_row_style)
	{
		if(lpLS_row->cfont != NULL)
			pCFontRow = lpLS_row->cfont;
		else
		{	// Create it if necessary
			//
			if((lpLS_row->StyleFlag & mask) != 0)
			{	pCFontRow = CreateFont(lpLS_row->StyleFlag);
				lpLS_row->cfont = pCFontRow;
				lpLS_row->ifont = true;
				must_erase_merged_font = true;
			}
		}
	}

	// -----------------
	// -- Column Font --
	// -----------------
	if(lpLS_col && enable_col_style)
	{
		if(lpLS_col->cfont != NULL)
			pCFontCol =	lpLS_col->cfont;
		else
		{	// Create it if necessary
			//
			if((lpLS_col->StyleFlag & mask) != 0)
			{	pCFontCol = CreateFont(lpLS_col->StyleFlag);
				lpLS_col->cfont = pCFontCol;
				lpLS_col->ifont = true;
				must_erase_merged_font = true;
			}
		}
	}

	// -- See if a font is define or not (if not, use default font)
	// --
	if(pCFontItem == NULL && pCFontRow == NULL && pCFontCol == NULL)
		return NULL;

	// Delete the merged font if needed
	//
	if(lpLS_item)
	{	if(lpLS_item->merged_font != NULL && must_erase_merged_font)
			this->Free_LS_font(lpLS_item,FONT_MERGED);
	}

	// -- At least One Font is required, Find it if only one is required
	// --
	CFont * pFont = this->SelectFont(pCFontItem,pCFontRow,pCFontCol);

	// -- If a Font can be selected, then we can return it !! no melting font was needed
	// --
	if(pFont != NULL)
		return pFont;

	// -- Test if a merged font exist, if it's the case, we can directly return it
	// --
	if(lpLS_item)
	{	if(lpLS_item->merged_font != NULL)
			return lpLS_item->merged_font;
	}
	else
		return NULL;

	// -- So, if we can't select a Font, it's because we must created a melted Font from all fonts
	// --
	CFont * pFontMelted = this->MergeFont(pCFontItem,pCFontRow,pCFontCol);

	// -- Attach the melted font
	// --
	lpLS_item->merged_font = pFontMelted;

	// -- Return
	// --
	return pFontMelted;
}

CFont * CListCtrlStyled::SelectFont(CFont * pCFont1,CFont * pCFont2,CFont * pCFont3)
{	if(pCFont1)
	{	if(!pCFont2 && ! pCFont3)
			return pCFont1;
		else
			return NULL;
	}

	if(pCFont2)
	{	if(!pCFont3)
			return pCFont2;
		else
			return NULL;
	}

	return (pCFont3);
}

CFont * CListCtrlStyled::MergeFont(CFont * pCFont1,CFont * pCFont2,CFont * pCFont3)
{	LOGFONT merge_logfont;

	// -- Set Base Font Informations
	// --
	merge_logfont = this->m_Default_LOGFONT;

	if(pCFont3)
	{	LOGFONT logfont;
		pCFont3->GetLogFont(&logfont);
		this->MergeLogFont(&logfont,&merge_logfont);
	}

	if(pCFont2)
	{	LOGFONT logfont;
		pCFont2->GetLogFont(&logfont);
		this->MergeLogFont(&logfont,&merge_logfont);
	}

	if(pCFont1)
	{	LOGFONT logfont;
		pCFont1->GetLogFont(&logfont);
		this->MergeLogFont(&logfont,&merge_logfont);
	}

	// -- So now we have the logfont corresponding at our merged font
	// --

	CFont * pCFontMerged;
	pCFontMerged = new(CFont);
	pCFontMerged->CreateFontIndirect(&merge_logfont);

	return pCFontMerged;
}

void CListCtrlStyled::MergeLogFont(LOGFONT * font,LOGFONT * dest)
{
	/*	typedef struct tagLOGFONT {
  		LONG lfHeight;
  		LONG lfWidth;
  		LONG lfEscapement;
  		LONG lfOrientation;
  		LONG lfWeight;
  		BYTE lfItalic;
  		BYTE lfUnderline;
  		BYTE lfStrikeOut;
  		BYTE lfCharSet;
  		BYTE lfOutPrecision;
  		BYTE lfClipPrecision;
  		BYTE lfQuality;
  		BYTE lfPitchAndFamily;
  		TCHAR lfFaceName[LF_FACESIZE];
		} LOGFONT, *PLOGFONT;
	*/

	if(font->lfHeight != this->m_Default_LOGFONT.lfHeight)
		dest->lfHeight = font->lfHeight;

	if(font->lfWidth != this->m_Default_LOGFONT.lfWidth)
		dest->lfWidth = font->lfWidth;

	if(font->lfEscapement != this->m_Default_LOGFONT.lfEscapement)
		dest->lfEscapement = font->lfEscapement;

	if(font->lfOrientation != this->m_Default_LOGFONT.lfOrientation)
		dest->lfOrientation = font->lfOrientation;

	if(font->lfWeight != this->m_Default_LOGFONT.lfWeight)
		dest->lfWeight = font->lfWeight;

	if(font->lfItalic != this->m_Default_LOGFONT.lfItalic)
		dest->lfItalic = font->lfItalic;

	if(font->lfUnderline != this->m_Default_LOGFONT.lfUnderline)
		dest->lfUnderline = font->lfUnderline;

	if(font->lfStrikeOut != this->m_Default_LOGFONT.lfStrikeOut)
		dest->lfStrikeOut = font->lfStrikeOut;

	if(font->lfCharSet != this->m_Default_LOGFONT.lfCharSet)
		dest->lfCharSet = font->lfCharSet;

	if(font->lfOutPrecision != this->m_Default_LOGFONT.lfOutPrecision)
		dest->lfOutPrecision = font->lfOutPrecision;

	if(font->lfClipPrecision != this->m_Default_LOGFONT.lfClipPrecision)
		dest->lfClipPrecision = font->lfClipPrecision;

	if(font->lfQuality != this->m_Default_LOGFONT.lfQuality)
		dest->lfQuality = font->lfQuality;

	if(font->lfPitchAndFamily != this->m_Default_LOGFONT.lfPitchAndFamily)
		dest->lfPitchAndFamily = font->lfPitchAndFamily;

	if(strcmp(font->lfFaceName,this->m_Default_LOGFONT.lfFaceName) != 0)
		strcpy(dest->lfFaceName,font->lfFaceName);

}

CFont * CListCtrlStyled::CreateFont(DWORD StyleFlag)
{
	// Retrieve informations about the current font of this item
	//
	LOGFONT logfont = this->m_Default_LOGFONT;					// Information about our new font

	// Update the logfont structure for prepare our new font
	//
    /*	typedef struct tagLOGFONT {
  		LONG lfHeight;
  		LONG lfWidth;
  		LONG lfEscapement;
  		LONG lfOrientation;
  		LONG lfWeight;
  		BYTE lfItalic;
  		BYTE lfUnderline;
  		BYTE lfStrikeOut;
  		BYTE lfCharSet;
  		BYTE lfOutPrecision;
  		BYTE lfClipPrecision;
  		BYTE lfQuality;
  		BYTE lfPitchAndFamily;
  		TCHAR lfFaceName[LF_FACESIZE];
		} LOGFONT, *PLOGFONT;
	*/

	if(StyleFlag & LIS_BOLD)
		logfont.lfWeight = FW_BOLD;
	else
		logfont.lfWeight = FW_NORMAL;

	logfont.lfItalic = (StyleFlag & LIS_ITALIC) > 0;
	logfont.lfUnderline = (StyleFlag & LIS_UNDERLINE) > 0;
	logfont.lfStrikeOut = (StyleFlag & LIS_STROKE) > 0;

	CFont * lpCFont;
	lpCFont = new(CFont);

	// Create the font
	//
	lpCFont->CreateFontIndirect(&logfont);

	return (lpCFont);
}

// **************************************
// ** Internal INITIALISATIONS Methods **
// **************************************
void CListCtrlStyled::Init_LS_item(LS_item * lpLS_item,bool allow_subitems)
{	// lParam Init
	//
	lpLS_item->lParam = 0;
	lpLS_item->mParam = false;

	// Set Default Style
	//
	lpLS_item->StyleFlag = 0;
	lpLS_item->in_use = false;

	lpLS_item->txtColor = 0;
	lpLS_item->bgColor = 0;
	lpLS_item->cfont = NULL;
	lpLS_item->ifont = false;
	lpLS_item->merged_font = NULL;
	lpLS_item->row_style = NULL;
	lpLS_item->selected_style = NULL;

	// Init the Array for hold SubItems Style
	//
	if(allow_subitems)
	{	int nSubItems = this->GetHeaderCtrl()->GetItemCount();
		if(nSubItems > 0) nSubItems--;
		lpLS_item->subitems.SetSize( nSubItems );
	}
	else
		lpLS_item->subitems.SetSize(0);

	lpLS_item->subitems.RemoveAll();
}

void CListCtrlStyled::Free_LS_font(LS_item* lpLS_item,DWORD mask)
{
	// Free his font object if needed (if it's an internal font)
	//
	if(lpLS_item->cfont != NULL && lpLS_item->ifont && (mask & FONT_NORMAL))
	{	delete lpLS_item->cfont;
		lpLS_item->cfont = NULL;
		lpLS_item->ifont = false;
	}

	// Free Merged Font
	//
	if(lpLS_item->merged_font != NULL && (mask & FONT_MERGED))
	{	delete lpLS_item->merged_font;
		lpLS_item->merged_font = NULL;
	}

}

void CListCtrlStyled::Free_LS_item(LS_item* lpLS_item)
{
	if(lpLS_item == NULL)
		return;

	if(lpLS_item->subitems.GetSize() > 0)
	{	// We must delete the subitems structure also
		//
		int nSubItems = lpLS_item->subitems.GetSize();

		for(int nSubItem = 0; nSubItem < nSubItems; nSubItem++)
			this->Free_LS_item(lpLS_item->subitems[nSubItem]);
	}

	// We can reset the list of subitems style
	//
	lpLS_item->subitems.RemoveAll();
	lpLS_item->subitems.SetSize(0);


	// Free CFont objects
	//
	this->Free_LS_font(lpLS_item);

	// Delete the Row Style if needed
	//
	if(lpLS_item->row_style != NULL)
		this->Free_LS_item(lpLS_item->row_style);

	// Delete the Selected Style if needed
	//
	if(lpLS_item->selected_style != NULL)
		this->Free_LS_item(lpLS_item->selected_style);

	// Delete the item structure
	//
	delete lpLS_item;
	lpLS_item = NULL;
}

void CListCtrlStyled::InitLVITEM(int nItem,int nSubItem,LVITEM * pItem)
{ /*
  typedef struct _LV_ITEM {
    UINT   mask;         // see below
    int    iItem;        // see below
    int    iSubItem;     // see below
    UINT   state;        // see below
    UINT   stateMask;    // see below
    LPSTR  pszText;      // see below
    int    cchTextMax;   // see below
    int    iImage;       // see below
    LPARAM lParam;       // 32-bit value to associate with item
   } LV_ITEM;
  */
	pItem->mask = LVIF_PARAM;
	pItem->iItem = nItem;
	pItem->iSubItem = nSubItem;
	pItem->state = NULL;
	pItem->stateMask = NULL;
	pItem->pszText = NULL;
	pItem->cchTextMax = NULL;
	pItem->iImage = NULL;
	pItem->lParam = NULL;
	CListCtrl::GetItem(pItem);
}
