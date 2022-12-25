//#define VECTORC
#include "Precomp.h"

#ifndef VECTORC
	#define __VCALLMS__
#else
	#include <VU.h>
#endif

// this assumes the 4th and 8th rows of B are zero.

typedef struct tArray4OfFloats {float f [4];} Array4OfFloats;

#ifndef WIN32
	#if !defined(_XBOX) && !defined(_XENON)
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


float  _BtimesC ( short i, short j)
{
    float sum;
    sum =  JinvM[8*i+0]*_pfJ[8*j+0];
    sum += JinvM[8*i+1]*_pfJ[8*j+1];
    sum += JinvM[8*i+2]*_pfJ[8*j+2];
    sum += JinvM[8*i+4]*_pfJ[8*j+4];
    sum += JinvM[8*i+5]*_pfJ[8*j+5];
    sum += JinvM[8*i+6]*_pfJ[8*j+6];
	return sum;
}

float  _BtimesC0 ( short i)
{
    float sum;
    sum =  _pfJ[8*i+0]*oTmp1[0];
    sum += _pfJ[8*i+1]*oTmp1[1];
    sum += _pfJ[8*i+2]*oTmp1[2];
    sum += _pfJ[8*i+4]*oTmp1[4];
    sum += _pfJ[8*i+5]*oTmp1[5];
    sum += _pfJ[8*i+6]*oTmp1[6];
	return sum;
}

void ClearMatrices()
{
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
	for (j = 0; j < _m; j++ )
	{
		// linear component : Jinv1l = (1/Mass) * J1l
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

	j1 = _m;
	for (j = 0; j < _m; j++ )
	{		
		_pfJ[8*j+0] = _pfJ[8*j1+0];
		_pfJ[8*j+1] = _pfJ[8*j1+1];
		_pfJ[8*j+2] = _pfJ[8*j1+2];
		_pfJ[8*j+3] = _pfJ[8*j1+3];
		_pfJ[8*j+4] = _pfJ[8*j1+4];
		_pfJ[8*j+5] = _pfJ[8*j1+5];
		_pfJ[8*j+6] = _pfJ[8*j1+6];
		_pfJ[8*j+7] = _pfJ[8*j1+7];
		j1++;
	}

	// Inertia body 2 -> Inertia body 1	
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

	PrepareSolveLCP(_m);
}

void VUmain (void)
{
}