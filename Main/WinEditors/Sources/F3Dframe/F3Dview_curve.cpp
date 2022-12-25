/*$T F3Dview_curve.cpp GC! 1.081 04/18/00 16:24:06 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "EDIapp.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dstrings.h"
#include "F3Dframe/F3Dview_undo.h"
#include "CAMera/CAMera.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "EDImainframe.h"
#include "EDImsg.h"
#include "INOut/INOkeyboard.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKmsg.h"
#include "ENGine/Sources/WORld/WOR.h"
#include "ENGine/Sources/ENGcall.h"
#include "ENGine/Sources/WORld/WORrender.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJboundingvolume.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIaccess.h"

#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/COLlision/COLconst.h"
#include "ENGine/Sources/COLlision/COLaccess.h"

#include "GEOmetric/GEOdebugobject.h"
#include "GraphicDK/Sources/CAMera/CAMstruct.h"
#include "SOFT/SOFThelper.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "Res/Res.h"
#include "BASe/MEMory/MEM.h"
#include "EDIpaths.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "MATHs/MATH.h"
#include "EDItors/Sources/OUTput/OUTframe.h"

#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEconst.h"
#include "AIinterp/Sources/Events/EVEnt_interpolationkey.h"

/*$4
 ***********************************************************************************************************************
    globals
 ***********************************************************************************************************************
 */

OBJ_tdst_GameObject			*F3D_Curve_GO;
ANI_st_GameObjectAnim	    *F3D_Curve_GOAnim;
SOFT_tdst_HelperCurve		*F3D_Curve_Helper;
EVE_tdst_Data               *F3D_Curve_Data;
EVE_tdst_ListTracks			*F3D_Curve_Events;
EVE_tdst_Track				*F3D_Curve_Track;
EVE_tdst_Track				*F3D_Curve_TrackTrans;
EVE_tdst_Track				*F3D_Curve_TrackRot;
EVE_tdst_Event				*F3D_Curve_Evt;
EVE_tdst_Event				*F3D_Curve_NextEvt;
SOFT_tdst_HelperCurvePick	*F3D_Curve_Pick;

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    update global var with current edited curve data
 =======================================================================================================================
 */
BOOL F3D_cl_View::Curve_b_GetData(void)
{
	if(mi_EditedCurve == -1) return FALSE;

	F3D_Curve_Helper = &M_F3D_Helpers->ast_Curve[mi_EditedCurve];

	F3D_Curve_GO = (OBJ_tdst_GameObject *) F3D_Curve_Helper->p_CurGO;
    F3D_Curve_Data = (EVE_tdst_Data *) F3D_Curve_Helper->p_EventData;
	F3D_Curve_Events = F3D_Curve_Data->pst_ListTracks;
	F3D_Curve_TrackTrans = (EVE_tdst_Track *) F3D_Curve_Helper->p_Track[SOFT_Cl_TrackTrans];
	F3D_Curve_TrackRot = (EVE_tdst_Track *) F3D_Curve_Helper->p_Track[SOFT_Cl_TrackRot];

	if(!F3D_Curve_TrackTrans) F3D_Curve_Helper->i_TransEvt = -1;
	if(!F3D_Curve_TrackRot) F3D_Curve_Helper->i_RotEvt = -1;

    if(F3D_Curve_Helper->ul_Flags & SOFT_Cl_AnimCurve)
    {
        if (F3D_Curve_Helper->l_PickCur == -1) return FALSE;
        F3D_Curve_Pick = F3D_Curve_Helper->dst_Pick + F3D_Curve_Helper->l_PickCur;
        F3D_Curve_GOAnim = F3D_Curve_GO->pst_Base->pst_GameObjectAnim;
        F3D_Curve_Track = F3D_Curve_Events->pst_AllTracks + F3D_Curve_Pick->i_Track;
        F3D_Curve_Evt = F3D_Curve_Track->pst_AllEvents + F3D_Curve_Pick->i_Evt;
        if (F3D_Curve_Pick->i_Evt + 1 == F3D_Curve_Track->uw_NumEvents)
            F3D_Curve_NextEvt = F3D_Curve_Track->pst_AllEvents;
        else
            F3D_Curve_NextEvt = F3D_Curve_Evt + 1;
    }
	else
    {
	    F3D_Curve_Pick = NULL;
	    if(F3D_Curve_Helper->i_TransEvt != -1)
        {
		    F3D_Curve_Evt = (EVE_tdst_Event *) F3D_Curve_TrackTrans->pst_AllEvents + F3D_Curve_Helper->i_TransEvt;
            if (F3D_Curve_Helper->i_TransEvt + 1 == F3D_Curve_TrackTrans->uw_NumEvents)
                F3D_Curve_NextEvt = F3D_Curve_TrackTrans->pst_AllEvents;
            else
                F3D_Curve_NextEvt = F3D_Curve_Evt + 1;
        }
	    else if(F3D_Curve_Helper->i_RotEvt != -1)
        {
		    F3D_Curve_Evt = (EVE_tdst_Event *) F3D_Curve_TrackRot->pst_AllEvents + F3D_Curve_Helper->i_RotEvt;
            if (F3D_Curve_Helper->i_RotEvt + 1 == F3D_Curve_TrackRot->uw_NumEvents)
                F3D_Curve_NextEvt = F3D_Curve_TrackRot->pst_AllEvents;
            else
                F3D_Curve_NextEvt = F3D_Curve_Evt + 1;
        }
	    else
		    return FALSE;
    }

	return TRUE;
}

