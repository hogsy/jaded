/*$T AIfunctions_text.c GC! 1.100 03/20/01 14:40:55 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "LINks/LINKmsg.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/AItools.h"
#include "AIinterp/Sources/AIdebug.h"
#include "ENGine/Sources/TEXT/TEXTload.h"
#include "ENGine/Sources/TEXT/TEXT.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"
#include "EDIpaths.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_TEXTLipsIdxGet_C(TEXT_tdst_Eval *_pst_Text)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEXT_tdst_OneText	*pst_Txt;
    int id;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Text) return -1;
	if(_pst_Text->i_FileKey == BIG_C_InvalidIndex) return -1;
    
    id = TEXT_i_GetOneTextIndex(_pst_Text->i_FileKey);
	AI_Check(id >= 0, "Text has not been loaded");
	if(id < 0) return -1;

	pst_Txt = TEXT_gst_Global.pst_AllTexts[id];
    AI_Check(pst_Txt, "Text has not been loaded");
	if(!pst_Txt) return -1;

    id = TEXT_i_GetEntryIndex(pst_Txt, _pst_Text->i_Id);
    AI_Check((ULONG) id < pst_Txt->ul_Num, "Invalid text number");
    AI_Check(id != -1, "Invalid text number");
	if(id == -1) return -1;
		
	return  (0xFF & (int)pst_Txt->pst_Ids[id].c_LipsIdx);
}
/**/
AI_tdst_Node *AI_EvalFunc_TEXTLipsIdxGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Eval	*ptr;
	/*~~~~~~~~~~~~~~~~~*/

	ptr = AI_PopTextPtr();
	AI_PushInt(AI_EvalFunc_TEXTLipsIdxGet_C(ptr));
	return ++_pst_Node;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_TEXTFacialIdxGet_C(TEXT_tdst_Eval *_pst_Text)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEXT_tdst_OneText	*pst_Txt;
    int id;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Text) return -1;
	if(_pst_Text->i_FileKey == BIG_C_InvalidIndex) return -1;
    
    id = TEXT_i_GetOneTextIndex(_pst_Text->i_FileKey);
	AI_Check(id >= 0, "Text has not been loaded");
	if(id < 0) return -1;

	pst_Txt = TEXT_gst_Global.pst_AllTexts[id];
    AI_Check(pst_Txt, "Text has not been loaded");
	if(!pst_Txt) return -1;

    id = TEXT_i_GetEntryIndex(pst_Txt, _pst_Text->i_Id);
    AI_Check((ULONG) id < pst_Txt->ul_Num, "Invalid text number");
    AI_Check(id != -1, "Invalid text number");
	if(id == -1) return -1;
		
	return  (0xFF & (int)pst_Txt->pst_Ids[id].c_FacialIdx);
}
/**/
AI_tdst_Node *AI_EvalFunc_TEXTFacialIdxGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Eval	*ptr;
	/*~~~~~~~~~~~~~~~~~*/

	ptr = AI_PopTextPtr();
	AI_PushInt(AI_EvalFunc_TEXTFacialIdxGet_C(ptr));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_TEXTAnimIdxGet_C(TEXT_tdst_Eval *_pst_Text)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEXT_tdst_OneText	*pst_Txt;
    int id;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Text) return -1;
	if(_pst_Text->i_FileKey == BIG_C_InvalidIndex) return -1;
    
    id = TEXT_i_GetOneTextIndex(_pst_Text->i_FileKey);
	AI_Check(id >= 0, "Text has not been loaded");
	if(id < 0) return -1;

	pst_Txt = TEXT_gst_Global.pst_AllTexts[id];
    AI_Check(pst_Txt, "Text has not been loaded");
	if(!pst_Txt) return -1;

    id = TEXT_i_GetEntryIndex(pst_Txt, _pst_Text->i_Id);
    AI_Check((ULONG) id < pst_Txt->ul_Num, "Invalid text number");
    AI_Check(id != -1, "Invalid text number");
	if(id == -1) return -1;
		
	return  (0xFF & (int)pst_Txt->pst_Ids[id].c_AnimIdx);
}
/**/
AI_tdst_Node *AI_EvalFunc_TEXTAnimIdxGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Eval	*ptr;
	/*~~~~~~~~~~~~~~~~~*/

	ptr = AI_PopTextPtr();
	AI_PushInt(AI_EvalFunc_TEXTAnimIdxGet_C(ptr));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_TEXTTimeSet_C(TEXT_tdst_Eval *_pst_Text, float f_time)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEXT_tdst_OneText	*pst_Txt;
    int id;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Text) return ;
	if(_pst_Text->i_FileKey == BIG_C_InvalidIndex) return ;
    
    id = TEXT_i_GetOneTextIndex(_pst_Text->i_FileKey);
	AI_Check(id >= 0, "Text has not been loaded");
	if(id < 0) return ;

	pst_Txt = TEXT_gst_Global.pst_AllTexts[id];
    AI_Check(pst_Txt, "Text has not been loaded");
	if(!pst_Txt) return ;

    id = TEXT_i_GetEntryIndex(pst_Txt, _pst_Text->i_Id);
    AI_Check((ULONG) id < pst_Txt->ul_Num, "Invalid text number");
    AI_Check(id != -1, "Invalid text number");
	if(id == -1) return ;
		
    pst_Txt->pst_Ids[id].f_LastTime = f_time;
}
/**/
AI_tdst_Node *AI_EvalFunc_TEXTTimeSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Eval	*ptr;
    float ff;
	/*~~~~~~~~~~~~~~~~~*/

    ff = AI_PopFloat();
	ptr = AI_PopTextPtr();
	AI_EvalFunc_TEXTTimeSet_C(ptr, ff);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_TEXTTimeGet_C(TEXT_tdst_Eval *_pst_Text)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEXT_tdst_OneText	*pst_Txt;
    int id;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Text) return -1;
	if(_pst_Text->i_FileKey == BIG_C_InvalidIndex) return -1;
    
    id = TEXT_i_GetOneTextIndex(_pst_Text->i_FileKey);
	AI_Check(id >= 0, "Text has not been loaded");
	if(id < 0) return -1;

	pst_Txt = TEXT_gst_Global.pst_AllTexts[id];
    AI_Check(pst_Txt, "Text has not been loaded");
	if(!pst_Txt) return -1;

    id = TEXT_i_GetEntryIndex(pst_Txt, _pst_Text->i_Id);
    AI_Check((ULONG) id < pst_Txt->ul_Num, "Invalid text number");
    AI_Check(id != -1, "Invalid text number");
	if(id == -1) return -1;
		
    return  pst_Txt->pst_Ids[id].f_LastTime;
}
/**/
AI_tdst_Node *AI_EvalFunc_TEXTTimeGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Eval	*ptr;
	/*~~~~~~~~~~~~~~~~~*/

	ptr = AI_PopTextPtr();
	AI_PushFloat(AI_EvalFunc_TEXTTimeGet_C(ptr));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_TEXTIsEqual_C(TEXT_tdst_Eval *_pst_Text1, TEXT_tdst_Eval *_pst_Text2)
{
	if(!_pst_Text1 && !_pst_Text2) return 1;
	if(!_pst_Text1 || !_pst_Text2) return 0;

	if(_pst_Text1->i_FileKey != _pst_Text2->i_FileKey) return 0;
    if(_pst_Text1->i_Id != _pst_Text2->i_Id) return 0;

    return  1;
}
/**/
AI_tdst_Node *AI_EvalFunc_TEXTIsEqual(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Eval	*ptr1, *ptr2;
	/*~~~~~~~~~~~~~~~~~*/

	ptr1 = AI_PopTextPtr();
	ptr2 = AI_PopTextPtr();
	AI_PushInt(AI_EvalFunc_TEXTIsEqual_C(ptr1, ptr2));
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_TEXTFileGet_C(TEXT_tdst_Eval *_pst_Text)
{
	if(!_pst_Text) return -1;
    return  _pst_Text->i_FileKey;
}
/**/
AI_tdst_Node *AI_EvalFunc_TEXTFileGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Eval	*ptr;
	/*~~~~~~~~~~~~~~~~~*/

	ptr = AI_PopTextPtr();
	AI_PushInt(AI_EvalFunc_TEXTFileGet_C(ptr));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_TEXTEntryGet_C(TEXT_tdst_Eval *_pst_Text)
{
	if(!_pst_Text) return -1;
    return  _pst_Text->i_Id;
}
/**/
AI_tdst_Node *AI_EvalFunc_TEXTEntryGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Eval	*ptr;
	/*~~~~~~~~~~~~~~~~~*/

	ptr = AI_PopTextPtr();
	AI_PushInt(AI_EvalFunc_TEXTEntryGet_C(ptr));
	return ++_pst_Node;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_TEXTObjGet_C(TEXT_tdst_Eval *_pst_Text)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEXT_tdst_OneText	*pst_Txt;
    int id;
    ULONG ul;
#ifdef ACTIVE_EDITORS
    char log[512];
#endif
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Text) return NULL;
	if(_pst_Text->i_FileKey == BIG_C_InvalidIndex) return NULL;
    
    id = TEXT_i_GetOneTextIndex(_pst_Text->i_FileKey);
	AI_Check(id >= 0, "Text has not been loaded");
	if(id < 0) return NULL;

	pst_Txt = TEXT_gst_Global.pst_AllTexts[id];
    AI_Check(pst_Txt, "Text has not been loaded");
	if(!pst_Txt) return NULL;

    id = TEXT_i_GetEntryIndex(pst_Txt, _pst_Text->i_Id);
    AI_Check((ULONG) id < pst_Txt->ul_Num, "Invalid text number");
    AI_Check(id != -1, "Invalid text number");
	if(id == -1) return NULL;
		
    if(pst_Txt->pst_Ids[id].pv_Obj == NULL && pst_Txt->pst_Ids[id].ul_ObjKey)
    {
        ul = BIG_ul_SearchKeyToPos(pst_Txt->pst_Ids[id].ul_ObjKey);
        if(ul != -1)
            ul = LOA_ul_SearchAddress(ul);
        
        if(ul != -1)
            pst_Txt->pst_Ids[id].pv_Obj = (void*)ul;


#ifdef ACTIVE_EDITORS        
        sprintf
            (log, "a gao [%x] is referenced by a text [%x] (entry %x) but is not loaded",
            pst_Txt->pst_Ids[id].ul_ObjKey,
            pst_Txt->ul_Key, 
            pst_Txt->pst_Ids[id].ul_IdKey
            );
        ERR_X_Warning(pst_Txt->pst_Ids[id].pv_Obj , log, NULL);
#else
        ERR_X_Warning(pst_Txt->pst_Ids[id].pv_Obj , "a gao is referenced by a text but is not loaded", NULL);
#endif

    }

    return  (OBJ_tdst_GameObject*)pst_Txt->pst_Ids[id].pv_Obj;
}
/**/
AI_tdst_Node *AI_EvalFunc_TEXTObjGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Eval	*ptr;
	/*~~~~~~~~~~~~~~~~~*/

	ptr = AI_PopTextPtr();
	AI_PushGameObject(AI_EvalFunc_TEXTObjGet_C(ptr));
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_TEXTToPriority_C(TEXT_tdst_Eval *_pst_Text)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEXT_tdst_OneText	*pst_Txt;
    int id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Text) return TEXT_e_PrioInvalid;
	if(_pst_Text->i_FileKey == BIG_C_InvalidIndex) return TEXT_e_PrioInvalid;
    
    id = TEXT_i_GetOneTextIndex(_pst_Text->i_FileKey);
	AI_Check(id >= 0, "Text has not been loaded");
	if(id < 0) return TEXT_e_PrioInvalid;

	pst_Txt = TEXT_gst_Global.pst_AllTexts[id];
    AI_Check(pst_Txt, "Text has not been loaded");
	if(!pst_Txt) return TEXT_e_PrioInvalid;

    id = TEXT_i_GetEntryIndex(pst_Txt, _pst_Text->i_Id);
    AI_Check((ULONG) id < pst_Txt->ul_Num, "Invalid text number");
    AI_Check(id != -1, "Invalid text number");
	if(id == -1) return TEXT_e_PrioInvalid;
		
    return  (int)pst_Txt->pst_Ids[id].us_Priority;
}
/**/
AI_tdst_Node *AI_EvalFunc_TEXTToPriority(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Eval	*ptr;
	/*~~~~~~~~~~~~~~~~~*/

	ptr = AI_PopTextPtr();
	AI_PushInt(AI_EvalFunc_TEXTToPriority_C(ptr));
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_TEXTToSound_C(TEXT_tdst_Eval *_pst_Text)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEXT_tdst_OneText	*pst_Txt;
    int id;
    extern LONG SND_l_GetSoundIndex(BIG_KEY _ul_WavKey);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Text) return -1;
	if(_pst_Text->i_FileKey == BIG_C_InvalidIndex) return -1;
    
    id = TEXT_i_GetOneTextIndex(_pst_Text->i_FileKey);
	AI_Check(id >= 0, "Text has not been loaded");
	if(id < 0) return -1;

	pst_Txt = TEXT_gst_Global.pst_AllTexts[id];
    AI_Check(pst_Txt, "Text has not been loaded");
	if(!pst_Txt) return -1;

    id = TEXT_i_GetEntryIndex(pst_Txt, _pst_Text->i_Id);
    AI_Check((ULONG) id < pst_Txt->ul_Num, "Invalid text number");
    AI_Check(id != -1, "Invalid text number");
	if(id == -1) return -1;
		
    return  SND_l_GetSoundIndex(pst_Txt->pst_Ids[id].ul_SoundKey);
}
/**/
AI_tdst_Node *AI_EvalFunc_TEXTToSound(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Eval	*ptr;
	/*~~~~~~~~~~~~~~~~~*/

	ptr = AI_PopTextPtr();
	AI_PushInt(AI_EvalFunc_TEXTToSound_C(ptr));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *AI_EvalFunc_TEXTToString_C(TEXT_tdst_Eval *_pst_Text)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEXT_tdst_OneText	*pst_Txt;
    int id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Text) return "";
	if(_pst_Text->i_FileKey == BIG_C_InvalidIndex) return "";
    
    id = TEXT_i_GetOneTextIndex(_pst_Text->i_FileKey);
	AI_Check(id >= 0, "Text has not been loaded");
	if(id < 0) return "";

	pst_Txt = TEXT_gst_Global.pst_AllTexts[id];
    AI_Check(pst_Txt, "Text has not been loaded");
	if(!pst_Txt) return "";

    id = TEXT_i_GetEntryIndex(pst_Txt, _pst_Text->i_Id);
    AI_Check((ULONG) id < pst_Txt->ul_Num, "Invalid text number");
    AI_Check(id != -1, "Invalid text number");
	if(id == -1) return "";
	
	return pst_Txt->psz_Text + pst_Txt->pst_Ids[id].i_Offset;
}
/**/
AI_tdst_Node *AI_EvalFunc_TEXTToString(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Eval	*ptr;
	/*~~~~~~~~~~~~~~~~~*/

	ptr = AI_PopTextPtr();
	AI_PushString(AI_EvalFunc_TEXTToString_C(ptr));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_TEXTIsEmpty_C(TEXT_tdst_Eval *_pst_Text)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEXT_tdst_OneText	*pst_Txt;
    int id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Text) return 1;
	if(_pst_Text->i_FileKey == BIG_C_InvalidIndex) return 1;
	if(_pst_Text->i_FileKey == 0) return 1;
    
    id = TEXT_i_GetOneTextIndex(_pst_Text->i_FileKey);
	if(id < 0) return 1;

	pst_Txt = TEXT_gst_Global.pst_AllTexts[id];
    if(!pst_Txt) return 1;

    id = TEXT_i_GetEntryIndex(pst_Txt, _pst_Text->i_Id);
    if(id < 0) return 1;
    if(id > (int)pst_Txt->ul_Num) return 1;

	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_TEXTIsEmpty(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Eval	*ptr;
	/*~~~~~~~~~~~~~~~~~*/

	ptr = AI_PopTextPtr();
	AI_Check(ptr, "Text is null");
	AI_PushInt(AI_EvalFunc_TEXTIsEmpty_C(ptr));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_TEXTCompare_C(char *s1, char *s2)
{
//  TMP pour jak...
//    return (L_strcmp(s1, s2) ? 0 : 1);
    return (L_strncmp(s1, s2, L_strlen(s2)) ? 0 : 1);
}
/**/
AI_tdst_Node *AI_EvalFunc_TEXTCompare(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~*/
	char	*s1, *s2;
	/*~~~~~~~~~~~~~*/

	s2 = (char *) AI_PopStringPtr();
	s1 = (char *) AI_PopStringPtr();

	AI_Check(s1, "String 1 is null");
	AI_Check(s2, "String 2 is null");

	AI_PushInt(AI_EvalFunc_TEXTCompare_C(s1, s2));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_TEXTLength_C(char *s1)
{
	AI_Check(s1, "String is null");
	return L_strlen(s1);
}
/**/
AI_tdst_Node *AI_EvalFunc_TEXTLength(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_TEXTLength_C((char *) AI_PopStringPtr()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_TEXTSetLang(AI_tdst_Node *_pst_Node)
{
	TEXT_ChangeLang(AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_TEXTSetSndLang(AI_tdst_Node *_pst_Node)
{
#ifdef JADEFUSION
	extern void TEXT_ChangeSndLang(int);
#endif
	TEXT_ChangeSndLang(AI_PopInt());
	return ++_pst_Node;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_TEXTGet_C(int i_Key, int i_Index, TEXT_tdst_Eval *_pst_Text)
{
    TEXT_tdst_OneText	*pst_Txt;
    int id;

	_pst_Text->i_FileKey = -1;
	_pst_Text->i_Id = -1;

	if(!_pst_Text) return ;
	if(i_Key == BIG_C_InvalidIndex) return ;
    
    id = TEXT_i_GetOneTextIndex(i_Key);
	AI_Check(id >= 0, "Text has not been loaded");
	if(id < 0) return ;

	pst_Txt = TEXT_gst_Global.pst_AllTexts[id];
    AI_Check(pst_Txt, "Text has not been loaded");
	if(!pst_Txt) return ;

    id = TEXT_i_GetEntryIndex(pst_Txt, i_Index);
    AI_Check((ULONG) id < pst_Txt->ul_Num, "Invalid text number");
    AI_Check(id != -1, "Invalid text number");
	if(id == -1) return ;


	_pst_Text->i_FileKey = i_Key;
	_pst_Text->i_Id = i_Index;
}
/**/
AI_tdst_Node *AI_EvalFunc_TEXTGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Eval		txt;
	AI_tdst_UnionVar	Val;
	int					i_Key, i_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Index = AI_PopInt();
	i_Key = AI_PopInt();
	AI_EvalFunc_TEXTGet_C(i_Key, i_Index, &txt);
	Val.t.i_FileKey = txt.i_FileKey;
	Val.t.i_Id = txt.i_Id;
	AI_PushVal(&Val, AI_gaw_EnumLink[TYPE_TEXT]);
	return ++_pst_Node;
}

void AI_EvalFunc_TEXTRankShift_C(TEXT_tdst_Eval *_pInText, int iShift, TEXT_tdst_Eval *_pOutText)
{
    TEXT_tdst_OneText	*pTxt;
    int id;

	if(!_pInText) return ;
	if(!_pOutText) return ;

    if(_pInText->i_FileKey == BIG_C_InvalidIndex) return ;
    
    id = TEXT_i_GetOneTextIndex(_pInText->i_FileKey);
	AI_Check(id >= 0, "Text has not been loaded");
	if(id < 0) return ;

	pTxt = TEXT_gst_Global.pst_AllTexts[id];
    AI_Check(pTxt, "Text has not been loaded");
	if(!pTxt) return ;

    id = TEXT_i_GetEntryIndex(pTxt, _pInText->i_Id);
    AI_Check((ULONG) id < pTxt->ul_Num, "Invalid text number");
    if((ULONG) id >= pTxt->ul_Num) return;

    AI_Check(id >= 0, "Invalid text number");
    if(id < 0) return;
    
    id += iShift;
    AI_Check( id >=0, "Underflow text rank operation");
    AI_Check( id < (int)pTxt->ul_Num, "Overflow text rank operation");

    if(id< 0) id= 0;
    if(id>= (int)pTxt->ul_Num) id= (int)pTxt->ul_Num - 1;

    _pOutText->i_FileKey = _pInText->i_FileKey;
    _pOutText->i_Id = pTxt->pst_Ids[id].ul_IdKey;
}
/**/
AI_tdst_Node *AI_EvalFunc_TEXTRankShift(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Eval		*itxt;
	TEXT_tdst_Eval		otxt;
	AI_tdst_UnionVar	Val;
	int					iShift;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	iShift = AI_PopInt();
	itxt = AI_PopTextPtr();
    AI_EvalFunc_TEXTRankShift_C(itxt, iShift, &otxt);

    Val.t.i_FileKey = otxt.i_FileKey;
	Val.t.i_Id = otxt.i_Id;

    AI_PushVal(&Val, AI_gaw_EnumLink[TYPE_TEXT]);
	return ++_pst_Node;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_TEXTCharGet_C(char *s1, int index )
{
	AI_Check(s1, "String is null");
    //if ( ((ULONG) index) > L_strlen( s1 ) ) return 0;
    return s1[index];
}
/**/
AI_tdst_Node *AI_EvalFunc_TEXTCharGet(AI_tdst_Node *_pst_Node)
{
    int index;

    index = AI_PopInt();
	AI_PushInt(AI_EvalFunc_TEXTCharGet_C((char *) AI_PopStringPtr(), index ) );
	return ++_pst_Node;
}



#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
