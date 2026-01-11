#include "StdAfx.h"
#include "GAS_Skin.h"

using namespace GAS;


CSkin::CSkin( const char* name, unsigned int numVertAsss, unsigned int numReservedGizmoBones ) :
m_data( name, numVertAsss ),
m_vertAsss( numVertAsss )
{
    if (numReservedGizmoBones)
        m_gizmoBones.reserve( numReservedGizmoBones );
}

CSkin::~CSkin()
{
}

bool CSkin::Read( HANDLE hFile )
{
    if (!TRead( hFile, m_data ))
        return false;

    if (m_data.numGizmoBones)
    {
        m_gizmoBones.resize( m_data.numGizmoBones );

        DWORD read;
        if (!ReadFile( hFile, &m_gizmoBones[0], m_data.numGizmoBones*sizeof(int), &read, NULL ))
            return false;
        if (read!=(m_data.numGizmoBones*sizeof(int)))
            return false;
    }

    if (m_data.numVertAsss)
    {
        m_vertAsss.resize( m_data.numVertAsss );

        DWORD read;
        if (!ReadFile( hFile, &m_vertAsss[0], m_data.numVertAsss*sizeof(GAS_VertAss), &read, NULL ))
            return false;
        if (read!=(m_data.numVertAsss*sizeof(GAS_VertAss)))
            return false;
    }

    return true;
}

bool CSkin::Write( HANDLE hFile ) const
{
    if (!TWrite( hFile, m_data ))
        return false;

    DWORD written;

    if (m_data.numGizmoBones)
    {
        if (!WriteFile( hFile, &m_gizmoBones[0], m_data.numGizmoBones*sizeof(int), &written, NULL ))
            return false;
        if (written!=(m_data.numGizmoBones*sizeof(int)))
            return false;
    }

    if (m_data.numVertAsss)
    {
        if (!WriteFile( hFile, &m_vertAsss[0], m_data.numVertAsss*sizeof(GAS_VertAss), &written, NULL ))
            return false;
        if (written!=(m_data.numVertAsss*sizeof(GAS_VertAss)))
            return false;
    }

    return true;
}

const GAS_SkinData& CSkin::GetData() const
{
    return m_data;
}

unsigned int CSkin::GetGizmoBone( unsigned int i ) const
{
    return m_gizmoBones[i];
}

const GAS_VertAss& CSkin::GetVertAss( unsigned int i ) const
{
    return m_vertAsss[i];
}

void CSkin::AppendGizmoBone( unsigned int iBone )
{
    m_data.numGizmoBones++;
    m_gizmoBones.push_back( iBone );
}

GAS_VertAss& CSkin::VertAss( unsigned int i )
{
    return m_vertAsss[i];
}
