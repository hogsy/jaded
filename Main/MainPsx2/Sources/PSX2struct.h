/*$T PSX2struct.h GC! 1.097 01/30/01 14:46:51 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* $off */
#ifndef __PSX2STRUCT_H__
#define __PSX2STRUCT_H__

#ifdef __PSX2MAIN_C__
#define EXTERN
#else
#define EXTERN	extern
#endif
#ifdef __cplusplus
extern "C"
{
#endif
//#define PSX2_Cte_DefaultIniFile		"atfile:10.5.120.51,x:/JadePsx2Atmon.ini"
#define PSX2_Cte_DefaultIniFile		"host0:x:/JadePsx2.ini\0 FREE SPACE FREE SPACE FREE SPACE FREE SPACE  "
#define PSX2_Cte_IniFileVersion		"1.01"
#define PSX2_Field_IniVersion		"IniVersion"
#define PSX2_Field_BigFile			"BigFile"
#define PSX2_Field_IconFile			"IconFile"
#define PSX2_Field_SpeFile			"SpeFile"
#define PSX2_Field_SysModulePath	"SysModulePath"
#define PSX2_Field_HomeModulePath	"HomeModulePath"
#define PSX2_Field_Bin				"Bin"
#define PSX2_Field_Xopt				"Xopt"
#define PSX2_Field_Sound			"Sound"
#define PSX2_Field_VideoMode		"VideoMode"



#ifdef __cplusplus
}
#endif
#endif /* __PSX2STRUCT_H__ */
/* $on */