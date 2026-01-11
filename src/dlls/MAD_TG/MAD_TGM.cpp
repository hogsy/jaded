/**********************************************************************
 *<
	FILE: MAD_TGM.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/

#include "MAD_TG.h"

#define PBLOCK_REF	0
#define MAD_R2O2 0.70710678118654752440084436210485f /* sqrt 2 on 2 ; cos 45° */



//--- ClassDescriptor and class vars ---------------------------------

IObjParam* MAD_TGM::ip = NULL;
HWND MAD_TGM::hParams  = NULL;

class MAD_TGMDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) {return new MAD_TGM;}
	const TCHAR *	ClassName() {return "MAD_TGM";}
	SClass_ID		SuperClassID() {return OSM_CLASS_ID;}
	Class_ID		ClassID() {return MAD_TGM_CLASS_ID; }
	const TCHAR* 	Category() {return "MAD";}
	};

static MAD_TGMDesc MAD_TGMDescCl;
extern ClassDesc* GetMAD_TGMDesc() {return &MAD_TGMDescCl;}


static BOOL CALLBACK ClustParamDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	MAD_TGM *mod = (MAD_TGM*)GetWindowLong(hWnd,GWL_USERDATA);
	
	switch (msg) {
		case WM_INITDIALOG: {
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			mod = (MAD_TGM*)lParam;
			return TRUE;
			break;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_BUTTON2:
					break;
				}
			break;
		
		default:
			return FALSE;
		}
	return FALSE;
	}

//--- ClustMod methods -------------------------------

MAD_TGM::MAD_TGM()
	{
	node  = NULL;
	tm    = Matrix3(1);
	invtm = Matrix3(1);
	O[0] = CreateNewTriObject();
	O[1] = CreateNewTriObject();
	O[2] = CreateNewTriObject();
	O[3] = CreateNewTriObject();
	O[4] = CreateNewTriObject();
	O[5] = CreateNewTriObject();
#ifdef MAX3
    M0 = &O[0]->GetMesh();
    M1 = &O[1]->GetMesh();
    M2 = &O[2]->GetMesh();
    M3 = &O[3]->GetMesh();
    MA = &O[4]->GetMesh();
    MB = &O[5]->GetMesh();
#else
    M0 = &O[0]->Mesh();
    M1 = &O[1]->Mesh();
    M2 = &O[2]->Mesh();
    M3 = &O[3]->Mesh();
    MA = &O[4]->Mesh();
    MB = &O[5]->Mesh();
#endif

	}

MAD_TGM::~MAD_TGM()
	{
	DeleteAllRefsFromMe();
	}

#define CLUSTNODE_TM_CHUNK		0x0100
#define CLUSTNODE_INVTM_CHUNK	0x0110

IOResult MAD_TGM::Load(ILoad *iload)
	{
	Modifier::Load(iload);
	IOResult res = IO_OK;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch (iload->CurChunkID()) {
			case CLUSTNODE_TM_CHUNK:
				tm.Load(iload);
				break;

			case CLUSTNODE_INVTM_CHUNK:
				invtm.Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK)  return res;
		}
	return IO_OK;
	}

IOResult MAD_TGM::Save(ISave *isave)
	{
	Modifier::Save(isave);
	isave->BeginChunk(CLUSTNODE_TM_CHUNK);
	tm.Save(isave);
	isave->EndChunk();

	isave->BeginChunk(CLUSTNODE_INVTM_CHUNK);
	invtm.Save(isave);
	isave->EndChunk();

	return IO_OK;
	}

void MAD_TGM::BeginEditParams(
		IObjParam *ip, ULONG flags,Animatable *prev)
	{
	this->ip = ip;
	hParams  = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_DIALOG2),
		ClustParamDlgProc,
		"Texture geometric Creator",
		(LPARAM)this);
	}

void MAD_TGM::EndEditParams(
		IObjParam *ip,ULONG flags,Animatable *next)
	{
	ip->ClearPickMode();
	this->ip = NULL;
	ip->DeleteRollupPage(hParams);
	}

class ClustNodeDeformer: public Deformer {
	public:
		Matrix3 tm,invtm;
		ClustNodeDeformer(Matrix3 m,Matrix3 mi) {tm=m;invtm=mi;}
		Point3 Map(int i, Point3 p) {return (p*tm)*invtm;}
	};

