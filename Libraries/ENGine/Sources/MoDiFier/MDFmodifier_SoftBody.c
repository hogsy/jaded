//------------------------------------------------------------------------
//
// (C) Copyright 2001 Ubisoft
//
// Author		Stephane Girard
// Date			18 Mar 2002
//
// File			MDFmodifier_SoftBody.c
// Description
//
//------------------------------------------------------------------------


#include "Precomp.h"
#include "MDFmodifier_SoftBody.h"
#include "SDK/sources/random/perlinnoise.h"
#include "ENGine/Sources/ENGvars.h"
#include "BASe/BENch/BENch.h"
#include "ENGine/Sources/Wind/WindManager.h"
#include "ENGine/Sources/Wind/CurrentWind.h"

#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "OBJects/OBJmain.h"
#include "ENGine/Sources/ANImation/ANImain.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "SDK/Sources/MATHs/MATHmatrixScale.h"
#include "ENGine/Sources/COLlision/COLaccess.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORaccess.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#include "GEOmetric/GEOcreateobject.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "OBJects/OBJsave.h"
#include "GRP/GRPMain.h"
#include "GRP/GRPSave.h"
#include "ENGine/Sources/OBJects/OBJgizmo.h"
#include "GraphicDK/Sources/GEOmetric/GEO_SKIN.h"
#include "ANImation/ANIrender.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGmdfy_dir.h"
//#include "COLlision/COLevent.h"
#include "COLlision/COLinit.h"
#include "COLlision/COLsave.h"
#include "COLlision/COLload.h"
#include "F3Dframe/F3Dview.h"

#include "BIGfiles/LOAding/LOAdefs.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGfat.h"
#include "Engine/Sources/MoDiFier/MDFstruct.h"

#endif // #ifdef ACTIVE_EDITORS

#ifdef ACTIVE_EDITORS
BOOL SoftBody_gb_DisplayPlaneCollision = FALSE;
BOOL SoftBody_gb_Tear = FALSE;
#endif //#ifdef ACTIVE_EDITORS

#ifdef PS2_RENDER_NEW
#include "JadeSkinnedMesh.h"
#endif

#define forAll(type,container,iter) for(type##::iterator iter = container.begin();iter != container.end();iter++)
#define forAllReverse(type,container,iter) for(type##::reverse_iterator iter = container.rbegin();iter != container.rend();iter++)

/*
=======================================================================================================================
 Private 
=======================================================================================================================
*/
void   GAO_ModifierSoftBody_UpdateWind(GAO_tdst_ModifierSoftBody * _p_SoftBody, FLOAT _f_DT);
void   GAO_ModifierSoftBody_AccumulateForces(GAO_tdst_ModifierSoftBody * _p_SoftBody);
void   GAO_ModifierSoftBody_Solver(GAO_tdst_ModifierSoftBody * _p_SoftBody, FLOAT _f_DT);
void   GAO_ModifierSoftBody_SatisfyConstraints(GAO_tdst_ModifierSoftBody * _p_SoftBody);
void   GAO_ModifierSoftBody_Collision(GAO_tdst_ModifierSoftBody * _p_SoftBody);
void   GAO_ModifierSoftBody_Normalize(GAO_tdst_ModifierSoftBody * _p_SoftBody);
void   GAO_ModifierSoftBody_Tear(GAO_tdst_ModifierSoftBody * p_SoftBody);
void   GAO_ModifierSoftBody_Untear(GAO_tdst_ModifierSoftBody * p_SoftBody);
void   GAO_ModifierSoftBody_StartTearing(GAO_tdst_ModifierSoftBody * p_SoftBody);
void   GAO_ModifierSoftBody_StopTearing(GAO_tdst_ModifierSoftBody * p_SoftBody);
void   GAO_ModifierSoftBody_TearClosest(GAO_tdst_ModifierSoftBody * p_SoftBody,MATH_tdst_Vector *vertex,FLOAT minimalDistance);
BOOL   GAO_ModifierSoftBody_UpdatePointers(MDF_tdst_Modifier *_pst_Mod);

void   GAO_ModifierSoftBody_CollisionPlanes(GAO_tdst_ModifierSoftBody * _p_SoftBody);
BOOL   GAO_ModifierSoftBody_CollideSphere(GAO_tdst_ModifierSoftBody * _p_SoftBody, INT_tdst_Sphere * _pst_Sphere);
BOOL   GAO_ModifierSoftBody_CollideGao(GAO_tdst_ModifierSoftBody * _p_SoftBody, OBJ_tdst_GameObject * _pst_Gao, BOOL _bCheckBV );
BOOL   GAO_ModifierSoftBody_CollideTriangles(GAO_tdst_ModifierSoftBody * _p_SoftBody, MATH_tdst_Matrix * _pst_Matrix, ULONG _ul_NbTriangles, USHORT * _auw_Triangle, COL_tdst_IndexedTriangle	* _ast_Triangle, MATH_tdst_Vector * _ast_Points, MATH_tdst_Vector * _ast_Normals);
BOOL   GAO_ModifierSoftBody_CollideBox(GAO_tdst_ModifierSoftBody * _p_SoftBody, MATH_tdst_Matrix * _pst_MatrixBox, MATH_tdst_Vector * _pst_LocalMin, MATH_tdst_Vector * _pst_LocalMax);
BOOL   GAO_ModifierSoftBody_CollideCylinder(GAO_tdst_ModifierSoftBody * _p_SoftBody, COL_tdst_Cylinder * _pst_Cylinder);
void   GAO_ModifierSoftBody_Friction(GAO_tdst_ModifierSoftBody * _p_SoftBody, CSoftBodyVertex  * _p_Vertex, MATH_tdst_Vector * _pst_Normal);


//#ifdef ACTIVE_EDITORS
void GAO_ModifierSoftBody_Duplicate(OBJ_tdst_GameObject       * _pst_Gao,
                                    GAO_tdst_ModifierSoftBody * _pst_SoftBodyDest,
                                    GAO_tdst_ModifierSoftBody * _pst_SoftBodySrc)
{
    ULONG             i,j;
    WOR_tdst_World  * pst_World;
    GEO_tdst_Object * pst_Obj;

    pst_Obj = (GEO_tdst_Object *) OBJ_p_GetGro(_pst_Gao);
    if((!pst_Obj) || (pst_Obj->st_Id.i->ul_Type != GRO_Geometric))
    {
        return;
    }

    L_memset(_pst_SoftBodyDest, 0, sizeof(GAO_tdst_ModifierSoftBody));

    pst_World = WOR_World_GetWorldOfObject(_pst_Gao);

    // Create Skin
#ifdef ACTIVE_EDITORS
    GAO_MofifierSoftBody_CreateSkin(_pst_Gao, pst_Obj);
#endif

    // copy vertices
    _pst_SoftBodyDest->ul_NbVertices = _pst_SoftBodySrc->ul_NbVertices;
    _pst_SoftBodyDest->a_Vertices = new CSoftBodyVertex[_pst_SoftBodySrc->ul_NbVertices];
    L_memcpy(_pst_SoftBodyDest->a_Vertices,_pst_SoftBodySrc->a_Vertices,_pst_SoftBodySrc->ul_NbVertices*sizeof(CSoftBodyVertex));

    // copy rods
    _pst_SoftBodyDest->ul_NbRods = _pst_SoftBodySrc->ul_NbRods;
    _pst_SoftBodyDest->a_Rods = new CSoftBodyLengthConstraint[_pst_SoftBodySrc->ul_NbRods];
    L_memcpy(_pst_SoftBodyDest->a_Rods,_pst_SoftBodySrc->a_Rods,_pst_SoftBodySrc->ul_NbRods*sizeof(CSoftBodyLengthConstraint));	

    //copy zipper
    _pst_SoftBodyDest->zipper.clear();
    for(i=0;i<_pst_SoftBodySrc->zipper.size();i++)
    {
        CSoftBodyVertex *v = _pst_SoftBodySrc->zipper[i];

        for(j=0;j<_pst_SoftBodySrc->ul_NbVertices;j++)
        {
            if( v == &_pst_SoftBodySrc->a_Vertices[j])
            {
                _pst_SoftBodyDest->zipper.push_back(&_pst_SoftBodyDest->a_Vertices[j]);
            }
        }
    }

    // copy anchor point
    CSoftBodyVertex *v = _pst_SoftBodySrc->pst_AnchorPoint;
    for(j=0;j<_pst_SoftBodySrc->ul_NbVertices;j++)
    {
        if( v == &_pst_SoftBodySrc->a_Vertices[j])
        {
            _pst_SoftBodyDest->pst_AnchorPoint = &_pst_SoftBodyDest->a_Vertices[j];
        }
    }

    // setup tearing vertices
    for(j=0;j<_pst_SoftBodySrc->ul_NbVertices;j++)
    {		
        CSoftBodyVertex *v = &_pst_SoftBodySrc->a_Vertices[j];

        if(v->m_tearingVertex != NULL)
        {
            _pst_SoftBodyDest->a_Vertices[j].m_tearingID = v->m_tearingID;
            _pst_SoftBodyDest->a_Vertices[j].m_tearingVertex = &_pst_SoftBodyDest->a_Vertices[v->m_tearingID];
        }		
    }

    _pst_SoftBodyDest->ul_NbIter = _pst_SoftBodySrc->ul_NbIter;
    _pst_SoftBodyDest->ul_NbNormalizeIter = _pst_SoftBodySrc->ul_NbNormalizeIter;
    _pst_SoftBodyDest->ul_NbCollPlane = _pst_SoftBodySrc->ul_NbCollPlane;

    // Copy plane
    _pst_SoftBodyDest->a_CollPlane = NULL;

#ifdef ACTIVE_EDITORS
    GAO_ModifierSoftBody_ResizePlanes(_pst_SoftBodyDest, 0);
#endif

    for(i=0; i<_pst_SoftBodyDest->ul_NbCollPlane; i++)
    {
        _pst_SoftBodyDest->a_CollPlane[i].m_LocalPlane = _pst_SoftBodySrc->a_CollPlane[i].m_LocalPlane;
    }

    // Copy vertex flag
    for(i=0; i<_pst_SoftBodyDest->ul_NbVertices; i++)
    {
        _pst_SoftBodyDest->a_Vertices[i].m_ulFlags = _pst_SoftBodySrc->a_Vertices[i].m_ulFlags;
    }

    MATH_CopyVector(&_pst_SoftBodyDest->st_Gravity, &_pst_SoftBodySrc->st_Gravity);

    _pst_SoftBodyDest->f_DTDamping = _pst_SoftBodySrc->f_DTDamping;

    MATH_CopyVector(&_pst_SoftBodyDest->st_WindMin, &_pst_SoftBodySrc->st_WindMin);
    MATH_CopyVector(&_pst_SoftBodyDest->st_WindMax, &_pst_SoftBodySrc->st_WindMax);

    _pst_SoftBodyDest->f_WindUScale = _pst_SoftBodySrc->f_WindUScale;
    _pst_SoftBodyDest->f_WindVScale = _pst_SoftBodySrc->f_WindVScale;
    _pst_SoftBodyDest->f_WindUPan = _pst_SoftBodySrc->f_WindUPan ;
    _pst_SoftBodyDest->f_WindVPan = _pst_SoftBodySrc->f_WindVPan;

    _pst_SoftBodyDest->f_StaticFriction = _pst_SoftBodySrc->f_StaticFriction;
    _pst_SoftBodyDest->f_KineticFriction = _pst_SoftBodySrc->f_KineticFriction;

    _pst_SoftBodyDest->f_PrevDT = 1.0f / 30.0f;
    _pst_SoftBodyDest->f_WindUOri = 0.0f;
    _pst_SoftBodyDest->f_WindVOri = 0.0f;

    // tearing
    _pst_SoftBodyDest->f_DampLength = _pst_SoftBodySrc->f_DampLength;
    _pst_SoftBodyDest->f_InitialSpeed = _pst_SoftBodySrc->f_InitialSpeed;
    _pst_SoftBodyDest->f_Acceleration = _pst_SoftBodySrc->f_Acceleration;		
    _pst_SoftBodyDest->f_Angles[0] = _pst_SoftBodySrc->f_Angles[0];
    _pst_SoftBodyDest->f_Angles[1] = _pst_SoftBodySrc->f_Angles[1];
    _pst_SoftBodyDest->f_MaxSwing = _pst_SoftBodySrc->f_MaxSwing;

    // dress
    _pst_SoftBodyDest->actor = _pst_SoftBodySrc->actor;
    _pst_SoftBodyDest->fatherChannel = _pst_SoftBodySrc->fatherChannel;
    _pst_SoftBodyDest->f_DampingDistance = _pst_SoftBodySrc->f_DampingDistance;
    _pst_SoftBodyDest->f_BlendRatio = _pst_SoftBodySrc->f_BlendRatio;



#ifdef ACTIVE_EDITORS
    _pst_SoftBodyDest->ul_SizeX = _pst_SoftBodySrc->ul_SizeX;
    _pst_SoftBodyDest->ul_SizeY = _pst_SoftBodySrc->ul_SizeY;
#endif // #ifdef ACTIVE_EDITORS
}
//#endif // #ifdef ACTIVE_EDITORS

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				19 Mar 2002
// 
// Prototype		GAO_ModifierSoftBody_Create
// Parameters		_pst_GO : 
//					_pst_Mod : 
//					p_Data : 
// Return Type		void
// 
// Description		
// 
//------------------------------------------------------------------------
void GAO_ModifierSoftBody_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierSoftBody	*p_SoftBody;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GAO_tdst_ModifierSoftBody) );
	p_SoftBody = (GAO_tdst_ModifierSoftBody *) _pst_Mod->p_Data;
	if(p_Data == NULL)
	{
		// First init
		L_memset(_pst_Mod->p_Data, 0, sizeof(GAO_tdst_ModifierSoftBody));
		_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;

        p_SoftBody->ul_NbIter = 2;
        p_SoftBody->ul_NbNormalizeIter = 2;
        p_SoftBody->ul_NbVertices = 0;
        p_SoftBody->ul_NbRods = 0;
        p_SoftBody->ul_NbCollPlane = 0;
        p_SoftBody->a_Vertices = NULL;
        p_SoftBody->a_Rods = NULL;
        p_SoftBody->a_CollPlane = NULL;
        p_SoftBody->f_DTDamping = 1.0f;
        p_SoftBody->f_PrevDT = 1.0f / 30.0f;
        
        MATH_InitVector(&p_SoftBody->st_WindMin, 0.0f, 0.0f, 0.0f);
        MATH_InitVector(&p_SoftBody->st_WindMax, 0.0f, 0.0f, 0.0f);
        p_SoftBody->f_WindUScale = 0.8f;
        p_SoftBody->f_WindVScale = 0.8f;
        p_SoftBody->f_WindUPan = 1.2f;
        p_SoftBody->f_WindVPan = 1.2f;
        p_SoftBody->f_StaticFriction = 0.5f;
        p_SoftBody->f_KineticFriction = 0.95f;

        MATH_InitVector(&p_SoftBody->st_Gravity, 0.0f, 0.0f, -30.0f);

        GAO_ModifierSoftBody_UpdatePointers(_pst_Mod);

		// tearing
		p_SoftBody->f_DampLength = 6.0f;
		p_SoftBody->f_InitialSpeed = 1.5f;
		p_SoftBody->f_Acceleration = 3.0f;		
		p_SoftBody->f_Angles[0] = 20.0f;
		p_SoftBody->f_Angles[1] = 1.5f;
		p_SoftBody->f_MaxSwing = 1.5f;

		// dress
		p_SoftBody->actor = NULL;		
		p_SoftBody->fatherChannel = -1;	
        p_SoftBody->b_actorAssigned = FALSE;		
	}
	else
	{
		_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;

        //#ifdef ACTIVE_EDITORS
        GAO_ModifierSoftBody_Duplicate(_pst_GO, p_SoftBody, (GAO_tdst_ModifierSoftBody*)p_Data);
        GAO_ModifierSoftBody_UpdatePointers(_pst_Mod);
        //#endif // #ifdef ACTIVE_EDITORS
	}

    p_SoftBody->pst_Hook = _pst_GO;
    p_SoftBody->pst_CollidedGao = NULL;		

	// spring code
	/*
	for(int i=0;i<4;i++)
	{
		p_SoftBody->channels[i] = 0xFFFFFFFF;
		p_SoftBody->pst_Colliders[i] = NULL;
	}

	for(int i=0;i<10;i++)
	{
		p_SoftBody->springs[i].m_boneChannel = -1;
		p_SoftBody->springs[i].m_vertexID = -1;
	}
	*/
#ifdef ACTIVE_EDITORS
	p_SoftBody->ulCodeKey = 0xC0DE2001;
#endif

}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				19 Mar 2002
// 
// Prototype		GAO_ModifierSoftBody_Destroy
// Parameters		_pst_Mod : 
// Return Type		void
// 
// Description		
// 
//------------------------------------------------------------------------
void GAO_ModifierSoftBody_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	if(_pst_Mod->p_Data)
	{
        GAO_tdst_ModifierSoftBody * p_SoftBody;
        p_SoftBody = (GAO_tdst_ModifierSoftBody *) _pst_Mod->p_Data;

        delete [] p_SoftBody->a_Vertices;
        delete [] p_SoftBody->a_Rods;
        delete [] p_SoftBody->a_CollPlane;

		MEM_Free(_pst_Mod->p_Data);
	}
}


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				19 Mar 2002
// 
// Prototype		GAO_ModifierSoftBody_Apply
// Parameters		_pst_Mod : 
//					_pst_Obj : 
// Return Type		void
// 
// Description		
// 
//------------------------------------------------------------------------
void GAO_ModifierSoftBody_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
    GAO_tdst_ModifierSoftBody * p_SoftBody;
    FLOAT                       f_DT;
    ULONG i;
	
    //if (_pst_Mod->ul_Flags & MDF_C_Modifier_Pause)
    //{
    //    return;	
    //}

    // Make sure the mesh is skinned in software
    _pst_Obj->b_ForceSoftSkinning = TRUE;

    p_SoftBody = (GAO_tdst_ModifierSoftBody *) _pst_Mod->p_Data;
    
    p_SoftBody->pst_CollidedGao = NULL;


    if(p_SoftBody->actor != NULL && OBJ_pst_GetFather(_pst_Mod->pst_GO) == NULL)
	{
		// force the hierarchy flag
  		OBJ_ChangeIdentityFlags(_pst_Mod->pst_GO,_pst_Mod->pst_GO->ul_IdentityFlags | OBJ_C_IdentityFlag_Hierarchy,_pst_Mod->pst_GO->ul_IdentityFlags);
		p_SoftBody->ul_Flags |= SoftBody_C_FullHierarchy;

		// set the father of the object
		_pst_Mod->pst_GO->pst_Base->pst_Hierarchy->pst_Father = ANI_pst_GetObjectByAICanal(p_SoftBody->actor,(UCHAR)p_SoftBody->fatherChannel);		
	}	

#ifdef ACTIVE_EDITORS
    if (TIM_gf_dt == 0.0f)
    {
        f_DT = 1.0f/60.0f;
    }
#endif // #ifdef ACTIVE_EDITORS

    f_DT = fMax(TIM_gf_dt, 0.005f); // (1.0f / 200.0f)
    f_DT = fMin(f_DT, 0.0333333f);  // (1.0f / 30.0f)

	MATH_InvertMatrix(&p_SoftBody->st_InvMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO));

	if(p_SoftBody->b_IsSomeoneAttached)
	{
		p_SoftBody->f_EllapsedTime += f_DT;
		p_SoftBody->f_RealDT = f_DT;
		GAO_ModifierSoftBody_AnimateTearingPoint(p_SoftBody);
	}

    f_DT = f_DT * p_SoftBody->f_DTDamping + p_SoftBody->f_PrevDT * (1.0f - p_SoftBody->f_DTDamping);
    p_SoftBody->f_PrevDT = f_DT;

    if(f_DT > 1.0f/30.0f)
        f_DT = 1.0f/30.0f;

    //ERR_OutputDebugString("!---1\n");
    //for(i=0; i<p_SoftBody->ul_NbVertices; i++)
    //{
    //    p_SoftBody->a_Vertices[i].Print();
    //}    

    GAO_ModifierSoftBody_UpdateWind(p_SoftBody, f_DT);
    GAO_ModifierSoftBody_AccumulateForces(p_SoftBody);

    //ERR_OutputDebugString("!---2\n");
    //for(i=0; i<p_SoftBody->ul_NbVertices; i++)
    //{
    //    p_SoftBody->a_Vertices[i].Print();
    //}

    GAO_ModifierSoftBody_Solver(p_SoftBody, f_DT);

    //ERR_OutputDebugString("!---3\n");
    //for(i=0; i<p_SoftBody->ul_NbVertices; i++)
    //{
    //    p_SoftBody->a_Vertices[i].Print();
    //}

    GAO_ModifierSoftBody_SatisfyConstraints(p_SoftBody);
 
	if(p_SoftBody->b_IsSomeoneAttached)
	{	
		GAO_ModifierSoftBody_BlendNextPoint(p_SoftBody);			
	}

    // reinit the computed flag on tearable vertex
    for(i=0; i<p_SoftBody->ul_NbVertices; i++)
    {
		if(p_SoftBody->a_Vertices[i].m_ulFlags & CSoftBodyVertex::eVertexFlagTearable)
		{
			p_SoftBody->a_Vertices[i].m_ulFlags &= ~CSoftBodyVertex::eVertexFlagComputed;
		}       
    }

    //PRO_StopTrameRaster(&ENG_gpst_RasterEng_SoftBody);
    //_GSP_EndRaster(Raster_SoftBody);
}


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				19 Mar 2002
// 
// Prototype		GAO_ModifierSoftBody_Unapply
// Parameters		_pst_Mod : 
//					_pst_Obj : 
// Return Type		void
// 
// Description		
// 
//------------------------------------------------------------------------
void GAO_ModifierSoftBody_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

