//#define VECTORC

#ifndef VECTORC
	#define __VCALLMS__
#else
	#include <VU.h>
#endif

#ifdef GCN
	#include <dolphin/os.h>
#endif

// this assumes the 4th and 8th rows of B are zero.

typedef struct tArray4OfFloats {float f [4];} Array4OfFloats;

#ifndef WIN32
	// VECTOR-C DECLARATIONS
	float _roInvI[16*2]	__attribute__ ((aligned(16)));
	float _pfJ[6*8*2]	__attribute__ ((aligned(16)));
	float _pfA[48]		__attribute__ ((aligned(16)));
	float _pfRHS[8]		__attribute__ ((aligned(16)));
	float oTmp1 [2*8]	__attribute__ ((aligned(16))); 
	float JinvM [6*8]	__attribute__ ((aligned(16)));	
#else	
	// WINDOWS DECLARATIONS
	float _roInvI[16*2] ;
	float _pfJ[6*8*2];
	float _pfA[48];
	float _pfRHS[8];
	float oTmp1 [2*8]; 	
	float JinvM [6*8];	
#endif

	// body 1
Array4OfFloats      _fIMass0fStep1;
Array4OfFloats      _roForce; 
Array4OfFloats      _roTorque;
Array4OfFloats      _roLvel;
Array4OfFloats      _roAvel;

	// body 2
Array4OfFloats      _fIMass0fStep12;
Array4OfFloats      _roForce2; 
Array4OfFloats      _roTorque2;
Array4OfFloats      _roLvel2;
Array4OfFloats      _roAvel2;


inline float  _BtimesC ( short i, short j)
{
#ifndef GCN
    float sum;
    sum =  JinvM[8*i+0]*_pfJ[8*j+0];
    sum += JinvM[8*i+1]*_pfJ[8*j+1];
    sum += JinvM[8*i+2]*_pfJ[8*j+2];
    sum += JinvM[8*i+4]*_pfJ[8*j+4];
    sum += JinvM[8*i+5]*_pfJ[8*j+5];
    sum += JinvM[8*i+6]*_pfJ[8*j+6];
	return sum;
#else
	// <JSPG> GameCube Assembler version
	float sum;
	register float* s = &sum;
	register const float* jm = &JinvM[8*i];
	register const float* pj = &_pfJ[8*j];
	asm
	{
		// (x) means jx*px
		psq_l		fp0, 0(jm), 0, 0; 	// fp0 <- j0,j1
		psq_l		fp1, 0(pj), 0, 0; 	// fp1 <- p0,p1
		ps_mul		fp3, fp0, fp1;   	// fp3 <- (0),(1)
		psq_l		fp0, 8(jm), 1, 0; 	// fp0 <- j2,1
		psq_l		fp1, 8(pj), 1, 0; 	// fp1 <- p2,1
		ps_mul		fp2, fp0, fp1;		// fp2 <- (2),1 <-
		psq_l		fp0, 16(jm), 0, 0; 	// fp0 <- j4,j5
		psq_l		fp1, 16(pj), 0, 0; 	// fp1 <- p4,p5
		ps_madd		fp4, fp0, fp1, fp3;	// fp4 <- (4)+(0),(5)+(1) <-
		psq_l		fp0, 24(jm), 1, 0; 	// fp0 <- j6,1
		psq_l		fp1, 24(pj), 1, 0; 	// fp1 <- p6,1	
		ps_mul		fp3, fp0, fp1;		// fp3 <- (6),1 <-
		ps_sum0		fp5, fp4, fp3, fp4; // fp5 <- (4)+(0)+(5)+(1),1
		ps_add		fp6, fp5, fp3;		// fp6 <- (4)+(0)+(5)+(1)+(6),2
		ps_add		fp5, fp6, fp2;		// fp5 <- (4)+(0)+(5)+(1)+(6)+(2),3
		psq_st		fp5, 0(s), 1, 0;	// sum = fp5
	}
	return(sum);
#endif
}

