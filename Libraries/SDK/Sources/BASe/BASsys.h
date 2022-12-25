/*$T BASsys.h GC!1.26 04/21/99 11:52:18 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef __BASSYS_H__
#define __BASSYS_H__



#if !defined(PSX2_TARGET) && !defined(_GAMECUBE)
#pragma         once
#define         _inline_    __inline
#define	        _fastcall_  __fastcall
#define         _const_     const
#define         _volatile_  volatile

#else       /* if not PSX2_TARGET */
#ifdef          __GNUC__ 
#define             _inline_    __inline__
#define             _asm_       __asm__
#define             _const_     __const__
#define             _volatile_  __volatile__

#else           /* if not GNU Compiler */
#define             _inline_    inline
#define             _asm_       asm
#define             _const_     const
#define             _volatile_  volatile

#endif          /* GNU compiler */
#endif      /* PSX2_TARGET */
#endif /* __BASSYS_H__ */