/*$T STRfont.c GC! 1.081 05/03/01 10:03:14 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "TEXture/TEXfile.h"
#include "BASe/CLibrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "STRing/STRstruct.h"
#include "MATerial/MATstruct.h"
#include "TEXture/TEXfile.h"
#include "TEXture/TEXstruct.h"

#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/SAVing/SAVdefs.h"

#ifdef JADEFUSION
#include "INOut/INO.h"
#endif

#ifdef PSX2_TARGET
#include "MainPsx2/Sources/PSX2debug.h"
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

extern LONG STR_sgl_MaxNumberOfFont;
#ifdef JADEFUSION
extern int  INO_gai_PresentLanguage[INO_e_MaxLangNb];
#endif
/*$4
 ***********************************************************************************************************************
    Font desc
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
STR_tdst_FontDesc *STR_pst_GetFontDesc(void *_pst_Material, ULONG _ul_SubId)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_FontDesc	*pst_Font, *pst_Last;
	LONG				i;
	ULONG				ul_Key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Key = 0xFFFFFFFF;

#ifdef _XENON
	// offset id by 22 for japanese as this is the position of the first japanese entry in the multi-material
	if (_ul_SubId < 2 && XGetLanguage( ) == XC_LANGUAGE_JAPANESE && INO_gai_PresentLanguage[INO_e_Japanese] == 1 )
		_ul_SubId += 22;
#endif

	if(_pst_Material == NULL) return NULL;

	if(((GRO_tdst_Struct *) _pst_Material)->i->ul_Type == GRO_MaterialMulti)
	{
		if(_ul_SubId >= (ULONG) ((MAT_tdst_Multi *) _pst_Material)->l_NumberOfSubMaterials) _ul_SubId = 0;

		_pst_Material = (void *) ((MAT_tdst_Multi *) _pst_Material)->dpst_SubMaterial[_ul_SubId];
	}

	if(((GRO_tdst_Struct *) _pst_Material)->i->ul_Type == GRO_MaterialSingle)
	{
		ul_Key = ((MAT_tdst_Single *) _pst_Material)->l_TextureId;
	}
	else if(((GRO_tdst_Struct *) _pst_Material)->i->ul_Type == GRO_MaterialMultiTexture)
	{
		if(((MAT_tdst_MultiTexture *) _pst_Material)->pst_FirstLevel)
			ul_Key = ((MAT_tdst_MultiTexture *) _pst_Material)->pst_FirstLevel->s_TextureId;
	}

	if(ul_Key != 0xFFFFFFFF)
	{
        if (TEX_gst_GlobalList.dst_Texture[ul_Key].uw_Flags & TEX_uw_RawPal)
            ul_Key = TEX_gst_GlobalList.dst_Texture[ul_Key].w_Height;

		ul_Key = TEX_gst_GlobalList.dst_Texture[ul_Key].ul_Key;

		pst_Font = STR_sgpst_Font;
		pst_Last = pst_Font + STR_sgl_NumberOfFont;

		for(; pst_Font < pst_Last; pst_Font++)
		{
			if ( (pst_Font->ul_Key == ul_Key) || (pst_Font->ul_AltKey == ul_Key) ) return pst_Font;
		}
	}

	/* Search for font */
	pst_Font = STR_sgpst_Font;
	pst_Last = pst_Font + STR_sgl_NumberOfFont;

	for(; pst_Font < pst_Last; pst_Font++)
	{
		if(pst_Font->pst_Material == _pst_Material) return pst_Font;
	}

	/* Font not found : add it */
	if(STR_sgl_NumberOfFont >= STR_sgl_MaxNumberOfFont)
	{
		STR_sgl_MaxNumberOfFont += 20;
		i = sizeof(STR_tdst_FontDesc) * (STR_sgl_MaxNumberOfFont);
		if(STR_sgpst_Font)
			STR_sgpst_Font = (STR_tdst_FontDesc *) MEM_p_Realloc(STR_sgpst_Font, i);
		else
			STR_sgpst_Font = (STR_tdst_FontDesc *) MEM_p_Alloc(i);
	}

	pst_Font = STR_sgpst_Font + STR_sgl_NumberOfFont++;
	L_memset( pst_Font, 0, sizeof( STR_tdst_FontDesc ));
	pst_Font->pst_Material = _pst_Material;

	for(i = 0; i < (int) pst_Font->ul_NbChar; i++)
	{
		pst_Font->pst_Letter[i].f_U[0] = 0.0f;
		pst_Font->pst_Letter[i].f_U[1] = 0.0f;
		pst_Font->pst_Letter[i].f_V[0] = 0.0f;
		pst_Font->pst_Letter[i].f_V[1] = 1.0f;
	}

	return pst_Font;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