void AddMesh(Mesh *Mdst , Mesh *M2 )
{
	unsigned long OldNumVert , OldNumFaces;
	int Counter;
	OldNumVert  = Mdst -> numVerts  ;
	OldNumFaces = Mdst -> numFaces	;
	Mdst -> setNumVerts	(Mdst -> numVerts  + M2 -> numVerts  , TRUE, TRUE);
	Mdst -> setNumFaces	(Mdst -> numFaces  + M2 -> numFaces  , TRUE, TRUE);
	for (Counter = 0; Counter < M2 -> numVerts  ;Counter++)
	{
		Mdst -> verts[Counter + OldNumVert] = M2 -> verts[Counter];
	}
	for (Counter = 0;Counter < M2 -> numFaces ;Counter++)
	{
		Mdst -> faces[Counter + OldNumFaces] = M2 -> faces[Counter];
		Mdst -> faces[Counter + OldNumFaces] .v[0] += OldNumVert;
		Mdst -> faces[Counter + OldNumFaces] .v[1] += OldNumVert;
		Mdst -> faces[Counter + OldNumFaces] .v[2] += OldNumVert;
	}
}

void FlipMesh(Mesh *Mdst )
{
	int Counter;
	int Swap;
	for (Counter = 0;Counter < Mdst -> numFaces ;Counter++)
	{
		Swap = Mdst -> faces[Counter] .v[0];
		Mdst -> faces[Counter] .v[0] = Mdst -> faces[Counter] .v[1] ;
		Mdst -> faces[Counter] .v[1] = Swap;
	}
}

Mesh *MAD_TGM::GetTextureMesh(unsigned long Num)
{
	if (Num) return MB;
	return MA;
}


void MAD_TGM::ClipMesh(Mesh *Msrc , Mesh *M1 , Mesh *M2 , BOOL CutY , float Value)

