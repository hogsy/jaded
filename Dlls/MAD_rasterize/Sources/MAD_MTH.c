
#define MTH_OPTIMIZED 
#define MTH_INLINE static __inline
#define INLINE __inline

static double MATH_gd_Decal;
#define fInterpretLongAsFloat(a)    (*((float *) &(a)))
#define lInterpretFloatAsLong(a)    (*((long *) &(a)))

MTH_INLINE long lFloatToLongOpt(float a)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    double  b;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    b = a + MATH_gd_Decal;
    return lInterpretFloatAsLong(b);
}


static unsigned long MTH_g_a2048_fSquareRootTable[1024*2];
static unsigned long MTH_g_a1024_fInverse[1024];
static unsigned long MTH_g_a2048_fInvSquareRootTable[1024*2];


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
	lenght = MTH_fn_fInvSquareRootOpt(lenght);\
	a -> _X = a -> _X * lenght;\
	a -> _Y = a -> _Y * lenght;\
	a -> _Z = a -> _Z * lenght;\
}

#define ADD_VECTOR_NORMALIZED(a,b,c,_X,_Y,_Z)\
{\
	WD3D_Vertex	VectorA, VectorB;\
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
	N = MTH_fn_fSquareRootOpt(N);\
}

#define DISTANCE(a,b,N,_X,_Y,_Z)\
{\
	WD3D_Vertex	VectorA;\
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
	WD3D_Vertex V1,V2;\
	SUB_VECTOR(PA,PB,(&V1),_X,_Y,_Z);\
	SUB_VECTOR(PA,PC,(&V2),_X,_Y,_Z);\
	CROSS_PRODUCT((&V2),(&V1),NT,_X,_Y,_Z);\
}

#define GRADIENT_DIRECTION(PA,PB,PC,NT,GA,GB,GC,NTP,_X,_Y,_Z)\
{\
	WD3D_Vertex	PAP,PBP,PCP;\
	float DPR;\
	ADD_MUL_ADD_VECTOR(PA,NT,GA,(&PAP),_X,_Y,_Z);\
	ADD_MUL_ADD_VECTOR(PB,NT,GB,(&PBP),_X,_Y,_Z);\
	ADD_MUL_ADD_VECTOR(PC,NT,GC,(&PCP),_X,_Y,_Z);\
	NORMALE_TRIANGLE((&PAP),(&PBP),(&PCP),NTP,_X,_Y,_Z);\
	DPR = DOT_PRODUCT(NTP, NT ,_X,_Y,_Z);\
	ADD_MUL_ADD_VECTOR(NTP,NT,-DPR,NTP,_X,_Y,_Z);\
}

#ifdef MTH_OPTIMIZED

#define MTH_SIMULATELONG(a) *(long *)&(a)
#define MTH_ABSOLUTE(a) MTH_SIMULATELONG(a) &= 0x7fffffff;
#define MTH_IS_NEGATIVE(a) (a < 0.0f)


MTH_INLINE float MTH_fn_fSquareRootOpt(float f)
{
  float res_sqrt;
//  MTH_M_vCHK(f);

  /* f= (-1)^s.2^E.[1.M] */
  _asm{
#ifdef MTH_PARANOID
        push ebx
        push eax
#endif /* MTH_PARANOID */

        mov  ebx,f              /* ebx = f */
        mov  eax,f              /* eax = f */
        and  ebx,0x7F800000     /* ebx = E */

        and  eax,0x00FFE000     /* eax = 1st bit of E & M */

        add  ebx,0x3F800000     /* ebx= E + (127<<23) */
        shr  ebx,1              /* ebx = ebx/2 */

        shr  eax,11             /* eax = index on table */

        and  ebx,0x7F800000     /* ebx = new E */
        add  ebx,dword ptr[MTH_g_a2048_fSquareRootTable+eax]    /* Get from table */
        mov  dword ptr[res_sqrt],ebx

#ifdef MTH_PARANOID
        pop eax
        pop ebx
#endif /* MTH_PARANOID */
      }

//  MTH_M_vCHK(res_sqrt);

  return (res_sqrt);
}




MTH_INLINE float MTH_fn_fInverseOpt(float f)
{
  float res_inv;
  _asm{        
#ifdef MTH_PARANOID
      push ecx
      push ebx
      push eax
#endif /* MTH_PARANOID */

      mov  ebx,f
      mov  ecx,0x7E800000                               /* 1 Clocks                          */
      mov  eax,ebx
      and  eax,0x007FE000                               /* 1 Clocks                          */
      and  ebx,0xFF800000
      shr  eax,11                                       /* 1 Clocks                          */
      sub  ecx,ebx
      add  ecx,dword ptr[MTH_g_a1024_fInverse + eax]    /* 3 Clocks     Exp_AGI_U_Pem:1      */
      mov  dword ptr[res_inv],ecx                       /* 1 Clocks     Exp_Flow_Dep_ecx     */

#ifdef MTH_PARANOID
      pop eax
      pop ebx
      pop ecx
#endif /* MTH_PARANOID */
      }

  return (res_inv);
}

