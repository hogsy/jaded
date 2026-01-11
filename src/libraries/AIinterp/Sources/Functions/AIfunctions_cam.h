// Defines
#define	CAM_OBJ_MAX		40
#define	CAM_COB_MAX		20

// Options for AI_EvalFunc_CamCastInitSpheres_C
#define CAM_INITSCAST_OPTIONS_NONE				0x00000000000000000000000000000000b

// Options for AI_EvalFunc_CamCastSphere_C 
#define CAM_SCAST_OPTIONS_NONE					0x00000000000000000000000000000000b
#define CAM_SCAST_OPTIONS_USE_SPECIFICCROSSABLE	0x00000000000000000000000000000001b

// Options for  AI_EvalFunc_CamRayObjectVector_C
//#define CAM_RCAST_OPTIONS_NONE					0x00000000000000000000000000000000b
//#define CAM_RCAST_OPTIONS_USE_SPECIFICCROSSABLE	0x00000000000000000000000000000001b

// Declaration of some extern var
extern USHORT COL_guw_UserCrossable;


// Declaration of some extern functions hidden in a .cpp with no .h !
extern void AI_EvalFunc_DbgTraceInt_C(int _i_Val);
extern void AI_EvalFunc_DbgTraceEOL_C(void);
extern void AI_EvalFunc_DbgTraceFloat_C(float _f_Val);
extern void AI_EvalFunc_DbgTraceVector_C(MATH_tdst_Vector *_pst_Vec);
extern void AI_EvalFunc_DbgTraceString_C(char *_psz_Str);
extern void AI_EvalFunc_DbgTraceObject_C(OBJ_tdst_GameObject *_pst_GO);

// My functions
_inline_	BOOL	AI_CamCastTestCrossable(COL_tdst_GameMat	* _pst_Cob_ElementGMat, INT _i_Options);
_inline_	BOOL	AI_CamCastProcessObject(WOR_tdst_World	* _pst_World, OBJ_tdst_GameObject * _pst_CurrentObject,INT _i_Options);
			BOOL	AI_CamCastProcessAllObjects(WOR_tdst_World	* _pst_World,INT _i_Options);
			BOOL	AI_CamCastProcessOverlappingObjects(WOR_tdst_World	* _pst_World, OBJ_tdst_GameObject * _pst_GO,INT _i_Options);

