/*$T OUTframe_ini.cpp GC!1.63 12/27/99 15:04:40 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h" 
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "OUTframe.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "SOFT/SOFThelper.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "SOFT/SOFTbackgroundimage.h"
#include "ENGine/Sources/GRId/GRI_display.h"
#include "ENGine/Sources/WORld/WORmain.h"

#ifdef JADEFUSION
extern int		TEX_gi_ForceText;
extern BOOL		ENG_gb_DistCompute;
#else
extern "C" int		TEX_gi_ForceText;
extern "C" BOOL		ENG_gb_DistCompute;
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::ReinitIni(void)
{
    EDI_cl_BaseFrame::ReinitIni();
    mst_Ini.ul_Mark = 0x1310DA7A;
    mst_Ini.ul_Version = 19;//POPOWARNING 18 popoverif
	mst_Ini.ul_GridPaintMode = 0;
    mst_Ini.b_EngineDisplay = TRUE;
    mst_Ini.b_EngineCam = FALSE;
    mst_Ini.ul_DisplayFlags = 0;
    mst_Ini.ul_DrawMask = 0xFFFFFFFF;
    mst_Ini.ul_ConstantColor = 0xFFFFFFFF;
	mst_BaseIni.b_EngineRefresh = TRUE;
    mst_BaseIni.b_EditorRefresh = TRUE;
    mst_Ini.b_SnapToGrid = FALSE;
    mst_Ini.f_SnapGridSizeXY = 1.0f;
    mst_Ini.f_SnapGridSizeZ = 1.0f;
    mst_Ini.b_SnapGridDisplay = FALSE;
    mst_Ini.f_SnapGridWidth = 10.0f;
    mst_Ini.b_SnapAngle = FALSE;
    mst_Ini.f_SnapAngleStep = 0.1f;
    mst_Ini.ul_GridFlags = 0;
    mst_Ini.uc_GridPaintValue = 1;
	mst_Ini.uc_SelDialogLeft = 1;
    mst_Ini.uc_CopyMatrixFrom_Flags = 0x3b;
    mst_Ini.ul_GridKeyForZLocked = 0xFFFFFFFF;
    mst_Ini.ul_ScreenFlags = 0;
    mst_Ini.f_ScreenYoverX = 1;
    mst_Ini.f_PixelYoverX = 1;
    mst_Ini.l_ScreenFormat = 0;
	mst_Ini.ul_SelMinimize = FALSE;
    mst_Ini.ul_PickingBufferFlags = 0;
    mst_Ini.f_NormalLength = 1.0f;
    mst_Ini.ul_VertexColor[0] = 0xFF0000FF;
    mst_Ini.ul_VertexColor[1] = 0xFF00FFFF;
    mst_Ini.ul_VertexColor[2] = 0xFF007FFF;
    mst_Ini.ul_VertexColor[3] = 0xFF00FF7F;
    mst_Ini.ul_EdgeColor[0] = 0xFFFF7F00;
    mst_Ini.ul_EdgeColor[1] = 0xFFFFFF7F;
    mst_Ini.ul_EdgeColor[2] = 0xFFFF7F7F;
    mst_Ini.ul_EdgeColor[3] = 0xFFFFFFFF;
    mst_Ini.ul_FaceColor[0] = 0xFF0000FF;
    mst_Ini.ul_FaceColor[1] = 0xFF00FFFF;
    mst_Ini.ul_FaceColor[2] = 0xFF007FFF;
    mst_Ini.ul_FaceColor[3] = 0xFF00FF7F;
    mst_Ini.ul_NormalColor[0] = 0xFF00FF00;
    mst_Ini.ul_NormalColor[1] = 0xFF00FFFF;
    mst_Ini.f_VertexSize = 1.0f;
    mst_Ini.f_EdgeSize = 1.0f;
    mst_Ini.f_FaceSize = 1.0f;
    mst_Ini.f_NormalSize = 1.0f;
	mst_Ini.f_GridDeltaZ = 1.0f;
    mst_Ini.ul_BackgroundImage = 0xFFFFFFFF;
    mst_Ini.ul_TextureManager_Flags = TEX_Manager_FastCompression | TEX_Manager_UseMipmap | TEX_Manager_Accept32bpp | TEX_Manager_RGB2BGR  /*| TEX_Manager_Accept8bpp*/ ;
    mst_Ini.ul_TextureManager_MaxSize = 512;
    mst_Ini.ul_TextureManager_MinSize = 8;
    mst_Ini.ul_TextureManager_MaxMem = 16 * 1024 * 1024;
	mst_Ini.DistCompute = TRUE;
	mst_Ini.DispStrips = FALSE;
	mst_Ini.DispLOD = FALSE;
    mst_Ini.ul_WPFlags = 0;
    mst_Ini.f_WPSize = 1.0f;
    mst_Ini.ul_MorphVectorColor[0] = 0xFF000000;
    mst_Ini.ul_MorphVectorColor[1] = 0xFFFFFFFF;
    mst_Ini.ul_SelFlags = 0;
    mst_Ini.c_Teleport_KeepRelativePos = 1;
    mst_Ini.c_DuplicateExtended = 0;
    mst_Ini.c_ShowHistogramme = 0;
    mst_Ini.f_VertexWeldThresh = 1.0;
    
    mst_Ini.ul_FaceIDColor[  0 ] = 0xFFFFFFFF;
    mst_Ini.ul_FaceIDColor[  1 ] = 0xFFFFFF00;
    mst_Ini.ul_FaceIDColor[  2 ] = 0xFFFF00FF;
    mst_Ini.ul_FaceIDColor[  3 ] = 0xFF00FFFF;
    mst_Ini.ul_FaceIDColor[  4 ] = 0xFFFF0000;
    mst_Ini.ul_FaceIDColor[  5 ] = 0xFF00FF00;
    mst_Ini.ul_FaceIDColor[  6 ] = 0xFF0000FF;
    mst_Ini.ul_FaceIDColor[  7 ] = 0xFFCFCF00;
    mst_Ini.ul_FaceIDColor[  8 ] = 0xFFCF00CF;
    mst_Ini.ul_FaceIDColor[  9 ] = 0xFF00CFCF;
    mst_Ini.ul_FaceIDColor[ 10 ] = 0xFFCF0000;
    mst_Ini.ul_FaceIDColor[ 11 ] = 0xFF00CF00;
    mst_Ini.ul_FaceIDColor[ 12 ] = 0xFF0000CF;
    mst_Ini.ul_FaceIDColor[ 13 ] = 0xFF9F9F00;
    mst_Ini.ul_FaceIDColor[ 14 ] = 0xFF9F009F;
    mst_Ini.ul_FaceIDColor[ 15 ] = 0xFF009F9F;
    mst_Ini.ul_FaceIDColor[ 16 ] = 0xFF9F0000;
    mst_Ini.ul_FaceIDColor[ 17 ] = 0xFF009F00;
    mst_Ini.ul_FaceIDColor[ 18 ] = 0xFF00009F;
    mst_Ini.ul_FaceIDColor[ 19 ] = 0xFF6F6F00;
    mst_Ini.ul_FaceIDColor[ 20 ] = 0xFF6F006F;
    mst_Ini.ul_FaceIDColor[ 21 ] = 0xFF006F6F;
    mst_Ini.ul_FaceIDColor[ 22 ] = 0xFF6F0000;
    mst_Ini.ul_FaceIDColor[ 23 ] = 0xFF006F00;
    mst_Ini.ul_FaceIDColor[ 24 ] = 0xFF00006F;
    mst_Ini.ul_FaceIDColor[ 25 ] = 0xFF3F3F00;
    mst_Ini.ul_FaceIDColor[ 26 ] = 0xFF3F003F;
    mst_Ini.ul_FaceIDColor[ 27 ] = 0xFF003F3F;
    mst_Ini.ul_FaceIDColor[ 28 ] = 0xFF3F0000;
    mst_Ini.ul_FaceIDColor[ 29 ] = 0xFF003F00;
    mst_Ini.ul_FaceIDColor[ 30 ] = 0xFF00003F;
    mst_Ini.ul_FaceIDColor[ 31 ] = 0xFF000000;
    
    L_memset( &mst_Ini.ul_ColorStock, 0, sizeof(mst_Ini.ul_ColorStock) );
    
    /*mst_Ini.ul_MRMLevelNb = 4;
    mst_Ini.f_MRMLevels[ 0 ] = .8f;
    mst_Ini.f_MRMLevels[ 1 ] = .5f;
    mst_Ini.f_MRMLevels[ 2 ] = .2f;
    mst_Ini.f_MRMLevels[ 3 ] = 0.f;
    mst_Ini.f_MRMLevels[ 4 ] = 0.f;*/

    mst_Ini.f_CamWheelFactor = 1.0f;

	mst_Ini.i_EditOptions_SymetrieOp = 0;

