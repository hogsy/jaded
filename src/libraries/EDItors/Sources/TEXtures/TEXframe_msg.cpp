/*$T TEXframe_msg.cpp GC!1.62 12/27/99 12:50:04 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "TEXscroll.h"
#include "TEXframe.h"
#include "EDImsg.h"
#include "EDImainframe.h"
#include "EDIpaths.h"
#include "EDItors/Sources/BROwser/BROmsg.h"
#include "ENGine/Sources/ENGinit.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ETEX_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDI_tdst_DragDrop           *pst_DragDrop;
    CPoint                      pt;
    POSITION                    pos;
    MAIEDITEX_tdst_BitmapDes    *pdes;
    int                         i_Msg;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    switch(_ul_Msg)
    {
    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_FATHASCHANGED:
        if((mul_DirIndex == BIG_C_InvalidIndex) && (mul_FileIndex == BIG_C_InvalidIndex))
            return 0;

        /* Have we deleted the single view file ? */
        if(EDI_go_FHCFile.Lookup(mul_FileIndex, i_Msg))
        {
            switch(i_Msg)
            {
            case EDI_FHC_Delete:
            case EDI_FHC_Move:
                OnClose();
                return TRUE;
            }
        }

        /* Else search dir */
        if(EDI_go_FHCDir.Lookup(mul_DirIndex, i_Msg))
        {
            switch(i_Msg)
            {
            case EDI_FHC_Delete:
                mul_DirIndex = BIG_C_InvalidIndex;
                break;
            }

            Browse();
        }

        break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANDRAGDROP:
        pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
        //if(pst_DragDrop->po_SourceEditor == this)
        //    return FALSE;
		/* another thing that file or directory ? */
        if(pst_DragDrop->i_Type != EDI_DD_File)
            return FALSE;
		/* directory ? */
		if ( pst_DragDrop->ul_FatFile == BIG_C_InvalidIndex ) 
			return TRUE;
		/* supported file */
		i_Msg = (pst_DragDrop->po_SourceEditor == this) ? 5 : 4;
		if  ( !TEX_l_File_IsFormatSupported( BIG_NameFile( pst_DragDrop->ul_FatFile ), i_Msg ) )
			return FALSE;
        return TRUE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_ENDDRAGDROP:
        pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
        mul_DirIndex = pst_DragDrop->ul_FatDir;
        mul_FileIndex = pst_DragDrop->ul_FatFile;
		
        //if ( ( GetAsyncKeyState( VK_SHIFT ) >= 0) || !b_UpdateTex( &pst_DragDrop->o_Pt ) )
        {
            Browse(pst_DragDrop->b_Copy ? FALSE : TRUE);
        }
        break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_SELDIR:
        mul_DirIndex = (BIG_INDEX) _ul_Param1;
        mul_FileIndex = BIG_C_InvalidIndex;
        Browse();
        break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANSELDATA: return FALSE;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_SELFILE:
        if(((BIG_INDEX) _ul_Param1 != mul_DirIndex) || (mul_FileIndex != BIG_C_InvalidIndex))
        {
            mul_DirIndex = (BIG_INDEX) _ul_Param1;
            if(mul_FileIndex != BIG_C_InvalidIndex)
                mul_FileIndex = (BIG_INDEX) _ul_Param2;
            Browse();
        }

        pos = mpo_ScrollView->mo_BmpList.GetHeadPosition();
        if(!pos) return FALSE;
        pt.x = mpo_ScrollView->GetScrollPos(SB_HORZ);
        pt.y = mpo_ScrollView->GetScrollPos(SB_VERT);
        while(pos)
        {
            pdes = mpo_ScrollView->mo_BmpList.GetNext(pos);
            if(pdes->ul_FatFile == (BIG_INDEX) _ul_Param2)
            {
                pt.x += pdes->o_Rect.left + OFFSET;
                pt.y += pdes->o_Rect.top + OFFSET;
                pt.x -= mpo_ScrollView->GetScrollPos(SB_HORZ);
                pt.y -= mpo_ScrollView->GetScrollPos(SB_VERT);
                mpo_ScrollView->OnLButtonDown(-1, pt);
                break;
            }
        }

        break;
    }

    return TRUE;
}

#endif /* ACTIVE_EDITORS */
