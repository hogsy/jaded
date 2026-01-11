/**********************************************************************
 *<
	FILE: render.h

	DESCRIPTION:

	CREATED BY: Dan Silva

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/


#ifndef __RENDER__H
#define __RENDER__H

#define FIELD_EVEN 0
#define FIELD_ODD 1

// Render Types   RB: I didn't want to include render.h in MAXAPI.H...
#ifndef _REND_TYPE_DEFINED
#define _REND_TYPE_DEFINED
enum RendType { 
	RENDTYPE_NORMAL,
	RENDTYPE_REGION,
	RENDTYPE_BLOWUP,
	RENDTYPE_SELECT
	};
#endif


class DefaultLight {
	public:
	LightState ls;
	Matrix3 tm;	
	};

class ViewParams {
	public:
		Matrix3 prevAffineTM; // world space to camera space transform 2 ticks previous 
		Matrix3 affineTM;  // world space to camera space transform
		int projType;      // PROJ_PERSPECTIVE or PROJ_PARALLEL
		float hither,yon;
		// Parallel projection params
		float zoom;  // Zoom factor 
		// Perspective params
		float fov; 	// field of view
		float nearRange; // for fog effects
		float farRange;  // for fog effects
		// Generic expansion function
		virtual int Execute(int cmd, ULONG arg1=0, ULONG arg2=0, ULONG arg3=0) { return 0; } 
	};


// Common renderer parameters

class Atmospheric;

// These parameters are passed to the renderer when the renderer is opend.
class RendParams {
	public:
	RendType rendType;	 	// normal, region, blowup, selection
	// boundaries of the region for render region or render blowup (device coords).
	int regxmin,regxmax;
	int regymin,regymax;

	BOOL isNetRender;  		// is this a render on a network slave?	
	BOOL fieldRender;
	int fieldOrder;    		// 0->even, 1-> odd
	TimeValue frameDur; 	// duration of one frame
	
	BOOL colorCheck;
	int vidCorrectMethod; 	// 0->FLAG, 1->SCALE_LUMA 2->SCALE_SAT
	int ntscPAL;  			// 0 ->NTSC,  1 ->PAL
	BOOL superBlack;		// impose superBlack minimum intensity?
	int sbThresh;			// value used for superBlack
	BOOL rendHidden;		// render hidden objects?
	BOOL force2Side;		// force two-sided rendering?
	BOOL inMtlEdit;	  		// rendering in the mtl editor?
	float mtlEditTile; 		// if in mtl editor, scale tiling by this value
	BOOL mtlEditAA;   		// if in mtl editor, antialias? 
	BOOL multiThread; 		// for testing only
	BOOL useEnvironAlpha;  	// use alpha from the environment map.
	BOOL dontAntialiasBG; 	// Don't antialias against background (for video games)		
	Texmap *envMap;			// The environment map, may be NULL
	Atmospheric *atmos; 	// The atmosphere effects, may be NULL.
	TimeValue firstFrame; 	// The first frame that will be rendered
	int scanBandHeight;		// height of a scan band (default scanline renderer)
	ULONG extraFlags;		// for expansion
	RendParams() { 
		inMtlEdit=0; 
		mtlEditAA = 0; 
		envMap = NULL; 
		atmos = NULL;  
		firstFrame = 0; 
		}
	// Generic expansion function
	virtual int Execute(int cmd, ULONG arg1=0, ULONG arg2=0, ULONG arg3=0) { return 0; } 
	};						   

// These are passed to the renderer on every frame
class FrameRendParams {
	public:
	Color ambient;
	Color background;
	Color globalLightLevel;
	float frameDuration; // duration of one frame, in current frames
	float relSubFrameDuration;  // relative fraction of frameDuration used by subframe.
	FrameRendParams() { frameDuration = 1.0f; relSubFrameDuration = 1.0f; }
	// Generic expansion function
	virtual int Execute(int cmd, ULONG arg1=0, ULONG arg2=0, ULONG arg3=0) { return 0; } 
	};

// Since this dialog is modless and non-interactive, as the user changes
// parameters in the dialog, the renderer does not need to update it's
// state. When the user is through, they may choose 'OK' or 'Cancel'.
//
// If the user OKs the dialog, AcceptParams() will be called, at which time the
// renderer can read the parameter out of the UI and modify its state.
// 
// If RejectParams() is called, typically the renderer will not have to do anything
// since it has not yet modify its state, but if for some reason it has, it
// should restore its state.
class RendParamDlg {
	public:
		virtual void AcceptParams()=0;
		virtual void RejectParams() {}
		virtual void DeleteThis()=0;		
	};

// Flag bits for DoMaterialBrowseDlg()
#define BROWSE_MATSONLY		(1<<0)
#define BROWSE_MAPSONLY		(1<<1)
#define BROWSE_INCNONE		(1<<2) 	// Include 'None' as an option
#define BROWSE_INSTANCEONLY	(1<<3) 	// Only allow instances, no copy

// passed to SetPickMode. This is a callback that gets called as
// the user tries to pick objects in the scene.
class RendPickProc {
	public:
		// Called when the user picks something.
		// return TRUE to end the pick mode.
		virtual BOOL Pick(INode *node)=0;

		// Return TRUE if this is an acceptable hit, FALSE otherwise.
		virtual BOOL Filter(INode *node)=0;

		// These are called as the mode is entered and exited
		virtual void EnterMode() {}
		virtual void ExitMode() {}

		// Provides two cursor, 1 when over a pickable object and 1 when not.
		virtual HCURSOR GetDefCursor() {return NULL;}
		virtual HCURSOR GetHitCursor() {return NULL;}

		// Return TRUE to allow the user to pick more than one thing.
		// In this case the Pick method may be called more than once.
		virtual BOOL AllowMultiSelect() {return FALSE;}
	};


// This is the interface given to a renderer when it needs to display its parameters
// It is also given to atmospheric effects to display thier parameters.
class IRendParams {
	public:
		// The current position of the frame slider
		virtual TimeValue GetTime()=0;

		// Register a callback object that will get called every time the
		// user changes the frame slider.
		virtual void RegisterTimeChangeCallback(TimeChangeCallback *tc)=0;
		virtual void UnRegisterTimeChangeCallback(TimeChangeCallback *tc)=0;

		// Brings up the material browse dialog allowing the user to select a material.
		// newMat will be set to TRUE if the material is new OR cloned.
		// Cancel will be set to TRUE if the user cancels the dialog.
		// The material returned will be NULL if the user selects 'None'
		virtual MtlBase *DoMaterialBrowseDlg(HWND hParent,DWORD flags,BOOL &newMat,BOOL &cancel)=0;

		// Adds rollup pages to the render params dialog. Returns the window
		// handle of the dialog that makes up the page.
		virtual HWND AddRollupPage(HINSTANCE hInst, TCHAR *dlgTemplate, 
			DLGPROC dlgProc, TCHAR *title, LPARAM param=0,DWORD flags=0)=0;

		// Removes a rollup page and destroys it.
		virtual void DeleteRollupPage(HWND hRollup)=0;

		// When the user mouses down in dead area, the plug-in should pass
		// mouse messages to this function which will pass them on to the rollup.
		virtual void RollupMouseMessage(HWND hDlg, UINT message, 
					WPARAM wParam, LPARAM lParam)=0;

		// This will set the command mode to a standard pick mode.
		// The callback implements hit testing and a method that is
		// called when the user actually picks an item.
		virtual void SetPickMode(RendPickProc *proc)=0;
		
		// If a plug-in is finished editing its parameters it should not
		// leave the user in a pick mode. This will flush out any pick modes
		// in the command stack.
		virtual void EndPickMode()=0;
			
		// When a plugin has a Texmap, clicking on the button
		// associated with that map should cause this routine
		// to be called.
		virtual void PutMtlToMtlEditor(MtlBase *mb)=0;

		// This is for use only by the scanline renderer.
		virtual float GetMaxPixelSize() = 0;
		virtual int Execute(int cmd, ULONG arg1=0, ULONG arg2=0, ULONG arg3=0) { return 0; } 
	};


//----------------------------------------------------------------
// Render Instance flags
#define INST_HIDE	  		(1<<0) // instance is hidden
#define INST_CLIP			(1<<1) // clip instance: ray tracers should skip it 
#define INST_BLUR			(1<<2) // secondary motion blur instance 
#define INST_RCV_SHADOWS	(1<<3) // instance receives shadows
#define INST_TM_NEGPARITY	(1<<4) // mesh is inside-out: need to reverse normals on-the-fly


class RenderInstance {
	public:
		ULONG flags;
		Mtl *mtl;       		// from inode, for convenience
		float wireSize;         // Mtl wireframe size
		Mesh *mesh;				// result of GetRenderMesh call
		float vis;				// Object visibility
		int nodeID;				// unique within scene during render- corresponds to ShadeContext::NodeID()
		int objMotBlurFrame;  	// Object motion blur sub frame (= NO_MOTBLUR for non-blurred objects)
		int objBlurID;		    // Blur instances for an object share a objBlurID value.
		Matrix3 objToWorld;		// transforms object coords to world coords
		Matrix3 objToCam;		// transforms object coords to cam coords
		Matrix3 normalObjToCam; // for transforming surface normals from obj to camera space
		Matrix3 camToObj;    	// transforms camera coords to object coords
		Box3 obBox;				// Object space extents
		Point3 center;			// Bounding sphere center (camera coords)
		float radsq;			// square of bounding sphere's radius

		void SetFlag(ULONG f, BOOL b) { if (b) flags |= f; else flags &= ~f; }
		void SetFlag(ULONG f) {  flags |= f; }
		void ClearFlag(ULONG f) {  flags &= ~f; }
		BOOL TestFlag(ULONG f) { return flags&f?1:0; }

		virtual RenderInstance *Next()=0;	// next in list

		virtual Interval MeshValidity()=0;
		virtual int NumLights()=0;
		virtual LightDesc *Light(int n)=0; 
		virtual int NumShadLights()=0;  
		virtual LightDesc *ShadLight(int n)=0; 

		virtual INode *GetINode()=0;  						 // get INode for instance
		virtual Object *GetEvalObject()=0; 					 // evaluated object for instance
		virtual ULONG MtlRequirements(int mtlNum)=0;		 // node's mtl requirements
		virtual Point3 GetFaceNormal(int faceNum)=0;         // geometric normal in camera coords
		virtual Point3 GetFaceVertNormal(int faceNum, int vertNum)=0;  // camera coords
		virtual void GetFaceVertNormals(int faceNum, Point3 n[3])=0;   // camera coords
		virtual Point3 GetCamVert(int vertnum)=0; 			 // coord for vertex in camera coords		
		virtual void GetObjVerts(int fnum, Point3 obp[3])=0; // vertices of face in object coords
		virtual void GetCamVerts(int fnum, Point3 cp[3])=0; // vertices of face in camera(view) coords
		// Generic expansion function
		virtual int Execute(int cmd, ULONG arg1=0, ULONG arg2=0, ULONG arg3=0) { return 0; } 
	};

//----------------------------------------------------------------


// Values returned from Progress()
#define RENDPROG_CONTINUE	1
#define RENDPROG_ABORT		0

// Values passed to SetCurField()
#define FIELD_FIRST		0
#define FIELD_SECOND	1
#define FIELD_NONE		-1

// A callback passed in to the renderer
class RendProgressCallback {
	public:
		virtual void SetTitle(const TCHAR *title)=0;
		virtual int Progress(int done, int total)=0;
		virtual void SetCurField(int which) {}
		virtual void SetSceneStats(int nlights, int nrayTraced, int nshadowed, int nobj, int nfaces) {}
	};




// RB: my version of a renderer...
class Renderer : public ReferenceTarget {
	public:
		// Reference/Animatable methods.
		// In addition, the renderer would need to implement ClassID() and DeleteThis()
		// Since a renderer will probably not itself have references, this implementation should do
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
	         PartID& partID,  RefMessage message) {return REF_SUCCEED;}
		SClass_ID SuperClassID() {return RENDERER_CLASS_ID;}
		
		virtual int Open(
			INode *scene,     	// root node of scene to render
			INode *vnode,     	// view node (camera or light), or NULL
			ViewParams *viewPar,// view params for rendering ortho or user viewport
			RendParams &rp,  	// common renderer parameters
			HWND hwnd, 				// owner window, for messages
			DefaultLight* defaultLights=NULL, // Array of default lights if none in scene
			int numDefLights=0	// number of lights in defaultLights array
			)=0;
						
		//
		// Render a frame -- will use camera or view from open
		//
		virtual int Render(
			TimeValue t,   			// frame to render.
   			Bitmap *tobm, 			// optional target bitmap
			FrameRendParams &frp,	// Time dependent parameters
			HWND hwnd, 				// owner window
			RendProgressCallback *prog=NULL,
			ViewParams *viewPar=NULL  // override viewPar given on Open.
			)=0;
		virtual void Close(	HWND hwnd )=0;		

		// Adds rollup page(s) to renderer configure dialog
		// If prog==TRUE then the rollup page should just display the parameters
		// so the user has them for reference while rendering, they should not be editable.
		virtual RendParamDlg *CreateParamDialog(IRendParams *ir,BOOL prog=FALSE)=0;
		virtual void ResetParams()=0;

	};


class ShadowBuffer;
class ShadowQuadTree;

class RendContext {
	public:
		virtual ShadowBuffer* NewShadowBuffer() const=0;
		virtual ShadowQuadTree* NewShadowQuadTree() const=0;
		virtual int Progress(int done, int total) const { return 1; }
		virtual Color GlobalLightLevel() const = 0;
	};

struct SubRendParams {
	RendType rendType;	
	BOOL fieldRender;
	BOOL evenLines; // when field rendering
	BOOL doingMirror;
	BOOL doEnvMap;  // do environment maps?
	int devWidth, devHeight;
	float devAspect;
	int xorg, yorg;           // location on screen of upper left corner of output bitmap
	int xmin,xmax,ymin,ymax;  // area of screen being rendered
	virtual int Execute(int cmd, ULONG arg1=0, ULONG arg2=0, ULONG arg3=0) { return 0; } 
	};

// flags passed to RenderMapsContext::Render()
#define RENDMAP_SHOW_NODE  1  // *Dont* exclude this node from the render.


// A pointer to this data structure is passed to MtlBase::BuildMaps();
// when rendering reflection and refraction maps.
class RenderMapsContext { 
	public:
	    virtual INode *GetNode()=0;
		virtual int NodeRenderID()=0;
		virtual void GetCurrentViewParams(ViewParams &vp)=0;
		virtual void GetSubRendParams(SubRendParams &srp)=0;
		virtual int SubMtlIndex()=0;
		virtual void SetSubMtlIndex(int mindex)=0;
	    virtual void FindMtlPlane(float pl[4])=0;
		virtual void FindMtlScreenBox(Rect &sbox, Matrix3* viewTM=NULL, int mtlIndex=-1)=0;
		virtual Box3 CameraSpaceBoundingBox()=0;
		virtual Box3 ObjectSpaceBoundingBox()=0;
		virtual Matrix3 ObjectToWorldTM()=0;
		virtual RenderGlobalContext *GetGlobalContext() { return NULL; }
		// ClipPlanes is a pointer to an array of Point4's,  each of which
		// represents a clip plane.  nClip Planes is the number of planes (up to 6);
		// The planes are in View space.
		virtual	int Render(Bitmap *bm, ViewParams &vp, SubRendParams &srp, Point4 *clipPlanes=NULL, int nClipPlanes=0)=0; 
		virtual int Execute(int cmd, ULONG arg1=0, ULONG arg2=0, ULONG arg3=0) { return 0; } 
	};	



// flags passed into ShadowBuffer::Update() and 
// 			ShadowQuadTree::Update()  
#define SHAD_BIAS_ABSOLUTE  1
#define SHAD_PARALLEL  2

#define DONT_CLIP 1.0E38f

//------- A generic Shadow Buffer class for use by lights-------------.
class ShadowBuffer { 
	public:
	virtual int Update(
		const ObjLightDesc& light,
		const RendContext& rendCntxt,
		Matrix3& lightTM,  // lightToWorld
		int size,	// width and height of buffer in pixels
		float aspect, // non-square
		float param,  // persp:field-of-view  (radians)--	parallel : width in world coords
		float bias, 
		float sampSize,
		ULONG flags,
		float clipDist=DONT_CLIP  
		)=0;
	virtual int UpdateViewDepParams(const Matrix3& worldToCam)=0;
	virtual	float Sample(float x, float y, float z, float xslope, float yslope)=0;
	virtual BOOL QuickSample(int x, int y, float z)=0; // this just tells you if the given point is in a shadow.
	virtual float FiltSample(int x, int y, float z,int level)=0;
	virtual float LineSample(int x1, int y1, float z1,int x2, int y2, float z2) {return 1.0f;}
	virtual void DeleteThis()=0;
	virtual int Execute(int cmd, ULONG arg1=0, ULONG arg2=0, ULONG arg3=0) { return 0; } 
	};

class ShadowQuadTree { 
	public:
	virtual int Update(
		const ObjLightDesc &light,
		const RendContext& rendCntxt,
		Matrix3& lightToWorld, // light to world space
		float aspect, // non-square
		float param,  // persp:field-of-view  (radians)-- parallel : width in world coords
		float bias, 
		ULONG flags,
		float clipDist=DONT_CLIP  
		)=0;

	// update state that depends on view matrix.
	virtual int UpdateViewDepParams(const Matrix3& worldToCam)=0;
	virtual float Sample(ShadeContext& sc, 
		Point3 p,  // Point in camera space
		Color& color
		)=0;
	virtual void DeleteThis()=0;
	virtual int Execute(int cmd, ULONG arg1=0, ULONG arg2=0, ULONG arg3=0) { return 0; } 
	};


//--- Atmospheric plug-in interfaces -----------------------------------------------

// Returned by an atmospheric effect when it is asked to put up its rollup page.
class AtmosParamDlg {
	public:
		virtual Class_ID ClassID()=0;
		virtual void SetThing(ReferenceTarget *m)=0;
		virtual ReferenceTarget* GetThing()=0;
		virtual void SetTime(TimeValue t) {}		
		virtual void DeleteThis()=0;		
		virtual int Execute(int cmd, ULONG arg1=0, ULONG arg2=0, ULONG arg3=0) { return 0; } 
	};

// Atmospheric plug-in base class
class Atmospheric : public ReferenceTarget {
	public:
		TSTR name;

		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
	         PartID& partID,  RefMessage message) {return REF_SUCCEED;}
		SClass_ID SuperClassID() {return ATMOSPHERIC_CLASS_ID;}
		
		// This name will appear in the track view and the list of current atmospheric effects.
		virtual TSTR GetName() {return _T("");}		

		// Put up a modal dialog that lets the user edit the plug-ins parameters.
		virtual AtmosParamDlg *CreateParamDialog(IRendParams *ip) {return NULL;}

		// Called when the render steps to a new frame
		virtual	void Update(TimeValue t, Interval& valid) {}

		// This is the function that is called to apply the effect.
		virtual void Shade(ShadeContext& sc,const Point3& p0,const Point3& p1,Color& color, Color& trans, BOOL isBG=FALSE)=0;

		// Saves and loads name. These should be called at the start of
		// a plug-in's save and load methods.
		CoreExport IOResult Save(ISave *isave);
		CoreExport IOResult Load(ILoad *iload);

		// If an atmospheric has references to gizmos or other objects in the scene it can optionally 
		// provide access to the object list.
		virtual int NumGizmos() {return 0;}
		virtual INode *GetGizmo(int i) {return NULL;}
		virtual void DeleteGizmo(int i) {}
		virtual void AppendGizmo(INode *node) {}
	};


// Chunk IDs saved by base class
#define ATMOSHPERICBASE_CHUNK	0x39bf
#define ATMOSHPERICNAME_CHUNK	0x0100


//--------------------------------------------------------------------------
// Interface into the default scanline renderer, Class_ID(SREND_CLASS_ID,0)
//---------------------------------------------------------------------------
class IScanRenderer: public Renderer {
	public:
	virtual void SetAntialias(BOOL b) = 0;
	virtual BOOL GetAntialias() = 0;
	virtual void SetFilter(BOOL b) = 0;
	virtual BOOL GetFilter() = 0;
	virtual void SetShadows(BOOL b) = 0;
	virtual BOOL GetShadows() = 0;
	virtual void SetMapping(BOOL b) = 0;
	virtual BOOL GetMapping() = 0;
	virtual void SetForceWire(BOOL b) = 0;
	virtual BOOL GetForceWire() = 0;
	virtual	void SetAutoReflect(BOOL b)=0;
	virtual	BOOL GetAutoReflect()=0;
	virtual void SetObjMotBlur(BOOL b) = 0;
	virtual BOOL GetObjMotBlur() = 0;
	virtual void SetVelMotBlur(BOOL b) = 0;
	virtual BOOL GetVelMotBlur() = 0;
	virtual void SetPixelSize(float size) = 0;
	virtual void SetAutoReflLevels(int n) = 0;
	virtual void SetWireThickness(float t) = 0;
	virtual void SetObjBlurDuration(float dur) = 0;
	virtual void SetVelBlurDuration(float dur) = 0;
	virtual void SetNBlurFrames(int n) = 0;
	virtual void SetNBlurSamples(int n) = 0;
	virtual void SetMaxRayDepth(int n) = 0;
	};

#endif

