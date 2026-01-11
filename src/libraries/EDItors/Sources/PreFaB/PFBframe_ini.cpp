/*$T TEXframe_ini.cpp GC!1.26 04/22/99 14:38:49 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGfat.h"
#include "PFBframe.h"
#include "PFBtreeview.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
void EPFB_cl_Frame::ReinitIni(void)
{
    EDI_cl_BaseFrame::ReinitIni();
    
    mst_Ini.i_Version = 6;
    mst_Ini.i_Pane0Width = 150;
    mst_Ini.i_Pane0Height = 150;
    mst_Ini.b_SelIsDir = TRUE;
    mst_Ini.ul_DirIndex = BIG_C_InvalidIndex;
    mst_Ini.ul_FileKey = BIG_C_InvalidKey;
    mst_Ini.i_SaveAuto = 0;
    *mst_Ini.sz_DirName = 0;
    mst_Ini.i_VerticalLayout = 0;
    mst_Ini.i_DisplayPoint = 1;
	mst_Ini.i_DisplayBV = 1;
	mst_Ini.i_MoveToPrefabMap = 0;
	mst_Ini.ul_PrefabMapIndex = BIG_C_InvalidIndex;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EPFB_cl_Frame::LoadIni(void)
{
	L_memset( &mst_Ini, 0, sizeof(mst_Ini) );
_Try_
    EDI_cl_BaseFrame::BaseLoadIni(mst_Def.asz_Name, (UCHAR *) &mst_Ini, sizeof(mst_Ini));
_Catch_
_End_
	/* version 1 to version 2 */
	if (mst_Ini.i_Version == 1)
	{
		mst_Ini.i_Version = 2;
		mst_Ini.i_SaveAuto = 0;
	}
	/* version 2 to version 3 */
	if (mst_Ini.i_Version == 2 )
	{
		mst_Ini.i_Version = 3;
		if (mst_Ini.ul_FileKey != BIG_C_InvalidIndex )
			mst_Ini.ul_FileKey = BIG_FileKey( mst_Ini.ul_FileKey );
		if ( mst_Ini.ul_DirIndex != BIG_C_InvalidIndex )
			BIG_ComputeFullName( mst_Ini.ul_DirIndex, mst_Ini.sz_DirName );
		else
			*mst_Ini.sz_DirName = 0;
	}
	/* version 3 to version 4 */
	if (mst_Ini.i_Version == 3)
	{
		mst_Ini.i_Version = 4;
		mst_Ini.i_Pane0Height = 150;
		mst_Ini.i_VerticalLayout = 0;
	}
	/* version 4 to version 5 */
	if (mst_Ini.i_Version == 4 )
	{
		mst_Ini.i_Version = 5;
		mst_Ini.i_DisplayPoint = 1;
		mst_Ini.i_DisplayBV = 1;
	}
	/* version 5 to version 6 */
	if (mst_Ini.i_Version == 5 )
	{
		mst_Ini.i_Version = 6;
		mst_Ini.i_MoveToPrefabMap = 0;
		mst_Ini.ul_PrefabMapIndex = BIG_C_InvalidIndex;
	}
	
	
	
	if (mst_Ini.i_Version != 6 )
		ReinitIni();
	
    TreatIni();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EPFB_cl_Frame::TreatIni(void)
{
	mb_SelIsDir = mst_Ini.b_SelIsDir;
	
	if ( *mst_Ini.sz_DirName == 0)
		mul_DirIndex = BIG_C_InvalidIndex;
	else
		mul_DirIndex = BIG_ul_SearchDir( mst_Ini.sz_DirName );
	
	mul_FileIndex = BIG_ul_SearchKeyToFat( mst_Ini.ul_FileKey );
	mi_PrefabSaveAuto = mst_Ini.i_SaveAuto;
	
	if (mpo_Splitter)
	{
		mpo_Splitter->SetColumnInfo( 0, mst_Ini.i_Pane0Width, 100 );
		mpo_Splitter->SetRowInfo( 0, mst_Ini.i_Pane0Height, 100 );
	}
		
	/* select current dir or data */
	if ( (mb_SelIsDir) && ( mul_DirIndex != BIG_C_InvalidIndex ) )
	{
		if ( mpo_TreeView->SelDir( mul_DirIndex ) )
			return;
	}
	if ( mul_FileIndex != BIG_C_InvalidIndex )
	{
		if ( mpo_TreeView->SelFile( mul_FileIndex ) )
			mb_SelIsDir = FALSE; 
	}
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EPFB_cl_Frame::SaveIni(void)
{
	int		i_CurW, i_CurH, i_Min;
	
	if (mpo_Splitter)
	{
		mpo_Splitter->GetColumnInfo( 0, i_CurW, i_Min );
		mpo_Splitter->GetRowInfo( 0, i_CurH, i_Min );
	}
	else
		i_CurW = i_CurH = 150;
		
	mst_Ini.i_Version = 6;
	mst_Ini.i_Pane0Width = i_CurW;
	mst_Ini.i_Pane0Height = i_CurH;
	mst_Ini.b_SelIsDir = mb_SelIsDir;
	if (mul_DirIndex == BIG_C_InvalidIndex )
		*mst_Ini.sz_DirName = 0;
	else
		BIG_ComputeFullName( mul_DirIndex, mst_Ini.sz_DirName );
	mst_Ini.ul_FileKey = BIG_FileKey( mul_FileIndex );
	mst_Ini.i_SaveAuto = mi_PrefabSaveAuto;
	
_Try_
    EDI_cl_BaseFrame::BaseSaveIni(mst_Def.asz_Name, (UCHAR *) &mst_Ini, sizeof(mst_Ini));
_Catch_
_End_
}

#endif /* ACTIVE_EDITORS */
