#include "precomp.h"

#include "Light/LIGHTrejection.h"
#include "Res/Res.h"
#include "EDIpaths.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "engine/sources/objects/objload.h"
#include "GraphicDK\Sources\GEOmetric\GEOload.h"
#include "SDK\Sources\LINks\LINKtoed.h"
#include "BIGfiles\BIGread.h"
#include "BIGfiles\SAVing\SAVdefs.h"
#include "BIGfiles\BIGdefs.h"
#include <algorithm>

#define MAX_OBJECT_IN_LRL	10240

GRO_tdst_Visu * OBJ_p_GetVisu( OBJ_tdst_GameObject * pObject )
{
	ERR_X_Assert( pObject && pObject->pst_Base && pObject->pst_Base->pst_Visu );
	return pObject->pst_Base->pst_Visu;
}

struct EqualObject
{
	EqualObject( OBJ_tdst_GameObject * _pGameObject ) : m_pGameObject( _pGameObject ) {}

	bool operator()( const LightRejectionManager::ObjectLoadRefs & _ref )
	{
		return ( _ref.m_object == m_pGameObject );
	}

	OBJ_tdst_GameObject * m_pGameObject;
};

//----------------------------------------------------
// LightRejectionManager::LightRejectionManager
//----------------------------------------------------
LightRejectionManager::LightRejectionManager( ) :
m_ulNbrObjectsLoad  ( 0 ),
m_pObjectLoadRefs   ( NULL )
{
}

//----------------------------------------------------
// LightRejectionManager::Add
//----------------------------------------------------
void 
LightRejectionManager::Add( OBJ_tdst_GameObject *pLight, OBJ_tdst_GameObject *pObject )
{
	if( !pLight || !pObject ) return;

	// SC: Make sure the object is valid
	if (!OBJ_b_TestIdentityFlag(pObject, OBJ_C_IdentityFlag_Visu))
		return;

	// See if rejected light vector exists for object
	LightRejectionList::iterator itLRL = m_rejections.find( pObject );
	if( itLRL == m_rejections.end() )
	{
		// Create a new entry
		RejectedLights * pRejectedLights = new RejectedLights;
		itLRL = m_rejections.insert( LightRejectionList::value_type( pObject, pRejectedLights ) ).first;
	}

	ERR_X_Assert( itLRL != m_rejections.end() );

	// Add light to rejection list
	(itLRL->second)->insert( pLight );

	// Set rejected light list in object
	OBJ_p_GetVisu( pObject )->pRejectedLights = itLRL->second; 
}

//----------------------------------------------------
// LightRejectionManager::Add
//----------------------------------------------------
void 
LightRejectionManager::Add( BIG_KEY ulLightKey, BIG_KEY ulObjectKey  )
{
	// Note: This is only called with version 1 loading

	// Both object of the relation must exist or else we skip this entry
	BIG_INDEX ul_PosLight = BIG_ul_SearchKeyToPos(ulLightKey);
	if(ul_PosLight==BIG_C_InvalidIndex)
		return;	

	BIG_INDEX ul_PosObject = BIG_ul_SearchKeyToPos(ulObjectKey);
	if(ul_PosObject==BIG_C_InvalidIndex)
		return;	

	// Find an entry for the object
	ObjectLoadRefs * pObjectLoadRefs = NULL;

	if( m_ulNbrObjectsLoad > 0 )
	{
		pObjectLoadRefs = std::find_if( m_pObjectLoadRefs, m_pObjectLoadRefs + m_ulNbrObjectsLoad, EqualObject((OBJ_tdst_GameObject *)ul_PosObject) );
		if( pObjectLoadRefs == m_pObjectLoadRefs + m_ulNbrObjectsLoad )
		{
			pObjectLoadRefs = NULL;
		}
	}

	if( pObjectLoadRefs == NULL )
	{
		// Append new object
		if( m_ulNbrObjectsLoad == 0 )
		{
			m_pObjectLoadRefs =  (ObjectLoadRefs *) MEM_p_Alloc( sizeof(ObjectLoadRefs) );
		}
		else
		{
			m_pObjectLoadRefs =  (ObjectLoadRefs *) MEM_p_Realloc( m_pObjectLoadRefs, (m_ulNbrObjectsLoad+1) * sizeof(ObjectLoadRefs) );
		}

		pObjectLoadRefs = m_pObjectLoadRefs + m_ulNbrObjectsLoad;
		pObjectLoadRefs->m_object = (OBJ_tdst_GameObject *)ul_PosObject;
		pObjectLoadRefs->m_lights = NULL;
		pObjectLoadRefs->m_ulNbrLights = 0;

		++m_ulNbrObjectsLoad;
	}

	ERR_X_Assert( pObjectLoadRefs != NULL );

	// Append new light
	if( pObjectLoadRefs->m_ulNbrLights == 0 )
	{
		pObjectLoadRefs->m_lights = (OBJ_tdst_GameObject **) MEM_p_Alloc( sizeof(OBJ_tdst_GameObject *) );
	}
	else
	{
		pObjectLoadRefs->m_lights = (OBJ_tdst_GameObject **) MEM_p_Realloc( pObjectLoadRefs->m_lights, (pObjectLoadRefs->m_ulNbrLights+1) * sizeof(OBJ_tdst_GameObject *) );
	}

	pObjectLoadRefs->m_lights[pObjectLoadRefs->m_ulNbrLights] = (OBJ_tdst_GameObject *)ul_PosLight;
	pObjectLoadRefs->m_ulNbrLights++;
}

