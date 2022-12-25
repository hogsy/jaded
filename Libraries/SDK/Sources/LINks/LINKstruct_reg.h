/*$T LINKstruct_reg.h GC!1.71 01/13/00 11:34:57 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __LINKSTRUCT_REG_H__
#define __LINKSTRUCT_REG_H__

#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Define here all IDs of structures
 -----------------------------------------------------------------------------------------------------------------------
 */

#define LINK_C_Matrix                       0
#define LINK_C_Vector                       1

#define LINK_C_StructCamera                 2
#define LINK_C_StructDisplayData            3
#define LINK_C_FlagsDrawMask                4
#define LINK_C_DisplayFlags                 5
#define LINK_C_GDK_StructId                 6

#define LINK_C_AI_Model                     7
#define LINK_C_AI_Instance                  8

#define LINK_C_ENG_World                    9
#define LINK_C_ENG_GameObjectOriented       11
#define LINK_C_ENG_GO_IdentityFlags         12
#define LINK_C_ENG_GO_StatusAndControlFlags 13
#define LINK_C_ENG_GO_Base                  14
#define LINK_C_ENG_GO_Extended              15
#define LINK_C_ENG_GO_Visu                  16
#define LINK_C_ENG_Activator                17
#define LINK_C_ENG_GO_Dyna                  18
#define LINK_C_ENG_BoundingVolume           19
#define LINK_C_ENG_GO_EditorFlags           20

#define LINK_C_GDK_LightFlag                21

#define LINK_C_DYN_Flags                    22
#define LINK_C_DYN_BasicForces              23

#define LINK_C_MSG_LiFo                     24
#define LINK_C_MSG_Struct                   25

#define LINK_C_StructBV                     26
#define LINK_C_FlagsMatrix                  27

#define LINK_C_Capacities                   28
#define LINK_C_CustomBits                   29

#define LINK_C_NetLink                      30

#define LINK_C_ZoneFlags                    31
#define LINK_C_Zone                         32
#define LINK_C_ENG_Zone                     33

#define LINK_C_ENG_GameObjectAnim           34
#define LINK_C_ENG_Anim                     35
#define LINK_C_ENG_GO_BaseAnim              36

#define LINK_C_COL_Base                     37
#define LINK_C_COL_Detection                38
#define LINK_C_COL_Instance                 39

#define LINK_C_StructTextureManager         40
#define LINK_C_ENG_GO_BaseHierarchy         41
#define LINK_C_ENG_GO_BaseAdditionalMatrix  42
#define LINK_C_ENG_GO_Group                 43

#define LINK_C_ENG_GO_DesignFlags           44
#define LINK_C_ENG_GO_FixFlags              45

#define LINK_C_ENG_GameObjectTrace          46

#define LINK_C_ENG_ActionKit                47
#define LINK_C_ENG_Action                   48
#define LINK_C_ENG_ActionItem               49
#define LINK_C_ENG_AnimForAction            50
#define LINK_C_ENG_GameObjectEditorInfos    51

#define LINK_C_ENG_Grid                     52
#define LINK_C_ENG_GridDisplayOptions       53
#define LINK_C_ENG_GridDisplayOptionsFlags  54

#define LINK_C_DYN_Solid                    55
#define LINK_C_DYN_Col                      56

#define LINK_C_CobFlags                     57
#define LINK_C_Cob                          58
#define LINK_C_ENG_Cob                      59

#define LINK_C_ENG_ActionItemCustomBit      60

#define LINK_C_ENG_SpriteGen                61

#define LINK_C_ENG_DesignStruct             62
#define LINK_C_MDF_Modifier                 63

#define LINK_C_GMStruct						64
#define LINK_C_GameMaterialFlags			65
#define LINK_C_ElementFlags                 66

#define LINK_C_DYN_Constraint				67

#define LINK_C_TextureManagerFlag           68

#define LINK_C_ENG_ActionItemFlag           69

#define LINK_C_ENG_Skeleton                 70
#define LINK_C_ENG_Skin                     71

#define LINK_C_GEO_ModifierOTCFlag          73

#define LINK_C_GDI_ScreenFormat             74
#define LINK_C_GDI_ScreenFormatFlags        75

#define LINK_C_LOD							76

#define LINK_C_ENG_HelpersOptions           77

#define LINK_C_GAO_ModifierSDWFlag          78

#define LINK_C_GDI_EditOptions              79

#define LINK_C_ENG_TrackList				80

#define LINK_C_TEX_Procedural               81

#define LINK_C_ENG_ParticuleGeneratorFlags  82

#define LINK_C_ENG_COL_Crossable			83

#define LINK_C_GDK_LightAddMaterialFlag     84

#define LINK_C_ENG_GO_DesDesFlags			85

#define LINK_C_ENG_SpriteGenFlags           86

#define LINK_C_MBFlags						87

#define LINK_C_BlendFlags					88

#define LINK_C_ENG_BlendAnim				89

#define LINK_C_ENG_GO_ExtraFlags			90

#define LINK_C_SND_Sound                    91
#define LINK_C_SND_Bank                     92

#define LINK_C_GAO_Modifier_XMEN			93

#define LINK_C_MDF_LegLinkFlag              94

#define LINK_C_AnimFlags					95

#define LINK_C_FlagsSelect32				96

#define LINK_C_GEO_ModifierSymetrieFlag     97

#define LINK_C_SPG_SpriteMapper				98

#define LINK_C_TEX_ProceduralFlag           99

#define LINK_C_GFXShowMask                  100

#define LINK_C_SPGFlags						101

#define LINK_C_TEX_Animated                 102
#define LINK_C_3DView_WPFlags               103

#define LINK_C_GEO_EditorFlags              104

#define LINK_C_Animation_LOD				105

#define LINK_C_GDI_OceanStruct				106

#define LINK_C_ModifierCrushFlags			107

#define LINK_C_GPG_Geom						108
#define LINK_C_GPG_GeomLOD					109

#define LINK_C_Secto						110

#define LINK_C_ENG_ODE						111

#define LINK_C_ODEFlags						112

#define LINK_C_SPG2Flags					113

#define LINK_C_InternalBodyFlags			114

#define LINK_C_SurfaceFlags					115

#define LINK_C_ChannelFlag					116

#ifndef JADEFUSION
#define LINK_C_SPG2Flags1					117
#else
// -- XENON --
#define LINK_C_LightShaftFlags              117

#define LINK_C_LightCullingFlags            118

#define LINK_C_MDF_SoftBodyListRods         119
#define LINK_C_MDF_SoftBodyListVertices     120
#define LINK_C_MDF_SoftBodyListPlanes       121
#define LINK_C_MDF_SoftBodyColPlane         122

#define LINK_C_SPG2Flags1					123

#define LINK_C_XenonMeshProcessingFlags     124

#define LINK_C_GDI_HighlightColors			125

// -----------
#endif//JADEFUSION
#define LINK_C_ModifierFurFlags				130

#define LINK_C_ModifierCloneFlags			131
/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

extern void LINK_RegisterAllStructs(void);
extern void LINK_CallBack_Modifier_Refresh(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData);
extern void LINK_CallBack_Refresh3DEngine(void *, void *, void *, LONG);
extern void LINK_CallBack_Refresh3DEngineAndPointers(void *, void *, void *, LONG);

#endif /* ACTIVE_EDITORS */

#endif /* __LINKSTRUCT_REG_H__ */