/*$T BIGspecial.h GC! 1.098 12/12/00 12:35:42 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "BIGfiles/BIGdefs.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

extern int	BIG_specialmode;
extern void BIG_FreeSpecialBuffer(void);

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

extern char *BIG_special_LZOload(char *, ULONG *);
extern char *BIG_special_HUFload(char *, ULONG *);


//		----- CRYPTAGE ----
#define	ENCRYPT_CRC_SIZE	6 * 4	
#define	ENCRYPT_KEY_SIZE	8				/* Puissance de 2 qu'on veut .. plus c'est gros, plus c'est gros :) */
#define	CRYPT_MARK_SIZE		4				/* ON TOUCHE PAS A CE 4 PLIZ */
#define CRYPT_MODE1_MARK	0xC0DE1BAF		/* The KEY is also encrypted in the crypted buffer */
#define CRYPT_MODE2_MARK	0xC0DE2BAF		/* The KEY is known and not encrypted*/
#define ENCRYPT_ADD_SIZE	ENCRYPT_CRC_SIZE + ENCRYPT_KEY_SIZE + CRYPT_MARK_SIZE

extern void  BIG_TestCryptage(char *);

#define ADLER_BASE			65521

/*
 =======================================================================================================================
	Adler Algo.		Start Value is 1 for Adler.
 =======================================================================================================================
 */
_inline_ ULONG BIG_special_ComputeCheckSum1(ULONG _ul_Adler, char *_pc_Buf, ULONG _ul_Size)
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
	XOR awful algo.
 =======================================================================================================================
 */
_inline_ ULONG BIG_special_ComputeCheckSum2(ULONG _ul_Start, char *_pc_Buf, ULONG _ul_Size)
{
	ULONG n, res;

	res = _ul_Start;
	for (n = 0; n < _ul_Size; n++) 
	{
		res ^= (_pc_Buf[n] << ((n % 4) * 8));
	}

	return res;
}

/*
 =======================================================================================================================
	ADD awful algo.
 =======================================================================================================================
 */
_inline_ ULONG BIG_special_ComputeCheckSum3(ULONG _ul_Start, char *_pc_Buf, ULONG _ul_Size)
{
	ULONG n, res;

	res = _ul_Start;
	for (n = 0; n < _ul_Size; n++) 
	{
		res += (_pc_Buf[n] << ((n % 4) * 8));
	}

	return res;
}
/*
 =======================================================================================================================
	ADD2 awful algo.
 =======================================================================================================================
 */

_inline_ ULONG BIG_special_ComputeCheckSum4(ULONG _ul_Start, char *_pc_Buf, ULONG _ul_Size)
{
	ULONG n, res;

	res = _ul_Start;
	for (n = 0; n < _ul_Size; n++) 
	{
		res += (_pc_Buf[n] << ((n % 3) * 8));
	}

	return res;
}

/*
 =======================================================================================================================
	XOR2 awful algo.
 =======================================================================================================================
 */
_inline_ ULONG BIG_special_ComputeCheckSum5(ULONG _ul_Start, char *_pc_Buf, ULONG _ul_Size)
{
	ULONG n, res;

	res = _ul_Start;
	for (n = 0; n < _ul_Size; n++) 
	{
		res ^= (_pc_Buf[n] << ((n % 3) * 8));
	}

	return res;
}


/*
 =======================================================================================================================
	SUB awful algo.
 =======================================================================================================================
 */
