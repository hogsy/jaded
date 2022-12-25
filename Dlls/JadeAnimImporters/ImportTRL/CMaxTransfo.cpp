#include "StdAfx.h"
#include "CMaxTransfo.h"

// Constructor
CMaxTransfo::CMaxTransfo()
{
	iBone = UNINIT;
	iTransfoTime = UNINIT;
}

// Constructor
CMaxTransfo::CMaxTransfo(int a_iNewBone, int a_iNewTime, Quat a_Rotation, Point3 a_Translation)
{
	iBone = a_iNewBone;
	iTransfoTime = a_iNewTime;
	qRotation = a_Rotation;
	pTranslate = a_Translation;	
	pParentOffset.x = 0;
	pParentOffset.y = 0;
	pParentOffset.z = 0;
}

// Constructor copy
CMaxTransfo::CMaxTransfo(CMaxTransfo* a_CopyTransfo)
{
	iBone = a_CopyTransfo->getBone();
	iTransfoTime = a_CopyTransfo->getTransfoTime();
	qRotation = a_CopyTransfo->getRotation();
	pTranslate = a_CopyTransfo->getTranslation();	
	pParentOffset = a_CopyTransfo->getParentOffset();	
}


// Destructor 
CMaxTransfo::~CMaxTransfo()
{
}

void CMaxTransfo::setTranslation(Point3 a_NewTranslation)
{
	pTranslate = a_NewTranslation;
}

void CMaxTransfo::setParentOffset(Point3 a_NewParentOffset)
{
	pParentOffset = a_NewParentOffset;
}

void CMaxTransfo::setRotation(Quat a_NewRotation)
{
	qRotation = a_NewRotation;
}

void CMaxTransfo::setBone(int a_NewBone)
{
	iBone = a_NewBone;
}

void CMaxTransfo::setTransfoTime(int a_NewTime)
{
	iTransfoTime = a_NewTime;
}

Point3	CMaxTransfo::getTranslation()
{	
	return pTranslate;
}

Point3	CMaxTransfo::getParentOffset()
{	
	return pParentOffset;
}

Quat	CMaxTransfo::getRotation()
{
	return qRotation;
}

int		CMaxTransfo::getBone()
{
	return iBone;
}
int		CMaxTransfo::getTransfoTime()
{
	return iTransfoTime;
}

Matrix3		CMaxTransfo::getTransfoMatrix()
{
	Matrix3 TransfoMatrix;

	TransfoMatrix.IdentityMatrix();
	if (ValidRotation() == VALIDTRANS)
	{
		qRotation.MakeMatrix(TransfoMatrix);
	}
	if (ValidTranslation() == VALIDTRANS)
	{
		TransfoMatrix.SetRow(3,pTranslate);
	}

	return TransfoMatrix;
}

int		CMaxTransfo::ValidTranslation()
{
	if (pTranslate.x == INVALIDTRANS && pTranslate.y == INVALIDTRANS 
		&& pTranslate.z == INVALIDTRANS)
	{
		return INVALIDTRANS;
	}
	
	return VALIDTRANS;
}

int		CMaxTransfo::ValidRotation()
{
	if (qRotation.x == INVALIDTRANS && qRotation.y == INVALIDTRANS 
		&& qRotation.z == INVALIDTRANS && qRotation.w == INVALIDTRANS)
	{
		return INVALIDTRANS;
	}

	return VALIDTRANS;
}

void	CMaxTransfo::InterpolateRotation(CMaxTransfo* a_FirstTransfo, CMaxTransfo* a_SecTransfo)
{
	Quat FirstQuat;
	Quat SecQuat;
	Quat InterpolQuat;
	float interpolationFactor;

	FirstQuat = a_FirstTransfo->getRotation();
	SecQuat = a_SecTransfo->getRotation();
	
	interpolationFactor = ((float)iTransfoTime - (float)a_FirstTransfo->getTransfoTime())
		/ ((float)a_SecTransfo->getTransfoTime() - (float)a_FirstTransfo->getTransfoTime());

	qRotation = Slerp(FirstQuat,SecQuat,interpolationFactor);
}

void	CMaxTransfo::InterpolateTranslation(CMaxTransfo* a_FirstTransfo, CMaxTransfo* a_SecTransfo)
{	
	float	Slope, Orig, midX, midY, midZ;
	
	Point3	firstKeyTrans = a_FirstTransfo->getTranslation();
	Point3	secKeyTrans = a_SecTransfo->getTranslation();

	Slope = (secKeyTrans[0] - firstKeyTrans[0]) / (float)(a_SecTransfo->getTransfoTime() - a_FirstTransfo->getTransfoTime());
	Orig = secKeyTrans[0] - (Slope * a_SecTransfo->getTransfoTime());
	midX = Slope * (float)iTransfoTime + Orig;

	Slope = (secKeyTrans[1] - firstKeyTrans[1]) / (float)(a_SecTransfo->getTransfoTime() - a_FirstTransfo->getTransfoTime());
	Orig = secKeyTrans[1] - (Slope * a_SecTransfo->getTransfoTime());
	midY = Slope * (float)iTransfoTime + Orig;

	Slope = (secKeyTrans[2] - firstKeyTrans[2]) / (float)(a_SecTransfo->getTransfoTime() - a_FirstTransfo->getTransfoTime());
	Orig = secKeyTrans[2] - (Slope * a_SecTransfo->getTransfoTime());
	midZ = Slope * (float)iTransfoTime + Orig;

	pTranslate.Set(midX, midY, midZ);
}