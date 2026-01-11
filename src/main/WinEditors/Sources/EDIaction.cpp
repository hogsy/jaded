/*$T EDIaction.cpp GC!1.41 08/04/99 12:25:40 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/ERRors/ERRasser.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "EDIaction.h"
#include "EDImainframe.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGread.h"


/*$4
 ***************************************************************************************************
    Global
 ***************************************************************************************************
 */

/*
 ---------------------------------------------------------------------------------------------------
    A key definition
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  EDI_keydef_
{
    char    *mpsz_Name;
    char    mc_Code;
} EDI_keydef;

/* All the known keys */
static EDI_keydef   sgast_KeyDefTable[] =
{
    { "BACK", (char) 0x08 },
    { "TAB", (char) 0x09 },
    { "RETURN", (char) 0x0D },
    { "SHIFT", (char) 0x10 },
    { "CONTROL", (char) 0x11 },
    { "ALT", (char) 0x12 },
    { "PAUSE", (char) 0x13 },
	{ "ALTGR", (char) 0xA5 },
    { "ESCAPE", (char) 0x1B },
    { "CAPS_LOCK", (char) 0x14 },
    { "VERR_NUM", (char) 0x90 },
    { "SPACE", (char) 0x20 },
    { "INS", (char) 0x2D },
    { "DEL", (char) 0x2E },
    { "END", (char) 0x23 },
    { "HOME", (char) 0x24 },
    { "PAGEUP", (char) 0x21 },
    { "PAGEDOWN", (char) 0x22 },
    { "LEFT", (char) 0x25 },
    { "UP", (char) 0x26 },
    { "RIGHT", (char) 0x27 },
    { "DOWN", (char) 0x28 },
    { "NUMPAD0", (char) 0x60 },
    { "NUMPAD1", (char) 0x61 },
    { "NUMPAD2", (char) 0x62 },
    { "NUMPAD3", (char) 0x63 },
    { "NUMPAD4", (char) 0x64 },
    { "NUMPAD5", (char) 0x65 },
    { "NUMPAD6", (char) 0x66 },
    { "NUMPAD7", (char) 0x67 },
    { "NUMPAD8", (char) 0x68 },
    { "NUMPAD9", (char) 0x69 },
    { "NUMPAD*", (char) 0x6A },
    { "NUMPAD+", (char) 0x6B },
    { "NUMPAD-", (char) 0x6D },
    { "NUMPAD.", (char) 0x6E },
    { "NUMPAD/", (char) 0x6F },
    { "F1", (char) 0x70 },
    { "F2", (char) 0x71 },
    { "F3", (char) 0x72 },
    { "F4", (char) 0x73 },
    { "F5", (char) 0x74 },
    { "F6", (char) 0x75 },
    { "F7", (char) 0x76 },
    { "F8", (char) 0x77 },
    { "F9", (char) 0x78 },
    { "F10", (char) 0x79 },
    { "F11", (char) 0x7A },
    { "F12", (char) 0x7B },
    { "²", (char) 0xDE },
    { "°", (char) 0xDB },
    { "+", (char) 0xBB },
    { "^", (char) 0xDD },
    { "$", (char) 0xBA },
    { "%", (char) 0xC0 },
    { "*", (char) 0xDC },
    { "<", (char) 0xE2 },
    { "?", (char) 0xBC },
    { ".", (char) 0xBE },
    { "/", (char) 0xBF },
    { "!", (char) 0xDF },
};

/*$4
 ***************************************************************************************************
    Functions
 ***************************************************************************************************
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
EDI_cl_ActionList::EDI_cl_ActionList(void)
{
    mpo_Editor = NULL;
    mi_CurrentList = 0;

    /* Add action list to mainframe list */
    M_MF()->mo_ActionList.AddTail(this);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
