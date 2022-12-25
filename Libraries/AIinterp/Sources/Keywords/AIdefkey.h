
/*$T AIdefkey.h GC!1.36 05/19/99 15:21:03 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Definition of all types.

    Note:   ID is <2048, 3500> £
            NEVER CHANGE THE ID OF AN EXISTING ITEM !!!!
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$off*/
DEFINE_KEYWORD(2048,    KEYWORD_IF,                 "if",                   AI_dum)
DEFINE_KEYWORD(2049,    KEYWORD_ELSE,               "else",                 AI_dum)
DEFINE_KEYWORD(2050,    KEYWORD_WHILE,              "while",                AI_dum)
DEFINE_KEYWORD(2051,    KEYWORD_AND,                "&&",                   AI_dum)
DEFINE_KEYWORD(2052,    KEYWORD_OR,                 "||",                   AI_dum)
DEFINE_KEYWORD(2053,    KEYWORD_META,               "meta",                 AI_EvalKeyword_Meta)
DEFINE_KEYWORD(2054,    KEYWORD_ULTRA,              "ultra",                AI_EvalKeyword_Ultra)
DEFINE_KEYWORD(2057,    KEYWORD_FOR,				"for",					AI_dum)
DEFINE_KEYWORD(2058,    KEYWORD_PROCEDURE,			"procedure",			AI_dum)
DEFINE_KEYWORD(2059,    KEYWORD_SWITCH,				"switch",				AI_EvalKeyword_Switch)
DEFINE_KEYWORD(2060,    KEYWORD_CASE,				"case",					AI_dum)
DEFINE_KEYWORD(2061,    KEYWORD_DEFAULT,			"default:",				AI_dum)
DEFINE_KEYWORD(2062,    KEYWORD_PROCTRIGGER,		"procedure_trigger",	AI_dum)
DEFINE_KEYWORD(2063,    KEYWORD_PROCEDURELOCAL,		"procedure_local",		AI_dum)
DEFINE_KEYWORD(2064,    KEYWORD_PROCEDUREULTRA,		"procedure_ultra",		AI_dum)

DEFINE_KEYWORD(2100,    KEYWORD_RETURN,             "return",               AI_EvalKeyword_Return)
DEFINE_KEYWORD(2101,    KEYWORD_BREAK,              "break",                AI_dum)
DEFINE_KEYWORD(2102,    KEYWORD_PUSH,               "push",                 AI_EvalKeyword_Push)
DEFINE_KEYWORD(2103,    KEYWORD_POP,                "pop",                  AI_EvalKeyword_Pop)
DEFINE_KEYWORD(2106,    KEYWORD_STOP,				"stop",					AI_EvalKeyword_Stop)
DEFINE_KEYWORD(2107,    KEYWORD_RETURNTRACK,        "returntrack",          AI_EvalKeyword_ReturnTrack)
DEFINE_KEYWORD(2108,    KEYWORD_CONTINUE,           "continue",             AI_dum)
DEFINE_KEYWORD(2109,    KEYWORD_CALLTRIGGER,        "call_trigger",         AI_EvalKeyword_CallTrigger)

DEFINE_KEYWORD(2110,	KEYWORD_CALLPROC,			"",						AI_EvalKeyword_call)
DEFINE_KEYWORD(2111,	KEYWORD_INPROCSTACK,		"",						AI_EvalKeyword_inprocstack)
DEFINE_KEYWORD(2112,	KEYWORD_OUTPROCSTACK,		"",						AI_EvalKeyword_outprocstack)

DEFINE_KEYWORD(2150,    KEYWORD_AFFECTP,            "p=",                   AI_EvalKeyword_AffectP)
DEFINE_KEYWORD(2151,    KEYWORD_VECAFFECTP,         "vp=",                  AI_EvalKeyword_VecAffectP)
DEFINE_KEYWORD(2152,    KEYWORD_MSGAFFECTP,         "mp=",                  AI_EvalKeyword_MsgAffectP)
DEFINE_KEYWORD(2153,    KEYWORD_TEXTAFFECTP,        "tp=",                  AI_EvalKeyword_TextAffectP)