//----------------------------------------------------
// LightRejectionManager::Remove
//----------------------------------------------------
void       
LightRejectionManager::Remove( OBJ_tdst_GameObject *pLight, OBJ_tdst_GameObject *pObject )
{
	// See if rejected light vector exists for object
	LightRejectionList::iterator itLRL = m_rejections.find( pObject );
	if( itLRL != m_rejections.end() )
	{
		(itLRL->second)->erase( pLight );
	}
}

//----------------------------------------------------
// LightRejectionManager::Refresh
//----------------------------------------------------
void         
LightRejectionManager::Refresh( )
{
	if( m_ulNbrObjectsLoad == 0 ) 
		return;

	ObjectLoadRefs * pObjectLoadRef = m_pObjectLoadRefs;
	ERR_X_Assert( pObjectLoadRef );

	for( ULONG i=0; i<m_ulNbrObjectsLoad; ++i, ++pObjectLoadRef )
	{
		// Object must exist and have visual
		if( (pObjectLoadRef && (pObjectLoadRef->m_object == (OBJ_tdst_GameObject *)0xFFFFFFFF)) || !OBJ_b_TestIdentityFlag(pObjectLoadRef->m_object, OBJ_C_IdentityFlag_Visu))
			continue;

		// Find set for object
		LightRejectionList::iterator itLRL = m_rejections.find( pObjectLoadRef->m_object );
		if( itLRL == m_rejections.end() )
		{
			// Create a new entry
			RejectedLights * pRejectedLights = new RejectedLights;
			itLRL = m_rejections.insert( LightRejectionList::value_type( pObjectLoadRef->m_object, pRejectedLights ) ).first;
		}

		ERR_X_Assert( itLRL != m_rejections.end() );
		RejectedLights * pRejectedLights = itLRL->second;

		// Insert every light
		for( ULONG j=0; j<pObjectLoadRef->m_ulNbrLights; ++j )
		{
			if( (pObjectLoadRef->m_lights[j] != (OBJ_tdst_GameObject *)0xFFFFFFFF) )
			{
				pRejectedLights->insert( pObjectLoadRef->m_lights[j] );
			}
		}

		OBJ_p_GetVisu( pObjectLoadRef->m_object )->pRejectedLights = itLRL->second; 
	}

	// For each object in vector, check if GAO Src exists in rejection list. If it does
	// then give to the duplicate the corresponding rejected lights
	OBJ_tdst_GameObject *pObjectSrc = NULL ;
	OBJ_tdst_GameObject *pObjectDuplicate = NULL ;
	
	std::vector<OBJ_tdst_GAOSrcDuplicate>::iterator itGAOSrcDuplicate = m_vcGAOSrcDuplicate.begin();
	for( ; itGAOSrcDuplicate != m_vcGAOSrcDuplicate.end(); ++itGAOSrcDuplicate )
	{
		// Search for GAO src in rejection list	
		LightRejectionList::iterator itLRL = m_rejections.find((*itGAOSrcDuplicate).m_pObjectSrc);
		if ( itLRL != m_rejections.end() )
		{
			pObjectSrc = (*itGAOSrcDuplicate).m_pObjectSrc;
			pObjectDuplicate = (*itGAOSrcDuplicate).m_pObjectDuplicate;

			if ( pObjectSrc && pObjectDuplicate )
			{
				if ( (OBJ_b_TestIdentityFlag(pObjectSrc, OBJ_C_IdentityFlag_Visu) && pObjectSrc->pst_Base && pObjectSrc->pst_Base->pst_Visu) &&
				  	 (OBJ_b_TestIdentityFlag(pObjectDuplicate, OBJ_C_IdentityFlag_Visu) && pObjectDuplicate->pst_Base && pObjectDuplicate->pst_Base->pst_Visu) )
				{
					OBJ_p_GetVisu(pObjectDuplicate)->pRejectedLights = OBJ_p_GetVisu(pObjectSrc)->pRejectedLights;
				}
			}
		}
	}
	m_vcGAOSrcDuplicate.clear();
	
	ClearLoadRefs();
}

