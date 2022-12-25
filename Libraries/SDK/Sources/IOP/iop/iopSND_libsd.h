/*$T iopSND_libsd.h GC! 1.097 09/14/01 08:24:54 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_IOP
#ifndef __iopSND_libsd_h__
#define __iopSND_libsd_h__

#ifdef _DEBUG

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    debug version
 -----------------------------------------------------------------------------------------------------------------------
 */

#define L_sceSdInit(_flag)					{ int code; code = sceSdInit((_flag)); iopDbg_M_Assert((code == 0), iopDbg_Err_0062 " : sceSdInit failed"); }

#define L_sceSdSetParam(_entry, _value)		sceSdSetParam((_entry), (_value))
#define L_sceSdGetParam(_entry)				sceSdGetParam((_entry))
#define L_sceSdSetSwitch(_entry, _value)	sceSdSetSwitch((_entry), (_value))
#define L_sceSdGetSwitch(_entry)			sceSdGetSwitch((_entry))
#define L_sceSdSetAddr(_entry, _value)		sceSdSetAddr((_entry), (_value))
#define L_sceSdGetAddr(_entry)				sceSdGetAddr((_entry))
#define L_sceSdSetCoreAttr(_entry, _value)	sceSdSetCoreAttr((_entry), (_value))
#define L_sceSdGetCoreAttr(_entry)			sceSdGetCoreAttr((_entry))
inline int L_sceSdVoiceTrans(int _channel, u_short _mode, u_char*_m_addr, u_int _s_addr, u_int _size) 
{ 
	int code; 
	code = sceSdVoiceTrans((_channel), (_mode), (_m_addr), (_s_addr), (_size)); 
	iopDbg_M_AssertX((code > 0), iopDbg_Err_0062 " : sceSdVoiceTrans failed dma %d, mode 0x%X, maddr 0x%X, saddr 0x%X, size %d" , (int)(_channel), (int)(_mode), (int)(_m_addr), (int)(_s_addr), (int)(_size));
	return code;
}

inline int L_sceSdVoiceTransStatus(short _channel, short _flag) 
{ 
	int code; 
	code = sceSdVoiceTransStatus(_channel, _flag); 
	iopDbg_M_Assert(((code == 0) || (code == 1)), iopDbg_Err_0062 " : sceSdVoiceTransStatus failed"); 
	return code;
}

#define L_sceSdSetEffectAttr(_core, _attr)			sceSdSetEffectAttr((_core), (_attr))
#define L_sceSdSetTransCallback(_channel, _func)	sceSdSetTransCallback((_channel), (_func))
#define L_sceSdSetTransIntrHandler(_channel, _func, _arg)	sceSdSetTransIntrHandler((_channel), (_func), (_arg))
#else

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    release version
 -----------------------------------------------------------------------------------------------------------------------
 */

#define L_sceSdInit(_flag)					sceSdInit((_flag))
#define L_sceSdSetParam(_entry, _value)		sceSdSetParam((_entry), (_value))
#define L_sceSdGetParam(_entry)				sceSdGetParam((_entry))
#define L_sceSdSetSwitch(_entry, _value)	sceSdSetSwitch((_entry), (_value))
#define L_sceSdGetSwitch(_entry)			sceSdGetSwitch((_entry))
#define L_sceSdSetAddr(_entry, _value)		sceSdSetAddr((_entry), (_value))
#define L_sceSdGetAddr(_entry)				sceSdGetAddr((_entry))
#define L_sceSdSetCoreAttr(_entry, _value)	sceSdSetCoreAttr((_entry), (_value))
#define L_sceSdGetCoreAttr(_entry)			sceSdGetCoreAttr((_entry))
#define L_sceSdVoiceTrans(_channel, _mode, _m_addr, _s_addr, _size) \
	sceSdVoiceTrans \
	( \
		(_channel), \
		(_mode), \
		(_m_addr), \
		(_s_addr), \
		(_size) \
	)
#define L_sceSdVoiceTransStatus(_channel, _flag)	sceSdVoiceTransStatus((_channel), (_flag))
#define L_sceSdSetEffectAttr(_core, _attr)			sceSdSetEffectAttr((_core), (_attr))
#define L_sceSdSetTransCallback(_channel, _func)	sceSdSetTransCallback((_channel), (_func))
#define L_sceSdSetTransIntrHandler(_channel, _func, _arg)	sceSdSetTransIntrHandler((_channel), (_func), (_arg))
#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* __iopSND_libsd_h__ */
#endif /* PSX2_IOP */