void GAO_ModifierSoftBody_SetActor(MDF_tdst_Modifier *_pst_Mod, OBJ_tdst_GameObject* _p_Actor)
{
	GAO_tdst_ModifierSoftBody *p_SoftBody;
    
	p_SoftBody = (GAO_tdst_ModifierSoftBody *) _pst_Mod->p_Data;
    if (p_SoftBody)
    {
        p_SoftBody->actor = _p_Actor;
        GAO_ModifierSoftBody_Reinit(_pst_Mod);
		if(_p_Actor != NULL)
			p_SoftBody->b_actorAssigned = TRUE;        
    }
}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				19 Mar 2002
// 
// Prototype		GAO_ModifierSoftBody_Reinit
// Parameters		_pst_Mod : 
// Return Type		void
// 
// Description		
// 
//------------------------------------------------------------------------
void GAO_ModifierSoftBody_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
	GAO_tdst_ModifierSoftBody *p_SoftBody;
    OBJ_tdst_Gizmo            *pst_CurGizmo;
    GEO_tdst_Object           *pst_Obj;
	ULONG 	  				   i;

	p_SoftBody = (GAO_tdst_ModifierSoftBody *) _pst_Mod->p_Data;

    if (GAO_ModifierSoftBody_UpdatePointers(_pst_Mod))
    {          
		// clear out zipper
		p_SoftBody->zipper.clear();

		// reset the anchor point if it was changed
		if(p_SoftBody->pst_AnchorPoint != NULL)
		{
			p_SoftBody->pst_AnchorPoint->m_ulFlags = CSoftBodyVertex::eVertexFlagTearable;
			p_SoftBody->pst_AnchorPoint->m_tearingVertex->m_ulFlags = CSoftBodyVertex::eVertexFlagTearable;
		}

        for(i = 0; i < p_SoftBody->ul_NbVertices; i++)
        {
            p_SoftBody->a_Vertices[i].Reinit();

			if(p_SoftBody->a_Vertices[i].m_ulFlags & CSoftBodyVertex::eVertexFlagTearable)
			{
				// add to zipper				
				p_SoftBody->zipper.push_back(&p_SoftBody->a_Vertices[i]);
				p_SoftBody->b_IsTearable = TRUE;
			}
        }				
    }

	p_SoftBody->f_WindUOri = 0.0f;
	p_SoftBody->f_WindVOri = 0.0f;
	p_SoftBody->f_PrevDT = 1.0f/30.0f;
	p_SoftBody->f_DTDamping = 0.5f;

    //for(i = 0; i < p_SoftBody->ul_NbCollPlane; i++)
    //{
    //    p_SoftBody->a_CollPlane[i].Update(*_pst_Mod->pst_GO->pst_GlobalMatrix);
    //}

    pst_Obj = (GEO_tdst_Object *) OBJ_p_GetGro(_pst_Mod->pst_GO);
    if (pst_Obj 
#ifndef PS2_RENDER_NEW
        && pst_Obj->p_SKN_Objectponderation 
#endif
        && !OBJ_b_TestIdentityFlag(_pst_Mod->pst_GO, OBJ_C_IdentityFlag_AddMatArePointer))
    {		
        pst_CurGizmo = _pst_Mod->pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo;

        for (LONG j = 0; j < _pst_Mod->pst_GO->pst_Base->pst_AddMatrix->l_Number; j++, pst_CurGizmo++)
        {
#ifndef PS2_RENDER_NEW   
            if (pst_Obj->p_SKN_Objectponderation->pp_PdrtLst[j])
                MATH_SetTranslation(&pst_CurGizmo->st_Matrix, &pst_Obj->p_SKN_Objectponderation->pp_PdrtLst[j]->st_FlashedMatrix.T);
#else
            if (pst_Obj->pInitialMatrix)
                MATH_SetTranslation(&pst_CurGizmo->st_Matrix, &((MATH_tdst_Matrix*)&pst_Obj->pInitialMatrix->m_InitialMatrix[j])->T );
#endif
            MATH_ScaleEqualVector(&pst_CurGizmo->st_Matrix.T, -1);
        }
    }

	if(p_SoftBody->ul_NbTearable > 0)
	{
		// sort the zipper according to the z position of the vertex
		std::sort(p_SoftBody->zipper.begin(),p_SoftBody->zipper.end(),compCSoftBodyVertex());

		//ERR_OutputDebugString("Zipper %i\n",p_SoftBody->zipper.size());		
		forAll(GAO_tdst_ModifierSoftBody::ZipperVertices,p_SoftBody->zipper,item)
		{
			(*item)->m_initialZ = (*item)->m_pCurrPos->z;
			(*item)->m_ulFlags &= ~CSoftBodyVertex::eVertexFlagFixe;

			MATH_tdst_Vector *v = (*item)->m_pCurrPos;
			//ERR_OutputDebugString("Zipper Vertex:%3f %3f %3f\n",v->x,v->y,v->z);			
		}

		p_SoftBody->f_EllapsedTime = 0.0f;
		p_SoftBody->f_TearingSpeed = p_SoftBody->f_InitialSpeed; // 50cm/second
		p_SoftBody->f_TearingLength = 0.0f;	
		p_SoftBody->f_PendulumLength = 0.0f;
		p_SoftBody->pst_NextPoint = NULL;
		p_SoftBody->f_Angle = 0.0f;		

		if(p_SoftBody->zipper.size() != 0)
		{			
			// init the pendulum parameters
			p_SoftBody->pst_AnchorPoint = p_SoftBody->zipper.back();
			p_SoftBody->pst_AnchorPoint->m_ulFlags = CSoftBodyVertex::eVertexFlagFixe;
			p_SoftBody->pst_AnchorPoint->m_tearingVertex->m_ulFlags = CSoftBodyVertex::eVertexFlagFixe;
		}
	}
	
	// dress code
	if(p_SoftBody->actor != NULL )//&& OBJ_pst_GetFather(_pst_Mod->pst_GO) == NULL)
	{			
		// force the hierarchy flag
		OBJ_ChangeIdentityFlags(p_SoftBody->pst_Hook,p_SoftBody->pst_Hook->ul_IdentityFlags | OBJ_C_IdentityFlag_Hierarchy,p_SoftBody->pst_Hook->ul_IdentityFlags);
		p_SoftBody->ul_Flags |= SoftBody_C_FullHierarchy;

		// set the father of the object
		p_SoftBody->pst_Hook->pst_Base->pst_Hierarchy->pst_Father = ANI_pst_GetObjectByAICanal(p_SoftBody->actor,(UCHAR)p_SoftBody->fatherChannel);
		p_SoftBody->pst_Hook->pst_Base->pst_Hierarchy->pst_FatherInit = p_SoftBody->pst_Hook->pst_Base->pst_Hierarchy->pst_Father;		
		p_SoftBody->b_FirstInit = TRUE;
	}	
    if(p_SoftBody->b_actorAssigned)    
    {
        OBJ_ChangeIdentityFlags(p_SoftBody->pst_Hook,p_SoftBody->pst_Hook->ul_IdentityFlags & ~OBJ_C_IdentityFlag_Hierarchy,p_SoftBody->pst_Hook->ul_IdentityFlags);
        p_SoftBody->ul_Flags &= ~SoftBody_C_FullHierarchy;       
        p_SoftBody->b_actorAssigned = FALSE;
        p_SoftBody->actor = NULL;
    }

	// spring code
	/*
	if(p_SoftBody->actor != NULL )
	{
		for(i=0;i<4;i++)
		{
			if( p_SoftBody->channels[i] != 0xFFFFFFFF )
			{
				p_SoftBody->pst_Colliders[i] = ANI_pst_GetObjectByAICanal(p_SoftBody->actor, (UCHAR)p_SoftBody->channels[i]);
			}
		}
	}
	*/
	
}

 
//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				19 Mar 2002
// 
// Prototype		GAO_ModifierSoftBody_UpdatePointers
// Parameters		_pst_Mod : 
// Return Type		void
// 
// Description		
// 
//------------------------------------------------------------------------
BOOL GAO_ModifierSoftBody_UpdatePointers(MDF_tdst_Modifier *_pst_Mod)
{
    GAO_tdst_ModifierSoftBody *p_SoftBody;

	OBJ_tdst_Group			  *pst_Group;
    OBJ_tdst_AdditionalMatrix * pst_AddMatrix;
	ULONG  					   i;

    p_SoftBody = (GAO_tdst_ModifierSoftBody *) _pst_Mod->p_Data;

    p_SoftBody->ul_Flags |= SoftBody_C_InitPrevPos;

    
	pst_Group = MDF_Modifier_GetCurGroup(_pst_Mod);

	if(!pst_Group)
    {
        pst_AddMatrix = NULL;

        if( _pst_Mod->pst_GO->pst_Base )
        {
            pst_AddMatrix = _pst_Mod->pst_GO->pst_Base->pst_AddMatrix;
        }
 
        if (pst_AddMatrix)
        {
            for(i = 0; i < p_SoftBody->ul_NbVertices; i++)
            { 
                p_SoftBody->a_Vertices[i].UpdatePointer(pst_AddMatrix);
                p_SoftBody->a_Vertices[i].Reinit();                
            }
        }
        else
        {
            #ifdef ACTIVE_EDITORS
            _pst_Mod->ul_Flags |= MDF_C_Modifier_Inactive;

            if (p_SoftBody->pst_Hook)
            {
                CHAR szTmp[255];

                _pst_Mod->ul_Flags |= MDF_C_Modifier_Inactive;

                sprintf(szTmp, "ModifierSoftBody_ReInit : No group associated to gao (%s)", p_SoftBody->pst_Hook->sz_Name);
                ERR_X_Warning(0, szTmp, NULL);
            }

            #endif // #ifdef ACTIVE_EDITORS

             return FALSE;
        }
    }
    else
    {
        for(i = 0; i < p_SoftBody->ul_NbVertices; i++)
        { 
            p_SoftBody->a_Vertices[i].UpdatePointer(pst_Group);
            p_SoftBody->a_Vertices[i].Reinit();
            //p_SoftBody->a_Vertices[i].Print();			
        }
	
        /*
        MATH_tdst_Vector * p_P1;
        MATH_tdst_Vector * p_P2;

        for(i = 0; i < p_SoftBody->ul_NbRods; i++)
        {
            p_P1 = (p_SoftBody->a_Vertices[p_SoftBody->a_Rods[i].m_v1].GetCurrPos());
            p_P2 = (p_SoftBody->a_Vertices[p_SoftBody->a_Rods[i].m_v2].GetCurrPos());

            ERR_OutputDebugString("line %f,%f,%f %f,%f,%f\n", p_P1->x, p_P1->y, p_P1->z, p_P2->x, p_P2->y, p_P2->z);
        }
        */

    }	

    return TRUE;
}


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				19 Mar 2002
// 
// Prototype		GAO_ModifierSoftBody_Load
// Parameters		_pst_Mod : 
//					_pc_Buffer : 
// Return Type		ULONG
// 
// Description		
// 
//------------------------------------------------------------------------
ULONG GAO_ModifierSoftBody_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierSoftBody *p_SoftBody;
	char					  *pc_BufferSave;
	ULONG					   ul_Version;
    ULONG                      i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pc_BufferSave = _pc_Buffer;
	p_SoftBody = (GAO_tdst_ModifierSoftBody *) _pst_Mod->p_Data;

	p_SoftBody->ul_NbTearable = 0;	

	/* size */
	LOA_ReadLong_Ed(&_pc_Buffer, NULL); // skip size

	/* version */
	ul_Version = LOA_ReadULong(&_pc_Buffer);

    if (ul_Version >= 3)
    {
        p_SoftBody->ul_Flags = LOA_ReadULong(&_pc_Buffer);
    }

    if (ul_Version >= 1)
    {
	    p_SoftBody->ul_NbIter = LOA_ReadULong(&_pc_Buffer);
	    p_SoftBody->ul_NbNormalizeIter = LOA_ReadULong(&_pc_Buffer);
    
        p_SoftBody->ul_NbVertices  = LOA_ReadULong(&_pc_Buffer);
        p_SoftBody->ul_NbRods      = LOA_ReadULong(&_pc_Buffer);
        p_SoftBody->ul_NbCollPlane = LOA_ReadULong(&_pc_Buffer);

	    LOA_ReadVector(&_pc_Buffer, &p_SoftBody->st_Gravity);
        p_SoftBody->f_DTDamping = LOA_ReadFloat(&_pc_Buffer);

	    LOA_ReadVector(&_pc_Buffer, &p_SoftBody->st_WindMin);
        LOA_ReadVector(&_pc_Buffer, &p_SoftBody->st_WindMax);
        p_SoftBody->f_WindUScale = LOA_ReadFloat(&_pc_Buffer);
	    p_SoftBody->f_WindVScale = LOA_ReadFloat(&_pc_Buffer);
	    p_SoftBody->f_WindUPan = LOA_ReadFloat(&_pc_Buffer);
	    p_SoftBody->f_WindVPan = LOA_ReadFloat(&_pc_Buffer);		

        if (ul_Version >= 2)
        {
            p_SoftBody->f_StaticFriction = LOA_ReadFloat(&_pc_Buffer);
            p_SoftBody->f_KineticFriction = LOA_ReadFloat(&_pc_Buffer);
        }

		if( ul_Version >= 5)
		{
			// tearable parameters
			p_SoftBody->f_DampLength = LOA_ReadFloat(&_pc_Buffer);
			p_SoftBody->f_InitialSpeed = LOA_ReadFloat(&_pc_Buffer);
			p_SoftBody->f_Acceleration = LOA_ReadFloat(&_pc_Buffer);			
			p_SoftBody->f_Angles[0] = LOA_ReadFloat(&_pc_Buffer);
			p_SoftBody->f_Angles[1] = LOA_ReadFloat(&_pc_Buffer);
			p_SoftBody->f_MaxSwing  = LOA_ReadFloat(&_pc_Buffer);

			//////////////////////////////////////////////////////////////////////////
			// Overwrite the values to be sure they have not been screwed by the artists
			//////////////////////////////////////////////////////////////////////////			
			p_SoftBody->f_DampLength = 6.0f;
			p_SoftBody->f_InitialSpeed = 1.5f;
			p_SoftBody->f_Acceleration = 3.0f;		
			p_SoftBody->f_Angles[0] = 20.0f;
			p_SoftBody->f_Angles[1] = 1.5f;
			p_SoftBody->f_MaxSwing = 1.5f;
		}

		if(ul_Version >= 6)
		{
			// dress parameters
			p_SoftBody->fatherChannel = LOA_ReadULong(&_pc_Buffer);
			ULONG ul_Key = LOA_ReadULong(&_pc_Buffer);
			p_SoftBody->actor = (OBJ_tdst_GameObject *) ul_Key;
			
			if((ULONG) p_SoftBody->actor && (ULONG) p_SoftBody->actor != BIG_C_InvalidKey)
			{
				LOA_MakeFileRef
					(
					(ULONG) p_SoftBody->actor,
					(ULONG *) &p_SoftBody->actor,
					OBJ_ul_GameObjectCallback,
					LOA_C_MustExists
					);
			}
			else
            {
            	p_SoftBody->actor = NULL;

            }

			p_SoftBody->f_DampingDistance = LOA_ReadFloat(&_pc_Buffer);
			p_SoftBody->f_BlendRatio = LOA_ReadFloat(&_pc_Buffer);
		}

        if (p_SoftBody->ul_NbVertices > 0)
        {
            p_SoftBody->a_Vertices = new CSoftBodyVertex[p_SoftBody->ul_NbVertices];

            for(i=0; i<p_SoftBody->ul_NbVertices; i++)
            {
                _pc_Buffer += p_SoftBody->a_Vertices[i].Load(_pc_Buffer);
				if(ul_Version >= 4)
				{
					if(p_SoftBody->a_Vertices[i].m_ulFlags & CSoftBodyVertex::eVertexFlagTearable )
					{
						p_SoftBody->a_Vertices[i].m_tearingID = LOA_ReadULong(&_pc_Buffer);
						p_SoftBody->a_Vertices[i].m_tearingVertex = NULL;
						p_SoftBody->ul_NbTearable++;
					}					
				}
            }

			if(ul_Version >= 4)
			{
				// link the tearable vertices together
				for(i=0; i<p_SoftBody->ul_NbVertices; i++)
				{
					CSoftBodyVertex *v = &p_SoftBody->a_Vertices[i];
					if( v->m_ulFlags & CSoftBodyVertex::eVertexFlagTearable && v->m_tearingVertex == NULL )
					{
						v->m_tearingVertex = &p_SoftBody->a_Vertices[v->m_tearingID];
						p_SoftBody->a_Vertices[v->m_tearingID].m_tearingVertex = v;						
					}
				}				
			}
        }

        if (p_SoftBody->ul_NbRods > 0)
        {
            p_SoftBody->a_Rods = new CSoftBodyLengthConstraint[p_SoftBody->ul_NbRods];

            for(i=0; i<p_SoftBody->ul_NbRods; i++)
            {
                _pc_Buffer += p_SoftBody->a_Rods[i].Load(_pc_Buffer);
            }
        }
        
        if (p_SoftBody->ul_NbCollPlane > 0)
        {
            p_SoftBody->a_CollPlane = new CSoftBodyPlaneConstraint[p_SoftBody->ul_NbCollPlane];

            for(i=0; i<p_SoftBody->ul_NbCollPlane; i++)
            {
                _pc_Buffer += p_SoftBody->a_CollPlane[i].Load(_pc_Buffer);
            }
        }
    }	

    p_SoftBody->pst_Hook = _pst_Mod->pst_GO;

#ifdef ACTIVE_EDITORS

    if (!(p_SoftBody->ul_Flags & SoftBody_C_NoCollisionActor))
    {
        if (!(OBJ_b_TestControlFlag(_pst_Mod->pst_GO, OBJ_C_ControlFlag_EnableSnP) &&
              OBJ_b_TestControlFlag(_pst_Mod->pst_GO, OBJ_C_ControlFlag_ForceDetectionList)))
        {
            ULONG ul_BakColor = LINK_gul_ColorTxt;
            LINK_gul_ColorTxt = 0x00a00000;
            LINK_PrintStatusMsg(ERR_szFormatMessage("%s [%08X]: Softbody is flagged to collide with actors but does not have the SNP flag, it WILL NOT collide with actors, please activate the SNP flag + save + checkin the gao if you want it to collide.  If you don't want it to collide, set the non collidable flag in the soft body modifier to remove this warning",
                                                    _pst_Mod->pst_GO->sz_Name, LOA_ul_GetCurrentKey()));
            LINK_gul_ColorTxt = ul_BakColor;
            ERR_gb_Warning = TRUE;

            // LOA_ul_GetCurrentKey
        }
    }

	p_SoftBody->ulCodeKey = 0xC0DE2001;
