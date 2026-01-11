/*$T F3Dview_postit.cpp GC 1.134 04/23/04 10:31:43 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "SOFT/SOFTlinear.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "EDIapp.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dstrings.h"
#include "F3Dframe/F3Dview_undo.h"
#include "F3Dframe/F3Dview_postit.h"
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
#include "BIGfiles/BIGread.h"
#include "MATHs/MATH.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
extern char					EDI_gaz_Message[4096];
#include "DIAlogs/DIApostit_dlg.h"

#ifdef JADEFUSION
extern F3D_tdst_PostIt	**WOR_gpt_AllPostIt;
extern ULONG			WOR_gul_AllPostIt;
#include "DATaControl/DATCPerforce.h"
#else
extern "C" F3D_tdst_PostIt	**WOR_gpt_AllPostIt;
extern "C" ULONG			WOR_gul_AllPostIt;
#endif
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::KillPostIt(void)
{
	mo_ListPostIt.RemoveAll();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::PostItPath(char *dest, BOOL create)
{
	/*~~~~~~~~~~~~~~~~*/
	char		*pz;
	BIG_INDEX	ul_File;
	/*~~~~~~~~~~~~~~~~*/

	*dest = 0;
	if(!mst_WinHandles.pst_World) return;
	if(!M_MF()->mst_Ini.b_LinkControlON || !(*M_MF()->mst_Ini.asz_CurrentRefFile)) return;
#ifdef JADEFUSION
	strcpy(dest, M_MF()->mst_Ini.asz_CurrentRefFile);
