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

typedef struct  NEW_MAT_tdst_MTLevel_
{
    short						s_TextureId;
	unsigned short				s_AditionalFlags;
    ULONG               ul_Flags;
    ULONG               ScaleSPeedPosU;
    ULONG               ScaleSPeedPosV;
    struct NEW_MAT_tdst_MTLevel_ *pst_NextLevel;
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
    NEW_MAT_tdst_MTLevel_ *pst_FirstLevel;
    ULONG       ul_DrawMask;
} CURRENT_MAT_tdst_MultiTexture;

/*
 ===================================================================================================
    Aim:    Update bigfile from version 11 to Version 12
            reset material rotation 
 ===================================================================================================
 */
void VERsion_UpdateOneFileVersion12(char *_pc_Buf, ULONG _ul_Len, char *_psz_Path, char *_psz_File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    *psz_Ext, *pc_Cur;
	NEW_MAT_tdst_MTLevel *MewLine;
	CURRENT_MAT_tdst_MultiTexture *Material;
	ULONG ContinueOK;
	ULONG lLen;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Find extension of file */
	return;
    psz_Ext = strrchr(_psz_File, '.');
	lLen = 0;
    /* Update Gao */
    if(psz_Ext && !L_stricmp(psz_Ext, EDI_Csz_ExtGraphicMaterial))
    {
		pc_Cur = _pc_Buf;
        /* type */
		if (*(ULONG *)pc_Cur != GRO_MaterialMultiTexture) return;
		pc_Cur += 4;
		lLen += 4;
		/* name & name lenght */
		lLen += *(ULONG *)pc_Cur;
		lLen += 4;
		pc_Cur += *(ULONG *)pc_Cur;
		pc_Cur += 4;
		Material = (CURRENT_MAT_tdst_MultiTexture *)pc_Cur;
		pc_Cur += sizeof(CURRENT_MAT_tdst_MultiTexture);
		lLen += sizeof(CURRENT_MAT_tdst_MultiTexture);
		ContinueOK = (ULONG)Material->pst_FirstLevel;
		if (!ContinueOK) return;
		while ((ContinueOK) && (lLen <= _ul_Len))
		{
			MewLine = (NEW_MAT_tdst_MTLevel *)pc_Cur;
			ContinueOK = 0;
			if (MewLine -> s_TextureId)
				ContinueOK = 1;
			/* reset rotation */
			MewLine -> ScaleSPeedPosU &= ~0x00010001;
 			MewLine -> ScaleSPeedPosV &= ~0x00010001;
			pc_Cur += sizeof(NEW_MAT_tdst_MTLevel);
			lLen +=sizeof(NEW_MAT_tdst_MTLevel);
		}
		BIG_UpdateFileFromBuffer(_psz_Path, _psz_File, _pc_Buf, _ul_Len );
    }
}

#endif
