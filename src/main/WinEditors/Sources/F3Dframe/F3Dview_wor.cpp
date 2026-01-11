/*$T F3Dview_wor.cpp GC!1.56 12/20/99 14:24:44 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "LINKs/LINKstruct.h"
#include "CAMera/CAMera.h"
#include "EDImainframe.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "ENGine/Sources/WORld/WORexporttomad.h"
#include "ENGine/Sources/WORld/WORexport.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "GEOmetric/GEOload.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dview_undo.h"
#include "SOFT/SOFThelper.h"
#include "DIAlogs/DIAname_dlg.h"
#include "EDIpaths.h"
#include "EDItors/Sources/OUTput/OUTframe.h"

#ifdef JADEFUSION
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "XenonMeshProcessing/Sources/XMP_XenonMesh.h"
#endif

#ifdef JADEFUSION
#include "Light/LIGHTrejection.h"
#include "DIAlogs/DIA_UPDATE_dlg.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BIGfiles/BIGio.h"
#include "DIAlogs/CCheckList.h"
#include <map>
#include <string>

extern ULONG	EDI_OUT_gl_ForceSetMode;
extern ULONG	OBJ_ul_GameObjectRLICallback(ULONG _ul_PosFile);

// TODO: Move me
GEO_tdst_Object* GEO_pst_ExtractGeo(GRO_tdst_Struct* _pst_GRO)
{
	if (!_pst_GRO)
		return NULL;

	if (_pst_GRO->i->ul_Type == GRO_GeoStaticLOD)
	{
		GEO_tdst_StaticLOD* pst_LOD = (GEO_tdst_StaticLOD*)_pst_GRO;

		if (pst_LOD->uc_NbLOD == 0)
			return NULL;

		if (pst_LOD->dpst_Id[0] == NULL)
			return NULL;

		return GEO_pst_ExtractGeo(pst_LOD->dpst_Id[0]);
	}
	else if (_pst_GRO->i->ul_Type == GRO_Geometric)
	{
		return (GEO_tdst_Object*)_pst_GRO;
	}

	return NULL;
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::OneGameObjectChanged(BIG_INDEX _ul_Ref, ULONG _ul_Address, INT _i_Msg)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::OneGraphicObjectChanged(BIG_INDEX _ul_Ref, ULONG _ul_Address, INT _i_Msg)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    long                l_Index;
    WOR_tdst_World      *pst_World;
    OBJ_tdst_GameObject *pst_GO;
    TAB_tdst_PFelem     *pst_PFElem, *pst_PFLastElem;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if((pst_World = mst_WinHandles.pst_World) == NULL) return;

    if(_i_Msg == EDI_FHC_Delete)
    {
        /* Search if object is used by the world */
        l_Index = TAB_ul_Ptable_GetElemIndexWithPointer(&pst_World->st_GraphicObjectsTable, (void *) _ul_Address);

        if(l_Index != -1)
        {
            pst_PFElem = TAB_pst_PFtable_GetFirstElem(&pst_World->st_AllWorldObjects);
            pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&pst_World->st_AllWorldObjects);
            while(pst_PFElem <= pst_PFLastElem)
            {
                pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
                pst_PFElem++;
                if(TAB_b_IsAHole(pst_GO)) continue;

                if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)) continue;

                if(pst_GO->pst_Base->pst_Visu->pst_Object == (GRO_tdst_Struct *) _ul_Address)
                    pst_GO->pst_Base->pst_Visu->pst_Object = NULL;
            }
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::OneGraphicLightChanged(BIG_INDEX _ul_Ref, ULONG _ul_Address, INT _i_Msg)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    long                l_Index;
    WOR_tdst_World      *pst_World;
    OBJ_tdst_GameObject *pst_GO;
    TAB_tdst_PFelem     *pst_PFElem, *pst_PFLastElem;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if((pst_World = mst_WinHandles.pst_World) == NULL) return;

    if(_i_Msg == EDI_FHC_Delete)
    {
        /* Search if object is used by the world */
        l_Index = TAB_ul_Ptable_GetElemIndexWithPointer(&pst_World->st_GraphicObjectsTable, (void *) _ul_Address);

        if(l_Index != -1)
        {
            pst_PFElem = TAB_pst_PFtable_GetFirstElem(&pst_World->st_AllWorldObjects);
            pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&pst_World->st_AllWorldObjects);
            while(pst_PFElem <= pst_PFLastElem)
            {
                pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
                pst_PFElem++;
                if(TAB_b_IsAHole(pst_GO)) continue;

                if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Lights)) continue;

                if(pst_GO->pst_Extended->pst_Light == (GRO_tdst_Struct *) _ul_Address)
                {
                    pst_GO->pst_Extended->pst_Light = NULL;
                    pst_GO->ul_IdentityFlags -= OBJ_C_IdentityFlag_Lights;
                }
            }
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::OneGraphicMaterialChanged(BIG_INDEX _ul_Ref, ULONG _ul_Address, INT _i_Msg)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    long                l_Index;
    WOR_tdst_World      *pst_World;
    OBJ_tdst_GameObject *pst_GO;
    TAB_tdst_PFelem     *pst_PFElem, *pst_PFLastElem;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if((pst_World = mst_WinHandles.pst_World) == NULL) return;

    if(_i_Msg == EDI_FHC_Delete)
    {
        /* Search if object is used by the world */
        l_Index = TAB_ul_Ptable_GetElemIndexWithPointer(&pst_World->st_GraphicMaterialsTable, (void *) _ul_Address);

        if(l_Index)
        {
            pst_PFElem = TAB_pst_PFtable_GetFirstElem(&pst_World->st_AllWorldObjects);
            pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&pst_World->st_AllWorldObjects);
            while(pst_PFElem <= pst_PFLastElem)
            {
                pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
                pst_PFElem++;
                if(TAB_b_IsAHole(pst_GO)) continue;

                if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)) continue;

                if(pst_GO->pst_Base->pst_Visu->pst_Material == (GRO_tdst_Struct *) _ul_Address)
                    pst_GO->pst_Base->pst_Visu->pst_Material = NULL;
            }
        }
    }
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::SaveWorld(bool onlySelected)
{
    if(mst_WinHandles.pst_World == NULL) return;
    WOR_l_World_Save(mst_WinHandles.pst_World, onlySelected ? WOR_C_SaveSelected : 0);
    mo_UndoManager.mi_MarkedIndex = mo_UndoManager.mi_CurrentIndex;
}

