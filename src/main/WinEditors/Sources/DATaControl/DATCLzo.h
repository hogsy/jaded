//------------------------------------------------------------------------------
//   DATCLzo.h
/// \author    YCharbonneau
/// \date      2005-01-07
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef _DAT_CLZO_H__
#define _DAT_CLZO_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "BIGfiles\minilzo.h"
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
		static void Compress(UCHAR *Src,UCHAR **Dst,ULONG SrcLen,ULONG *DstLen)
		{
			int iResult;
			ULONG ulProcessLen = 0;
			UCHAR* pProcessCursor = Src;
			lzo_uint uiLen =  SrcLen > MAX_LEN ? MAX_LEN : SrcLen;
			lzo_uint out_len = SrcLen;
			
			static StrBuf buffer;
			buffer.Clear();
				
			char tmpchar[LZO1X_MEM_COMPRESS];
			

			while ( ulProcessLen < SrcLen )  
			{
				UCHAR* pOut = new UCHAR[uiLen];
				iResult = lzo1x_1_compress(pProcessCursor,uiLen,pOut,&out_len,tmpchar);


				if (iResult == LZO_E_OK)
				{
					buffer.Append((char*)pOut,out_len);
				}
				else
				{
					/* this should NEVER happen */
					assert(0 && "internal error - compression failed");
				}
				//delete[] pOut;
			
				ulProcessLen += uiLen;
				uiLen =  (SrcLen - ulProcessLen)  > MAX_LEN ? MAX_LEN : (SrcLen - ulProcessLen);
			}

			*Dst = new UCHAR[ buffer.Length() ] ;
			*DstLen = buffer.Length();
			memcpy(*Dst,buffer.Value(),buffer.Length());
			buffer.Clear();
		}

		static void Uncompress(UCHAR *Src,UCHAR **Dst,ULONG SrcLen,ULONG *DstLen)
		{
			//	L_memset(s_WorkMemory, 0, workmemlen);	
			//	lzo1x_decompress(Src, SrcLen,Dst,DstLen,s_WorkMemory);
			unsigned int out_len = SrcLen*20;
			lzo_init () ;
			int iResult = LZO_E_OK;
			
			ULONG ulProcessLen = 0;
			UCHAR* pProcessCursor = Src;
			lzo_uint uiLen =  SrcLen > MAX_LEN ? MAX_LEN : SrcLen;

			*Dst = new UCHAR[out_len];
			memset(*Dst,0,out_len);
			
			while ( ulProcessLen < SrcLen )  
			{

				iResult = lzo1x_decompress(Src, uiLen, *Dst, (lzo_uint*)&out_len, NULL);
				if ( iResult == LZO_E_OK || iResult == LZO_E_INPUT_NOT_CONSUMED ) 
				{
					Dst += out_len;
					Src += uiLen;
					ulProcessLen += uiLen;
					uiLen =  (SrcLen - ulProcessLen)  > MAX_LEN ? MAX_LEN : (SrcLen - ulProcessLen);				
				}

				if ( iResult == LZO_E_OUTPUT_OVERRUN )  
				{
					out_len *= 2;
					delete []*Dst;
				}
				
			}			
		}
protected:
	
	   //--------------------------------------------------------------------------
	   // private definitions
	   //--------------------------------------------------------------------------
	
private:
	
};

//------------------------------------------------------------------------------

#endif //#ifndef _DAT_CLZO_H__