STR_tdst_FontDesc *STR_pst_GetFont(ULONG _ul_Key)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < STR_sgl_NumberOfFont; i++)
	{
		if(STR_sgpst_Font[i].ul_Key == _ul_Key) return &STR_sgpst_Font[i];
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL STR_b_LoadFontDescriptor(STR_tdst_FontDesc *pst_Font, ULONG _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_Pos, ul_Size, i_Index;
	char	*pc_Buf, *pc_BufInit, *pc_BufCheck;
	CHAR	szFileID[8];
    float   dw, dh;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Pos = BIG_ul_SearchKeyToPos(_ul_Key);
	if(ul_Pos == BIG_C_InvalidIndex) return FALSE;

	pc_Buf = pc_BufInit = BIG_pc_ReadFileTmp(ul_Pos, &ul_Size);
	
	if(ul_Size < 12) 
		return FALSE;

	LOA_ReadCharArray(&pc_Buf, szFileID, 8);

	if(L_strncmp(szFileID, "FONTDESC", 8)) 
		return FALSE;

	pc_BufCheck = pc_BufInit;
	pc_BufCheck += ul_Size - sizeof(ULONG);
	if(_LOA_ReadLong(&pc_BufCheck, NULL, LOA_eBinLookAheadData) != -1)
		return FALSE;

	pst_Font->ul_NbChar = LOA_ReadULong(&pc_Buf);
	if (pst_Font->ul_NbChar < 4 )
		pst_Font->ul_NbChar = 224;
	pst_Font->pst_Letter = (STR_tdst_FontLetterDesc *) MEM_p_Alloc( pst_Font->ul_NbChar * sizeof(STR_tdst_FontLetterDesc ) );
	L_memset( pst_Font->pst_Letter, 0, pst_Font->ul_NbChar * sizeof(STR_tdst_FontLetterDesc ) );

    dw = 1.0f / (float) pst_Font->uw_Width;
    dh = 1.0f / (float) pst_Font->uw_Height;
	
	while(pc_Buf < (pc_BufInit + ul_Size - sizeof(ULONG)))
	{
		i_Index = LOA_ReadULong(&pc_Buf);
		i_Index -= 32;
		if((i_Index < 0) || (i_Index >= pst_Font->ul_NbChar)) return FALSE;

		pst_Font->pst_Letter[i_Index].f_U[0] = LOA_ReadFloat(&pc_Buf);
		pst_Font->pst_Letter[i_Index].f_V[0] = LOA_ReadFloat(&pc_Buf);
		pst_Font->pst_Letter[i_Index].f_U[1] = LOA_ReadFloat(&pc_Buf);
		pst_Font->pst_Letter[i_Index].f_V[1] = LOA_ReadFloat(&pc_Buf);
	}

	if(LOA_ReadULong(&pc_Buf) != -1)
		return FALSE;
	return TRUE;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG STR_ul_SaveFontDescriptor(STR_tdst_FontDesc *pst_Font, char *sz_Path, char *sz_Name )
{
	/*~~~~~~~~~~~~~~*/
	int		i;
	ULONG	ul_Letter, ul_Key, ul_Index;
    TEX_tdst_File_Params st_Params;
	/*~~~~~~~~~~~~~~*/

	SAV_Begin(sz_Path, sz_Name);

	SAV_Buffer("FONTDESC", 8);
	SAV_Buffer(&pst_Font->ul_NbChar, 4);

	for(i = 0; i < (int )pst_Font->ul_NbChar; i++)
	{
		if(pst_Font->pst_Letter[i].f_U[1] == 0) continue;

		ul_Letter = i + 32;
		SAV_Buffer(&ul_Letter, 4);

		SAV_Buffer(&pst_Font->pst_Letter[i].f_U[0], 4);
		SAV_Buffer(&pst_Font->pst_Letter[i].f_V[0], 4);
		SAV_Buffer(&pst_Font->pst_Letter[i].f_U[1], 4);
		SAV_Buffer(&pst_Font->pst_Letter[i].f_V[1], 4);
	}

	ul_Letter = -1;
	SAV_Buffer(&ul_Letter, 4);

	ul_Index = SAV_ul_End();
    if (ul_Index == BIG_C_InvalidIndex) return BIG_C_InvalidIndex;
    ul_Key = BIG_FileKey( ul_Index );

    ul_Index = BIG_ul_SearchKeyToFat( pst_Font->ul_Key );
    if (ul_Index != BIG_C_InvalidIndex) 
    {
        TEX_File_GetParams( &st_Params, ul_Index );
        st_Params.ul_Params[0] = ul_Key;
        TEX_File_SetParams( &st_Params, ul_Index, FALSE);
    }
    return ul_Key;
}

/*
 =======================================================================================================================
	Create empty font
 =======================================================================================================================
 */
ULONG STR_ul_CreateFontDescriptor( ULONG _ul_Dir, char *_sz_Name, ULONG _ul_TexKey )
{
	char				sz_Path[ 260 ];
	STR_tdst_FontDesc	st_FontDesc;
	BIG_INDEX			ul_Index;

	ul_Index = BIG_ul_SearchFile( _ul_Dir, _sz_Name );
	if (ul_Index != BIG_C_InvalidIndex )
		return ul_Index;

	L_memset( &st_FontDesc, 0, sizeof( st_FontDesc ) );
	st_FontDesc.ul_NbChar = 224;
	st_FontDesc.pst_Letter = (STR_tdst_FontLetterDesc *) MEM_p_Alloc( 224 * sizeof( STR_tdst_FontLetterDesc ) );
	L_memset( st_FontDesc.pst_Letter, 0, 224 * sizeof( STR_tdst_FontLetterDesc ) );
	st_FontDesc.ul_Key = _ul_TexKey;

	BIG_ComputeFullName( _ul_Dir, sz_Path );
	STR_ul_SaveFontDescriptor( &st_FontDesc, sz_Path, _sz_Name ); 

	MEM_Free( st_FontDesc.pst_Letter );

	ul_Index = BIG_ul_SearchFile( _ul_Dir, _sz_Name );
	return ul_Index;
}


#endif


/*
 =======================================================================================================================
 =======================================================================================================================
 */
#define STR_C_FontAltKeyMax 16
static int   STR_FontAltKeyNumber = 0;
static ULONG STR_FontAltKey[ STR_C_FontAltKeyMax ][2];
/**/
void STR_FontAddAlternativeKey( ULONG _ul_Key, ULONG _ul_AltKey )
{
    if (STR_FontAltKeyNumber >= STR_C_FontAltKeyMax) return;
    STR_FontAltKey[ STR_FontAltKeyNumber ][ 0 ] = _ul_Key;
    STR_FontAltKey[ STR_FontAltKeyNumber ][ 1 ] = _ul_AltKey;
    STR_FontAltKeyNumber++;
}
/**/
void STR_FontResolveAlternativeKey( void )
{
    STR_tdst_FontDesc	*pst_Font;
    int i;

    for (i = 0; i < STR_FontAltKeyNumber; i++)
    {
        pst_Font = STR_pst_GetFont( STR_FontAltKey[ i ][ 0 ] );
        if (pst_Font)
            pst_Font->ul_AltKey = STR_FontAltKey[ i ][ 1 ];
    }
    STR_FontAltKeyNumber = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void STR_AddFont(ULONG _ul_Key, TEX_tdst_File_Desc *_pst_Tex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_FontDesc		*pst_Font, *pst_LastFD;
    int                     x;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Tex) return;
	if(!_pst_Tex->p_Bitmap) return;

	/* Search for font */
	pst_Font = NULL;
	if(STR_sgl_NumberOfFont)
	{
		pst_Font = STR_sgpst_Font;
		pst_LastFD = pst_Font + STR_sgl_NumberOfFont;

		for(; pst_Font < pst_LastFD; pst_Font++)
			if(pst_Font->ul_Key == _ul_Key) break;

		if(pst_Font == pst_LastFD) pst_Font = NULL;
	}

	if(pst_Font == NULL)
	{
		if(STR_sgl_NumberOfFont >= STR_sgl_MaxNumberOfFont)
		{
			STR_sgl_MaxNumberOfFont += 20;
			x = sizeof(STR_tdst_FontDesc) * (STR_sgl_MaxNumberOfFont);
			if(STR_sgpst_Font)
				STR_sgpst_Font = (STR_tdst_FontDesc *) MEM_p_Realloc(STR_sgpst_Font, x);
			else
				STR_sgpst_Font = (STR_tdst_FontDesc *) MEM_p_Alloc(x);
		}

		pst_Font = STR_sgpst_Font + STR_sgl_NumberOfFont++;
		L_memset( pst_Font, 0, sizeof( STR_tdst_FontDesc) );
		pst_Font->ul_Key = _ul_Key;
	}

	pst_Font->uw_Width = _pst_Tex->uw_Width;
	pst_Font->uw_Height = _pst_Tex->uw_Height;
	pst_Font->fWoH = pst_Font->uw_Height ? (float) _pst_Tex->uw_Width / (float) pst_Font->uw_Height : 1.0f;
	
	pst_Font->ul_NbChar = 0;
	pst_Font->pst_Letter = NULL;
	//L_memset(pst_Font->ast_Letter, 0, sizeof(pst_Font->ast_Letter));

	if(_pst_Tex->st_Params.ul_Params[0] != 0)
	{
		if(STR_b_LoadFontDescriptor(pst_Font, _pst_Tex->st_Params.ul_Params[0])) return;
	}

    return;
}

/* $on */
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
