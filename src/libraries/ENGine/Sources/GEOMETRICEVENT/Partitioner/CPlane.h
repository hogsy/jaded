//------------------------------------------------------------------------
//
// (C) Copyright 2001 Ubisoft
//
// Author		Stephane Girard
// Date			20 Dec 2001
//
// File			CPlane.h
// Description
//
//------------------------------------------------------------------------
#ifndef __CPLANE_H__INCLUDED
#define __CPLANE_H__INCLUDED

//#ifdef USE_COL_EVENTOBJECT

class CPlane
{
public:
	// Variables.
    FLOAT X;
    FLOAT Y;
    FLOAT Z;
	FLOAT W;

	// Constructors.
	CPlane()
	{}
	~CPlane()
	{}
	CPlane( const CPlane& P )
	:	X(P.X), Y(P.Y), Z(P.Z), W(P.W)
	{}
	CPlane( MATH_tdst_Vector *  _pN )
	:	X(_pN->x), Y(_pN->y), Z(_pN->z), W(0)
	{}

	CPlane( float InX, float InY, float InZ, float InW )
	:	X(InX), Y(InY), Z(InZ), W(InW)
	{}
	CPlane( MATH_tdst_Vector * _pN, float InW )
	:	X(_pN->x), Y(_pN->y), Z(_pN->z), W(InW)
	{}

    /*
	CPlane( FVector InBase, const FVector &InNormal )
	:	FVector(InNormal)
	,	W(InBase | InNormal)
	{}
	CPlane( FVector A, FVector B, FVector C )
	:	FVector( ((B-A)^(C-A)).SafeNormal() )
	,	W( A | ((B-A)^(C-A)).SafeNormal() )
	{}
    */

	// Functions.
	FLOAT PlaneDot( MATH_tdst_Vector * _pP ) const
	{
		return X*_pP->x + Y*_pP->y + Z*_pP->z - W;
	}
	CPlane Flip() const
	{
		return CPlane(-X,-Y,-Z,-W);
	}

    CPlane TransformBy(MATH_tdst_Matrix & _stM)
    {
        CPlane             P;
        MATH_tdst_Vector * pN0;
        MATH_tdst_Vector * pN1;
        MATH_tdst_Vector   V;

        pN0 = (MATH_tdst_Vector *)&X;
        pN1 = (MATH_tdst_Vector *)&(P.X);
        MATH_TransformVector(pN1, &_stM, pN0);
        MATH_ScaleVector(&V, pN1, W);
        MATH_TransformVertex(&V, &_stM, &V);
        P.W = MATH_f_DotProduct(&V, pN1);

        return P;
    }

    MATH_tdst_Vector * GetNormal() { return (MATH_tdst_Vector*) &X; }

	//CPlane TransformPlaneByOrtho( const FCoords &Coords ) const;
	//CPlane TransformBy( const FCoords &Coords ) const;

	BOOL operator==( const CPlane& V ) const
	{
		return X==V.X && Y==V.Y && Z==V.Z && W==V.W;
	}
	BOOL operator!=( const CPlane& V ) const
	{
		return X!=V.X || Y!=V.Y || Z!=V.Z || W!=V.W;
	}
	CPlane operator+( const CPlane& V ) const
	{
		return CPlane( X + V.X, Y + V.Y, Z + V.Z, W + V.W );
	}
	CPlane operator-( const CPlane& V ) const
	{
		return CPlane( X - V.X, Y - V.Y, Z - V.Z, W - V.W );
	}
	CPlane operator/( float Scale ) const
	{
		float RScale = 1.f/Scale;
		return CPlane( X * RScale, Y * RScale, Z * RScale, W * RScale );
	}
	CPlane operator*( float Scale ) const
	{
		return CPlane( X * Scale, Y * Scale, Z * Scale, W * Scale );
	}
	CPlane operator*( const CPlane& V )
	{
		return CPlane ( X*V.X,Y*V.Y,Z*V.Z,W*V.W );
	}
	CPlane operator+=( const CPlane& V )
	{
		X += V.X; Y += V.Y; Z += V.Z; W += V.W;
		return *this;
	}
	CPlane operator-=( const CPlane& V )
	{
		X -= V.X; Y -= V.Y; Z -= V.Z; W -= V.W;
		return *this;
	}
	CPlane operator*=( float Scale )
	{
		X *= Scale; Y *= Scale; Z *= Scale; W *= Scale;
		return *this;
	}
	CPlane operator*=( const CPlane& V )
	{
		X *= V.X; Y *= V.Y; Z *= V.Z; W *= V.W;
		return *this;
	}
	CPlane operator/=( float V )
	{
		float RV = 1.f/V;
		X *= RV; Y *= RV; Z *= RV; W *= RV;
		return *this;
	}
};

//#endif // #ifdef USE_COL_EVENTOBJECT

#endif // !defined(__CPLANE_H__INCLUDED)
