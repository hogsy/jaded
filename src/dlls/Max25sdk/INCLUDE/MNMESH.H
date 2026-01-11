/**********************************************************************
 *<
	FILE: MNMesh.h

	DESCRIPTION:  Special mesh structures useful for face and edge based mesh operations.

	CREATED BY: Steve Anderson, working for Kinetix!

	HISTORY: created March 1997 from old SDMesh and WMesh.

 *>	Copyright (c) 1996 Autodesk, Inc., All Rights Reserved.
 **********************************************************************/

// Necessary prior inclusions: max.h

// Classes:
// MNMesh
// MNVert
// MNEdge
// MNFace
// MNMeshBorder

#ifndef __MN_MESH_H_
#define __MN_MESH_H_

// Boolean types: we use the same ones defined in mesh.h
//#define MESHBOOL_UNION 1
//#define MESHBOOL_INTERSECTION 2
//#define MESHBOOL_DIFFERENCE 3

// General flags for all components
// For MNVerts, MNEdges, and MNFaces, bits 0-7 are used for general characteristics
// of the object.  Bits 8-15 are used internally to certain algorithms, for characteristics
// that only matter for a little while.  Bits 16-31 are reserved for MNMath.lib users.
#define MN_SEL (1<<0)
#define MN_DEAD (1<<1)
#define MN_TARG (1<<2)
#define MN_WHATEVER (1<<15) // Temporary flag used internally for whatever.
#define MN_USER (1<<16)	// Anything above this can be used by applications.

// Vertex flags
#define MN_VERT_SEL	MN_SEL
#define MN_VERT_DEAD MN_DEAD
#define MN_VERT_TARG MN_TARG
#define MN_VERT_DONE (1<<3)
#define MN_VERT_WHATEVER MN_WHATEVER

class MNMesh;

class MNVert : public FlagUser {
public:
	Point3 p;
	IntTab fac;	// list of faces using this vert
	IntTab edg;	// list of edges using this vert
	int orig;	// Original point this vert comes from.

	MNVert () { orig = -1; }
	DllExport int FaceIndex (int ff, int ee=-1);
	DllExport int EdgeIndex (int ee);
	void DeleteEdge (int ee) { edg.Delete (EdgeIndex(ee), 1); }
	DllExport void DeleteFace (int ff);
	DllExport void ReplaceEdge (int oe, int ne);
	DllExport void ReplaceFace (int of, int nf);
	DllExport MNVert & operator= (MNVert & from);	// Copies fac & edg too.
	DllExport void MNDebugPrint ();
	// DO NOT USE following: it does nothing and will go away in 3.0.
	DllExport void MNDebugPrint (FILE *fp, char *leading);
};

// Edge flags
#define MN_EDGE_SEL MN_SEL
#define MN_EDGE_DEAD MN_DEAD
#define MN_EDGE_TARG MN_TARG
#define MN_EDGE_INVIS (1<<3)
#define MN_EDGE_HALFINVIS (1<<4)
#define MN_EDGE_NOCROSS (1<<5)
#define MN_EDGE_TV_SEAM (1<<6)
#define MN_EDGE_WHATEVER MN_WHATEVER

// Edge goes from v1 to v2
// f1 is forward-indexing face (face on "left" if surface normal above, v2 in front)
// f2 is backward-indexing face, or -1 if no such face exists.  (Face on "right")
class MNEdge : public FlagUser {
public:
	int v1, v2;
	int f1, f2;
	int track;	// Keep track of whatever.

