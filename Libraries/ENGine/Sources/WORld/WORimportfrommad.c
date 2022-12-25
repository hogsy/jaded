/*$T WORimportfrommad.c GC!1.71 02/21/00 12:09:13 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "EDIpaths.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/BAStypes.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGread.h"
#include "LINKs/LINKmsg.h"
#include "LINKs/LINKtoed.h"
#include "MAD_mem/Sources/Mad_mem.h"
#include "MAD_loadsave/Sources/MAD_Struct_V0.h"

#include "GRObject/GROimportfrommad.h"
#include "MATerial/MATstruct.h"
#include "TEXture/TEXfile.h"
#include "TEXture/TEXstruct.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOload.h"
#include "GEOmetric/GEOstaticLOD.h"
#include "GEOmetric/GEO_SKIN.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "WAYpoint/WAYpoint.h"

#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJgrp.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/OBJects/OBJsave.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/WORld/WORimportfrommad.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/WORld/WORuniverse.h"
#include "ENGine/Sources/ENGstring.h"

/*$4
 ***********************************************************************************************************************
    Structure
 ***********************************************************************************************************************
 */

typedef struct  WOR_tdst_Import_Info_
{
    LONG        l_Flags;
    LONG        l_TableIndex;
    BIG_INDEX   ul_Bank;
    char        sz_Name[BIG_C_MaxLenName];
} WOR_tdst_Import_Info;

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

#define C_LogWidth              80

#define WOR_C_IFU_GONever       0x00000000
#define WOR_C_IFU_GOChild       0x00000001
#define WOR_C_IFU_GOAlways      0x00000002
#define WOR_C_IFU_GOMask        0x00000003

#define WOR_C_IFU_GrmNever      0x00000000
#define WOR_C_IFU_GrmChild      0x00000001
#define WOR_C_IFU_GrmAlways     0x00000002
#define WOR_C_IFU_GrmMask       0x00000003
#define WOR_C_IFU_GrmMerge      0x00000004
#define WOR_C_IFU_GrmNoSingle   0x00000008

#define WOR_C_IFU_GroNever      0x00000000
#define WOR_C_IFU_GroChild      0x00000001
#define WOR_C_IFU_GroAlways     0x00000002
#define WOR_C_IFU_GroMask       0x00000003
#define WOR_C_IFU_GroMerge      0x00000004
#define WOR_C_IFU_GroMesh       0x00000008
#define WOR_C_IFU_GroRLI        0x00000010
#define WOR_C_IFU_GroParams     0x00000020

#define WOR_C_OIF_Update        0x00000001
#define WOR_C_OIF_Create        0x00000002
#define WOR_C_OIF_In            0x00000004
#define WOR_C_OIF_Out           0x00000008
#define WOR_C_OIF_Treated       0x00000010
#define WOR_C_OIF_TexBadFormat  0x00000020
#define WOR_C_OIF_TexBadSize    0x00000040
#define WOR_C_OIF_TexInLib      0x00000080
#define WOR_C_OIF_TexInTrash    0x00000100
#define WOR_C_OIF_AddedInTable  0x00000200
#define WOR_C_OIF_AlreadyLoaded 0x00000400

#define WOR_C_IT_Object         0
#define WOR_C_IT_Material       1
#define WOR_C_IT_Texture        2

LONG                    WOR_gl_ImportGO = WOR_C_IFU_GOChild;
LONG                    WOR_gl_ImportGrm = WOR_C_IFU_GrmChild;
LONG                    WOR_gl_ImportGro = WOR_C_IFU_GroChild;

LONG                    WOR_gl_ImportForced = 0;
LONG                    WOR_gl_ImportAutomatic = 1;

LONG                    WOR_gl_ImportAsBank;

WOR_tdst_Import_Info    *WOR_gpst_ObjInfo = NULL;
WOR_tdst_Import_Info    *WOR_gpst_MatInfo = NULL;
WOR_tdst_Import_Info    *WOR_gpst_TexInfo = NULL;
LONG                    WOR_gl_NbTexInfos = 0;
static char             *WOR_gssz_WorldName;
static char             WOR_gsz_GroDir[BIG_C_MaxLenPath];

/*$4
 ***********************************************************************************************************************
    Private macros
 ***********************************************************************************************************************
 */

#define C_IF_Dummy \
        ( \
            OBJ_C_IdentityFlag_HasInitialPos\
        )

#define C_IF_Object \
        ( \
            OBJ_C_IdentityFlag_HasInitialPos | OBJ_C_IdentityFlag_BaseObject | \
                OBJ_C_IdentityFlag_Visu | OBJ_C_IdentityFlag_OBBox \
        )

#define C_IF_Light \
        ( \
            OBJ_C_IdentityFlag_HasInitialPos | OBJ_C_IdentityFlag_ExtendedObject | \
                OBJ_C_IdentityFlag_Lights \
        )

#define C_IF_LightOmni (OBJ_C_IdentityFlag_HasInitialPos | OBJ_C_IdentityFlag_ExtendedObject | OBJ_C_IdentityFlag_Lights)

#define C_IF_Waypoint (OBJ_C_IdentityFlag_HasInitialPos | OBJ_C_IdentityFlag_Waypoints)

#define Out(text, color) \
    LINK_gul_ColorTxt = color; \
    LINK_PrintStatusMsg(text); \
    LINK_gul_ColorTxt = 0;

/*$4
 ***********************************************************************************************************************
    Private functions prototypes
 ***********************************************************************************************************************
 */

void    WOR_ImportTexturesFromMadCallback(FILE *, int, char *, int);
LONG    WOR_l_World_ImportTextures(WOR_tdst_World *, MAD_World *, LONG);
LONG    WOR_l_World_ImportObjects(WOR_tdst_World *, MAD_World *, char *);
LONG    WOR_l_World_ImportMaterials(WOR_tdst_World *, MAD_World *, char *);
LONG    WOR_l_World_ImportGameObjects(WOR_tdst_World *, MAD_World *, char *);
LONG    WOR_l_World_MergeGameObjects(WOR_tdst_World *, char *);
void    WOR_World_DisplayInfo(WOR_tdst_World *, MAD_World *, LONG);
BOOL    WOR_b_CheckWorldBeforeMerging( BIG_INDEX, int );

