
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                            +
+ Library : Main Editor                                                      +
+ File    : MAIN_DesdacEditorTags.h                                          +
+                                                                            +
+ Description   :  This file contains identifiable log tags that can be      +
+                  searched in log for auto tests purpose (cf. Desdac)       +
+                  The same file must used on each side (editor/desdac)      +
+                                                                            +
+ Engine/Editor :  Editor                                                    +
+                                                                            +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


#ifndef __MAIN_DESDACEDITORTAGS_H__
#define __MAIN_DESDACEDITORTAGS_H__


//-------------------------------------

// AI CHECK - COMPILATION 
#define MAIN_kaz_DESDACTAG_AICOMPILOK               "[AI compilation success]"
#define MAIN_kaz_DESDACTAG_AICOMPILFAILED           "[AI compilation failure]"


// AI CHECK - RUN 
#define MAIN_kaz_DESDACTAG_AIERROR                  "[AI ERROR]"


// Database Check
#define MAIN_kaz_DESDACTAG_DBOK                     "Check and repair on database done"
#define MAIN_kaz_DESDACTAG_DBFAILED                 "Check and repair on database found errors"


// MAP TESTING PC
#define MAIN_kaz_DESDACTAG_MAPTESTING_BEGIN         "====== BEGIN OF LAUNCHMAP ======"
#define MAIN_kaz_DESDACNAMEDTAG_LOADMAP_START       "MapTesting start on %s"
#define MAIN_kaz_DESDACNAMEDTAG_LOADMAP_OK          "Map %s loaded successfully"
#define MAIN_kaz_DESDACNAMEDTAG_LAUNCHMAP_OK        "Map %s executed successfully"
#define MAIN_kaz_DESDACNAMEDTAG_LAUNCHMAP_FAILED    "Map %s execution failed"
#define MAIN_kaz_DESDACNAMEDTAG_CLOSEMAP_OK         "Map %s closed successfully"

//-------------------------------------


#endif /* __MAIN_DESDACEDITORTAGS_H__ */
