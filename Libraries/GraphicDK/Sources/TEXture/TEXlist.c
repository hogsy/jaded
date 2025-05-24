/*$T TEXlist.c GC! 1.081 10/11/00 14:39:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "TEXture/TEXstruct.h"
#include "TEXture/TEXfile.h"
#include "GDInterface/GDInterface.h"
#include "GraphicDK/Sources/MATerial/MATstruct.h"
#include "GraphicDK/Sources/MATerial/MATSprite.h"
#include "BIGfiles/BIGfat.h"

#include "../Main/Shared/FileSystem/FileSystem.h"

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */
#ifdef JADEFUSION
extern ULONG		LOA_gul_CurrentKey;
#endif
TEX_tdst_List	TEX_gst_GlobalList;
#ifdef ACTIVE_EDITORS
LONG			TEX_gal_MadToGlobalIndex[1024];
#endif
#ifdef _XBOX
int xb_NumberOfPalettes;
#endif
/*$4
 ***********************************************************************************************************************
    Public functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Description:: Initialize a list of texture
 =======================================================================================================================
 */
void TEX_List_Init(TEX_tdst_List *_pst_TexList)
{
	L_memset(_pst_TexList, 0, sizeof(TEX_tdst_List));
}

/*
 =======================================================================================================================
    Description:: Free a list of texture
 =======================================================================================================================
 */
void TEX_List_Free(TEX_tdst_List *_pst_TexList)
{
	/*~~*/
	int i;
	/*~~*/

	if(_pst_TexList->l_NumberMaxOfTextures) 
        TEX_M_File_Free(_pst_TexList->dst_Texture);

	if(_pst_TexList->l_NumberMaxOfPalettes)
	{
		
		for(i = 0; i < _pst_TexList->l_NumberOfPalettes; i++)
		{
			if(_pst_TexList->dst_Palette[i].pul_Color) 
            {
                TEX_M_File_Free(_pst_TexList->dst_Palette[i].pul_Color);
                _pst_TexList->dst_Palette[i].pul_Color = NULL;
            }
		}

		TEX_M_File_Free(_pst_TexList->dst_Palette);
	}

	L_memset(_pst_TexList, 0, sizeof(TEX_tdst_List));
}

