/**********************************************************************
 *<
	FILE: patch.h

	DESCRIPTION: Main include file for bezier patches

	CREATED BY: Tom Hudson

	HISTORY: Created June 21, 1995
			 June 17, 1997 TH -- Added second texture mapping channel

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/

#ifndef _PATCH_H_

#define _PATCH_H_

#include "mesh.h"
#include <hitdata.h>
#include "maxtess.h"

// Handy-dandy integer table class
typedef Tab<int> IntTab;

// Value for undefined patches and vertices
#define PATCH_UNDEFINED -1

// Number of texture mapping channels
#define PATCH_TEXTURE_CHANNELS 2

#define MULTI_PROCESSING	TRUE		// TRUE turns on mp vertex transformation

class ISave;
class ILoad;
class PatchMesh;

#define NEWPATCH

// PRVertex flags: contain clip flags, number of normals at the vertex
// and the number of normals that have already been rendered.
// fine PLANE_MASK	0x00003f00UL -- now in gfx.h
#define NORCT_MASK			0x000000ffUL
#define SPECIFIED_NORMAL	0x00004000UL
#define OUT_LEFT			0x00010000UL
#define OUT_RIGHT			0x00020000UL
#define OUT_TOP				0x00040000UL
#define OUT_BOTTOM			0x00080000UL
#define RECT_MASK			0x000f0000UL
#define RND_MASK			0xfff00000UL
#define RND_NOR0			0x00100000UL
#define RND_NOR(n)  		(RND_NOR0 << (n))

class PRVertex {
	public:
		PRVertex()	{ rFlags = 0; /*ern = NULL;*/ }
		DllExport ~PRVertex();	

		DWORD		rFlags;     
		int			pos[3];	
	};					  

// Patch vector flags
#define PVEC_INTERIOR	(1<<0)

// Patch vectors

class PatchVec {
	public:
		Point3 p;			// Location
		int vert;			// Vertex which owns this vector
		int patches[2];		// Patches using this vector
		DWORD flags;
		PatchVec() { p = Point3(0,0,0); vert = -1; patches[0] = patches[1] = -1; flags = 0; }
		DllExport PatchVec(PatchVec &from);
		void ResetData() { vert = patches[0] = patches[1] = PATCH_UNDEFINED; }
		DllExport BOOL AddPatch(int index);
		DllExport PatchVec& operator=(PatchVec& from);
		void Transform(Matrix3 &tm) { p = p * tm; }

		DllExport IOResult Save(ISave* isave);
		DllExport IOResult Load(ILoad* iload);
	};

// Patch vertex flags
#define PVERT_COPLANAR (1<<0)

// Patch vertex

class PatchVert {
	public:
		Point3 p;			// Location
		IntTab vectors;		// List of vectors attached to this vertex
		IntTab patches;		// List of patches using this vertex
		DWORD flags;
		int aux2;			// Used to track topo changes during editing
		DllExport PatchVert();
		DllExport PatchVert(PatchVert &from);
		~PatchVert() { ResetData(); }
		DllExport PatchVert& operator=(PatchVert& from);
		void ResetData() { vectors.Delete(0,vectors.Count()); patches.Delete(0,patches.Count()); }
		DllExport int FindVector(int index);
		DllExport void AddVector(int index);
		DllExport void DeleteVector(int index);
		DllExport int FindPatch(int index);
		DllExport void AddPatch(int index);
		DllExport void DeletePatch(int index);
		void Transform(Matrix3 &tm) { p = p * tm; }
		DllExport IOResult Save(ISave* isave);
		DllExport IOResult Load(ILoad* iload);
	};

class PatchEdge {
	public:
		int v1;		// Index of first vertex
		int vec12;	// Vector from v1 to v2
		int vec21;	// Vector from v2 to v1
		int v2;		// Index of second vertex
		int patch1;	// Index of first patch
		int patch2;	// Index of second patch
		int aux2;	// Used to track topo changes during editing
		DllExport PatchEdge();
		DllExport PatchEdge(PatchEdge &from);
		DllExport PatchEdge(int v1, int vec12, int vec21, int v2, int p1, int p2, int aux2=-1);
		// Dump the patch edge structure via DebugPrints
		DllExport void Dump();
	};

