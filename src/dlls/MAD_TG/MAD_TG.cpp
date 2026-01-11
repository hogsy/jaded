/**********************************************************************
 *<
	FILE: MAD_TG.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/

#include "MAD_TG.h"
#include "modstack.h"


#define PB_SPINNER   0
#define PB_SPINNER2  1
#define PB_CHK1  2
#define PB_CHK2  3
#define PB_CHK3  4
#define PB_CHK4  5
#define PB_CHK5  6
#define PB_CHK6  7
#define PB_CHK7  8
#define PB_CHK8  9
#define PB_CHK9  10
#define PB_CHK10  11
#define PB_CHK11  12

static int typeChk1[] = {IDC_CHECK1};
static int typeChk2[] = {IDC_CHECK2};
/* TODO: Add ParamUIDesc's for the various parameters */
static ParamUIDesc  descParam[] =
{
    /* Spinner */
    ParamUIDesc(PB_SPINNER , EDITTYPE_FLOAT, IDC_EDIT3, IDC_SPIN3, 0.0f, 100.0f, 0.0005f),
    ParamUIDesc(PB_SPINNER2, EDITTYPE_INT  , IDC_EDIT2, IDC_SPIN2, 0, 100, 1),
};

/* TODO: Parameter descriptor length */
#define PARAMDESC_LENGTH    2

/* TODO: Add ParamBlockDescID's for the various parameters */
static ParamBlockDescID 
descVer1[] = { 
	{ TYPE_FLOAT, NULL, TRUE, 0 }, 
	{ TYPE_INT	, NULL, TRUE, 1 }, 
	{ TYPE_INT,   NULL, TRUE, 2 },
	{ TYPE_INT,   NULL, TRUE, 3 },
	{ TYPE_INT,   NULL, TRUE, 4 },
	{ TYPE_INT,   NULL, TRUE, 5 },
	{ TYPE_INT,   NULL, TRUE, 6 },
	{ TYPE_INT,   NULL, TRUE, 7 },
	{ TYPE_INT,   NULL, TRUE, 8 },
	{ TYPE_INT,   NULL, TRUE, 9 },
	{ TYPE_INT,   NULL, TRUE, 10 },
	{ TYPE_INT,   NULL, TRUE, 11 },
	};

#define CURRENT_DESCRIPTOR  descVer1

#define PBLOCK_LENGTH       12

#define CURRENT_VERSION     1


//--- ClassDescriptor and class vars ---------------------------------

IObjParam* MAD_TG::ip = NULL;
IParamMap *MAD_TG::  pmapParam = NULL;
HWND MAD_TG::hParams  = NULL;

class MAD_TGDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) {return new MAD_TG;}
	const TCHAR *	ClassName() {return "MAD_TG";}
	SClass_ID		SuperClassID() {return OSM_CLASS_ID;}
	Class_ID		ClassID() {return MAD_TG_CLASS_ID; }
	const TCHAR* 	Category() {return "MAD";}
	};

static MAD_TGDesc MAD_TGDesccl;
extern ClassDesc* GetMAD_TGDesc() {return &MAD_TGDesccl;}


//--- CustMod dlg proc ------------------------------

class PickControlNode :
		public PickModeCallback,
		public PickNodeCallback {
	public:				
		MAD_TG *mod;
		PickControlNode() {mod=NULL;}
		BOOL HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags);		
		BOOL Pick(IObjParam *ip,ViewExp *vpt);		
		void EnterMode(IObjParam *ip);
		void ExitMode(IObjParam *ip);		
		BOOL Filter(INode *node);
		PickNodeCallback *GetFilter() {return this;}
		BOOL RightClick(IObjParam *ip,ViewExp *vpt) {return TRUE;}
	};
static PickControlNode thePickMode;

BOOL PickControlNode::Filter(INode *node)
	{
	node->BeginDependencyTest();
	mod->NotifyDependents(FOREVER,0,REFMSG_TEST_DEPENDENCY);
	if (node->EndDependencyTest()) {
		return FALSE;
	} else {
		return TRUE;
		}
	}

BOOL PickControlNode::HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags)
	{	
	if (ip->PickNode(hWnd,m,this)) 
	{
		return TRUE;
	} else 
	{
		return FALSE;
	}
	}