//----------------------------------------------------
// LightRejectionManager::LoadAll
//----------------------------------------------------
void         
LightRejectionManager::LoadAll( char * & pc_Buf )
{
	m_pObjectLoadRefs = NULL;

	// Read the number of objects
	m_ulNbrObjectsLoad = LOA_ReadULong(&pc_Buf);

	// Check validity of data
	if( m_ulNbrObjectsLoad == 0 )
		return;
	if( m_ulNbrObjectsLoad > MAX_OBJECT_IN_LRL)
	{
		ERR_X_Warning(0, ERR_szFormatMessage("LRLLoad: There is a bizarre amount of objects (%d) in LRL. It's probably corrupted. Skipping its loading.", m_ulNbrObjectsLoad), NULL);
		return;
	}

	// Allocate for all objects
	m_pObjectLoadRefs = (ObjectLoadRefs *) MEM_p_Alloc( m_ulNbrObjectsLoad * sizeof(ObjectLoadRefs) );
	memset( m_pObjectLoadRefs, 0, m_ulNbrObjectsLoad * sizeof(ObjectLoadRefs) );

	// Read every object
	ObjectLoadRefs * pObjectLoadRef = m_pObjectLoadRefs;
	for( ULONG i=0; i<m_ulNbrObjectsLoad; ++i, ++pObjectLoadRef )
	{
		// Read object 
		BIG_KEY ul_ObjectKey = LOA_ReadULong(&pc_Buf);
		pObjectLoadRef->m_object = (OBJ_tdst_GameObject*) BIG_ul_SearchKeyToPos(ul_ObjectKey);
		
		// Read the number of lights
		pObjectLoadRef->m_ulNbrLights = LOA_ReadULong(&pc_Buf);

		// Check validity of data
		if( pObjectLoadRef->m_ulNbrLights == 0 )
			continue;
		if( pObjectLoadRef->m_ulNbrLights > MAX_OBJECT_IN_LRL)
		{
			// reinit object load ref so that it doens't crash the resolve
			memset(pObjectLoadRef, 0, sizeof(ObjectLoadRefs));
			ERR_X_Warning(0, ERR_szFormatMessage("LRLLoad: There is a bizarre amount of lights (%d) in LRL. It's probably corrupted. Skipping its loading.", pObjectLoadRef->m_ulNbrLights), NULL);
			return;
		}

		// Allocate for all lights
		pObjectLoadRef->m_lights = (OBJ_tdst_GameObject**) MEM_p_Alloc( pObjectLoadRef->m_ulNbrLights * sizeof(OBJ_tdst_GameObject*) );
		memset( pObjectLoadRef->m_lights, 0, pObjectLoadRef->m_ulNbrLights * sizeof(BIG_INDEX) );

		// Read all the lights
		for( ULONG j=0; j<pObjectLoadRef->m_ulNbrLights; ++j )
		{
			BIG_KEY ul_LightKey = LOA_ReadULong(&pc_Buf);
			pObjectLoadRef->m_lights[j] =  (OBJ_tdst_GameObject*) BIG_ul_SearchKeyToPos(ul_LightKey);
		}
	}
}

