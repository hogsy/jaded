/**********************************************************************
 *<
	FILE: MAD_FaceMAP.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/
/*$off*/
/*
UV Numerotation used in Facemap:
					0 - 1
					|	|
					2 - 3
*/
/*$on*/


#include "MAD_FaceMAP.h"

#define MAD_FACEMAP_CLASS_ID	Class_ID(0x34709e94, 0x477f7e73)

#define PBLOCK_REF	0




class MAD_FaceMAP : public Modifier {
	public:
		static SelectModBoxCMode *selectMode;	// used for multi selection
		// Parameter block
		static IObjParam *ip;			//Access to the interface
		unsigned long	ByPass;
		unsigned long	NotUseSel;
		unsigned long	IsEdited;
		unsigned long	AtLeastOneIsSelected;
		static HWND hParams1;
		unsigned long	*AllFlags;
		unsigned long	AllFlagsNumber;

		Mesh *M;
		
		// From Animatable
		TCHAR *GetObjectName() { return GetString(IDS_CLASS_NAME); }

		//From Modifier
		//TODO: Add the channels that the modifier needs to perform its modification
		ChannelMask ChannelsUsed()  { return PART_GEOM | PART_TOPO | PART_SELECT | PART_TEXMAP | PART_VERTCOLOR; }
		//TODO: Add the channels that the modifier actually modifies
		ChannelMask ChannelsChanged() { return PART_GEOM | PART_TOPO | PART_SELECT | PART_TEXMAP | PART_VERTCOLOR; }
		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		//TODO: Return the ClassID of the object that the modifier can modify
		Class_ID InputType() {return defObjectClassID;}
		Interval LocalValidity(TimeValue t);

		// From BaseObject
		//TODO: Return true if the modifier changes topology
		BOOL ChangeTopology() {return FALSE;}

		
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;}
		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);

		Interval GetValidity(TimeValue t);

		// Automatic texture support
		BOOL HasUVW();
		void SetGenUVW(BOOL sw);
		
		// Loading/Saving
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);

		//From Animatable
		Class_ID ClassID() {return MAD_FACEMAP_CLASS_ID;}		
		SClass_ID SuperClassID() { return OSM_CLASS_ID; }
		void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}
		
		RefTargetHandle Clone( RemapDir &remap );
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
			PartID& partID,  RefMessage message);

		int NumSubs() { return 1; }
		TSTR SubAnimName(int i) { return GetString(IDS_PARAMS); }				
//		Animatable* SubAnim(int i) { return pblock; }
		int NumRefs() { return 1; }
//		RefTargetHandle GetReference(int i) { return pblock; }
//		void SetReference(int i, RefTargetHandle rtarg) { pblock=(IParamBlock2*)rtarg; }
	int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc);
	void ActivateSubobjSel(int level, XFormModes& modes );
//	void ActivateSubobjSel(int level, XFormModes& modes );
	void SelectSubComponent(HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert);
/*	void ClearSelection(int selLevel);
	void SelectAll(BOOL selLevel);
	void InvertSelection(BOOL selLevel);*/

		int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
/*		IParamBlock2* GetParamBlock(int i) { return pblock; } // return i'th ParamBlock
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock*/
		void DeleteThis() { delete this; }		
		//Constructor/Destructor
		MAD_FaceMAP();
		~MAD_FaceMAP();		
		void SetFaceUVW(unsigned long *Flag, unsigned long I1,unsigned long I2,unsigned long I3);
		unsigned long GetFaceUVW(unsigned long Flag, unsigned long I /* 0, 1 , 2 */);
		unsigned long ComputeOriginalFM(Face *Fce);
};
class MAD_FaceMAPLocalData : public LocalModData
{	
public:
	Mesh *m_mesh;						// mesh
};	

SelectModBoxCMode*	MAD_FaceMAP::selectMode = NULL;

