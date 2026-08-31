// File created for Jaded, the community patched Jade engine

#pragma once

#if defined( __cplusplus )

namespace gfx
{
	class OcclusionQuery
	{
		GLuint id{};

		bool isWaiting{};
		bool isOccluded{};

	public:
		OcclusionQuery();
		~OcclusionQuery();

		void Begin() const;
		void End() const;

		bool QueryResult();

		bool IsOccluded() const;
		bool IsWaiting() const;
	};
}// namespace gfx

#else

typedef struct GfxOcclusionQuery GfxOcclusionQuery;

void Gfx_Occlusion_DrawObjects( WOR_tdst_World *world );

bool Gfx_Occlusion_QueryResult( const OBJ_tdst_GameObject *object );

bool Gfx_Occlusion_IsOccluded( const OBJ_tdst_GameObject *object );

#endif