/*$4
 ***********************************************************************************************************************
    texture list
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
//extern int	WOR_gi_CurrentConsole;
static	int si_SpecialLangageVersion = -1;
extern  int AI_EvalFunc_IoGameLangGet_C(void);

#define SPECIALLANG_POLISH	(1 << 10)
#define SPECIALLANG_RUSSE	(1 << 11)

ULONG TEX_ul_List_ChangeKey( BIG_KEY _ul_FileKey )
{
	// Je remets une couche de bordel : texture de fonte qui change en fonction de la version russe ou polonaise ou normal
	if ( si_SpecialLangageVersion == -1 )
	{
		si_SpecialLangageVersion = AI_EvalFunc_IoGameLangGet_C();
		
		//pour forcer le polonais 
		//si_SpecialLangageVersion = SPECIALLANG_POLISH;
		//pour forcer le russe		
		//si_SpecialLangageVersion = SPECIALLANG_RUSSE;
		
		if (si_SpecialLangageVersion & SPECIALLANG_POLISH)
			si_SpecialLangageVersion = 1;
		else if (si_SpecialLangageVersion & SPECIALLANG_RUSSE)
			si_SpecialLangageVersion = 2;
		else
			si_SpecialLangageVersion = 0;
	}

	switch ( si_SpecialLangageVersion )
	{
	case 1: // polonais
		if		(_ul_FileKey == 0x4902d956 )	_ul_FileKey = 0x4902ef4b;		// copperplate
		else if (_ul_FileKey == 0x4902ef01 )	_ul_FileKey = 0x49030547;		// copperplate small
		else if (_ul_FileKey == 0x4902dc12 )	_ul_FileKey = 0x4902ef4f;		// courrier
		else if (_ul_FileKey == 0x4902ef04 )	_ul_FileKey = 0x4903054b;		// courrier small
		break;
	case 2: // russe
		if		(_ul_FileKey == 0x4902d956 )	_ul_FileKey = 0x4902ef53;		// copperplate
		else if (_ul_FileKey == 0x4902ef01 )	_ul_FileKey = 0x49030531;		// copperplate small
		else if (_ul_FileKey == 0x4902dc12 )	_ul_FileKey = 0x4902ef57;		// courrier
		else if (_ul_FileKey == 0x4902ef04 )	_ul_FileKey = 0x4903053A;		// courrier small
		break;
	}

	return _ul_FileKey;

#ifdef OLDCODEQUIDOITPLUSSERVIR
	if ( ((_ul_FileKey & 0xFFFFFF0F) == 0) && (_ul_FileKey & 0xF0) )
	{
        switch (WOR_gi_CurrentConsole)
        {
        case 0: break;  /* PC */
        case 1: break;  /* PS2 */
        case 2: _ul_FileKey += 1; break;  /* GAMECUBE */
        case 3: _ul_FileKey += 2; break;  /* XBOX */
        }
    }
    
    {
	// clé special E3
	// GAMECUBE
	if (WOR_gi_CurrentConsole == 2 )
	{
		if (_ul_FileKey == 0x1F0016B6 )
			_ul_FileKey = 0x8F005893;
		else if (_ul_FileKey == 0x62000872 )
			_ul_FileKey = 0x8F005895;
		else if (_ul_FileKey == 0x720006F4)
			_ul_FileKey = 0x8F005897;
		else if (_ul_FileKey == 0x62000F6D)
			_ul_FileKey = 0x8F005899;
	}
	else if (WOR_gi_CurrentConsole == 3 )
	{
		if (_ul_FileKey == 0x1F0016B6 )
			_ul_FileKey = 0x8F005894;
		else if (_ul_FileKey == 0x62000872 )
			_ul_FileKey = 0x8F005896;
		else if (_ul_FileKey == 0x720006F4)
			_ul_FileKey = 0x8F005898;
		else if (_ul_FileKey == 0x62000F6D)
			_ul_FileKey = 0x8F00589A;
	}
	// fin clé spécial E3
    }
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
short TEX_w_List_AddTexture(TEX_tdst_List *_pst_TexList, BIG_KEY _ul_FileKey, char _c_RealTexture )
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_Data	*pst_Tex;
	LONG			l_Size;
	/*~~~~~~~~~~~~~~~~~~~~~*/

#ifdef _DEBUG
	{
		static ULONG test = -1;
		if(_ul_FileKey == test)
			test = test;
	}
#endif

	_ul_FileKey = TEX_ul_List_ChangeKey( _ul_FileKey );

    if(_ul_FileKey == BIG_C_InvalidKey) return -1;

/* check texture : do not allow a raw referenced by a material */
#ifdef ACTIVE_EDITORS
    if ( _c_RealTexture )
    {
        ULONG   ul_Index;

        ul_Index = Jaded_FileSystem_GetFileIndexByKey( _ul_FileKey );
        if (ul_Index == BIG_C_InvalidIndex ) 
        {
			extern ULONG LOA_gul_CurrentKey;

            char sz_Text[ 256 ];
			snprintf( sz_Text, sizeof( sz_Text ), "[%08X] is an invalid key, associated with [%08X]", _ul_FileKey, LOA_gul_CurrentKey );
			ERR_X_Warning( 0, sz_Text, NULL );
            return -1;
        }

		const char *filename = Jaded_FileSystem_GetFilePathByIndex( ul_Index );
        if ( !TEX_l_File_IsFormatSupported( filename, -1) )
        {
            char sz_Text[ 256 ];
			snprintf( sz_Text, sizeof( sz_Text ), "[%08X] is referenced as a real texture but isn't ! - CHECK TEXTURE REF", _ul_FileKey );
            ERR_X_Warning( 0, sz_Text, NULL );
            return -1;
        }
    }