#ifdef JADEFUSION
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::CheckXMPConsistency(void)
{
    TAB_tdst_PFelem* pst_Elem;
    TAB_tdst_PFelem* pst_LastElem;

    if (mst_WinHandles.pst_World == NULL)
        return;

    pst_Elem     = &mst_WinHandles.pst_World->st_AllWorldObjects.p_Table[0];
    pst_LastElem =  mst_WinHandles.pst_World->st_AllWorldObjects.p_NextElem;
    for (; pst_Elem < pst_LastElem; ++pst_Elem)
    {
        if(TAB_b_IsAHole(pst_Elem->p_Pointer))
            continue;

        XMP_CheckConsistency(mst_WinHandles.pst_World, (OBJ_tdst_GameObject*)(pst_Elem->p_Pointer));
    }
}

/*
=======================================================================================================================
=======================================================================================================================
*/

#if defined(_XENON_RENDER)

// SC: Enable debug information about objects and groups they belong to
#define XE_TRACK_GAO_PACKER

class XeGAOPacker
{
    typedef std::vector<OBJ_tdst_GameObject*> GAOArray;
    typedef std::list<GAOArray*>              GAOArrayList;

    enum
    {
        MAX_GROUP = 4,
    };

    class GAOGroup
    {
    public:

        M_DeclareOperatorNewAndDelete();

        GAOGroup(void);
        ~GAOGroup(void);

        void AddGAO(OBJ_tdst_GameObject* _pst_GO);

        void Pack(void);

    private:

        BOOL IsGAOIntersecting(OBJ_tdst_GameObject* _pst_GO, GAOArray* _pst_Array);

    private:

        GAOArrayList m_AllGroups;
    };

public:

    M_DeclareOperatorNewAndDelete();

    XeGAOPacker(void);
    ~XeGAOPacker(void);

    void AddGAO(OBJ_tdst_GameObject* _pst_GO, ULONG _ul_Group);

    void Pack(void);

private:

    GAOGroup m_aoGroups[MAX_GROUP];
};

XeGAOPacker::GAOGroup::GAOGroup(void)
{
}

XeGAOPacker::GAOGroup::~GAOGroup(void)
{
    GAOArrayList::iterator it;

    it = m_AllGroups.begin();
    while (it != m_AllGroups.end())
    {
        delete (*it);

        ++it;
    }
}

void XeGAOPacker::GAOGroup::AddGAO(OBJ_tdst_GameObject* _pst_GO)
{
    GAOArrayList::iterator it = m_AllGroups.begin();
    GAOArrayList oConnectedArrays;

#if defined(XE_TRACK_GAO_PACKER)
    ERR_OutputDebugString("[XeGAOPacker] - Adding object %s\n", _pst_GO->sz_Name ? _pst_GO->sz_Name : "<Unknown>");
#endif

    while (it != m_AllGroups.end())
    {
        GAOArray* pArray = *it;

        if (IsGAOIntersecting(_pst_GO, pArray))
        {
            oConnectedArrays.push_back(pArray);
        }

        ++it;
    }

    if (!oConnectedArrays.empty())
    {
        if (oConnectedArrays.size() == 1)
        {
            it = oConnectedArrays.begin();
            (*it)->push_back(_pst_GO);

#if defined(XE_TRACK_GAO_PACKER)
            ERR_OutputDebugString("[XeGAOPacker]   Intersection with only one group\n");
#endif
        }
        else
        {
            it = oConnectedArrays.begin();

            GAOArray* pParentArray = *it;
            ++it;

#if defined(XE_TRACK_GAO_PACKER)
            ERR_OutputDebugString("[XeGAOPacker]   Intersection with multiple groups (%u)\n", oConnectedArrays.size());
#endif

            while (it != oConnectedArrays.end())
            {
                GAOArray* pChildArray = *it;

                // Insert the child array into the parent array
                for (ULONG i = 0; i < pChildArray->size(); ++i)
                {
                    pParentArray->push_back((*pChildArray)[i]);
                }

                m_AllGroups.remove(pChildArray);
                delete pChildArray;

                ++it;
            }

            pParentArray->push_back(_pst_GO);
        }
    }
    else
    {
        GAOArray* pNewArray = new GAOArray;

        pNewArray->push_back(_pst_GO);
        m_AllGroups.push_back(pNewArray);

#if defined(XE_TRACK_GAO_PACKER)
        ERR_OutputDebugString("[XeGAOPacker]   No intersection found, creating a new group\n");
#endif
    }
}

