/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS

#include "EDIbaseview.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGread.h"
#include "TEXframe.h"
#include "TEXframe_act.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "DIAlogs/CCheckList.h"
#include "TEXture/TEXstruct.h"
#include "TEXture/TEXeditorfct.h"

#ifdef JADEFUSION
void Xe_GenerateAllDds();
#endif
/*
 ===================================================================================================
 ===================================================================================================
 */
#define M_SwapShowFlag( FLAG ) \
	( mst_Ini.l_ShowFlag & FLAG ) ? (mst_Ini.l_ShowFlag -= FLAG) : (mst_Ini.l_ShowFlag |= FLAG);

void ETEX_cl_Frame::OnAction(ULONG _ul_Action)
{
    switch(_ul_Action)
    {
    case ETEX_ACTION_ALPHA:
        OnAlpha();
        break;

    case ETEX_ACTION_SHOWNAME:
		M_SwapShowFlag( ETEX_C_ShowName );
        Refresh();
        break;

	case ETEX_ACTION_TEXSHOWALL:
		M_SwapShowFlag( ETEX_C_TexShowAllSlot );
        Browse();
        break;

	case ETEX_ACTION_TEXUSETMORDER:
		M_SwapShowFlag( ETEX_C_TexUseTMSlotOrder );
        Browse();
        break;

	case ETEX_ACTION_TEXSHOWTC:
		M_SwapShowFlag( ETEX_C_TexShowTga );
        Browse();
        break;

	case ETEX_ACTION_TEXSHOWRAWPAL:
		M_SwapShowFlag( ETEX_C_TexShowRawPal );
        Browse();
        break;

	case ETEX_ACTION_TEXSLOTPS2:
		M_SwapShowFlag( ETEX_C_TexShowSlotPS2 );
        Browse();
        break;

	case ETEX_ACTION_TEXSLOTGC:
		M_SwapShowFlag( ETEX_C_TexShowSlotGC );
        Browse();
        break;

	case ETEX_ACTION_TEXSLOTXBOX:
		M_SwapShowFlag( ETEX_C_TexShowSlotXBOX );
        Browse();
        break;

	case ETEX_ACTION_TEXSLOTPC:
		M_SwapShowFlag( ETEX_C_TexShowSlotPC );
        Browse();
        break;

    case ETEX_ACTION_CLOSE:
        OnClose();
        break;

    case ETEX_ACTION_RESFREE:
        OnChooseResolution(0);
        break;

    case ETEX_ACTION_RES32_32:
        OnChooseResolution(1);
        break;

    case ETEX_ACTION_RES64_64:
        OnChooseResolution(2);
        break;

    case ETEX_ACTION_RES128_128:
        OnChooseResolution(3);
        break;
    
    case ETEX_ACTION_RES256_256:
        OnChooseResolution(4);
        break;

    case ETEX_ACTION_FORCERES:
        OnForceRes();
        break;

    case ETEX_ACTION_CHECKUNIQUENAME:
        Check_DoubleName();
        break;

    case ETEX_ACTION_CHECKLOADED:
        Check_Loaded();
        break;

    case ETEX_ACTION_CHECKTEXFILE:
        Check_TexFile();
        break;

    case ETEX_ACTION_CHECKPALETTE:
        Check_Palette();
        break;

    case ETEX_ACTION_CHECKBADPARAMS:
        Check_BadParams();
        break;

    case ETEX_ACTION_CHECKFONT:
        Check_Font();
        break;

    case ETEX_ACTION_CHECKFORUSE:
        Check_User();
        break;

	case ETEX_ACTION_CHECKFORDUPLICATERAW:
        Check_DuplicateRaw();
        break;

    case ETEx_ACTION_EXPORTTEX2TGA:
		{
			ULONG				ul_Dir;
			EDIA_cl_FileDialog	o_DirOnDisk("Choose directory to import in", 2, FALSE);
			EDIA_cl_FileDialog	o_Dir("Choose directory to import from", 2, FALSE, TRUE);
			CCheckList			o_Check;
			BOOL				ab_Flags[ 16 ];
			CRect				o_Rect;
			int					i;
			
			if (o_Dir.DoModal() != IDOK)
				break;
			ul_Dir = BIG_ul_SearchDir( (char *) (LPCSTR) o_Dir.masz_FullPath );

			if (o_DirOnDisk.DoModal() != IDOK)
				break;
				
			o_Check.AddString( "recursive" );			ab_Flags[ 0 ] = 1;
			o_Check.AddString( "tga with palette" );	ab_Flags[ 1 ] = 0;
			for (i = 0; i < ETEX_C_SlotNb; i++)
			{
				o_Check.AddString( ETEX_gsz_SlotName[ i ] );
				ab_Flags[ 2 + i ] = (i == 0 ) ? 1 : 0;
			}
			
			mpo_MyView->GetWindowRect(&o_Rect);
			o_Rect.left += (o_Rect.Width() / 2) - 100;
			o_Rect.top += (o_Rect.Height() / 2) - 100;
			o_Check.SetArrayBool( ab_Flags );
			o_Check.Do( &o_Rect, 200, this, 20, "export flags");
			
			TEX_4Edit_ExportTga( ul_Dir, o_DirOnDisk.masz_FullPath, ab_Flags[ 0 ], ab_Flags[ 1 ], ab_Flags + 2, ETEX_gsz_SlotName ); 
		}
		break;

#ifdef JADEFUSION
	case ETEx_ACTION_CREATEALLDDS:
		Xe_GenerateAllDds();
		break;
#endif
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL ETEX_cl_Frame::b_OnActionValidate(ULONG _ul_Action, BOOL)
{
    switch(_ul_Action)
    {
    case ETEX_ACTION_CLOSE:
        if((mul_DirIndex == BIG_C_InvalidIndex) && (mul_FileIndex == BIG_C_InvalidIndex))
            return FALSE;
        break;
	}

    return TRUE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
UINT ETEX_cl_Frame::ui_OnActionState(ULONG _ul_Action)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    UINT    mui_State;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    mui_State = (UINT) - 1;

    switch(_ul_Action)
    {
    case ETEX_ACTION_ALPHA:
        mui_State = DFCS_BUTTONCHECK | ( (mst_Ini.b_AlphaShow) ? DFCS_CHECKED : 0);
        break;

    case ETEX_ACTION_SHOWNAME:
        mui_State = DFCS_BUTTONCHECK | ( (mst_Ini.l_ShowFlag & ETEX_C_ShowName) ? DFCS_CHECKED : 0 );
        break;

	case ETEX_ACTION_TEXSHOWALL:
		mui_State = DFCS_BUTTONCHECK | ( (mst_Ini.l_ShowFlag & ETEX_C_TexShowAllSlot) ? DFCS_CHECKED : 0 );
        break;

	case ETEX_ACTION_TEXUSETMORDER:
		mui_State = DFCS_BUTTONCHECK | ( (mst_Ini.l_ShowFlag & ETEX_C_TexUseTMSlotOrder) ? DFCS_CHECKED : 0 );
        break;

	case ETEX_ACTION_TEXSHOWTC:
		mui_State = DFCS_BUTTONCHECK | ( (mst_Ini.l_ShowFlag & ETEX_C_TexShowTga) ? DFCS_CHECKED : 0 );
        break;

	case ETEX_ACTION_TEXSHOWRAWPAL:
		mui_State = DFCS_BUTTONCHECK | ( (mst_Ini.l_ShowFlag & ETEX_C_TexShowRawPal) ? DFCS_CHECKED : 0 );
        break;

	case ETEX_ACTION_TEXSLOTPS2:
		mui_State = DFCS_BUTTONCHECK | ( (mst_Ini.l_ShowFlag & ETEX_C_TexShowSlotPS2) ? DFCS_CHECKED : 0 );
        break;

	case ETEX_ACTION_TEXSLOTGC:
		mui_State = DFCS_BUTTONCHECK | ( (mst_Ini.l_ShowFlag & ETEX_C_TexShowSlotGC) ? DFCS_CHECKED : 0 );
        break;

	case ETEX_ACTION_TEXSLOTXBOX:
		mui_State = DFCS_BUTTONCHECK | ( (mst_Ini.l_ShowFlag & ETEX_C_TexShowSlotXBOX) ? DFCS_CHECKED : 0 );
        break;

	case ETEX_ACTION_TEXSLOTPC:
		mui_State = DFCS_BUTTONCHECK | ( (mst_Ini.l_ShowFlag & ETEX_C_TexShowSlotPC) ? DFCS_CHECKED : 0 );
        break;

    case ETEX_ACTION_RESFREE:
        mui_State = DFCS_BUTTONRADIO;
        if(mst_Ini.i_Res == 0)
            mui_State |= DFCS_CHECKED;
        break;

    case ETEX_ACTION_RES32_32:
        mui_State = DFCS_BUTTONRADIO;
        if(mst_Ini.i_Res == 1)
            mui_State |= DFCS_CHECKED;
        break;

    case ETEX_ACTION_RES64_64:
        mui_State = DFCS_BUTTONRADIO;
        if(mst_Ini.i_Res == 2)
            mui_State |= DFCS_CHECKED;
        break;

    case ETEX_ACTION_RES128_128:
        mui_State = DFCS_BUTTONRADIO;
        if(mst_Ini.i_Res == 3)
            mui_State |= DFCS_CHECKED;
        break;
    
    case ETEX_ACTION_RES256_256:
        mui_State = DFCS_BUTTONRADIO;
        if(mst_Ini.i_Res == 4)
            mui_State |= DFCS_CHECKED;
        break;

    case ETEX_ACTION_FORCERES:
        mui_State = DFCS_BUTTONCHECK | ( (mst_Ini.b_ForceRes) ? DFCS_CHECKED : 0);
        break;
    }

    return mui_State;
}

#endif

