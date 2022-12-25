//------------------------------------------------------------------------
//
// (C) Copyright 2001 Ubisoft
//
// Author		Stephane Girard
// Date			18 Mar 2002
//
// File			MDFmodifier_SoftBody.h
// Description
//
//------------------------------------------------------------------------
#ifndef __MDFMODIFIER_SOFTBODY_H__INCLUDED
#define __MDFMODIFIER_SOFTBODY_H__INCLUDED

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/INTersection/INTmain.h"
#include "bigfiles/loading/loaread.h"
#include "Engine/Sources/GEOMETRICEVENT/Partitioner/CPlane.h"

#include "Engine/Sources/COLlision/COLstruct.h"


#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif // #ifdef ACTIVE_EDITORS

// ***********************************************************************************************************************
//    Modifier desc
// ***********************************************************************************************************************

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    Soft Body flags
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef ACTIVE_EDITORS
extern BOOL SoftBody_gb_DisplayPlaneCollision;
extern BOOL SoftBody_gb_Tear;
#endif //#ifdef ACTIVE_EDITORS

#include <vector>
#include <algorithm>

struct GEO_tdst_Object_;

#define SoftBody_C_VersionNumber 6

#define SoftBody_C_InitPrevPos      0x00000001
#define SoftBody_C_FullHierarchy    0x00000002
#define SoftBody_C_NoCollisionActor 0x00000004

#define SoftBody_C_ZipperEpsilon 0.001f
#define SoftBody_C_EndPhase0 0.2f
#define SoftBody_C_EndPhase1 0.7f
#define SoftBody_C_EndPhase2 1.0f
#define SoftBody_C_BlendRatio 0.7f

#define SoftBody_C_MaxAngularDamping 0.4f
#define SoftBody_C_Phase0Damping 0.6f
#define SoftBody_C_Phase1Damping 0.2f


class CSoftBodyVertex
{
public:

    M_DeclareOperatorNewAndDelete();

    enum eVertexFlag
    {
        eVertexFlagNone	 = 0x00000000,
        eVertexFlagFixe	 = 0x00000001,
		eVertexFlagTearable = 0x00000002,
		eVertexFlagTeared = 0x00000004,		
		eVertexFlagComputed =0x00000008,		
        eVertexFlagDummy = 0xFFFFFFFF
    };

	// SC: Note
	// The order of the first 4 component of this class is important because of the
	// PS2 ASM_ModifierSoftBody_Normalize_*() functions that are using hardcoded offsets
	// within this class
	ULONG                 m_ulFlags;
    ULONG                 m_ulBoneId;
    MATH_tdst_Vector *    m_pCurrPos;
	CSoftBodyVertex*      m_tearingVertex;

	MATH_tdst_Vector      m_PrevPos;
	MATH_tdst_Vector      m_Accel;
	MATH_tdst_Vector      m_Force;
	MATH_tdst_Vector      m_InstForce;
    FLOAT                 m_u;
    FLOAT                 m_v;

	ULONG				m_tearingID;
	BOOL				m_computed;
	FLOAT				m_initialZ;

	CSoftBodyVertex(){ m_tearingVertex = NULL; }
	CSoftBodyVertex(ULONG _ul_Flag, ULONG _ul_BoneId, OBJ_tdst_GameObject * _pst_Bone, FLOAT _f_u, FLOAT _f_v)
	{
		m_ulFlags = _ul_Flag;
        m_ulBoneId = _ul_BoneId;
        m_u = _f_u;
        m_v = _f_v;

		m_tearingVertex = NULL;

        MATH_InitVectorToZero(&m_Accel);
        MATH_InitVectorToZero(&m_Force);
        MATH_InitVectorToZero(&m_InstForce);
	}

	BOOL operator==( CSoftBodyVertex& _Other )
	{
        return ((m_ulFlags == _Other.m_ulFlags) && (m_ulBoneId== _Other.m_ulBoneId));
	}	

    void Reinit()
    {
		m_ulFlags &= ~CSoftBodyVertex::eVertexFlagTeared;

        MATH_CopyVector(&m_PrevPos, m_pCurrPos);
        MATH_InitVectorToZero(&m_Accel);
        MATH_InitVectorToZero(&m_Force);
        MATH_InitVectorToZero(&m_InstForce);
    }

