
#pragma once

#if defined( _MSC_VER ) && defined( _WIN32 )
#	define _inline_   __inline
#	define _fastcall_ __fastcall
#	define _const_    const
#	define _volatile_ volatile
#else
#	define _inline_   __inline__
#	define _asm_      __asm__
#	define _const_    __const__
#	define _volatile_ __volatile__
#	define _fastcall_
#endif
