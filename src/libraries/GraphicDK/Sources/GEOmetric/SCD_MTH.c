#ifndef PSX2_TARGET
#include "BASe/BAStypes.h"

#define CROSS_PRODUCT(a,b,c,_X,_Y,_Z)\
{\
	c -> _X =   a -> _Y *  b -> _Z - a -> _Z *  b -> _Y;\
	c -> _Y =   a -> _Z *  b -> _X - a -> _X *  b -> _Z;\
	c -> _Z =   a -> _X *  b -> _Y - a -> _Y *  b -> _X;\
}

#define SUB_VECTOR(a,b,c,_X,_Y,_Z)\
{\
	c -> _X  = (a -> _X - b -> _X);\
	c -> _Y  = (a -> _Y - b -> _Y);\
	c -> _Z  = (a -> _Z - b -> _Z);\
}

#define ADD_VECTOR(a,b,c,_X,_Y,_Z)\
{\
	c -> _X  = (a -> _X+ b -> _X);\
	c -> _Y  = (a -> _Y+ b -> _Y);\
	c -> _Z  = (a -> _Z+ b -> _Z);\
}
#define ADD_MUL_ADD_VECTOR(a,b,Mltp,c,_X,_Y,_Z)\
{\
	c -> _X  = (a -> _X+ Mltp * b -> _X);\
	c -> _Y  = (a -> _Y+ Mltp * b -> _Y);\
	c -> _Z  = (a -> _Z+ Mltp * b -> _Z);\
}

#define NORMALIZE(a,_X,_Y,_Z)\
{\
	float lenght;\
	lenght = a -> _X * a -> _X + a -> _Y * a -> _Y + a -> _Z * a -> _Z;\
	if (lenght != 0.0f)\
	lenght = LOD_MTH_fn_fInvSquareRootOpt(lenght);\
	a -> _X = a -> _X * lenght;\
	a -> _Y = a -> _Y * lenght;\
	a -> _Z = a -> _Z * lenght;\
}

#define ADD_VECTOR_NORMALIZED(a,b,c,_X,_Y,_Z)\
{\
	MAD_Vertex	VectorA, VectorB;\
	VectorA = *a;\
	VectorB = *b;\
	NORMALIZE((&VectorA),_X,_Y,_Z)\
	NORMALIZE((&VectorB),_X,_Y,_Z)\
	ADD_VECTOR((&VectorA),(&VectorB),c,_X,_Y,_Z)\
}


#define DOT_PRODUCT(a,b,_X,_Y,_Z)\
	(a -> _Y * b -> _Y) + (a -> _Z * b -> _Z) + (a -> _X * b -> _X) 

#define NORME(a,N,_X,_Y,_Z)\
{\
	N = a -> _X * a -> _X + a -> _Y * a -> _Y + a -> _Z * a -> _Z;\
	if (N != 0.0f)\
	N = LOD_MTH_fn_fSquareRootOpt(N);\
}

#define DISTANCE(a,b,N,_X,_Y,_Z)\
{\
	MAD_Vertex	VectorA;\
	SUB_VECTOR(a,b,(&VectorA),_X,_Y,_Z);\
	NORME((&VectorA),N,_X,_Y,_Z)\
}

#define COLOR_TO_VERTEX(Color,Vertex,_X,_Y,_Z)\
{\
	Vertex -> _X = (float)(Color & 0xff)       ;\
	Vertex -> _Y = (float)((Color>>8) & 0xff)  ;\
	Vertex -> _Z = (float)((Color>>16) & 0xff) ;\
}
#define NORMALE_TRIANGLE(PA,PB,PC,NT,_X,_Y,_Z)\
{\
	MAD_Vertex V1,V2;\
	SUB_VECTOR(PA,PB,(&V1),_X,_Y,_Z);\
	SUB_VECTOR(PA,PC,(&V2),_X,_Y,_Z);\
	CROSS_PRODUCT((&V2),(&V1),NT,_X,_Y,_Z);\
}


#define LOD_MTH_SIMULATELONG(a) *(LONG *)&(a)
#define LOD_MTH_ABSOLUTE(a) LOD_MTH_SIMULATELONG(a) &= 0x7fffffff;
#define LOD_MTH_IS_NEGATIVE(a) (a < 0.0f)


float LOD_MTH_fn_fSquareRootOpt(float f)  {return (float)(sqrt(f));}
float LOD_MTH_fn_fInverseOpt(float f)     {return (1.0f / f);}
float LOD_MTH_fn_fInvSquareRootOpt(float f) {return (float)(1.0f / sqrt(f));}


void LOD_MTH_fn_vInit( void )
{
  static unsigned char InitDone=0;

  if(InitDone==0)
    {
      InitDone=1;
    }
}
#endif /* ! PSX2_TARGET */