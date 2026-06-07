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
#include "ShaderInfoCollection.h"

ShaderInfoCollection* ShaderInfoCollection::ms_Instance;

ShaderInfoCollection* ShaderInfoCollection::GetInstance()
{
    if(ms_Instance == NULL)
    {
        ms_Instance = new ShaderInfoCollection;
        ms_Instance->Load();
    }
    
    return ms_Instance;
}

void ShaderInfoCollection::Initialize()
{
    ms_Instance = new ShaderInfoCollection;
    ms_Instance->Load();
}

bool ShaderInfoCollection::Load()
{
        // Starup the Pixel Shaders
    NiMaterialToolkit::CreateToolkit();

    char acShaderPath[MAX_PATH];
#if defined(_MSC_VER) && _MSC_VER >= 1400
    NiUInt32 uiLen = 0;
    getenv_s(&uiLen, acShaderPath, MAX_PATH, "EGB_SHADER_LIBRARY_PATH");
    NIASSERT(uiLen <= MAX_PATH);
#else
    NiStrcpy(acShaderPath, MAX_PATH, getenv("EGB_SHADER_LIBRARY_PATH"));
#endif
    bool bShadersLoaded = NiMaterialToolkit::GetToolkit()->LoadFromDLL(
        acShaderPath);
    NI_UNUSED_ARG(bShadersLoaded);

    NiStrcat(acShaderPath, MAX_PATH, "\\Data");

    NiMaterialToolkit::GetToolkit()->SetMaterialDirectory(acShaderPath, true);

    std::string kErrorString;

    bool bReturnValue = Reload(kErrorString);

    return bReturnValue;

}

bool ShaderInfoCollection::Reload(std::string& kErrorString)
{
    NI_UNUSED_ARG(kErrorString);
    bool bReturnValue = true;

    m_NiShaderDescMap.clear();

    NiMaterialToolkit* pkToolkit = NiMaterialToolkit::GetToolkit();

    unsigned int uiLLoop;
    for (uiLLoop = 0; uiLLoop < pkToolkit->GetLibraryCount(); uiLLoop++)
    {
        NiMaterialLibrary* pkLibrary = pkToolkit->GetLibraryAt(uiLLoop);

        // Loop through all shaders in a library

        const NiShaderDesc* pkDesc = pkLibrary->GetFirstMaterialDesc();
        while (pkDesc)
        {
            
            m_NiShaderDescMap.insert(std::pair<std::string, 
                const NiShaderDesc*>(pkDesc->GetName(), pkDesc));
            pkDesc = pkLibrary->GetNextMaterialDesc();
        }
    }

    return bReturnValue;
}

bool ShaderInfoCollection::GetTextureNames(const char* pcShaderName, 
    const char** ppcTextureNames, NiUInt32 uiCount, std::string& kErrorString)
{
    NI_UNUSED_ARG(uiCount);
    bool bReturnValue = true;

    NiShaderDescMap::iterator kShaderDescMapIter = 
        m_NiShaderDescMap.find(pcShaderName);

    if(kShaderDescMapIter == m_NiShaderDescMap.end())
    {
        NIASSERT("Couldn't find shader!");
        kErrorString = "No Shader with name \"";
        kErrorString += pcShaderName;
        kErrorString += "\"";
        return false;
    }

    const NiShaderAttributeDesc* kNiShaderAttributeDesc = 
        kShaderDescMapIter->second->GetFirstAttribute();

    NiUInt32 uiAttributeIndex = 0;

    while(kNiShaderAttributeDesc != NULL)
    {
        if(kNiShaderAttributeDesc->GetType() == 
            NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE)
        {
            ppcTextureNames[uiAttributeIndex] = 
                kNiShaderAttributeDesc->GetName();
            kNiShaderAttributeDesc = 
                kShaderDescMapIter->second->GetNextAttribute();
            uiAttributeIndex++;
        }
    }

    return bReturnValue;
}

