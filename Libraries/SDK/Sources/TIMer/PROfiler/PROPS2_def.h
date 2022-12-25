/*$T PROPS2_def.h GC! 1.081 06/30/00 14:11:47 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* 
 * This file allows definition, initialisation and link of rasters.
 * A line must be writen like this :
 * __M_Macro__(function Id, next function Id, called function Id)
 */

/* first raster, always the same */
__M_Macro__(FirstRaster, test1, none)
__M_Macro__(test1, test2, none)
__M_Macro__(test2, test3, none)
__M_Macro__(test3, test4, none)
__M_Macro__(test4, test5, none)
__M_Macro__(test5, SpeedMode, none)
__M_Macro__(SpeedMode, BIGcomp_DecodeBlocks, none)


/* divers */
__M_Macro__(BIGcomp_DecodeBlocks, BIGcomp_Decompress, none)
__M_Macro__(BIGcomp_Decompress, BIG_fread, none)
__M_Macro__(BIG_fread, BIG_pc_ReadFileTmp1, fread)
__M_Macro__(BIG_pc_ReadFileTmp1, sceLseek, none)
__M_Macro__(sceLseek, Big_Open, none)

/*  */
__M_Macro__(fread, BIG_fread_mode2, none)
__M_Macro__(BIG_fread_mode2, none, none)

/* main */
__M_Macro__(Big_Open, PS2auxInit, none)
__M_Macro__(PS2auxInit, s_CreateDisplay, none)
__M_Macro__(s_CreateDisplay, ENG_InitEngine, none)
__M_Macro__(ENG_InitEngine, AI_Reset, none)

/* loop */
__M_Macro__(AI_Reset, WOR_Universe_Open, none)
__M_Macro__(WOR_Universe_Open, ENG_ReinitOneWorld, none)
__M_Macro__(ENG_ReinitOneWorld, ENG_gp_Input, none)
__M_Macro__(ENG_gp_Input, ENG_gp_Engine, none)
__M_Macro__(ENG_gp_Engine, ENG_gp_Display, ENG_EngineCall1)
__M_Macro__(ENG_gp_Display, none, OGL_l_AttachWorld1)

/* loop - ENG_EngineCall */
__M_Macro__(ENG_EngineCall1, ENG_EngineCall2, none)
__M_Macro__(ENG_EngineCall2, ENG_EngineCall3, AI_EvalFunc_WORActive)
__M_Macro__(ENG_EngineCall3, none, ENG_OneWorldEngineCall)

/* ENG_EngineCall3 */
__M_Macro__(ENG_OneWorldEngineCall, none, PositionModificator)

/* ENG_OneWorldEngineCall */
__M_Macro__(PositionModificator, WOR_World_ActivateObjects, none)
__M_Macro__(WOR_World_ActivateObjects, EOT_SetOfEOT_Build, none)
__M_Macro__(EOT_SetOfEOT_Build, ENG_OneWorldEngineCall1, none)
__M_Macro__(ENG_OneWorldEngineCall1, EVE_MainCall, none)
__M_Macro__(EVE_MainCall, ANI_MainCall, none)
__M_Macro__(ANI_MainCall, AI_MainCall, none)
__M_Macro__(AI_MainCall, DYN_MainCall, none)
__M_Macro__(DYN_MainCall, COL_MainCall, none)
__M_Macro__(COL_MainCall, REC_MainCall, none)
__M_Macro__(REC_MainCall, OBJ_HierarchyMainCall, none)
__M_Macro__(OBJ_HierarchyMainCall, AI_ExecCallbackAll, none)
__M_Macro__(AI_ExecCallbackAll, ViewPointModificator, none)
__M_Macro__(ViewPointModificator, WOR_World_MakeObjectsVisible, none)
__M_Macro__(WOR_World_MakeObjectsVisible, none, none)



/*
 * loop - ENG_EngineCall2 - AI_EvalCateg_Type - AI_EvalKeyword_Meta -
 * AI_EvalKeyword_Ultra1
 */
__M_Macro__(AI_EvalKeyword_Ultra1, AI_EvalKeyword_Ultra2, none)
__M_Macro__(AI_EvalKeyword_Ultra2, none, none)

