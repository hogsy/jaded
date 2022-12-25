/**********************************************************************
 *<
	FILE: MAD_TG.h

	DESCRIPTION:	Template Utility

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/

#ifndef __MAD_TG__H
#define __MAD_TG__H
#include "BASe/BAStypes.h"


#include "Max.h"
#include "resource.h"
#include "istdplug.h"
/*#include "iparamb2.h"
#include "iparamm2.h"*/

#include "meshadj.h"
#include "iparamm.h"

#define BoxSizeXY 100.0f
#define BoxSizeZ 10.0f



extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

#define MAD_TG_CLASS_ID		Class_ID(0x740ebff5, 0x4a0b2c7f)
#define MAD_TGM_CLASS_ID	Class_ID(0x741eCff0, 0x4a0bCc7c)


void AddMesh(Mesh *Mdst , Mesh *M2 );
void FlipMesh(Mesh *Mdst );
/**********************************************************************
 Geometric Texture creator;
 **********************************************************************/
class MAD_TGM : public Modifier {	
	public:
		INode *node;
		static IObjParam *ip;
		static HWND hParams;
		Matrix3 tm, invtm;
		Mesh		*M0 , *M1 , *M2 ,*M3 , *MA , *MB ;
		TriObject   *O[6];

		MAD_TGM();
		~MAD_TGM();

		// From Animatable
		void DeleteThis() {delete this;}
		void GetClassName(TSTR& s) { s= "MAD_TGM"; }  ;
		virtual Class_ID ClassID() { return MAD_TGM_CLASS_ID;}
		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip,ULONG flags,Animatable *next);		
		TCHAR *GetObjectName() {return "MAD_TGM";}
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;} 

		ChannelMask ChannelsUsed()		{return	OBJ_CHANNELS;};
		ChannelMask ChannelsChanged()	{return	PART_TOPO | PART_GEOM | PART_TEXMAP | PART_MTL | PART_SELECT | PART_SUBSEL_TYPE | PART_DISPLAY | PART_VERTCOLOR | PART_GFX_DATA	;};
		Class_ID InputType() {return defObjectClassID;}
		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		Interval LocalValidity(TimeValue t);

		int NumRefs() {return 1;}
		RefTargetHandle GetReference(int i) {return node;}
		void SetReference(int i, RefTargetHandle rtarg) {node=(INode*)rtarg;}
		
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);
				
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message);

		void ClipMesh(Mesh *Msrc , Mesh *M1 , Mesh *M2,BOOL CutY,float Value);
		Mesh *GetTextureMesh(ULONG Num);
};
/**********************************************************************
 Geometric Texture applicator
 **********************************************************************/

class MAD_TG : public Modifier {	
	public:
	    static IParamMap    *pmapParam; /* Handle to the parammap Parameter block */
	    IParamBlock         *pblock;    /* Ref 0 */
		static IObjParam	*ip;
		static HWND			hParams;
		INode				*node;
		ULONG		CurrenID;
		MAD_TGM				*clTheGeomText;
		Mesh				*M1;

		MAD_TG();
		~MAD_TG();

    /*
     ===============================================================================================
        From Animatable
     ===============================================================================================
     */
    void    DeleteThis(void) { delete this; }



    RefTargetHandle Clone(RemapDir &remap = NoRemap());

    IOResult    Load(ILoad *iload);

    /*
     ===============================================================================================
        From Modifier TODO: Add the channels that the modifier needs to perform its modification
     ===============================================================================================
     */
    ChannelMask ChannelsUsed(void)
    {
        return PART_GEOM | PART_TOPO | PART_SELECT | PART_SUBSEL_TYPE | PART_TEXMAP |
            PART_VERTCOLOR;
    }

    /*
     ===============================================================================================
        TODO: Add the channels that the modifier actually modifies
     ===============================================================================================
     */
    ChannelMask ChannelsChanged(void)
    {
        return PART_GEOM | PART_TOPO | PART_SELECT | PART_TEXMAP | PART_VERTCOLOR;
    }

    void        ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);

    /*
     ===============================================================================================
        TODO: Return the ClassID of the object that the modifier can modify
     ===============================================================================================
     */
    Class_ID    InputType(void) { return triObjectClassID; }

    Interval    LocalValidity(TimeValue t);

    /*
     ===============================================================================================
        From BaseObject TODO: Return true if the modifier changes topology
     ===============================================================================================
     */
    BOOL        ChangeTopology(void) { return TRUE; }

    int GetParamBlockIndex(int id) { return id; }

    /*
     ===============================================================================================
        From ReferenceMaker TODO::Implement the following methods for references
     ===============================================================================================
     */
    int NumRefs(void) { return 1; }

    RefTargetHandle GetReference(int i) { return pblock; }

    void    SetReference(int i, RefTargetHandle rtarg) { pblock = (IParamBlock *) rtarg; }

    int NumSubs(void) { return 1; }

    Animatable  *SubAnim(int i) { return pblock; }

    TSTR    SubAnimName(int i) { return GetString(IDS_PARAMS); }

    /*
     ===============================================================================================
        TODO: Return NULL if you do no want to make the paramblock available
     ===============================================================================================
     */
    IParamArray *GetParamBlock(void) { return pblock; }

    RefResult           NotifyRefChanged
                        (
                            Interval        changeInt,
                            RefTargetHandle hTarget,
                            PartID          &partID,
                            RefMessage      message
                        );

    CreateMouseCallBack *GetCreateMouseCallBack(void) { return NULL; }

    void            BeginEditParams(IObjParam *ip, ULONG flags, Animatable *prev);
    void            EndEditParams(IObjParam *ip, ULONG flags, Animatable *next);

    Interval        GetValidity(TimeValue t);
    ParamDimension  *GetParameterDim(int pbIndex);
    TSTR            GetParameterName(int pbIndex);

	BOOL			ChmpfrClbk(TimeValue t, IParamMap *map, HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam) ;
	/*
     ===============================================================================================
        Automatic texture support
     ===============================================================================================
     */
    BOOL            HasUVW(void);
    void            SetGenUVW(BOOL sw);

		IOResult Save(ISave *isave);
		// From Animatable
		void GetClassName(TSTR& s) { s= "MAD_TG"; }  ;
		virtual Class_ID ClassID() { return MAD_TG_CLASS_ID;}
		TCHAR *GetObjectName() {return "MAD_TG";}


	};

#endif // __MAD_TG__H