_inline_ ULONG BIG_special_ComputeCheckSum6(ULONG _ul_Start, char *_pc_Buf, ULONG _ul_Size)
{
	ULONG n, res;

	res = _ul_Start;
	for (n = 0; n < _ul_Size; n++) 
	{
		res -= (_pc_Buf[n] << ((n % 4) * 8));
	}

	return res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void BIG_special_EncryptOne(char *_pc_Dest, char _pc_Char, char _c_Key)
{
	UINT		i_CharToCrypt,  i_CryptChar;

	i_CharToCrypt = ((int) _pc_Char) & 0x000000FF;
	i_CryptChar = i_CharToCrypt << ((int)((UCHAR) (_c_Key) % 8));
	*_pc_Dest = (char) i_CryptChar;
	i_CryptChar &= ~0xFFFF00FF;
	i_CryptChar >>= 8;
	*_pc_Dest += (UCHAR) i_CryptChar ;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void BIG_special_DecryptOne(char *_pc_Dest, char _pc_Char, char _c_Key)
{
	UINT		i_CharToCrypt, i_CryptChar;

	i_CharToCrypt = ((int) _pc_Char) & 0x000000FF;
	i_CryptChar = i_CharToCrypt << ((int)(8 - ((UCHAR)(_c_Key) % 8)));
	*_pc_Dest = (char) i_CryptChar;
	i_CryptChar &= ~0xFFFF00FF;
	i_CryptChar >>= 8;
	*_pc_Dest += (UCHAR) i_CryptChar ;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void BIG_special_Encryption(char **ppc_Dest, char **ppc_Buf, char* pc_Key, ULONG _ul_Size, BOOL _b_EncryptKey)
{
	ULONG	i, StartIndex;
	UCHAR	uc_NextKeyChar;
	ULONG	ul_CurrentOffset;
	ULONG	ul_TotalSize;

	if(!_ul_Size) return;


	/* Si on veut crypter dans le même buffer, 
	   il faut augmenter la taille de ce buffer, au moins de la taille de la marque
	   mais aussi de la taille de la clef si celle ci doit etre encryptée */
	if(*ppc_Dest == *ppc_Buf)
	{
		*ppc_Dest = (char *) MEM_p_Realloc(*ppc_Dest, _ul_Size + (_b_EncryptKey ? ENCRYPT_KEY_SIZE + ENCRYPT_CRC_SIZE : ENCRYPT_CRC_SIZE) + CRYPT_MARK_SIZE);
		*ppc_Buf = *ppc_Dest;

		/* Je vais rajouter la mark de cryptage en début de fichier ... je memmove le reste */
		L_memmove(*ppc_Dest + CRYPT_MARK_SIZE, *ppc_Dest, _ul_Size);
		StartIndex = CRYPT_MARK_SIZE;
	}
	else
		StartIndex = 0;

	/*	MARK */
	/*	ATTENTION, FAUDRA FAIRE GAFFE SI CRYPT_MARK_SIZE CHANGE PUISQUE JE NE METS QU'UN LONG
		EN DEBUT DE BUFFER .. FAUDRA FAIRE UN PETIT TRUC QUI MET PLUSIEURS MARQUES SI ON
		TOUCHE A CRYPT_MARK_SIZE */
	ERR_X_Assert(CRYPT_MARK_SIZE == 4);

	if(_b_EncryptKey)
		*(ULONG *)(*ppc_Dest) = CRYPT_MODE1_MARK;
	else
		*(ULONG *)(*ppc_Dest) = CRYPT_MODE2_MARK;

	/* J'encrypte le buffer */
	for(i = 0; i < _ul_Size; i++)
		BIG_special_EncryptOne(*ppc_Dest + i + CRYPT_MARK_SIZE, *(*ppc_Buf + i + StartIndex), pc_Key[i % ENCRYPT_KEY_SIZE]);

	ul_CurrentOffset = CRYPT_MARK_SIZE;
	ul_TotalSize = _ul_Size;
	if(_b_EncryptKey)
	{
		/* Je planque la clef dans le buffer crypté, l'air de rien ... 
			Ca rajoute donc ENCRYPT_KEY_SIZE  octets au buffer crypté */
		uc_NextKeyChar = (*(*ppc_Dest + CRYPT_MARK_SIZE) & 0x0F);
		if(!uc_NextKeyChar || (ULONG) (uc_NextKeyChar) > ul_TotalSize) uc_NextKeyChar = 1;
		ul_CurrentOffset = CRYPT_MARK_SIZE + (ULONG) (uc_NextKeyChar & 0x000000FF);
		for(i = 0; i < ENCRYPT_KEY_SIZE; i++)
		{
			uc_NextKeyChar = (UCHAR) lRand(1, 15);
			if(ul_CurrentOffset + uc_NextKeyChar >= _ul_Size) uc_NextKeyChar = 1;

			L_memmove(*ppc_Dest + ul_CurrentOffset + 1, *ppc_Dest + ul_CurrentOffset, ul_TotalSize - ul_CurrentOffset + CRYPT_MARK_SIZE);
			ul_TotalSize ++;
			*(*ppc_Dest + ul_CurrentOffset) = pc_Key[i] | ((uc_NextKeyChar) << 4);

			ul_CurrentOffset += uc_NextKeyChar;
		}
	}

	/* Je stocke les différents checksums dans le buffer crypté */
	uc_NextKeyChar = (UCHAR) ((_ul_Size % 32) + 1);
	if((ul_CurrentOffset + uc_NextKeyChar) >ul_TotalSize) uc_NextKeyChar = 1;
	ul_CurrentOffset += (ULONG) (uc_NextKeyChar & 0x000000FF);
	for(i = 0; i < ENCRYPT_CRC_SIZE; i++)
	{
		uc_NextKeyChar = ((((UCHAR)pc_Key[ENCRYPT_KEY_SIZE + i]) % 8) + 1);
		if(ul_CurrentOffset + uc_NextKeyChar >= _ul_Size) uc_NextKeyChar = 1;
		L_memmove(*ppc_Dest + ul_CurrentOffset + 1, *ppc_Dest + ul_CurrentOffset, ul_TotalSize - ul_CurrentOffset + CRYPT_MARK_SIZE);
		ul_TotalSize ++;
		*(*ppc_Dest + ul_CurrentOffset) = (UCHAR) pc_Key[ENCRYPT_KEY_SIZE + i];
		ul_CurrentOffset += uc_NextKeyChar;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ BOOL BIG_b_IsBufferCrypted(char *_pc_Buf)
{
	return((*(ULONG *)_pc_Buf == CRYPT_MODE1_MARK) || (*(ULONG *)_pc_Buf == CRYPT_MODE2_MARK));
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void BIG_special_Decryption(char **ppc_Dest, char **ppc_Buf, char* _pc_Key, ULONG _ul_Size, ULONG *_aul_crc)
{
	ULONG	i, j, k;
	ULONG	ul_GlobalOffset[ENCRYPT_KEY_SIZE + ENCRYPT_CRC_SIZE];
	ULONG	ul_CurrentOffset;
	UCHAR	uc_NextKeyChar;
	BOOL	b_KeyEncrypted;
	char	pc_Key[ENCRYPT_KEY_SIZE + ENCRYPT_CRC_SIZE];

	if(!_ul_Size) return;

	/* Buffer crypté ... y'a au moins mes ENCRYPT_CRC_SIZE octets de clef dans la size .. sinon, c mort */
	ERR_X_Assert(_ul_Size >= ENCRYPT_CRC_SIZE + CRYPT_MARK_SIZE);
	ERR_X_Assert( (*(ULONG *)(*ppc_Buf) == CRYPT_MODE1_MARK) || (*(ULONG *)(*ppc_Buf) == CRYPT_MODE2_MARK));

	/* The Crypted file indicates that the key has not been encrypted inside it but no key is given	==> error */
	ERR_X_Assert(_pc_Key || (*(ULONG *)(*ppc_Buf) == CRYPT_MODE1_MARK));

	b_KeyEncrypted = (*(ULONG *)(*ppc_Buf) == CRYPT_MODE1_MARK);

	if(b_KeyEncrypted)
	{
		ERR_X_Assert(_ul_Size >= ENCRYPT_KEY_SIZE + ENCRYPT_CRC_SIZE + CRYPT_MARK_SIZE);
	}

	L_memset(ul_GlobalOffset, 0, (ENCRYPT_KEY_SIZE + ENCRYPT_CRC_SIZE) * sizeof(ULONG));

	// First, we have to recreate the Key that is hidden in the crypted data ...
	ul_CurrentOffset = CRYPT_MARK_SIZE;
	if(b_KeyEncrypted)
	{
		uc_NextKeyChar = ((*(*ppc_Buf + CRYPT_MARK_SIZE) & 0x0F));
		if(!uc_NextKeyChar || ((ULONG) (uc_NextKeyChar) > (_ul_Size - ENCRYPT_KEY_SIZE - ENCRYPT_CRC_SIZE - 4)))
			uc_NextKeyChar = 1;
		ul_CurrentOffset = CRYPT_MARK_SIZE + (ULONG) uc_NextKeyChar & 0x000000FF;
		for(i = 0; i < ENCRYPT_KEY_SIZE ; i++)
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

	// Secondly, we get the different checksum that are stored in the crypted buffer ...
	uc_NextKeyChar = (UCHAR) (((_ul_Size - (b_KeyEncrypted ? ENCRYPT_KEY_SIZE : 0) - ENCRYPT_CRC_SIZE - 4) % 32) + 1);
	if(ul_CurrentOffset + uc_NextKeyChar >= _ul_Size - (b_KeyEncrypted ? ENCRYPT_KEY_SIZE : 0) - ENCRYPT_CRC_SIZE - 4)  uc_NextKeyChar = 1;
	ul_CurrentOffset += (ULONG) (uc_NextKeyChar & 0x000000FF);
	for(i = 0; i < ENCRYPT_CRC_SIZE ; i++)
	{
		pc_Key[ENCRYPT_KEY_SIZE + i] = *(*ppc_Buf + ul_CurrentOffset);
		ul_GlobalOffset[b_KeyEncrypted ? ENCRYPT_KEY_SIZE + i : i] = ul_CurrentOffset;
		uc_NextKeyChar = (UCHAR) (((UCHAR)pc_Key[ENCRYPT_KEY_SIZE + i]) % 8) + 1;
		if(ul_CurrentOffset + uc_NextKeyChar >= _ul_Size - (b_KeyEncrypted ? ENCRYPT_KEY_SIZE : 0) - ENCRYPT_CRC_SIZE - 4) uc_NextKeyChar = 1;
		ul_CurrentOffset += uc_NextKeyChar;
	}

	if(_aul_crc)
	{
		for(i = 0; i < ENCRYPT_CRC_SIZE / 4; i++)
		{
			_aul_crc[i] =  ((pc_Key[ENCRYPT_KEY_SIZE + (4 * i) + 0]) & 0x000000FF);
			_aul_crc[i] += ((pc_Key[ENCRYPT_KEY_SIZE + (4 * i) + 1] << 8) & 0x0000FF00);
			_aul_crc[i] += ((pc_Key[ENCRYPT_KEY_SIZE + (4 * i) + 2] << 16) & 0x00FF0000);
			_aul_crc[i] += ((pc_Key[ENCRYPT_KEY_SIZE + (4 * i) + 3] << 24) & 0xFF000000);
		}
	}


	k = 0;
	j = 0;
	for(i = CRYPT_MARK_SIZE; i < _ul_Size; i++)
	{
		/* Si je tombe sur un octet correspondant à un morceau de la clef planquée 
		   ou de CRC dans le corps crypté, je laisse passer ... */
		if((k < ENCRYPT_KEY_SIZE + ENCRYPT_CRC_SIZE) && (i == ul_GlobalOffset[k]))
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
		*ppc_Dest = (char *)MEM_p_Realloc(*ppc_Dest, _ul_Size - ((*(ULONG *)(*ppc_Buf) == CRYPT_MODE2_MARK) ? 0 : ENCRYPT_KEY_SIZE + ENCRYPT_CRC_SIZE) - CRYPT_MARK_SIZE);
		*ppc_Buf = *ppc_Dest;
	}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void BIG_ComputeCRC(ULONG *ul_CRC, char *pc_Buf, ULONG ul_Size)
{
	// CheckSum du buffer initial ...
	ul_CRC[0] = BIG_special_ComputeCheckSum1(1, pc_Buf, ul_Size);
	ul_CRC[1] = BIG_special_ComputeCheckSum2(1, pc_Buf, ul_Size);
	ul_CRC[2] = BIG_special_ComputeCheckSum3(1, pc_Buf, ul_Size);
	ul_CRC[3] = BIG_special_ComputeCheckSum4(1, pc_Buf, ul_Size);
	ul_CRC[4] = BIG_special_ComputeCheckSum5(1, pc_Buf, ul_Size);
	ul_CRC[5] = BIG_special_ComputeCheckSum6(1, pc_Buf, ul_Size);

}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void BIG_ComputeKey(char *Key, char *pc_Buf, ULONG ul_Size)
{
	ULONG	ul_CRC[6];
	int		i;


	// CheckSum du buffer initial ...
	BIG_ComputeCRC(ul_CRC, pc_Buf, ul_Size);

	for(i = 0; i < ENCRYPT_KEY_SIZE; i ++)
	{
		Key[i] = (char) lRand(0, 0x0F);
	}

	// Checksums
	Key[ENCRYPT_KEY_SIZE + 0 + 0] = (char) ((ul_CRC[0] & 0x000000FF) >> 0);
	Key[ENCRYPT_KEY_SIZE + 1 + 0] = (char) ((ul_CRC[0] & 0x0000FF00) >> 8);
	Key[ENCRYPT_KEY_SIZE + 2 + 0] = (char) ((ul_CRC[0] & 0x00FF0000) >> 16);
	Key[ENCRYPT_KEY_SIZE + 3 + 0] = (char) ((ul_CRC[0] & 0xFF000000) >> 24);

	Key[ENCRYPT_KEY_SIZE + 0 + 4] = (char) ((ul_CRC[1] & 0x000000FF) >> 0);
	Key[ENCRYPT_KEY_SIZE + 1 + 4] = (char) ((ul_CRC[1] & 0x0000FF00) >> 8);
	Key[ENCRYPT_KEY_SIZE + 2 + 4] = (char) ((ul_CRC[1] & 0x00FF0000) >> 16);
	Key[ENCRYPT_KEY_SIZE + 3 + 4] = (char) ((ul_CRC[1] & 0xFF000000) >> 24);

	Key[ENCRYPT_KEY_SIZE + 0 + 8] = (char) ((ul_CRC[2] & 0x000000FF) >> 0);
	Key[ENCRYPT_KEY_SIZE + 1 + 8] = (char) ((ul_CRC[2] & 0x0000FF00) >> 8);
	Key[ENCRYPT_KEY_SIZE + 2 + 8] = (char) ((ul_CRC[2] & 0x00FF0000) >> 16);
	Key[ENCRYPT_KEY_SIZE + 3 + 8] = (char) ((ul_CRC[2] & 0xFF000000) >> 24);

	Key[ENCRYPT_KEY_SIZE + 0 + 12] = (char) ((ul_CRC[3] & 0x000000FF) >> 0);
	Key[ENCRYPT_KEY_SIZE + 1 + 12] = (char) ((ul_CRC[3] & 0x0000FF00) >> 8);
	Key[ENCRYPT_KEY_SIZE + 2 + 12] = (char) ((ul_CRC[3] & 0x00FF0000) >> 16);
	Key[ENCRYPT_KEY_SIZE + 3 + 12] = (char) ((ul_CRC[3] & 0xFF000000) >> 24);

	Key[ENCRYPT_KEY_SIZE + 0 + 16] = (char) ((ul_CRC[4] & 0x000000FF) >> 0);
	Key[ENCRYPT_KEY_SIZE + 1 + 16] = (char) ((ul_CRC[4] & 0x0000FF00) >> 8);
	Key[ENCRYPT_KEY_SIZE + 2 + 16] = (char) ((ul_CRC[4] & 0x00FF0000) >> 16);
	Key[ENCRYPT_KEY_SIZE + 3 + 16] = (char) ((ul_CRC[4] & 0xFF000000) >> 24);

	Key[ENCRYPT_KEY_SIZE + 0 + 20] = (char) ((ul_CRC[5] & 0x000000FF) >> 0);
	Key[ENCRYPT_KEY_SIZE + 1 + 20] = (char) ((ul_CRC[5] & 0x0000FF00) >> 8);
	Key[ENCRYPT_KEY_SIZE + 2 + 20] = (char) ((ul_CRC[5] & 0x00FF0000) >> 16);
	Key[ENCRYPT_KEY_SIZE + 3 + 20] = (char) ((ul_CRC[5] & 0xFF000000) >> 24);
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void BIG_special_Encrypt4FAT(char * _pc_Buf, ULONG _ul_Size)
{
	ULONG	i;
	ULONG	ul_Key;
	extern ULONG BIG_ul_FAT_GetCryptKey();	

	ul_Key = BIG_ul_FAT_GetCryptKey();

	for(i = 0; i < _ul_Size; i++)
	{
		_pc_Buf[i] ^= (ul_Key << ((_ul_Size - i) % 4));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void BIG_special_Decrypt4FAT(char * _pc_Buf, ULONG _ul_Size)
{
	ULONG	i;
	ULONG	ul_Key;
	extern ULONG BIG_ul_FAT_GetCryptKey();	

	ul_Key = BIG_ul_FAT_GetCryptKey();

	for(i = 0; i < _ul_Size; i++)
	{
		_pc_Buf[i] ^= (ul_Key << ((_ul_Size - i) % 4));
	}
}


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
