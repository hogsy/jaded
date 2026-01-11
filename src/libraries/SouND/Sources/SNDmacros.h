/*$T SNDmacros.h GC! 1.097 04/09/02 10:53:29 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SNDmacros_h__
#define __SNDmacros_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
#define SND_EdiRedWarningMsg(_module, _msg)\
    {\
        char	  strtmp[1024];\
        sprintf(strtmp, "[SND-%s] %s", _module, _msg);\
        ERR_X_Warning(0, strtmp, NULL);\
    }
#define SND_EdiBlackWarningMsg(_module, _msg)\
    {\
        char	  strtmp[1024];\
        sprintf(strtmp, "[SND-%s] %s", _module, _msg);\
        LINK_PrintStatusMsg(strtmp); \
    }
#define SND_EdiRedWarningMsgKey(_module, _msg, _key) \
	{ \
		char	strtmp[1024]; \
		sprintf(strtmp, "[SND-%s] %s key [%08x]", _module, _msg, _key); \
		ERR_X_Warning(0, strtmp, NULL); \
	}

#define SND_EdiBlackWarningMsgKey(_module, _msg, _key)\
    {\
        char	  strtmp[1024]; \
        sprintf(strtmp, "[SND-%s] %s [%08x]", _module, _msg, _key); \
        LINK_PrintStatusMsg(strtmp); \
    }
#else
#define SND_EdiRedWarningMsg(_module, _msg)
#define SND_EdiBlackWarningMsg(_module, _msg)
#define SND_EdiRedWarningMsgKey(_module, _msg, _key)
#define SND_EdiBlackWarningMsgKey(_module, _msg, _key)
#endif

/*$4
 ***********************************************************************************************************************
    inline
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ float SND_f_SqrDist(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	/*~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	V;
	/*~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&V, A, B);
	return(MATH_f_SqrNormVector(&V));
}

_inline_ float SND_f_FloatModulo(float x, float fModulo)
{
    while(x > fModulo) x-=fModulo;
    return x;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SNDmacros_h__ */
