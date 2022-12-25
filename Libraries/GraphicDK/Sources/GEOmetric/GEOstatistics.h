/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __GEOMETRIC_STATISTICS__
#define __GEOMETRIC_STATISTICS__

#ifdef XENONVIDEOSTATISTICS
// ***********************************************************************************************************************
//    Enums
// ***********************************************************************************************************************
typedef enum
{
    eSelection_None,
    eSelection_AllGro,
    eSelection_ODE,
    eSelection_CollMap,
    eSelection_SnP,
    eSelection_AO,
    eSelection_CollisionMesh,

    eSelection_Count
}
eSelection;

typedef enum
{
    eSectorisation_CurrentView,
    eSectorisation_ThisSector,
    eSectorisation_VisibleSectors,

    eSectorisation_Count
}
eSectorisation;

typedef enum
{
    eSorting_Poly,
    eSorting_Key,
    eSorting_Instances,
    eSorting_PolyInstances,
    eSorting_InstancesOnly_Poly,
    eSorting_InstancesOnly_Key,
    eSorting_InstancesOnly_Instances,
    eSorting_InstancesOnly_PolyInstances,
    eSorting_SingleOnly_Poly,
    eSorting_SingleOnly_Key,    
    eSorting_Count
}
eSorting;

typedef enum
{    
    eTableIndex_Key,
    eTableIndex_CountPoly,
    eTableIndex_Instances,
    eTableIndex_Count
}
eTableIndex;

// Const
static const int MAX_ELEMENT = 2048;
static const int MAX_ELEMENT_SMALL = 512;
static const int NUM_POSSIBLE_SECTOR = 4;

// class used for Xenon Game Objects Statistics
class XeGOStatistics
{
public:
    static XeGOStatistics* Instance();
    void InitStats();
    void SortAndCleanStats();
    void AddStatElement(OBJ_tdst_GameObject*, XeMesh*);
    void AddStatElement(OBJ_tdst_GameObject*);
    void TriggerDumpStat();
    void DumpStatToFile();
    int GetTriggerDumpStat();
    int GetCurrentViewNumPoly();

    ~XeGOStatistics();

protected:
    // ----- methods -----
    XeGOStatistics();
    XeGOStatistics(const XeGOStatistics&);
    XeGOStatistics& operator= (const XeGOStatistics&);
        
private:
    static XeGOStatistics* m_pInstance;

    // ----- attributes -----    
    int         m_iNbRenderedTriangles;
    int         m_iNbTriangles;
    int         m_iNbElements;
    int         m_iNbGRO;
    int         m_iNotItRendering;

    eSelection      m_eObjectType;
    eSectorisation  m_eSectorisation;
    eSorting        m_eSorting;

    int         m_iOldNbTriangles;
    int         m_iOldNbElements;
    int         m_iTriggerDumpStat;

    ULONG       m_ulGroKCI[eTableIndex_Count][MAX_ELEMENT];         // Key Count Instance       
    ULONG       m_ulGROList[MAX_ELEMENT];

    void ShowInformationOnConsole();
    void AddAllObjects();
    void ComputeCollMesh();    
    void ComputeInstances();
    void RemoveNonInstances();
    void RemoveInstances();
    void RemoveHoles();
    void SortStats();
    void PrepareStatTable();
    void PrintStatTableToFile(HANDLE);
    ULONG ComputeNumTriangles(OBJ_tdst_GameObject*,XeMesh*);
    ULONG ComputeNumTriangles(OBJ_tdst_GameObject*);
};
#endif /* XENONVIDEOSTATISTICS */
#endif /* __GEOMETRIC_STATISTICS__ */
