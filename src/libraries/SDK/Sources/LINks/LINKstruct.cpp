/*$T LINKstruct.cpp GC!1.71 02/12/00 18:34:54 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/MEMory/MEM.h"
#include "BASe/BASarray.h"
#include "BASe/ERRors/ERRasser.h"
#include "LINKs/LINKstruct.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "EDImainframe.h"
#include "EDIapp.h"
#include "BASe/CLIbrary/CLImem.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/* Array to remember engine pointers */
CMapStringToPtr     LINK_gx_TreeToPointers;
CMapStringToPtr     LINK_gx_TreeToNum;
CMapPtrToPtr        LINK_gx_PointersToTree;

/* Array for all structures definition */
#define MAX_STRUCT  200
LINK_tdst_Struct    LINK_gast_StructTypes[MAX_STRUCT];

/* List of pointers that have been deleted */
CMapPtrToPtr        LINK_gx_PointersJustDeleted;

/* List of pointers that have been added */
CMapPtrToPtr        LINK_gx_PointersJustAdded;

/* List of pointers that have been updated */
CMapPtrToPtr        LINK_gx_PointersJustUpdated;

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

extern void LINK_SetDelPointer(void *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

/*$4
 ***********************************************************************************************************************
    Structure types
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    To register a specific engine structure.

    In:     _i_Num              The id of the structure. This id must be define in LINKstruct_reg.h
            _psz_StructName     Name of the structure to register.
            _pfn_CB             Callback when structure is modified.
            ...                 1- Name of the field £
                                2- Type of the field £
                                3- Offset in the structure of the field £
                                4- Flags £
                                5- First param (depending of type) £
                                float: min value 6- Second param (depending of type) £
                                float: max value 7- Third param (depeinding of type) £
                                8- Display group (0 for none)
 =======================================================================================================================
 */
void LINK_RegisterStructType(int _i_Num, char *_psz_StructName, EVAV_tdpfnv_Change _pfn_CB, ...)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    va_list                     marker;
    EVAV_tde_VarsViewItemType   e_Type;
    char                        *psz_Name;
    void                        *p_Pointer;
    int                         i_Param1, i_Param2, i_Param3, i_Param4, i_SubGroup;
    ULONG                       ul_Flags;
    float                       f_Value;
	EVAV_cl_ViewItem			*po_Item;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ERR_X_Assert(_i_Num < MAX_STRUCT);
    va_start(marker, _pfn_CB);

    LINK_gast_StructTypes[_i_Num].i_NumFields = 0;
    LINK_gast_StructTypes[_i_Num].psz_Name = _psz_StructName;
    LINK_gast_StructTypes[_i_Num].pfn_CB = _pfn_CB;
    LINK_gast_StructTypes[_i_Num].po_Fields = new CList < EVAV_cl_ViewItem *, EVAV_cl_ViewItem * > ;
    LINK_gast_StructTypes[_i_Num].i_NameKit = 0;
    LINK_gast_StructTypes[_i_Num].i_MaxNameKit = 0;

    while(1)
    {
        psz_Name = va_arg(marker, char *);
        if((int) psz_Name == -1)
        {
            break;
        }

        e_Type = va_arg(marker, EVAV_tde_VarsViewItemType);
        p_Pointer = va_arg(marker, void *);
        ul_Flags = va_arg(marker, ULONG);
        if(e_Type == EVAV_EVVIT_Float)
        {
            f_Value = (float) va_arg(marker, double);
            i_Param1 = *(int *) &f_Value;
            f_Value = (float) va_arg(marker, double);
            i_Param2 = *(int *) &f_Value;
        }
        else
        {
            i_Param1 = va_arg(marker, int);
            i_Param2 = va_arg(marker, int);
        }

        i_Param3 = va_arg(marker, int);
        i_Param4 = va_arg(marker, int);
        i_SubGroup = va_arg(marker, int);

        LINK_gast_StructTypes[_i_Num].po_Fields->AddTail
            (
                po_Item = new EVAV_cl_ViewItem
                    (
                        psz_Name,
                        e_Type,
                        p_Pointer,
                        ul_Flags,
                        i_Param1,
                        i_Param2,
                        i_Param3,
                        NULL,
                        GetSysColor(COLOR_WINDOW),
                        i_Param4,
                        0,
                        0,
                        0,
                        i_SubGroup
                    )
            );
		po_Item->psz_Help = va_arg(marker, char *);

        LINK_gast_StructTypes[_i_Num].i_NumFields++;
    }

    va_end(marker);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_LoadKitNumbers(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~*/
    int         i;
    BIG_INDEX   ul_Index;
    ULONG       ul_Size;
    char        *pc_Buf;
    /*~~~~~~~~~~~~~~~~~~~~~*/

    LINK_ReadNameKits();
    ul_Index = BIG_ul_SearchFileExt(EDI_Csz_Ini, EDI_Csz_DefaultKitNames);
    if(ul_Index == BIG_C_InvalidIndex) return;

    pc_Buf = BIG_pc_ReadFileTmp(BIG_PosFile(ul_Index), &ul_Size);
    for(i = 0; i < MAX_STRUCT; i++)
    {
        LINK_gast_StructTypes[i].i_NameKit = *(int *) pc_Buf;
        pc_Buf += 4;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_SaveKitNumbers(void)
{
    /*~~~~~~*/
    int i;
    /*~~~~~~*/

    SAV_Begin(EDI_Csz_Ini, EDI_Csz_DefaultKitNames);
    for(i = 0; i < MAX_STRUCT; i++) SAV_Buffer(&LINK_gast_StructTypes[i].i_NameKit, 4);
    SAV_ul_End();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_ReadNameKits(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_Index;
    int         i_Struct;
    char        *pc_Buf, *pc_Beg;
    ULONG       ul_Size;
    int         i_Count;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Reset all */
    for(i_Struct = 0; i_Struct < MAX_STRUCT; i_Struct++)
    {
        L_memset
        (
            LINK_gast_StructTypes[i_Struct].apsz_NameKits,
            0,
            sizeof(LINK_gast_StructTypes[i_Struct].apsz_NameKits)
        );
        LINK_gast_StructTypes[i_Struct].i_MaxNameKit = 0;
    }

    /* Read all */
    ul_Index = BIG_ul_SearchDir(EDI_Csz_Ini_NameKits);
    if(ul_Index == BIG_C_InvalidIndex) return;
    ul_Index = BIG_FirstFile(ul_Index);
    while(ul_Index != BIG_C_InvalidIndex)
    {
        pc_Buf = BIG_pc_ReadFileTmp(BIG_PosFile(ul_Index), &ul_Size);
        pc_Beg = pc_Buf;
        while(*pc_Buf && L_isdigit(*pc_Buf)) pc_Buf++;
        if(!*pc_Buf) break;

        /* Structure number */
        *pc_Buf = 0;
        i_Struct = L_atoi(pc_Beg);
        pc_Buf++;
        while(*pc_Buf && L_isspace(*pc_Buf)) pc_Buf++;
        if(!*pc_Buf) break;
        pc_Beg = pc_Buf;

        LINK_gast_StructTypes[i_Struct].i_MaxNameKit++;
        ERR_X_Assert(LINK_gast_StructTypes[i_Struct].i_MaxNameKit < MAX_NAME_KIT);

        i_Count = 0;
        while(1)
        {
            while(*pc_Buf && (*pc_Buf != '\n')) 
			{
				if(*pc_Buf == '\r') *pc_Buf = ' ';
				pc_Buf++;
			}

            if(!*pc_Buf) break;
            *pc_Buf = 0;

            ERR_X_Assert(i_Count < MAX_KIT_PER_NAME);
            LINK_gast_StructTypes[i_Struct].apsz_NameKits[LINK_gast_StructTypes[i_Struct].i_MaxNameKit - 1][i_Count++] = L_strdup(pc_Beg);

            pc_Buf++;
            while(*pc_Buf && L_isspace(*pc_Buf))
			{
				if(*pc_Buf == '\r') *pc_Buf = ' ';
				pc_Buf++;
			}

            if(!*pc_Buf) break;
            pc_Beg = pc_Buf;
        }

        ul_Index = BIG_NextFile(ul_Index);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_InitRegisterStructType(void)
{
    L_memset(LINK_gast_StructTypes, 0, sizeof(LINK_gast_StructTypes));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_DelRegisterStructType(void)
{
    /*~~~~~~~~~~~~~~~~*/
    int         i;
    POSITION    pos;
    /*~~~~~~~~~~~~~~~~*/

    for(i = 0; i < MAX_STRUCT; i++)
    {
        if(LINK_gast_StructTypes[i].i_NumFields > 0)
        {
            pos = LINK_gast_StructTypes[i].po_Fields->GetHeadPosition();
            while(pos) delete LINK_gast_StructTypes[i].po_Fields->GetNext(pos);

            LINK_gast_StructTypes[i].po_Fields->RemoveAll();
        }

        delete LINK_gast_StructTypes[i].po_Fields;
    }

    LINK_DeleteNameKits();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_DeleteNameKits(void)
{
    /*~~~~~~~~~~~~*/
    int i, j, k;
    /*~~~~~~~~~~~~*/

    for(i = 0; i < MAX_STRUCT; i++)
    {
        for(j = 0; j < LINK_gast_StructTypes[i].i_MaxNameKit; j++)
        {
            for(k = 0; k < 50; k++)
            {
                if(LINK_gast_StructTypes[i].apsz_NameKits[j][k])
                {
                    L_free(LINK_gast_StructTypes[i].apsz_NameKits[j][k]);
                    LINK_gast_StructTypes[i].apsz_NameKits[j][k] = NULL;
                }
            }
        }

        LINK_gast_StructTypes[i].i_MaxNameKit = 0;
    }
}

/*$4
 ***********************************************************************************************************************
    Pointers
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_InitRegisterPointer(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_ResetRegisterPointer(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION            pos;
    BAS_tdst_barray     *ptree;
    CString             key;
    int                 i;
    LINK_tdst_Pointer   *p;
    char                *psz_Tree;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Delete tree to pointers list, and all dynamic arrays */
    pos = LINK_gx_TreeToPointers.GetStartPosition();
    while(pos)
    {
        LINK_gx_TreeToPointers.GetNextAssoc(pos, key, (void * &) ptree);
		if(ptree)
		{
			for(i = 0; i < ptree->num; i++)
			{
				L_free((LINK_tdst_Pointer *) ptree->base[i].ul_Val);
			}

			BAS_bfree(ptree);
			L_free(ptree);
		}
    }

    LINK_gx_TreeToPointers.RemoveAll();

    /* Delete pointers to tree array */
    pos = LINK_gx_PointersToTree.GetStartPosition();
    while(pos)
    {
        LINK_gx_PointersToTree.GetNextAssoc(pos, (void * &) p, (void * &) psz_Tree);
        L_free(psz_Tree);
    }

    LINK_gx_PointersToTree.RemoveAll();
    LINK_gx_TreeToNum.RemoveAll();
}

/*
 =======================================================================================================================
    Aim:    You must call the function to register an engine pointer.

    In:     _p_Pointer  Pointer to register.
            _i_Type     The structure type of the pointer. The structure must have been registered with the
                        LINK_RegisterStructType function.
            _psz_Name   Name of the pointer (to display it).
            _psz_Tree   A tree to organize hierarchy of pointers.
 =======================================================================================================================
 */
void LINK_RegisterPointer(void *_p_Pointer, int _i_Type, char *_psz_Name, char *_psz_Tree)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LINK_tdst_Pointer   *p;
    BAS_tdst_barray     *ptree;
    char                asz_Path[BIG_C_MaxLenPath];
    char                *psz_Temp;
    char                *psz_Adr;
    int                 i_Num;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Compute full path name */
    L_strcpy(asz_Path, _psz_Tree);

    /* We first search if the tree is already registered */
    psz_Temp = asz_Path;
    do
    {
        psz_Temp = L_strchr(psz_Temp, '/');
        if(psz_Temp) *psz_Temp = 0;

        if(!LINK_gx_TreeToPointers.Lookup(asz_Path, (void * &) ptree))
        {
            ptree = (BAS_tdst_barray *) L_malloc(sizeof(BAS_tdst_barray));
            BAS_binit(ptree, 200);
            LINK_gx_TreeToPointers.SetAt(asz_Path, ptree);
            LINK_gx_TreeToNum.SetAt(asz_Path, (void *) 1);
        }
        else
        {
            LINK_gx_TreeToNum.Lookup(asz_Path, (void * &) i_Num);
            LINK_gx_TreeToNum.SetAt(asz_Path, (void *) (i_Num + 1));
        }

        if(psz_Temp)
        {
            *psz_Temp = '/';
            psz_Temp++;
        }
    } while(psz_Temp);

    /* Add the "pointer to path" to retreive it will be deleted */
    psz_Adr = (char *) L_malloc(L_strlen(asz_Path) + 1);
    L_strcpy(psz_Adr, asz_Path);
    LINK_gx_PointersToTree.SetAt(_p_Pointer, psz_Adr);

    /* Init the pointer structure */
    p = (LINK_tdst_Pointer *) L_malloc(sizeof(LINK_tdst_Pointer));
    p->i_Type = _i_Type;
    L_strcpy(p->asz_Name, _psz_Name);
    L_strcpy(p->asz_Path, asz_Path);
    p->pv_Data = _p_Pointer;

    /* Insert the pointers in the list */
	if(!ptree)
	{
        ptree = (BAS_tdst_barray *) L_malloc(sizeof(BAS_tdst_barray));
        BAS_binit(ptree, 200);
        LINK_gx_TreeToPointers.SetAt(asz_Path, ptree);
	}
	BAS_binsert((ULONG) _p_Pointer, (ULONG) p, ptree);

    /* Create the corresponding dummy directory in the bigfile */
    BIG_ul_CreateDir(asz_Path);

    /* Add pointers to list of added ones */
    LINK_gx_PointersJustAdded.SetAt(_p_Pointer, NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_DelRegisterPointer(void *_p_Pointer)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LINK_tdst_Pointer   *p;
    char                *psz_Tree, *psz_Tree1;
    BAS_tdst_barray     *ptree;
    char                asz_Path[BIG_C_MaxLenPath];
    char                *psz_Temp;
    int                 i_Num;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(LINK_gx_PointersToTree.Lookup(_p_Pointer, (void * &) psz_Tree))
    {
        if(LINK_gx_TreeToPointers.Lookup(psz_Tree, (void * &) ptree))
        {
            p = (LINK_tdst_Pointer *) BAS_bsearch((ULONG) _p_Pointer, ptree);
            if((int) p != -1)
            {
                BAS_bdelete((ULONG) _p_Pointer, ptree);

                /* If no more pointers for that path */
                if(ptree->num == 0)
                {
					BAS_bfree(ptree);
					L_free(ptree);
					LINK_gx_TreeToPointers.SetAt(psz_Tree, NULL);
                }

                L_strcpy(asz_Path, psz_Tree);
                psz_Tree1 = asz_Path;
                do
                {
                    LINK_gx_TreeToNum.Lookup(asz_Path, (void * &) i_Num);
                    if(i_Num <= 1)
                    {
						LINK_gx_TreeToPointers.Lookup(asz_Path, (void * &) ptree);
						if(ptree)
						{
							BAS_bfree(ptree);
							L_free(ptree);
						}
                        LINK_gx_TreeToPointers.RemoveKey(asz_Path);
                        LINK_gx_PointersToTree.RemoveKey(_p_Pointer);
                        LINK_gx_TreeToNum.RemoveKey(asz_Path);
                    }
                    else
                    {
                        LINK_gx_TreeToNum.SetAt(asz_Path, (void *) (i_Num - 1));
                    }

                    psz_Temp = L_strrchr(asz_Path, '/');
                    if(psz_Temp) *psz_Temp = 0;
                } while(psz_Temp);

                /* Remove and free pointer */
                L_free(psz_Tree);
                LINK_gx_PointersToTree.RemoveKey(_p_Pointer);

                L_free(p);

                /* Add pointers to list of deleted ones */
                LINK_gx_PointersJustDeleted.SetAt(_p_Pointer, NULL);
            }
        }
    }
}

void LINK_SetDelPointer(void *p)
{
    LINK_gx_PointersJustDeleted.SetAt(p, NULL);
}

/*
 =======================================================================================================================
    Aim:    Search the registered structure associated with a given engine pointer. That structure
            can be used to retreive the structure type, the name of the pointer etc...
 =======================================================================================================================
 */
LINK_tdst_Pointer *LINK_p_SearchPointer(void *_p_Pointer)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LINK_tdst_Pointer   *p;
    char                *psz_Tree;
    BAS_tdst_barray     *ptree;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    p = NULL;
    if(LINK_gx_PointersToTree.Lookup(_p_Pointer, (void * &) psz_Tree))
    {
        if(LINK_gx_TreeToPointers.Lookup(psz_Tree, (void * &) ptree))
            p = (LINK_tdst_Pointer *) BAS_bsearch((ULONG) _p_Pointer, ptree);
    }

    if((int) p == -1) return NULL;
    return p;
}

/*
 =======================================================================================================================
	vincent : I add a argument : _b_KeyPrefix£
			when this _b_KeyPrefix is false the function work as before£
			otherwise the prefix [newkey] is added after src gao name instead of @adress
 =======================================================================================================================
 */
void LINK_RegisterDuplicate(void *_p_Src, void *_p_Dst, ULONG _ul_KeyPrefix )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LINK_tdst_Pointer   *p2;
    char                asz_Name[LINK_C_MaxLenNamePointer];
    char                asz_Ext[LINK_C_MaxLenNamePointer];
    char                *psz_Temp;
    BOOL				b_Prefix;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer(_p_Src);
    if(p2)
    {
        L_strcpy(asz_Ext, p2->asz_Name);
        psz_Temp = L_strrchr(asz_Ext, '.');
        if(psz_Temp) L_strcpy(asz_Ext, psz_Temp);
        
        if ( (_ul_KeyPrefix != 0) && (_ul_KeyPrefix != BIG_C_InvalidKey) )
        {
			b_Prefix = TRUE;
			sprintf(asz_Name, "[%08x] ", _ul_KeyPrefix );
			L_strcat(asz_Name, p2->asz_Name);
		}
		else
		{
			b_Prefix = FALSE;
			if ( ( L_strlen( p2->asz_Name) > 10 ) && (p2->asz_Name[0] == '[' ) && (p2->asz_Name[9] == ']' ) )
			{
				if (p2->asz_Name[10] == ' ' )
					L_strcpy(asz_Name, p2->asz_Name + 11);
				else
					L_strcpy(asz_Name, p2->asz_Name + 10);
			}
			else
				L_strcpy(asz_Name, p2->asz_Name);
		}
        
        psz_Temp = L_strrchr(asz_Name, '.');
        if(psz_Temp) *psz_Temp = 0;
        psz_Temp = L_strrchr(asz_Name, '@');
		if(psz_Temp) *psz_Temp = 0;
        
        if ( !b_Prefix )
			sprintf(asz_Name + L_strlen(asz_Name), "@%x", (ULONG) _p_Dst);
		
		L_strcat(asz_Name, asz_Ext);
		LINK_RegisterPointer(_p_Dst, p2->i_Type, asz_Name, p2->asz_Path);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_UpdatePointer(void *_p_Pointer)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/
    LINK_tdst_Pointer   *p2;
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/

    p2 = LINK_p_SearchPointer(_p_Pointer);
    ERR_X_Assert(p2);
    LINK_gx_PointersJustUpdated.SetAt(_p_Pointer, NULL);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_UpdatePointerAndName(void *_p_Pointer, char *_psz_Name)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/
    LINK_tdst_Pointer   *p2;
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/

    p2 = LINK_p_SearchPointer(_p_Pointer);
    ERR_X_Assert(p2);
    L_strcpy(p2->asz_Name, _psz_Name);
    LINK_gx_PointersJustUpdated.SetAt(_p_Pointer, NULL);
}

#endif /* ACTIVE_EDITORS */
