#include "iparamm2.h"

#pragma once

class MixMat;

//dialog stuff to get the Set Ref button
class MixMatDlgProc : public ParamMap2UserDlgProc {
//public ParamMapUserDlgProc {
	public:
		MixMat *mixmat;		
		BOOL valid;
		HWND hPanel; 
		MixMatDlgProc(MixMat *m);	
		BOOL DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);		
		void DeleteThis();
		void SetThing(ReferenceTarget *m);

	};

class MixMat : public Mtl, public IReshading  {	
	public:
		IParamBlock2 *pblock; 	// ref #0
		Mtl *sub1, *sub2;		// ref #1, 2		
		Texmap *map;			// ref #3
		BOOL mapOn[3];
		float u, crvA, crvB;
		BOOL useCurve;		
		Interval ivalid;
		ReshadeRequirements mReshadeRQ; // mjm - 06.02.00
		BOOL Param1;

		static MixMatDlgProc *paramDlg;
		// [attilas|29.5.2000]
		// min and max Mix tresholds
		static const float minMix;
		static const float maxMix;

		MixMat(BOOL loading);
		void NotifyChanged();
		Mtl *UseMtl();
		float mixCurve(float x);

		// From MtlBase and Mtl
		void SetAmbient(Color c, TimeValue t) {}		
		void SetDiffuse(Color c, TimeValue t) {}		
		void SetSpecular(Color c, TimeValue t) {}
		void SetShininess(float v, TimeValue t) {}				
		
		Color GetAmbient(int mtlNum=0, BOOL backFace=FALSE);
	    Color GetDiffuse(int mtlNum=0, BOOL backFace=FALSE);
		Color GetSpecular(int mtlNum=0, BOOL backFace=FALSE);
		float GetXParency(int mtlNum=0, BOOL backFace=FALSE);
		float GetShininess(int mtlNum=0, BOOL backFace=FALSE);		
		float GetShinStr(int mtlNum=0, BOOL backFace=FALSE);
		float WireSize(int mtlNum=0, BOOL backFace=FALSE);
				
		ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
		void EnableStuff();
		
		void Shade(ShadeContext& sc);
		float EvalDisplacement(ShadeContext& sc); 
		Interval DisplacementValidity(TimeValue t); 
		void Update(TimeValue t, Interval& valid);
		void Init();
		void Reset();
		Interval Validity(TimeValue t);
		
		Class_ID ClassID();
		SClass_ID SuperClassID();
		void GetClassName(TSTR& s);  

		void DeleteThis();	

		Sampler*  GetPixelSampler(int mtlNum, BOOL backFace );

	// Methods to access sub-materials of meta-materials
	   	int NumSubMtls();
		Mtl* GetSubMtl(int i);
		void SetSubMtl(int i, Mtl *m);
		int VPDisplaySubMtl();
		TSTR GetSubMtlSlotName(int i);

		// Methods to access sub texture maps of material or texmap
		int NumSubTexmaps();
		Texmap* GetSubTexmap(int i);
		void SetSubTexmap(int i, Texmap *m);
		TSTR GetSubTexmapSlotName(int i);

		int NumSubs(); 
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);
		int SubNumToRefNum(int subNum);

		// From ref
 		int NumRefs();
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);

		RefTargetHandle Clone(RemapDir &remap = NoRemap());
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message);

		// IO
		IOResult Save(ISave *isave); 
		IOResult Load(ILoad *iload); 

// JBW: direct ParamBlock access is added
		int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
		IParamBlock2* GetParamBlock(int i); // return i'th ParamBlock
		IParamBlock2* GetParamBlockByID(BlockID id); // return id'd ParamBlock
		BOOL SetDlgThing(ParamDlg* dlg);

// begin - ke/mjm - 03.16.00 - merge reshading code
		BOOL SupportsRenderElements();
//		BOOL SupportsReShading(ShadeContext& sc);
		ReshadeRequirements GetReshadeRequirements(); // mjm - 06.02.00
		void PreShade(ShadeContext& sc, IReshadeFragment* pFrag);
		void PostShade(ShadeContext& sc, IReshadeFragment* pFrag, int& nextTexIndex, IllumParams* ip);
// end - ke/mjm - 03.16.00 - merge reshading code

		// From Mtl
		bool IsOutputConst( ShadeContext& sc, int stdID );
		bool EvalColorStdChannel( ShadeContext& sc, int stdID, Color& outClr );
		bool EvalMonoStdChannel( ShadeContext& sc, int stdID, float& outVal );

		void* GetInterface(ULONG id);
	};

class MixMatClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic();
	void *			Create(BOOL loading);
	const TCHAR *	ClassName();
	SClass_ID		SuperClassID();
	Class_ID 		ClassID();
	const TCHAR* 	Category();
// JBW: new descriptor data accessors added.  Note that the 
//      internal name is hardwired since it must not be localized.
	const TCHAR*	InternalName();	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance();			// returns owning module handle

	};

class NameData
{
public:
	int channel,subid;
};

//--------------------------------------------------------------
// VCol: A Composite texture map
//--------------------------------------------------------------
class VCol: public Texmap { 

	Interval ivalid;
	BOOL useUVW;
	public:
		VCol();
		ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
		ULONG Requirements(int subMtlNum);
		void LocalMappingsRequired(int subMtlNum, BitArray & mapreq, BitArray &bumpreq);			
		void Update(TimeValue t, Interval& valid);
		void Reset();
		Interval Validity(TimeValue t);
		void NotifyChanged();

		// Evaluate the color of map for the context.
		AColor EvalColor(ShadeContext& sc);
		float EvalMono(ShadeContext& sc);
		AColor EvalFunction(ShadeContext& sc, float u, float v, float du, float dv);

		// For Bump mapping, need a perturbation to apply to a normal.
		// Leave it up to the Texmap to determine how to do this.
		Point3 EvalNormalPerturb(ShadeContext& sc);

		Class_ID ClassID();
		SClass_ID SuperClassID();
		void GetClassName(TSTR& s);  
		void DeleteThis();	

//pblock2
		int NumSubs();  

		// From ref
 		int NumRefs();

// JBW: direct ParamBlock access is added
		IParamBlock2 *pblock;   // ref #1
		int	NumParamBlocks();					// return number of ParamBlocks in this instance
		IParamBlock2* GetParamBlock(int i); // return i'th ParamBlock
		IParamBlock2* GetParamBlockByID(BlockID id); // return id'd ParamBlock
		BOOL SetDlgThing(ParamDlg* dlg);

		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);
		Animatable* SubAnim(int i);


		RefTargetHandle Clone(RemapDir &remap = NoRemap());
		RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message );

		// IO
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);

		bool IsLocalOutputMeaningful( ShadeContext& sc );

		int mapID;
		int subID;
		HWND hWnd;
		Tab<NameData> channelData;
		void FilloutNames(HWND hWnd);
	};