/*
 =======================================================================================================================
    compute pos of help matrix when working with curve
 =======================================================================================================================
 */
BOOL F3D_cl_View::Curve_b_ComputeHelperMatrix(MATH_tdst_Matrix *_pst_Matrix)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Pos, *pst_Pos;
    ULONG               ul_Father;
    int                 i_NextEvt;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!Curve_b_GetData()) return FALSE;

    if (F3D_Curve_Pick)
    {
        EVE_Event_InterpolationKey_SpecialPlay(F3D_Curve_GO, F3D_Curve_Data, F3D_Curve_Pick->f_Time, F3D_Curve_Pick->i_Track, F3D_Curve_Pick->i_Evt, &EVE_st_MagicBox, EVE_ast_Gizmo);
		EVE_Event_InterpolationKey_UpdateGizmos(F3D_Curve_GO, F3D_Curve_GOAnim->pst_Skeleton, &EVE_st_MagicBox, EVE_ast_Gizmo);
        MATH_CopyMatrix( _pst_Matrix, EVE_ast_Gizmo + F3D_Curve_Track->uw_Gizmo );

        if ( F3D_Curve_Pick->i_Translation > 1)
        {
            MATH_CopyVector( &M_F3D_Helpers->st_SeparateCenter, MATH_pst_GetTranslation( _pst_Matrix ) );

            ul_Father = ANI_ul_GetFatherIndex( F3D_Curve_GOAnim->pst_Skeleton, F3D_Curve_Track->uw_Gizmo );
            pst_Pos = EVE_pst_Event_InterpolationKey_GetPos(F3D_Curve_Evt) + (F3D_Curve_Pick->i_Translation - 1);
            MATH_TransformVertex( MATH_pst_GetTranslation(_pst_Matrix) , EVE_ast_Gizmo + ul_Father, pst_Pos );

            if ( F3D_Curve_Pick->i_Translation > 2)
            {
                i_NextEvt = (F3D_Curve_Pick->i_Evt + 1 == F3D_Curve_Track->uw_NumEvents) ? 0 : F3D_Curve_Pick->i_Evt + 1;
                EVE_Event_InterpolationKey_SpecialPlay(F3D_Curve_GO, F3D_Curve_Data, F3D_Curve_Pick->f_Time + EVE_FrameToTime(F3D_Curve_Evt->uw_NumFrames), F3D_Curve_Pick->i_Track, i_NextEvt, &EVE_st_MagicBox, EVE_ast_Gizmo);
		        EVE_Event_InterpolationKey_UpdateGizmos(F3D_Curve_GO, F3D_Curve_GOAnim->pst_Skeleton, &EVE_st_MagicBox, EVE_ast_Gizmo);
                MATH_CopyVector( &M_F3D_Helpers->st_SeparateCenter, MATH_pst_GetTranslation( EVE_ast_Gizmo + F3D_Curve_Track->uw_Gizmo) );
            }
            M_F3D_Helpers->ul_Flags |= SOFT_Cul_HF_SeparateCenterForRotation | SOFT_Cul_HF_LocalMode;
        }
        return TRUE;
    }

	EVE_Event_InterpolationKey_GetPathInitMatrix
	(
		F3D_Curve_TrackTrans,
		F3D_Curve_GO,
		_pst_Matrix
	);

	if(F3D_Curve_Helper->i_TransEvt != -1)
	{
		pst_Pos = EVE_pst_Event_InterpolationKey_GetPos(F3D_Curve_Evt);
		if(pst_Pos)
		{
			MATH_CopyVector(&st_Pos, pst_Pos + F3D_Curve_Helper->i_TransPos);

			if(F3D_Curve_Helper->i_TransPos)
			{
				if(F3D_Curve_Helper->i_TransPos == 2)
					pst_Pos = EVE_pst_Event_InterpolationKey_GetPos( F3D_Curve_NextEvt );

				MATH_TransformVertex(&M_F3D_Helpers->st_SeparateCenter, _pst_Matrix, pst_Pos);
				M_F3D_Helpers->ul_Flags |= SOFT_Cul_HF_SeparateCenterForRotation;
			}
		}
	}
	else if(F3D_Curve_Helper->i_RotEvt != -1)
	{
		EVE_Event_InterpolationKey_GetRotationPos
		(
			F3D_Curve_TrackRot,
			F3D_Curve_TrackTrans,
			F3D_Curve_Helper->i_RotEvt,
			&st_Pos
		);
	}
	else
		return FALSE;

	MATH_TransformVertex(&st_Pos, _pst_Matrix, &st_Pos);
	MATH_SetTranslation(_pst_Matrix, &st_Pos);
	M_F3D_Helpers->ul_Flags |= SOFT_Cul_HF_LocalMode;
	return TRUE;
}

