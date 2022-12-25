/**********************************************************************
*<
FILE: DxStdMtl2.cpp

DESCRIPTION:	DirectX interface for the Standard Material

CREATED BY:		Neil Hazzard

HISTORY:		Created:  19/04/04


*>	Copyright (c) 2004, All Rights Reserved.
**********************************************************************/


#include "mtlhdr.h"
#include "DxStdMtl2.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "custattrib.h"
#include "icustattribcontainer.h"
#include "IViewportManager.h"

#ifndef NO_ASHLI // MSW 5/14/2004
#include "rtmax.h"

#pragma comment( lib, "rtmax.lib" )
#pragma comment( lib, "mnmath.lib" ) 
#pragma comment( lib, "d3dx9.lib" )

#define VIEWPORTLOADER_CLASS_ID Class_ID(0x5a06293c, 0x30420c1e)

#define RENTRANT_COUNT 3

//#define FORCE_SMOKETEST

static LPDIRECT3DDEVICE9 GetDevice()
{
	GraphicsWindow		*GW;
	ViewExp				*View;
	LPDIRECT3DDEVICE9	Device;

	View = GetCOREInterface()->GetActiveViewport();

	if(View)
	{
		GW = View->getGW();

		if(GW)
		{
			ID3D9GraphicsWindow *D3DGW = (ID3D9GraphicsWindow *)GW->GetInterface(D3D9_GRAPHICS_WINDOW_INTERFACE_ID);

			if(D3DGW)
			{
				Device = D3DGW->GetDevice();

				return(Device);
			}
		}
	}
	return NULL;
}

static bool IsDxMaterialEnabled(MtlBase * map)
{
	bool enabled = false;
	ICustAttribContainer * cont = map->GetCustAttribContainer();

	if(cont)
	{
		for(int i=0; i< cont->GetNumCustAttribs(); i++)
		{
			CustAttrib * ca = cont->GetCustAttrib(i);
			if(ca->GetInterface(VIEWPORT_SHADER_MANAGER_INTERFACE2)){

				IViewportShaderManager2 * vsm = (IViewportShaderManager2*)ca->GetInterface(VIEWPORT_SHADER_MANAGER_INTERFACE2);
				return vsm->IsDxStdMtlEnabled();
			}
		}
	}
	return enabled;
}

static bool TMNegParity(Matrix3 &Mat)
{
	return (DotProd(CrossProd(Mat.GetRow(0),Mat.GetRow(1)),Mat.GetRow(2)) < 0.0) ? 1 : 0;
}

DxStdMtl2::DxStdMtl2(StdMat2 * m) : map(m)
{
	stdDualVS = (IStdDualVS*) CreateInstance(REF_MAKER_CLASS_ID, STD_DUAL_VERTEX_SHADER);
	if(stdDualVS)
		stdDualVS->SetCallback((IStdDualVSCallback*)this);

	pd3dDevice = NULL;
	pEffect = NULL;
	pEffectParser = NULL;
	bTransparency = false;
	bPreDeleteLight = false;
	mpMeshCache = IRenderMeshCache::GetRenderMeshCache();
//	codeGen = NULL;
	mpMeshCache->SetSizeOfCache(MAX_MESH_CACHE,IRenderMesh::kMesh);
    m_CurCache		= 0;
	sceneLights.SetCount(0);
	bRefreshElements = false;
	bReEntrant = false;
	ReEntrantCount = RENTRANT_COUNT;
	UpdateSceneLights();
	RegisterNotification(NotificationCallback,this,NOTIFY_SCENE_ADDED_NODE);
	RegisterNotification(NotificationCallback,this,NOTIFY_SCENE_POST_DELETED_NODE);
	RegisterNotification(NotificationCallback,this,NOTIFY_SCENE_PRE_DELETED_NODE);
	RegisterNotification(NotificationCallback,this,NOTIFY_SCENE_UNDO);
	RegisterNotification(NotificationCallback,this,NOTIFY_SCENE_REDO);
	RegisterNotification(NotificationCallback,this,NOTIFY_SCENE_PRE_UNDO);
	RegisterNotification(NotificationCallback,this,NOTIFY_SCENE_PRE_REDO);
	RegisterNotification(NotificationCallback,this,NOTIFY_FILE_POST_OPEN);
	RegisterNotification(NotificationCallback, this, NOTIFY_D3D_PRE_DEVICE_RESET);
	RegisterNotification(NotificationCallback, this, NOTIFY_D3D_POST_DEVICE_RESET);


}

DxStdMtl2::~DxStdMtl2()
{
	if(stdDualVS)
		stdDualVS->DeleteThis();

	//need to the get the macros in here.
	SAFE_RELEASE(pEffect);
	SAFE_DELETE(mpMeshCache);

	if(pEffectParser)
	{
		pEffectParser->DestroyParser();
		pEffectParser = NULL;
	}



	for(int f =0; f<sceneLights.Count();f++)
	{
		delete sceneLights[f];
	}


	UnRegisterNotification(NotificationCallback,this,NOTIFY_SCENE_ADDED_NODE);
	UnRegisterNotification(NotificationCallback,this,NOTIFY_SCENE_POST_DELETED_NODE);
	UnRegisterNotification(NotificationCallback,this,NOTIFY_SCENE_PRE_DELETED_NODE);
	UnRegisterNotification(NotificationCallback,this,NOTIFY_SCENE_UNDO);
	UnRegisterNotification(NotificationCallback,this,NOTIFY_SCENE_REDO);
	UnRegisterNotification(NotificationCallback,this,NOTIFY_FILE_POST_OPEN);
	UnRegisterNotification(NotificationCallback,this,NOTIFY_SCENE_PRE_UNDO);
	UnRegisterNotification(NotificationCallback,this,NOTIFY_SCENE_PRE_REDO);
	UnRegisterNotification(NotificationCallback, this, NOTIFY_D3D_PRE_DEVICE_RESET);
	UnRegisterNotification(NotificationCallback, this, NOTIFY_D3D_POST_DEVICE_RESET);




}

IHLSLCodeGenerator::CodeVersion DxStdMtl2::GetPixelShaderSupport(LPDIRECT3DDEVICE9 pd3dDevice, DWORD & instSlots)
{
	D3DCAPS9	Caps;
	IHLSLCodeGenerator::CodeVersion code;
	pd3dDevice->GetDeviceCaps(&Caps);
	UINT major = D3DSHADER_VERSION_MAJOR(Caps.PixelShaderVersion);
	UINT minor = D3DSHADER_VERSION_MINOR(Caps.PixelShaderVersion);

	instSlots = 96;

	if(major < 2 )
		code = IHLSLCodeGenerator::PS_1_X;
	else if(major == 2)
	{
		instSlots = Caps.PS20Caps.NumInstructionSlots;
		if(minor > 0)
			code = IHLSLCodeGenerator::PS_2_X;
		else
			code = IHLSLCodeGenerator::PS_2_0;
	}
	else if(major >=3)
	{
		instSlots = Caps.MaxPixelShader30InstructionSlots;
		code = IHLSLCodeGenerator::PS_3_0;
	}
	else
	{
		code = IHLSLCodeGenerator::PS_1_X;
	}

	return code;

}

