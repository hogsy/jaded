#ifndef _BUILD_VER_
#define _BUILD_VER_


// Don't use! Edu version of MAX now keyed off serial number
// Define EDU_VERSION to build the educational version of MAX
//#define EDU_VERSION

// Define BETA_VERSION to use Beta lock
//#define BETA_VERSION

// Define STUDENT_VER to build the student version of MAX
// #define STUDENT_VER
#ifdef STUDENT_VER
#define WIN95_ONLY
#endif

//TURN ON SNAPPING FOR INTEGRATION TO ATHENA
#define _OSNAP TRUE

//TURN ON PRIMITIVE CREATION WITH 3D SNAPPING
#define _3D_CREATE

// Turn on sub material assignment : 1/19/98 - CCJ
#define _SUBMTLASSIGNMENT

// Define to build VIZ; now may be only defined using the CL environment variable.
//#define DESIGN_VER

#ifdef DESIGN_VER
	// Define turn on layers
	#define _LAYERS_
#endif
//JH 1/31/02 Enabling layer UI for Luna
#define _LAYERS_LEVEL2_

// Define to build a version with no NURBS
//#define NO_NURBS

// Define APL_DBCS for double-byte character set versions (i.e. Japanese, Chinese)
//#define APL_DBCS

// Define to build a version with EDP (Event-Driven Particles)
//#define EDP_VER

// Define to Enable Status Bar Function Publishing : 11/14/01
#define USE_STATUSPANEL_FP

//turn on features for Lightscape radiosity into MAX
#define NOVA_VER

// SDK version defines : 02/26/01 - russom
#define SDK_RESERVED_METHOD(a)
#define SDK_ORDER_VER	0

// no longer used by MAX
#if !defined(EDU_VERSION) && !defined(STUDENT_VER) && !defined(DESIGN_VER) && !defined(BETA_VERSION)
#define ORDINARY_VER
#endif

// errors that will no longer occur
#if defined(EDU_VERSION) && defined(STUDENT_VER)
#error "Both EDU_VERSION and STUDENT_VER defined in buildver.h!"
#endif

#if defined(EDU_VERSION) && defined(BETA_VERSION)
#error "Both EDU_VERSION and BETA_VERSION defined in buildver.h!"
#endif

// Enables mapping of a generic resource id to a product specific one
// through the resource overwrite mechanism 
#define RESOURCE_OVERRIDE

#endif // _BUILD_VER_
