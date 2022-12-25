#ifndef __GAS_SKIN__H
#define __GAS_SKIN__H


#include "GAS_Interface.h"
#include <vector>

namespace GAS {


class CSkin : public ISkin
{
public:
    CSkin() {}
    CSkin( const char* name, unsigned int numVertAsss, unsigned int numReservedGizmoBones );
    virtual ~CSkin();

    bool Read( HANDLE hFile );
    bool Write( HANDLE hFile ) const;

    virtual const GAS_SkinData& GetData() const;
    virtual unsigned int GetGizmoBone( unsigned int i ) const;
    virtual const GAS_VertAss& GetVertAss( unsigned int i ) const;

    virtual void AppendGizmoBone( unsigned int iBone );
    virtual GAS_VertAss& VertAss( unsigned int i );

private:
    GAS_SkinData                m_data;
    std::vector<int>            m_gizmoBones;
    std::vector<GAS_VertAss>    m_vertAsss;
};


}

#endif // __GAS_SKIN__H
