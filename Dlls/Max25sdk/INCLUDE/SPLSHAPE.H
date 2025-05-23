/**********************************************************************
 *<
	FILE: spline.h

	DESCRIPTION:  Defines a Spline Object Class

	CREATED BY: Tom Hudson

	HISTORY: created 23 February 1995

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/

#ifndef __SPLSHAPE_H__ 

#define __SPLSHAPE_H__

#include "Max.h"
//#include "mouseman.h"
//#include "objmode.h"
#include "shape.h"

extern CoreExport Class_ID  splineShapeClassID; 

extern HINSTANCE hInstance;

// The boolean operations
#define BOOL_UNION 0
#define BOOL_SUBTRACTION 1
#define BOOL_INTERSECTION 2

// The mirror operations
#define MIRROR_HORIZONTAL 3
#define MIRROR_VERTICAL 4
#define MIRROR_BOTH 5

// Flags used for boolean polygons
#define POLYBOOL (1 << 0)
#define POLYOUTSIDE (1 << 1)
#define POLYINSIDE (1 << 2)

// These are values for selLevel.
#define SS_OBJECT	0
#define SS_VERTEX	1
#define SS_SEGMENT	2
#define SS_SPLINE	3

#define CID_OUTLINE		CID_USER + 201
#define CID_SEGBREAK	CID_USER + 202
#define CID_SEGREFINE	CID_USER + 203
#define CID_VERTCONNECT	CID_USER + 204
#define CID_VERTINSERT	CID_USER + 205
#define CID_BOOLEAN		CID_USER + 206
#define CID_CREATELINE	CID_USER + 207

// General-purpose shape point table -- Maintains point table for each of n polygons
class SplineShapePointTab {
	public:
		int polys;
		int *pUsed;	// Tells whether polygon is affected
		Point3Tab *ptab;
		IntTab *ktab;
		IntTab *ltab;
		SplineShapePointTab();
		~SplineShapePointTab();
		void Empty();
		void Zero();
		void MakeCompatible(BezierShape& shape, BOOL clear=TRUE);
		SplineShapePointTab& operator=(SplineShapePointTab& from);
		BOOL IsCompatible(BezierShape &shape);
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);
	};

class SplineShapeVertexDelta {
	public:
		SplineShapePointTab dtab;

		void SetSize(BezierShape& shape, BOOL load=TRUE);
		void Empty() { dtab.Empty(); }
		void Zero() { dtab.Zero(); }
		void SetPoint(int poly, int i, const Point3& p) { dtab.pUsed[poly] = 1; dtab.ptab[poly][i] = p; }
		void SetKType(int poly, int i, int k) { dtab.pUsed[poly] = 1; dtab.ktab[poly][i] = k; }
		void SetLType(int poly, int i, int l) { dtab.pUsed[poly] = 1; dtab.ltab[poly][i] = l; }
		void Move(int poly, int i, const Point3& p) { dtab.pUsed[poly] = 1; dtab.ptab[poly][i] += p; }
		void Apply(BezierShape& shape);
		void UnApply(BezierShape& shape);
		void ClearUsed(int poly) { dtab.pUsed[poly] = 0; }
		void SetUsed(int poly) { dtab.pUsed[poly] = 1; }
		int IsUsed(int poly) { return dtab.pUsed[poly] ? 1 : 0; }
		SplineShapeVertexDelta& operator=(SplineShapeVertexDelta& from) { dtab = from.dtab; return *this; }
		void ApplyHandlesAndZero(BezierShape &shape, int handlePoly, int handleVert);
		BOOL IsCompatible(BezierShape &shape) { return dtab.IsCompatible(shape); }
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);
	};

class NamedVertSelSetList {
	public:
		Tab<ShapeVSel*> sets;
		Tab<TSTR*>	   names;

		~NamedVertSelSetList();
		ShapeVSel &operator[](int i) {return *sets[i];}
		int Count() {return sets.Count();}
		void AppendSet(ShapeVSel &nset,TSTR &name);
		void DeleteSet(int i);
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);
		void SetSize(BezierShape& shape);
		NamedVertSelSetList& operator=(NamedVertSelSetList& from);
		void DeleteSetElements(ShapeVSel &set,int m=1);
		int FindSet(TSTR &name);
	};

class NamedSegSelSetList {
	public:
		Tab<ShapeSSel*> sets;
		Tab<TSTR*>	   names;

		~NamedSegSelSetList();
		ShapeSSel &operator[](int i) {return *sets[i];}
		int Count() {return sets.Count();}
		void AppendSet(ShapeSSel &nset,TSTR &name);
		void DeleteSet(int i);
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);
		void SetSize(BezierShape& shape);
		NamedSegSelSetList& operator=(NamedSegSelSetList& from);
		void DeleteSetElements(ShapeSSel &set,int m=1);
		int FindSet(TSTR &name);
	};

class NamedPolySelSetList {
	public:
		Tab<ShapePSel*> sets;
		Tab<TSTR*>	   names;

		~NamedPolySelSetList();
		ShapePSel &operator[](int i) {return *sets[i];}
		int Count() {return sets.Count();}
		void AppendSet(ShapePSel &nset,TSTR &name);
		void DeleteSet(int i);
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);
		void SetSize(BezierShape& shape);
		NamedPolySelSetList& operator=(NamedPolySelSetList& from);
		void DeleteSetElements(ShapePSel &set,int m=1);
		int FindSet(TSTR &name);
	};

// Named selection set list types for SplineShape
#define NS_SS_VERT 0
#define NS_SS_SEG 1
#define NS_SS_POLY 2

class SSNamedSelSetList {
	public:
		int type;
		void *ptr;
		SSNamedSelSetList(int type, void *ptr) {this->type=type; this->ptr=ptr;}
		int Count();
		TSTR* Names(int i);
	};

class SSOutlineRecord;
typedef SSOutlineRecord* PSSOutlineRecord;
typedef Tab<PSSOutlineRecord> SSOutlineRecordTab;

class SplineShape : public ShapeObject {			   
	friend BOOL CALLBACK SplShapeCreateParamDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
	friend BOOL CALLBACK SplShapeObjectParamDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
	friend BOOL CALLBACK SplShapePolygonParamDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
	friend BOOL CALLBACK SplShapeSegmentParamDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
	friend BOOL CALLBACK SplShapeVertexParamDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
	friend class SSXFormProc;
	friend class SSOutlineCMode;
	friend class SSSegBreakCMode;
	friend class SSSegRefineCMode;
	friend class SSVertConnectCMode;
	friend class SSVertInsertCMode;
	friend class SSCreateLineCMode;
	friend class SSBooleanCMode;
	friend class SSOutlineMouseProc;
	friend class SSSegBreakMouseProc;
	friend class SSSegRefineMouseProc;
	friend class SSVertConnectMouseProc;
	friend class SSVertInsertMouseProc;
	friend class SSCreateLineMouseProc;
	friend class SSBooleanMouseProc;
	friend class SplineShapeRestore;
	friend class SSVertexRightMenu;
	friend class SSSegmentRightMenu;
	friend class SSPolygonRightMenu;
	friend class SSMBackspaceUser;
	friend class SSIBackspaceUser;
	friend class SSPickSplineAttach;

	private:
		static HWND hSplShapeParams;
		static MoveModBoxCMode *moveMode;
		static RotateModBoxCMode *rotMode;
		static UScaleModBoxCMode *uscaleMode;
		static NUScaleModBoxCMode *nuscaleMode;
		static SquashModBoxCMode *squashMode;
		static SelectModBoxCMode *selectMode;
		static SSOutlineCMode *outlineMode;
		static SSSegBreakCMode *segBreakMode;
		static SSSegRefineCMode *segRefineMode;
		static SSVertConnectCMode *vertConnectMode;
		static SSVertInsertCMode *vertInsertMode;
		static SSCreateLineCMode *createLineMode;
		static SSBooleanCMode *booleanMode;
		static ISpinnerControl *outlineSpin;
		static ISpinnerControl *weldSpin;
		static ISpinnerControl *stepsSpin;
		static ISpinnerControl *thickSpin;
		static ICustButton *iUnion;
		static ICustButton *iSubtraction;
		static ICustButton *iIntersection;
		static ICustButton *iMirrorHorizontal;
		static ICustButton *iMirrorVertical;
		static ICustButton *iMirrorBoth;
		static BOOL inOutline;
		static BOOL inSegBreak;
		static int boolType;
		static int mirrorType;
		static SSPickSplineAttach pickCB;
		Interval geomValid;
		Interval topoValid;
		Interval selectValid;
		DWORD validBits; // for the remaining constant channels
		void CopyValidity(SplineShape *fromOb, ChannelMask channels);

		// Remembered info
		BezierShape *rememberedShape;	// NULL if using all selected verts
		int rememberedPoly;
		int rememberedIndex;
		int rememberedData;

		// Vertex insertion information
		Spline3D *insertSpline;
		BezierShape *insertShape;
		int insertPoly;
		int insertVert;
		INode *insertNode;
		Matrix3 insertTM;	// Transform for the insert node

		// Create line data
		BezierShape *createShape;
		INode *createNode;
		Matrix3 createTM;	// Transform for the create node

		// Boolean info
		int boolPoly1;
		// Outline info
		SSOutlineRecordTab outRecs;
		// Transform stuff
		BOOL doingHandles;
		int handlePoly;
		int handleVert;

	protected:
		//  inherited virtual methods for Reference-management
		CoreExport RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message );
		// Special flag -- If TRUE, overrides for vertex tick display
		BOOL drawTicks;
		BOOL generalUIDisplayed;

	public:
		static IObjParam *ip;		

		BezierShape		shape;

		// Local storage of steps value -- Retains steps value when shape is adaptive
		int steps;

		NamedVertSelSetList vselSet;
		NamedSegSelSetList sselSet;
		NamedPolySelSetList pselSet;		
		Tab<Control*> cont;

		CoreExport SplineShape();
		CoreExport ~SplineShape();

		CoreExport SplineShape &operator=(SplineShape &from);

		//  inherited virtual methods:

		// From BaseObject
		CoreExport virtual int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
		CoreExport virtual int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc);
		CoreExport virtual void Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt);
		CoreExport virtual int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
		CoreExport virtual CreateMouseCallBack* GetCreateMouseCallBack();
		CoreExport virtual RefTargetHandle Clone(RemapDir& remap = NoRemap());

		// Specialized xform for bezier handles
		CoreExport void XFormHandles( SSXFormProc *xproc, TimeValue t, Matrix3& partm, Matrix3& tmAxis );

		// Generic xform procedure.
		CoreExport void XFormVerts( SSXFormProc *xproc, TimeValue t, Matrix3& partm, Matrix3& tmAxis );

		// Affine transform methods		
		CoreExport void Move( TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin=FALSE );
		CoreExport void Rotate( TimeValue t, Matrix3& partm, Matrix3& tmAxis, Quat& val, BOOL localOrigin=FALSE );
		CoreExport void Scale( TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin=FALSE );
		
		// The following is called before the first Move(), Rotate() or Scale() call
		CoreExport void TransformStart(TimeValue t);

		// The following is called after the user has completed the Move, Rotate or Scale operation and
		// the undo object has been accepted.
		CoreExport void TransformFinish(TimeValue t);		

		// The following is called when the transform operation is cancelled by a right-click and the undo
		// has been cancelled.
		CoreExport void TransformCancel(TimeValue t);		

		// From Object			 
		CoreExport ObjectState Eval(TimeValue time);
		CoreExport Interval ObjectValidity(TimeValue t);

		// Named selection set support:
		BOOL SupportsNamedSubSels() {return TRUE;}
		CoreExport void ActivateSubSelSet(TSTR &setName);
		CoreExport void NewSetFromCurSel(TSTR &setName);
		CoreExport void RemoveSubSelSet(TSTR &setName);
		CoreExport void SetupNamedSelDropDown();
		CoreExport int NumNamedSelSets();
		CoreExport TSTR GetNamedSelSetName(int i);
		CoreExport void SetNamedSelSetName(int i,TSTR &newName);
		CoreExport void NewSetByOperator(TSTR &newName,Tab<int> &sets,int op);
		CoreExport BOOL GetUniqueSetName(TSTR &name);
		CoreExport int SelectNamedSet();
		CoreExport void NSCopy();
		CoreExport void NSPaste();
		CoreExport SSNamedSelSetList GetSelSet();

		// The validty interval of channels necessary to do a convert to type
		CoreExport Interval ConvertValidity(TimeValue t);

		// get and set the validity interval for the nth channel
	   	CoreExport Interval ChannelValidity(TimeValue t, int nchan);
		CoreExport void SetChannelValidity(int i, Interval v);
		CoreExport void InvalidateChannels(ChannelMask channels);

		// Deformable object procs	
		int IsDeformable() { return 1; }  
		CoreExport int NumPoints();
		CoreExport Point3 GetPoint(int i);
		CoreExport void SetPoint(int i, const Point3& p);
		
		CoreExport void PointsWereChanged();
		CoreExport void GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm=NULL,BOOL useSel=FALSE );
		CoreExport void Deform(Deformer *defProc, int useSel);

		CoreExport int CanConvertToType(Class_ID obtype);
		CoreExport Object* ConvertToType(TimeValue t, Class_ID obtype);
		CoreExport void GetCollapseTypes(Tab<Class_ID> &clist,Tab<TSTR*> &nlist);


		CoreExport void FreeChannels(ChannelMask chan);
		CoreExport Object *MakeShallowCopy(ChannelMask channels);
		CoreExport void ShallowCopy(Object* fromOb, ChannelMask channels);
		CoreExport void NewAndCopyChannels(ChannelMask channels);

		CoreExport DWORD GetSubselState();

		// From ShapeObject
		CoreExport int IntersectRay(TimeValue t, Ray& r, float& at);
		CoreExport ObjectHandle CreateTriObjRep(TimeValue t);  // for rendering, also for deformation		
		CoreExport void GetWorldBoundBox(TimeValue t, INode *inode, ViewExp* vpt, Box3& box );
		CoreExport void GetLocalBoundBox(TimeValue t, INode *inode, ViewExp* vpt, Box3& box );
		CoreExport int NumberOfCurves();
		CoreExport BOOL CurveClosed(TimeValue t, int curve);
		CoreExport Point3 InterpCurve3D(TimeValue t, int curve, float param, int ptype=PARAM_SIMPLE);
		CoreExport Point3 TangentCurve3D(TimeValue t, int curve, float param, int ptype=PARAM_SIMPLE);
		CoreExport float LengthOfCurve(TimeValue t, int curve);
		CoreExport int NumberOfPieces(TimeValue t, int curve);
		CoreExport Point3 InterpPiece3D(TimeValue t, int curve, int piece, float param, int ptype=PARAM_SIMPLE);
		CoreExport Point3 TangentPiece3D(TimeValue t, int curve, int piece, float param, int ptype=PARAM_SIMPLE);
		BOOL CanMakeBezier() { return TRUE; }
		CoreExport void MakeBezier(TimeValue t, BezierShape &shape);
		CoreExport ShapeHierarchy &OrganizeCurves(TimeValue t, ShapeHierarchy *hier = NULL);
		CoreExport void MakePolyShape(TimeValue t, PolyShape &shape, int steps = PSHAPE_BUILTIN_STEPS, BOOL optimize = FALSE);
		CoreExport int MakeCap(TimeValue t, MeshCapInfo &capInfo, int capType);
		CoreExport int MakeCap(TimeValue t, PatchCapInfo &capInfo);

		BezierShape& GetShape() { return shape; }

		// Animatable methods

		void DeleteThis() { delete this; }
		void FreeCaches() { shape.InvalidateGeomCache(); }
		virtual Class_ID ClassID() { return splineShapeClassID; }
		CoreExport virtual void GetClassName(TSTR& s);
		// This is the name that will appear in the history browser.
		CoreExport virtual TCHAR *GetObjectName();

		// Controller stuff for animatable points
		int NumSubs()  {return cont.Count();}
		CoreExport Animatable* SubAnim(int i);
		CoreExport TSTR SubAnimName(int i);
		CoreExport BOOL AssignController(Animatable *control,int subAnim);
		int SubNumToRefNum(int subNum) {return subNum;}
		CoreExport BOOL SelectSubAnim(int subNum);

		// Reference methods
		CoreExport void RescaleWorldUnits(float f);
		int NumRefs() {return cont.Count();}
		CoreExport RefTargetHandle GetReference(int i);
		CoreExport void SetReference(int i, RefTargetHandle rtarg);
		CoreExport void CreateContArray();
		CoreExport void SynchContArray();
		CoreExport void InsertPointConts(int index, int count);
		CoreExport void DeletePointConts(BitArray &set);
		CoreExport void ReversePointConts(int index, int count, BOOL keepFirst);
		CoreExport void NullPointConts(int index, int count);
		CoreExport void NullPolyPointConts(int poly);
		CoreExport void PlugControllersSel(TimeValue t);
		CoreExport BOOL PlugControl(TimeValue t,int i);
		CoreExport void SetPointAnim(TimeValue t, int poly, int vert, Point3 pt);
		CoreExport BOOL CloneVertCont(int from, int to);

		// Editable spline stuff follows...
		CoreExport void SetRollupPage(IObjParam *ip, BOOL creating);
		CoreExport void RemoveRollupPage(IObjParam *ip);
		CoreExport virtual void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
		CoreExport virtual void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);
		CoreExport int GetSubobjectLevel();
		CoreExport void SetSubobjectLevel(int level);
		CoreExport void ActivateSubobjSel(int level, XFormModes& modes );
		int NeedUseSubselButton() { return 0; }
		CoreExport void SelectSubComponent( HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert );
		CoreExport void ClearSelection(int level);		
		CoreExport void SelectAll(int level);
		CoreExport void InvertSelection(int level);
		CoreExport void GetSubObjectCenters(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc);
		CoreExport void GetSubObjectTMs(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc);
		CoreExport int SubObjectIndex(HitRecord *hitRec);
		CoreExport void UpdatePolyVertCount(HWND hwnd = NULL);
		CoreExport void MultiAttachObject(INodeTab &nodeTab);
		CoreExport void BeginOutlineMove(TimeValue t);
		CoreExport void OutlineMove( TimeValue t, float amount );
		CoreExport void EndOutlineMove(TimeValue t,BOOL accept=TRUE);
		CoreExport void StartOutlineMode();
		CoreExport void DoOutline();
		CoreExport void StartSegBreakMode();
		CoreExport void DoSegBreak(ViewExp *vpt, BezierShape *shape, int poly, int seg, IPoint2 p);
		CoreExport void StartSegRefineMode(int type);
		CoreExport void DoSegRefine(ViewExp *vpt, BezierShape *shape, int poly, int seg, IPoint2 p);
		CoreExport void StartVertConnectMode();
		CoreExport void DoVertConnect(ViewExp *vpt, BezierShape *shape, int poly1, int vert1, int poly2, int vert2);
		CoreExport void StartVertInsertMode(int controlID);
		CoreExport int StartVertInsert(ViewExp *vpt, BezierShape *shape, int poly, int seg, int vert);
		CoreExport void EndVertInsert();
		CoreExport void StartCreateLineMode();
		CoreExport BOOL StartCreateLine(BezierShape **shape);
		CoreExport void EndCreateLine();
		CoreExport BOOL BooleanStartUp();
		CoreExport void StartBooleanMode();
		CoreExport void DoBoolean(int poly2);
		CoreExport int DoAttach(INode *node, BezierShape *attShape);
		CoreExport void DoVertBreak();
		CoreExport void DoVertWeld();
		CoreExport void DoMakeFirst();
		CoreExport void DoVertDelete();
		CoreExport void DoSegDelete();
		CoreExport void DoSegDetach(int sameShape, int copy, int reorient);
		CoreExport void DoSegDivide(int divisions);
		CoreExport void DoPolyClose();
		CoreExport void DoPolyDetach(int copy, int reorient);
		CoreExport void DoPolyMirror(int type, int copy);
		CoreExport void DoPolyDelete();
		CoreExport void DoPolyReverse();
		CoreExport void SetBoolOperation(int type) { boolType = type; }
		CoreExport void SetMirrorOperation(int type) { mirrorType = type; }
		CoreExport int GetBoolOperation() { return boolType; }
		CoreExport int GetMirrorOperation() { return mirrorType; }
		CoreExport int GetBoolCursorID();
		CoreExport int GetBoolMirrString(int type);
		CoreExport void SetBooleanButton();
		CoreExport void SetMirrorButton();
		CoreExport void ChangeSelVerts(int type);
		CoreExport void ChangeRememberedVert(int type);
		CoreExport int RememberVertThere(HWND hWnd, IPoint2 m);
		CoreExport void SetRememberedVertType(int type);
		CoreExport void ChangeSelSegs(int type);
		CoreExport void ChangeRememberedSeg(int type);
		CoreExport int RememberSegThere(HWND hWnd, IPoint2 m);
		CoreExport void SetRememberedSegType(int type);
		CoreExport void ChangeSelPolys(int type);
		CoreExport void ChangeRememberedPoly(int type);
		CoreExport int RememberPolyThere(HWND hWnd, IPoint2 m);
		CoreExport void SetRememberedPolyType(int type);

		CoreExport void SplineShapeClone( SplineShape *source );

		// The following methods do the job and update controllers, named selections, etc.
		CoreExport void DeleteSpline(int poly);
		CoreExport void InsertSpline(Spline3D *spline, int poly);
		CoreExport void Reverse(int poly, BOOL keepFirst = FALSE);
		CoreExport void DeleteKnot(int poly, int index);
		CoreExport void AddKnot(int poly, SplineKnot &k, int where = -1);
		CoreExport BOOL Append(int poly, Spline3D *spline, BOOL weldCoincidentFirstVertex=TRUE);
		CoreExport BOOL Prepend(int poly, Spline3D *spline, BOOL weldCoincidentLastVertex=TRUE);
		CoreExport void ReplaceSpline(int poly, Spline3D *spline);
		CoreExport BOOL DeleteSelVerts(int poly);
		CoreExport BOOL DeleteSelSegs(int poly);
		CoreExport void MakeFirst(int poly, int index);

		// Support for general parameters
		CoreExport void SetOptimize(BOOL sw);
		CoreExport void SetAdaptive(BOOL sw);
		CoreExport void SetSteps(int n);
		CoreExport void SetGenUVs(BOOL sw);
		CoreExport void SetRenderable(BOOL sw);
		CoreExport void SetThickness(float t);

		// Store current topology in the BezierShape
		CoreExport void RecordTopologyTags();

		// Re-match named selection sets, etc. with changed topology (Call RecordTopologyTags
		// before making the changes to the shape, then call this)
		CoreExport void ResolveTopoChanges(BezierShape *shape = NULL);

		// IO
		CoreExport IOResult Save(ISave *isave);
		CoreExport IOResult Load(ILoad *iload);

	};				

class SSPickSplineAttach : 
		public PickModeCallback,
		public PickNodeCallback {
	public:		
		SplineShape *ss;
		
		SSPickSplineAttach() {ss=NULL;}

		BOOL HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags);
		BOOL Pick(IObjParam *ip,ViewExp *vpt);

		void EnterMode(IObjParam *ip);
		void ExitMode(IObjParam *ip);

		HCURSOR GetHitCursor(IObjParam *ip);

		BOOL Filter(INode *node);
		
		PickNodeCallback *GetFilter() {return this;}

		BOOL RightClick(IObjParam *ip,ViewExp *vpt)	{return TRUE;}
	};

class SSOutlineTransformer : public Transformer {
 	public:
 		CoreExport Point3 Eval(ViewExp *vpt);
		SSOutlineTransformer(IObjParam *i) : Transformer(i) {}
 		};	

class SSOutlineMouseProc : public MouseCallBack {
	private:
		SSOutlineTransformer outlineTrans;
		SplineShape *ss;
		IObjParam *ip;
		Point3 p0, p1;
		IPoint2 sp0;

	public:
		SSOutlineMouseProc(SplineShape* shp, IObjParam *i)
			: outlineTrans(i) {ss=shp;ip=i;}
		int proc( 
			HWND hwnd, 
			int msg, 
			int point, 
			int flags, 
			IPoint2 m );
	};

class SSOutlineSelectionProcessor : public GenModSelectionProcessor {
	protected:
		HCURSOR GetTransformCursor();
		
	public:
		SSOutlineSelectionProcessor(SSOutlineMouseProc *mc, SplineShape *s, IObjParam *i) 
			: GenModSelectionProcessor(mc,s,i) {}
	};


class SSOutlineCMode : public CommandMode {
	private:
		ChangeFGObject fgProc;
		SSOutlineSelectionProcessor mouseProc;
		SSOutlineMouseProc eproc;
		SplineShape* ss;

	public:
		SSOutlineCMode(SplineShape* ss, IObjParam *i) :
			fgProc(ss), mouseProc(&eproc,ss,i), eproc(ss,i) {this->ss=ss;}

		int Class() { return MODIFY_COMMAND; }
		int ID() { return CID_OUTLINE; }
		MouseCallBack *MouseProc(int *numPoints) { *numPoints=2; return &mouseProc; }
		ChangeForegroundCallback *ChangeFGProc() { return &fgProc; }
		BOOL ChangeFG( CommandMode *oldMode ) { return oldMode->ChangeFGProc() != &fgProc; }
		void EnterMode();
		void ExitMode();
	};

/*-------------------------------------------------------------------*/

