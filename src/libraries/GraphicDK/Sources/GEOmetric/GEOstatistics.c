/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */
#include "Precomp.h"
#include "GEOmetric/GEOstatistics.h"
#include "GEOmetric/GEOobject.h"
#include "ENGine\Sources\ANImation\ANIinit.h"
#include "ENGine/Sources/COLlision/COLaccess.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"

#include "XenonGraphics/XeVideoConsole.h"

#include "XenonGraphics/XeMesh.h"
#include "XenonGraphics/XeIndexBuffer.h"
#include "XenonGraphics/XeVertexBuffer.h"
#include "XenonGraphics/XeUtils.h"
#include "BIGfiles/VERsion/VERsion_Number.h"
#include "XenonGraphics/XeRenderer.h"



#ifdef XENONVIDEOSTATISTICS
extern XeRenderer g_oXeRenderer;
extern WOR_tdst_World       *WOR_gpst_CurrentWorld;

// This is a Singleton Class
XeGOStatistics* XeGOStatistics::m_pInstance = 0;// initialize pointer

/*
***********************************************************************************************************************
Singleton Instance function declaration
***********************************************************************************************************************
*/
XeGOStatistics* XeGOStatistics::Instance()
{
	if (m_pInstance == 0)  // is it the first call?
    {  
        m_pInstance = new XeGOStatistics; // create sole instance
    }
    return m_pInstance; // address of sole instance
}

/*
***********************************************************************************************************************
 Constructor
***********************************************************************************************************************
*/
XeGOStatistics::XeGOStatistics()
: m_iNbElements(0)
{ 
    m_eObjectType = eSelection_None;
    m_eSectorisation = eSectorisation_CurrentView;
    m_eSorting = eSorting_Poly;
    m_iTriggerDumpStat = FALSE;
    m_iNbGRO = 0;
    m_iNotItRendering = FALSE;

    // Init values to 0
    InitStats();

#ifdef VIDEOCONSOLE_ENABLE
    VideoConsole_AddAdditionalStatisticsOption(L"Type of objects", &m_eObjectType, kstTrackerType_Enum_ObjectType);
    VideoConsole_AddAdditionalStatisticsOption(L"Sectorisation", &m_eSectorisation, kstTrackerType_Enum_Sectorisation);
    VideoConsole_AddAdditionalStatisticsOption(L"Sorting", &m_eSorting, kstTrackerType_Enum_Sorting);
#endif

}


/*
***********************************************************************************************************************
Function call for every rendered Game Object (GRO) (Add this object to the stats)
***********************************************************************************************************************
*/
void XeGOStatistics::AddStatElement(OBJ_tdst_GameObject* _pst_GO)
{
    GRO_tdst_Visu						*pst_Visu;
    GEO_tdst_Object						*pst_Obj;
    GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;

    pst_Visu = _pst_GO->pst_Base->pst_Visu;
    pst_Obj = (GEO_tdst_Object *) pst_Visu->pst_Object;

    if ( !pst_Obj ) return;
    if ( !pst_Obj->dst_Point) return;
    if ( pst_Visu->ucFlag & GRO_VISU_FLAG_WATERFX) return;
    
    // send graphic elements
    pst_Element = pst_Obj->dst_Element;
    pst_LastElement = pst_Element + pst_Obj->l_NbElements;

    // for objects using the temporary geometric, create dynamic meshes
    MAT_tdst_Material* pst_Material = (MAT_tdst_Material *)pst_Visu->pst_Material;

    for(int iElem=0; pst_Element < pst_LastElement; pst_Element++, iElem++)
    {
        if (pst_Visu->pst_Material != NULL)
        {
            // Reset the material pointer so that we'll be able to fetch the right layer
            pst_Material = (MAT_tdst_Material *)pst_Visu->pst_Material;
        }

        if(pst_Visu->p_XeElements && pst_Visu->p_XeElements[iElem].pst_Mesh && pst_Material && ((pst_Material->st_Id.i->ul_Type == GRO_MaterialMulti) || (pst_Material->st_Id.i->ul_Type == GRO_MaterialMultiTexture)))
        {
            XeMesh* poMesh = pst_Visu->p_XeElements[iElem].pst_Mesh;

            if (pst_Material->st_Id.i->ul_Type == GRO_MaterialMulti)
            {
                if(((MAT_tdst_Multi *)pst_Material)->l_NumberOfSubMaterials == 0)
                    pst_Material = NULL;
                else
                    pst_Material = ((MAT_tdst_Multi *)pst_Material)->dpst_SubMaterial[lMin(pst_Element->l_MaterialId , ((MAT_tdst_Multi *)pst_Material)->l_NumberOfSubMaterials - 1)];
            }

            MAT_tdst_MultiTexture	*pst_MLTTX = (MAT_tdst_MultiTexture *) pst_Material;
            MAT_tdst_MTLevel		*pst_MLTTXLVL = pst_MLTTX->pst_FirstLevel;

            if(pst_Material == NULL) pst_Material= (MAT_tdst_Material *) &MAT_gst_DefaultSingleMaterial;

            if (pst_MLTTXLVL != NULL)
            {
                // for all material layers
                while (pst_MLTTXLVL && (LONG)pst_MLTTXLVL != -1)
                {
                    if(!(pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_InActive))
                    {
                        // ADD OBJECT
                        AddStatElement(_pst_GO,poMesh);
                    }

                    pst_MLTTXLVL = pst_MLTTXLVL->pst_NextLevel;
                }
            }
        }
    }
}

