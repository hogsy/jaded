#ifndef TEX_MESH_INTERNAL_H
#define TEX_MESH_INTERNAL_H

#ifdef _GAMECUBE
#define WTR_ComputeNORMALS
#define WTR_ComputeOriginalsUV
#endif

#ifdef _M_IX86
#define WTR_ComputeNORMALS
#endif

typedef unsigned	u32;
typedef int			s32;

#endif // TEX_MESH_INTERNAL_H