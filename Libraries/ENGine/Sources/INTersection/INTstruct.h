/*$T INTstruct.h GC!1.5 12/08/99 16:25:45 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __INT_STRUCT__
#define __INT_STRUCT__

#ifndef PSX2_TARGET
#	pragma once
#endif

#include "BASe/BAStypes.h"

#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "INTConst.h"

#if defined( __cplusplus )

class SnPBitVec
{
private:
	size_t maxobjs;
	// hogsy: discussed set vs. vector - we're going vector as it should be more cpu efficient
	std::vector< bool > vec;

public:
	SnPBitVec( size_t maxobjs );

	bool Get( size_t obj_a, size_t obj_b ) const;
	bool Set( size_t obj_a, size_t obj_b, bool value );
	void Resize( size_t maxobjs );
	void Clear();
};

#endif

#if defined( __cplusplus ) && !defined( JADEFUSION )
extern "C"
{
#endif

	/*
 ---------------------------------------------------------------------------------------------------
    Sweep and Prune structures ...
 ---------------------------------------------------------------------------------------------------
 */

#if defined( __cplusplus )

	typedef struct INT_tdst_AxisNode_
	{
		OBJ_tdst_GameObject *pst_Obj;
		USHORT us_Ref;
		unsigned char uc_Flags;
		unsigned char uc_Dummy;
		union
		{
			float *pf_BVCoord;
			MATH_tdst_Vector *pst_Center;
		};
		float f_Val;
	} INT_tdst_AxisNode;

	typedef struct INT_tdst_AxisTable_
	{
		//INT_tdst_AxisNode *pst_Nodes{};
		std::array< INT_tdst_AxisNode, INT_Cul_MaxNodes > pst_Nodes;

		SnPBitVec flags;

		INT_tdst_AxisTable_() : flags( INT_Cul_MaxObjects ) {}
	} INT_tdst_AxisTable;

	typedef struct INT_tdst_SnP_Manager_
	{
		USHORT us_IndexToPlace;
		USHORT us_NbObjToRemove;
		OBJ_tdst_GameObject *apst_ObjToRemove[ INT_Cul_MaxObjects ];
		USHORT us_NbRanks;
		USHORT aus_Rank[ INT_Cul_MaxObjects ];
	} INT_tdst_SnP_Manager;

	typedef struct INT_tdst_SnP_
	{
		USHORT us_NbElems;
		INT_tdst_SnP_Manager *pst_Manager;
		INT_tdst_AxisTable *apst_AxisTable[ 3 ];
		OBJ_tdst_GameObject *apst_IndexToObj[ INT_Cul_MaxObjects ];
	} INT_tdst_SnP;

#else

typedef struct INT_tdst_AxisNode_ INT_tdst_AxisNode;
typedef struct INT_tdst_AxisTable_ INT_tdst_AxisTable_;
typedef struct INT_tdst_SnP_Manager_ INT_tdst_SnP_Manager;
typedef struct INT_tdst_SnP_ INT_tdst_SnP;

#endif

	/* ---------------------------------------------------------------------------------------------------
    Classcial Intersection structures ...
 ---------------------------------------------------------------------------------------------------
 */
	typedef struct INT_tdst_Box_
	{
		MATH_tdst_Vector st_Min;
		MATH_tdst_Vector st_Max;
	} INT_tdst_Box;

	typedef struct INT_tdst_Sphere_
	{
		MATH_tdst_Vector st_Center;
		float f_Radius;
	} INT_tdst_Sphere;

#if defined( __cplusplus ) && !defined( JADEFUSION )
}
#endif /* __cplusplus */
#endif /* __INT_STRUCT__ */
