#ifndef __JADEBONESGROUP__H
#define __JADEBONESGROUP__H


#include "JadeBone.h"

class CWarning;


class CJadeBonesGroup
{
    friend class CJadeBonesGroupDlg;

public:
    enum ECreator {EJadeBonesGroupUtility, EExportGAS, EExportTRL};

    CJadeBonesGroup(Interface& _ip, ECreator _creator);
    bool ValidateGroup(CWarning* log=NULL) const;
    void GetFromScene(CWarning* log=NULL);
    void SetToScene() const;
    int DoModal();
    ECreator GetCreator() const {return creator;}

    const CJadeBoneList& Group() const {return group;}
    int GetNumAnimatables() const {return iNumAnimatables;}
    bool IsModified() const {return bMajorModifications || bMinorModifications;}
    bool HasMajorModifications() const {return bMajorModifications;}
    bool HasMinorModifications() const {return bMinorModifications;}
    bool IsInitLocalPosValid() const {return bIsInitLocalPosValid;}
    void ProcessMad(TCHAR* szMADFilePath, CWarning* log=NULL);
    void ProcessGas(TCHAR* szGASFilePath, CWarning* log=NULL);

    int FindBoneByINode(INode* node) const;
    CJadeBoneList::const_iterator FindInGroup(int index) const;

    void LogGroup(CWarning& log) const;

private:
    void LogError(const TCHAR* errorMsg, CWarning* log) const;
    void LogWarning(const TCHAR* errorMsg, CWarning* log) const;
    bool GetDataFromJadeBoneDataChunk(INode* node, bool bRecurse, CWarning* log);

    void ClearGroup();
    bool AppendNewBones();
    void RemoveBone(const CJadeBone& bone);
    void DecrementFollowingIndices(int index);

    void SetModified(bool bMajor);

    const CJadeBone* FindParentBone(const CJadeBone& bone, bool* pbAmbiguous=NULL) const;

    Interface& ip;
    const ECreator creator;
    CJadeBoneList group;
    int iNumAnimatables;

    TSTR statusStr;
    bool bMajorModifications;
    bool bMinorModifications;
    bool bIsInitLocalPosValid;
};


#endif // __JADEBONESGROUP__H