inline float  _BtimesC0 ( short i)
{
#ifndef GCN
    float sum;
    sum =  _pfJ[8*i+0]*oTmp1[0];
    sum += _pfJ[8*i+1]*oTmp1[1];
    sum += _pfJ[8*i+2]*oTmp1[2];
    sum += _pfJ[8*i+4]*oTmp1[4];
    sum += _pfJ[8*i+5]*oTmp1[5];
    sum += _pfJ[8*i+6]*oTmp1[6];
	return sum;
#else
	// <JSPG> GameCube Assembler version
	float sum;
	register float* s = &sum;
	register const float* jm = &_pfJ[8*i];
	register const float* pj = oTmp1;
	asm
	{
		// (x) means jx*px
		psq_l		fp0, 0(jm), 0, 0; 	// fp0 <- j0,j1
		psq_l		fp1, 0(pj), 0, 0; 	// fp1 <- p0,p1
		ps_mul		fp3, fp0, fp1;   	// fp3 <- (0),(1)
		psq_l		fp0, 8(jm), 1, 0; 	// fp0 <- j2,1
		psq_l		fp1, 8(pj), 1, 0; 	// fp1 <- p2,1
		ps_mul		fp2, fp0, fp1;		// fp2 <- (2),1 <-
		psq_l		fp0, 16(jm), 0, 0; 	// fp0 <- j4,j5
		psq_l		fp1, 16(pj), 0, 0; 	// fp1 <- p4,p5
		ps_madd		fp4, fp0, fp1, fp3;	// fp4 <- (4)+(0),(5)+(1) <-
		psq_l		fp0, 24(jm), 1, 0; 	// fp0 <- j6,1
		psq_l		fp1, 24(pj), 1, 0; 	// fp1 <- p6,1	
		ps_mul		fp3, fp0, fp1;		// fp3 <- (6),1 <-
		ps_sum0		fp5, fp4, fp3, fp4; // fp5 <- (4)+(0)+(5)+(1),1
		ps_add		fp6, fp5, fp3;		// fp6 <- (4)+(0)+(5)+(1)+(6),2
		ps_add		fp5, fp6, fp2;		// fp5 <- (4)+(0)+(5)+(1)+(6)+(2),3
		psq_st		fp5, 0(s), 1, 0;	// sum = fp5
	}
	return(sum);
#endif
}

void ClearMatrices()
{
#ifndef GCN
	short k;
	for (k=0; k< 12; k++)
	{
		_pfA[4*k] =0.0f;
		_pfA[4*k+1] =0.0f;
		_pfA[4*k+2] =0.0f;
		_pfA[4*k+3] =0.0f;
	}
	_pfRHS[0] =0.0f;
	_pfRHS[1] =0.0f;
	_pfRHS[2] =0.0f;
	_pfRHS[3] =0.0f;
	_pfRHS[4] =0.0f;
	_pfRHS[5] =0.0f;
	_pfRHS[6] =0.0f;
	_pfRHS[7] =0.0f;
#else
	// <JSPG> GameCube Assembler version
	static float a2fZero[2] = {0.0f, 0.0f};
	register const float* z = a2fZero;
	register const float* pa = _pfA;
	register const float* prhs = _pfA;
	asm
	{
		psq_l	fp0, 0(z), 0, 0; // fp0 <- 0,0
		// k=0
		psq_st  fp0, 0(pa), 0, 0;
		psq_st  fp0, 8(pa), 0, 0;
		// k=1
		psq_st  fp0, 16(pa), 0, 0;
		psq_st  fp0, 24(pa), 0, 0;
		// k=2
		psq_st  fp0, 32(pa), 0, 0;
		psq_st  fp0, 40(pa), 0, 0;
		// k=3
		psq_st  fp0, 48(pa), 0, 0;
		psq_st  fp0, 56(pa), 0, 0;
		// k=4
		psq_st  fp0, 64(pa), 0, 0;
		psq_st  fp0, 72(pa), 0, 0;
		// k=5
		psq_st  fp0, 80(pa), 0, 0;
		psq_st  fp0, 88(pa), 0, 0;
		// k=6
		psq_st  fp0, 96(pa), 0, 0;
		psq_st  fp0, 104(pa), 0, 0;
		// k=7
		psq_st  fp0, 112(pa), 0, 0;
		psq_st  fp0, 120(pa), 0, 0;
		// k=8
		psq_st  fp0, 128(pa), 0, 0;
		psq_st  fp0, 136(pa), 0, 0;
		// k=9
		psq_st  fp0, 144(pa), 0, 0;
		psq_st  fp0, 152(pa), 0, 0;
		// k=10
		psq_st  fp0, 160(pa), 0, 0;
		psq_st  fp0, 168(pa), 0, 0;
		// k=11
		psq_st  fp0, 176(pa), 0, 0;
		psq_st  fp0, 184(pa), 0, 0;
		// rhs
		psq_st	fp0, 0(prhs), 0, 0;
		psq_st	fp0, 8(prhs), 0, 0;
		psq_st	fp0, 16(prhs), 0, 0;
		psq_st	fp0, 24(prhs), 0, 0;
	}
#endif
}

