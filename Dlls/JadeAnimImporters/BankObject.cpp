#include "StdAfx.h"
#include "BankObject.h"
#include "NodeInfo.h"


CBankObject::CBankObject( const TCHAR* name, short index, int parent, const MATH_tdst_Vector& st_LocalPos ) :
m_wIndex( index ),
m_iParent( parent ),
m_stLocalPos( st_LocalPos ),
m_node( NULL )
{
	_tcscpy( m_szName, name );
}

void CBankObject::SetNode( CNodeInfo* node )
{
	if (m_node)
		m_node->DecNumRefs();

	m_node = node;

	if (m_node)
		m_node->IncNumRefs();
}