/*
***********************************************************************************************************************
Function call for every rendered Game Object (GRO) (Add this object to the stats)
***********************************************************************************************************************
*/
void XeGOStatistics::AddStatElement(OBJ_tdst_GameObject* _pst_GO, XeMesh *_poMesh)
{
    GRO_tdst_Visu *pst_Visu = _pst_GO->pst_Base->pst_Visu;
    GEO_tdst_Object	*pst_Obj = (GEO_tdst_Object *) pst_Visu->pst_Object;
    OBJ_tdst_GameObject *pst_AssociatedGO = _pst_GO;
    ULONG ulAddThisOne = 1;

    // For boned GAO (Part of animation) Find the key of the father
    while (pst_AssociatedGO->ul_MyKey == NULL && pst_AssociatedGO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone )
    {
        if (pst_AssociatedGO->pst_Base != NULL)
        {
            if (pst_AssociatedGO->pst_Base->pst_Hierarchy != NULL)
                if (pst_AssociatedGO->pst_Base->pst_Hierarchy->pst_Father)
                {
                    pst_AssociatedGO = pst_AssociatedGO->pst_Base->pst_Hierarchy->pst_Father;
                }
                else if (pst_AssociatedGO->pst_Base->pst_Hierarchy->pst_FatherInit)
                {
                    pst_AssociatedGO = pst_AssociatedGO->pst_Base->pst_Hierarchy->pst_FatherInit;
                }
                else
                {
                    return;
                }
            else 
                return;
        }
        else
        {
            return;
        }        
    }

    if (pst_AssociatedGO->ul_MyKey == NULL)
    {
        return;
    }

    if (m_iNotItRendering == 0)
    {    
        // For multiple elements object (avoid multiple entry)
        for (int i = 0; i < m_iNbGRO; i++)
        {
            if (m_ulGROList[i] == pst_AssociatedGO->ul_MyKey)
            {
                ulAddThisOne = 0;
            }
        }

        if (ulAddThisOne)
        {
            m_ulGROList[m_iNbGRO] = pst_AssociatedGO->ul_MyKey;
            m_iNbGRO++;       
        }
    }

    int uiNumTriangles = ComputeNumTriangles(_pst_GO, _poMesh);

    if (uiNumTriangles == 0)
    {
        return;
    }

    m_iNbRenderedTriangles += uiNumTriangles;

    // Nothing to add with these objects
    if (m_eObjectType == eSelection_None || m_eObjectType == eSelection_CollisionMesh)
    {
        return;
    }
    
    // Normal geometry          
    ulAddThisOne = 0;

    switch(m_eObjectType) {
    case eSelection_AllGro:
        if (pst_AssociatedGO == _pst_GO)
        {
            ulAddThisOne = 1;
        }
        break;
    case eSelection_SnP:
        if (_pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_EnableSnP)
        {
            ulAddThisOne = 1;
        }
        break;
    case eSelection_ODE:
        if (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
        {
            ulAddThisOne = 1;
        }
        break;
    case eSelection_CollMap:
        if (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap)
        {
            ulAddThisOne = 1;
        }
        break;
    case eSelection_AO:
        if (pst_AssociatedGO != _pst_GO)
        {
            ulAddThisOne = 1;
        }
        break;
    }         

    if (ulAddThisOne == 1)
    {
        m_ulGroKCI[eTableIndex_Key][m_iNbElements] = pst_AssociatedGO->ul_MyKey;
        m_ulGroKCI[eTableIndex_CountPoly][m_iNbElements] = uiNumTriangles;
        m_ulGroKCI[eTableIndex_Instances][m_iNbElements] = 1;
        m_iNbTriangles += uiNumTriangles;                    
        m_iNbElements++;
    }

    
    return;
}

/*
***********************************************************************************************************************
This function compute the coll mesh informations
***********************************************************************************************************************
*/
void XeGOStatistics::ComputeCollMesh()
{
    TAB_tdst_PFelem		        *pst_CurrentElem, *pst_EndElem;
    OBJ_tdst_GameObject         *pst_GO;
    COL_tdst_ColMap				*pst_ColMap;
    COL_tdst_IndexedTriangles	*pst_CobObj;
    COL_tdst_ElementIndexedTriangles	*pst_CobElement, *pst_LastCobElement;

    // Get all collision mesh elements of the current world
    pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&WOR_gpst_CurrentWorld->st_EOT.st_ColMap);
    pst_EndElem = TAB_pst_PFtable_GetLastElem(&WOR_gpst_CurrentWorld->st_EOT.st_ColMap);

    for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
    {
        pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
        if(TAB_b_IsAHole(pst_GO) || !(pst_GO->ul_MyKey)) continue;
        pst_ColMap = ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap;

        /* Dont want to deal with Mathematical Cobs. */
        if(pst_ColMap->dpst_Cob[0]->uc_Type != COL_C_Zone_Triangles) continue;
        pst_CobObj = pst_ColMap->dpst_Cob[0]->pst_TriangleCob;

        /* Loop thru all the Cob Elements */
        pst_CobElement = pst_CobObj->dst_Element;
        pst_LastCobElement = pst_CobElement + pst_CobObj->l_NbElements;

        // Compute the number of triangle of this collision mesh
        for(; pst_CobElement < pst_LastCobElement; pst_CobElement++)
        {
            m_iNbTriangles += pst_CobElement->uw_NbTriangles;
        }
    }
}