    void UpdatePointer(OBJ_tdst_Group * _pst_Group)
    {
        OBJ_tdst_GameObject * pBone = OBJ_pst_GroupGetByRank(_pst_Group, m_ulBoneId);
        if (pBone != NULL)
        {
            m_pCurrPos = &pBone->pst_GlobalMatrix->T;
            MATH_CopyVector(&m_PrevPos, m_pCurrPos);

        }
        else
        {
            //ERR_X_Warning(0, ERR_szFormatMessage("CSoftBodyVertex::UpdatePointer - Invalide Bone (0x%08x)\n", m_ulBoneId), NULL);
        }
    }

    void UpdatePointer(OBJ_tdst_AdditionalMatrix * _pst_AddMatrix)
    {
        m_pCurrPos = &_pst_AddMatrix->dst_Gizmo[m_ulBoneId].st_Matrix.T;
        MATH_CopyVector(&m_PrevPos, m_pCurrPos);
    }

    MATH_tdst_Vector * GetCurrPos()
    {
        return m_pCurrPos;
    }

	MATH_tdst_Vector * GetPrevPos()
	{
		return &m_PrevPos;
	}

    ULONG Load(CHAR *_pc_Buffer)
    {
        CHAR * pc_BufferSave = _pc_Buffer;
        
        m_ulFlags = LOA_ReadULong(&_pc_Buffer);
        m_ulBoneId = LOA_ReadULong(&_pc_Buffer);
        m_u = LOA_ReadFloat(&_pc_Buffer);
        m_v = LOA_ReadFloat(&_pc_Buffer);

        return (_pc_Buffer - pc_BufferSave);
    }

    #ifdef ACTIVE_EDITORS
    void Save()
    {
        SAV_Buffer(&m_ulFlags, sizeof(ULONG));
        SAV_Buffer(&m_ulBoneId, sizeof(ULONG));
        SAV_Buffer(&m_u, sizeof(FLOAT));
        SAV_Buffer(&m_v, sizeof(FLOAT));
    }

    void Print()
    {
		/*
        ERR_OutputDebugString("point %f,%f,%f\n", GetCurrPos()->x, GetCurrPos()->y, GetCurrPos()->z);
        ERR_OutputDebugString("line %f,%f,%f %f,%f,%f\n", GetCurrPos()->x, GetCurrPos()->y, GetCurrPos()->z,
                              m_PrevPos.x, m_PrevPos.y, m_PrevPos.z);

        if(m_ulFlags & eVertexFlagFixe)
        {
            ERR_OutputDebugString("sphere %f,%f,%f 0.1\n", GetCurrPos()->x, GetCurrPos()->y, GetCurrPos()->z);
        }
		*/
    }
    static ULONG GetSaveSize()
    { 
        return ((2 * sizeof(ULONG)) + (2 * sizeof(FLOAT)));
    }

    #endif // #ifdef ACTIVE_EDITORS
};

class CSoftBodyLengthConstraint
{
public:

    M_DeclareOperatorNewAndDelete();

	ULONG m_v1;
	ULONG m_v2;
	FLOAT m_fLength;

	CSoftBodyLengthConstraint(){}
	CSoftBodyLengthConstraint(ULONG _a, ULONG _b, FLOAT _l)
	{
		m_v1 = _a;
		m_v2 = _b;
		m_fLength = _l;
	}

	BOOL operator==( const CSoftBodyLengthConstraint& _Other ) const
	{
		return ((m_v1 == _Other.m_v1 && m_v2 == _Other.m_v2) || (m_v1 == _Other.m_v2 && m_v2 == _Other.m_v1));
	}

    ULONG Load(CHAR *_pc_Buffer)
    {
        CHAR * pc_BufferSave = _pc_Buffer;
        
        m_v1 = LOA_ReadULong(&_pc_Buffer);
        m_v2 = LOA_ReadULong(&_pc_Buffer);
        m_fLength = LOA_ReadFloat(&_pc_Buffer);
        
        return (_pc_Buffer - pc_BufferSave);
    }

    #ifdef ACTIVE_EDITORS
    void Save()
    {
        SAV_Buffer(&m_v1, sizeof(ULONG));
        SAV_Buffer(&m_v2, sizeof(ULONG));
        SAV_Buffer(&m_fLength, sizeof(FLOAT));
    }