class SSSegBreakTransformer : public Transformer {
 	public:
 		CoreExport Point3 Eval(ViewExp *vpt);
		SSSegBreakTransformer(IObjParam *i) : Transformer(i) {}
 		};	

class SSSegBreakMouseProc : public MouseCallBack {
	private:
		SplineShape *ss;
		IObjParam *ip;
		IPoint2 om;

	protected:
		HCURSOR GetTransformCursor();
		BOOL HitTest( ViewExp *vpt, IPoint2 *p, int type, int flags );
		BOOL AnyHits( ViewExp *vpt ) { return vpt->NumSubObjHits(); }		

	public:
		SSSegBreakMouseProc(SplineShape* spl, IObjParam *i) { ss=spl; ip=i; }
		int proc( 
			HWND hwnd, 
			int msg, 
			int point, 
			int flags, 
			IPoint2 m );
//		int override(int mode) { return CLICK_DOWN_POINT; }
	};

class SSSegBreakCMode : public CommandMode {
	private:
		ChangeFGObject fgProc;
		SSSegBreakMouseProc eproc;
		SplineShape* ss;

	public:
		SSSegBreakCMode(SplineShape* spl, IObjParam *i) :
			fgProc(spl), eproc(spl,i) {ss=spl;}

		int Class() { return MODIFY_COMMAND; }
		int ID() { return CID_SEGBREAK; }
		MouseCallBack *MouseProc(int *numPoints) { *numPoints=1; return &eproc; }
		ChangeForegroundCallback *ChangeFGProc() { return &fgProc; }
		BOOL ChangeFG( CommandMode *oldMode ) { return oldMode->ChangeFGProc() != &fgProc; }
		void EnterMode();
		void ExitMode();
	};