/*
***********************************************************************************************************************
Initialisation function (Called at every frame)
***********************************************************************************************************************
*/
void XeGOStatistics::InitStats()
{
    // These integer defined which metrics are computed
    if (m_iNbElements == 0)
    {
        m_iNbElements = MAX_ELEMENT;
    }

    for (int i = eTableIndex_Key; i < eTableIndex_Count ; i++)
    {
        for (int j = 0; j < m_iNbElements; j++)
        {
            m_ulGroKCI[i][j] = 0;
        }        
    }

    if (m_eObjectType == eSelection_None)
    {
        if (m_iNbGRO == 0)
        {
            m_iNbGRO = MAX_ELEMENT;
        }

        for (int i = 0; i < m_iNbGRO ; i++)
        {
            m_ulGROList[i] = 0;
        }

        m_iNbGRO = 0;
    }
   
    // Init
    m_iNbTriangles = 0;
    m_iNbElements = 0;
    m_iNbRenderedTriangles = 0;
}


/*
***********************************************************************************************************************
Add all objects from game (sectorisation dependent)
***********************************************************************************************************************
*/
void XeGOStatistics::AddAllObjects()
{
    TAB_tdst_PFelem		        *pst_CurrentElem, *pst_EndElem;
    OBJ_tdst_GameObject         *pst_GO;

    m_iNotItRendering = TRUE;

    // Get all collision mesh elements of the current world
    pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&WOR_gpst_CurrentWorld->st_AllWorldObjects);
    pst_EndElem = TAB_pst_PFtable_GetLastElem(&WOR_gpst_CurrentWorld->st_AllWorldObjects);

    for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
    {
        pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
        if(TAB_b_IsAHole(pst_GO)) continue;
        if (!pst_GO->pst_Base) continue;
        if (!pst_GO->pst_Base->pst_Visu) continue;      
        if (pst_GO->pst_Base->pst_Visu->c_DisplayPos != 0) continue;

        if (m_eSectorisation == eSectorisation_CurrentView)
        {
            AddStatElement(pst_GO);
        }
        else
        {                
            int nNumNullSector = 0;

            // Check if current sector is in the list of sector of this GRO
            for (int i = 0; i < NUM_POSSIBLE_SECTOR; i++)
            {
                if (!pst_GO->pst_Extended)
                {
                    AddStatElement(pst_GO);
                }
                else
                {                    

                    if (pst_GO->pst_Extended->auc_Sectos[i] == 0)
                        nNumNullSector++;

                    // Case of Current Sector
                    if (m_eSectorisation == eSectorisation_ThisSector)
                    {
                        if (pst_GO->pst_Extended)
                        {
                            if (pst_GO->pst_Extended->auc_Sectos[i] == WOR_gpst_CurrentWorld->ul_CurrentSector)
                            {
                                AddStatElement(pst_GO);
                                i = NUM_POSSIBLE_SECTOR;
                            }
                        }
                    }
                    else if (m_eSectorisation == eSectorisation_VisibleSectors)
                    {
                        for (int j = 0; j < WOR_C_MaxSecto; j++)
                        {
                            if (WOR_gpst_CurrentWorld->ast_AllSectos[j].ul_Flags & WOR_CF_SectorVisible)
                            {
                                if (pst_GO->pst_Extended)
                                {
                                    if (pst_GO->pst_Extended->auc_Sectos[i] == j)
                                    {
                                        AddStatElement(pst_GO);
                                        i = NUM_POSSIBLE_SECTOR;
                                        j = WOR_C_MaxSecto;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Or no sector is defined (All sector are NULL)
            if (nNumNullSector == NUM_POSSIBLE_SECTOR)
            {
                AddStatElement(pst_GO);
            }          
        }
    }

    m_iNotItRendering = FALSE;
}

/*
***********************************************************************************************************************
Table preparation for information showing
***********************************************************************************************************************
*/
void XeGOStatistics::SortAndCleanStats()
{
    if (0)
    {
        DumpStatToFile();
    }

    // Processing object 
    if ((m_eSectorisation != eSectorisation_CurrentView) 
        && (m_eObjectType != eSelection_CollisionMesh))
    {
        AddAllObjects();
    }

    if (m_eObjectType == eSelection_CollisionMesh)
    {
        ComputeCollMesh();
    }

    if (m_iOldNbTriangles != m_iNbTriangles || m_iOldNbElements != m_iNbElements)
    {    
        // Sorting and table preparations
        if (m_eObjectType == eSelection_AllGro || m_eObjectType == eSelection_ODE 
            || m_eObjectType == eSelection_CollMap || m_eObjectType == eSelection_SnP || m_eObjectType == eSelection_AO )
        {
            ComputeInstances();

            if (m_eSorting == eSorting_InstancesOnly_Poly || m_eSorting == eSorting_InstancesOnly_Key
                || m_eSorting == eSorting_InstancesOnly_Instances || m_eSorting == eSorting_InstancesOnly_PolyInstances)
            {
                RemoveNonInstances();        
            }
            else if (m_eSorting == eSorting_SingleOnly_Poly || m_eSorting == eSorting_SingleOnly_Key)
            {
                RemoveInstances();  
            }    

            RemoveHoles();
            SortStats();
            
        }
        
        ShowInformationOnConsole(); 
    }

    m_iOldNbTriangles = m_iNbTriangles;
    m_iOldNbElements = m_iNbElements;
}

/*
***********************************************************************************************************************
Display output informtion on contole
***********************************************************************************************************************
*/
void XeGOStatistics::ShowInformationOnConsole()
{
#if defined(VIDEOCONSOLE_ENABLE)
    Desktop::TWChar temp[80];

    VideoConsole_ClearStatistics();

    swprintf(temp, L"Current Sector : %d\n",WOR_gpst_CurrentWorld->ul_CurrentSector);
    VideoConsole_AddStatistics(temp);
	//VideoConsole_choucroutte();
    
    if (m_iNbTriangles == 0 || m_eObjectType == eSelection_None)
    {
        swprintf(temp, L"No elements for current selection",m_iNbTriangles,m_iNbElements);
        VideoConsole_AddStatistics(temp);
    }
    else
    {    
        if (m_eObjectType == eSelection_AllGro)
        {
            swprintf(temp, L"All Gro --> %d Poly in %d object\n",m_iNbTriangles,m_iNbElements);
            VideoConsole_AddStatistics(temp);
        }
        else if (m_eObjectType == eSelection_ODE)
        {
            swprintf(temp, L"ODE --> %d Poly in %d object\n",m_iNbTriangles,m_iNbElements);
            VideoConsole_AddStatistics(temp);
        }
        else if (m_eObjectType == eSelection_CollMap)
        {
            swprintf(temp, L"Map --> %d Poly in %d object\n",m_iNbTriangles,m_iNbElements);
            VideoConsole_AddStatistics(temp);
        }
        else if (m_eObjectType == eSelection_SnP)
        {
            swprintf(temp, L"SnP --> %d Poly in %d object\n",m_iNbTriangles,m_iNbElements);
            VideoConsole_AddStatistics(temp);
        }
        else if (m_eObjectType == eSelection_AO)
        {
            swprintf(temp, L"Animated Objects --> %d Poly in %d object\n",m_iNbTriangles,m_iNbElements);
            VideoConsole_AddStatistics(temp);
        }
        else if (m_eObjectType == eSelection_CollisionMesh)
        {
            swprintf(temp, L"Coll Mesh --> %d Poly in the collision mesh\n",m_iNbTriangles);
            VideoConsole_AddStatistics(temp);
        }

        if (m_eObjectType != eSelection_CollisionMesh)
        {            
            for (int j = 0; j < m_iNbElements; j++)
            {
                if (m_ulGroKCI[eTableIndex_Key][j] == 0) 
                    j = m_iNbElements;
                else
                {            
                    swprintf(temp, L"Key: %X - %d Polys - %d Instances\n", 
                        m_ulGroKCI[eTableIndex_Key][j],
                        m_ulGroKCI[eTableIndex_CountPoly][j],
                        m_ulGroKCI[eTableIndex_Instances][j]);        
                    VideoConsole_AddStatistics(temp);
                }
            }
        }
    }
#endif
}

/*
***********************************************************************************************************************
Compute the number of instances of each objects
***********************************************************************************************************************
*/
void XeGOStatistics::ComputeInstances()
{
    for (int i = 0; i < m_iNbElements - 1; i++)
    {
        for (int j = i + 1; j < m_iNbElements; j++)
        {            
            if (m_ulGroKCI[eTableIndex_Key][i] == m_ulGroKCI[eTableIndex_Key][j]
                && m_ulGroKCI[eTableIndex_Key][i] != 0)
            {
                if (m_ulGroKCI[eTableIndex_CountPoly][i] == m_ulGroKCI[eTableIndex_CountPoly][j])
                {
                    m_ulGroKCI[eTableIndex_Instances][i] += m_ulGroKCI[eTableIndex_Instances][j];
                }
                else
                {
                    m_ulGroKCI[eTableIndex_CountPoly][i] += m_ulGroKCI[eTableIndex_CountPoly][j];
                }
                
                m_ulGroKCI[eTableIndex_Key][j] = 0;
                m_ulGroKCI[eTableIndex_CountPoly][j] = 0;
                m_ulGroKCI[eTableIndex_Instances][j] = 0;
            }                        
        }
    }
}

/*
***********************************************************************************************************************
Remove unique objects 
***********************************************************************************************************************
*/
void XeGOStatistics::RemoveNonInstances()
{
    for (int i = 0 ; i < m_iNbElements; i++)
    {
        if (m_ulGroKCI[eTableIndex_Instances][i] == 1)
        {
            m_ulGroKCI[eTableIndex_Key][i] = 0;
            m_ulGroKCI[eTableIndex_CountPoly][i] = 0;
            m_ulGroKCI[eTableIndex_Instances][i] = 0;            
        }
    }
}

/*
***********************************************************************************************************************
Remove non unique objects
***********************************************************************************************************************
*/
void XeGOStatistics::RemoveInstances()
{
    for (int i = 0 ; i < m_iNbElements; i++)
    {
        if (m_ulGroKCI[eTableIndex_Instances][i] != 1)
        {
            m_ulGroKCI[eTableIndex_Key][i] = 0;
            m_ulGroKCI[eTableIndex_CountPoly][i] = 0;
            m_ulGroKCI[eTableIndex_Instances][i] = 0;            
        }
    }    
}

/*
***********************************************************************************************************************
Parse result table to remove 0 0 0 line
***********************************************************************************************************************
*/
void XeGOStatistics::RemoveHoles()
{
    // Remove holes (place them at the end)
    // Selection Sort
    for (int i = 0; i < (m_iNbElements - 1); i++)
    {
        int maxIndex = i;
        ULONG tempValue;

        // Find the index of the maximum element
        for (int j = i + 1; j < m_iNbElements; j++)
        {
            if (m_ulGroKCI[eTableIndex_Key][j] > m_ulGroKCI[eTableIndex_Key][maxIndex])
            {
                maxIndex = j;
            }
        }

        // Swap if i-th element not already largest
        if (maxIndex > i) 
        {
            for (int j = eTableIndex_Key; j < eTableIndex_Count; j++)
            {
                tempValue = m_ulGroKCI[j][i];
                m_ulGroKCI[j][i] = m_ulGroKCI[j][maxIndex];               
                m_ulGroKCI[j][maxIndex] = tempValue;
            }            
        }
    }
}

/*
***********************************************************************************************************************
Write all statistics to a file
***********************************************************************************************************************
*/
void XeGOStatistics::DumpStatToFile()
{
    HANDLE StatFile;
    DWORD	dwNbBytesWritten;
    TAB_tdst_PFelem	*pst_CurrentElem, *pst_EndElem;
    MATH_tdst_Vector CameraPosition;
    eSectorisation SectorisationOldValue;
    eSorting SortingOldValue;
    eSelection SelectionOldValue;
    char temp[512];  
    char GenericFileName[512];  

    CameraPosition = WOR_gpst_CurrentWorld->pst_CurrentView->st_ViewPoint.T;

    CreateDirectory("D:\\GEOStats",NULL);

    sprintf(temp, "D:\\GEOStats\\");
    GetGenericFileName(GenericFileName);
    strcat(temp,GenericFileName);
    strcat(temp,".txt");

    StatFile = CreateFile( temp, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
   
    sprintf(temp, "Xenon Statistics\n\nCurrent application version (Jade) : %i\n\n",BIG_Cu4_AppVersion);
    WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );

    sprintf(temp, "Current Sector : %d\n",WOR_gpst_CurrentWorld->ul_CurrentSector);
    WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );

    sprintf(temp, "Current Frame Rate (Curr,min,max,avg): (%.2f,%.2f,%.2f,%.2f)\n",
        g_oXeRenderer.GetFPS(), g_oXeRenderer.GetMinFPS(),g_oXeRenderer.GetMaxFPS(),g_oXeRenderer.GetAvgFPS());
    WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );

    sprintf(temp, "Current camera position : (%.2f,%.2f,%.2f,%.2f)\n",
        CameraPosition.x, CameraPosition.y, CameraPosition.z);
    WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );

    sprintf(temp, "Current View Statistics\n");
    WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );

    // Rendering Information
    sprintf(temp,"\n\n============ RENDERED OBJECTS =============\n\n");
    WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );

    for (int i = 0; i < m_iNbGRO; i++)
    {
        sprintf(temp, "Key : %X\n ",m_ulGROList[i]);
        WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );     
    }

    // Common configuration :
    SectorisationOldValue = m_eSectorisation;
    SortingOldValue = m_eSorting;
    SelectionOldValue = m_eObjectType;

    m_eSectorisation = eSectorisation_CurrentView;
    m_eSorting = eSorting_Poly;
    m_eObjectType = eSelection_AllGro;  

    InitStats();
    PrepareStatTable();
    sprintf(temp, "\n[GETPOLYCOUNTMAPTOTALWOACTORS] All Gro --> %d Poly \n[GETMODELCOUNTMAPTOTAL] All Gro --> %d object\n",m_iNbTriangles,m_iNbElements);
    WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );
    PrintStatTableToFile(StatFile);

    m_eObjectType = eSelection_ODE;  

    InitStats();      
    PrepareStatTable();
    sprintf(temp, "\n[GETPOLYCOUNTODE] ODE --> %d Poly \n[GETMODELCOUNTODE] ODE --> %d object\n",m_iNbTriangles,m_iNbElements);
    WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );
    PrintStatTableToFile(StatFile);

    m_eObjectType = eSelection_CollMap;   

    InitStats();     
    PrepareStatTable();
    sprintf(temp, "\n[GETPOLYCOUNTLANDSCAPE] Map --> %d Poly \n[GETMODELCOUNTLANDSCAPE] Map --> %d object\n",m_iNbTriangles,m_iNbElements);
    WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );
    PrintStatTableToFile(StatFile);

    m_eObjectType = eSelection_SnP;    

    InitStats();    
    PrepareStatTable();
    sprintf(temp, "\n[GETPOLYCOUNTSNP] SnP --> %d Poly \n[GETMODELCOUNTSNP] SnP --> %d object\n",m_iNbTriangles,m_iNbElements);
    WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );
    PrintStatTableToFile(StatFile);

    m_eObjectType = eSelection_AO;   

    InitStats();
    PrepareStatTable();
    sprintf(temp, "\n[GETPOLYCOUNTALLACTORS] Animated Objects --> %d Poly \n",m_iNbTriangles);
    WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );
    PrintStatTableToFile(StatFile);

    m_eObjectType = eSelection_CollisionMesh;   

    InitStats();
    ComputeCollMesh();
    sprintf(temp, "\n[GETPOLYCOUNTCOLMAP] Coll Mesh --> %d Poly in the collision mesh\n",m_iNbTriangles);
    WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );

    // Dump all objects informations
    // Output a table of all GRO
    sprintf(temp,"\n\n============ ALL GRO TABLE (EXCEL FORMAT)  =============\n");
    WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );

    sprintf(temp,"Key, Obj, Triangle, Bone, Anims, Dyna, SharedMatrix, Lights,AI, DesignStruct, Waypoints, ColMap, ZDM, ZDE, Unused, BaseObject, ExtendedObject,Visu, Msg, HasInitialPos, Generated, Links, OBBox, DesignHelper, AdditionalMatrix,Hierarchy, Group, AddMatArePointer, Events, FlashMatrix, Sound, ODE\n");    WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );
        
    
    pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&WOR_gpst_CurrentWorld->st_AllWorldObjects);
    pst_EndElem = TAB_pst_PFtable_GetLastElem(&WOR_gpst_CurrentWorld->st_AllWorldObjects);

    for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
    {
        GEO_tdst_Object* pst_Obj;
        OBJ_tdst_GameObject* pst_GO;

        pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
        if(TAB_b_IsAHole(pst_GO) || !(pst_GO->ul_MyKey)) continue;

        pst_Obj = (GEO_tdst_Object *) OBJ_p_GetGro(pst_GO);
        if(pst_Obj)
        {
            if (pst_Obj->st_Id.i->ul_Type == GRO_Geometric)
            {
                int uiNumTriangles = ComputeNumTriangles(pst_GO);

                sprintf(temp, "%X,%X,%i,", pst_GO->ul_MyKey,pst_Obj,uiNumTriangles);	                

                for (int i = OBJ_C_IdentityFlag_Bone; i <= OBJ_C_IdentityFlag_ODE; i *= 2)
                {
                    if (pst_GO->ul_IdentityFlags & i)
                    {
                        strcat(temp, "x");		
                    }
                    strcat(temp, ",");
                }                	
                strcat(temp, "\n");		

                WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );

            }
        }
    }                       


    // Dump sectorisation informations
    sprintf(temp,"\n\n============ SECTORS INFORMATIONS =============");
    WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );

    for (int i = 1; i < WOR_C_MaxSecto; i++)
    {
        sprintf(temp, "\n\nSector : %i\nList of visible sectors : ",i);
        WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );

        for (int j = 0; j < WOR_C_MaxSectoRef; j++)
        {
            if (WOR_gpst_CurrentWorld->ast_AllSectos[i].auc_RefVis[j])
            {
                sprintf(temp,"%i ", WOR_gpst_CurrentWorld->ast_AllSectos[i].auc_RefVis[j]);
                WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );
            }
        }        

        sprintf(temp,"\nList of active sectors : " );
        WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );
        for (int j = 0; j < WOR_C_MaxSectoRef; j++)
        {
            if (WOR_gpst_CurrentWorld->ast_AllSectos[i].auc_RefAct[j])
            {
                sprintf(temp,"%i ", WOR_gpst_CurrentWorld->ast_AllSectos[i].auc_RefAct[j]);
                WriteFile(StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );                
            }
        }        
    }

    m_eSectorisation = SectorisationOldValue;
    m_eSorting = SortingOldValue;
    m_eObjectType = SelectionOldValue;

    CloseHandle( StatFile );  
}

