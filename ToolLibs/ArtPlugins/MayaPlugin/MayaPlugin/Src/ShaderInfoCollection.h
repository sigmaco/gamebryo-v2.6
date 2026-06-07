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

#ifndef SHADERINFOCOLLECTION_H
#define SHADERINFOCOLLECTION_H

#include <map>
#include <string>

class ShaderInfoCollection
{
public:

    typedef std::map<std::string, const NiShaderDesc*> NiShaderDescMap;

    static void Initialize();
    static void Shutdown();

    static ShaderInfoCollection* GetInstance();

    bool Reload(std::string& kErrorString);

    bool GetShaderCount(NiUInt32& uiShaderCount, std::string& kErrorString);

    bool GetShaderNames(const char** ppcShaderNames, NiUInt32 uiSize, 
        std::string& kErrorString);

    bool GetShaderDescription(const char* pcName, const char*& pcDescription, 
        std::string& kErrorString);

    bool GetShaderAttributeCount(const char* pcName, 
        NiUInt32& uiAttributeCount, std::string& kErrorString);

    bool GetShaderAttributeNames(const char* pcShaderName, 
        const char** ppcAttributeNames, NiUInt32 uiCount, 
        std::string& kErrorString);
    
    bool GetShaderAttributeType(const char* pcShaderName, 
        const char* pcAttributeName, 
        NiShaderAttributeDesc::AttributeType& kAttributeType,
        std::string& kErrorString);

    bool GetShaderAttributeIsRanged(const char* pcShaderName, 
        const char* pcAttributeName, bool& bRanged, 
        std::string& kErrorString);

    bool GetRange_UnsignedInt(const char* pcShaderName, 
        const char* pcAttributeName, NiUInt32& uiLow, NiUInt32& uiHigh, 
        std::string& kErrorString);

    bool GetRange_Float(const char* pcShaderName, 
        const char* pcAttributeName,float& fLow, float& fHigh, 
        std::string& kErrorString);

    bool GetRange_Point2(const char* pcShaderName, 
        const char* pcAttributeName, double& lowX, 
        double& lowY, double& highX, 
        double& highY, std::string& kErrorString);

    bool GetRange_Point3(const char* pcShaderName, 
        const char* pcAttributeName, double& lowX, 
        double& lowY, double& lowZ, 
        double& highX, double& highY, double& highZ, 
        std::string& kErrorString);

    bool GetRange_Floats(const char* pcShaderName, 
        const char* pcAttributeName,
        unsigned int uiCount, float* pafLow, float* pafHigh, 
        std::string& kErrorString);

    bool GetRange_Color(const char* pcShaderName, 
        const char* pcAttributeName,
        double& lowR, double& lowG, double& lowB, 
        double& highR, double& highG, 
        double& highB, std::string& kErrorString);

    bool GetRange_ColorA(const char* pcShaderName, 
        const char* pcAttributeName, 
        double& lowR, double& lowG, double& lowB, 
        double& lowA, double& highR, 
        double& highG, double& highB, double& highA, 
        std::string& kErrorString);

    bool GetShaderImplemenationCount(const char* pcShaderName, 
        NiUInt32& uiImplementationCount, std::string& kErrorString);

    bool GetShaderImplementationNames(const char* pcShaderName, 
        const char** ppcImplementationNames, NiUInt32 uiImplementationCount, 
        std::string& kErrorString);

    bool GetShaderImplementationDescription(
        const char* pcShaderName, const char* pcImplementationName, 
        const char*& pcImplementationDescription, std::string& kErrorString);

    bool GetTextureNames(const char* pcShaderName, 
        const char** ppcTextureNames, NiUInt32 uiCount, 
        std::string& kErrorString);

    bool GetTextureCount(const char* pcShaderName, NiUInt32& textureCount,
        std::string& kErrorString);

    bool GetShaderAttributeIsArray(const char* pcShaderName, 
        const char* pcAttributeName, bool& bIsArray, 
        std::string& kErrorString);

    bool GetShaderAttributeDescription(const char* pcShaderName, 
        const char* pcAttributeName, const char*& pcDescription, 
        std::string& kErrorString);

private:

    static ShaderInfoCollection* ms_Instance;

    bool Load();
    

    NiShaderDescMap m_NiShaderDescMap;

};


#endif //SHADERINFOCOLLECTION_H