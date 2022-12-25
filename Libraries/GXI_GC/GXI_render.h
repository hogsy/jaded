
#ifndef __GXI_RENDER_H__
#define __GXI_RENDER_H__

/*
 ===================================================================================================
    Draw primitives
 ===================================================================================================
 */

LONG GXI_l_DrawElementIndexedTriangles     ( struct GEO_tdst_ElementIndexedTriangles_ *, GEO_Vertex *, MATH_tdst_Vector *,struct GEO_tdst_UV_ * , ULONG);
LONG GXI_l_CloneDrawElementIndexedTriangles( struct GEO_tdst_ElementIndexedTriangles_ *, GEO_Vertex *, MATH_tdst_Vector *,struct GEO_tdst_UV_ * , ULONG);
LONG GXI_l_DrawElementIndexedSprite(struct GEO_tdst_ElementIndexedSprite_ *,GEO_Vertex	*,	ULONG);

void GXI_DrawRectangle(f32 fx, f32 fy, f32 fw, f32 fh, u32 C , u32 T);
void GXI_DrawShadowedRectangle(f32 x, f32 y, f32 w, f32 h, u32 C);
void GXI_prepare_to_draw_material(GXColorSrc _MatSrc);

#endif /* __GXI_INIT_H__ */