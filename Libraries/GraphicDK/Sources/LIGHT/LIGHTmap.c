#include "Precomp.h"

#include "GDInterface/GDInterface.h"
#include "texture/texfile.h"
#include "texture/texstruct.h"
#include "LIGHT/LIGHTmapstruct.h"
#include "LIGHT/LIGHTmap.h"
#include "TABles/TABles.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "GEOmetric/GEOobject.h"

#if defined(ACTIVE_EDITORS)
#include "ENGine/Sources/OBJects/OBJaccess.h"
#endif

LIGHT_tdst_LightmapPageInfo*	LIGHT_gFirstLightmapPage = NULL;

void LIGHT_Lightmaps_UsedTexture(char* _pcUsedTextures)
{
	LIGHT_tdst_LightmapPageInfo* pPage;
	
	pPage = LIGHT_gFirstLightmapPage;

	while (pPage)
	{
		_pcUsedTextures[pPage->texIndex] = 1;

		pPage = pPage->pNextLightmap;
	}
}

LIGHT_tdst_LightmapPageInfo* LIGHT_Lightmaps_GetPointerForKey(ULONG _ulKey, bool _bCreateIfNeeded, bool _bAddRefIfPresent, bool _bIsNotaKey)
{
	LIGHT_tdst_LightmapPageInfo** pInsertTo;
	
	pInsertTo = &LIGHT_gFirstLightmapPage;

	while (*pInsertTo)
	{
		if ((*pInsertTo)->texBFKey == _ulKey)
		{
			break;
		}

		pInsertTo = &(*pInsertTo)->pNextLightmap;
	}

	// was the page found?
	if (*pInsertTo)
	{
		if (_bAddRefIfPresent)
		{
			// inc the refcount
			(*pInsertTo)->nbRef++;

			// add a ref to the texture also
			//if(!_bIsNotaKey)
			//	TEX_List_AddRefTexture((*pInsertTo)->texIndex);		
		}
	}
	else
	{
		// if need to create it if it's not present
		if (_bCreateIfNeeded)
		{
			// Allocate it
			LIGHT_tdst_LightmapPageInfo* pNewPage = (LIGHT_tdst_LightmapPageInfo*) MEM_p_Alloc(sizeof(LIGHT_tdst_LightmapPageInfo));	

			// Initialize it
			if (!_bIsNotaKey)
				pNewPage->nbRef = 1;
			else
				pNewPage->nbRef = 0;

			pNewPage->pNextLightmap = NULL;

			if (!_bIsNotaKey)
			{
				pNewPage->texIndex = TEX_w_List_AddTexture(&TEX_gst_GlobalList, _ulKey, false);
				//TEX_List_AddRefTexture(pNewPage->texIndex);
				pNewPage->texBFKey = _ulKey;
			}
			else
			{
				pNewPage->texBFKey = _ulKey;
			}

#ifdef ACTIVE_EDITORS
			pNewPage->pData = NULL;
			pNewPage->pageHeight = 0;
			pNewPage->pageWidth = 0;
#endif

			// Insert it in the list
			*pInsertTo = pNewPage;
		}
	}

	return *pInsertTo;
}

void	LIGHT_Lightmaps_RemoveRefLightmapPage(LIGHT_tdst_LightmapPageInfo* _pPage)
{
	// reduce the references
	_pPage->nbRef--;
	// DJ_TEMP : TEX_List_DestroyTexture(_pPage->texIndex);
	
	// if refcount is zero we remove it from the list
	if (_pPage->nbRef == 0)
	{
		LIGHT_tdst_LightmapPageInfo* pPage;
		LIGHT_tdst_LightmapPageInfo** pOld;

		pPage = LIGHT_gFirstLightmapPage;
		pOld = &LIGHT_gFirstLightmapPage;

		while (pPage)
		{
			if (pPage == _pPage)
			{
				// remove that page

				// check if it's the first one
				*pOld = pPage->pNextLightmap;

#ifdef ACTIVE_EDITORS
				if (pPage->pData)
					MEM_Free(pPage->pData);
#endif

				MEM_Free(pPage);

				pPage = NULL;
			}
			else
			{
				pOld = &pPage->pNextLightmap;
				pPage = pPage->pNextLightmap;		
			}
		}	
	}
}

#ifdef ACTIVE_EDITORS

void	LIGHT_Lightmaps_CleanUnusedLightmapTextures(void)
{
    // DJ_TEMP : TODO

    /*
	LIGHT_tdst_LightmapPageInfo* pPage;
	BIG_INDEX	currentLMPageIndex, currentIndex, nextIndex, dirIndex;

	pPage = LIGHT_gFirstLightmapPage;
	
	// loop on all the currently loaded lightmaps
	while (pPage)
	{	
		// get the fat index of the lightmap
		currentLMPageIndex = BIG_ul_SearchKeyToFat(pPage->texBFKey);

		if (currentLMPageIndex != BIG_C_InvalidIndex)
		{
			// get the folder of the lightmap texture
			dirIndex = BIG_ParentFile(currentLMPageIndex);

			// since we save only one lightmap texture per world and that we save
			// the lightmap texture in a subfolder of it's world there can only
			// be one texture in that folder, thus we enumerate the textures
			// in the folder and we destroy all the files except the current
			// lightmap file

			// enumerate all files
			currentIndex = BIG_FirstFile(dirIndex);

			while (currentIndex != BIG_C_InvalidIndex)
			{
				// get the next file (has to be done at the beginning since we may delete this file)
				nextIndex = BIG_NextFile(currentIndex);

				// check if it's the current file
				if (currentIndex != currentLMPageIndex)
				{	
					// if it's not the current file then delete it
					BIG_DeleteFile(dirIndex, currentIndex);
				}
				
				currentIndex = nextIndex;
			}
		}

		pPage = pPage->pNextLightmap;
	}
    */
}

void LIGHT_Lightmaps_DestroyGAOLightmaps(OBJ_tdst_GameObject* _pst_GO)
{
    if (!_pst_GO)
        return;

    // object must have a visual 
    if (OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu) && _pst_GO->pst_Base->pst_Visu)
    {
        // check if it has lightmaps
        if (_pst_GO->pst_Base->pst_Visu->p_us_NbTrianglesInElement)
        {
            MEM_Free(_pst_GO->pst_Base->pst_Visu->p_us_NbTrianglesInElement);

            _pst_GO->pst_Base->pst_Visu->p_us_NbTrianglesInElement = NULL;
        }

        if (_pst_GO->pst_Base->pst_Visu->pp_st_LightmapCoords)
        {
            for (int i = 0; i < _pst_GO->pst_Base->pst_Visu->usNbElements; i++)
            {
                MEM_Free( _pst_GO->pst_Base->pst_Visu->pp_st_LightmapCoords[i]);
            }

            MEM_Free( _pst_GO->pst_Base->pst_Visu->pp_st_LightmapCoords);

            LIGHT_Lightmaps_RemoveRefLightmapPage(_pst_GO->pst_Base->pst_Visu->pLMPage);

            _pst_GO->pst_Base->pst_Visu->pp_st_LightmapCoords = NULL;
            _pst_GO->pst_Base->pst_Visu->pLMPage = NULL;				
        }
    }
}

#endif