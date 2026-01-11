
/*$T AIdefcateg.h GC!1.26 04/20/99 12:45:36 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Definition of all categories.

    Note:   ID is <1, 31> £
            NEVER CHANGE THE ID OF AN EXISTING ITEM !!!!
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$off*/
DEFINE_CATEG(1,     CATEG_TYPE,                 AI_EvalCateg_Type)
DEFINE_CATEG(2,     CATEG_KEYWORD,              AI_EvalCateg_Keyword)
DEFINE_CATEG(3,     CATEG_FUNCTION,             AI_EvalCateg_Function)
DEFINE_CATEG(4,     CATEG_FIELD,                AI_EvalCateg_Field)
DEFINE_CATEG(5,     CATEG_LOCALVAR,             AI_EvalCateg_LocalVar)
DEFINE_CATEG(6,     CATEG_LOCALVARARRAY,        AI_EvalCateg_LocalVarArray)
DEFINE_CATEG(7,     CATEG_LOCALVARARRAY2,       AI_EvalCateg_LocalVarArray2)
DEFINE_CATEG(8,     CATEG_LOCALVARARRAY3,       AI_EvalCateg_LocalVarArray3)
DEFINE_CATEG(10,    CATEG_LOCALVARARRAYREF,     AI_EvalCateg_LocalVarArrayRef)
DEFINE_CATEG(11,    CATEG_LOCALVARARRAY2REF,    AI_EvalCateg_LocalVarArray2Ref)
DEFINE_CATEG(12,    CATEG_LOCALVARARRAY3REF,    AI_EvalCateg_LocalVarArray3Ref)
DEFINE_CATEG(20,    CATEG_GLOBALVAR,            AI_EvalCateg_GlobalVar)
DEFINE_CATEG(21,    CATEG_GLOBALVARARRAY,       AI_EvalCateg_GlobalVarArray)
DEFINE_CATEG(22,    CATEG_GLOBALVARARRAY2,      AI_EvalCateg_GlobalVarArray2)
DEFINE_CATEG(23,    CATEG_GLOBALVARARRAY3,      AI_EvalCateg_GlobalVarArray3)
DEFINE_CATEG(25,    CATEG_INITLOCALVARARRAY,    AI_EvalCateg_InitLocalVarArray)
DEFINE_CATEG(26,    CATEG_EVENTPARAM,			AI_EvalCateg_EventParam)
DEFINE_CATEG(27,    CATEG_POPPROC,				AI_EvalCateg_PopProc)
DEFINE_CATEG(28,    CATEG_LOCALVARREF,          AI_EvalCateg_LocalVarRef)
DEFINE_CATEG(29,    CATEG_POPPROCREF,			AI_EvalCateg_PopProcRef)

DEFINE_CATEG(31,    CATEG_ENDTREE,				AI_dum) // DONT MOVE

/*$on*/
#undef DEFINE_CATEG
