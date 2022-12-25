/*$T AIerrid.h GC! 1.075 03/07/00 12:17:12 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#define ERR_COMPILER_Csz_Identifier         "Expected identifier"
#define ERR_COMPILER_Csz_Number             "Expected number"
#define ERR_COMPILER_Csz_VarName            "Expected variable name"
#define ERR_COMPILER_Csz_UltraGO            "Ultra operator needs a game object"
#define ERR_COMPILER_Csz_BadType            "Bad variable type"
#define ERR_COMPILER_Csz_DupSymbol          "Symbol already defined"
#define ERR_COMPILER_Csz_ExpString          "Expected a given string"
#define ERR_COMPILER_Csz_UnkownID           "Unknow identifier"
#define ERR_COMPILER_Csz_Syntax             "Syntax error"
#define ERR_COMPILER_Csz_BadNumPars         "Bad number of parameters"
#define ERR_COMPILER_Csz_BadTypes           "Incompatible types"
#define ERR_COMPILER_Csz_CantCreate         "This function already exists"
#define ERR_COMPILER_Csz_SizeArray          "Expected a constant size for array"
#define ERR_COMPILER_Csz_BreakWhile         "Break must be used in a while"
#define ERR_COMPILER_Csz_MissString         "Missing end mark of string"
#define ERR_COMPILER_Csz_Field              "Invalid field name"
#define ERR_COMPILER_Csz_Dim                "Too many dimensions for array"
#define ERR_COMPILER_Csz_BadDim             "Bad number of dimension to access array"
#define ERR_COMPILER_Csz_VarModeDecl        "Can't define code in a variable file"
#define ERR_COMPILER_Csz_UnkownRef          "Unknown reference"
#define ERR_COMPILER_Csz_TooTrack           "The number of the track is invalid"
#define ERR_COMPILER_Csz_TooCallback        "The number of the callback is invalid"
#define ERR_COMPILER_Csz_CantGlobal         "Can't define a global variable here !"
#define ERR_COMPILER_Csz_CantInitLocal      "You can't init local variables here"
#define ERR_COMPILER_Csz_CantPrivateLocal   "Can't define a private local variable"
#define ERR_COMPILER_Csz_NotUltra           "Function is not ultra able"
#define ERR_COMPILER_Csz_RefNotLoaded       "Model must be loaded to make that reference"
#define ERR_COMPILER_Csz_MustCast           "Need a model cast"
#define ERR_COMPILER_Csz_InitArray          "Too much init values for that array"

#define ERR_PP_Csz_InvalidDirective         "Invalid preprocessor directive"
#define ERR_PP_Csz_DefineTwice              "Constant already defines"
#define ERR_PP_Csz_InvalidPath              "Invalid #include path"
#define ERR_PP_Csz_UnkownFile               "Can't find included file"
#define ERR_PP_Csz_IncludeTwice             "File has been included twice"
#define ERR_PP_Csz_RecurseReplace           "Preprocessor recurse replacement"
#define ERR_PP_Csz_BadNumParamMacro         "Bad number of parameters for macro"
#define ERR_PP_Csz_UnknownConstant          "Unknown constant"

#define EAI_ERR_Csz_ModelAlreadyExists      "Model already exists !"
#define EAI_ERR_Csz_FunctionAlreadyExists   "Function already exists !"
