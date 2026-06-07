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

#ifndef XSI_GAMEBRYO_CONVERTER_CONV_MAP_H
#define XSI_GAMEBRYO_CONVERTER_CONV_MAP_H

#include "SLTypes.h"

#include <map>

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class XSIEXPORT CSLModel;
class XSIEXPORT CSLBaseMaterial;

namespace epg
{
    class Context;

    //---------------------------------------------------------------------------
    // The function signature for a function that converts a Crosswalk object
    // to its gamebryo equivalent using a specified conversion context.
    //---------------------------------------------------------------------------

    typedef bool (*MaterialConversionFunction)(Context& io_Context, CSLBaseMaterial& in_Material);
    typedef bool (*ModelConversionFunction)(Context& io_Context, CSLModel& in_Model);

    //---------------------------------------------------------------------------
    /// Conversion function map.
    //---------------------------------------------------------------------------

    class ConversionMap
    {
    public:
        /// Create a conversion map initialized with the default conversion functions.
        ConversionMap();

        /** Call the proper conversion function with the material. If no conversion
            function is found then call the default conversion function.
            @param Context& the context of the conversion.
            @param CSLBaseMaterial& the material to convert.
            @return true if the conversion function returned true.
        */
        bool Convert(Context& io_Context, CSLBaseMaterial& in_Material) const;

        /** Call the proper conversion function with the model. If no conversion
            function is found then call the default conversion function.
            @param Context& the context of the conversion.
            @param CSLModel& the model to convert.
            @return true if the conversion function returned true.
        */
        bool Convert(Context& io_Context, CSLModel& in_Model) const;

    private:
        // Function indexed by CSLTemplate::ETemplateType.
        // Currently, only one function per type. Might use multi-map in the future?
        typedef std::map<int,MaterialConversionFunction> MaterialFunctions;
        typedef std::map<int,ModelConversionFunction>    ModelFunctions;

        MaterialFunctions           m_MaterialFunctions;
        ModelFunctions              m_ModelFunctions;
    };
}

#endif /* XSI_GAMEBRYO_CONVERTER_CONV_MAP_H */