MTH_INLINE float MTH_fn_fInvSquareRootOpt(float f)
{
  float res_invsqrt;
  /* To test vality of this function :
  float res_high;
  char  c_test[30];
  */

  /* f= (-1)^s.2^E.[1.M] */
  _asm{
#ifdef MTH_PARANOID
      push ecx
      push ebx
      push eax
#endif /* MTH_PARANOID */

      mov  ecx,f                /* ecx = f */                   
      mov  eax,f                /* eax = f, to allow pairing */
      and  ecx,0x7F800000       /* ecx = E */
        
      mov  ebx,0xBD800000       /* ebx= 379 << 23 */

      and  eax,0x00FFE000       /* 1st bit of E (odd/even) & 10 high of M */ 

      sub  ebx,ecx              /* ebx= (379 << 23) -E */
      shr  ebx,1                /* ebx= ebx/2 */
      
      shr  eax,11               /* eax = index on table */ 
      and  ebx,0x7F800000       /* ebx = new E */
      add  ebx,dword ptr[MTH_g_a2048_fInvSquareRootTable + eax] /* Get from table */
      mov  dword ptr[res_invsqrt],ebx
#ifdef MTH_PARANOID
      pop eax
      pop ebx
      pop ecx
#endif /* MTH_PARANOID */
  }
  /*
  res_high= 1.0F/sqrt((float)f);
  assert( abs(1.0F-res_high/res_invsqrt) <0.001 );
  */
  return (res_invsqrt);
}
#else // MTH_OPTIMIZED

#define MTH_SIMULATELONG(a) *(long *)&(a)
#define MTH_ABSOLUTE(a) MTH_SIMULATELONG(a) &= 0x7fffffff;
#define MTH_IS_NEGATIVE(a) (a < 0.0f)


MTH_INLINE float MTH_fn_fSquareRootOpt(float f)  {return (float)(sqrt(f));}
MTH_INLINE float MTH_fn_fInverseOpt(float f)     {return (float)(1.0f / f);}
MTH_INLINE float MTH_fn_fInvSquareRootOpt(float f) {return (float)(1.0f / sqrt(f));}

#endif // MTH_OPTIMIZED


MTH_INLINE void MTH_fn_vInitSqrtRootOpt( void )
{
  long i;
  float ft;
  unsigned long m;

  for ( i=0; i<1024; i++)
    {
      ft = (float) sqrt((double) 1.0+((float)i/1024) );
      m = *(long *)&ft;
      m&= 0x7FFFFF;
      MTH_g_a2048_fSquareRootTable[i+1024] = m;

      ft = (float) ( sqrt(2.0)*sqrt((double) 1.0+((float)i/1024) ) );
      m = *(long *)&ft;
      m&= 0x7FFFFF;
      MTH_g_a2048_fSquareRootTable[i] = m;
    }
}


MTH_INLINE void MTH_fn_vInitInverseOpt( void )
{
  long i;
  float ft;

  for ( i=0; i<1024; i++ )
    {
      ft = 1.0F / ( 1.0F + ((float)i/1024) );
      MTH_g_a1024_fInverse[i]  = (*(long *)&ft) & 0x7FFFFF;
    }
  MTH_g_a1024_fInverse[0]=(1<<23); /* Because of problems with 2^n */
}

MTH_INLINE void MTH_fn_vInitInvSqrtRootOpt(void)
{
  long i;               /* index on the table   */
  unsigned long m;      /* mantis               */
  float ft;             /* float value          */
    
  for ( i=0; i<1024; i++ )
    {        
      ft = (float) ( 2.0/sqrt((double)( 1.0+((float)i/1024) )) );
      m = *(long *)&ft;
      m &= 0x7FFFFF;
      MTH_g_a2048_fInvSquareRootTable[i+1024] = m;
      ft = (float) ( sqrt(2.0)/sqrt((double)( 1.0+((float)i/1024) )) ); 
      m = *(long *)&ft;
      m &= 0x7FFFFF;
      m+=0x800000;
      MTH_g_a2048_fInvSquareRootTable[i] = m;
    }  
  MTH_g_a2048_fInvSquareRootTable[1024]=(1<<23);
}

MTH_INLINE void MTH_fn_vInit( void )
{
  static unsigned char InitDone=0;

  if(InitDone==0)
    {
      MTH_fn_vInitSqrtRootOpt();
      MTH_fn_vInitInverseOpt();
      MTH_fn_vInitInvSqrtRootOpt();
#ifdef JADEFUSION
		MATH_gd_Decal = 3.0F * pow(2.0f, 51.0f);
#else
		MATH_gd_Decal = 3.0F * pow(2, 51);
#endif
	  InitDone=1;
    }
}