/* loop - ENG_EngineCall2 - AI_EvalCateg_Type - AI_EvalCateg_Function */
__M_Macro__(AI_EvalFunc_WORActive, AI_EvalFunc_WORLoadList, none)
__M_Macro__(AI_EvalFunc_WORLoadList, none, WOR_pst_Universe_AddWorldList)
__M_Macro__(WOR_pst_Universe_AddWorldList, none, WOR_pst_Universe_AddWorldList1)
__M_Macro__(WOR_pst_Universe_AddWorldList1, none, WOR_pst_Universe_MergeWorld)
__M_Macro__(WOR_pst_Universe_MergeWorld, none, WOR_pst_Universe_AddWorld)


__M_Macro__(WOR_pst_Universe_AddWorld, WOR_pst_World_Loadbis, WOR_pst_World_Load)
__M_Macro__(WOR_pst_World_Loadbis, none, none)

/*  */
__M_Macro__(WOR_pst_World_Load, ENG_InitOneWorld, LOA_Resolve)
__M_Macro__(ENG_InitOneWorld, none, none)

/*  */
__M_Macro__(LOA_Resolve, EVE_ResolveListTrackRef, LOA_Resolve1)
__M_Macro__(EVE_ResolveListTrackRef, WOR_World_ResolveAIRef, none)
__M_Macro__(WOR_World_ResolveAIRef, none, none)
__M_Macro__(LOA_Resolve1, none, LOA_Resolve2)
__M_Macro__(LOA_Resolve2, none, none)

/* loop - s_Display - OGL_l_AttachWorld */
__M_Macro__(OGL_l_AttachWorld1, OGL_l_AttachWorld2, none)
__M_Macro__(OGL_l_AttachWorld2, TEX_Manager_Reinit, TEX_l_File_GetInfo)
__M_Macro__(TEX_Manager_Reinit, TEX_Manager_ComputeCompression, none)
__M_Macro__(TEX_Manager_ComputeCompression, OGL_l_AttachWorld3, none)
__M_Macro__(OGL_l_AttachWorld3, WOR_Render, OGL_ul_Texture_Create)
__M_Macro__(WOR_Render, none, WOR_SetCam)

/*  */
__M_Macro__(TEX_l_File_GetInfo, TEX_Procedural_Add, none)
__M_Macro__(TEX_Procedural_Add, tex_create_realloc, none)
__M_Macro__(tex_create_realloc, none, none)

/*  */
__M_Macro__(OGL_ul_Texture_Create, none, TEX_M_File_Alloc)

/*  */
__M_Macro__(TEX_M_File_Alloc, TEX_pst_Memory_FindTexture, none)
__M_Macro__(TEX_pst_Memory_FindTexture, TEX_l_Memory_DeleteTexture, none)
__M_Macro__(TEX_l_Memory_DeleteTexture, TEX_l_File_GetContent, none)
__M_Macro__(TEX_l_File_GetContent, TEX_l_ConvertSize, none)
__M_Macro__(TEX_l_ConvertSize, TEX_File_FreeDescription, none)

__M_Macro__(TEX_File_FreeDescription, TEX_M_File_Free, none)
__M_Macro__(TEX_M_File_Free, TEX_Blend_Alpha, none)
__M_Macro__(TEX_Blend_Alpha, TEX_Compress_Xo2, none)
__M_Macro__(TEX_Compress_Xo2, TEX_Compress_Yo2InvertColor, none)
__M_Macro__(TEX_Compress_Yo2InvertColor, TEX_Compress_Yo2, none)
__M_Macro__(TEX_Compress_Yo2, none, none)

/* */
__M_Macro__(WOR_SetCam, WOR_Render1, none)
__M_Macro__(WOR_Render1, MDF_ApplyAllGao, none)
__M_Macro__(MDF_ApplyAllGao, GRO_Render, none)
__M_Macro__(GRO_Render, ANI_Render, none)
__M_Macro__(ANI_Render, MDF_UnApplyAllGao, none)
__M_Macro__(MDF_UnApplyAllGao, GRI_Display, none)
__M_Macro__(GRI_Display, SOFT_ZList_Send, none)
__M_Macro__(SOFT_ZList_Send, none, none)