BOOL XeGAOPacker::GAOGroup::IsGAOIntersecting(OBJ_tdst_GameObject* _pst_GO, GAOArray* _pst_Array)
{
    MATH_tdst_Vector st_ObjMin;
    MATH_tdst_Vector st_ObjMax;

    // Only consider boxes
    if (!OBJ_BV_IsAABBox(_pst_GO->pst_BV))
        return FALSE;

    // Bounding volume of the main game object
    MATH_AddVector(&st_ObjMax, OBJ_pst_BV_GetGMax(_pst_GO->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_GO));
    MATH_AddVector(&st_ObjMin, OBJ_pst_BV_GetGMin(_pst_GO->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_GO));

    // Make the box just a little bigger to make sure we don't discard a valid neighbor
    st_ObjMax.x += 0.01f; st_ObjMax.y += 0.01f; st_ObjMax.z += 0.01f;
    st_ObjMin.x -= 0.01f; st_ObjMin.y -= 0.01f; st_ObjMin.z -= 0.01f;

    for (ULONG i = 0; i < _pst_Array->size(); ++i)
    {
        OBJ_tdst_GameObject* pst_GO = (OBJ_tdst_GameObject*)(*_pst_Array)[i];

        if (!OBJ_BV_IsAABBox(pst_GO))
            continue;

        MATH_tdst_Vector st_Min;
        MATH_tdst_Vector st_Max;

        MATH_AddVector(&st_Max, OBJ_pst_BV_GetGMax(pst_GO->pst_BV), OBJ_pst_GetAbsolutePosition(pst_GO));
        MATH_AddVector(&st_Min, OBJ_pst_BV_GetGMin(pst_GO->pst_BV), OBJ_pst_GetAbsolutePosition(pst_GO));

        if (!INT_AABBoxAABBox(&st_ObjMin, &st_ObjMax, &st_Min, &st_Max))
            continue;

        return TRUE;
    }

    return FALSE;
}

void XeGAOPacker::GAOGroup::Pack(void)
{
    GAOArrayList::iterator it = m_AllGroups.begin();

    while (it != m_AllGroups.end())
    {
        GAOArray* pArray = *it;

#if defined(XE_TRACK_GAO_PACKER)
        CHAR szMsg[1024];

        sprintf(szMsg, "-> Packing game object group");
        LINK_PrintStatusMsg(szMsg);

        for (ULONG i = 0; i < pArray->size(); ++i)
        {
            OBJ_tdst_GameObject* pst_GO = (*pArray)[i];

            sprintf(szMsg, "--- GAO: %s", pst_GO->sz_Name ? pst_GO->sz_Name : "<Unknown>");
            LINK_PrintStatusMsg(szMsg);
        }
#endif

        GEO_PackGameObjectArray(&(*pArray)[0], pArray->size());

        ++it;
    }
}

XeGAOPacker::XeGAOPacker(void)
{
}

XeGAOPacker::~XeGAOPacker(void)
{
}

void XeGAOPacker::AddGAO(OBJ_tdst_GameObject* _pst_GO, ULONG _ul_Group)
{
    ERR_X_Assert(_ul_Group < MAX_GROUP);
    ERR_X_Assert(_pst_GO != NULL);

    m_aoGroups[_ul_Group].AddGAO(_pst_GO);
}

void XeGAOPacker::Pack(void)
{
    for (ULONG i = 0; i < MAX_GROUP; ++i)
    {
#if defined(XE_TRACK_GAO_PACKER)
        CHAR szMsg[1024];

        sprintf(szMsg, "]> Packing smoothing group %u", i + 1);
        LINK_PrintStatusMsg(szMsg);
#endif

        m_aoGroups[i].Pack();
    }
}

#endif