#endif


	return _pc_Buffer - pc_BufferSave;
}


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				19 Mar 2002
// 
// Prototype		GAO_ModifierSoftBody_UpdateWind
// Parameters		_p_SoftBody : 
//					_f_DT : 
// Return Type		void
// 
// Description		
// 
//------------------------------------------------------------------------
void GAO_ModifierSoftBody_UpdateWind(GAO_tdst_ModifierSoftBody * _p_SoftBody, FLOAT _f_DT)
{
    CSoftBodyVertex * p_Vertex;
    FLOAT             noisePoint[2];
    FLOAT             f_WindFactor;
    MATH_tdst_Vector  st_Strength;

    MATH_tdst_Vector  st_Wind;
    MATH_tdst_Vector  st_WindLocal;

    if (CWindManager::GetInstance()->GetWind(_p_SoftBody->pst_Hook, OBJ_pst_GetCurrentWind(_p_SoftBody->pst_Hook), &st_Wind))
    {
        MATH_TransformVector(&st_WindLocal, &_p_SoftBody->st_InvMatrix, &st_Wind);
        MATH_MulEqualTwoVectors(&st_WindLocal, &_p_SoftBody->st_WindMin);

        for(ULONG i = 0; i < _p_SoftBody->ul_NbVertices; i++)
        {
            p_Vertex = &_p_SoftBody->a_Vertices[i];

            if(!(p_Vertex->m_ulFlags & CSoftBodyVertex::eVertexFlagFixe))
            {
                noisePoint[0] = (_p_SoftBody->f_WindUScale * p_Vertex->m_u) + _p_SoftBody->f_WindUOri;
                noisePoint[1] = (_p_SoftBody->f_WindVScale * p_Vertex->m_v) + _p_SoftBody->f_WindVOri;

                f_WindFactor = g_PerlinNoiseMaker.noise2(noisePoint) * 0.5f + 0.5f;

                MATH_ScaleVector(&st_Strength, &st_WindLocal, f_WindFactor);
                MATH_AddEqualVector(&p_Vertex->m_Force, &st_Strength);
            }
        }

        _p_SoftBody->f_WindUOri += _p_SoftBody->f_WindUPan * _f_DT;
        if (_p_SoftBody->f_WindUOri > 512.0f)
        {
            _p_SoftBody->f_WindUOri -= 512.0f;
        }

        _p_SoftBody->f_WindVOri += _p_SoftBody->f_WindVPan * _f_DT;
        if (_p_SoftBody->f_WindVOri > 512.0f)
        {
            _p_SoftBody->f_WindVOri -= 512.0f;
        }
    }
    else
    {
        MATH_SubVector(&st_WindLocal, &_p_SoftBody->st_WindMax, &_p_SoftBody->st_WindMin);

        if (!MATH_b_NulVector(&st_WindLocal))
        {
            for(ULONG i = 0; i < _p_SoftBody->ul_NbVertices; i++)
            {
                p_Vertex = &_p_SoftBody->a_Vertices[i];

                if(!(p_Vertex->m_ulFlags & CSoftBodyVertex::eVertexFlagFixe))
                {
                    noisePoint[0] = (_p_SoftBody->f_WindUScale * p_Vertex->m_u) + _p_SoftBody->f_WindUOri;
                    noisePoint[1] = (_p_SoftBody->f_WindVScale * p_Vertex->m_v) + _p_SoftBody->f_WindVOri;

                    f_WindFactor = g_PerlinNoiseMaker.noise2(noisePoint) * 0.5f + 0.5f;

                    MATH_ScaleVector(&st_Strength, &st_WindLocal, f_WindFactor);
                    MATH_AddEqualVector(&st_Strength, &_p_SoftBody->st_WindMin);
                    MATH_AddEqualVector(&p_Vertex->m_Force, &st_Strength);
                }
            }

            _p_SoftBody->f_WindUOri += _p_SoftBody->f_WindUPan * _f_DT;
            if (_p_SoftBody->f_WindUOri > 512.0f)
            {
                _p_SoftBody->f_WindUOri -= 512.0f;
            }

            _p_SoftBody->f_WindVOri += _p_SoftBody->f_WindVPan * _f_DT;
            if (_p_SoftBody->f_WindVOri > 512.0f)
            {
                _p_SoftBody->f_WindVOri -= 512.0f;
            }
        }
    }
}


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				19 Mar 2002
// 
// Prototype		GAO_ModifierSoftBody_AccumulateForces
// Parameters		_p_SoftBody : 
// Return Type		void
// 
// Description		
// 
//------------------------------------------------------------------------
void GAO_ModifierSoftBody_AccumulateForces(GAO_tdst_ModifierSoftBody * _p_SoftBody)
{
    CSoftBodyVertex * p_Vertex;
    MATH_tdst_Vector  st_GravityLocal;

    // Transform gravity to soft body local space
    MATH_TransformVector(&st_GravityLocal, &_p_SoftBody->st_InvMatrix, &_p_SoftBody->st_Gravity);

	// Add gravity to accumulated force, this becomes the particle acceleration
	for(ULONG i = 0; i < _p_SoftBody->ul_NbVertices; i++)
	{
        p_Vertex = &_p_SoftBody->a_Vertices[i];		
        
		if(!(p_Vertex->m_ulFlags & CSoftBodyVertex::eVertexFlagFixe))
		{
            MATH_AddVector(&p_Vertex->m_Accel, &st_GravityLocal, &p_Vertex->m_Force);
			MATH_AddEqualVector(&p_Vertex->m_Accel, &p_Vertex->m_InstForce);

            MATH_InitVectorToZero(&p_Vertex->m_InstForce);
            MATH_InitVectorToZero(&p_Vertex->m_Force);
		}
	}	

	// Dress
	if(_p_SoftBody->actor != NULL && _p_SoftBody->fatherChannel != -1)
	{				
		if(_p_SoftBody->b_FirstInit)
		{
			MATH_CopyMatrix(&_p_SoftBody->previousGM,_p_SoftBody->pst_Hook->pst_GlobalMatrix);
			_p_SoftBody->b_FirstInit = FALSE;
		}

		MATH_tdst_Matrix blendMatrix;
		MATH_SetTranslationType(&blendMatrix);			
		MATH_MatrixBlend(&blendMatrix,&_p_SoftBody->previousGM,_p_SoftBody->pst_Hook->pst_GlobalMatrix,_p_SoftBody->f_BlendRatio,true);
		MATH_BlendVector(&blendMatrix.T,&_p_SoftBody->previousGM.T,&_p_SoftBody->pst_Hook->pst_GlobalMatrix->T,_p_SoftBody->f_BlendRatio);			

        float fDampingDistSqr = _p_SoftBody->f_DampingDistance * _p_SoftBody->f_DampingDistance;

		// Apply inverse transformation on all vertices
		for(ULONG i = 0; i < _p_SoftBody->ul_NbVertices; i++)
		{
			p_Vertex = &_p_SoftBody->a_Vertices[i];

			if(!(p_Vertex->m_ulFlags & CSoftBodyVertex::eVertexFlagFixe))
			{
					
				MATH_tdst_Vector v,globalCurPos;

				// transform current pos
				MATH_TransformVertex(&globalCurPos,&blendMatrix,p_Vertex->m_pCurrPos);
				MATH_TransformVertex(p_Vertex->m_pCurrPos,&_p_SoftBody->st_InvMatrix,&globalCurPos);

				// transform previous pos
				MATH_TransformVertex(&globalCurPos,&blendMatrix,&p_Vertex->m_PrevPos);
				MATH_TransformVertex(&p_Vertex->m_PrevPos,&_p_SoftBody->st_InvMatrix,&globalCurPos);

				// put damping in movement
				MATH_SubVector(&v,p_Vertex->m_pCurrPos,&p_Vertex->m_PrevPos);
				float distance = MATH_f_SqrNormVector(&v);
				if( distance > fDampingDistSqr)
				{
					float dampingRatio = (distance - fDampingDistSqr)/distance;
					MATH_ScaleEqualVector(&v,dampingRatio);
					MATH_AddEqualVector(&p_Vertex->m_PrevPos,&v);
				}
			}				
		}		

		MATH_CopyMatrix(&_p_SoftBody->previousGM,_p_SoftBody->pst_Hook->pst_GlobalMatrix);
	}
	
	/*
	// add the spring forces
   	for(int i=0;i<10;i++)
	{
		CSoftBodySpring *s = &_p_SoftBody->springs[i];
 		if(s->m_boneChannel != -1 && s->m_vertexID != -1)
		{ 		
 			CSoftBodyVertex *v = &_p_SoftBody->a_Vertices[s->m_vertexID];
			OBJ_tdst_GameObject *boneGao = s->GetBoneGao(_p_SoftBody);

			if(boneGao == NULL)
				continue;

 			MATH_tdst_Vector vVertexGlobal;
			MATH_tdst_Vector result,normalized,vBoneOffsetGlobal,temp;

			// transform local vertex position into global position
			MATH_TransformVertex(&vVertexGlobal,_p_SoftBody->pst_Hook->pst_GlobalMatrix,v->GetCurrPos());

			// transform bone offset local(bone space)->global
			MATH_TransformVertex(&vBoneOffsetGlobal,OBJ_pst_GetAbsoluteMatrix(boneGao),&s->m_boneOffset);
			
			// compute vector that goes from vertex to bone offset
			MATH_SubVector(&result,&vBoneOffsetGlobal,&vVertexGlobal);
			
			float length = MATH_f_NormVector(&result);
			float k = 100.0f;			

			// scale by force			
			MATH_ScaleVector(&result,&result,k);			
			MATH_TransformVector(&temp, &_p_SoftBody->st_InvMatrix, &result);
			MATH_AddEqualVector(&v->m_Accel, &temp);
		}
	}
	*/

}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				19 Mar 2002
// 
// Prototype		GAO_ModifierSoftBody_Solver
// Parameters		_p_SoftBody : 
//					_f_DT : 
// Return Type		void
// 
// Description		Verlet method : x2 = 2*x1 - x0 + a * dt^2
// 
//------------------------------------------------------------------------
void GAO_ModifierSoftBody_Solver(GAO_tdst_ModifierSoftBody * _p_SoftBody, FLOAT _f_DT)
{
    MATH_tdst_Vector   st_Tmp1;
    MATH_tdst_Vector   st_Tmp2;
    MATH_tdst_Vector * pst_Pos;
    CSoftBodyVertex  * p_Vertex;
    MATH_tdst_Matrix   st_MVertexxFixe;

    if (!(_p_SoftBody->ul_Flags & SoftBody_C_FullHierarchy)&&
        OBJ_b_TestIdentityFlag(_p_SoftBody->pst_Hook, OBJ_C_IdentityFlag_Hierarchy))
    {
        MATH_tdst_Matrix st_Tmp;
        OBJ_tdst_GameObject * pst_Father = OBJ_pst_GetFatherInit(_p_SoftBody->pst_Hook);

        MATH_MulMatrixMatrix(&st_Tmp, OBJ_pst_GetLocalMatrix(_p_SoftBody->pst_Hook), OBJ_pst_GetAbsoluteMatrix(pst_Father));
        MATH_MulMatrixMatrix(&st_MVertexxFixe, &st_Tmp, &_p_SoftBody->st_InvMatrix);
        MATH_Orthonormalize(&st_MVertexxFixe);

        // Remove father but keep father init.
        _p_SoftBody->pst_Hook->pst_Base->pst_Hierarchy->pst_Father = NULL;
    }

	for(ULONG i = 0; i < _p_SoftBody->ul_NbVertices; i++)
	{
        p_Vertex = &_p_SoftBody->a_Vertices[i];


		if(!(p_Vertex->m_ulFlags & CSoftBodyVertex::eVertexFlagFixe))
		{
			if(!(p_Vertex->m_ulFlags & CSoftBodyVertex::eVertexFlagComputed))
			{
				if (_p_SoftBody->ul_Flags & SoftBody_C_InitPrevPos)
				{
					MATH_CopyVector(&p_Vertex->m_PrevPos, p_Vertex->GetCurrPos());
				}			

				pst_Pos = p_Vertex->GetCurrPos();

				MATH_ScaleVector(&st_Tmp1, pst_Pos, 2.0f);
				MATH_SubEqualVector(&st_Tmp1, &p_Vertex->m_PrevPos);
				MATH_ScaleVector(&st_Tmp2, &p_Vertex->m_Accel, _f_DT*_f_DT);

				MATH_CopyVector(&p_Vertex->m_PrevPos, pst_Pos);
				MATH_AddVector(pst_Pos, &st_Tmp1, &st_Tmp2);				
				
				// update tearing vertex
				if(p_Vertex->m_ulFlags & CSoftBodyVertex::eVertexFlagTearable && !(p_Vertex->m_ulFlags & CSoftBodyVertex::eVertexFlagTeared) )
				{
					MATH_CopyVector(p_Vertex->m_tearingVertex->GetPrevPos(), &p_Vertex->m_PrevPos);
					MATH_CopyVector(p_Vertex->m_tearingVertex->GetCurrPos(), pst_Pos);
					p_Vertex->m_tearingVertex->m_ulFlags |= CSoftBodyVertex::eVertexFlagComputed;
				}
			}
		}
        else if (!(_p_SoftBody->ul_Flags & SoftBody_C_FullHierarchy)&&
                 OBJ_b_TestIdentityFlag(_p_SoftBody->pst_Hook, OBJ_C_IdentityFlag_Hierarchy))
        {
            if (_p_SoftBody->ul_Flags & SoftBody_C_InitPrevPos)
            {
                MATH_CopyVector(&p_Vertex->m_PrevPos, p_Vertex->GetCurrPos());
            }

            MATH_TransformVertex(p_Vertex->GetCurrPos(), &st_MVertexxFixe, &p_Vertex->m_PrevPos);
        }
	}

    _p_SoftBody->ul_Flags &= ~SoftBody_C_InitPrevPos;
}


 
//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				19 Mar 2002
// 
// Prototype		GAO_ModifierSoftBody_SatisfyConstraints
// Parameters		_p_SoftBody : 
// Return Type		void
// 
// Description		
// 
//------------------------------------------------------------------------
void GAO_ModifierSoftBody_SatisfyConstraints(GAO_tdst_ModifierSoftBody * _p_SoftBody)
{
	for(ULONG i = 0; i < _p_SoftBody->ul_NbIter; i++)
	{
        GAO_ModifierSoftBody_CollisionPlanes(_p_SoftBody);

        if (!(_p_SoftBody->ul_Flags & SoftBody_C_NoCollisionActor))
		    GAO_ModifierSoftBody_Collision(_p_SoftBody);

		GAO_ModifierSoftBody_Normalize(_p_SoftBody);
	}

    if (!(_p_SoftBody->ul_Flags & SoftBody_C_NoCollisionActor))
        GAO_ModifierSoftBody_Collision(_p_SoftBody);

    GAO_ModifierSoftBody_CollisionPlanes(_p_SoftBody);
}


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				19 Mar 2002
// 
// Prototype		GAO_ModifierSoftBody_Collision
// Parameters		_p_SoftBody : 
// Return Type		void
// 
// Description		
// 
//------------------------------------------------------------------------
void GAO_ModifierSoftBody_Collision(GAO_tdst_ModifierSoftBody * _p_SoftBody)
{
	OBJ_tdst_GameObject	*pst_GaoB;
    COL_tdst_Base       *pst_Col;		

    pst_Col = (COL_tdst_Base *) _p_SoftBody->pst_Hook->pst_Extended->pst_Col;

    if (pst_Col && pst_Col->pst_List && pst_Col->pst_List->ul_NbCollidedObjects > 0)
    {
        for(ULONG i=0; i<pst_Col->pst_List->ul_NbCollidedObjects; i++)
        {
            pst_GaoB = pst_Col->pst_List->dpst_CollidedObject[i];
            if (_p_SoftBody->pst_Hook != pst_GaoB)
            {
                if(TAB_b_IsAHole(pst_GaoB)) continue;

                if ( pst_GaoB != _p_SoftBody->actor && GAO_ModifierSoftBody_CollideGao(_p_SoftBody, pst_GaoB, TRUE ))
                {
                    _p_SoftBody->pst_CollidedGao = pst_GaoB;
                }
            }
        }
    }

    if( _p_SoftBody->actor != NULL )
    {
        // The actor may have col maps associated to its bones that need to be collided against also
        ERR_X_Assert( _p_SoftBody->actor != NULL );
        if( OBJ_b_TestIdentityFlag(_p_SoftBody->actor,  OBJ_C_IdentityFlag_Anims) )
        {
            ERR_X_Assert( _p_SoftBody->actor->pst_Base != NULL && _p_SoftBody->actor->pst_Base->pst_GameObjectAnim != NULL );

            OBJ_tdst_Group * pSkeleton = _p_SoftBody->actor->pst_Base->pst_GameObjectAnim->pst_Skeleton;
            if( pSkeleton != NULL )
            {
                TAB_tdst_PFtable * pTable = pSkeleton->pst_AllObjects;
                if( pTable != NULL )
                {
                    TAB_tdst_PFelem * pst_Elem      = TAB_pst_PFtable_GetFirstElem( pTable );
                    TAB_tdst_PFelem * pst_LastElem  = TAB_pst_PFtable_GetLastElem( pTable );
                    for( ;pst_Elem <= pst_LastElem; ++pst_Elem )
                    {
                        OBJ_tdst_GameObject * pBoneGao = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
                        ERR_X_Assert( pBoneGao != NULL );

                        if( TAB_b_IsAHole(pBoneGao) )
                        {
                            continue;
                        }

                        if( GAO_ModifierSoftBody_CollideGao(_p_SoftBody, pBoneGao, FALSE ) )
                        {
                            _p_SoftBody->pst_CollidedGao = pst_GaoB;
                        }
                    }
                }
            }
        }
    }
	
	// spring code
	/*
	MATH_tdst_Matrix m;
	MATH_InvertMatrix(&m,_p_SoftBody->pst_Hook->pst_GlobalMatrix);

	for(int i=0;i<4;i++)
	{
		OBJ_tdst_GameObject *gao = _p_SoftBody->pst_Colliders[i] ;
		if(gao != NULL)
		{
			INT_tdst_Sphere s;
			MATH_tdst_Vector v0;

			MATH_TransformVertex(&v0,gao->pst_GlobalMatrix,OBJ_pst_BV_GetCenter(gao->pst_BV));
			MATH_TransformVertex(&s.st_Center,&m,&v0);

			s.f_Radius = OBJ_f_BV_GetRadius(gao->pst_BV);
			
			GAO_ModifierSoftBody_CollideSphere(_p_SoftBody,&s);
		}
	}
	*/
}

void GAO_ModifierSoftBody_CollisionPlanes(GAO_tdst_ModifierSoftBody * _p_SoftBody)
{
    ULONG              i, j;
    FLOAT              dist;
    CSoftBodyVertex  * p_Vertex;
    CPlane           * p_Plane;
    MATH_tdst_Vector   st_Tmp;
    MATH_tdst_Vector * pst_Pos;
    MATH_tdst_Vector   st_Normal;

    for(j = 0; j < _p_SoftBody->ul_NbCollPlane; j++)
    {
        p_Plane = &(_p_SoftBody->a_CollPlane[j].m_LocalPlane);

        for(i = 0; i < _p_SoftBody->ul_NbVertices; i++)
        {
            p_Vertex = &_p_SoftBody->a_Vertices[i];

            if(!(p_Vertex->m_ulFlags & CSoftBodyVertex::eVertexFlagFixe))
            {
                pst_Pos = p_Vertex->GetCurrPos();

                dist = p_Plane->PlaneDot(pst_Pos);
                if(dist < 0.0f /*&& dist > -0.1f*/)
                {
                    MATH_InitVector(&st_Normal, p_Plane->X, p_Plane->Y, p_Plane->Z);
                    MATH_ScaleVector(&st_Tmp, &st_Normal, -dist);

                    MATH_AddEqualVector(pst_Pos, &st_Tmp);

                    GAO_ModifierSoftBody_Friction(_p_SoftBody, p_Vertex, &st_Normal);
                    //MATH_CopyVector(&p_Vertex->m_PrevPos, pst_Pos);
                }
            }
        }
    }
}


// if any new bugs concern softbody, please tell me.
#ifdef PS2_RENDER_NEW
asm void ASM_ModifierSoftBody_Normalize_Aligned(
			int NbRods, 						// a0.
			int	NbVertices,						// a1.
			int NbNormalize, 					// a2.
			int LengthOfSoftBodyVertex, 		// a3.
			CSoftBodyLengthConstraint * a_Rods, // t0.
			CSoftBodyVertex * a_Vertices,		// t1.
			u_int DriverStart)                  // t2.
{
// vector aligned by 16bytes.
	.set noreorder
	beq 	a0, $zero, NormalizeEnd
	addi	t7, zero, 0x20
	beq 	a2, $zero, NormalizeEnd
	ctc2 	t7, vi01

	ctc2 	$t2, vi27
	
	ctc2	a0, vi02							// NbRods
	ctc2	a2, vi03							// NbNormalize
	ctc2	a1, vi04							// NbVertices
	add		t5, a1, zero
	add		t6, t1, zero

CopyNormalLoop:
	lw		t3, 0(t1)
	lw 		t2, 8(t1)
	addi 	a1, a1, -1
	add		t1, t1, a3
	andi	t3, t3, 0x01
	ctc2	t3, vi05
	lqc2	vf01, 0(t2)
	viswr.w vi05, (vi01)
	bne 	a1, zero, CopyNormalLoop
	vsqi.xyz vf01, (vi01++)
	
CopyRodsLoop:
	lq		t2, 0(t0)
	lq		t3, 16(t0)
	mtsab	t0, 0
	addi 	a0, a0, -1
	qfsrv	t4, t3, t2
	qmtc2	t4, vf01
	addiu	t0, t0, 12
	bne		a0, zero, CopyRodsLoop
	vsqi	vf01, (vi01++)
	
	vcallmsr vi27
	vnop

	ctc2.i	t7, vi01
CopyNormalBackLoop:
	lw		t2, 8(t6)
	vlqi	vf01, (vi01++)

	// Tearable check & copy for current vertex
	lw		$t0, 0(t6)
	andi	$t3, $t0, 2
	beq		$t3, zero, NoCopyNeeded
	nop
	andi	$t3, $t0, 4
	bne		$t3, zero, NoCopyNeeded
	nop

	// Tearable and not teared, copy
	lw		$t3, 12(t6)
	lw		$t3, 8(t3)
	sqc2	vf01, 0(t3)

NoCopyNeeded:		
	addi	t5, t5, -1
	add		t6, t6, a3
	bne		t5, zero, CopyNormalBackLoop
	sqc2	vf01, 0(t2)
	
NormalizeEnd:	
	jr 	ra
	nop
	
	.set reorder
}

