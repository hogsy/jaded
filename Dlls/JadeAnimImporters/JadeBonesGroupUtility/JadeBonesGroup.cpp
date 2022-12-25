#include "StdAfx.h"
#include "JadeBonesGroup.h"
#include "JadeBonesGroupDlg.h"
#include "JadeBonesHitByNameDlgCallback.h"
#include "JadeBonesGroupUtility.h"
#include "MAD_loadsave/sources/MAD_Struct_V0.h" // for MAD_Load
#include "MAD_mem/sources/MAD_mem.h"
#include "GAS_LoadSave/GAS_Interface.h"
#include "Warning.h"
#include "resource.h"


// Structure of the app data chunk
struct st_JadeBoneDataChunk
{
    unsigned long version;
    int index;
    int iParent;
    int iNumAnimatables;
    unsigned long flags;
};

#define JADE_BONE_DATA_CHUNK                    0
#define JADE_BONE_DATA_CHUNK_CURRENT_VERSION    0
#define JADE_BONE_DATA_CHUNK_FLAG_IS_NOT_ANIMATED   1



CJadeBonesGroup::CJadeBonesGroup(Interface& _ip, CJadeBonesGroup::ECreator _creator) :
ip(_ip),
creator(_creator),
iNumAnimatables(0),
bMajorModifications(false),
bMinorModifications(false),
bIsInitLocalPosValid(false)
{
}

void CJadeBonesGroup::LogError(const TCHAR* errorMsg, CWarning* log) const
{
    if (log)
        log->PrintError(_T("%s\r\n"), errorMsg);
    else
        MessageBox(ip.GetMAXHWnd(), errorMsg, GetString(IDS_CLASS_NAME_JADE_BONES_GROUP), MB_OK|MB_ICONERROR);
}

void CJadeBonesGroup::LogWarning(const TCHAR* errorMsg, CWarning* log) const
{
    if (log)
        log->PrintWarning(_T("%s\r\n"), errorMsg);
    else
        MessageBox(ip.GetMAXHWnd(), errorMsg, GetString(IDS_CLASS_NAME_JADE_BONES_GROUP), MB_OK|MB_ICONWARNING);
}

bool CJadeBonesGroup::ValidateGroup(CWarning* log) const
{
    TSTR strErrorMsg;

    if (group.empty())
    {
        LogError(_T("Group is empty!"), log);
        return false;
    }

    // Check if there is one bone for each index,
    // and if the hierarchy is ok
    CJadeBoneList::const_iterator itBone = group.begin();
    for (int index=0; itBone!=group.end(); index++, ++itBone)
	{
        if (itBone->GetIndex()>index)
        {
            strErrorMsg.printf(_T("There is a hole at index %d."), index);
            LogError(strErrorMsg.data(), log);
            return false;
        }

        if (itBone->GetIndex()<index)
        {
            strErrorMsg.printf(_T("There are more than one bone at index %d."), itBone->GetIndex());
            LogError(strErrorMsg.data(), log);
            return false;
        }

        if (itBone->GetParent()>=index)
        {
            strErrorMsg.printf(_T("Bone %d - %s parent's index %d must be lower than its index."),
                index, itBone->GetNode().GetName(), itBone->GetParent());
            LogError(strErrorMsg.data(), log);
            return false;
        }

        if (!itBone->IsNotAnimated() && itBone->GetParent()!=-1)
        {
            const CJadeBone* parentBone = FindParentBone(*itBone);
            if (!parentBone)
            {
                strErrorMsg.printf(_T("Animated bone %d - %s's parent %d has no associated node."),
                    index, itBone->GetNode().GetName(), itBone->GetParent());
                LogError(strErrorMsg.data(), log);
                return false;
            }
            else if (parentBone->IsNotAnimated())
            {
                strErrorMsg.printf(_T("Animated bone %d - %s's parent %d - %s is not animated."),
                    index, itBone->GetNode().GetName(), itBone->GetParent(), parentBone->GetNode().GetName());
                LogError(strErrorMsg.data(), log);
                return false;
            }
        }
	}

    return true;
}