bool ShaderInfoCollection::GetTextureCount(const char* pcShaderName, 
    NiUInt32& uiCount, std::string& kErrorString)
{
    bool bReturnValue = true;

    NiShaderDescMap::iterator kShaderDescMapIter = 
        m_NiShaderDescMap.find(pcShaderName);

    if(kShaderDescMapIter == m_NiShaderDescMap.end())
    {
        NIASSERT("Couldn't find shader!");
        kErrorString = "No Shader with name \"";
        kErrorString += pcShaderName;
        kErrorString += "\"";
        return false;
    }
    

    const NiShaderAttributeDesc* kNiShaderAttributeDesc = 
        kShaderDescMapIter->second->GetFirstAttribute();

    while(kNiShaderAttributeDesc != NULL)
    {
        if(kNiShaderAttributeDesc->GetType() == 
            NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE)
        {
            kNiShaderAttributeDesc = 
                kShaderDescMapIter->second->GetNextAttribute();
            uiCount++;
        }
    }

    return bReturnValue;
}

void ShaderInfoCollection::Shutdown()
{
    delete ms_Instance;
}

bool ShaderInfoCollection::GetShaderCount(NiUInt32& uiShaderCount, 
    std::string& kErrorString)
{
    NI_UNUSED_ARG(kErrorString);
    bool bReturnValue = true;

    uiShaderCount = (NiUInt32)m_NiShaderDescMap.size();

    return bReturnValue;
}

bool ShaderInfoCollection::GetShaderNames(const char** ppcShaderNames, 
    NiUInt32 uiSize, std::string& kErrorString)
{
    bool bReturnValue = true;

    if(uiSize != m_NiShaderDescMap.size())
    {
        NIASSERT(!"uiSize != m_NiShaderDescMap.size()");
        kErrorString = "Shader name buffer is the wrong size.";
        return false;
    }

    NiUInt32 uiShaderNameIndex = 0;

    for(NiShaderDescMap::iterator kShaderDescMapIter = 
        m_NiShaderDescMap.begin(); kShaderDescMapIter != 
        m_NiShaderDescMap.end(); kShaderDescMapIter++, uiShaderNameIndex++)
    {  
        ppcShaderNames[uiShaderNameIndex] = kShaderDescMapIter->first.c_str();
    }

    return bReturnValue;
}

bool ShaderInfoCollection::GetShaderDescription(
    const char* pcShaderName, const char*& pcDescription, 
    std::string& kErrorString)
{
    bool bReturnValue = true;

    NiShaderDescMap::iterator kShaderDescMapIter = 
        m_NiShaderDescMap.find(pcShaderName);

    if(kShaderDescMapIter == m_NiShaderDescMap.end())
    {
        NIASSERT("Couldn't find shader!");
        kErrorString = "No Shader with name \"";
        kErrorString += pcShaderName;
        kErrorString += "\"";
        return false;
    }

    pcDescription = kShaderDescMapIter->second->GetDescription();

    return bReturnValue;
}

bool ShaderInfoCollection::GetShaderAttributeCount(
    const char* pcShaderName, NiUInt32& uiAttributeCount, 
    std::string& kErrorString)
{
    bool bReturnValue = true;

    NiShaderDescMap::iterator kShaderDescMapIter = 
        m_NiShaderDescMap.find(pcShaderName);

    if(kShaderDescMapIter == m_NiShaderDescMap.end())
    {
        NIASSERT("Couldn't find shader!");
        kErrorString = "No Shader with name \"";
        kErrorString += pcShaderName;
        kErrorString += "\"";
        return false;
    }
    
    uiAttributeCount = kShaderDescMapIter->second->GetNumberOfAttributes();
    
    return bReturnValue;
}

