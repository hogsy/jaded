#include "MAD_TG.h"
#define TM_REF		0
#define PBLOCK_REF	1

#define MM_CLASS_ID Class_ID(0xff92fa7c,0x5f1bfef5);

class MM : public Modifier {	
	public:		
		BYTE sel[2];

		static IObjParam *ip;
		
		MM();
						
		// From Animatable
		void DeleteThis() { delete this; }
		void GetClassName(TSTR& s) {s = "CHTROUMPTF";}  
		virtual Class_ID ClassID() {return MM_CLASS_ID;}
		void BeginEditParams(IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip,ULONG flags,Animatable *next);		
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() {return "CHTROUMPTF";}
//		int SubNumToRefNum(int subNum);
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);

		// From modifier
		ChannelMask ChannelsUsed()  {return PART_GEOM|PART_TOPO|PART_SELECT|PART_SUBSEL_TYPE;}
		ChannelMask ChannelsChanged() {return PART_GEOM|PART_TOPO|PART_SELECT|PART_TEXMAP;}
		Class_ID InputType() {return defObjectClassID;}
		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		Interval LocalValidity(TimeValue t);

		// From BaseObject
		int Display(TimeValue t, INode* inode, ViewExp *vpt, int flagst, ModContext *mc);
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;} 
//		void ActivateSubobjSel(int level, XFormModes& modes);
				
/*		int NumRefs() {return 2;}*/
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);

		RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message);

		void DrawGizmo(float size,PolyLineProc& lp);
	};


class MirrorDeformer : public Deformer {
	public:		
		Matrix3 tm, itm;
		MirrorDeformer(int axis, float offset, Matrix3 &tm, Matrix3 &itm);			
		Point3 Map(int i, Point3 p) {return (tm*p)*itm;}
	};


//--- ClassDescriptor and class vars ---------------------------------

/*IParamMap          *MM::pmapParam   = NULL;*/
IObjParam          *MM::ip          = NULL;
/*MM          *MM::editMod     = NULL;
MoveModBoxCMode    *MM::moveMode    = NULL;
RotateModBoxCMode  *MM::rotMode 	   = NULL;
UScaleModBoxCMode  *MM::uscaleMode  = NULL;
NUScaleModBoxCMode *MM::nuscaleMode = NULL;
SquashModBoxCMode  *MM::squashMode  = NULL;*/
		

class MMClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) {return new MM;}
	const TCHAR *	ClassName() { return "CHTROUMPTF"; }
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	Class_ID		ClassID() { return MM_CLASS_ID;}
	const TCHAR* 	Category() { return "CHTROUMPTF";}
	};

static MMClassDesc mirrorDesc;
extern ClassDesc* GetMMDesc() {return &mirrorDesc;}


//--- Parameter map/block descriptors -------------------------------

#define PB_AXIS				0
#define PB_COPY				1
#define PB_OFFSET			2

// Spline methods
#define SPLINE_REVERSE 0	// Pre-r3 method
#define SPLINE_NOREVERSE 1	// r3 method

//
//
// Parameters

static int axisIDs[] = {IDS_MIRROR_X,IDS_MIRROR_Y,IDS_MIRROR_Z,IDS_MIRROR_XY,IDS_MIRROR_YZ,IDS_MIRROR_ZX};
#define BIGFLOAT	float(999999)
static ParamUIDesc descParam[] = {	
	// Axis
	ParamUIDesc(PB_AXIS,TYPE_RADIO,axisIDs,6),

	// Copy
	ParamUIDesc(PB_COPY,TYPE_SINGLECHEKBOX,IDC_MIRROR_COPY),	

	// Offset
	ParamUIDesc(
		PB_OFFSET,
		EDITTYPE_UNIVERSE,
		IDC_MIRROR_OFFSET,IDC_MIRROR_OFFSETSPIN,
		-BIGFLOAT,BIGFLOAT,
		SPIN_AUTOSCALE),
	};
#define PARAMDESC_LENGTH	3

#define NUM_OLDVERSIONS	1

#define CURRENT_VERSION	1

//--- Affect region mod methods -------------------------------

MM::MM() 
	{
	}

void MM::BeginEditParams(
		IObjParam  *ip, ULONG flags,Animatable *prev)
	{
	this->ip = ip;
	TimeValue t = ip->GetTime();
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_BEGIN_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_ON);
	SetAFlag(A_MOD_BEING_EDITED);

	}

void MM::EndEditParams(
		IObjParam *ip,ULONG flags,Animatable *next)
	{
	}

RefTargetHandle MM::Clone(RemapDir& remap)
	{
	MM *mod = new MM();
	return mod;
	}


void MM::ModifyObject(
		TimeValue t, ModContext &mc, ObjectState *os, INode *node)
	{	
	}

