#include "windows.h"
#include "shlobj.h"
/*
 ---------------------------------------------------------------------------------------
  callback for browse window
 ---------------------------------------------------------------------------------------
 */
int _stdcall fn_CBBrowse( HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
	lParam = lParam;
	if ( uMsg == BFFM_INITIALIZED)
	{
		SendMessage( hWnd, BFFM_SETSELECTION, 1, lpData );
	}
	return 0;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : choose a folder
  _oWnd               -> CWnd that calls
  _szTitle            -> title to display in browse window
  _szPath             -> _szPath to store choosen path
						_szPath can be used to pass old path.
						if not set this path empty
  Returns (BOOL ) TRUE if new folder is chosen, otherwise false
 ----------------------------------------------------------------------------------------
 */
int fn_SH_bBrowseForFolder( /*HWND _p_oWnd, */char *_szTitle, char *_szPath )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//    HWND _p_oWnd;
	BROWSEINFO		stBI;
	ITEMIDLIST		*p_stItem;
	char			szDisplayName[ MAX_PATH ];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	stBI.hwndOwner = NULL;
	stBI.pidlRoot = NULL;
    stBI.pszDisplayName = szDisplayName;
    stBI.lpszTitle = _szTitle;
    stBI.ulFlags = 0;
	stBI.lpfn = (*_szPath) ? fn_CBBrowse : NULL;
	stBI.lParam = (LPARAM) _szPath;
	
	if ((p_stItem = SHBrowseForFolder( &stBI )) == NULL)
		return FALSE;

    SHGetPathFromIDList( p_stItem, _szPath );
    while ((*_szPath) != 0) _szPath++;
    *_szPath = 0x5c;
    *(_szPath + 1) = 0;

	return TRUE;
}