HRESULT DxStdMtl2::Initialize(Mesh *mesh, INode *node)
{
    m_CurCache = mpMeshCache->SetCachedMesh(mesh,node,GetCOREInterface()->GetTime(),bRefreshElements);
//	UpdateGeomElements();

	if(stdDualVS)
	{
		stdDualVS->Initialize(mesh, node);
	}
	Draw();

	return S_OK;
}

HRESULT DxStdMtl2::Initialize(MNMesh *mnmesh, INode *node)
{
	Mesh mesh;
	mnmesh->OutToTri(mesh);
	m_CurCache = mpMeshCache->SetCachedMesh(&mesh,node,GetCOREInterface()->GetTime(),bRefreshElements);

    //	UpdateGeomElements();
	if(stdDualVS)
	{
		stdDualVS->Initialize(mnmesh, node);
	}
	Draw();
	return S_OK;
}

HRESULT DxStdMtl2::ConfirmDevice(ID3D9GraphicsWindow *d3dgw)
{

	myGWindow = d3dgw;
	return S_OK;
}

HRESULT DxStdMtl2::ConfirmPixelShader(IDX9PixelShader *pps)
{
	return S_OK;
}

ReferenceTarget *DxStdMtl2::GetRefTarg()
{
	return map;
}

VertexShaderCache *DxStdMtl2::CreateVertexShaderCache()
{
	return new IDX8VertexShaderCache;
}

HRESULT DxStdMtl2::InitValid(Mesh *mesh, INode *node)
{
	mpMeshCache->GetActiveRenderMesh(m_CurCache)->Invalidate();
	return(S_OK);
}

HRESULT DxStdMtl2::InitValid(MNMesh *mnmesh, INode *node)
{
	HRESULT hr = S_OK;
	mpMeshCache->GetActiveRenderMesh(m_CurCache)->Invalidate();
    return hr;
}


static bool IsTransparent(StdMat2 * map)
{
	TimeValue t = GetCOREInterface()->GetTime();

	if(map->MapEnabled(ID_OP) && map->GetTexmapAmt(ID_OP,t)>0)
		return true;

	if(map->GetOpacity(t)<1.0f)
		return true;


	return false;
}

void DxStdMtl2::SetRenderStates()
{

	if(pd3dDevice)
	{
		pd3dDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
		pd3dDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
		pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);	
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);	
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);
		pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
		pd3dDevice->SetRenderState(D3DRS_SHADEMODE,D3DSHADE_GOURAUD);
		pd3dDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);

		if(map->Requirements(0)& MTLREQ_2SIDE)
			pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		else
			pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

		if(IsTransparent(map))
		{
			pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);	
			pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);  
		}

        for(int i=0; i<8; i++)
			pd3dDevice->SetTextureStageState(i, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
	}
}


int DxStdMtl2::GetMatIndex(Mtl * m_Mtl)
{
	int				i;
	Mtl			*Std;
	StdMat2  * Shader;
	//	MaxShader   *Shader, *s;


	if(m_Mtl->IsMultiMtl())
	{
		Shader = (StdMat2 *)GetRefTarg();

		// I use the SubAnims here, as I do not want any NULL material introduced that are not visible to the user
		// this can happen when you have N materials and you select N+1 mat ID - the material will add a NULL material 
		// to the list to compensate - this screws with the index into the paramblock
		for(i=0; i < m_Mtl->NumSubs(); i++)
		{	
			Std = (Mtl*)m_Mtl->SubAnim(i);

			if(Std!=NULL)
			{
				if(Std->NumSubMtls()>0)
				{

					for(int j=0; j< Std->NumSubMtls();j++)
					{
						Mtl * subMtl = Std->GetSubMtl(j);
						if(subMtl == Shader)
							Std = subMtl;

					}
				}
				MSPlugin* plugin = (MSPlugin*)((ReferenceTarget*)Std)->GetInterface(I_MAXSCRIPTPLUGIN);
				ReferenceTarget * targ = NULL;
				if(plugin){
					targ = plugin->get_delegate();
					Std = (Mtl*)targ;
				}
				if(Std == Shader)
				{
					int id=0;
					// this gets interesting - the MatID are editable !! must get them from the PAramblock
					IParamBlock2 * pblock = m_Mtl->GetParamBlock(0);	// there is only one
					if(pblock)
					{
						ParamBlockDesc2 * pd = pblock->GetDesc();

						for(int j=0;j<pd->count;j++)
						{
							if(_tcsicmp(_T("materialIDList"),pd->paramdefs[j].int_name)==0)	//not localised
							{
								//int id;
								pblock->GetValue(j,0,id,FOREVER,i); 
								id = id+1;	//for some reason this is stored as a zero index, when a 1's based index is used
							}	

						}
						pblock->ReleaseDesc();

					}
					return(id);
				}
			}
		}
	}
	return(0);
}