/*$4
 ***********************************************************************************************************************
    Public functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_Import_ResetFlags(void)
{
    WOR_gl_ImportForced = 0;
    WOR_gl_ImportAutomatic = 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL WOR_b_World_Import(char *_psz_BigPathName, char *_psz_BigFileName, char *_psz_ExternName)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    WOR_tdst_World      *pst_World, *pst_OldWorld;
    MAD_World           *pst_MadWorld;
    char                *sz_Name, *sz_Ext, *sz_Name2;
    LONG                l_TextureInFile;
    char                sz_WorldName[BIG_C_MaxLenName], sz_WorldFileName[BIG_C_MaxLenName];
    char                sz_Path[BIG_C_MaxLenPath], sz_Trash[BIG_C_MaxLenPath];
    char                *psz_Text, sz_Text[ 1024 ];
    BIG_INDEX           ul_Index, ul_Dir;
    LONG                l_SaveFlags;
    BIG_INDEX           ul_OldWorldIndex;
    struct L_finddata_t st_FileInfo;
    ULONG               ul_Handle;
    BOOL                b_New;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Init */
    l_TextureInFile = 0;
    if(BIG_gpsz_RealName) *BIG_gpsz_RealName = 0;

    /* Look if importation is done in good directory */
    ul_Dir = BIG_ul_SearchDir(_psz_BigPathName);
    if(BIG_b_IsDirInDirRec(BIG_ul_SearchDir(EDI_Csz_Path_Objects), ul_Dir))
        WOR_gl_ImportAsBank = 1;
    else if(BIG_b_IsDirInDirRec(BIG_ul_SearchDir(EDI_Csz_Path_Levels), ul_Dir))
        WOR_gl_ImportAsBank = 0;
    else
    {
        ERR_X_Warning(0, ENG_STR_Csz_ImportFromMadFailed, NULL);
        ERR_X_Warning(0, ENG_STR_Csz_BadMadImportDirectory, NULL);
        return 0;
    }

    /* Search for a wow in importation directory */
    ul_Dir = BIG_ul_SearchDir(_psz_BigPathName);
    if(ul_Dir != BIG_C_InvalidIndex)
    {
        ul_Index = BIG_FirstFile(ul_Dir);
        while(ul_Index != BIG_C_InvalidIndex)
        {
            /* Compare extension */
            if(BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtWorld))
                break;

            /* Pass to brother */
            ul_Index = BIG_NextFile(ul_Index);
        }
    }

    /* Compute world name */
    sz_Ext = strrchr(_psz_ExternName, '.');
    *sz_Ext = 0;
    sz_Name = strrchr(_psz_ExternName, '\\');
    sz_Name2 = strrchr(_psz_ExternName, '/');
    if(sz_Name == NULL)
    {
        if(sz_Name2 == NULL)
            sz_Name = _psz_ExternName;
        else
            sz_Name = sz_Name2 + 1;
    }
    else
    {
        if((sz_Name2 == NULL) || (sz_Name2 < sz_Name))
            sz_Name++;
        else
            sz_Name = sz_Name2 + 1;
    }

    /* Check for new world or merging */
    b_New = 1;
    if(ul_Index != BIG_C_InvalidIndex)
    {
        L_strcpy(sz_WorldName, BIG_NameFile(ul_Index));
        *strrchr(sz_WorldName, '.') = 0;
        if(L_stricmp(sz_WorldName, sz_Name) == 0)
            b_New = 0;
        else
        {
            sprintf(sz_Path, "Merge %s into %s ?", sz_Name, sz_WorldName);
            if(MessageBox(MAI_gh_MainWindow, sz_Path, "Merge ?", MB_ICONQUESTION | MB_YESNO) == IDYES)
                b_New = 0;
        }
    }

    L_strcpy(sz_Path, _psz_BigPathName);
    if(b_New)
    {
        L_strcpy(sz_WorldName, sz_Name);
        L_strcat(sz_Path, "/");
        L_strcat(sz_Path, sz_WorldName);
    }
    else
    {
        if ( !WOR_b_CheckWorldBeforeMerging( ul_Dir, 0 ) )
        {
            psz_Text = sz_Text + sprintf(sz_Text, "Warning : there's gameobject outside GameObject dir ?\n", sz_Name, sz_WorldName);
            psz_Text += sprintf(psz_Text, "If you continue Jade may crash !\n", sz_Name, sz_WorldName);
            psz_Text += sprintf(psz_Text, "continue anyway ???", sz_Name, sz_WorldName);
            if( MessageBox(MAI_gh_MainWindow, sz_Text, "!!! BIG WARNING !!!", MB_ICONQUESTION | MB_YESNO) == IDNO )
                return 0;
        }
    }

    /* Compute path and file name of wow that will be created/modified */
    L_strcpy(sz_WorldFileName, sz_WorldName);
    L_strcat(sz_WorldFileName, EDI_Csz_ExtWorld);

    *sz_Ext = '.';

    /* Watch if world already exist */
    if (b_New)
    {
        ul_Dir = BIG_ul_SearchDir(sz_Path);
        if(ul_Dir == BIG_C_InvalidIndex)
            ul_OldWorldIndex = BIG_C_InvalidIndex;
        else
        {
            ul_OldWorldIndex = BIG_ul_SearchFile(ul_Dir, sz_WorldFileName);
            b_New = 0;
        }
    }
    else
        ul_OldWorldIndex = ul_Index;

    /* Open mad file */
    Out(ENG_STR_Csz_ImportFromMadStart, 0x00800080);

    /* Compare date of the two files */
    if((BIG_gb_ImportWithExt) || (!WOR_gl_ImportForced))
    {
        if(ul_OldWorldIndex != BIG_C_InvalidIndex)
        {
            ul_Handle = L_findfirst(_psz_ExternName, &st_FileInfo);
            L_findclose(ul_Handle);
            if(st_FileInfo.time_write <= BIG_TimeFile(ul_OldWorldIndex))
            {
                LINK_PrintStatusMsg(ENG_STR_Csz_MadFileOlder);
                return 0;
            }
        }
    }

    /* Close mad world */
    WOR_gssz_WorldName = sz_WorldName;
    Mad_meminit();
    pst_MadWorld = MAD_Load(_psz_ExternName, WOR_ImportTexturesFromMadCallback, (int) & l_TextureInFile);

    if(pst_MadWorld == NULL)
    {
        ERR_X_Warning(0, ENG_STR_Csz_ImportFromMadFailed, NULL);
        ERR_X_Warning(0, ENG_STR_Csz_BadMadFile, NULL);
        return 0;
    }

    /* Allocate and init world */
    if (b_New)
    {
        LINK_PrintStatusMsg(ENG_STR_Csz_NewWorld);
        pst_World = WOR_pst_World_Create();
        WOR_World_Init(pst_World, pst_MadWorld->NumberOfHierarchieNodes - 1);
        strcpy(pst_World->sz_Name, sz_WorldName);
        pst_OldWorld = NULL;
        
        pst_World->ul_AmbientColor = pst_MadWorld->AmbientColor;
    }
    else
    {
        Out(ENG_STR_Csz_LoadOldWorld, 0x00800080);
        pst_OldWorld = WOR_pst_Universe_AddWorld(BIG_FileKey(ul_OldWorldIndex), 1, TRUE);
        pst_World = pst_OldWorld;
    }

    WOR_gpst_WorldToLoadIn = pst_World;

    /* Display information about importing textures */
    WOR_World_DisplayInfo(pst_World, pst_MadWorld, WOR_C_IT_Texture);
   
    /* Import object */
    Out(ENG_STR_Csz_ImportObjects, 0x00800080);
    if(WOR_l_World_ImportObjects(pst_World, pst_MadWorld, sz_Path))
        return 0;

    /* Import textures */
    Out(ENG_STR_Csz_ImportTextures, 0x00800080);
    if(WOR_l_World_ImportTextures(pst_World, pst_MadWorld, l_TextureInFile))
        return 0;

    /* Import materials */
    Out(ENG_STR_Csz_ImportMaterials, 0x00800080);
    if(WOR_l_World_ImportMaterials(pst_World, pst_MadWorld, sz_Path))
        return 0;

    /* Import game objects */
    Out(ENG_STR_Csz_ConvertMadWorld, 0x00800080);
    if(WOR_l_World_ImportGameObjects(pst_World, pst_MadWorld, sz_Path))
        return 0;

    /* Merge gameobjects */
    Out(ENG_STR_Csz_MergeGameObjects, 0x00800080);
    if(WOR_l_World_MergeGameObjects(pst_World, sz_Path))
        return 0;

    LINK_PrintStatusMsg(ENG_STR_Csz_ImportFromMadSuccess);

    /* Free infos array */
    if(WOR_gpst_ObjInfo) L_free(WOR_gpst_ObjInfo);
    WOR_gpst_ObjInfo = NULL;
    if(WOR_gpst_MatInfo) L_free(WOR_gpst_MatInfo);
    WOR_gpst_MatInfo = NULL;
    if(WOR_gpst_TexInfo) L_free(WOR_gpst_TexInfo);
    WOR_gpst_TexInfo = NULL;
    WOR_gl_NbTexInfos = 0;

    /* Now save the world */
    l_SaveFlags = (WOR_gl_ImportAsBank ? WOR_C_IsABank : 0) | WOR_C_DoNotSaveNetwork;
    OBJ_gl_SaveOnlyImportedDataFromMad = 1;
    WOR_l_World_SaveWithFileName(pst_World, sz_Path, sz_WorldName, l_SaveFlags);
    OBJ_gl_SaveOnlyImportedDataFromMad = 0;

    /* Move unmodified object */
    if(BIG_gb_ImportDirMirror)
    {
        ul_Index = BIG_ul_SearchDir(sz_Path);
        if(ul_Index != BIG_C_InvalidIndex)
        {
            L_strcpy(sz_Trash, EDI_Csz_Path_TrashCan);
            L_strcat(sz_Trash, sz_Path + L_strlen(EDI_Csz_Path_GameData));
            BIG_MoveUnchangedFiles(ul_Index, sz_Trash, ".gro\0.grm\0");
        }
    }

    /* Destroy world */
    if (b_New)
    {
        WOR_World_Close(pst_World);
        MEM_Free(pst_World);
    }
    else
    {
        WOR_Universe_DeleteWorld(pst_OldWorld, 2);
    }

    /* Test if there's warning(s) */
    LINK_Refresh();

    return 0;
}

/*$4
 ***********************************************************************************************************************
    Private functions
 ***********************************************************************************************************************
 */

/*$5
 #######################################################################################################################
    Texture functions
 #######################################################################################################################
 */

/*
 =======================================================================================================================
    Aim:    This is a callback function called by mad world loading function for each texture that
            is in mad file.
 =======================================================================================================================
 */