EDI_cl_ActionList::~EDI_cl_ActionList(void)
{
    /* Delete action list from mainframe list */
    M_MF()->mo_ActionList.RemoveAt(M_MF()->mo_ActionList.Find(this));

    /* Delete content */
    DeleteAll();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_ActionList::ReadFile(char *_psz_Name)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_Index;
    char        *pc_Buf;
    CString     o_String;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Be sure that current lists are empty */
    mo_Name = _psz_Name;
    o_String = mo_Name + EDI_Csz_ExtEdiAction;

    ul_Index = BIG_ul_SearchFileExt(EDI_Csz_Ini_Keyboard, (char *) (LPCSTR) o_String);
    if(ul_Index == BIG_C_InvalidIndex) return;

_Try_
    pc_Buf = BIG_pc_ReadFileTmp(BIG_PosFile(ul_Index), NULL);
    ParseBuffer(pc_Buf);

_Catch_
_End_
}

#define isspace(a)  (a == ' ')

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_ActionList::ParseBuffer(char *_psz_Buffer)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char                *pc_Buf, *p;
    EDI_cl_Action       *po_Action;
    EDI_cl_ConfigList   *po_List;
    CString             mo_Name;
    char                asz_Temp[512];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pc_Buf = _psz_Buffer;

    if(!_psz_Buffer) return;

    po_List = new EDI_cl_ConfigList;
    mo_List.AddTail(po_List);

    while(1)
    {
        /* Zap spaces */
        while(isspace(*pc_Buf)) pc_Buf++;

        if(*pc_Buf == ';') break;

        /* Detect a new action */
        po_Action = new EDI_cl_Action;
        po_Action->mb_Dyn = FALSE;
        po_List->mo_List.AddTail(po_Action);

        /* Action name */
        p = pc_Buf;
        while(*p != '=') p++;

        L_memcpy(asz_Temp, pc_Buf, p - pc_Buf);
        asz_Temp[p - pc_Buf] = 0;
        po_Action->mo_DisplayName = asz_Temp;
        pc_Buf += p - pc_Buf + 1;
        while(isspace(*pc_Buf)) pc_Buf++;

        /* Action number */
        p = pc_Buf;
        while(*p != '=') p++;

        L_memcpy(asz_Temp, pc_Buf, p - pc_Buf);
        asz_Temp[p - pc_Buf] = 0;
        po_Action->mul_Action = L_atol(asz_Temp);
        pc_Buf += p - pc_Buf + 1;
        while(isspace(*pc_Buf)) pc_Buf++;

        /* Key */
        p = pc_Buf;
        while(*p != '=') p++;

        L_memcpy(asz_Temp, pc_Buf, p - pc_Buf);
        asz_Temp[p - pc_Buf] = 0;
        po_Action->mo_Key = asz_Temp;
        po_Action->muw_Key = uw_StringToKey(asz_Temp);
        pc_Buf += p - pc_Buf + 1;
        while(isspace(*pc_Buf)) pc_Buf++;

        /* Resource name */
        p = pc_Buf;
        while(*p != ';') p++;

        L_memcpy(asz_Temp, pc_Buf, p - pc_Buf);
        asz_Temp[p - pc_Buf] = 0;
        po_Action->mui_Resource = 0;
        pc_Buf += p - pc_Buf + 1;
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
USHORT EDI_cl_ActionList::uw_StringToKey(char *_psz_Name)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    USHORT  uw_Res;
    int     i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Zap spaces */
    uw_Res = 0;

    while(L_isspace(*_psz_Name)) _psz_Name++;

    /* An invalid key (--) */
    if(_psz_Name[0] == '-' && _psz_Name[1] == '-') return 0;

    /* Modifiers */
    if(L_strlen(_psz_Name) > 1)
    {
        while(1)
        {
            switch(*_psz_Name)
            {
            case '^':
                uw_Res |= CONTROL;
                _psz_Name++;
                break;

            case '#':
                uw_Res |= SHIFT;
                _psz_Name++;
                break;

            case '@':
                uw_Res |= ALT;
                _psz_Name++;
                break;

            case '~':
                _psz_Name++;
                break;

            default:
                goto breakbreak;
            }
        }
    }

    /* Search if it's a special key */
breakbreak:
    if(*_psz_Name)
    {
        for(i = 0; i < sizeof(sgast_KeyDefTable) / sizeof(EDI_keydef); i++)
        {
            if(!L_strcmpi(sgast_KeyDefTable[i].mpsz_Name, _psz_Name))
            {
                return uw_Res | sgast_KeyDefTable[i].mc_Code;
            }
        }
    }

    /* Normal ASCII char */
    return uw_Res | L_toupper(*_psz_Name);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_ActionList::SaveUser(ULONG _ul_Action, USHORT _uw_Key, CString &_o_Key)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION            pos;
    EDI_cl_ConfigList   *po_Config;
    EDI_cl_Action       *po_Action;
    char                asz_Temp[100];
    CString             o_String;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    o_String = mo_Name + EDI_Csz_ExtEdiAction;
    SAV_Begin(EDI_Csz_Ini_Keyboard, (char *) (LPCSTR) o_String);

    /*
     * Save the file. We get first list of config (that is the default list that can't be
     * changed), except if there's already a custom one.
     */
    if(mo_List.GetCount() == 1)
        po_Config = mo_List.GetAt(mo_List.GetHeadPosition());
    else
        po_Config = mo_List.GetAt(mo_List.FindIndex(1));

    pos = po_Config->mo_List.GetHeadPosition();
    while(pos)
    {
        po_Action = po_Config->mo_List.GetNext(pos);

        /* Not saving a dynamic action */
        if(po_Action->mb_Dyn) continue;

        /* Save action string */
        SAV_Buffer
        (
            (char *) (LPCSTR) po_Action->mo_DisplayName,
            po_Action->mo_DisplayName.GetLength()
        );
        SAV_Buffer("=", 1);

        /* Save action */
        sprintf(asz_Temp, "%d", po_Action->mul_Action);
        SAV_Buffer(asz_Temp, L_strlen(asz_Temp));
        SAV_Buffer("=", 1);

        /* Save key */
        if(po_Action->mul_Action == _ul_Action)
            SAV_Buffer((char *) (LPCSTR) _o_Key, _o_Key.GetLength());
        else
            SAV_Buffer((char *) (LPCSTR) po_Action->mo_Key, po_Action->mo_Key.GetLength());

        /* Save resource */
        SAV_Buffer("=", 1);
        sprintf(asz_Temp, "%d", po_Action->mui_Resource);
        SAV_Buffer(asz_Temp, L_strlen(asz_Temp));

        /* End mark */
        SAV_Buffer(";", 1);
    }

    SAV_Buffer(";", 1);

    /* No key creation for ini files */
    SAV_ul_End();

    /* Load the list if there's only one config list for now. */
    if(mo_List.GetCount() == 1)
    {
        ReadFile((char *) (LPCSTR) mo_Name);
    }

    /* Change key in custom action list (in case the file has not been loaded just before). */
    po_Config = mo_List.GetAt(mo_List.FindIndex(1));
    pos = po_Config->mo_List.GetHeadPosition();
    while(pos)
    {
        po_Action = po_Config->mo_List.GetNext(pos);
        if(po_Action->mul_Action == _ul_Action)
        {
            po_Action->muw_Key = _uw_Key;
            po_Action->mo_Key = _o_Key;
            break;
        }
    }

    /* Force current list to be custom one */
    M_MF()->ForceActionConfig(1);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_ActionList::DeleteAll(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION            pos, pos1;
    EDI_cl_ConfigList   *po_Config;
    EDI_cl_Action       *po_Action;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pos = mo_List.GetHeadPosition();
    while(pos)
    {
        po_Config = mo_List.GetNext(pos);
        pos1 = po_Config->mo_List.GetHeadPosition();
        while(pos1)
        {
            po_Action = po_Config->mo_List.GetNext(pos1);
            delete po_Action;
        }

        po_Config->mo_List.RemoveAll();
        delete po_Config;
    }

    mo_List.RemoveAll();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
static char *sfnpsz_SearchKey(char _c_Key, char &_c_Code)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(i = 0; i < sizeof(sgast_KeyDefTable) / sizeof(EDI_keydef); i++)
    {
        if(_c_Key == sgast_KeyDefTable[i].mc_Code)
        {
            _c_Code = sgast_KeyDefTable[i].mc_Code;
            return sgast_KeyDefTable[i].mpsz_Name;
        }
    }

    return NULL;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
CString EDI_cl_ActionList::o_KeyToString(char _c_Key, USHORT &_uw_Key)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CString o_String;
    char    *psz_Res;
    char    c_Code;
	char	astr[2];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    _uw_Key = GetAsyncKeyState(VK_SHIFT) & 0x8000 ? SHIFT : 0;
    _uw_Key |= GetAsyncKeyState(VK_CONTROL) & 0x8000 ? CONTROL : 0;
    _uw_Key |= GetAsyncKeyState(VK_MENU) & 0x8000 ? ALT : 0;

    /* Special keys */
    o_String = "";
    if((_uw_Key & CONTROL) && (_c_Key != VK_CONTROL)) o_String += "^";
    if((_uw_Key & ALT) && (_c_Key != VK_MENU)) o_String += "@";
    if((_uw_Key & SHIFT) && (_c_Key != VK_SHIFT)) o_String += "#";

    /* Get associated key */
    psz_Res = sfnpsz_SearchKey(_c_Key, c_Code);

    /* That's the end for a single modifier */
    if
    (
        ((_uw_Key == SHIFT) && (_c_Key == VK_SHIFT)) ||
        ((_uw_Key == CONTROL) && (_c_Key == VK_CONTROL)) ||
        ((_uw_Key == ALT) && (_c_Key == VK_MENU))
    )
    {
        _uw_Key = c_Code;
        return psz_Res;
    }

    /* Else add the key itself */
    if(psz_Res)
    {
        o_String += psz_Res;
        _uw_Key |= c_Code;
    }
    else
    {
		astr[1] = 0;
		astr[0] = _c_Key;
        o_String += astr;
        _uw_Key |= L_toupper(_c_Key);
    }

    return o_String;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
ULONG EDI_cl_ActionList::ul_KeyToAction(USHORT _uw_Key)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION            pos;
    EDI_cl_Action       *po_Action;
    EDI_cl_ConfigList   *po_List;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(mo_List.GetCount() == 0) return 0;

    /* Search key in current list */
    po_List = mo_List.GetAt(mo_List.FindIndex(mi_CurrentList));
    pos = po_List->mo_List.GetHeadPosition();
    while(pos)
    {
        po_Action = po_List->mo_List.GetNext(pos);
        if(po_Action->muw_Key == _uw_Key)
            return po_Action->mul_Action;
    }

    return 0;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EDI_cl_ActionList::b_ActionKnowsKey(USHORT _uw_Key)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION            pos;
    EDI_cl_ConfigList   *po_List;
    EDI_cl_Action       *po_Action;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pos = mo_List.FindIndex(mi_CurrentList);
    if(!pos) return FALSE;

    po_List = mo_List.GetAt(pos);
    pos = po_List->mo_List.GetHeadPosition();
    while(pos)
    {
        po_Action = po_List->mo_List.GetNext(pos);
        if(po_Action->muw_Key == _uw_Key) return TRUE;
    }

    return FALSE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
EDI_cl_Action *EDI_cl_ActionList::po_GetActionById(ULONG _ul_Action)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    POSITION            pos;
    EDI_cl_ConfigList   *po_List;
    EDI_cl_Action       *po_Action;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pos = mo_List.FindIndex(mi_CurrentList);
    if(!pos) return FALSE;

    po_List = mo_List.GetAt(pos);
    pos = po_List->mo_List.GetHeadPosition();
    while(pos)
    {
        po_Action = po_List->mo_List.GetNext(pos);
        if(po_Action->mul_Action == _ul_Action) return po_Action;
    }

    return NULL;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
USHORT EDI_cl_ActionList::uw_GetKeyOfAction(ULONG _ul_Action)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDI_cl_Action   *po_Act;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    po_Act = po_GetActionById(_ul_Action);
    if(!po_Act) return FALSE;
    return po_Act->muw_Key;
}

#endif