void DxStdMtl2::SetEffectData()
{

	D3DXMATRIX	MatWorld;
	D3DXMATRIX	MatView;
	D3DXMATRIX	MatProj;
	D3DXMATRIX	Result;
	D3DXMATRIX  WorldView;
	D3DXMATRIX  WorldViewProj;
	D3DXMATRIX	MatConv;

	D3DXMatrixIdentity(&MatConv);
	MatConv.m[1][1] = 0.0f;
	MatConv.m[1][2] = 1.0f;
	MatConv.m[2][1] = 1.0f;
	MatConv.m[2][2] = 0.0f;

	TimeValue t = GetCOREInterface()->GetTime();



	MatWorld = myGWindow->GetWorldXform();
	MatView  = myGWindow->GetViewXform();
	MatProj  = myGWindow->GetProjXform();


	D3DXMATRIX MatTemp;

	D3DXMatrixMultiply(&MatTemp,&MatWorld,&MatView);
	D3DXMatrixMultiply(&WorldViewProj,&MatTemp,&MatProj);
	D3DXMatrixMultiply(&WorldView,&MatWorld,&MatView);

//	D3DXMatrixMultiply(&MatWorld,&MatWorld,&MatConv);

	for(int i=0; i<elementContainer.NumberOfElements();i++)
	{

		EffectElements * ele = elementContainer.GetElement(i);
		ele->SetRenderNode(mpMeshCache->GetActiveNode(m_CurCache));

		switch(ele->GetSemantic())
		{
		case kSemanticWorldView:
			{
				pEffect->SetMatrixTranspose( ele->GetParamHandle(), &WorldView);
			}
			break;
		case kSemanticProjection:
			{
				pEffect->SetMatrixTranspose( ele->GetParamHandle(), &MatProj);
			}
			break;
		case kSemanticWorldT:
			{
				pEffect->SetMatrixTranspose( ele->GetParamHandle(), &MatWorld);
			}
			break;
		case kSemanticWorldI:
			{
				D3DXMATRIX MatInvWorld;
				D3DXMatrixInverse(&MatInvWorld,NULL,&MatWorld);
				pEffect->SetMatrix( ele->GetParamHandle(), &MatInvWorld);
				
			}
			break;

		case kSemanticWorldViewProj:
			{
				pEffect->SetMatrix( ele->GetParamHandle(), &WorldViewProj);
			}
			break;
		case kSemanticWorldCamPos:
			{
				D3DXVECTOR3 camPos,camWPos;
				camPos.x = camPos.y = camPos.z  = 0.0f;

				// Inverse of View Transform to Transform Camera from Direct3D Camera Space
				// to 3ds max World Space

				D3DXMatrixInverse(&MatTemp,NULL,&MatView);
	
				// Find Camera position in Direct3D World Space
				D3DXVec3TransformCoord(&camWPos, &camPos, &MatTemp);
				D3DXVECTOR4 temp = D3DXVECTOR4(camWPos.x,camWPos.y,camWPos.z,1.0f);
				pEffect->SetVector(ele->GetParamHandle(), &temp);

			}
			break;
		case kSemanticViewportDimensions:
			{
				int w =0, h =0;
				myGWindow->GetWindowDimension(w,h);
				D3DXVECTOR4 temp(float(w),float(h),0.5,0.5);
				pEffect->SetVector(ele->GetParamHandle(),&temp);
			}
			break;


		}
		ele->SetValue(pEffect,map);



	}

}

#define RED_COLOR	0x00ff0000

void DxStdMtl2::Draw()
{
	bool	NegScale;

	if(!IsDxMaterialEnabled(map))
		return;

	TimeValue m_T = GetCOREInterface()->GetTime();
	NegScale = TMNegParity(mpMeshCache->GetActiveNode(m_CurCache)->GetObjTMAfterWSM(m_T));
	Mtl * m_Mtl	 = mpMeshCache->GetActiveNode(m_CurCache)->GetMtl();

	// make sure mapping it setup before we evaluate.
	if(pEffectParser)
		mpMeshCache->GetActiveRenderMesh(m_CurCache)->SetMappingData(pEffectParser->GetMappingChannels());

	if(!mpMeshCache->GetActiveRenderMesh(m_CurCache)->Evaluate(pd3dDevice, mpMeshCache->GetActiveMesh(m_CurCache),GetMatIndex(m_Mtl),NegScale))
		return;

	Color  col = GetCOREInterface()->GetViewportBGColor();
	D3DCOLOR bkgColor = col.toRGB();
	
	if(!pd3dDevice)
		pd3dDevice = GetDevice();


	int w =0, h =0;
	myGWindow->GetWindowDimension(w,h);
	SetRenderStates();
	// force this
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    if(pEffect && pEffectParser)
	{
		SetEffectData();
		if(!pEffectParser->PreRender(pd3dDevice,pEffect,mpMeshCache->GetActiveRenderMesh(m_CurCache),this,bkgColor, w,h))
			goto error;
	
		
		pEffect->SetTechnique(hTechnique);

		if(!pEffectParser->Render(pd3dDevice,pEffect,mpMeshCache->GetActiveRenderMesh(m_CurCache),hTechnique))
		{
			UINT uPasses = 0;
			UINT uPass;

			pEffect->Begin(&uPasses, 0 );
			for(uPass = 0; uPass < uPasses; uPass++)
			{
				pEffect->BeginPass(uPass);
				mpMeshCache->GetActiveRenderMesh(m_CurCache)->Render(pd3dDevice);
				pEffect->EndPass();
			}
			pEffect->End();	
		}
		pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);	
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	}
//draw the object in Red WireFrame mode for testing purposes

	else{
		
error:	D3DMATERIAL9 mtrl;
		ZeroMemory(&mtrl, sizeof(mtrl));
		mtrl.Diffuse.r = 1.0f;
		mtrl.Ambient.r = 1.0f;
		mtrl.Ambient.g = 1.0f;
		mtrl.Ambient.b = 1.0f;
		mtrl.Ambient.a = 1.0f;
		IDirect3DStateBlock9 * pState;
		pd3dDevice->CreateStateBlock(D3DSBT_ALL,&pState);

		pd3dDevice->SetMaterial(&mtrl);
		pd3dDevice->SetRenderState(D3DRS_SHADEMODE,D3DSHADE_FLAT);
		pd3dDevice->SetRenderState(D3DRS_AMBIENT, RED_COLOR);
		pd3dDevice->SetRenderState(D3DRS_LIGHTING,TRUE);
		pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
		pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		mpMeshCache->GetActiveRenderMesh(m_CurCache)->Evaluate(pd3dDevice, mpMeshCache->GetActiveMesh(m_CurCache),GetMatIndex(m_Mtl),NegScale);
		mpMeshCache->GetActiveRenderMesh(m_CurCache)->Render(pd3dDevice);
		
		if(pState){
			pState->Apply();
			SAFE_RELEASE(pState);
		}
	}

}

bool DxStdMtl2::DrawMeshStrips(ID3D9GraphicsWindow *d3dgw, MeshData *data)
{
	return IsDxMaterialEnabled(map);
}