#ifdef JADEFUSION
	//reinit highlightcolors
	mst_Ini.ul_HighlightColors[0] = 0xFFFFFF00;
	mst_Ini.ul_HighlightColors[1] = 0xFFFF00FF;
	mst_Ini.ul_HighlightColors[2] = 0xFF00FFFF;
	mst_Ini.ul_HighlightColors[3] = 0xFFFF0000;
	mst_Ini.ul_HighlightColors[4] = 0xFF00FF00;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::LoadIni(void)
{
_Try_
    EDI_cl_BaseFrame::BaseLoadIni(mst_Def.asz_Name, (UCHAR *) &mst_Ini, sizeof(mst_Ini));
_Catch_
_End_
    TreatIni();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::TreatIni(void)
{
    int i, j;

    if ( (mst_Ini.ul_Mark != 0x1310DA7A) || (mst_Ini.ul_Version < 12 ) )
        ReinitIni();
        
    if (mst_Ini.ul_Version == 12 )
    {
		mst_Ini.ul_Version = 13;
		mst_Ini.c_Teleport_KeepRelativePos = 1;
    }
    
    if (mst_Ini.ul_Version == 13 )
    {
		mst_Ini.ul_Version = 14;
		mst_Ini.c_DuplicateExtended = 0;
    }
    
    if (mst_Ini.ul_Version == 14 )
    {
		mst_Ini.ul_Version = 15;
		mst_Ini.c_ShowHistogramme = 0;
    }
    
    if (mst_Ini.ul_Version == 15 )
    {
		mst_Ini.ul_Version = 16;
		mst_Ini.ul_FaceIDColor[  0 ] = 0xFFFFFFFF;
		mst_Ini.ul_FaceIDColor[  1 ] = 0xFFFFFF00;
		mst_Ini.ul_FaceIDColor[  2 ] = 0xFFFF00FF;
		mst_Ini.ul_FaceIDColor[  3 ] = 0xFF00FFFF;
		mst_Ini.ul_FaceIDColor[  4 ] = 0xFFFF0000;
		mst_Ini.ul_FaceIDColor[  5 ] = 0xFF00FF00;
		mst_Ini.ul_FaceIDColor[  6 ] = 0xFF0000FF;
		mst_Ini.ul_FaceIDColor[  7 ] = 0xFFCFCF00;
		mst_Ini.ul_FaceIDColor[  8 ] = 0xFFCF00CF;
		mst_Ini.ul_FaceIDColor[  9 ] = 0xFF00CFCF;
		mst_Ini.ul_FaceIDColor[ 10 ] = 0xFFCF0000;
		mst_Ini.ul_FaceIDColor[ 11 ] = 0xFF00CF00;
		mst_Ini.ul_FaceIDColor[ 12 ] = 0xFF0000CF;
		mst_Ini.ul_FaceIDColor[ 13 ] = 0xFF9F9F00;
		mst_Ini.ul_FaceIDColor[ 14 ] = 0xFF9F009F;
		mst_Ini.ul_FaceIDColor[ 15 ] = 0xFF009F9F;
		mst_Ini.ul_FaceIDColor[ 16 ] = 0xFF9F0000;
		mst_Ini.ul_FaceIDColor[ 17 ] = 0xFF009F00;
		mst_Ini.ul_FaceIDColor[ 18 ] = 0xFF00009F;
		mst_Ini.ul_FaceIDColor[ 19 ] = 0xFF6F6F00;
		mst_Ini.ul_FaceIDColor[ 20 ] = 0xFF6F006F;
		mst_Ini.ul_FaceIDColor[ 21 ] = 0xFF006F6F;
		mst_Ini.ul_FaceIDColor[ 22 ] = 0xFF6F0000;
		mst_Ini.ul_FaceIDColor[ 23 ] = 0xFF006F00;
		mst_Ini.ul_FaceIDColor[ 24 ] = 0xFF00006F;
		mst_Ini.ul_FaceIDColor[ 25 ] = 0xFF3F3F00;
		mst_Ini.ul_FaceIDColor[ 26 ] = 0xFF3F003F;
		mst_Ini.ul_FaceIDColor[ 27 ] = 0xFF003F3F;
		mst_Ini.ul_FaceIDColor[ 28 ] = 0xFF3F0000;
		mst_Ini.ul_FaceIDColor[ 29 ] = 0xFF003F00;
		mst_Ini.ul_FaceIDColor[ 30 ] = 0xFF00003F;
		mst_Ini.ul_FaceIDColor[ 31 ] = 0xFF000000;
    }
    
    if (mst_Ini.ul_Version == 16 )
    {
		mst_Ini.ul_Version = 17;
		L_memset( &mst_Ini.ul_ColorStock, 0, sizeof(mst_Ini.ul_ColorStock ) );
	}

	if (mst_Ini.ul_Version == 17 )
	{
		mst_Ini.ul_Version = 18;
		mst_Ini.i_EditOptions_SymetrieOp = 0;
	}

    if (mst_Ini.ul_Version == 18)
    {
		mst_Ini.ul_Version = 19;
#ifdef JADEFUSION
		mst_Ini.ul_HighlightColors[0] = 0xFFFFFF00;
		mst_Ini.ul_HighlightColors[1] = 0xFFFF00FF;
		mst_Ini.ul_HighlightColors[2] = 0xFF00FFFF;
		mst_Ini.ul_HighlightColors[3] = 0xFFFF0000;
		mst_Ini.ul_HighlightColors[4] = 0xFF00FF00;
#endif
		
		mst_Ini.f_SnapGridSizeZ = 1.f;
    }
      

    DDD()->uc_EngineCamera = mst_Ini.b_EngineCam ? 1 : 0;
    DDD()->ul_DisplayFlags = mst_Ini.ul_DisplayFlags;
    DDD()->ul_DrawMask = mst_Ini.ul_DrawMask ;//& ~GDI_Cul_DM_NoAutoClone; // Pour activer les clones tout le temps /*& ~GDI_Cul_DM_TestBackFace;*/;
    DDD()->ul_ColorConstant = mst_Ini.ul_ConstantColor;
    
    DDD()->pst_Helpers->ul_Flags |= (mst_Ini.b_SnapToGrid ? SOFT_Cul_HF_SnapGrid : 0 );
    DDD()->pst_Helpers->f_GridSizeXY= mst_Ini.f_SnapGridSizeXY;
    DDD()->pst_Helpers->f_GridSizeZ= mst_Ini.f_SnapGridSizeZ;
    DDD()->pst_Helpers->ul_Flags |= (mst_Ini.b_SnapAngle ? SOFT_Cul_HF_SnapAngle : 0 );
    DDD()->pst_Helpers->f_AngleSnap = mst_Ini.f_SnapAngleStep;
    DDD()->pst_Helpers->ul_Flags |= (mst_Ini.b_SnapGridDisplay ? SOFT_Cul_HF_SnapGridRender : 0 );
    DDD()->pst_Helpers->f_GridWidth = mst_Ini.f_SnapGridWidth;
    
    DDD()->st_ScreenFormat.ul_Flags = mst_Ini.ul_ScreenFlags;
    DDD()->st_ScreenFormat.f_PixelYoverX = mst_Ini.f_PixelYoverX;
    DDD()->st_ScreenFormat.f_ScreenYoverX = mst_Ini.f_ScreenYoverX;
    DDD()->st_ScreenFormat.l_ScreenRatioConst = mst_Ini.l_ScreenFormat;
    DDD()->pst_PickingBuffer->ul_UserFlags = mst_Ini.ul_PickingBufferFlags & 0x8;
    
    L_memcpy( DDD()->pst_EditOptions->ul_VertexColor, mst_Ini.ul_VertexColor, 32 );
    L_memcpy( DDD()->pst_EditOptions->ul_EdgeColor, mst_Ini.ul_EdgeColor, 32 );
    L_memcpy( DDD()->pst_EditOptions->ul_FaceColor, mst_Ini.ul_FaceColor, 32 );
    L_memcpy( DDD()->pst_EditOptions->ul_NormalColor, mst_Ini.ul_NormalColor, 16 );

    DDD()->pst_EditOptions->f_ScaleForNormals = mst_Ini.f_NormalLength;
    DDD()->pst_EditOptions->f_VertexSize      = mst_Ini.f_VertexSize;
    DDD()->pst_EditOptions->f_FaceSize        = mst_Ini.f_FaceSize;
    DDD()->pst_EditOptions->f_EdgeSize        = mst_Ini.f_EdgeSize;
    DDD()->pst_EditOptions->f_NormalSize      = mst_Ini.f_NormalSize;

    DDD()->pst_EditOptions->f_UVWeldThresh    = mst_Ini.f_UVWeldThresh;
    DDD()->pst_EditOptions->f_VertexWeldThresh= mst_Ini.f_VertexWeldThresh;

    //DDD()->pst_EditOptions->ul_MRMLevelNb = mst_Ini.ul_MRMLevelNb;
    //L_memcpy(DDD()->pst_EditOptions->f_MRMLevels,mst_Ini.f_MRMLevels,sizeof(mst_Ini.f_MRMLevels));

    DDD()->pst_EditOptions->ul_WiredColorSel  = mst_Ini.ul_WiredColorSel;
    DDD()->pst_EditOptions->ul_WiredColor     = mst_Ini.ul_WiredColor;
    L_memcpy( DDD()->pst_EditOptions->ul_UVMapperColor, mst_Ini.ul_UVMapperColors, 14 * 4 );
    L_memcpy( DDD()->pst_EditOptions->ul_FaceIDColor, mst_Ini.ul_FaceIDColor, 32 * 4 );
    L_memcpy( DDD()->pst_EditOptions->ul_ColorStock, mst_Ini.ul_ColorStock, sizeof( mst_Ini.ul_ColorStock ) );

    DDD()->pst_EditOptions->ul_MorphVectorColor[0] = mst_Ini.ul_MorphVectorColor[0];
    DDD()->pst_EditOptions->ul_MorphVectorColor[1] = mst_Ini.ul_MorphVectorColor[1];
	DDD()->pst_EditOptions->i_SymetrieOp = mst_Ini.i_EditOptions_SymetrieOp;
    
	mpo_EngineFrame->mb_LeftSel = mst_Ini.uc_SelDialogLeft ? TRUE : FALSE;
	mpo_EngineFrame->mb_SelMinimize = mst_Ini.ul_SelMinimize ? TRUE : FALSE;

	DDD()->pst_GridOptions->f_Z = mst_Ini.f_GridDeltaZ;
    DDD()->pst_GridOptions->ul_Flags = mst_Ini.ul_GridFlags;
    DDD()->pst_GridOptions->ul_KeyForLockedZ = mst_Ini.ul_GridKeyForZLocked;

	ENG_gb_DistCompute = mst_Ini.DistCompute;

    if (mst_Ini.ul_BackgroundImage != 0xFFFFFFFF )
    {
        SOFT_BackgroundImage_Load( DDD()->pst_BackgroundImage, mst_Ini.ul_BackgroundImage );
    }


    i = mst_Ini.ul_TextureManager_MaxSize & 0x7FFFFFFF;
    j = -1;
    while (i)
    {
        i >>= 1;
        j++;
    }

    i = mst_Ini.ul_TextureManager_MaxSize;
    if ( (i < 2) || ((ULONG) (1 << j) != mst_Ini.ul_TextureManager_MaxSize) )
    {
        mst_Ini.ul_TextureManager_Flags = TEX_Manager_FastCompression | TEX_Manager_UseMipmap | TEX_Manager_Accept32bpp | TEX_Manager_RGB2BGR/*| TEX_Manager_Accept8bpp */;
        mst_Ini.ul_TextureManager_MaxSize = 512;
        mst_Ini.ul_TextureManager_MaxMem = 16 * 1024 * 1024;
    }
#if !defined(_XENON_RENDER)
    DDD()->st_TexManager.ul_Flags = mst_Ini.ul_TextureManager_Flags;
    /* 
     * force flags with command line switch : £
     * TEX_Manager_Accept8bpp or £
     * TEX_Manager_Accept4bpp or £
     * TEX_Manager_AcceptAlphaPalette
     */
    DDD()->st_TexManager.ul_Flags  |= TEX_gi_ForceText;

    DDD()->st_TexManager.l_MaxTextureSize = mst_Ini.ul_TextureManager_MaxSize;
    DDD()->st_TexManager.l_MinTextureSize = mst_Ini.ul_TextureManager_MinSize;
    DDD()->st_TexManager.l_VRamAvailable = mst_Ini.ul_TextureManager_MaxMem;
    DDD()->st_TexManager.l_RamAvailable = mst_Ini.ul_TextureManager_MaxMemInterface;
#endif

	switch(mst_Ini.ul_BoxDlg)
	{
	case 1:
		DP()->Selection_WithDialog();
		break;
	case 2:
		DP()->Selection_ToolBox();
		break;
	}

    DDD()->f_WPSize = mst_Ini.f_WPSize;
    DDD()->ul_WPFlags = mst_Ini.ul_WPFlags;
    DDD()->ShowHistogramm = mst_Ini.c_ShowHistogramme;
#ifdef JADEFUSION
	L_memcpy( DDD()->st_HighlightColors.ul_Colors, mst_Ini.ul_HighlightColors, sizeof(mst_Ini.ul_HighlightColors) );
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::SaveIni(void)
{
    if(DP()->mst_WinHandles.pst_DisplayData)
    {
        mst_Ini.ul_DisplayFlags = DDD()->ul_DisplayFlags;
        mst_Ini.ul_DrawMask = DDD()->ul_DrawMask ;
        mst_Ini.ul_ConstantColor = DDD()->ul_ColorConstant;
        
        mst_Ini.b_SnapToGrid = (DDD()->pst_Helpers->ul_Flags & SOFT_Cul_HF_SnapGrid) ? TRUE : FALSE;
        mst_Ini.f_SnapGridSizeXY = DDD()->pst_Helpers->f_GridSizeXY;
        mst_Ini.f_SnapGridSizeZ = DDD()->pst_Helpers->f_GridSizeZ;
        mst_Ini.b_SnapAngle = (DDD()->pst_Helpers->ul_Flags & SOFT_Cul_HF_SnapAngle) ? TRUE : FALSE;
        mst_Ini.f_SnapAngleStep = DDD()->pst_Helpers->f_AngleSnap;
        mst_Ini.b_SnapGridDisplay = (DDD()->pst_Helpers->ul_Flags & SOFT_Cul_HF_SnapGridRender) ? TRUE : FALSE;
        mst_Ini.f_SnapGridWidth = DDD()->pst_Helpers->f_GridWidth;
        
        mst_Ini.ul_ScreenFlags = DDD()->st_ScreenFormat.ul_Flags;
        mst_Ini.f_PixelYoverX = DDD()->st_ScreenFormat.f_PixelYoverX;
        mst_Ini.f_ScreenYoverX = DDD()->st_ScreenFormat.f_ScreenYoverX;
        mst_Ini.l_ScreenFormat = DDD()->st_ScreenFormat.l_ScreenRatioConst;
		mst_Ini.ul_SelMinimize = mpo_EngineFrame->mb_SelMinimize;
        mst_Ini.ul_PickingBufferFlags = DDD()->pst_PickingBuffer->ul_UserFlags;
        
        L_memcpy( mst_Ini.ul_VertexColor, DDD()->pst_EditOptions->ul_VertexColor, 32 );
        L_memcpy( mst_Ini.ul_EdgeColor, DDD()->pst_EditOptions->ul_EdgeColor,  32 );
        L_memcpy( mst_Ini.ul_FaceColor, DDD()->pst_EditOptions->ul_FaceColor,  32 );
        L_memcpy( mst_Ini.ul_NormalColor, DDD()->pst_EditOptions->ul_NormalColor, 16 );
        mst_Ini.f_NormalLength =  DDD()->pst_EditOptions->f_ScaleForNormals;
        mst_Ini.f_VertexSize   =  DDD()->pst_EditOptions->f_VertexSize;     
        mst_Ini.f_FaceSize     =  DDD()->pst_EditOptions->f_FaceSize;      
        mst_Ini.f_EdgeSize     =  DDD()->pst_EditOptions->f_EdgeSize;        
        mst_Ini.f_NormalSize   =  DDD()->pst_EditOptions->f_NormalSize;      
        mst_Ini.f_UVWeldThresh =  DDD()->pst_EditOptions->f_UVWeldThresh;      
        mst_Ini.f_VertexWeldThresh =  DDD()->pst_EditOptions->f_VertexWeldThresh;      
        mst_Ini.ul_WiredColorSel = DDD()->pst_EditOptions->ul_WiredColorSel;
        mst_Ini.ul_WiredColor    = DDD()->pst_EditOptions->ul_WiredColor;
        L_memcpy( mst_Ini.ul_UVMapperColors, DDD()->pst_EditOptions->ul_UVMapperColor, 14 * 4 );
        L_memcpy( mst_Ini.ul_FaceIDColor, DDD()->pst_EditOptions->ul_FaceIDColor, 32 * 4 );
        L_memcpy( mst_Ini.ul_ColorStock, DDD()->pst_EditOptions->ul_ColorStock, sizeof(mst_Ini.ul_ColorStock) );
		mst_Ini.i_EditOptions_SymetrieOp = DDD()->pst_EditOptions->i_SymetrieOp;

        //mst_Ini.ul_MRMLevelNb = DDD()->pst_EditOptions->ul_MRMLevelNb;
        //L_memcpy(mst_Ini.f_MRMLevels,DDD()->pst_EditOptions->f_MRMLevels,sizeof(mst_Ini.f_MRMLevels));

        mst_Ini.ul_MorphVectorColor[0] = DDD()->pst_EditOptions->ul_MorphVectorColor[0];
        mst_Ini.ul_MorphVectorColor[1] = DDD()->pst_EditOptions->ul_MorphVectorColor[1];
        
        mst_Ini.f_GridDeltaZ = DDD()->pst_GridOptions->f_Z;
        mst_Ini.ul_GridFlags = DDD()->pst_GridOptions->ul_Flags;
        mst_Ini.ul_GridKeyForZLocked = DDD()->pst_GridOptions->ul_KeyForLockedZ;

        mst_Ini.ul_BackgroundImage = DDD()->pst_BackgroundImage->ul_Key;
        
        mst_Ini.ul_TextureManager_Flags = DDD()->st_TexManager.ul_Flags;
        mst_Ini.ul_TextureManager_MaxSize = DDD()->st_TexManager.l_MaxTextureSize;
        mst_Ini.ul_TextureManager_MinSize = DDD()->st_TexManager.l_MinTextureSize;
        mst_Ini.ul_TextureManager_MaxMem = DDD()->st_TexManager.l_VRamAvailable;
        mst_Ini.ul_TextureManager_MaxMemInterface = DDD()->st_TexManager.l_RamAvailable;

		if(DP()->mb_SelectOn && DP()->mpo_SelectionDialog) 
			mst_Ini.ul_BoxDlg = 1;
		else if(DP()->mpo_ToolBoxDialog && DP()->mb_ToolBoxOn)
			mst_Ini.ul_BoxDlg = 2;       
		else
			mst_Ini.ul_BoxDlg = 0;

        mst_Ini.f_WPSize = DDD()->f_WPSize;
        mst_Ini.ul_WPFlags = DDD()->ul_WPFlags;
        mst_Ini.c_ShowHistogramme = DDD()->ShowHistogramm;
#ifdef JADEFUSION
		L_memcpy( mst_Ini.ul_HighlightColors, DDD()->st_HighlightColors.ul_Colors, sizeof(mst_Ini.ul_HighlightColors));
#endif
    }

_Try_
    EDI_cl_BaseFrame::BaseSaveIni(mst_Def.asz_Name, (UCHAR *) &mst_Ini, sizeof(mst_Ini));
_Catch_
_End_
}

#endif /* ACTIVE_EDITORS */
