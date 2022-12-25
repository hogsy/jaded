
/*$T AIdeftyp.h GC! 1.076 03/08/00 18:25:07 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Definition of all types.

    Note:   ID is <32, 511> £
			PAS DE 64 A 100 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            NEVER CHANGE THE ID OF AN EXISTING ITEM !!!!
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$off*/
DEFINE_TYPE(32,		TYPE_BOOL,			"bool",			sizeof(BOOL),						AI_EvalType_GetBool)
DEFINE_TYPE(33,		TYPE_INT,			"int",			sizeof(int),						AI_EvalType_GetInt)
DEFINE_TYPE(34,		TYPE_FLOAT,			"float",		sizeof(float),						AI_EvalType_GetInt)
DEFINE_TYPE(37,		TYPE_VECTOR,		"vector",		sizeof(MATH_tdst_Vector),			AI_EvalType_GetVector)
DEFINE_TYPE(38,		TYPE_STRING,		"string",		sizeof(int),						AI_EvalType_GetString)
DEFINE_TYPE(39,		TYPE_FUNCTIONREF,	"function",		sizeof(void *),						AI_EvalType_GetPointerRef)
DEFINE_TYPE(40,		TYPE_GAMEOBJECT,	"object",		sizeof(void *),						AI_EvalType_GetGAO)
DEFINE_TYPE(41,		TYPE_MESSAGE,		"message",		sizeof(AI_tdst_Message),			AI_EvalType_GetMessage)
DEFINE_TYPE(42,		TYPE_MODEL,			"model",		sizeof(void *),						AI_EvalType_GetPointerRef)
DEFINE_TYPE(43,		TYPE_NETWORK,		"network",		sizeof(void *),						AI_EvalType_GetPointerRef)
DEFINE_TYPE(44,		TYPE_TEXT,			"text",			sizeof(TEXT_tdst_Eval),				AI_EvalType_GetText)
DEFINE_TYPE(45,		TYPE_KEY,			"key",			sizeof(ULONG),						AI_EvalType_GetKey)
DEFINE_TYPE(46,		TYPE_COLOR,			"color",		sizeof(ULONG),						AI_EvalType_GetColor)
DEFINE_TYPE(48,		TYPE_BYREF,			"byref",		sizeof(ULONG),						AI_EvalType_GetColor)
DEFINE_TYPE(49,		TYPE_BYREFARR,		"byrefarr",		sizeof(ULONG),						AI_EvalType_GetColor)
DEFINE_TYPE(50,		TYPE_MESSAGEID,		"messageid",	sizeof(AI_tdst_GlobalMessageId),	AI_EvalType_GetMessageId)
DEFINE_TYPE(51,		TYPE_TRIGGER,		"trigger",		sizeof(SCR_tt_Trigger),				AI_EvalType_GetTrigger)

/* Dummy for editors */
DEFINE_TYPE(124,	TYPE_VOID,			"void",			sizeof(MATH_tdst_Vector),	AI_EvalType_GetVector)
DEFINE_TYPE(125,	TYPE_EVERY,			"every",		sizeof(MATH_tdst_Vector),	AI_EvalType_GetVector)
DEFINE_TYPE(126,	TYPE_HEXA,			"hexa",			sizeof(MATH_tdst_Vector),	AI_EvalType_GetVector)
DEFINE_TYPE(127,	TYPE_BINARY,		"binary",		sizeof(MATH_tdst_Vector),	AI_EvalType_GetVector)
DEFINE_TYPE(128,	TYPE_PRIVATE,		"private",		sizeof(MATH_tdst_Vector),	AI_EvalType_GetVector)
DEFINE_TYPE(129,	TYPE_SEPARATOR,		"separator",	sizeof(MATH_tdst_Vector),	AI_EvalType_GetVector)
DEFINE_TYPE(130,	TYPE_ENUM,			"enum",			sizeof(MATH_tdst_Vector),	AI_EvalType_GetVector)
DEFINE_TYPE(131,	TYPE_SAVE,			"save",			sizeof(MATH_tdst_Vector),	AI_EvalType_GetVector)
DEFINE_TYPE(132,	TYPE_REINIT,		"reinit",		sizeof(void *),				AI_EvalType_GetVector)
DEFINE_TYPE(133,	TYPE_SAVEAL,		"saveal",		sizeof(MATH_tdst_Vector),	AI_EvalType_GetVector)
DEFINE_TYPE(134,	TYPE_OPTIM,			"optim",		sizeof(MATH_tdst_Vector),	AI_EvalType_GetVector)
/*$on*/
#undef DEFINE_TYPE

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Definition of some constants (for editors)
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifdef ACTIVE_EDITORS
/*$off*/
DEFINE_CONSTANT("nobody",	TYPE_GAMEOBJECT,	"0")
DEFINE_CONSTANT("nonet",	TYPE_NETWORK,		"0")
DEFINE_CONSTANT("nofunc",	TYPE_FUNCTIONREF,	"0")
/*$on*/
#undef DEFINE_CONSTANT
#endif /* ACTIVE_EDITORS */
