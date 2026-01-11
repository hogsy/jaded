/*$T DYN.h GC!1.40 07/31/99 12:01:42 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    The REC module handles the recalage of an object when a collision has occured
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*
 ===================================================================================================
    Initialisation and close of the module
 ===================================================================================================
 */
extern void REC_InitModule(void);
extern void REC_CloseModule(void);


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
