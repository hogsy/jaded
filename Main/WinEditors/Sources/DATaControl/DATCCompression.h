//------------------------------------------------------------------------------
//   DAT_CCompression.h
/// \author    YCharbonneau
/// \date      2005-01-07
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef _DAT_CCOMPRESSION_H__
#define _DAT_CCOMPRESSION_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "zlib\zlib.h"
//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------
//  Name   :   DAT_CCompression
/// \author    YCharbonneau
/// \date      2005-01-18
/// \par       Description: 
///            No description available ...
/// \see 
//------------------------------------------------------------
class DAT_CCompression
{
	   //--------------------------------------------------------------------------
	   // public definitions 
	   //--------------------------------------------------------------------------
	
public:
	    DAT_CCompression ();
		virtual ~DAT_CCompression ();
		
		//--------------------------------------------------------------------------
		// protected definitions
		//--------------------------------------------------------------------------
		//------------------------------------------------------------------------------
		#define MAX_LEN 256
		static void Compress(const Byte *Src,Byte **Dst,ULONG SrcLen,ULONG *DstLen)
		{
			int err;
			*DstLen = SrcLen * 10;
			if ( *DstLen < MAX_PATH ) 
			{
				*Dst = new Byte[ MAX_PATH ];
				*DstLen = MAX_PATH;
			}
			else
			{
				*Dst = new Byte[ *DstLen ];
			}

			memset(*Dst,0,*DstLen);
			err = compress(*Dst, DstLen, Src, SrcLen);
			assert((err == Z_OK) && "Compression error, buffer not written");
		}

		static void Uncompress(char *Src,UCHAR **Dst,ULONG SrcLen,ULONG *DstLen)
		{
			*DstLen = SrcLen * 20;
			*Dst = new UCHAR[*DstLen];

			int err;
			err = uncompress(*Dst, DstLen, (Byte*)Src, SrcLen);
			while ( err == Z_BUF_ERROR ) 
			{
				delete [] (*Dst);
				
				*DstLen *= 10;
				// not enough space

				*Dst = new UCHAR[*DstLen];
				err = uncompress(*Dst, DstLen, (Byte*)Src, SrcLen);
			}

			if ( err == Z_DATA_ERROR ) 
			{
				delete [] (*Dst);
				*Dst = NULL;
				*DstLen = -1;
			}

			assert((err == Z_OK) && "Decompression error, buffer not read");
		}
protected:
	
	   //--------------------------------------------------------------------------
	   // private definitions
	   //--------------------------------------------------------------------------
	
private:
	
};

//------------------------------------------------------------------------------

#endif //#ifndef _DAT_CCOMPRESSION_H__




