#ifndef __DEFINES_H__
#define __DEFINES_H__



#include "DefinesShared.h"


//////////////////////////////////////////////////////
// INI FILE
#define kaz_CERBERE_IniFileCommon   ".\\cerbere.ini"        // use relative path to avoid windows folder
#define kaz_CERBERE_IniFileUser     "cerbereUser.ini"       // default location is windows folder

// INI Sections
#define kaz_CERBERE_SettingsSection "Settings"


// INI Entries
#define kaz_CERBERE_NbPCWatched     "NbPCWatched"
#define kaz_CERBERE_PCWatched       "PCWatched%d"


// INI Default Values
#define kaz_CERBERE_Unknown         "<unknown>"



//////////////////////////////////////////////////////
// TEXTS
#define kaz_CERBERE_Name                        "Cerbere"

#define kaz_CERBERE_ActivityOK                  "active"
#define kaz_CERBERE_ActivityPCDown              "DOWN"
#define kaz_CERBERE_ActivityNotLaunched         "stopped"
#define kaz_CERBERE_ActivityFrozen              "FROZEN"
#define kaz_CERBERE_ActivitySuspended           "suspended"

#define kaz_CERBERE_StatusIddle                 "iddle"
#define kaz_CERBERE_StatusTesting               "Testing"
#define kaz_CERBERE_StatusTestingUpdatePending  "Testing (update pending)"
#define kaz_CERBERE_StatusUpdating              "updating"

#define kaz_CERBERE_GlobalStatusInitializing    "Initializing..."
#define kaz_CERBERE_GlobalStatusOK              "OK"
#define kaz_CERBERE_GlobalStatusNOTOK           "NOT OK"

// Tests
#define kaz_CERBERE_TEST_IADB                    "IA-DB"
#define kaz_CERBERE_TESTMAP_PC                   "Map PC"
#define kaz_CERBERE_TESTMAP_XENON                "Map Xenon"
#define kaz_CERBERE_TEST_CLIENT                  "(client)"

#define kaz_CERBERE_TESTRESULT_OK                "OK"
#define kaz_CERBERE_TESTRESULT_FAILED            "FAILED"


//////////////////////////////////////////////////////
// LIST COLUMNS 
#define kaz_COLUMNTITLE_PCname                  "PC Name"
#define kaz_COLUMNTITLE_TestType                "Test Type"
#define kaz_COLUMNTITLE_Status                  "Status"
#define kaz_COLUMNTITLE_Results                 "Results"
#define kaz_COLUMNTITLE_ProjectName             "Project"
#define kaz_COLUMNTITLE_EngineVersion           "Ver."

const int ki_COLUMNDEFAULTWIDTH_PCname          = 100;
const int ki_COLUMNDEFAULTWIDTH_TestType        = 70;
const int ki_COLUMNDEFAULTWIDTH_Status          = 70;
const int ki_COLUMNDEFAULTWIDTH_Results         = 50;
const int ki_COLUMNDEFAULTWIDTH_ProjectName     = 80;
const int ki_COLUMNDEFAULTWIDTH_EngineVersion   = 50;


#define kTab                                    "\t"


//////////////////////////////////////////////////////
// internal
const UINT kui_UpdateTimerID        = 1;    
const int  ki_UpdateTimerValue      = 500;    // milliseconds
const UINT kui_IconAnimTimerID       = 2;    
const int  ki_IconAnimTimerValue    = 200;    // milliseconds

const int ki_MaxNameLength          = 256;
const int ki_MaxCommandLength       = 1024;

const float kf_InactivityTimeThreshold  = (float)(10*ki_DESDAC_TimerValue/1000.0f);   // in Seconds

const COLORREF kRed   = 0x000000FF;
const COLORREF kGreen = 0x0000FF00;
const COLORREF kBlue  = 0x00FF0000;


#endif//__DEFINES_H__