#ifdef ACTIVE_EDITORS
//----------------------------------------------------
// LightRejectionManager::SaveAll
//----------------------------------------------------
void         
LightRejectionManager::SaveAll( )
{
	Refresh( );

	ULONG ul_Key;
	UINT ui_NbObjects = 0;
	LightRejectionList::iterator itLRL;

	// Count the objects that are instances and that do not exists physically
	// in BF (ex: instances of actors). We don't want to save those objects.
	itLRL = m_rejections.begin( );
	for( ; itLRL != m_rejections.end(); ++itLRL )
	{
		OBJ_tdst_GameObject *pObject = itLRL->first;
		
		if ( ! (pObject && pObject->pst_Base && pObject->pst_Base->pst_Visu) )
			continue;

		// Skip counting objects that are instances, that do not physically exists in BF 
		ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pObject);
		if ( ul_Key == BIG_C_InvalidKey)
			continue;
		if ( OBJ_b_TestIdentityFlag(pObject, OBJ_C_IdentityFlag_Generated) )
			continue;
		
		// Count objects that have a visu
		if ( OBJ_b_TestIdentityFlag(pObject, OBJ_C_IdentityFlag_Visu) )
			ui_NbObjects++;
	}

	// Write the number of objects
	SAV_Buffer(&ui_NbObjects, 4);

	// Save each object and its lights
	itLRL = m_rejections.begin( );
	for( ; itLRL != m_rejections.end(); ++itLRL )
	{
		OBJ_tdst_GameObject *pObject = itLRL->first;
		
		if ( ! (pObject && pObject->pst_Base && pObject->pst_Base->pst_Visu) )
			continue;

		// Skip saving of objects that are instances and do not physically exists in BF
		ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pObject);
		if ( ul_Key == BIG_C_InvalidKey)
			continue;
		if ( OBJ_b_TestIdentityFlag(pObject, OBJ_C_IdentityFlag_Generated) )
			continue;
	
		// Make sure object has a visu before saving it to LRL
		if( ! OBJ_b_TestIdentityFlag(pObject, OBJ_C_IdentityFlag_Visu) )
			continue;

		// Write object key
		SAV_Buffer(&ul_Key, 4);

		// Write number of lights
		UINT ui_NbLights = 0;
		RejectedLights::iterator itLight;
		RejectedLights* pRejectedLights = itLRL->second;
		
		// Check that all lights exists in BF before saving the number of lights
		itLight = pRejectedLights->begin( );
		for( ; itLight != pRejectedLights->end(); ++itLight )
		{
			OBJ_tdst_GameObject* pLight = *itLight;

			if ( ! (pLight && pLight->pst_Extended && pLight->pst_Extended->pst_Light) )
				continue;

			// Skip saving of objects that are instances and do not physically exists in BF
			ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pLight);
			if ( ul_Key == BIG_C_InvalidKey)
				continue;
			if ( OBJ_b_TestIdentityFlag(pLight, OBJ_C_IdentityFlag_Generated) )
				continue;
			
			// Make sure object is a light before counting it
			if( OBJ_b_TestIdentityFlag(pLight, OBJ_C_IdentityFlag_Lights) )
				ui_NbLights++;	
			else
				continue;
		}		
		SAV_Buffer(&ui_NbLights, 4);

		// Write all lights
		itLight = pRejectedLights->begin();
		for( ;itLight != pRejectedLights->end(); ++itLight )
		{
			OBJ_tdst_GameObject * pLight = *itLight;

			if ( ! (pLight && pLight->pst_Extended && pLight->pst_Extended->pst_Light) )
				continue;

			// Skip saving of objects that are instances and do not physically exists in BF
			ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pLight);
			if ( ul_Key == BIG_C_InvalidKey)
				continue;
			if ( OBJ_b_TestIdentityFlag(pLight, OBJ_C_IdentityFlag_Generated) )
				continue;

			// Write light key
			if( OBJ_b_TestIdentityFlag(pLight, OBJ_C_IdentityFlag_Lights) )
				SAV_Buffer(&ul_Key, 4);
		}
	}
}
#endif

