#ifndef __JADEBONE__H
#define __JADEBONE__H


#include <list>


class CJadeBone
{
public:
    inline CJadeBone(INode& _node, int _index, int _parent, bool _bIsNotAnimated) :
        node(_node), index(_index), parent(_parent), bIsNotAnimated(_bIsNotAnimated),
        initLocalPos(0.0f, 0.0f, 0.0f), iListItem(-1), hTreeItem(NULL) {}

    inline INode& GetNode() const {return node;}
    inline int GetIndex() const {return index;}
    inline int GetParent() const {return parent;}
    inline bool IsNotAnimated() const {return bIsNotAnimated;}
    inline const float* GetInitLocalPos() const {return const_cast<Point3&>(initLocalPos);}

    inline void SetIndex(int _index) {index = _index;}
    inline void SetParent(int _parent) {parent = _parent;}
    inline void SetIsNotAnimated(bool _bIsNotAnimated) {bIsNotAnimated = _bIsNotAnimated;}
    inline void SetInitLocalPos(const float* _initLocalPos) {initLocalPos.Set(_initLocalPos[0], _initLocalPos[1], _initLocalPos[2]);}

    // For UI purpose
    inline int GetListItemIndex() const {return iListItem;}
    inline void SetListItemIndex(int _iListItem = NULL) const {iListItem = _iListItem;}
    inline HTREEITEM GetTreeItemHandle() const {return hTreeItem;}
    inline void SetTreeItemHandle(HTREEITEM _hTreeItem = NULL) const {hTreeItem = _hTreeItem;}

    inline bool operator<(const CJadeBone& other) const
    {
        if (index!=other.index)
            return index<other.index;
        return _tcscmp(node.GetName(), other.node.GetName())<0;
    }

private:
    INode&  node;
    int     index;
    int     parent;
    bool    bIsNotAnimated;
    Point3  initLocalPos;

    // For UI purpose
    mutable int         iListItem;
    mutable HTREEITEM   hTreeItem;
};

typedef std::list<CJadeBone> CJadeBoneList;


#endif // __JADEBONE__H
