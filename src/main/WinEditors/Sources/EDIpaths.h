/*$T EDIpaths.h GC! 1.078 03/17/00 10:14:00 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __EDIPATHS_H__
#define __EDIPATHS_H__

/* Some paths */
#define EDI_Csz_Path_GameData			BIG_Csz_Root "/EngineDatas"
#define EDI_Csz_Path_EditData			BIG_Csz_Root "/EditorDatas"

#define EDI_Csz_Path_Textures			BIG_Csz_Root "/EngineDatas/01 Texture Bank"
#define EDI_Csz_Path_Animation			BIG_Csz_Root "/EngineDatas/03 Animation Bank"
#define EDI_Csz_Path_AnimationAni		EDI_Csz_Path_Animation "/Animations"
#define EDI_Csz_Path_AnimationAct		EDI_Csz_Path_Animation "/Actions"
#define EDI_Csz_Path_Technical			BIG_Csz_Root "/EngineDatas/04 Technical Bank"
#define EDI_Csz_Path_ObjModels			BIG_Csz_Root "/EngineDatas/04 Technical Bank/Object Models"
#define EDI_Csz_Path_Prefab				BIG_Csz_Root "/EngineDatas/04 Technical Bank/Object Models"
#define EDI_Csz_Path_PrefabUnsorted		BIG_Csz_Root "/EngineDatas/04 Technical Bank/Object Models/Unsorted"
#define EDI_Csz_Path_PrefabUnsortedSub	BIG_Csz_Root "Unsorted"

#define EDI_Csz_Path_AIModels			BIG_Csz_Root "/EngineDatas/04 Technical Bank/AI Models"
#define EDI_Csz_Path_AILib				BIG_Csz_Root "/EngineDatas/04 Technical Bank/AI Models/Global Library"
#define EDI_Csz_Path_COLModels			BIG_Csz_Root "/EngineDatas/04 Technical Bank/Col Models"
#define EDI_Csz_Path_COLGameMaterials	BIG_Csz_Root "/EngineDatas/04 Technical Bank/Game Materials"
#define EDI_Csz_Path_ToolSettings		BIG_Csz_Root "/EngineDatas/04 Technical Bank/Tools settings"
#define EDI_Csz_Path_Audio				BIG_Csz_Root "/EngineDatas/05 Audio Bank"

#define EDI_Csz_Path_Objects			BIG_Csz_Root "/EngineDatas/02 Modelisation Bank"
#define EDI_Csz_Path_Levels				BIG_Csz_Root "/EngineDatas/06 Levels"
#define EDI_Csz_Path_Texts				BIG_Csz_Root "/EngineDatas/04 Technical Bank/Texts"

#define EDI_Csz_Path_DisplayData		BIG_Csz_Root "/EditorDatas/Display Datas"
#define EDI_Csz_Path_Activators			BIG_Csz_Root "/EngineDatas/Activators"

#define EDI_Csz_Ini						BIG_Csz_Root "/EditorDatas/Ini"
#define EDI_Csz_Ini_Keyboard			BIG_Csz_Root "/EditorDatas/Ini/Keyboard"
#define EDI_Csz_Ini_NameKits			BIG_Csz_Root "/EditorDatas/Ini/Name Kits"
#define EDI_Csz_Ini_Desktop				BIG_Csz_Root "/EditorDatas/Ini/Desktop"
#define EDI_Csz_VSSPath					BIG_Csz_Root "/EditorDatas/Data Control"
#define EDI_Csz_Help					BIG_Csz_Root "/EditorDatas/Help"

#define EDI_Csz_Path_TrashCan			BIG_Csz_Root "/EngineDatas/TrashCan"
#define EDI_Csz_Path_TrashCan_Textures	EDI_Csz_Path_TrashCan "/Textures"
#define EDI_Csz_Path_TrashCan_Objects	EDI_Csz_Path_TrashCan "/Modelisation"

#define EDI_Csz_Path_GameObject			"Game Objects"
#define EDI_Csz_Path_GameObjectRLI		"Game Objects RLI"
#define EDI_Csz_Path_GraphicObject		"Graphic Objects"
#define EDI_Csz_Path_GraphicMaterial	"Materials"
#define EDI_Csz_Path_AIInstances		"AI Instances"
#define EDI_Csz_Path_COLInstances		"COL Instances"
#define EDI_Csz_Path_COLObjects			"Collision Objects"
#define EDI_Csz_Path_PostIt				"PostIt"
#define EDI_Csz_Path_Network			"Networks"
#define EDI_Csz_Path_NetLinks			"Links"
#define EDI_Csz_Path_Dyna				"Dynamic"
#define EDI_Csz_Path_Groups				"Groups"
#define EDI_Csz_Path_Cinematics			"Cinematics"
#define EDI_Csz_Path_Grid				"Grid"
#define EDI_Csz_Path_ListTracks			"Events"
#ifdef JADEFUSION
#define EDI_Csz_Path_LightRejection		"LightRejection"
#endif

/* Some files */
#define EDI_Csz_ExternalIniFile "Jade.ini"
#define EDI_Csz_ExternalHelp	"JadeHelp"
#define EDI_Csz_FrameIniFile	"MainFrame"
#define EDI_Csz_DefaultDesktop	"Default"
#define EDI_Csz_NameDesktop		"Desktop.dsk"
#define EDI_Csz_NameToolBar		"ToolBar.dsk"
#define EDI_Csz_DefaultKitNames "KitNames.ini"
#define EDI_Csz_AdminUserName	"Admin"
#define EDI_Csz_LockedFileRead	"__locked__.r"
#define EDI_Csz_LockedFileWrite "__locked__.w"
#define EDI_Csz_LockedFileOpen	"__locked__.o"

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Some extensions. For a group, you must change the BIG_b_IsGrpFile function in BIGmdfy_file.c to validate the
    extension in the desction of a file group.
 -----------------------------------------------------------------------------------------------------------------------
 */

