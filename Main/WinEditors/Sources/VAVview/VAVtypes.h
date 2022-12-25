/*$T VAVtypes.h GC!1.59 12/22/99 14:58:52 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifdef ACTIVE_EDITORS
/*$off*/
EVAV_DEFINE_TYPE(EVAV_EVVIT_Separator,      EVAV_EVVIET_None,   EVAV_Bool_DrawItem,         EVAV_Bool_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Pointer,        EVAV_EVVIET_None,   EVAV_Bool_DrawItem,         EVAV_Bool_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_SubStruct,      EVAV_EVVIET_None,   EVAV_Bool_DrawItem,         EVAV_Bool_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Bool,           EVAV_EVVIET_Check,  EVAV_Bool_DrawItem,         EVAV_Bool_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Flags,          EVAV_EVVIET_Check,  EVAV_Flags_DrawItem,        EVAV_Flags_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Int,            EVAV_EVVIET_Edit,   EVAV_Int_DrawItem,          EVAV_Int_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Hexa,           EVAV_EVVIET_Edit,   EVAV_Hexa_DrawItem,         EVAV_Hexa_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_String,         EVAV_EVVIET_Edit,   EVAV_String_DrawItem,       EVAV_String_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Float,          EVAV_EVVIET_Edit,   EVAV_Float_DrawItem,        EVAV_Float_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Vector,         EVAV_EVVIET_Button, EVAV_Vector_DrawItem,       EVAV_Vector_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Matrix,         EVAV_EVVIET_Button, EVAV_Matrix_DrawItem,       EVAV_Matrix_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Color,          EVAV_EVVIET_Button, EVAV_Color_DrawItem,        EVAV_Color_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_AdrLoaFile,     EVAV_EVVIET_None,   EVAV_File_DrawItem,         EVAV_Bool_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_AIFunction,     EVAV_EVVIET_Combo,  EVAV_AIFunc_DrawItem,       EVAV_AIFunc_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_GO,             EVAV_EVVIET_Combo,  EVAV_GO_DrawItem,           EVAV_GO_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_NetWork,        EVAV_EVVIET_Combo,  EVAV_Network_DrawItem,      EVAV_Network_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Custom,         EVAV_EVVIET_Button, EVAV_Custom_DrawItem,       EVAV_Custom_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_GroType,        EVAV_EVVIET_Combo,  EVAV_GroType_DrawItem,      EVAV_GroType_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_GroLightType,   EVAV_EVVIET_Combo,  EVAV_GroLightType_DrawItem, EVAV_GroLightType_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_BV,             EVAV_EVVIET_Button, EVAV_BV_DrawItem,           EVAV_BV_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_ODE,            EVAV_EVVIET_Button, EVAV_ODE_DrawItem,          EVAV_ODE_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Group,			EVAV_EVVIET_Combo,	EVAV_Grp_DrawItem,			EVAV_Grp_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Zone,           EVAV_EVVIET_Button, EVAV_Zone_DrawItem,         EVAV_Zone_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Cob,			EVAV_EVVIET_Button, EVAV_Cob_DrawItem,			EVAV_Cob_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_ConstInt,       EVAV_EVVIET_Combo,  EVAV_ConstInt_DrawItem,     EVAV_ConstInt_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_ConstFloat,     EVAV_EVVIET_Combo,  EVAV_ConstFloat_DrawItem,   EVAV_ConstFloat_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_CustSep,        EVAV_EVVIET_None,   EVAV_CustSep_DrawItem,      EVAV_CustSep_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Text,           EVAV_EVVIET_Button, EVAV_Text_DrawItem,         EVAV_Text_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Anim,           EVAV_EVVIET_Button, EVAV_Anim_DrawItem,			EVAV_Anim_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Skeleton,       EVAV_EVVIET_Button, EVAV_Anim_DrawItem,			EVAV_Anim_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Skin,           EVAV_EVVIET_Button, EVAV_Anim_DrawItem,			EVAV_Anim_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Key,			EVAV_EVVIET_Button, EVAV_Key_DrawItem,			EVAV_Key_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_SndKey,			EVAV_EVVIET_Button, EVAV_SndKey_DrawItem,		EVAV_SndKey_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Modifier,		EVAV_EVVIET_Button, EVAV_Modifier_DrawItem, 	EVAV_Modifier_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Geom,			EVAV_EVVIET_Button, EVAV_Geom_DrawItem, 		EVAV_Geom_FillSelect)
EVAV_DEFINE_TYPE(EVAV_EVVIT_Trigger,		EVAV_EVVIET_Button, EVAV_Trigger_DrawItem, 		EVAV_Trigger_FillSelect)
#ifdef JADEFUSION
EVAV_DEFINE_TYPE(EVAV_EVVIT_WindCurve,		EVAV_EVVIET_Button, EVAV_WindCurve_DrawItem,	EVAV_WindCurve_FillSelect)
#endif
/*$on*/
#endif /* ACTIVE_EDITORS */