#define IsSelcted (1<<13)
void MAD_FaceMAP::SetFaceUVW(unsigned long *Flag , unsigned long I1,unsigned long I2,unsigned long I3)
{
	*Flag &= ~(0x3F << 7);
	*Flag |= ((I1 & 3) << 7) | ((I2 & 3) << 9) | ((I3 & 3) << 11);
}

#define AllFlagFm ((3<<7) | (3<<9) | (3<<11))
unsigned long MAD_FaceMAP::GetFaceUVW(unsigned long Flag , unsigned long I1 /* = 0,1 or 2 */)
{
	switch (I1)
	{
		case 0:return ((Flag >> 7 ) & 3);break;
		case 1:return ((Flag >> 9 ) & 3);break;
		case 2:return ((Flag >> 11) & 3);break;
	}
	return ( ( Flag >> (3 << ( ( I1 << 1 ) + 7) )) & 3 );
}
unsigned long MAD_FaceMAP::ComputeOriginalFM(Face *Fce)
{
	unsigned long V0,V1,V2;
	if (Fce -> flags & AllFlagFm) return Fce -> flags;
	/* Compute Facemap A or B */
	if ((Fce->flags & 7) == 6)
	{
		V0 = Fce->v[2];
		V1 = Fce->v[0];
		V2 = Fce->v[1];
		if (V1 < V2) 
			SetFaceUVW(&Fce->flags , 0,3,1);
		else
			SetFaceUVW(&Fce->flags , 3,0,2);
	} else
	if ((Fce->flags & 7) == 5)
	{
		V0 = Fce->v[0];
		V1 = Fce->v[1];
		V2 = Fce->v[2];
		if (V1 < V2) 
			SetFaceUVW(&Fce->flags , 1,0,3);
		else
			SetFaceUVW(&Fce->flags , 2,3,0);
	} else
	if ((Fce->flags & 7) == 3)
	{
		V0 = Fce->v[1];
		V1 = Fce->v[2];
		V2 = Fce->v[0];
		if (V1 < V2) 
			SetFaceUVW(&Fce->flags , 3,1,0);
		else
			SetFaceUVW(&Fce->flags , 0,2,3);
	} else 
	{
		V0 = Fce->v[0];
		V1 = Fce->v[1];
		V2 = Fce->v[2];
		if (V1 < V2) 
			SetFaceUVW(&Fce->flags , 2,3,1);
		else
			SetFaceUVW(&Fce->flags , 1,0,2);
	}

	return Fce->flags;
}

