/*$T BIGkey.c GC! 1.098 11/27/00 14:22:02 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGerrid.h"
#include "BASe/BAStypes.h"
#include "LOAding/LOAdefs.h"

#if defined ACTIVE_EDITORS
#include "BASe/ERRors/ERRasser.h"
#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "LINks\LINKmsg.h"
#include <fcntl.h>

BOOL	BIG_gb_CanAskKey = TRUE;
BIG_KEY	BIG_gul_CanAskKey = -1;

/*
 =======================================================================================================================
    Aim:    Compute a key id (high byte of a complete key) depending on current computer name. As key ID is 8 bits
            LONG, we are sure that this function will no return different IDs for each compute names. So this is a
            simple function to facilitate creation of key ID, but the ID will need to be hands changed to be sure that
            all users have a different ID.

    Out:    One byte for the ID.
 =======================================================================================================================
 */
UCHAR BIG_uc_ComputeKeyID(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	char	asz_Name[100];
	int		i_Len;
	UCHAR	uc_Val;
	int		i;
	/*~~~~~~~~~~~~~~~~~~*/

	i_Len = 99;
	uc_Val = 0;

#ifdef JADEFUSION
	GetComputerName(asz_Name, (LPDWORD)&i_Len);
#else
	GetComputerName(asz_Name, &i_Len);
#endif

	/* A simple hashing routine depending on compute name */
	uc_Val = 0;
	for(i = 0; i < i_Len; i++) uc_Val = (uc_Val << 32) + L_toupper(asz_Name[i]);

	switch(uc_Val)
	{
	case 0x00:	
	case 0xFF:	
	case 0xFE:	
	case 0xFD:	
		uc_Val = 1; 
		break;
	}

	return uc_Val;
}

/*
 =======================================================================================================================
    Aim:    To store a new key ID in registry table.

    In:     _uc_Key     The key ID to store.
 =======================================================================================================================
 */
void BIG_SetKeyID(UCHAR _uc_Key)
{
	/*~~~~~~~~~~~~~~~~*/
	HKEY	h_Key;
	char	asz_ID[100];
	/*~~~~~~~~~~~~~~~~*/

	ERR_X_Error(RegCreateKey(HKEY_CURRENT_USER, KEY_ROOT, &h_Key) == ERROR_SUCCESS, ERR_BIG_Csz_CantFindKey, NULL);
	sprintf(asz_ID, "%d", _uc_Key);
	RegSetValue(h_Key, KEY_ID, REG_SZ, asz_ID, L_strlen(asz_ID));
	RegCloseKey(h_Key);
}

/*
 =======================================================================================================================
    Aim:    To get actual key ID in registry table.

    In:     To retreive a string of the ID (to display it). 

    Out:    Return the 8bits of the key ID.
 =======================================================================================================================
 */
UCHAR BIG_uc_GetKeyID(char *_psz_ID)
{
	/*~~~~~~~~~~~~~~~~*/
	HKEY	h_Key;
	char	asz_ID[100];
	LONG	l_Len;
	/*~~~~~~~~~~~~~~~~*/

	l_Len = 99;

	ERR_X_Error(RegOpenKey(HKEY_CURRENT_USER, KEY_ROOT, &h_Key) == ERROR_SUCCESS, ERR_BIG_Csz_CantFindKey, NULL);

	if(_psz_ID == NULL) _psz_ID = asz_ID;

	RegQueryValue(h_Key, KEY_ID, _psz_ID, &l_Len);
	RegCloseKey(h_Key);
	return (UCHAR) L_atol(_psz_ID);
}

/*
 =======================================================================================================================
    Aim:    Store a new value in the registry.

    In:     _ul_Value   Value to store.
 =======================================================================================================================
 */
