#ifndef __GAS_GROUP__H
#define __GAS_GROUP__H


#include "GAS_Interface.h"
#include <vector>

namespace GAS {


class CGroup : public IGroup
{
public:
    CGroup() {}
    CGroup( const char* name, unsigned int numReservedObjects, unsigned long flags, int numAnimatableBones );
    virtual ~CGroup();

    bool Read( HANDLE hFile, unsigned short version, unsigned short subVersion );
    bool Write( HANDLE hFile ) const;

    virtual const GAS_GroupData& GetData() const;
    virtual const char* GetObject( unsigned int i ) const;

    virtual bool HasObjectParents() const;
    virtual int GetObjectParent( unsigned int i ) const;

    virtual bool HasObjectFlags() const;
    virtual unsigned long GetObjectFlags( unsigned int i ) const;

    virtual bool HasNumAnimatableBones() const;
    virtual int GetNumAnimatableBones() const;

    virtual bool HasObjectInitLocalPos() const;
    virtual const float* GetObjectInitLocalPos( unsigned int i ) const;

    virtual void AppendObject( const char* name, int parent, unsigned long flags, const float* initLocalPos );

private:
    unsigned long m_flags;
    GAS_GroupData m_data;
    int m_numAnimatableBones;
    std::vector<GAS_Str> m_objects;
    std::vector<int> m_objectParents;
    std::vector<unsigned long> m_objectFlags;
    std::vector<float> m_objectInitLocalPos;
};


}

#endif // __GAS_GROUP__H