/*
***********************************************************************************************************************
Functions wrapper : This function compute all object, compute instance and prepare results
***********************************************************************************************************************
*/
void XeGOStatistics::PrepareStatTable()
{
    AddAllObjects();
    ComputeInstances();
    RemoveHoles();
    SortStats();
}

/*
***********************************************************************************************************************
Sorting
***********************************************************************************************************************
*/
void XeGOStatistics::SortStats()
{
    // Selection Sort
    for (int i = 0; i < (m_iNbElements - 1); i++)
    {
        int maxIndex = i;
        long tempValue;

        // Find the index of the maximum element
        for (int j = i + 1; j < m_iNbElements; j++)
        {
            if (m_ulGroKCI[eTableIndex_Key][j] == 0)
            {
                j = m_iNbElements;
            }
            else
            {
                if (m_eSorting == eSorting_Key || m_eSorting == eSorting_InstancesOnly_Key || m_eSorting == eSorting_SingleOnly_Key)
                {
                    if (m_ulGroKCI[eTableIndex_Key][j] < m_ulGroKCI[eTableIndex_Key][maxIndex])                
                        maxIndex = j;                
                }
                else if(m_eSorting == eSorting_Poly || m_eSorting == eSorting_InstancesOnly_Poly || m_eSorting == eSorting_SingleOnly_Poly)
                {
                    if (m_ulGroKCI[eTableIndex_CountPoly][j] > m_ulGroKCI[eTableIndex_CountPoly][maxIndex])
                        maxIndex = j;
                }
                else if (m_eSorting == eSorting_Instances || m_eSorting == eSorting_InstancesOnly_Instances)
                {            
                    if (m_ulGroKCI[eTableIndex_Instances][j] > m_ulGroKCI[eTableIndex_Instances][maxIndex])
                        maxIndex = j;
                }
                else if (m_eSorting == eSorting_PolyInstances || m_eSorting == eSorting_InstancesOnly_PolyInstances)
                {            
                    if ((m_ulGroKCI[eTableIndex_CountPoly][j] * m_ulGroKCI[eTableIndex_Instances][j]) > (m_ulGroKCI[eTableIndex_CountPoly][maxIndex] * m_ulGroKCI[eTableIndex_Instances][maxIndex]))
                        maxIndex = j;
                }
            }
        }

        // Swap if i-th element not already largest
        if (maxIndex > i) 
        {
            for (int j = eTableIndex_Key; j < eTableIndex_Count; j++)
            {
                tempValue = m_ulGroKCI[j][i];
                m_ulGroKCI[j][i] = m_ulGroKCI[j][maxIndex];
                m_ulGroKCI[j][maxIndex] = tempValue;
            }
        }

        if (m_ulGroKCI[eTableIndex_Key][i] == 0)
            i = m_iNbElements;
    }
}