void __VCALLMS__ MultiplyAdd1_8q1_vu0 ()
{
	float sum;	
		
	sum = _pfJ[0*8] * _roInvI[0];	
	sum += _pfJ[1*8] * _roInvI[1];
	sum += _pfJ[2*8] * _roInvI[2];
	sum += _pfJ[3*8] * _roInvI[3];
	sum += _pfJ[4*8] * _roInvI[4];
	sum += _pfJ[5*8] * _roInvI[5];
	_pfRHS[0] = sum;
	
	sum  = _pfJ[0*8+1] * _roInvI[0];	
	sum += _pfJ[1*8+1] * _roInvI[1];
	sum += _pfJ[2*8+1] * _roInvI[2];
	sum += _pfJ[3*8+1] * _roInvI[3];
	sum += _pfJ[4*8+1] * _roInvI[4];
	sum += _pfJ[5*8+1] * _roInvI[5];	
	_pfRHS[1] = sum;
	
	sum  = _pfJ[0*8+2] * _roInvI[0];	
	sum += _pfJ[1*8+2] * _roInvI[1];
	sum += _pfJ[2*8+2] * _roInvI[2];
	sum += _pfJ[3*8+2] * _roInvI[3];
	sum += _pfJ[4*8+2] * _roInvI[4];
	sum += _pfJ[5*8+2] * _roInvI[5];	
	_pfRHS[2] = sum;
	
	sum  = _pfJ[0*8+4] * _roInvI[0];	
	sum += _pfJ[1*8+4] * _roInvI[1];
	sum += _pfJ[2*8+4] * _roInvI[2];
	sum += _pfJ[3*8+4] * _roInvI[3];
	sum += _pfJ[4*8+4] * _roInvI[4];
	sum += _pfJ[5*8+4] * _roInvI[5];	
	_pfRHS[4] = sum;
		
	sum  = _pfJ[0*8+5] * _roInvI[0];	
	sum += _pfJ[1*8+5] * _roInvI[1];
	sum += _pfJ[2*8+5] * _roInvI[2];
	sum += _pfJ[3*8+5] * _roInvI[3];
	sum += _pfJ[4*8+5] * _roInvI[4];
	sum += _pfJ[5*8+5] * _roInvI[5];	
	_pfRHS[5] = sum;
	
	sum  = _pfJ[0*8+6] * _roInvI[0];	
	sum += _pfJ[1*8+6] * _roInvI[1];
	sum += _pfJ[2*8+6] * _roInvI[2];
	sum += _pfJ[3*8+6] * _roInvI[3];
	sum += _pfJ[4*8+6] * _roInvI[4];
	sum += _pfJ[5*8+6] * _roInvI[5];	
	_pfRHS[6] = sum;
}


// global variables


