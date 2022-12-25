/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifdef ACTIVE_EDITORS

/* Define constantsfor action. */
#define EPFB_ACTION_REFRESHBROWSER			1
#define EPFB_ACTION_SAVEPREFAB				2
#define EPFB_ACTION_AUTOSAVE				3
#define EPFB_ACTION_MOVEPREFABTOPREFABMAP	4

#define EPFB_ACTION_DISPLAYPOINT			10
#define EPFB_ACTION_DISPLAYBV				11

#define EPFB_ACTION_CHECKOUT				20
#define EPFB_ACTION_LOCALCHECKOUT			21
#define EPFB_ACTION_UNDOCHECKOUT			22
#define EPFB_ACTION_GETLVERSION				23
#define EPFB_ACTION_CHECKIN					24
#define EPFB_ACTION_CHECKINNOOUT			25
#define EPFB_ACTION_CHECKINMIRROR			26



#define EPFB_ACTION_PERFORCE_EDIT			50
#define EPFB_ACTION_PERFORCE_REVERT			51
#define EPFB_ACTION_PERFORCE_SYNC			52
#define EPFB_ACTION_PERFORCE_FORCESYNC		53
#define EPFB_ACTION_PERFORCE_ADD			56
#define EPFB_ACTION_PERFORCE_DELETE			57
#define EPFB_ACTION_PERFORCE_DIFF			54
#define EPFB_ACTION_PERFORCE_HISTORY		55
#define EPFB_ACTION_PERFORCE_SUBMIT			58
#define EPFB_ACTION_PERFORCE_SUBMITEDIT		59


/* Define constants for strings . */
#define EPFB_STRING_OffsetTitle					0
#define EPFB_STRING_TypeTitle					1
#define EPFB_STRING_TypeAll						2
#define EPFB_STRING_TypeCycle					3
#define EPFB_STRING_TypeRandom					4
#define EPFB_STRING_TypeRandomUser				5
#define EPFB_STRING_MenuShowInBrowser			6
#define EPFB_STRING_TreeMenuRename				7
#define EPFB_STRING_TreeMenuRemove				8
#define EPFB_STRING_TreeMenuCreateDir			9
#define EPFB_STRING_TreeMenuCreateEmptyPrefab	10
#define EPFB_STRING_TreeMenuCreateRootDir		11
#define EPFB_STRING_RefMenuRemoveRef			12
#define EPFB_STRING_RefMenuOpenPrefab			13
#define EPFB_STRING_MenuCheckout				14
#define EPFB_STRING_MenuLocalCheckout			15
#define EPFB_STRING_MenuUndoCheckout			16
#define EPFB_STRING_MenuGetLVersion				17
#define EPFB_STRING_MenuCheckIn					18
#define EPFB_STRING_MenuCheckInOut				19
#define EPFB_STRING_MenuCheckInMirror			20

#define EPFB_STRING_P4MenuCheckout				21
#define EPFB_STRING_P4MenuUndoCheckout			22
#define EPFB_STRING_P4MenuGetLVersion			23
#define EPFB_STRING_P4MenuForceGetLVersion		24
#define EPFB_STRING_P4MenuAdd					25
#define EPFB_STRING_P4MenuDelete				26
#define EPFB_STRING_P4MenuDiff					27
#define EPFB_STRING_P4MenuHistory				28
#define EPFB_STRING_P4MenuSubmit				29
#define EPFB_STRING_P4MenuSubmitEdit			30


/* Base buffer. */
#ifdef ACTION_GLOBAL

#define EXTERN
#else

#define EXTERN  extern
#endif

EXTERN char * EPFB_asz_ActionBase

#ifdef ACTION_GLOBAL
=
    "\
File=-1=--=0;\
Refresh browser=1=F4=0;\
Save prefab=2=^S=0;\
Autosave=3==0;\
When creating move to prefabmap=4==0;\
Display=-2=--=0;\
Display point=10==0;\
Display BV=11==0;\
Data Control=-3=--=0;\
Check Out=20=^#O=222;\
Local Check Out=21=^#C=225;\
Undo Check Out=22=^#U=223;\
Get Latest Version=34=^#L=0;\
Check In=24=^#I=221;\
Check InOut=25=^#Q=0;\
Check In Mirror=26=--=0;\
Perforce - Check In=58==0;\
Perforce - Check Out=50==0;\
Perforce - Check InOut=59==0;\
Perforce - Undo Check Out=51==0;\
Perforce - Get Latest Version=52==0;\
Perforce - Force Get Latest Version=53==0;\
Perforce - Add to server=56==0;\
Perforce - Delete from server=57==0;\
Perforce - Show difference=54==0;\
Perforce - Show history=55==0;\
;\
"
#endif
;

EXTERN char * EPFB_asz_String[]

#ifdef ACTION_GLOBAL
=
{
	"( Offset )",
	"( Type ) ",
	"All",
	"Cycle",
	"Random", 
	"UserRandom",
	"Show in browser", 
	"Rename",
	"Remove",
	"Create Dir",
	"Create empty prefab",
	"Create Root Dir",
	"Remove Ref", 
	"Open prefab",
	"Check Out",
	"Local Check Out",
	"Undo Check Out",
	"Get Latest Version",
	"Check In",
	"Check InOut",
	"Check In Mirror",
	"Perforce - Check Out",
	"Perforce - Undo Check Out",
	"Perforce - Get Latest Version",
	"Perforce - Force Get Latest Version",
	"Perforce - Add to server",
	"Perforce - Delete from server",
	"Perforce - Show difference",
	"Perforce - Show history",
	"Perforce - Check In",
	"Perforce - Check InOut"
};

#endif

;

#undef EXTERN
#undef ACTION_GLOBAL
#endif