//----------------------------------------------------
// LightRejectionManager::GetObjectCount
//----------------------------------------------------
ULONG                
LightRejectionManager::GetObjectCount( )
{
	return (ULONG) m_rejections.size();
}

//----------------------------------------------------
// LightRejectionManager::GetRejectionList
//----------------------------------------------------
RejectedLights *  
LightRejectionManager::GetRejectionList( OBJ_tdst_GameObject * pObject )
{
	LightRejectionList::iterator itLRL = m_rejections.find( pObject );
	if( itLRL != m_rejections.end() )
	{
		return itLRL->second;
	}
	return NULL;
}

//----------------------------------------------------
// LightRejectionManager::GetRejectionList
//----------------------------------------------------
RejectedLights *   
LightRejectionManager::GetRejectionList( ULONG _index )
{
	if( _index < GetObjectCount() )
	{
		LightRejectionList::iterator itLRL = m_rejections.begin( );
		std::advance(itLRL, _index);

		return itLRL->second;
	}

	return NULL;
}

//----------------------------------------------------
// LightRejectionManager::GetObject
//----------------------------------------------------
OBJ_tdst_GameObject * 
LightRejectionManager::GetObject( ULONG _index )
{
	if( _index < GetObjectCount() )
	{
		LightRejectionList::iterator itLRL = m_rejections.begin( );
		std::advance(itLRL, _index);
		return itLRL->first;
	}
	return NULL;
}

//----------------------------------------------------
// LightRejectionManager::Clear
//----------------------------------------------------
void     
LightRejectionManager::Clear( )
{
	// Delete all rejection lists
	LightRejectionList::iterator itLRL = m_rejections.begin();
	for( ;itLRL != m_rejections.end(); ++itLRL )
	{
		// SC: Validation
		if (OBJ_b_TestIdentityFlag(itLRL->first, OBJ_C_IdentityFlag_Visu))
		{
			OBJ_p_GetVisu( itLRL->first )->pRejectedLights = NULL;
		}
		// Delete list of lights associated to GAO
		delete itLRL->second;
	}
	m_rejections.clear();

	ClearLoadRefs();
}

//----------------------------------------------------
// LightRejectionManager::ClearLoadRefs
//----------------------------------------------------
void        
LightRejectionManager::ClearLoadRefs( )
{
	if ( m_pObjectLoadRefs )
	{
		ObjectLoadRefs * pObjectLoadRef = m_pObjectLoadRefs;
		for( ULONG i=0; i<m_ulNbrObjectsLoad; ++i, ++pObjectLoadRef )
		{
			if( pObjectLoadRef->m_lights )
			{
				MEM_Free( pObjectLoadRef->m_lights );
			}
		}
		if( m_pObjectLoadRefs )
		{
			MEM_Free( m_pObjectLoadRefs );
		}
	}
	m_pObjectLoadRefs = NULL;
	m_ulNbrObjectsLoad = 0;
}

//----------------------------------------------------
// LightRejectionManager::Resolve
//----------------------------------------------------
void       
LightRejectionManager::Resolve( )
{
	if ( m_pObjectLoadRefs == NULL )
		return;

	ObjectLoadRefs * pObjectLoadRef = m_pObjectLoadRefs;
	for( ULONG i=0; i<m_ulNbrObjectsLoad; ++i, ++pObjectLoadRef )
	{
		if ( pObjectLoadRef != NULL )
		{
			pObjectLoadRef->m_object = (OBJ_tdst_GameObject*)LOA_ul_SearchAddress((ULONG)pObjectLoadRef->m_object);
			if( (pObjectLoadRef->m_object != (OBJ_tdst_GameObject*)0xFFFFFFFF) )
			{
				for( ULONG j=0; j<pObjectLoadRef->m_ulNbrLights; ++j )
				{
					pObjectLoadRef->m_lights[j] = (OBJ_tdst_GameObject*)LOA_ul_SearchAddress((ULONG)pObjectLoadRef->m_lights[j]);
					if( (pObjectLoadRef->m_lights[j] == (OBJ_tdst_GameObject*)0xFFFFFFFF) )
					{
						LINK_PrintStatusMsg(ERR_szFormatMessage("LRLResolve: LRL list contains light [0x%08x] that doesn't exist physically in BF or is not loaded in memory.", pObjectLoadRef->m_lights[j]));		
					}
				}
			}
			else
			{
				LINK_PrintStatusMsg(ERR_szFormatMessage("LRLResolve: LRL list contains object [0x%08x] that doesn't exist physically in BF or is not loaded in memory.", pObjectLoadRef->m_object));		
			}
		}
	}
	Refresh();
}