{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int ulFcounter;
	unsigned long L1,L2;
	unsigned long F1,F2;
	unsigned long v1,v2;
	unsigned long *p_ulNewIndexs;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_ulNewIndexs = (unsigned long *)malloc(Msrc -> numVerts * 4L );
	memset(p_ulNewIndexs , 0xff , Msrc -> numVerts * 4L);
/*	M -> setNumVerts	(SaveNP + lNewNumberOfPoints	, TRUE, TRUE);
	M -> setNumFaces	(SaveNT + ulNewNumberOfFaces	, TRUE, TRUE);
	M -> setNumVertCol	(SaveNP + lNewNumberOfPoints	, TRUE);
	M -> setNumVCFaces	(SaveNT + ulNewNumberOfFaces	, TRUE , SaveNT);
	M -> setNumTVerts	(SaveUV + lNewNumberOfUV		, TRUE);
	M -> setNumTVFaces	(SaveNT + ulNewNumberOfFaces	, TRUE , SaveNT);//*/
	L1 = L2 = 0;
	F1 = F2 = 0;
	if (CutY)
	for (ulFcounter= 0 ; ulFcounter < Msrc -> numVerts	; ulFcounter++)
	{
		if (Msrc -> verts[ulFcounter] . y > Value) 
			p_ulNewIndexs[ulFcounter] = (L2++) | 0x10000000;
		else 
			p_ulNewIndexs[ulFcounter] = L1++;
	}
	else
	for (ulFcounter= 0 ; ulFcounter < Msrc -> numVerts	; ulFcounter++)
	{
		if (Msrc -> verts[ulFcounter] . x > Value) 
			p_ulNewIndexs[ulFcounter] = (L2++) | 0x10000000;
		else 
			p_ulNewIndexs[ulFcounter] = L1++;
	}
	/* Clip X */
	for (ulFcounter= 0 ; ulFcounter < Msrc -> numFaces	; ulFcounter++)
	{
		unsigned long PCount;
		PCount = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v[0]] + p_ulNewIndexs[Msrc -> faces[ulFcounter] . v[1]] + p_ulNewIndexs[Msrc -> faces[ulFcounter] . v[2]];
		PCount >>= 28;
		if (PCount == 3) 
			F2++;
		else
			if (PCount == 0)
				F1++;
			else
			{
				L1+= 2;
				L2+= 2;
				if (PCount == 1)
				{
					F1+=2;
					F2++;
				}
				else
				{
					F1++;
					F2+=2;
				}
			}
	}
	M1 -> setNumVerts	(L1 , TRUE, TRUE);
	M2 -> setNumVerts	(L2 , TRUE, TRUE);
	M1 -> setNumFaces	(F1 , TRUE, TRUE);
	M2 -> setNumFaces	(F2 , TRUE, TRUE);
	L1 = L2 = 0;
	F1 = F2 = 0;
	for (ulFcounter= 0 ; ulFcounter < Msrc -> numVerts	; ulFcounter++)
	{
		if (p_ulNewIndexs[ulFcounter] & 0x10000000)
			M2 -> verts[L2++] = Msrc -> verts[ulFcounter];
		else
			M1 -> verts[L1++] = Msrc -> verts[ulFcounter];
	}
	for (ulFcounter= 0 ; ulFcounter < Msrc -> numFaces	; ulFcounter++)
	{
		unsigned long PCount;
		PCount = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v[0]] + p_ulNewIndexs[Msrc -> faces[ulFcounter] . v[1]] + p_ulNewIndexs[Msrc -> faces[ulFcounter] . v[2]];
		PCount >>= 28;
		if (PCount == 3)
		{
			M2 -> faces[F2] . v[0] = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v [0]] & 0xfffffff;
			M2 -> faces[F2] . v[1] = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v [1]] & 0xfffffff;
			M2 -> faces[F2] . v[2] = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v [2]] & 0xfffffff;
			M2 -> faces[F2] . flags = Msrc -> faces[ulFcounter] . flags;
			M2 -> faces[F2] . smGroup = Msrc -> faces[ulFcounter] . smGroup;
			F2 ++;
		}
		else
			if (PCount== 0)
			{
				M1 -> faces[F1] . v[0] = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v [0]];
				M1 -> faces[F1] . v[1] = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v [1]];
				M1 -> faces[F1] . v[2] = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v [2]];
				M1 -> faces[F1] . flags = Msrc -> faces[ulFcounter] . flags;
				M1 -> faces[F1] . smGroup = Msrc -> faces[ulFcounter] . smGroup;
				F1++;
			}
			else
			{
				v1 = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v [0]];
				v2 = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v [1]];
				if ((v1 ^ v2) & 0x10000000)
				{
					v1 = Msrc -> faces[ulFcounter] . v [0];
					v2 = Msrc -> faces[ulFcounter] . v [1];
					if (CutY)
						M1 -> verts[L1++] = M2 -> verts[L2++] = Msrc-> verts[v1] +  (Msrc-> verts[v2] - Msrc-> verts[v1]) * (Value - Msrc-> verts[v1] . y) / (Msrc-> verts[v2] . y  - Msrc-> verts[v1] . y);
					else
						M1 -> verts[L1++] = M2 -> verts[L2++] = Msrc-> verts[v1] +  (Msrc-> verts[v2] - Msrc-> verts[v1]) * (Value - Msrc-> verts[v1] . x) / (Msrc-> verts[v2] . x  - Msrc-> verts[v1] . x);
				}
				v1 = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v [1]];
				v2 = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v [2]];
				if ((v1 ^ v2) & 0x10000000)
				{
					v1 = Msrc -> faces[ulFcounter] . v [1];
					v2 = Msrc -> faces[ulFcounter] . v [2];
					if (CutY)
						M1 -> verts[L1++] = M2 -> verts[L2++] = Msrc-> verts[v1] +  (Msrc-> verts[v2] - Msrc-> verts[v1]) * (Value - Msrc-> verts[v1] . y) / (Msrc-> verts[v2] . y  - Msrc-> verts[v1] . y);
					else
						M1 -> verts[L1++] = M2 -> verts[L2++] = Msrc-> verts[v1] +  (Msrc-> verts[v2] - Msrc-> verts[v1]) * (Value - Msrc-> verts[v1] . x) / (Msrc-> verts[v2] . x  - Msrc-> verts[v1] . x);
				}
				v1 = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v [2]];
				v2 = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v [0]];
				if ((v1 ^ v2) & 0x10000000)
				{
					v1 = Msrc -> faces[ulFcounter] . v [2];
					v2 = Msrc -> faces[ulFcounter] . v [0];
					if (CutY)
						M1 -> verts[L1++] = M2 -> verts[L2++] = Msrc-> verts[v1] +  (Msrc-> verts[v2] - Msrc-> verts[v1]) * (Value - Msrc-> verts[v1] . y) / (Msrc-> verts[v2] . y  - Msrc-> verts[v1] . y);
					else
						M1 -> verts[L1++] = M2 -> verts[L2++] = Msrc-> verts[v1] +  (Msrc-> verts[v2] - Msrc-> verts[v1]) * (Value - Msrc-> verts[v1] . x) / (Msrc-> verts[v2] . x  - Msrc-> verts[v1] . x);
				}
				if (PCount == 1)
				{
					if (p_ulNewIndexs[Msrc -> faces[ulFcounter] . v[0]] & 0x10000000) v1 = 0;
						else
							if (p_ulNewIndexs[Msrc -> faces[ulFcounter] . v[1]] & 0x10000000) v1 = 1;
							else
								v1 = 2;
					M1 -> faces[F1] . v[0] = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v[(v1 + 1) % 3]];
					M1 -> faces[F1] . v[1] = L1 - 2;
					M1 -> faces[F1] . v[2] = L1 - 1;
					M1 -> faces[F1] . flags = Msrc -> faces[ulFcounter] . flags;
					M1 -> faces[F1] . flags |= EDGE_B;
					M1 -> faces[F1] . smGroup = Msrc -> faces[ulFcounter] . smGroup;
					if (v1 == 0)
					{
						M1 -> faces[F1] . v[1] = L1 - 1;
						M1 -> faces[F1] . v[2] = L1 - 2;
					} 
					F1++;
					M1 -> faces[F1] . v[0] = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v[(v1 + 1) % 3]];
					M1 -> faces[F1] . v[1] = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v[(v1 + 2) % 3]];
					M1 -> faces[F1] . v[2] = L1 - 2;
					M1 -> faces[F1] . flags = Msrc -> faces[ulFcounter] . flags;
					M1 -> faces[F1] . smGroup = Msrc -> faces[ulFcounter] . smGroup;
					if (v1 == 0)
						M1 -> faces[F1] . v[2] = L1 - 1;
					F1++;
					M2 -> faces[F2] . v[0] = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v[v1]] & 0xfffffff;
					M2 -> faces[F2] . v[1] = L2 - 2;
					M2 -> faces[F2] . v[2] = L2 - 1;
					M2 -> faces[F2] . flags = Msrc -> faces[ulFcounter] . flags;
					M2 -> faces[F2] . flags |= EDGE_B;
					M2 -> faces[F2] . smGroup = Msrc -> faces[ulFcounter] . smGroup;
					if (v1 != 0)
					{
						M2 -> faces[F2] . v[1] = L2 - 1;
						M2 -> faces[F2] . v[2] = L2 - 2;//*/
					}
