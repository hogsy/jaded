/*$T VERsion_Update.cpp GC! 1.078 03/14/00 15:16:22 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/VERsion/VERsion_Update.h"

extern void VERsion_UpdateOneFileVersion2(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion3(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion4(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion5(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion6(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion7(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion8(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion9(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion10(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion11(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion12(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion13(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion14(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion15(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion16(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion17(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion18(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion19(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion20(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion21(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion22(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion23(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion24(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion25(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion26(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion27(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion28(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion29(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion30(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion31(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion32(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion33(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion34(char *, ULONG, char *, char *);
extern void VERsion_UpdateOneFileVersion35(char *, ULONG, char *, char *);
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VERsion_UpdateOneFile(ULONG _ul_Version, char *_pc_Buf, ULONG _ul_Len, char *_psz_Path, char *_psz_File)
{
	if(_ul_Version == 1) VERsion_UpdateOneFileVersion2(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 2) VERsion_UpdateOneFileVersion3(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 3) VERsion_UpdateOneFileVersion4(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 4) VERsion_UpdateOneFileVersion5(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 5) VERsion_UpdateOneFileVersion6(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 6) VERsion_UpdateOneFileVersion7(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 7) VERsion_UpdateOneFileVersion8(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 8) VERsion_UpdateOneFileVersion9(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 9) VERsion_UpdateOneFileVersion10(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 10) VERsion_UpdateOneFileVersion11(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 11) VERsion_UpdateOneFileVersion12(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 12) VERsion_UpdateOneFileVersion13(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 13) VERsion_UpdateOneFileVersion14(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 14) VERsion_UpdateOneFileVersion15(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 15) VERsion_UpdateOneFileVersion16(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 16) VERsion_UpdateOneFileVersion17(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 17) VERsion_UpdateOneFileVersion18(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 18) VERsion_UpdateOneFileVersion19(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 19) VERsion_UpdateOneFileVersion20(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 20) VERsion_UpdateOneFileVersion21(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 21) VERsion_UpdateOneFileVersion22(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 22) VERsion_UpdateOneFileVersion23(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 23) VERsion_UpdateOneFileVersion24(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 24) VERsion_UpdateOneFileVersion25(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
	if(_ul_Version == 25) VERsion_UpdateOneFileVersion26(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
    if(_ul_Version == 26) VERsion_UpdateOneFileVersion27(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
    if(_ul_Version == 27) VERsion_UpdateOneFileVersion28(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
    if(_ul_Version == 28) VERsion_UpdateOneFileVersion29(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
    if(_ul_Version == 29) VERsion_UpdateOneFileVersion30(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
    if(_ul_Version == 30) VERsion_UpdateOneFileVersion31(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
    if(_ul_Version == 31) VERsion_UpdateOneFileVersion32(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
    if(_ul_Version == 32) VERsion_UpdateOneFileVersion33(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
    if(_ul_Version == 33) VERsion_UpdateOneFileVersion34(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
    if(_ul_Version == 34) VERsion_UpdateOneFileVersion35(_pc_Buf, _ul_Len, _psz_Path, _psz_File);
}

#endif
