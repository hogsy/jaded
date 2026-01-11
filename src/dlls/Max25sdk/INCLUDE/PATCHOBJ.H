/**********************************************************************
 *<
	FILE: patchobj.h

	DESCRIPTION:  Defines Patch Mesh Object

	CREATED BY: Tom Hudson

	HISTORY: created 21 June 1995

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/

#ifndef __PATCHOBJ__ 

#define __PATCHOBJ__

#include "meshlib.h"
#include "patchlib.h"
#include "snap.h"

extern CoreExport Class_ID patchObjectClassID;

class PatchObject: public GeomObject {
		Interval geomValid;
		Interval topoValid;
		Interval texmapValid;
		Interval selectValid;
		DWORD validBits; // for the remaining constant channels
		void CopyValidity(PatchObject *fromOb, ChannelMask channels);
		
	protected:
		//  inherited virtual methods for Reference-management
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message );
	public:
		PatchMesh patch;

		// Mesh cache
		Mesh mesh;
		int cacheSteps;		// meshSteps used for the cache
		BOOL cacheAdaptive;	// adaptive switch used for cache
		TessApprox viewTess;
		TessApprox prodTess;
		BOOL meshValid;
		BOOL showMesh;
		BOOL showLattice;

		CoreExport PatchObject();
		CoreExport ~PatchObject();

		//  inherited virtual methods:

		// From BaseObject
		CoreExport int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
		CoreExport int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
		CoreExport void Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt);
		CoreExport CreateMouseCallBack* GetCreateMouseCallBack();
		CoreExport RefTargetHandle Clone(RemapDir& remap = NoRemap());

		// From Object			 
		CoreExport ObjectState Eval(TimeValue time);
		CoreExport Interval ObjectValidity(TimeValue t);

		// get and set the validity interval for the nth channel
	   	CoreExport Interval ChannelValidity(TimeValue t, int nchan);
		CoreExport void SetChannelValidity(int i, Interval v);
		CoreExport void InvalidateChannels(ChannelMask channels);

		// Convert-to-type validity
		CoreExport Interval ConvertValidity(TimeValue t);

		// Deformable object procs	
		virtual int IsDeformable() { return 1; }  
		CoreExport int NumPoints();
		CoreExport Point3 GetPoint(int i);
		CoreExport void SetPoint(int i, const Point3& p);
		
		CoreExport void PointsWereChanged();
		CoreExport void GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm=NULL,BOOL useSel=FALSE );
		CoreExport void Deform(Deformer *defProc, int useSel);

		virtual BOOL IsParamSurface() {return TRUE;}
		CoreExport  Point3 GetSurfacePoint(TimeValue t, float u, float v,Interval &iv);

		// Mappable object procs
		virtual int IsMappable() { return 1; }
		virtual void ApplyUVWMap(int type, float utile, float vtile, float wtile,
			int uflip, int vflip, int wflip, int cap,const Matrix3 &tm,int channel=0) {
				patch.ApplyUVWMap(type,utile,vtile,wtile,uflip,vflip,wflip,cap,tm,channel); }

		CoreExport int CanConvertToType(Class_ID obtype);
		CoreExport Object* ConvertToType(TimeValue t, Class_ID obtype);
		CoreExport void FreeChannels(ChannelMask chan);
		CoreExport Object *MakeShallowCopy(ChannelMask channels);
		CoreExport void ShallowCopy(Object* fromOb, ChannelMask channels);
		CoreExport void NewAndCopyChannels(ChannelMask channels);

		CoreExport DWORD GetSubselState();

		// From GeomObject
		CoreExport int IntersectRay(TimeValue t, Ray& r, float& at);
		CoreExport void GetWorldBoundBox(TimeValue t, INode *inode, ViewExp* vpt, Box3& box );
		CoreExport void GetLocalBoundBox(TimeValue t, INode *inode, ViewExp* vpt, Box3& box );
		CoreExport int IsInstanceDependent();	// Not view-dependent (yet)
		CoreExport Mesh* GetRenderMesh(TimeValue t, INode *inode, View& view, BOOL& needDelete);

		CoreExport PatchMesh& GetPatchMesh(TimeValue t);
		CoreExport Mesh& GetMesh(TimeValue t);

		// Animatable methods

		virtual void DeleteThis() { delete this; }
		virtual void FreeCaches() {patch.InvalidateGeomCache(); }
		virtual Class_ID ClassID() { return Class_ID(PATCHOBJ_CLASS_ID,0); }
		virtual void GetClassName(TSTR& s) { s = TSTR(_T("PatchObject")); }
		virtual void NotifyMe(Animatable *subAnim, int message) {}
		virtual int IsKeyable() { return 0;}
		virtual int Update(TimeValue t) { return 0; }
		virtual BOOL BypassTreeView() { return TRUE; }
		// This is the name that will appear in the history browser.
		virtual TCHAR *GetObjectName() { return _T("Patch"); }

		// Reference methods
		CoreExport void RescaleWorldUnits(float f);

		// IO
		CoreExport IOResult Save(ISave *isave);
		CoreExport IOResult Load(ILoad *iload);

		// PatchObject-specific methods
		virtual void UpdatePatchMesh(TimeValue t) {}
		CoreExport void PrepareMesh(TimeValue t);
		CoreExport BOOL ShowLattice() { return patch.GetDispFlag(DISP_LATTICE) ? TRUE : FALSE; }
		CoreExport BOOL ShowVerts() { return patch.GetDispFlag(DISP_VERTS) ? TRUE : FALSE; }
		CoreExport void SetShowLattice(BOOL sw) { if(sw) patch.SetDispFlag(DISP_LATTICE); else patch.ClearDispFlag(DISP_LATTICE); }
		CoreExport void SetShowVerts(BOOL sw) { if(sw) patch.SetDispFlag(DISP_VERTS); else patch.ClearDispFlag(DISP_VERTS); }
		CoreExport void SetMeshSteps(int steps);
		CoreExport int GetMeshSteps();
		CoreExport void SetAdaptive(BOOL sw);
		CoreExport BOOL GetAdaptive();
		CoreExport void SetViewTess(TessApprox tess);
		CoreExport TessApprox GetViewTess();
		CoreExport void SetProdTess(TessApprox tess);
		CoreExport TessApprox GetProdTess();
		CoreExport void InvalidateMesh();
	};

CoreExport ClassDesc* GetPatchObjDescriptor();

#endif // __PATCHOBJ__