//					M2 -> faces[F2] . v[0] = M2 -> faces[F2] . v[1] = M2 -> faces[F2] . v[2] = 0;
					F2++;
				}
				else
				{
					if (!(p_ulNewIndexs[Msrc -> faces[ulFcounter] . v[0]] & 0x10000000)) v1 = 0;
						else
							if (!(p_ulNewIndexs[Msrc -> faces[ulFcounter] . v[1]] & 0x10000000)) v1 = 1;
							else
								v1 = 2;
					M2 -> faces[F2] . v[0] = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v[(v1 + 1) % 3]] & 0xfffffff;
					M2 -> faces[F2] . v[1] = L2 - 2;
					M2 -> faces[F2] . v[2] = L2 - 1;
					M2 -> faces[F2] . flags = Msrc -> faces[ulFcounter] . flags;
					M2 -> faces[F2] . flags |= EDGE_B;
					M2 -> faces[F2] . smGroup = Msrc -> faces[ulFcounter] . smGroup;
					if (v1 == 0)
					{
						M2 -> faces[F2] . v[1] = L2 - 1;
						M2 -> faces[F2] . v[2] = L2 - 2;
					} 
//					M2 -> faces[F2] . v[0] = M2 -> faces[F2] . v[1] = M2 -> faces[F2] . v[2] = 0;
					F2++;
					M2 -> faces[F2] . v[0] = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v[(v1 + 1) % 3]] & 0xfffffff;
					M2 -> faces[F2] . v[1] = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v[(v1 + 2) % 3]] & 0xfffffff;
					M2 -> faces[F2] . v[2] = L2 - 2;
					M2 -> faces[F2] . flags = Msrc -> faces[ulFcounter] . flags;
					M2 -> faces[F2] . smGroup = Msrc -> faces[ulFcounter] . smGroup;