bool ShaderInfoCollection::GetShaderAttributeNames(const char* pcShaderName, 
    const char** ppcAttributeNames, NiUInt32 uiCount, 
    std::string& kErrorString)
{
    bool bReturnValue = true;

    NiShaderDescMap::iterator kShaderDescMapIter = 
        m_NiShaderDescMap.find(pcShaderName);

    if(kShaderDescMapIter == m_NiShaderDescMap.end())
    {
        NIASSERT("Couldn't find shader!");
        kErrorString = "No Shader with name \"";
        kErrorString += pcShaderName;
        kErrorString += "\"";
        return false;
    }

    if(uiCount != kShaderDescMapIter->second->GetNumberOfAttributes())
    {
        NIASSERT(!"uiCount != kShaderDescMapIter->second->"
            "GetNumberOfAttributes()");
        kErrorString = "Attribute name buffer is the wrong size.";
        return false;
    }
    
    const NiShaderAttributeDesc* kNiShaderAttributeDesc = 
        kShaderDescMapIter->second->GetFirstAttribute();

    NiUInt32 uiAttributeIndex = 0;

    while(kNiShaderAttributeDesc != NULL)
    {
        ppcAttributeNames[uiAttributeIndex] = 
            kNiShaderAttributeDesc->GetName();
        kNiShaderAttributeDesc = 
            kShaderDescMapIter->second->GetNextAttribute();
        uiAttributeIndex++;
    }

    return bReturnValue;

}

bool ShaderInfoCollection::GetRange_UnsignedInt(const char* pcShaderName, 
    const char* pcAttributeName, NiUInt32& uiLow, NiUInt32& uiHigh, 
    std::string& kErrorString)
{
    bool bReturnValue = true;

    NiShaderDescMap::iterator kShaderDescMapIter = 
        m_NiShaderDescMap.find(pcShaderName);

    if(kShaderDescMapIter == m_NiShaderDescMap.end())
    {
        NIASSERT("Couldn't find shader!");
        kErrorString = "No Shader with name \"";
        kErrorString += pcShaderName;
        kErrorString += "\"";
        return false;
    }

    const NiShaderAttributeDesc* kNiShaderAttributeDesc = 
        kShaderDescMapIter->second->GetAttribute(pcAttributeName);

    kNiShaderAttributeDesc->GetRange_UnsignedInt(uiLow, uiHigh);

    return bReturnValue;
}

bool ShaderInfoCollection::GetRange_Float(const char* pcShaderName, 
    const char* pcAttributeName,float& fLow, float& fHigh, 
    std::string& kErrorString)
{
    bool bReturnValue = true;

    NiShaderDescMap::iterator kShaderDescMapIter = 
        m_NiShaderDescMap.find(pcShaderName);

    if(kShaderDescMapIter == m_NiShaderDescMap.end())
    {
        NIASSERT("Couldn't find shader!");
        kErrorString = "No Shader with name \"";
        kErrorString += pcShaderName;
        kErrorString += "\"";
        return false;
    }

    const NiShaderAttributeDesc* kNiShaderAttributeDesc = 
        kShaderDescMapIter->second->GetAttribute(pcAttributeName);

    kNiShaderAttributeDesc->GetRange_Float(fLow, fHigh);

    return bReturnValue;
}

bool ShaderInfoCollection::GetRange_Point2(const char* pcShaderName, 
    const char* pcAttributeName, double& lowX, double& lowY, double& highX, 
        double& highY, std::string& kErrorString)
{
    bool bReturnValue = true;

    NiShaderDescMap::iterator kShaderDescMapIter = 
        m_NiShaderDescMap.find(pcShaderName);

    if(kShaderDescMapIter == m_NiShaderDescMap.end())
    {
        NIASSERT("Couldn't find shader!");
        kErrorString = "No Shader with name \"";
        kErrorString += pcShaderName;
        kErrorString += "\"";
        return false;
    }

    const NiShaderAttributeDesc* kNiShaderAttributeDesc = 
        kShaderDescMapIter->second->GetAttribute(pcAttributeName);

    NiPoint2 kPt2Low;
    NiPoint2 kPt2High;

    kNiShaderAttributeDesc->GetRange_Point2(kPt2Low, kPt2High);

    lowX = kPt2Low.x;
    lowY = kPt2Low.y;

    highX = kPt2High.x;
    highY = kPt2High.y;

    return bReturnValue;
}