bool DxStdMtl2::CreateAndLoadEffectData()
{
	ResetDXStates();

	if(IsDxMaterialEnabled(map))
	{
		bool useLPRT;
		TCHAR filename[MAX_PATH];
		_tcscpy(filename,GetCOREInterface()->GetDir(APP_PLUGCFG_DIR));
		_tcscat(filename,"\\DXDisplay.ini");

		useLPRT = GetPrivateProfileInt(_T("Settings"),_T("LPRTEnabled"),0,filename) ? true : false;
			
		if(!pd3dDevice)
			pd3dDevice = GetDevice();

		if(!pd3dDevice)
			return false;

		SAFE_RELEASE(pEffect);
		

		if(pEffectParser)
		{
			pEffectParser->DestroyParser();
			pEffectParser = NULL;
		}

		elementContainer.DeleteAllElements();

		UpdateSceneLights();
		DWORD numberOfPSinst = 0;
		
		//lets make sure we get the correct code for the GFX card..
		IHLSLCodeGenerator::CodeVersion code = GetPixelShaderSupport(pd3dDevice,numberOfPSinst);

		if(code == IHLSLCodeGenerator::PS_1_X)
			return false;

		IHLSLCodeGenerator * codeGen = IHLSLCodeGenerator::GetHLSLCodeGenerator();

		Tab<INode*> lights;
		for(int j=0;j<sceneLights.Count();j++){
			INode * Light = sceneLights[j]->GetLightNode();
			lights.Append(1, &Light);
		}

		TCHAR * effectString = codeGen->GenerateEffectFile(map,lights,code,bTransparency,numberOfPSinst,useLPRT);


		if(effectString)
			D3DXCreateEffect(pd3dDevice, effectString, _tcslen(effectString),NULL, NULL,NULL,NULL, &pEffect, NULL);

		if(pEffect && !pEffectParser)
			CreateEffectParser();
		if(pEffect){
			pEffectParser->SetUseLPRT(useLPRT);
			pEffectParser->ParseEffectFile(pd3dDevice,pEffect,NULL,this);
			PatchInLightNodes();
			LoadTextureData(codeGen);
		}
		delete codeGen;

		mpMeshCache->GetActiveRenderMesh(m_CurCache)->Invalidate();
		GetCOREInterface()->ForceCompleteRedraw(FALSE);

	}

	return true;
}

static bool CheckParamBlock(IParamBlock2 * pblock)
{
	ParamBlockDesc2 * pbd = pblock->GetDesc();
	ParamID id = pblock->LastNotifyParamID();
	if(id != -1)
	{
		ParamDef &def = pbd->GetParamDef(id);
		for(int j=0; j < supportedParam_count;j++)
		{
			if(_tcscmp(def.int_name, supportedParams[j])==0){
				pblock->ReleaseDesc();
				return true;
			}
		}
	}
	return false;
}

static bool CheckParamBlock(IParamBlock * pblock)
{
	int id = pblock->LastNotifyParamNum();
	if(id != -1)
	{
		for(int j=0; j < supportedParam_count;j++)
		{
			if(_tcsicmp(pblock->SubAnimName(id).data(), supportedParams[j])==0){
				return true;
			}
		}
	}
	return false;
}

//This function will determine whether the effect needs rebuiling or the change comes from a EffectElement managed parameter
RefResult DxStdMtl2::NotifyMaterialChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message)
{
	bool supported = false;
	RefResult res = REF_SUCCEED;

	if(IsDxMaterialEnabled(map)&& !bReEntrant)
	{
/*		if(hTarget->SuperClassID() == SClass_ID(TEXMAP_CONTAINER_CLASS_ID))
		{
			Texmap * tex = (Texmap*)hTarget;
			if(!tex->TestMtlFlag(MTL_HW_TEX_ENABLED))
				return REF_SUCCEED;

		}
*/
		if(hTarget->ClassID() == Class_ID(PARAMETER_BLOCK2_CLASS_ID,0))
		{
			IParamBlock2 * pblock = (IParamBlock2*)hTarget;
			if(CheckParamBlock(pblock))
				return REF_SUCCEED;
		}
		for(int i=0; i< hTarget->NumParamBlocks();i++)
		{
			IParamBlock2 * pblock = hTarget->GetParamBlock(i);
			if(CheckParamBlock(pblock))
				return REF_SUCCEED;
		}
		if(!supported){
//			ReEntrantCount--;
			bReEntrant = true;
			CreateAndLoadEffectData();
		}
	}
	else
	{
		res = bReEntrant ? REF_STOP : REF_SUCCEED;
//		ReEntrantCount = RENTRANT_COUNT;
		bReEntrant = false;	
	}
	//make sure the cache is flushed...
	if(!IsDxMaterialEnabled(map))
	{
		elementContainer.DeleteAllElements();
		if(pEffectParser){
			pEffectParser->DestroyParser();
			pEffectParser = NULL;
		}
	}

	return res;
}

HRESULT DxStdMtl2::CreateEffectParser()
{	
	LPDIRECT3DDEVICE9 pd3dDevice = GetDevice();
	HRESULT hr = D3D_OK;
	LPD3DXEFFECT effect = NULL;

	IParserLoader * parserLoader = GetParserLoader();
	pEffectParser = parserLoader->GetParser(0L);	//default to the old parser for now...
	if(!pEffectParser)
		return D3DERR_INVALIDCALL;

	return hr;
}

void DxStdMtl2::FindLights(INode * parentNode)
{
	TimeValue t = GetCOREInterface()->GetTime();

	for(int i=0; i < parentNode->NumberOfChildren(); i++) 
	{
		INode * node = parentNode->GetChildNode(i);
		ObjectState Os   = node->EvalWorldState(t);
		if(Os.obj && Os.obj->SuperClassID() == LIGHT_CLASS_ID) 
		{
			LightCache * lc = new LightCache(this,node);
			sceneLights.Append(1, &lc);
		}
		FindLights(node);
	}

}

void DxStdMtl2::UpdateSceneLights(bool forceUpdate)
{
	INode * rootNode  = GetCOREInterface()->GetRootNode();
	int i=0;

#ifndef FORCE_SMOKETEST
	if(!IsDxMaterialEnabled(map))
		return;
#endif

	for(i =0; i<sceneLights.Count();i++)
	{
		delete sceneLights[i];
	}

	sceneLights.SetCount(0);
	FindLights(rootNode);
}


//we use the number on the light name i.e LightPos0,LightPos1 to associate a scene light.  The parser will 
//request direction,position and light color based on the semantics

void DxStdMtl2::PatchInLightNodes()
{
	for(int i = 0; i< sceneLights.Count(); i++)
	{
		int curLightIndex = -1;

		for(int j=0; j<elementContainer.NumberofElementsByType(EffectElements::kEleLight);j++)
		{
			LightElement * le = static_cast<LightElement*>(elementContainer.GetElementByType(j, EffectElements::kEleLight));
			TSTR paramName = le->GetParameterName();

			int actualLength = paramName.first('_');

			for(int k=0; k<actualLength;k++)
			{
				if(isdigit(paramName[k]))
				{
					TSTR numChar = paramName.Substr(k,actualLength-k);
					int index = atoi(numChar.data());
					if(index == i)
					{						
						le->AddLight(sceneLights[i]->GetLightNode());

						break;
					}

				}
			}
		}
	}
}


void DxStdMtl2::ResetDXStates(bool deviceReset)
{
	for(int i=0; i<MAX_MESH_CACHE;i++)
	{
		IRenderMesh * rm = GetMeshCache()->GetActiveRenderMesh(i);
		if(rm)
			rm->Evaluate(NULL,NULL,0,false);
	}
	elementContainer.DeleteAllElements();
	if(deviceReset && pEffectParser)
	{
		pEffectParser->LoadTexture(NULL,NULL,0L,NULL,false);
	}


}

