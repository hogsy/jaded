#include "precomp.h"

#include "BASe/BAScrc.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIxxx.h"
#include "BASe/CLIbrary/CLImem.h"

//Global instance
BAS_CRC32 BAS_g_CRC32;

void BAS_CRC32::Init_CRC32_Table() 
{// Call this function only once to initialize the CRC table. 

      // This is the official polynomial used by CRC-32 
      // in PKZip, WinZip and Ethernet. 
      ULONG ulPolynomial = 0x04c11db7; 

      // 256 values representing ASCII character codes. 
      for(ULONG i = 0; i <= 0xFF; i++) 
      { 
            crc32_table[i]=Reflect(i, 8) << 24; 
            for (ULONG j = 0; j < 8; j++) 
                  crc32_table[i] = (crc32_table[i] << 1) ^ (crc32_table[i] & (1 << 31) ? ulPolynomial : 0); 
            crc32_table[i] = Reflect(crc32_table[i], 32); 
      } 
} 

ULONG BAS_CRC32::Reflect(ULONG ref, CHAR ch) 
{// Used only by Init_CRC32_Table(). 

      ULONG value(0); 

      // Swap bit 0 for bit 7 
      // bit 1 for bit 6, etc. 
      for(LONG i = 1; i < (ch + 1); i++) 
      { 
            if(ref & 1) 
                  value |= 1 << (ch - i); 
            ref >>= 1; 
      } 
      return value; 
} 

ULONG BAS_CRC32::Get_CRC(void* pBuffer, ULONG size) 
{
      // Once the lookup table has been filled in by the two functions above, 
      // this function creates all CRCs using only the lookup table. 

      // Be sure to use unsigned variables, 
      // because negative values introduce high bits 
      // where zero bits are required. 

      // Start out with all bits set high. 
      ULONG ulCRC(0xffffffff); 
	  UCHAR* buffer = (UCHAR*)pBuffer;
      
      // Perform the algorithm on each character 
      // in the string, using the lookup table values. 
      while(size--) 
            ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ *buffer++]; 

      return ulCRC;
}

ULONG BAS_CRC32::ComputeCRC32LittleEndian(void* _pBuffer, ULONG _ulNbDWORDs)
{
#if defined(_XENON)

    // Start out with all bits set high.
    ULONG  ulCRC     = 0xffffffff;
    UCHAR* pucBuffer = (UCHAR*)_pBuffer;

    while (_ulNbDWORDs--)
    {
        ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ pucBuffer[3]];
        ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ pucBuffer[2]];
        ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ pucBuffer[1]];
        ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ pucBuffer[0]];

        pucBuffer += 4;
    }

    return ulCRC;

#else

    return Get_CRC(_pBuffer, _ulNbDWORDs * sizeof(ULONG));

#endif
}