void WOR_ImportTexturesFromMadCallback(FILE *_hFile, int _iFileSize, char *_sz_Name, int _iParam)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX               ul_TextureDir;
    BIG_INDEX               ul_File;
    char                    *psz_ShortName, sz_ShortName[256];
    char                    *pc_Buffer;
    char                    sz_Path[BIG_C_MaxLenPath];
    WOR_tdst_Import_Info    *pst_Info;
    TEX_tdst_File_Params    st_TextureParams;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_iParam) *(LONG *) _iParam = 1;

    psz_ShortName = L_strrchr(_sz_Name, '/');
    if(psz_ShortName == NULL) psz_ShortName = L_strrchr(_sz_Name, '\\');

    if(psz_ShortName == NULL)
        psz_ShortName = _sz_Name;
    else
        psz_ShortName++;

    /* Create new info slot to store info about texture importation */
    if(WOR_gl_NbTexInfos == 0)
        WOR_gpst_TexInfo = (WOR_tdst_Import_Info *) L_malloc(sizeof(WOR_tdst_Import_Info));
    else
    {
        WOR_gpst_TexInfo = (WOR_tdst_Import_Info *) L_realloc
            (
                WOR_gpst_TexInfo,
                sizeof(WOR_tdst_Import_Info) * (WOR_gl_NbTexInfos + 1)
            );
    }

    pst_Info = WOR_gpst_TexInfo + WOR_gl_NbTexInfos++;

    L_strcpy(pst_Info->sz_Name, psz_ShortName);
    pst_Info->l_Flags = 0;
    pst_Info->ul_Bank = BIG_C_InvalidIndex;
    pst_Info->l_TableIndex = -1;

    strcpy(sz_Path, EDI_Csz_Path_TrashCan_Textures);

    /* Check for extension */
    if(TEX_l_File_IsFormatSupported(psz_ShortName, -1))
    {
        ul_TextureDir = BIG_ul_CreateDir(EDI_Csz_Path_Textures);
        ul_File = BIG_ul_SearchFileInDirRec(ul_TextureDir, psz_ShortName);

        /* File exist in library : link texture to existent one */
        if(ul_File != BIG_C_InvalidIndex)
        {
            pst_Info->l_Flags |= WOR_C_OIF_TexInLib;
            pst_Info->ul_Bank = BIG_ParentFile(ul_File);

            BIG_ComputeFullName(BIG_ParentFile(ul_File), sz_ShortName);
            L_strcat(sz_ShortName, "/");
            L_strcat(sz_ShortName, psz_ShortName);
            L_strcpy(_sz_Name, sz_ShortName);
            L_fseek(_hFile, _iFileSize, SEEK_CUR);
            return;
        }

        /* File isn't in library : create new texture in trashcan texture dir */
        if(_iFileSize)
        {
            if(WOR_gl_ImportAsBank)
                strcat(sz_Path, "/FromBankImport/");
            else
                strcat(sz_Path, "/FromLevelImport/");
            strcat(sz_Path, WOR_gssz_WorldName);
            BIG_ul_CreateDir(sz_Path);
            pc_Buffer = (char *) L_malloc(_iFileSize + 32);
            L_fread(pc_Buffer, 1, _iFileSize, _hFile);

            if ( TEX_i_File_DefaultParams(&st_TextureParams, sz_Path, psz_ShortName, pc_Buffer, _iFileSize) )
            {
                L_memcpy(pc_Buffer + _iFileSize, &st_TextureParams, 32);
                BIG_UpdateFileFromBuffer(sz_Path, psz_ShortName, pc_Buffer, _iFileSize + 32);
            }
            L_free(pc_Buffer);
            pst_Info->l_Flags |= WOR_C_OIF_TexInTrash;

        }
        else
        {
            pst_Info->l_Flags |= WOR_C_OIF_TexBadSize;
        }
    }
    else
    {
        L_fseek(_hFile, _iFileSize, SEEK_CUR);
        pst_Info->l_Flags |= WOR_C_OIF_TexBadFormat;
    }

    L_strcpy(sz_ShortName, psz_ShortName);
    sprintf(_sz_Name, "%s/%s", sz_Path, sz_ShortName);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG WOR_l_World_ImportTextures(WOR_tdst_World *_pst_World, MAD_World *_pst_MadWorld, LONG l_TextureInFile)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_texture **pst_Texture, **pst_LastTexture;
    BIG_INDEX   ul_TrashDir, ul_Dir, ul_File, ul_SubDir;
    BIG_KEY     ul_Key;
    char        *sz_Name, *sz_Slash;
    int         i_TextureIndex;
    char        sz_Message[300];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ul_Dir = BIG_ul_CreateDir(EDI_Csz_Path_Textures);
    ul_TrashDir = BIG_ul_CreateDir(EDI_Csz_Path_TrashCan_Textures);

    pst_Texture = _pst_MadWorld->AllTextures;
    pst_LastTexture = pst_Texture + _pst_MadWorld->NumberOftexture;
    for(i_TextureIndex = 0; pst_Texture < pst_LastTexture; pst_Texture++, i_TextureIndex++)
    {
        /* Init */
        TEX_gal_MadToGlobalIndex[i_TextureIndex] = -1;

        if(l_TextureInFile)
        {
            sz_Slash = strrchr((*pst_Texture)->Texturefile, '/');
            if(sz_Slash)
            {
                *sz_Slash = 0;
                ul_SubDir = BIG_ul_SearchDir((*pst_Texture)->Texturefile);
                *sz_Slash = '/';
                if(ul_SubDir != BIG_C_InvalidIndex)
                {
                    ul_File = BIG_ul_SearchFile(ul_SubDir, sz_Slash + 1);
                    ul_Key = (ul_File == BIG_C_InvalidIndex) ? BIG_C_InvalidKey : BIG_FileKey(ul_File);
                    TEX_gal_MadToGlobalIndex[i_TextureIndex] = TEX_w_List_AddTexture(&TEX_gst_GlobalList, ul_Key, 0);
                }
            }
        }
        else
        {
            /* Try to find texture in bigfile */
            sz_Name = strrchr((*pst_Texture)->Texturefile, '/');
            if(sz_Name == NULL) sz_Name = strrchr((*pst_Texture)->Texturefile, '\\');
            if(sz_Name != NULL)
                sz_Name++;
            else
                sz_Name = (*pst_Texture)->Texturefile;

            ul_File = BIG_ul_SearchFileInDirRec(ul_Dir, sz_Name);
            if(ul_File == BIG_C_InvalidIndex) ul_File = BIG_ul_SearchFileInDirRec(ul_TrashDir, sz_Name);
            ul_Key = (ul_File == BIG_C_InvalidIndex) ? BIG_C_InvalidKey : BIG_FileKey(ul_File);
            TEX_gal_MadToGlobalIndex[i_TextureIndex] = (LONG) TEX_w_List_AddTexture
                (
                    &TEX_gst_GlobalList,
                    ul_Key, 0
                );
        }

        /* Not found : set as invalid texture */
        if ( (TEX_gal_MadToGlobalIndex[i_TextureIndex] == -1) && (*(*pst_Texture)->Texturefile) )
        {
            sprintf(sz_Message, "....Texture %s not found", (*pst_Texture)->Texturefile);
            ERR_X_Warning(0, sz_Message, NULL);
        }
    }

    return 0;
}

