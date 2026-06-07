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

#include "MayaPluginPCH.h"
#include "ShaderData.h"
#include "maya/MArgList.h"
#include "MyiPlugin.h"
#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include "ShaderInfoCollection.h"
#include "StatusChecking.h"
#include <maya/MSyntax.h>
#include <NiShaderAttributeDesc.h>

#define ARG_DEBUG (FALSE)

const ShaderData::Argument ShaderData::ms_ARGUMENTS[FLAG_OFFSET_MAX] = {
    {"-rl", "-reloadShaders", RELOAD_SHADERS_MASK, MSyntax::kNoArg},
    {"-sn", "-shaderNames", SHADER_NAMES_MASK, MSyntax::kNoArg},
    {"-s", "-shader", SHADER_MASK, MSyntax::kString},
    {"-atn", "-attributeNames", ATTRIBUTE_NAMES_MASK, MSyntax::kNoArg},
    {"-at", "-attribute", ATTRIBUTE_MASK, MSyntax::kString},
    {"-t", "-type", TYPE_MASK, MSyntax::kNoArg},
    {"-hr", "-hasRange", HAS_RANGE_MASK, MSyntax::kNoArg},
    {"-r", "-range", RANGE_MASK, MSyntax::kNoArg},
    {"-d", "-description", DESCRIPTION_MASK, MSyntax::kNoArg},
    {"-in", "-implementationNames", IMPLEMENTATION_NAMES_MASK, 
        MSyntax::kNoArg},
    {"-i", "-implementation", IMPLEMENTATION_MASK, MSyntax::kString},
    {"-tx", "-textures", TEXTURE_MASK, MSyntax::kNoArg}, 
    {"-ia", "-isArray", ATTRIBUTE_IS_ARRAY_MASK, MSyntax::kNoArg}
};


MSyntax ShaderData::newSyntax()
{
    MStatus kStat = MStatus::kSuccess;

    MSyntax syntax;

    for(NiUInt32 uiArgumentIndex = FLAG_OFFSET_FIRST; 
        uiArgumentIndex < FLAG_OFFSET_MAX; uiArgumentIndex++)
    {
        VERIFY_MSTATUS_AND_RETURN(syntax.addFlag(
            ms_ARGUMENTS[uiArgumentIndex].shortName,
            ms_ARGUMENTS[uiArgumentIndex].longName, 
            ms_ARGUMENTS[uiArgumentIndex].argType), syntax);
    }

    return syntax;
}


MStatus ShaderData::doIt( const MArgList& args )
{
#if ARG_DEBUG == TRUE
    MString argString;
    for ( unsigned int argIndex = 0; argIndex < args.length(); argIndex++ )
    {
        if ( argIndex  > 0 )
        {
            argString += " ";
        }

        argString += args.asString( argIndex );
    }

    argString += "\n";

    OutputDebugString( argString.asChar() );
#endif

    MStatus kStat = MStatus::kSuccess;
    
    MArgDatabase argData(syntax(), args, &kStat);
    if(kStat != MStatus::kSuccess)
    {
        kStat.perror(kStat.errorString());
    }

    ParseArguments(argData);

    redoIt();

    return kStat;

}