void  PrepareSolveLCP (short _m)
{	
	float oPt1 [4];
	short j;	
	float sum;
	int iidx;

#ifdef GCN
	register const float* in = _roInvI;
	register const float* ma = &_fIMass0fStep1.f[0];
	asm
	{
		psq_l	fp18, 0(ma), 1, 0;		// fp18 <- m0,1
		ps_merge00	fp18, fp18, fp18;	// fp18 <- m0,m0
		psq_l	fp19, 4(ma), 1, 0;	// fp19 <- m1,1
		ps_merge00	fp19, fp19, fp19;// fp19 <- m1,m1
	}
#endif

	for (j = 0; j < _m; j++ )
	{
		// linear component : Jinv1l = (1/Mass) * J1l
		iidx = (int)8*j;
		#ifdef GCN
			// <JSPG> GameCube Assembler version
			register const float* ji = &JinvM[iidx];
			register const float* pj = &_pfJ[iidx];
			asm
			{
				psq_l	fp1, 0(pj), 0, 0;		// fp1 <- j0,j1
				ps_mul	fp4, fp18, fp1;			// fp4 <- j0*m0,j1*m0
				psq_st	fp4, 0(ji), 0, 0;		// store JinvM[idx] y JinvM[idx+1]
				psq_l	fp1, 8(pj), 1, 0;		// fp1 <- j2,1
				ps_mul	fp4, fp18, fp1;			// fp4 <- j1*m0, m0
				psq_st	fp4, 8(ji), 1, 0;		// store JinvM[idx+2]
				
				// angular component
				psq_l	fp0, 16(pj), 0, 0;		// fp0 <- pt0,pt1 -> pt0=pt4, pt1=pj5
				psq_l	fp1, 24(pj), 1, 0;		// fp1 <- pt2,1   -> pt2 = pj6
				psq_l	fp12, 0(in), 0, 0;		// fp12 <- in0,in1
				psq_l	fp13, 8(in), 1, 0;		// fp13 <- in2,1
				psq_l	fp14, 16(in), 0, 0;		// fp14 <- in4,in5
				psq_l	fp15, 24(in), 1, 0;		// fp15 <- in6,1
				psq_l	fp16, 32(in), 0, 0;		// fp16 <- in8,in9
				psq_l	fp17, 40(in), 1, 0;		// fp17 <- in10,1
				ps_mul	fp8, fp0, fp12;			// fp8 <- pt0in0,pt1in1
				ps_mul	fp9, fp1, fp13;			// fp9 <- pt2in2,1
				ps_sum1	fp10, fp8, fp9, fp8;	// fp10 <- pt2in2,pt0in0+pt1in1
				ps_sum0 fp10, fp10, fp1, fp10	// fp10 <- pt2in2+pt0in0+pt1in1,1
				ps_mul	fp8, fp0, fp14;			// fp8 <- pt0in4,pt1in5
				ps_mul	fp9, fp1, fp15;			// fp9 <- pt2in6,1
				ps_sum1	fp11, fp8, fp9, fp8;	// fp11 <- pt2in6,pt0in4+pt1in5
				ps_sum1 fp11, fp11, fp10, fp11	// fp11 <- pt2in2+pt0in0+pt1in1,pt0in4+pt1in5+pt2in6
				psq_st	fp11, 16(ji), 0, 0;		// store JinvM[idx+4],JinvM[idx+5]
				ps_mul	fp8, fp0, fp16;			// fp8 <- pt0in8,pt1in9
				ps_mul	fp9, fp1, fp17;			// fp9 <- pt2in10,1
				ps_sum1 fp10, fp8, fp9, fp8;	// fp10 <- pt2in10,pt0in8+pt1in9
				ps_sum0 fp11, fp10, fp1, fp10;	// fp11 <- pt2in10+pt0in8+pt1in9,x
				psq_st	fp11, 24(ji), 1, 0;		// store JinvM[idx+6]
			}
		#else
			JinvM[8*j]   = _fIMass0fStep1.f[0] * _pfJ[8*j] ;
			JinvM[8*j+1] = _fIMass0fStep1.f[0] * _pfJ[8*j+1] ;
			JinvM[8*j+2] = _fIMass0fStep1.f[0] * _pfJ[8*j+2] ;
					
			// angular component : Jinv1a = body_invI * J1a
			oPt1[0] = _pfJ[8*j+4];
			oPt1[1] = _pfJ[8*j+5];
			oPt1[2] = _pfJ[8*j+6];
			JinvM[8*j+4] =
				oPt1[0] * _roInvI[0] + oPt1[1] * _roInvI[1] + oPt1[2] * _roInvI[2];
			JinvM[8*j+5] =
				oPt1[0] * _roInvI[4] + oPt1[1] * _roInvI[5] + oPt1[2] * _roInvI[6];
			JinvM[8*j+6] =
				oPt1[0] * _roInvI[8] + oPt1[1] * _roInvI[9] + oPt1[2] * _roInvI[10];
		#endif
	}		
	if (_m <= 4)
	{	
		for (j=0; j<_m; j++) 
		{				
			sum = _BtimesC( j, 0);
			_pfA[4*j] += sum;		
			sum = _BtimesC(  j, 1);
			_pfA[4*j+1] += sum;
			sum  = _BtimesC( j, 2);
			_pfA[4*j+2] += sum;			
			if ( _m == 4 )
			{
				sum = _BtimesC( j, 3);
				_pfA[4*j+3] += sum;
			}
		}			
	}
	else		
	{		
		for (j=0; j< _m; j++) 
		{				
			sum = _BtimesC( j, 0);
			_pfA[8*j] += sum;		
			sum = _BtimesC( j, 1);
			_pfA[8*j+1] += sum;
			sum = _BtimesC(j, 2);
			_pfA[8*j+2] += sum;
			sum = _BtimesC( j, 3);
			_pfA[8*j+3] += sum;			
			if ( _m >= 5 )
			{
				sum  = _BtimesC( j,4);
				_pfA[8*j+4] += sum;
			}
			if ( _m == 6 )
			{
				sum  = _BtimesC( j,5);
				_pfA[8*j+5]  += sum;
			}  		
		} 		
	}
	
	// Now compute the right hand side 'rhs'		

	#ifdef GCN
		// <JSPG> GameCube Assembler Version
		
		register const float* tm = oTmp1;
		register const float* fo = &_roForce.f[0];
		register const float* ve = &_roLvel.f[0];
		register const float* to = &_roTorque.f[0];
		register const float* av = &_roAvel.f[0];

		asm
		{
			// Linear component
			psq_l	fp1, 0(fo), 0, 0;	// fp1 <- f0,f1
			psq_l	fp2, 0(ve), 0, 0;	// fp2 <- v0,v1
			ps_mul	fp3, fp18, fp1;		// fp3 <- m0f0,m0f1
			ps_mul	fp4, fp19, fp2;		// fp4 <- m1v0,m1v1
			ps_add	fp5, fp3, fp4;		// fp5 <- m0f0+m1v0,m0f1+m1v1
			psq_st	fp5, 0(tm), 0, 0;	// store oTmp1[0] & oTmp1[1]
			psq_l	fp1, 8(fo), 1, 0;	// fp1 <- f2,1
			psq_l	fp2, 8(ve), 1, 0;	// fp2 <- v2,1
			ps_mul	fp3, fp18, fp1;		// fp3 <- m0f2,x
			ps_mul	fp4, fp19, fp2;		// fp4 <- m1v2,x
			ps_add	fp5, fp3, fp4;		// fp5 <- m0f2+m1v2,x
			psq_st	fp5, 8(tm), 1, 0;	// store oTmp1[2]

			// Angular component
			psq_l	fp0, 0(to), 1, 0;	// fp0 <- t0,1
			ps_merge00	fp0, fp0, fp0;	// fp0 <- t0,t0
			psq_l	fp1, 4(to), 1, 0;	// fp1 <- t1,1
			ps_merge00	fp1, fp1, fp1;	// fp1 <- t1,t1
			psq_l	fp2, 8(to), 1, 0;	// fp2 <- t2,1
			ps_merge00	fp2, fp2, fp2;	// fp2 <- t2,t2
			psq_l	fp3, 0(av), 0, 0;	// fp3 <- a0,a1
			ps_mul	fp4, fp0, fp12;		// fp4 <- t0i0,t0i1
			ps_mul	fp5, fp1, fp14;		// fp5 <- t1i4,t1i5
			ps_mul	fp6, fp2, fp16;		// fp6 <- t2i8,t2i9
			ps_mul	fp7, fp3, fp19;		// fp7 <- a0m1,a1m1
			ps_add	fp8, fp4, fp5;		// fp8 <- t0i0+t1i4,t0i1+t1i5
			ps_add	fp9, fp6, fp7;		// fp9 <- t2i8+a0m1,t2i9+a1m1
			ps_add	fp10, fp8, fp9;		// fp10 <- oTmp1[4], oTmp1[5]
			psq_st	fp10, 16(tm), 0, 0;	// store oTmp1[4], oTmp1[5]
			psq_l	fp3, 8(av), 1, 0;	// fp3 <- a2,1   13,15,17
			ps_mul	fp4, fp0, fp13;		// fp4 <- t0i2,x
			ps_mul	fp5, fp1, fp15;		// fp5 <- t1i6,x
			ps_mul	fp6, fp2, fp17;		// fp6 <- t2i10,x
			ps_mul	fp7, fp3, fp19;		// fp7 <- a2m1,x
			ps_add	fp8, fp4, fp5;		// fp8 <- t0i2+t1i6,x
			ps_add	fp9, fp6, fp7;		// fp9 <- t2i10+a2m1
			ps_add	fp10, fp8, fp9;		// fp10 <- oTmp1[6],x
			psq_st	fp10, 24(tm), 1, 0;	// store oTmp1[6]
		}

	#else
		// Linear Component	
		oTmp1[0] = _fIMass0fStep1.f[0] * _roForce.f[0] + _roLvel.f[0] * _fIMass0fStep1.f[1];
		oTmp1[1] = _fIMass0fStep1.f[0] * _roForce.f[1] + _roLvel.f[1] * _fIMass0fStep1.f[1];
		oTmp1[2] = _fIMass0fStep1.f[0] * _roForce.f[2] + _roLvel.f[2] * _fIMass0fStep1.f[1];
		
		// Angular Component
		oTmp1[4] = 
				_roTorque.f[0] * _roInvI[0] + _roTorque.f[1] * _roInvI[4] + _roTorque.f[2] * _roInvI[8]			
				+ _roAvel.f[0] * _fIMass0fStep1.f[1];

		oTmp1[5] = 
				_roTorque.f[0] * _roInvI[1] + _roTorque.f[1] * _roInvI[5] + _roTorque.f[2] * _roInvI[9]
				+ _roAvel.f[1] * _fIMass0fStep1.f[1];

		oTmp1[6] = 
				_roTorque.f[0] * _roInvI[2] + _roTorque.f[1] * _roInvI[6] + _roTorque.f[2] * _roInvI[10]
				+ _roAvel.f[2] * _fIMass0fStep1.f[1];
	#endif

	// Put J*tmp1 int rhs	
	sum = _BtimesC0(  0);	   
	_pfRHS[0] += sum;
	sum = _BtimesC0(  1);
	_pfRHS[1] += sum;
	sum  = _BtimesC0( 2);
	_pfRHS[2] += sum;	
	if ( _m >= 4 )
	{
		sum =  _BtimesC0( 3);
		_pfRHS[3] += sum;
	}
	if ( _m >= 5 )
	{
		sum  = _BtimesC0( 4);
		_pfRHS[4] += sum;
	}
	if ( _m == 6 )
	{
		sum  = _BtimesC0( 5);
		_pfRHS[5] += sum;
	}  
}