	MNEdge() { f1=f2=-1; v1=v2=0; track=-1; }
	MNEdge (int vv1, int vv2, int fc) { f1=fc; f2=-1; v1=vv1; v2=vv2; track=-1; }
	int OtherFace (int ff) { return (ff==f1) ? f2 : f1; }
	int OtherVert (int vv) { return (vv==v1) ? v2 : v1; }
	void Invert () { int hold=v1; v1=v2; v2=hold; hold=f1; f1=f2; f2=hold; }
	DllExport void ReplaceFace (int of, int nf, int vv1=-1);
	void ReplaceVert (int ov, int nv) { if (v1 == ov) v1 = nv; else { assert (v2==ov); v2 = nv; } }
	DllExport bool Uncrossable ();
	DllExport MNEdge & operator= (const MNEdge & from);
	DllExport void MNDebugPrint ();
	// DO NOT USE following: it does nothing and will go away in 3.0.
	DllExport void MNDebugPrint (FILE *fp, char *leading);
};

// MNFace flags:
#define MN_FACE_SEL MN_SEL
#define MN_FACE_DEAD MN_DEAD
#define MN_FACE_TARG MN_TARG
#define MN_FACE_OPEN_REGION (1<<5)	// Face is not part of closed submesh.
#define MN_FACE_CHECKED (1<<7)	// for recursive face-and-neighbor-checking
#define MN_FACE_CHANGED (1<<8)
#define MN_FACE_WHATEVER MN_WHATEVER

class MNFace : public FlagUser {
	friend class MNMesh;

	int dalloc, halloc, talloc;
public:
	int deg;	// Degree: number of vtx's and edg's that are relevant.
	int *vtx;	// Defining verts of this face.
	int *edg;
	int *tri;	// Hidden triangulation
	int *tvrt, *cvrt;
	int hdeg;
	Point3 *hvtx;	// Hidden verts
	int *htvrt, *hcvrt;	// Hidden tverts & cverts
	DWORD smGroup;
	MtlID material;
	int track;	// Keep track of whatever -- MNMesh internal use only.
	BitArray visedg, edgsel;

	MNFace() { Init(); }
	DllExport MNFace (int d, int h=0);
	DllExport MNFace (const MNFace *from);
	~MNFace () { Clear(); }
	DllExport void Init();
	DllExport void Clear();
	DllExport void SetAlloc (int d, int h=0);
	DllExport void MakePoly (int fdeg, int *vv, int *tt=NULL, int *cc=NULL, bool *vis=NULL, bool *sel=NULL);
	DllExport void Insert (int pos, int num=1);
	// In 3.0, there will be one bool Delete (int pos, int num=1, int edir=1, bool fixtri=TRUE);
	DllExport bool Delete (int pos, int num, int edir, bool fixtri);
	DllExport void Delete (int pos, int num=1, int edir=1);
	DllExport void HInsert (int pos, int num=1);
	DllExport void HDelete (int pos, int num=1);
	// in 3.0, there will be one RotateStart (int newstart, bool do_t=TRUE, bool do_c=TRUE);
	DllExport void RotateStart (int newstart);
	DllExport void RotateStart (int newstart, bool do_t, bool do_c);
	DllExport void Flip (bool do_t=TRUE, bool do_c=TRUE);	// Reverses order of verts.  0 remains start.

	DllExport int VertIndex (int vv, int ee=-1);
	DllExport int EdgeIndex (int ee, int vv=-1);
	DllExport void ReplaceVert (int ov, int nv, int ee=-1);
	DllExport void ReplaceEdge (int oe, int ne, int vv=-1);

	DllExport MNFace & operator= (const MNFace & from);
	DllExport void MNDebugPrint (bool tvprint, bool cvprint, bool triprint=FALSE, bool hinfo=TRUE);
	// DO NOT USE following: it does nothing and will go away in 3.0.
	DllExport void MNDebugPrint (FILE *fp, char *leading, bool tvprint, bool cvprint,
		bool triprint, bool hinfo);
};

class MNFaceTriCache {
friend class MNMesh;
	int fid;
	int deg;
	int *tri;
	int hdeg;
	int *htri;
	double *hbary;

	MNFaceTriCache () { tri=NULL; htri=NULL; hbary=NULL; hdeg=0; deg=0; fid=-1; }
	~MNFaceTriCache () { if (tri) delete [] tri; if (htri) delete [] htri; if (hbary) delete [] hbary; }

