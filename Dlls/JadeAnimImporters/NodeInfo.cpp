#include "StdAfx.h"
#include "NodeInfo.h"


CNodeInfo::CNodeInfo( int index, INode* p_INode, bool bNoNodesSelected ):
_p_INode( p_INode ),
_bSelected( bNoNodesSelected || p_INode->Selected() ),
_iNumRefs( 0 ),
_iNumberInTable( index )
{
    _szObject[0] = 0;
}