//					M2 -> faces[F2] . v[0] = M2 -> faces[F2] . v[1] = M2 -> faces[F2] . v[2] = 0;
					if (v1 == 0)
						M2 -> faces[F2] . v[2] = L2 - 1;

					F2++;
					M1 -> faces[F1] . v[0] = p_ulNewIndexs[Msrc -> faces[ulFcounter] . v[v1]] ;
					M1 -> faces[F1] . v[1] = L1 - 2;
					M1 -> faces[F1] . v[2] = L1 - 1;//*/
					M1 -> faces[F1] . flags = Msrc -> faces[ulFcounter] . flags;
					M1 -> faces[F1] . flags |= EDGE_B;
					M1 -> faces[F1] . smGroup = Msrc -> faces[ulFcounter] . smGroup;
					if (v1 != 0)
					{
						M1 -> faces[F1] . v[1] = L1 - 1;
						M1 -> faces[F1] . v[2] = L1 - 2;//*/
					}
					F1++;
				}
			}
	}
/*	M -> setNumVertCol	(SaveNP + lNewNumberOfPoints	, TRUE);
	M -> setNumVCFaces	(SaveNT + ulNewNumberOfFaces	, TRUE , SaveNT);
	M -> setNumTVerts	(SaveUV + lNewNumberOfUV		, TRUE);
	M -> setNumTVFaces	(SaveNT + ulNewNumberOfFaces	, TRUE , SaveNT);//*/

	/* Clip Y */
	free(p_ulNewIndexs);
}

