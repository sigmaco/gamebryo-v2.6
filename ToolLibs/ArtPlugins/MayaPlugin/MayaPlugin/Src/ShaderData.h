// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
// 
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
// 
// Copyright (c) 1996-2008 Emergent Game Technologies.
// All Rights Reserved.
// 
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#ifndef SHADERDATA_H
#define SHADERDATA_H

#include "maya/MPxCommand.h"
#include "maya/MSyntax.h"
#include <maya/MArgDatabase.h>
#include <NiShaderAttributeDesc.h>

#define MAKE_BIT_FLAG(offset) (0x0001 << (offset))

#pragma warning( push )
#pragma warning( disable: 4510 )
#pragma warning( disable: 4512 )
#pragma warning( disable: 4610 )

class ShaderData : public MPxCommand 
{ 
public:

    MStatus    doIt( const MArgList& );
    MStatus    redoIt();
    MStatus    undoIt();
    bool isUndoable() const;
    static MSyntax newSyntax();
    static void* creator();

private:

    struct Argument
    {
        const char*  shortName;
        const char* longName;
        const NiUInt32 bitFlag;
        const MSyntax::MArgType argType;
    };

    enum FlagOffsets
    {
        FLAG_OFFSET_FIRST = 0,
        RELOAD_SHADERS_BIT_FLAG_OFFSET = 0,
        SHADER_NAMES_BIT_FLAG_OFFSET,
        SHADER_BIT_FLAG_OFFSET,
        ATTRIBUTES_NAMES_BIT_FLAG_OFFSET,
        ATTRIBUTE_BIT_FLAG_OFFSET,
        TYPE_BIT_FLAG_OFFSET,
        HAS_RANGE_BIT_FLAG_OFFSET,
        RANGE_BIT_FLAG_OFFSET,
        DESCRIPTION_BIT_FLAG_OFFSET,
        IMPLEMENTATION_NAMES_BIT_FLAG_OFFSET,
        IMPLEMENTATION_BIT_FLAG_OFFSET,
        TEXTURE_BIT_FLAG_OFFSET,
        ATTRIBUTE_IS_ARRAY_BIT_FLAG_OFFSET,
        FLAG_OFFSET_MAX
    };

    enum FlagMask
    {
        RELOAD_SHADERS_MASK = MAKE_BIT_FLAG(RELOAD_SHADERS_BIT_FLAG_OFFSET),
        SHADER_NAMES_MASK = MAKE_BIT_FLAG(SHADER_NAMES_BIT_FLAG_OFFSET),
        SHADER_MASK = MAKE_BIT_FLAG(SHADER_BIT_FLAG_OFFSET),
        ATTRIBUTE_NAMES_MASK = MAKE_BIT_FLAG(ATTRIBUTES_NAMES_BIT_FLAG_OFFSET),
        ATTRIBUTE_MASK = MAKE_BIT_FLAG(ATTRIBUTE_BIT_FLAG_OFFSET),
        TYPE_MASK = MAKE_BIT_FLAG(TYPE_BIT_FLAG_OFFSET),
        HAS_RANGE_MASK = MAKE_BIT_FLAG(HAS_RANGE_BIT_FLAG_OFFSET),
        RANGE_MASK = MAKE_BIT_FLAG(RANGE_BIT_FLAG_OFFSET),
        DESCRIPTION_MASK = MAKE_BIT_FLAG(DESCRIPTION_BIT_FLAG_OFFSET),
        IMPLEMENTATION_NAMES_MASK = 
            MAKE_BIT_FLAG(IMPLEMENTATION_NAMES_BIT_FLAG_OFFSET),
        IMPLEMENTATION_MASK = MAKE_BIT_FLAG(IMPLEMENTATION_BIT_FLAG_OFFSET),
        TEXTURE_MASK = MAKE_BIT_FLAG(TEXTURE_BIT_FLAG_OFFSET),
        ATTRIBUTE_IS_ARRAY_MASK = 
            MAKE_BIT_FLAG(ATTRIBUTE_IS_ARRAY_BIT_FLAG_OFFSET),
        FLAG_MASK_MAX

    };
    
    MStatus    ParseArguments(const MArgDatabase& argData);

    MStatus HandleInvalidFlagCombinations();

    MStatus GetShaderNames();
    MStatus GetShaderDescription();
    MStatus GetAttributeNames();
    MStatus GetAttributeDescription();
    MStatus GetAttributeType();
    MStatus GetAttributeHasRange();
    MStatus GetAttributeRange();
    MStatus GetImplementationNames();
    MStatus GetImplementationDescription();
    MStatus ReloadShaders();
    MStatus GetTextureAttributes();
    MStatus GetAttributesIsArray();

    MStatus ErrorPrinter(MString kMessage, std::string kErrorString);

    MStatus ErrorPrinterShader(MString kMessage, MString kShaderName, 
        std::string kErrorString);

    MStatus ErrorPrinterShaderAndAttribute(MString kMessage, 
        MString kShaderName, MString kAttributeName, std::string kErrorString);

    MStatus ErrorPrinterShaderAndImplementation(MString kMessage, 
        MString kShaderName, MString kImplementationName, 
        std::string kErrorString);

    const char* convertAttributeTypeToString(
        NiShaderAttributeDesc::AttributeType eType);

    NiUInt32 m_argFlags;

    MStringArray m_FlagArguments;

    static const Argument ms_ARGUMENTS[FLAG_OFFSET_MAX]; 

}; 

#pragma warning( pop )

#endif // SHADERDATA_H