	DllExport void SetDeg (int degg);
	DllExport void SetHDeg (int hdegg);
};

#define MN_MESH_NONTRI (1<<0) // At least 2 triangles have been joined
#define MN_MESH_FILLED_IN (1<<1) // All topological links complete
#define MN_MESH_RATSNEST (1<<2) // Set if we've replicated points to avoid rats' nest meshes.
#define MN_MESH_NO_BAD_VERTS (1<<3)	// Set if we've established that each vert has exactly one connected component of faces & edges.
#define MN_MESH_VERTS_ORDERED (1<<4)	// Set if we've ordered the fac, edg tables in each vert.
#define MN_MESH_MAPPING (1<<5)	// Mesh has valid mapping coords.
#define MN_MESH_CVERTS (1<<6) // Mesh has valid colored verts
#define MN_MESH_HAS_VOLUME (1<<7)	// Some subset of mesh describes closed surface of solid

#define MN_MESH_CACHE_FLAGS (MN_MESH_FILLED_IN|MN_MESH_NO_BAD_VERTS|MN_MESH_VERTS_ORDERED)

class MNMeshBorder;

// Following will be used in 3.0:
//typedef MNVert * MNVP;
//typedef MNEdge * MNEP;
//typedef MNFace * MNFP;

class MNMesh : public FlagUser {
private:
 	Tab<MNVert *> v;
	Tab<MNEdge *> e;
	Tab<MNFace *> f;
	Tab<UVVert> tv;
	Tab<VertColor> cv;

	/*  For 3.0: new data format.  Above tables will not be used.
	//  This change should be transparent to developers, and should enhance speed.
	MNVP *v;
	MNEP *e;
	MNFP *f;
	UVVert *tv;
	VertColor *cv;
	int numv, nume, numf, numt, numc;
	int nv_alloc, ne_alloc, nf_alloc, nt_alloc, nc_alloc;
	*/
	Tab<MNFaceTriCache *> tcache;

	// Array allocation: these functions (& Init) have sole control over nvalloc, etc.
	void VAlloc (int num, bool keep=TRUE);
	void EAlloc (int num, bool keep=TRUE);
	void FAlloc (int num, bool keep=TRUE);
	void TVAlloc (int num, bool keep=TRUE);
	void CVAlloc (int num, bool keep=TRUE);

	// Internal part of SabinDoo method:
	void SDVEdgeFace (int id, int vid, int *fv, int *ftv, int *fcv, DWORD selLevel);

	// Internal part of recursive smoothing-group search.
	DWORD FindReplacementSmGroup (int ff, DWORD os, int call);

	// Internal parts of Boolean.
	int BooleanZip (DWORD sortFlag, float weldThresh);
	bool BoolZipSeam (IntTab *lpi, IntTab *lpj, int & starth, int & startk, float weldThresh);
	void BoolPaintClassification (int ff, DWORD classification, int calldepth);

public:
	// Basic class ops
	MNMesh () { DefaultFlags (); }
	MNMesh (const Mesh & from) { SetFromTri (from); FillInMesh (); }
	~MNMesh() { Clear(); }

	// Initialization:
	void DefaultFlags () { ClearAllFlags (); SetFlag (MN_MESH_MAPPING | MN_MESH_CVERTS); }
	//DllExport void Init ();	// will be added in 3.0.

	// Access to components
	int VNum () const { return v.Count(); }
	MNVert *V(int i) const { return v[i]; }
	Point3 & P(int i) const { return v[i]->p; }
	int ENum () const { return e.Count(); }
	MNEdge *E(int i) const { return e[i]; }
	int FNum () const { return f.Count(); }
	MNFace *F(int i) const { return f[i]; }
	int TVNum () const { return GetFlag (MN_MESH_MAPPING) ? tv.Count() : 0; }
	UVVert & TV(int i) const { return tv[i]; }
	int CVNum () const { return GetFlag (MN_MESH_CVERTS) ? cv.Count() : 0; }
	VertColor & CV(int i) const { return cv[i]; }
	DllExport int TriNum () const;

