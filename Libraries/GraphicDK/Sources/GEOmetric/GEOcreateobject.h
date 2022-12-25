/*$T GEOcreateobject.h GC! 1.081 03/23/01 16:42:43 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef ACTIVE_EDITORS
#ifndef __GEOCREATEOBJECT_H__
#define __GEOCREATEOBJECT_H__

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Structure
 ***********************************************************************************************************************
 */

typedef struct GEO_tdst_CO_Text_
{
	int		i_NbChar;
	float	f_Width;
	float	f_Height;
	float	f_Espacement;
    float   f_Radius;
} GEO_tdst_CO_Text;

typedef struct	GEO_tdst_CO_Plane_
{
	float	x, y;
	int		i_NbX, i_NbY;
} GEO_tdst_CO_Plane;

typedef struct	GEO_tdst_CO_Box_
{
	float	x, y, z;
	int		i_NbX, i_NbY, i_NbZ, i_NbElem;
} GEO_tdst_CO_Box;

typedef struct	GEO_tdst_CO_Cylindre_
{
	float	f_Radius, z;
	int		i_NbZSegs, i_NbCapSegs, i_NbSides;
} GEO_tdst_CO_Cylindre;

typedef struct GEO_tdst_CO_Sphere_
{
    float   f_Radius;
    int     i_NbSeg;
} GEO_tdst_CO_Sphere;

typedef struct GEO_tdst_CO_Geosphere_
{
    float   f_Radius;
    int     i_NbSeg;
} GEO_tdst_CO_Geosphere;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

ULONG	GEO_ul_CO_Text(struct GEO_tdst_Object_ **, char *, char *, GEO_tdst_CO_Text *);
ULONG	GEO_ul_CO_Plane(struct GEO_tdst_Object_ **, char *, char *, GEO_tdst_CO_Plane *);
ULONG	GEO_ul_CO_Box(struct GEO_tdst_Object_ **, char *, char *, GEO_tdst_CO_Box *);
ULONG	GEO_ul_CO_Cylindre(struct GEO_tdst_Object_ **, char *, char *, GEO_tdst_CO_Cylindre *);
ULONG	GEO_ul_CO_Sphere(struct GEO_tdst_Object_ **, char *, char *, GEO_tdst_CO_Sphere *);
ULONG	GEO_ul_CO_Geosphere(struct GEO_tdst_Object_ **, char *, char *, GEO_tdst_CO_Geosphere *);

ULONG	COL_ul_CO_Plane(struct COL_tdst_Cob_ **, char *, char *, GEO_tdst_CO_Plane *);
ULONG	COL_ul_CO_Box(struct COL_tdst_Cob_ **, char *, char *, GEO_tdst_CO_Box *);
ULONG	COL_ul_CO_Cylindre(struct COL_tdst_Cob_ **, char *, char *, GEO_tdst_CO_Cylindre *);
ULONG	COL_ul_CO_Sphere(struct COL_tdst_Cob_ **, char *, char *, GEO_tdst_CO_Sphere *);
ULONG	COL_ul_CO_Geosphere(struct COL_tdst_Cob_ **, char *, char *, GEO_tdst_CO_Geosphere *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GEOCREATEOBJECT_H */

#endif /* ACTIVE_EDITORS */
