/*$T INTconst.h GC!1.5 10/15/99 09:39:59 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __INT_CONST__
#define __INT_CONST__

#ifndef PSX2_TARGET
#	pragma once
#endif

#if defined( __cplusplus ) && !defined( JADEFUSION )
extern "C"
{
#endif

#define INT_Cul_InvalidObject 0xFFFF
#define INT_Cul_MaxObjects    4096// old : 1024
#define INT_Cul_MaxNodes      ( 2 * INT_Cul_MaxObjects )

#define INT_Cul_AxisX 0
#define INT_Cul_AxisY 1
#define INT_Cul_AxisZ 2

#define INT_Cul_RIGHT  0
#define INT_Cul_LEFT   1
#define INT_Cul_MIDDLE 2

#define INT_Cul_StartPoint 0x01
#define INT_Cul_EndPoint   0x02
#define INT_Cul_Sphere     0x08
#define INT_Cul_AABBox     0x10


#if defined( __cplusplus ) && !defined( JADEFUSION )
}
#endif
#endif