#endif

    /* cherche si la texture est déja dans la liste */
    if ( _pst_TexList->l_NumberOfTextures )
    {
        pst_Tex = TEX_pst_List_FindTexture(_pst_TexList, _ul_FileKey);
		if(pst_Tex) return pst_Tex->w_Index;
    }

    /* nouvelle texture : no la rajoute */
    /* y'a t'il de la place */
	if(_pst_TexList->l_NumberOfTextures == _pst_TexList->l_NumberMaxOfTextures)
    {
        _pst_TexList->l_NumberMaxOfTextures += 256;
        l_Size = sizeof(TEX_tdst_Data) * _pst_TexList->l_NumberMaxOfTextures;

        if ( _pst_TexList->l_NumberMaxOfTextures == 256 )
		    TEX_M_File_Alloc(_pst_TexList->dst_Texture, l_Size, TEX_tdst_Data)
	    else
	        TEX_M_File_Realloc(_pst_TexList->dst_Texture, l_Size, TEX_tdst_Data)
	}

	pst_Tex = &_pst_TexList->dst_Texture[_pst_TexList->l_NumberOfTextures++];

	L_memset(pst_Tex, 0, sizeof(TEX_tdst_Data));
	pst_Tex->ul_Key = _ul_FileKey;
	pst_Tex->w_Index = (short)(_pst_TexList->l_NumberOfTextures - 1);

	return pst_Tex->w_Index;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
TEX_tdst_Data *TEX_pst_List_FindTexture(TEX_tdst_List *_pst_TexList, BIG_KEY _ul_FileKey)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_Data	*pst_Tex, *pst_LastTex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_ul_FileKey = TEX_ul_List_ChangeKey( _ul_FileKey );

	pst_Tex = _pst_TexList->dst_Texture;
	pst_LastTex = pst_Tex + _pst_TexList->l_NumberOfTextures;
	for(; pst_Tex < pst_LastTex; pst_Tex++)
	{
		if(pst_Tex->ul_Key == _ul_FileKey) return pst_Tex;
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BIG_KEY TEX_ul_List_FindTexture(TEX_tdst_List *_pst_TexList, short w_Id)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_Data	*pst_Tex, *pst_LastTex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Tex = _pst_TexList->dst_Texture;
	pst_LastTex = pst_Tex + _pst_TexList->l_NumberOfTextures;
	for(; pst_Tex < pst_LastTex; pst_Tex++)
	{
		if(pst_Tex->w_Index == w_Id) return pst_Tex->ul_Key;
	}

	return BIG_C_InvalidKey;
}

/*$4
 ***********************************************************************************************************************
    palette list
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
short TEX_w_List_AddPalette(TEX_tdst_List *_pst_TexList, BIG_KEY _ul_FileKey)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_Palette	*pst_Pal;
	LONG				l_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ul_FileKey == BIG_C_InvalidKey) return -1;

    /* la palette est déja dans la liste ? */
    if(_pst_TexList->l_NumberOfPalettes)
    {
        pst_Pal = TEX_pst_List_FindPalette(_pst_TexList, _ul_FileKey);
		if(pst_Pal) 
            return pst_Pal->w_Index;
    }

    /* nouvelle palette : on l'ajoute */

    /* y'a t'il de la place */
    if (_pst_TexList->l_NumberOfPalettes == _pst_TexList->l_NumberMaxOfPalettes)
    {
        _pst_TexList->l_NumberMaxOfPalettes += 256;
        l_Size = sizeof(TEX_tdst_Palette) * _pst_TexList->l_NumberMaxOfPalettes;
	    if(_pst_TexList->l_NumberMaxOfPalettes == 256)
		    TEX_M_File_Alloc(_pst_TexList->dst_Palette, l_Size, TEX_tdst_Palette)
	    else
		    TEX_M_File_Realloc(_pst_TexList->dst_Palette, l_Size, TEX_tdst_Palette)
	}

	pst_Pal = &_pst_TexList->dst_Palette[_pst_TexList->l_NumberOfPalettes++];
#ifdef _XBOX
xb_NumberOfPalettes=_pst_TexList->l_NumberOfPalettes;
#endif

	L_memset(pst_Pal, 0, sizeof(TEX_tdst_Palette));
	pst_Pal->ul_Key = _ul_FileKey;
	pst_Pal->w_Index = (short)(_pst_TexList->l_NumberOfPalettes - 1);

	return pst_Pal->w_Index;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
TEX_tdst_Palette *TEX_pst_List_FindPalette(TEX_tdst_List *_pst_TexList, BIG_KEY _ul_FileKey)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_Palette	*pst_Pal, *pst_LastPal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Pal = _pst_TexList->dst_Palette;
	pst_LastPal = pst_Pal + _pst_TexList->l_NumberOfPalettes;
	for(; pst_Pal < pst_LastPal; pst_Pal++)
	{
		if(pst_Pal->ul_Key == _ul_FileKey) return pst_Pal;
	}

	return NULL;
}

