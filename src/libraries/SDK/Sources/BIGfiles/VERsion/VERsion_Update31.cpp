/*$T VERsion_Update29.cpp GC! 1.081 10/05/00 11:29:56 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGkey.h"

extern BIG_INDEX BIG_VERSION_ul_CurrentFile;

/*
 =======================================================================================================================
    Commentaire pour ce $%)#{[@^\#{| de CB
    changement :    swap les clés des fichiers tex et tga liés
 =======================================================================================================================
 */
void VERsion_UpdateOneFileVersion31(char *pc_Buf, ULONG _ul_Len, char *_psz_Path, char *_psz_File)
{
	/* Find extension of file */
	char *psz_Ext = strrchr( _psz_File, '.' );
    if (!psz_Ext) return;

    if ( L_strcmp(psz_Ext + 1, "tex" ) ) return;
    if (_ul_Len != 44) return;

    if ( BIG_FileChanged( BIG_VERSION_ul_CurrentFile ) & EDI_FHC_Touch )
        return;

    ULONG ul_File = *( ULONG * ) ( pc_Buf + 8 );
    if (ul_File == BIG_C_InvalidKey) return;
    ul_File = BIG_ul_SearchKeyToFat( ul_File );
    if (ul_File == BIG_C_InvalidIndex) return;

    if ( BIG_FileChanged( ul_File ) & EDI_FHC_Touch )
        return;

    ULONG ul_SaveKey = BIG_FileKey( BIG_VERSION_ul_CurrentFile );

    /* update tex file */
    *(ULONG *) (pc_Buf + 8) = ul_SaveKey;
    BIG_UpdateFileFromBuffer(_psz_Path, _psz_File, pc_Buf, 44);


    /* swap keys */
    BIG_gst.dst_FileTable[BIG_VERSION_ul_CurrentFile].ul_Key = BIG_FileKey( ul_File );
    BIG_gst.dst_FileTable[ul_File].ul_Key = ul_SaveKey;

    /* save fat */
    BIG_UpdateOneFileInFat( ul_File );
    BIG_UpdateOneFileInFat( BIG_VERSION_ul_CurrentFile );

    /* mark file */
    BIG_FileChanged( BIG_VERSION_ul_CurrentFile ) |= EDI_FHC_Touch;
    BIG_FileChanged( ul_File ) |= EDI_FHC_Touch;
}

#endif