HWND MAD_FaceMAP::hParams1  = NULL;
#ifdef MAX3
class MAD_FaceMAPClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) {return new MAD_FaceMAP();}
	const TCHAR *	ClassName() {return GetString(IDS_CLASS_NAME);}
	SClass_ID		SuperClassID() {return OSM_CLASS_ID;}
	Class_ID		ClassID() {return MAD_FACEMAP_CLASS_ID;}
	const TCHAR* 	Category() {return GetString(IDS_CATEGORY);}
	const TCHAR*	InternalName() { return _T("MAD_FaceMAP"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle
};
#else
class MAD_FaceMAPClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) {return new MAD_FaceMAP();}
	const TCHAR *	ClassName() {return GetString(IDS_CLASS_NAME);}
	SClass_ID		SuperClassID() {return OSM_CLASS_ID;}
	Class_ID		ClassID() {return MAD_FACEMAP_CLASS_ID;}
	const TCHAR* 	Category() {return GetString(IDS_CATEGORY);}
	const TCHAR*	InternalName() { return _T("MAD_FaceMAP"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle
};
#endif

static MAD_FaceMAPClassDesc MAD_FaceMAPDesc;
#ifdef MAX3
ClassDesc2* GetMAD_FaceMAPDesc() {return &MAD_FaceMAPDesc;}
#else 
ClassDesc* GetMAD_FaceMAPDesc() {return &MAD_FaceMAPDesc;}
#endif
/*
enum { mad_facemap_params };

//TODO: Add enums for various parameters
enum { 
	pb_spin,
};*/

/*static ParamBlockDesc2 mad_facemap_param_blk ( mad_facemap_params, _T("params"),  0, &MAD_FaceMAPDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF, 
	//rollout
	IDD_PANEL, IDS_PARAMS, 0, 0, NULL,
	// params
	pb_spin, 			_T("spin"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SPIN, 
		p_default, 		0.1f, 
		p_range, 		0.0f,1000.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_EDIT,	IDC_SPIN, 0.01f, 
		end,
	end
	);
*/


IObjParam *MAD_FaceMAP::ip			= NULL;

//--- MAD_FaceMAP -------------------------------------------------------
MAD_FaceMAP::MAD_FaceMAP()
{
	ByPass = 1;
	NotUseSel = 1;
	IsEdited = 0;
	AllFlags = NULL;
}

MAD_FaceMAP::~MAD_FaceMAP()
{
}

Interval MAD_FaceMAP::LocalValidity(TimeValue t)
{
	// if being edited, return NEVER forces a cache to be built 
	// after previous modifier.
	if (TestAFlag(A_MOD_BEING_EDITED))
		return NEVER;
	//TODO: Return the validity interval of the modifier
		return NEVER;
}

RefTargetHandle MAD_FaceMAP::Clone(RemapDir& remap)
{
	MAD_FaceMAP* newmod = new MAD_FaceMAP();	
	//TODO: Add the cloning code here
//	newmod->ReplaceReference(0,pblock->Clone(remap));
	return(newmod);
}

void MAD_FaceMAP::ModifyObject(TimeValue t, ModContext &mc, ObjectState * os, INode *node) 
{
	Face *FacePtr,*FaceLast;
	unsigned long *P1,*PL;
	unsigned long Counter;
    Interval        iv;
	M = &((TriObject *) os->obj)->mesh;
    iv = os->obj->ChannelValidity(t, GEOM_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, TOPO_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, SELECT_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, SUBSEL_TYPE_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, TEXMAP_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, VERT_COLOR_CHAN_NUM);

	if (AllFlags == NULL)
	{
		/* Compute Classic UV Mapping */ 
		AllFlags = (unsigned long *)malloc(M->numFaces * 4L);
		AllFlagsNumber = M->numFaces;
		FacePtr = M->faces;
		FaceLast = FacePtr + M->numFaces;
		P1 = AllFlags;
		while (FacePtr < FaceLast)
		{
			*(P1++) = ComputeOriginalFM(FacePtr++);
		}
	}

	P1 = AllFlags;
	PL = AllFlags+M->numFaces;
	Counter = 0;
	AtLeastOneIsSelected = 0;
	while (P1 < PL)
	{
		if (M->faceSel[Counter++])
		{
			AtLeastOneIsSelected = 1;
			*(P1++) |= IsSelcted;
		}
		else
			*(P1++) &= ~IsSelcted;
	}
	

	if ((ByPass & 1) && (IsEdited))
	{
		M -> setNumTVerts	(4, FALSE);
		M->tVerts[0].x = 0.0f;
		M->tVerts[0].y = 0.0f;
		M->tVerts[1].x = 0.0f;
		M->tVerts[1].y = 1.0f;
		M->tVerts[2].x = 1.0f;
		M->tVerts[2].y = 0.0f;
		M->tVerts[3].x = 1.0f;
		M->tVerts[3].y = 1.0f;
		M->tVerts[0].z = 0.0f;
		M->tVerts[1].z = 0.0f;
		M->tVerts[2].z = 0.0f;
		M->tVerts[3].z = 0.0f;
		M -> setNumTVFaces	(M->numFaces);
		P1 = AllFlags;
		for (Counter = 0 ;  Counter < (unsigned long )M->numFaces;Counter++)
		{
			M->tvFace[Counter].t[0] = GetFaceUVW(*P1, 0);
			M->tvFace[Counter].t[1] = GetFaceUVW(*P1, 1);
			M->tvFace[Counter].t[2] = GetFaceUVW(*P1, 2);
			P1++;
		}
	}
	P1 = AllFlags;
	for (Counter = 0 ;  Counter < (unsigned long )M->numFaces;Counter++)
	{
		M->faces[Counter].flags = *P1;
		P1++;
	}

	M->SetDispFlag(DISP_SELPOLYS);

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
	MAD_FaceMAP *mod = (MAD_FaceMAP*)GetWindowLong(hWnd,GWL_USERDATA);
	unsigned long *p_flags,*p_flaglast;
	
	switch (msg) {
		case WM_INITDIALOG: {
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			mod = (MAD_FaceMAP*)lParam;
			ICustButton *iBut = GetICustButton(GetDlgItem(hWnd,IDC_BYPASS));
			iBut->SetType(CBT_CHECK);
//			iBut->SetHighlightColor(BLUE_WASH);
			iBut->SetCheck( mod->ByPass );
			ReleaseICustButton(iBut);
			iBut = GetICustButton(GetDlgItem(hWnd,IDC_APPLYTOSELECTEDFACES));
			iBut->SetType(CBT_CHECK);
			iBut->SetHighlightColor(BLUE_WASH);
			iBut->SetCheck( !mod->NotUseSel);
			ReleaseICustButton(iBut);
			
			break;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_BYPASS:
					mod->ByPass^= 1;
					mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
					mod->ip->RedrawViews(mod->ip->GetTime());
					break;
				case IDC_APPLYTOSELECTEDFACES:
					mod->NotUseSel^= 1;
					mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
					mod->ip->RedrawViews(mod->ip->GetTime());
					break;
				case IDC_ROT90:
					{
						unsigned long Localou;
						p_flags = mod->AllFlags;
						p_flaglast = p_flags + mod->AllFlagsNumber;
						while (p_flags < p_flaglast)
						{
							if ((!mod->AtLeastOneIsSelected) || mod->NotUseSel || (*p_flags & IsSelcted))
							{
								/* inverse 2 and 3 */
								Localou = *p_flags & ((2<<7) | (2<<9) | (2<<11));
								*p_flags ^= Localou >> 1;
								/* */
								Localou = *p_flags & ((1<<7) | (1<<9) | (1<<11));
								*p_flags ^= (3<<7) | (3<<9) | (3<<11);
								*p_flags ^= Localou << 1;
								/* inverse 2 and 3 */
								Localou = *p_flags & ((2<<7) | (2<<9) | (2<<11));
								*p_flags ^= Localou >> 1;
							}
							p_flags++;
						}
						mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
						mod->ip->RedrawViews(mod->ip->GetTime());
					}
					break;
				case IDC_ROT270:
					{
						unsigned long Localou;
						p_flags = mod->AllFlags;
						p_flaglast = p_flags + mod->AllFlagsNumber;
						while (p_flags < p_flaglast)
						{
							if ((!mod->AtLeastOneIsSelected) || mod->NotUseSel || (*p_flags & IsSelcted))
							{
								/* inverse 1 and 0 */
								Localou = *p_flags & ((2<<7) | (2<<9) | (2<<11));
								Localou ^= ((2<<7) | (2<<9) | (2<<11));
								*p_flags ^= Localou >> 1;
								/* */
								Localou = *p_flags & ((1<<7) | (1<<9) | (1<<11));
								*p_flags ^= (3<<7) | (3<<9) | (3<<11);
								*p_flags ^= Localou << 1;
								/* inverse 1 and 0 */
								Localou = *p_flags & ((2<<7) | (2<<9) | (2<<11));
								Localou ^= ((2<<7) | (2<<9) | (2<<11));
								*p_flags ^= Localou >> 1;
							}
							p_flags++;
						}
						mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
						mod->ip->RedrawViews(mod->ip->GetTime());
					}
					break;
				case IDC_HFLIP:
					p_flags = mod->AllFlags;
					p_flaglast = p_flags + mod->AllFlagsNumber;
					while (p_flags < p_flaglast)
					{
						if ((!mod->AtLeastOneIsSelected) || mod->NotUseSel || (*p_flags & IsSelcted))
						{
							*p_flags ^= (2<<7) | (2<<9) | (2<<11);
						}
						p_flags++;
					}
					mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
					mod->ip->RedrawViews(mod->ip->GetTime());
					break;
				case IDC_VFLIP:
					p_flags = mod->AllFlags;
					p_flaglast = p_flags + mod->AllFlagsNumber;
					while (p_flags < p_flaglast)
					{
						if ((!mod->AtLeastOneIsSelected) || mod->NotUseSel || (*p_flags & IsSelcted))
						{
							*p_flags ^= (1<<7) | (1<<9) | (1<<11);
						}
						p_flags++;
					}
					mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
					mod->ip->RedrawViews(mod->ip->GetTime());
					break;
				}
			break;
		
		default:
			return FALSE;
		}
	return TRUE;
	}

void MAD_FaceMAP::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
{
	const char *types[] = { "Face" };
	this->ip = ip;
	IsEdited = 1;

	selectMode = new SelectModBoxCMode(this, ip);

	ip->EnableSubObjectSelection(TRUE);				// enable face selection
	ip->RegisterSubObjectTypes(types, 1);			// register MAX subobject (faces) selection
	ip->SetSubObjectLevel(1);						// 0 for box selection

	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime());
	hParams1  = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		ClustParamDlgProc,
		"MAD facemap",
		(LPARAM)this);
	
}