bool CJadeBonesGroup::GetDataFromJadeBoneDataChunk(INode* node, bool bRecurse, CWarning* log)
{
    AppDataChunk *chunk = node->GetAppDataChunk(JADEBONESGROUPUTILITY_CLASS_ID, UTILITY_CLASS_ID, JADE_BONE_DATA_CHUNK);
    if (chunk && chunk->data)
    {
        const st_JadeBoneDataChunk* jadeBoneDataChunk = reinterpret_cast<const st_JadeBoneDataChunk*>(chunk->data);
        if (jadeBoneDataChunk->version>JADE_BONE_DATA_CHUNK_CURRENT_VERSION)
        {
            LogError(_T("Jade Bones Group plugin is not recent enough to read node data."), log);
            return false;
        }

        group.push_back(CJadeBone(*node, jadeBoneDataChunk->index, jadeBoneDataChunk->iParent,
            jadeBoneDataChunk->flags&JADE_BONE_DATA_CHUNK_FLAG_IS_NOT_ANIMATED));
        if (jadeBoneDataChunk->index==0)
        {
            if (iNumAnimatables==0)
                iNumAnimatables = jadeBoneDataChunk->iNumAnimatables;
            else
                iNumAnimatables = -1;
        }
    }

    if (bRecurse)
    {
        const int numChildren = node->NumberOfChildren();
        for (int i=0; i<numChildren; i++)
        {
            if (!GetDataFromJadeBoneDataChunk(node->GetChildNode(i), true, log))
                return false;
        }
    }

    return true;
}

void CJadeBonesGroup::GetFromScene(CWarning* log)
{
    if (log)
        log->Print(_T("Processing scene to get bones group.\r\n"));

    ClearGroup();
    
    const int selNodecount = ip.GetSelNodeCount();
    if (selNodecount)
    {
        for (int i=0; i<selNodecount; i++)
        {
            if (!GetDataFromJadeBoneDataChunk(ip.GetSelNode(i), false, log))
            {
                group.clear();
                break;
            }
        }
    }
    else
    {
        if (!GetDataFromJadeBoneDataChunk(ip.GetRootNode(), true, log))
            group.clear();
    }

    if (iNumAnimatables==-1)
    {
        LogWarning(_T("Group has more than one bone at index 0."), log);
        iNumAnimatables = 0;
    }

    group.sort();
    statusStr = _T("Bones group from scene");
    bMajorModifications = false;
    bMinorModifications = false;

    if (log)
        log->Print(_T("Processing scene completed.\r\n"));
}

void RemoveJadeBoneDataChunk(INode* node, bool bRecurse=false)
{
    node->RemoveAppDataChunk(JADEBONESGROUPUTILITY_CLASS_ID, UTILITY_CLASS_ID, JADE_BONE_DATA_CHUNK);

    if (bRecurse)
    {
        const int numChildren = node->NumberOfChildren();
        for (int i=0; i<numChildren; i++)
            RemoveJadeBoneDataChunk(node->GetChildNode(i), true);
    }
}

void CJadeBonesGroup::SetToScene() const
{
    assert(creator!=EExportTRL);

    const int selNodeCount = ip.GetSelNodeCount();
    if (selNodeCount)
    {
        for (int i=0; i<selNodeCount; i++)
            RemoveJadeBoneDataChunk(ip.GetSelNode(i));
    }
    else
    {
        RemoveJadeBoneDataChunk(ip.GetRootNode(), true);
    }

    for (CJadeBoneList::const_iterator itBone=group.begin(); itBone!=group.end(); ++itBone)
    {
        if (selNodeCount && !itBone->GetNode().Selected())
            RemoveJadeBoneDataChunk(&itBone->GetNode());

        st_JadeBoneDataChunk* jadeBoneDataChunk = static_cast<st_JadeBoneDataChunk*>(malloc(sizeof(st_JadeBoneDataChunk)));
        memset(jadeBoneDataChunk, 0, sizeof(st_JadeBoneDataChunk));
        jadeBoneDataChunk->version = JADE_BONE_DATA_CHUNK_CURRENT_VERSION;
        jadeBoneDataChunk->index = itBone->GetIndex();
        jadeBoneDataChunk->iParent = itBone->GetParent();
        if (itBone->IsNotAnimated())
            jadeBoneDataChunk->flags |= JADE_BONE_DATA_CHUNK_FLAG_IS_NOT_ANIMATED;
        if (itBone->GetIndex()==0)
            jadeBoneDataChunk->iNumAnimatables = iNumAnimatables;
        itBone->GetNode().AddAppDataChunk(JADEBONESGROUPUTILITY_CLASS_ID, UTILITY_CLASS_ID, JADE_BONE_DATA_CHUNK,
            sizeof(st_JadeBoneDataChunk), jadeBoneDataChunk);
    }

    SetSaveRequiredFlag();
}

int CJadeBonesGroup::DoModal()
{
    CJadeBonesGroupDlg dlg(*this, ip);
    return dlg.DoModal();
}