#endif
	/* Serveur */
	strcpy(dest, M_MF()->mst_Ini.asz_CurrentRefFile);
	pz = L_strlen(dest) + dest;
	while(*pz != '/' && *pz != '\\') pz--;
	if(pz) *pz = 0;
	L_strcat(dest, "\\" EDI_Csz_Path_PostIt "\\");

	pz = dest;
	while(*pz)
	{
		if(*pz == '/') *pz = '\\';
		pz++;
	}

	if(create) CreateDirectory(dest, NULL);
	ul_File = BIG_ul_SearchKeyToFat(mst_WinHandles.pst_World->h_WorldKey);
	L_strcat(dest, BIG_NameDir(BIG_ParentFile(ul_File)));
	if(create) CreateDirectory(dest, NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::LoadPostIt(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World		*pst_World;
	char				az[1024];
	F3D_tdst_PostIt		pit;
	struct L_finddata_t st_FileInfos;
	LONG				l_Handle;
	FILE				*file;
	int					numread;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mo_ListPostIt.RemoveAll();
	if((pst_World = mst_WinHandles.pst_World) == NULL) return;
	if(!M_MF()->mst_Ini.b_LinkControlON || !(*M_MF()->mst_Ini.asz_CurrentRefFile)) return;

	numread = 0;
	PostItPath(az);
	L_strcat(az, "/*" EDI_Csz_PostIt);
	if((l_Handle = L_findfirst(az, &st_FileInfos)) != -1)
	{
		do
		{
			PostItPath(az);
			L_strcat(az, "/");
			L_strcat(az, st_FileInfos.name);
			file = fopen(az, "rb");
			if(file)
			{
				fread(&pit, sizeof(pit), 1, file);
				fclose(file);
				L_strcpy(pit.az_Name, st_FileInfos.name);

				/* Detection non lue */
				pit.ul_Flags &= ~PIT_C_ForMe;
				pit.ul_Flags &= ~PIT_C_NoRead;
				pit.ul_Flags &= ~PIT_C_Draw;
				if(!pit.az_To[0] || PostItFindName(pit.az_To, M_MF()->mst_Ini.asz_CurrentUserName))
				{
					pit.ul_Flags |= PIT_C_ForMe;
					if(!PostItFindName(pit.az_Read, M_MF()->mst_Ini.asz_CurrentUserName))
					{
						pit.ul_Flags |= PIT_C_NoRead;
						numread++;
					}
				}

				mo_ListPostIt.AddTail(pit);
			}
		} while(L_findnext(l_Handle, &st_FileInfos) != -1);
		L_findclose(l_Handle);
	}

	/* Warning si messages en attentes */
	if(numread)
	{
		sprintf(EDI_gaz_Message, "%d PostIt message(s) not read", numread);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_View::PostItFindName(char *pz_All, char *pz_Name)
{
	/*~~~~~~~~~~~~~~*/
	char	az[128];
	char	*pz, *pz1;
	/*~~~~~~~~~~~~~~*/

	pz = pz_All;
	while(*pz)
	{
		pz1 = az;
		while(*pz && *pz != ';') *pz1++ = *pz++;
		*pz1 = 0;
		if(!L_strcmpi(pz_Name, az)) return TRUE;
		if(*pz) pz++;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::CreatePostIt(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	F3D_tdst_PostIt *pit;
	int				cpt;
	/*~~~~~~~~~~~~~~~~~*/

	WOR_gpt_AllPostIt = NULL;
	WOR_gul_AllPostIt = 0;
	if(!mst_WinHandles.pst_DisplayData->uc_DrawPostIt) return;

	cpt = 0;
	pos = mo_ListPostIt.GetHeadPosition();
	while(pos)
	{
		pit = &mo_ListPostIt.GetNext(pos);
		if(pit->ul_Flags & PIT_C_Draw)
		{
			if(!MATH_b_NulVector(&pit->st_Pos))
			{
				cpt++;
			}
		}
	}

	WOR_gpt_AllPostIt = (F3D_tdst_PostIt **) malloc(cpt * sizeof(void *));
	WOR_gul_AllPostIt = cpt;

	cpt = 0;
	pos = mo_ListPostIt.GetHeadPosition();
	while(pos)
	{
		pit = &mo_ListPostIt.GetNext(pos);
		if(pit->ul_Flags & PIT_C_Draw)
		{
			if(!MATH_b_NulVector(&pit->st_Pos))
			{
				WOR_gpt_AllPostIt[cpt] = pit;
				cpt++;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::DestroyPostIt(void)
{
	if(WOR_gpt_AllPostIt) free(WOR_gpt_AllPostIt);
	WOR_gpt_AllPostIt = NULL;
	WOR_gul_AllPostIt = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::PostItRefresh(F3D_tdst_PostIt *pit)
{
	/*~~~~~~~~~~~~~~~~~~*/
	char	az_Path[1024];
	int		cpt;
	FILE	*f;
	/*~~~~~~~~~~~~~~~~~~*/

	PostItPath(az_Path);
	L_strcat(az_Path, "/");
	L_strcat(az_Path, pit->az_Name);
	cpt = 0;
	do
	{
		f = fopen(az_Path, "rb");
		cpt++;
	} while(!f && cpt < 100);
	if(f)
	{
		fread(pit, sizeof(F3D_tdst_PostIt), 1, f);
		fclose(f);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_View::PostItPickMouse(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION				pos;
	F3D_tdst_PostIt			*pit;
	MATH_tdst_Vector		vpoint, v2d[8], v3d[8];
	MATH_tdst_Vector		vmin, vmax;
	CPoint					point;
	CRect					rect;
	EDIA_cl_PostItDialog	o_Dlg(mst_WinHandles.pst_World, this);
	CAM_tdst_Camera			*pst_Cam;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define LARG	0.1f;
	GetCursorPos(&point);
	ScreenToClient(&point);
	GetClientRect(&rect);
	vpoint.x = (float) point.x / (float) rect.Width();
	vpoint.y = (float) point.y / (float) rect.Height();

	pos = mo_ListPostIt.GetHeadPosition();
	while(pos)
	{
		pit = &mo_ListPostIt.GetNext(pos);
		if(pit->ul_Flags & PIT_C_Draw)
		{
			vmin.x = vmin.y = 100000.0f;
			vmax.x = vmax.y = -100000.0f;
			for(i = 0; i < 8; i++)
			{
				MATH_CopyVector(&v3d[i], &pit->st_Pos);
				if(i >= 4) v3d[i].z += 1.5f;
				switch(i % 4)
				{
				case 0: v3d[i].x -= LARG; v3d[i].y -= LARG; break;
				case 1: v3d[i].x += LARG; v3d[i].y -= LARG; break;
				case 2: v3d[i].x -= LARG; v3d[i].y += LARG; break;
				case 3: v3d[i].x += LARG; v3d[i].y += LARG; break;
				}

				pst_Cam = &mst_WinHandles.pst_DisplayData->st_Camera;
				pst_Cam->pst_ObjectToCameraMatrix = &pst_Cam->st_InverseMatrix;
				SOFT_TransformAndProject(&v2d[i], &v3d[i], 1, pst_Cam);
				v2d[i].x = 0.5f + (v2d[i].x - pst_Cam->f_CenterX) / pst_Cam->f_Width;
				v2d[i].y = 0.5f + (pst_Cam->f_CenterY - v2d[i].y) / pst_Cam->f_Height;

				if(v2d[i].x < vmin.x) vmin.x = v2d[i].x;
				if(v2d[i].y < vmin.y) vmin.y = v2d[i].y;
				if(v2d[i].x > vmax.x) vmax.x = v2d[i].x;
				if(v2d[i].y > vmax.y) vmax.y = v2d[i].y;
			}

			if(vpoint.x < vmin.x) continue;
			if(vpoint.x > vmax.x) continue;
			if(vpoint.y < vmin.y) continue;
			if(vpoint.y > vmax.y) continue;
			PostItRefresh(pit);
			L_memcpy(&o_Dlg.mst_PostIt, pit, sizeof(o_Dlg.mst_PostIt));
			o_Dlg.mpt_Org = pit;
			if(o_Dlg.DoModal() == IDOK)
			{
				pit->ul_Flags &= ~PIT_C_NoRead;
				LINK_Refresh();
			}

			return TRUE;
		}
	}

	return FALSE;
}
#endif /* ACTIVE_EDITORS */
