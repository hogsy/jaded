#ifndef __NODEINFO__H
#define __NODEINFO__H


#include "MATHs/MATH.h"

class CNodeInfo
{
public:
    CNodeInfo( int index, INode* p_INode, bool bNoNodesSelected );

    void UpdateMatrices( TimeValue t );
    inline void IncNumRefs() {_iNumRefs++;}
    inline void DecNumRefs() {_iNumRefs--;}
	inline void ResetNumRefs() {_iNumRefs = 0;}

    inline INode* GetINode() const {return _p_INode;}
    inline bool IsSelected() const {return _bSelected;}
    inline const TCHAR* GetName() const {return _p_INode->GetName();}
    inline int GetNumRefs() const {return _iNumRefs;}
    inline const TCHAR* GetObjectName() const {return _szObject;}
    inline int GetIndex() const {return _iNumberInTable;}

private:
    INode*  _p_INode;
    bool    _bSelected;
    int     _iNumRefs;
    TCHAR   _szObject[80];
    int     _iNumberInTable;
};


extern float MAD_SCALE_EXPORT_FACTOR;


#endif // __NODEINFO__H