bool ShaderInfoCollection::GetRange_Point3(const char* pcShaderName, 
    const char* pcAttributeName, double& lowX, double& lowY, double& lowZ, 
        double& highX, double& highY, double& highZ, 
        std::string& kErrorString)
{
    bool bReturnValue = true;

    NiShaderDescMap::iterator kShaderDescMapIter = 
        m_NiShaderDescMap.find(pcShaderName);

    if(kShaderDescMapIter == m_NiShaderDescMap.end())
    {
        NIASSERT("Couldn't find shader!");
        kErrorString = "No Shader with name \"";
        kErrorString += pcShaderName;
        kErrorString += "\"";
        return false;
    }

    const NiShaderAttributeDesc* kNiShaderAttributeDesc = 
        kShaderDescMapIter->second->GetAttribute(pcAttributeName);

    NiPoint3 kPtLow;
    NiPoint3 kPtHigh;

    kNiShaderAttributeDesc->GetRange_Point3(kPtLow, kPtHigh);

    lowX = kPtLow.x;
    lowY = kPtLow.y;
    lowZ = kPtLow.z;

    highX = kPtHigh.x;
    highY = kPtHigh.y;
    highZ = kPtHigh.z;

    return bReturnValue;
}

bool ShaderInfoCollection::GetRange_Floats(const char* pcShaderName, 
    const char* pcAttributeName,unsigned int uiCount, float* pafLow,
        float* pafHigh, std::string& kErrorString)
{
    bool bReturnValue = true;

    NiShaderDescMap::iterator kShaderDescMapIter = 
        m_NiShaderDescMap.find(pcShaderName);

    if(kShaderDescMapIter == m_NiShaderDescMap.end())
    {
        NIASSERT("Couldn't find shader!");
        kErrorString = "No Shader with name \"";
        kErrorString += pcShaderName;
        kErrorString += "\"";
        return false;
    }

    const NiShaderAttributeDesc* kNiShaderAttributeDesc = 
        kShaderDescMapIter->second->GetAttribute(pcAttributeName);

    kNiShaderAttributeDesc->GetRange_Floats(uiCount, pafLow, pafHigh);

    return bReturnValue;
}


bool ShaderInfoCollection::GetRange_Color(const char* pcShaderName, 
    const char* pcAttributeName, double& lowR, double& lowG, double& lowB, 
    double& highR, double& highG, double& highB, std::string& kErrorString)
{
    bool bReturnValue = true;

    NiShaderDescMap::iterator kShaderDescMapIter = 
        m_NiShaderDescMap.find(pcShaderName);

    if(kShaderDescMapIter == m_NiShaderDescMap.end())
    {
        NIASSERT("Couldn't find shader!");
        kErrorString = "No Shader with name \"";
        kErrorString += pcShaderName;
        kErrorString += "\"";
        return false;
    }

    const NiShaderAttributeDesc* kNiShaderAttributeDesc = 
        kShaderDescMapIter->second->GetAttribute(pcAttributeName);

    NiColor kClrLow;
    NiColor kClrHigh;

    kNiShaderAttributeDesc->GetRange_Color(kClrLow, kClrHigh);

    lowR = kClrLow.r;
    lowG = kClrLow.b;
    lowB = kClrLow.g;

    highR = kClrHigh.r;
    highG = kClrHigh.b;
    highB = kClrHigh.g;

    return bReturnValue;
}

bool ShaderInfoCollection::GetRange_ColorA(const char* pcShaderName, 
    const char* pcAttributeName, double& lowR, double& lowG, double& lowB, 
    double& lowA, double& highR, double& highG, double& highB, double& highA, 
    std::string& kErrorString)
{
    bool bReturnValue = true;

    NiShaderDescMap::iterator kShaderDescMapIter = 
        m_NiShaderDescMap.find(pcShaderName);

    if(kShaderDescMapIter == m_NiShaderDescMap.end())
    {
        NIASSERT("Couldn't find shader!");
        kErrorString = "No Shader with name \"";
        kErrorString += pcShaderName;
        kErrorString += "\"";
        return false;
    }
    
    const NiShaderAttributeDesc* kNiShaderAttributeDesc = 
        kShaderDescMapIter->second->GetAttribute(pcAttributeName);

    NiColorA kClrLow;
    NiColorA kClrHigh;

    kNiShaderAttributeDesc->GetRange_ColorA(kClrLow, kClrHigh);

    lowR = kClrLow.r;
    lowG = kClrLow.b;
    lowB = kClrLow.g;
    lowA = kClrLow.a;

    highR = kClrHigh.r;
    highG = kClrHigh.b;
    highB = kClrHigh.g;
    highA = kClrHigh.a;

    return bReturnValue;
}

