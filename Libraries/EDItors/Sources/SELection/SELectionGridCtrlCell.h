//------------------------------------------------------------------------------
//   SELectionGridCtrlCell.h
/// \author    NBeaufils
/// \date      2005-05-02
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __SELECTIONGRIDCTRLCELL_H__
#define __SELECTIONGRIDCTRLCELL_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "GridCtrl/GridCell.h"
#include "GridCtrl/GridCellCombo.h"
#include "GridCtrl/GridCellCheck.h"
#include "GridCtrl/GridCellNumeric.h"
#include "GridCtrl/GridBtnCell.h"

//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class SELectionGridCellNoModify : public CGridCell
{
	DECLARE_DYNCREATE(SELectionGridCellNoModify)

public:
	virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
};

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class SELectionGridCellPerforce : public SELectionGridCellNoModify
{
	DECLARE_DYNCREATE(SELectionGridCellPerforce)

public:
	virtual void OnRClick( CPoint PointCellRelative);
};

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class SELectionGridCell : public CGridCell
{
	DECLARE_DYNCREATE(SELectionGridCell)

public:
	virtual LPCTSTR GetTipText() const;
};

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class SELectionGridCellCombo : public CGridCellCombo
{
	DECLARE_DYNCREATE(SELectionGridCellCombo)

public:	
	virtual LPCTSTR GetTipText() const;
	
};

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class SELectionGridCellCheck : public CGridCellCheck
{
	DECLARE_DYNCREATE(SELectionGridCellCheck)

public:
	virtual void OnRClick( CPoint PointCellRelative);

	virtual LPCTSTR GetTipText() const;
};

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class SELectionGridCellButtonName : public CGridBtnCell
{
	DECLARE_DYNCREATE(SELectionGridCellButtonName)

public:
	virtual void OnRClick( CPoint PointCellRelative);

	virtual void SetData(LPARAM lParam)		{ m_lParam = lParam ; } 
	virtual LPARAM GetData() const			{ return m_lParam ;  }

	virtual LPCTSTR GetTipText() const;

protected:
	LPARAM   m_lParam;
};

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class SELectionGridCellButtonDelete : public CGridBtnCell
{
	DECLARE_DYNCREATE(SELectionGridCellButtonDelete)

public:
	virtual void OnRClick( CPoint PointCellRelative);

	virtual LPCTSTR GetTipText() const;

protected:
	LPARAM   m_lParam;
};

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class SELectionGridCellFloat : public CGridCellNumeric
{
	DECLARE_DYNCREATE(SELectionGridCellFloat)

public:
	float GetFloat();
	
	virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
	virtual void OnEndEdit();

	virtual LPCTSTR GetTipText() const;

protected:
	float m_fValue;
};

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class SELectionGridCellInteger : public CGridCellNumeric
{
	DECLARE_DYNCREATE(SELectionGridCellInteger)

public:
	int GetInt();

	virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
	virtual void OnEndEdit();

	virtual LPCTSTR GetTipText() const;

protected:
	int m_iValue;
};

//------------------------------------------------------------------------------

#endif //#ifndef __SELECTIONGRIDCTRLCELL_H__