BOOL PickControlNode::Pick(IObjParam *ip,ViewExp *vpt)
	{
	Object *Obj;
	IDerivedObject *DrvdObj;
	CStr CLNam;
	unsigned long OK;
	INode *node = vpt->GetClosestHit();
	if (node) {
		OK = 0;
		Obj = node -> GetObjectRef();
		if (Obj)
		{
			if (Obj -> ClassID() == derivObjClassID)
			{
				DrvdObj = (IDerivedObject *)Obj;
				if (DrvdObj -> NumModifiers())
				{
					if (DrvdObj -> GetModifier(0) -> ClassID() == MAD_TGM_CLASS_ID)
						OK = 1;
				}
			}
		}
		if (!OK)
		{
			MessageBox
			(
				ip->GetMAXHWnd(),
				_T("You must pick an object with \n MAD TGM modifier on the top \n of the modifier stack"),
				_T(""),
				MB_OK | MB_ICONERROR | MB_TASKMODAL
			);
			return FALSE;
		}
		mod->clTheGeomText = (MAD_TGM *)DrvdObj -> GetModifier(0);
		mod->node = node;
		mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
		mod->ip->RedrawViews(mod->ip->GetTime());
		SetWindowText(GetDlgItem(mod->hParams,IDC_CLUST_NODENAME),node->GetName());
		}
	return TRUE;
	}

void PickControlNode::EnterMode(IObjParam *ip)
	{
	ICustButton *iBut = GetICustButton(GetDlgItem(mod->hParams,IDC_CLUST_PICKNODE));
	if (iBut) iBut->SetCheck(TRUE);
	ReleaseICustButton(iBut);
	}

void PickControlNode::ExitMode(IObjParam *ip)
	{
	ICustButton *iBut = GetICustButton(GetDlgItem(mod->hParams,IDC_CLUST_PICKNODE));
	if (iBut) iBut->SetCheck(FALSE);
	ReleaseICustButton(iBut);
	}

// Face-type dlg user proc
class CmpfrDlgProc : public ParamMapUserDlgProc {
public:
	MAD_TG *mod;
	CmpfrDlgProc () { mod = NULL; }
	BOOL DlgProc (TimeValue t, IParamMap *map, HWND hWnd,
		UINT msg, WPARAM wParam, LPARAM lParam);
	void DeleteThis () { }
};


BOOL CmpfrDlgProc::DlgProc (TimeValue t, IParamMap *map, HWND hWnd,
							  UINT msg,WPARAM wParam,LPARAM lParam) 
	{
	if (mod == NULL) return FALSE;
	mod -> hParams = hWnd;
	
	switch (msg) {
		case WM_INITDIALOG: {
			ICustButton *iBut = GetICustButton(GetDlgItem(hWnd,IDC_CLUST_PICKNODE));
			iBut->SetType(CBT_CHECK);
			iBut->SetHighlightColor(GREEN_WASH);
			ReleaseICustButton(iBut);
			if (mod->node) {
				SetWindowText(GetDlgItem(hWnd,IDC_CLUST_NODENAME),
					mod->node->GetName());
			} else {
					SetWindowText(GetDlgItem(hWnd,IDC_CLUST_NODENAME),
					"No object");
				}
			break;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_CLUST_PICKNODE:
					thePickMode.mod  = mod;					
					mod->ip->SetPickMode(&thePickMode);
					break;
				}
			break;
		}
	return FALSE;
	}

//--- ClustMod methods -------------------------------

MAD_TG::MAD_TG()
	{
    pblock = CreateParameterBlock(CURRENT_DESCRIPTOR, PBLOCK_LENGTH, CURRENT_VERSION);
	pblock->SetValue(PB_SPINNER  , 0 , 0.001f);
	pblock->SetValue(PB_SPINNER2 , 0 , 0);
	pblock->SetValue(PB_CHK1 , 0 , 1);
	pblock->SetValue(PB_CHK2 , 0 , 0);
	pblock->SetValue(PB_CHK3 , 0 , 0);
	pblock->SetValue(PB_CHK4 , 0 , 0);
	pblock->SetValue(PB_CHK5 , 0 , 1);
	pblock->SetValue(PB_CHK6 , 0 , 0);
	pblock->SetValue(PB_CHK7 , 0 , 0);
	pblock->SetValue(PB_CHK8 , 0 , 0);
	pblock->SetValue(PB_CHK9 , 0 , 0);
	pblock->SetValue(PB_CHK10, 0 , 0);
    assert(pblock);
    MakeRefByID(FOREVER, 0, pblock);
	node  = NULL;
	CurrenID = 1;
	clTheGeomText = NULL;
	}

MAD_TG::~MAD_TG()
	{
	}

#define CLUSTNODE_TM_CHUNK		0x0100
#define CLUSTNODE_INVTM_CHUNK	0x0110

IOResult MAD_TG::Load(ILoad *iload)
	{
	Modifier::Load(iload);
	IOResult res = IO_OK;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch (iload->CurChunkID()) {
			case CLUSTNODE_TM_CHUNK:
//				tm.Load(iload);
				break;

			case CLUSTNODE_INVTM_CHUNK:
//				invtm.Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK)  return res;
		}
	return IO_OK;
	}