    static ULONG GetSaveSize()
    {
        return (2 * sizeof(ULONG) + sizeof(FLOAT));
    }

    #endif // #ifdef ACTIVE_EDITORS
};

class CSoftBodyPlaneConstraint
{
public:
    M_DeclareOperatorNewAndDelete();

	CPlane m_LocalPlane;

    CSoftBodyPlaneConstraint() : m_LocalPlane(0.0f, 0.0f, 1.0f, 0.0) {}
	CSoftBodyPlaneConstraint(const CPlane &_p)
	{
		m_LocalPlane = _p;
	}

	BOOL operator==( const CSoftBodyPlaneConstraint& _Other ) const
	{
		return m_LocalPlane == _Other.m_LocalPlane;
	}

    CSoftBodyPlaneConstraint& operator=(const CSoftBodyPlaneConstraint& _Other)
    {
        m_LocalPlane = _Other.m_LocalPlane;

        return *this;
    }

    ULONG Load(CHAR *_pc_Buffer)
    {
        CHAR * pc_BufferSave = _pc_Buffer;
        
        m_LocalPlane.X = LOA_ReadFloat(&_pc_Buffer);
        m_LocalPlane.Y = LOA_ReadFloat(&_pc_Buffer);
        m_LocalPlane.Z = LOA_ReadFloat(&_pc_Buffer);
        m_LocalPlane.W = LOA_ReadFloat(&_pc_Buffer);
        
        return (_pc_Buffer - pc_BufferSave);
    }

    #ifdef ACTIVE_EDITORS
    void Save()
    {
        SAV_Buffer(&m_LocalPlane.X, sizeof(FLOAT));
        SAV_Buffer(&m_LocalPlane.Y, sizeof(FLOAT));
        SAV_Buffer(&m_LocalPlane.Z, sizeof(FLOAT));
        SAV_Buffer(&m_LocalPlane.W, sizeof(FLOAT));
    }

    static ULONG GetSaveSize()
    {
        return (4 * sizeof(FLOAT));
    }

    #endif // #ifdef ACTIVE_EDITORS
};

class CSoftBodyAttachment
{
public:
    M_DeclareOperatorNewAndDelete();

	OBJ_tdst_GameObject * m_pActor;
	ULONG m_Hook;
	ULONG m_TargetX;
	ULONG m_TargetZ;

	CSoftBodyAttachment(){}
	CSoftBodyAttachment(OBJ_tdst_GameObject *_inActor, ULONG _inHook, ULONG _inTargetX, ULONG _inTargetZ)
	{
		m_pActor = _inActor;
		m_Hook = _inHook;
		m_TargetX = _inTargetX;
		m_TargetZ = _inTargetZ;
	}

	BOOL operator==( const CSoftBodyAttachment& _Other ) const
	{
		return m_pActor == _Other.m_pActor;
	}
};