//----------------------------------------------------
// LightRejectionManager::UpdateLRLWithGAODuplicate
//----------------------------------------------------
void 
LightRejectionManager::UpdateLRLWithGAODuplicate( OBJ_tdst_GameObject *pObjectSrc, OBJ_tdst_GameObject *pObjectDuplicate, eLRLAction LRLAction )
{
	// -NOTE- If we are currently loading a map, we need to keep track of GAOs being duplicated in a vector.
	// When the map is finished loading we must give to those duplicate GAOs the light rejection list of their
	// source GAOs.
	if ( LRLAction == eLRLLoading )	
	{
		// We only want GAOs that have a GRO object attached to it.
		if ( pObjectSrc && pObjectDuplicate )
		{
			if ( (OBJ_b_TestIdentityFlag(pObjectSrc, OBJ_C_IdentityFlag_Visu) && pObjectSrc->pst_Base && pObjectSrc->pst_Base->pst_Visu) &&
				(OBJ_b_TestIdentityFlag(pObjectDuplicate, OBJ_C_IdentityFlag_Visu) && pObjectDuplicate->pst_Base && pObjectDuplicate->pst_Base->pst_Visu) )
			{	
				m_vcGAOSrcDuplicate.push_back(OBJ_tdst_GAOSrcDuplicate(pObjectSrc, pObjectDuplicate));
			}
		}
	}
	// -NOTE- If we are running a map, when GAO is duplicated and the source GAO has a light rejection list then
	// we give that light rejection list for the duplicate immediately.
	else if ( LRLAction == eLRLSpawing )
	{
		// We only want GAOs that have a GRO object attached to it.
		if ( pObjectSrc && pObjectDuplicate )
		{
			if ( (OBJ_b_TestIdentityFlag(pObjectSrc, OBJ_C_IdentityFlag_Visu) && pObjectSrc->pst_Base && pObjectSrc->pst_Base->pst_Visu) &&
				(OBJ_b_TestIdentityFlag(pObjectDuplicate, OBJ_C_IdentityFlag_Visu) && pObjectDuplicate->pst_Base && pObjectDuplicate->pst_Base->pst_Visu) )
			{	
				OBJ_p_GetVisu(pObjectDuplicate)->pRejectedLights = OBJ_p_GetVisu(pObjectSrc)->pRejectedLights;
			}
		}
	}
}

#ifdef ACTIVE_EDITORS
//----------------------------------------------------
// LightRejectionManager::SpecialResolve
// Note : Special function for the automatic update of return maps
//----------------------------------------------------
void       
LightRejectionManager::SpecialResolve( std::map< BIG_KEY, BIG_KEY >& _MapSrcKey_GAOKey )
{
	std::map< BIG_KEY, BIG_KEY >::iterator	it;

	ObjectLoadRefs * pObjectLoadRef = m_pObjectLoadRefs;

	for( ULONG i=0; i<m_ulNbrObjectsLoad; ++i, ++pObjectLoadRef )
	{
		it = _MapSrcKey_GAOKey.find((ULONG)pObjectLoadRef->m_object);

		if(it != _MapSrcKey_GAOKey.end())
		{
			pObjectLoadRef->m_object = (OBJ_tdst_GameObject*)LOA_ul_SearchAddress(it->second);
			if( (pObjectLoadRef->m_object != (OBJ_tdst_GameObject*)0xFFFFFFFF) )
			{
				for( ULONG j=0; j<pObjectLoadRef->m_ulNbrLights; ++j )
				{
					pObjectLoadRef->m_lights[j] = (OBJ_tdst_GameObject*)LOA_ul_SearchAddress((ULONG)pObjectLoadRef->m_lights[j]);
				}
			}
		}
		else
		{
			pObjectLoadRef->m_object = (OBJ_tdst_GameObject*)0xFFFFFFFF;
		}
	}

	Refresh();
}

