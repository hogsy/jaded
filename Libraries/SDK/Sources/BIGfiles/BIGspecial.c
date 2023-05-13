/*$T BIGspecial.c GC! 1.098 12/12/00 17:28:06 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGerrid.h"
#include "BIGfiles/BIGcomp.h"
#include "BIGfiles/BIGspecial.h"
#include "BIGfiles/minilzo.h"
#include "BIGfiles/BIGio.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

int		BIG_specialmode = 0;
int		BIG_gi_GlobalSpecialBufferSize = 0;
void	*BIG_gp_GlobalSpecialBuffer = NULL;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *BIG_p_RequestSpecialBuffer(int _i_Size)
{
	if(_i_Size > BIG_gi_GlobalSpecialBufferSize)
	{
		BIG_gi_GlobalSpecialBufferSize = _i_Size;
		if(BIG_gp_GlobalSpecialBuffer)
		{
			BIG_gp_GlobalSpecialBuffer = MEM_p_Realloc(BIG_gp_GlobalSpecialBuffer, BIG_gi_GlobalSpecialBufferSize);
		}
		else
		{
			BIG_gp_GlobalSpecialBuffer = MEM_p_Alloc(BIG_gi_GlobalSpecialBufferSize);
		}
	}

	return BIG_gp_GlobalSpecialBuffer;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_FreeSpecialBuffer(void)
{
	if(BIG_gp_GlobalSpecialBuffer) MEM_Free(BIG_gp_GlobalSpecialBuffer);
	BIG_gp_GlobalSpecialBuffer = NULL;
	BIG_gi_GlobalSpecialBufferSize = 0;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
char	adic[65535];

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *BIG_special_LZOsave(char *_pc_Buf, ULONG *_pul_Size)
{
	/*~~~~~~~~~~~~*/
	char	*pbuf;
	ULONG	ul_Size;
	/*~~~~~~~~~~~~*/

	pbuf = (char*)BIG_p_RequestSpecialBuffer(*_pul_Size + 64);
	lzo1x_1_compress((const unsigned char*)_pc_Buf, *_pul_Size, (unsigned char*)(pbuf + 12), (lzo_uint*)&ul_Size, adic);
	if(ul_Size + 12 < *_pul_Size)
	{
		*(int *) pbuf = 0xCBCBCBCB;
		((int *) pbuf)[1] = 0xC0DE6666;
		((int *) pbuf)[2] = *_pul_Size;
		*_pul_Size = ul_Size + 12;
		return pbuf;
	}

	return _pc_Buf;
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *BIG_special_LZOload(char *_pc_Buf, ULONG *_pul_Size)
{
	/*~~~~~~~~~~~~*/
	char	*pbuf;
	ULONG	ul_Size;
	/*~~~~~~~~~~~~*/

	BIG_specialmode = 0;
	if(((int *) _pc_Buf)[0] != 0xCBCBCBCB) return _pc_Buf;	/* Not compressed */
	if(((int *) _pc_Buf)[1] != 0xC0DE6666) return _pc_Buf;	/* Not compressed */
	pbuf = (char*)BIG_p_RequestSpecialBuffer(((int *) _pc_Buf)[2] + 8);
	lzo1x_decompress((const unsigned char*)_pc_Buf + 12, *_pul_Size - 12, (unsigned char*)pbuf, (lzo_uint*)&ul_Size, 0);
	*_pul_Size = ul_Size;

	return pbuf;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

static short			father[512];
static unsigned short	code[256], heap_length;
static unsigned int	    compress_charcount, file_size, heap[257];
static unsigned char	code_length[256];
static int				frequency_count[512];
static short			decomp_tree[512];

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void get_frequency_count(char *pin)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	register unsigned int	loop;
	register unsigned short	w;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(loop = 0; loop < file_size; loop++) 
	{
		w = (unsigned short) *(pin++) & 0x00FF;
		frequency_count[w]++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void reheap(unsigned short heap_entry)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	register unsigned short index;
	register unsigned short flag = 1;
	unsigned int			heap_value;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	heap_value = heap[heap_entry];

	while((heap_entry <= (heap_length >> 1)) && (flag))
	{
		index = heap_entry << 1;

		if(index < heap_length)
			if(frequency_count[heap[index]] >= frequency_count[heap[index + 1]]) index++;

		if(frequency_count[heap_value] < frequency_count[heap[index]])
			flag--;
		else
		{
			heap[heap_entry] = heap[index];
			heap_entry = index;
		}
	}

	heap[heap_entry] = heap_value;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void build_initial_heap(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	register unsigned short loop;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	heap_length = 0;

	for(loop = 0; loop < 256; loop++)
		if(frequency_count[loop]) heap[++heap_length] = (unsigned int) loop;
	for(loop = heap_length; loop > 0; loop--) reheap(loop);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void build_code_tree(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	register unsigned short findex;
	register unsigned int	heap_value;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	while(heap_length != 1)
	{
		heap_value = heap[1];
		heap[1] = heap[heap_length--];

		reheap(1);
		findex = heap_length + 255;

		frequency_count[findex] = frequency_count[heap[1]] + frequency_count[heap_value];
		father[heap_value] = findex;
		father[heap[1]] = -findex;
		heap[1] = findex;

		reheap(1);
	}

	father[256] = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned short generate_code_table(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	register unsigned short loop;
	register unsigned short current_length;
	register unsigned short current_bit;
	unsigned short			bitcode;
	short					parent;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(loop = 0; loop < 256; loop++)
	{
		if(frequency_count[loop])
		{
			current_length = bitcode = 0;
			current_bit = 1;
			parent = father[loop];

			while(parent)
			{
				if(parent < 0)
				{
					bitcode += current_bit;
					parent = -parent;
				}

				parent = father[parent];
				current_bit <<= 1;
				current_length++;
			}

			code[loop] = bitcode;

			if(current_length > 16)
				return(0);
			else
				code_length[loop] = (unsigned char) current_length;
		}
		else
			code[loop] = code_length[loop] = 0;
	}

	return(1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void compress_image(char *pin, char *pout)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	register unsigned int	thebyte = 0;
	register short			loop1;
	register unsigned short current_code;
	register unsigned int	loop;
	unsigned short			current_length, dvalue;
	unsigned int			curbyte = 0;
	short					curbit = 7;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(loop = 0L; loop < file_size; loop++)
	{
		dvalue = *(pin++) & 0x00FF;
		current_code = code[dvalue];
		current_length = (unsigned short) code_length[dvalue];

		for(loop1 = current_length - 1; loop1 >= 0; --loop1)
		{
			if((current_code >> loop1) & 1) thebyte |= (char) (1 << curbit);

			if(--curbit < 0)
			{
				*(pout++) = thebyte;
				thebyte = 0;
				curbyte++;
				curbit = 7;
			}
		}
	}

	*(pout++) = thebyte;
	compress_charcount = ++curbyte;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *BIG_special_HUFsave(char *_pc_Buf, ULONG *_pul_Size)
{
	/*~~~~~~~~~~*/
	char	*pbuf;
	int		ii;
	/*~~~~~~~~~~*/

	BIG_specialmode = 0;
	if(*_pul_Size < 12 + 516 + 256) return _pc_Buf;

	L_memset(father, 0, sizeof(father));
	L_memset(code, 0, sizeof(code));
	heap_length = 0;
	compress_charcount = 0;
	file_size = 0;
	L_memset(heap, 0, sizeof(heap));
	L_memset(code_length, 0, sizeof(code_length));
	L_memset(frequency_count, 0, sizeof(frequency_count));

	file_size = *_pul_Size;
	get_frequency_count(_pc_Buf);
	build_initial_heap();
	build_code_tree();
	if(generate_code_table())
	{
		pbuf = (char*)BIG_p_RequestSpecialBuffer(*_pul_Size * 4);
		*(int *) pbuf = 0xCBCBCBCB;
		((int *) pbuf)[1] = 0xC0DE6667;
		((int *) pbuf)[2] = *_pul_Size;

		L_memcpy(pbuf + 12, code, 512);
		L_memcpy(pbuf + 12 + 512, code_length, 256);

		compress_image(_pc_Buf, pbuf + 12 + 512 + 256);
		if(compress_charcount + 12 + 512 + 256 > *_pul_Size) return _pc_Buf;

		ii = compress_charcount + 12 + 512 + 256;

		*_pul_Size = ii;
		return pbuf;
	}

	return _pc_Buf;
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void build_decomp_tree(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	register unsigned short loop1;
	register unsigned short current_index;
	unsigned short			loop;
	unsigned short			current_node = 1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	decomp_tree[1] = 1;

	for(loop = 0; loop < 256; loop++)
	{
		if(code_length[loop])
		{
			current_index = 1;
			for(loop1 = code_length[loop] - 1; loop1 > 0; loop1--)
			{
				current_index = (decomp_tree[current_index] << 1) + ((code[loop] >> loop1) & 1);
				if(!(decomp_tree[current_index])) decomp_tree[current_index] = ++current_node;
			}

			decomp_tree[(decomp_tree[current_index] << 1) + (code[loop] & 1)] = -loop;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void decompress_image(char *pin, char *pout)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	register unsigned short cindex = 1;
	register char			curchar;
	register short			bitshift;
	unsigned int			charcount = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	while(charcount < file_size)
	{
		curchar = *(pin++);

		for(bitshift = 7; bitshift >= 0; --bitshift)
		{
			cindex = (cindex << 1) + ((curchar >> bitshift) & 1);

			if(decomp_tree[cindex] <= 0)
			{
				*(pout++) = -decomp_tree[cindex];

				if((++charcount) == file_size)
					bitshift = 0;
				else
					cindex = 1;
			}
			else
				cindex = decomp_tree[cindex];
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *BIG_special_HUFload(char *_pc_Buf, ULONG *_pul_Size)
{
	/*~~~~~~~~~~*/
	char	*pbuf;
	/*~~~~~~~~~~*/

	BIG_specialmode = 0;
	if(((int *) _pc_Buf)[0] != 0xCBCBCBCB) return _pc_Buf;	/* Not compressed */
	if(((int *) _pc_Buf)[1] != 0xC0DE6667) return _pc_Buf;	/* Not compressed */

	file_size = ((int *) _pc_Buf)[2];
	pbuf = (char*)BIG_p_RequestSpecialBuffer(file_size + 4);

	L_memset(decomp_tree, 0, sizeof(decomp_tree));
	L_memcpy(code, _pc_Buf + 8, 512);
	L_memcpy(code_length, _pc_Buf + 8 + 512, 256);

	build_decomp_tree();
	file_size = ((int *) _pc_Buf)[1];
	decompress_image(_pc_Buf + 8 + 512 + 256, pbuf);
	*_pul_Size = file_size;

	return pbuf;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG BIG_special_ComputeCheckSum(ULONG _ul_Adler, char *_pc_Buf, ULONG _ul_Size)
{
	ULONG s1 = _ul_Adler & 0xffff;
	ULONG s2 = (_ul_Adler >> 16) & 0xffff;

	ULONG n;

	for (n = 0; n < _ul_Size; n++) 
	{
		s1 = (s1 + _pc_Buf[n]) % ADLER_BASE;
		s2 = (s2 + s1) % ADLER_BASE;
	}

	return (s2 << 16) + s1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef JADEFUSION
void BIG_special_Decryption(char **ppc_Dest, char **ppc_Buf, char* _pc_Key, ULONG _ul_Size)
{
	ULONG	i, j, k;
	ULONG	ul_GlobalOffset[ENCRYPT_KEY_SIZE];
	ULONG	ul_CurrentOffset;
	UCHAR	uc_NextKeyChar;
	char	pc_Key[ENCRYPT_KEY_SIZE];

	if(!_ul_Size) return;

	/* Buffer crypté ... y'a au moins mes ENCRYPT_KEY_SIZE octets de clef dans la size .. sinon, c mort */
	ERR_X_Assert(_ul_Size >= ENCRYPT_KEY_SIZE + CRYPT_MARK_SIZE);
	ERR_X_Assert( (*(ULONG *)(*ppc_Buf) == CRYPT_MODE1_MARK) || (*(ULONG *)(*ppc_Buf) == CRYPT_MODE2_MARK));

	/* The Crypted file indicates that the key has not been encrypted inside it but no key is given	==> error */
	ERR_X_Assert(_pc_Key || (*(ULONG *)(*ppc_Buf) == CRYPT_MODE1_MARK));

	// First, we have to recreate the Key that is hidden in the crypted data ...
	if((*(ULONG *)(*ppc_Buf) == CRYPT_MODE1_MARK))
	{
		uc_NextKeyChar = (*(*ppc_Buf + CRYPT_MARK_SIZE) & 0xF0);
		if((ULONG) (uc_NextKeyChar) > (_ul_Size - ENCRYPT_KEY_SIZE - 4))
			uc_NextKeyChar = 0;
		ul_CurrentOffset = CRYPT_MARK_SIZE + (ULONG) uc_NextKeyChar & 0x000000FF;
		for(i = 0; i < ENCRYPT_KEY_SIZE; i++)
		{
			ul_GlobalOffset[i] = ul_CurrentOffset;
			pc_Key[i] = *(*ppc_Buf + ul_CurrentOffset) & 0x0F;
			uc_NextKeyChar = (UCHAR) ((*(*ppc_Buf + ul_CurrentOffset) & 0xF0) >> 4);
			ul_CurrentOffset += uc_NextKeyChar;
		}

		if(_pc_Key)
		{
			ERR_X_Assert(!L_memcmp(pc_Key, _pc_Key, ENCRYPT_KEY_SIZE));
		}
	}
	else
	{
		L_memcpy(pc_Key, _pc_Key, ENCRYPT_KEY_SIZE);
	}

	k = 0;
	j = 0;
	for(i = CRYPT_MARK_SIZE; i < _ul_Size; i++)
	{
		/* Si je tombe sur un octet correspondant à un morceau de la clef planquée 
		   dans le corps crypté, je laisse passer ... */
		if((*(ULONG *)(*ppc_Buf) == CRYPT_MODE1_MARK) && (k < ENCRYPT_KEY_SIZE) && (i == ul_GlobalOffset[k]))
		{
			k ++;
			continue;
		}
			
		BIG_special_DecryptOne(*ppc_Dest + j, *(*ppc_Buf+i), pc_Key[j % ENCRYPT_KEY_SIZE]);
		j ++;
	}

	/* Si on veut décrypter dans le même buffer, on peut diminuer la taille de ce buffer de la taille de la clé 
	   une fois le décryptage terminé */
	if(*ppc_Dest == *ppc_Buf)
	{
		*ppc_Dest = (char* )MEM_p_Realloc(*ppc_Dest, _ul_Size - ((*(ULONG *)(*ppc_Buf) == CRYPT_MODE2_MARK) ? 0 : ENCRYPT_KEY_SIZE) - CRYPT_MARK_SIZE);
		*ppc_Buf = *ppc_Dest;
	}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
#endif
 #if 0
void BIG_TestCryptage(char *asz_File)
{
	FILE	*h_File;
	ULONG	ul_Size;
	ULONG	ul_CRC[6];
	ULONG	ul_CRC_Read[ENCRYPT_CRC_SIZE / 4];
	char	*pc_Buf, *pc_Dest, *pc_Test;
	char	ac_Key[ENCRYPT_KEY_SIZE + ENCRYPT_CRC_SIZE];

	h_File = fopen(asz_File, "r");
	if(h_File)
	{
		L_fseek(h_File, 0, SEEK_END);
		ul_Size = L_ftell(h_File);
		L_fseek(h_File, 0, SEEK_SET);

		pc_Buf = MEM_p_Alloc(ul_Size);
		L_fread(pc_Buf, 1, ul_Size, h_File);

		// Je compute la clef avec aléatoirité et CRC
		BIG_ComputeKey(ac_Key, pc_Buf, ul_Size);

		// Cryptage ...
		pc_Dest = MEM_p_Alloc(ul_Size + ENCRYPT_ADD_SIZE);

		BIG_special_Encryption(&pc_Dest, &pc_Buf, ac_Key, ul_Size, TRUE);

		// Décryptage histoire de voir si c'est pareil ... 
		// La size crypté vaut ENCRYPT_KEY_SIZE + ENCRYPT_CRC_SIZE octets de plus (Clef + CRC)
		// + 4 octet de mark
		pc_Test =  MEM_p_Alloc(ul_Size);

		BIG_special_Decryption(&pc_Test, &pc_Dest, ac_Key, ul_Size + ENCRYPT_CRC_SIZE + ENCRYPT_KEY_SIZE + CRYPT_MARK_SIZE, ul_CRC_Read);

		// CheckSum du buffer reconstitué ...
		ul_CRC[0] = BIG_special_ComputeCheckSum1(1, pc_Test, ul_Size);
		ul_CRC[1] = BIG_special_ComputeCheckSum2(1, pc_Test, ul_Size);
		ul_CRC[2] = BIG_special_ComputeCheckSum3(1, pc_Test, ul_Size);
		ul_CRC[3] = BIG_special_ComputeCheckSum4(1, pc_Test, ul_Size);
		ul_CRC[4] = BIG_special_ComputeCheckSum5(1, pc_Test, ul_Size);
		ul_CRC[5] = BIG_special_ComputeCheckSum6(1, pc_Test, ul_Size);

		ERR_X_Assert(ul_CRC[0] == ul_CRC_Read[0]);
		ERR_X_Assert(ul_CRC[1] == ul_CRC_Read[1]);
		ERR_X_Assert(ul_CRC[2] == ul_CRC_Read[2]);
		ERR_X_Assert(ul_CRC[3] == ul_CRC_Read[3]);
		ERR_X_Assert(ul_CRC[4] == ul_CRC_Read[4]);
		ERR_X_Assert(ul_CRC[5] == ul_CRC_Read[5]);

		MEM_Free(pc_Dest);
		MEM_Free(pc_Buf);
		MEM_Free(pc_Test);

		fclose(h_File);
	}
}
#endif


