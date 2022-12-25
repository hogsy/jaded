/*$T VERsion_Update29.cpp GC! 1.081 10/05/00 11:29:56 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/VERsion/VERsion_Update.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "EDIpaths.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGkey.h"
#include "TEXture/TEXstruct.h"
#include "TEXture/TEXfile.h"

static int  si_NbPalette = 0;
static int  si_NbBadPalette = 0;
static int  si_NbPal4 = 0;
static int  si_PalMemBefore = 0;
static int  si_PalMemAfter = 0;

static int  si_NbRaw = 0;
static int  si_NbRaw4b = 0;
static int  si_NbBadRaw = 0;
static int  si_RawMemBefore = 0;
static int  si_RawMemAfter = 0;

/*
 =======================================================================================================================
    Commentaire pour ce $%)#{[@^\#{| de CB
    changement :
        palette et raw toutes les palettes/raw dont le nom se termine par 4Bits sont passés à 16 couleur au lieu de 256
        tex : on retire les extensions (4Bits ou 8Bits) du nom et on essaye de trouver un tga/bmp/jpeg avec le même nom
            pour le rajouter dans le tex.
 =======================================================================================================================
 */
void VERsion_UpdateOneFileVersion30(char *pc_Buf, ULONG _ul_Len, char *_psz_Path, char *_psz_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	                *psz_Ext;
    UCHAR                   *puc_Tgt, *puc_Src;
    TEX_tdst_File_Params    st_Params;
    int                     count;
    BOOL                    b_4Bit;
    ULONG                   ul_File, ul_TextureDir;
    char                    sz_NewName[ BIG_C_MaxLenName ];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Find extension of file */
	psz_Ext = strrchr(_psz_File, '.');
    if (!psz_Ext) return;

    /*
    if (!L_strnicmp( _psz_File, "Ombre", 5) )
    {
        puc_Tgt = NULL;
    }
    */
	
    /* palette : just ensure that palette is not empty */
    if (!strcmp(psz_Ext + 1, "pal" ) )
    {
        si_NbPalette++;
        si_PalMemBefore += _ul_Len;
        si_PalMemAfter += _ul_Len;

        b_4Bit = ((psz_Ext - 5) >= _psz_File) && (L_strnicmp( psz_Ext-5, "4bits",5) == 0);
        if (!_ul_Len) 
        {
            si_NbBadPalette++;
            puc_Tgt = (UCHAR *) L_malloc( 1024 );
            _ul_Len = (b_4Bit ? 16 : 256) * 3;
            L_memset( puc_Tgt, 0, _ul_Len );
            BIG_UpdateFileFromBuffer(_psz_Path, _psz_File, puc_Tgt, _ul_Len);
            L_free( puc_Tgt );
        }
        else if (b_4Bit)
        {
            si_NbPal4++;
            for ( count = 0; count < 48; count++)
                if (pc_Buf[count] != 0) break;
            if (count == 48)
            {
                for (count = 0; count < 16; count++)
                {
                    pc_Buf[ count * 3    ] = pc_Buf[ (255 - count) * 3    ];
                    pc_Buf[ count * 3 + 1] = pc_Buf[ (255 - count) * 3 + 1];
                    pc_Buf[ count * 3 + 2] = pc_Buf[ (255 - count) * 3 + 2];
                }
            }
            si_PalMemAfter -= _ul_Len;
            si_PalMemAfter += 48;
            BIG_UpdateFileFromBuffer(_psz_Path, _psz_File, pc_Buf, 48);
        }
        return;
    }

    /* Empty File ... We do nothing ... */
    if (!_ul_Len) return;

    /* raw : have just to decrease color depth (each raw with a name terminated by 4Bits is actually a 8bit indexed raw) */
    /*      we have to decrease its color depth to 4bit */
    if (!strcmp(psz_Ext + 1, "raw" ) )
    {
        si_NbRaw++;
        si_RawMemBefore += _ul_Len;
        si_RawMemAfter += _ul_Len;

        b_4Bit = ((psz_Ext - 5) >= _psz_File) && (L_strnicmp( psz_Ext-5, "4bits",5) == 0);
        
        if (!b_4Bit) return;
        si_NbRaw4b++;
        L_memcpy( &st_Params, pc_Buf + _ul_Len - 32, 32 );
        /* check format */
        if (st_Params.uc_Type != TEX_FP_RawFile)
        {
            si_NbBadRaw++;
            return;
        }
        /* check size */
        count = st_Params.uw_Height * st_Params.uw_Width;
        if (count != (int) (_ul_Len - 32))
        {
            si_NbBadRaw++;
            return;
        }

        puc_Src = puc_Tgt = (UCHAR *) pc_Buf;
        if ( *puc_Tgt > 16 )
        {
            while (count)
            {
                *puc_Tgt++ = ((255 - *puc_Src) << 4) | ( (255 - *(puc_Src + 1)) & 0xF);
                puc_Src += 2;
                count -= 2;
            }
        }
        else
        {
            while (count)
            {
                *puc_Tgt++ = (*puc_Src << 4) | ( *(puc_Src + 1) & 0xF);
                puc_Src += 2;
                count -= 2;
            }
        }
        si_RawMemAfter -= (_ul_Len - 32) / 2;
        st_Params.uc_Format = TEX_FP_4bpp;
        L_memcpy( puc_Tgt, &st_Params, 32 );
        BIG_UpdateFileFromBuffer(_psz_Path, _psz_File, pc_Buf, (puc_Tgt - (UCHAR *) pc_Buf) + 32);
        return;
    }

    /* tex : if there's a 4bit or 8bit terminaison into name we delete it */
    /* we also try to find a tga / jpg / bmp with same name */
    /*      if so we add the key to the text file */
    if (!strcmp(psz_Ext + 1, "tex" ) )
    {
        if ( (psz_Ext - 6 > _psz_File) && (*(psz_Ext - 6) == '_') && ( (*(psz_Ext - 5) == '4') || (*(psz_Ext - 5) == '8') ) && (L_strnicmp( psz_Ext - 4, "bits", 4) == 0) )
        {
            strcpy( sz_NewName, _psz_File );
            psz_Ext = strrchr( sz_NewName, '.' );
            psz_Ext -= 6;
            L_strcpy( psz_Ext, ".tex" );
        
            ul_File = BIG_ul_SearchFileExt(_psz_Path, sz_NewName);
            while (ul_File != BIG_C_InvalidKey)
            {
                L_strcpy( psz_Ext, "_.tex" );
                psz_Ext++;
                ul_File = BIG_ul_SearchFileExt(_psz_Path, sz_NewName);
            }
            BIG_RenFile( sz_NewName, _psz_Path, _psz_File);
            _psz_File = sz_NewName;
        }

        if (_ul_Len != 40) return;

        ul_TextureDir = BIG_ul_CreateDir(EDI_Csz_Path_Textures);
        L_strcpy( psz_Ext + 1, "tga" );
        ul_File = BIG_ul_SearchFileInDirRec(ul_TextureDir, _psz_File );
        if (ul_File == BIG_C_InvalidIndex)
        {
            L_strcpy( psz_Ext + 1, "bmp" );
            ul_File = BIG_ul_SearchFileInDirRec(ul_TextureDir, _psz_File );
            if (ul_File == BIG_C_InvalidIndex)
            {
                L_strcpy( psz_Ext + 1, "jpg" );
                ul_File = BIG_ul_SearchFileInDirRec(ul_TextureDir, _psz_File );
            }
        }
        L_strcpy( psz_Ext, ".tex" );

        if (ul_File != BIG_C_InvalidIndex)
        {
            puc_Tgt = (UCHAR *) L_malloc( 44 );
            L_memcpy( puc_Tgt, pc_Buf, 8 );
            L_memcpy( puc_Tgt + 12, pc_Buf + 8, 32 );
            *(ULONG *) (puc_Tgt + 8) = BIG_FileKey( ul_File );
            BIG_UpdateFileFromBuffer(_psz_Path, _psz_File, puc_Tgt, 44);
            L_free( puc_Tgt );
        }
        return;
    }


}

#endif