	// Adding new components -- all allocation should go through here!
	DllExport int NewTri (int a, int b, int c, DWORD smG=0, MtlID mt=0);
	DllExport int NewTri (int *vv, int *tt, int *cc, DWORD smG=0, MtlID mt=0);
	DllExport int NewQuad (int a, int b, int c, int d, DWORD smG=0, MtlID mt=0);
	DllExport int NewQuad (int *vv, int *tt, int *cc, DWORD smG=0, MtlID mt=0);
	DllExport int NewFace (MNFace *ff, int degg=0, int *vv=NULL, int *tt=NULL, int *cc=NULL, bool *vis=NULL, bool *sel=NULL);
	DllExport int RegisterEdge (int v1, int v2, int f, int fpos);
	DllExport int SimpleNewEdge (int v1, int v2);
	DllExport int NewEdge (int v1, int v2, int f, int fpos);
	DllExport int NewVert (Point3 & p, MNVert *vv=NULL);
	DllExport int NewVert (MNVert *vv);
	DllExport int NewTVert (UVVert oldtv, int uoff=0, int voff=0);
	DllExport int NewCVert (VertColor color);

	// To delete, set MN_*_DEAD flag and use following routines, which are all o(n).
	DllExport void CollapseDeadVerts ();
	DllExport void CollapseDeadEdges ();
	DllExport void CollapseDeadFaces ();
	DllExport void CollapseDeadTVerts ();	// Figures out which are dead.
	DllExport void CollapseDeadCVerts ();	// Figures out which are dead.
	void CollapseDeadStructs() {
		CollapseDeadVerts(); CollapseDeadEdges(); CollapseDeadFaces();
		CollapseDeadTVerts(); CollapseDeadCVerts();
	}
	DllExport void Clear ();	// Deletes everything.
	//DllExport void ClearAndFree ();	// For 3.0 data: deletes everything, frees all memory

	// En Masse flag-clearing and setting:
	void ClearVFlags (DWORD fl) { for (int i=0; i<v.Count(); i++) v[i]->ClearFlag (fl); }
	void ClearEFlags (DWORD fl) { for (int i=0; i<e.Count(); i++) e[i]->ClearFlag (fl); }
	void ClearFFlags (DWORD fl) { for (int i=0; i<f.Count(); i++) f[i]->ClearFlag (fl); }
	DllExport void PaintFaceFlag (int ff, DWORD fl, DWORD fenceflags=0x0);

	// I/O with regular Meshes.
	void SetFromTri (const Mesh & from) { Clear (); AddTri (from); }
	DllExport void AddTri (const Mesh & from);	// o(n) -- Add another mesh -- simple union
	DllExport void OutToTri (Mesh & tmesh);	// o(n)

	// Internal computation: each of the following 3 requires the previous.
	DllExport void FillInMesh ();	// o(n*5) or so
	DllExport void EliminateBadVerts ();	// o(n*8) or so
	DllExport void OrderVerts ();	// o(n*3) or so
	DllExport void Triangulate ();	// o(n)
	DllExport void TriangulateFace (int ff);	// o(triangles)
	void InvalidateTopoCache () { ClearFlag (MN_MESH_CACHE_FLAGS); }

	// Random useful stuff.
	DllExport void Transform (Matrix3 & xfm);	// o(n) -- transforms verts
	bool IsClosed() { for (int i=0; i<e.Count(); i++) if (e[i]->f2<0) return FALSE; return TRUE; } // o(n)
	DllExport void FaceBBox (int ff, Box3 & bbox);
	DllExport void BBox (Box3 & bbox, bool targonly=FALSE);

	// Methods for handling MN_TARG flags.
	DllExport int TargetVertsBySelection (DWORD selLevel);	// o(n)
	DllExport int TargetEdgesBySelection (DWORD selLevel);	// o(n)
	DllExport int TargetFacesBySelection (DWORD selLevel);	// o(n)
	DllExport void DetargetVertsBySharpness (float sharpval);	// o(n*deg)