/*
 =======================================================================================================================
    Aim:    Move selected item into selected curve
 =======================================================================================================================
 */
void F3D_cl_View::Curve_Move(MATH_tdst_Vector *_pst_Move)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v, st_Pos, *pst_Pos;
	MATH_tdst_Matrix	*M, MI, st_Matrix, *pst_FatherMatrix;
	short				w_Type;
    ULONG               ul_Father;
    int                 i_PrevEvt;
    EVE_tdst_Event      *pst_PrevEvt;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!Curve_b_GetData()) return;

	if(F3D_Curve_Helper->i_TransPos == -1) return;

	/* move is in camera system axis, transform into world SA */
	M = &mst_WinHandles.pst_DisplayData->st_Camera.st_Matrix;
	MATH_TransformVector(&v, M, _pst_Move);

    if (F3D_Curve_Pick)
    {
        if ( F3D_Curve_Pick->i_Translation)
        {
            EVE_Event_InterpolationKey_SpecialPlay(F3D_Curve_GO, F3D_Curve_Data, F3D_Curve_Pick->f_Time, F3D_Curve_Pick->i_Track, F3D_Curve_Pick->i_Evt, &EVE_st_MagicBox, EVE_ast_Gizmo);
		    EVE_Event_InterpolationKey_UpdateGizmos(F3D_Curve_GO, F3D_Curve_GOAnim->pst_Skeleton, &EVE_st_MagicBox, EVE_ast_Gizmo);
            ul_Father = ANI_ul_GetFatherIndex( F3D_Curve_GOAnim->pst_Skeleton, F3D_Curve_Track->uw_Gizmo );
            pst_FatherMatrix = (ul_Father != (ULONG) -1) ? (EVE_ast_Gizmo + ul_Father) : &EVE_st_MagicBox;
            pst_Pos = EVE_pst_Event_InterpolationKey_GetPos(F3D_Curve_Evt) + (F3D_Curve_Pick->i_Translation - 1);

            if (F3D_Curve_Pick->i_Translation == 1)
            {
                MATH_AddVector( &st_Pos, &v, MATH_pst_GetTranslation( EVE_ast_Gizmo + F3D_Curve_Track->uw_Gizmo ) );
            }
            else
            {
                MATH_TransformVertex( &st_Pos, pst_FatherMatrix, pst_Pos );
                MATH_AddEqualVector( &st_Pos, &v);
            }

            MATH_SetIdentityMatrix(&MI);
            MATH_InvertMatrix(&MI, pst_FatherMatrix );
            MATH_TransformVertex( pst_Pos, &MI, &st_Pos );

            if (F3D_Curve_Pick->i_Translation == 1)
            {
                w_Type = EVE_w_Event_InterpolationKey_GetType(F3D_Curve_Evt) & EVE_InterKeyType_Translation_Mask;
                if ( w_Type > 1)
                {
                    MATH_TransformVertex( &st_Pos, pst_FatherMatrix, pst_Pos + 1);
                    MATH_AddEqualVector( &st_Pos, &v );
                    MATH_TransformVertex( pst_Pos + 1, &MI, &st_Pos );
                }

                i_PrevEvt = (F3D_Curve_Pick->i_Evt == 0) ? F3D_Curve_Track->uw_NumEvents - 1 : F3D_Curve_Pick->i_Evt - 1;
                pst_PrevEvt = F3D_Curve_Track->pst_AllEvents + i_PrevEvt;
                w_Type = EVE_w_Event_InterpolationKey_GetType( pst_PrevEvt ) & EVE_InterKeyType_Translation_Mask;
                if (w_Type > 2)
                {
                    EVE_Event_InterpolationKey_SpecialPlay(F3D_Curve_GO, F3D_Curve_Data, F3D_Curve_Pick->f_Time - EVE_FrameToTime(pst_PrevEvt->uw_NumFrames), F3D_Curve_Pick->i_Track, i_PrevEvt, &EVE_st_MagicBox, EVE_ast_Gizmo);
			        EVE_Event_InterpolationKey_UpdateGizmos(F3D_Curve_GO, F3D_Curve_GOAnim->pst_Skeleton, &EVE_st_MagicBox, EVE_ast_Gizmo);
                    pst_Pos = EVE_pst_Event_InterpolationKey_GetPos(pst_PrevEvt) + 2;
                    MATH_TransformVertex( &st_Pos, pst_FatherMatrix, pst_Pos);
                    MATH_AddEqualVector( &st_Pos, &v);
                    MATH_SetIdentityMatrix(&MI);
	                MATH_InvertMatrix(&MI, pst_FatherMatrix) ;
                    MATH_TransformVertex( pst_Pos, &MI, &st_Pos );
                }
            }
        }
    }
    else
    {
	    EVE_Event_InterpolationKey_GetAbsoluteMatrix
	    (
		    F3D_Curve_Data,
		    F3D_Curve_TrackTrans,
		    F3D_Curve_GO,
		    0,
		    0,
		    &st_Matrix
	    );
	    MATH_InvertMatrix(&MI, &st_Matrix);
	    MATH_TransformVector(&v, &MI, &v);

	    pst_Pos = EVE_pst_Event_InterpolationKey_GetPos(F3D_Curve_Evt);
	    MATH_AddEqualVector(pst_Pos + F3D_Curve_Helper->i_TransPos, &v);

	    if(F3D_Curve_Helper->i_TransPos == 0)
	    {
		    w_Type = EVE_w_Event_InterpolationKey_GetType(F3D_Curve_Evt);
		    if((w_Type & EVE_InterKeyType_Translation_Mask) > EVE_InterKeyType_Translation_0)
			    MATH_AddEqualVector(pst_Pos + 1, &v);

		    if(F3D_Curve_Evt != F3D_Curve_TrackTrans->pst_AllEvents)
		    {
			    w_Type = EVE_w_Event_InterpolationKey_GetType(F3D_Curve_Evt - 1);
			    if((w_Type & EVE_InterKeyType_Translation_Mask) > EVE_InterKeyType_Translation_1)
			    {
				    pst_Pos = EVE_pst_Event_InterpolationKey_GetPos(F3D_Curve_Evt - 1);
				    MATH_AddEqualVector(pst_Pos + 2, &v);
			    }
		    }
	    }
    }
	Refresh();
}