MStatus    ShaderData::redoIt()
{
    MStatus kStat = MStatus::kSuccess;

    //here we test for combinations of flags set during the initial call

    switch(m_argFlags)
    {
    case 0:
        displayError("No flags specified");
        return MStatus::kFailure;

    case SHADER_NAMES_MASK:
        kStat = GetShaderNames();
        break;
    
    case SHADER_MASK | DESCRIPTION_MASK:
        kStat = GetShaderDescription();
        break;

    case SHADER_MASK | ATTRIBUTE_NAMES_MASK:
        kStat = GetAttributeNames();
        break;

    case SHADER_MASK | ATTRIBUTE_MASK | DESCRIPTION_MASK:
        kStat = GetAttributeDescription();
        break;

    case SHADER_MASK | ATTRIBUTE_MASK | TYPE_MASK:
        kStat = GetAttributeType();
        break;

    case SHADER_MASK | ATTRIBUTE_MASK | HAS_RANGE_MASK:
        kStat = GetAttributeHasRange();
        break;

    case SHADER_MASK | ATTRIBUTE_MASK | RANGE_MASK:
        kStat = GetAttributeRange();
        break;

    case SHADER_MASK | IMPLEMENTATION_NAMES_MASK :
        kStat = GetImplementationNames();
        break;

    case SHADER_MASK | IMPLEMENTATION_MASK | DESCRIPTION_MASK :
        kStat = GetImplementationDescription();
        break;
    
    case RELOAD_SHADERS_MASK:
        kStat = ReloadShaders();
        break;
    
    case SHADER_MASK | TEXTURE_MASK :
        kStat = GetTextureAttributes();
        break;

    case SHADER_MASK | ATTRIBUTE_MASK | ATTRIBUTE_IS_ARRAY_MASK :
        kStat = GetAttributesIsArray();
        break;

    default:
        kStat = HandleInvalidFlagCombinations();
        break;
    }

    return kStat;
}

MStatus ShaderData::GetAttributeDescription()
{
    MStatus kStat = MStatus::kSuccess;
    std::string kErrorString;

    //return the type of the attribute
    MString kShaderName = m_FlagArguments[SHADER_BIT_FLAG_OFFSET];

    MString kAttributeName = m_FlagArguments[ATTRIBUTE_BIT_FLAG_OFFSET];

    const char* pcDescription = NULL;

    if(!ShaderInfoCollection::GetInstance()->GetShaderAttributeDescription(
        kShaderName.asChar(), kAttributeName.asChar(), pcDescription, 
        kErrorString))
    {
        return ErrorPrinterShaderAndAttribute("Couldn't determine if attribute"
            "is an array", kShaderName, kAttributeName, kErrorString);
    }
    
    setResult(pcDescription);

    return kStat;    
}

MStatus ShaderData::GetAttributesIsArray()
{
    MStatus kStat = MStatus::kSuccess;
    std::string kErrorString;

    //return the type of the attribute
    MString kShaderName = m_FlagArguments[SHADER_BIT_FLAG_OFFSET];

    MString kAttributeName = m_FlagArguments[ATTRIBUTE_BIT_FLAG_OFFSET];

    bool bIsArray = false;

    if(!ShaderInfoCollection::GetInstance()->GetShaderAttributeIsArray(
        kShaderName.asChar(), kAttributeName.asChar(), bIsArray, kErrorString))
    {
        return ErrorPrinterShaderAndAttribute("Couldn't determine if attribute"
            "is an array", kShaderName, kAttributeName, kErrorString);
    }
    
    setResult(bIsArray);

    return kStat;    
}

MStatus ShaderData::HandleInvalidFlagCombinations()
{

    MString kErrorString = "Invalid flag combination, with flags: ";

    for(NiUInt32 uiArgumentIndex = FLAG_OFFSET_FIRST; 
        uiArgumentIndex < FLAG_OFFSET_MAX; uiArgumentIndex++)
    {
        if(m_argFlags & ms_ARGUMENTS[uiArgumentIndex].bitFlag)
        {
            kErrorString += ms_ARGUMENTS[uiArgumentIndex].longName;
            kErrorString += ", ";
        }
    }
    
    kErrorString = kErrorString.substring(0, kErrorString.length() - 3);

    NIASSERT(!"Invalid flag combination");

    displayError(kErrorString);

    return MStatus::kFailure;

}