// Patch types

#define PATCH_UNDEF	0	// Undefined (watch out!)
#define PATCH_TRI	3	// Triangular patch
#define PATCH_QUAD	4	// Quadrilateral patch

// Patch Flags:
#define PATCH_AUTO			(1<<0)	// Interior verts computed automatically if set

// The mat ID is stored in the HIWORD of the patch flags
#define PATCH_MATID_SHIFT	16
#define PATCH_MATID_MASK	0xFFFF

class Patch {	
	public:
		int type;			// See types, above
		int	v[4];			// Can have three or four vertices
		int	vec[8];			// Can have six or eight vector points
		int	interior[4];	// Can have one or four interior vertices
		Point3 aux[9];		// Used for triangular patches only -- Degree 4 control points
		int	adjacent[4];	// Adjacent patches -- Can have three or four
		int	edge[4];		// Pointers into edge list -- Can have three or four
		DWORD	smGroup;	// Defaults to 1 -- All patches smoothed in a PatchMesh
		DWORD	flags;		// See flags, above
		int aux2;			// Used to track topo changes during editing

		DllExport Patch();	// WARNING: This does not allocate arrays -- Use SetType(type) or Patch(type)
		DllExport Patch(int type);
		DllExport Patch(Patch& fromPatch);
		DllExport ~Patch();
		DllExport void Init();
		void	setVerts(int *vrt) { memcpy(v, vrt, type * sizeof(int)); }
		void	setVerts(int a, int b, int c)  { assert(type == PATCH_TRI); v[0]=a; v[1]=b; v[2]=c; }
		void	setVerts(int a, int b, int c, int d)  { assert(type == PATCH_QUAD); v[0]=a; v[1]=b; v[2]=c; v[3]=d; }
		void	setVecs(int ab, int ba, int bc, int cb, int ca, int ac) {
			assert(type == PATCH_TRI);
			vec[0]=ab; vec[1]=ba; vec[2]=bc; vec[3]=cb; vec[4]=ca; vec[5]=ac;
			}
		void	setVecs(int ab, int ba, int bc, int cb, int cd, int dc, int da, int ad) {
			assert(type == PATCH_QUAD);
			vec[0]=ab; vec[1]=ba; vec[2]=bc; vec[3]=cb; vec[4]=cd; vec[5]=dc; vec[6]=da, vec[7]=ad;
			}
		void	setInteriors(int a, int b, int c) {
			assert(type == PATCH_TRI);
			interior[0]=a; interior[1]=b; interior[2]=c;
			}
		void	setInteriors(int a, int b, int c, int d) {
			assert(type == PATCH_QUAD);
			interior[0]=a; interior[1]=b; interior[2]=c; interior[3]=d;
			}
		int		getVert(int index)	{ return v[index]; }
		int *	getAllVerts(void)	{ return v; }
		MtlID	getMatID() {return (int)((flags>>FACE_MATID_SHIFT)&FACE_MATID_MASK);}
		void    setMatID(MtlID id) {flags &= 0xFFFF; flags |= (DWORD)(id<<FACE_MATID_SHIFT);}
		DllExport Point3 interp(PatchMesh *pMesh, float u, float v);			// Quadrilateral
		DllExport Point3 interp(PatchMesh *pMesh, float u, float v, float w);	// Triangle
		DllExport void ComputeAux(PatchMesh *pMesh, int index);
		DllExport void computeInteriors(PatchMesh* pMesh);
		DllExport void SetType(int type, BOOL init = FALSE);
		DllExport Patch& operator=(Patch& from);
		DllExport void SetAuto(BOOL sw = TRUE);
		BOOL IsAuto() { return (flags & PATCH_AUTO) ? TRUE : FALSE; }
		// Dump the patch mesh structure via DebugPrints
		DllExport void Dump();

