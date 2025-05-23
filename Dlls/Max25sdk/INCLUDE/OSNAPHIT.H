/**********************************************************************
 	FILE: osnaphit.h

	DESCRIPTION:  Defines the classes which are passed from the osnaps
					to the manager.

	CREATED BY: John Hutchinson
	HISTORY: December 9, 1996
	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/
#ifndef _OSNAPHIT_H_
#define _OSNAPHIT_H_

class Osnap;
class HitMesh;
class OsnapMarker;

class OsnapHit
{
	friend class OsnapManager;
	friend class Osnap;
public:
	CoreExport OsnapHit(Point3 p3, Osnap* s, int sub, HitMesh *m);
	CoreExport OsnapHit(const OsnapHit& h);
	virtual CoreExport OsnapHit& operator=(const OsnapHit& h);
	virtual CoreExport ~OsnapHit();
	virtual CoreExport OsnapHit* clone();

	void setscreendata(IPoint3 screen3, int len);

	//////////////////////////////////////////////////////////////////////
	// Display Methods
	//////////////////////////////////////////////////////////////////////
	CoreExport boolean display(ViewExp *vpt, TimeValue t, Point3 color,  \
		int markersize, boolean markers = TRUE, boolean hilite = TRUE);
	CoreExport void erase(ViewExp *vpt, TimeValue t) const; // the hit can erase itself
	CoreExport void GetViewportRect(TimeValue t,ViewExp *vpt,Rect *rect, int marksize)const;

	//////////////////////////////////////////////////////////////////////
	// Accessor Methods
	//////////////////////////////////////////////////////////////////////
	CoreExport Point3 GetHitpoint(){return hitpoint;};
	CoreExport Point3 GetWorldHitpoint(){return worldpoint;};
	CoreExport IPoint3 GetHitscreen(){return m_hitscreen;};
	CoreExport int GetSubsnap(){return subsnap;}
	CoreExport POINT GetCursor(){return m_cursor;}
	INode *GetNode(){return node;}
	void Dump()const;

	//////////////////////////////////////////////////////////////////////
	// Operator Methods
	//////////////////////////////////////////////////////////////////////
	//define comparators so we can sort a list of these
	CoreExport BOOL operator<(OsnapHit& hit);
	CoreExport BOOL operator>(OsnapHit& hit);

public:
	void Update(TimeValue t);
	CoreExport Point3 ReEvaluate(TimeValue t);


private: //data
	Point3 hitpoint; // the hit location in object space
	Point3 worldpoint; // the hit location in world space
	IPoint3 m_hitscreen; // the hit location in screen space
	POINT m_cursor;//the position of the cursor when this guy was recorded
	int m_len; //The distace from the cursor
	int m_z_depth; //The depth in z space
	BOOL m_complete; //indicates whether the screendata has been set
	Osnap*  snap; //the snap which made this hit
	int subsnap; //the subsnap index that made this hit
	OsnapMarker *m_pmarker; //a pointer to this snaps marker

	INode* node;// the node which got hit
	HitMesh *hitmesh;//a mesh used to hilite the topolgy we hit

	ViewExp *vpt;//the viewport which was active

	BOOL m_invisible;//this guy won't display itself
};

//a class to hold a list of object space points for highlighting the geometry
class HitMesh 
{
private:
	int m_numverts;
	Point3 *m_pverts;
public:
	HitMesh()
	{
		m_numverts = 0;
		m_pverts = NULL;
	}

	~HitMesh()
	{
		if(m_pverts)
		{
			delete []m_pverts;
			m_pverts = NULL;
			m_numverts = 0;
		}
	}

	Point3 operator[](int i){return m_pverts[i];}

	HitMesh(const HitMesh& h)
	{
		m_numverts = h.m_numverts;
		m_pverts = new Point3[m_numverts];
		for (int i = 0;i<m_numverts; i++)
			m_pverts[i] = h.m_pverts[i];
	}
/*
	operator=(const hitMesh &h)
	{
		m_numverts = h.m_numverts;
		m_pverts = new Point3[m_numverts];
		for (int i = 0;i<m_numverts; i++)
			m_pverts[i] = h.m_pverts[i];
	}
*/
	HitMesh(int n)
	{
		m_numverts = n;
		m_pverts = new Point3[n];
	}

	int getNumVerts(){return m_numverts;}
	void setNumVerts(int n){
		if(m_pverts != NULL)
			delete [] m_pverts;
		m_numverts = n;
		m_pverts = new Point3[n];
	}
	void setVert(int i, const Point3 &xyz){ m_pverts[i] = xyz; }	
	Point3& getVert(int i){return m_pverts[i];}
	Point3* getVertPtr(){return m_pverts;}
};
#endif //_OSNAPHIT_H