/*-------------------------------------------------------------------*/

#define REFINE_VERT 0
#define REFINE_SEG 1

class SSSegRefineMouseProc : public MouseCallBack {
	private:
		SplineShape *ss;
		IObjParam *ip;
		IPoint2 om;
		int type; // See above
	
	protected:
		HCURSOR GetTransformCursor();
		BOOL HitTest( ViewExp *vpt, IPoint2 *p, int type, int flags );
		BOOL AnyHits( ViewExp *vpt ) { return vpt->NumSubObjHits(); }		

	public:
		SSSegRefineMouseProc(SplineShape* spl, IObjParam *i) { ss=spl; ip=i; }
		int proc( 
			HWND hwnd, 
			int msg, 
			int point, 
			int flags, 
			IPoint2 m );
		void SetType(int type) { this->type = type; }
	};

class SSSegRefineCMode : public CommandMode {
	private:
		ChangeFGObject fgProc;
		SSSegRefineMouseProc eproc;
		SplineShape* ss;
		int type; // See above

	public:
		SSSegRefineCMode(SplineShape* spl, IObjParam *i) :
			fgProc(spl), eproc(spl,i) {ss=spl;}

		int Class() { return MODIFY_COMMAND; }
		int ID() { return CID_SEGREFINE; }
		MouseCallBack *MouseProc(int *numPoints) { *numPoints=1; return &eproc; }
		ChangeForegroundCallback *ChangeFGProc() { return &fgProc; }
		BOOL ChangeFG( CommandMode *oldMode ) { return oldMode->ChangeFGProc() != &fgProc; }
		void EnterMode();
		void ExitMode();
		void SetType(int type) { this->type = type; eproc.SetType(type); }
	};