void F3D_cl_View::XeComputeTangentSpaceSmoothing(ULONG _ul_Mask)
{
#if defined(_XENON_RENDER)

    const ULONG CTSS_NB_GROUPS = 4;

    if (mst_WinHandles.pst_World == NULL)
        return;

    XeGAOPacker      oPacker;
    TAB_tdst_PFelem* pst_Elem;
    TAB_tdst_PFelem* pst_LastElem;

    pst_Elem     = &mst_WinHandles.pst_World->st_AllWorldObjects.p_Table[0];
    pst_LastElem =  mst_WinHandles.pst_World->st_AllWorldObjects.p_NextElem;
    for ( ; pst_Elem < pst_LastElem; ++pst_Elem)
    {
        if (TAB_b_IsAHole(pst_Elem->p_Pointer))
            continue;

        OBJ_tdst_GameObject* pst_GO = (OBJ_tdst_GameObject*)pst_Elem->p_Pointer;

        if (!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
            continue;

        if (!pst_GO->pst_Base || !pst_GO->pst_Base->pst_Visu)
            continue;

        ULONG ulSmoothGroup = ((pst_GO->pst_Base->pst_Visu->ul_XenonMeshProcessingFlags & GRO_XMPF_SmoothGroupMask) >> GRO_XMPF_SmoothGroupShift);
        if ((ulSmoothGroup == 0) || (ulSmoothGroup > CTSS_NB_GROUPS))
            continue;

        --ulSmoothGroup;
        if (((1 << ulSmoothGroup) & _ul_Mask) == 0)
            continue;

        oPacker.AddGAO(pst_GO, ulSmoothGroup);
    }

    oPacker.Pack();

    LINK_Refresh();

#endif
}

/*
=======================================================================================================================
=======================================================================================================================
*/
typedef struct tdst_AddInfo_
{
	ULONG ul_DrawMask;
	ULONG ul_XMPFlags;
	BIG_KEY GAO_Key;
	BIG_KEY GRO_Key;
	BIG_KEY RLI_Key;
} tdst_AddInfo;

void F3D_cl_View::ImportGRO_MTLassociation(char* _sz_Path, CRect* _po_Rect,char* _sz_PrefixeList)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	L_FILE								 x_File;

	std::map<std::string, BIG_KEY>					MapName_MTL;
	std::map<std::string, BIG_KEY>::iterator		MapName_MTL_It;

	std::map<std::string, tdst_AddInfo>				MapName_MTLAddInfo;
	std::map<std::string, tdst_AddInfo>::iterator	MapName_MTLAddInfo_It;

	std::map<BIG_KEY, BIG_KEY>						MapSrcKey_GAOKey;

	//prefixe list
	std::vector<std::string>						vPrefixeList;

	SEL_tdst_SelectedItem							*pst_SelCurrentElem, *pst_SelEndElem;

	TAB_tdst_PFtable	*pst_AIEOT;
	TAB_tdst_PFelem		*pst_CurrentElem, *pst_EndElem;
	OBJ_tdst_GameObject	*pst_GAO;
	GRO_tdst_Visu		*pst_Visu;
	GEO_tdst_Object		*pst_Geo;

	tdst_AddInfo		st_AddInfo;
	ULONG				ul_CoupleNum, ul_Copy;
	ULONG				*pul_RLI;
	BIG_KEY				MTL_Key, LRL_Key;
	BIG_KEY				CurrentGRO_Key, CurrentMTL_Key;
	BIG_INDEX			ul_Index;
	char				sz_Name[BIG_C_MaxLenName];
	char*				sz_ptr;
	std::string			strName, strGaoName, strPrefixe;
	int					i_NumOfPair, j, pos, i_NbPrefixe, i_UpdateSel;

	static EDIA_cl_UPDATEDialog		*mpo_Progress;
	char							str[256];
	BOOL							b_IsCanceled;

	CCheckList			o_Check;
	BOOL				pb[F3D_UPDATE_NUM];
	BOOL				b_SomethingIsUpdated;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	//open file
	x_File = L_fopen(_sz_Path, L_fopen_RB);
	ERR_X_Error(CLI_FileOpen(x_File), L_ERR_Csz_FOpen, _sz_Path);

	// read entry number
	L_freadA((void *) &ul_CoupleNum, sizeof(ul_CoupleNum), 1, x_File);  

	// read Source Light Rejection List key
	L_freadA((void *) &LRL_Key, sizeof(LRL_Key), 1, x_File);  

	for (ULONG i = 0; i < ul_CoupleNum; ++i)
	{
		L_freadA((void *) &sz_Name, sizeof(sz_Name), 1, x_File);
		L_freadA((void *) &MTL_Key, sizeof(MTL_Key), 1, x_File);

		//get GRO and RLI key
		L_freadA((void *) &st_AddInfo.GRO_Key, sizeof(st_AddInfo.GRO_Key), 1, x_File);
		L_freadA((void *) &st_AddInfo.RLI_Key, sizeof(st_AddInfo.RLI_Key), 1, x_File);

		L_freadA((void *) &st_AddInfo.ul_DrawMask, sizeof(st_AddInfo.ul_DrawMask), 1, x_File);
		L_freadA((void *) &st_AddInfo.ul_XMPFlags, sizeof(st_AddInfo.ul_XMPFlags), 1, x_File);
		L_freadA((void *) &st_AddInfo.GAO_Key, sizeof(st_AddInfo.GAO_Key), 1, x_File);

		//construct Map
		MapName_MTL.insert(std::map<std::string, BIG_KEY>::value_type(sz_Name, MTL_Key));
		MapName_MTLAddInfo.insert(std::map<std::string, tdst_AddInfo>::value_type(sz_Name, st_AddInfo));
	}

	//*************
	//update what ?
	b_SomethingIsUpdated = FALSE;
	for(int i = 0; i < F3D_UPDATE_NUM; ++i)
	{
		pb[i] = FALSE;
	}

	o_Check.AddString("Materials");
	o_Check.AddString("Flags (DrawMask & XMP)");
	o_Check.AddString("Light Rejection List");
	o_Check.AddString("GRO and RLI");

	_po_Rect->left += (_po_Rect->Width() / 2) - 100;
	_po_Rect->top += (_po_Rect->Height() / 2) - 100;	
	o_Check.SetArrayBool( pb );
	o_Check.Do( _po_Rect, 200, this, 20, "Update...");
	//*************

	i_NumOfPair = MapName_MTL.size();

	//close file
	if(CLI_FileOpen(x_File))
		L_fclose(x_File);

	for(int i = 0; i < F3D_UPDATE_NUM; ++i)
	{
		if(pb[i])
			b_SomethingIsUpdated = TRUE;
	}

	//quit if no update needed
	if(!b_SomethingIsUpdated)
		return;

	//init prefixe list
	if(_sz_PrefixeList)
	{
		//tokenize
		sz_ptr = strtok(_sz_PrefixeList, ",");
		while(sz_ptr != NULL)
		{
			vPrefixeList.push_back(sz_ptr);
			sz_ptr = strtok(NULL, ",");
		}
		i_NbPrefixe = (int) vPrefixeList.size();
	}
	else
		i_NbPrefixe = 0;


	i_UpdateSel = MessageBox("Update only selection ?", "Please confirm", MB_YESNOCANCEL | MB_ICONQUESTION);
	
	//user cancel
	if(i_UpdateSel != IDCANCEL)
	{
		i_UpdateSel = (i_UpdateSel == IDYES) ? TRUE : FALSE;
	}
	else
		return;

	//init selection pointers
	pst_SelCurrentElem = mst_WinHandles.pst_World->pst_Selection->pst_FirstItem;
	pst_SelEndElem     = mst_WinHandles.pst_World->pst_Selection->pst_LastItem;

	//init all world objects table pointers
	pst_AIEOT = &(mst_WinHandles.pst_World->st_AllWorldObjects);
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
	pst_EndElem     = TAB_pst_PFtable_GetLastElem(pst_AIEOT);
	
	//init progress bar
	mpo_Progress = new EDIA_cl_UPDATEDialog("Updating...");
	mpo_Progress->DoModeless();
	
	b_IsCanceled = FALSE;
	j = 0;

	//for each objects
	for( ; ((pst_CurrentElem <= pst_EndElem) && !i_UpdateSel) 
	    || ((pst_SelCurrentElem <= pst_SelEndElem) && i_UpdateSel)
		 ; ++pst_CurrentElem, ++pst_SelCurrentElem)
	{
		if(i_UpdateSel)
		{
			//get gao reference from gao selection
			pst_GAO = (OBJ_tdst_GameObject *) pst_SelCurrentElem->p_Content;
		}
		else
		{
			//get gao reference from all word objects table
			pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		}

		if(TAB_b_IsAHole(pst_GAO))	continue;
		if( !OBJ_b_TestIdentityFlag(pst_GAO, OBJ_C_IdentityFlag_Visu) ) continue;
		if(
			!pst_GAO->pst_Base
			|| !pst_GAO->pst_Base->pst_Visu
			|| !pst_GAO->pst_Base->pst_Visu->pst_Material
			|| !pst_GAO->pst_Base->pst_Visu->pst_Object
		  ) continue;
	
		pst_Geo = GEO_pst_ExtractGeo(pst_GAO->pst_Base->pst_Visu->pst_Object);

		CurrentGRO_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Geo);
		CurrentMTL_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_GAO->pst_Base->pst_Visu->pst_Material);
		
		if(CurrentGRO_Key == BIG_C_InvalidKey || CurrentMTL_Key == BIG_C_InvalidKey)
			continue;

		//update progress bar
		sprintf(str, "Updating Gao (%d of %d)", j, i_NumOfPair);
		mpo_Progress->OnRefreshBarText((float)j++/(float)i_NumOfPair, str);

		if(mpo_Progress->bIsCanceled)
		{
			b_IsCanceled = TRUE;
			break;
		}

		pst_Visu = pst_GAO->pst_Base->pst_Visu;

		strGaoName = pst_GAO->sz_Name;

		//remove prefixe if it has one
		if(i_NbPrefixe)
		{
			for(int i = 0; i < i_NbPrefixe; i++)
			{
				if(strGaoName.find(vPrefixeList[i]) == 0)
				{
					strGaoName.erase(0, vPrefixeList[i].size());
					break;
				}
			}
		}
		
		/*get gao name without .gao */
		pos = strGaoName.rfind(".");
		if(pos != -1)
			strName = strGaoName.substr(0, pos);
		else
			continue;
		
		//search in map
		MapName_MTL_It = MapName_MTL.find(strName);
		MapName_MTLAddInfo_It = MapName_MTLAddInfo.find(strName);

		if(MapName_MTL_It == MapName_MTL.end())
		{
			pos = strGaoName.rfind("@");
			if(pos != -1)
			{
				strName = strGaoName.substr(0, pos);
				MapName_MTL_It = MapName_MTL.find(strName);
				MapName_MTLAddInfo_It = MapName_MTLAddInfo.find(strName);
			}
			else
				continue;
		}
		
		//Update Material
		if(pb[F3D_UPDATE_MTL] && MapName_MTL_It != MapName_MTL.end())
		{
			b_SomethingIsUpdated = TRUE;

			//CurrentGRO_Key is in list
     		if(CurrentMTL_Key != MapName_MTL_It->second)
			{
				ul_Index = BIG_ul_SearchKeyToFat(MapName_MTL_It->second);
      
				if(ul_Index != BIG_C_InvalidIndex) 
				{
					AssignMaterial(pst_GAO, ul_Index);

					//auto set the ForceRtl flag
					pst_GAO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_ForceRTL;
				}
			}
		}

		//Update GRO and RLI
		if(pb[F3D_UPDATE_GRO_RLI] && MapName_MTLAddInfo_It != MapName_MTLAddInfo.end())
		{
			b_SomethingIsUpdated = TRUE;

			st_AddInfo = MapName_MTLAddInfo_It->second;

			if(st_AddInfo.GRO_Key != BIG_C_InvalidKey && CurrentGRO_Key != st_AddInfo.GRO_Key)
			{
				//GRO is different, update both GRO and RLI

				//GRO********************************************************************
				GRO_tdst_Struct* pst_GRO = (GRO_tdst_Struct*)LOA_ul_SearchAddress(st_AddInfo.GRO_Key);

				if (pst_GRO != (GRO_tdst_Struct*)BIG_C_InvalidIndex)
				{
					pst_Geo = GEO_pst_ExtractGeo(pst_GRO);
				}
				else
				{
					pst_Geo = NULL;
				}
				
				if(pst_Geo) 
				{	
					//GRO already loaded, inc ref count and assign to GAO
					pst_GAO->pst_Base->pst_Visu->pst_Object = (GRO_tdst_Struct*) pst_Geo;
					pst_Geo->st_Id.i->pfn_AddRef(&pst_Geo->st_Id, 1);
				}
				else
				{
					//GRO not loaded, load it
					pst_Geo = (GEO_tdst_Object*) GEO_pst_Object_Load(BIG_ul_SearchKeyToFat(st_AddInfo.GRO_Key), mst_WinHandles.pst_World);
					pst_GAO->pst_Base->pst_Visu->pst_Object = (GRO_tdst_Struct*) pst_Geo;
					pst_Geo->st_Id.i->pfn_AddRef(&pst_Geo->st_Id, 1);
				}

				//RLI*********************************************************************
				ul_Index =  BIG_ul_SearchKeyToFat(st_AddInfo.RLI_Key);

				if(ul_Index != BIG_C_InvalidIndex)
				{
					/* LOAD RLI */
					pul_RLI = (ULONG *) OBJ_ul_GameObjectRLICallback( BIG_PosFile(ul_Index) );
					if (pul_RLI)
					{
						/* CHECK GO RLI buffer */
						if ( pst_Visu->dul_VertexColors )
						{
							if ( pst_Visu->dul_VertexColors[0] != pst_Geo->l_NbPoints)
							{
								OBJ_VertexColor_Realloc( pst_GAO, pst_Geo->l_NbPoints );
							}
						}
						else
						{
							pst_Visu->dul_VertexColors = (ULONG *) MEM_p_Alloc( (pst_Geo->l_NbPoints + 1) * sizeof( ULONG ) );
							pst_Visu->dul_VertexColors[ 0 ] = pst_Geo->l_NbPoints;
						}

						/* COPY RLI into GO RLI buffer */
						if ( pul_RLI[ 0 ] < (ULONG) pst_Geo->l_NbPoints)
							ul_Copy = pul_RLI[ 0 ];
						else
							ul_Copy = pst_Geo->l_NbPoints;
						L_memcpy( &pst_GAO->pst_Base->pst_Visu->dul_VertexColors[ 1 ], &pul_RLI[ 1 ], ul_Copy * sizeof(ULONG) );
					}
				}
			}
		}

		//Update FLAGS
		if(pb[F3D_UPDATE_FLAGS] && MapName_MTLAddInfo_It != MapName_MTLAddInfo.end())
		{
			b_SomethingIsUpdated = TRUE;

			st_AddInfo = MapName_MTLAddInfo_It->second;

			pst_Visu->ul_DrawMask = st_AddInfo.ul_DrawMask;
			pst_Visu->ul_XenonMeshProcessingFlags = st_AddInfo.ul_XMPFlags;
		}
		//Update Lights Rejection List
		if(pb[F3D_UPDATE_LRL] && MapName_MTLAddInfo_It != MapName_MTLAddInfo.end())
		{
			st_AddInfo = MapName_MTLAddInfo_It->second;
			if(st_AddInfo.GAO_Key != pst_GAO->ul_MyKey)
			{
				MapSrcKey_GAOKey.insert(std::map<BIG_KEY, BIG_KEY>::value_type(BIG_ul_SearchKeyToPos(st_AddInfo.GAO_Key), BIG_ul_SearchKeyToPos(pst_GAO->ul_MyKey)));
			}
		}

		j++;
	}

	//Update Lights Rejection List
	if(pb[F3D_UPDATE_LRL])
	{
		b_SomethingIsUpdated = TRUE;

		//update progress bar
		mpo_Progress->OnRefreshBarText((float)j++/(float)i_NumOfPair, "Update Rejection List");

		LRL_Clean();
		LRL_Load(LRL_Key);
		LRL_SpecialResolve(MapSrcKey_GAOKey);
	}

	if(pb[F3D_UPDATE_MTL])
	{
		//update progress bar
		mpo_Progress->OnRefreshBarText((float)j++/(float)i_NumOfPair, "Reload all textures");

		//force reload of textures
		EDI_OUT_gl_ForceSetMode = 1;
		LINK_Refresh();
		EDI_OUT_gl_ForceSetMode = 0;
	}

	//notify user
	if(!b_IsCanceled && b_SomethingIsUpdated)
		MessageBox("Importing Name - MTL association successfully completed", "Info", MB_OK);

	delete mpo_Progress;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void F3D_cl_View::ExportGRO_MTLassociation(char* _sz_Path, char* _sz_PrefixeList)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	L_FILE				x_File;

	std::map<std::string, BIG_KEY>			 MapName_MTL;
	std::map<std::string, BIG_KEY>::iterator MapName_MTL_It;

	//prefixe list
	std::vector<std::string>				 vPrefixeList;
	char*				sz_ptr;
	std::string			szPrefixe;
	int					i_NbPrefixe;


	TAB_tdst_PFtable	*pst_AIEOT;
	TAB_tdst_PFelem		*pst_CurrentElem, *pst_EndElem;
	OBJ_tdst_GameObject	*pst_GAO;
	GRO_tdst_Visu		*pst_Visu;

	ULONG				ul_CoupleNum, ul_NbGao;
	BIG_KEY				GRO_Key, MTL_Key, RLI_Key;
	char				sz_Name[BIG_C_MaxLenName];
	std::string			strName, strGaoName;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_WinHandles.pst_World == NULL)
		return;

	//Create and open file
	x_File = L_fopen(_sz_Path, L_fopen_WB);
	ERR_X_Error(CLI_FileOpen(x_File), L_ERR_Csz_FOpen, _sz_Path);

	//init prefixe list
	if(_sz_PrefixeList)
	{
		//tokenize
		sz_ptr = strtok(_sz_PrefixeList, ",");
		while(sz_ptr != NULL)
		{
			vPrefixeList.push_back(sz_ptr);
			sz_ptr = strtok(NULL, ",");
		}
		i_NbPrefixe = (int) vPrefixeList.size();
	}

	ul_CoupleNum = 0;
	ul_NbGao = 0;

	//write couple num in file
	L_fwrite(&ul_CoupleNum, sizeof(ul_CoupleNum), 1, x_File);

	//write LRL Key
	L_fwrite(&g_LRL_ul_Key, sizeof(g_LRL_ul_Key), 1, x_File);

	//loop trough each object
	pst_AIEOT = &(mst_WinHandles.pst_World->st_AllWorldObjects);
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AIEOT);

	//for each objects
	for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;

		if(TAB_b_IsAHole(pst_GAO))	continue;
		
		if( !OBJ_b_TestIdentityFlag(pst_GAO, OBJ_C_IdentityFlag_Visu) ) continue;
		if(
			!pst_GAO->pst_Base
			|| !pst_GAO->pst_Base->pst_Visu
			|| !pst_GAO->pst_Base->pst_Visu->pst_Material
			|| !pst_GAO->pst_Base->pst_Visu->pst_Object
		  ) continue;

		GEO_tdst_Object* pst_Geo = GEO_pst_ExtractGeo(pst_GAO->pst_Base->pst_Visu->pst_Object);

		if(pst_Geo)
		{
			GRO_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Geo);
		}
		else
		{
			GRO_Key = BIG_C_InvalidKey;
		}

		MTL_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_GAO->pst_Base->pst_Visu->pst_Material);

		//get RLI Key
		if(pst_GAO->pst_Base->pst_Visu->dul_VertexColors)
			RLI_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_GAO->pst_Base->pst_Visu->dul_VertexColors);
		else
			RLI_Key = BIG_C_InvalidKey;

		if(GRO_Key == BIG_C_InvalidKey || MTL_Key == BIG_C_InvalidKey)
			continue;

		/* get gao name without ext(.gao) */
		strGaoName = pst_GAO->sz_Name;

		//remove prefixe if it has one
		if(i_NbPrefixe)
		{
			for(int i = 0; i < i_NbPrefixe; i++)
			{
				if(strGaoName.find(vPrefixeList[i]) == 0)
				{
					strGaoName.erase(0, vPrefixeList[i].size());
					break;
				}
			}
		}

		int pos = strGaoName.rfind(".");
		if(pos != -1 )
		{
			strName = strGaoName.substr(0, pos);
		}

		//copy name to sz_Name before writing it to file
		L_memset(sz_Name, 0, sizeof(sz_Name));
		strcpy(sz_Name, strName.c_str());
		
		if(MapName_MTL.insert(std::map<std::string, BIG_KEY>::value_type(strName, MTL_Key)).second)
		{
			++ul_CoupleNum;

			//Couple not already in map, write it to file
			L_fwrite(&sz_Name, sizeof(sz_Name), 1, x_File);
			L_fwrite(&MTL_Key, sizeof(MTL_Key), 1, x_File);

			//add GRO and RLI Key to file
			L_fwrite(&GRO_Key, sizeof(GRO_Key), 1, x_File);
			L_fwrite(&RLI_Key, sizeof(RLI_Key), 1, x_File);

			//write flags
			pst_Visu = pst_GAO->pst_Base->pst_Visu;
			L_fwrite(&pst_Visu->ul_DrawMask, sizeof(pst_Visu->ul_DrawMask), 1, x_File);
			L_fwrite(&pst_Visu->ul_XenonMeshProcessingFlags, sizeof(pst_Visu->ul_XenonMeshProcessingFlags), 1, x_File);
			
			//write GAO_key for LRL processing
			L_fwrite(&pst_GAO->ul_MyKey, sizeof(pst_GAO->ul_MyKey), 1, x_File);
		}
	}

	if(ul_CoupleNum)
	{
		// going to the begining of the file
		L_fseek(x_File, 0, 0);

		// Write number of KEY couple
		L_fwriteA(&ul_CoupleNum, sizeof(ul_CoupleNum), 1, x_File);   
	}

	//close files
	if(CLI_FileOpen(x_File))
		L_fclose(x_File);

	//notify user
	MessageBox("Exporting Name - MTL association successfully completed", "Info", MB_OK);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void F3D_cl_View::AssignMaterial(OBJ_tdst_GameObject* _pst_GO, BIG_INDEX _ul_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World					*pst_World;
	GRO_tdst_Struct					*pst_GRO, *pst_Mat;
	unsigned long					i, ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

 	pst_World = mst_WinHandles.pst_World;

	/* Verify if Xenon material is already loaded */
	pst_GRO = (GRO_tdst_Struct *)(LOA_ul_SearchAddress(BIG_PosFile(_ul_Index)));

	if((ULONG)pst_GRO == BIG_C_InvalidKey)
	{
		//load material
		pst_GRO = GEO_pst_Object_Load(_ul_Index, pst_World);
	}

	if(TAB_ul_Ptable_GetElemIndexWithPointer(&pst_World->st_GraphicMaterialsTable, pst_GRO) == TAB_Cul_BadIndex)
	{
		TAB_Ptable_AddElemAndResize(&pst_World->st_GraphicMaterialsTable, pst_GRO);
		pst_GRO->i->pfn_AddRef(pst_GRO, 1);
	}

	if(_pst_GO->pst_Base->pst_Visu->pst_Material)
		_pst_GO->pst_Base->pst_Visu->pst_Material->i->pfn_AddRef(_pst_GO->pst_Base->pst_Visu->pst_Material, -1);

	_pst_GO->pst_Base->pst_Visu->pst_Material = pst_GRO;
	pst_GRO->i->pfn_AddRef(pst_GRO, 1);

	if(pst_GRO->i->ul_Type == GRO_MaterialMulti)
	{
		for(i = 0; (long) i < ((MAT_tdst_Multi *) pst_GRO)->l_NumberOfSubMaterials; i++)
		{
			pst_Mat = (GRO_tdst_Struct *) ((MAT_tdst_Multi *) pst_GRO)->dpst_SubMaterial[i];
			if(!pst_Mat) continue;
			pst_Mat->i->pfn_AddRef(pst_Mat, 1);
			ul_Index = TAB_ul_Ptable_GetElemIndexWithPointer(&pst_World->st_GraphicMaterialsTable, pst_Mat);
			if(ul_Index == TAB_Cul_BadIndex)
			{
				TAB_Ptable_AddElemAndResize(&pst_World->st_GraphicMaterialsTable, pst_Mat);
				pst_Mat->i->pfn_AddRef(pst_Mat, 1);
			}
		}
	}
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::ExportWorldToMad(const char *_sz_FileName, BOOL _b_ExportSkin)
{
    if(mst_WinHandles.pst_World == NULL) return;
	snprintf( msz_AssociatedMadFile, sizeof( msz_AssociatedMadFile ), "%s", _sz_FileName );
	if (!WOR_b_World_ExportMadFile
	(
		mst_WinHandles.pst_World,
		msz_AssociatedMadFile,
		msz_ExportDir,
		((EOUT_cl_Frame*)mpo_AssociatedEditor)->mst_Ini.uc_ExportOnlySelection,
		((EOUT_cl_Frame*)mpo_AssociatedEditor)->mst_Ini.uc_ExportTexture,
		_b_ExportSkin
	))
	{
		MessageBox( "MAD export failed! Check console for details.", "Warning", MB_OK | MB_ICONWARNING );
	}
}

void F3D_cl_View::ExportWorldToSmd( const char *filename, bool skin )
{
	if ( mst_WinHandles.pst_World == nullptr )
	{
		return;
	}

	SMDExporter smdExport;
	if ( !smdExport.ExportFile( mst_WinHandles.pst_World, nullptr, filename,
		((EOUT_cl_Frame*)mpo_AssociatedEditor)->mst_Ini.uc_ExportOnlySelection,
		((EOUT_cl_Frame*)mpo_AssociatedEditor)->mst_Ini.uc_ExportTexture))
	{
		MessageBox( "SMD export failed! Check console for details.", "Warning", MB_OK | MB_ICONWARNING );
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::NewWorld(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDIA_cl_NameDialog	o_Dialog("Enter name of world");
    char                asz_Path[BIG_C_MaxLenPath];
    WOR_tdst_World      *pst_World;
	CString				o_Str;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(o_Dialog.DoModal() == IDOK)
    {
        if(!BIG_b_CheckName((char *) (LPCSTR) o_Dialog.mo_Name)) return;
        L_strcpy(asz_Path, EDI_Csz_Path_Levels);
        L_strcat(asz_Path, "/");
        L_strcat(asz_Path, (char *) (LPCSTR) o_Dialog.mo_Name);
        ERR_X_Error(BIG_ul_SearchDir(asz_Path) == BIG_C_InvalidIndex, "That level already exists", NULL);
        BIG_ul_CreateDir(asz_Path);

		/* Create engine world */
        pst_World = WOR_pst_World_Create();
		WOR_World_Init(pst_World, 0);

		o_Str = o_Dialog.mo_Name;
		L_strcpy(pst_World->sz_Name, (char *) (LPCSTR) o_Str);
		MATH_SetIdentityMatrix(&pst_World->st_CameraPosSave);
		pst_World->ul_AmbientColor = 0x00FFFF00;

		o_Str += EDI_Csz_ExtWorld;
		WOR_l_World_SaveWithFileName(pst_World, NULL, (char *) (LPCSTR) o_Str, 0);

		WOR_World_Destroy(pst_World);
    }
}

/*$4
 ***********************************************************************************************************************
    optimize
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Optimize_Omni( void )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TAB_tdst_Ptable     *pst_Table;
	GRO_tdst_Struct		**ppst_First, **ppst_Last;
    char                sz_Text[ 256 ];
 	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 
    if ( !mst_WinHandles.pst_World ) return;

	pst_Table = &mst_WinHandles.pst_World->st_GraphicObjectsTable;

	ppst_First = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetFirstElem( pst_Table);
	ppst_Last = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetLastElem( pst_Table);

	for(; ppst_First <= ppst_Last; ppst_First++)
	{
		if(TAB_b_IsAHole(*ppst_First)) continue;

        if ( *ppst_First == NULL ) continue;
        
        if ( (*ppst_First)->i->ul_Type != GRO_Light) 
            continue;

         if ( ( ((LIGHT_tdst_Light *) (*ppst_First))->ul_Flags & LIGHT_Cul_LF_Type ) != LIGHT_Cul_LF_Omni) 
            continue;


        if ( ( (LIGHT_tdst_Light *) (*ppst_First))->ul_Flags & LIGHT_Cul_LF_OmniConst) 
            continue;

        if ( ( ((LIGHT_tdst_Light *) (*ppst_First))->ul_Flags & (LIGHT_Cul_LF_RealTimeOnDynam | LIGHT_Cul_LF_RealTimeOnNonDynam)) == 0) 
            continue;

        sprintf( sz_Text, "Optimize %s", ( (LIGHT_tdst_Light *) (*ppst_First))->st_Id.sz_Name );
        LINK_PrintStatusMsg( sz_Text );
        ( (LIGHT_tdst_Light *) (*ppst_First))->ul_Flags |= LIGHT_Cul_LF_OmniConst;
	}
}

#endif /* ACTIVE_EDITORS */
