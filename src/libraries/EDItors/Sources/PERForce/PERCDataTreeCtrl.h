//------------------------------------------------------------------------------
//   PERCDataCtrl.h
/// \author    YCharbonneau
/// \date      27/02/2005
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __PERCDATATREECTRL_H__
#define __PERCDATATREECTRL_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "EDItors/Sources/PERForce/PERCDataCtrl.h"
//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
//------------------------------------------------------------
//  Name   :   PER_CDataTreeCtrl
/// \author    YCharbonneau
/// \date      27/02/2005
/// \par       Description: 
///            No description available ...
/// \see 
//------------------------------------------------------------

class PER_CDataTreeCtrl : public PER_CDataCtrl 
{
	   //--------------------------------------------------------------------------
	   // public definitions 
	   //--------------------------------------------------------------------------

public:
		PER_CDataTreeCtrl (CTreeCtrl* _pTree) : m_pTree(_pTree){}

		virtual ~PER_CDataTreeCtrl (){}
		
		// data retrival
		virtual ULONG			GetItemReference(HDATACTRLITEM _hItem) 
		{
			return m_pTree->GetItemData((HTREEITEM)_hItem);
		}
		
		// selection functions
		virtual HDATACTRLITEM	GetFirstSelectedItem() const
		{ 
			return (HDATACTRLITEM)(m_pTree->GetSelectedItem()); 
		}

		virtual HDATACTRLITEM	GetNextSelectedItem	(HDATACTRLITEM _hItem) const
		{
			return -1;
		}


		virtual BOOL			ItemIsDirectory		(HDATACTRLITEM _hItem) const
		{
			return m_pTree->ItemHasChildren( (HTREEITEM)_hItem );
		}

		
		//--------------------------------------------------------------------------
		// protected definitions
		//--------------------------------------------------------------------------
protected:

	   //--------------------------------------------------------------------------
	   // private definitions
	   //--------------------------------------------------------------------------

private:

	CTreeCtrl* m_pTree;

};

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

#endif //#ifndef __PERCDATATREECTRL_H__