IOResult MAD_TG::Save(ISave *isave)
	{
	Modifier::Save(isave);
	isave->BeginChunk(CLUSTNODE_TM_CHUNK);
//	tm.Save(isave);
	isave->EndChunk();

	isave->BeginChunk(CLUSTNODE_INVTM_CHUNK);
//	invtm.Save(isave);
	isave->EndChunk();
	
	return IO_OK;
	}
static CmpfrDlgProc CmpfrDlgProcVAR;


void MAD_TG::BeginEditParams(
		IObjParam *ip, ULONG flags,Animatable *prev)
	{
	this->ip = ip;
    pmapParam = CreateCPParamMap
        (
            descParam,
            PARAMDESC_LENGTH,
            pblock,
            ip,
            hInstance,
            MAKEINTRESOURCE(IDD_PANEL),
            GetString(IDS_PARAMS),
            0
        );
	CmpfrDlgProcVAR.mod = this;
	pmapParam -> SetUserDlgProc  (&CmpfrDlgProcVAR);
	}

void MAD_TG::EndEditParams(
		IObjParam *ip,ULONG flags,Animatable *next)
	{
    DestroyCPParamMap(pmapParam);
    this->ip = NULL;
	}

void ApplyMeshOnAFace(Mesh *MFace , unsigned long FaceNum , MAD_TGM *clGeoText , Mesh *FacetizedMesh , float Height , Point3 *Normales)
{
	Point3 VFBase1 , VFBase2;
	Point3 VF0 , VF1 , VFN0 , VFN1;
	unsigned long V0,V1,V2;
	unsigned long Counter;
	/* Compute Facemap A or B */
	if ((MFace->faces[FaceNum].flags & 7) == 6)
	{
		V0 = MFace->faces[FaceNum].v[2];
		V2 = MFace->faces[FaceNum].v[0];
		V1 = MFace->faces[FaceNum].v[1];
		if (MFace->faces[FaceNum].v[0] < MFace->faces[FaceNum].v[1]) 
			*FacetizedMesh = *clGeoText ->GetTextureMesh(0);
		else
			*FacetizedMesh = *clGeoText ->GetTextureMesh(1);
	} else
	if ((MFace->faces[FaceNum].flags & 7) == 5)
	{
		V0 = MFace->faces[FaceNum].v[0];
		V2 = MFace->faces[FaceNum].v[1];
		V1 = MFace->faces[FaceNum].v[2];
		if (MFace->faces[FaceNum].v[1] < MFace->faces[FaceNum].v[2]) 
			*FacetizedMesh = *clGeoText ->GetTextureMesh(0);
		else
			*FacetizedMesh = *clGeoText ->GetTextureMesh(1);
	} else
	if ((MFace->faces[FaceNum].flags & 7) == 3)
	{
		V0 = MFace->faces[FaceNum].v[1];
		V2 = MFace->faces[FaceNum].v[2];
		V1 = MFace->faces[FaceNum].v[0];
		if (MFace->faces[FaceNum].v[2] < MFace->faces[FaceNum].v[0]) 
			*FacetizedMesh = *clGeoText ->GetTextureMesh(0);
		else
			*FacetizedMesh = *clGeoText ->GetTextureMesh(1);
	} else 
	{
		V0 = MFace->faces[FaceNum].v[0];
		V2 = MFace->faces[FaceNum].v[1];
		V1 = MFace->faces[FaceNum].v[2];
		*FacetizedMesh = *clGeoText ->GetTextureMesh(0);
	}
	VFBase1 = MFace->verts[V0];
	VF0		= MFace->verts[V1] - VFBase1;
	VF1		= MFace->verts[V2] - VFBase1;

	VFBase2 = VFBase1 - Height * Normales[V0];
	VFN0	= MFace->verts[V1] - Height * Normales[V1] - VFBase2;
	VFN1	= MFace->verts[V2] - Height * Normales[V2] - VFBase2;
/* Face normales */
/*	VFBase2 = VFBase1 + ZZoom * MFace->getFaceNormal(FaceNum);
	VFN0	= MFace->verts[MFace->faces[FaceNum].v[0]] + ZZoom * MFace->getFaceNormal(FaceNum) - VFBase2;
	VFN1	= MFace->verts[MFace->faces[FaceNum].v[2]] + ZZoom * MFace->getFaceNormal(FaceNum)  - VFBase2;
/*/
	VF0		*= 1.0f / BoxSizeXY;
	VF1		*= 1.0f / BoxSizeXY;
	VFN0	*= 1.0f / BoxSizeXY;
	VFN1	*= 1.0f / BoxSizeXY;

	for (Counter = 0; Counter < (unsigned long)FacetizedMesh->numVerts; Counter ++)
	{

		FacetizedMesh->verts[Counter] = 
			((VFBase1 + VF0 *  FacetizedMesh->verts[Counter] . x + VF1 *  FacetizedMesh->verts[Counter] . y) * (1.0f - FacetizedMesh->verts[Counter] . z)) + 
			((VFBase2 + VFN0 * FacetizedMesh->verts[Counter] . x + VFN1 * FacetizedMesh->verts[Counter] . y) * (FacetizedMesh->verts[Counter] . z));
	}

}

