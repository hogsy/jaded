#include "StdAfx.h"
#include "GAS_Group.h"

using namespace GAS;


CGroup::CGroup( const char* name, unsigned int numReservedObjects, unsigned long flags,
                int numAnimatableBones ) :
m_flags( flags ),
m_data( name ),
m_numAnimatableBones( numAnimatableBones )
{
    if (numReservedObjects)
        m_objects.reserve( numReservedObjects );
}

CGroup::~CGroup()
{
}

bool CGroup::Read( HANDLE hFile, unsigned short version, unsigned short subVersion )
{
    if (version >= 1)
    {
        if (!TRead( hFile, m_flags ))
            return false;
    }
    else
    {
        m_flags = 0;
    }

    if (!TRead( hFile, m_data ))
        return false;

    if (HasNumAnimatableBones())
    {
        if (!TRead( hFile, m_numAnimatableBones ))
            return false;
    }
    else
    {
        m_numAnimatableBones = -1;
    }

    if (m_data.numObjects)
    {
        m_objects.resize( m_data.numObjects );

        DWORD read;
        if (!ReadFile( hFile, &m_objects[0], m_data.numObjects*sizeof(GAS_Str), &read, NULL ))
            return false;
        if (read!=(m_data.numObjects*sizeof(GAS_Str)))
            return false;

        if (HasObjectParents())
        {
            m_objectParents.resize( m_data.numObjects, -1 );
            if (!ReadFile( hFile, &m_objectParents[0], m_data.numObjects*sizeof(int), &read, NULL ))
                return false;
            if (read!=(m_data.numObjects*sizeof(int)))
                return false;
        }

        if (HasObjectFlags())
        {
            m_objectFlags.resize( m_data.numObjects, 0 );
            if (!ReadFile( hFile, &m_objectFlags[0], m_data.numObjects*sizeof(unsigned long), &read, NULL ))
                return false;
            if (read!=(m_data.numObjects*sizeof(unsigned long)))
                return false;
        }

        if (HasObjectInitLocalPos())
        {
            m_objectInitLocalPos.resize( m_data.numObjects*3, 0 );
            if (!ReadFile( hFile, &m_objectInitLocalPos[0], m_data.numObjects*3*sizeof(float), &read, NULL ))
                return false;
            if (read!=(m_data.numObjects*3*sizeof(float)))
                return false;
        }
    }

    return true;
}

bool CGroup::Write( HANDLE hFile ) const
{
    if (!TWrite( hFile, m_flags ))
        return false;

    if (!TWrite( hFile, m_data ))
        return false;

    if (HasNumAnimatableBones())
    {
        if (!TWrite( hFile, m_numAnimatableBones ))
            return false;
    }

    if (m_data.numObjects)
    {
        DWORD written;
        if (!WriteFile( hFile, &m_objects[0], m_data.numObjects*sizeof(GAS_Str), &written, NULL ))
            return false;
        if (written!=(m_data.numObjects*sizeof(GAS_Str)))
            return false;

        if (HasObjectParents())
        {
            if (!WriteFile( hFile, &m_objectParents[0], m_data.numObjects*sizeof(int), &written, NULL ))
                return false;
            if (written!=(m_data.numObjects*sizeof(int)))
                return false;
        }

        if (HasObjectFlags())
        {
            if (!WriteFile( hFile, &m_objectFlags[0], m_data.numObjects*sizeof(unsigned long), &written, NULL ))
                return false;
            if (written!=(m_data.numObjects*sizeof(unsigned long)))
                return false;
        }

        if (HasObjectInitLocalPos())
        {
            if (!WriteFile( hFile, &m_objectInitLocalPos[0], m_data.numObjects*3*sizeof(float), &written, NULL ))
                return false;
            if (written!=(m_data.numObjects*3*sizeof(float)))
                return false;
        }
    }

    return true;
}

const GAS_GroupData& CGroup::GetData() const
{
    return m_data;
}

const char* CGroup::GetObject( unsigned int i ) const
{
    return m_objects[i].str;
}

bool CGroup::HasObjectParents() const
{
    return (m_flags&GAS_GROUP_HAS_OBJECT_PARENTS)!=0;
}

int CGroup::GetObjectParent( unsigned int i ) const
{
    return m_objectParents[i];
}

bool CGroup::HasObjectFlags() const
{
    return (m_flags&GAS_GROUP_HAS_OBJECT_FLAGS)!=0;
}

unsigned long CGroup::GetObjectFlags( unsigned int i ) const
{
    return m_objectFlags[i];
}

bool CGroup::HasNumAnimatableBones() const
{
    return (m_flags&GAS_GROUP_HAS_NUM_ANIMATABLE_BONES)!=0;
}

int CGroup::GetNumAnimatableBones() const
{
    return m_numAnimatableBones;
}

bool CGroup::HasObjectInitLocalPos() const
{
    return (m_flags&GAS_GROUP_HAS_OBJECT_INIT_LOCAL_POS)!=0;
}

const float* CGroup::GetObjectInitLocalPos( unsigned int i ) const
{
    return &m_objectInitLocalPos[i*3];
}

void CGroup::AppendObject( const char* name, int parent, unsigned long flags, const float* initLocalPos )
{
    m_data.numObjects++;
    m_objects.push_back( name );

    if (HasObjectParents())
        m_objectParents.push_back( parent );

    if (HasObjectFlags())
        m_objectFlags.push_back( flags );

    if (HasObjectInitLocalPos())
    {
        m_objectInitLocalPos.push_back(initLocalPos[0]);
        m_objectInitLocalPos.push_back(initLocalPos[1]);
        m_objectInitLocalPos.push_back(initLocalPos[2]);
    }
}
