////////////////////////////////////////////////////////////////////
//
// ../Sources/Xenon/XLAST/KingKong.spa.h
//
// Auto-generated on Friday, 10 March 2006 at 17:24:24
// XLAST project version 1.0.228.0
// SPA Compiler version 2.0.2571.0
//
////////////////////////////////////////////////////////////////////

#ifndef __PETER_JACKSON_S_KING_KONG_SPA_H__
#define __PETER_JACKSON_S_KING_KONG_SPA_H__

#ifdef __cplusplus
extern "C" {
#endif

//
// Title info
//

#define TITLEID_PETER_JACKSON_S_KING_KONG           0x555307D3

//
// Context ids
//
// These values are passed as the dwContextId to XUserSetContext.
//

#define CONTEXT_MAP_NAME                            3

//
// Context values
//
// These values are passed as the dwContextValue to XUserSetContext.
//

// Values for CONTEXT_MAP_NAME

#define CONTEXT_MAP_NAME_NO_MAP                     0
#define CONTEXT_MAP_NAME_01A                        1
#define CONTEXT_MAP_NAME_02A                        2
#define CONTEXT_MAP_NAME_02C1                       3
#define CONTEXT_MAP_NAME_02C2                       4
#define CONTEXT_MAP_NAME_03A                        5
#define CONTEXT_MAP_NAME_03B                        6
#define CONTEXT_MAP_NAME_03C                        7
#define CONTEXT_MAP_NAME_03E                        8
#define CONTEXT_MAP_NAME_03F                        9
#define CONTEXT_MAP_NAME_03D                        10
#define CONTEXT_MAP_NAME_04A                        11
#define CONTEXT_MAP_NAME_04A1                       12
#define CONTEXT_MAP_NAME_04A2                       13
#define CONTEXT_MAP_NAME_04B                        14
#define CONTEXT_MAP_NAME_05A1                       15
#define CONTEXT_MAP_NAME_05A2                       16
#define CONTEXT_MAP_NAME_05C                        17
#define CONTEXT_MAP_NAME_06A                        18
#define CONTEXT_MAP_NAME_07B                        19
#define CONTEXT_MAP_NAME_07D                        20
#define CONTEXT_MAP_NAME_09B                        21
#define CONTEXT_MAP_NAME_10B                        22
#define CONTEXT_MAP_NAME_11A                        23
#define CONTEXT_MAP_NAME_12A                        24
#define CONTEXT_MAP_NAME_07B2                       25
#define CONTEXT_MAP_NAME_14A                        26
#define CONTEXT_MAP_NAME_14B                        27
#define CONTEXT_MAP_NAME_15A                        28
#define CONTEXT_MAP_NAME_17A                        29
#define CONTEXT_MAP_NAME_17A2                       30
#define CONTEXT_MAP_NAME_17B                        31
#define CONTEXT_MAP_NAME_18A                        32
#define CONTEXT_MAP_NAME_03CR                       33
#define CONTEXT_MAP_NAME_03BR                       34
#define CONTEXT_MAP_NAME_03AR                       35
#define CONTEXT_MAP_NAME_19A                        36
#define CONTEXT_MAP_NAME_19B                        37
#define CONTEXT_MAP_NAME_20A                        38
#define CONTEXT_MAP_NAME_20B                        39
#define CONTEXT_MAP_NAME_20C                        40
#define CONTEXT_MAP_NAME_20D                        41
#define CONTEXT_MAP_NAME_01B                        42
#define CONTEXT_MAP_NAME_12B                        43
#define CONTEXT_MAP_NAME_21S                        44

// Values for X_CONTEXT_PRESENCE

#define CONTEXT_PRESENCE_CURRENT_MAP                0

// Values for X_CONTEXT_GAME_MODE

#define CONTEXT_GAME_MODE_CAMPAIGN_MODE             0

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

#define STATS_VIEW_SKILL_RANKED_CAMPAIGN_MODE       0xFFFF0000

// Skill leaderboards for unranked (standard) game modes

#define STATS_VIEW_SKILL_STANDARD_CAMPAIGN_MODE     0xFFFE0000

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

#define SPASTRING_CURRENT_MAP                       5
#define SPASTRING_CAMPAIGN_MODE                     26
#define SPASTRING_DEVELOPER                         27
#define SPASTRING_PUBLISHER                         28
#define SPASTRING_SHORT_DESCRIPTION                 29
#define SPASTRING_SELL_TEXT                         30
#define SPASTRING_GENRE                             31
#define SPASTRING_CURRENT_MAP_NAME                  32
#define SPASTRING_NO_MAP                            34
#define SPASTRING_01A                               44
#define SPASTRING_02A                               45
#define SPASTRING_02C1                              46
#define SPASTRING_02C2                              47
#define SPASTRING_03A                               48
#define SPASTRING_03B                               49
#define SPASTRING_03C                               50
#define SPASTRING_03E                               51
#define SPASTRING_03F                               52
#define SPASTRING_03D                               53
#define SPASTRING_04A                               54
#define SPASTRING_04A1                              55
#define SPASTRING_04A2                              56
#define SPASTRING_04B                               57
#define SPASTRING_05A1                              58
#define SPASTRING_05A2                              59
#define SPASTRING_05C                               60
#define SPASTRING_07B                               62
#define SPASTRING_07D                               63
#define SPASTRING_09B                               64
#define SPASTRING_10B                               65
#define SPASTRING_11A                               66
#define SPASTRING_12A                               67
#define SPASTRING_07B2                              68
#define SPASTRING_14A                               69
#define SPASTRING_14B                               70
#define SPASTRING_15A                               71
#define SPASTRING_17A                               72
#define SPASTRING_17A2                              73
#define SPASTRING_17B                               74
#define SPASTRING_18A                               75
#define SPASTRING_03CR                              76
#define SPASTRING_03BR                              77
#define SPASTRING_03AR                              78
#define SPASTRING_19A                               79
#define SPASTRING_19B                               80
#define SPASTRING_20A                               81
#define SPASTRING_20B                               82
#define SPASTRING_20C                               83
#define SPASTRING_20D                               84
#define SPASTRING_ACH_END_GAME_NAME                 85
#define SPASTRING_ACH_END_GAME_DESC                 86
#define SPASTRING_ACH_PYROMANIAC_NAME               90
#define SPASTRING_ACH_PYROMANIAC_DESC               91
#define SPASTRING_ACH_KING_OF_JUNGLE_NAME           92
#define SPASTRING_ACH_KING_OF_JUNGLE_DESC           93
#define SPASTRING_ACH_TREX_ANNIHILATOR_NAME         94
#define SPASTRING_ACH_TREX_ANNIHILATOR_DESC         95
#define SPASTRING_ACH_PROTECTOR_NAME                96
#define SPASTRING_ACH_PROTECTOR_DESC                97
#define SPASTRING_ACH_KING_OF_TREX_NAME             98
#define SPASTRING_ACH_KING_OF_TREX_DESC             99
#define SPASTRING_ACH_FOODCHAIN_MASTER_NAME         100
#define SPASTRING_ACH_FOODCHAIN_MASTER_DESC         101
#define SPASTRING_ACH_KING_OF_SKULL_ISLAND_NAME     102
#define SPASTRING_ACH_KING_OF_SKULL_ISLAND_DESC     103
#define SPASTRING_ACH_KING_OF_DEMOLITION_NAME       104
#define SPASTRING_ACH_KING_OF_DEMOLITION_DESC       105
#define SPASTRING_01B                               114
#define SPASTRING_12B                               115
#define SPASTRING_06A                               117
#define SPASTRING_21S                               118


#ifdef __cplusplus
}
#endif

#endif // __PETER_JACKSON_S_KING_KONG_SPA_H__


