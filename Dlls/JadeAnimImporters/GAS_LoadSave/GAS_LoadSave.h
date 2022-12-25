#ifndef __GAS_LOADSAVE__H
#define __GAS_LOADSAVE__H


#include "GAS_Interface.h"
#include "GAS_Group.h"
#include "GAS_Skin.h"
#include <vector>

namespace GAS {

class CLoadSave : public ILoadSave
{
public:
    CLoadSave();
    virtual ~CLoadSave();

    virtual bool Load( const TCHAR* filename );
    virtual bool Save( const TCHAR* filename ) const;
    virtual const GAS_LoadSaveData& GetData() const;
    virtual const IGroup& GetGroup( unsigned int i ) const;
    virtual const ISkin& GetSkin( unsigned int i ) const;

    virtual IGroup& AppendGroup( const char* name, unsigned int numReservedObjects,
        unsigned long flags, int numAnimatableBones );
    virtual ISkin& AppendSkin( const char* name, unsigned int numVertAsss, unsigned int numReservedGizmoBones );

private:
    bool Read( HANDLE hFile );
    bool Write( HANDLE hFile ) const;
    
    GAS_LoadSaveData m_data;

    std::vector<CGroup> m_groups;
    std::vector<CSkin> m_skins;
};

}

#endif // __GAS_LOADSAVE__H
