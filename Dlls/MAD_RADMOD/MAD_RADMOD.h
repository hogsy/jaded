/**********************************************************************
 *<
	FILE: MAD_FaceMAP.h

	DESCRIPTION:	USINE A GAZ 

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/

#ifndef __MAD_MADRADMOD__H
#define __MAD_MADRADMOD__H


extern TCHAR *GetString(int id);

#define MAD_RadMod_CLASS_ID	Class_ID(0x3470c0de, 0x477fc0de)

class MAD_RadMod : public Modifier {
	public:
		// Parameter block
		static IObjParam *ip;			//Access to the interface
		static HWND hParams1;
		Mesh	*RadiosityObject;
		Point3	*P3_RadiosityObjectSecondRliField;
		ULONG *UL_RadiosityObjectSecondRliField;
		Point3	*RLI; /* Used for original object */
		ULONG *OriginalsRLI;
		int		UseRo;
		int		UseRLI;
		ULONG InstanciedObjectNumberOfPoints;
		ULONG RadiosityObjectNumberOfPoints;
		float MIXING;
		
		//From Modifier
		TCHAR *GetObjectName() { return "MAD_RadMod"; }
		ChannelMask ChannelsUsed()  { return PART_GEOM | PART_TOPO | PART_SELECT | PART_TEXMAP | PART_VERTCOLOR; }
		ChannelMask ChannelsChanged() { return PART_GEOM | PART_TOPO | PART_SELECT | PART_TEXMAP | PART_VERTCOLOR; }
		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		Class_ID InputType() {return defObjectClassID;}
		Interval LocalValidity(TimeValue t);
		BOOL ChangeTopology() {return FALSE;}
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;}
		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);
		Interval GetValidity(TimeValue t);
		BOOL HasUVW();
		void	SetGenUVW(BOOL sw) ;
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);
		int NumRefs() { return 1; }
		int NumSubs() { return 1; }
		Class_ID ClassID() {return MAD_RadMod_CLASS_ID;}
		SClass_ID SuperClassID() { return OSM_CLASS_ID;}
		void GetClassName(TSTR& s) {s = "MAD RAdNMod";}
		RefTargetHandle Clone( RemapDir &remap );
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID,  RefMessage message);
		int	NumParamBlocks() { return 0; }					// return number of ParamBlocks in this instance
		void DeleteThis() { delete this; }		
		MAD_RadMod();
		~MAD_RadMod();		
		/* Following is used with ImportExport MAD*/
		void SetRLi(Point3 *RLIs);
		Point3 *GetRLi();
		void SetRadObj(Mesh *RLIs);
		Mesh *GetRadObj();
//		Point3 *GetRLi();
};

#endif // __MAD_FACEMAP__H
