/*$T MAD_export.h GC!1.32 10/20/99 15:08:20 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    MAD_export.h - Ouf File Exporter By Philippe Vimont UBI Pictures mai 99
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
//#include "BASe/BAStypes.h"

extern HINSTANCE    hInstance;

ClassDesc           *GetMadOutDesc(void);

#define VERSION 100                             /* Version number * 100 */

class MadOut : public SceneExport
{
public:
    MadOut(void);
    ~MadOut();

    /*
     ===============================================================================================
        SceneExport methods
     ===============================================================================================
     */
    int             ExtCount(void);             /* Number of extensions supported */
    const TCHAR     *Ext(int n);                /* Extension #n (i.e. "ASC") */
    const TCHAR     *LongDesc(void);            /* Long ASCII description (i.e. "Ascii Export") */
    const TCHAR     *ShortDesc(void);           /* Short ASCII description (i.e. "Ascii") */
    const TCHAR     *AuthorName(void);          /* ASCII Author name */
    const TCHAR     *CopyrightMessage(void);    /* ASCII Copyright message */
    const TCHAR     *OtherMessage1(void);       /* Other message #1 */
    const TCHAR     *OtherMessage2(void);       /* Other message #2 */
    unsigned int    Version(void);              /* Version number * 100 (i.e. v3.01 = 301) */
    void            ShowAbout(HWND hWnd);       /* Show DLL's "About..." box */
#if defined(MAX3) || defined(MAX5) || defined(MAX7) || defined(MAX8)
    int             DoExport
                    (
                        const TCHAR     *name,
                        ExpInterface    *ei,
                        Interface       *i,
                        BOOL            suppressPrompts = FALSE,
                        DWORD           Option = 0
                    );              /* Export file */
#else
    int             DoExport
                    (
                        const TCHAR     *name,
                        ExpInterface    *ei,
                        Interface       *i,
                        BOOL            suppressPrompts = FALSE
                    );              /* Export file */
#endif
    BOOL            nodeEnum(INode *node);
    BOOL            DOnodeEnum(INode * node, DWORD(WINAPI *) (MadOut * MO, INode * node));
	BOOL			HaveOneChildrenSelected(INode * node);

    ULONG   MAX_Node_To_Index(INode *MAXnode);
    ULONG   MAX_Material_To_Index(Mtl *MaxMat);
    void            ResolveNameConflict(unsigned char *, unsigned char *Scr1, unsigned char *Scr2);

    /* Other methods */

private:
    Interface       *ip;
    DWORD(WINAPI * MAD_NodeEnumCallBack) (MadOut * MO, INode * node);

	static BOOL CALLBACK	SkinDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
    BOOL            ExportSelected;
    TimeValue       ExportTime;
    FILE            *pStream;
    ULONG   NodeNumber;

    /* All the hierarchie */
    ULONG   AllNodeNumber;
    INode           **AllINode;

    /* All the materials */
    ULONG   MaximumMaterials;
    ULONG   MaterialsNumber;
    Mtl             **AllMaterials;

    /* All the textures */
    ULONG   MaximumTextures;
    ULONG   TexturesNumber;
    Texmap          **AllTexture;

    /* All object (light, camera, geometrie, targets...) */
    ULONG   AllObjRefNumber;
	ULONG   AdditionnalObjects;
    Object          **AllObjRef;
    INode           **AllObjRefINode;
    ULONG   *All_INODE_To_ObjRef;
	ULONG   *All_INODE_To_ObjRADIOSITY;
	ULONG   *All_INODE_RADIOSITY_RLI;

    /* Finaly the world */
    MAD_NodeID      **AllObjects;   /* Number = AllObjRefNumber */
    MAD_WorldNode   *Hierarchie;    /* Number = AllNodeNumber */
    MAD_World       MW;
};