Interval MM::LocalValidity(TimeValue t)
	{
	return FOREVER;
	}



#define AXIS_SIZE		2.0f
#define SCREEN_SCALE	0.1f

static void SetupAxisPoints(
		Point3 &v, Point3 &vp, float size,Point3 *pts) 
	{
	pts[0] = (vp)*size;
	pts[2] = (-vp)*size;
	pts[1] = (v^vp)*size;
	pts[3] = (v^(-vp))*size;
	pts[4] = pts[0];

	pts[5] = -v*size*AXIS_SIZE;
	pts[6] = v*size*AXIS_SIZE;
	
	Point3 v2 = v*0.9f*size*AXIS_SIZE;

	pts[7] = v2+pts[0]*0.1f;
	pts[8] = pts[6];
	pts[9] = v2+pts[2]*0.1f;

	pts[10] = v2+pts[1]*0.1f;
	pts[11] = pts[6];
	pts[12] = v2+pts[3]*0.1f;
	
	pts[13] = -v2+pts[0]*0.1f;
	pts[14] = -pts[6];
	pts[15] = -v2+pts[2]*0.1f;

	pts[16] = -v2+pts[1]*0.1f;
	pts[17] = -pts[6];
	pts[18] = -v2+pts[3]*0.1f;
	}

void MM::DrawGizmo(float size,PolyLineProc& lp)
	{
	Point3 v0(0,0,0), pv0(0,0,0), v1(0,0,0), pv1(0,0,0);
	int ct=0;
	int axis;
	axis = 0;

	switch (axis) {
		case 0:
		case 1:
		case 2:
			v0[axis]        = -1.0f;
			pv0[(axis+1)%3]	= 1.0f;
			pv0[(axis+2)%3]	= 1.0f;
			ct=1;
			break;
		
		case 3:
		case 4:
		case 5:
			v0[(axis)%3]    = -1.0f;
			pv0[(axis+1)%3]	= 1.0f;
			pv0[(axis+2)%3]	= 1.0f;
			v1[(axis+1)%3]  = -1.0f;
			pv1[(axis+2)%3]	= 1.0f;
			pv1[(axis+3)%3]	= 1.0f;
			ct=2;
			break;
		}
	
	Point3 pts[20];
	SetupAxisPoints(v0, pv0, size, pts);
	lp.proc(pts,5);
	lp.proc(pts+5,2);
	lp.proc(pts+7,3);
	lp.proc(pts+10,3);
	lp.proc(pts+13,3);
	lp.proc(pts+16,3);

	if (ct==2) {
		SetupAxisPoints(v1, pv1, size, pts);
		lp.proc(pts,5);
		lp.proc(pts+5,2);
		lp.proc(pts+7,3);
		lp.proc(pts+10,3);
		lp.proc(pts+13,3);
		lp.proc(pts+16,3);
		}
	}


int MM::Display(
		TimeValue t, INode* inode, ViewExp *vpt, 
		int flagst, ModContext *mc)
	{
	GraphicsWindow *gw = vpt->getGW();
	Point3 pt[4];
	int savedLimits;

	gw->setRndLimits((savedLimits = gw->getRndLimits()) & ~GW_ILLUM);
//	gw->setTransform(tm);
	if (ip && ip->GetSubObjectLevel() == 1) {
		//gw->setColor(LINE_COLOR, (float)1.0, (float)1.0, (float)0.0);
		gw->setColor(LINE_COLOR,GetUIColor(COLOR_SEL_GIZMOS));
	} else {
		//gw->setColor(LINE_COLOR, (float).85, (float).5, (float)0.0);
		gw->setColor(LINE_COLOR,GetUIColor(COLOR_GIZMOS));
		}
	
	DrawLineProc lp(gw);
	DrawGizmo(
		100.0f,lp);
	
	gw->setRndLimits(savedLimits);
	return 0;
	}

/*void MM::ActivateSubobjSel(int level, XFormModes& modes)
	{
	switch (level) {
		case 1: // Mirror center
//			modes = XFormModes(moveMode,rotMode,nuscaleMode,uscaleMode,squashMode,NULL);
			break;		
		}
	}*/


RefTargetHandle MM::GetReference(int i)
	{
	return NULL;
	}

void MM::SetReference(int i, RefTargetHandle rtarg)
	{
	}

/*TSTR MM::SubAnimName(int i)
	{
	switch (i) {
		case TM_REF: return "CHTROUMPTF"; break;		
		default: return _T(""); break;
		}
	}*/

RefResult MM::NotifyRefChanged(
		Interval changeInt,RefTargetHandle hTarget, 
		PartID& partID, RefMessage message)
	{
	return REF_SUCCEED;
	}

IOResult MM::Save(ISave *isave) {	
	return IO_OK;
	}

IOResult MM::Load(ILoad *iload)
	{
	return IO_OK;
	}