DEFINE_KEYWORD(2200,    KEYWORD_AFFECT,             "=",                    AI_EvalKeyword_Affect)
DEFINE_KEYWORD(2201,    KEYWORD_PLUSAFFECT,         "+=",                   AI_EvalKeyword_PlusAffect)
DEFINE_KEYWORD(2202,    KEYWORD_MINUSAFFECT,        "-=",                   AI_EvalKeyword_MinusAffect)
DEFINE_KEYWORD(2203,    KEYWORD_INCAFFECT,          "++",                   AI_EvalKeyword_IncAffect)
DEFINE_KEYWORD(2204,    KEYWORD_DECAFFECT,          "--",                   AI_EvalKeyword_DecAffect)
DEFINE_KEYWORD(2205,    KEYWORD_MULAFFECT,          "*=",                   AI_EvalKeyword_MulAffect)
DEFINE_KEYWORD(2206,    KEYWORD_DIVAFFECT,          "/=",                   AI_EvalKeyword_DivAffect)
DEFINE_KEYWORD(2207,    KEYWORD_NOT,                "!",                    AI_EvalKeyword_Not)
DEFINE_KEYWORD(2208,    KEYWORD_NEG,                "-",                    AI_EvalKeyword_Neg)
DEFINE_KEYWORD(2209,    KEYWORD_EQUAL,              "==",                   AI_EvalKeyword_Equal)
DEFINE_KEYWORD(2210,    KEYWORD_GREATEQ,            ">=",                   AI_EvalKeyword_GreatEq)
DEFINE_KEYWORD(2211,    KEYWORD_LESSEQ,             "<=",                   AI_EvalKeyword_LessEq)
DEFINE_KEYWORD(2212,    KEYWORD_GREAT,              ">",                    AI_EvalKeyword_Great)
DEFINE_KEYWORD(2213,    KEYWORD_LESS,               "<",                    AI_EvalKeyword_Less)
DEFINE_KEYWORD(2214,    KEYWORD_DIFFERENT,          "!=",                   AI_EvalKeyword_Different)
DEFINE_KEYWORD(2215,    KEYWORD_MINUS,              "-",                    AI_EvalKeyword_Minus)
DEFINE_KEYWORD(2216,    KEYWORD_PLUS,               "+",                    AI_EvalKeyword_Plus)
DEFINE_KEYWORD(2217,    KEYWORD_MUL,                "*",                    AI_EvalKeyword_Mul)
DEFINE_KEYWORD(2218,    KEYWORD_DIV,                "/",                    AI_EvalKeyword_Div)
DEFINE_KEYWORD(2219,    KEYWORD_ORAFFECT,           "|=",                   AI_EvalKeyword_OrAffect)
DEFINE_KEYWORD(2220,    KEYWORD_ANDAFFECT,          "&=",                   AI_EvalKeyword_AndAffect)
DEFINE_KEYWORD(2221,    KEYWORD_XORAFFECT,          "^=",                   AI_EvalKeyword_XorAffect)
DEFINE_KEYWORD(2222,    KEYWORD_OROP,				"|",					AI_EvalKeyword_OrOp)
DEFINE_KEYWORD(2223,    KEYWORD_ANDOP,				"&",					AI_EvalKeyword_AndOp)
DEFINE_KEYWORD(2224,    KEYWORD_XOROP,				"^",					AI_EvalKeyword_XorOp)
DEFINE_KEYWORD(2225,    KEYWORD_SLEFT,				"<<",					AI_EvalKeyword_SLeft)
DEFINE_KEYWORD(2226,    KEYWORD_SRIGHT,				">>",					AI_EvalKeyword_SRight)
DEFINE_KEYWORD(2227,    KEYWORD_SLEFTAFFECT,		"<<=",					AI_EvalKeyword_SLeftAffect)
DEFINE_KEYWORD(2228,    KEYWORD_SRIGHTAFFECT,		">>=",					AI_EvalKeyword_SRightAffect)
DEFINE_KEYWORD(2229,    KEYWORD_INVERT,				"~",					AI_EvalKeyword_Invert)
DEFINE_KEYWORD(2230,	KEYWORD_OBJEQUAL,			"o==",					AI_EvalKeyword_ObjEqual)
DEFINE_KEYWORD(2231,	KEYWORD_OBJDIFFERENT,		"o!=",					AI_EvalKeyword_ObjDifferent)
DEFINE_KEYWORD(2232,	KEYWORD_NOTOBJ,				"o!",					AI_EvalKeyword_ObjNot)

