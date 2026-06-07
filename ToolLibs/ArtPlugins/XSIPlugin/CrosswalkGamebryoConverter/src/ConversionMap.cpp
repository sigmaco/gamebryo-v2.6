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

#include "CrosswalkGamebryoConvMap.h"
#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoHelpers.h"
#include "CrosswalkGamebryoImpl.h"

#include "Model.h"
#include "Material.h"

namespace epg
{
    //---------------------------------------------------------------------------
    // Implementation of the conversion. Do not call directly.
    //---------------------------------------------------------------------------

    bool ConvertDefault(Context& io_Context, CSLModel& in_Model);

    bool ConvertMesh( Context & io_Context, CSLModel& in_Model );
    bool ConvertXSIMesh( Context & io_Context, CSLModel& in_Model );

    bool ConvertXSICamera( Context & io_Context, CSLModel& in_Model );

    bool ConvertDirectionalLight( Context & io_Context, CSLModel& in_Model );
    bool ConvertPointLight( Context & io_Context, CSLModel& in_Model );
    bool ConvertSpotLight( Context & io_Context, CSLModel& in_Model );
    bool ConvertInfiniteLight( Context & io_Context, CSLModel& in_Model );

    bool ConvertMaterial(Context& io_Context, CSLBaseMaterial& in_Material);
    bool ConvertXSIMaterial(Context& io_Context, CSLBaseMaterial& in_Material);

    //---------------------------------------------------------------------------
    // Conversion function map.
    //---------------------------------------------------------------------------

    ConversionMap::ConversionMap()
    {
        m_ModelFunctions[CSLTemplate::SI_MESH] = ConvertMesh;
        m_ModelFunctions[CSLTemplate::XSI_MESH] = ConvertXSIMesh;
        m_ModelFunctions[CSLTemplate::XSI_CAMERA] = ConvertXSICamera;
        m_ModelFunctions[CSLTemplate::SI_MODEL] = ConvertDefault;
        m_ModelFunctions[CSLTemplate::SI_NULL_OBJECT] = ConvertDefault;

        m_MaterialFunctions[CSLTemplate::SI_MATERIAL] = ConvertMaterial;
        m_MaterialFunctions[CSLTemplate::XSI_MATERIAL] = ConvertXSIMaterial;

        m_ModelFunctions[CSLTemplate::SI_INFINITE_LIGHT] = ConvertInfiniteLight;
        m_ModelFunctions[CSLTemplate::SI_POINT_LIGHT] = ConvertPointLight;
        m_ModelFunctions[CSLTemplate::SI_DIRECTIONAL_LIGHT] = ConvertDirectionalLight;
        m_ModelFunctions[CSLTemplate::SI_SPOT_LIGHT] = ConvertSpotLight;
    }
    //---------------------------------------------------------------------------
    bool ConversionMap::Convert(Context& io_Context, CSLBaseMaterial& in_Material) const
    {
        CSLTemplate::ETemplateType tmplType = in_Material.Type();
        MaterialFunctions::const_iterator pos = m_MaterialFunctions.find(tmplType);
        if (pos != m_MaterialFunctions.end())
        {
            return pos->second(io_Context, in_Material);
        }
        else
        {
            return io_Context.Logf(
                LOG_WARNING,
                "Cannot find conversion function for material \"%s\""
                " of type \"%s\"; skipping conversion.",
                in_Material.GetName(), GetMaterialTypeName(in_Material));
        }
    }
    //---------------------------------------------------------------------------
    bool ConversionMap::Convert(Context& io_Context, CSLModel& in_Model) const
    {
        CSLTemplate::ETemplateType tmplType = in_Model.GetPrimitiveType();
        ModelFunctions::const_iterator pos = m_ModelFunctions.find(tmplType);
        if (pos != m_ModelFunctions.end())
        {
            return pos->second(io_Context, in_Model);
        }
        else
        {
            io_Context.Logf(
                LOG_WARNING,
                "Cannot find conversion function for model \"%s\""
                " of type \"%s\"; using default conversion.",
                in_Model.GetName(), GetModelTypeName(in_Model));
            return ConvertDefault(io_Context, in_Model);
        }
    }
    //---------------------------------------------------------------------------
}