/* This is not a boolean operation */
void MAD_TG::ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   C2;
	int	MatID;
	float			Height;
    Mesh            *M;
    Interval        iv;
	Mesh		*M0 ;

	
	TriObject   *O[2];
	Point3		*TheNormales;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if (!clTheGeomText) return; /* No geometric texture, -> exit */

	pblock->GetValue(PB_SPINNER2 , t,	MatID , iv);
	pblock->GetValue(PB_SPINNER  , t,	Height , iv);
	

	O[0] = CreateNewTriObject();
	O[1] = CreateNewTriObject();
#ifdef MAX3
    M0 = &O[0]->GetMesh();
    M1 = &O[1]->GetMesh();
#else
    M0 = &O[0]->Mesh();
    M1 = &O[1]->Mesh();
#endif
    iv = os->obj->ChannelValidity(t, GEOM_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, TOPO_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, SELECT_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, SUBSEL_TYPE_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, TEXMAP_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, VERT_COLOR_CHAN_NUM);

    M = &((TriObject *) os->obj)->mesh;
	M -> buildNormals();

	TheNormales = (Point3 *)malloc (sizeof(Point3) * M -> numVerts );
	for(C2 = 0; C2 < (unsigned long)M->numVerts; C2++)
	{
		TheNormales[C2] = Point3(0.f,0.f,0.f);
	}
	for(C2 = 0; C2 < (unsigned long)M->numFaces; C2++)
	{
		Point3 V1,V2,V3;
		V1 = M->verts[M->faces[C2].v[1]] - M->verts[M->faces[C2].v[0]];
		V2 = M->verts[M->faces[C2].v[2]] - M->verts[M->faces[C2].v[0]];
		V3 = V1 ^ V2;
		TheNormales[M->faces[C2].v[0]] += V3;
		TheNormales[M->faces[C2].v[1]] += V3;
		TheNormales[M->faces[C2].v[2]] += V3;
	}
	for(C2 = 0; C2 < (unsigned long)M->numVerts; C2++)
	{
		Normalize(TheNormales[C2]);
	}


	for(C2 = 0; C2 < (unsigned long)M->numFaces; C2++)
	{
		if (M->faces[C2].getMatID() == MatID)
		{
			M->faces[C2].flags |= FACE_HIDDEN;
			ApplyMeshOnAFace(M , C2 , clTheGeomText , M0 , Height , TheNormales);
			AddMesh(M1,M0);
		}
	}
	AddMesh(M,M1);
    os->obj->UpdateValidity(GEOM_CHAN_NUM, FOREVER);
    os->obj->UpdateValidity(GEOM_CHAN_NUM, iv);
    os->obj->UpdateValidity(TOPO_CHAN_NUM, iv);
    os->obj->UpdateValidity(SELECT_CHAN_NUM, iv);
    os->obj->UpdateValidity(SUBSEL_TYPE_CHAN_NUM, iv);
    os->obj->UpdateValidity(TEXMAP_CHAN_NUM, iv);
    os->obj->UpdateValidity(VERT_COLOR_CHAN_NUM, iv);
}


Interval MAD_TG::LocalValidity(TimeValue t)
	{
    if(TestAFlag(A_MOD_BEING_EDITED)) return NEVER;

    /* TODO: Return the validity interval of the modifier */
    return NEVER;
	}

RefTargetHandle MAD_TG::Clone(RemapDir& remap)
	{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_TG   *newmod;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    newmod = new MAD_TG();
    /* TODO: Add the cloning code here */
    newmod->ReplaceReference(0, pblock->Clone(remap));
    return(newmod);
	}

RefResult MAD_TG::NotifyRefChanged(
		Interval changeInt, 
		RefTargetHandle hTarget, 
		PartID& partID, 
		RefMessage message)
	{
	return REF_SUCCEED;
	}
/*
 ===================================================================================================
    From Object
 ===================================================================================================
 */
BOOL MAD_TG::HasUVW(void)
{
    /* TODO: Return whether the object has UVW coordinates or not */
    return TRUE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_TG::SetGenUVW(BOOL sw)
{
    if(sw == HasUVW()) return;
    /* TODO: Set the plugin internal value to sw */
}