/*$5
 #######################################################################################################################
    Graphic object functions
 #######################################################################################################################
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG WOR_l_MergeGeometricGro(GRO_tdst_Struct *pst_Local, GRO_tdst_Struct *pst_Extern)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_Object *objl, *obje, swap;
    LONG            l_Size;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    objl = (GEO_tdst_Object *) pst_Local;
    obje = (GEO_tdst_Object *) pst_Extern;

    if((WOR_gl_ImportGro & WOR_C_IFU_GroMesh) & (WOR_gl_ImportGro & WOR_C_IFU_GroRLI))
        return 1;

    /* Merge mesh */
    if(WOR_gl_ImportGro & WOR_C_IFU_GroMesh)
    {
        if(obje->l_NbPoints != objl->l_NbPoints)
        {
            if(obje->dul_PointColors)
            {
                l_Size = obje->l_NbPoints * 4;
                obje->dul_PointColors = (ULONG *) MEM_p_Realloc(obje->dul_PointColors, l_Size);
            }
        }

        swap.l_NbPoints = objl->l_NbPoints;
        swap.dst_Point = objl->dst_Point;
        swap.dst_PointNormal = objl->dst_PointNormal;
        swap.l_NbUVs = objl->l_NbUVs;
        swap.dst_UV = objl->dst_UV;
        swap.l_NbElements = objl->l_NbElements;
        swap.dst_Element = objl->dst_Element;

        objl->l_NbPoints = obje->l_NbPoints;
        objl->dst_Point = obje->dst_Point;
        objl->dst_PointNormal = obje->dst_PointNormal;
        objl->l_NbUVs = obje->l_NbUVs;
        objl->dst_UV = obje->dst_UV;
        objl->l_NbElements = obje->l_NbElements;
        objl->dst_Element = obje->dst_Element;

        obje->l_NbPoints = swap.l_NbPoints;
        obje->dst_Point = swap.dst_Point;
        obje->dst_PointNormal = swap.dst_PointNormal;
        obje->l_NbUVs = swap.l_NbUVs;
        obje->dst_UV = swap.dst_UV;
        obje->l_NbElements = swap.l_NbElements;
        obje->dst_Element = swap.dst_Element;
    }

    /* Merge RLI */
    if(WOR_gl_ImportGro & WOR_C_IFU_GroRLI)
    {
        if(obje->dul_PointColors == NULL) return 1;

        l_Size = obje->l_NbPoints;
        if(objl->l_NbPoints < l_Size) l_Size = objl->l_NbPoints;
        l_Size *= 4;

        L_memcpy(objl->dul_PointColors, obje->dul_PointColors, l_Size);
    }

    /* merge skin */
    if ( GEO_SKN_IsSkinned( objl) )
    {
        obje->p_SKN_Objectponderation = GEO_SKN_CreateObjPonderation( obje, objl->p_SKN_Objectponderation->NumberPdrtLists, obje->l_NbPoints );
        GEO_SKN_AdaptToAnotherSkin( NULL, NULL,obje, objl, 0, NULL, NULL);
    }

    return 2;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG WOR_l_MergeGro(GRO_tdst_Struct *pst_Local, GRO_tdst_Struct *pst_Extern)
{
    /* Check for data */
    if(!pst_Local) return 0;
    if(!pst_Extern) return 0;

    /* Check for identical type */
    if(pst_Local->i->ul_Type != pst_Extern->i->ul_Type)
        return 0;

    /* Merge data */
    switch(pst_Local->i->ul_Type)
    {
    case GRO_Geometric:
        return WOR_l_MergeGeometricGro(pst_Local, pst_Extern);

    case GRO_Light:
    case GRO_Camera:
        if(WOR_gl_ImportGro & WOR_C_IFU_GroParams)
            return 2;
        else
            return 1;

    case GRO_GeoStaticLOD:
        return 0;
    }

    return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG WOR_l_World_ImportObjects(WOR_tdst_World *_pst_World, MAD_World *_pst_MadWorld, char *_sz_Path)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_NodeID              **ppst_Node, **ppst_LastNode;
    GRO_tdst_Struct         *pst_Object, *pst_LocalObject;
    char                    sz_Name[BIG_C_MaxLenPath];
    char                    sz_Path[BIG_C_MaxLenPath];
    char                    sz_Temp[BIG_C_MaxLenPath];
    BIG_INDEX               ul_Index, ul_Index2, ul_BankDirIndex, ul_MyDirIndex;
    WOR_tdst_Import_Info    *pst_Info;
    TAB_tdst_Ptable         *pst_Table;
	ULONG					*pst_CurrentElem, *pst_EndElem;
    ULONG                   ul_OldAddress;
    LONG                    l_Merge;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_pst_MadWorld->NumberOfObjects <= 0) return 0;

    pst_Table = &_pst_World->st_GraphicObjectsTable;
    ppst_Node = _pst_MadWorld->AllObjects;
    ppst_LastNode = ppst_Node + _pst_MadWorld->NumberOfObjects;

    WOR_gpst_ObjInfo = (WOR_tdst_Import_Info *) L_malloc(sizeof(WOR_tdst_Import_Info) * _pst_MadWorld->NumberOfObjects);

    /* Init directory data */
    L_strcpy(sz_Path, _sz_Path);
    L_strcat(sz_Path, "/");
    L_strcat(sz_Path, EDI_Csz_Path_GraphicObject);
    ul_MyDirIndex = BIG_ul_CreateDir(sz_Path);
    ul_BankDirIndex = BIG_ul_CreateDir(EDI_Csz_Path_Objects);

    for(pst_Info = WOR_gpst_ObjInfo; ppst_Node < ppst_LastNode; ppst_Node++, pst_Info++)
    {
        L_strcpy(sz_Name, (*ppst_Node)->Name);
        if((*ppst_Node)->IDType == ID_MAD_Light)
            L_strcat(sz_Name, EDI_Csz_ExtGraphicLight);
        else
            L_strcat(sz_Name, EDI_Csz_ExtGraphicObject);
        ul_Index = BIG_ul_SearchFileInDirRec(ul_BankDirIndex, sz_Name);

        pst_Info->l_TableIndex = -1;
        pst_Info->l_Flags = 0;
        pst_Info->ul_Bank = BIG_C_InvalidIndex;
        L_strcpy(pst_Info->sz_Name, sz_Name);

        if(ul_Index != BIG_C_InvalidIndex)
        {
            pst_Info->ul_Bank = BIG_ParentFile(ul_Index);

            if(pst_Info->ul_Bank == ul_MyDirIndex)
            {
                pst_Info->l_Flags |= WOR_C_OIF_In;
                if((WOR_gl_ImportGro & WOR_C_IFU_GroMask) != WOR_C_IFU_GroNever)
                    pst_Info->l_Flags |= WOR_C_OIF_Update;
            }
            else
            {
                pst_Info->l_Flags |= WOR_C_OIF_Out;
                if((WOR_gl_ImportGro & WOR_C_IFU_GroMask) == WOR_C_IFU_GroAlways)
                    pst_Info->l_Flags |= WOR_C_OIF_Update;
            }
        }

        ul_Index2 = BIG_ul_SearchFile(ul_MyDirIndex, sz_Name);
		if (ul_Index2 == BIG_C_InvalidIndex )
		{
			pst_CurrentElem = (ULONG *) TAB_ppv_Ptable_GetFirstElem( pst_Table );
			pst_EndElem = (ULONG *) TAB_ppv_Ptable_GetLastElem( pst_Table );
			strcpy( sz_Temp, sz_Name );
			if (strrchr( sz_Temp, '.' ))
				*strrchr( sz_Temp, '.' ) = 0;
			for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
			{
				if(TAB_b_IsAHole((void *) *pst_CurrentElem)) continue;
				pst_Object = (GRO_tdst_Struct *) *pst_CurrentElem;
				if( pst_Object->sz_Name && !L_strcmp( pst_Object->sz_Name, sz_Temp ) )
				{
					ul_Index2 = LOA_ul_SearchIndexWithAddress( (ULONG ) pst_Object );
					break;
				}
			}
		}

        if(pst_Info->ul_Bank == BIG_C_InvalidIndex)
            pst_Info->ul_Bank = ul_MyDirIndex;

        if(ul_Index2 != BIG_C_InvalidIndex)
        {
            if((WOR_gl_ImportGro & WOR_C_IFU_GroMask) != WOR_C_IFU_GrmNever)
                pst_Info->l_Flags |= WOR_C_OIF_In | WOR_C_OIF_Update;
        }
        else
        {
            if(!(pst_Info->l_Flags & WOR_C_OIF_Out))
                pst_Info->l_Flags |= WOR_C_OIF_In | WOR_C_OIF_Create;
        }

        ul_OldAddress = BIG_C_InvalidIndex;
        if(ul_Index == BIG_C_InvalidIndex) ul_Index = ul_Index2;
        if (ul_Index != BIG_C_InvalidIndex)
        {
            /* Get old adress */
            ul_OldAddress = LOA_ul_SearchAddress(BIG_PosFile(ul_Index));
        }


        /* Treat object now */
        if((pst_Info->l_Flags & (WOR_C_OIF_Update | WOR_C_OIF_Create)) == 0)
        {
            pst_Object = GEO_pst_Object_Load(ul_Index, _pst_World);
        }
        else
        {
            pst_Object = GRO_pst_ImportFromMad(*ppst_Node, _pst_World, GRO_C_Import_MadObject);
            if(pst_Object->i->ul_Type == GRO_Waypoint)
            {
                pst_Info->l_Flags |= WOR_C_OIF_Treated;
                continue;
            }
            else
            {
                l_Merge = 0;
                if(pst_Info->l_Flags & WOR_C_OIF_Update)
                {
                    if(WOR_gl_ImportGro & WOR_C_IFU_GroMerge)
                    {
                        pst_LocalObject = GEO_pst_Object_Load(ul_Index, _pst_World);
                        l_Merge = WOR_l_MergeGro(pst_LocalObject, pst_Object);
                        if((l_Merge == 0) || (l_Merge == 2))
                            pst_LocalObject->i->pfn_Destroy(pst_LocalObject);
                        else if(l_Merge == 1)
                        {
                            pst_Object->i->pfn_Destroy(pst_Object);
                            pst_Object = pst_LocalObject;
                        }
                    }
                }

                if(l_Merge != 1)
                {
                    BIG_ComputeFullName(pst_Info->ul_Bank, sz_Temp);
                    SAV_Begin(sz_Temp, sz_Name);
                    pst_Object->i->pfnl_SaveInBuffer(pst_Object, NULL);
                    ul_Index = SAV_ul_End();

                    /* Watch if key wasn't already assigned to an object */
                    if(ul_OldAddress != BIG_C_InvalidIndex) WOR_Universe_Update_GroPointerChange(pst_Object, (void *) ul_OldAddress);

                    LOA_AddAddress(ul_Index, pst_Object);
                }
            }
        }

        /* Add object in table */
        pst_Info->l_TableIndex = TAB_ul_Ptable_GetElemIndexWithPointer(pst_Table, pst_Object);
        if(pst_Info->l_TableIndex == TAB_Cul_BadIndex)
        {
            TAB_Ptable_AddElemAndResize(pst_Table, pst_Object);
            pst_Object->i->pfn_AddRef(pst_Object, 1);
            pst_Info->l_TableIndex = pst_Table->ul_NbElems - 1;
        }
    }

    /* Destroy created directory if empty */
    if((BIG_SubDir(ul_MyDirIndex) == BIG_C_InvalidIndex) && (BIG_FirstFile(ul_MyDirIndex) == BIG_C_InvalidIndex))
        BIG_DelDir(sz_Path);

    /* Display now information */
    WOR_World_DisplayInfo(_pst_World, _pst_MadWorld, WOR_C_IT_Object);

    return 0;
}