/*-------------------------------------------------------------------*/

class SSVertConnectMouseProc : public MouseCallBack {
	private:
		SplineShape *ss;
		IObjParam *ip;
		IPoint2 om;

	protected:
		HCURSOR GetTransformCursor();
		BOOL HitTest( ViewExp *vpt, IPoint2 *p, int type, int flags );
		BOOL AnyHits( ViewExp *vpt ) { return vpt->NumSubObjHits(); }		
		BOOL HitAnEndpoint(ViewExp *vpt, IPoint2 *p, BezierShape *shape, int poly, int vert,
			BezierShape **shapeOut, int *polyOut, int *vertOut);
	public:
		SSVertConnectMouseProc(SplineShape* spl, IObjParam *i) { ss=spl; ip=i; }
		int proc( 
			HWND hwnd, 
			int msg, 
			int point, 
			int flags, 
			IPoint2 m );
	};

class SSVertConnectCMode : public CommandMode {
	private:
		ChangeFGObject fgProc;
		SSVertConnectMouseProc eproc;
		SplineShape* ss;

	public:
		SSVertConnectCMode(SplineShape* spl, IObjParam *i) :
			fgProc(spl), eproc(spl,i) {ss=spl;}

		int Class() { return MODIFY_COMMAND; }
		int ID() { return CID_VERTCONNECT; }
		MouseCallBack *MouseProc(int *numPoints) { *numPoints=2; return &eproc; }
		ChangeForegroundCallback *ChangeFGProc() { return &fgProc; }
		BOOL ChangeFG( CommandMode *oldMode ) { return oldMode->ChangeFGProc() != &fgProc; }
		void EnterMode();
		void ExitMode();
	};

