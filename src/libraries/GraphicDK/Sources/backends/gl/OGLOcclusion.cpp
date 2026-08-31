// File created for Jaded, the community patched Jade engine
// Purpose: GPU occlusion query logic.
// Author:  Mark E. Sowden

#include "Precomp.h"

#include "OGLOcclusion.h"

#include "OBJects/OBJBoundingVolume.h"
#include "OBJects/OBJorient.h"
#include "WORld/WORstruct.h"

gfx::OcclusionQuery::OcclusionQuery()
{
	glGenQueries( 1, &id );
}

gfx::OcclusionQuery::~OcclusionQuery()
{
	glDeleteQueries( 1, &id );
}

void gfx::OcclusionQuery::Begin() const
{
	glBeginQuery( GL_ANY_SAMPLES_PASSED, id );
}

void gfx::OcclusionQuery::End() const
{
	glEndQuery( GL_ANY_SAMPLES_PASSED );
}

bool gfx::OcclusionQuery::QueryResult()
{
	GLuint status;
	glGetQueryObjectuiv( id, GL_QUERY_RESULT_AVAILABLE, &status );
	if ( !status )
	{
		// return the last result
		isWaiting = true;
		return isOccluded;
	}

	glGetQueryObjectuiv( id, GL_QUERY_RESULT, &status );
	isOccluded = status;
	isWaiting  = false;
	return isOccluded;
}

bool gfx::OcclusionQuery::IsOccluded() const
{
	return isOccluded;
}

bool gfx::OcclusionQuery::IsWaiting() const
{
	return isWaiting;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//	C API
//

static void DrawBox( OBJ_tdst_GameObject *object )
{
	void *bv = object->pst_BV;
	if ( OBJ_BV_IsAABBox( bv ) )
	{
		const MATH_tdst_Vector *min = OBJ_pst_BV_GetGMax( bv );
		const MATH_tdst_Vector *max = OBJ_pst_BV_GetGMin( bv );

		return;
	}

	// apparently the only other type is a sphere, and other code just skips checking so
	// despite this being a little dumb, I'll do the same
	// (god forbid we ever want to add other types!)

	MATH_tdst_Vector origin;
	MATH_TransformVertexNoScale( &origin, OBJ_pst_GetAbsoluteMatrix( object ), OBJ_pst_BV_GetCenter( bv ) );

	// we need to translate it into a box
	const float      radius = OBJ_f_BV_GetRadius( bv );
	MATH_tdst_Vector min    = { -radius, -radius, -radius };
	MATH_tdst_Vector max    = { radius, radius, radius };
}

extern "C" void Gfx_Occlusion_DrawObjects( WOR_tdst_World *world )
{
	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	glDepthMask( GL_FALSE );

	const TAB_tdst_PFelem *elem     = TAB_pst_PFtable_GetFirstElem( &world->st_VisibleObjects );
	const TAB_tdst_PFelem *lastElem = TAB_pst_PFtable_GetLastElem( &world->st_VisibleObjects );
	for ( ; elem <= lastElem; elem++ )
	{
		OBJ_tdst_GameObject *object = ( OBJ_tdst_GameObject * ) elem->p_Pointer;
		if ( OBJ_HasNoBV( object ) )
		{
			continue;
		}

		// check if we've got a query object ready to go
		gfx::OcclusionQuery *query = ( gfx::OcclusionQuery * ) object->occlusionQuery;
		if ( query == nullptr )
		{
			query = new gfx::OcclusionQuery();

			// assign it back, bleh
			object->occlusionQuery = ( GfxOcclusionQuery * ) query;
		}

		query->Begin();

		DrawBox( object );

		query->End();
	}

	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask( GL_TRUE );
}

extern "C" bool Gfx_Occlusion_QueryResult( const OBJ_tdst_GameObject *object )
{
	if ( object->occlusionQuery == nullptr )
	{
		return false;
	}

	return ( ( gfx::OcclusionQuery * ) object->occlusionQuery )->QueryResult();
}

extern "C" bool Gfx_Occlusion_IsOccluded( const OBJ_tdst_GameObject *object )
{
	if ( object->occlusionQuery == nullptr )
	{
		return false;
	}

	return ( ( gfx::OcclusionQuery * ) object->occlusionQuery )->IsOccluded();
}