bool ShaderInfoCollection::GetShaderAttributeType(const char* pcShaderName, 
    const char* pcAttributeName, 
    NiShaderAttributeDesc::AttributeType& kAttributeType, 
    std::string& kErrorString)
{
    bool bReturnValue = true;

    NiShaderDescMap::iterator kShaderDescMapIter = 
        m_NiShaderDescMap.find(pcShaderName);

    if(kShaderDescMapIter == m_NiShaderDescMap.end())
    {
        NIASSERT("Couldn't find shader!");
        kErrorString = "No Shader with name \"";
        kErrorString += pcShaderName;
        kErrorString += "\"";
        return false;
    }

    const NiShaderAttributeDesc* kNiShaderAttributeDesc = 
        kShaderDescMapIter->second->GetAttribute(pcAttributeName);
    
    kAttributeType = kNiShaderAttributeDesc->GetType();
    if(kAttributeType == NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY)
    {
        NiUInt32 uiElementSize, uiNumElements;
        kNiShaderAttributeDesc->GetArrayParams(kAttributeType,
            uiElementSize, uiNumElements);
    }

    return bReturnValue;
}

bool ShaderInfoCollection::GetShaderAttributeIsRanged(
    const char* pcShaderName, const char* pcAttributeName, 
    bool& bRanged, std::string& kErrorString)
{
    bool bReturnValue = true;

    NiShaderDescMap::iterator kShaderDescMapIter = 
        m_NiShaderDescMap.find(pcShaderName);

    if(kShaderDescMapIter == m_NiShaderDescMap.end())
    {
        NIASSERT("Couldn't find shader!");
        kErrorString = "No Shader with name \"";
        kErrorString += pcShaderName;
        kErrorString += "\"";
        return false;
    }

    const NiShaderAttributeDesc* kNiShaderAttributeDesc = 
        kShaderDescMapIter->second->GetAttribute(pcAttributeName);
    
    bRanged = kNiShaderAttributeDesc->IsRanged();

    return bReturnValue;
}

bool ShaderInfoCollection::GetShaderImplemenationCount(
    const char* pcShaderName, NiUInt32& uiImplementationCount, 
    std::string& kErrorString)
{
    bool bReturnValue = true;

    NiShaderDescMap::iterator kShaderDescMapIter = 
        m_NiShaderDescMap.find(pcShaderName);

    if(kShaderDescMapIter == m_NiShaderDescMap.end())
    {
        NIASSERT("Couldn't find shader!");
        kErrorString = "No Shader with name \"";
        kErrorString += pcShaderName;
        kErrorString += "\"";
        return false;
    }

    uiImplementationCount = 
        kShaderDescMapIter->second->GetNumberOfImplementations();

    return bReturnValue;
}   