/*
***********************************************************************************************************************
Print the m_ulGroKCI table to file
***********************************************************************************************************************
*/
void XeGOStatistics::PrintStatTableToFile(HANDLE a_StatFile)
{
    char temp[64];
    DWORD	dwNbBytesWritten;
    ULONG   ulRenderedTriangles = 0;

    for (int j = 0; j < m_iNbElements; j++)
    {
        if (m_ulGroKCI[eTableIndex_Key][j] == 0) 
            j = m_iNbElements;
        else
        {            
            for (int i = 0; i < m_iNbGRO; i++)
            {
                if (m_ulGroKCI[eTableIndex_Key][j] == m_ulGROList[i])
                {
                    sprintf(temp, "*");        
                    WriteFile(a_StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );
                    if (m_eObjectType != eSelection_AO)
                    {
                        ulRenderedTriangles += m_ulGroKCI[eTableIndex_CountPoly][j] * m_ulGroKCI[eTableIndex_Instances][j];
                    }
                    else
                    {
                        ulRenderedTriangles += m_ulGroKCI[eTableIndex_CountPoly][j];
                    }
                    i = m_iNbGRO;
                }
            }
            if (m_eObjectType != eSelection_AO)
            {
                sprintf(temp, "Key: %X - %d Polys - %d Instances\n", 
                    m_ulGroKCI[eTableIndex_Key][j],
                    m_ulGroKCI[eTableIndex_CountPoly][j],
                    m_ulGroKCI[eTableIndex_Instances][j]);        
            }
            else
            {
                sprintf(temp, "Key: %X - %d Polys\n", 
                    m_ulGroKCI[eTableIndex_Key][j],
                    m_ulGroKCI[eTableIndex_CountPoly][j]);        
            }

            WriteFile(a_StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );
        }
    }

    sprintf(temp, "\nTotal Rendered Triangles: %d Polys\n",ulRenderedTriangles);        
    WriteFile(a_StatFile, &temp, strlen(temp), &dwNbBytesWritten, NULL );
}