DEFINE_KEYWORD(2300,    KEYWORD_JUMP,               "jump",                 AI_EvalKeyword_Jump)
DEFINE_KEYWORD(2301,    KEYWORD_JUMPFALSE,          "jumpfalse",            AI_EvalKeyword_JumpFalse)
DEFINE_KEYWORD(2302,    KEYWORD_JUMPTRUE,           "jumptrue",             AI_EvalKeyword_JumpTrue)
DEFINE_KEYWORD(2304,    KEYWORD_JUMPFALSESP,        "jumpfalsesp",          AI_EvalKeyword_JumpFalseSP)
DEFINE_KEYWORD(2305,    KEYWORD_JUMPTRUESP,         "jumptruesp",           AI_EvalKeyword_JumpTrueSP)
DEFINE_KEYWORD(2306,    KEYWORD_JUMPFF,             "jumpff",               AI_EvalKeyword_JumpFF)
DEFINE_KEYWORD(2307,    KEYWORD_GOTO,				"goto",					AI_dum)

DEFINE_KEYWORD(2400,    KEYWORD_VECAFFECT,          "v=",                   AI_EvalKeyword_VecAffect)
DEFINE_KEYWORD(2401,    KEYWORD_VECPLUSAFFECT,      "v+=",                  AI_EvalKeyword_VecPlusAffect)
DEFINE_KEYWORD(2402,    KEYWORD_VECMINUSAFFECT,     "v-=",                  AI_EvalKeyword_VecMinusAffect)
DEFINE_KEYWORD(2403,    KEYWORD_VECMULAFFECT,       "v*=",                  AI_EvalKeyword_VecMulAffect)
DEFINE_KEYWORD(2404,    KEYWORD_VECDIVAFFECT,       "v/=",                  AI_EvalKeyword_VecDivAffect)
DEFINE_KEYWORD(2405,    KEYWORD_VECMINUS,           "v-",                   AI_EvalKeyword_VecMinus)
DEFINE_KEYWORD(2406,    KEYWORD_VECPLUS,            "v+",                   AI_EvalKeyword_VecPlus)
DEFINE_KEYWORD(2407,    KEYWORD_VECMUL,             "v*",                   AI_EvalKeyword_VecMul)
DEFINE_KEYWORD(2408,    KEYWORD_VECDIV,             "v/",                   AI_EvalKeyword_VecDiv)
DEFINE_KEYWORD(2409,    KEYWORD_VECNEG,             "v-",                   AI_EvalKeyword_VecNeg)
DEFINE_KEYWORD(2410,    KEYWORD_VECEQUAL,			"v==",                  AI_EvalKeyword_VecEqual)
DEFINE_KEYWORD(2411,    KEYWORD_VECDIFFERENT,		"v!=",                  AI_EvalKeyword_VecDifferent)
DEFINE_KEYWORD(2412,    KEYWORD_VECMUL2,            "v**",                  AI_EvalKeyword_VecMul2)

DEFINE_KEYWORD(2500,    KEYWORD_MSGAFFECT,          "m=",                   AI_EvalKeyword_MsgAffect)

DEFINE_KEYWORD(2550,    KEYWORD_TEXTAFFECT,         "t=",                   AI_EvalKeyword_TextAffect)
DEFINE_KEYWORD(2551,    KEYWORD_TEXTPLUS,           "t+",                   AI_EvalKeyword_TextPlus)

DEFINE_KEYWORD(2560,    KEYWORD_MSGIDAFFECT,        "mid=",                 AI_EvalKeyword_MsgIdAffect)
DEFINE_KEYWORD(2561,    KEYWORD_TRIGGERAFFECT,      "trg=",                 AI_EvalKeyword_TriggerAffect)

/*$on*/
#undef DEFINE_KEYWORD