//----------------------------------------------------
// LightRejectionManager::DuplicateList
// Note : Duplicate list of an object to another
//----------------------------------------------------
BOOL       
LightRejectionManager::DuplicateList( OBJ_tdst_GameObject *pSrcObject, OBJ_tdst_GameObject *pDstObject )
{
	if( !pSrcObject || !pDstObject ) return FALSE;

	// SC: Make sure the object is valid
	if (!OBJ_b_TestIdentityFlag(pSrcObject, OBJ_C_IdentityFlag_Visu) || !OBJ_b_TestIdentityFlag(pDstObject, OBJ_C_IdentityFlag_Visu))
		return FALSE;

	// See if rejected light vector exists for object
	LightRejectionList::iterator itLRL = m_rejections.find( pSrcObject );
	if( itLRL != m_rejections.end() )
	{
		//make sure the DstObject doesn't already have a list
		LightRejectionList::iterator itDstLRL = m_rejections.find( pDstObject );
		if( itDstLRL != m_rejections.end() )
		{
			(itDstLRL->second)->clear();
			(itDstLRL->second)->insert((itLRL->second)->begin(),(itLRL->second)->end());
		}
		else
		{
			// Duplicate rejectedList and create a new entry
			RejectedLights * pRejectedLights = new RejectedLights(*(itLRL->second));
			itLRL = m_rejections.insert( LightRejectionList::value_type( pDstObject, pRejectedLights ) ).first;
		}
	}
	else
	{
		char sz_Msg[256];
		sprintf(sz_Msg, "[%08x] - Source object(%s) doesn't have a rejected lights list!\n", pSrcObject->ul_MyKey, pSrcObject->sz_Name);
		ERR_X_Warning(0, sz_Msg, NULL);
		return FALSE;
	}

	ERR_X_Assert( itLRL != m_rejections.end() );

	// Set rejected light list in object
	OBJ_p_GetVisu( pDstObject )->pRejectedLights = itLRL->second;

	return TRUE;
}

#endif

/*
=======================================================================================================================
=======================================================================================================================
FUNCTIONS TO MANIPULATE LIGHT REJECTION LIST AND OBJECTS	
=======================================================================================================================
=======================================================================================================================
*/

ULONG LRL_GetWorldKey() 
{ 
	return g_LRL_ul_world_id; 
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void LRL_SetWorldKey(ULONG ul_world_id) 
{ 
	g_LRL_ul_world_id = ul_world_id; 
}  

/*
=======================================================================================================================
=======================================================================================================================
*/
void LRL_Rem(OBJ_tdst_GameObject *pLight, OBJ_tdst_GameObject *pObject)
{
	if(!pLight || !pObject) return;

	g_LightRejectionList.Remove( pLight, pObject );
}

/*
=======================================================================================================================
=======================================================================================================================
*/
#define LRL_CURRENT_VERSION	2

#ifdef ACTIVE_EDITORS
/*
=======================================================================================================================
=======================================================================================================================
*/
BIG_KEY LRL_Save(WOR_tdst_World *_pst_World)
{
	BIG_INDEX	ul_Index;
	char		asz_Path[BIG_C_MaxLenPath];
	char		asz_Name[BIG_C_MaxLenName];
	ULONG       ul_Key;
	ULONG       ul_LRLKey = BIG_C_InvalidKey;

	// if no world loaded...
	if(!_pst_World) 
		return BIG_C_InvalidKey;

	// Make LRL is not empty before saving
	if ( g_LightRejectionList.GetObjectCount() == 0 )
		return BIG_C_InvalidKey;

	// always recompute name to use parent world (wol used for lighting)
	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_World);
	if (ul_Index == BIG_C_InvalidIndex)
		return BIG_C_InvalidKey;

	L_strcpy(asz_Name, BIG_NameFile(ul_Index));
	*L_strrchr(asz_Name, '.') = 0;

	BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
	L_strcat(asz_Path, "/");
	L_strcat(asz_Path, EDI_Csz_Path_LightRejection);
	L_strcat(asz_Name, EDI_Csz_ExtLightRejectionList);

	SAV_Begin(asz_Path, asz_Name);
	_Try_

	// file version
	ul_Key = LRL_CURRENT_VERSION;
	SAV_Buffer(&ul_Key, sizeof(ULONG)); 

	// save content
	g_LightRejectionList.SaveAll( );

	ul_Index = SAV_ul_End();

	_Catch_
		ERR_X_Warning(0, ERR_szFormatMessage("LRLSave: %s save failed for unknown reasons", BIG_NameFile(ul_Index)), NULL);
	ul_Index = SAV_ul_End();
	_End_

	ul_LRLKey = BIG_FileKey(ul_Index);		
	return ul_LRLKey;
}

