//------------------------------------------------------------------------------
//   SELectionGrid.h
/// \author    NBeaufils
/// \date      2005-04-29
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __SELECTIONGRID_H__
#define __SELECTIONGRID_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "GridCtrl/GridCtrl.h"
#include "GridCtrl/GridCellCombo.h"
#include "GridCtrl/GridCellCheck.h"
#include "GridCtrl/GridCellNumeric.h"
#include "GridCtrl/BtnDataBase.h"

#include "SELectionHelper.h"

#include "GRObject/GROstruct.h"
#include "MATerial/MATstruct.h"
#include "EDItors/Sources/MATerial/MATframe.h"
#include "EDItors/Sources/PERForce/PERCDataCtrl.h"

#include <vector>

//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class SELectionGridCtrl : public CGridCtrl, public PER_CDataCtrl
{
public:
	// Constructor/Destructor
	SELectionGridCtrl();
	virtual ~SELectionGridCtrl();

	// Methods
	void InitializeGrid();
	void InitializeData(GRO_tdst_Struct* _pst_Gmt);

	void EnableColumns(int nRow, int nCol, BOOL bEnable);
	void EnableCell(int nRow, int nCol, BOOL bEnable);

	void RefreshColumns(int nRow, int nCol);

	BOOL ClickSelectionDialog(BIG_KEY& _ulKeyTextureSelected, int nCol);
	BOOL ClickDeleteDialog();

	// Perforce
	void InitializeVectorSelectedItem();
	virtual ULONG GetItemReference(HDATACTRLITEM hItem) ;
	virtual HDATACTRLITEM GetFirstSelectedItem() const ;
	virtual HDATACTRLITEM GetNextSelectedItem(HDATACTRLITEM _hItem) const ;
	virtual BOOL ItemIsDirectory(HDATACTRLITEM _hItem) const { return FALSE; }

	// Handlers
	virtual CGridCellBase* CreateCell(int nRow, int nCol);
	virtual void OnEndEditCell(int nRow, int nCol, CString str);
	
	virtual BOOL ValidateEdit(int nRow, int nCol, LPCTSTR str);

	virtual void ClickCheck(int nRow, int nCol, BOOL bChecked);
	virtual void ClickButton(int nRow, int nCol, ULONG ulKeyTextureSelected);

protected:
	SELectionHelper* m_pSELectionHelper;

	BOOL CheckMaterialDuplicate(MAT_tdst_Material* _pst_Gmt, ULONG _indexTextures);

	void SetMaterialSingleToGrid(GRO_tdst_Struct* _pst_Gmt); ;
	void SetMaterialMultiToGrid(GRO_tdst_Struct* _pst_Gmt);
	void SetTextureToGrid(MUTEX_TextureLine* _pst_TexLine, SELectionGridCtrl_Textures* _pst_SELTextures, ULONG _indexTextures);

	void AutoSetNormalMapName(int iRow);
	void AutoSetMossMapAndDetailMapName(int iRow);

private:
	MAT_tdst_MultiTexture*	mpst_Multi_Sample;

	CBtnDataBase m_BtnDataBase;

	std::vector<BIG_INDEX> m_vcIndexInGrid;
};

//------------------------------------------------------------------------------

#endif //#ifndef __SELECTIONGRID_H__