void MAD_TGM::ModifyObject(
		TimeValue t, ModContext &mc, ObjectState *os, INode *node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int C2;
    Mesh            *M;
    Interval        iv;
	Point3			stMax,stMin;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//	if (!node) return; /* No geometric texture, -> exit */
    iv =  os->obj->ChannelValidity(t, GEOM_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, TOPO_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, SELECT_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, TEXMAP_CHAN_NUM);
    iv &= os->obj->ChannelValidity(t, VERT_COLOR_CHAN_NUM);
	iv &= os->obj->ChannelValidity(t, DISP_ATTRIB_CHANNEL);

    M = &((TriObject *) os->obj)->mesh;
	/*----------------------------------------------------------------------------------------------*/
    /* Compute bdvol ..
	/*----------------------------------------------------------------------------------------------*/
	stMax . x = stMax . y = stMax . z = -10.0e37f;
	stMin . x = stMin . y = stMin . z = 10.0e37f;
	for (C2 = 0 ; C2 < M->numVerts; C2 ++)
	{
		if (stMax . x < M->verts[C2] . x) stMax . x = M->verts[C2] . x;
		if (stMax . y < M->verts[C2] . y) stMax . y = M->verts[C2] . y;
		if (stMax . z < M->verts[C2] . z) stMax . z = M->verts[C2] . z;

		if (stMin . x > M->verts[C2] . x) stMin . x = M->verts[C2] . x;
		if (stMin . y > M->verts[C2] . y) stMin . y = M->verts[C2] . y;
		if (stMin . z > M->verts[C2] . z) stMin . z = M->verts[C2] . z;
	}
	/*==============================================================================================*/
    /* Buil the four Meshes																			*/
	/*==============================================================================================*/
	/*----------------------------------------------------------------------------------------------*/
    /* Rotate mesh of 45°..
	/*----------------------------------------------------------------------------------------------*/
	for (C2 = 0 ; C2 < M->numVerts; C2 ++)
	{
		float fX;
		fX = M->verts[C2] . x;
		M->verts[C2] . x = fX * MAD_R2O2 + M->verts[C2] . y * MAD_R2O2 ;
		M->verts[C2] . y = - fX * MAD_R2O2 + M->verts[C2] . y * MAD_R2O2 ;
	}
	ClipMesh(M , MA , MB , FALSE , 0.0f);
	ClipMesh(MA , M0 , M1 , TRUE , 0.0f);
	ClipMesh(MB , M2 , M3 , TRUE , 0.0f);
	*M = *M0;
	AddMesh(M, M1 );
	AddMesh(M, M2 );
	AddMesh(M, M3 );
	*M = *MA;
	AddMesh(M, MB );
	/*----------------------------------------------------------------------------------------------*/
    /* Rotate mesh of -45°..
	/*----------------------------------------------------------------------------------------------*/
	float fX;
	for (C2 = 0 ; C2 < M->numVerts; C2 ++)
	{
		fX = M->verts[C2] . x;
		M->verts[C2] . x = fX * MAD_R2O2 - M->verts[C2] . y * MAD_R2O2 ;
		M->verts[C2] . y = fX * MAD_R2O2 + M->verts[C2] . y * MAD_R2O2 ;
	}

	for (C2 = 0 ; C2 < MA->numVerts; C2 ++)
	{
		fX = MA->verts[C2] . x;
		MA->verts[C2] . x = fX * MAD_R2O2 - MA->verts[C2] . y * MAD_R2O2 ;
		MA->verts[C2] . y = fX * MAD_R2O2 + MA->verts[C2] . y * MAD_R2O2 ;
	}
	for (C2 = 0 ; C2 < MB->numVerts; C2 ++)
	{
		fX = MB->verts[C2] . x;
		MB->verts[C2] . x = fX * MAD_R2O2 - MB->verts[C2] . y * MAD_R2O2 ;
		MB->verts[C2] . y = fX * MAD_R2O2 + MB->verts[C2] . y * MAD_R2O2 ;
	}

	/*----------------------------------------------------------------------------------------------*/
    /* Make it feet the mesh in a box 1 1 1 */
	/*----------------------------------------------------------------------------------------------*/
	stMax . x = BoxSizeXY / (stMax . x - stMin . x);
	stMax . y = BoxSizeXY / (stMax . y - stMin . y);
	stMax . z = BoxSizeZ / (stMax . z - stMin . z);
	stMin . x *= stMax . x;
	stMin . y *= stMax . y;
	stMin . z *= stMax . z;
	stMin . x += BoxSizeXY;
	stMin . y += BoxSizeXY;
	stMin . z += BoxSizeZ;
	for (C2 = 0 ; C2 < M->numVerts; C2 ++)
	{
		M->verts[C2] = M->verts[C2] * stMax + stMin;
	}
	for (C2 = 0 ; C2 < MA->numVerts; C2 ++)
	{
		MA->verts[C2] = MA->verts[C2] * stMax + stMin;
		MA->verts[C2] .z *= 1.0f / BoxSizeZ;
		MA->verts[C2] .z -= 1.0f;
	}
	for (C2 = 0 ; C2 < MB->numVerts; C2 ++)
	{
		MB->verts[C2] = MB->verts[C2] * stMax + stMin;
		MB->verts[C2] .z *= -1.0f / BoxSizeZ;
		/* rotate 180° */
		MB->verts[C2] = -MB->verts[C2];
		MB->verts[C2] .x += BoxSizeXY;
		MB->verts[C2] .y += BoxSizeXY;
		MB->verts[C2] .z -= 1.0f;
	}
/*	FlipMesh(MB);
	FlipMesh(MA);*/
   
    /* Compute the new number of triangles */
    os->obj->UpdateValidity(GEOM_CHAN_NUM, FOREVER);
    os->obj->UpdateValidity(TOPO_CHAN_NUM, FOREVER);
    os->obj->UpdateValidity(SELECT_CHAN_NUM, FOREVER);
    os->obj->UpdateValidity(TEXMAP_CHAN_NUM, FOREVER);
    os->obj->UpdateValidity(VERT_COLOR_CHAN_NUM, FOREVER);
    os->obj->UpdateValidity(DISP_ATTRIB_CHANNEL, FOREVER);
}

Interval MAD_TGM::LocalValidity(TimeValue t)
	{
	Interval valid = FOREVER;
	if (node) {
		node->GetNodeTM(t,&valid);
		}
	return valid;
	}

RefTargetHandle MAD_TGM::Clone(RemapDir& remap)
	{
	MAD_TGM *newmod = new MAD_TGM;
	newmod->ReplaceReference(0,node);
	newmod->tm    = tm;
	newmod->invtm = invtm;
	return newmod;
	}

RefResult MAD_TGM::NotifyRefChanged(
		Interval changeInt, 
		RefTargetHandle hTarget, 
		PartID& partID, 
		RefMessage message)
	{
	switch (message) {
		case REFMSG_TARGET_DELETED:
			if (hTarget==node) {
				node = NULL;
				}
			break;
		}
	return REF_SUCCEED;
	}
