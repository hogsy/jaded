/*$T AI2C_fctheader.h GC! 1.100 03/21/01 10:59:17 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef AI2C_FCTHEADER
#define AI2C_FCTHEADER

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AImsg.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJmain.h"


/******************************************************************/
typedef OBJ_tdst_GameObject *object;
typedef AI_tdst_Function	*function;
typedef WAY_tdst_Network	*network;
typedef char               *string;
#define color		int
#define private
#define procedure
#define procedure_trigger
#define procedure_local
#define procedure_ultra
#define RETURN		return
#define nobody		NULL
#define nofunc		NULL
#define nonet		NULL
#define key			int
#define returntrack	return AI_CR_StopReturnTrack;
#define stop		return AI_CR_StopInstance;
#define meta		while
#define save
#define saveal
#define reinit
#define optim
#ifdef PSX2_TARGET
#define bool	int
#endif
/******************************************************************/
#define AI_EvalFunc_OBJMe_C()		AI_gpst_CurrentGameObject
#define AI_EvalFunc_TIMEGetDt_C()	TIM_gf_dt
/******************************************************************/

typedef MATH_tdst_Vector *PVEC;
typedef MATH_tdst_Vector VEC;
class vector
{
public:
	vector(void) {}
	vector(const float _x, const float _y, const float _z) { x = _x; y = _y; z = _z; }
	vector(MATH_tdst_Vector *v) { x = v->x; y = v->y; z = v->z; }
	vector(MATH_tdst_Vector v) { x = v.x; y = v.y; z = v.z; }
	operator PVEC() const { return (MATH_tdst_Vector *) this; }
	operator VEC() const { MATH_tdst_Vector v; v.x=x; v.y=y; v.z=z; return v; }
	vector operator+(const vector &vec2) const { return vector(x + vec2.x, y + vec2.y, z + vec2.z); }
	vector operator-(const vector &vec2) const { return vector(x - vec2.x, y - vec2.y, z - vec2.z); }
	void operator+=(const vector &vec2) { x += vec2.x; y += vec2.y; z += vec2.z; }
	void operator-=(const vector &vec2) { x -= vec2.x; y -= vec2.y; z -= vec2.z; }
	void operator/=(const float f) { x /= f; y /= f; z /= f; }
	void operator*=(const float f) { x *= f; y *= f; z *= f; }
	BOOL operator!=(const vector& v) { return ((v.x != x) || (v.y != y) || (v.z != z));  }
	BOOL operator==(const vector& v) { return ((v.x == x) && (v.y == y) && (v.z == z));  }
	vector operator-(void) const { return vector(-x, -y, -z); }
	friend vector operator*(const float f, const vector &v) { return vector(v.x * f, v.y * f, v.z * f); }
	friend vector operator*(const vector &v, const float f) { return vector(v.x * f, v.y * f, v.z * f); }
	friend vector operator/(const float f, const vector &v) { return vector(v.x / f, v.y / f, v.z / f); }
	friend vector operator/(const vector &v, const float f) { return vector(v.x / f, v.y / f, v.z / f); }

	float x, y, z;
};

#define vect_h vector

/******************************************************************/
typedef AI_tdst_Message *PPMSG;
class message
{
public:
	message(void) {}
	operator PPMSG() const { return (AI_tdst_Message *) this; }

    object	msg_sender;
    object	msg_gao1;
    object	msg_gao2;
    object	msg_gao3;
    object	msg_gao4;
    object	msg_gao5;
    vector	msg_vec1;
    vector	msg_vec2;
    vector	msg_vec3;
    vector	msg_vec4;
    vector	msg_vec5;
    int		msg_int1;
    int		msg_int2;
    int		msg_int3;
    int		msg_int4;
    int		msg_int5;
    int		msg_id;
};

/******************************************************************/
typedef AI_tdst_GlobalMessageId *PMSGID;
class messageid
{
public:
	messageid(void) {}
	operator PMSGID() const { return (AI_tdst_GlobalMessageId *) this; }
	int						msgid;
	int						id;
};