/*
 =======================================================================================================================
    Aim:    rotate selected item into selected curve
 =======================================================================================================================
 */
void F3D_cl_View::Curve_Rotate(MATH_tdst_Vector *_pst_Axe, MATH_tdst_Vector *_pst_Angle)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f_Angle;
	MATH_tdst_Vector	v, a, *pst_Pos, st_Center;
	MATH_tdst_Matrix	MI, Rotate, st_Matrix, st_Temp, *pst_Rot, *pst_FatherM;
	CAM_tdst_Camera		*pst_Camera;
    ULONG               ul_Father;
    int                 i_NextEvt;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!Curve_b_GetData()) return;

	/* get angle */
	f_Angle = (_pst_Angle->x) ? _pst_Angle->x : ((_pst_Angle->y) ? _pst_Angle->y : _pst_Angle->z);
	if(!f_Angle) return;

	/* move axis into world system axis */
	pst_Camera = (CAM_tdst_Camera *) (&M_F3D_DD->st_Camera);
	MATH_TransformVector(&a, &pst_Camera->st_Matrix, _pst_Axe);
	MATH_NormalizeVector(&a, &a);

    if (!F3D_Curve_Pick)
    {
	    EVE_Event_InterpolationKey_GetAbsoluteMatrix
	    (
		    F3D_Curve_Data,
		    F3D_Curve_TrackTrans,
		    F3D_Curve_GO,
		    0,
		    0,
		    &st_Matrix
	    );
        
        MATH_SetIdentityMatrix(&MI);
	    MATH_InvertMatrix(&MI, &st_Matrix);

	    MATH_TransformVector(&a, &MI, &a);
	    MATH_NormalizeVector(&a, &a);
    }

	MATH_MakeRotationMatrix_AxisAngle(&Rotate, &a, f_Angle, NULL, 1);

    if (F3D_Curve_Pick)
    {
        EVE_Event_InterpolationKey_SpecialPlay(F3D_Curve_GO, F3D_Curve_Data, F3D_Curve_Pick->f_Time, F3D_Curve_Pick->i_Track, F3D_Curve_Pick->i_Evt, &EVE_st_MagicBox, EVE_ast_Gizmo);
		EVE_Event_InterpolationKey_UpdateGizmos(F3D_Curve_GO, F3D_Curve_GOAnim->pst_Skeleton, &EVE_st_MagicBox, EVE_ast_Gizmo);

        if (F3D_Curve_Pick->i_Translation)
        {
            if (F3D_Curve_Pick->i_Translation > 1)
            {
                ul_Father = ANI_ul_GetFatherIndex( F3D_Curve_GOAnim->pst_Skeleton, F3D_Curve_Track->uw_Gizmo );
                pst_FatherM = (ul_Father != (ULONG) -1) ? (EVE_ast_Gizmo + ul_Father) : &EVE_st_MagicBox;
                pst_Pos = EVE_pst_Event_InterpolationKey_GetPos(F3D_Curve_Evt) + (F3D_Curve_Pick->i_Translation - 1);
                MATH_TransformVertex( &v, pst_FatherM, pst_Pos );
                MATH_SetIdentityMatrix(&MI);
	            MATH_InvertMatrix(&MI, pst_FatherM);

                if (F3D_Curve_Pick->i_Translation == 2)
                    MATH_CopyVector( &st_Center, MATH_pst_GetTranslation( EVE_ast_Gizmo + F3D_Curve_Track->uw_Gizmo ) );
                else
                {
                    i_NextEvt = (F3D_Curve_Pick->i_Evt + 1 == F3D_Curve_Track->uw_NumEvents) ? 0 : F3D_Curve_Pick->i_Evt + 1;
                    EVE_Event_InterpolationKey_SpecialPlay(F3D_Curve_GO, F3D_Curve_Data, F3D_Curve_Pick->f_Time + EVE_FrameToTime(F3D_Curve_Evt->uw_NumFrames), F3D_Curve_Pick->i_Track, i_NextEvt, &EVE_st_MagicBox, EVE_ast_Gizmo);
		            EVE_Event_InterpolationKey_UpdateGizmos(F3D_Curve_GO, F3D_Curve_GOAnim->pst_Skeleton, &EVE_st_MagicBox, EVE_ast_Gizmo);
                    MATH_CopyVector( &st_Center, MATH_pst_GetTranslation( EVE_ast_Gizmo + F3D_Curve_Track->uw_Gizmo) );
                }
            
                MATH_SubVector(&a, &v, &st_Center);
                MATH_TransformVector(&v, &Rotate, &a);
		        MATH_AddEqualVector( &v, &st_Center);
                MATH_TransformVertex( pst_Pos, &MI, &v );
            }
        }
        else
        {
            pst_Rot = EVE_ast_Gizmo + F3D_Curve_Track->uw_Gizmo;
            MATH_MulMatrixMatrix(&st_Temp, pst_Rot, &Rotate);

            ul_Father = ANI_ul_GetFatherIndex( F3D_Curve_GOAnim->pst_Skeleton, F3D_Curve_Track->uw_Gizmo );
            pst_FatherM = (ul_Father != (ULONG) -1) ? (EVE_ast_Gizmo + ul_Father) : &EVE_st_MagicBox;
            MATH_SetIdentityMatrix(&MI);
	        MATH_InvertMatrix(&MI, pst_FatherM);

            //pst_Rot = EVE_pst_Event_InterpolationKey_GetRotation(F3D_Curve_Evt);
		    //MATH_MulMatrixMatrix(pst_Rot, &st_Temp, &MI );
            MATH_MulMatrixMatrix( &st_Matrix, &st_Temp, &MI);
            EVE_Event_InterpolationKey_SetMatrix( &st_Matrix, F3D_Curve_Evt );
        }
    }
    else
    {
	    if(F3D_Curve_Helper->i_TransEvt != -1)
	    {
		    if(F3D_Curve_Helper->i_TransPos == 0) return;

		    if(F3D_Curve_Helper->i_TransPos == 1)
		    {
			    pst_Pos = EVE_pst_Event_InterpolationKey_GetPos(F3D_Curve_Evt);
			    MATH_CopyVector(&st_Center, pst_Pos);
		    }
		    else
		    {
			    pst_Pos = EVE_pst_Event_InterpolationKey_GetPos(F3D_Curve_Evt + 1);
			    MATH_CopyVector(&st_Center, pst_Pos);
			    pst_Pos = EVE_pst_Event_InterpolationKey_GetPos(F3D_Curve_Evt);
		    }

		    MATH_SubVector(&a, pst_Pos + F3D_Curve_Helper->i_TransPos, &st_Center);
		    MATH_TransformVector(&v, &Rotate, &a);
		    MATH_AddVector(pst_Pos + F3D_Curve_Helper->i_TransPos, &st_Center, &v);
	    }
	    else if(F3D_Curve_Helper->i_RotEvt != -1)
	    {
		    //pst_Rot = EVE_pst_Event_InterpolationKey_GetRotation(F3D_Curve_Evt);
		    //MATH_CopyMatrix(&st_Matrix, pst_Rot);
            EVE_Event_InterpolationKey_CopyMatrix( &st_Matrix, F3D_Curve_Evt, 0 );
		    //MATH_MulMatrixMatrix(pst_Rot, &st_Matrix, &Rotate);
            MATH_MulMatrixMatrix(&st_Temp, &st_Matrix, &Rotate);
            EVE_Event_InterpolationKey_SetMatrix( &st_Temp, F3D_Curve_Evt );
	    }
    }

	Refresh();
}

#endif /* ACTIVE_EDITORS */