/*
***********************************************************************************************************************
Compute the number of triangle of an object
***********************************************************************************************************************
*/
ULONG XeGOStatistics::ComputeNumTriangles(OBJ_tdst_GameObject* _pst_GO, XeMesh *_poMesh)
{
    XeIndexBuffer   *poIndices;
    XeBuffer        *poVertexBuffer;    

    poIndices = _poMesh->GetIndices();

    if (poIndices != NULL)
    {
        return poIndices->GetFaceCount();
    }
    else
    {
        poVertexBuffer = _poMesh->GetStream(0)->pBuffer;
        return poVertexBuffer->GetVertexCount() / 3;
    }
}

/*
***********************************************************************************************************************
Compute the number of triangle of an object
***********************************************************************************************************************
*/
ULONG XeGOStatistics::ComputeNumTriangles(OBJ_tdst_GameObject* _pst_GO)
{
    ULONG                               ulNumTriangles;
    GRO_tdst_Visu						*pst_Visu;
    GEO_tdst_Object						*pst_Obj;
    GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;

    ulNumTriangles = 0;

    pst_Visu = _pst_GO->pst_Base->pst_Visu;
    pst_Obj = (GEO_tdst_Object *) pst_Visu->pst_Object;

    if ( !pst_Obj ) return 0;
    if ( !pst_Obj->dst_Point) return 0;
    if ( pst_Visu->ucFlag & GRO_VISU_FLAG_WATERFX) return 0;

    // send graphic elements
    pst_Element = pst_Obj->dst_Element;
    pst_LastElement = pst_Element + pst_Obj->l_NbElements;

    // for objects using the temporary geometric, create dynamic meshes
    MAT_tdst_Material* pst_Material = (MAT_tdst_Material *)pst_Visu->pst_Material;

    for(int iElem=0; pst_Element < pst_LastElement; pst_Element++, iElem++)
    {
        if (pst_Visu->pst_Material != NULL)
        {
            // Reset the material pointer so that we'll be able to fetch the right layer
            pst_Material = (MAT_tdst_Material *)pst_Visu->pst_Material;
        }

        if(pst_Visu->p_XeElements && pst_Visu->p_XeElements[iElem].pst_Mesh && pst_Material && ((pst_Material->st_Id.i->ul_Type == GRO_MaterialMulti) || (pst_Material->st_Id.i->ul_Type == GRO_MaterialMultiTexture)))
        {
            XeMesh* poMesh = pst_Visu->p_XeElements[iElem].pst_Mesh;

            if (pst_Material->st_Id.i->ul_Type == GRO_MaterialMulti)
            {
                if(((MAT_tdst_Multi *)pst_Material)->l_NumberOfSubMaterials == 0)
                    pst_Material = NULL;
                else
                    pst_Material = ((MAT_tdst_Multi *)pst_Material)->dpst_SubMaterial[lMin(pst_Element->l_MaterialId , ((MAT_tdst_Multi *)pst_Material)->l_NumberOfSubMaterials - 1)];
            }

            MAT_tdst_MultiTexture	*pst_MLTTX = (MAT_tdst_MultiTexture *) pst_Material;
            MAT_tdst_MTLevel		*pst_MLTTXLVL = pst_MLTTX->pst_FirstLevel;

            if(pst_Material == NULL) pst_Material= (MAT_tdst_Material *) &MAT_gst_DefaultSingleMaterial;

            if (pst_MLTTXLVL != NULL)
            {
                // for all material layers
                while (pst_MLTTXLVL && (LONG)pst_MLTTXLVL != -1)
                {
                    if(!(pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_InActive))
                    {
                        // ADD OBJECT
                        ulNumTriangles += ComputeNumTriangles(_pst_GO,poMesh);
                    }

                    pst_MLTTXLVL = pst_MLTTXLVL->pst_NextLevel;
                }
            }
        }
    }

    return ulNumTriangles;
}

/*
***********************************************************************************************************************
This function trigger the dump stat after a certain number of frame
***********************************************************************************************************************
*/
void XeGOStatistics::TriggerDumpStat()
{
    m_iTriggerDumpStat = TRUE;
}

/*
***********************************************************************************************************************
Get The number Of Poly in the view
***********************************************************************************************************************
*/
int XeGOStatistics::GetCurrentViewNumPoly()
{
    return m_iNbRenderedTriangles;
}

/*
***********************************************************************************************************************
This function return the value of the trigger to dump stat 
***********************************************************************************************************************
*/
 int XeGOStatistics::GetTriggerDumpStat()
 {
     return m_iTriggerDumpStat;
 }

#endif // XENONVIDEOSTATISTICS
