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

#ifndef XSI_GAMEBRYO_CONVERTER_MAT_HELPERS_H
#define XSI_GAMEBRYO_CONVERTER_MAT_HELPERS_H

#include "SLTypes.h"

#include "NiMainLibType.h"

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class XSIEXPORT CSLXSIMaterial;
class XSIEXPORT CSLXSIShader;

class NIMAIN_ENTRY NiMaterialProperty;

namespace epg
{
    class Context;

    //---------------------------------------------------------------------------
    /// Shader helpers.
    //---------------------------------------------------------------------------

    /** Lookup a specified named shader.
        @return the shader. Returns null if not found.
    */
    CSLXSIShader* FindShader(CSLXSIMaterial& in_MaterialXSI, const char * in_ParamName);

    //---------------------------------------------------------------------------
    // @name Surface material helpers.
    // @{
    //---------------------------------------------------------------------------

    /** Convert simple constant shader colors.
        @return true if any color were converted.
    */
    bool ConvertRealtimeShaderColors(
        Context& io_Context,
        CSLXSIShader& in_Shader,
        NiMaterialProperty& io_MatProp);

    /** Convert the shininess.
        @return true if the shininess was set.
    */
    bool ConvertRealtimeShaderShineness(
        CSLXSIShader& in_Shader,
        NiMaterialProperty& io_MatProp);

    /** Convert the transparency.
        @return true if transparency was set.
    */
    bool ConvertRealtimeShaderTransparency(
        Context& io_Context,
        CSLXSIShader& in_Shader,
        NiMaterialProperty& io_MatProp);

    /** Convert the shader colors to a Gamebryo material property.
        @return true on success. Doing nothing is considered successful.
    */
    bool ConvertSurfaceColors(
        Context& io_Context,
        CSLXSIMaterial& in_Material);

    // @}
}

#endif /* XSI_GAMEBRYO_CONVERTER_MAT_HELPERS_H */