void CJadeBonesGroup::ProcessMad(TCHAR* szMADFilePath, CWarning* log)
{
    if (log)
        log->Print(_T("\r\nProcessing MAD file %s\r\n"), szMADFilePath);

	MAD_World *pst_MadWorld = MAD_Load(szMADFilePath, NULL, 0);
	if (!pst_MadWorld)
    {
        LogError(_T("Cannot load MAD file."), log);
        return;
    }

    ClearGroup();

	MAD_WorldNode* pst_MADNode = pst_MadWorld->Hierarchie;

    // Table that maps MAD indices to Bone indices
    std::vector<int> madToBone( pst_MadWorld->NumberOfHierarchieNodes, -1 );

    unsigned long ul;
    int iBone = 0;
	for (ul = 0; ul < pst_MadWorld->NumberOfHierarchieNodes; ul++, pst_MADNode++)
	{
		if (strnicmp(pst_MADNode->ID.Name, "b_", 2)==0)		// bone found
		{
			if (pst_MadWorld->MAD_Version >= MAD_WORLD_VERION_2)
				madToBone[ul] = pst_MADNode->Undefined4;
			else
				madToBone[ul] = iBone;
            INode* node = ip.GetINodeByName(pst_MADNode->ID.Name);

            // Check if the node has a match
            if (node)
            {
                group.push_back(CJadeBone(*node, madToBone[ul], madToBone[pst_MADNode->Parent], false));
            }
            else
            {
                TSTR strErrorMsg;
                strErrorMsg.printf(_T("Cannot find matching node for MAD bone %s."), pst_MADNode->ID.Name);
                LogWarning(strErrorMsg.data(), log);
            }

            iBone++;
		}
	}

	MAD_FREE();

    group.sort();
    iNumAnimatables = group.size();
    statusStr = szMADFilePath;
    bMajorModifications = false;
    bMinorModifications = false;

    if (creator==EExportGAS)
        SetModified(false);

    if (log)
        log->Print(_T("Processing MAD file completed.\r\n"));
}

void CJadeBonesGroup::ProcessGas(TCHAR* szGASFilePath, CWarning* log)
{
    if (log)
        log->Print(_T("\r\nProcessing GAS file %s\r\n"), szGASFilePath);

    TSTR strErrorMsg;

    GAS::ILoadSave* pLoad = GAS::ILoadSave::CreateInstance();
    assert(pLoad);
    BOOL res = pLoad->Load(szGASFilePath);
    const GAS_LoadSaveData& rLoadData = pLoad->GetData();

    if (!res)
    {
        if (rLoadData.version>GAS_CURRENT_VERSION)
            strErrorMsg.printf( "Cannot load GAS file, because its version is too recent. Editor: %d.%d, File: %d.%d",
                GAS_CURRENT_VERSION, GAS_CURRENT_SUBVERSION, rLoadData.version, rLoadData.subVersion );
        else
            strErrorMsg.printf( "Cannot load GAS file." );

        LogError(strErrorMsg.data(), log);
        return;
    }

    if (!rLoadData.numGroups)
    {
        LogError(_T("There are no groups in GAS file!"), log);
        return;
    }

    const GAS::IGroup& rGroup = pLoad->GetGroup(0);
    const GAS_GroupData& rGroupData = rGroup.GetData();

    if (_strnicmp( "bones", rGroupData.name.str, 5 )!=0)
    {
        LogError(_T("There are no bones group in GAS file!"), log);
        return;
    }

    ClearGroup();
    
    for (unsigned int iObject=0; iObject<rGroupData.numObjects; iObject++)
    {
        const char* pcszObject = rGroup.GetObject(iObject);
        INode* node = ip.GetINodeByName(pcszObject);

        // Check if the node has a match
        if (node)
        {
            int iParent = -1;
            if (rGroup.HasObjectParents())
            {
                iParent = rGroup.GetObjectParent(iObject);
            }
            else
            {
                INode* parentNode = node->GetParentNode();
                if (parentNode && !parentNode->IsRootNode())
                {
                    // Search for the parent in the group
                    CJadeBoneList::const_iterator itParentBone;
                    for (itParentBone=group.begin(); itParentBone!=group.end(); ++itParentBone)
                    {
                        if (&itParentBone->GetNode()==parentNode)
                            break;
                    }

                    if (itParentBone!=group.end())
                    {
                        iParent = itParentBone->GetIndex();
                    }
                    else
                    {
                        strErrorMsg.printf(_T("Cannot find matching GAS bone for %s's parent %s."),
                            pcszObject, parentNode->GetName());
                        LogWarning(strErrorMsg.data(), log);
                    }
                }
            }

            bool bIsNotAnimated = false;
            if (rGroup.HasObjectFlags())
                bIsNotAnimated = (rGroup.GetObjectFlags(iObject)&GAS_GROUP_OBJECT_BONE_IS_NOT_ANIMATED)!=0;

            group.push_back(CJadeBone(*node, iObject, iParent, bIsNotAnimated));

            if (rGroup.HasObjectInitLocalPos())
                group.back().SetInitLocalPos(rGroup.GetObjectInitLocalPos(iObject));
        }
        else
        {
            strErrorMsg.printf(_T("Cannot find matching node for GAS bone %s."), pcszObject);
            LogWarning(strErrorMsg.data(), log);
        }
	}

    group.sort();

    if (rGroup.HasNumAnimatableBones())
        iNumAnimatables = rGroup.GetNumAnimatableBones();
    else
        iNumAnimatables = group.empty()?0:(group.back().GetIndex()+1);

    statusStr = szGASFilePath;
    bMajorModifications = false;
    bMinorModifications = false;
    bIsInitLocalPosValid = rGroup.HasObjectInitLocalPos();

    if (creator==EExportGAS)
        SetModified(false);

    if (log)
        log->Print(_T("Processing GAS file completed.\r\n"));
}