/*$5
 #######################################################################################################################
    Materials functions
 #######################################################################################################################
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG WOR_l_World_ImportMaterials(WOR_tdst_World *_pst_World, MAD_World *_pst_MadWorld, char *_sz_Path)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_MAT_MatID           **ppst_MadMaterial, **ppst_LastMadMaterial;
    MAT_tdst_Multi          **ppst_MM, **ppst_LastMM;
    LONG                    lIndex, lIndex2, lTableIndex;
    GRO_tdst_Struct         *pst_Material;
    char                    sz_Name[BIG_C_MaxLenPath];
    char                    sz_Path[BIG_C_MaxLenPath];
    char                    sz_Temp[BIG_C_MaxLenPath];
    BIG_INDEX               ul_Index, ul_Index2, ul_BankDirIndex, ul_MyDirIndex;
    TAB_tdst_Ptable         *pst_Table;
	ULONG					*pst_CurrentElem, *pst_EndElem;
    WOR_tdst_Import_Info    *pst_Info;
    ULONG                   ul_OldAddress;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_pst_MadWorld->NumberOfMaterials <= 0)
        return 0;

    pst_Table = &_pst_World->st_GraphicMaterialsTable;
    ppst_MadMaterial = _pst_MadWorld->AllMaterial;
    ppst_LastMadMaterial = ppst_MadMaterial + _pst_MadWorld->NumberOfMaterials;

    lIndex = sizeof(WOR_tdst_Import_Info) * _pst_MadWorld->NumberOfMaterials; 
    WOR_gpst_MatInfo = (WOR_tdst_Import_Info *) L_malloc(lIndex);

    /* Init directory data */
    L_strcpy(sz_Path, _sz_Path);
    L_strcat(sz_Path, "/");
    L_strcat(sz_Path, EDI_Csz_Path_GraphicMaterial);
    ul_MyDirIndex = BIG_ul_CreateDir(sz_Path);
    ul_BankDirIndex = BIG_ul_SearchDir(EDI_Csz_Path_Objects);

    for(pst_Info = WOR_gpst_MatInfo; ppst_MadMaterial < ppst_LastMadMaterial; ppst_MadMaterial++, pst_Info++)
    {
        sprintf(sz_Name, "%s%s", (*ppst_MadMaterial)->Name, EDI_Csz_ExtGraphicMaterial);
        ul_Index = BIG_ul_SearchFileInDirRec(ul_BankDirIndex, sz_Name);

        pst_Info->l_TableIndex = -1;
        pst_Info->l_Flags = 0;
        pst_Info->ul_Bank = BIG_C_InvalidIndex;
        L_strcpy(pst_Info->sz_Name, sz_Name);

        if(ul_Index != BIG_C_InvalidIndex)
        {
            pst_Info->ul_Bank = BIG_ParentFile(ul_Index);

            if(pst_Info->ul_Bank == ul_MyDirIndex)
            {
                pst_Info->l_Flags |= WOR_C_OIF_In;
                if((WOR_gl_ImportGrm & WOR_C_IFU_GrmMask) != WOR_C_IFU_GrmNever)
                    pst_Info->l_Flags |= WOR_C_OIF_Update;
            }
            else
            {
                pst_Info->l_Flags |= WOR_C_OIF_Out;
                if((WOR_gl_ImportGrm & WOR_C_IFU_GrmMask) == WOR_C_IFU_GrmAlways)
                    pst_Info->l_Flags |= WOR_C_OIF_Update;
            }
        }

        ul_Index2 = BIG_ul_SearchFile(ul_MyDirIndex, sz_Name);
		if (ul_Index2 == BIG_C_InvalidIndex )
		{
			pst_CurrentElem = (ULONG *) TAB_ppv_Ptable_GetFirstElem( pst_Table );
			pst_EndElem = (ULONG *) TAB_ppv_Ptable_GetLastElem( pst_Table );
			strcpy( sz_Temp, sz_Name );
			if ( strrchr( sz_Temp, '.' ) )*strrchr( sz_Temp, '.' ) = 0;
			for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
			{
				if(TAB_b_IsAHole((void *) *pst_CurrentElem)) continue;
				pst_Material = (GRO_tdst_Struct *) *pst_CurrentElem;
				if( pst_Material->sz_Name && !L_strcmp( pst_Material->sz_Name, sz_Temp ) )
				{
					ul_Index2 = LOA_ul_SearchIndexWithAddress( (ULONG ) pst_Material );
					break;
				}
			}
		}

        if(pst_Info->ul_Bank == BIG_C_InvalidIndex)
            pst_Info->ul_Bank = ul_MyDirIndex;

        if(ul_Index2 != BIG_C_InvalidIndex)
        {
            if((WOR_gl_ImportGrm & WOR_C_IFU_GrmMask) != WOR_C_IFU_GrmNever)
                pst_Info->l_Flags |= WOR_C_OIF_In | WOR_C_OIF_Update;
        }
        else
        {
            if(!(pst_Info->l_Flags & WOR_C_OIF_Out))
                pst_Info->l_Flags |= WOR_C_OIF_In | WOR_C_OIF_Create;
        }

        ul_OldAddress = BIG_C_InvalidIndex;
        if(ul_Index == BIG_C_InvalidIndex) ul_Index = ul_Index2;
        if(ul_Index != BIG_C_InvalidIndex)
        {
            ul_OldAddress = LOA_ul_SearchAddress(BIG_PosFile(ul_Index));
            if ( ul_OldAddress != BIG_C_InvalidIndex )
                pst_Info->l_Flags |= WOR_C_OIF_AlreadyLoaded;
        }

        /* Treat object now */
        if((pst_Info->l_Flags & (WOR_C_OIF_Update | WOR_C_OIF_Create)) == 0)
        {
            pst_Material = GEO_pst_Object_Load(ul_Index, _pst_World);
        }
        else
        {
            pst_Material = GRO_pst_ImportFromMad
                (
                    (MAD_NodeID *) * ppst_MadMaterial,
                    _pst_World,
                    GRO_C_Import_MadMaterial
                );

            if ( (pst_Material->i->ul_Type != GRO_MaterialMulti) && (pst_Info->l_Flags & WOR_C_OIF_Update) && (WOR_gl_ImportGrm & WOR_C_IFU_GrmNoSingle) )
            {
                pst_Material->i->pfn_Destroy(pst_Material);
                pst_Material = GEO_pst_Object_Load(ul_Index, _pst_World);
                pst_Info->l_Flags &= ~(WOR_C_OIF_Update);
            }
            else
            {
                if (pst_Material->i->ul_Type != GRO_MaterialMulti)
                {
                    BIG_ComputeFullName(pst_Info->ul_Bank, sz_Temp);
                    SAV_Begin(sz_Temp, sz_Name);
                    pst_Material->i->pfnl_SaveInBuffer(pst_Material, &TEX_gst_GlobalList);
                    ul_Index = SAV_ul_End();

                    LOA_AddAddress(ul_Index, pst_Material);
                }

                if(ul_OldAddress != BIG_C_InvalidIndex) 
                {
                    WOR_Universe_Update_GroPointerChange(pst_Material, (void *) ul_OldAddress);
                    pst_Info->l_Flags |= WOR_C_OIF_AddedInTable;
                }
            }
        }

        /* Add object in table */
        pst_Info->l_TableIndex = TAB_ul_Ptable_GetElemIndexWithPointer(pst_Table, pst_Material);
        if(pst_Info->l_TableIndex == TAB_Cul_BadIndex)
        {
            pst_Info->l_Flags |= WOR_C_OIF_AddedInTable;
            TAB_Ptable_AddElemAndResize(pst_Table, pst_Material);
            pst_Material->i->pfn_AddRef(pst_Material, 1);
            pst_Info->l_TableIndex = pst_Table->ul_NbElems - 1;
        }
    }

    /* second pass : save new material and update multi material */
    ppst_MM = (MAT_tdst_Multi **) TAB_ppv_Ptable_GetFirstElem( pst_Table );
    ppst_LastMM = (MAT_tdst_Multi **) TAB_ppv_Ptable_GetLastElem( pst_Table );
    for( lTableIndex = 0; ppst_MM <= ppst_LastMM; ppst_MM++, lTableIndex++)
    {
        pst_Info = WOR_gpst_MatInfo;
        for ( lIndex = 0; lIndex < (LONG) _pst_MadWorld->NumberOfMaterials; lIndex++, pst_Info++)
        {
            if (pst_Info->l_TableIndex == lTableIndex)
                break;
        }
        if (lIndex == (LONG) _pst_MadWorld->NumberOfMaterials) continue;
        if (!(pst_Info->l_Flags & WOR_C_OIF_AddedInTable)) continue;

        if ((*ppst_MM)->st_Id.i->ul_Type == GRO_MaterialMulti)
        {
            if((pst_Info->l_Flags & (WOR_C_OIF_Update | WOR_C_OIF_Create)) == 0)
            {
                if (pst_Info->l_Flags & WOR_C_OIF_AlreadyLoaded) continue;
                for(lIndex = 0; lIndex < (*ppst_MM)->l_NumberOfSubMaterials; lIndex++)
                {
                    if ( !(*ppst_MM)->dpst_SubMaterial[lIndex]) continue;
                    (*ppst_MM)->dpst_SubMaterial[lIndex]->st_Id.i->pfn_AddRef((*ppst_MM)->dpst_SubMaterial[lIndex], 1);
                }
            }
            else
            {
                for(lIndex = 0; lIndex < (*ppst_MM)->l_NumberOfSubMaterials; lIndex++)
                {
                    lIndex2 = (LONG) (*ppst_MM)->dpst_SubMaterial[lIndex];
                    if (lIndex2 == BIG_C_InvalidIndex)
                    {
                        (*ppst_MM)->dpst_SubMaterial[lIndex] = NULL;
                    }
                    else
                    {
                        lIndex2 = WOR_gpst_MatInfo[lIndex2].l_TableIndex;
#ifdef JADEFUSION
						(*ppst_MM)->dpst_SubMaterial[lIndex] = (MAT_tdst_Material_*)pst_Table->p_Table[lIndex2];
#else
						(*ppst_MM)->dpst_SubMaterial[lIndex] = pst_Table->p_Table[lIndex2];
#endif
						(*ppst_MM)->dpst_SubMaterial[lIndex]->st_Id.i->pfn_AddRef((*ppst_MM)->dpst_SubMaterial[lIndex], 1);
                    }
                }

                BIG_ComputeFullName(pst_Info->ul_Bank, sz_Temp);
                SAV_Begin(sz_Temp, pst_Info->sz_Name);
                (*ppst_MM)->st_Id.i->pfnl_SaveInBuffer((*ppst_MM), &TEX_gst_GlobalList);
                ul_Index = SAV_ul_End();

                LOA_AddAddress(ul_Index, (*ppst_MM));
            }
            MAT_Validate_Multi( *ppst_MM );
        }
    }

    /* Destroy created directory if empty */
    if((BIG_SubDir(ul_MyDirIndex) == BIG_C_InvalidIndex) && (BIG_FirstFile(ul_MyDirIndex) == BIG_C_InvalidIndex))
        BIG_DelDir(sz_Path);

    /* Display now information */
    WOR_World_DisplayInfo(_pst_World, _pst_MadWorld, WOR_C_IT_Material);

    return 0;
}

