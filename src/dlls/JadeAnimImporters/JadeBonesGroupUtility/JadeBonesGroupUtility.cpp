#include "StdAfx.h"
#include "JadeBonesGroupUtility.h"
#include "JadeBonesGroup.h"
#include "resource.h"


static CJadeBonesGroupUtility theJadeBonesGroupUtility;

class CJadeBonesGroupUtilityClassDesc:public ClassDesc2
{
public:
    int             IsPublic() {return 1;}
    void *          Create(BOOL loading = FALSE) {return &theJadeBonesGroupUtility;}
    const TCHAR *   ClassName() {return GetString(IDS_CLASS_NAME_JADE_BONES_GROUP);}
    SClass_ID       SuperClassID() {return UTILITY_CLASS_ID;}
    Class_ID        ClassID() {return JADEBONESGROUPUTILITY_CLASS_ID;}
    const TCHAR*    Category() {return GetString(IDS_CATEGORY);}
    const TCHAR*    InternalName() { return _T("UbiJadeBonesGroupUtility"); }    // returns fixed parsable name (scripter-visible name)
    HINSTANCE       HInstance() { return hInstance; }               // returns owning module handle
};

static CJadeBonesGroupUtilityClassDesc theJadeBonesGroupUtilityDesc;
ClassDesc2* GetJadeBonesGroupUtilityDesc() {return &theJadeBonesGroupUtilityDesc;}

HWND CJadeBonesGroupUtility::hPanel = NULL;
IUtil* CJadeBonesGroupUtility::iu = NULL;
Interface* CJadeBonesGroupUtility::ip = NULL;

BOOL CALLBACK CJadeBonesGroupUtility::DlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch (msg)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_INFO:
            ShellExecute(theJadeBonesGroupUtility.ip->GetMAXHWnd(), "open", GetString(IDS_INFO_LINK), NULL, "", SW_SHOW);
            break;
            
        case IDC_EDIT_BONES_GROUP:
            {
                CJadeBonesGroup jbg(*ip, CJadeBonesGroup::EJadeBonesGroupUtility);
                jbg.GetFromScene();
                jbg.DoModal();
            }
            break;
        }
        break;


    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MOUSEMOVE:
        theJadeBonesGroupUtility.ip->RollupMouseMessage( hWnd, msg, wParam, lParam );
        break;

    default:
        return FALSE;
    }
    return TRUE;
}


//--- CJadeBonesGroupUtility -------------------------------------------------------
CJadeBonesGroupUtility::CJadeBonesGroupUtility()
{
}

CJadeBonesGroupUtility::~CJadeBonesGroupUtility()
{
}


void CJadeBonesGroupUtility::DeleteThis()
{
}

void CJadeBonesGroupUtility::BeginEditParams( Interface* ip, IUtil* iu )
{
    CJadeBonesGroupUtility::iu = iu;
    CJadeBonesGroupUtility::ip = ip;

    hPanel = ip->AddRollupPage( hInstance, MAKEINTRESOURCE(IDD_JADE_BONES_GROUP_ROLLUP), DlgProc, GetString(IDS_PARAMS_JADE_BONES_GROUP_UTILITY), 0 );

    // Set the info text
    HWND hInfo = GetDlgItem(hPanel, IDC_INFO);
    const int textLength = GetWindowTextLength(hInfo);
    if (textLength)
    {
        TCHAR* szBuffer = new TCHAR[textLength+1];
        TCHAR* szBuffer2 = new TCHAR[textLength+32];

        GetWindowText(hInfo, szBuffer, textLength+1);
        _stprintf(szBuffer2, szBuffer, C_szVersionString);
        SetWindowText(hInfo, szBuffer2);

        delete [] szBuffer;
        delete [] szBuffer2;
    }
}

void CJadeBonesGroupUtility::EndEditParams( Interface* ip, IUtil* iu ) 
{
    CJadeBonesGroupUtility::iu = NULL;
    CJadeBonesGroupUtility::ip = NULL;
    ip->DeleteRollupPage( hPanel );
    hPanel = NULL;
}
