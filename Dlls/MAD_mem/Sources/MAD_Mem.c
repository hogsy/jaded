/*$T MAD_Mem.cpp GC!1.32 05/20/99 18:58:35 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>


static void             **MALLOCTABLE = NULL;
static unsigned long    *MALLOCSizes  = NULL;
static unsigned long    MAD_Mallocnumber;

static unsigned long  MAX_Mallox = 0;
static unsigned long  MadMemIsInit = 0;
static unsigned long  MadMem_ulTotalMalloc;
/*
 ===================================================================================================
 ===================================================================================================
 */
void Mad_meminit(void)
{
	if (MadMemIsInit) return;
    MAD_Mallocnumber = 0;
	MAX_Mallox = 128L;
#ifdef JADEFUSION
	MALLOCTABLE = (void**)malloc(MAX_Mallox * 4L);
	MALLOCSizes = (unsigned long*)malloc(MAX_Mallox * 4L);
#else
	MALLOCTABLE = malloc(MAX_Mallox * 4L);
	MALLOCSizes = malloc(MAX_Mallox * 4L);
#endif
	MadMem_ulTotalMalloc = 0;
	MadMemIsInit = 1;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void *Mad_malloc(unsigned long size)
{
	if (!MadMemIsInit) Mad_meminit();
	MadMem_ulTotalMalloc += size;
	if (MAD_Mallocnumber >= MAX_Mallox) 
	{
		void *MALLOCSAVE ;
		unsigned long Old_MAX_Mallox;
		Old_MAX_Mallox = MAX_Mallox;

		/* Size Expansion function */
		MAX_Mallox = MAX_Mallox + (MAX_Mallox >> 1);

		MALLOCSAVE = (void *)MALLOCTABLE;
		MALLOCTABLE = (void **)malloc(MAX_Mallox * 4L);
		memcpy(MALLOCTABLE , MALLOCSAVE , Old_MAX_Mallox * 4L);
		free(MALLOCSAVE) ;
		MALLOCSAVE = (unsigned long *)MALLOCSizes;
		MALLOCSizes = (unsigned long*)malloc(MAX_Mallox * 4L);
		memcpy(MALLOCSizes , MALLOCSAVE , Old_MAX_Mallox * 4L);
		free(MALLOCSAVE) ;
	}
    MALLOCTABLE[MAD_Mallocnumber] = malloc(size);
    MALLOCSizes[MAD_Mallocnumber] = size;
    memset(MALLOCTABLE[MAD_Mallocnumber], 0, size);
    return MALLOCTABLE[MAD_Mallocnumber++];
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void *Mad_Realloc(void *Adresse, unsigned long Newsize)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   AdCount;
    void            *SaveOld;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if (Adresse == NULL)
		return Mad_malloc(Newsize);

	if (!MadMemIsInit) Mad_meminit();
    for(AdCount = 0; AdCount < MAD_Mallocnumber; AdCount++)
    {
        if(MALLOCTABLE[AdCount] == Adresse)
        {
			MadMem_ulTotalMalloc += Newsize;
			MadMem_ulTotalMalloc -= MALLOCSizes[AdCount];
            if(MALLOCSizes[AdCount] > Newsize) return MALLOCTABLE[AdCount];
            SaveOld = MALLOCTABLE[AdCount];
            MALLOCTABLE[AdCount] = malloc(Newsize);
			memset(MALLOCTABLE[AdCount], 0, Newsize);
            memcpy(MALLOCTABLE[AdCount], SaveOld , MALLOCSizes[AdCount]);
            MALLOCSizes[AdCount] = Newsize;
            free(SaveOld);
            return MALLOCTABLE[AdCount];
        }
    }
    return Mad_malloc(Newsize);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void Mad_free(void)
{
	if (!MadMemIsInit) return;
    while(MAD_Mallocnumber--)
        free(MALLOCTABLE[MAD_Mallocnumber]);
	MAX_Mallox = 4096L;
	free(MALLOCTABLE);
	free(MALLOCSizes);
	MadMemIsInit = 0;
	MadMem_ulTotalMalloc = 0;
}

