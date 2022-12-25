#ifndef __GCMATHGECTOR_H__

#define __GCMATHGECTOR_H__ 1


/*
 * Map compiler vector float to Gector  
 */ 
typedef __vec2x32float__ Gector;


/*
 * init_ps : Initlize and return Gecko vector (Gector) 
 */
inline Gector init_ps (const register float ps0, 
					   const register float ps1);

/*
 * get_ps0 : Return first float of a vector. 
 */
inline float get_ps0(const register Gector v);

/*
 * get_ps1 : Return second float of a vector. 
 */
inline float get_ps1(const register Gector v);


/* 
 * get_pair : Place PS0 and PS1 of v into fa and fb respectivily.
 *            Note : The compiler can deal with get_ps0/1 better
 *                   optimization wise in most cases. It's 
 *                   recommended you use those.
 */
inline void get_pair (register Gector v, 
			   		  register float *fa,
			   		  register float *fb);


Gector init_ps (const register float ps0, 
 			    const register float ps1)
{
    register Gector fd; 
    asm
	{
	    ps_merge00   fd, ps0, ps1;
	}
    return fd;
}

float get_ps0 (const register Gector v)
{
	register float a;
	asm
	{
		ps_mr 	a, v;
	}
	return a;
}

float get_ps1 (const register Gector v)
{
	register float b;
	asm
	{
		ps_merge11 	b, v, v;
	}
	return b;
}

void get_pair (register Gector v, 
			   register float *fa,
			   register float *fb)
{
	asm
	{
		psq_stx		v, 0, fa, 1, 0; 
		ps_merge11 	v, v, v;
		psq_stx		v, 0, fb, 1, 0;				
	}
}
 
#endif
