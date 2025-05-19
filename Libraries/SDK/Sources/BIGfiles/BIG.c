/*$T BIG.c GC!1.39 06/15/99 12:36:17 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"
#include "LOAding/LOA.h"
#include "BIGfiles/BIGio.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif

/*
 ===================================================================================================
 ===================================================================================================
 */
void BIG_InitModule(void)
{
    LOA_InitModule();
    BIGio_InitStat();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void BIG_CloseModule(void)
{
    LOA_CloseModule();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOG_Use(char *bigfile)
{
#if !defined(PSX2_TARGET) && !defined(_GAMECUBE) && !defined(_XBOX) && !defined(_XENON)//POPOWARNING
	ULONG	ul_File;
	FILE	*f;
	int		chg = 0;
	char	*buf, *pz, *pz1;
	char	asz_Name[1024];
	char	asz_Date[1024];
	char	asz_Time[1024];
	int		len, b_end;
	int		i;

	ul_File = BIG_ul_SearchKeyToPos(0xFFC0D666);
	if(ul_File == -1) return;

	f = fopen(bigfile, "r+b");
	if(!f)
	{
		if(_access(bigfile, 2))
		{
			_chmod(bigfile, _S_IWRITE);
			if(_access(bigfile, 2)) return;
			f = fopen(bigfile, "r+b");
			if(!f) return;
			chg = 1;
		}
	}

	buf = (char *) malloc(50000);
	if(!buf) return;
	fseek(f, ul_File + 4, SEEK_SET);
	fread(buf, 50000, 1, f);

	// Get infos
	len = 99;
	GetComputerName(asz_Name, (LPDWORD)&len);
	L_memset(asz_Date, ' ', sizeof(asz_Date));
	_strdate(asz_Date);
	_strtime(asz_Time);
	L_strcat(asz_Date, " ");
	L_strcat(asz_Date, asz_Time);

	asz_Date[strlen(asz_Date)] = ' ';	// Assure une taille fixe
	asz_Date[30] = 0;

	// Crypt
	i = 0;
	pz1 = asz_Name;
	while(*pz1)
	{
		*pz1 = *pz1 + i++;
		pz1++;
	}

	i = 0;
	pz1 = asz_Date;
	while(*pz1)
	{
		*pz1 = *pz1 + i++;
		pz1++;
	}

	////////
	pz = buf;
	while(*pz) pz++;	// Zap phrase
	pz++;

	b_end = 1;
	while(*pz)
	{
		if(!L_strncmp(pz, asz_Name, strlen(asz_Name)))	// Deja la ?
		{
			b_end = 0;
			goto ok;
		}
		pz += strlen(pz);	// Zap nom
		pz++;
		pz += strlen(pz);	// Zap date/heure
		pz++;
	}

ok:
	if(pz - buf > 49500) goto end;

	L_strcpy(pz, asz_Name);
	pz += strlen(asz_Name);
	pz++;

	L_strcpy(pz, asz_Date);
	pz += strlen(asz_Date);
	pz++;

	if(b_end) *pz = 0;

	fseek(f, ul_File + 4, SEEK_SET);
	fwrite(buf, 50000, 1, f);
end:
	fclose(f);
#endif
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
