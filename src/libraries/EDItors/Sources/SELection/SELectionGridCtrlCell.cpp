//------------------------------------------------------------------------------
// Filename   :SELectionGridCtrlCell.cpp
/// \author    NBeaufils
/// \date      2005-05-02
/// \par       Description: Implementation of SELectionGridCtrlCell
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"

#include "SELectionGridCtrlCell.h"
#include "SELectionGridCtrl.h"
#include "SELectionframe.h"

#include "GridCtrl/InPlaceEditNumeric.h"

//------------------------------------------------------------------------------
// PreProcessor defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// externals and global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// statics and constants
//------------------------------------------------------------------------------
CString strTipText;

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(SELectionGridCell, CGridCell)
IMPLEMENT_DYNCREATE(SELectionGridCellNoModify, CGridCell)
IMPLEMENT_DYNCREATE(SELectionGridCellPerforce, SELectionGridCellNoModify)
IMPLEMENT_DYNCREATE(SELectionGridCellCombo, CGridCellCombo)
IMPLEMENT_DYNCREATE(SELectionGridCellCheck, CGridCellCheck)
IMPLEMENT_DYNCREATE(SELectionGridCellFloat, CGridCellNumeric)
IMPLEMENT_DYNCREATE(SELectionGridCellInteger, CGridCellNumeric)
IMPLEMENT_DYNCREATE(SELectionGridCellButtonName, CGridBtnCell)
IMPLEMENT_DYNCREATE(SELectionGridCellButtonDelete, CGridBtnCell)

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
LPCTSTR SELectionGridCell::GetTipText() const
{
	strTipText.Format("MAT: %s | TEX: %s", GetGrid()->GetItemText(m_nRow, eCOL_MaterialName), GetGrid()->GetItemText(m_nRow, eCOL_STD_TextureName));
	return strTipText;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
BOOL SELectionGridCellNoModify::Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar)
{
	return TRUE;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void SELectionGridCellPerforce::OnRClick( CPoint PointCellRelative)
{
	GetGrid()->ClientToScreen(&PointCellRelative);
	((ESELection_cl_Frame*)(GetGrid()->GetParent()))->OnCtrlPopup(PointCellRelative);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
LPCTSTR SELectionGridCellCombo::GetTipText() const
{
	strTipText.Format("MAT: %s | TEX: %s", GetGrid()->GetItemText(m_nRow, eCOL_MaterialName), GetGrid()->GetItemText(m_nRow, eCOL_STD_TextureName));
	return strTipText;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
float SELectionGridCellFloat::GetFloat()
{
	return m_fValue;
}

BOOL SELectionGridCellFloat::Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar)
{
	m_bEditing = TRUE;
	m_fValue = 0;

	m_pEditWnd = new CInPlaceEditNumeric(GetGrid(), rect, ES_RIGHT /*| ES_NUMBER*/, nID, nRow, nCol, GetText(), nChar, TRUE, 4);

	return TRUE;
}

void SELectionGridCellFloat::OnEndEdit()
{
	CString strText = GetText();
	m_fValue = (float)atof(strText.GetBuffer());
	CGridCellNumeric::OnEndEdit();
}

LPCTSTR SELectionGridCellFloat::GetTipText() const
{
	strTipText.Format("MAT: %s | TEX: %s" , GetGrid()->GetItemText(m_nRow, eCOL_MaterialName), GetGrid()->GetItemText(m_nRow, eCOL_STD_TextureName));
	return strTipText;

}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int SELectionGridCellInteger::GetInt()
{
	return m_iValue;
}

BOOL SELectionGridCellInteger::Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar)
{
	m_bEditing = TRUE;
	m_iValue = 0;

	m_pEditWnd = new CInPlaceEditNumeric(GetGrid(), rect, ES_RIGHT /*| ES_NUMBER*/, nID, nRow, nCol, GetText(), nChar, TRUE, 0);

	return TRUE;
}

void SELectionGridCellInteger::OnEndEdit()
{
	CString strText = GetText();
	m_iValue = atoi(strText.GetBuffer());
	CGridCellNumeric::OnEndEdit();
}

LPCTSTR SELectionGridCellInteger::GetTipText() const
{
	strTipText.Format("MAT: %s | TEX: %s ", GetGrid()->GetItemText(m_nRow, eCOL_MaterialName), GetGrid()->GetItemText(m_nRow, eCOL_STD_TextureName));
	return strTipText;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void SELectionGridCellCheck::OnRClick( CPoint PointCellRelative)
{
	// Bail if cell is read-only
	CCellID cell = GetGrid()->GetCellFromPt(PointCellRelative);	
	if (!GetGrid()->IsCellEditable(cell))		
		return;

	m_bChecked = !m_bChecked;
	GetGrid()->InvalidateRect(m_Rect);
	((SELectionGridCtrl*)GetGrid())->ClickCheck(cell.row, cell.col, m_bChecked);
}

LPCTSTR SELectionGridCellCheck::GetTipText() const
{
	strTipText.Format("MAT: %s | TEX: %s ", GetGrid()->GetItemText(m_nRow, eCOL_MaterialName), GetGrid()->GetItemText(m_nRow, eCOL_STD_TextureName));
	return strTipText;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void SELectionGridCellButtonName::OnRClick( CPoint PointCellRelative)
{
	// Bail if cell is read-only
	CCellID cell = GetGrid()->GetCellFromPt(PointCellRelative);	
	if (!GetGrid()->IsCellEditable(cell))		
		return;

	CGridBtnCell::OnRClick(PointCellRelative);
	
	ReleaseCapture();

	BIG_KEY ulKeyTextureSelected = BIG_C_InvalidKey;
	((SELectionGridCtrl*)GetGrid())->ClickSelectionDialog(ulKeyTextureSelected, cell.col);
	
	if ( ulKeyTextureSelected != BIG_C_InvalidKey )
		((SELectionGridCtrl*)GetGrid())->ClickButton(cell.row, cell.col, ulKeyTextureSelected);
}

LPCTSTR SELectionGridCellButtonName::GetTipText() const
{
	strTipText.Format("MAT: %s | TEX: %s ", GetGrid()->GetItemText(m_nRow, eCOL_MaterialName), GetGrid()->GetItemText(m_nRow, eCOL_STD_TextureName));
	return strTipText;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void SELectionGridCellButtonDelete::OnRClick( CPoint PointCellRelative)
{
	// Bail if cell is read-only
	CCellID cell = GetGrid()->GetCellFromPt(PointCellRelative);	
	if (!GetGrid()->IsCellEditable(cell))		
		return;

	CGridBtnCell::OnRClick(PointCellRelative);

	ReleaseCapture();

	((SELectionGridCtrl*)GetGrid())->ClickDeleteDialog();
	((SELectionGridCtrl*)GetGrid())->ClickButton(cell.row, cell.col, BIG_C_InvalidKey);
}

LPCTSTR SELectionGridCellButtonDelete::GetTipText() const
{
	strTipText.Format("MAT: %s | TEX: %s ", GetGrid()->GetItemText(m_nRow, eCOL_MaterialName), GetGrid()->GetItemText(m_nRow, eCOL_STD_TextureName));
	return strTipText;
}

//------------------------------------------------------------------------------