/*-------------------------------------------------------------------*/

class SSVertInsertMouseProc : public MouseCallBack {
	private:
		SplineShape *ss;
		IObjParam *ip;
		IPoint2 om;

	protected:
		HCURSOR GetTransformCursor();
		BOOL HitTest( ViewExp *vpt, IPoint2 *p, int type, int flags, int hitType );
		BOOL AnyHits( ViewExp *vpt ) { return vpt->NumSubObjHits(); }		
		BOOL InsertWhere(ViewExp *vpt, IPoint2 *p, BezierShape **shapeOut, int *polyOut,int *segOut, int *vertOut);
	public:
		SSVertInsertMouseProc(SplineShape* spl, IObjParam *i) { ss=spl; ip=i; }
		int proc( 
			HWND hwnd, 
			int msg, 
			int point, 
			int flags, 
			IPoint2 m );
		int override(int mode) { return CLICK_DOWN_POINT; }
	};

class SSVertInsertCMode : public CommandMode {
	private:
		ChangeFGObject fgProc;
		SSVertInsertMouseProc eproc;
		SplineShape* ss;
		int control;	// ID of the resource button
	public:
		SSVertInsertCMode(SplineShape* spl, IObjParam *i) :
			fgProc(spl), eproc(spl,i) {ss=spl; control= -1;}