int CJadeBonesGroup::FindBoneByINode(INode* node) const
{
    CJadeBoneList::const_iterator itBone=group.begin();
    while (itBone!=group.end() && &itBone->GetNode()!=node)
        ++itBone;
    if (itBone!=group.end())
        return itBone->GetIndex();
    return -1;
}

void CJadeBonesGroup::ClearGroup()
{
    group.clear();
    iNumAnimatables = 0;
}

bool CJadeBonesGroup::AppendNewBones()
{
    CJadeBonesHitByNameDlgCallback hbncb(group, ip);
    bool res = ip.DoHitByNameDialog(&hbncb)!=0;
    if (res)
        SetModified(false);
    return res;
}

void CJadeBonesGroup::RemoveBone(const CJadeBone& bone)
{
    CJadeBoneList::iterator itBone=group.begin();
    while (itBone!=group.end() && &*itBone!=&bone)
        ++itBone;
    if (itBone!=group.end())
        group.erase(itBone);

    SetModified(false);
}

// This method assumes that the group is sorted
void CJadeBonesGroup::DecrementFollowingIndices(int index)
{
    assert(index>=0);
    for (CJadeBoneList::iterator itBone=group.begin(); itBone!=group.end(); ++itBone)
    {
        assert(itBone->GetIndex()!=index);
        if (itBone->GetIndex()>index)
            itBone->SetIndex(itBone->GetIndex()-1);

        if (itBone->GetParent()>index)
            itBone->SetParent(itBone->GetParent()-1);
        else if (itBone->GetParent()==index)
            itBone->SetParent(-1);
    }

    SetModified(false);
}

void CJadeBonesGroup::SetModified(bool bMajor)
{
    if (bMajorModifications)
        return;

    if (bMajor)
    {
        statusStr = _T("Major Modifications");
        bMajorModifications = true;
        bIsInitLocalPosValid = false;
    }
    else if (!bMinorModifications)
    {
        statusStr += _T(" *");
        bMinorModifications = true;
    }
}

// Returns the first bone which index is the same.
// This method assumes that the group is sorted
CJadeBoneList::const_iterator CJadeBonesGroup::FindInGroup(int index) const
{
    CJadeBoneList::const_iterator itBone=group.begin();
    while (itBone!=group.end() && itBone->GetIndex()<index)
        ++itBone;

    if (itBone->GetIndex()==index)
        return itBone;
    return group.end();
}

const CJadeBone* CJadeBonesGroup::FindParentBone(const CJadeBone& bone, bool* pbAmbiguous) const
{
    if (pbAmbiguous)
        *pbAmbiguous = false;

    if (bone.GetParent()==-1)
        return NULL;

    // Find the first bone which has this index
    CJadeBoneList::const_iterator itParentBone = FindInGroup(bone.GetParent());
    if (itParentBone==group.end())
        return NULL;

    const CJadeBone* parentBone = &*itParentBone;

    if (pbAmbiguous)
    {
        // If the next bone in the group has the same index, there is ambiguity
        itParentBone++;
        if (itParentBone!=group.end() && itParentBone->GetIndex()==bone.GetParent())
            *pbAmbiguous = true;
    }
    
    return parentBone;
}

void CJadeBonesGroup::LogGroup(CWarning& log) const
{
    if (group.empty())
    {
        log.Print(_T("    Group empty\r\n"));
        return;
    }

    for (CJadeBoneList::const_iterator itBone=group.begin(); itBone!=group.end(); ++itBone)
    {
        log.Print(_T("    %s %.2d - %s"),
            (itBone->GetIndex()<iNumAnimatables && !itBone->IsNotAnimated())?_T("*"):_T(" "),
            itBone->GetIndex(), itBone->GetNode().GetName());
        if (itBone->GetParent()!=-1)
            log.Print(_T("\t%.2d"), itBone->GetParent());
        log.Print(_T("\r\n"));
    }
}
