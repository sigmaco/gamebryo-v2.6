// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#include "NiMaterialHelpers.h"
#include "NiMaterialToolkit.h"
#include <NiRenderObject.h>
#include <NiNode.h>
#include <NiShader.h>
#include <NiShaderFactory.h>

// Disable the "identifier was truncated" Warning
#pragma warning( disable : 4786 )

//---------------------------------------------------------------------------
void NiMaterialHelpers::RecursivelyRegisterMaterialsHelper(
    NiRenderer* pkRenderer, NiAVObject* pkObj, NiTList<const char*>& kErrList)
{
    if (pkRenderer == NULL)
        return;

    if (pkObj == NULL)
        return;

    if (NiIsKindOf(NiNode, pkObj))
    {
        NiNode* pkNode = (NiNode*) pkObj;
        for (unsigned int ui =0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild != NULL)
            {
                RecursivelyRegisterMaterialsHelper(pkRenderer, 
                    pkChild, kErrList);
            }
        }
    }
    else if (NiIsKindOf(NiRenderObject, pkObj))
    {
        NiRenderObject* pkGeom = (NiRenderObject*) pkObj;
        for (unsigned int ui = 0; ui < pkGeom->GetMaterialCount(); ui++)
        {
            bool bSuccess = pkGeom->ComputeCachedShader(ui);

            if (!bSuccess)
            {
                const NiMaterialInstance* pkInstance =
                    pkGeom->GetMaterialInstance(ui);

                if (!pkInstance->GetMaterial())
                {
                    continue;
                }

                const char* pcName = pkInstance->GetMaterial()->GetName();
                NiTListIterator kIter;    
                kIter = kErrList.GetHeadPos();
                bool bAlreadyAdded = false;
                while (kIter != NULL)
                {
                    const char* pcListName = kErrList.GetNext(kIter);
                    if (strcmp(pcListName, pcName) == 0)
                        bAlreadyAdded = true;
                }
                if (!bAlreadyAdded)
                    kErrList.AddHead(pcName);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiMaterialHelpers::RecursivelyReleaseMaterialsHelper(
    NiRenderer* pkRenderer, NiAVObject* pkObj, bool bReplace)
{
    if (pkRenderer == NULL)
        return;
    if (pkObj == NULL)
        return;

    if (NiIsKindOf(NiNode, pkObj))
    {
        NiNode* pkNode = (NiNode*) pkObj;
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild != NULL)
            {
                RecursivelyReleaseMaterialsHelper(pkRenderer, pkChild,
                    bReplace);
            }
        }
    }
    else if (NiIsKindOf(NiRenderObject, pkObj))
    {
        NiRenderObject* pkGeom = (NiRenderObject*) pkObj;
        for (unsigned int ui = 0; ui < pkGeom->GetMaterialCount(); ui++)
        {
            pkGeom->ClearCachedShaderAndSetNeedsUpdate(ui);
        }
    }
}
//---------------------------------------------------------------------------
bool NiMaterialHelpers::RegisterMaterials(NiAVObject* pkScene, 
    NiRenderer* pkRenderer)
{
    if (pkScene == NULL || pkRenderer == NULL)
    {
        return false;
    }

    NiMaterialToolkit::UpdateMaterialDirectory();

    NiTList<const char*> kErrorMaterials;
    RecursivelyRegisterMaterialsHelper(pkRenderer, pkScene, kErrorMaterials);

    if (kErrorMaterials.GetSize() > 0)
    {
        NiTListIterator kIter;
        char acString[1024];
        NiSprintf(acString, 1024, "The following shaders failed to load:\n");
            
        kIter = kErrorMaterials.GetHeadPos();
        while (kIter != NULL)
        {
            const char* pcName = kErrorMaterials.GetNext(kIter);
            NiSprintf(acString, 1024, "%s%s\n", acString, pcName);
        }
        NiSprintf(acString, 1024, "%s\nPossible Problems:\nPlease verify "
            "that these shaders can be found in the directory specified by "
            "your EGB_SHADER_LIBRARY_PATH\n"
            "This shader potentially uses Pixel/Vertex shaders that "
            "your video card cannot run.\n"
            "\nFixed function pipeline will be used instead!\n", 
            acString);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiMaterialHelpers::UnRegisterMaterials(NiAVObject* pkScene, 
    NiRenderer* pkRenderer, bool bReplace)
{
    if (pkScene == NULL || pkRenderer == NULL)
        return false;

    RecursivelyReleaseMaterialsHelper(pkRenderer, pkScene, bReplace);
    return true;
}
//---------------------------------------------------------------------------