MStatus ShaderData::GetShaderNames()
{
    MStatus kStat = MStatus::kSuccess;

    //return all the shader names
    NiUInt32 uiNamesCount = 0;
    std::string kErrorString;
    if(!ShaderInfoCollection::GetInstance()->GetShaderCount(uiNamesCount,
        kErrorString))
    {
        return ErrorPrinter("Couldn't get shader count", kErrorString);
    }

    const char** ppcNames = new const char*[uiNamesCount];
    
    if(!ShaderInfoCollection::GetInstance()->GetShaderNames(ppcNames,
        uiNamesCount, kErrorString))
    {
        return ErrorPrinter("Couldn't get shader names", kErrorString);
    }

    MStringArray returnArray(ppcNames, uiNamesCount);
    setResult(returnArray);

    return kStat;
}

MStatus ShaderData::GetShaderDescription()
{
    MStatus kStat = MStatus::kSuccess;
    
    //return the shader with the description
    MString kShaderName = m_FlagArguments[SHADER_BIT_FLAG_OFFSET];
    
    std::string kErrorString;

    const char* pcDescription = NULL;
    if(!ShaderInfoCollection::GetInstance()->GetShaderDescription(
        kShaderName.asChar(), pcDescription, kErrorString))
    {
         return ErrorPrinterShader("Couldn't get shader description", 
            kShaderName, kErrorString);
    }


    setResult(MString(pcDescription));

    return kStat;
}

MStatus ShaderData::GetAttributeNames()
{
    MStatus kStat = MStatus::kSuccess;
    std::string kErrorString;

    //return all the names of the attributes 
    MString kShaderName = m_FlagArguments[SHADER_BIT_FLAG_OFFSET];
    
    NiUInt32 uiAttributesCount = 0;

    if(!ShaderInfoCollection::GetInstance()->GetShaderAttributeCount(
        kShaderName.asChar(), uiAttributesCount, kErrorString))
    {
         return ErrorPrinterShader("Couldn't get shader attribute count", 
            kShaderName, kErrorString);
    }

    const char** ppcAttributeNames = new const char*[uiAttributesCount];

    if(!ShaderInfoCollection::GetInstance()->GetShaderAttributeNames(
        kShaderName.asChar(), ppcAttributeNames, uiAttributesCount, 
        kErrorString))
    {
        return ErrorPrinterShader("Couldn't get attribute names", kShaderName, 
            kErrorString);
    }

    MStringArray returnArray(ppcAttributeNames, uiAttributesCount);
    setResult(returnArray);

    return kStat;
}

MStatus ShaderData::GetAttributeType()
{
    MStatus kStat = MStatus::kSuccess;
    std::string kErrorString;

    //return the type of the attribute
    MString kShaderName = m_FlagArguments[SHADER_BIT_FLAG_OFFSET];

    MString kAttributeName = m_FlagArguments[ATTRIBUTE_BIT_FLAG_OFFSET];

    NiShaderAttributeDesc::AttributeType kType = 
        NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;

    if(!ShaderInfoCollection::GetInstance()->GetShaderAttributeType(
        kShaderName.asChar(), kAttributeName.asChar(), kType, kErrorString))
    {
        return ErrorPrinterShaderAndAttribute("Couldn't get attribute type", 
            kShaderName, kAttributeName, kErrorString);
    }

    MString returnString(convertAttributeTypeToString(kType));
    setResult(returnString);

    return kStat;
}

MStatus ShaderData::GetAttributeHasRange()
{
    MStatus kStat = MStatus::kSuccess;
    std::string kErrorString;

    MString kShaderName = m_FlagArguments[SHADER_BIT_FLAG_OFFSET];

    MString kAttributeName = m_FlagArguments[ATTRIBUTE_BIT_FLAG_OFFSET];

    bool bHasMax = true;
    if(!ShaderInfoCollection::GetInstance()->GetShaderAttributeIsRanged(
        kShaderName.asChar(), kAttributeName.asChar(), bHasMax, kErrorString))
    {
        return ErrorPrinterShaderAndAttribute("Couldn't determine if attribute"
            " is ranged", kShaderName, kAttributeName, kErrorString);
    }

    setResult(bHasMax);

    return kStat;
}