#define EDI_Csz_ExtIni			".ini"
#define EDI_Csz_ExtIniDef		".def"
#define EDI_Csz_ExtLnk			".lin"
#define EDI_Csz_ExtEdiAction	".eac"
#define EDI_Csz_ExtVss			".vss"
#define EDI_Csz_ExtTree			".tre"
#define EDI_Csz_PostIt			".pst"

/* AI */
#define EDI_Csz_ExtAIEditorModel	".mdl"	/* GROUP */
#define EDI_Csz_ExtAIEditorVars		".var"
#define EDI_Csz_ExtAIEditorFct		".fct"
#define EDI_Csz_ExtAIEditorDepend	".dep"
#define EDI_Csz_ExtAIEngineModel	".omd"	/* GROUP */
#define EDI_Csz_ExtAIEngineFct		".ofc"
#define EDI_Csz_ExtAIEngineVars		".ova"
#define EDI_Csz_ExtAIEngineInstance ".oin"
#define EDI_Csz_ExtAIEngineFctLib	".fce"
#define EDI_Csz_ExtAIEditorFctLib	".fcl"

#define EDI_Csz_ExtCOLSetModel		".cmd"
#define EDI_Csz_ExtCOLInstance		".cin"
#define EDI_Csz_ExtCOLObject		".cob"
#define EDI_Csz_ExtCOLMap			".map"
#define EDI_Csz_ExtCOLGMAT			".gam"

/* Graphic */
#define EDI_Csz_ExtGraphicObject	".gro"
#define EDI_Csz_ExtGraphicMaterial	".grm"
#define EDI_Csz_ExtGraphicLight		".grl"
#define EDI_Csz_ExtTexture1			".tga"
#define EDI_Csz_ExtTexture2			".bmp"
#define EDI_Csz_ExtTexture3			".jpg"
#define EDI_Csz_ExtTextureCubeMap   ".cbm"

/* Game Object */
#define EDI_Csz_ExtGameObject		".gao"
#define EDI_Csz_ExtGameObjects		".gol"		/* GROUP : Game object list */
#define EDI_Csz_ExtObjModels		".mod"		/* GROUP : Object models */
#define EDI_Csz_ExtObjGroups		".grp"
#define EDI_Csz_ExtObjGolGroups		".gog"
#define EDI_Csz_ExtWorldText		".txt"
#define EDI_Csz_ExtPrefab			".pfb"		/* Prefab */
#define EDI_Csz_ExtGameObjectRLI	".rli"

/* World and universe */
#define EDI_Csz_ExtUniverse		".uuu"
#define EDI_Csz_ExtWorld		".wow"
#define EDI_Csz_ExtWorldList	".wol"
#define EDI_Csz_ExtGrpWorld		".grw"		/* Table of all groups attached to a world */

/* Texts */
#define EDI_Csz_ExtTextFile ".txl"			
#define EDI_Csz_ExtTextLang ".txg"			/* GROUP : List of all languages for one text */

/* Mecanics */
#define EDI_Csz_ExtDyna ".dyn"

/* Animations */
#define EDI_Csz_ExtAnimation	".nim"
#define EDI_Csz_ExtSkeleton		".skl"
#define EDI_Csz_ExtSkin			".ski"
#define EDI_Csz_ExtAnimTbl		".tbl"
#define EDI_Csz_ExtShape		".shp"

/* Actions */
#define EDI_Csz_ExtAction		".act"
#define EDI_Csz_ExtActionKit	".ack"		/* GROUP */

/* Networks */
#define EDI_Csz_ExtNetWorld		".net"
#define EDI_Csz_ExtNetWay		".way"
#define EDI_Csz_ExtNetObject	".lnk"

/* Grid */
#define EDI_Csz_ExtGridDef			".gri"
#define EDI_Csz_ExtGridCompressed	".grc"

/* Events */
#define EDI_Csz_ExtEventAllsTracks	".trl"

/* Light rejection list */
#define EDI_Csz_ExtLightRejectionList	".lrl"

/* Video */
#define EDI_Csz_ExtVideo1			".ipu"
#define EDI_Csz_ExtVideo2			".m2v"
#define EDI_Csz_ExtVideo3			".mtx"
#define EDI_Csz_ExtVideo4			".bik"
#define EDI_Csz_ExtVideo5			".xmv"

/* Sound */
#define EDI_Csz_ExtSoundMetaBank	".msk"  /* GROUP : list of snk */
#define EDI_Csz_ExtSoundBank	    ".snk"  /* GROUP : list of wav, waa, wam, wad, smd, wac */
#define EDI_Csz_ExtSoundMusic	    ".wam"  /* music */
#define EDI_Csz_ExtSoundAmbience    ".waa"  /* ambience */
#define EDI_Csz_ExtSoundDialog      ".wad"  /* dialog */
#define EDI_Csz_ExtSoundFile	    ".wav"  /* sound */
#define EDI_Csz_ExtSModifier        ".smd"  /* sound modifier */
#define EDI_Csz_ExtLoadingSound     ".wac"  /* loading sound */
#define EDI_Csz_ExtSoundInsert		".ins"  /* sound insert */
#define EDI_Csz_ExtSoundFade		".fad"  /* sound fade */


#endif /* __EDIPATHS_H__ */
