#ifndef __WARNING__H
#define __WARNING__H


/*=============================================================================
 *
 * globals / functions to manage a text buffer, that is display at the end of the 
 * import as a report. There's function to write in buffer
 * 
 *=============================================================================*/


class CWarning
{
    TSTR m_p_cBuffer;                       // pointer on buffer allocated by fn_vInitTextBuffer

    bool m_bWarning;
    bool m_bError;

public:
    CWarning();                             // Allocate and initialize the main buffer
    ~CWarning();                            // Free the buffer
    void Reset();                           // Resets the buffer
    void AddString( const TCHAR* szString );    // Add a string to the buffer
    void Print( const TCHAR* szFormat, ... );   // Add characters and values to the buffer
    void PrintWarning( const TCHAR* szFormat, ... );   // Add characters and values to the buffer
    void PrintError( const TCHAR* szFormat, ... );   // Add characters and values to the buffer
    const TCHAR* GetBuffer() const;             // Get the buffer pointer

    inline bool HasWarnings() const {return m_bWarning;}
    inline bool HasErrors() const {return m_bError;}
};


#endif // __WARNING__H