MStatus ShaderData::GetAttributeRange()
{
    MStatus kStat = MStatus::kSuccess;
    std::string kErrorString;

    MString kShaderName = m_FlagArguments[SHADER_BIT_FLAG_OFFSET];

    MString kAttributeName = m_FlagArguments[ATTRIBUTE_BIT_FLAG_OFFSET];
    
    NiShaderAttributeDesc::AttributeType kType = 
        NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;

    if(!ShaderInfoCollection::GetInstance()->GetShaderAttributeType(
        kShaderName.asChar(), kAttributeName.asChar(), kType, kErrorString))
    {
        return ErrorPrinterShaderAndAttribute("Couldn't get attribute's type",
            kShaderName, kAttributeName, kErrorString);
    }

    switch(kType)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
        NIASSERT("Invalid Attribute Type");
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
        NIASSERT("Not Ranged!");
        break;

    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
    {
        NiUInt32 uiLow = 0;
        NiUInt32 uiHigh = 0;

        if(!ShaderInfoCollection::GetInstance()->GetRange_UnsignedInt(
            kShaderName.asChar(), kAttributeName.asChar(),
            uiLow, uiHigh, kErrorString))
        {
            return ErrorPrinterShaderAndAttribute("Couldn't get attribute's "
                "range", kShaderName, kAttributeName, kErrorString);
        }
                
        MIntArray returnArray;
        returnArray.append(uiLow);
        returnArray.append(uiHigh);

        setResult(returnArray);
    }
        break;

    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
    {
        float fLow = 0;
        float fHigh = 0;

        if(!ShaderInfoCollection::GetInstance()->GetRange_Float(
            kShaderName.asChar(), kAttributeName.asChar(),
            fLow, fHigh, kErrorString))
        {
            return ErrorPrinterShaderAndAttribute("Couldn't get attribute's "
                "range", kShaderName, kAttributeName, kErrorString);
        }
                
        MDoubleArray returnArray;
        returnArray.append(fLow);
        returnArray.append(fHigh);

        setResult(returnArray);
    }
        break;

    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
    {
        double dLowX, dLowY;
        double dHighX, dHighY;
        if(!ShaderInfoCollection::GetInstance()->GetRange_Point2(
            kShaderName.asChar(), kAttributeName.asChar(), 
            dLowX, dLowY, dHighX, dHighY, kErrorString))
        {
            return ErrorPrinterShaderAndAttribute("Couldn't get attribute's "
                "range", kShaderName, kAttributeName, kErrorString);
        }
                
        MDoubleArray returnArray;
        returnArray.append(dLowX);
        returnArray.append(dLowX);
        returnArray.append(dHighX);
        returnArray.append(dHighY);

        setResult(returnArray);
    }
        break;

    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
    {
        double dLowX, dLowY, dLowZ;
        double dHighX, dHighY, dHighZ;
        if(!ShaderInfoCollection::GetInstance()->GetRange_Point3(
            kShaderName.asChar(), kAttributeName.asChar(), 
            dLowX, dLowY, dLowZ, dHighX, dHighY, dHighZ, kErrorString))
        {
            return ErrorPrinterShaderAndAttribute("Couldn't get attribute's "
                "range", kShaderName, kAttributeName, kErrorString);
        }
                
        MDoubleArray returnArray;
        returnArray.append(dLowX);
        returnArray.append(dLowY);
        returnArray.append(dLowZ);
        returnArray.append(dHighX);
        returnArray.append(dHighY);
        returnArray.append(dHighZ);

        setResult(returnArray);
    }
        break;

    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:  
    {
        double lowR, lowG, lowB;
        double highR, highG, highB;
        
        if(!ShaderInfoCollection::GetInstance()->GetRange_Color(
            kShaderName.asChar(), kAttributeName.asChar(), lowR, lowG, 
            lowB, highR, highG, highB, kErrorString))
        {
            return ErrorPrinterShaderAndAttribute("Couldn't get attribute's "
                "range", kShaderName, kAttributeName, kErrorString);
        }
                
        MDoubleArray returnArray;
        returnArray.append(lowR);
        returnArray.append(lowG);
        returnArray.append(lowB);
        returnArray.append(highR);
        returnArray.append(highG);
        returnArray.append(highB);

        setResult(returnArray);
    }
        break;

    default:
        NIASSERT("Invalid Attribute Type");
        displayError("Invalid Attribute Type");
        return MStatus::kFailure;
    }

    return kStat;
}

