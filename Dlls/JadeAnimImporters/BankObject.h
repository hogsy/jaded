#ifndef __BANKOBJECT__H
#define __BANKOBJECT__H


#include "MATHs/MATH.h"

class CNodeInfo;


class CBankObject
{
public:
	CBankObject( const TCHAR* name, short index, int parent, const MATH_tdst_Vector& st_LocalPos );
	void SetNode( CNodeInfo* node );

	inline const TCHAR* GetName() const {return m_szName;}
	inline short GetIndex() const {return m_wIndex;}
	inline int GetParent() const {return m_iParent;}
    inline const MATH_tdst_Vector& GetLocalPos() const {return m_stLocalPos;}

	inline const CNodeInfo* GetNode() const {return m_node;}
	
private:
    TCHAR               m_szName[80];
    short               m_wIndex;

    int                 m_iParent;
    MATH_tdst_Vector    m_stLocalPos;   // Position relative to its parent
	CNodeInfo*          m_node;
};


#endif // __BANKOBJECT__H