#endif /* ACTIVE_EDITORS */

// Instanciation and intialization of global variables to be used
LightRejectionManager g_LightRejectionList;
ULONG g_LRL_ul_world_id = 0;

#ifdef ACTIVE_EDITORS
BIG_KEY g_LRL_ul_Key = BIG_C_InvalidKey;
#endif

/*
=======================================================================================================================
=======================================================================================================================
*/
ULONG LRL_ul_LoadCallback(ULONG _ul_PosFile)
{
	BIG_KEY						ulObjectKey;
	BIG_KEY						ulLightKey;
	ULONG						ul_Size;
	BIG_KEY						ul_Version;
	CHAR*  pc_Buffer;

	pc_Buffer = BIG_pc_ReadFileTmp(_ul_PosFile, &ul_Size);

	// file version
	ul_Version = LOA_ReadULong(&pc_Buffer); 

	if (ul_Version <= 1 )
	{

		// read content
		ulLightKey = LOA_ReadULong(&pc_Buffer); 	

		while(ulLightKey != BIG_C_InvalidKey)
		{
			ulObjectKey = LOA_ReadULong(&pc_Buffer); 

			LRL_Add(ulLightKey, ulObjectKey);

			ulLightKey = LOA_ReadULong(&pc_Buffer); 
		}		
	}
	else if( ul_Version == 2 )
	{		
		g_LightRejectionList.LoadAll( pc_Buffer );
	}
	else
	{
#ifdef ACTIVE_EDITORS
		ERR_X_Warning(0, ERR_szFormatMessage("Skipped loading of LRL (key [%08X]) because it is corrupted", LOA_ul_GetCurrentKey()), NULL);
#endif
	}

	return LOA_ul_GetCurrentKey();
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void LRL_Load(BIG_KEY _ul_LightRejectionKey)
{
#ifdef ACTIVE_EDITORS
	g_LRL_ul_Key = _ul_LightRejectionKey;
#endif

    if(_ul_LightRejectionKey==BIG_C_InvalidKey)
        return;

	LOA_MakeFileRef(_ul_LightRejectionKey, 
					(ULONG* )&_ul_LightRejectionKey,
					LRL_ul_LoadCallback,
					LOA_C_MustExists | LOA_C_NotSavePtr);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void LRL_Add(OBJ_tdst_GameObject *pLight, OBJ_tdst_GameObject *pObject )
{
	g_LightRejectionList.Add( pLight, pObject );
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void LRL_Add(BIG_KEY ulLightKey, BIG_KEY ulObjectKey)
{
	g_LightRejectionList.Add( ulLightKey, ulObjectKey );
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void LRL_Clean()
{
	g_LightRejectionList.Clear( );
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void LRL_Init(ULONG ul_world_id)
{
	g_LRL_ul_world_id = ul_world_id;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void LRL_Resolve()
{
	g_LightRejectionList.Resolve( );
}

#ifdef ACTIVE_EDITORS
/*
=======================================================================================================================
=======================================================================================================================
*/
void LRL_SpecialResolve( std::map< BIG_KEY, BIG_KEY >& _MapSrcKey_GAOKey )
{
	g_LightRejectionList.SpecialResolve( _MapSrcKey_GAOKey );
}

/*
=======================================================================================================================
=======================================================================================================================
*/
BOOL LRL_DuplicateList( OBJ_tdst_GameObject *pSrcObject, OBJ_tdst_GameObject *pDstObject )
{
	return g_LightRejectionList.DuplicateList(pSrcObject, pDstObject);
}
#endif