void MAD_FaceMAP::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next)
{
	ip->DeleteRollupPage(hParams1);
	IsEdited = 0;
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime());
	this->ip = NULL;
}


//From ReferenceMaker 
RefResult MAD_FaceMAP::NotifyRefChanged(
		Interval changeInt, RefTargetHandle hTarget,
		PartID& partID,  RefMessage message) 
{
	//TODO: Add code to handle the various reference changed messages
	return REF_SUCCEED;
}

//From Object
BOOL MAD_FaceMAP::HasUVW() 
{ 
	//TODO: Return whether the object has UVW coordinates or not
	return TRUE; 
}

void MAD_FaceMAP::SetGenUVW(BOOL sw) 
{  
	if (sw==HasUVW()) return;
	//TODO: Set the plugin internal value to sw				
}
#define PROUT_CHUNK 0xC0DE

IOResult MAD_FaceMAP::Load(ILoad *iload)
{
	ULONG nb;
	if (IO_OK==iload->OpenChunk()) 
	{
		iload->Read(&ByPass, sizeof(unsigned long), &nb);
		iload->Read(&NotUseSel, sizeof(unsigned long), &nb);
		iload->Read(&IsEdited, sizeof(unsigned long), &nb);
		iload->Read(&AtLeastOneIsSelected, sizeof(unsigned long), &nb);
		iload->Read(&AllFlagsNumber, sizeof(unsigned long), &nb);
		AllFlags = (unsigned long *)malloc(AllFlagsNumber * 4L);
		iload->Read(AllFlags, AllFlagsNumber * sizeof(unsigned long), &nb);
	}
	iload->CloseChunk();
	return IO_OK;
}

