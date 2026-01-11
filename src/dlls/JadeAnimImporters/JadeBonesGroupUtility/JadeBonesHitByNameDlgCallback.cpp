#include "StdAfx.h"
#include "JadeBonesHitByNameDlgCallback.h"
#include <vector>


// From HitByNameDlgCallback
TCHAR *CJadeBonesHitByNameDlgCallback::dialogTitle()
{
    return _T("Pick bones to append");
}

TCHAR *CJadeBonesHitByNameDlgCallback::buttonText()
{
    return _T("Pick");
}

BOOL CJadeBonesHitByNameDlgCallback::singleSelect()
{
    return FALSE;
}

BOOL CJadeBonesHitByNameDlgCallback::useFilter()
{
    return TRUE;
}

int CJadeBonesHitByNameDlgCallback::filter(INode *node)
{
    if (!node)
        return FALSE;
    if (ip.GetSelNodeCount() && !node->Selected())
        return FALSE;
    if (strnicmp(node->GetName(), "b_", 2)!=0)
        return FALSE;

    for (CJadeBoneList::const_iterator itBone=group.begin(); itBone!=group.end(); ++itBone)
    {
        if (node==&itBone->GetNode())
            return FALSE;
    }

    return TRUE;
}

BOOL CJadeBonesHitByNameDlgCallback::useProc()
{
    return TRUE;
}

static int CompNodes( const void *elem1, const void *elem2 )
{
	const TCHAR* aName = (*reinterpret_cast<INode*const *>(elem1))->GetName();
	const TCHAR* bName = (*reinterpret_cast<INode*const *>(elem2))->GetName();
	int res = _tcsicmp(aName, bName);
    if (res!=0)
        return res;
    return _tcscmp(aName, bName);
}

void RecurseSetIndex(int id, int& nextIndex, const std::vector<CJadeBone*>& nodes,
                     const std::vector<std::vector<int> >& childIDs)
{
    assert(id!=-1);
    CJadeBone& bone = *nodes[id];
    assert(bone.GetIndex()==-1);

    bone.SetIndex(nextIndex);
    nextIndex++;

    const std::vector<int>& curChildIDs = childIDs[id];
    const int numChilds = curChildIDs.size();
    for (int i=0; i<numChilds; i++)
        RecurseSetIndex(curChildIDs[i], nextIndex, nodes, childIDs);
}

void CJadeBonesHitByNameDlgCallback::proc(INodeTab &nodeTab)
{
    if (!nodeTab.Count())
        return;

    nodeTab.Sort(CompNodes);

    CJadeBoneList::iterator itNextBone = group.end();
    int nextIndex = 0;
    if (!group.empty())
    {
        nextIndex = group.back().GetIndex() + 1;
        --itNextBone;
    }

    // Append the picked nodes
    int i;
    const int numNodes = nodeTab.Count();
    std::vector<CJadeBone*> nodes(numNodes, (CJadeBone*)NULL);
    for (i=0; i<numNodes; i++)
    {
        INode* node = nodeTab[i];
        assert(node);
        group.push_back(CJadeBone(*node, -1, -1, true));
        nodes[i] = &group.back();
    }

    ++itNextBone;

    // Assign the same parents as in MAX
    std::vector<CJadeBone*> parents(numNodes, (CJadeBone*)NULL);
    std::vector<int> parentIDs(numNodes, -1);
    std::vector<std::vector<int> > childIDs(numNodes);
    CJadeBoneList::iterator itBone;
    for (itBone=itNextBone, i=0; itBone!=group.end(); ++itBone, i++)
    {
        INode* parentNode = itBone->GetNode().GetParentNode();
        if (!parentNode || parentNode->IsRootNode())
            continue;

        // Search for the parent in the group
        CJadeBoneList::iterator itParentBone;
        int parentID = -1;
        for (itParentBone=group.begin(); itParentBone!=group.end(); ++itParentBone)
        {
            if (itParentBone==itNextBone)
                parentID = 0;

            if (&itParentBone->GetNode()==parentNode)
                break;

            if (parentID>=0)
                parentID++;
        }

        if (itParentBone!=group.end())
        {
            parents[i] = &*itParentBone;
            parentIDs[i] = parentID;
            if (parentID!=-1)
                childIDs[parentID].push_back(i);
        }
    }

    // Set the indices
    int index = nextIndex;
    for (itBone=itNextBone, i=0; itBone!=group.end(); ++itBone, i++)
    {
        if (itBone->GetIndex()!=-1)
            continue;

        // Find the id of the parent of this branch
        int id = i;
        while (parentIDs[id]!=-1)
            id = parentIDs[id];

        RecurseSetIndex(id, index, nodes, childIDs);
    }
    assert(index==(nextIndex+numNodes));

    // Finally, set the parent indices
    for (itBone=itNextBone, i=0; itBone!=group.end(); ++itBone, i++)
    {
        const CJadeBone* parentBone = parents[i];
        if (parentBone)
            itBone->SetParent(parentBone->GetIndex());
    }

    group.sort();
}

BOOL CJadeBonesHitByNameDlgCallback::showHiddenAndFrozen()
{
    return TRUE;
}


CMagicBoxHitByNameDlgCallback::CMagicBoxHitByNameDlgCallback(const CJadeBoneList& _group, Interface& _ip) :
group(_group),
ip(_ip),
magicBoxNode(NULL)
{
}

void CMagicBoxHitByNameDlgCallback::RecurseCountMagicBoxes(INode* node, int& numMagicBoxes)
{
    if (_tcsicmp(node->GetName(), _T("Magic Box"))==0)
    {
        numMagicBoxes++;
        magicBoxNode = node;
    }

    const int numChilds = node->NumberOfChildren();
    for (int i=0; i<numChilds; i++)
        RecurseCountMagicBoxes(node->GetChildNode(i), numMagicBoxes);
}

int CMagicBoxHitByNameDlgCallback::CountMagicBoxesInScene()
{
    int numMagicBoxes = 0;
    RecurseCountMagicBoxes(ip.GetRootNode(), numMagicBoxes);
    return numMagicBoxes;
}

// From HitByNameDlgCallback
TCHAR *CMagicBoxHitByNameDlgCallback::dialogTitle()
{
    return _T("Pick Magic Box");
}

TCHAR *CMagicBoxHitByNameDlgCallback::buttonText()
{
    return _T("Pick");
}

BOOL CMagicBoxHitByNameDlgCallback::singleSelect()
{
    return TRUE;
}

BOOL CMagicBoxHitByNameDlgCallback::useFilter()
{
    return TRUE;
}

int CMagicBoxHitByNameDlgCallback::filter(INode *node)
{
    if (!node)
        return FALSE;

    for (CJadeBoneList::const_iterator itBone=group.begin(); itBone!=group.end(); ++itBone)
    {
        if (node==&itBone->GetNode())
            return FALSE;
    }

    return TRUE;
}

BOOL CMagicBoxHitByNameDlgCallback::useProc()
{
    return TRUE;
}

void CMagicBoxHitByNameDlgCallback::proc(INodeTab &nodeTab)
{
    if (nodeTab.Count()==1)
        magicBoxNode = nodeTab[0];
}

BOOL CMagicBoxHitByNameDlgCallback::showHiddenAndFrozen()
{
    return TRUE;
}
