#ifndef __IMPORTTRL__H
#define __IMPORTTRL__H

#include "Warning.h"
#include "resource.h"
#include "JadeBonesGroupUtility/JadeBonesGroup.h"
#include "JadeBonesGroupUtility/JadeBonesHitByNameDlgCallback.h"
#include "NodeInfo.h"
#include "BankObject.h"
#include <vector>

// From anim
#define ANI_C_MagicBoxBone		0		// bone index for magic box
#define ANI_Cuc_ThisIsAMainBone	255

#define C_az_MagicBoxName		"Magic Box"
#define C_TRL_uw_MagicBox		USHORT(-1)

//    Types of JADE events
#define EVE_C_EventFlag_Empty							0x0000
#define EVE_C_EventFlag_Symetric						0x0020
#define EVE_C_EventFlag_AIFunction						0x0040
#define EVE_C_EventFlag_InterpolationKey				0x0080
#define EVE_C_EventFlag_Type							0x07C0
#define EVE_InterKeyType_Rotation_Quaternion			0x0010

#define TICK_PER_FRAME			80
#define CONVERTION_FACTOR		100
#define MAXSELBONEOPT			50

//#define LOG_DEBUG				1
//#define OUTPUT_TRL_ONLY			1
#define AUTO_SKIP_TRANS			1	// Always skip translations on non main bones

struct TRL_Event;
struct TRL_Track;

struct SLinkObjects
{
	std::vector<CNodeInfo>  nodes;
	std::vector<CBankObject>    objects;
	TCHAR	szBankName[MAX_PATH];

	SLinkObjects() {szBankName[0] = _T('\0');}
};

// Parameters for import
//     Most of them are not used yet. Maybe later...
struct SAnimParams
{
	TimeValue   beginTime;      // time of first frame
	TimeValue   endTime;        // time of last	frame
	long        numFrames;      // number of frames in animation
	float       optimRotEps;    // epsilon to reduce rotation keys
	float       optimTraEps;    // epsilon to reduce translation keys
	float       optimFovEps;    // epsilon to reduce FOV keys
	bool        bIsInitLocalPosValid;   // not an option, but it determines if we can remove useless translation tracks or not
	bool        bRemoveUselessTranslationTracks;    // remove or not the useless translation tracks
	bool        bDoMysticalOptimisation;            // perform mystical optimisation
	bool        bDiscardMagicBoxAnims;              // discard magic box animation data
	bool        bImportNonAnimatables;              // import non animatable bones
	bool        bPerformSelBoneOpt;                 // perform selective bone optimization
	float       optimSelBoneRotEps;                 // epsilon to reduce rotation keys
	float       optimSelBoneTraEps;                 // epsilon to reduce translation keys
	bool        bPerformCamOpt;                     // perform camera optimization
	float       optimCamRotEps;    // epsilon to reduce rotation keys
	float       optimCamTraEps;    // epsilon to reduce translation keys
	float       optimCamFovEps;    // epsilon to reduce FOV keys

	int         iSelBoneOptCount;
	int         iSelBoneOptArray[MAXSELBONEOPT];	

	TimeValue   tvDelta;        // = (endTime - beginTime) / (numFrames-1)

	SAnimParams() : beginTime( 0 ), endTime( 0 ), numFrames( 0 ),
		optimRotEps(1e-4f), optimTraEps(1e-3f), optimFovEps(1e-2f),
		bIsInitLocalPosValid(false),
		bRemoveUselessTranslationTracks(true),
		bDoMysticalOptimisation(true),
		bDiscardMagicBoxAnims(false),
		bImportNonAnimatables(false),
		bPerformSelBoneOpt(false),
		optimSelBoneRotEps(1e-4f), optimSelBoneTraEps(1e-3f),
		bPerformCamOpt(false),
		optimCamRotEps(0.5f), optimCamTraEps(0.25f), optimCamFovEps(0.0f),
		iSelBoneOptCount(0)
	{}
};

// struct to define an AI node - to export FOV information
typedef struct	AI_tdst_Node_
{
	LONG	l_Param;
	short	w_Param;
	char	c_Flags;
	char	c_Type;
} AI_tdst_Node;

DWORD WINAPI fnimp(LPVOID arg) {
	return(0);
}

/* 
Definition of TRL Import class file. 
This class is child of a 3DSMax class used for importation. 
*/
class CImportTRL : public SceneImport
{
public:
	// Destuctor
	virtual ~CImportTRL();

	// This class is a singleton
    static CImportTRL& Instance();

