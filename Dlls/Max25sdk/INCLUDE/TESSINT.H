/**********************************************************************
 *<
	FILE: tessint.h

	DESCRIPTION: include file for gap integration

	CREATED BY: Charles Thaeler

	HISTORY: created 10 Jan 1997

 *>	Copyright (c) 1996, All Rights Reserved.
 **********************************************************************/

#ifndef TESSINT_H
#define TESSINT_H

#include "max.h"
#include "maxtess.h"

class GmSurface;

// This class describes the parameters for a projective mapper.  They
// are set when a UVW mapper modifer is applied.
class UVWMapperDesc {
public:
    UVWMapperDesc() {}
    UVWMapperDesc(int type, float utile, float vtile, float wtile,
                  int uflip, int vflip, int wflip, int cap,
                  const Matrix3 &tm,int channel);
    UVWMapperDesc(UVWMapperDesc& m);
    void ApplyMapper(Mesh* pMesh);
    void InvalidateMapping(Mesh* pMesh);

    virtual Point3 MapPoint(Point3 p, Point3 norm);

    IOResult Load(ILoad* iload);
    IOResult Save(ISave* isave);

private:

    int     mType;
    float   mUtile;
    float   mVtile;
    float   mWtile;
    int     mUflip;
    int     mVflip;
    int     mWflip;
    int     mCap;
    Matrix3 mTM;
    int     mChannel;
};

struct SurfTabEntry {
	GmSurface *gmsurf;
    UVWMapperDesc* mpChannel1Mapper;
    UVWMapperDesc* mpChannel2Mapper;

    SurfTabEntry() {
        gmsurf = NULL;
        mpChannel1Mapper = NULL;
        mpChannel2Mapper = NULL;
    }
};

typedef Tab<SurfTabEntry> SurfTab;


typedef enum {
	BEZIER_PATCH,
	GMSURFACE,
	MAX_MESH
} SurfaceType;

CoreExport TCHAR* GapVersion(void);
CoreExport int GapTessellate(void *surf, SurfaceType type, Matrix3 *otm, Mesh *mesh,
							TessApprox *tess, View *view, Mtl* mtl, BOOL dumpMiFile);

CoreExport int GapInit(void);     // this should never be used by user code
CoreExport int GapShutdown(void); // this should never be used by user code

#endif
