#ifndef __CMAXTRANSFO__H
#define __CMAXTRANSFO__H

#define INVALIDTRANS			-999
#define UNINIT					-999
#define VALIDTRANS				1
#define NMAX					2000

/* 
Definition of CMaxTransfo Class
*/
class CMaxTransfo 
{
public:
	CMaxTransfo();
	CMaxTransfo(CMaxTransfo*);
	CMaxTransfo(int,int,Quat,Point3);
	~CMaxTransfo();

	void setTranslation(Point3);
	void setParentOffset(Point3);
	void setRotation(Quat);
	void setBone(int);
	void setTransfoTime(int);

	Point3	getTranslation();
	Point3	getParentOffset();
	Quat	getRotation();	
	int		getBone();
	int		getTransfoTime();
	Matrix3 getTransfoMatrix();

	int		ValidTranslation();
	int		ValidRotation();
	void	InterpolateRotation(CMaxTransfo*, CMaxTransfo*);	
	void	InterpolateTranslation(CMaxTransfo*, CMaxTransfo*);

private:
	Point3	pTranslate;				// Translation apply for this transfo
	Point3	pParentOffset;			// Offset from the parent
	Quat	qRotation;				// Rotation apply for this transfo
	int		iBone;					// Bone of the transformation
	int		iTransfoTime;			// Time of the transformation
};




#endif