	// from SceneImport (3DSMax)
	virtual int	ExtCount();
	virtual const TCHAR *Ext(int i);
	virtual const TCHAR *LongDesc();
	virtual const TCHAR *ShortDesc();
	virtual const TCHAR *AuthorName();
	virtual const TCHAR	*CopyrightMessage();
	virtual const TCHAR	*OtherMessage1();
	virtual const TCHAR	*OtherMessage2();
	virtual unsigned int Version();
	virtual void ShowAbout(HWND hWnd);

	// Main import function (Call when file is selected)
	virtual int DoImport(const TCHAR *name, ImpInterface *ii,Interface *i, BOOL suppressPrompts=FALSE);	

	// Nodes
	int NumNodes() const {return _linkObjects.nodes.size();}

	// Actor name, actor directory and anim name
	inline const TCHAR* GetActorName() const {return _actorName;}
	inline const TCHAR* GetActorDir() const {return _actorDir;}
	inline const TCHAR* GetAnimName() const {return _animName;}

	// Anim params
	inline TimeValue GetBeginTime() const {return _animParams.beginTime;}
	inline TimeValue GetEndTime() const {return _animParams.endTime;}
	inline float GetOptimRotEps() const {return _animParams.optimRotEps;}
	inline float GetOptimTraEps() const {return _animParams.optimTraEps;}
	inline float GetOptimFovEps() const {return _animParams.optimFovEps;}
	inline bool GetRemoveUselessTranslationTracks() const {return _animParams.bIsInitLocalPosValid && _animParams.bRemoveUselessTranslationTracks;}
	inline bool GetDoMysticalOptimisation() const {return _animParams.bDoMysticalOptimisation;}
	inline bool GetDiscardMagicBoxAnims() const {return _animParams.bDiscardMagicBoxAnims;}
	inline bool GetbImportNonAnimatableBones() const {return _animParams.bImportNonAnimatables;}
	inline bool GetPeformCamOptim() const {return _animParams.bPerformCamOpt;}
	inline float GetCamOptimRotEps() const {return _animParams.optimCamRotEps;}
	inline float GetCamOptimTraEps() const {return _animParams.optimCamTraEps;}
	inline float GetCamOptimFovEps() const {return _animParams.optimCamFovEps;}
	inline float GetSelBoneRotEps() const {return _animParams.optimSelBoneRotEps;}
	inline float GetSelBoneTraEps() const {return _animParams.optimSelBoneTraEps;}
	inline const SAnimParams& GetAnimPrams()  { return _animParams;}

protected:
	// Constructor (Singleton)
	CImportTRL();

private:
	static BOOL CALLBACK LogDlgProc( HWND, UINT, WPARAM, LPARAM );
	bool GetNodes( bool );
	void SaveLog( BOOL ) const;
	void GetFileNameAndDirectory (const TCHAR*);
	BOOL SetTRLToMAX( const TCHAR* );
	int TrackRead(FILE*);
	int EventRead(FILE*, int, int, int*);

	// Unique instance
	static	CImportTRL* _instance;
	mutable	CWarning _log;

	Interface* _ip;
	TCHAR   _actorName[MAX_PATH];   // actor name
	TCHAR   _actorDir[MAX_PATH];    // actor directory
	TCHAR   _animName[MAX_PATH]; 
    
	SAnimParams     _animParams;
	SLinkObjects    _linkObjects;	// For Boning

	INode*			_MagicBoxNode;

	int				_MaxFrame;
	int				_MaxNode;

	int				mb_SkipTranslations;
};

extern const TCHAR* C_szVersionString;

// Generated ClassID by gencid.exe (3DSMax SDK utility)
// Attention : celui-ci est le même que l'exporteur!!! A CHANGER!!! (Je n'ai pas 
// gencid.exe sur ma machine...)
// const Class_ID ubiImportTrlClassID(0x14e77c5, 0x334a53b2);
const Class_ID ubiImportTrlClassID(0x14e77c6, 0x334a53b3);

// DLL creation (Used by DLL Entry for DLL initialisation)
class CImportTRLClassDesc:public ClassDesc2
{
public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) {return &CImportTRL::Instance();}
	const TCHAR *	ClassName() {return GetString(IDS_IMPORT_TRL_CLASS_NAME);}
	SClass_ID		SuperClassID() {return SCENE_IMPORT_CLASS_ID;}
	Class_ID		ClassID() {return ubiImportTrlClassID;}
	const TCHAR* 	Category() {return GetString(IDS_CATEGORY);}
	const TCHAR*	InternalName() { return _T("ImportTRL"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle
};

static CImportTRLClassDesc gImportTRLDesc;

#endif