/******************************************************************/
typedef TEXT_tdst_Eval *PTEXT;
typedef char*  PCHAR;
class text
{
public:
	text(void) {}
	operator PTEXT() const { return (TEXT_tdst_Eval *) this; }
	operator PCHAR() const { return AI_EvalFunc_TEXTToString_C((TEXT_tdst_Eval*)(this)); }

	int i_FileKey;
	int i_Id;
};

/******************************************************************/
typedef int (*AI2C_tdfn_TriggerFunc) (message &);
typedef struct	AI2C_fctdeftrigger_
{
	ULONG					ul_File;
	ULONG					ul_Index;
#ifdef GAMECUBE_USE_AI2C_DLL
	AI2C_tdfn_TriggerFunc	*ppst_Func;
#else // GAMECUBE_USE_AI2C_DLL
	AI2C_tdfn_TriggerFunc	pst_Func;
#endif // GAMECUBE_USE_AI2C_DLL
	char					*name;
} AI2C_fctdeftrigger;
extern AI2C_fctdeftrigger AI2C_gat_fctdefstrigger[];

/******************************************************************/
typedef SCR_tt_Trigger *PTRIG;
class trigger
{
public:
	trigger(void) {}
	operator PTRIG() const { return (SCR_tt_Trigger *) this; }

	BIG_KEY					h_KeyFile;
	AI2C_tdfn_TriggerFunc	pfn_CFunc;
	char					az_Name[AI_MaxLenTrigger];		/* Name of the trigger */
	message					msg;
};

#define call_trigger(a)		((a).pfn_CFunc(a.msg))

/******************************************************************/
_inline_ void push(int a)			{ AI2C_PushInt(a); }
_inline_ void push(object a)		{ AI2C_PushGameObject(a); }
_inline_ void push(float a)			{ AI2C_PushFloat(a); }
_inline_ void push(vector a)		{ AI2C_PushVector(a); }
_inline_ void push(messageid a)		{ AI2C_PushMessageId(a); }
#define intpop						AI2C_PopInt()
#define floatpop					AI2C_PopFloat()
#define objpop						AI2C_PopGameObject()
#define vecpop						_vecpop()
#define msgidpop					_msgidpop()
_inline_ vector _vecpop(void)		{ vector vv; AI2C_PopVector(vv); return vv; }
_inline_ messageid _msgidpop(void)	{ messageid vv; AI2C_PopMessageId(vv); return vv; }

#define ULTRAPROC(__obj, __a) \
{ \
	AI_tdst_Instance *___save; \
	___save = AI_gpst_CurrentInstance; \
	AI_gpst_CurrentInstance = __obj->pst_Extended->pst_Ai; \
	__a; \
	AI_gpst_CurrentInstance = ___save; \
};

/******************************************************************/
#ifndef AI2C_FCTLIST
#define AI2C_FCTLIST
#include "AI2C_fctlist.h"
#endif

/******************************************************************/
#define AI2C_FCTDEF(a, b)			
#define AI2C_FCTDEFTRIGGER(a, b, c)	extern int	  c(message &);
#include "AI2C_fctdefs.h"
#undef AI2C_FCTDEF
#undef AI2C_FCTDEFTRIGGER

/******************************************************************/
class _StoreContext_
{
public:
    OBJ_tdst_GameObject *_p;
    _StoreContext_(OBJ_tdst_GameObject *p)
    {
		_p = AI_gpst_CurrentGameObject;
		AI_gpst_CurrentInstance  = AI_gpst_CurrentInstanceUltra = p->pst_Extended->pst_Ai;
		AI_gpst_CurrentGameObject = p;
	}

	~_StoreContext_(void)
	{
		AI_gpst_CurrentInstance  = AI_gpst_CurrentInstanceUltra = _p->pst_Extended->pst_Ai;
		AI_gpst_CurrentGameObject = _p;
	}
};

/******************************************************************/

#endif /* AI2C_FCTHEADER */