void BIG_SetKeyValue(ULONG _ul_Value)
{
	/*~~~~~~~~~~~~~~~~~~*/
	HKEY	h_Key;
	char	asz_Value[50];
	/*~~~~~~~~~~~~~~~~~~*/

	ERR_X_Error(RegCreateKey(HKEY_CURRENT_USER, KEY_ROOT, &h_Key) == ERROR_SUCCESS, ERR_BIG_Csz_CantFindKey, NULL);
	sprintf(asz_Value, "%d", _ul_Value);
	RegSetValue(h_Key, KEY_VALUE, REG_SZ, asz_Value, L_strlen(asz_Value));
	RegCloseKey(h_Key);
}

/*
 =======================================================================================================================
    Aim:    To retrive the key value in the registry.

    In:     To retreive value as a string (to display it). 

    Out:    Return value.
 =======================================================================================================================
 */
ULONG BIG_ul_GetKeyValue(char *_psz_Value)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	HKEY	h_Key;
	char	asz_Value[100];
	LONG	l_Len;
	/*~~~~~~~~~~~~~~~~~~~*/

	l_Len = 99;

	ERR_X_Error(RegOpenKey(HKEY_CURRENT_USER, KEY_ROOT, &h_Key) == ERROR_SUCCESS, ERR_BIG_Csz_CantFindKey, NULL);

	if(_psz_Value == NULL) _psz_Value = asz_Value;

	RegQueryValue(h_Key, KEY_VALUE, _psz_Value, &l_Len);
	RegCloseKey(h_Key);
	return (ULONG) L_atol(_psz_Value);
}

/*
 =======================================================================================================================
    Aim:    To retreive the bfsize value from the registry.
    Out:    BF size in registry, or 0 if key was not found.
 =======================================================================================================================
 */
ULONG BIG_ul_GetRegistryBfSize()
{
	/*~~~~~~~~~~~~~~~~~~~*/
	char	asz_Value[100];
	LONG	l_Len = 99;
	/*~~~~~~~~~~~~~~~~~~~*/

	if (ERROR_SUCCESS == RegQueryValue(HKEY_CURRENT_USER, KEY_BFSIZE, asz_Value, &l_Len))
    {
	    return (ULONG) L_atol(asz_Value);
    }
    else
        return 0;
}


/*
 =======================================================================================================================
    Aim:    To get the bf size.
 =======================================================================================================================
 */
extern BIG_tdst_BigFile	BIG_gst;

ULONG BIG_ul_GetBFSize()
{
    int fh = _open( BIG_gst.asz_Name, _O_RDONLY, _S_IREAD);
    if (fh == -1)
    {
        return 0;
    }
    else
    {
        ULONG ulBfSize = _filelength(fh);
        _close( fh );
        return ulBfSize;
    }
}

/*
 =======================================================================================================================
    Aim:    To write the bfsize value in the registry.
 =======================================================================================================================
 */
BOOL BIG_bSaveBfSize = TRUE;
void BIG_SetRegistryBfSize()
{
	/*~~~~~~~~~~~~~~~~~~*/
	HKEY	h_Key;
	char	asz_Value[50];
    ULONG   ulBFSize;
	/*~~~~~~~~~~~~~~~~~~*/

    if (!BIG_bSaveBfSize) return;

    ulBFSize = BIG_ul_GetBFSize();

    if (ulBFSize)
    {
    	ERR_X_Error(RegCreateKey(HKEY_CURRENT_USER, KEY_BFSIZE, &h_Key) == ERROR_SUCCESS, "Can't create key" , NULL);
        sprintf(asz_Value, "%d", ulBFSize);
        RegSetValue(h_Key, NULL, REG_SZ, asz_Value, L_strlen(asz_Value));
        RegCloseKey(h_Key);
    }
}


/*
 =======================================================================================================================
    Aim:    Determins if the windows registry table has informations for keys of application. This will seach KEY_ROOT
            string in HKEY_CURRENT_USER path.

    Out:    TRUE if registry is OK, FALSE else.
 =======================================================================================================================
 */
BOOL BIG_b_IsKeyExists(void)
{
	/*~~~~~~~~~~*/
	HKEY	h_Key;
	/*~~~~~~~~~~*/

	if(RegOpenKey(HKEY_CURRENT_USER, KEY_ROOT, &h_Key) == ERROR_SUCCESS)
	{
		RegCloseKey(h_Key);
		return TRUE;
	}

	return FALSE;
}

