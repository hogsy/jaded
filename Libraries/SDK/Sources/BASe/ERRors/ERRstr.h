/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifndef __ERRSTR_H__
#define __ERRSTR_H__

/* Common errors. */
#define ERR_Csz_ChMode                  "chmode error"
#define ERR_Csz_FOpen                   "Unable to open file"
#define ERR_Csz_FClose                  "fclose error"
#define ERR_Csz_FSeek                   "fseek error"
#define ERR_Csz_FWrite                  "Error while writing to file"
#define ERR_Csz_FRead                   "Error while reading file"
#define ERR_Csz_Unlink                  "Error while deleting file on disk"
#define ERR_Csz_Rename                  "Error renaming file"
#define ERR_Csz_MkDir                   "Error creating directory"
#define ERR_Csz_NotEnoughMemory         "Not enough memory"
#define ERR_Csz_BigFileCorrupt          "Big file corrupt"
#define ERR_Csz_BIGVSS_AlreadyCheckOut  "The file is already check out"
#define ERR_Csz_BIGVSS_NotCheckOut      "File is not check out"

/* Editors errors. */
#ifdef ACTIVE_EDITORS

#define ERR_Csz_IniCorrupt  "Ini file corrupt"
#endif

#endif /* __ERRSTR_H__ */