IOResult MAD_FaceMAP::Save(ISave *isave)
{
	ULONG nb;
	isave->BeginChunk(PROUT_CHUNK);
	isave->Write(&ByPass, sizeof(unsigned long), &nb);
	isave->Write(&NotUseSel, sizeof(unsigned long), &nb);
	isave->Write(&IsEdited, sizeof(unsigned long), &nb);
	isave->Write(&AtLeastOneIsSelected, sizeof(unsigned long), &nb);
	isave->Write(&AllFlagsNumber, sizeof(unsigned long), &nb);
	isave->Write(AllFlags, AllFlagsNumber * sizeof(unsigned long), &nb);
	isave->EndChunk();

	return IO_OK;
}



void MAD_FaceMAP::ActivateSubobjSel(int level, XFormModes& modes)
{	
	switch (level) 
	{	
	case 1: modes = XFormModes(NULL, NULL, NULL, NULL, NULL, selectMode); break;
	}	
}


int MAD_FaceMAP::HitTest(TimeValue t, 
						 INode* inode, 
						 int type, 
						 int crossing, 
						 int flags, 
						 IPoint2 *p, 
						 ViewExp *vpt, 
						 ModContext* mc)
{	
	Interval valid;
	int savedLimits, res = 0;
	GraphicsWindow *gw = vpt->getGW();				 // get the graphics window associate with the current viewport
	HitRegion hr;

	MakeHitRegion(hr, type, crossing, 4, p);	// create a hit region
	gw->setHitRegion(&hr);						// assign the region to the GW
	Matrix3 mat = inode->GetObjectTM(t);		// get the object's transform matrix
	gw->setTransform(mat);						// assign the matric to the GW
	savedLimits = gw->getRndLimits();			// save the GW status
	gw->setRndLimits((savedLimits | GW_PICK | GW_WIREFRAME | GW_Z_BUFFER) & ~GW_ILLUM); // set the hit preferences
	gw->clearHitCode();
	
	SubObjHitList hitList;
	MeshSubHitRec *rec;
	Mesh *mesh = M;  // get the tested mesh

	if (mesh)
	{
		res = mesh->SubObjectHitTest(gw, gw->getMaterial(), &hr, flags | SUBHIT_FACES, hitList); // hit test the mesh
		if (res)
		{
			rec = hitList.First();
			while (rec) 																									// save all hits
			{	
				vpt->LogHit(inode, mc, rec->dist, rec->index, NULL);
				rec = rec->Next();
			}
		}
	}
	gw->setRndLimits(savedLimits);							 // restore the GW status

	return res;
}