/*
 =======================================================================================================================
    Aim:    To compute a new key for a given file. This function will be called when a new file is created.

    In:     _ul_Fat     Position of the file in the fat. It is used to retreive the position of the file in the
                        bigfile, cause that position is the associated value of the key in the global table. 

    Out:    The new key.
 =======================================================================================================================
 */
BIG_KEY BIG_ul_GetNewKey(BIG_INDEX _ul_Fat)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_Key, ul_Value;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	if(!BIG_gb_CanAskKey) return BIG_gul_CanAskKey;

	/* Compute new key */
	ul_Key = BIG_uc_GetKeyID(NULL);
	ul_Key <<= 24;
	ul_Key += (ul_Value = BIG_ul_GetKeyValue(NULL));

    // If key already exists, display warning.
    {
        char sMsg[256];
        sprintf(sMsg,"Creating key which already exists %x",ul_Key);
        ERR_X_Warning(BIG_ul_SearchKeyToPos(ul_Key) == BIG_C_InvalidIndex ,sMsg ,NULL);
    }

	/* Insert key in table */
	if ( _ul_Fat != BIG_C_InvalidKey )
		BIG_InsertKeyToFat(ul_Key, _ul_Fat);

	/* Remember for the next time the next valid value */
	BIG_SetKeyValue(ul_Value + 1);
	return ul_Key;
}

#endif /* ACTIVE_EDITORS */

/*
 * Aim: To insert a new key, with a new associated value (position in bigfile), in
 * the global table. In: _ul_Key Key to insert _ul_Pos Asociated valued (position
 * of the file in bigfile).
 */
#ifndef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_InsertKeyToPos(BIG_KEY _ul_Key, ULONG _ul_Pos)
{
	BAS_binsert(_ul_Key, _ul_Pos, &BIG_gst.st_KeyTableToPos);
}

#endif

/* Aim: To delete a given key in the global table. In: _ul_Key Key to delete. */
#ifndef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_DeleteKeyToPos(BIG_KEY _ul_Key)
{
	if(_ul_Key == BIG_C_InvalidKey) return;
	BAS_bdelete(_ul_Key, &BIG_gst.st_KeyTableToPos);
}

#endif

/*
 =======================================================================================================================
    Aim:    Search a given key in global table.

    In:     _ul_Key     Key to search. 

    Out:    -1 if key is not here, else value associated with the key.
 =======================================================================================================================
 */