		DllExport IOResult Save(ISave* isave);
		DllExport IOResult Load(ILoad* iload);
	};

// Separate class for patch texture verts
class TVPatch {	
	public:
		int	tv[4];			// Texture verts (always 4 here, even for tri patches)
		DllExport TVPatch();
		DllExport TVPatch(TVPatch& fromPatch);
		DllExport void Init();
		DllExport void setTVerts(int *vrt, int count);
		DllExport void setTVerts(int a, int b, int c, int d = 0);
		int		getTVert(int index)	{ return tv[index]; }
		int *	getAllTVerts(void)	{ return tv; }
		DllExport TVPatch& operator=(TVPatch& from);

		DllExport IOResult Save(ISave* isave);
		DllExport IOResult Load(ILoad* iload);
	};


// Flag definitions
#define COMP_TRANSFORM	0x0001	// forces recalc of model->screen transform; else will attempt to use cache
#define COMP_IGN_RECT	0x0002	// forces all polys to be rendered; else only those intersecting the box will be
#define COMP_LIGHTING	0x0004	// forces re-lighting of all verts (as when a light moves); else only relight moved verts

#define COMP_ALL		0x00ff

// If this bit is set then the node being displayed by this mesh is selected.
// Certain display flags only activate when this bit is set.
#define COMP_OBJSELECTED	(1<<8)
#define COMP_OBJFROZEN		(1<<9)

typedef int (*INTRFUNC)();

DllExport void setPatchIntrFunc(INTRFUNC fn);

// Special types for patch vertex hits -- Allows us to distinguish what they hit on a pick
#define PATCH_HIT_PATCH		0
#define PATCH_HIT_EDGE		1
#define PATCH_HIT_VERTEX	2
#define PATCH_HIT_VECTOR	3
#define PATCH_HIT_INTERIOR	4

class PatchSubHitRec {
	private:		
		PatchSubHitRec *next;
	public:
		DWORD	dist;
		PatchMesh *patch;
		int		index;
		int		type;

		PatchSubHitRec( DWORD dist, PatchMesh *patch, int index, int type, PatchSubHitRec *next ) 
			{ this->dist = dist; this->patch = patch; this->index = index; this->type = type; this->next = next; }

		PatchSubHitRec *Next() { return next; }		
	};

class SubPatchHitList {
	private:
		PatchSubHitRec *first;
	public:
		SubPatchHitList() { first = NULL; }
		~SubPatchHitList() {
			PatchSubHitRec *ptr = first, *fptr;
			while ( ptr ) {
				fptr = ptr;
				ptr = ptr->Next();
				delete fptr;
				}
			first = NULL;
			}	

		PatchSubHitRec *First() { return first; }
		void AddHit( DWORD dist, PatchMesh *patch, int index, int type ) {
			first = new PatchSubHitRec(dist,patch,index,type,first);
			}
	};


// Special storage class for hit records so we can know which object was hit
class PatchHitData : public HitData {
	public:
		PatchMesh *patch;
		int index;
		int type;
		PatchHitData(PatchMesh *patch, int index, int type)
			{ this->patch = patch; this->index = index; this->type = type; }
		~PatchHitData() {}
	};

// Flags for sub object hit test

// NOTE: these are the same bits used for object level.
#define SUBHIT_PATCH_SELONLY	(1<<0)
#define SUBHIT_PATCH_UNSELONLY	(1<<2)
#define SUBHIT_PATCH_ABORTONHIT	(1<<3)
#define SUBHIT_PATCH_SELSOLID	(1<<4)

#define SUBHIT_PATCH_VERTS		(1<<24)
#define SUBHIT_PATCH_VECS		(1<<25)
#define SUBHIT_PATCH_PATCHES	(1<<26)
#define SUBHIT_PATCH_EDGES		(1<<27)
#define SUBHIT_PATCH_TYPEMASK	(SUBHIT_PATCH_VERTS|SUBHIT_PATCH_VECS|SUBHIT_PATCH_EDGES|SUBHIT_PATCH_PATCHES)


