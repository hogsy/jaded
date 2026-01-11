//------------------------------------------------------------------------------
//   PERCDataCtrl.h
/// \author    YCharbonneau
/// \date      27/02/2005
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __PERCDATACTRL_H__
#define __PERCDATACTRL_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------

#include <vector>

//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
//------------------------------------------------------------
//  Name   :   PER_CDataCtrl
/// \author    YCharbonneau
/// \date      27/02/2005
/// \par       Description: 
///            No description available ...
/// \see 
//------------------------------------------------------------
typedef INT HDATACTRLITEM  ;

class PER_CDataCtrl 
{
	   //--------------------------------------------------------------------------
	   // public definitions 
	   //--------------------------------------------------------------------------

public:
		enum ePER_ItemReferenceType
		{
			eItemReferenceBIG_KEY,
			eItemReferenceBIG_INDEX
		};

		PER_CDataCtrl (){};
		virtual ~PER_CDataCtrl (){};
		
		// data retrival
		virtual ULONG			GetItemReference(HDATACTRLITEM hItem)  = 0;
		virtual ULONG			GetItemReferenceData(HDATACTRLITEM hItem)  { return -1;}  
		
		// selection functions
		virtual HDATACTRLITEM	GetFirstSelectedItem() const = 0;
		virtual HDATACTRLITEM	GetNextSelectedItem	(HDATACTRLITEM _hItem) const = 0;
		virtual BOOL			ItemIsDirectory		(HDATACTRLITEM _hItem) const  = 0 ;
		virtual HDATACTRLITEM	GetParentReference	(HDATACTRLITEM _hItem) { return NULL; }

		virtual ePER_ItemReferenceType ItemReferenceType() { return eItemReferenceBIG_INDEX ; } ;

		//--------------------------------------------------------------------------
		// protected definitions
		//--------------------------------------------------------------------------
protected:

	   //--------------------------------------------------------------------------
	   // private definitions
	   //--------------------------------------------------------------------------

private:

};

//------------------------------------------------------------------------------
// This class emulates a real data control, enabling the use of an arbitrary
// list of indices anywhere a PER_CDataCtrl is expected
//------------------------------------------------------------------------------
class PER_CDataCtrlEmulator : public PER_CDataCtrl
{
public:
	void					Clear( ) { m_lstIndices.clear( ); }
	void					AddIndex( ULONG ulFileIndex ) { m_lstIndices.push_back(ulFileIndex); }

	virtual BIG_INDEX		GetItemReference(HDATACTRLITEM hItem) { if( hItem != 0xFFFFFFFF ) return m_lstIndices[ hItem - 1 ]; else return BIG_C_InvalidIndex; }
	virtual HDATACTRLITEM	GetFirstSelectedItem() const { if( m_lstIndices.empty( ) ) return 0xFFFFFFFF; else return 0x00000001; }
	virtual HDATACTRLITEM	GetNextSelectedItem	(HDATACTRLITEM _hItem) const { if( _hItem == m_lstIndices.size( ) ) return -1; else return _hItem + 1; }
	virtual BOOL			ItemIsDirectory		(HDATACTRLITEM _hItem) const { return FALSE; }

private:
	std::vector< ULONG >	m_lstIndices;
};


//------------------------------------------------------------------------------

#endif //#ifndef __PERCDATACTRL_H__

