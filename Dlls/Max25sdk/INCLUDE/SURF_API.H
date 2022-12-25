/**********************************************************************
 *<
    FILE: surf_api.h

    DESCRIPTION:  Provides the SDK api for NURBS surfaces

    CREATED BY: Charlie Thaeler

    HISTORY: created 15 April, 1997

 *> Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/

#ifndef SURF_API_H
#define SURF_API_H
#include "point3.h"
#include "maxtess.h"


#define EDITABLE_SURF_CLASS_ID Class_ID(0x76a11646, 0x12a822fb)
#define FITPOINT_PLANE_CLASS_ID Class_ID(0x76a11646, 0xbadbeff)
#define EDITABLE_CVCURVE_CLASS_ID Class_ID(0x76a11646, 0x12a83145)
#define EDITABLE_FPCURVE_CLASS_ID Class_ID(0x76a11646, 0x12a92143)

#define NURBS_NAME_SIZE 80


typedef unsigned long NURBSId;  // NOTE: THESE ARE NOT PERSISTANT ACROSS SESSIONS
								//       and should NOT be saved to a file.


typedef Tab<NURBSId> NURBSIdTab;
typedef Tab<BOOL> BoolTab;

enum NURBSResult {
	kNOk,
	kNInvalidObject,
	kNInvalidId,
	kNInvalidParameter,
	kNBad
};

enum NURBSMirrorAxis {
	kMirrorX,
	kMirrorY,
	kMirrorZ,
	kMirrorXY,
	kMirrorXZ,
	kMirrorYZ
};

enum NURBSConstType {
	kNConstOnObject,
	kNConstOffset,
	kNConstNormal,
	kNConstTangent
};

enum NURBSKind {
	kNURBSPoint,
	kNURBSCV,
	kNURBSCurve,
	kNURBSSurface
};

enum NURBSType {
	kNPoint,
	kNPointCPoint, // constrained points
	kNCurveCPoint,
	kNCurveCurveIntersectionPoint,
	kNSurfaceCPoint,
	kNCurveSurfaceIntersectionPoint,

	kNCV,

	kNCVCurve,
	kNPointCurve,
	kNBlendCurve,
	kNOffsetCurve,
	kNXFormCurve,
	kNMirrorCurve,
	kNFilletCurve,
	kNChamferCurve,
	kNIsoCurve,
	kNProjectVectorCurve,
	kNProjectNormalCurve,
	kNSurfSurfIntersectionCurve,
	kNCurveOnSurface,
	kNPointCurveOnSurface,
	kNSurfaceNormalCurve,

	kNCVSurface,
	kNPointSurface,
	kNBlendSurface,
	kNOffsetSurface,
	kNXFormSurface,
	kNMirrorSurface,
	kNRuledSurface,
	kNULoftSurface,
	kNExtrudeSurface,
	kNLatheSurface,
	kNUVLoftSurface,
	kNNBlendSurface,
	kN1RailSweepSurface,
	kN2RailSweepSurface,
	kNCapSurface,
	kNMultiCurveTrimSurface
};


class NURBSSet;

class NURBSObject {
protected:
	TCHAR mName[NURBS_NAME_SIZE];
	NURBSType mType;
	NURBSKind mKind;
	NURBSId mId;
	Object *mpObject;
	NURBSSet* mpNSet;
	BOOL mSelected;
public:
	DllExport NURBSObject(void);
	DllExport virtual ~NURBSObject(void);
	DllExport NURBSObject & operator=(const NURBSObject& pt);
	DllExport void SetName(TCHAR *name);
	DllExport TCHAR *GetName(void);
	DllExport NURBSType GetType();
	DllExport NURBSKind GetKind();
	DllExport NURBSId GetId();
	DllExport void SetId(NURBSId id);
	DllExport void SetNSet(NURBSSet *nset);
	DllExport void SetObject(Object *object);
	DllExport Object* GetObject();
	DllExport NURBSSet* GetNSet();
	DllExport int GetIndex();
	DllExport BOOL IsSelected();
	DllExport void SetSelected(BOOL set);
};


class NURBSPoint : public NURBSObject {
protected:
	double mX, mY, mZ;
public:
	DllExport NURBSPoint();
	DllExport virtual Point3 GetPosition(TimeValue t);
	DllExport virtual void GetPosition(TimeValue t, float& x, float& y, float& z);
	DllExport virtual void GetPosition(TimeValue t, double& x, double& y, double& z);
};


class NURBSIndependentPoint : public NURBSPoint {
public:
	DllExport NURBSIndependentPoint(void);
	DllExport virtual ~NURBSIndependentPoint(void);
	DllExport NURBSIndependentPoint & operator=(const NURBSIndependentPoint& pt);
	DllExport BOOL operator==(const NURBSIndependentPoint& pt);
	DllExport BOOL operator!=(const NURBSIndependentPoint& pt);
	DllExport void SetPosition(TimeValue t, Point3 pt);
	DllExport void SetPosition(TimeValue t, float x, float y, float z);
	DllExport void SetPosition(TimeValue t, double x, double y, double z);

};


class NURBSControlVertex : public NURBSObject {
	double mX, mY, mZ;
	double mW;  // weight
public:
	DllExport NURBSControlVertex(void);
	DllExport virtual ~NURBSControlVertex(void);
	DllExport NURBSControlVertex & operator=(const NURBSControlVertex& cv);
	DllExport BOOL operator==(const NURBSControlVertex& cv);
	DllExport BOOL operator!=(const NURBSControlVertex& cv);
	DllExport void SetPosition(TimeValue t, Point3 pt);
	DllExport void SetPosition(TimeValue t, float x, float y, float z);
	DllExport void SetPosition(TimeValue t, double x, double y, double z);
	DllExport Point3 GetPosition(TimeValue t);
	DllExport void GetPosition(TimeValue t, float& x, float& y, float& z);
	DllExport void GetPosition(TimeValue t, double& x, double& y, double& z);
	DllExport void SetWeight(TimeValue t, float wt);
	DllExport void SetWeight(TimeValue t, double wt);
	DllExport void GetWeight(TimeValue t, float& wt);
	DllExport double GetWeight(TimeValue t);
	DllExport void GetWeight(TimeValue t, double& wt);
};


class NURBSPointConstPoint : public NURBSPoint {
protected:
	NURBSId mParentId;
	int mParentIndex;
	NURBSConstType mCType;
	Point3 mOffset;

public:
	DllExport NURBSPointConstPoint(void);
	DllExport virtual ~NURBSPointConstPoint(void);
	DllExport NURBSPointConstPoint & operator=(const NURBSPointConstPoint& pt);
	DllExport void SetParent(int index);
	DllExport void SetParentId(NURBSId id);
	DllExport int GetParent(void);
	DllExport NURBSId GetParentId(void);
	DllExport void SetPointType(NURBSConstType type);
	DllExport NURBSConstType GetPointType(void);
	DllExport void SetOffset(TimeValue t, Point3 pt);
	DllExport Point3 GetOffset(TimeValue t);
};

class NURBSCurveConstPoint : public NURBSPoint {
protected:
	NURBSId mParentId;
	int mParentIndex;
	NURBSConstType mCType;
	Point3 mOffset;
	float mNormal;
	float mUTangent;
	double mUParam;
    BOOL mTrimCurve;
    BOOL mFlipTrim;

public:
	DllExport NURBSCurveConstPoint(void);
	DllExport virtual ~NURBSCurveConstPoint(void);
	DllExport NURBSCurveConstPoint & operator=(const NURBSCurveConstPoint& pt);
	DllExport void SetParent(int index);
	DllExport void SetParentId(NURBSId id);
	DllExport int GetParent(void);
	DllExport NURBSId GetParentId(void);
	DllExport void SetPointType(NURBSConstType type);
	DllExport NURBSConstType GetPointType(void);
	DllExport void SetOffset(TimeValue t, Point3 pt);
	DllExport Point3 GetOffset(TimeValue t);
	DllExport void SetUParam(TimeValue t, double param);
	DllExport double GetUParam(TimeValue t);
	DllExport void SetNormal(TimeValue t, float dist);
	DllExport float GetNormal(TimeValue t);
	DllExport void SetUTangent(TimeValue t, float dist);
	DllExport float GetUTangent(TimeValue t);
    DllExport BOOL GetTrimCurve();
    DllExport void SetTrimCurve(BOOL trim);
    DllExport BOOL GetFlipTrim();
    DllExport void SetFlipTrim(BOOL flip);
};

class NURBSCurveCurveIntersectionPoint : public NURBSPoint {
protected:
	NURBSId mParentId[2];
	int mParentIndex[2];
    double mCurveParam[2];
    BOOL mTrimCurve[2];
    BOOL mFlipTrim[2];

public:
	DllExport NURBSCurveCurveIntersectionPoint(void);
	DllExport virtual ~NURBSCurveCurveIntersectionPoint(void);
	DllExport NURBSCurveCurveIntersectionPoint & operator=(const NURBSCurveCurveIntersectionPoint &pt);
    DllExport void SetCurveParam(int curveNum, double param);
    DllExport double GetCurveParam(int curveNum);
	DllExport void SetParent(int pnum, int index);
	DllExport void SetParentId(int pnum, NURBSId id);
	DllExport int GetParent(int pnum);
	DllExport NURBSId GetParentId(int pnum);

    DllExport BOOL GetTrimCurve(int pnum);
    DllExport void SetTrimCurve(int pnum, BOOL trim);
    DllExport BOOL GetFlipTrim(int pnum);
    DllExport void SetFlipTrim(int pnum, BOOL flip);
};

class NURBSSurfConstPoint : public NURBSPoint {
protected:
	NURBSId mParentId;
	int mParentIndex;
	NURBSConstType mCType;
	Point3 mOffset;
	float mNormal;
	float mUTangent;
	double mUParam;
	float mVTangent;
	double mVParam;
public:
	DllExport NURBSSurfConstPoint(void);
	DllExport virtual ~NURBSSurfConstPoint(void);
	DllExport NURBSSurfConstPoint & operator=(const NURBSSurfConstPoint& pt);
	DllExport void SetParent(int index);
	DllExport void SetParentId(NURBSId id);
	DllExport int GetParent(void);
	DllExport NURBSId GetParentId(void);
	DllExport void SetPointType(NURBSConstType type);
	DllExport NURBSConstType GetPointType(void);
	DllExport void SetUParam(TimeValue t, double param);
	DllExport double GetUParam(TimeValue t);
	DllExport void SetVParam(TimeValue t, double param);
	DllExport double GetVParam(TimeValue t);
	DllExport void SetOffset(TimeValue t, Point3 pt);
	DllExport Point3 GetOffset(TimeValue t);
	DllExport void SetNormal(TimeValue t, float dist);
	DllExport float GetNormal(TimeValue t);
	DllExport void SetUTangent(TimeValue t, float dist);
	DllExport float GetUTangent(TimeValue t);
	DllExport void SetVTangent(TimeValue t, float dist);
	DllExport float GetVTangent(TimeValue t);
};

class NURBSCurveSurfaceIntersectionPoint : public NURBSPoint {
protected:
	// parent 0 should be the surface parent 1 should be the curve
	NURBSId mParentId[2];
	int mParentIndex[2];
	double mSeed;
    BOOL mTrimCurve;
    BOOL mFlipTrim;

public:
	DllExport NURBSCurveSurfaceIntersectionPoint(void);
	DllExport virtual ~NURBSCurveSurfaceIntersectionPoint(void);
	DllExport NURBSCurveSurfaceIntersectionPoint & operator=(const NURBSCurveSurfaceIntersectionPoint &pt);
    DllExport void SetSeed(double seed);
    DllExport double GetSeed();
	DllExport void SetParent(int pnum, int index);
	DllExport void SetParentId(int pnum, NURBSId id);
	DllExport int GetParent(int pnum);
	DllExport NURBSId GetParentId(int pnum);

    DllExport BOOL GetTrimCurve();
    DllExport void SetTrimCurve(BOOL trim);
    DllExport BOOL GetFlipTrim();
    DllExport void SetFlipTrim(BOOL flip);
};





typedef Tab<NURBSControlVertex> NURBSCVTab;
typedef Tab<double> NURBSKnotTab;



enum NURBSTrimDirection // defines the side to Keep
{
    kNone = 0,
    kPositive = 1,
    kNegative = 2
};


class NURBSTrimPoint {
public:
    DllExport NURBSTrimPoint(double parameter, NURBSTrimDirection direction) :
        mParameter(parameter), mDirection(direction) {}
    DllExport double GetParameter() {return mParameter; }
    DllExport NURBSTrimDirection GetDirection() {return mDirection; }

private:
    double mParameter;
    NURBSTrimDirection mDirection;
};



class NURBSCurve : public NURBSObject {
protected:
	friend class NURBSCVCurve;
	friend class NURBSPointCurve;
	friend class NURBSBlendCurve;
	friend class NURBSOffsetCurve;
	friend class NURBSXFormCurve;
	friend class NURBSMirrorCurve;
	friend class NURBSFilletCurve;
	friend class NURBSChamferCurve;
	friend class NURBSIsoCurve;
	friend class NURBSProjectVectorCurve;
	friend class NURBSProjectNormalCurve;
	friend class NURBSSurfaceNormalCurve;
	friend class NURBSNBlendSurface;
	friend class NURBSRuledSurface;
	friend class NURBSULoftSurface;
	friend class NURBSUVLoftSurface;
	friend class NURBSExtrudeSurface;
	friend class NURBSLatheSurface;
	friend class NURBSCapSurface;
	friend class NURBS1RailSweepSurface;
	friend class NURBS2RailSweepSurface;
	friend class NURBSMultiCurveTrimSurface;

	void* mCache;
	Interval mCacheValid;

	virtual void BuildCache(TimeValue t) = 0;
	void ClearCache();
public:
	DllExport NURBSCurve(void);
	DllExport virtual ~NURBSCurve(void);
	DllExport NURBSCurve & operator=(const NURBSCurve& curve);
	DllExport BOOL IsClosed(void);

    DllExport int NumTrimPoints();
    DllExport NURBSTrimPoint GetTrimPoint(TimeValue t, int i);
    
	DllExport BOOL Evaluate(TimeValue t, double u, Point3& pt, Point3& tangent);
	DllExport void GetParameterRange(TimeValue t, double& uMin, double& uMax);
	DllExport BOOL GetNURBSData(TimeValue t,
								int& degree,
								int& numCVs,
								NURBSCVTab& cvs,
								int& numKnots,
								NURBSKnotTab knots);
};


class NURBSCVCurve : public NURBSCurve {
protected:
	NURBSControlVertex *mpCVs;
	double *mpKnots;
	BOOL mClosed;
	int mOrder;
	int mNumKnots;
	int mNumCVs;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSCVCurve(void);
	DllExport virtual ~NURBSCVCurve(void);
	DllExport NURBSCVCurve & operator=(const NURBSCVCurve& curve);
	DllExport void Close(void);
	DllExport void SetOrder(int order);
	DllExport int GetOrder(void);
	DllExport void SetNumKnots(int num);         // data is NOT maintained
	DllExport int GetNumKnots(void);
	DllExport void SetNumCVs(int num);           // data is NOT maintained
	DllExport void GetNumCVs(int& num);
	DllExport int GetNumCVs(void);
	DllExport double GetKnot(int index);
	DllExport void SetKnot(int index, double value);
	DllExport NURBSControlVertex* GetCV(int index);
	DllExport void SetCV(int index, NURBSControlVertex &cv);
	DllExport void SetTransformMatrix(TimeValue t, SetXFormPacket& xPack);
	DllExport Matrix3 GetTransformMatrix(TimeValue t);

	DllExport void EndsOverlap(BOOL& overlap);
	DllExport void Refine(TimeValue t, double u); // looses animation
	DllExport void Insert(TimeValue t, double u);
};


class NURBSPointCurve : public NURBSCurve {
protected:
	NURBSIndependentPoint *mpPts;
	BOOL mClosed;
	int mNumPts;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSPointCurve(void);
	DllExport virtual ~NURBSPointCurve(void);
	DllExport NURBSPointCurve & operator=(const NURBSPointCurve& curve);
	DllExport void Close(void);
	DllExport void SetNumPts(int num);       // data is NOT maintained
	DllExport int GetNumPts(void);
	DllExport void GetNumPts(int &num);
	DllExport NURBSIndependentPoint* GetPoint(int index);
	DllExport void SetPoint(int index, NURBSIndependentPoint &pt);
	DllExport void SetTransformMatrix(TimeValue t, SetXFormPacket& xPack);
	DllExport Matrix3 GetTransformMatrix(TimeValue t);
	DllExport void Refine(TimeValue t, double u); // looses animation
};


class NURBSBlendCurve : public NURBSCurve {
	NURBSId mParentId[2];
	int mParentIndex[2];
	BOOL mParentEnd[2];
	double mTension[2];
	void BuildCache(TimeValue t);
public:
	DllExport NURBSBlendCurve(void);
	DllExport virtual ~NURBSBlendCurve(void);
	DllExport NURBSBlendCurve & operator=(const NURBSBlendCurve& curve);
	DllExport void SetParent(int pnum, int index);
	DllExport void SetParentId(int pnum, NURBSId id);
	DllExport int GetParent(int pnum);
	DllExport NURBSId GetParentId(int pnum);
	DllExport void SetEnd(int pnum, BOOL end);
	DllExport BOOL GetEnd(int pnum);
	DllExport void SetTension(TimeValue t, int pnum, double ten);
	DllExport double GetTension(TimeValue t, int pnum);
};


class NURBSOffsetCurve : public NURBSCurve {
	NURBSId mParentId;
	int mParentIndex;
	double mDistance;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSOffsetCurve(void);
	DllExport virtual ~NURBSOffsetCurve(void);
	DllExport NURBSOffsetCurve & operator=(const NURBSOffsetCurve& curve);
	DllExport void SetParent(int index);
	DllExport void SetParentId(NURBSId id);
	DllExport int GetParent(void);
	DllExport NURBSId GetParentId(void);
	DllExport void SetDistance(TimeValue t, double d);
	DllExport double GetDistance(TimeValue t);
};

class NURBSXFormCurve : public NURBSCurve {
	NURBSId mParentId;
	int mParentIndex;
	Matrix3 mXForm;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSXFormCurve(void);
	DllExport virtual ~NURBSXFormCurve(void);
	DllExport NURBSXFormCurve & operator=(const NURBSXFormCurve& curve);
	DllExport void SetParent(int index);
	DllExport void SetParentId(NURBSId id);
	DllExport int GetParent(void);
	DllExport NURBSId GetParentId(void);
	DllExport void SetXForm(TimeValue t, Matrix3& mat);
	DllExport Matrix3& GetXForm(TimeValue t);
};

class NURBSMirrorCurve : public NURBSCurve {
	NURBSId mParentId;
	int mParentIndex;
	NURBSMirrorAxis mAxis;
	Matrix3 mXForm;
	double mDistance;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSMirrorCurve(void);
	DllExport virtual ~NURBSMirrorCurve(void);
	DllExport NURBSMirrorCurve & operator=(const NURBSMirrorCurve& curve);
	DllExport void SetParent(int index);
	DllExport void SetParentId(NURBSId id);
	DllExport int GetParent(void);
	DllExport NURBSId GetParentId(void);
	DllExport void SetAxis(NURBSMirrorAxis axis);
	DllExport NURBSMirrorAxis GetAxis(void);
	DllExport void SetXForm(TimeValue t, Matrix3& mat);
	DllExport Matrix3& GetXForm(TimeValue t);
	DllExport void SetDistance(TimeValue t, double d);
	DllExport double GetDistance(TimeValue t);
};


class NURBSFilletCurve : public NURBSCurve {
	NURBSId mParentId[2];
	int mParentIndex[2];
	BOOL mParentEnd[2];
	double mRadius;
    BOOL mTrimCurve[2];
    BOOL mFlipTrim[2];

	void BuildCache(TimeValue t);
public:
	DllExport NURBSFilletCurve(void);
	DllExport virtual ~NURBSFilletCurve(void);
	DllExport NURBSFilletCurve & operator=(const NURBSFilletCurve& curve);
	DllExport void SetParent(int pnum, int index);
	DllExport void SetParentId(int pnum, NURBSId id);
	DllExport int GetParent(int pnum);
	DllExport NURBSId GetParentId(int pnum);
	DllExport void SetEnd(int pnum, BOOL end);
	DllExport BOOL GetEnd(int pnum);
	DllExport void SetRadius(TimeValue t, double radius);
	DllExport double GetRadius(TimeValue t);

    DllExport BOOL GetTrimCurve(int pnum);
    DllExport void SetTrimCurve(int pnum, BOOL trim);
    DllExport BOOL GetFlipTrim(int pnum);
    DllExport void SetFlipTrim(int pnum, BOOL flip);
};


class NURBSChamferCurve : public NURBSCurve {
	NURBSId mParentId[2];
	int mParentIndex[2];
	BOOL mParentEnd[2];
	double mLength[2];
    BOOL mTrimCurve[2];
    BOOL mFlipTrim[2];

	void BuildCache(TimeValue t);
public:
	DllExport NURBSChamferCurve(void);
	DllExport virtual ~NURBSChamferCurve(void);
	DllExport NURBSChamferCurve & operator=(const NURBSChamferCurve& curve);
	DllExport void SetParent(int pnum, int index);
	DllExport void SetParentId(int pnum, NURBSId id);
	DllExport int GetParent(int pnum);
	DllExport NURBSId GetParentId(int pnum);
	DllExport void SetEnd(int pnum, BOOL end);
	DllExport BOOL GetEnd(int pnum);
	DllExport void SetLength(TimeValue t, int pnum, double length);
	DllExport double GetLength(TimeValue t, int pnum);

    DllExport BOOL GetTrimCurve(int pnum);
    DllExport void SetTrimCurve(int pnum, BOOL trim);
    DllExport BOOL GetFlipTrim(int pnum);
    DllExport void SetFlipTrim(int pnum, BOOL flip);
};


class NURBSIsoCurve : public NURBSCurve {
	NURBSId mParentId;
	int mParentIndex;
	BOOL mIsU;  // false for V...
	double mParam;
	BOOL mTrim;
	BOOL mFlipTrim;
	Point2 mSeed;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSIsoCurve(void);
	DllExport virtual ~NURBSIsoCurve(void);
	DllExport NURBSIsoCurve & operator=(const NURBSIsoCurve& curve);
	DllExport void SetParent(int index);
	DllExport void SetParentId(NURBSId id);
	DllExport int GetParent(void);
	DllExport NURBSId GetParentId(void);
	DllExport void SetDirection(BOOL isU);
	DllExport BOOL GetDirection(void);
	DllExport void SetParam(TimeValue t, double p);
	DllExport double GetParam(TimeValue t);
	DllExport BOOL GetTrim();
	DllExport void SetTrim(BOOL trim);
	DllExport BOOL GetFlipTrim();
	DllExport void SetFlipTrim(BOOL flip);
	DllExport Point2 GetSeed();
	DllExport void SetSeed(Point2& seed);
};


class NURBSProjectVectorCurve : public NURBSCurve {
	// parent 0 should be the surface parent 1 should be the curve
	NURBSId mParentId[2];
	int mParentIndex[2];
	BOOL mTrim;
	BOOL mFlipTrim;
	Point2 mSeed;
	Point3 mPVec;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSProjectVectorCurve(void);
	DllExport virtual ~NURBSProjectVectorCurve(void);
	DllExport NURBSProjectVectorCurve & operator=(const NURBSProjectVectorCurve& curve);
	DllExport void SetParent(int pnum, int index);
	DllExport void SetParentId(int pnum, NURBSId id);
	DllExport int GetParent(int pnum);
	DllExport NURBSId GetParentId(int pnum);
	DllExport BOOL GetTrim();
	DllExport void SetTrim(BOOL trim);
	DllExport BOOL GetFlipTrim();
	DllExport void SetFlipTrim(BOOL flip);
	DllExport Point2 GetSeed();
	DllExport void SetSeed(Point2& seed);
	DllExport void SetPVec(TimeValue t, Point3& pvec); // projection vector
	DllExport Point3& GetPVec(TimeValue t);
};



class NURBSProjectNormalCurve : public NURBSCurve {
	// parent 0 should be the surface parent 1 should be the curve
	NURBSId mParentId[2];
	int mParentIndex[2];
	BOOL mTrim;
	BOOL mFlipTrim;
	Point2 mSeed;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSProjectNormalCurve(void);
	DllExport virtual ~NURBSProjectNormalCurve(void);
	DllExport NURBSProjectNormalCurve & operator=(const NURBSProjectNormalCurve& curve);
	DllExport void SetParent(int pnum, int index);
	DllExport void SetParentId(int pnum, NURBSId id);
	DllExport int GetParent(int pnum);
	DllExport NURBSId GetParentId(int pnum);
	DllExport BOOL GetTrim();
	DllExport void SetTrim(BOOL trim);
	DllExport BOOL GetFlipTrim();
	DllExport void SetFlipTrim(BOOL flip);
	DllExport Point2 GetSeed();
	DllExport void SetSeed(Point2& seed);
};


class NURBSSurfSurfIntersectionCurve : public NURBSCurve {
	// parent 0 should be the surface parent 1 should be the curve
	NURBSId mParentId[2];
	int mParentIndex[2];
	BOOL mTrim[2];
	BOOL mFlipTrim[2];
	Point2 mSeed;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSSurfSurfIntersectionCurve(void);
	DllExport virtual ~NURBSSurfSurfIntersectionCurve(void);
	DllExport NURBSSurfSurfIntersectionCurve & operator=(const NURBSSurfSurfIntersectionCurve& curve);
	DllExport void SetParent(int pnum, int index);
	DllExport void SetParentId(int pnum, NURBSId id);
	DllExport int GetParent(int pnum);
	DllExport NURBSId GetParentId(int pnum);
	DllExport BOOL GetTrim(int tnum);
	DllExport void SetTrim(int tnum, BOOL trim);
	DllExport BOOL GetFlipTrim(int tnum);
	DllExport void SetFlipTrim(int tnum, BOOL flip);
	DllExport Point2 GetSeed();
	DllExport void SetSeed(Point2& seed);
};



class NURBSCurveOnSurface : public NURBSCVCurve {
	NURBSId mParentId;
	int mParentIndex;
	BOOL mTrim;
	BOOL mFlipTrim;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSCurveOnSurface(void);
	DllExport virtual ~NURBSCurveOnSurface(void);
	DllExport NURBSCurveOnSurface & operator=(const NURBSCurveOnSurface& curve);
	DllExport void SetParent(int index);
	DllExport void SetParentId(NURBSId id);
	DllExport int GetParent();
	DllExport NURBSId GetParentId();
	DllExport BOOL GetTrim();
	DllExport void SetTrim(BOOL trim);
	DllExport BOOL GetFlipTrim();
	DllExport void SetFlipTrim(BOOL flip);
};

class NURBSPointCurveOnSurface : public NURBSPointCurve {
	NURBSId mParentId;
	int mParentIndex;
	BOOL mTrim;
	BOOL mFlipTrim;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSPointCurveOnSurface(void);
	DllExport virtual ~NURBSPointCurveOnSurface(void);
	DllExport NURBSPointCurveOnSurface & operator=(const NURBSPointCurveOnSurface& curve);
	DllExport void SetParent(int index);
	DllExport void SetParentId(NURBSId id);
	DllExport int GetParent();
	DllExport NURBSId GetParentId();
	DllExport BOOL GetTrim();
	DllExport void SetTrim(BOOL trim);
	DllExport BOOL GetFlipTrim();
	DllExport void SetFlipTrim(BOOL flip);
};


class NURBSSurfaceNormalCurve : public NURBSCurve {
	NURBSId mParentId;
	int mParentIndex;
	double mDistance;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSSurfaceNormalCurve(void);
	DllExport virtual ~NURBSSurfaceNormalCurve(void);
	DllExport NURBSSurfaceNormalCurve & operator=(const NURBSSurfaceNormalCurve& curve);
	DllExport void SetParent(int index);
	DllExport void SetParentId(NURBSId id);
	DllExport int GetParent();
	DllExport NURBSId GetParentId();
	DllExport void SetDistance(TimeValue t, double dist);
	DllExport double GetDistance(TimeValue t);
};







// NOTE: we only use the CVs internally at this point
// Texture surfaces, internally, are cubic with uniform
// knot vectors.  Anything else set will be ignored.
class NURBSTextureSurface {
	BOOL mDefCLP;
	NURBSControlVertex *mpCVs;
	double *mpUKnots;
	double *mpVKnots;
	int mUOrder;
	int mVOrder;
	int mNumUCVs;
	int mNumVCVs;
	int mNumUKnots;
	int mNumVKnots;
	void* mCache;
	Interval mCacheValid;
	void BuildCache(TimeValue t);
	void ClearCache();
public:
	DllExport NURBSTextureSurface(void);
	DllExport ~NURBSTextureSurface(void);
	DllExport NURBSTextureSurface & operator=(const NURBSTextureSurface& surf);
	DllExport BOOL IsCLP();
	DllExport void SetCLP(BOOL clp);
	DllExport void SetUOrder(int order);
	DllExport int GetUOrder(void);
	DllExport int GetVOrder(void);
	DllExport void SetVOrder(int order);
	DllExport void SetNumUKnots(int num);
	DllExport void SetNumVKnots(int num);
	DllExport int GetNumUKnots(void);
	DllExport int GetNumVKnots(void);
	DllExport void SetNumCVs(int u, int v);
	DllExport int GetNumUCVs(void);
	DllExport int GetNumVCVs(void);
	DllExport void GetNumCVs(int &u, int &v);
	DllExport double GetUKnot(int index);
	DllExport double GetVKnot(int index);
	DllExport void SetUKnot(int index, double value);
	DllExport void SetVKnot(int index, double value);
	DllExport NURBSControlVertex* GetCV(int u, int v);
	DllExport void SetCV(int u, int v, NURBSControlVertex& cv);
};


class NURBSSurface : public NURBSObject {
protected:
	friend class NURBSCVSurface;
	friend class NURBSPointSurface;
	friend class NURBSBlendSurface;
	friend class NURBSNBlendSurface;
	friend class NURBSOffsetSurface;
	friend class NURBSXFormSurface;
	friend class NURBSMirrorSurface;
	friend class NURBSCapSurface;
	friend class NURBSIsoCurve;
	friend class NURBSProjectVectorCurve;
	friend class NURBSProjectNormalCurve;
	friend class NURBSSurfSurfIntersectionCurve;
	friend class NURBSCurveOnSurface;
	friend class NURBSPointCurveOnSurface;
	friend class NURBSMultiCurveTrimSurface;

	BOOL mGenUVs[2];
	BOOL mFlipNormals;
	BOOL mRenderable;
	int mMatID;
	Point2 mTexUVs[2][4];
	float mUTile[2], mVTile[2], mUOffset[2], mVOffset[2];
	BOOL mClosedInU, mClosedInV;

	void* mCache;
	Interval mCacheValid;
	virtual void BuildCache(TimeValue t) = 0;
	void ClearCache();
public:
	NURBSTextureSurface mTexSurface[2];

	DllExport NURBSSurface(void);
	DllExport virtual ~NURBSSurface(void);
	DllExport NURBSSurface & operator=(const NURBSSurface& surf);
	DllExport BOOL Renderable();
	DllExport void Renderable(BOOL state);
	DllExport BOOL FlipNormals();
	DllExport void FlipNormals(BOOL state);
	DllExport BOOL GenerateUVs(int channel = 0);
	DllExport void SetGenerateUVs(BOOL state, int channel = 0);
	DllExport int MatID();
	DllExport void MatID(int id);
	DllExport Point2 GetTextureUVs(TimeValue t, int i, int channel = 0);
	DllExport void SetTextureUVs(TimeValue t, int i, Point2 pt, int channel = 0);
	DllExport void GetTileOffset(TimeValue t, float &ut, float &vt, float &uo, float &vo, int channel = 0);
	DllExport void SetTileOffset(TimeValue t, float ut, float vt, float uo, float vo, int channel = 0);
	DllExport BOOL IsClosedInU(void);
	DllExport BOOL IsClosedInV(void);

	DllExport BOOL Evaluate(TimeValue t, double u, double v, Point3& pt,
			Point3& dPdU, Point3& dPdV);
	DllExport BOOL Evaluate(TimeValue t, double u, double v, Point3& pt,
			Point3& dPdU, Point3& dPdV, Point3& d2PdU2, Point3& d2PdV2, Point3& d2PdUdV);
	DllExport void GetParameterRange(TimeValue t, double& uMin, double& uMax, double& vMin, double& vMax);
	DllExport BOOL GetNURBSData(TimeValue t,
								int& degreeInU,
								int& degreeInV,
								int& numInU,
								int& numInV,
								NURBSCVTab& cvs,
								int& numKnotsInU,
								int& numKnotsInV,
								NURBSKnotTab uKnots, 
								NURBSKnotTab vKnots);
	DllExport BOOL GetCLPTextureSurfaceData(TimeValue t, int channel,
								int& degreeInU,
								int& degreeInV,
								int& numInU,
								int& numInV,
								NURBSCVTab& cvs,
								int& numKnotsInU,
								int& numKnotsInV,
								NURBSKnotTab uKnots, 
								NURBSKnotTab vKnots);
	DllExport int NumTrimLoops(TimeValue t);
	DllExport int NumCurvesInLoop(TimeValue t, int loop);
	DllExport BOOL Get2dTrimCurveData(TimeValue t, int loop, int curve,
										int& degree,
										int& numCVs,
										NURBSCVTab& cvs,
										int& numKnots,
										NURBSKnotTab knots);
	DllExport BOOL Get3dTrimCurveData(TimeValue t, int loop, int curve,
										int& degree,
										int& numCVs,
										NURBSCVTab& cvs,
										int& numKnots,
										NURBSKnotTab knots);
};


class NURBSCVSurface : public NURBSSurface {
	NURBSControlVertex *mpCVs;
	double *mpUKnots;
	double *mpVKnots;
	int mUOrder;
	int mVOrder;
	int mNumUCVs;
	int mNumVCVs;
	int mNumUKnots;
	int mNumVKnots;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSCVSurface(void);
	DllExport virtual ~NURBSCVSurface(void);
	DllExport NURBSCVSurface & operator=(const NURBSCVSurface& surf);
	DllExport void CloseInU(void);
	DllExport void CloseInV(void);
	DllExport void SetUOrder(int order);
	DllExport int GetUOrder(void);
	DllExport int GetVOrder(void);
	DllExport void SetVOrder(int order);
	DllExport void SetNumUKnots(int num);         // data is NOT maintained
	DllExport void SetNumVKnots(int num);         // data is NOT maintained
	DllExport int GetNumUKnots(void);
	DllExport int GetNumVKnots(void);
	DllExport void SetNumCVs(int u, int v);       // data is NOT maintained
	DllExport int GetNumUCVs(void);
	DllExport int GetNumVCVs(void);
	DllExport void GetNumCVs(int &u, int &v);
	DllExport double GetUKnot(int index);
	DllExport double GetVKnot(int index);
	DllExport void SetUKnot(int index, double value);
	DllExport void SetVKnot(int index, double value);
	DllExport NURBSControlVertex* GetCV(int u, int v);
	DllExport void SetCV(int u, int v, NURBSControlVertex& cv);

	DllExport void SetTransformMatrix(TimeValue t, SetXFormPacket& mat);
	DllExport Matrix3 GetTransformMatrix(TimeValue t);

	DllExport void EdgesOverlap(BOOL& uOverlap, BOOL& vOverlap);
	// If you refine in U (U_V_Both = 0) you must specify v
	// If you refine in V (U_V_Both = 1) you must specify u
	// If you refine in U and (U_V_Both = -1) you must specify u and v
	DllExport void Refine(TimeValue t, double u, double v, int U_V_Both);
	DllExport void Insert(TimeValue t, double u, double v, int U_V_Both);
};


class NURBSPointSurface : public NURBSSurface {
	NURBSIndependentPoint *mpPts;
	int mNumUPts;
	int mNumVPts;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSPointSurface(void);
	DllExport virtual ~NURBSPointSurface(void);
	DllExport NURBSPointSurface & operator=(const NURBSPointSurface& surf);
	DllExport void CloseInU(void);
	DllExport void CloseInV(void);
	DllExport void SetNumPts(int u, int v);       // data is NOT maintained
	DllExport int GetNumUPts(void);
	DllExport int GetNumVPts(void);
	DllExport void GetNumPts(int &u, int &v);
	DllExport NURBSIndependentPoint* GetPoint(int u, int v);
	DllExport void SetPoint(int u, int v, NURBSIndependentPoint& pt);

	DllExport void SetTransformMatrix(TimeValue t, SetXFormPacket& mat);
	DllExport Matrix3 GetTransformMatrix(TimeValue t);

	// If you refine in U (U_V_Both = 0) you must specify v
	// If you refine in V (U_V_Both = 1) you must specify u
	// If you refine in U and (U_V_Both = -1) you must specify u and v
	DllExport void Refine(TimeValue t, double u, double v, int U_V_Both);
};

class NURBSBlendSurface : public NURBSSurface {
	NURBSId mParentId[2];
	int mParentIndex[2];
	int mParentEdge[2];
	BOOL mFlip[2];
	double mTension[2];
	void BuildCache(TimeValue t);
public:
	DllExport NURBSBlendSurface(void);
	DllExport virtual ~NURBSBlendSurface(void);
	DllExport NURBSBlendSurface & operator=(const NURBSBlendSurface& surf);
	DllExport void SetParent(int pnum, int index);
	DllExport void SetParentId(int pnum, NURBSId id);
	DllExport int GetParent(int pnum);
	DllExport NURBSId GetParentId(int pnum);
	DllExport void SetEdge(int pnum, int edge);
	DllExport int GetEdge(int pnum);
	DllExport void SetTension(TimeValue t, int pnum, double ten);
	DllExport double GetTension(TimeValue t, int pnum);
	DllExport void SetFlip(int pnum, BOOL flip);
	DllExport BOOL GetFlip(int pnum);
};


class NURBSNBlendSurface : public NURBSSurface {
	// The parents can be either curves or surfaces (with edge IDs)
	NURBSId mParentId[4];
	int mParentIndex[4];
	int mParentEdge[4];  // used only if the parent is a surface
	void BuildCache(TimeValue t);
public:
	DllExport NURBSNBlendSurface(void);
	DllExport virtual ~NURBSNBlendSurface(void);
	DllExport NURBSNBlendSurface & operator=(const NURBSNBlendSurface& surf);
	DllExport void SetParent(int pnum, int index);
	DllExport void SetParentId(int pnum, NURBSId id);
	DllExport int GetParent(int pnum);
	DllExport NURBSId GetParentId(int pnum);
	DllExport void SetEdge(int pnum, int edge);
	DllExport int GetEdge(int pnum);
};

class NURBSOffsetSurface : public NURBSSurface {
	NURBSId mParentId;
	int mParentIndex;
	double mDistance;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSOffsetSurface(void);
	DllExport virtual ~NURBSOffsetSurface(void);
	DllExport NURBSOffsetSurface & operator=(const NURBSOffsetSurface& surf);
	DllExport void SetParent(int index);
	DllExport void SetParentId(NURBSId id);
	DllExport int GetParent(void);
	DllExport NURBSId GetParentId(void);
	DllExport void SetDistance(TimeValue t, double d);
	DllExport double GetDistance(TimeValue t);
};

class NURBSXFormSurface : public NURBSSurface {
	NURBSId mParentId;
	int mParentIndex;
	Matrix3 mXForm;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSXFormSurface(void);
	DllExport virtual ~NURBSXFormSurface(void);
	DllExport NURBSXFormSurface & operator=(const NURBSXFormSurface& surf);
	DllExport void SetParent(int index);
	DllExport void SetParentId(NURBSId id);
	DllExport int GetParent(void);
	DllExport NURBSId GetParentId(void);
	DllExport void SetXForm(TimeValue t, Matrix3& mat);
	DllExport Matrix3& GetXForm(TimeValue t);
};

class NURBSMirrorSurface : public NURBSSurface {
	NURBSId mParentId;
	int mParentIndex;
	NURBSMirrorAxis mAxis;
	Matrix3 mXForm;
	double mDistance;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSMirrorSurface(void);
	DllExport virtual ~NURBSMirrorSurface(void);
	DllExport NURBSMirrorSurface & operator=(const NURBSMirrorSurface& surf);
	DllExport void SetParent(int index);
	DllExport void SetParentId(NURBSId id);
	DllExport int GetParent(void);
	DllExport NURBSId GetParentId(void);
	DllExport void SetAxis(NURBSMirrorAxis axis);
	DllExport NURBSMirrorAxis GetAxis(void);
	DllExport void SetXForm(TimeValue t, Matrix3& mat);
	DllExport Matrix3& GetXForm(TimeValue t);
	DllExport void SetDistance(TimeValue t, double d);
	DllExport double GetDistance(TimeValue t);
};

class NURBSRuledSurface : public NURBSSurface {
	NURBSId mParentId[2];
	int mParentIndex[2];
	BOOL mFlip[2];
	void BuildCache(TimeValue t);
public:
	DllExport NURBSRuledSurface(void);
	DllExport virtual ~NURBSRuledSurface(void);
	DllExport NURBSRuledSurface & operator=(const NURBSRuledSurface& surf);
	DllExport void SetParent(int pnum, int index);
	DllExport void SetParentId(int pnum, NURBSId id);
	DllExport int GetParent(int pnum);
	DllExport NURBSId GetParentId(int pnum);
	DllExport void SetFlip(int pnum, BOOL flip);
	DllExport BOOL GetFlip(int pnum);
};


class NURBSULoftSurface : public NURBSSurface {
	Tab<NURBSId> mParentId;
	Tab<int> mParentIndex;
	Tab<BOOL> mFlip;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSULoftSurface(void);
	DllExport virtual ~NURBSULoftSurface(void);
	DllExport NURBSULoftSurface & operator=(const NURBSULoftSurface& surf);
	DllExport void SetNumCurves(int num);
	DllExport int GetNumCurves(void);
	DllExport int AppendCurve(int index, BOOL flip);
	DllExport int AppendCurve(NURBSId id, BOOL flip);
	DllExport void SetParent(int pnum, int index);
	DllExport void SetParentId(int pnum, NURBSId id);
	DllExport int GetParent(int pnum);
	DllExport NURBSId GetParentId(int pnum);
	DllExport void SetFlip(int pnum, BOOL flip);
	DllExport BOOL GetFlip(int pnum);
};


class NURBSUVLoftSurface : public NURBSSurface {
	Tab<NURBSId> mUParentId;
	Tab<int> mUParentIndex;
	Tab<NURBSId> mVParentId;
	Tab<int> mVParentIndex;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSUVLoftSurface(void);
	DllExport virtual ~NURBSUVLoftSurface(void);
	DllExport NURBSUVLoftSurface & operator=(const NURBSUVLoftSurface& surf);

	DllExport void SetNumUCurves(int num);
	DllExport int GetNumUCurves(void);
	DllExport int AppendUCurve(int index);
	DllExport int AppendUCurve(NURBSId id);
	DllExport void SetUParent(int pnum, int index);
	DllExport void SetUParentId(int pnum, NURBSId id);
	DllExport int GetUParent(int pnum);
	DllExport NURBSId GetUParentId(int pnum);

	DllExport void SetNumVCurves(int num);
	DllExport int GetNumVCurves(void);
	DllExport int AppendVCurve(int index);
	DllExport int AppendVCurve(NURBSId id);
	DllExport void SetVParent(int pnum, int index);
	DllExport void SetVParentId(int pnum, NURBSId id);
	DllExport int GetVParent(int pnum);
	DllExport NURBSId GetVParentId(int pnum);
};



class NURBSExtrudeSurface : public NURBSSurface {
	NURBSId mParentId;
	int mParentIndex;
	Point3 mEVec;
	double mDistance;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSExtrudeSurface(void);
	DllExport virtual ~NURBSExtrudeSurface(void);
	DllExport NURBSExtrudeSurface & operator=(const NURBSExtrudeSurface& surf);
	DllExport void SetParent(int index);
	DllExport void SetParentId(NURBSId id);
	DllExport int GetParent(void);
	DllExport NURBSId GetParentId(void);
	DllExport void SetEVec(TimeValue t, Point3& evec); // extrusion vector
	DllExport Point3& GetEVec(TimeValue t);
	DllExport void SetDistance(TimeValue t, double d);
	DllExport double GetDistance(TimeValue t);
};


class NURBSLatheSurface : public NURBSSurface {
	NURBSId mParentId;
	int mParentIndex;
	Matrix3 mXForm;
	double mRotation;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSLatheSurface(void);
	DllExport virtual ~NURBSLatheSurface(void);
	DllExport NURBSLatheSurface & operator=(const NURBSLatheSurface& surf);
	DllExport void SetParent(int index);
	DllExport void SetParentId(NURBSId id);
	DllExport int GetParent(void);
	DllExport NURBSId GetParentId(void);
	DllExport void SetAxis(TimeValue t, Matrix3& ray);
	DllExport Matrix3& GetAxis(TimeValue t);
	DllExport void SetRotation(TimeValue t, double degrees);
	DllExport double GetRotation(TimeValue t);
};


class NURBSCapSurface : public NURBSSurface {
	NURBSId mParentId;
	int mParentIndex;
	int mParentEdge;
	void BuildCache(TimeValue t);
public:
	DllExport NURBSCapSurface(void);
	DllExport virtual ~NURBSCapSurface(void);
	DllExport NURBSCapSurface & operator=(const NURBSCapSurface& surf);
	DllExport void SetParent(int index);
	DllExport void SetParentId(NURBSId id);
	DllExport int GetParent(void);
	DllExport NURBSId GetParentId(void);
	DllExport void SetEdge(int edge);
	DllExport int GetEdge();
};


class NURBS1RailSweepSurface : public NURBSSurface {
	NURBSId mRailId;
	int mRailIndex;
	Tab<NURBSId> mParentId;
	Tab<int> mParentIndex;
	Tab<BOOL> mFlip;
	BOOL mParallel;
	void BuildCache(TimeValue t);
public:
	DllExport NURBS1RailSweepSurface(void);
	DllExport virtual ~NURBS1RailSweepSurface(void);
	DllExport NURBS1RailSweepSurface & operator=(const NURBS1RailSweepSurface& surf);
	DllExport void SetParentRail(int index);
	DllExport void SetParentRailId(NURBSId id);
	DllExport int GetParentRail();
	DllExport NURBSId GetParentRailId();
	DllExport void SetNumCurves(int num);
	DllExport int GetNumCurves(void);
	DllExport int AppendCurve(int index, BOOL flip);
	DllExport int AppendCurve(NURBSId id, BOOL flip);
	DllExport void SetParent(int pnum, int index);
	DllExport void SetParentId(int pnum, NURBSId id);
	DllExport int GetParent(int pnum);
	DllExport NURBSId GetParentId(int pnum);
	DllExport void SetFlip(int pnum, BOOL flip);
	DllExport BOOL GetFlip(int pnum);
	DllExport void SetParallel(BOOL para);
	DllExport BOOL GetParallel();
};


class NURBS2RailSweepSurface : public NURBSSurface {
	Tab<NURBSId> mParentId;
	Tab<int> mParentIndex;
	Tab<BOOL> mFlip;
	NURBSId mRailParentId[2];
	int mRailParentIndex[2];
	BOOL mParallel;
	BOOL mScale;
	void BuildCache(TimeValue t);
public:
	DllExport NURBS2RailSweepSurface(void);
	DllExport virtual ~NURBS2RailSweepSurface(void);
	DllExport NURBS2RailSweepSurface & operator=(const NURBS2RailSweepSurface& surf);

	DllExport void SetNumCurves(int num);
	DllExport int GetNumCurves(void);
	DllExport int AppendCurve(int index, BOOL flip);
	DllExport int AppendCurve(NURBSId id, BOOL flip);
	DllExport void SetParent(int pnum, int index);
	DllExport void SetParentId(int pnum, NURBSId id);
	DllExport int GetParent(int pnum);
	DllExport NURBSId GetParentId(int pnum);
	DllExport void SetFlip(int pnum, BOOL flip);
	DllExport BOOL GetFlip(int pnum);
	DllExport void SetParallel(BOOL para);
	DllExport BOOL GetParallel();
	DllExport void SetScale(BOOL scale);
	DllExport BOOL GetScale();

	DllExport void SetRailParent(int pnum, int index);
	DllExport void SetRailParentId(int pnum, NURBSId id);
	DllExport int GetRailParent(int pnum);
	DllExport NURBSId GetRailParentId(int pnum);
};



class NURBSMultiCurveTrimSurface : public NURBSSurface {
	Tab<NURBSId> mParentId;
	Tab<int> mParentIndex;
	NURBSId mSurfaceId;
	int mSurfaceIndex;
	void BuildCache(TimeValue t);
	BOOL mFlipTrim;
public:
	DllExport NURBSMultiCurveTrimSurface(void);
	DllExport virtual ~NURBSMultiCurveTrimSurface(void);
	DllExport NURBSMultiCurveTrimSurface & operator=(const NURBSMultiCurveTrimSurface& surf);

	DllExport void SetNumCurves(int num);
	DllExport int GetNumCurves(void);
	DllExport int AppendCurve(int index);
	DllExport int AppendCurve(NURBSId id);
	DllExport void SetParent(int pnum, int index);
	DllExport void SetParentId(int pnum, NURBSId id);
	DllExport int GetParent(int pnum);
	DllExport NURBSId GetParentId(int pnum);

	DllExport void SetSurfaceParent(int index);
	DllExport void SetSurfaceParentId(NURBSId id);
	DllExport int GetSurfaceParent();
	DllExport NURBSId GetSurfaceParentId();

	DllExport BOOL GetFlipTrim();
	DllExport void SetFlipTrim(BOOL flip);
};


class NURBSDisplay {
public:
	DllExport NURBSDisplay();
	DllExport NURBSDisplay & operator=(const NURBSDisplay& disp);

	BOOL mDisplayCurves;
	BOOL mDisplaySurfaces;
	BOOL mDisplayLattices;
	BOOL mDisplaySurfCVLattices;
	BOOL mDisplayCurveCVLattices;
	BOOL mDisplayDependents;
	BOOL mDisplayTrimming;
	BOOL mDegradeOnMove;
};



class NURBSFuseSurfaceCV {
public:
	DllExport NURBSFuseSurfaceCV();
	int mSurf1, mSurf2;
	int mRow1, mCol1, mRow2, mCol2;
};


class NURBSFuseCurveCV {
public:
	DllExport NURBSFuseCurveCV();
	int mCurve1, mCurve2;
	int mCV1, mCV2;
};


class NURBSSet {
protected:
	friend DllExport Object* CreateNURBSObject(IObjParam* ip, NURBSSet *nset, Matrix3& mat);
	friend DllExport int AddNURBSObjects(Object* MAXobj, IObjParam* ip, NURBSSet *nset);
    friend DllExport BOOL GetNURBSSet(Object *object, TimeValue t, NURBSSet &nset, BOOL Relational);
	TessApprox mTessView;
	TessApprox mTessProd;
	float mTessMerge;
	Tab<NURBSObject*> mObjects;
	Object *mpObject;
	NURBSDisplay mDisplay;

public:
	DllExport NURBSSet(void);
	DllExport virtual ~NURBSSet(void);
	DllExport int GetNumObjects();
	DllExport void SetObject(int index, NURBSObject* obj);
	DllExport int AppendObject(NURBSObject* obj);
	DllExport void RemoveObject(int index);
	DllExport void DeleteObjects();
	DllExport NURBSObject* GetNURBSObject(int index);
	DllExport NURBSObject* GetNURBSObject(NURBSId id);
	DllExport TessApprox& GetProdTess();
	DllExport TessApprox& GetViewTess();
	DllExport void SetProdTess(TessApprox& tess);
	DllExport void SetViewTess(TessApprox& tess);
	DllExport float GetTessMerge();
	DllExport void SetTessMerge(float merge);
	DllExport Object* GetObject();
	DllExport NURBSDisplay GetDisplaySettings();
	DllExport void SetDisplaySettings(NURBSDisplay& disp);


	Tab<NURBSFuseSurfaceCV> mSurfFuse;
	Tab<NURBSFuseCurveCV> mCurveFuse;
};


DllExport NURBSResult GenNURBSLatheSurface(NURBSCVCurve& curve, Point3& origin, Point3& north,
									float start, float end, NURBSCVSurface& surf);
DllExport NURBSResult GenNURBSSphereSurface(float radius, Point3& center, Point3& northAxis, Point3& refAxis,
					float startAngleU, float endAngleU, float startAngleV, float endAngleV, NURBSCVSurface& surf);
DllExport NURBSResult GenNURBSCylinderSurface(float radius, float height, Point3& origin, Point3& symAxis, Point3& refAxis,
					float startAngle, float endAngle, NURBSCVSurface& surf);
DllExport NURBSResult GenNURBSConeSurface(float radius1, float radius2, float height, Point3& origin, Point3& symAxis, Point3& refAxis,
					float startAngle, float endAngle, NURBSCVSurface& surf);
DllExport NURBSResult GenNURBSTorusSurface(float majorRadius, float minorRadius, Point3& origin,
					Point3& symAxis, Point3& refAxis, float startAngleU, float endAngleU,
					float startAngleV, float endAngleV, NURBSCVSurface& surf);

DllExport Object *CreateNURBSObject(IObjParam* ip, NURBSSet *nset, Matrix3& mat);
DllExport int AddNURBSObjects(Object* obj, IObjParam* ip, NURBSSet *nset);

DllExport Object *CreateNURBSLatheShape(IObjParam* ip, TSTR name, TimeValue t, ShapeObject *shape,
                     Matrix3& axis, float degrees, int capStart, int capEnd,
                     int capType, BOOL weldCore, BOOL flipNormals, BOOL texturing, int segs, BOOL matIds);
DllExport Object *CreateNURBSExtrudeShape(IObjParam* ip, TSTR name, TimeValue t, ShapeObject *shape, float amount,
					   int capStart, int capEnd, int capType, BOOL texturing, BOOL matIds);

DllExport BOOL GetNURBSSet(Object *object, TimeValue t, NURBSSet &nset, BOOL Relational);



// modify extant objects
DllExport NURBSResult SetSurfaceApprox(Object* obj, BOOL viewport, TessApprox& tess);
DllExport NURBSResult SetSurfaceDisplaySettings(Object* obj, NURBSDisplay& disp);

DllExport NURBSResult Transform(Object* obj, NURBSIdTab& ids, SetXFormPacket& xPack, Matrix3& mat, TimeValue t);

DllExport NURBSResult BreakCurve(Object* obj, NURBSId id, double u, TimeValue t);
DllExport NURBSResult BreakSurface(Object* obj, NURBSId id, BOOL breakU, double param, TimeValue t);

DllExport NURBSResult JoinCurves(Object* obj, NURBSId id1, NURBSId id2, BOOL begin1, BOOL begin2, double tolerance, TimeValue t);
DllExport NURBSResult JoinSurfaces(Object* obj, NURBSId id1, NURBSId id2, int edge1, int edge2, double tolerance, TimeValue t);

DllExport NURBSId MakeIndependent(Object* obj, NURBSId id, TimeValue t);

#endif