// Display flags
#define DISP_VERTTICKS		(1<<0)

#define DISP_SELVERTS		(1<<10)
#define DISP_SELPATCHES		(1<<11)
#define DISP_SELEDGES		(1<<12)
#define DISP_SELPOLYS		(1<<13)

#define DISP_LATTICE		(1<<16)
#define DISP_VERTS			(1<<17)

// Selection level bits.
#define PATCH_OBJECT		(1<<0)
#define PATCH_VERTEX		(1<<1)
#define PATCH_PATCH			(1<<2)
#define PATCH_EDGE			(1<<3)

// PatchMesh flags

class PatchMesh {
	friend class Patch;

	private:
#if MULTI_PROCESSING
		static int		refCount;
		static HANDLE	xfmThread;
		static HANDLE	xfmMutex;
		static HANDLE	xfmStartEvent;
		static HANDLE	xfmEndEvent;
		friend DWORD WINAPI xfmFunc(LPVOID ptr);
#endif
		// derived data-- can be regenerated
		PRVertex 		*rVerts;		// <<< instance specific.
		GraphicsWindow 	*cacheGW;  		// identifies rVerts cache
		Box3			bdgBox;			// object space--depends on geom+topo
 
		// The number of interpolations this patch will use for mesh conversion
		int			meshSteps;
		BOOL		adaptive;
		// GAP tessellation
		TessApprox	viewTess;	// tessellation control for the interactive renderer
		TessApprox	prodTess;	// tessellation control for the production renderer

 		// Vertex and patch work arrays -- for snap code
		int			snapVCt;
		int			snapPCt;
		char		*snapV;
		char		*snapP;

		// -------------------------------------
		//
		DWORD  		flags;		  	// work flags- 

		int 		renderPatch( GraphicsWindow *gw, int index);
		int 		renderEdge( GraphicsWindow *gw, int index);
		void		checkRVertsAlloc(void);
		void		setCacheGW(GraphicsWindow *gw)	{ cacheGW = gw; }
		GraphicsWindow *getCacheGW(void)			{ return cacheGW; }

		void 		freeVerts();
		void 		freeTVerts(int channel=0);
		void 		freeVecs();
		void  		freePatches();
		void  		freeTVPatches(int channel=0);
		void  		freeEdges();
		void  		freeRVerts();
		void		freeSnapData();
		int			buildSnapData(GraphicsWindow *gw,int verts,int edges);

	public:
		// Topology
		int			numVerts;
		int			numVecs;
		int	 		numPatches;
		int			numEdges;
		Patch *		patches;
		PatchVec *	vecs;
		PatchEdge *	edges;

		// Geometry
		PatchVert *	verts;

		// Texture Coord assignment 
		int			numTVerts[PATCH_TEXTURE_CHANNELS];
		UVVert *	tVerts[PATCH_TEXTURE_CHANNELS];
		TVPatch *	tvPatches[PATCH_TEXTURE_CHANNELS];  	 

		// Material assignment
		MtlID		mtlIndex;     // object material

		// Selection
		BitArray	vertSel;  		// selected vertices
		BitArray	edgeSel;  		// selected edges
		BitArray	patchSel;  		// selected patches

		// If hit bezier vector, this is its info:
		int bezVecVert;

		// Display attribute flags
		DWORD		dispFlags;

		// Selection level
		DWORD		selLevel;


		DllExport PatchMesh();
		DllExport PatchMesh(PatchMesh& fromPatch);

		DllExport void Init();

		DllExport ~PatchMesh();

		DllExport PatchMesh& 		operator=(PatchMesh& fromPatchMesh);
		DllExport PatchMesh& 		operator=(Mesh& fromMesh);
		
		DllExport BOOL	setNumVerts(int ct, BOOL keep = FALSE);
		int				getNumVerts(void)	{ return numVerts; }
		
		DllExport BOOL	setNumTVertsChannel(int channel, int ct, BOOL keep=FALSE);
		BOOL	setNumTVerts(int ct, BOOL keep=FALSE) { return setNumTVertsChannel(0, ct, keep); }
		int		getNumTVertsChannel(int channel) const { return numTVerts[channel]; }
		int		getNumTVerts() const { return numTVerts[0]; }

