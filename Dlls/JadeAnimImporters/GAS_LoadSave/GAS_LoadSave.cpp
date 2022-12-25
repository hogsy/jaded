#include "StdAfx.h"
#include "GAS_LoadSave.h"

using namespace GAS;


HGASLOADSAVE GAS_h_Load( const TCHAR* filename )
{
    if (!filename)
        return NULL;

    ILoadSave* iLoadSave = ILoadSave::CreateInstance();
    if (!iLoadSave)
        return NULL;

    if (!iLoadSave->Load( filename ))
    {
        delete iLoadSave;
        return NULL;
    }

    return iLoadSave;
}

void GAS_v_ReleaseHandleLoadSave( HGASLOADSAVE hLoadSave )
{
    if (hLoadSave)
        delete (static_cast<const ILoadSave*>(hLoadSave));
}

const GAS_LoadSaveData* GAS_p_GetLoadSaveData( HGASLOADSAVE hLoadSave )
{
    if (!hLoadSave)
        return NULL;

    return &(static_cast<const ILoadSave*>(hLoadSave)->GetData());
}

HGASGROUP GAS_h_GetGroup( HGASLOADSAVE hLoadSave, unsigned int i )
{
    if (!hLoadSave)
        return NULL;

    return &(static_cast<const ILoadSave*>(hLoadSave)->GetGroup( i ));
}

const GAS_GroupData* GAS_p_GetGroupData( HGASGROUP hGroup )
{
    if (!hGroup)
        return NULL;

    return &(static_cast<const IGroup*>(hGroup)->GetData());
}

const char* GAS_csz_GetGroupObject( HGASGROUP hGroup, unsigned int i )
{
    if (!hGroup)
        return NULL;

    return (static_cast<const IGroup*>(hGroup)->GetObject( i ));
}

HGASSKIN GAS_h_GetSkin( HGASLOADSAVE hLoadSave, unsigned int i )
{
    if (!hLoadSave)
        return NULL;

    return &(static_cast<const ILoadSave*>(hLoadSave)->GetSkin( i ));
}

const GAS_SkinData* GAS_p_GetSkinData( HGASSKIN hSkin )
{
    if (!hSkin)
        return NULL;

    return &(static_cast<const ISkin*>(hSkin)->GetData());
}

unsigned int GAS_ui_GetSkinGizmoBone( HGASSKIN hSkin, unsigned int i )
{
    if (!hSkin)
        return NULL;

    return (static_cast<const ISkin*>(hSkin)->GetGizmoBone( i ));
}

const GAS_VertAss* GAS_p_GetSkinVertAss( HGASSKIN hSkin, unsigned int i )
{
    if (!hSkin)
        return NULL;

    return &(static_cast<const ISkin*>(hSkin)->GetVertAss( i ));
}


ILoadSave* ILoadSave::CreateInstance()
{
    return new CLoadSave;
}

ILoadSave::~ILoadSave()
{
}


CLoadSave::CLoadSave()
{
}

CLoadSave::~CLoadSave()
{
}

bool CLoadSave::Load( const TCHAR* filename )
{
    HANDLE hFile = CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if (hFile == INVALID_HANDLE_VALUE)
		return false;

    bool res = Read( hFile );
    CloseHandle( hFile );
    return res;
}

bool CLoadSave::Save( const TCHAR* filename ) const
{
    HANDLE hFile = CreateFile( filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if (hFile == INVALID_HANDLE_VALUE)
		return false;

    bool res = Write( hFile );
    CloseHandle( hFile );
    return res;
}

const GAS_LoadSaveData& CLoadSave::GetData() const
{
    return m_data;
}

const IGroup& CLoadSave::GetGroup( unsigned int i ) const
{
    return m_groups[i];
}

const ISkin& CLoadSave::GetSkin( unsigned int i ) const
{
    return m_skins[i];
}

IGroup& CLoadSave::AppendGroup( const char* name, unsigned int numReservedObjects,
                                unsigned long flags, int numAnimatableBones )
{
    m_data.numGroups++;
    m_groups.push_back( CGroup( name, numReservedObjects, flags, numAnimatableBones ) );
    return m_groups.back();
}

ISkin& CLoadSave::AppendSkin( const char* name, unsigned int numVertAsss, unsigned int numReservedGizmoBones )
{
    m_data.numSkins++;
    m_skins.push_back( CSkin( name, numVertAsss, numReservedGizmoBones ) );
    return m_skins.back();
}

bool CLoadSave::Read( HANDLE hFile )
{
    if (!TRead( hFile, m_data ))
        return false;

    if (m_data.version > GAS_CURRENT_VERSION)
        return false;

    m_groups.resize( m_data.numGroups );
    m_skins.resize( m_data.numSkins );

    unsigned int i;
    for (i=0; i<m_data.numGroups; i++)
    {
        if (!m_groups[i].Read( hFile, m_data.version, m_data.subVersion ))
            return false;
    }

    for (i=0; i<m_data.numSkins; i++)
    {
        if (!m_skins[i].Read( hFile ))
            return false;
    }

    return true;
}

bool CLoadSave::Write( HANDLE hFile ) const
{
    if (!TWrite( hFile, m_data ))
        return false;

    unsigned int i;
    for (i=0; i<m_data.numGroups; i++)
    {
        if (!m_groups[i].Write( hFile ))
            return false;
    }

    for (i=0; i<m_data.numSkins; i++)
    {
        if (!m_skins[i].Write( hFile ))
            return false;
    }

    return true;
}