// spring code
/*
typedef struct GAO_tdst_ModifierSoftBody_ GAO_tdst_ModifierSoftBody;

class CSoftBodySpring
{
public:
	M_DeclareOperatorNewAndDelete(0,"ENG::MDF::CSoftBodySpring");

	FLOAT m_initialLength;
	FLOAT m_length;
	FLOAT m_k;

	ULONG m_boneChannel;
	ULONG m_vertexID;

	MATH_tdst_Vector m_boneOffset;	

	OBJ_tdst_GameObject *GetBoneGao(GAO_tdst_ModifierSoftBody *modifier);
};
*/
typedef struct GAO_tdst_ModifierSoftBody_
{
#ifdef ACTIVE_EDITORS
	ULONG				        ulCodeKey;
#endif

	// Data variables
    ULONG                       ul_Flags;
    OBJ_tdst_GameObject       * pst_Hook;
	ULONG                       ul_NbIter;
	ULONG                       ul_NbNormalizeIter;
    
    ULONG                       ul_NbVertices;
    ULONG                       ul_NbRods;
    ULONG                       ul_NbCollPlane;

    CSoftBodyVertex           * a_Vertices;	
	CSoftBodyLengthConstraint * a_Rods;
	CSoftBodyPlaneConstraint  * a_CollPlane;

	MATH_tdst_Vector            st_Gravity;
    FLOAT                       f_DTDamping;

	MATH_tdst_Vector            st_WindMin;
	MATH_tdst_Vector            st_WindMax;
	FLOAT                       f_WindUScale;
	FLOAT                       f_WindVScale;
	FLOAT                       f_WindUPan;
	FLOAT                       f_WindVPan;

    FLOAT                       f_StaticFriction;
    FLOAT                       f_KineticFriction;

	// Working variables
	FLOAT                       f_PrevDT;
	FLOAT                       f_WindUOri;
	FLOAT                       f_WindVOri;

    OBJ_tdst_GameObject *       pst_CollidedGao;
    MATH_tdst_Matrix            st_InvMatrix;
	
	// tearable soft body
	BOOL						b_IsTearable; // indicates wether the soft body is tearable or not
	BOOL						b_IsSomeoneAttached;

	typedef std::vector<CSoftBodyVertex*> ZipperVertices;
	ZipperVertices				zipper; // contains all the points of the zipper, the last point is the ancor point of the pendulum
	ZipperVertices				abovePoints; // contains all the points above the tearing position
	CSoftBodyVertex				*pst_NextPoint; // contain the next point meeting the tearing position, we need to blend its position
	CSoftBodyVertex				*pst_StartPoint; // point from which the prince starts to tear the soft body
	CSoftBodyVertex				*pst_AnchorPoint; // point that serves as the anchor of the pendulum	

	FLOAT						f_Angles[2]; // the saved angles
	FLOAT						f_MaxAngles[2]; // the used angles, depending on wether the pendulum exceed the max swing
	FLOAT						f_Angle; // current angle of the pendulum	
	FLOAT						f_AngularDamping; // damping of the speed in function of the angle
	FLOAT						f_MaxSwing; // max horinzontal swing of the pendulum in meters
	FLOAT						f_EllapsedTime; // time ellapsed since the attach			
	FLOAT						f_DampLength; // length over which the amplitude goes from max to zero
	FLOAT						f_InitialSpeed,f_TearingSpeed;
	FLOAT						f_Acceleration;
	FLOAT						f_RealDT; // DT not affected by DT damping
		
	FLOAT						f_TearingLength,f_PendulumLength;
	MATH_tdst_Vector			st_TearingPosition,st_TearingPositionGlobal; // current position of the prince	
	
	enum eAnimType
	{
		front = 0x00000001,
		right = 0x00000002,
		left = 0x00000004
	};
	ULONG						ul_AnimType;
	ULONG                       ul_NbTearable;
	
	#ifdef ACTIVE_EDITORS
	typedef std::vector<MATH_tdst_Vector> TearingVertices;
	TearingVertices				tearingVertices; // trace the trajectory of the tearing
	#endif

	// soft body attached to an actor	
	OBJ_tdst_GameObject			*actor;
	ULONG						fatherChannel;	
	MATH_tdst_Matrix			previousGM;
	float						f_DampingDistance;
	float						f_BlendRatio;
	BOOL						b_FirstInit;
    BOOL                        b_actorAssigned;

	// springs
	// spring code
	//ULONG						channels[4];
	//OBJ_tdst_GameObject			*pst_Colliders[4];
	//CSoftBodySpring	springs[10];

#ifdef ACTIVE_EDITORS
    ULONG                       ul_SizeX;
    ULONG                       ul_SizeY;	
#endif // #ifdef ACTIVE_EDITORS	
} GAO_tdst_ModifierSoftBody;

class compCSoftBodyVertex
{
public:
	bool operator()(CSoftBodyVertex *a,CSoftBodyVertex *b) const
	{
		return a->m_pCurrPos->z < b->m_pCurrPos->z;
	}
};
// ***********************************************************************************************************************
//    Modifier functions
// ***********************************************************************************************************************

void   GAO_ModifierSoftBody_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *_pst_Mod, void *);
void   GAO_ModifierSoftBody_Destroy(MDF_tdst_Modifier *_pst_Mod);
void   GAO_ModifierSoftBody_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object_ *);
void   GAO_ModifierSoftBody_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object_ *);
void   GAO_ModifierSoftBody_Reinit(MDF_tdst_Modifier *_pst_Mod);
ULONG  GAO_ModifierSoftBody_Load(MDF_tdst_Modifier *_pst_Mod, char *);
void   GAO_ModifierSoftBody_SetActor(MDF_tdst_Modifier *_pst_Mod, OBJ_tdst_GameObject* _p_Actor);

