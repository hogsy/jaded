////////////////////////////////////////////////////////////////////
//
// kingkong.spa.h
//
// Auto-generated on Friday, 03 March 2006 at 16:27:17
// XLAST project version 1.0.11.0
// SPA Compiler version 2.0.2571.0
//
////////////////////////////////////////////////////////////////////

#ifndef __KINGKONG_SPA_H__
#define __KINGKONG_SPA_H__

#ifdef __cplusplus
extern "C" {
#endif

//
// Title info
//

#define TITLEID_KINGKONG                            0x11111111

//
// Context ids
//
// These values are passed as the dwContextId to XUserSetContext.
//


//
// Context values
//
// These values are passed as the dwContextValue to XUserSetContext.
//

// Values for X_CONTEXT_PRESENCE


// Values for X_CONTEXT_GAME_MODE

#define CONTEXT_GAME_MODE_CACA                      0

//
// Property ids
//
// These values are passed as the dwPropertyId value to XUserSetProperty
// and as the dwPropertyId value in the XUSER_PROPERTY structure.
//


//
// Achievement ids
//
// These values are used in the dwAchievementId member of the
// XUSER_ACHIEVEMENT structure that is used with
// XUserWriteAchievements and XUserCreateAchievementEnumerator.
//


//
// Stats view ids
//
// These are used in the dwViewId member of the XUSER_STATS_SPEC structure
// passed to the XUserReadStats* and XUserCreateStatsEnumerator* functions.
//

// Skill leaderboards for ranked game modes

#define STATS_VIEW_SKILL_RANKED_CACA                0xFFFF0000

// Skill leaderboards for unranked (standard) game modes

#define STATS_VIEW_SKILL_STANDARD_CACA              0xFFFE0000

// Title defined leaderboards


//
// Stats view column ids
//
// These ids are used to read columns of stats views.  They are specified in
// the rgwColumnIds array of the XUSER_STATS_SPEC structure.  Rank, rating
// and gamertag are not retrieved as custom columns and so are not included
// in the following definitions.  They can be retrieved from each row's
// header (e.g., pStatsResults->pViews[x].pRows[y].dwRank, etc.).
//

//
// Matchmaking queries
//
// These values are passed as the dwProcedureIndex parameter to
// XSessionSearch to indicate which matchmaking query to run.
//


//
// Gamer pictures
//
// These ids are passed as the dwPictureId parameter to XUserAwardGamerTile.
//


//
// Strings
//
// These ids are passed as the dwStringId parameter to XReadStringsFromSpaFile.
//

#define SPASTRING_MESFESSE                          2
#define SPASTRING_CUCU                              3
#define SPASTRING_LOLO                              4
#define SPASTRING_BOUGNIA                           5


#ifdef __cplusplus
}
#endif

#endif // __KINGKONG_SPA_H__