		int Class() { return MODIFY_COMMAND; }
		int ID() { return CID_VERTINSERT; }
		MouseCallBack *MouseProc(int *numPoints) { *numPoints=999999; return &eproc; }
		ChangeForegroundCallback *ChangeFGProc() { return &fgProc; }
		BOOL ChangeFG( CommandMode *oldMode ) { return oldMode->ChangeFGProc() != &fgProc; }
		void EnterMode();
		void ExitMode();
		void SetControl(int id) { control = id; }
	};

/*-------------------------------------------------------------------*/

class SSCreateLineMouseProc : public MouseCallBack {
	private:
		SplineShape *ss;
		IObjParam *ip;
		IPoint2 om;

	protected:
		BOOL HitTest( ViewExp *vpt, IPoint2 *p, int type, int flags, int hitType );
		BOOL AnyHits( ViewExp *vpt ) { return vpt->NumSubObjHits(); }		
		BOOL InsertWhere(ViewExp *vpt, IPoint2 *p, BezierShape **shapeOut, int *polyOut, int *vertOut);
	public:
		SSCreateLineMouseProc(SplineShape* spl, IObjParam *i) { ss=spl; ip=i; }
		int proc( 
			HWND hwnd, 
			int msg, 
			int point, 
			int flags, 
			IPoint2 m );
		int override(int mode) { return CLICK_DOWN_POINT; }
	};

