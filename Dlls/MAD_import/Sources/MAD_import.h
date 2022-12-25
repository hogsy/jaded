/*$T MAD_import.h GC!1.32 05/25/99 18:51:37 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    MAD_Import.h - Ouf File Importer By Philippe Vimont UBI Pictures mai 99
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
extern HINSTANCE    hInstance;

ClassDesc           *GetMadInDesc(void);

#define VERSION 100                             /* Version number * 100 */



class MadIn : public SceneImport
{
public:
    MadIn(void);
    ~MadIn();

    /*
     ===============================================================================================
        SceneImport methods
     ===============================================================================================
     */
    int             ExtCount(void);             /* Number of extensions supported */
    const TCHAR     *Ext(int n);                /* Extension #n (i.e. "ASC") */
    const TCHAR     *LongDesc(void);            /* Long ASCII description (i.e. "Ascii Import") */
    const TCHAR     *ShortDesc(void);           /* Short ASCII description (i.e. "Ascii") */
    const TCHAR     *AuthorName(void);          /* ASCII Author name */
    const TCHAR     *CopyrightMessage(void);    /* ASCII Copyright message */
    const TCHAR     *OtherMessage1(void);       /* Other message #1 */
    const TCHAR     *OtherMessage2(void);       /* Other message #2 */
    unsigned int    Version(void);              /* Version number * 100 (i.e. v3.01 = 301) */
    void            ShowAbout(HWND hWnd);       /* Show DLL's "About..." box */
    int             DoImport
                    (
                        const TCHAR     *name,
                        ImpInterface    *ei,
                        Interface       *i,
                        BOOL            suppressPrompts = FALSE
                    );                          /* Import file */
    void            MAD_Color_To_MAX_Color(MAD_ColorARGB *MadColor, Color *MaxColor);
    void            MAD_Matrix_To_MAX_Matrix(MAD_Matrix *MadMatrix, Matrix3 *MaxMAt);
    Texmap          *MAD_Tex_To_MAX_Tex(MAD_texture *MadTexture);
    Object          *MAD_Object_To_MAX_Object(MAD_GeometricObject *MADMesh);
    Object          *MAD_Light_To_MAX_Light(MAD_Light *MADNode);
    Object          *MAD_Cam_To_MAX_Cam(MAD_Camera *MADNode);
    Object          *MAD_Dum_To_MAX_Dum(MAD_NodeID *MADNode);
    Object          *MAD_Shape_To_MAX_Shape(MAD_NodeID *MADNode);
    Object          *MAD_Target_To_MAX_Target(MAD_NodeID *MADNode);

    Object          *MAD_X_To_MAX_X(MAD_NodeID *MADNode);
    Mtl             *MAD_Simple_MAT_To_MAX_Simple_Mat(MAD_StandarMaterial *MadMat);
    Mtl             *MAD_Cplx_MAT_To_MAX_Cplx_Mat(MAD_MultiMaterial *MadMat);
    //Mtl             *MAD_MultiTex_MAT_To_MAX_MultiTex_Mat(MAD_MultiTexMaterial *MadMat);

    /* Other methods */

private:
    ImpInterface    *ii;
    TimeValue       ImportTime;

public:
    Interface       *ip;
    BOOL            MERGE;
    BOOL            MERGETEXTURES;
    char            TexturePATH[260];

    /* replace textures options */
    int             ReplaceOption; /* 0 replace , 1 rename , 2 skip  */
    BOOL            ASKUSERFORTEXTURECONFLICT;
    BOOL            USER_AS_BEEN_PREVENT_ON_WO;
	BOOL            USER_AS_BEEN_PREVENT_ON_DNE;
    char            *CurrentTextureName;

    MAD_World       *MW;
    INode           **AllINode;
    Mtl             **AllMaterials;
    Texmap          **AllTexture;
    Object          **AllObjRef;
    ImpNode         **AllImpNodes;
    INode           **AllINodes;

private:
	BOOL			mb_SetFullOpacity;
	BOOL			mb_ImportMultiTexMat;
};