void MAD_FaceMAP::SelectSubComponent(HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert)
{	
	int i;
	i = 0;
/*	if ( theHold.Holding() ) {
		theHold.Put(new FaceSelRestore(ld,this));
	}*/

/*	for(; hitRec; hitRec = all ? hitRec->Next() : NULL)							// for all hits
	{
		ld = (CRotateFaceLocalData*)hitRec->modContext->localData;	// get local data
		if(ld->m_iSelCount != m_iSelCount) 
		{	ld->m_mark.ClearAll();
			ld->m_iSelCount = m_iSelCount;
		}
		Mesh *m = ld->m_mesh;							// get the mesh
		if (!m) continue;
		if(m->getNumTVerts() && m->tvFace)						// if the object have a texture applied
		{	DWORD cl = ld->m_fclist[hitRec->hitInfo];			// get the selected face's cluster index
			if(!ld->m_mark[cl])															// if the cluster is not marked
			{	ld->m_mark.Set(cl);														// mark the cluster
				for(i=0; i<m->getNumFaces() && i<m->faceSel.GetSize(); i++)				// for all faces belonging to the same cluster
					if(ld->m_fclist[i] == cl) 
						if(all && !invert)
							if(selected) 
								if(id < 0 || id == m->getFaceMtlIndex(i)) m->faceSel.Set(i);				// select the face
								else;
							else m->faceSel.Clear(i);														// unselect the face
						else	
							if(m->faceSel[i]) m->faceSel.Clear(i);											// invert the face's selection
							else if(id < 0 || id == m->getFaceMtlIndex(i)) m->faceSel.Set(i); 
			}
		}
		for(i=0; i<m->getNumFaces(); i++)
			if(m->faces[i].Hidden()) m->faceSel.Clear(i);												 // unselect the hidden faces
	}
	CreatePolygon(m_hMapEdit);																							 // update the map editor window
//	oCustToolbar->Enable();										// Enable the tool bar
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);*/
}