		// These are parallel to patches
		// These are called from setNumPatches() to maintain the same count.
		//
		// If they are NULL and keep = TRUE they stay NULL.
		// If they are NULL and keep = FALSE they are allocated (3D verts also init themselves from the main vert array)
		// If they are non-NULL and ct = 0 they are set to NULL (and freed)
		DllExport BOOL 	setNumTVPatchesChannel(int channel, int ct, BOOL keep=FALSE, int oldCt=0);
		BOOL 	setNumTVPatches(int ct, BOOL keep=FALSE, int oldCt=0) { return setNumTVPatchesChannel(0, ct, keep, oldCt); }
		
		DllExport BOOL	setNumVecs(int ct, BOOL keep = FALSE);
		int				getNumVecs(void)	{ return numVecs; }
		
		DllExport BOOL	setNumPatches(int ct, BOOL keep = FALSE);
		int				getNumPatches(void)		{ return numPatches; }

		DllExport BOOL	setNumEdges(int ct, BOOL keep = FALSE);
		int				getNumEdges(void)		{ return numEdges; }
		
		void		setVert(int i, const Point3 &xyz)	{ verts[i].p = xyz; }
		void		setVert(int i, float x, float y, float z)	{ verts[i].p.x=x; verts[i].p.y=y; verts[i].p.z=z; }
		void		setTVertChannel(int channel, int i, const UVVert &xyz)	{ tVerts[channel][i] = xyz; }
		void		setTVert(int i, const UVVert &xyz)	{ tVerts[0][i] = xyz; }
		void		setTVertChannel(int channel, int i, float x, float y, float z)	{ tVerts[channel][i].x=x; tVerts[channel][i].y=y; tVerts[channel][i].z=z; }
		void		setTVert(int i, float x, float y, float z)	{ tVerts[0][i].x=x; tVerts[0][i].y=y; tVerts[0][i].z=z; }
		void		setTVPatchChannel(int channel, int i, TVPatch &tvp)	{ tvPatches[channel][i] = tvp; }
		void		setTVPatch(int i, TVPatch &tvp)	{ tvPatches[0][i] = tvp; }
		void		setVec(int i, const Point3 &xyz)	{ vecs[i].p = xyz; }
		void		setVec(int i, float x, float y, float z)	{ vecs[i].p.x=x; vecs[i].p.y=y; vecs[i].p.z=z; }
		
		PatchVert &	getVert(int i)					{ return verts[i];  }
		PatchVert *	getVertPtr(int i)				{ return verts+i; }
		UVVert &	getTVertChannel(int channel, int i)	{ return tVerts[channel][i];  }
		UVVert &	getTVert(int i)	{ return tVerts[0][i];  }
		UVVert *	getTVertPtrChannel(int channel, int i)	{ return tVerts[channel]+i; }
		UVVert *	getTVertPtr(int i)	{ return tVerts[0]+i; }
		TVPatch &	getTVPatchChannel(int channel, int i)	{ return tvPatches[channel][i];  }
		TVPatch &	getTVPatch(int i)	{ return tvPatches[0][i];  }
		PatchVec &	getVec(int i)					{ return vecs[i];  }
		PatchVec *	getVecPtr(int i)				{ return vecs+i; }
		PRVertex &	getRVert(int i)					{ return rVerts[i]; }
		PRVertex *	getRVertPtr(int i)				{ return rVerts+i; }
		
		void		setMtlIndex(MtlID i)	{ mtlIndex = i; }
		MtlID		getMtlIndex(void) 		{ return mtlIndex; }
	    DllExport MtlID		getPatchMtlIndex(int i);
		DllExport void		setPatchMtlIndex(int i, MtlID id); 	

		// Automatically update all the adjacency info, etc.
		// Returns TRUE if patch mesh is valid, FALSE if it's not!
		DllExport BOOL		buildLinkages();
		