/*$5
 #######################################################################################################################
    Gameobjects functions
 #######################################################################################################################
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_l_World_TransformMadNodeData
(
    WOR_tdst_World      *_pst_World,
    MAD_WorldNode       *_pst_MadNode,
    GRO_tdst_Struct     **_ppst_Obj,
    GRO_tdst_Struct     **_ppst_Mat,
    MATH_tdst_Matrix    *_pst_Matrix
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG                i;
    MATH_tdst_Vector    st_Axis;
    GEO_tdst_StaticLOD  *pst_LOD;
    char                sz_Text[100];
    ULONG               ul_Index;
    TAB_tdst_Ptable     *pst_Table;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_pst_MadNode->Object != MAD_NULL_INDEX)
    {
        i = WOR_gpst_ObjInfo[_pst_MadNode->Object].l_TableIndex;
        if(i == -1)
        {
            *_ppst_Obj = &WAY_gst_GroStruct;
        }
        else
        {
            *_ppst_Obj = (GRO_tdst_Struct*)_pst_World->st_GraphicObjectsTable.p_Table[i];
            if((_pst_MadNode->ObjectWithRadiosity != MAD_NULL_INDEX) && (_pst_MadNode->ObjectWithRadiosity != 0))
            {
                pst_LOD = GEO_pst_StaticLOD_Create(2);
                pst_LOD->auc_EndDistance[0] = 20;
                pst_LOD->auc_EndDistance[1] = 1;
                pst_LOD->dpst_Id[1] = *_ppst_Obj;
                (*_ppst_Obj)->i->pfn_AddRef( (*_ppst_Obj), 1 );
                i = WOR_gpst_ObjInfo[_pst_MadNode->ObjectWithRadiosity].l_TableIndex;
                *_ppst_Obj = (GRO_tdst_Struct*)_pst_World->st_GraphicObjectsTable.p_Table[i];
                pst_LOD->dpst_Id[0] = *_ppst_Obj;
                (*_ppst_Obj)->i->pfn_AddRef( (*_ppst_Obj), 1 );
                sprintf(sz_Text, "%s_LOD", _pst_MadNode->ID.Name);
                GRO_Struct_SetName(&pst_LOD->st_Id, sz_Text);

                L_strcat(sz_Text, EDI_Csz_ExtGraphicObject);
                BIG_ul_CreateDir(WOR_gsz_GroDir);
                SAV_Begin(WOR_gsz_GroDir, sz_Text);
                pst_LOD->st_Id.i->pfnl_SaveInBuffer(pst_LOD, NULL);
                ul_Index = SAV_ul_End();

                LOA_AddAddress(ul_Index, pst_LOD);

                pst_Table = &_pst_World->st_GraphicObjectsTable;
                TAB_Ptable_AddElemAndResize(pst_Table, pst_LOD);
                pst_LOD->st_Id.i->pfn_AddRef(pst_LOD, 1);
                *_ppst_Obj = &pst_LOD->st_Id;
            }
        }
    }
    else
        *_ppst_Obj = NULL;

    /* Geometric material */
    if(_pst_MadNode->Material != MAD_NULL_INDEX)
    {
        i = WOR_gpst_MatInfo[_pst_MadNode->Material].l_TableIndex;
        *_ppst_Mat = (GRO_tdst_Struct*)_pst_World->st_GraphicMaterialsTable.p_Table[i];
    }
    else
        *_ppst_Mat = NULL;

    /* Matrix */

    /*
     * Compute new matrix in case of camera object Cause max camera look at -z axis,
     * jade camera look at -y axis
     */
    if((*_ppst_Obj) && (((*_ppst_Obj)->i->ul_Type == GRO_Camera) || ((*_ppst_Obj)->i->ul_Type == GRO_Light)))
    {
        MATH_NormalizeVector(&st_Axis, (MATH_tdst_Vector *) &_pst_MadNode->Matrix.J);
        MATH_NormalizeVector
        (
            (MATH_tdst_Vector *) &_pst_MadNode->Matrix.J,
            (MATH_tdst_Vector *) &_pst_MadNode->Matrix.K
        );
        MATH_CopyVector((MATH_tdst_Vector *) &_pst_MadNode->Matrix.K, &st_Axis);
        MATH_NegEqualVector((MATH_tdst_Vector *) &_pst_MadNode->Matrix.I);
        MATH_NormalizeVector
        (
            (MATH_tdst_Vector *) &_pst_MadNode->Matrix.I,
            (MATH_tdst_Vector *) &_pst_MadNode->Matrix.I
        );
    }

    MATH_Set33Matrix
    (
        _pst_Matrix,
        (MATH_tdst_Vector *) &_pst_MadNode->Matrix.I,
        (MATH_tdst_Vector *) &_pst_MadNode->Matrix.J,
        (MATH_tdst_Vector *) &_pst_MadNode->Matrix.K
    );
    MATH_SetTranslation(_pst_Matrix, (MATH_tdst_Vector *) &_pst_MadNode->Matrix.Translation);

    MATH_GetScaleFrom33Matrix(&st_Axis, _pst_Matrix);
    MATH_SetScale(_pst_Matrix, &st_Axis);
    if(MATH_b_TestScaleType(_pst_Matrix))
    {
        MATH_InvEqualVector(&st_Axis);
        MATH_Scale33Matrix(_pst_Matrix, _pst_Matrix, &st_Axis);
    }

    MATH_SetCorrectType(_pst_Matrix);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG WOR_l_World_ImportGameObjects(WOR_tdst_World *_pst_World, MAD_World *_pst_MadWorld, char *_sz_Path)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject			*pst_GO, **dpst_GO, *pst_Father, *pst_GizmoGO;
	OBJ_tdst_AdditionalMatrix	*pst_AddMat;
    MAD_WorldNode				*pst_MadNode;
    LONG						lIndex, lIndex2;
    GRO_tdst_Struct				*pst_Obj, *pst_Mat, *pst_Gro, *pst_RLIGeo;
    BIG_INDEX					ul_GaoDirIndex, ul_GaoIndex;
    char						sz_Path[BIG_C_MaxLenPath];
    MATH_tdst_Matrix			st_Matrix;
    ULONG						ul_Type, ul_GaoFlags, ul_GaoOldFlags, ul_Number;
    ULONG						*pul_OldRLI, *pul_NewRLI;
    MAD_NodeID					*obj;
	TAB_tdst_PFelem				*pst_Elem;
	TAB_tdst_PFelem				*pst_LastElem;
	GEO_tdst_Object				*pst_GeoObj;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_MadNode = &_pst_MadWorld->Hierarchie[0];

    L_strcpy(sz_Path, _sz_Path);
    L_strcat(sz_Path, "/");

    L_strcpy(WOR_gsz_GroDir, sz_Path);
    L_strcat(WOR_gsz_GroDir, EDI_Csz_Path_GraphicObject);

    L_strcat(sz_Path, EDI_Csz_Path_GameObject);
    ul_GaoDirIndex = BIG_ul_SearchDir(sz_Path);
    ul_GaoIndex = BIG_C_InvalidIndex;

    dpst_GO = (OBJ_tdst_GameObject **) L_malloc( 4 * _pst_MadWorld->NumberOfHierarchieNodes);

    for(lIndex = 1; lIndex < (LONG) _pst_MadWorld->NumberOfHierarchieNodes; lIndex++)
    {
        pst_MadNode = &_pst_MadWorld->Hierarchie[lIndex];
        pst_GO = NULL;

        /* First get data that is to be imported */
        WOR_l_World_TransformMadNodeData(_pst_World, pst_MadNode, &pst_Obj, &pst_Mat, &st_Matrix);
        if(pst_Obj == NULL)
            ul_Type = 0;
        else if(pst_Obj->i->ul_Type == GRO_Waypoint)
            ul_Type = GRO_Waypoint;
        else if(pst_Obj->i->ul_Type == GRO_Light)
            ul_Type = GRO_Light;
        else
            ul_Type = 0;

        /* Test if gao already exists or not */
        if(ul_GaoDirIndex != BIG_C_InvalidIndex)
        {
            OBJ_GameObject_BuildName(pst_MadNode->ID.Name, sz_Path);
            ul_GaoIndex = BIG_ul_SearchFile(ul_GaoDirIndex, sz_Path);

			/* not found into current level directory, search through object of world */
			if (ul_GaoIndex == BIG_C_InvalidIndex )
			{
				pst_Elem = _pst_World->st_AllWorldObjects.p_Table;
				pst_LastElem = _pst_World->st_AllWorldObjects.p_NextElem;
				for(; pst_Elem < pst_LastElem; pst_Elem++)
				{
					pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
					if(TAB_b_IsAHole( pst_GO )) continue;
					if (pst_GO->sz_Name && !L_strcmp( pst_GO->sz_Name, sz_Path ) )
					{
						ul_GaoIndex = LOA_ul_SearchIndexWithAddress( (ULONG) pst_GO );
						break;
					}
				}
			}
			pst_GO = NULL;
        }

        if(ul_GaoIndex != BIG_C_InvalidIndex)
        {
            pst_GO = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(ul_GaoIndex));
            if(((ULONG) pst_GO == -1) || ((ULONG) pst_GO == 0))
            {
                LOA_MakeFileRef
                (
                    BIG_FileKey(ul_GaoIndex),
                    (ULONG *) &pst_GO,
                    OBJ_ul_GameObjectCallback,
                    LOA_C_MustExists
                );
                LOA_Resolve();
            
                if(pst_GO != NULL)
                {
                    ul_GaoFlags = ul_GaoOldFlags = OBJ_ul_FlagsIdentityGet(pst_GO);

                    if(ul_Type == GRO_Waypoint)
                    {
                        if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
                            ul_GaoFlags = ul_GaoOldFlags - OBJ_C_IdentityFlag_Visu;
                    }
                    else if(ul_Type == GRO_Light)
                    {
                        if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Lights))
                            ul_GaoFlags = ul_GaoOldFlags | OBJ_C_IdentityFlag_Lights;
                    }
                    else
                    {
                        if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
                            ul_GaoFlags = ul_GaoOldFlags | OBJ_C_IdentityFlag_Visu;
                    }

                    if(ul_GaoFlags != ul_GaoOldFlags) OBJ_ChangeIdentityFlags(pst_GO, ul_GaoFlags, ul_GaoOldFlags);

                    WOR_World_JustAfterLoadObject(_pst_World, pst_GO, TRUE, TRUE);
                }
            }
        }

        if(pst_GO == NULL)
        {
            /* Create node */
            if(ul_Type == GRO_Waypoint)
                pst_GO = OBJ_GameObject_Create(C_IF_Waypoint);
            else if(ul_Type == GRO_Light)
                pst_GO = OBJ_GameObject_Create(C_IF_Light);
            else if (pst_Obj)
                pst_GO = OBJ_GameObject_Create(C_IF_Object);
            else
                pst_GO = OBJ_GameObject_Create(C_IF_Dummy );

            /*
             * We set the RayInsensitive Flag when the object is a waypoint, a light, a camera
             * or an invisible object.
             */
            if
            (
                (ul_Type == GRO_Waypoint) ||
                (ul_Type == GRO_Camera) ||
                (ul_Type == GRO_Light) ||
                (ul_Type == GRO_Waypoint) ||
                (
                    !(
                        OBJ_b_TestIdentityFlag
                        (
                            pst_GO,
                            (
                                OBJ_C_IdentityFlag_Visu | OBJ_C_IdentityFlag_Anims |
                                    OBJ_C_IdentityFlag_Waypoints | OBJ_C_IdentityFlag_Links |
                                        OBJ_C_IdentityFlag_Lights
                            )
                        )
                    )
                )
            ) pst_GO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_RayInsensitive;

            OBJ_GameObject_SetName(pst_GO, pst_MadNode->ID.Name);
        }

        /* Set imported data */
        if(ul_Type == GRO_Light)
        {
            pst_Gro = pst_GO->pst_Extended->pst_Light;
            if(pst_Gro) pst_Gro->i->pfn_AddRef(pst_Gro, -1);
            pst_GO->pst_Extended->pst_Light = pst_Obj;
            if(pst_Obj) pst_Obj->i->pfn_AddRef(pst_Obj, 1);
        }
        else if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
        {
            /* geometry */
            pst_Gro = pst_RLIGeo = pst_GO->pst_Base->pst_Visu->pst_Object;
            if(pst_Gro) pst_Gro->i->pfn_AddRef(pst_Gro, -1);
            pst_GO->pst_Base->pst_Visu->pst_Object = pst_Obj;
            if(pst_Obj) pst_Obj->i->pfn_AddRef(pst_Obj, 1);

            /* material */
            pst_Gro = pst_GO->pst_Base->pst_Visu->pst_Material;
            if(pst_Gro) pst_Gro->i->pfn_AddRef(pst_Gro, -1);
            pst_GO->pst_Base->pst_Visu->pst_Material = pst_Mat;
            if(pst_Mat) pst_Mat->i->pfn_AddRef(pst_Mat, 1);

            /* RLI */
            if ( pst_Obj && pst_RLIGeo )
            {
                obj = _pst_MadWorld->AllObjects[pst_MadNode->Object];
                ul_Number = 0;
                if (obj->IDType == ID_MAD_GeometricObject_V0)
                    ul_Number = ((MAD_GeometricObject *) obj)->NumberOfPoints;

                pul_OldRLI = pst_GO->pst_Base->pst_Visu->dul_VertexColors;
            
                if (!ul_Number)
                {
					OBJ_VertexColor_Free( pst_GO );
                }
                else
                {
                    if ( pul_OldRLI )
                    {
                        if (pst_Obj != pst_RLIGeo)
                        {
                            pul_NewRLI = NULL;
                            GEO_PickRLI( (GEO_tdst_Object *) pst_Obj, &pul_NewRLI, (GEO_tdst_Object *) pst_RLIGeo, pul_OldRLI, 0 );
                            OBJ_VertexColor_ChangePointer( pst_GO, pul_NewRLI, 1 );
                        }
                    }
                    else if(pst_MadNode->RLIOfObject)
                    {
                        pul_NewRLI = (ULONG*)MEM_p_Alloc(4 * (ul_Number + 1));
                        pst_GO->pst_Base->pst_Visu->dul_VertexColors = pul_NewRLI;
                        pul_NewRLI[0] = ul_Number;
                        L_memcpy( pul_NewRLI + 1, pst_MadNode->RLIOfObject, ul_Number * 4 );
                    }
                }
            }
        }


        dpst_GO[ lIndex ] = pst_GO;
        OBJ_SetAbsoluteMatrix(pst_GO, &st_Matrix);
        OBJ_SetInitialAbsoluteMatrix(pst_GO, &st_Matrix);

        /* Add node to world */
        WOR_World_AddLoadedObject(_pst_World, pst_GO, 0);

        /* Display information about new GameObject */
        {
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            char                    sz_Message[512], *psz_Message;
            WOR_tdst_Import_Info    *pst_Info;
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

            psz_Message = sz_Message + sprintf(sz_Message, "....%s -> ", pst_MadNode->ID.Name);
            if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
            {
                if (pst_Obj)
                {
                    psz_Message += sprintf(psz_Message, "%s ", GRO_sz_Struct_GetName(pst_Obj));

                    pst_Info = &WOR_gpst_ObjInfo[pst_MadNode->Object];
                    if((WOR_gl_ImportAsBank) || (pst_Info->l_Flags & WOR_C_OIF_Out))
                        psz_Message += sprintf(psz_Message, "(%s) ", BIG_NameDir(BIG_ParentDir(pst_Info->ul_Bank)));
                    else
                        psz_Message += sprintf(psz_Message, "(*) ");
                }

                if(pst_Mat)
                {
                    psz_Message += sprintf(psz_Message, "%s ", GRO_sz_Struct_GetName(pst_Mat));
                    pst_Info = &WOR_gpst_MatInfo[pst_MadNode->Material];
                    if((WOR_gl_ImportAsBank) || (pst_Info->l_Flags & WOR_C_OIF_Out))
                        psz_Message += sprintf(psz_Message, "(%s) ", BIG_NameDir(BIG_ParentDir(pst_Info->ul_Bank)));
                    else
                        psz_Message += sprintf(psz_Message, "(*) ");
                }
            }
            else
            {
                psz_Message += sprintf(psz_Message, "no graphic data");
            }

            LINK_PrintStatusMsg(sz_Message);
        }

        /* Look at */
        if(pst_MadNode->NODE_Flags & MNF_IsLookAt)
            OBJ_SetControlFlag(pst_GO, OBJ_C_ControlFlag_LookAt);
    }

    /* second pass for hierarchy */
    for(lIndex = 1; lIndex < (LONG) _pst_MadWorld->NumberOfHierarchieNodes; lIndex++)
    {
		pst_MadNode = &_pst_MadWorld->Hierarchie[lIndex];
        pst_GO = dpst_GO[lIndex];

        if ( pst_MadNode->Parent == MAD_NULL_INDEX) continue;
        if ( pst_MadNode->Parent == 0) continue;
        pst_Father = dpst_GO[pst_MadNode->Parent];
        ul_GaoFlags = pst_GO->ul_IdentityFlags;
        OBJ_ChangeIdentityFlags( pst_GO, ul_GaoFlags | OBJ_C_IdentityFlag_Hierarchy, ul_GaoFlags);
        pst_GO->pst_Base->pst_Hierarchy->pst_Father = pst_Father;
        pst_GO->pst_Base->pst_Hierarchy->pst_FatherInit = pst_Father;

        OBJ_SetInitialAbsoluteMatrix( pst_GO, OBJ_pst_GetAbsoluteMatrix(pst_GO) );
        MATH_CopyMatrix( &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix, OBJ_pst_GetInitialAbsoluteMatrix(pst_GO) );
    }

    /* Here I set the gao_add_mat_ptr */
    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * TODO: Put the right index for each additional matrixes.               *
     *       The principle is easy, when you have to put an index on a new   *
     *       object, you "register" it so when you find it again, you        *
     *       already know his index.                                         *
     * Technics: maybe a map would be necessary, so that each object that is *
     *           implied in a ponderation has its own index.                 *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    for (lIndex = 1; lIndex < (LONG) _pst_MadWorld->NumberOfHierarchieNodes; lIndex++)
    {
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		WOR_tdst_Import_Info    *pst_Info;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_MadNode = &_pst_MadWorld->Hierarchie[lIndex];
		pst_GO = dpst_GO[lIndex];

		pst_Info = &WOR_gpst_ObjInfo[pst_MadNode->Object];

		if ((pst_Info->l_Flags & (WOR_C_OIF_Update | WOR_C_OIF_Create))
			&& OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_BaseObject)
			&& OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)
			&& pst_GO->pst_Base && pst_GO->pst_Base->pst_Visu)
		{
			if (pst_GO->pst_Base->pst_Visu->pst_Object->i->ul_Type != GRO_Geometric) continue;
			pst_GeoObj = (GEO_tdst_Object *)pst_GO->pst_Base->pst_Visu->pst_Object;
			if (GEO_SKN_IsSkinned(pst_GeoObj))
			{
				OBJ_ChangeIdentityFlags(
				pst_GO,
				pst_GO->ul_IdentityFlags
					| OBJ_C_IdentityFlag_AdditionalMatrix
					| OBJ_C_IdentityFlag_AddMatArePointer,
				pst_GO->ul_IdentityFlags);
				pst_AddMat = pst_GO->pst_Base->pst_AddMatrix;
				pst_AddMat->l_Number = pst_GeoObj->p_SKN_Objectponderation->NumberPdrtLists;
				pst_AddMat->dst_GizmoPtr = (OBJ_tdst_GizmoPtr*)MEM_p_Alloc(pst_GO->pst_Base->pst_AddMatrix->l_Number * sizeof (OBJ_tdst_GizmoPtr));
				for (lIndex2 = 0; lIndex2 < pst_AddMat->l_Number; ++lIndex2)
				{
					if (pst_GeoObj->p_SKN_Objectponderation->pp_PdrtLst[lIndex2]->us_IndexOfMatrix + 1 < _pst_MadWorld->NumberOfHierarchieNodes)
					{
						pst_AddMat->dst_GizmoPtr[lIndex2].pst_GO = dpst_GO[pst_GeoObj->p_SKN_Objectponderation->pp_PdrtLst[lIndex2]->us_IndexOfMatrix + 1];
						pst_AddMat->dst_GizmoPtr[lIndex2].pst_Matrix = pst_AddMat->dst_GizmoPtr[lIndex2].pst_GO->pst_GlobalMatrix;
					}
					else
					{
						pst_AddMat->dst_GizmoPtr[lIndex2].pst_GO = NULL;
						pst_AddMat->dst_GizmoPtr[lIndex2].pst_Matrix = NULL;
						continue;
					}
					pst_GeoObj->p_SKN_Objectponderation->pp_PdrtLst[lIndex2]->us_IndexOfMatrix = (unsigned short)lIndex2;
					pst_GizmoGO = pst_AddMat->dst_GizmoPtr[lIndex2].pst_GO;
					if (OBJ_b_TestIdentityFlag(pst_GizmoGO, OBJ_C_IdentityFlag_BaseObject)
						&& !OBJ_b_TestIdentityFlag(pst_GizmoGO, OBJ_C_IdentityFlag_AddMatArePointer))
					{
						OBJ_ChangeIdentityFlags(
							pst_GizmoGO,
							pst_GizmoGO->ul_IdentityFlags
								| OBJ_C_IdentityFlag_AdditionalMatrix
								| OBJ_C_IdentityFlag_AddMatArePointer,
							pst_GizmoGO->ul_IdentityFlags);
						pst_GizmoGO->pst_Base->pst_AddMatrix->l_Number = 1;
						pst_GizmoGO->pst_Base->pst_AddMatrix->dst_GizmoPtr = (OBJ_tdst_GizmoPtr*)MEM_p_Alloc(sizeof (OBJ_tdst_GizmoPtr));
						pst_GizmoGO->pst_Base->pst_AddMatrix->dst_GizmoPtr->l_MatrixId = lIndex2;
						pst_GizmoGO->pst_Base->pst_AddMatrix->dst_GizmoPtr->pst_GO = NULL;
						pst_GizmoGO->pst_Base->pst_AddMatrix->dst_GizmoPtr->pst_Matrix = NULL;
					}
					pst_AddMat->dst_GizmoPtr[lIndex2].l_MatrixId = lIndex2;
				}
			}
		}
    }

    L_free( dpst_GO );
    MAD_FREE();
    return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG WOR_l_World_MergeGameObjects(WOR_tdst_World *_pst_World, char *_sz_Path)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG               *pul_BufferStart, *pul_Buffer;
    ULONG               ul_Size, ul_Index;
    char                sz_Path[BIG_C_MaxLenPath];
    char                sz_Name[BIG_C_MaxLenName];
    BIG_INDEX           ul_Dir, ul_File;
    ULONG               ul_Pos;
    OBJ_tdst_GameObject *pst_GO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Search for the gol (game object list file) */
    L_strcpy(sz_Path, _sz_Path);

    L_strcpy(sz_Name, _pst_World->sz_Name);
    L_strcat(sz_Name, EDI_Csz_ExtGameObjects);

    ul_Dir = BIG_ul_SearchDir(sz_Path);
    if(ul_Dir == BIG_C_InvalidIndex) return 0;
    ul_File = BIG_ul_SearchFile(ul_Dir, sz_Name);
    if(ul_File == BIG_C_InvalidIndex) return 0;

    pul_BufferStart = pul_Buffer = (ULONG *) BIG_pc_ReadFileTmpMustFree(BIG_PosFile(ul_File), &ul_Size);
    ul_Size >>= 3;

    for(ul_Index = 0; ul_Index < ul_Size; ul_Index++, pul_Buffer += 2)
    {
        /* Watch if game object already loaded */
        ul_Pos = BIG_ul_SearchKeyToPos(*pul_Buffer);
        if(ul_Pos == -1) continue;
        pst_GO = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(ul_Pos);
        if((pst_GO != (OBJ_tdst_GameObject *) - 1) && (pst_GO != (OBJ_tdst_GameObject *) 0))
        {
            if(TAB_ul_PFtable_GetElemIndexWithPointer(&_pst_World->st_AllWorldObjects, pst_GO) != TAB_Cul_BadIndex)
                pst_GO = NULL;
            else
                WOR_World_AttachGameObject(_pst_World, pst_GO);
        }
        else
        {
            LOA_MakeFileRef(*pul_Buffer, (ULONG *) &pst_GO, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
            LOA_Resolve();
            WOR_World_JustAfterLoadObject(_pst_World, pst_GO, TRUE, TRUE);
        }
    }

    L_free(pul_BufferStart);

    return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_World_DisplayInfo(WOR_tdst_World *_pst_World, MAD_World *_pst_MadWorld, LONG _l_Type)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    WOR_tdst_Import_Info    *pst_Info, *pst_Last, *pst_InfoRef;
    char                    sz_Temp[260], *psz_Temp;
    char                    *asz_TypeName[3] = { "Object", "Material", "Texture" };
    LONG                    l, lmax, col, currentcol;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    switch(_l_Type)
    {
    case WOR_C_IT_Object:
        pst_Info = WOR_gpst_ObjInfo;
        pst_Last = pst_Info + _pst_MadWorld->NumberOfObjects;
        break;
    case WOR_C_IT_Material:
        pst_Info = WOR_gpst_MatInfo;
        pst_Last = pst_Info + _pst_MadWorld->NumberOfMaterials;
        break;
    case WOR_C_IT_Texture:
        pst_Info = WOR_gpst_TexInfo;
        pst_Last = pst_Info + WOR_gl_NbTexInfos;
        break;
    }

    /* Display now information */
    pst_InfoRef = NULL;

    for(; pst_Info < pst_Last; pst_Info++)
    {
        if(pst_InfoRef == NULL)
        {
            if(!(pst_Info->l_Flags & WOR_C_OIF_Treated))
            {
                pst_InfoRef = pst_Info;
                if(pst_Info->l_Flags & WOR_C_OIF_Out)
                {
                    if(pst_Info->l_Flags & WOR_C_OIF_In)
                    {
                        sprintf
                        (
                            sz_Temp,
                            "....%s found in bank \"%s\" and also in %s \"%s\" (%s)",
                            asz_TypeName[_l_Type],
                            BIG_NameDir(BIG_ParentDir(pst_Info->ul_Bank)),
                            WOR_gl_ImportAsBank ? "bank" : "level",
                            _pst_World->sz_Name,
                            (pst_Info->l_Flags & WOR_C_OIF_Update) ? "update" : "no update"
                        );
                        ERR_X_Warning(0, sz_Temp, NULL);
                    }
                    else
                    {
                        sprintf
                        (
                            sz_Temp,
                            "....%s found in bank \"%s\" (%s)",
                            asz_TypeName[_l_Type],
                            BIG_NameDir(BIG_ParentDir(pst_Info->ul_Bank)),
                            (pst_Info->l_Flags & WOR_C_OIF_Update) ? "update" : "no update"
                        );
                        Out(sz_Temp, 0x00008000);
                    }
                }
                else if(pst_Info->l_Flags & WOR_C_OIF_In)
                {
                    if(WOR_gl_ImportAsBank)
                    {
                        sprintf
                        (
                            sz_Temp,
                            "....%s %s in imported bank \"%s\"",
                            asz_TypeName[_l_Type],
                            (pst_Info->l_Flags & WOR_C_OIF_Update) ? "updated" : "created",
                            _pst_World->sz_Name
                        );
                        Out(sz_Temp, 0x00008000);
                    }
                    else
                    {
                        sprintf
                        (
                            sz_Temp,
                            "....%s %s in imported level \"%s\"",
                            asz_TypeName[_l_Type],
                            (pst_Info->l_Flags & WOR_C_OIF_Update) ? "updated" : "created",
                            _pst_World->sz_Name
                        );
                        ERR_X_Warning(0, sz_Temp, NULL);
                    }
                }
                else if(pst_Info->l_Flags & WOR_C_OIF_TexInLib)
                {
                    sprintf(sz_Temp, "....Textures found in bank %s", BIG_NameDir(pst_Info->ul_Bank));
                    Out(sz_Temp, 0x00008000);
                }
                else if(pst_Info->l_Flags & WOR_C_OIF_TexInTrash)
                {
                    sprintf(sz_Temp, "....Textures not found, created in trashcan");
                    ERR_X_Warning(0, sz_Temp, NULL);
                }
                else if(pst_Info->l_Flags & WOR_C_OIF_TexBadFormat)
                {
                    sprintf(sz_Temp, "....Can't import (Bad format)");
                    ERR_X_Warning(0, sz_Temp, NULL);
                }
                else if(pst_Info->l_Flags & WOR_C_OIF_TexBadSize)
                {
                    sprintf(sz_Temp, "....Can't import (Bad texture size)");
                    ERR_X_Warning(0, sz_Temp, NULL);
                }
            }
            else
                continue;
        }

        lmax = 0;
        for(; pst_Info < pst_Last; pst_Info++)
        {
            if((pst_Info->l_Flags == pst_InfoRef->l_Flags) && (pst_Info->ul_Bank == pst_InfoRef->ul_Bank))
            {
                l = strlen(pst_Info->sz_Name) + 1;
                if(lmax < l) lmax = l;
            }
        }

        col = C_LogWidth / lmax;
        currentcol = 0;

        for(pst_Info = pst_InfoRef; pst_Info < pst_Last; pst_Info++)
        {
            if((pst_Info->l_Flags == pst_InfoRef->l_Flags) && (pst_Info->ul_Bank == pst_InfoRef->ul_Bank))
            {
                if(pst_Info != pst_InfoRef) pst_Info->l_Flags |= WOR_C_OIF_Treated;

                if(currentcol == 0) psz_Temp = sz_Temp + sprintf(sz_Temp, "........");
                psz_Temp += sprintf(psz_Temp, "%s", pst_Info->sz_Name);
                l = strlen(pst_Info->sz_Name);
                while(l < lmax)
                {
                    *psz_Temp++ = ' ';
                    l++;
                }

                *psz_Temp = 0;
                if(currentcol >= col)
                {
                    LINK_PrintStatusMsg(sz_Temp);
                    currentcol = 0;
                }
                else
                    currentcol++;
            }
        }

        if(currentcol != 0) LINK_PrintStatusMsg(sz_Temp);

        pst_InfoRef->l_Flags |= WOR_C_OIF_Treated;
        pst_Info = pst_InfoRef;
        pst_InfoRef = NULL;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL WOR_b_CheckWorldBeforeMerging( BIG_INDEX ul_Dir, int i_Level )
{
    int     i_Res;
    ULONG   ul_Index;

    i_Res = 1;

    /* sub dir */
    ul_Index = BIG_SubDir( ul_Dir );
    while (ul_Index != BIG_C_InvalidIndex)
    {
        i_Res &= WOR_b_CheckWorldBeforeMerging( ul_Index, i_Level+1);
        ul_Index = BIG_NextDir( ul_Index );
    }

    if ( (i_Level == 1) && ( L_stricmp( BIG_NameDir( ul_Dir ), EDI_Csz_Path_GameObject) == 0) )
        return 1;

    /* file */
    ul_Index = BIG_FirstFile( ul_Dir );
    while (ul_Index != BIG_C_InvalidIndex)
    {
        if(BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtGameObject))
        {
            char    *psz_Text, sz_Text[ 512 ];
            
            psz_Text = sz_Text + sprintf( sz_Text, "%s", BIG_NameFile( ul_Index ) );
            psz_Text += sprintf( psz_Text, " is in bad dir : " );
            BIG_ComputeFullName( ul_Dir, psz_Text );
            LINK_PrintStatusMsg( sz_Text );
            i_Res = 0;
        }
        ul_Index = BIG_NextFile( ul_Index );
    }
    return i_Res;
}

#endif /* ACTIVE_EDITORS */
