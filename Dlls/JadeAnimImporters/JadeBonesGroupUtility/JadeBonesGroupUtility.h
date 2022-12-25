#ifndef __JADEBONESGROUPUTILITY__H
#define __JADEBONESGROUPUTILITY__H


//#define JADEBONESGROUPUTILITY_CLASS_ID  Class_ID(0xb3b47d2, 0x4c447578)
#define JADEBONESGROUPUTILITY_CLASS_ID  Class_ID(0xb3b47d3, 0x4c447579)


class CJadeBonesGroupUtility : public UtilityObj
{
public:
    // from UtilityObj
    void BeginEditParams( Interface* ip, IUtil* iu );
    void EndEditParams( Interface* ip, IUtil* iu );

    void DeleteThis();

    //Constructor/Destructor
    CJadeBonesGroupUtility();
    ~CJadeBonesGroupUtility();

private:
    static BOOL CALLBACK DlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

    static HWND         hPanel;
    static IUtil*       iu;
    static Interface*   ip;
};


#endif // __JADEBONESGROUPUTILITY__H
