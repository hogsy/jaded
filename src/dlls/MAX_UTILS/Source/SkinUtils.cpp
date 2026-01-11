#include "SkinUtils.h"

#include "Dummy.h"
#include "modstack.h"
#include "iparamm2.h"
#include "iskin.h"

/*
 ===================================================================================================
    Copied from ".../maxsdk/samples/modifiers/BonesDef/SkinUtils/SkinTools.cpp"
 ===================================================================================================
 */
Modifier *SkinUtils::GetSkin(INode *node)
{

	Object* pObj = node->GetObjectRef();

	if (!pObj) return NULL;

	
	while (pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		IDerivedObject* pDerObj = (IDerivedObject *)(pObj);
			
		int Idx = 0;

		while (Idx < pDerObj->NumModifiers())
		{
			// Get the modifier. 
			Modifier* mod = pDerObj->GetModifier(Idx);

			
			if (mod->ClassID() == SKIN_CLASSID)
			{
				// is this the correct Physique Modifier based on index?
				return mod;
			}

			Idx++;
		}

		pObj = pDerObj->GetObjRef();
	}

	return NULL;


}