bool ShaderInfoCollection::GetShaderImplementationNames(
    const char* pcShaderName, const char** ppcImplementationNames, 
    NiUInt32 uiImplementationCount, std::string& kErrorString)
{
    bool bReturnValue = true;

    NiShaderDescMap::iterator kShaderDescMapIter = 
        m_NiShaderDescMap.find(pcShaderName);

    if(kShaderDescMapIter == m_NiShaderDescMap.end())
    {
        NIASSERT("Couldn't find shader!");
        kErrorString = "No Shader with name \"";
        kErrorString += pcShaderName;
        kErrorString += "\"";
        return false;
    }

    NiUInt32 uiRealCount = 
        kShaderDescMapIter->second->GetNumberOfImplementations();

    if(uiImplementationCount != uiRealCount)
    {
        NIASSERT(!"uiImplementationCount != uiRealCount");
        kErrorString = "Implementation name buffer is the wrong size.";
        return false;
    }

    for(unsigned int uiImplemenationIndex = 0; 
        uiImplemenationIndex < uiImplementationCount; uiImplemenationIndex++)
    {
        const NiShaderRequirementDesc* pkNiShaderRequirementDesc = 
            kShaderDescMapIter->second->GetImplementationDescription(
            uiImplemenationIndex);

        ppcImplementationNames[uiImplemenationIndex] = 
            pkNiShaderRequirementDesc->GetName();
            
    }

    return bReturnValue;
}

bool ShaderInfoCollection::GetShaderImplementationDescription(
        const char* pcShaderName, const char* pcImplementationName, 
        const char*& pcImplemenationDescription, std::string& kErrorString)
{
    bool bReturnValue = true;

    NiShaderDescMap::iterator kShaderDescMapIter = 
        m_NiShaderDescMap.find(pcShaderName);

    if(kShaderDescMapIter == m_NiShaderDescMap.end())
    {
        NIASSERT("Couldn't find shader!");
        kErrorString = "No Shader with name \"";
        kErrorString += pcShaderName;
        kErrorString += "\"";
        return false;
    }

    NiUInt32 uiImplementationCount = 
        kShaderDescMapIter->second->GetNumberOfImplementations();

    for(unsigned int uiImplemenationIndex = 0; 
        uiImplemenationIndex < uiImplementationCount; uiImplemenationIndex++)
    {
        const NiShaderRequirementDesc* pkNiShaderRequirementDesc = 
            kShaderDescMapIter->second->GetImplementationDescription(
            uiImplemenationIndex);

        const char* pcCurrentImplemenationName = 
            pkNiShaderRequirementDesc->GetName();

        if(strncmp(pcImplementationName, pcCurrentImplemenationName, 256))
        {
            pcImplemenationDescription = 
                pkNiShaderRequirementDesc->GetDescription();
            break;
        }
    }

    return bReturnValue;
}

bool ShaderInfoCollection::GetShaderAttributeIsArray(const char* pcShaderName, 
        const char* pcAttributeName, bool& bIsArray, 
        std::string& kErrorString)
{
    bool bReturnValue = true;

    NiShaderDescMap::iterator kShaderDescMapIter = 
        m_NiShaderDescMap.find(pcShaderName);

    if(kShaderDescMapIter == m_NiShaderDescMap.end())
    {
        NIASSERT("Couldn't find shader!");
        kErrorString = "No Shader with name \"";
        kErrorString += pcShaderName;
        kErrorString += "\"";
        return false;
    }

    const NiShaderAttributeDesc* kNiShaderAttributeDesc = 
        kShaderDescMapIter->second->GetAttribute(pcAttributeName);
    
    NiShaderAttributeDesc::AttributeType kType = 
        kNiShaderAttributeDesc->GetType();

    if(kType == NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY)
    {
        bIsArray = true;
    }

    return bReturnValue;
}


bool ShaderInfoCollection::GetShaderAttributeDescription(
    const char* pcShaderName, const char* pcAttributeName, 
    const char*& pcDescription, std::string& kErrorString)
{
    bool bReturnValue = true;

    NiShaderDescMap::iterator kShaderDescMapIter = 
        m_NiShaderDescMap.find(pcShaderName);

    if(kShaderDescMapIter == m_NiShaderDescMap.end())
    {
        NIASSERT("Couldn't find shader!");
        kErrorString = "No Shader with name \"";
        kErrorString += pcShaderName;
        kErrorString += "\"";
        return false;
    }

    const NiShaderAttributeDesc* kNiShaderAttributeDesc = 
        kShaderDescMapIter->second->GetAttribute(pcAttributeName);
    
    pcDescription = kNiShaderAttributeDesc->GetDescription();

    return bReturnValue;
}