	// Face-center methods
	DllExport void ComputeCenters (Point3 *ctr, bool targonly=FALSE);	// o(n)
	DllExport void ComputeCenter (int ff, Point3 & ctr);
	DllExport void ComputeSafeCenters (Point3 *ctr, bool targonly=FALSE, bool detarg=FALSE);	// o(n)
	DllExport bool ComputeSafeCenter (int ff, Point3 & ctr);	// o(deg^2)

	// Triangulation-of-polygon methods:
	DllExport void RetriangulateFace (int ff);	// o(deg^2)
	DllExport void FindExternalTriangulation (int ff, int *tri);
	DllExport void BestConvexTriangulation (int ff, int *tri=NULL);

	// Normal methods
	DllExport int FindEdgeFromVertToVert (int vrt1, int vrt2);	// o(deg)
	DllExport Point3 GetVertexNormal (int vrt);	// o(deg)
	DllExport Point3 GetEdgeNormal (int ed);	// o(deg)
	DllExport Point3 GetFaceNormal (int fc, bool nrmlz=FALSE);	//o(deg)
	Point3 GetEdgeNormal (int vrt1, int vrt2) { return GetEdgeNormal (FindEdgeFromVertToVert(vrt1, vrt2)); }

	// Smoothing-group handling
	DllExport void Resmooth (bool smooth=TRUE, bool targonly=FALSE, DWORD targmask=~0x0);	// o(n)
	DllExport DWORD CommonSmoothing (bool targonly=FALSE);	// o(n)
	DllExport DWORD GetNewSmGroup (bool targonly=FALSE);	// o(n)
	DllExport MtlID GetNewMtlID (bool targonly = FALSE); // o(n)
	DllExport DWORD GetOldSmGroup (bool targonly=FALSE);	// up to o(n).
	DllExport DWORD GetAllSmGroups (bool targonly=FALSE);	// up to o(n)
	DllExport DWORD FindReplacementSmGroup (int ff, DWORD os);
	DllExport void PaintNewSmGroup (int ff, DWORD os, DWORD ns);
	DllExport bool SeparateSmGroups (int v1, int v2);

	// Use following to unify triangles into polygons across invisible edges.
	DllExport void MakePolyMesh ();
	// NOTE: MakeConvexPolyMesh result not guaranteed for now.  Still requires MakeConvex() afterwards to be sure.
	DllExport void MakeConvexPolyMesh ();
	DllExport void RemoveEdge (int edge);
	DllExport void MakeConvex ();
	DllExport void MakeFaceConvex (int ff);
	DllExport void EliminateCollinearVerts ();
	DllExport void EliminateCoincidentVerts (float thresh=MNEPS);

	// Following set NOCROSS flags, delete INVIS flags to make "fences" for Sabin-Doo
	DllExport void FenceMaterials ();
	DllExport void FenceSmGroups ();
	DllExport void FenceFaceSel ();
	DllExport void FenceOneSidedEdges();
	DllExport void FenceNonPlanarEdges (float thresh=.9999f);
	DllExport void SetTVSeamFlags ();

	DllExport int FindFacePointTri (int ff, Point3 & pt, double *bary=NULL, int *tri=NULL, int tnum=0);
	DllExport UVVert FindFacePointTV (int ff, Point3 & pt);
	DllExport VertColor FindFacePointCV (int ff, Point3 & pt);

	// Useful for tessellation algorithms
	DllExport void Relax (float relaxval, bool targonly=TRUE);

	// Returns tv's & cv's for both ends of each edge (from f1's perspective)
	// (Very useful for creating new faces at borders.) tv[j*2] corresponds to edge j's v1.
	DllExport void FindEdgeListTCVerts (const IntTab & lp, IntTab & tv, IntTab & cv);