typedef struct PIXELFMTtag {
	unsigned char b,g,r,a;
}PIXELFMT; 

#define MAPSCALE3D 100.0f
#define GNORMAL_CLASS_ID	Class_ID(0x243e22c6, 0x63f6a014)
#define DIMDEFAULT 512

static void BitmapDimensions(int &width, int &height, Texmap * map)
{
//	int h=dim; w=dim;
	if(map == NULL){
		height = width = 0;
		return;
	}

	if(map->ClassID() == GNORMAL_CLASS_ID)
	{
		Texmap * bmap;
		map->GetParamBlock(0)->GetValue(2,0,bmap,FOREVER); // normal map
		BitmapDimensions(width,height,bmap);
		return;
	}
	else if(map->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
	{
		BitmapTex *pBT;
		Bitmap *pTex;
		pBT = static_cast<BitmapTex *>(map);
		pTex = pBT->GetBitmap(0);
		if (pTex)
		{
			width = getClosestPowerOf2(pTex->Width());
			height = getClosestPowerOf2(pTex->Height());
		}
		return;
	}
	else{
	}
}

void DxStdMtl2::LoadTextureData(IHLSLCodeGenerator * codeGen)
{
	Bitmap * bmap;
	BitmapInfo stBI;

	TimeValue t = GetCOREInterface()->GetTime();
	int nWidth,nHeight;

	int numberOfTextures = elementContainer.NumberofElementsByType(EffectElements::kEleTex);
	for(int i=0; i<numberOfTextures;i++)
	{
		bool bBump;
		TextureElement * texEle = static_cast<TextureElement*>(elementContainer.GetElementByType(i,EffectElements::kEleTex));

		TSTR mapType = texEle->GetMapName();
		Texmap *texmap = codeGen->GetShaderDefinedTexmap(map,mapType.data(),bBump);

		if(texmap)
		{
			BMM_Color_64 *p;
			nWidth = nHeight = DIMDEFAULT;
			BitmapDimensions(nWidth,nHeight,texmap);
			// load and create the D3D texture;
/*			if(texmap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
			{
				BitmapTex *pBT;
				Bitmap *pTex;
				pBT = (BitmapTex *)texmap;
				pTex = pBT->GetBitmap(t);
				if (pTex)
				{
					nWidth = getClosestPowerOf2(pTex->Width());
					nHeight = getClosestPowerOf2(pTex->Height());
				}

			}
*/				
			stBI.SetType(BMM_TRUE_32);
			stBI.SetWidth(nWidth);
			stBI.SetHeight(nHeight);        
			bmap = TheManager->Create(&stBI);
			texmap->RenderBitmap(t, bmap, MAPSCALE3D * 2.0f);

			if (bmap)
			{
//				LPDIRECT3DTEXTURE9 pRenderTex = texEle->GetD3DTexture();

				p = new BMM_Color_64[nWidth*nHeight];

				for (int y = 0; y < nHeight; y++)
					bmap->GetLinearPixels(0, y, nWidth, p + y * nWidth);
			
				if(texEle->pTex)
				{
					D3DSURFACE_DESC stLD;
					texEle->pTex->GetLevelDesc(0, &stLD);
					if (stLD.Width != nWidth || stLD.Height != nHeight)
					{
						SAFE_RELEASE(texEle->pTex);
					}

				}
				if(!texEle->pTex)
					pd3dDevice->CreateTexture(nWidth,nHeight, 0,D3DUSAGE_AUTOGENMIPMAP,	D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&texEle->pTex, NULL);

				if(texEle->pTex)
				{
					int i;
					PIXELFMT *pT;
					D3DLOCKED_RECT stLR;
					texEle->pTex->LockRect(0, &stLR, 0, 0);
					pT = (PIXELFMT *)stLR.pBits;

					for (i = 0; i < nWidth * nHeight; i++)
					{
						pT[i].r = p[i].r >> 8;
						pT[i].g = p[i].g >> 8;
						pT[i].b = p[i].b >> 8;
						pT[i].a = p[i].a >> 8;
					}
					texEle->pTex->UnlockRect(0);
				
					if(bBump && texmap->ClassID() != GNORMAL_CLASS_ID)
					{
//						LPDIRECT3DTEXTURE9 normalTex = texEle->GetD3DBumpTexture();
						
						if(texEle->pBumpTex)
						{
							D3DSURFACE_DESC stLD;
							texEle->pBumpTex->GetLevelDesc(0, &stLD);
							if (stLD.Width != nWidth || stLD.Height != nHeight)
							{
								SAFE_RELEASE(texEle->pBumpTex);
							}
						}
						if(!texEle->pBumpTex)
							pd3dDevice->CreateTexture(nWidth,nHeight, 0,D3DUSAGE_AUTOGENMIPMAP,	D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&texEle->pBumpTex, NULL);

						D3DXComputeNormalMap(texEle->pBumpTex,texEle->pTex,NULL, NULL, D3DX_CHANNEL_RED,30.0f);

						if(texEle->GetParamHandle())
						{
							pEffect->SetTexture(texEle->GetParamHandle(),texEle->pBumpTex);
//							D3DXSaveTextureToFile("c:\\temp\\normal_notgnormal.dds", D3DXIFF_DDS, texEle->pBumpTex, NULL);
							SAFE_RELEASE(texEle->pBumpTex);
						}
					}
					else
					{
						if(texEle->GetParamHandle())
						{
							pEffect->SetTexture(texEle->GetParamHandle(),texEle->pTex);
//							D3DXSaveTextureToFile("c:\\temp\\normal_gnormal.dds", D3DXIFF_DDS, texEle->pTex, NULL);
							SAFE_RELEASE(texEle->pTex);
						}

					}
				}
				bmap->DeleteThis();

			}
			delete p;
		}
		else
		{
			TCHAR fileName[256];
			_tcscpy(fileName,GetCOREInterface()->GetDir(APP_MAXROOT_DIR));
			_tcscat(fileName, _T("maps\\noise.dds"));
			pEffectParser->LoadTexture(pd3dDevice,pEffect,texEle->GetParamHandle(),fileName,true);
		}

	}

}

void DxStdMtl2::SetTechnique(D3DXHANDLE handle, TCHAR * techniqueName, bool bDefault)
{
	hTechnique = handle;
}

Tab<INode*> undoNodes;
static BOOL inHold = FALSE;


void DxStdMtl2::NotificationCallback(void* param, NotifyInfo* info) {

	DxStdMtl2* shader = static_cast<DxStdMtl2*>(param);
	INode * newNode  = (INode*)info->callParam;
	TimeValue t = GetCOREInterface()->GetTime();

#ifndef FORCE_SMOKETEST
	if(!IsDxMaterialEnabled(shader->GetStdMat()))
		return;
#endif
	
	if(info->intcode == NOTIFY_SCENE_ADDED_NODE || info->intcode == NOTIFY_SCENE_PRE_DELETED_NODE)
	{
		if(newNode && !newNode->IsTarget())
		{
			if(!inHold)
			{
			
				ObjectState Os   = newNode->EvalWorldState(t);

				if(Os.obj->SuperClassID() == LIGHT_CLASS_ID)
				{
					//Update Effect File
					if(info->intcode == NOTIFY_SCENE_ADDED_NODE)
					{
						shader->UpdateSceneLights();
						shader->CreateAndLoadEffectData();
					}
					else
						shader->bPreDeleteLight = true;
				}
			}
		}
	}
	if(info->intcode == NOTIFY_FILE_POST_OPEN)
	{
		if(!inHold){
			shader->UpdateSceneLights();
			shader->CreateAndLoadEffectData();
		}
	}
	if(info->intcode == NOTIFY_SCENE_POST_DELETED_NODE && shader->bPreDeleteLight){
		if(!inHold){
			shader->UpdateSceneLights();
			shader->CreateAndLoadEffectData();
			shader->bPreDeleteLight = false;
		}
	}
	if(info->intcode == NOTIFY_SCENE_PRE_UNDO || info->intcode == NOTIFY_SCENE_PRE_REDO)
	{
		inHold = TRUE;
	}
	
	// no real option for now - but we need to rebuild the effect
	if(info->intcode == NOTIFY_SCENE_POST_UNDO || info->intcode == NOTIFY_SCENE_POST_REDO)
	{
		if(shader){
			shader->UpdateSceneLights();
			shader->CreateAndLoadEffectData();
		}
		inHold = FALSE;
	}
	if(info->intcode == NOTIFY_D3D_PRE_DEVICE_RESET)
	{
		shader->ResetDXStates(true);
		if(shader->GetEffect())
		{
			shader->GetEffect()->OnLostDevice();
			shader->GetEffect()->OnResetDevice();
		}
	}
	if(info->intcode == NOTIFY_D3D_POST_DEVICE_RESET)
	{
		if(shader)
		{
			shader->UpdateSceneLights();
			shader->CreateAndLoadEffectData();
		}
	}
}

void DxStdMtl2::SetLightParam(D3DXHANDLE handle,TCHAR * name,TCHAR * uiName, bool target,MaxSemantics semantic)
{
	TSTR UIName,ParamName;
	EffectElements * EffectParam = new LightElement(target);
	ParamName.printf("%s",name);
	EffectParam->SetParameterName(ParamName);
	EffectParam->SetParamHandle(handle);
	EffectParam->SetDevice(pd3dDevice);
	elementContainer.AddElement(EffectParam);
}

void DxStdMtl2::SetTextureParam(D3DXHANDLE handle, TCHAR * name,TCHAR * uiName, TCHAR * filename, MaxSemantics semantic, bool mappingEnabled, int mappingChannel)
{
	TextureElement * EffectParam = FindTextureElement(filename);

	if(!EffectParam)
	{
		EffectParam  = new TextureElement(handle,name,filename);
		EffectParam->SetSemantic(semantic);
		EffectParam->SetParamHandle(handle);
		elementContainer.AddElement(EffectParam);
		return;
	}
	EffectParam->SetSemantic(semantic);
	EffectParam->SetParamHandle(handle);
}

void DxStdMtl2::SetTransformParam(D3DXHANDLE handle, MaxSemantics semantic)
{
	EffectElements * EffectParam = new TransformElement();
	EffectParam->SetSemantic(semantic);
	EffectParam->SetParamHandle(handle);
	EffectParam->SetDevice(pd3dDevice);
	elementContainer.AddElement(EffectParam);
}

void DxStdMtl2::SetColorParam(D3DXHANDLE handle,TCHAR * name, TCHAR * uiName,D3DXVECTOR4 color, MaxSemantics semantic)
{
	EffectElements * EffectParam = new ColorElement();
	EffectParam->SetSemantic(semantic);
	EffectParam->SetParamHandle(handle);
	EffectParam->SetDevice(pd3dDevice);
	elementContainer.AddElement(EffectParam);
}

void DxStdMtl2::SetFloatParam(D3DXHANDLE handle, TCHAR * name,TCHAR * uiName, float val,float min, float max, float step,MaxSemantics semantic )
{
	EffectElements * EffectParam = new FloatElement();
	EffectParam->SetSemantic(semantic);
	EffectParam->SetParamHandle(handle);
	EffectParam->SetDevice(pd3dDevice);
	elementContainer.AddElement(EffectParam);

}

ILightingData * DxStdMtl2::GetLightingData(D3DXHANDLE handle)
{
	int numberOfLights = elementContainer.NumberofElementsByType(EffectElements::kEleLight);
	for(int i=0; i<numberOfLights;i++)
	{
		LightElement * le = static_cast<LightElement*>(elementContainer.GetElementByType(i,EffectElements::kEleLight));
		if(le->GetParamHandle() == handle)
			return (ILightingData*)le;
	}
	return NULL;
}

TextureElement * DxStdMtl2::FindTextureElement(TCHAR * name)
{
	int numberOfTextures = elementContainer.NumberofElementsByType(EffectElements::kEleTex);
	for(int i=0; i< numberOfTextures;i++)
	{
		TextureElement * texEle = static_cast<TextureElement*>(elementContainer.GetElementByType(i,EffectElements::kEleTex));
		if(_tcscmp(texEle->GetMapName().data(), name)==0)
			return texEle;
	}
	return NULL;
}

bool DxStdMtl2::SaveEffectFile(TCHAR * fileName)
{

	if(!pd3dDevice)
		pd3dDevice = GetDevice();

	if(!pd3dDevice)
		return false;

	DWORD instSize = 96;
	bool useLPRT;
	TCHAR filename[MAX_PATH];
	TCHAR value[10] = {_T("true;")};
	_tcscpy(filename,GetCOREInterface()->GetDir(APP_PLUGCFG_DIR));
	_tcscat(filename,"\\DXDisplay.ini");

	useLPRT = GetPrivateProfileInt(_T("Settings"),_T("LPRTEnabled"),0,filename) ? true : false;

	if(!useLPRT)
		_tcscpy(value,_T("false;"));

	IHLSLCodeGenerator2::CodeVersion code = GetPixelShaderSupport(pd3dDevice,instSize);

	if(code == IHLSLCodeGenerator2::PS_1_X)
		return false;

	IHLSLCodeGenerator2 * codeGen = IHLSLCodeGenerator2::GetHLSLCodeGenerator2();
	bool bTransparency;
	TSTR path,name;

	Tab<INode*> lights;
	for(int j=0;j<sceneLights.Count();j++){
		INode * Light = sceneLights[j]->GetLightNode();
		lights.Append(1, &Light);
	}

	TCHAR * effectString = codeGen->GenerateEffectFile2(map,lights,code,bTransparency,instSize,useLPRT);

	if(!effectString)
	{
		delete codeGen;
		return false;
	}

	TSTR buf = TSTR(effectString);

	buf = buf + _T("\nbool UseLPRT = ");
	buf = buf + value;

	FILE *fp;
	SplitFilename(TSTR(fileName),&path,&name,NULL);
	fp = _tfopen(fileName,"w");
	_ftprintf(fp,"%s",buf.data());
	fclose(fp);

	TSTR hlslName = path + _T("\\") + name + _T(".hlsl");
	fp = _tfopen(hlslName.data(),"w");
	effectString = codeGen->GenerateFragmentCode2(map,lights,bTransparency);
	_ftprintf(fp,"%s",effectString);
	fclose(fp);
	delete codeGen;

// Save the textures to disk as well
	int numberOfTextures = elementContainer.NumberofElementsByType(EffectElements::kEleTex);
	for(int i=0; i<numberOfTextures;i++)
	{
		TextureElement * texEle = static_cast<TextureElement*>(elementContainer.GetElementByType(i,EffectElements::kEleTex));
		D3DXHANDLE hParam = texEle->GetParamHandle();
		TSTR bitmapFileName = path + _T("\\") + texEle->GetFileName();
		LPDIRECT3DBASETEXTURE9 pTex; 
		pEffect->GetTexture(hParam,&pTex);
		D3DXSaveTextureToFile(bitmapFileName, D3DXIFF_DDS, pTex, NULL);
		SAFE_RELEASE(pTex);
	}
	return true;
}

#define NOT_SET -1

EffectElements::EffectElements()
{

	semantic = kSemanticUnknown;
	valid = false;
	renderNode = NULL;

}

void EffectElements::SetGenericData(D3DXHANDLE handle, int uiID, int DlgInt, TCHAR * UIName, TCHAR * paramName,LPDIRECT3DDEVICE9 dev)
{
	this->hParameter = handle;
	this->UIName = TSTR(UIName);
	this->ParameterName = TSTR(paramName);
	this->pDevice = dev;
}

TransformElement::TransformElement():EffectElements()
{
	SetStyle(EffectElements::kEleTM);
}

void ElementContainer::AddElement(EffectElements * ele)
{
	elementList.Append(1,&ele);	
}


EffectElements* ElementContainer::GetElement(int index)
{
	if(index < 0 || index >=elementList.Count())
		return NULL;

	return elementList[index];
}

void ElementContainer::DeleteAllElements()
{
	int count = elementList.Count();
	for(int i =0; i< count; i++)
	{
		delete elementList[i];
	}
	elementList.SetCount(0);
}

int ElementContainer::NumberofElementsByType(int Type)
{
	int count = 0;
	for(int i =0; i< elementList.Count(); i++)
	{
		if(elementList[i]->GetStyle()==Type)
			count++;
	}
	return count;
}

EffectElements * ElementContainer::GetElementByType(int index, int Type)
{
	int count = 0;

	for(int i =0; i< elementList.Count(); i++)
	{
		if(elementList[i]->GetStyle()==Type)
		{
			if(count == index)
				return elementList[i];
			else
				count++;
		}
	}
	return NULL;
}

#include <locale.h>

LightElement::LightElement( bool target):EffectElements()
{
	SetStyle(EffectElements::kEleLight);
	this->target = target;
	lightNode = NULL;
	lightIntens = 1.0f;

	TCHAR filename[MAX_PATH];
	TCHAR str[MAX_PATH];
	_tcscpy(filename,GetCOREInterface()->GetDir(APP_PLUGCFG_DIR));
	_tcscat(filename,"\\DXDisplay.ini");
	useLPRT = GetPrivateProfileInt(_T("Settings"),_T("LPRTEnabled"),0,filename) ? true : false;

	char* saved_lc = NULL;
	char* lc = setlocale(LC_NUMERIC, NULL); // query  
	if (lc)  
		saved_lc = strdup(lc);  
	lc = setlocale(LC_NUMERIC, "C");  

	int res = GetPrivateProfileString(_T("Settings"),_T("LightMultiplier"),_T("1.0"),str,MAX_PATH,filename);
	if (res && useLPRT) 
		sscanf(str,"%f",&lightIntens);
	
	if (saved_lc)  
	{  
		lc = setlocale(LC_NUMERIC, saved_lc);  
		free(saved_lc);  
		saved_lc = NULL;  
	} 
	
}

bool LightElement::SetValue(LPD3DXEFFECT pEffect, IParamBlock2 * pblock)
{
	return true;
}

int LightElement::GetLightType()
{
	if(target)
		return kTarget;
	else
		return kPoint;
}


D3DXVECTOR4 LightElement::GetColor()
{
	LightObject * pLight;
	TimeValue t = GetCOREInterface()->GetTime();
	D3DXVECTOR4 Color4;
	BOOL inHold = theHold.RestoreOrRedoing();


	if(lightNode && !inHold){
		ObjectState Os  = lightNode->EvalWorldState(t);
		if(Os.obj && Os.obj->SuperClassID() == LIGHT_CLASS_ID) 
		{
			pLight = (LightObject *)Os.obj;
			LightState Ls;
			Interval valid;
			pLight->EvalLightState(t,valid,&Ls);
			Color c = Ls.color * (Ls.intens * lightIntens);	// fudge factor for LPRTS
			Color4 = D3DXVECTOR4(c.r, c.g, c.b, 1.0f);
		}
	}
	else{
		D3DLIGHT9 DLight;
		pDevice->GetLight(0,&DLight);
		Color4 = D3DXVECTOR4(DLight.Diffuse.r * lightIntens,DLight.Diffuse.g*lightIntens,DLight.Diffuse.b*lightIntens,1.0f);
	}
	return Color4;
}

D3DXVECTOR4 LightElement::GetDirection()
{
	Point3 Dir;
	TimeValue t = GetCOREInterface()->GetTime();
	BOOL inHold = theHold.RestoreOrRedoing();

	if(lightNode && !inHold){
		Matrix3 Mat	= lightNode->GetNodeTM(t);			
		return D3DXVECTOR4(Mat.GetRow(2));
	}
	else{
		D3DLIGHT9 DLight;
		pDevice->GetLight(0,&DLight);
		return D3DXVECTOR4(-DLight.Direction.x,-DLight.Direction.y,-DLight.Direction.z,0.0f);
	}
}

D3DXVECTOR4 LightElement::GetPosition()
{
	Point3 Pos;
	TimeValue t = GetCOREInterface()->GetTime();
	BOOL inHold = theHold.RestoreOrRedoing();

	if(lightNode && !inHold){
		Matrix3 Mat = lightNode->GetObjTMAfterWSM(t); 
		Pos	= Mat.GetTrans();
		D3DXVECTOR4 pos = D3DXVECTOR4(Pos.x,Pos.y,Pos.z,1.0f);
		return pos;
	}
	else{
		D3DLIGHT9 DLight;
		pDevice->GetLight(0,&DLight);
		return D3DXVECTOR4(DLight.Position.x,DLight.Position.y,DLight.Position.z,1.0f);
	}


}

float LightElement::GetHotSpot()
{
	LightObject * pLight;
	TimeValue t = GetCOREInterface()->GetTime();
	float hs = DegToRad(180.0f) / 2.0f;
	BOOL inHold = theHold.RestoreOrRedoing();

	if(lightNode && !inHold){
		ObjectState Os  = lightNode->EvalWorldState(t);
		if(Os.obj && Os.obj->SuperClassID() == LIGHT_CLASS_ID) 
		{
			pLight = (LightObject *)Os.obj;
			LightState Ls;
			Interval valid;
			pLight->EvalLightState(t,valid,&Ls);
			hs = DegToRad(Ls.hotsize) / 2.0;
		}
	}

	return hs;
}

float LightElement::GetFallOff()
{
	LightObject * pLight;
	TimeValue t = GetCOREInterface()->GetTime();
	float hs = DegToRad(180.0f) / 2.0f;
	BOOL inHold = theHold.RestoreOrRedoing();

	if(lightNode && !inHold){
		ObjectState Os  = lightNode->EvalWorldState(t);
		if(Os.obj && Os.obj->SuperClassID() == LIGHT_CLASS_ID) 
		{
			pLight = (LightObject *)Os.obj;
			LightState Ls;
			Interval valid;
			pLight->EvalLightState(t,valid,&Ls);
			hs = DegToRad(Ls.fallsize) / 2.0;
		}
	}
	return hs;
}

TextureElement::TextureElement(D3DXHANDLE handle, TCHAR * mapName,TCHAR * filename)
{
	this->texHandle = handle;
	this->mapName = mapName;
	this->fileName = filename;
	pTex = NULL;
	pBumpTex = NULL;
	SetStyle(EffectElements::kEleTex);
}

TextureElement::~TextureElement()
{
	SAFE_RELEASE(pTex);
	SAFE_RELEASE(pBumpTex);
}

bool TextureElement::SetValue(LPD3DXEFFECT pEffect, IParamBlock2 * pblock){

	return true;
}

D3DCOLORVALUE GetD3DColor (Color c)
{
	D3DCOLORVALUE dc;
	dc.r = c.r;
	dc.g = c.g;
	dc.b = c.b;
	dc.a = 1.0f;
	return dc;
}

bool ColorElement::SetValue(LPD3DXEFFECT pEffect, StdMat2 * map)
{
	MaxSemantics semantic = GetSemantic();
	TimeValue t = GetCOREInterface()->GetTime();
	Color col;
	D3DCOLORVALUE effectColor;
	switch (semantic)
	{
	case kSemanticDiffuse:
		col = map->GetDiffuse(t);
		effectColor = GetD3DColor(col);
		pEffect->SetValue(GetParamHandle(), (void*)&effectColor, sizeof(D3DCOLORVALUE));
		break;
	case kSemanticAmbient:
		col = map->GetAmbient(t);
		effectColor = GetD3DColor(col);
		pEffect->SetValue(GetParamHandle(), (void*)&effectColor, sizeof(D3DCOLORVALUE));
		break;
	
	case kSemanticSpecular:
		col = map->GetSpecular(t);
		effectColor = GetD3DColor(col);
		pEffect->SetValue(GetParamHandle(), (void*)&effectColor, sizeof(D3DCOLORVALUE));
		break;
	case kSemanticEmissive:

		if(map->GetSelfIllumColorOn())
			col = map->GetSelfIllumColor(t);
		else
			col.r = col.g = col.b = map->GetSelfIllum(t);

		effectColor = GetD3DColor(col);
		pEffect->SetValue(GetParamHandle(), (void*)&effectColor, sizeof(D3DCOLORVALUE));
		break;

	}
	return true;
}

bool FloatElement::SetValue(LPD3DXEFFECT pEffect, StdMat2 * map)
{
	MaxSemantics semantic = GetSemantic();
	TimeValue t = GetCOREInterface()->GetTime();
	float f;
	switch (semantic)
	{

	case kSemanticSpecularLevel:
		f = map->GetShinStr(t);
		pEffect->SetValue(GetParamHandle(), (void*)&f, sizeof(float));
		break;
	case kSemanticGlossLevel:
		f = map->GetShininess(t);
		f = f*100.0f;
		pEffect->SetValue(GetParamHandle(), (void*)&f, sizeof(float));
		break;
	case kSemanticOpacity:
		f = map->GetOpacity(t);
		pEffect->SetValue(GetParamHandle(), (void*)&f, sizeof(float));
	}
	return true;

}

LightCache::LightCache(DxStdMtl2 * dxMtl, INode * lightNode)
{
	node = lightNode;
	shader = dxMtl;
	lightParamBlock = NULL;
	
	ObjectState os = lightNode->EvalWorldState(GetCOREInterface()->GetTime());
	IParamBlock * pblock = (IParamBlock*)(os.obj)->GetReference(0);
//	ReplaceReference(0,pblock);

}

LightCache::~LightCache()
{
//	DeleteAllRefsFromMe();
}

void LightCache::SetReference(int i, RefTargetHandle rtarg)
{
	lightParamBlock = (IParamBlock*) rtarg;
//	node = (INode*) rtarg;
}

RefTargetHandle LightCache::GetReference(int i)
{
	return (RefTargetHandle)lightParamBlock;
}

int LightCache::NumRefs()
{
	return 1;
}

RefResult LightCache::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message )
{
	bool supported = false;

	switch(message)
	{
	case REFMSG_CHANGE:
		if(IsDxMaterialEnabled(shader->GetStdMat())&& hTarget->ClassID() == Class_ID(PARAMETER_BLOCK_CLASS_ID,0))
		{
			if(CheckParamBlock((IParamBlock*)hTarget))
				return REF_SUCCEED;

/*			for(int i=0; i< hTarget->NumParamBlocks();i++)
			{
				IParamBlock2 * pblock = hTarget->GetParamBlock(i);
				if(CheckParamBlock(pblock))
					return;
			}
*/
			if(!supported)
				shader->CreateAndLoadEffectData();
		}
		break;
	}
	return REF_SUCCEED; 
}

EffectElements::~EffectElements()
{
}

#endif // NO_ASHLI