MStatus ShaderData::GetImplementationNames()
{
    MStatus kStat = MStatus::kSuccess;
    std::string kErrorString;

    //return all the names of the implementations

    MString kShaderName = m_FlagArguments[SHADER_BIT_FLAG_OFFSET];

    NiUInt32 uiImplemenationCount = 0;

    ShaderInfoCollection::GetInstance()->GetShaderImplemenationCount(
        kShaderName.asChar(), uiImplemenationCount, kErrorString);

    const char** ppcImplemenationNames = 
        new const char*[uiImplemenationCount];

    ShaderInfoCollection::GetInstance()->GetShaderImplementationNames(
        kShaderName.asChar(), ppcImplemenationNames, uiImplemenationCount, 
        kErrorString);

    MStringArray returnArray(ppcImplemenationNames, uiImplemenationCount);

    setResult(returnArray);

    return kStat;
}

MStatus ShaderData::GetImplementationDescription()
{
    MStatus kStat = MStatus::kSuccess;
    std::string kErrorString;

    MString kShaderName = m_FlagArguments[SHADER_BIT_FLAG_OFFSET];

    MString kImplementationName = 
        m_FlagArguments[IMPLEMENTATION_BIT_FLAG_OFFSET];

    const char* pcDescription = NULL;
    
    ShaderInfoCollection::GetInstance()->GetShaderImplementationDescription(
        kShaderName.asChar(), kImplementationName.asChar(), pcDescription, 
        kErrorString);

    setResult(pcDescription);

    return kStat;
}

MStatus ShaderData::ReloadShaders()
{
    MStatus kStat = MStatus::kSuccess;
    std::string kErrorString;

    NiMaterialToolkit::UnloadShaders();

    NiMaterialToolkit::ReloadShaders();

    if(!ShaderInfoCollection::GetInstance()->Reload(kErrorString))
    {
        ErrorPrinter("Shader Reload failed", kErrorString);
    }

    setResult("Shaders Reloaded.");

    return kStat;
}

MStatus ShaderData::GetTextureAttributes()
{
    MStatus kStat = MStatus::kSuccess;

    std::string kErrorString;

    MString kShaderName = m_FlagArguments[SHADER_BIT_FLAG_OFFSET];

    NiUInt32 uiTextureCount = 0;

    if(!ShaderInfoCollection::GetInstance()->GetTextureCount(
        kShaderName.asChar(), uiTextureCount, kErrorString))
    {
         return ErrorPrinterShader("Couldn't get texture attributes count", 
            kShaderName, kErrorString);
    }

    const char** ppcTextureNames = new const char*[uiTextureCount];

    if(!ShaderInfoCollection::GetInstance()->GetTextureNames(
        kShaderName.asChar(), ppcTextureNames, uiTextureCount, kErrorString))
    {
         return ErrorPrinterShader("Couldn't get texture attributes", 
            kShaderName, kErrorString);
    }

    MStringArray returnArray(ppcTextureNames, uiTextureCount);

    setResult(returnArray);

    return kStat;
}