	// Following functions can be used to find & fix holes in a mesh, if any.
	DllExport void GetBorder (MNMeshBorder & brd, DWORD selLevel=MESH_OBJECT);
	DllExport void FillInBorders (MNMeshBorder *b=NULL);
	DllExport void FindOpenRegions ();

	// To make hidden verts behave when moving real ones, use following:
	DllExport void PrepForGeomChange ();
	DllExport void CompleteGeomChange ();

	// operators and debug printing (MNFace.cpp)
	DllExport MNMesh & operator= (const MNMesh & from);
	DllExport MNMesh & operator+= (const MNMesh & from);
	DllExport void MNDebugPrint (bool triprint=FALSE);
	DllExport void MNDebugPrintVertexNeighborhood (int vv, bool triprint=FALSE);
	DllExport bool CheckAllData ();
	// Do not use these next three; they'll be eliminated in 3.0.
	DllExport void MNDebugPrintMsg (char *msg, bool append);
	DllExport void MNDebugPrint (bool append, bool triprint);
	DllExport void AssertDataOK ();

	// Split functions maintain topological info.  (MNSplit.cpp)
	DllExport int SplitTriEdge (int ee, float prop=.5f, float thresh=MNEPS,
		bool neVis=TRUE, bool neSel=FALSE);
	DllExport int SplitTriFace (int ff, double *bary=NULL, float thresh=MNEPS,
		bool neVis=TRUE, bool neSel=FALSE);
	DllExport void SplitTri6 (int ff, double *bary=NULL, int *nv=NULL);
	// In 3.0, following two will be united to int SplitEdge (int ee, float prop=.5f);
	DllExport int SplitEdge (int ee);
	DllExport int SplitEdge (int ee, float prop);
	DllExport int SplitEdge (int ff, int ed, float prop, bool right, int *nf=NULL,
		int *ne=NULL, bool neVis=FALSE, bool neSel=FALSE, bool allconvex=FALSE);
	DllExport int IndentFace (int ff, int ei, int nv, int *ne=NULL, bool nevis=TRUE, bool nesel=FALSE);
	// In 3.0, following two will be united to SeperateFace (int ff, int a, int b, int & nf, int &ne, bool neVis=FALSE, bool neSel=FALSE);
	DllExport void SeparateFace (int ff, int a, int b, int & nf, int & ne, bool neVis, bool neSel);
	DllExport void SeparateFace (int ff, int a, int b, int & nf, int & ne);
	DllExport void Slice (Point3 & N, float off, float thresh, bool split, bool remove, DWORD selLevel);
	DllExport void DeleteFlaggedFaces (DWORD deathflags, DWORD nvCopyFlags=0x0);
	DllExport bool WeldVerts (int a, int b);
	DllExport bool WeldEdge (int ee);

	// Tessellation methods: (MNTess.cpp)
	DllExport void TessellateByEdges (float bulge);
	DllExport bool AndersonDo (float interp, DWORD selLevel, bool abortable);
	DllExport void TessellateByCenters ();

	// Sabin-Doo tessellation: (MNSabDoo.cpp)
	DllExport void SabinDoo (float interp, DWORD selLevel);
	DllExport void SabinDooVert (int vid, float interp, DWORD selLevel, Point3 *ctr);

	// Boolean functions: (MNBool.cpp)
	DllExport void PrepForBoolean ();
	DllExport bool MakeBoolean (MNMesh & m1, MNMesh & m2, int type, MeshOpProgress *mop=NULL);
};

class MNMeshBorder {
	friend class MNMesh;
	Tab<IntTab *> bdr;
	BitArray btarg;
public:
	~MNMeshBorder () { Clear(); }
	void Clear () { for (int i=0; i<bdr.Count(); i++) if (bdr[i]) delete bdr[i]; bdr.ZeroCount(); }
	int Num () { return bdr.Count(); }
	IntTab *Loop (int i) { return bdr[i]; }
	bool LoopTarg (int i) { return ((i>=0) && (i<bdr.Count()) && (btarg[i])); }
	DllExport void MNDebugPrint (MNMesh *m);
};

#endif