asm void ASM_ModifierSoftBody_Normalize_NotAligned(
			int NbRods, 						// a0. -> _p_SoftBody->ul_NbRods
			int NbNormalize, 					// a1. -> _p_SoftBody->ul_NbNormalizeIter
			int LengthOfSoftBodyVertex, 		// a2. -> sizeof(CSoftBodyVertex)
			CSoftBodyLengthConstraint * a_Rods, // a3. -> _p_SoftBody->a_Rods
			CSoftBodyVertex * a_Vertices)		// t0. -> _p_SoftBody->a_Vertices
{
	.set noreorder

	beq a1, $zero, NormalizeEnd
	lui $t0, 0x3f00
	
	beq a0, $zero, NormalizeEnd
	qmtc2 $t0, vf05				// send 0.5f to vf05x

	add $v0, $t0, $zero

NormalizeIter_loop:
	addi a1, a1, -1
	add $t8, $zero, $zero
	add $t9, $zero, a3
Rods_loop:	
	add $t8, $t8, 1
Main_loop:
	lw	$t0, 0(t9)				// v1_idx
	lw 	$t1, 4(t9)				// v2_idx
	lw	$t7, 8(t9)				// length
	mul $t0, $t0, a2
	mul $t1, $t1, a2			
	add $t0, $t0, $v0			// v1
	add $t1, $t1, $v0			// v2
	lw  $t2, 0(t0)
	lw  $t3, 0(t1)				
	lw 	$t0, 8($t0)
	lw	$t1, 8($t1)
	lw  $t4, 0(t0)
	lw  $t5, 4(t0)
	lw  $t6, 8(t0)
	and $t2, $t2, 0x01			// flg1
	ppacw	t4, t5, t4
	ppacw	t5, t7, t6
	lw	$t6, 0(t1)
	lw	$t7, 4(t1)
	and $t3, $t3, 0x01			// flg2
	ppacw	t4, t5, t4
	lw 	$t5, 8(t1)
	ppacw	t6, t7, t6
	qmtc2 $t4, vf01
	ppacw	t5, $zero, t5
	ppacw	t5, t5, t6
	mtsab 	zero, 4
	qmtc2 $t5, vf02
	add t4, t2, t3
	
	vsub vf03, vf02, vf01
	vmul vf04, vf03, vf03
	vaddy.x vf04, vf04, vf04
	vaddz.x vf04, vf04, vf04
	vsqrt Q, vf04x
	vwaitq
	vaddq.x vf04, vf00, Q
	vdiv Q, vf00w, vf04x
	vsubw.x vf04x, vf04x, vf01w 
	vmulx vf03, vf03, vf04x
	
	bne $t4, $zero, Rods_normal1
	nop
	vmulx vf03, vf03, vf05x	
	vwaitq
	vmulq vf03, vf03, Q
	
	vadd.xyz vf01, vf01, vf03
	vsub.xyz vf02, vf02, vf03	

	qmfc2 $t6, vf01
	qmfc2 $t7, vf02	
	qfsrv	t2, zero, t6
	qfsrv	t4, zero, t7
	qfsrv	t3, zero, t2
	qfsrv	t5, zero, t4
	sw		t6, 0(t0)
	sw		t2, 4(t0)
	sw		t3, 8(t0)
	sw		t7, 0(t1)
	sw		t4, 4(t1)
	sw		t5, 8(t1)	
	b Rods_End
	nop
	
Rods_normal1:
	vwaitq
	vmulq vf03, vf03, Q

	bne $t2, $zero, Rods_normal2
	nop
	
	vadd.xyz vf01, vf01, vf03
	
	qmfc2	$t6, vf01
	qfsrv	t2, zero, t6
	qfsrv	t3, zero, t2
	sw		t6, 0(t0)
	sw		t2, 4(t0)
	sw		t3, 8(t0)
	
	b Rods_End
	nop	
	
Rods_normal2:	
	vsub.xyz vf02, vf02, vf03	

	qmfc2 $t7, vf02
	qfsrv	t4, zero, t7
	qfsrv	t5, zero, t4
	sw		t7, 0(t1)
	sw		t4, 4(t1)
	sw		t5, 8(t1)	
	
Rods_End:

	// Tearable check & copy for vertex 1
CheckTearable1:
	lw		$t0, 0(t9)
	lw		$t1, 4(t9)
	lw		$t2, 0(t0)
	andi	$t3, $t2, 2
	beq		$t3, zero, CheckTearable2
	nop
	andi	$t3, $t2, 4
	bne		$t3, zero, CheckTearable2
	nop

	// Copy the vector p_V1->m_pCurrPos to m_tearingVertex->m_pCurrPos
	lw		$t0, 0(t9)			// p_Rod->m_v1
	mul		$t0, $t0, a2		// Offset in a_Vertices
	add		$t0, $t0, $v0		// p_V1
	lw		$t1, 12(t0)			// m_tearingVertex
	lw		$t0, 8(t0)			// p_V1->m_pCurrPos
	lw		$t1, 8(t1)			// m_tearingVertex->m_pCurrPos
	lw		t2, 0(t0)			// Read and copy all components
	lw		t3, 4(t0)
	lw		t4, 8(t0)
	sw		t2, 0(t1)
	sw		t3, 4(t1)
	sw		t4, 8(t1)	

	// Tearable check & copy for vertex 2
CheckTearable2:
	lw		$t2, 0(t1)
	andi	$t3, $t2, 2
	beq		$t3, zero, Rods_FinalizeLoop
	nop
	andi	$t3, $t2, 4
	bne		$t3, zero, Rods_FinalizeLoop
	nop

	// Copy the vector p_V2->m_pCurrPos to m_tearingVertex->m_pCurrPos
	lw		$t0, 4(t9)			// p_Rod->m_v2
	mul		$t0, $t0, a2		// Offset in a_Vertices
	add		$t0, $t0, $v0		// p_V2
	lw		$t1, 12($t0)		// m_tearingVertex
	lw		$t0, 8($t0)			// p_V2->m_pCurrPos
	lw		$t1, 8($t1)			// m_tearingVertex->m_pCurrPos
	lw		t2, 0($t0)			// Read and copy all components
	lw		t3, 4($t0)
	lw		t4, 8($t0)
	sw		t2, 0($t1)
	sw		t3, 4($t1)
	sw		t4, 8($t1)

Rods_FinalizeLoop:
	add t9, t9, 12
	bne $t8, a0, Rods_loop
	nop
	
	bne a1, $zero, NormalizeIter_loop
	nop
	
NormalizeEnd:	
	jr ra
	nop
}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				19 Mar 2002
// 
// Prototype		GAO_ModifierSoftBody_Normalize
// Parameters		_p_SoftBody : 
// Return Type		void
// 
// Description		
// 
//------------------------------------------------------------------------
void GAO_ModifierSoftBody_Normalize(GAO_tdst_ModifierSoftBody * _p_SoftBody)
{
	GCurrentLightsBufferIsUsable = 0;
	if(_p_SoftBody->ul_NbVertices + _p_SoftBody->ul_NbRods >= 256 - 32) 
	{
		ASM_ModifierSoftBody_Normalize_NotAligned(_p_SoftBody->ul_NbRods, 
												_p_SoftBody->ul_NbNormalizeIter, 
												sizeof(CSoftBodyVertex), 
												_p_SoftBody->a_Rods, 
												_p_SoftBody->a_Vertices
												);
	}
	else		 
	{	
		ASM_ModifierSoftBody_Normalize_Aligned(_p_SoftBody->ul_NbRods, 
												_p_SoftBody->ul_NbVertices, 
												_p_SoftBody->ul_NbNormalizeIter, 
												sizeof(CSoftBodyVertex), 
												_p_SoftBody->a_Rods, 
												_p_SoftBody->a_Vertices, 
												GSoftBodyNormalizeStart
												);
	}
}

#else

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				19 Mar 2002
// 
// Prototype		GAO_ModifierSoftBody_Normalize
// Parameters		_p_SoftBody : 
// Return Type		void
// 
// Description		
// 
//------------------------------------------------------------------------
void GAO_ModifierSoftBody_Normalize(GAO_tdst_ModifierSoftBody * _p_SoftBody)
{
    CSoftBodyLengthConstraint * p_Rod;
    CSoftBodyVertex           * p_V1;
    CSoftBodyVertex           * p_V2;
    MATH_tdst_Vector            Dir;
    MATH_tdst_Vector          * pst_Pos1;
    MATH_tdst_Vector          * pst_Pos2;
    FLOAT                       f_Length;

    for(ULONG i = 0; i < _p_SoftBody->ul_NbNormalizeIter; i++)
	{
		p_Rod = _p_SoftBody->a_Rods;
		
		for(ULONG j = 0; j < _p_SoftBody->ul_NbRods; j++, p_Rod++)
		{			
			// each rod has 2 vertices
			p_V1 = &_p_SoftBody->a_Vertices[p_Rod->m_v1];
            p_V2 = &_p_SoftBody->a_Vertices[p_Rod->m_v2];

            pst_Pos1 = p_V1->GetCurrPos();
            pst_Pos2 = p_V2->GetCurrPos();			
			
			// find direction vector
            MATH_SubVector(&Dir, pst_Pos2, pst_Pos1);

			// find length, expansive call
			f_Length = MATH_f_NormVector(&Dir);

			if(f_Length > 0.0f)
			{
				// if we can move both vertices, move each vertices toward each other
				if(!(p_V1->m_ulFlags & CSoftBodyVertex::eVertexFlagFixe) && !(p_V2->m_ulFlags & CSoftBodyVertex::eVertexFlagFixe))
				{					
					MATH_ScaleEqualVector(&Dir, ((f_Length - p_Rod->m_fLength) * 0.5f / f_Length));

                    MATH_AddEqualVector(pst_Pos1, &Dir);
                    MATH_SubEqualVector(pst_Pos2, &Dir);
				}
				// if one of the 2 vertex is fixed, move the free one toward the other one
				else
				{
                    MATH_ScaleEqualVector(&Dir, ((f_Length - p_Rod->m_fLength) / f_Length));

					if(!(p_V1->m_ulFlags & CSoftBodyVertex::eVertexFlagFixe))
                        MATH_AddEqualVector(pst_Pos1, &Dir);
					else
                        MATH_SubEqualVector(pst_Pos2, &Dir);
				}
				
				if(p_V1->m_ulFlags & CSoftBodyVertex::eVertexFlagTearable && !(p_V1->m_ulFlags & CSoftBodyVertex::eVertexFlagTeared))
				{
					MATH_CopyVector(p_V1->m_tearingVertex->m_pCurrPos,pst_Pos1);
				}

				if(p_V2->m_ulFlags & CSoftBodyVertex::eVertexFlagTearable && !(p_V2->m_ulFlags & CSoftBodyVertex::eVertexFlagTeared))
				{
					MATH_CopyVector(p_V2->m_tearingVertex->m_pCurrPos,pst_Pos2);
				}				

			}
		}
	}
}

#endif

//------------------------------------------------------------------------
//
// Author			Charles Jacob
// Date				13 jan 2004
// 
// Prototype		GAO_ModifierSoftBody_Tear
// Parameters		void
// Return Type		void
// 
// Description		Detach all the tearable vertices.
// 
//------------------------------------------------------------------------
void GAO_ModifierSoftBody_Tear(GAO_tdst_ModifierSoftBody * p_SoftBody)
{	
	for(unsigned int i=0; i<p_SoftBody->ul_NbVertices; i++)
	{
		if(p_SoftBody->a_Vertices[i].m_ulFlags & CSoftBodyVertex::eVertexFlagTearable)
		{
			p_SoftBody->a_Vertices[i].m_ulFlags |= CSoftBodyVertex::eVertexFlagTeared;
		}
	}
}

void GAO_ModifierSoftBody_Untear(GAO_tdst_ModifierSoftBody *p_SoftBody)
{
	for(unsigned int i=0; i<p_SoftBody->ul_NbVertices; i++)
	{
		if(p_SoftBody->a_Vertices[i].m_ulFlags & CSoftBodyVertex::eVertexFlagTearable)
		{
			p_SoftBody->a_Vertices[i].m_ulFlags &= ~CSoftBodyVertex::eVertexFlagTeared;
		}
	}
}

