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

#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoImpl.h"
#include "CrosswalkGamebryoTexHelpers.h"
#include "CrosswalkGamebryoMatHelpers.h"
#include "CrosswalkGamebryoSettings.h"

#include "MaterialLibrary.h"
#include "XSIMaterial.h"
#include "XSIMaterialInfo.h"

namespace epg
{
    //---------------------------------------------------------------------------
    // Converts all material in the given library.
    //---------------------------------------------------------------------------
    bool ConvertMaterials(Context & io_Context, CSLMaterialLibrary* in_MatLib)
    {
        if (!io_Context.GetSettings().GetExportTextures())
            return true;

        if (!in_MatLib)
            return true;

        CSLBaseMaterial** matList = in_MatLib->GetMaterialList();
        const SI_Int matCount = in_MatLib->GetMaterialCount();
        Phaser phase(io_Context, CONVERT_MATERIALS_PHASE, matCount);
        for (SI_Int i = 0; i < matCount; ++i)
        {
            phase.Progress();

            if (matList[i])
            {
                if (!io_Context.Convert(matList[i][0]))
                {
                    return false;
                }
            }
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Convert materials.
    //---------------------------------------------------------------------------
    bool ConvertXSIMaterial(Context& io_Context, CSLBaseMaterial& in_Material)
    {
        if (!io_Context.GetSettings().GetExportTextures())
            return true;

        CSLXSIMaterial* materialXSI  = static_cast<CSLXSIMaterial *>(&in_Material);
        if (!materialXSI)
        {
            return io_Context.Logf(
                LOG_ERROR,
                "Invalid XSI material \"%s\".",
                in_Material.Name().GetText());
        }

        if (!ConvertSurfaceColors(io_Context, *materialXSI))
        {
            return io_Context.Logf(
                LOG_ERROR,
                "Material \"%s\" real-time material color conversion failed.",
                in_Material.Name().GetText());
        }

        if (!ConvertSurfaceTextures(io_Context, *materialXSI))
        {
            return io_Context.Logf(
                LOG_ERROR,
                "Material \"%s\" real-time shader texturing conversion failed.",
                in_Material.Name().GetText());
        }

        if (!ConvertTextureEffects(io_Context, *materialXSI))
        {
            return io_Context.Logf(
                LOG_ERROR,
                "Material \"%s\" real-time shader texturing effects conversion failed.",
                in_Material.Name().GetText());
        }

        return true;
    }

    //---------------------------------------------------------------------------
    bool ConvertMaterial(Context& io_Context, CSLBaseMaterial& in_Material)
    {
        if (!io_Context.GetSettings().GetExportTextures())
            return true;

        CSLMaterial* materialXSI  = static_cast<CSLMaterial *>(&in_Material);
        if (!materialXSI)
        {
            return io_Context.Logf(
                LOG_ERROR,
                "Invalid XSI material \"%s\".",
                in_Material.Name().GetText());
        }
        else
        {
            return io_Context.Logf(
                LOG_WARNING,
                "XSI material \"%s\" not supported.",
                in_Material.Name().GetText());
        }
    }
    //---------------------------------------------------------------------------
}
