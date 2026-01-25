/*$T BIGcheck.h GC!1.52 12/09/99 10:00:01 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once

#include "BASe/BAStypes.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGopen.h"

extern BOOL BIG_gb_CanClean;

void BIG_RestoreDeleted(void);
BOOL BIG_b_CheckFile(BOOL);
void BIG_Clean(BOOL);
