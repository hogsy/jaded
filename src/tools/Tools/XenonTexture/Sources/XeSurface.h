// ------------------------------------------------------------------------------------------------
// File   : XeSurface.h
// Date   : 2005-07-29
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

#ifndef GUARD_XESURFACE_H
#define GUARD_XESURFACE_H

// ------------------------------------------------------------------------------------------------
// CLASSES
// ------------------------------------------------------------------------------------------------
class XeSurface
{
public:

    XeSurface(BYTE* _pbySurface, DWORD _dwWidth, DWORD _dwHeight);
    ~XeSurface(void);

    void Flip(void);
    void Normalize(void);
    void Resize(DWORD _dwWidth, DWORD _dwHeight);

    inline const void* GetRawData(void) const { return m_pRawSurface; }

private:

    D3DXCOLOR* m_pRawSurface;
    DWORD      m_dwWidth;
    DWORD      m_dwHeight;
};

#endif // #ifdef GUARD_XESURFACE_H