MStatus    ShaderData::ParseArguments(const MArgDatabase& argData)
{
    MStatus kStat = MStatus::kSuccess;

    //this is to hold which arguments are provided
    m_argFlags = 0;

    m_FlagArguments.setLength(FLAG_OFFSET_MAX);

    for(NiUInt32 uiArgumentIndex = FLAG_OFFSET_FIRST; 
        uiArgumentIndex < FLAG_OFFSET_MAX; uiArgumentIndex++)
    {
        bool isFlagSet = argData.isFlagSet(
            ms_ARGUMENTS[uiArgumentIndex].shortName, &kStat);
        if(kStat != MStatus::kSuccess)
        {
            MString kErrorString = "Unable to determine if flag \"";
            kErrorString += ms_ARGUMENTS[uiArgumentIndex].longName;
            kErrorString += "\" is set.";
            NIASSERT(!"Unable to determine if flag is set");
        }

        m_argFlags |= static_cast<NiUInt32>(isFlagSet) << uiArgumentIndex;
        
        if(isFlagSet && ms_ARGUMENTS[uiArgumentIndex].argType != 
            MSyntax::kNoArg)
        {
            MString kFlagArgument; 
            kStat = argData.getFlagArgument(
                ms_ARGUMENTS[uiArgumentIndex].shortName, 0, kFlagArgument);
            if(kStat != MStatus::kSuccess)
            {
                MString kErrorString = "Can't retrieve argument for flag \"";
                kErrorString += ms_ARGUMENTS[uiArgumentIndex].longName;
                kErrorString += "\"";
                NIASSERT(!"Can't retrieve argument for flag");
            }

            m_FlagArguments.set(kFlagArgument, uiArgumentIndex);
        }
       
    }

    return kStat;
}

const char* ShaderData::convertAttributeTypeToString(
    NiShaderAttributeDesc::AttributeType eType)
{
    const char* pcTypeName = NULL;

    switch(eType)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
        pcTypeName = "Undefined";
        break;

    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
        pcTypeName = "Boolean";
        break;

    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
        pcTypeName = "String";
        break;

    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        pcTypeName = "UnsignedInt";
        break;

    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        pcTypeName = "Float";
        break;

    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        pcTypeName = "Point2";
        break;

    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        pcTypeName = "Point3";
        break;

    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        pcTypeName = "Point4";
        break;

    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3: 
        pcTypeName = "Matrix3";
        break;

    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:   
        pcTypeName = "Matrix4";    
        break;

    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:  
        pcTypeName = "Color";
        break;

    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:     
        pcTypeName = "Texture";    
        break;
    
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT8:    
        pcTypeName = "Float8";    
        break;

    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12:    
        pcTypeName = "Float12";    
        break;

    default:
    {
        pcTypeName = "Type Invalid";
        NIASSERT(!"Type Invalid");
    }
    }

    return pcTypeName;
}   

MStatus ShaderData::ErrorPrinter(MString kMessage, std::string kErrorString)
{
    MString kDisplayErrorString = kMessage + "\" for the following reason:" + 
        kErrorString.c_str();
    displayError(kDisplayErrorString);
    NIASSERT(!"ErrorPrinter");
    return MStatus::kFailure;

}

MStatus ShaderData::ErrorPrinterShader(MString kMessage, MString kShaderName, 
        std::string kErrorString)
{
    MString kDisplayErrorString = kMessage + " for shader \"" + kShaderName; 
    return ErrorPrinter(kMessage, kErrorString);
}

MStatus ShaderData::ErrorPrinterShaderAndAttribute(MString kMessage, 
    MString kShaderName, MString kAttributeName, std::string kErrorString)
{
    MString kDisplayErrorString = kMessage + " for shader \"" + kShaderName;
    kDisplayErrorString += " and attribute \"" + kAttributeName + "\""; 
    return ErrorPrinter(kMessage, kErrorString);
}

MStatus ShaderData::ErrorPrinterShaderAndImplementation(MString kMessage,  
    MString kShaderName, MString kImplementationName, std::string kErrorString)
{
    MString kDisplayErrorString = kMessage + " for shader \"" + kShaderName;
    kDisplayErrorString += " and implementation \"" + kImplementationName + 
        "\""; 
    return ErrorPrinter(kMessage, kErrorString);
}

MStatus ShaderData::undoIt()
{
    MStatus stat = MStatus::kSuccess;


    return stat;
}

bool ShaderData::isUndoable() const
{
    return false;
}

void* ShaderData::creator()
{
    return new ShaderData();
}

