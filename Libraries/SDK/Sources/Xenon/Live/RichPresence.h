//////////////////////////////////////////////////////////////////////////
// Xenon Rich Presence
// ------------------------
// Updates player's Rich Presence state on XBox Live!
// Changes occur when entering new map and when reaching new check point
//////////////////////////////////////////////////////////////////////////

#ifndef __RICH_PRESENCE_H__
#define __RICH_PRESENCE_H__

#if defined(_XENON) || defined(ACTIVE_EDITORS) || defined(WIN32)

//////////////////////////////////////////////////////////////////////////
// Rich Presence functions

void	SetMap( int nMap );
void	UpdateRichPresence( );

#endif // defined(_XENON) || defined(ACTIVE_EDITORS)
#endif // __RICH_PRESENCE_H__
