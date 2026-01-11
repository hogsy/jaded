/*$T VERsion_Update5.cpp GC!1.5 11/12/99 15:31:36 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/VERsion/VERsion_Update.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "EDIpaths.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGkey.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "MATHs/MATH.h"

typedef struct  OLD_MAT_tdst_MTLevel_
{
    ULONG               l_TextureId;
    ULONG               ul_Flags;
    ULONG               ScaleSPeedPosU;
    ULONG               ScaleSPeedPosV;
    struct OLD_MAT_tdst_MTLevel_    *pst_NextLevel;
} OLD_MAT_tdst_MTLevel;

typedef struct  NEW_MAT_tdst_MTLevel_
{
    short						s_TextureId;
	unsigned short				s_AditionalFlags;
    ULONG               ul_Flags;
    ULONG               ScaleSPeedPosU;
    ULONG               ScaleSPeedPosV;
    struct OLD_MAT_tdst_MTLevel_   *pst_NextLevel;
} NEW_MAT_tdst_MTLevel;

typedef struct  CURRENT_MAT_tdst_MultiTexture_
{
	/* BEGIN MUST BE THE SAME THAN MAT_tdst_Single */
	// GRO_tdst_Struct		st_Id; /* this is allready readed */
    ULONG		ul_Ambiant;
    ULONG		ul_Diffuse;
    ULONG		ul_Specular;
    float				f_SpecularExp;
    float				f_Opacity;
    ULONG		ul_Flags;
	/* END MUST BE THE SAME THAN MAT_tdst_Single */
    OLD_MAT_tdst_MTLevel *pst_FirstLevel;
    ULONG       ul_DrawMask;
} CURRENT_MAT_tdst_MultiTexture;

/*
 ===================================================================================================
    Aim:    Update bigfile from version 10 to Version 11
            Change material multitexture format
 ===================================================================================================
 */
void VERsion_UpdateOneFileVersion11(char *_pc_Buf, ULONG _ul_Len, char *_psz_Path, char *_psz_File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    *psz_Ext, *pc_Cur;
	NEW_MAT_tdst_MTLevel *MewLine;
	OLD_MAT_tdst_MTLevel OLDLine;
	CURRENT_MAT_tdst_MultiTexture *Material;
	ULONG ContinueOK;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Find extension of file */
    psz_Ext = strrchr(_psz_File, '.');
    /* Update Gao */
    if(psz_Ext && !L_stricmp(psz_Ext, EDI_Csz_ExtGraphicMaterial))
    {
		pc_Cur = _pc_Buf;
        /* type */
		if (*(ULONG *)pc_Cur != GRO_MaterialMultiTexture) return;

		pc_Cur += 4;
		/* name & name lenght */
		pc_Cur += *(ULONG *)pc_Cur;
		pc_Cur += 4;
		Material = (CURRENT_MAT_tdst_MultiTexture *)pc_Cur;
		pc_Cur += sizeof(CURRENT_MAT_tdst_MultiTexture);
		ContinueOK = (ULONG)Material->pst_FirstLevel;
		if (!ContinueOK) return;
		while (ContinueOK)
		{
			OLDLine = *(OLD_MAT_tdst_MTLevel *)pc_Cur;
			MewLine = (NEW_MAT_tdst_MTLevel *)pc_Cur;
			*(ULONG *)&MewLine -> pst_NextLevel = OLDLine.l_TextureId;
			ContinueOK = 0;
			if (OLDLine.pst_NextLevel)
			{
				MewLine -> s_TextureId = 1;
				ContinueOK = 1;
			}
			else
				MewLine -> s_TextureId = 0;

			MewLine -> s_AditionalFlags = 0;
			MewLine ->	ul_Flags &= ~0x00F00000;
			switch ((OLDLine.ul_Flags >> 20L) & 0xf)
			{
				case /*MAT_Cc_UV_Object1       */0:
					MewLine ->	ul_Flags |= 0L << 20;
					break;
				case /*MAT_Cc_UV_Object2       */1:
					MewLine ->	ul_Flags |= 1L << 20;
					break;
				case /*MAT_Cc_UV_Chrome        */2:
					MewLine ->	ul_Flags |= 2L << 20;
					break;
				case /*MAT_Cc_UV_Cartoon      */ 3:
					MewLine ->	ul_Flags |= 2L << 20;
					break;
				case /*MAT_Cc_UV_DF			*/ 4:
					MewLine ->	ul_Flags |= 3L << 20;
					break;
				case /*MAT_Cc_UV_SolarPhong	*/ 5:
					MewLine -> s_AditionalFlags = 0;
					MewLine ->	ul_Flags |= 4L << 20;
					break;
				case /*MAT_Cc_UV_Anisotropic  */ 6:
					MewLine ->	ul_Flags |= 4L << 20;
					break;
				case /*MAT_Cc_UV_Previous		*/ 7:
					MewLine ->	ul_Flags |= 5L << 20;
					break;
				case /*MAT_Cc_UV_XYLocal		*/ 8:
					MewLine ->	ul_Flags |= 6L << 20;		/*PLANAR GIZMO*/
					MewLine -> s_AditionalFlags |= 2L << 4; /* Z */
					MewLine -> s_AditionalFlags |= 0L << 6; /*OBJECT*/
					break;
				case /*MAT_Cc_UV_XYWorld		*/ 9:
					MewLine ->	ul_Flags |= 6L << 20;		/*PLANAR GIZMO*/
					MewLine -> s_AditionalFlags |= 2L << 4; /* Z */
					MewLine -> s_AditionalFlags |= 1L << 6; /*WORLD*/
					break;
				case /*MAT_Cc_UV_FaceMap		*/ 10:
					MewLine ->	ul_Flags |= 7L << 20;
					break;
				case /*MAT_Cc_UV_XZLocal		*/ 11:
					MewLine ->	ul_Flags |= 6L << 20;		/*PLANAR GIZMO*/
					MewLine -> s_AditionalFlags |= 1L << 4; /* Y */
					MewLine -> s_AditionalFlags |= 0L << 6; /*OBJECT*/
					break;
				case /*MAT_Cc_UV_XZWorld		*/ 12:
					MewLine ->	ul_Flags |= 6L << 20;		/*PLANAR GIZMO*/
					MewLine -> s_AditionalFlags |= 1L << 4; /* Y */
					MewLine -> s_AditionalFlags |= 1L << 6; /*WORLD*/
					break;
				case /*MAT_Cc_UV_YZLocal		*/ 13:
					MewLine ->	ul_Flags |= 6L << 20;		/*PLANAR GIZMO*/
					MewLine -> s_AditionalFlags |= 0L << 4; /* X */
					MewLine -> s_AditionalFlags |= 0L << 6; /*OBJECT*/
					break;
				case /*MAT_Cc_UV_YZWorld		*/ 14:
					MewLine ->	ul_Flags |= 6L << 20;		/*PLANAR GIZMO*/
					MewLine -> s_AditionalFlags |= 0L << 4; /* X */
					MewLine -> s_AditionalFlags |= 1L << 6;	/*WORLD*/
					break;
			}
			pc_Cur += sizeof(OLD_MAT_tdst_MTLevel);
		}
		BIG_UpdateFileFromBuffer(_psz_Path, _psz_File, _pc_Buf, _ul_Len );
    }
}

#endif