BOOL GAO_ModifierSoftBody_CollideGao(GAO_tdst_ModifierSoftBody * _p_SoftBody, OBJ_tdst_GameObject * _pst_Gao, BOOL _bCheckBV )
{
    BOOL                 b_Collision = FALSE;
    UCHAR                uc_Index;
    COL_tdst_ColMap     *pst_ColMap;
    COL_tdst_Cob       **dpst_Cob;
    COL_tdst_Cob       **dpst_LastCob;

    if (_pst_Gao == NULL)
        return FALSE;

    if (_pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap)
    {
        pst_ColMap = ((COL_tdst_Base *) _pst_Gao->pst_Extended->pst_Col)->pst_ColMap;
        if( !pst_ColMap )
        {
            return FALSE;
        }
        
        // TODO KK:
        //if (!(_pst_Gao->pst_Extended->uw_Capacities & OBJ_C_CapacityFlag_CollideWithSoftBody))
        //    return FALSE;

        if( _bCheckBV )
        {
            // TODO : This should be done only one per frame
            MATH_tdst_Vector st_MyBVMax = *OBJ_pst_BV_GetGMax(_p_SoftBody->pst_Hook->pst_BV);
            MATH_tdst_Vector st_MyBVMin = *OBJ_pst_BV_GetGMin(_p_SoftBody->pst_Hook->pst_BV);
            MATH_AddEqualVector(&st_MyBVMax, OBJ_pst_GetAbsolutePosition(_p_SoftBody->pst_Hook));
            MATH_AddEqualVector(&st_MyBVMin, OBJ_pst_GetAbsolutePosition(_p_SoftBody->pst_Hook));

            // BV Culling Test
            if(OBJ_BV_IsAABBox(_pst_Gao->pst_BV))
            {
                MATH_tdst_Vector st_BVMax = *OBJ_pst_BV_GetGMax(_pst_Gao->pst_BV);
                MATH_tdst_Vector st_BVMin = *OBJ_pst_BV_GetGMin(_pst_Gao->pst_BV);

                MATH_AddEqualVector(&st_BVMax, OBJ_pst_GetAbsolutePosition(_pst_Gao));
                MATH_AddEqualVector(&st_BVMin, OBJ_pst_GetAbsolutePosition(_pst_Gao));

                if(!INT_AABBoxAABBox( &st_MyBVMin, &st_MyBVMax, &st_BVMin, &st_BVMax))
                {
                    return FALSE;
                }
            }
            else
            {
                FLOAT            f_GlobalRadius;
                MATH_tdst_Vector st_GlobalCenter;

                MATH_TransformVertex(&st_GlobalCenter, _pst_Gao->pst_GlobalMatrix, OBJ_pst_BV_GetCenter(_pst_Gao->pst_BV));
                f_GlobalRadius = OBJ_f_BV_GetRadius(_pst_Gao->pst_BV) * MATH_GetMaxScale(_pst_Gao->pst_GlobalMatrix);

                if(!INT_SphereAABBox( &st_GlobalCenter, f_GlobalRadius, &st_MyBVMin, &st_MyBVMax))
                {
                    return FALSE;
                }
            }
        }

        // Collide with every collision object in the map
        dpst_Cob = pst_ColMap->dpst_Cob;
        dpst_LastCob = dpst_Cob + pst_ColMap->uc_NbOfCob;
        
        for(uc_Index = 0; dpst_Cob < dpst_LastCob; dpst_Cob++, uc_Index++)
        {
            if(COL_b_ColMap_IsActive(pst_ColMap, uc_Index))
            {
                switch((*dpst_Cob)->uc_Type)
                {
                case COL_C_Zone_Sphere:
                    {
                        MATH_tdst_Vector st_Center;
                        INT_tdst_Sphere st_Sphere;
                                  
                        MATH_TransformVertex(&st_Center, _pst_Gao->pst_GlobalMatrix,
                                             COL_pst_Shape_GetCenter((*dpst_Cob)->pst_MathCob->p_Shape));
                        MATH_TransformVertex(&st_Sphere.st_Center, &_p_SoftBody->st_InvMatrix, &st_Center);

                        if(MATH_b_TestScaleType(_pst_Gao->pst_GlobalMatrix))
                        {
                            st_Sphere.f_Radius = MATH_GetMaxScale(_pst_Gao->pst_GlobalMatrix)
                                               * COL_f_Shape_GetRadius((*dpst_Cob)->pst_MathCob->p_Shape);
                        }
                        else
                        {
                            st_Sphere.f_Radius = COL_f_Shape_GetRadius((*dpst_Cob)->pst_MathCob->p_Shape);
                        }

                        if (GAO_ModifierSoftBody_CollideSphere(_p_SoftBody, &st_Sphere))
                            b_Collision = TRUE;
                    }
                    break;
                    
                case COL_C_Zone_Box:
                    {
                        MATH_tdst_Matrix st_BoxMatrix;

                        MATH_MulMatrixMatrix(&st_BoxMatrix, _pst_Gao->pst_GlobalMatrix, &_p_SoftBody->st_InvMatrix);
                        MATH_tdst_Vector * pst_A_Max = COL_pst_Shape_GetMax((*dpst_Cob)->pst_MathCob->p_Shape);
                        MATH_tdst_Vector * pst_A_Min = COL_pst_Shape_GetMin((*dpst_Cob)->pst_MathCob->p_Shape);

                        if (GAO_ModifierSoftBody_CollideBox(_p_SoftBody, &st_BoxMatrix, pst_A_Min, pst_A_Max))
                            b_Collision = TRUE;
                            
                    }
                    break;
                    
                case COL_C_Zone_Cylinder:
                    {	
                        // patch-o-grement:
						// no collision for tearable soft body
						// otherwise the prince collide on approching soft body and make it very unstable
						if(_p_SoftBody->b_IsTearable)
							return FALSE;

                        MATH_tdst_Vector  st_Center;
                        COL_tdst_Cylinder st_Cylinder;

                        if(OBJ_b_TestIdentityFlag(_pst_Gao, OBJ_C_IdentityFlag_Dyna))
                        {
                            MATH_tdst_Vector st_Dist;
                            FLOAT            f_Dist;

                            DYN_GetSpeedVector(_pst_Gao->pst_Base->pst_Dyna, &st_Dist);
                            MATH_ScaleEqualVector(&st_Dist, TIM_gf_dt);

                            f_Dist = MATH_f_NormVector(&st_Dist);

                            MATH_TransformVertex(&st_Center, _pst_Gao->pst_GlobalMatrix,
                                                 COL_pst_Shape_GetCenter((*dpst_Cob)->pst_MathCob->p_Shape));

                            MATH_SubEqualVector(&st_Center, &st_Dist);
                            MATH_TransformVertex(&st_Cylinder.st_Center, &_p_SoftBody->st_InvMatrix, &st_Center);

                            st_Cylinder.f_Radius = f_Dist + COL_f_Shape_GetRadius((*dpst_Cob)->pst_MathCob->p_Shape);
                        }
                        else
                        {
                            MATH_TransformVertex(&st_Center, _pst_Gao->pst_GlobalMatrix,
                                                 COL_pst_Shape_GetCenter((*dpst_Cob)->pst_MathCob->p_Shape));
                            MATH_TransformVertex(&st_Cylinder.st_Center, &_p_SoftBody->st_InvMatrix, &st_Center);
                            st_Cylinder.f_Radius = COL_f_Shape_GetRadius((*dpst_Cob)->pst_MathCob->p_Shape);
                        }

                        st_Cylinder.f_Height = COL_f_Shape_GetHeight((*dpst_Cob)->pst_MathCob->p_Shape);
                        

                        if (GAO_ModifierSoftBody_CollideCylinder(_p_SoftBody, &st_Cylinder))
                            b_Collision = TRUE;
                    }
                    break;
                    
                case COL_C_Zone_Triangles:
                    {
                        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
                        COL_tdst_IndexedTriangles	*pst_B_Geo;
                        COL_tdst_ElementIndexedTriangles *pst_Element;
                        COL_tdst_ElementIndexedTriangles *pst_LastElement;
                        ULONG								ul_Element;
                        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

                        pst_B_Geo = (*dpst_Cob)->pst_TriangleCob;
                        pst_Element = pst_B_Geo->dst_Element;
                        pst_LastElement = (pst_Element + pst_B_Geo->l_NbElements);

                        if((*dpst_Cob)->pst_TriangleCob->pst_OK3)
                        {
                            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
                            COL_tdst_OK3						*pst_OK3;
                            COL_tdst_OK3_Box					*pst_Box, *pst_LastBox;
                            COL_tdst_OK3_Element				*pst_OK3_Element, *pst_LastElement;
                            COL_tdst_ElementIndexedTriangles	*pst_Cob_Element;
                            ULONG								ul_Save;
                            int									i;
                            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

                            pst_OK3 = (*dpst_Cob)->pst_TriangleCob->pst_OK3;
                            pst_Box = pst_OK3->pst_OK3_Boxes;
                            pst_LastBox = pst_Box + pst_OK3->ul_NumBox;

                            for(; pst_Box < pst_LastBox; pst_Box++)
                            {
                                /*Step-1
                                if(!INT_GlobalPointAABBox()
                                    (
                                    &_pst_GlobalVars->st_A_BCS_DynamicCenter,
                                    _pst_GlobalVars->f_A_GCS_DynamicRadius,
                                    &pst_Box->st_Min,
                                    &pst_Box->st_Max
                                    )
                                    )
                                {
                                    continue;
                                }*/

                                pst_OK3_Element = pst_Box->pst_OK3_Element;
                                pst_LastElement = pst_OK3_Element + pst_Box->ul_NumElement;

                                for(; pst_OK3_Element < pst_LastElement; pst_OK3_Element++)
                                {
                                    pst_Cob_Element = &(*dpst_Cob)->pst_TriangleCob->dst_Element[pst_OK3_Element->uw_Element];

                                    for(ul_Save = 0, i = 0; i < pst_OK3_Element->uw_Element; i++)
                                    {
                                        ul_Save += (*dpst_Cob)->pst_TriangleCob->dst_Element[i].uw_NbTriangles;;
                                    }

                                    pst_B_Geo = (*dpst_Cob)->pst_TriangleCob;

                                    if (GAO_ModifierSoftBody_CollideTriangles(_p_SoftBody, _pst_Gao->pst_GlobalMatrix,
                                                                              pst_OK3_Element->uw_NumTriangle,
                                                                              pst_OK3_Element->puw_OK3_Triangle,
                                                                              pst_Cob_Element->dst_Triangle,
                                                                              pst_B_Geo->dst_Point,
                                                                              ((*dpst_Cob)->pst_TriangleCob->dst_FaceNormal + ul_Save)))
                                    {
                                        b_Collision = TRUE;
                                    }
                                }
                            }
                        }
                        else
                        {
                            // We go thru all the Elements of the Geometric object.
                            for(ul_Element = 0; pst_Element < pst_LastElement; pst_Element++, ul_Element++)
                            {
                                pst_B_Geo = (*dpst_Cob)->pst_TriangleCob;

                                if (GAO_ModifierSoftBody_CollideTriangles(_p_SoftBody, _pst_Gao->pst_GlobalMatrix,
                                                                          pst_Element->uw_NbTriangles, NULL,
                                                                          pst_Element->dst_Triangle,
                                                                          pst_B_Geo->dst_Point,
                                                                          (*dpst_Cob)->pst_TriangleCob->dst_FaceNormal))
                                {
                                    b_Collision = TRUE;
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }
    }

    return b_Collision;
}

BOOL GAO_ModifierSoftBody_CollideSphere(GAO_tdst_ModifierSoftBody * _p_SoftBody, INT_tdst_Sphere * _pst_Sphere)
{
    BOOL               b_Collision = FALSE;
    ULONG              ul_Counter;
    FLOAT              f_Dist;
    MATH_tdst_Vector   st_Direction;
    CSoftBodyVertex  * p_V;

    //ERR_OutputDebugString("-----\nsphere %f,%f,%f %f\n", _pst_Sphere->st_Center.x, _pst_Sphere->st_Center.y,
    //                      _pst_Sphere->st_Center.z, _pst_Sphere->f_Radius);

    for( ul_Counter = 0; ul_Counter < _p_SoftBody->ul_NbVertices; ul_Counter++ )
    {
        p_V = &_p_SoftBody->a_Vertices[ul_Counter];

        //p_V->Print();

        if( !(p_V->m_ulFlags & CSoftBodyVertex::eVertexFlagFixe) )
        {
            MATH_SubVector(&st_Direction, p_V->GetCurrPos(), &_pst_Sphere->st_Center);
            f_Dist = MATH_f_NormVector(&st_Direction);

            if (f_Dist < _pst_Sphere->f_Radius && f_Dist > 0.0f)
            {
                MATH_AddScaleVector(p_V->GetCurrPos(), &_pst_Sphere->st_Center,
                                    &st_Direction, _pst_Sphere->f_Radius/f_Dist); 

                MATH_CopyVector(&p_V->m_PrevPos, p_V->GetCurrPos());
                b_Collision = TRUE;
            }
        }
    }

    return b_Collision;
}

BOOL GAO_ModifierSoftBody_CollideBox(GAO_tdst_ModifierSoftBody * _p_SoftBody, MATH_tdst_Matrix * _pst_MatrixBox, MATH_tdst_Vector * _pst_LocalMin, MATH_tdst_Vector * _pst_LocalMax)
{
    BOOL               b_Collision = FALSE;
    ULONG              ul_Counter;
    FLOAT              f_DistMin;
    FLOAT              f_Dist;
    MATH_tdst_Vector   st_LocalPoint;
    MATH_tdst_Vector   st_HitPoint;
    MATH_tdst_Vector   st_Normal;
    MATH_tdst_Matrix   st_InvMatrixBox;
    CSoftBodyVertex  * p_V;

    MATH_InvertMatrix(&st_InvMatrixBox, _pst_MatrixBox);

    for(ul_Counter = 0; ul_Counter < _p_SoftBody->ul_NbVertices; ul_Counter++)
    {
        p_V = &_p_SoftBody->a_Vertices[ul_Counter];

        if (!(p_V->m_ulFlags & CSoftBodyVertex::eVertexFlagFixe))
        {
            MATH_TransformVertex(&st_LocalPoint, &st_InvMatrixBox, p_V->GetCurrPos());

            if ((st_LocalPoint.x > _pst_LocalMin->x) &&
                (st_LocalPoint.x < _pst_LocalMax->x) &&
                (st_LocalPoint.y > _pst_LocalMin->y) &&
                (st_LocalPoint.y < _pst_LocalMax->y) &&
                (st_LocalPoint.z > _pst_LocalMin->z) &&
                (st_LocalPoint.z < _pst_LocalMax->z))
            {
                f_DistMin  =   st_LocalPoint.x - _pst_LocalMin->x;
                f_Dist     = - st_LocalPoint.x + _pst_LocalMax->x;

                if (f_Dist < f_DistMin)
                {
                    f_DistMin = f_Dist;
                    MATH_SetVector(&st_HitPoint, _pst_LocalMax->x, st_LocalPoint.y, st_LocalPoint.z);
                    MATH_SetVector(&st_Normal, 1.0f, 0.0f, 0.0f);
                }
                else
                {
                    MATH_SetVector(&st_HitPoint, _pst_LocalMin->x, st_LocalPoint.y, st_LocalPoint.z);
                    MATH_SetVector(&st_Normal, -1.0f, 0.0f, 0.0f);
                }

                f_Dist =   st_LocalPoint.y - _pst_LocalMin->y;
                
                if (f_Dist < f_DistMin)
                {
                    f_DistMin = f_Dist;
                    MATH_SetVector(&st_HitPoint, st_LocalPoint.x, _pst_LocalMin->y, st_LocalPoint.z);
                    MATH_SetVector(&st_Normal, 0.0f, -1.0f, 0.0f);
                }

                f_Dist = - st_LocalPoint.y + _pst_LocalMax->y;

                if (f_Dist < f_DistMin)
                {
                    f_DistMin = f_Dist;
                    MATH_SetVector(&st_HitPoint, st_LocalPoint.x, _pst_LocalMax->y, st_LocalPoint.z);
                    MATH_SetVector(&st_Normal, 0.0f, 1.0f, 0.0f);
                }

                f_Dist =   st_LocalPoint.z - _pst_LocalMin->z;
                
                if (f_Dist < f_DistMin)
                {
                    f_DistMin = f_Dist;
                    MATH_SetVector(&st_HitPoint, st_LocalPoint.x, st_LocalPoint.y, _pst_LocalMin->z);
                    MATH_SetVector(&st_Normal, 0.0f, 0.0f, -1.0f);
                }

                f_Dist = - st_LocalPoint.z + _pst_LocalMax->z;

                if (f_Dist < f_DistMin)
                {
                    MATH_SetVector(&st_HitPoint, st_LocalPoint.x, st_LocalPoint.y, _pst_LocalMax->z);
                    MATH_SetVector(&st_Normal, 0.0f, 0.0f, 1.0f);
                }

                MATH_TransformVertex(p_V->GetCurrPos(), _pst_MatrixBox, &st_HitPoint);

                GAO_ModifierSoftBody_Friction(_p_SoftBody, p_V, &st_Normal);
                //MATH_CopyVector(&p_V->m_PrevPos, p_V->GetCurrPos());

                b_Collision = TRUE;
            }
        }
    }

    return b_Collision;
}

BOOL GAO_ModifierSoftBody_CollideTriangles(GAO_tdst_ModifierSoftBody * _p_SoftBody,
                                           MATH_tdst_Matrix          * _pst_Matrix,
                                           ULONG                       _ul_NbTriangles,
                                           USHORT                    * _auw_Triangle,
                                           COL_tdst_IndexedTriangle	 * _ast_Triangle,
                                           MATH_tdst_Vector          * _ast_Points,
                                           MATH_tdst_Vector          * _ast_Normals)
{
    BOOL                       b_Col = FALSE;
    ULONG                      i;
    BOOL                       b_Collision;
    FLOAT                      f_Dist;
    FLOAT                      f_BestDist = 0.1f;
    CSoftBodyVertex          * p_V;
    CSoftBodyVertex          * p_EndV;
    MATH_tdst_Vector         * pst_T1;
    MATH_tdst_Vector         * pst_T2;
    MATH_tdst_Vector         * pst_T3;
    MATH_tdst_Vector         * pst_Normal;
    MATH_tdst_Vector         * pst_BestNormal;
    MATH_tdst_Matrix           st_TriToPos;
    MATH_tdst_Matrix           st_PosToTri;
    MATH_tdst_Vector           st_CurrPos;
    MATH_tdst_Vector           st_HitPoint;
    MATH_tdst_Vector           st_BestHitPoint;
    MATH_tdst_Vector           st_Normal;
    COL_tdst_IndexedTriangle * pst_Triangle;

    MATH_MulMatrixMatrix(&st_TriToPos, _pst_Matrix, &_p_SoftBody->st_InvMatrix);
    MATH_InvertMatrix(&st_PosToTri, &st_TriToPos);
         
    p_V = _p_SoftBody->a_Vertices;
    p_EndV = _p_SoftBody->a_Vertices + _p_SoftBody->ul_NbVertices;

    for(; p_V < p_EndV; p_V++)
    {
        if (!(p_V->m_ulFlags & CSoftBodyVertex::eVertexFlagFixe))
        {
            b_Collision = FALSE;
            f_BestDist = 0.2f;
            MATH_TransformVertex(&st_CurrPos, &st_PosToTri, p_V->GetCurrPos());

            for(i = 0; i < _ul_NbTriangles; i++)
            {
                if (_auw_Triangle)
                {
                    pst_Triangle = (_ast_Triangle + _auw_Triangle[i]);
                    pst_Normal = (_ast_Normals + _auw_Triangle[i]);
                }
                else
                {
                    pst_Triangle = (_ast_Triangle + i);
                    pst_Normal = (_ast_Normals + i);
                }

                /* We get the triangles points. */
                pst_T1 = &_ast_Points[pst_Triangle->auw_Index[0]];
                pst_T2 = &_ast_Points[pst_Triangle->auw_Index[1]];
                pst_T3 = &_ast_Points[pst_Triangle->auw_Index[2]];

                if (INT_FullRayTriangle(&st_CurrPos, pst_Normal, pst_T1, pst_T2, pst_T3, &f_Dist, &st_HitPoint, FALSE))
                {
                    if (f_Dist > 0.0f && f_Dist < f_BestDist)
                    {
                        b_Collision = TRUE;
                        f_BestDist = f_Dist;
                        MATH_CopyVector(&st_BestHitPoint, &st_HitPoint);
                        pst_BestNormal = pst_Normal;
                    }
                }
            } 

            if (b_Collision)
            {
                MATH_TransformVertex(p_V->GetCurrPos(), &st_TriToPos, &st_BestHitPoint);
                //MATH_CopyVector(p_V->GetCurrPos(), &st_BestHitPoint);

                MATH_TransformVector(&st_Normal, &st_TriToPos, pst_BestNormal);
                GAO_ModifierSoftBody_Friction(_p_SoftBody, p_V, &st_Normal);

                //MATH_CopyVector(&p_V->m_PrevPos, p_V->GetCurrPos());

                b_Col = TRUE;
            }
        }
    }

    return b_Col;
}



BOOL GAO_ModifierSoftBody_CollideCylinder(GAO_tdst_ModifierSoftBody * _p_SoftBody, COL_tdst_Cylinder * _pst_Cylinder)
{
    BOOL               b_Collision = FALSE;
	FLOAT              f_Top;
	FLOAT              f_Bottom;
    FLOAT              f_Dist2D;
    FLOAT              f_OutSide;
    FLOAT              f_OutTop;
    FLOAT              f_OutBottom;
	MATH_tdst_Vector   Offset2D;;
    MATH_tdst_Vector * pst_CurrPos;
    MATH_tdst_Vector   st_Normal;
    CSoftBodyVertex  * p_V;
    CSoftBodyVertex  * p_EndV;

	f_Top = _pst_Cylinder->st_Center.z + _pst_Cylinder->f_Height; 
	f_Bottom = _pst_Cylinder->st_Center.z - _pst_Cylinder->f_Height;

    p_V = _p_SoftBody->a_Vertices;
    p_EndV = _p_SoftBody->a_Vertices + _p_SoftBody->ul_NbVertices;

    for(; p_V < p_EndV; p_V++)
    {
        if (!(p_V->m_ulFlags & CSoftBodyVertex::eVertexFlagFixe))
        {
            pst_CurrPos = p_V->GetCurrPos();

            if((pst_CurrPos->z < f_Top) && (pst_CurrPos->z > f_Bottom))
            {
                MATH_SubVector(&Offset2D, pst_CurrPos, &_pst_Cylinder->st_Center);
                Offset2D.z = 0.0f;

                f_Dist2D = MATH_f_NormVector(&Offset2D);

                if (f_Dist2D < _pst_Cylinder->f_Radius)
                {
                    f_OutSide = _pst_Cylinder->f_Radius - f_Dist2D;
                    f_OutTop = f_Top - pst_CurrPos->z;
                    f_OutBottom = pst_CurrPos->z - f_Bottom;
                    if((f_OutTop < f_OutSide) && (f_OutTop < f_OutBottom))
                    {
                        pst_CurrPos->z += f_OutTop;
                        MATH_InitVector(&st_Normal, 0.0f, 0.0f, 1.0f);
                    }
                    else if((f_OutBottom < f_OutSide) && (f_OutBottom < f_OutTop))
                    {
                        pst_CurrPos->z -= f_OutBottom;
                        MATH_InitVector(&st_Normal, 0.0f, 0.0f, -1.0f);
                    }
                    else 
                    {
                        MATH_NormalizeEqualAnyVector(&Offset2D);
                        MATH_AddScaleVector(pst_CurrPos, pst_CurrPos, &Offset2D, f_OutSide);
                        MATH_CopyVector(&st_Normal, &Offset2D);
                    }

                    GAO_ModifierSoftBody_Friction(_p_SoftBody, p_V, &st_Normal);
                    //MATH_CopyVector(&p_V->m_PrevPos, pst_CurrPos);

                    b_Collision = TRUE;
                }
            }
        }
    }

    return b_Collision;
}

void GAO_ModifierSoftBody_Friction(GAO_tdst_ModifierSoftBody * _p_SoftBody, CSoftBodyVertex  * _p_Vertex, MATH_tdst_Vector * _pst_Normal)
{
    FLOAT            f_VdotN;
    FLOAT            f_SqrVt;
    MATH_tdst_Vector st_V;
    MATH_tdst_Vector st_Vt;

    MATH_SubVector(&st_V, _p_Vertex->GetCurrPos(), &_p_Vertex->m_PrevPos);

    f_VdotN = MATH_f_DotProduct(&st_V, _pst_Normal);

    MATH_ScaleVector(&st_Vt, _pst_Normal, -f_VdotN);
    MATH_AddEqualVector(&st_Vt, &st_V);
    f_SqrVt = MATH_f_SqrNormVector(&st_Vt);

    if (f_SqrVt > Cf_EpsilonBig)
    {
        // Kinetic friction        
        MATH_BlendVector(&_p_Vertex->m_PrevPos, _p_Vertex->GetCurrPos(), &_p_Vertex->m_PrevPos, _p_SoftBody->f_KineticFriction);
    }
    else
    {  
        // Satic friction
        MATH_BlendVector(&_p_Vertex->m_PrevPos, _p_Vertex->GetCurrPos(), &_p_Vertex->m_PrevPos, _p_SoftBody->f_StaticFriction);
    }

}

GAO_tdst_ModifierSoftBody * GAO_ModifierSoftBody_Get(OBJ_tdst_GameObject * _pst_Gao)
{
    GAO_tdst_ModifierSoftBody * p_SoftBody;
    MDF_tdst_Modifier	      * pst_Modifier;

	if((_pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (_pst_Gao->pst_Extended))
	{
		pst_Modifier = _pst_Gao->pst_Extended->pst_Modifiers;
		while(pst_Modifier)
		{
			if(/*(pst_Modifier->ul_Flags & (MDF_C_Modifier_ApplyGao|MDF_C_Modifier_Inactive|MDF_C_Modifier_ApplyGen))==0 &&*/
               (pst_Modifier->i->ul_Type == MDF_C_Modifier_SoftBody))
			{
                p_SoftBody = (GAO_tdst_ModifierSoftBody *)pst_Modifier->p_Data;

                return p_SoftBody;
			}

			pst_Modifier = pst_Modifier->pst_Next;
		}
	}

    return NULL;
}

FLOAT GAO_ModifierSoftBody_GetForceWindMax(OBJ_tdst_GameObject * _pst_Gao)
{
    GAO_tdst_ModifierSoftBody * p_SoftBody;

    p_SoftBody = GAO_ModifierSoftBody_Get(_pst_Gao);

    if (p_SoftBody)
    {
        return MATH_f_NormVector(&p_SoftBody->st_WindMax);
    }

    return 0.0f;
}

FLOAT GAO_ModifierSoftBody_GetForceWind(OBJ_tdst_GameObject * _pst_Gao)
{
    ULONG i;
    ULONG ul_NbVertex = 0;
    FLOAT f_Wind;
    FLOAT f_SumWind = 0.0f;
    CSoftBodyVertex * p_V;
    GAO_tdst_ModifierSoftBody * p_SoftBody;

    p_SoftBody = GAO_ModifierSoftBody_Get(_pst_Gao);

    if (p_SoftBody)
    {
        p_V = p_SoftBody->a_Vertices;

        for(i=0; i<p_SoftBody->ul_NbVertices; i++, p_V++)
        {
            if (!(p_V->m_ulFlags & CSoftBodyVertex::eVertexFlagFixe))
            {
                f_Wind = MATH_f_Distance(p_V->GetCurrPos(), &p_V->m_PrevPos);
                f_SumWind += fAbs(f_Wind);
                ul_NbVertex++;
            }
        }

        f_Wind = (f_SumWind / (FLOAT)ul_NbVertex)/(TIM_gf_dt*TIM_gf_dt);

        return f_Wind;
    }
    
    return 0.0f;
}

void GAO_ModifierSoftBody_SetForceWind(OBJ_tdst_GameObject * _pst_Gao, MATH_tdst_Vector * _pst_WindMin, MATH_tdst_Vector * _pst_WindMax)
{
    GAO_tdst_ModifierSoftBody * p_SoftBody;

    p_SoftBody = GAO_ModifierSoftBody_Get(_pst_Gao);

    if (p_SoftBody)
    {
        MATH_CopyVector(&p_SoftBody->st_WindMin, _pst_WindMin);
        MATH_CopyVector(&p_SoftBody->st_WindMax, _pst_WindMax);
    }
}

OBJ_tdst_GameObject * GAO_ModifierSoftBody_GetCollidedGao(OBJ_tdst_GameObject * _pst_Gao)
{
    GAO_tdst_ModifierSoftBody * p_SoftBody;

    p_SoftBody = GAO_ModifierSoftBody_Get(_pst_Gao);

    if (p_SoftBody)
    {
        return p_SoftBody->pst_CollidedGao;
    }

    return NULL;
}

#ifdef ACTIVE_EDITORS


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				19 Mar 2002
// 
// Prototype		GAO_ModifierSoftBody_Save
// Parameters		_pst_Mod : 
// Return Type		void
// 
// Description		
// 
//------------------------------------------------------------------------
void GAO_ModifierSoftBody_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierSoftBody *p_SoftBody;
	ULONG					   ulSize;
    ULONG                      ulVersion;
    ULONG                      ulFlags;
    ULONG                      i;
	ULONG					   ul_Key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_SoftBody = (GAO_tdst_ModifierSoftBody *) _pst_Mod->p_Data;

	/* Save Size */
	ulSize = 5 * sizeof(ULONG)
           + 3 * sizeof(MATH_tdst_Vector)
           + 13 * sizeof(FLOAT)
           + p_SoftBody->ul_NbVertices * CSoftBodyVertex::GetSaveSize() + p_SoftBody->ul_NbTearable * sizeof(ULONG) +
           + p_SoftBody->ul_NbRods * CSoftBodyLengthConstraint::GetSaveSize()
           + p_SoftBody->ul_NbCollPlane * CSoftBodyPlaneConstraint::GetSaveSize();

	SAV_Buffer(&ulSize, sizeof(ULONG));

	/* Save version */
	ulVersion = SoftBody_C_VersionNumber;
	SAV_Buffer(&ulVersion, sizeof(ULONG));

    ulFlags = p_SoftBody->ul_Flags & ~SoftBody_C_InitPrevPos;
    SAV_Buffer(&ulFlags, sizeof(ULONG));

	SAV_Buffer(&p_SoftBody->ul_NbIter, sizeof(ULONG));
	SAV_Buffer(&p_SoftBody->ul_NbNormalizeIter, sizeof(ULONG));
    
    SAV_Buffer(&p_SoftBody->ul_NbVertices, sizeof(ULONG));
    SAV_Buffer(&p_SoftBody->ul_NbRods, sizeof(ULONG));
    SAV_Buffer(&p_SoftBody->ul_NbCollPlane, sizeof(ULONG));

	SAV_Buffer(&p_SoftBody->st_Gravity, sizeof(MATH_tdst_Vector));
    SAV_Buffer(&p_SoftBody->f_DTDamping, sizeof(FLOAT));

	SAV_Buffer(&p_SoftBody->st_WindMin, sizeof(MATH_tdst_Vector));
    SAV_Buffer(&p_SoftBody->st_WindMax, sizeof(MATH_tdst_Vector));
    SAV_Buffer(&p_SoftBody->f_WindUScale, sizeof(FLOAT));
	SAV_Buffer(&p_SoftBody->f_WindVScale, sizeof(FLOAT));
    SAV_Buffer(&p_SoftBody->f_WindUPan, sizeof(FLOAT));
	SAV_Buffer(&p_SoftBody->f_WindVPan, sizeof(FLOAT));

    SAV_Buffer(&p_SoftBody->f_StaticFriction, sizeof(FLOAT));
    SAV_Buffer(&p_SoftBody->f_KineticFriction, sizeof(FLOAT));

	// tearing parameters
	SAV_Buffer(&p_SoftBody->f_DampLength, sizeof(FLOAT));
	SAV_Buffer(&p_SoftBody->f_InitialSpeed, sizeof(FLOAT));
	SAV_Buffer(&p_SoftBody->f_Acceleration, sizeof(FLOAT));	
	SAV_Buffer(&p_SoftBody->f_Angles[0], sizeof(FLOAT));
	SAV_Buffer(&p_SoftBody->f_Angles[1], sizeof(FLOAT));
	SAV_Buffer(&p_SoftBody->f_MaxSwing, sizeof(FLOAT));

	// dress parameters
	SAV_Buffer(&p_SoftBody->fatherChannel, sizeof(ULONG));
	if(p_SoftBody->actor != NULL)
		ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) p_SoftBody->actor);
	else
		ul_Key = BIG_C_InvalidKey;
	SAV_Buffer(&ul_Key, 4);

	SAV_Buffer(&p_SoftBody->f_DampingDistance,sizeof(FLOAT));
	SAV_Buffer(&p_SoftBody->f_BlendRatio,sizeof(FLOAT));	

	// reset all the zipper points before saving
	forAll(GAO_tdst_ModifierSoftBody::ZipperVertices,p_SoftBody->zipper,item)
	{
		(*item)->m_ulFlags = CSoftBodyVertex::eVertexFlagTearable;
	}

    if (p_SoftBody->ul_NbVertices > 0)
    {
        for(i=0; i<p_SoftBody->ul_NbVertices; i++)
        {
            p_SoftBody->a_Vertices[i].Save();
			if(p_SoftBody->a_Vertices[i].m_ulFlags & CSoftBodyVertex::eVertexFlagTearable)
			{
				SAV_Buffer(&p_SoftBody->a_Vertices[i].m_tearingVertex->m_ulBoneId, sizeof(ULONG));
			}
        }
    }
    
    if (p_SoftBody->ul_NbRods > 0)
    {
        for(i=0; i<p_SoftBody->ul_NbRods; i++)
        {
            p_SoftBody->a_Rods[i].Save();
        }
    }
    
    if (p_SoftBody->ul_NbCollPlane > 0)
    {
        for(i=0; i<p_SoftBody->ul_NbCollPlane; i++)
        {
            p_SoftBody->a_CollPlane[i].Save();
        }
    }
}


void GAO_ModifierSoftBody_CreateMesh(GAO_tdst_ModifierSoftBody * _p_SoftBody)
{
    ULONG i, x, y;
    ULONG ul_NbRodsX = (_p_SoftBody->ul_SizeX - 1) * _p_SoftBody->ul_SizeY;
    ULONG ul_NbRodsY = (_p_SoftBody->ul_SizeY - 1) * _p_SoftBody->ul_SizeX;
    ULONG ul_NbRodsXY = 4;
    OBJ_tdst_Group * pst_Group = NULL;

    FLOAT f_Length;

    if (_p_SoftBody->ul_SizeX <= 0 || _p_SoftBody->ul_SizeY <= 0)
    {
        return;
    }

    _p_SoftBody->ul_NbVertices = _p_SoftBody->ul_SizeX * _p_SoftBody->ul_SizeY;
    _p_SoftBody->ul_NbRods = ul_NbRodsX + ul_NbRodsY + ul_NbRodsXY;

    if (_p_SoftBody->a_Vertices)
        delete [] _p_SoftBody->a_Vertices;

    if (_p_SoftBody->a_Rods)
        delete [] _p_SoftBody->a_Rods;

    _p_SoftBody->a_Vertices = new CSoftBodyVertex[_p_SoftBody->ul_NbVertices];
    _p_SoftBody->a_Rods = new CSoftBodyLengthConstraint[_p_SoftBody->ul_NbRods];

    // Set Group
    if (_p_SoftBody->pst_Hook &&
        _p_SoftBody->pst_Hook->pst_Base &&
        _p_SoftBody->pst_Hook->pst_Base->pst_GameObjectAnim &&
        _p_SoftBody->pst_Hook->pst_Base->pst_GameObjectAnim->pst_Skeleton)
    {
        pst_Group = _p_SoftBody->pst_Hook->pst_Base->pst_GameObjectAnim->pst_Skeleton;
    }

    for (i=0; i<_p_SoftBody->ul_NbVertices; i++)
    {
        if(i< _p_SoftBody->ul_SizeX)
        {
            _p_SoftBody->a_Vertices[i].m_ulFlags = CSoftBodyVertex::eVertexFlagFixe;
        }
        else
        {
            _p_SoftBody->a_Vertices[i].m_ulFlags = CSoftBodyVertex::eVertexFlagNone;
        }

        _p_SoftBody->a_Vertices[i].m_ulBoneId = i;

        _p_SoftBody->a_Vertices[i].m_u = (FLOAT)(i % _p_SoftBody->ul_SizeX) / (FLOAT)(_p_SoftBody->ul_SizeX - 1);
        _p_SoftBody->a_Vertices[i].m_v = (FLOAT)(i / _p_SoftBody->ul_SizeX) / (FLOAT)(_p_SoftBody->ul_SizeY - 1);

        if (pst_Group)
        {
            _p_SoftBody->a_Vertices[i].UpdatePointer(pst_Group);
        }
    }

    

    x = 0;
    y = 0;
    for (i=0; i<ul_NbRodsX; i++)
    {
        _p_SoftBody->a_Rods[i].m_v1 = x + (y * _p_SoftBody->ul_SizeX);
        _p_SoftBody->a_Rods[i].m_v2 = (x + 1) +  (y * _p_SoftBody->ul_SizeX);

        f_Length = MATH_f_Distance(_p_SoftBody->a_Vertices[_p_SoftBody->a_Rods[i].m_v1].GetCurrPos(),
                                   _p_SoftBody->a_Vertices[_p_SoftBody->a_Rods[i].m_v2].GetCurrPos());
        _p_SoftBody->a_Rods[i].m_fLength = f_Length;

        x = (x + 1) % (_p_SoftBody->ul_SizeX - 1);
        if (x == 0)
            y++;
    }

    x = 0;
    y = 0;
    for (i=ul_NbRodsX; i<(ul_NbRodsX + ul_NbRodsY); i++)
    {
        _p_SoftBody->a_Rods[i].m_v1 = x + (y * _p_SoftBody->ul_SizeX);
        _p_SoftBody->a_Rods[i].m_v2 = x +  ((y + 1) * _p_SoftBody->ul_SizeX);

        f_Length = MATH_f_Distance(_p_SoftBody->a_Vertices[_p_SoftBody->a_Rods[i].m_v1].GetCurrPos(),
                                   _p_SoftBody->a_Vertices[_p_SoftBody->a_Rods[i].m_v2].GetCurrPos());
        _p_SoftBody->a_Rods[i].m_fLength = f_Length;

        x = (x + 1) % _p_SoftBody->ul_SizeX;
        if (x == 0)
            y++;
    }

    /*
    x = 1;
    y = 0;
    for (i=(ul_NbRodsX + ul_NbRodsY); i<_p_SoftBody->ul_NbRods; i++)
    {
        _p_SoftBody->a_Rods[i].m_v1 = x + (y * _p_SoftBody->ul_SizeX);
        _p_SoftBody->a_Rods[i].m_v2 = (x - 1) +  ((y + 1) * _p_SoftBody->ul_SizeX);

        f_Length = MATH_f_Distance(_p_SoftBody->a_Vertices[_p_SoftBody->a_Rods[i].m_v1].GetCurrPos(),
                                   _p_SoftBody->a_Vertices[_p_SoftBody->a_Rods[i].m_v2].GetCurrPos());

        _p_SoftBody->a_Rods[i].m_fLength = f_Length;

        x = (x + 1) % _p_SoftBody->ul_SizeX;
        if (x == 0)
        {
            y++;
            x++;
        }
    }
    */

    i = ul_NbRodsX + ul_NbRodsY;
    _p_SoftBody->a_Rods[i].m_v1 = 0;
    _p_SoftBody->a_Rods[i].m_v2 = _p_SoftBody->ul_SizeX + 1;
    f_Length = MATH_f_Distance(_p_SoftBody->a_Vertices[_p_SoftBody->a_Rods[i].m_v1].GetCurrPos(),
                               _p_SoftBody->a_Vertices[_p_SoftBody->a_Rods[i].m_v2].GetCurrPos());
    _p_SoftBody->a_Rods[i].m_fLength = f_Length;
    i++;

    _p_SoftBody->a_Rods[i].m_v1 = _p_SoftBody->ul_SizeX - 1;
    _p_SoftBody->a_Rods[i].m_v2 = 2 * _p_SoftBody->ul_SizeX - 2;
    f_Length = MATH_f_Distance(_p_SoftBody->a_Vertices[_p_SoftBody->a_Rods[i].m_v1].GetCurrPos(),
                               _p_SoftBody->a_Vertices[_p_SoftBody->a_Rods[i].m_v2].GetCurrPos());
    _p_SoftBody->a_Rods[i].m_fLength = f_Length;
    i++;

    _p_SoftBody->a_Rods[i].m_v1 = (_p_SoftBody->ul_SizeX) * (_p_SoftBody->ul_SizeY - 2) + 1;
    _p_SoftBody->a_Rods[i].m_v2 = (_p_SoftBody->ul_SizeX) * (_p_SoftBody->ul_SizeY - 1);
    f_Length = MATH_f_Distance(_p_SoftBody->a_Vertices[_p_SoftBody->a_Rods[i].m_v1].GetCurrPos(),
                               _p_SoftBody->a_Vertices[_p_SoftBody->a_Rods[i].m_v2].GetCurrPos());
    _p_SoftBody->a_Rods[i].m_fLength = f_Length;
    i++;

    _p_SoftBody->a_Rods[i].m_v1 = (_p_SoftBody->ul_SizeX * (_p_SoftBody->ul_SizeY - 1)) - 2;
    _p_SoftBody->a_Rods[i].m_v2 = (_p_SoftBody->ul_SizeX * _p_SoftBody->ul_SizeY) - 1;
    f_Length = MATH_f_Distance(_p_SoftBody->a_Vertices[_p_SoftBody->a_Rods[i].m_v1].GetCurrPos(),
                               _p_SoftBody->a_Vertices[_p_SoftBody->a_Rods[i].m_v2].GetCurrPos());
    _p_SoftBody->a_Rods[i].m_fLength = f_Length;
    i++;
}

void GAO_ModifierSoftBody_ResizePlanes(GAO_tdst_ModifierSoftBody * _p_SoftBody, ULONG _ul_OldData)
{
    CSoftBodyPlaneConstraint * a_OldCollPlan = _p_SoftBody->a_CollPlane;

    if (_p_SoftBody->ul_NbCollPlane == _ul_OldData) 
        return;

    _p_SoftBody->a_CollPlane = new CSoftBodyPlaneConstraint[_p_SoftBody->ul_NbCollPlane];

    for(ULONG i=0; i<_ul_OldData && i<_p_SoftBody->ul_NbCollPlane; i++)
    {
        _p_SoftBody->a_CollPlane[i] = a_OldCollPlan[i];
    }

    if (a_OldCollPlan)
        delete [] a_OldCollPlan;
}

void GAO_CreateName(CHAR * _sz_NewName, CHAR * _sz_BaseName, CHAR * _sz_IdentityName, CHAR * _sz_Ext)
{
    CHAR * sz_Ext;
	L_strcpy(_sz_NewName, _sz_BaseName);
    if((sz_Ext = L_strrchr(_sz_NewName, '.')) == NULL)
        sz_Ext = _sz_NewName + L_strlen(_sz_NewName);
    sprintf(sz_Ext, "%s%s", _sz_IdentityName, _sz_Ext);
}

OBJ_tdst_GameObject * OBJ_CreateGameObject(WOR_tdst_World * _pst_World, BIG_KEY _ul_WorldKey, BIG_INDEX _ul_GolIndex, CHAR * _sz_Name, CHAR * _sz_SubDirectory)
{
    OBJ_tdst_GameObject * pst_NewGao;
    CHAR         	      sz_Path[BIG_C_MaxLenPath];
    BIG_INDEX             ul_GaoIndex;

    pst_NewGao = OBJ_GameObject_Create(OBJ_C_IdentityFlag_HasInitialPos);

	/* Register and set a default name */
	WOR_GetGaoPathWithKey(_ul_WorldKey, sz_Path);
    L_strcat(sz_Path, "/");
    L_strcat(sz_Path, _sz_SubDirectory);
	BIG_BuildNewName(sz_Path, _sz_Name, EDI_Csz_ExtGameObject);

	WOR_gpst_WorldToLoadIn = _pst_World;
	OBJ_GameObject_RegisterWithName(pst_NewGao, BIG_C_InvalidIndex, _sz_Name, OBJ_GameObject_RegSetName);

	ul_GaoIndex = OBJ_ul_GameObject_Save(_pst_World, pst_NewGao, sz_Path);
	LOA_AddAddress(ul_GaoIndex, pst_NewGao);

	WOR_World_JustAfterLoadObject(_pst_World, pst_NewGao, TRUE, TRUE);

    if (_ul_GolIndex != BIG_C_InvalidIndex)
    {
        pst_NewGao->c_FixFlags |= OBJ_C_HasBeenMerge;
        BIG_AddRefInGroup(_ul_GolIndex, _sz_Name, BIG_FileKey(ul_GaoIndex));
    }

    return pst_NewGao;
}

void GAO_ModifierSoftBody_SetupFromGro(GAO_tdst_ModifierSoftBody * _p_SoftBody, GEO_tdst_Object * _pst_Obj)
{
    LONG                               i;
    ULONG                              j, k;
    unsigned short                     uw_Index0;
    unsigned short                     uw_Index1;
    ULONG                              ul_NewRods;
    ULONG                              ul_DelRods;
    FLOAT                              f_Length;
    GEO_tdst_ElementIndexedTriangles * pst_Element;
    GEO_tdst_IndexedTriangle         * pst_T;
    CSoftBodyLengthConstraint       ** ast_NewRods;
    CSoftBodyLengthConstraint       ** ast_DelRods;
    CSoftBodyLengthConstraint        * pst_Rod;

    static BOOL b_IgnoreFlagEdge = FALSE;

    ERR_X_Assert(_pst_Obj->l_NbElements == 1);
    
    _p_SoftBody->ul_NbVertices = _pst_Obj->l_NbPoints;
    _p_SoftBody->a_Vertices = new CSoftBodyVertex[_p_SoftBody->ul_NbVertices];
	_p_SoftBody->ul_NbTearable = 0;	

    for (k=0; k<_p_SoftBody->ul_NbVertices; k++)
    {
        _p_SoftBody->a_Vertices[k].m_ulBoneId = k;

        _p_SoftBody->a_Vertices[k].m_u = 0.0f;
        _p_SoftBody->a_Vertices[k].m_v = 0.0f;

		// fix point in white
		if (_pst_Obj->dul_PointColors && (_pst_Obj->dul_PointColors[k+1] & 0x00FFFFFF) == 0x00FFFFFF /* White */)
		{
			_p_SoftBody->a_Vertices[k].m_ulFlags = CSoftBodyVertex::eVertexFlagFixe;
			ERR_OutputDebugString("point fixe: %f,%f,%f 0.1\n", _pst_Obj->dst_Point[k].x, _pst_Obj->dst_Point[k].y, _pst_Obj->dst_Point[k].z);
		}
		// tearable point in black
        else if (_pst_Obj->dul_PointColors && (_pst_Obj->dul_PointColors[k+1] & 0x00FFFFFF) == 0x00000000 /* Black */ )
        { 
            _p_SoftBody->a_Vertices[k].m_ulFlags = CSoftBodyVertex::eVertexFlagTearable;			
			_p_SoftBody->ul_NbTearable++;
			ERR_OutputDebugString("point dechirable: %f,%f,%f 0.1\n", _pst_Obj->dst_Point[k].x, _pst_Obj->dst_Point[k].y, _pst_Obj->dst_Point[k].z);
        }
		// everything else is free
        else
        {
            _p_SoftBody->a_Vertices[k].m_ulFlags = CSoftBodyVertex::eVertexFlagNone;
			ERR_OutputDebugString("point libre: %f,%f,%f\n", _pst_Obj->dst_Point[k].x, _pst_Obj->dst_Point[k].y, _pst_Obj->dst_Point[k].z);
        }
    }
	
	// link the tearable vertices together
	GEO_Vertex *v0,*v1;
	for (k=0; k<_p_SoftBody->ul_NbVertices; k++)
	{
		// if the vertice is tearable and has not been linked
		if( _p_SoftBody->a_Vertices[k].m_ulFlags & CSoftBodyVertex::eVertexFlagTearable && _p_SoftBody->a_Vertices[k].m_tearingVertex == NULL )
		{
			v0 = &_pst_Obj->dst_Point[k];

			// find the matching vertex from the remaining vertices 
			// and set its tearing vertex
			for (j=(k+1); j<_p_SoftBody->ul_NbVertices; j++)
			{				
				v1 = &_pst_Obj->dst_Point[j];
				if( v0->x <= (v1->x+SoftBody_C_ZipperEpsilon) && v0->x >= (v1->x-SoftBody_C_ZipperEpsilon) &&
					v0->z <= (v1->z+SoftBody_C_ZipperEpsilon) && v0->z >= (v1->z-SoftBody_C_ZipperEpsilon))
				{
					_p_SoftBody->a_Vertices[k].m_tearingVertex = &_p_SoftBody->a_Vertices[j];
					_p_SoftBody->a_Vertices[j].m_tearingVertex = &_p_SoftBody->a_Vertices[k];
					break;
				}
			}

			ERR_X_ErrorAssert(_p_SoftBody->a_Vertices[k].m_tearingVertex != NULL, "Create SoftBody: Tearing vertex is  invalid, it needs a duplicated vertex to create a zipper node.", NULL);			
		}
	}	

    pst_Element = _pst_Obj->dst_Element;
	ast_NewRods = (CSoftBodyLengthConstraint **)MEM_p_Alloc(3*pst_Element->l_NbTriangles*sizeof(CSoftBodyLengthConstraint *) );
	ast_DelRods = (CSoftBodyLengthConstraint **)MEM_p_Alloc(3*pst_Element->l_NbTriangles*sizeof(CSoftBodyLengthConstraint *) );
    ul_NewRods = 0;
    ul_DelRods = 0;

    pst_T = pst_Element->dst_Triangle;
    for(i=0; i<pst_Element->l_NbTriangles; i++, pst_T++)
    { 
        for (k=0; k<3; k++)
        {
			uw_Index0 = pst_T->auw_Index[k];
			uw_Index1 = pst_T->auw_Index[(k+1)%3];
						        
            if (_pst_Obj->dst_UV)
            {
                _p_SoftBody->a_Vertices[uw_Index0].m_u = _pst_Obj->dst_UV[pst_T->auw_UV[k]].fU;
                _p_SoftBody->a_Vertices[uw_Index0].m_v = _pst_Obj->dst_UV[pst_T->auw_UV[k]].fV;
            }
            else
            {
                _p_SoftBody->a_Vertices[uw_Index0].m_u = 0.0f;
                _p_SoftBody->a_Vertices[uw_Index0].m_v = 0.0f;
            }

            f_Length = MATH_f_Distance(&_pst_Obj->dst_Point[uw_Index0], &_pst_Obj->dst_Point[uw_Index1]);

            pst_Rod = new CSoftBodyLengthConstraint(uw_Index0, uw_Index1, f_Length);

            if(b_IgnoreFlagEdge || (pst_T->ul_MaxFlags & (1 << k)))
            {
                // Edge visible
                for(j=0; j<ul_NewRods; j++)
                {
                    if ((*ast_NewRods[j]) == (*pst_Rod))
                        break;
                }
                if (j == ul_NewRods)
                {
                    ast_NewRods[ul_NewRods] = pst_Rod;
                    ul_NewRods++;
                }
            }
            else
            {
                for(j=0; j<ul_DelRods; j++)
                {
                    if (((*ast_DelRods[j]) == (*pst_Rod)))
                        break;
                }
                if (j == ul_DelRods)
                {
                    ast_DelRods[ul_DelRods] = pst_Rod;

                    ul_DelRods++;
                }
            }
        }
    }	

    for(j=0; j<ul_DelRods; j++)
    {
        for(k=0; k<ul_NewRods; k++)
        {
            if((*ast_DelRods[j]) == (*ast_NewRods[k]))
            {
                //ERR_X_ErrorAssert(0, "Create SoftBody: Visible edge invalid", NULL);
                ERR_X_Warning(0, "Create SoftBody: Visible edge invalid", NULL);
            }
        }
    }

    _p_SoftBody->ul_NbRods = ul_NewRods;
    _p_SoftBody->a_Rods = new CSoftBodyLengthConstraint[_p_SoftBody->ul_NbRods];

    for (k=0; k<_p_SoftBody->ul_NbRods; k++)
    {
        _p_SoftBody->a_Rods[k] = *ast_NewRods[k];

        /*
        ERR_OutputDebugString("line %f,%f,%f %f,%f,%f\n",
                              _pst_Obj->dst_Point[_p_SoftBody->a_Rods[k].m_v1].x,
                              _pst_Obj->dst_Point[_p_SoftBody->a_Rods[k].m_v1].y,
                              _pst_Obj->dst_Point[_p_SoftBody->a_Rods[k].m_v1].z,
                              _pst_Obj->dst_Point[_p_SoftBody->a_Rods[k].m_v2].x,
                              _pst_Obj->dst_Point[_p_SoftBody->a_Rods[k].m_v2].y,
                              _pst_Obj->dst_Point[_p_SoftBody->a_Rods[k].m_v2].z);
        */
    }

    MEM_Free(ast_NewRods);
    MEM_Free(ast_DelRods);
}

//OBJ_tdst_Group * GAO_ModifierSoftBody_CreateGroup(WOR_tdst_World * _pst_World, OBJ_tdst_GameObject * _pst_Gao, ULONG _ul_Bones, MATH_tdst_Vector * _ast_Position)
//{
//    ULONG                 i;
//    BIG_INDEX             ul_GolIndex;
//    BIG_KEY               ul_GolKey;
//    CHAR                  sz_NameGao[BIG_C_MaxLenName];
//    CHAR                  sz_SubDir[BIG_C_MaxLenName];
//    CHAR                  sz_GroupsPath[BIG_C_MaxLenPath];
//    CHAR                  sz_GroupName[BIG_C_MaxLenName];
//    CHAR                  sz_IdentityName[BIG_C_MaxLenName];
//    CHAR *                pc_Ext;
//    BOOL                  b_Del;
//    OBJ_tdst_Group      * pst_Group;
//    OBJ_tdst_GameObject * pst_NewGao;
//    MATH_tdst_Matrix      st_GlobalMatrix;
//
//    // *********************************************************************************************
//    // Create Group
//    // *********************************************************************************************
//
//    if (_pst_Gao->ul_InitialWorldKey != LOA_ul_SearchKeyWithAddress((ULONG)_pst_World))
//    {
//        ul_GolKey = WOR_ul_GetGolKeyFromWow(_pst_Gao->ul_InitialWorldKey);
//        ul_GolIndex = BIG_ul_SearchKeyToFat(ul_GolKey);
//    }
//    else
//        ul_GolIndex = BIG_C_InvalidIndex;
//    
//
//    // Compute groups directory
//	WOR_GetSubPathWithKey(_pst_Gao->ul_InitialWorldKey, EDI_Csz_Path_Groups, sz_GroupsPath);
//
//    L_strcpy(sz_GroupName, _pst_Gao->sz_Name);
//    pc_Ext = L_strstr(sz_GroupName, ".");
//    if (pc_Ext)
//        *pc_Ext = '\0';
//
//    pst_Group = GRP_pst_CreateAndSaveNewGroup(sz_GroupsPath, sz_GroupName, TRUE, TRUE, &b_Del);
//
//    // *********************************************************************************************
//    // Create GameObject
//    // *********************************************************************************************
//
//    MATH_CopyMatrix(&st_GlobalMatrix, _pst_Gao->pst_GlobalMatrix);
//
//    sprintf(sz_IdentityName, "_Bones");
//    GAO_CreateName(sz_SubDir, _pst_Gao->sz_Name, sz_IdentityName, "");
//
//    for(i=0; i<_ul_Bones; i++)
//    {
//        sprintf(sz_IdentityName, "_Bone%02d", i);
//        GAO_CreateName(sz_NameGao, _pst_Gao->sz_Name, sz_IdentityName, EDI_Csz_ExtGameObject);
//
//        pst_NewGao = OBJ_CreateGameObject(_pst_World, _pst_Gao->ul_InitialWorldKey, ul_GolIndex, sz_NameGao, sz_SubDir);
//
//        OBJ_ChangeIdentityFlags(pst_NewGao, pst_NewGao->ul_IdentityFlags | OBJ_C_IdentityFlag_Hierarchy,
//                                pst_NewGao->ul_IdentityFlags);
//
//        pst_NewGao->pst_Base->pst_Hierarchy->pst_FatherInit = _pst_Gao;
//        pst_NewGao->pst_Base->pst_Hierarchy->pst_Father = _pst_Gao;
//        MATH_CopyVector(&pst_NewGao->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &_ast_Position[i]);
//
//        MATH_TransformVertex(&st_GlobalMatrix.T, _pst_Gao->pst_GlobalMatrix, &_ast_Position[i]);
//        OBJ_SetInitialAbsoluteMatrix(pst_NewGao, &st_GlobalMatrix);
//        OBJ_RestoreInitialPos(pst_NewGao);
//
//        // Add the object in the group
//        OBJ_AddInGroup(pst_Group, pst_NewGao);
//        OBJ_ChangeIdentityFlags(pst_NewGao, pst_NewGao->ul_IdentityFlags | OBJ_C_IdentityFlag_AdditionalMatrix | OBJ_C_IdentityFlag_AddMatArePointer,
//                                pst_NewGao->ul_IdentityFlags);
//    }
//
//    // Save Group
//    GRP_SaveGroup(pst_Group);
//
//    // Associate group
//    GRP_ObjAttachReplaceGroup(_pst_Gao, pst_Group);
//    OBJ_ComputeBV(_pst_Gao, OBJ_C_BV_ForceComputation, OBJ_C_BV_Sphere);
//
//    return pst_Group;
//}

void GAO_MofifierSoftBody_CreateSkin(OBJ_tdst_GameObject * _pst_Gao, GEO_tdst_Object * _pst_Obj /*, OBJ_tdst_Group * _pst_Group*/)
{
    LONG                l_OldNumberOfGizmos;
    LONG                iVertex, i;
    OBJ_tdst_Gizmo * pst_CurGizmo;
    GEO_tdst_VertexPonderationList  *pst_VertexPonderation;

    // Set its identity flags & number of additional matrices
    OBJ_ChangeIdentityFlags(_pst_Gao, (_pst_Gao->ul_IdentityFlags | OBJ_C_IdentityFlag_AdditionalMatrix) & ~OBJ_C_IdentityFlag_AddMatArePointer,
                            _pst_Gao->ul_IdentityFlags);

    _pst_Gao->pst_Base->pst_AddMatrix->ul_GrpIndex = 0;
    l_OldNumberOfGizmos = _pst_Gao->pst_Base->pst_AddMatrix->l_Number;
    _pst_Gao->pst_Base->pst_AddMatrix->l_Number = _pst_Obj->l_NbPoints;
    OBJ_Gizmo_ChangeNumberOfAdditionalMatrix(_pst_Gao->pst_Base->pst_AddMatrix, l_OldNumberOfGizmos);

    // Set the additionnal matrices
    pst_CurGizmo = _pst_Gao->pst_Base->pst_AddMatrix->dst_Gizmo;
    for (i=0; i<_pst_Obj->l_NbPoints; i++, pst_CurGizmo++)
    {
        // Set one gizmo matrix
        MATH_SetIdentityMatrix(&pst_CurGizmo->st_Matrix);
        MATH_SetTranslation(&pst_CurGizmo->st_Matrix, (_pst_Obj->dst_Point+i));
    }

    GEO_SKN_SetNumberOfMatrix(_pst_Obj, _pst_Obj->l_NbPoints);
    if(!GEO_SKN_IsExpanded(_pst_Obj))
        GEO_SKN_Expand(_pst_Obj);

    for (i=0; i<_pst_Obj->l_NbPoints; i++)
        GEO_SKN_FlashGizmo(_pst_Gao, _pst_Obj, i);

    for (iVertex=0; iVertex<_pst_Obj->l_NbPoints; iVertex++)
    {
        for (i=0; i<_pst_Obj->l_NbPoints; i++)
            _pst_Obj->p_SKN_Objectponderation->pp_PdrtLst[i]->p_PdrtVrc_E[iVertex].f_Ponderation = 0.0f;

        // Set the vertex associations
        pst_VertexPonderation = _pst_Obj->p_SKN_Objectponderation->pp_PdrtLst[iVertex];
        pst_VertexPonderation->p_PdrtVrc_E[iVertex].f_Ponderation = 1.0f;
        pst_VertexPonderation->us_IndexOfMatrix = (USHORT) iVertex;
    }
}

void GAO_ModifierSoftBody_CreateLine(GAO_tdst_ModifierSoftBody *p_SoftBody)
{
    // TODO KK:

	//// *********************************************************************************************
	//// Set Cob
	//// *********************************************************************************************     
	//ULONG ul_FileCob;
	//CHAR  sz_PathCob[BIG_C_MaxLenPath];
	//CHAR  sz_NameCob[BIG_C_MaxLenName];
	//COL_tdst_Cob * pst_Cob = NULL;
	//CHAR * p_c;
	//WOR_tdst_World      * pst_World;

 //	FLOAT f_Distance = p_SoftBody->zipper.back()->m_pCurrPos->z - p_SoftBody->zipper.front()->m_pCurrPos->z;

	//pst_World = p_SoftBody->pst_Hook->pst_World;

	//// ???? Distance ???????????????????????
	//sprintf(sz_NameCob, "SoftBodySegment_%07.3f.cob", f_Distance);

	//// Replace '.' with '_'
	//p_c = strchr(sz_NameCob, '.');
	//if (p_c)
	//	*p_c = '_';

	//L_strcpy(sz_PathCob, EDI_Csz_Path_Levels);
	//L_strcat(sz_PathCob, "/_ObjectsBanks/interactive_Objects/Collision Objects");

	//ul_FileCob = BIG_ul_SearchFileExt(sz_PathCob, sz_NameCob);

	//if (ul_FileCob == BIG_C_InvalidIndex)
	//{
	//	BIG_ul_CreateDir(sz_PathCob);
	//	ul_FileCob = BIG_ul_CreateFile(sz_PathCob, sz_NameCob);
	//}

	//if (ul_FileCob != BIG_C_InvalidIndex)
	//{
	//	ULONG ul_Pos = BIG_ul_SearchKeyToPos(BIG_FileKey(ul_FileCob));
	//	ULONG ul_Size = BIG_ul_GetLengthFile(ul_Pos);

	//	if(!ul_Size)
	//	{						
	//		COL_tdst_Cob	      st_DefaultCob;
	//		L_memset(&st_DefaultCob, 0, sizeof(COL_tdst_Cob));			
	//		st_DefaultCob.uc_Type = COL_C_Zone_None;
	//		st_DefaultCob.uc_Flag = 0;			
	//		COL_SaveCob(&st_DefaultCob, BIG_FileKey(ul_FileCob));
	//	}

	//	F3D_AddCob(p_SoftBody->pst_Hook, ul_FileCob);
	//	p_SoftBody->pst_Hook->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_EnableSnP;
	//	INT_SnP_AddObject(p_SoftBody->pst_Hook, pst_World->pst_SnP);

	//	ul_Pos = BIG_ul_SearchKeyToPos(BIG_FileKey(ul_FileCob));

	//	if(ul_Pos != (ULONG) - 1)
	//	{ 
	//		pst_Cob = (COL_tdst_Cob *) LOA_ul_SearchAddress(ul_Pos);
	//		if((int) pst_Cob == -1)
	//		{
	//			LOA_MakeFileRef(BIG_FileKey(ul_FileCob), (ULONG *) &pst_Cob, COL_ul_CallBackLoadCob, LOA_C_MustExists);
	//			LOA_Resolve();
	//		}
	//	}
	//}

	//if (pst_Cob)
	//{
	//	if (pst_Cob->pst_EventObject == NULL)
	//	{
	//		// --------------------
	//		// Create Event Object
	//		// --------------------
	//		GELine           * pNewLine;
	//		MATH_tdst_Vector   st_PointA;
	//		MATH_tdst_Vector   st_PointB;
	//		MATH_tdst_Vector   st_Normal;

	//		pst_Cob->pst_EventObject = (COL_tdst_EventObject *)MEM_p_AllocTag(sizeof(COL_tdst_EventObject), "ENG::MDF::COL_tdst_EventObject");
	//		pst_Cob->pst_EventObject->ul_NbEventObject = 0;
	//		pst_Cob->pst_EventObject->a_ListOfEventObject = NULL;

	//		MATH_CopyVector(&st_PointA, p_SoftBody->zipper.back()->GetCurrPos());
	//		MATH_CopyVector(&st_PointB, p_SoftBody->zipper.front()->GetCurrPos());
	//		MATH_InitVector(&st_Normal, 0.0f, 1.0, 0.0f);

	//		pNewLine = new GELine(st_PointA, st_PointB);
	//		pNewLine->SetNormal(st_Normal);
	//		pNewLine->SetSubType(GELine::GEOSubType_TearableDrape);
	//		pNewLine->SetRadius(0.0f);

	//		EvtObj_Add(pst_Cob->pst_EventObject, pNewLine);
	//	}

	//}
}

void GAO_ModifierSoftBody_CreateZipper(GAO_tdst_ModifierSoftBody *p_SoftBody)
{
	p_SoftBody->zipper.clear();

	for(ULONG i = 0; i < p_SoftBody->ul_NbVertices; i++)
	{		
		p_SoftBody->a_Vertices[i].Reinit();

		if(p_SoftBody->a_Vertices[i].m_ulFlags & CSoftBodyVertex::eVertexFlagTearable)
		{
			// add to zipper				
			p_SoftBody->zipper.push_back(&p_SoftBody->a_Vertices[i]);
		}
	}				
}

void GAO_ModifierSoftBody_CreateFromGro(OBJ_tdst_GameObject * _pst_Gao)
{
    WOR_tdst_World             * pst_World;
    GEO_tdst_Object            * pst_Obj;
    MDF_tdst_Modifier          * pst_Modifier;
    GAO_tdst_ModifierSoftBody  * p_SoftBody;

    pst_Obj = (GEO_tdst_Object *) OBJ_p_GetGro(_pst_Gao);
	if((!pst_Obj) || (pst_Obj->st_Id.i->ul_Type != GRO_Geometric))
	{
		return;
	}

    pst_World = WOR_World_GetWorldOfObject(_pst_Gao);

    if (GAO_ModifierSoftBody_Get(_pst_Gao))
    {
        // Create Skin
        if (_pst_Gao->pst_Extended->pst_Group)
        {
            // Delete Group
            OBJ_ChangeIdentityFlags(_pst_Gao, _pst_Gao->ul_IdentityFlags & ~OBJ_C_IdentityFlag_Group, _pst_Gao->ul_IdentityFlags);
        }

        GAO_MofifierSoftBody_CreateSkin(_pst_Gao, pst_Obj);
        OBJ_ComputeBV(_pst_Gao, OBJ_C_BV_ForceComputation, OBJ_C_BV_Sphere);
        
        return;
    }
 
    // Create Skin
    GAO_MofifierSoftBody_CreateSkin(_pst_Gao, pst_Obj);

    OBJ_ChangeIdentityFlags(_pst_Gao, _pst_Gao->ul_IdentityFlags | OBJ_C_IdentityFlag_ExtendedObject, _pst_Gao->ul_IdentityFlags);

    // Create Modifier SoftBody
    pst_Modifier = MDF_pst_Modifier_Create(_pst_Gao, MDF_C_Modifier_SoftBody, NULL);
    
    p_SoftBody = (GAO_tdst_ModifierSoftBody *) pst_Modifier->p_Data;

    GAO_ModifierSoftBody_Reinit(pst_Modifier);
    MDF_Modifier_AddToGameObject(_pst_Gao, pst_Modifier);

    GAO_ModifierSoftBody_SetupFromGro(p_SoftBody, pst_Obj);

    GAO_ModifierSoftBody_UpdatePointers(pst_Modifier);

    OBJ_ComputeBV(_pst_Gao, OBJ_C_BV_ForceComputation, OBJ_C_BV_Sphere);
	
	// Create only if the soft body is tearable
	if(p_SoftBody->ul_NbTearable > 0)
	{
		GAO_ModifierSoftBody_CreateZipper(p_SoftBody);
		GAO_ModifierSoftBody_CreateLine(p_SoftBody);
	}	
}

#endif // #ifdef ACTIVE_EDITORS

void GAO_ModifierSoftBody_UpdateTearingLength(GAO_tdst_ModifierSoftBody *_p_SoftBody)
{
	FLOAT dL = _p_SoftBody->f_TearingSpeed * _p_SoftBody->f_RealDT * _p_SoftBody->f_AngularDamping;
	_p_SoftBody->f_TearingLength += dL;
	_p_SoftBody->f_PendulumLength += dL;
}

void GAO_ModifierSoftBody_UpdateTearingSpeed(GAO_tdst_ModifierSoftBody *_p_SoftBody)
{	
	_p_SoftBody->f_TearingSpeed += _p_SoftBody->f_Acceleration * _p_SoftBody->f_RealDT;	
}

void GAO_ModifierSoftBody_UpdateAngle(GAO_tdst_ModifierSoftBody *_p_SoftBody)
{
	if( _p_SoftBody->ul_AnimType == GAO_tdst_ModifierSoftBody::front )
	{
		return;
	}

	//////////////////////////////////////////////////////////////
	// The curve is broken into 4 phases:
	// 0- the curve does a sin(x)*maxAngle[0] until 1/2*pi
	// 1- a)the curve does a sin(x)*lerp(maxAngle[0],maxAngle[1]) until pi
	//    b)the curve does a sin(x)*maxAngle[1] until 3/2*pi
	// 2- the curve ease-out, which is a damped shifted sin(x)* maxAngle[1]/2 until 5/2*pi
	// 3- the curve just go straight
	//
	// The curve starts at 0 and ends at 1. The angles are computed using the end phase constant.	
	//////////////////////////////////////////////////////////////
	FLOAT x = _p_SoftBody->f_TearingLength/_p_SoftBody->f_DampLength;

	if( x < SoftBody_C_EndPhase0 )
	{
		// angular damping [SoftBody_C_Phase0Damping * SoftBody_C_EndPhase0,SoftBody_C_EndPhase0]
		if( x > SoftBody_C_Phase0Damping * SoftBody_C_EndPhase0)
		{
			FLOAT y = (x/SoftBody_C_EndPhase0 - SoftBody_C_Phase0Damping)/(1.0f - SoftBody_C_Phase0Damping); // [0,1]
			_p_SoftBody->f_AngularDamping = 1.0f - y * SoftBody_C_MaxAngularDamping;			
		}		

		x = x/SoftBody_C_EndPhase0 * Cf_Pi/2.0f;
		_p_SoftBody->f_Angle = fSin(x) *  _p_SoftBody->f_MaxAngles[0];		
	}
	else if( x < SoftBody_C_EndPhase1 )
	{
		x -= SoftBody_C_EndPhase0;
		x /= (SoftBody_C_EndPhase1 - SoftBody_C_EndPhase0); // [0,1]

		// angular damping
		if( x < SoftBody_C_Phase1Damping)
		{
			FLOAT y = x;
			y /= SoftBody_C_Phase1Damping; // [0,1]
			y = -(y - 1.0f);//[1,0]

			_p_SoftBody->f_AngularDamping = 1.0f - y * SoftBody_C_MaxAngularDamping;			
		}		

		FLOAT t =  fMin((x*1.0f/SoftBody_C_BlendRatio),1.0f);
		FLOAT angle = lerp(t,_p_SoftBody->f_MaxAngles[0],_p_SoftBody->f_MaxAngles[1]);
		
		if(x<SoftBody_C_BlendRatio)//[0,blend ratio] ->[pi/2,pi]
		{
			x  = x/SoftBody_C_BlendRatio * Cf_Pi/2.0f + Cf_Pi/2.0f;
		}
		else //[blend ratio,1]->[pi,3pi/2]
		{
			x = (x - SoftBody_C_BlendRatio)/(1.0f - SoftBody_C_BlendRatio) * Cf_Pi/2.0f + Cf_Pi;
		}
		
		_p_SoftBody->f_Angle = fSin(x) * angle;		
	}
	else if( x < SoftBody_C_EndPhase2)
	{
		// ease out the curve
		x -= SoftBody_C_EndPhase1; // [0.6,1]->[0,0.4]
		x *= 1.0f/(SoftBody_C_EndPhase2 - SoftBody_C_EndPhase1); // [0,1]
		x = x * Cf_Pi + Cf_3PiBy2;// [3pi/2,5pi/2]

		// angle = sin(x) (+/-) 1/2 secondAngle
		if(_p_SoftBody->f_Angle>0)
		{
			_p_SoftBody->f_Angle = (fSin(x) *  _p_SoftBody->f_MaxAngles[1])/2.0f + _p_SoftBody->f_MaxAngles[1]/2.0f;
		}
		else
		{
			_p_SoftBody->f_Angle = (fSin(x) *  _p_SoftBody->f_MaxAngles[1])/2.0f - _p_SoftBody->f_MaxAngles[1]/2.0f;
		}		
	}
	else
	{
		_p_SoftBody->f_Angle = 0.0f;
	}
}

void GAO_ModifierSoftBody_BlendNextPoint(GAO_tdst_ModifierSoftBody *_p_SoftBody)
{
	MATH_tdst_Vector blendVector;

	if(_p_SoftBody->pst_NextPoint != NULL)
	{
		MATH_SubVector(&blendVector,&_p_SoftBody->st_TearingPosition,_p_SoftBody->pst_NextPoint->m_pCurrPos);
		FLOAT dZ = _p_SoftBody->abovePoints.back()->m_initialZ - _p_SoftBody->pst_NextPoint->m_initialZ;
		FLOAT blendRatio = (_p_SoftBody->f_PendulumLength - _p_SoftBody->pst_AnchorPoint->m_initialZ + _p_SoftBody->abovePoints.back()->m_initialZ)/dZ;
		MATH_ScaleEqualVector(&blendVector,blendRatio);
		MATH_AddEqualVector(_p_SoftBody->pst_NextPoint->m_pCurrPos,&blendVector);
		MATH_CopyVector(_p_SoftBody->pst_NextPoint->m_tearingVertex->m_pCurrPos,_p_SoftBody->pst_NextPoint->m_pCurrPos);
	}	
}

void GAO_ModifierSoftBody_MoveOnPendulum(GAO_tdst_ModifierSoftBody *_p_SoftBody,MATH_tdst_Vector *position,FLOAT length)
{
	if(_p_SoftBody->ul_AnimType == GAO_tdst_ModifierSoftBody::front)
	{				
		position->x = _p_SoftBody->pst_AnchorPoint->GetCurrPos()->x;
		position->z = _p_SoftBody->pst_AnchorPoint->m_initialZ - length;				
	}
	else if(_p_SoftBody->ul_AnimType == GAO_tdst_ModifierSoftBody::right)
	{			
		position->x = _p_SoftBody->pst_AnchorPoint->GetCurrPos()->x - length * fSin(_p_SoftBody->f_Angle);
		position->z = _p_SoftBody->pst_AnchorPoint->m_initialZ - length * fCos(_p_SoftBody->f_Angle);				
	}
	else
	{				
		position->x = _p_SoftBody->pst_AnchorPoint->GetCurrPos()->x + length * fSin(_p_SoftBody->f_Angle);
		position->z = _p_SoftBody->pst_AnchorPoint->m_initialZ - length * fCos(_p_SoftBody->f_Angle);				
	}
}

void GAO_ModifierSoftBody_UpdateAbovePoints(GAO_tdst_ModifierSoftBody *_p_SoftBody)
{	
	//////////////////////////////////////////////////////////////
	// using the tearing length, we can find all the above points
	/////////////////////////////////////////////////////////////
	_p_SoftBody->abovePoints.clear();
	forAllReverse(GAO_tdst_ModifierSoftBody::ZipperVertices,_p_SoftBody->zipper,item)
	{
		if((_p_SoftBody->pst_AnchorPoint->m_initialZ - (*item)->m_initialZ) < _p_SoftBody->f_PendulumLength)
		{
			_p_SoftBody->abovePoints.push_back((*item));
			(*item)->m_ulFlags |= CSoftBodyVertex::eVertexFlagFixe | CSoftBodyVertex::eVertexFlagTeared;
			if(*item != *_p_SoftBody->zipper.begin())
			{
				_p_SoftBody->pst_NextPoint = *(item+1);
			}
			else
			{
				_p_SoftBody->pst_NextPoint = NULL;
				GAO_ModifierSoftBody_Detach(_p_SoftBody);
				return;
			}
		}
	}

	//////////////////////////////////////////////////////////////
	// animate all the above points
	/////////////////////////////////////////////////////////////	
	forAll(GAO_tdst_ModifierSoftBody::ZipperVertices,_p_SoftBody->abovePoints,item)
	{
		FLOAT length = (_p_SoftBody->pst_AnchorPoint->m_initialZ - (*item)->m_initialZ);
		GAO_ModifierSoftBody_MoveOnPendulum(_p_SoftBody,(*item)->m_pCurrPos,length);			
	}	
}

void GAO_ModifierSoftBody_UpdateTearingPosition(GAO_tdst_ModifierSoftBody *_p_SoftBody)
{
	GAO_ModifierSoftBody_MoveOnPendulum(_p_SoftBody,&_p_SoftBody->st_TearingPosition, _p_SoftBody->f_PendulumLength);  		

#ifdef ACTIVE_EDITORS	
	_p_SoftBody->tearingVertices.push_back(_p_SoftBody->st_TearingPosition);
#endif
}

void GAO_ModifierSoftBody_AnimateTearingPoint(GAO_tdst_ModifierSoftBody *_p_SoftBody)
{		
	GAO_ModifierSoftBody_UpdateTearingSpeed(_p_SoftBody);
	GAO_ModifierSoftBody_UpdateTearingLength(_p_SoftBody);		
	GAO_ModifierSoftBody_UpdateAngle(_p_SoftBody);	
	GAO_ModifierSoftBody_UpdateAbovePoints(_p_SoftBody);	
	GAO_ModifierSoftBody_UpdateTearingPosition(_p_SoftBody);	
}

// AI interface
MATH_tdst_Vector* GAO_ModifierSoftBody_SetAnchorPoint(GAO_tdst_ModifierSoftBody * _p_SoftBody,MATH_tdst_Vector * _pst_Position,MATH_tdst_Vector * _pst_Speed)
{	
	MATH_tdst_Vector localVertex,localSpeed;

	//////////////////////////////////////////////////////////////
	// find the closest point on the zipper using the speed vector
	/////////////////////////////////////////////////////////////

	// convert speed vector into local space
	MATH_TransformVector(&localSpeed,&_p_SoftBody->st_InvMatrix,_pst_Speed);
	
	_p_SoftBody->pst_StartPoint = NULL;	

	CSoftBodyVertex *above,*under;

	// convert the global position into local space
	MATH_TransformVertex(&localVertex,&_p_SoftBody->st_InvMatrix,_pst_Position);


	// using speed vector, find the best point to attach to on the zipper	
	if(fabs(localSpeed.x) < fabs(localSpeed.y))
	{
		if (localSpeed.y != 0.0f)
		{
			FLOAT dY = (_p_SoftBody->pst_AnchorPoint->GetCurrPos()->y - localVertex.y);
			FLOAT dZ = dY*localSpeed.z/localSpeed.y;

			// modify the position
			localVertex.y += dY;
			localVertex.z += dZ;
		}
	}
	else
	{
		if (localSpeed.x != 0.0f)
		{
			FLOAT dX = (_p_SoftBody->pst_AnchorPoint->GetCurrPos()->x - localVertex.x);
			FLOAT dZ = dX*localSpeed.z/localSpeed.x;

			// modify the position
			localVertex.x += dX;
			localVertex.z += dZ;
		}
	}
	

	// test if the point is above all
	if( localVertex.z >= _p_SoftBody->zipper.back()->GetCurrPos()->z )
	{
		_p_SoftBody->pst_StartPoint = _p_SoftBody->zipper.back();
		MATH_CopyVector(&_p_SoftBody->st_TearingPosition,_p_SoftBody->pst_StartPoint->GetCurrPos());
	}
	// test if the point is under all
	else if(localVertex.z <= _p_SoftBody->zipper.front()->GetCurrPos()->z )
	{
		_p_SoftBody->pst_StartPoint = _p_SoftBody->zipper.front();
		MATH_CopyVector(&_p_SoftBody->st_TearingPosition,_p_SoftBody->pst_StartPoint->GetCurrPos());
	}
	else
	{
		forAll(GAO_tdst_ModifierSoftBody::ZipperVertices,_p_SoftBody->zipper,item)
		{
			if((*item)->GetCurrPos()->z > localVertex.z)
			{	
				above = *item;
				under = *(item-1);

				// interpolate between the under and above point
				FLOAT t = (localVertex.z - under->m_pCurrPos->z)/(above->m_pCurrPos->z - under->m_pCurrPos->z);
				_p_SoftBody->st_TearingPosition.x = lerp(t,under->m_pCurrPos->x,above->m_pCurrPos->x);
				_p_SoftBody->st_TearingPosition.y = under->m_pCurrPos->y;
				_p_SoftBody->st_TearingPosition.z = localVertex.z;

				// set the start point for the tearing
				_p_SoftBody->pst_StartPoint = above;
				break;
			}					
		}
	}

	ERR_X_Assert(_p_SoftBody->pst_StartPoint != NULL);

	MATH_TransformVertex(&_p_SoftBody->st_TearingPositionGlobal,_p_SoftBody->pst_Hook->pst_GlobalMatrix,&_p_SoftBody->st_TearingPosition);
	return &_p_SoftBody->st_TearingPositionGlobal;
}

void GAO_ModifierSoftBody_SetDirection(GAO_tdst_ModifierSoftBody * _p_SoftBody,ULONG ul_Direction)
{
	switch(ul_Direction)
	{
		case 0:_p_SoftBody->ul_AnimType = GAO_tdst_ModifierSoftBody::front;break;
		case 1:_p_SoftBody->ul_AnimType = GAO_tdst_ModifierSoftBody::left;break;
		case 2:_p_SoftBody->ul_AnimType = GAO_tdst_ModifierSoftBody::right;break;
		default:_p_SoftBody->ul_AnimType = GAO_tdst_ModifierSoftBody::front;break;
	}
}

void GAO_ModifierSoftBody_AttachTo(GAO_tdst_ModifierSoftBody * _p_SoftBody)
{
	// set the pendulum based on the direction of prince	
	_p_SoftBody->b_IsSomeoneAttached = TRUE;
	_p_SoftBody->f_EllapsedTime = 0.0f;
	_p_SoftBody->f_Angle = 0.0f;	
	_p_SoftBody->f_AngularDamping = 1.0f;	
	_p_SoftBody->f_TearingLength = 0.0f;
	_p_SoftBody->f_PendulumLength = _p_SoftBody->pst_AnchorPoint->m_initialZ - _p_SoftBody->st_TearingPosition.z;
	_p_SoftBody->f_TearingSpeed = _p_SoftBody->f_InitialSpeed;
	
	// based on the distance, use first angle or max_swing
	for(int i=0;i<2;i++)
	{
		if(_p_SoftBody->f_PendulumLength*fSin(_p_SoftBody->f_Angles[i]* Cf_PiBy180) > _p_SoftBody->f_MaxSwing)
		{
			// compute angle based on max swing
			_p_SoftBody->f_MaxAngles[i] = fAsin(_p_SoftBody->f_MaxSwing/_p_SoftBody->f_PendulumLength);
		}
		else
		{
			// use the angle set in the editor
			_p_SoftBody->f_MaxAngles[i] = _p_SoftBody->f_Angles[i] * Cf_PiBy180;
		}
	}

#ifdef ACTIVE_EDITORS	
	_p_SoftBody->tearingVertices.clear();
#endif
}

void GAO_ModifierSoftBody_Detach(GAO_tdst_ModifierSoftBody *_p_SoftBody)
{
	_p_SoftBody->b_IsSomeoneAttached = FALSE;


	ULONG flag = CSoftBodyVertex::eVertexFlagTearable | CSoftBodyVertex::eVertexFlagTeared;
	// detach all the above points until the start position is reached
	forAllReverse(GAO_tdst_ModifierSoftBody::ZipperVertices,_p_SoftBody->abovePoints,item)
	{
		// anchor point needs to stay fix
		if((*item)==_p_SoftBody->pst_AnchorPoint || (*item)==_p_SoftBody->pst_AnchorPoint->m_tearingVertex)
		{
			(*item)->m_ulFlags = CSoftBodyVertex::eVertexFlagFixe;
		}
		else
		{
			// every points above start point, including it, will be free to move
			if((*item)==_p_SoftBody->pst_StartPoint)
			{
				flag = CSoftBodyVertex::eVertexFlagTearable;		
			}

			(*item)->m_ulFlags = flag;
		}
	}	
}

MATH_tdst_Vector* GAO_ModifierSoftBody_GetTearingPosition(GAO_tdst_ModifierSoftBody * _p_SoftBody)
{
	MATH_TransformVertex(&_p_SoftBody->st_TearingPositionGlobal,_p_SoftBody->pst_Hook->pst_GlobalMatrix,&_p_SoftBody->st_TearingPosition);
	return &_p_SoftBody->st_TearingPositionGlobal;
}

INT GAO_ModifierSoftBody_GetMoment(GAO_tdst_ModifierSoftBody * _p_SoftBody)
{
	if( _p_SoftBody->ul_AnimType == GAO_tdst_ModifierSoftBody::front )
	{
		return 3;	
	}

	FLOAT x = _p_SoftBody->f_TearingLength/_p_SoftBody->f_DampLength;

	if( x < SoftBody_C_EndPhase0 )
	{
		return 0;		
	}
	else if( x <SoftBody_C_EndPhase1)
	{
		return 1;		
	}
	else if( x < SoftBody_C_EndPhase2)
	{
		return 2;		
	}
	else
	{
		return 3;		
	}	
}

FLOAT GAO_ModifierSoftBody_GetSwingRatio(GAO_tdst_ModifierSoftBody * _p_SoftBody)
{
	if( _p_SoftBody->ul_AnimType == GAO_tdst_ModifierSoftBody::front )
	{
		return 0.0f;	
	}

	FLOAT x = _p_SoftBody->f_TearingLength/_p_SoftBody->f_DampLength;
	
	if( x < SoftBody_C_EndPhase0 )
	{
		x *= 1.0f/SoftBody_C_EndPhase0;		
	}
	else if( x <SoftBody_C_EndPhase1 )
	{
		x -= SoftBody_C_EndPhase0;
		x *= 1.0f/(SoftBody_C_EndPhase1 - SoftBody_C_EndPhase0);
	}
	else if( x < SoftBody_C_EndPhase2)
	{
		x -= SoftBody_C_EndPhase1;
		x *= 1.0f/(SoftBody_C_EndPhase2 - SoftBody_C_EndPhase1);
	}
	else
	{
		x = 0.0f;
	}

	return x;	
}

FLOAT GAO_ModifierSoftBody_GetHeightRatio(GAO_tdst_ModifierSoftBody * _p_SoftBody)
{
	// return 0 when starting
	// return 1 when finished
	return (_p_SoftBody->f_TearingLength/_p_SoftBody->f_DampLength);
}

BOOL GAO_ModifierSoftBody_IsStillAttached(GAO_tdst_ModifierSoftBody * _p_SoftBody)
{
	return _p_SoftBody->b_IsSomeoneAttached;
}

FLOAT GAO_ModifierSoftBody_GetSpeed(GAO_tdst_ModifierSoftBody * _p_SoftBody)
{
	// return vertical speed
	return _p_SoftBody->f_TearingSpeed;
}

void GAO_ModifierSoftBody_SetInitialSpeed(GAO_tdst_ModifierSoftBody * _p_SoftBody,FLOAT speed)
{
	_p_SoftBody->f_InitialSpeed = speed;
	_p_SoftBody->f_TearingSpeed = _p_SoftBody->f_InitialSpeed;
}

// spring code
/*
void GAO_ModifierSoftBody_SetSpring(GAO_tdst_ModifierSoftBody  *p_SoftBody,CSoftBodySpring *spring)
{
	OBJ_tdst_GameObject *boneGao = spring->GetBoneGao(p_SoftBody);
	CSoftBodyVertex *v = &p_SoftBody->a_Vertices[spring->m_vertexID];

	if( v != NULL && boneGao != NULL )
	{
		// store offset vector in the bone's local space
		MATH_tdst_Vector vGlobalSpace,vBoneSpace;
		MATH_tdst_Matrix invertBoneMatrix;

		// vertex position: local(hook space) -> global -> local(bone space)
		MATH_TransformVertex(&vGlobalSpace,p_SoftBody->pst_Hook->pst_GlobalMatrix,v->GetCurrPos());
		MATH_InvertMatrix(&invertBoneMatrix,boneGao->pst_GlobalMatrix);
		MATH_TransformVertex(&vBoneSpace,&invertBoneMatrix,&vGlobalSpace);
		
		MATH_CopyVector(&spring->m_boneOffset, &vBoneSpace);

		spring->m_initialLength = MATH_f_NormVector(&vBoneSpace);		
	}	
}

OBJ_tdst_GameObject* CSoftBodySpring::GetBoneGao(GAO_tdst_ModifierSoftBody *modifier) 
{
	return ANI_pst_GetObjectByAICanal(modifier->actor,(UCHAR)m_boneChannel); 
}
*/

//BOOL GAO_ModifierSoftBody_CollideTriangle(GAO_tdst_ModifierSoftBody * _p_SoftBody,
//                                          MATH_tdst_Vector          * _pst_T1,
//                                          MATH_tdst_Vector          * _pst_T2,
//                                          MATH_tdst_Vector          * _pst_T3,
//                                          MATH_tdst_Vector          * _pst_Normal)
//{
//    BOOL               b_Collision = FALSE;
//    ULONG              ul_Counter;
//    FLOAT              f_Length;
//    FLOAT              f_Dist;
//    MATH_tdst_Vector   st_Direction;
//    MATH_tdst_Vector   st_HitPoint;
//    CSoftBodyVertex  * p_V;
//
//    return FALSE;
//
//    for(ul_Counter = 0; ul_Counter < _p_SoftBody->ul_NbVertices; ul_Counter++)
//    {
//        p_V = &_p_SoftBody->a_Vertices[ul_Counter];
//
//        if (!(p_V->m_ulFlags & CSoftBodyVertex::eVertexFlagFixe))
//        {
//            MATH_SubVector(&st_Direction, p_V->GetCurrPos(), &p_V->m_PrevPos);
//            f_Length = MATH_f_NormVector(&st_Direction);
//
//            if (f_Length > 0.0f && MATH_f_DotProduct(&st_Direction, _pst_Normal) < 0.0f)
//            {
//                MATH_NormalizeEqualVector(&st_Direction);
//
//                if (INT_FullRayTriangle(&p_V->m_PrevPos, &st_Direction, _pst_T1, _pst_T2, _pst_T3, &f_Dist, &st_HitPoint, FALSE))
//                {
//                    if (f_Dist < 0.0f)
//                    {
//                        //MATH_SubVector(&st_HitPoint, &p_V->m_PrevPos, &_pst_Sphere->st_Center);
//                        //f_Dist = MATH_f_NormVector(&st_HitPoint);
//                        //MATH_NormalizeEqualVector(&st_HitPoint);
//                        //MATH_ScaleEqualVector(&st_HitPoint, _pst_Sphere->f_Radius - f_Dist);
//                        //MATH_AddEqualVector(&st_HitPoint, &p_V->m_PrevPos);
//
//                        //MATH_CopyVector(p_V->GetCurrPos(), &st_HitPoint);
//                    }
//                    else if (f_Dist > 0.0f && f_Dist < f_Length)
//                    {
//                        MATH_CopyVector(p_V->GetCurrPos(), &st_HitPoint);
//
//                        GAO_ModifierSoftBody_Friction(_p_SoftBody, p_V, _pst_Normal);
//                        //MATH_CopyVector(&p_V->m_PrevPos, p_V->GetCurrPos());
//
//                        b_Collision = TRUE;
//                    }
//                }
//            }
//        }
//    }
//
//    return b_Collision;
//}