		// Compute the interior bezier points for each patch in the mesh
		DllExport void		computeInteriors();

		DllExport void		render(GraphicsWindow *gw, Material *ma, RECT *rp, int compFlags, int numMat=1);
		DllExport BOOL		select(GraphicsWindow *gw, Material *ma, HitRegion *hr, int abortOnHit=FALSE, int numMat=1);
		DllExport void		snap(GraphicsWindow *gw, SnapInfo *snap, IPoint2 *p, Matrix3 &tm);
		DllExport BOOL 		SubObjectHitTest(GraphicsWindow *gw, Material *ma, HitRegion *hr,
								DWORD flags, SubPatchHitList& hitList, int numMat=1 );

		DllExport void		buildBoundingBox(void);
		DllExport Box3		getBoundingBox(Matrix3 *tm=NULL); // RB: optional TM allows the box to be calculated in any space.
		                                              // NOTE: this will be slower becuase all the points must be transformed.
		
		DllExport void 		InvalidateGeomCache();
		DllExport void 		FreeAll(); //DS
				
		// functions for use in data flow evaluation
		DllExport void 		ShallowCopy(PatchMesh *amesh, unsigned long channels);
		DllExport void 		DeepCopy(PatchMesh *amesh, unsigned long channels);
		DllExport void		NewAndCopyChannels(unsigned long channels);
		DllExport void 		FreeChannels( unsigned long channels, int zeroOthers=1);

		// Display flags
		void		SetDispFlag(DWORD f) { dispFlags |= f; }
		DWORD		GetDispFlag(DWORD f) { return dispFlags & f; }
		void		ClearDispFlag(DWORD f) { dispFlags &= ~f; }

		// Selection access
		BitArray& 	VertSel() { return vertSel;  }	
		BitArray& 	PatchSel() { return patchSel;  }	
		BitArray& 	EdgeSel() { return edgeSel;  }	

		// Constructs a vertex selection list based on the current selection level.
		DllExport BitArray 	VertexTempSel();

		// Apply the coplanar constraints to the patch mesh
		// (Optionally only apply it to selected vertices)
		DllExport void ApplyConstraints(BOOL selOnly = FALSE);

		// Create triangular or quadrilateral patch
		DllExport BOOL MakeQuadPatch(int index, int va, int vab, int vba, int vb, int vbc, int vcb, int vc, int vcd, int vdc, int vd, int vda, int vad, int i1, int i2, int i3, int i4, DWORD sm);
		DllExport BOOL MakeTriPatch(int index, int va, int vab, int vba, int vb, int vbc, int vcb, int vc, int vca, int vac, int i1, int i2, int i3, DWORD sm);

		// Get/Set mesh steps, adaptive switch
		DllExport void SetMeshSteps(int steps);
		DllExport int GetMeshSteps();
		DllExport void SetAdaptive(BOOL sw);
		DllExport BOOL GetAdaptive();
		DllExport void SetViewTess(TessApprox tess);
		DllExport TessApprox GetViewTess();
		DllExport void SetProdTess(TessApprox tess);
		DllExport TessApprox GetProdTess();

		// Find the edge index for a given vertex-vector-vector-vertex sequence
		int GetEdge(int v1, int v12, int v21, int v2, int p);

		// Apply mapping to the patch mesh
		DllExport void ApplyUVWMap(int type,
			float utile, float vtile, float wtile,
			int uflip, int vflip, int wflip, int cap,
			const Matrix3 &tm,int channel=0);

		// Tag the points in the patch components to record our topology (This stores
		// identifying values in the various aux2 fields in the Patch)
		// This info can be used after topology-changing operations to remap information
		// tied to vertices, edges and patches.
		// Returns TRUE if tagged successfully
		DllExport BOOL RecordTopologyTags();

		DllExport void Transform(Matrix3 &tm);

		// Dump the patch mesh structure via DebugPrints
		DllExport void Dump();

		DllExport IOResult Save(ISave* isave);
		DllExport IOResult Load(ILoad* iload);

	};


#endif // _PATCH_H_