ULONG BIG_ul_SearchKeyToPos(BIG_KEY _ul_Key)
{
#if defined(XML_CONV_TOOL)
	return 0;
#else
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	h;
	extern BOOL LOA_gb_SpeedMode; 
	extern ULONG LOA_ul_ExFileKey(ULONG);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
	// can exchange the key with another one
	_ul_Key = LOA_ul_ExFileKey(_ul_Key);

	h = BAS_bsearch(_ul_Key, &BIG_gst.st_KeyTableToFat);
	if(h == BIG_C_InvalidIndex) return BIG_C_InvalidIndex;
	return BIG_PosFile(h);
#else
    h = BAS_bsearch(_ul_Key, &BIG_gst.st_KeyTableToPos);
	if(LOA_gb_SpeedMode && ((int) h == -1) ) return _ul_Key;
	return (h);
#endif
#endif // XML_CONV_TOOL
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
    Aim:    To insert a new key, with a new associated value (bigfile fat pos), in the global table.

    In:     _ul_Key     Key to insert
            _ul_Fat     Asociated valued (bigfile fat position).
 =======================================================================================================================
 */
void BIG_InsertKeyToFat(BIG_KEY _ul_Key, BIG_INDEX _ul_Fat)
{
	BAS_binsert(_ul_Key, _ul_Fat, &BIG_gst.st_KeyTableToFat);
	BAS_binsert(BIG_PosFile(_ul_Fat), _ul_Fat, &BIG_gst.st_PosTableToFat);
}

/*
 =======================================================================================================================
    Aim:    To delete a given key in the global table.

    In:     _ul_Key     Key to delete.
 =======================================================================================================================
 */
void BIG_DeleteKeyToFat(BIG_KEY _ul_Key)
{
	BIG_INDEX	ul;
	if(_ul_Key == BIG_C_InvalidKey) return;
	ul = BIG_ul_SearchKeyToFat(_ul_Key);
	BAS_bdelete(BIG_PosFile(ul), &BIG_gst.st_PosTableToFat);
	BAS_bdelete(_ul_Key, &BIG_gst.st_KeyTableToFat);
}

/*
 =======================================================================================================================
    Aim:    Search a given key in global table.

    In:     _ul_Key     Key to search. 

    Out:    -1 if key is not here, else value associated with the key.
 =======================================================================================================================
 */
ULONG BIG_ul_SearchKeyToFat(BIG_KEY _ul_Key)
{
#if !defined(XML_CONV_TOOL)
	return BAS_bsearch(_ul_Key, &BIG_gst.st_KeyTableToFat);
#else
	return 0;
#endif // XML_CONV_TOOL
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_ChangeKey(BIG_INDEX _ul_Index, BIG_KEY _ul_NewKey)
{
	BIG_DeleteKeyToFat(BIG_FileKey(_ul_Index));

	BIG_FileKey(_ul_Index) = _ul_NewKey;
	BIG_InsertKeyToFat(_ul_NewKey, _ul_Index);
	BIG_UpdateOneFileInFat(_ul_Index);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_GetLastKeyForUser(int *_pi_Res,ULONG _ul_UserID)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	i;
	BIG_KEY		ul_Key;
	/*~~~~~~~~~~~~~~~~~~~~*/

	*_pi_Res = 0;
	for(i = BIG_Root(); i < (int) BIG_MaxFile(); i++)
	{
		ul_Key = BIG_FileKey(i);
		if((ul_Key >> 24) == _ul_UserID)
		{
			ul_Key &= 0x00FFFFFF;
			if(ul_Key > (BIG_INDEX) * _pi_Res) *_pi_Res = ul_Key;
		}

		i++;
	}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_GetLastKey(int *_pi_Res)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	i;
	BIG_KEY		ul_Key;
	char		asz_ID[128];
	UCHAR		id;
	/*~~~~~~~~~~~~~~~~~~~~*/

	*_pi_Res = 0;
	id = BIG_uc_GetKeyID(asz_ID);
	for(i = BIG_Root(); i < (int) BIG_MaxFile(); i++)
	{
		ul_Key = BIG_FileKey(i);
		if((ul_Key >> 24) == id)
		{
			ul_Key &= 0x00FFFFFF;
			if(ul_Key > (BIG_INDEX) * _pi_Res) *_pi_Res = ul_Key;
		}

		i++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_GetAllIdKey(void)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	int	i;
	BIG_KEY		ul_Key;
	char		asz_ID[128];
	BAS_tdst_barray st_IdKey;
	/*~~~~~~~~~~~~~~~~~~~~*/

    LINK_PrintStatusMsg("All Id Key used in this bigfile :");

    LINK_PrintStatusMsg("Forbiden :");
    LINK_PrintStatusMsg("FF");
    LINK_PrintStatusMsg("FE");
    LINK_PrintStatusMsg("FD");

    LINK_PrintStatusMsg("Currently used :");
    BAS_binit(&st_IdKey, 50);
	for(i = BIG_Root(); i < (int) BIG_MaxFile(); i++)
	{
		ul_Key = BIG_FileKey(i);
        if(ul_Key == -1) continue;
        if(ul_Key == 0) continue;

		ul_Key >>= 24;
        BAS_binsert(ul_Key, ul_Key, &st_IdKey);
	}

    for(i=0; i<st_IdKey.num; i++)
    {
        sprintf(asz_ID, "%02X", st_IdKey.base[i].ul_Key);
        LINK_PrintStatusMsg(asz_ID);
    }

    BAS_bfree(&st_IdKey);
}

#endif