class SSCreateLineCMode : public CommandMode {
	private:
		ChangeFGObject fgProc;
		SSCreateLineMouseProc eproc;
		SplineShape* ss;

	public:
		SSCreateLineCMode(SplineShape* spl, IObjParam *i) :
			fgProc(spl), eproc(spl,i) {ss=spl;}

		int Class() { return MODIFY_COMMAND; }
		int ID() { return CID_CREATELINE; }
		MouseCallBack *MouseProc(int *numPoints) { *numPoints=999999; return &eproc; }
		ChangeForegroundCallback *ChangeFGProc() { return &fgProc; }
		BOOL ChangeFG( CommandMode *oldMode ) { return oldMode->ChangeFGProc() != &fgProc; }
		void EnterMode();
		void ExitMode();
	};

/*-------------------------------------------------------------------*/

class SSBooleanMouseProc : public MouseCallBack {
	private:
		SplineShape *ss;
		IObjParam *ip;
		IPoint2 om;

	protected:
		HCURSOR GetTransformCursor();
		BOOL HitTest( ViewExp *vpt, IPoint2 *p, int type, int flags );
		BOOL AnyHits( ViewExp *vpt ) { return vpt->NumSubObjHits(); }		
	public:
		SSBooleanMouseProc(SplineShape* spl, IObjParam *i) { ss=spl; ip=i; }
		int proc( 
			HWND hwnd, 
			int msg, 
			int point, 
			int flags, 
			IPoint2 m );
		int override(int mode) { return CLICK_DOWN_POINT; }
	};

class SSBooleanCMode : public CommandMode {
	private:
		ChangeFGObject fgProc;
		SSBooleanMouseProc eproc;
		SplineShape* ss;

	public:
		SSBooleanCMode(SplineShape* spl, IObjParam *i) :
			fgProc(spl), eproc(spl,i) {ss=spl;}

		int Class() { return MODIFY_COMMAND; }
		int ID() { return CID_BOOLEAN; }
		MouseCallBack *MouseProc(int *numPoints) { *numPoints=9999; return &eproc; }
		ChangeForegroundCallback *ChangeFGProc() { return &fgProc; }
		BOOL ChangeFG( CommandMode *oldMode ) { return oldMode->ChangeFGProc() != &fgProc; }
		void EnterMode();
		void ExitMode();
	};

CoreExport ClassDesc* GetSplineShapeDescriptor();
CoreExport int ApplyOffset(Interface *intf, INode *node, float amount);
CoreExport int MeasureOffset(Interface *intf, INode *node, Point3 *point, float *result);

#endif // __SPLSHAPE_H__
