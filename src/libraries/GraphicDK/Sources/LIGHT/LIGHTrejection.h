/*$T LIGHTrejection.h GC!1.52 07/04/99 10:35:14 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#endif

#ifndef __LIGHTREJECTION_H__
#define __LIGHTREJECTION_H__


#include "BASe/BAStypes.h"
#include "Engine\Sources\WORld\WORstruct.h"
#include <set>
#include <map>
#include <vector>

// --------------------------------------------------------------------------------------------
// Light Rejection List management structures -------------------------------------------------

// -NOTE- Used to store a pair of object (source object and its duplicate)
struct OBJ_tdst_GAOSrcDuplicate
{
	OBJ_tdst_GAOSrcDuplicate(OBJ_tdst_GameObject* pObjectSrc=NULL, OBJ_tdst_GameObject* pObjectDuplicate=NULL):
	m_pObjectSrc(pObjectSrc),
	m_pObjectDuplicate(pObjectDuplicate)
	{
	}

	OBJ_tdst_GameObject* m_pObjectSrc;
	OBJ_tdst_GameObject* m_pObjectDuplicate;
};

typedef std::set< OBJ_tdst_GameObject * >                     RejectedLights;
typedef std::map< OBJ_tdst_GameObject *, RejectedLights * >   LightRejectionList;

// -NOTE- Describes what action is occurring to know how to insert
// duplicate GAOs in light rejection list.
enum eLRLAction
{
	eLRLNone,
	eLRLLoading,
	eLRLSpawing 
};

class LightRejectionManager
{
    //----------------------------------
    // Types
    //----------------------------------
public:

    typedef struct ObjectLoadRefs
    {
        OBJ_tdst_GameObject *       m_object;
        ULONG                       m_ulNbrLights;
        OBJ_tdst_GameObject **      m_lights;
    };

    //----------------------------------
    // Construction / Destruction
    //----------------------------------
    LightRejectionManager                       ( );

    //----------------------------------
    // Data acess 
    //----------------------------------
    ULONG                   GetObjectCount      ( );
    RejectedLights *        GetRejectionList    ( OBJ_tdst_GameObject *pObject ); 
    RejectedLights *        GetRejectionList    ( ULONG _index );
    OBJ_tdst_GameObject *   GetObject           ( ULONG _index );
    BOOL                    IsDirty             ( ) { return m_ulNbrObjectsLoad != 0; }

    //----------------------------------
    // Operations
    //----------------------------------
    void                    Add                 ( OBJ_tdst_GameObject *pLight, OBJ_tdst_GameObject *pObject );
    void                    Add                 ( BIG_KEY ulLightKey, BIG_KEY ulObjectKey );
	void                    Remove              ( OBJ_tdst_GameObject *pLight, OBJ_tdst_GameObject *pObject );
	void                    Clear               ( );
	void					Resolve				( );

	void					UpdateLRLWithGAODuplicate	( OBJ_tdst_GameObject *pObjectSrc, OBJ_tdst_GameObject *pObjectDuplicate, eLRLAction LRLAction );

#ifdef ACTIVE_EDITORS
	BOOL					DuplicateList		( OBJ_tdst_GameObject *pSrcObject, OBJ_tdst_GameObject *pDstObject );
	void					SpecialResolve		( std::map< BIG_KEY, BIG_KEY >& _MapSrcKey_GAOKey );
#endif

    void                    SaveAll             ( );
    void                    LoadAll             ( char * & pc_Buf );

	
    //----------------------------------
    // Helpers
    //----------------------------------
private:
    void                    Refresh             ( );
    void                    ClearLoadRefs       ( );

    //----------------------------------
    // Attributes
    //----------------------------------
private:
    LightRejectionList      m_rejections;

    ULONG                   m_ulNbrObjectsLoad;
    ObjectLoadRefs *        m_pObjectLoadRefs;

	std::vector<OBJ_tdst_GAOSrcDuplicate> m_vcGAOSrcDuplicate;
};

extern __declspec(align(32)) LightRejectionManager g_LightRejectionList;

extern ULONG					g_LRL_ul_world_id;

#ifdef ACTIVE_EDITORS
extern BIG_KEY					g_LRL_ul_Key;
#endif

// --------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------
// Light rejection list management functions (implemented in DIAlogs\DIAlightrej_dlg.cpp)

void        LRL_Add( OBJ_tdst_GameObject *pLight, OBJ_tdst_GameObject *pObject );
void        LRL_Add(BIG_KEY ulLightKey, BIG_KEY ulObjectKey );
void        LRL_Rem( OBJ_tdst_GameObject *pLight, OBJ_tdst_GameObject *pObject );

#if defined(ACTIVE_EDITORS)
BOOL		LRL_DuplicateList( OBJ_tdst_GameObject *pSrcObject, OBJ_tdst_GameObject *pDstObject );
#endif

RejectedLights *
GetRejectedLightVector(  OBJ_tdst_GameObject * pObject );

void        LRL_Init(ULONG ul_world_id);
void        LRL_Clean(); 

void        LRL_Load(BIG_KEY _ul_LightRejectionKey);
BIG_KEY     LRL_Save(WOR_tdst_World *_pst_World);
void        LRL_Resolve();

#ifdef ACTIVE_EDITORS
void		LRL_SpecialResolve(std::map< BIG_KEY, BIG_KEY >& _MapSrcKey_GAOKey);
#endif

ULONG       LRL_GetWorldKey();
void		LRL_SetWorldKey(ULONG ul_world_id);

// --------------------------------------------------------------------------------------------

#endif /* __LIGHTREJECTION_H__ */