GAO_tdst_ModifierSoftBody * GAO_ModifierSoftBody_Get(OBJ_tdst_GameObject * _pst_Gao);
FLOAT  GAO_ModifierSoftBody_GetForceWindMax(OBJ_tdst_GameObject * _pst_Gao);
FLOAT  GAO_ModifierSoftBody_GetForceWind(OBJ_tdst_GameObject * _pst_Gao);
void   GAO_ModifierSoftBody_SetForceWind(OBJ_tdst_GameObject * _pst_Gao, MATH_tdst_Vector * _pst_WindMin, MATH_tdst_Vector * _pst_WindMax);
OBJ_tdst_GameObject * GAO_ModifierSoftBody_GetCollidedGao(OBJ_tdst_GameObject * _pst_Gao);

void GAO_ModifierSoftBody_SetStartPoint(GAO_tdst_ModifierSoftBody * _p_SoftBody, MATH_tdst_Vector * _pst_Position);
void GAO_ModifierSoftBody_UpdateTearingLength(GAO_tdst_ModifierSoftBody *_p_SoftBody);
void GAO_ModifierSoftBody_AnimateTearingPoint(GAO_tdst_ModifierSoftBody *p_SoftBody);
void GAO_ModifierSoftBody_BlendNextPoint(GAO_tdst_ModifierSoftBody *p_SoftBody);

// AI interface
MATH_tdst_Vector* GAO_ModifierSoftBody_SetAnchorPoint(GAO_tdst_ModifierSoftBody * _p_SoftBody,MATH_tdst_Vector * _pst_Position,MATH_tdst_Vector * _pst_Speed);
void GAO_ModifierSoftBody_SetDirection(GAO_tdst_ModifierSoftBody * _p_SoftBody,ULONG ul_Direction);
MATH_tdst_Vector* GAO_ModifierSoftBody_GetTearingPosition(GAO_tdst_ModifierSoftBody * _p_SoftBody);
void GAO_ModifierSoftBody_AttachTo(GAO_tdst_ModifierSoftBody * _p_SoftBody);
void GAO_ModifierSoftBody_Detach(GAO_tdst_ModifierSoftBody *_p_SoftBody);
FLOAT GAO_ModifierSoftBody_GetSwingRatio(GAO_tdst_ModifierSoftBody * _p_SoftBody);
INT GAO_ModifierSoftBody_GetMoment(GAO_tdst_ModifierSoftBody * _p_SoftBody);
FLOAT GAO_ModifierSoftBody_GetHeightRatio(GAO_tdst_ModifierSoftBody * _p_SoftBody);
BOOL GAO_ModifierSoftBody_IsStillAttached(GAO_tdst_ModifierSoftBody * _p_SoftBody);
FLOAT GAO_ModifierSoftBody_GetSpeed(GAO_tdst_ModifierSoftBody * _p_SoftBody);
void GAO_ModifierSoftBody_SetInitialSpeed(GAO_tdst_ModifierSoftBody * _p_SoftBody,FLOAT speed);

#ifdef ACTIVE_EDITORS

void   GAO_ModifierSoftBody_Save(MDF_tdst_Modifier *_pst_Mod);
void   GAO_ModifierSoftBody_CreateMesh(GAO_tdst_ModifierSoftBody * _p_SoftBody);
void   GAO_ModifierSoftBody_ResizePlanes(GAO_tdst_ModifierSoftBody * _p_SoftBody, ULONG _ul_OldData);
void   GAO_ModifierSoftBody_CreateFromGro(OBJ_tdst_GameObject * _pst_Gao);
void   GAO_MofifierSoftBody_CreateSkin(OBJ_tdst_GameObject * _pst_Gao, GEO_tdst_Object_ * _pst_Obj);
void   GAO_ModifierSoftBody_SetupFromGro(GAO_tdst_ModifierSoftBody * _p_SoftBody, GEO_tdst_Object_ * _pst_Obj);
// spring code
//void   GAO_ModifierSoftBody_SetSpring(GAO_tdst_ModifierSoftBody  *p_SoftBody,CSoftBodySpring *spring);
#endif // #ifdef ACTIVE_EDITORS

#endif // !defined(__MDFMODIFIER_SOFTBODY_H__INCLUDED)