void __VCALLMS__ PrepareSolveLCP1 (short _m)
{	
	ClearMatrices();
	PrepareSolveLCP(_m);
}

void __VCALLMS__ PrepareSolveLCP2 (short _m)
{
	short j, j1;

	ClearMatrices();
	PrepareSolveLCP(_m);
		
	// Jacobian body 2 -> Jacobian body 1 

#ifdef GCN
	register const float* pfj0;
	register const float* pfj1;
#endif

	j1 = _m;
	for (j = 0; j < _m; j++ )
	{		
		#ifdef GCN
			// <JSPG> GameCube Assembler Version
			pfj0 = &_pfJ[8*j];
			pfj1 = &_pfJ[8*j1];
			
			asm
			{
				psq_l	fp0, 0(pfj1), 0, 0;
				psq_st	fp0, 0(pfj0), 0, 0;
				psq_l	fp0, 8(pfj1), 0, 0;
				psq_st	fp0, 8(pfj0), 0, 0;
				psq_l	fp0, 16(pfj1), 0, 0;
				psq_st	fp0, 16(pfj0), 0, 0;
				psq_l	fp0, 24(pfj1), 0, 0;
				psq_st	fp0, 24(pfj0), 0, 0;
			}

			j1++;
		#else
			_pfJ[8*j+0] = _pfJ[8*j1+0];
			_pfJ[8*j+1] = _pfJ[8*j1+1];
			_pfJ[8*j+2] = _pfJ[8*j1+2];
			_pfJ[8*j+3] = _pfJ[8*j1+3];
			_pfJ[8*j+4] = _pfJ[8*j1+4];
			_pfJ[8*j+5] = _pfJ[8*j1+5];
			_pfJ[8*j+6] = _pfJ[8*j1+6];
			_pfJ[8*j+7] = _pfJ[8*j1+7];
			j1++;
		#endif
	}

	// Inertia body 2 -> Inertia body 1	
	#ifdef GCN
	
		register const float* inv = &_roInvI[0];
		register const float* step1 = &_fIMass0fStep1.f[0];
		register const float* step12 = &_fIMass0fStep12.f[0];
		register const float* force = &_roForce.f[0];
		register const float* force2 = &_roForce2.f[0];
		register const float* torque = &_roTorque.f[0];
		register const float* torque2 = &_roTorque2.f[0];
		register const float* lvel = &_roLvel.f[0];
		register const float* lvel2 = &_roLvel2.f[0];
		register const float* avel = &_roAvel.f[0];
		register const float* avel2 = &_roAvel2.f[0];

		asm
		{
			psq_l	fp0, 64(inv), 0, 0;
			psq_st	fp0, 0(inv), 0, 0;
			psq_l	fp0, 72(inv), 0, 0;
			psq_st	fp0, 8(inv), 0, 0;

			psq_l	fp0, 80(inv), 0, 0;
			psq_st	fp0, 16(inv), 0, 0;
			psq_l	fp0, 88(inv), 0, 0;
			psq_st	fp0, 24(inv), 0, 0;

			psq_l	fp0, 96(inv), 0, 0;
			psq_st	fp0, 32(inv), 0, 0;
			psq_l	fp0, 104(inv), 0, 0;
			psq_st	fp0, 40(inv), 0, 0;

			psq_l	fp0, 112(inv), 0, 0;
			psq_st	fp0, 48(inv), 0, 0;
			psq_l	fp0, 120(inv), 0, 0;
			psq_st	fp0, 56(inv), 0, 0;
			
			psq_l	fp0, 0(step12), 0, 0;
			psq_st	fp0, 0(step1), 0, 0;
			
			psq_l	fp0, 0(force2), 0, 0;
			psq_st	fp0, 0(force), 0, 0;			
			psq_l	fp0, 8(force2), 0, 0;
			psq_st	fp0, 8(force), 0, 0;
			
			psq_l	fp0, 0(torque2), 0, 0;
			psq_st	fp0, 0(torque), 0, 0;			
			psq_l	fp0, 8(torque2), 0, 0;
			psq_st	fp0, 8(torque), 0, 0;
			
			psq_l	fp0, 0(lvel2), 0, 0;
			psq_st	fp0, 0(lvel), 0, 0;			
			psq_l	fp0, 8(lvel2), 0, 0;
			psq_st	fp0, 8(lvel), 0, 0;

			psq_l	fp0, 0(avel2), 0, 0;
			psq_st	fp0, 0(avel), 0, 0;			
			psq_l	fp0, 8(avel2), 0, 0;
			psq_st	fp0, 8(avel), 0, 0;
		}
	#else
		_roInvI[0] = _roInvI[16];
		_roInvI[1] = _roInvI[17];
		_roInvI[2] = _roInvI[18];
		_roInvI[3] = _roInvI[19];

		_roInvI[4] = _roInvI[20];
		_roInvI[5] = _roInvI[21];
		_roInvI[6] = _roInvI[22];
		_roInvI[7] = _roInvI[23];

		_roInvI[8]  = _roInvI[24];
		_roInvI[9]  = _roInvI[25];
		_roInvI[10] = _roInvI[26];
		_roInvI[11] = _roInvI[27];

		_roInvI[12] = _roInvI[28];
		_roInvI[13] = _roInvI[29];
		_roInvI[14] = _roInvI[30];
		_roInvI[15] = _roInvI[31];

		 _fIMass0fStep1.f[0]	= _fIMass0fStep12.f[0];
		 _fIMass0fStep1.f[1]	= _fIMass0fStep12.f[1];	 

		_roForce.f[0]	 = _roForce2.f[0]; 
		_roForce.f[1]	 = _roForce2.f[1]; 
		_roForce.f[2]	 = _roForce2.f[2]; 
		_roForce.f[3]	 = _roForce2.f[3]; 

		_roTorque.f[0] = _roTorque2.f[0];
		_roTorque.f[1] = _roTorque2.f[1];
		_roTorque.f[2] = _roTorque2.f[2];
		_roTorque.f[3] = _roTorque2.f[3];

		_roLvel.f[0]	 = _roLvel2.f[0];
		_roLvel.f[1]	 = _roLvel2.f[1];
		_roLvel.f[2]	 = _roLvel2.f[2];
		_roLvel.f[3]	 = _roLvel2.f[3];

		_roAvel.f[0]	 = _roAvel2.f[0];
		_roAvel.f[1]	 = _roAvel2.f[1];
		_roAvel.f[2]	 = _roAvel2.f[2];
		_roAvel.f[3]	 = _roAvel2.f[3];
	#endif

	PrepareSolveLCP(_m);
}

void VUmain (void)
{
}