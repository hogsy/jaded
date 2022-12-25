//------------------------------------------------------------------------
//
#ifndef INCLUDED_BAS_CRC32_H
#define INCLUDED_BAS_CRC32_H

class BAS_CRC32
{
public:
	BAS_CRC32() { Init_CRC32_Table(); }

	ULONG Get_CRC(void* pBuffer, ULONG size);  // Creates a CRC from a text string 

    ULONG ComputeCRC32LittleEndian(void* _pBuffer, ULONG _ulNbDWORDs);

private:
	ULONG crc32_table[256];  // Lookup table array 

	void Init_CRC32_Table();  // Builds lookup table array 
	ULONG Reflect(ULONG ref, CHAR ch);  // Reflects CRC bits in the lookup table 
};
	
extern BAS_CRC32 BAS_g_CRC32;

#endif //#ifndef INCLUDED_CRC32_H

