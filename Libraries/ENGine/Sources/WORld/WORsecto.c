/*$T WORsecto.c GC 1.139 03/26/04 10:24:18 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "stdio.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "LINks/LINKmsg.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"
#include "LINks/LINKtoed.h"

#include "MATHs/MATH.h"
#include "TIMer/PROfiler/PROdefs.h"

#include "AIinterp/Sources/AIload.h"
#include "AIinterp/Sources/AIengine.h"
#include "TIMer/PROfiler/PROdefs.h"
#include "TIMer/TIM.h"

#include "ENGine/Sources/WORld/WOR.h"
#include "ENGine/Sources/WORld/WORuniverse.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/EOT/EOTinit.h"
#include "ENGine/Sources/OBJects/OBJ.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/WAYs/WAYinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/GRId/GRI_compute.h"
#include "ENGine/Sources/ANImation/ANIinit.h"

#include "ENGine/Sources/INTersection/INTinit.h"
#include "INTersection/INTSnP.h"

#include "AIinterp/Sources/AIdebug.h"

#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/GRP/GRPmain.h"
#include "GFX/GFX.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
extern BOOL ENG_gb_ActiveSectorization;
extern UINT SPG2_gb_Recompute;
ULONG SCT_gul_Flags = 0;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_SectoReinit(WOR_tdst_World *_pst_World)
{
	/*~~*/
	int i;
	/*~~*/

	MATH_InitVectorToZero(&_pst_World->st_SectorLastPos);
	_pst_World->ul_CurrentSector = 0;
	SCT_gul_Flags = SCT_MODE_Cam;
	for(i = 1; i < WOR_C_MaxSecto; i++)
	{
		_pst_World->ast_AllSectos[i].ul_Flags |= WOR_CF_SectorVisible;
		_pst_World->ast_AllSectos[i].ul_Flags |= WOR_CF_SectorActive;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL WOR_SectoObjVisible(WOR_tdst_World *_pst_World, OBJ_tdst_GameObject *_pst_GAO)
{
	/*~~~~~~~~*/
	int		i;
	BOOL	ok;
	UCHAR	val;
	/*~~~~~~~~*/

	if(!ENG_gb_ActiveSectorization) return TRUE;
	if(!_pst_World->ul_CurrentSector) return TRUE;
	if(!_pst_GAO->pst_Extended) return TRUE;
	if(_pst_GAO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_AlwaysVisible) return TRUE;

	ok = TRUE;
	for(i = 0; i < 4; i++)
	{
		val = _pst_GAO->pst_Extended->auc_Sectos[i];
		if(!val) continue;
		if(_pst_World->ast_AllSectos[val].ul_Flags & WOR_CF_SectorAlwaysVisible) return TRUE;
		if(_pst_World->ast_AllSectos[val].ul_Flags & WOR_CF_SectorNeverVisible) return FALSE;
	}

	for(i = 0; i < 4; i++)
	{
		val = _pst_GAO->pst_Extended->auc_Sectos[i];
		if(!val) continue;
		if(!(_pst_World->ast_AllSectos[val].ul_Flags & WOR_CF_SectorValid)) continue;
		if(_pst_World->ast_AllSectos[val].ul_Flags & WOR_CF_SectorVisible) return TRUE;
		ok = FALSE;
	}

	return ok;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL WOR_SectoObjActive(WOR_tdst_World *_pst_World, OBJ_tdst_GameObject *_pst_GAO)
{
	/*~~~~~~~~*/
	int		i;
	BOOL	ok;
	UCHAR	val;
	/*~~~~~~~~*/

	if(!ENG_gb_ActiveSectorization) return TRUE;
	if(!_pst_World->ul_CurrentSector) return TRUE;
	if(!_pst_GAO->pst_Extended) return TRUE;
	if(_pst_GAO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_AlwaysActive) return TRUE;

	ok = TRUE;
	for(i = 0; i < 4; i++)
	{
		val = _pst_GAO->pst_Extended->auc_Sectos[i];
		if(!val) continue;
		if(_pst_World->ast_AllSectos[val].ul_Flags & WOR_CF_SectorAlwaysActive) return TRUE;
		if(_pst_World->ast_AllSectos[val].ul_Flags & WOR_CF_SectorNeverActive) return FALSE;
	}

	for(i = 0; i < 4; i++)
	{
		val = _pst_GAO->pst_Extended->auc_Sectos[i];
		if(!val) continue;
		if(!(_pst_World->ast_AllSectos[val].ul_Flags & WOR_CF_SectorValid)) continue;
		if(_pst_World->ast_AllSectos[val].ul_Flags & WOR_CF_SectorActive) return TRUE;
		ok = FALSE;
	}

	return ok;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL WOR_ThroughPortal(MATH_tdst_Vector *_pst_OldPos, MATH_tdst_Vector *_pst_NewPos, WOR_tdst_Portal *_pst_Portal)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				cos1, cos2, t;
	MATH_tdst_Vector	ANew, AOld, Seg, Temp;
	MATH_tdst_Vector	PU, PV, PN;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* calcul des vecteur du plan du portal + normale au plan */
	MATH_SubVector(&PU, &_pst_Portal->vB, &_pst_Portal->vA);
	MATH_SubVector(&PV, &_pst_Portal->vD, &_pst_Portal->vA);
	MATH_CrossProduct(&PN, &PU, &PV);

	/*
	 * calcul des cos + test traversée potentiel = changement de demi espace délimité
	 * par le plan du portal £
	 * faudra rajouter un test pour le sens de traverser £
	 * genre cos2 < 0 return FALSE
	 */
	MATH_SubVector(&ANew, &_pst_Portal->vA, _pst_NewPos);
	cos2 = MATH_f_DotProduct(&ANew, &PN);

	MATH_SubVector(&AOld, &_pst_Portal->vA, _pst_OldPos);
	cos1 = MATH_f_DotProduct(&AOld, &PN);

	/* test traversée du plan (si cos1 et cos2 ont meme signe => pas de traversée */
	if(cos1 * cos2 > 0) return FALSE;

	MATH_SubVector(&Seg, _pst_NewPos, _pst_OldPos);

	t = MATH_f_DotProduct(&Seg, &PN);

	/* Sens de traversée */
	if(t < 0 && !(_pst_Portal->uw_Flags & WOR_CF_PortalShare)) return FALSE;
	if(t > 0 && (_pst_Portal->uw_Flags & WOR_CF_PortalShare)) return FALSE;

	/*
	 * devrait pas etre nécessaire, a rajouter éventuellement £
	 * vire les cas ou le segment oldpos -> newpos est parrallèle au plan £
	 * > if(fAbs(t) < Cf_EpsilonBig) return FALSE; £
	 * calcul le point d'intersection du segment dans le plan du portal
	 */
	MATH_AddScaleVector(&Temp, _pst_OldPos, &Seg, cos1 / t);

	/* passe ce point dans le repère du portal */
	MATH_SubEqualVector(&Temp, &_pst_Portal->vA);

	t = MATH_f_DotProduct(&Temp, &PU);
	if((t < 0) || (t > MATH_f_SqrVector(&PU))) return FALSE;

	t = MATH_f_DotProduct(&Temp, &PV);
	if((t < 0) || (t > MATH_f_SqrVector(&PV))) return FALSE;

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_NoSectorVisAct(WOR_tdst_World *_pst_World)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 1; i < WOR_C_MaxSecto; i++)
	{
		_pst_World->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorVisible;
		_pst_World->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorActive;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_SetCurrentSector(WOR_tdst_World *_pst_World, int ul_NewCurrent, BOOL force)
{
	/*~~~~~~~~*/
	int		i;
	UCHAR	val;
	/*~~~~~~~~*/

	/*
	 * On force, sans lien moteur, donc on vire tous les secteurs pour remettre que
	 * les bons
	 */
	if(force) WOR_NoSectorVisAct(_pst_World);

	/* Recompute active objects, SnP and Views next frame */
	if(_pst_World->ul_CurrentSector != ul_NewCurrent)
	{
		_pst_World->b_ForceActivationRefresh = TRUE;
		_pst_World->b_ForceVisibilityRefresh = TRUE;
		_pst_World->b_ForceBVRefresh = TRUE;
		_pst_World->ul_CurrentSector = ul_NewCurrent;
		SPG2_gb_Recompute = 2;
	}

	/* Treat current */
	_pst_World->ast_AllSectos[ul_NewCurrent].ul_Flags |= WOR_CF_SectorVisible;
	_pst_World->ast_AllSectos[ul_NewCurrent].ul_Flags |= WOR_CF_SectorActive;
	for(i = 0; i < WOR_C_MaxSectoRef; i++)
	{
		val = _pst_World->ast_AllSectos[ul_NewCurrent].auc_RefVis[i];
		if(val) _pst_World->ast_AllSectos[val].ul_Flags |= WOR_CF_SectorVisible;
		val = _pst_World->ast_AllSectos[ul_NewCurrent].auc_RefAct[i];
		if(val) _pst_World->ast_AllSectos[val].ul_Flags |= WOR_CF_SectorActive;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_ComputeSector(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ul_OldCurrent;
	ULONG				ul_NewCurrent;
	int					i, j;
	MATH_tdst_Vector	st_LastPos, st_NewPos;
	OBJ_tdst_GameObject	*pst_GAO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(SCT_gul_Flags)
	{
	case SCT_MODE_Cam:
	default:
		if(_pst_World->pst_CurrentView && _pst_World->pst_CurrentView->pst_Father)
		{
			MATH_CopyVector(&st_LastPos, &_pst_World->st_SectorLastPos);
			MATH_CopyVector(&st_NewPos, &_pst_World->pst_CurrentView->pst_Father->pst_GlobalMatrix->T);
			MATH_CopyVector(&_pst_World->st_SectorLastPos, &st_NewPos);
		}
		break;

	case SCT_MODE_Main0:
		pst_GAO = AI_gpst_MainActors[0];
		if(pst_GAO)
		{
			MATH_CopyVector(&st_LastPos, &_pst_World->st_SectorLastPos);
			MATH_CopyVector(&st_NewPos, OBJ_pst_GetAbsolutePosition(pst_GAO));
			MATH_CopyVector(&_pst_World->st_SectorLastPos, &st_NewPos);
		}
		break;
	case SCT_MODE_Main1:
		pst_GAO = AI_gpst_MainActors[1];
		if(pst_GAO)
		{
			MATH_CopyVector(&st_LastPos, &_pst_World->st_SectorLastPos);
			MATH_CopyVector(&st_NewPos, OBJ_pst_GetAbsolutePosition(pst_GAO));
			MATH_CopyVector(&_pst_World->st_SectorLastPos, &st_NewPos);
		}
		break;
	}

    ul_OldCurrent = _pst_World->ul_CurrentSector;
	ul_NewCurrent = ul_OldCurrent;
	if(!ul_OldCurrent) return;

	for(i = 1; i < WOR_C_MaxSecto; i++)
	{
		/* Il peut y avoir des trous dans la liste de secteurs */
		if(!(_pst_World->ast_AllSectos[i].ul_Flags & WOR_CF_SectorValid)) continue;

		/*
		 * On se fout de quitter un secteur, mais juste de rentrer. Si on est dans un
		 * secteur donné, pas la peine donc de tester ses portes à lui (il se le secteur
		 * courant tant qu'un autre secteur n'a pas pris la main)
		 */
		if(i == _pst_World->ul_CurrentSector) continue;

		for(j = 0; j < WOR_C_MaxSectoRef; j++)
		{
			/* "Invalid" marque la fin de la liste de portals */
			if(!(_pst_World->ast_AllSectos[i].ast_Portals[j].uw_Flags & WOR_CF_PortalValid)) break;

			/* Est-ce que la camera a traversé ce portal ? */
			if(WOR_ThroughPortal(&st_LastPos, &st_NewPos, &_pst_World->ast_AllSectos[i].ast_Portals[j]))
			{
				/* Oui, alors c'est le nouveau secteur courant */
				ul_NewCurrent = i;
			}
		}
	}

	/* Nouveau secteur ou changement de secteur -> on remet tout à 0 */
    if (ul_OldCurrent == 0xFFFFFFFF || ul_OldCurrent != ul_NewCurrent)
    {
    	for(i = 1; i < WOR_C_MaxSecto; i++)
        {
    		_pst_World->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorVisible;
	    	_pst_World->ast_AllSectos[i].ul_Flags &= ~WOR_CF_SectorActive;
        }
    }

	/* Recompute active objects, SnP and Views next frame */
    if (ul_NewCurrent != ul_OldCurrent)
    	WOR_SetCurrentSector(_pst_World, ul_NewCurrent, FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_ValidatePortal(WOR_tdst_World *_pst_World, WOR_tdst_Portal *_pst_Portal)
{
	/*~~~~~~~~~~~~~~~~~~*/
	int				i, j;
	WOR_tdst_Portal *port;
	/*~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < WOR_C_MaxSecto; i++)
	{
		if(!(_pst_World->ast_AllSectos[i].ul_Flags & WOR_CF_SectorValid)) continue;
		for(j = 0; j < WOR_C_MaxSectoPortals; j++)
		{
			port = &_pst_World->ast_AllSectos[i].ast_Portals[j];
			if(!(port->uw_Flags & WOR_CF_PortalValid)) break;
			if(port == _pst_Portal) continue;

			if(port->uc_ShareSect == _pst_Portal->uc_ShareSect && port->uc_SharePortal == _pst_Portal->uc_SharePortal)
			{
#ifdef ACTIVE_EDITORS
				L_strcpy(port->az_Name, _pst_Portal->az_Name);
#endif
				MATH_CopyVector(&port->vA, &_pst_Portal->vA);
				MATH_CopyVector(&port->vB, &_pst_Portal->vB);
				MATH_CopyVector(&port->vC, &_pst_Portal->vC);
				MATH_CopyVector(&port->vD, &_pst_Portal->vD);
			}
		}
	}
}

/*$4-******************************************************************************************************************/

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
