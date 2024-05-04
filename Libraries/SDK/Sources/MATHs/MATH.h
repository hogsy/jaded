/*$T MATH.h GC!1.32 05/18/99 15:55:42 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Fichier � inclure si on veut utiliser le module maths
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Note: this include includes the standard C math.h file */

#ifndef Maths_h
#define Maths_h
#include "BASe/BAStypes.h"


/* Constantes Math�matiques */
#include "MATHs/MATHconst.h"

/* Structures */
#include "MATHs/MATHstruct.h"

/* Variables externes */
#include "MATHs/MATHvars.h"

/* Op�rations sur les floats */
#include "MATHs/MATHfloat.h"

/* Op�rations sur les vecteurs */
#include "MATHs/MATHvector.h"

/* Op�rations sur les matrices */
#include "MATHs/MATHmatrix.h"

/* Initialisation du module de Maths */
#include "MATHs/MATHinit.h"

/* Fonctions pour g�rer les LONGs */
#include "MATHs/MATHlong.h"

/* Fonctions pour g�rer les quaternions */
#include "MATHs/MATHquat.h"

/* Fonctions pour debugger */
#include "MATHs/MATHdebug.h"

#if defined( __cplusplus )
extern "C"
{
#endif

	/*
 ===================================================================================================
    Initialisation g�n�rale du module math
 ===================================================================================================
 */
	void MATH_InitModule( void );

#if defined( __cplusplus )
};
#endif

#endif
