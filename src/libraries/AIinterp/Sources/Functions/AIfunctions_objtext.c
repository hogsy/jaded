/*$T AIfunctions_objtext.c GC! 1.100 03/19/01 17:55:35 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstack.h"
#include "STRing/STRstruct.h"
#include "STRing/STRdata.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "ENGine/Sources/TEXT/TEXTstruct.h"
#include "ENGine/Sources/TEXT/TEXT.h"
#include "ENGine/Sources/TEXT/TEXTload.h"


/*
 =======================================================================================================================
 A DEPLACER QUAND THIERRY AURA RENDU LES DROITS SUR OBJ
 =======================================================================================================================
 */
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEO.h"

void AI_EvalFunc_OBJ_MdfOTCResetAll_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier				*pst_Mod;
	GEO_tdst_ModifierOnduleTonCorps *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO || !_pst_GO->pst_Extended) return;

	pst_Mod = _pst_GO->pst_Extended->pst_Modifiers;
	while(pst_Mod)
	{
		if(pst_Mod->i->ul_Type == (ULONG) MDF_C_Modifier_OnduleTonCorps)
		{
			pst_Data = (GEO_tdst_ModifierOnduleTonCorps *) pst_Mod->p_Data;
			pst_Data->f_Angle = 0.0;
		}
		pst_Mod = pst_Mod->pst_Next;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_MdfOTCResetAll(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJ_MdfOTCResetAll_C(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_STR_RegisterMaterial_C(OBJ_tdst_GameObject *_pst_Gao )
{
	if (!_pst_Gao ) return;
	if (!_pst_Gao->pst_Base ) return;
	if (!(_pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)) return ;
	if (!_pst_Gao->pst_Base->pst_Visu ) return;
	STR_gst_Data.pst_Material = (MAT_tdst_Material *) _pst_Gao->pst_Base->pst_Visu->pst_Material;
}
/**/
AI_tdst_Node *AI_EvalFunc_STR_RegisterMaterial(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_STR_RegisterMaterial_C( AI_PopGameObject() );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_STR_GroupDisplaySet( AI_tdst_Node *_pst_Node )
{
	int i_on, i_off;
	
	i_off = AI_PopInt();
	i_on = AI_PopInt();
	AI_PushInt( STRDATA_i_Group_SetDisplay( i_on, i_off ) );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_STR_CreateText(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~*/
	float f_Val;
	MATH_tdst_Vector *V;
	/*~~~~~~~~~~~~~*/
	
	f_Val = AI_PopFloat();
	V = AI_PopVectorPtr();
	AI_PushInt( STRDATA_i_CreateText(AI_PopStringPtr(), V, f_Val ) );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_STR_CreateTextGroup(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_Group;
	float				f_Val;
	MATH_tdst_Vector	*V;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	i_Group = AI_PopInt();
	f_Val = AI_PopFloat();
	V = AI_PopVectorPtr();
	AI_PushInt( STRDATA_i_CreateTextGroup(AI_PopStringPtr(), V, f_Val, i_Group ) );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_STR_DeleteText(AI_tdst_Node *_pst_Node)
{
	STRDATA_DeleteText( AI_PopInt() );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_STR_AppendText(AI_tdst_Node *_pst_Node)
{
	char *sz = AI_PopStringPtr();
	STRDATA_AppendText( AI_PopInt(), sz );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_STR_AppendInt(AI_tdst_Node *_pst_Node)
{	
	int i_Val = AI_PopInt();
	STRDATA_AppendInt( AI_PopInt(), i_Val );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_STR_AppendHexa(AI_tdst_Node *_pst_Node)
{
	int i_Val = AI_PopInt();
	STRDATA_AppendHexa( AI_PopInt(), i_Val );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_STR_AppendFloat(AI_tdst_Node *_pst_Node)
{	
	int	i_Dec = AI_PopInt();
	float f_Val = AI_PopFloat();
	STRDATA_AppendFloat( AI_PopInt(), f_Val, i_Dec );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_STR_AppendVector(AI_tdst_Node *_pst_Node)
{	
	MATH_tdst_Vector *V = AI_PopVectorPtr();
	STRDATA_AppendVector( AI_PopInt(), V );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_STR_AppendGao(AI_tdst_Node *_pst_Node)
{
	OBJ_tdst_GameObject *GO = AI_PopGameObject();
	STRDATA_AppendGao( AI_PopInt(), GO );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

/*$4
 ***********************************************************************************************************************
    Request£
	pour l'instant une seule requete : savoir si les fontes sont chargées ou non
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_TextRequest_C(int _i_Request, int _i_Param )
{
	if (_i_Request == 0)
		return STR_sgl_NumberOfFont;
	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_TextRequest(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int	i_Param;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	i_Param = AI_PopInt();
	AI_PushInt(AI_EvalFunc_OBJ_TextRequest_C(AI_PopInt(), i_Param));
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
    Global string
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextGlobalSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~*/
	char *pc_String;
	/*~~~~~~~~~~~~~*/

	pc_String = AI_PopStringPtr();
	AI_Check(pc_String != 0, "TextSet error : Null string");
	STR_GlobalSet( AI_PopInt(), pc_String);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextGlobalCharSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~*/
    int     i_Value;
    int     i_Char;
	/*~~~~~~~~~~~~~*/

    i_Value = AI_PopInt();
    i_Char = AI_PopInt();
	STR_GlobalSetChar( AI_PopInt(), i_Char, i_Value );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextGlobalGet( AI_tdst_Node *_pst_Node )
{
    AI_PushString( STR_sz_GlobalGet( AI_PopInt() ) );
    return ++_pst_Node;
}

#ifdef JADEFUSION
/*
=======================================================================================================================
=======================================================================================================================
*/
INT AI_EvalFunc_OBJ_TextGetNum_C( )
{
    int i;
    int iNumText = 0;

    for (i = 0; i < TEXT_gst_Global.i_Num; i++)
    {
        if (TEXT_gst_Global.pst_AllTexts[i])
        {
            iNumText += TEXT_gst_Global.pst_AllTexts[i]->ul_Num;
        }        
    }

    return iNumText;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_TextGetNum(AI_tdst_Node *_pst_Node)
{
    AI_PushInt( AI_EvalFunc_OBJ_TextGetNum_C() );    
    return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
int AI_EvalFunc_OBJ_TextGetByIndex_C(INT a_Index )
{
    int i, j;
    int iTextSeek = a_Index;    

    for (i = 0; i < TEXT_gst_Global.i_Num; i++)
    {
        if (TEXT_gst_Global.pst_AllTexts[i])
        {
            for (j = 0; j < (int)TEXT_gst_Global.pst_AllTexts[i]->ul_Num; j++)
            {
                iTextSeek--;
                if (iTextSeek == 0)
                {
                    return TEXT_gst_Global.pst_AllTexts[i]->pst_Ids[j].ul_IdKey;
                }
            }                
        }        
    }

    return NULL;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_TextGetByIndex(AI_tdst_Node *_pst_Node)
{
    AI_PushInt(AI_EvalFunc_OBJ_TextGetByIndex_C(AI_PopInt()));
    return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
int AI_EvalFunc_OBJ_TextGroupGetByIndex_C(INT a_Index )
{
    int i, j;
    int iTextSeek = a_Index;    

    for (i = 0; i < TEXT_gst_Global.i_Num; i++)
    {
        if (TEXT_gst_Global.pst_AllTexts[i])
        {
            for (j = 0; j < (int)TEXT_gst_Global.pst_AllTexts[i]->ul_Num; j++)
            {
                iTextSeek--;
                if (iTextSeek == 0)
                {
                    return TEXT_gst_Global.pst_AllTexts[i]->ul_FatherKey;
                }
            }                
        }        
    }

    return NULL;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_TextGroupGetByIndex(AI_tdst_Node *_pst_Node)
{
    AI_PushInt(AI_EvalFunc_OBJ_TextGroupGetByIndex_C(AI_PopInt()));
    return ++_pst_Node;
}
#endif
/*$4
 ***********************************************************************************************************************
    Text 2D
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_TextMaxWidthSet_C(OBJ_tdst_GameObject *_pst_GO, float _f_MaxWidth )
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct *pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	if(pst_STR) pst_STR->f_MaxWidth = _f_MaxWidth;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_TextMaxWidthSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJ_TextMaxWidthSet_C(pst_GO, AI_PopFloat() );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextReset(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	STR_Reset(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	char				*pc_String;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	pc_String = AI_PopStringPtr();
	AI_Check(pc_String != 0, "TextSet error : Null string");
	STR_SetString(pst_GO, AI_PopInt(), pc_String);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextSetExt(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	char				*pc_String;
	MATH_tdst_Vector	*v;
	int					flags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	v = AI_PopVectorPtr();
	pc_String = AI_PopStringPtr();
	flags = AI_PopInt();
	AI_Check(pc_String != 0, "TextSetExt error : Null string");
	STR_SetStringExt(pst_GO, AI_PopInt(), flags, pc_String, v );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextInsert(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_From;
	char				*pc_String;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	pc_String = AI_PopStringPtr();
	i_From = AI_PopInt();
	AI_Check(pc_String != 0, "TextInsert error : Null string");
	STR_InsertString(pst_GO, AI_PopInt(), i_From, pc_String);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextFloatSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f;
	int					dec;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	dec = AI_PopInt();
	f = AI_PopFloat();
	STR_SetFloat(pst_GO, AI_PopInt(), f, dec);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextFloatInsert(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_From, dec;
	float				f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	dec = AI_PopInt();
	f = AI_PopFloat();
	i_From = AI_PopInt();
	STR_InsertFloat(pst_GO, AI_PopInt(), i_From, f, dec);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextIntInsert(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_From, i_Val;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Val = AI_PopInt();
	i_From = AI_PopInt();
	STR_InsertInt(pst_GO, AI_PopInt(), i_From, i_Val);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_TextPivotSet_C(OBJ_tdst_GameObject *_pst_GO, int _i_Number, MATH_tdst_Vector *v)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct *pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	if(pst_STR) STR_SetPivot(pst_STR, _i_Number, v);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_TextPivotSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	*v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	v = AI_PopVectorPtr();
	AI_EvalFunc_OBJ_TextPivotSet_C(pst_GO, AI_PopInt(), v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_TextPivotGet_C(OBJ_tdst_GameObject *_pst_GO, int _i_Index, MATH_tdst_Vector *v)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct *pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	if(pst_STR)
	{
		MATH_CopyVector(v, STR_pst_GetPivot(pst_STR, _i_Index));
		return;
	}

	MATH_InitVectorToZero(v);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_TextPivotGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJ_TextPivotGet_C(pst_GO, AI_PopInt(), &v);
	AI_PushVector(&v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_TextFlagSet_C(OBJ_tdst_GameObject *_pst_GO, int _i_Index, int _i_Flag)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct *pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	if(pst_STR) STR_SetFlag(pst_STR, _i_Index, _i_Flag);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_TextFlagSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Flag;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Flag = AI_PopInt();
	AI_EvalFunc_OBJ_TextFlagSet_C(pst_GO, AI_PopInt(), i_Flag);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_TextFlagGet_C(OBJ_tdst_GameObject *_pst_GO, int _i_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct *pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	if(!pst_STR) return 0;
	return STR_ul_GetFlag(pst_STR, _i_Index);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_TextFlagGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_OBJ_TextFlagGet_C(pst_GO, AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_TextColorSet_C
(
	OBJ_tdst_GameObject *_pst_GO,
	int					_i_Index,
	int					_i_From,
	int					_i_Number,
	int					_i_Color
)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct *pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	if(pst_STR) STR_SetColor(pst_STR, _i_Index, _i_From, _i_Number, _i_Color);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_TextColorSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_From, i_Number, i_Color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Color = AI_PopInt();
	i_Number = AI_PopInt();
	i_From = AI_PopInt();
	AI_EvalFunc_OBJ_TextColorSet_C(pst_GO, AI_PopInt(), i_From, i_Number, i_Color);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_TextAlphaSet_C
(
	OBJ_tdst_GameObject *_pst_GO,
	int					_i_Index,
	int					_i_From,
	int					_i_Number,
	int					_i_Alpha
)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct *pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	if(pst_STR) STR_SetAlpha(pst_STR, _i_Index, _i_From, _i_Number, _i_Alpha);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_TextAlphaSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_From, i_Number, i_Color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Color = AI_PopInt();
	i_Number = AI_PopInt();
	i_From = AI_PopInt();
	AI_EvalFunc_OBJ_TextAlphaSet_C(pst_GO, AI_PopInt(), i_From, i_Number, i_Color);
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_TextColorSetExt_C
(
	OBJ_tdst_GameObject *_pst_GO,
	int					_i_Index,
	int					_i_From,
	int					_i_Number,
	int					_i_Color,
    int                 _i_Mask

)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct *pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	if(pst_STR) STR_SetColorExt(pst_STR, _i_Index, _i_From, _i_Number, _i_Color, _i_Mask);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_TextColorSetExt(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_From, i_Number, i_Color, i_Mask;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
    i_Mask = AI_PopInt();
	i_Color = AI_PopInt();
	i_Number = AI_PopInt();
	i_From = AI_PopInt();
	AI_EvalFunc_OBJ_TextColorSetExt_C(pst_GO, AI_PopInt(), i_From, i_Number, i_Color, i_Mask);
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_TextColorGet_C
(
	OBJ_tdst_GameObject *_pst_GO,
	int					_i_Index,
	int					_i_From,
	int					_i_Number
)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct *pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	return STR_GetColor(pst_STR, _i_Index, _i_From, _i_Number);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_TextColorGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_From, i_Number;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Number = AI_PopInt();
	i_From = AI_PopInt();
	AI_PushInt(AI_EvalFunc_OBJ_TextColorGet_C(pst_GO, AI_PopInt(), i_From, i_Number));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_TextPageSet_C
(
	OBJ_tdst_GameObject *_pst_GO,
	int					_i_Index,
	int					_i_From,
	int					_i_Number,
	int					_i_Page,
	int					_i_OldPage
)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct *pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	if(pst_STR) STR_ChangePage(pst_STR, _i_Index, _i_From, _i_Number, _i_Page, _i_OldPage);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_TextPageSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_From, i_Number, i_Color, i_OldColor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_OldColor = AI_PopInt();
	i_Color = AI_PopInt();
	i_Number = AI_PopInt();
	i_From = AI_PopInt();
	AI_EvalFunc_OBJ_TextPageSet_C(pst_GO, AI_PopInt(), i_From, i_Number, i_Color, i_OldColor);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextPosSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	*v;
	int					i_From, i_Number;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	v = AI_PopVectorPtr();
	i_Number = AI_PopInt();
	i_From = AI_PopInt();
	STR_SetPos(pst_GO, AI_PopInt(), i_From, i_Number, v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextSizeSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	*v;
	int					i_Number, i_From;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	v = AI_PopVectorPtr();
	i_Number = AI_PopInt();
	i_From = AI_PopInt();
	STR_SetSize(pst_GO, AI_PopInt(), i_From, i_Number, v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextDelete(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Number, i_From;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Number = AI_PopInt();
	i_From = AI_PopInt();
	STR_DelChar(pst_GO, AI_PopInt(), i_From, i_Number);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_TextCharSet_C(OBJ_tdst_GameObject *_pst_GO, int _i_String, int _i_Index, int _i_Char)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct *pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	if(pst_STR) STR_SetChar(pst_STR, _i_String, _i_Index, _i_Char);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_TextCharSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Index, i_Char;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Char = AI_PopInt();
	i_Index = AI_PopInt();
	AI_EvalFunc_OBJ_TextCharSet_C(pst_GO, AI_PopInt(), i_Index, i_Char);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_TextCharGet_C(OBJ_tdst_GameObject *_pst_GO, int _i_String, int _i_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct *pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	if(!pst_STR) return 0;
	return STR_i_GetChar(pst_STR, _i_String, _i_Index);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_TextCharGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Index = AI_PopInt();
	AI_PushInt(AI_EvalFunc_OBJ_TextCharGet_C(pst_GO, AI_PopInt(), i_Index));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_TextLength_C(OBJ_tdst_GameObject *_pst_GO, int _i_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct *pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	if(!pst_STR) return -1;
	return STR_l_GetLength(pst_STR, _i_Index);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_TextLength(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_OBJ_TextLength_C(pst_GO, AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_TextSubLength_C(OBJ_tdst_GameObject *_pst_GO, int _i_Index, int _i_Sub)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_Struct *pst_STR;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_STR = STR_pst_Get(_pst_GO);
	if(!pst_STR) return -1;
	return STR_l_GetSubLength(pst_STR, _i_Index, _i_Sub);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_TextSubLength(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Sub;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Sub = AI_PopInt();
	AI_PushInt(AI_EvalFunc_OBJ_TextSubLength_C(pst_GO, AI_PopInt(), i_Sub));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_TextPosGetMin_C
(
	OBJ_tdst_GameObject *pst_GO,
	int					i1,
	int					i_From,
	int					i_Number,
	MATH_tdst_Vector	*dest
)
{
	/*~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~*/

	MATH_CopyVector(dest, STR_pst_GetMin(pst_GO, i1, i_From, i_Number, &v));
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_TextPosGetMin(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_From, i_Number, i1;
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Number = AI_PopInt();
	i_From = AI_PopInt();
	i1 = AI_PopInt();
	AI_EvalFunc_OBJ_TextPosGetMin_C(pst_GO, i1, i_From, i_Number, &v);
	AI_PushVector(&v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_TextPosGetMax_C(OBJ_tdst_GameObject *pst_GO,int ii, int i_From,int i_Number, MATH_tdst_Vector*dest)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	MATH_CopyVector(dest, STR_pst_GetMax(pst_GO, ii, i_From, i_Number, &v));
}

AI_tdst_Node *AI_EvalFunc_OBJ_TextPosGetMax(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_From, i_Number, ii;
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Number = AI_PopInt();
	i_From = AI_PopInt();
	ii= AI_PopInt();
	AI_EvalFunc_OBJ_TextPosGetMax_C(pst_GO, ii, i_From, i_Number, &v);
	AI_PushVector(&v);
	
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextAlign(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_From, i_Number, i_Align;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
    i_Align = AI_PopInt();
	i_Number = AI_PopInt();
	i_From = AI_PopInt();
	STR_Align(pst_GO, AI_PopInt(), i_From, i_Number, i_Align);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_TextSizeGet_C
(
	OBJ_tdst_GameObject *pst_GO,
	int					i0,
	int					i_From,
	int					i_Number,
	MATH_tdst_Vector	*dest
)
{
	/*~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~*/

	MATH_CopyVector(dest, STR_pst_GetSize(pst_GO, i0, i_From, i_Number, &v));
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_TextSizeGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	int					i0;
	OBJ_tdst_GameObject *pst_GO;
	int					i_Number, i_From;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Number = AI_PopInt();
	i_From = AI_PopInt();
	i0 = AI_PopInt();
	AI_EvalFunc_OBJ_TextSizeGet_C(pst_GO, i0, i_From, i_Number, &v);
	AI_PushVector(&v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextRectGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Number, i_From, i_Type;
	MATH_tdst_Vector	V;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Type = AI_PopInt();
	i_Number = AI_PopInt();
	i_From = AI_PopInt();
	STR_GetRect( pst_GO, AI_PopInt(), i_From, i_Number, i_Type, &V );
	AI_PushVector(&V);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextFrameSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Char, i_Page;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Char = AI_PopInt();
	i_Page = AI_PopInt();

	STR_SetFrame(pst_GO, AI_PopInt(), i_Page, i_Char);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextFramePosSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	*v1, *v2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	v2 = AI_PopVectorPtr();
	v1 = AI_PopVectorPtr();
	STR_SetFramePos(pst_GO, AI_PopInt(), v1, v2);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextFrameBorderSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_Size = AI_PopFloat();

	STR_SetFrameBorderSize(pst_GO, AI_PopInt(), f_Size);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextFrameColorSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_Color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	ul_Color = (ULONG) AI_PopInt();

	STR_SetFrameColor(pst_GO, AI_PopInt(), ul_Color);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#if 0
AI_tdst_Node *AI_EvalFunc_OBJ_TextFrame4ColorSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_Color1, ul_Color2, ul_Color3, ul_Color4;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	ul_Color4 = (ULONG) AI_PopInt();
	ul_Color3 = (ULONG) AI_PopInt();
	ul_Color2 = (ULONG) AI_PopInt();
	ul_Color1 = (ULONG) AI_PopInt();

	STR_SetFrame4Color(pst_GO, AI_PopInt(), ul_Color1, ul_Color2, ul_Color3, ul_Color4);

	return ++_pst_Node;
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextPartWithLine(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					f, n;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	n = AI_PopInt();
	f = AI_PopInt();

	STR_SetSubStringWithLine(pst_GO, AI_PopInt(), f, n);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextPartWithMark(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					f, n;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	n = AI_PopInt();
	f = AI_PopInt();

	STR_SetSubStringWithMark(pst_GO, AI_PopInt(), f, n);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextPart(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					f, n;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	n = AI_PopInt();
	f = AI_PopInt();

	STR_SetSubString(pst_GO, AI_PopInt(), f, n);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextCountLine(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					f, n;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	n = AI_PopInt();
	f = AI_PopInt();
	AI_PushInt(STR_i_CountLine(pst_GO, AI_PopInt(), f, n));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextCountMark(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Mark, f, n;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Mark = AI_PopInt();
	n = AI_PopInt();
	f = AI_PopInt();
	AI_PushInt(STR_i_CountMark(pst_GO, AI_PopInt(), f, n, i_Mark));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_Text2Text_C(OBJ_tdst_GameObject *_pst_GO, int i, int f, int n, TEXT_tdst_Eval *_pst_Text)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_OneText	*pst_Txt;
    int id;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Text->i_FileKey == (int) BIG_C_InvalidIndex) return;
    i = TEXT_i_GetOneTextIndex(_pst_Text->i_FileKey);
    AI_Check(i!= -1, "Text has not been loaded");
    if(i==-1) return;

    pst_Txt = TEXT_gst_Global.pst_AllTexts[i];
	AI_Check(pst_Txt != NULL, "Text has not been loaded");

	id = TEXT_i_GetEntryIndex(pst_Txt, _pst_Text->i_Id);
    AI_Check(id != -1, "Entry key doesn't exist");
    if(id == -1) return;

    STR_ToString(_pst_GO, i, f, n, pst_Txt->psz_Text + pst_Txt->pst_Ids[id].i_Offset);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_Text2Text(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Eval		*ptr;
	OBJ_tdst_GameObject *pst_GO;
	int					f, n;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	ptr = AI_PopTextPtr();
	n = AI_PopInt();
	f = AI_PopInt();
	AI_EvalFunc_OBJ_Text2Text_C(pst_GO, AI_PopInt(), f, n, ptr);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextCharFlagSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_From, i_Number, i_Value;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Value = AI_PopInt();
	i_Number = AI_PopInt();
	i_From = AI_PopInt();
	STR_SetLetterFlag(pst_GO, AI_PopInt(), i_From, i_Number, i_Value);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextCharFlagGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Char;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Char = AI_PopInt();
	AI_PushInt(STR_GetLetterFlag(pst_GO, AI_PopInt(), i_Char));
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextToUpper(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Number, i_From;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Number = AI_PopInt();
	i_From = AI_PopInt();
	STR_ToUpper(pst_GO, AI_PopInt(), i_From, i_Number );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextToLower(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Number, i_From;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Number = AI_PopInt();
	i_From = AI_PopInt();
	STR_ToLower(pst_GO, AI_PopInt(), i_From, i_Number );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_3DText(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~*/
	char	*pc_String;
	/*~~~~~~~~~~~~~~~*/

	pc_String = AI_PopStringPtr();
	STR_Init3DString(AI_PopGameObject(), pc_String);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_3DTextExt(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~*/
	char	*pc_String;
    int     i_NbChar;
	/*~~~~~~~~~~~~~~~*/

    i_NbChar = AI_PopInt();
	pc_String = AI_PopStringPtr();
	STR_Init3DStringExt(AI_PopGameObject(), pc_String, i_NbChar);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_3DTextCenter(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~*/
	char	*pc_String;
    int     i_NbChar;
	/*~~~~~~~~~~~~~~~*/

    i_NbChar = AI_PopInt();
	pc_String = AI_PopStringPtr();
	STR_Init3DStringCenter(AI_PopGameObject(), pc_String, i_NbChar);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_3DTextFromStrObj(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                 i_NbChar, i_Str, i_Center;
    OBJ_tdst_GameObject *pst_GOSTR;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    i_Center = AI_PopInt();
    i_NbChar = AI_PopInt();
	i_Str = AI_PopInt();
    pst_GOSTR = AI_PopGameObject();
	STR_Init3DStringFromSTR(AI_PopGameObject(), pst_GOSTR, i_Str, i_NbChar, i_Center);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_3DTextLangDependent(AI_tdst_Node *_pst_Node)
{
	STR_3DStringList_Add(AI_PopGameObject());
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
    Text 2D - Effect
 ***********************************************************************************************************************
 */


/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextEffectAdd(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Type, i_Str, i_From, i_Number;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Number = AI_PopInt();
	i_From = AI_PopInt();
    i_Str = AI_PopInt();
    i_Type = AI_PopInt();
    AI_PushInt( STR_i_EffectAdd( pst_GO, i_Type, i_Str, i_From, i_Number ) );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextEffectDel(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
    i_Id = AI_PopInt();
    STR_EffectDel( pst_GO, i_Id );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextEffectSeti(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Id, i_Param, i_Value;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
    i_Value = AI_PopInt();
    i_Param = AI_PopInt();
    i_Id = AI_PopInt();
    STR_EffectSeti( pst_GO, i_Id, i_Param, i_Value );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextEffectGeti(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Id, i_Param;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
    i_Param = AI_PopInt();
    i_Id = AI_PopInt();
    AI_PushInt( STR_EffectGeti( pst_GO, i_Id, i_Param ) );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_TextEffectSetf(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Id, i_Param;
    float               f_Value;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
    f_Value = AI_PopFloat();
    i_Param = AI_PopInt();
    i_Id = AI_PopInt();
    STR_EffectSetf( pst_GO, i_Id, i_Param, f_Value );
	return ++_pst_Node;
}

