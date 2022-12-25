#ifndef __JADEBONESHITBYNAMEDLGCALLBACK__H
#define __JADEBONESHITBYNAMEDLGCALLBACK__H


#include "JadeBone.h"


class CJadeBonesHitByNameDlgCallback : public HitByNameDlgCallback
{
public:
    inline CJadeBonesHitByNameDlgCallback(CJadeBoneList& _group, Interface& _ip) : group(_group), ip(_ip) {}

    // From HitByNameDlgCallback
    virtual TCHAR *dialogTitle();
    virtual TCHAR *buttonText();
    virtual BOOL singleSelect();
    virtual BOOL useFilter();
    virtual int filter(INode *node);
    virtual BOOL useProc();
    virtual void proc(INodeTab &nodeTab);
    virtual BOOL showHiddenAndFrozen();

private:
    CJadeBoneList& group;
    Interface& ip;
};


class CMagicBoxHitByNameDlgCallback : public HitByNameDlgCallback
{
public:
    CMagicBoxHitByNameDlgCallback(const CJadeBoneList& _group, Interface& _ip);
    int CountMagicBoxesInScene();
    inline INode* GetMagicBoxNode() const {return magicBoxNode;}

    // From HitByNameDlgCallback
    virtual TCHAR *dialogTitle();
    virtual TCHAR *buttonText();
    virtual BOOL singleSelect();
    virtual BOOL useFilter();
    virtual int filter(INode *node);
    virtual BOOL useProc();
    virtual void proc(INodeTab &nodeTab);
    virtual BOOL showHiddenAndFrozen();

private:
    void RecurseCountMagicBoxes(INode* node, int& numMagicBoxes);
    
    const CJadeBoneList& group;
    Interface& ip;
    INode* magicBoxNode;
};


#endif // __JADEBONESHITBYNAMEDLGCALLBACK__H
