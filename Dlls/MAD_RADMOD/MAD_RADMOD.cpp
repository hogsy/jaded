/**********************************************************************
 *<
	FILE: MAD_RadMod.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/

#include "Max.h"
#include "resource.h"
#if defined(MAX3) || defined(MAX8)
#include "iparamm2.h"
#include "istdplug.h"
#include "meshadj.h"
#endif


#include "MAD_RADMOD.h"
extern HINSTANCE hInstance;


#define PBLOCK_REF	0


HWND MAD_RadMod::hParams1  = NULL;
#if defined(MAX3) || defined(MAX8)
class MAD_RadModClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) {return new MAD_RadMod();}
	const TCHAR *	ClassName() {return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() {return OSM_CLASS_ID;}
	Class_ID		ClassID() {return MAD_RadMod_CLASS_ID;}
	const TCHAR* 	Category() {return _T("MAD KEEPER");}
	const TCHAR*	InternalName() { return _T("MAD_RadMod"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle
};
#else
class MAD_RadModClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) {return new MAD_RadMod();}
	const TCHAR *	ClassName() {return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() {return OSM_CLASS_ID;}
	Class_ID		ClassID() {return MAD_RadMod_CLASS_ID;}
	const TCHAR* 	Category() {return _T("MAD KEEPER");}
	const TCHAR*	InternalName() { return _T("MAD_RadMod"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle
};
#endif

static MAD_RadModClassDesc MAD_RadModDesc;
#if defined(MAX3) || defined(MAX8)
ClassDesc2* GetMAD_RadModDesc() {return &MAD_RadModDesc;}
#else 
ClassDesc* GetMAD_RadModDesc() {return &MAD_RadModDesc;}
#endif

IObjParam *MAD_RadMod::ip			= NULL;

//--- MAD_RadMod -------------------------------------------------------
MAD_RadMod::MAD_RadMod()
{
	RadiosityObject = NULL;
	UseRo = 1;
	MIXING = 0.0f;
	RLI = NULL;
}

MAD_RadMod::~MAD_RadMod()
{
}

Interval MAD_RadMod::LocalValidity(TimeValue t)
{
	//TODO: Return the validity interval of the modifier
	return NEVER;
}

RefTargetHandle MAD_RadMod::Clone(RemapDir& remap)
{
	MAD_RadMod* newmod = new MAD_RadMod();	
	return(newmod);
}

void MAD_RadMod::ModifyObject(TimeValue t, ModContext &mc, ObjectState * os, INode *node) 
{
    Interval        iv;
	Mesh			*M;
	int Counter;
	if (RadiosityObject == NULL) UseRo = 0;
	M = &((TriObject *) os->obj)->mesh;
	InstanciedObjectNumberOfPoints	=	M->numVerts;
	RadiosityObjectNumberOfPoints	=	0;
	if (RadiosityObject)
	{
		RadiosityObjectNumberOfPoints	=	RadiosityObject->numVerts;
	} 
    iv = os->obj->ChannelValidity(t, GEOM_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, TOPO_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, SELECT_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, SUBSEL_TYPE_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, TEXMAP_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, VERT_COLOR_CHAN_NUM);

	if (UseRo) 
	{
		*M = *RadiosityObject;
		for (Counter=0;Counter<M->numVerts;Counter++)
		{
			M -> vertCol[Counter] = RadiosityObject-> vertCol[Counter] * (1.0f - MIXING) + P3_RadiosityObjectSecondRliField[Counter] * MIXING;
		}
	}
	else
	if (RLI)
	{
		M -> setNumVertCol	(M->numVerts);
		M -> setNumVCFaces	(M->numFaces);
		for (Counter=0;Counter<M->numVerts;Counter++)
		{
			M-> vertCol[Counter] = RLI[Counter];
		}
		for (Counter=0;Counter<M->numFaces;Counter++)
		{
			M-> vcFace[Counter].t[0] = M-> faces[Counter].v[0];
			M-> vcFace[Counter].t[1] = M-> faces[Counter].v[1];
			M-> vcFace[Counter].t[2] = M-> faces[Counter].v[2];
		}
	}

    os->obj->UpdateValidity(GEOM_CHAN_NUM, iv);
    os->obj->UpdateValidity(TOPO_CHAN_NUM, iv);
    os->obj->UpdateValidity(SELECT_CHAN_NUM, iv);
    os->obj->UpdateValidity(SUBSEL_TYPE_CHAN_NUM, iv);
    os->obj->UpdateValidity(TEXMAP_CHAN_NUM, iv);
    os->obj->UpdateValidity(VERT_COLOR_CHAN_NUM, iv);

}

static BOOL CALLBACK ClustParamDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	MAD_RadMod *mod = (MAD_RadMod*)GetWindowLong(hWnd,GWL_USERDATA);
	
	switch (msg) {
		case WM_INITDIALOG: {
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			mod = (MAD_RadMod*)lParam;
			ICustButton *iBut = GetICustButton(GetDlgItem(hWnd,IDC_APPLYTOSELECTEDFACES));
			iBut->SetType(CBT_CHECK);
			iBut->SetHighlightColor(BLUE_WASH);
			iBut->SetCheck( mod -> UseRo );
			ReleaseICustButton(iBut);
			ISpinnerControl *ispin = SetupFloatSpinner(hWnd, IDC_SPIN, IDC_EDIT, 0.0f, 1.0f, mod->MIXING, 0.0025f);
			ReleaseISpinner(ispin);
			break;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_APPLYTOSELECTEDFACES:
					mod -> UseRo ^= 1;
					mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
					mod->ip->RedrawViews(mod->ip->GetTime());
					break;
			}
			break;
	case CC_SPINNER_CHANGE:
		switch (LOWORD(wParam)) { // Switch on ID
			case IDC_SPIN: // A specific spinner ID.
				mod->MIXING = ((ISpinnerControl *)lParam)->GetFVal();
				mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
				mod->ip->RedrawViews(mod->ip->GetTime());
				break;
			};
		break;

		
		default:
			return FALSE;
		}
	return TRUE;
	}

void MAD_RadMod::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
{
	this->ip = ip;
	hParams1  = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		ClustParamDlgProc,
		"MAD RadMOD",
		(LPARAM)this);
	
}

void MAD_RadMod::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next)
{
	ip->DeleteRollupPage(hParams1);
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime());
	this->ip = NULL;
}


//From ReferenceMaker 
RefResult MAD_RadMod::NotifyRefChanged(
		Interval changeInt, RefTargetHandle hTarget,
		PartID& partID,  RefMessage message) 
{
	//TODO: Add code to handle the various reference changed messages
	return REF_SUCCEED;
}

//From Object
BOOL MAD_RadMod::HasUVW() 
{ 
	//TODO: Return whether the object has UVW coordinates or not
	return TRUE; 
}
void MAD_RadMod::SetGenUVW(BOOL sw) 
{  
	//TODO: Set the plugin internal value to sw				
}

#define PROUT_CHUNK 0xC0DE

IOResult MAD_RadMod::Load(ILoad *iload)
{
	ULONG nb;

	if (IO_OK==iload->OpenChunk()) 
	{
		iload->Read(&InstanciedObjectNumberOfPoints, sizeof(unsigned long), &nb);
		iload->Read(&RadiosityObjectNumberOfPoints , sizeof(unsigned long), &nb);
		iload->Read(&MIXING , 4 , &nb);
		RLI = (Point3 *)malloc (sizeof(Point3) * InstanciedObjectNumberOfPoints);
		OriginalsRLI = (unsigned long* )malloc (sizeof(unsigned long) * InstanciedObjectNumberOfPoints);
		iload->Read(RLI, sizeof(Point3) * InstanciedObjectNumberOfPoints, &nb);
		iload->Read(OriginalsRLI , sizeof(unsigned long) * InstanciedObjectNumberOfPoints, &nb);
		P3_RadiosityObjectSecondRliField = (Point3 *)malloc (sizeof(Point3) * RadiosityObjectNumberOfPoints);
		UL_RadiosityObjectSecondRliField = (unsigned long* )malloc (sizeof(unsigned long) * RadiosityObjectNumberOfPoints);
		if (RadiosityObjectNumberOfPoints)
		{
			iload->Read(P3_RadiosityObjectSecondRliField, sizeof(Point3) * RadiosityObjectNumberOfPoints, &nb);
			iload->Read(UL_RadiosityObjectSecondRliField, sizeof(unsigned long) * RadiosityObjectNumberOfPoints, &nb);
		}
	}
	iload->CloseChunk();
	if (RadiosityObjectNumberOfPoints)
	{
		RadiosityObject = new Mesh;
		RadiosityObject->Load(iload);
	}

	return IO_OK;
}

IOResult MAD_RadMod::Save(ISave *isave)
{
	ULONG nb;

	isave->BeginChunk(PROUT_CHUNK);
	isave->Write(&InstanciedObjectNumberOfPoints, sizeof(unsigned long), &nb);
	isave->Write(&RadiosityObjectNumberOfPoints, sizeof(unsigned long), &nb);
	isave->Write(&MIXING , 4 , &nb);
	isave->Write(RLI, sizeof(Point3) * InstanciedObjectNumberOfPoints, &nb);
	isave->Write(OriginalsRLI , sizeof(unsigned long) * InstanciedObjectNumberOfPoints, &nb);
	if (RadiosityObjectNumberOfPoints)
	{
		isave->Write(P3_RadiosityObjectSecondRliField, sizeof(Point3) * RadiosityObjectNumberOfPoints, &nb);
		isave->Write(UL_RadiosityObjectSecondRliField, sizeof(unsigned long) * RadiosityObjectNumberOfPoints, &nb);
	}
	isave->EndChunk();
	if (RadiosityObjectNumberOfPoints)
	 	RadiosityObject->Save(isave);

	return IO_OK;
}

