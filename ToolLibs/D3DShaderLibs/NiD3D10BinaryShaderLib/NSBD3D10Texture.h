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

#ifndef NSBD3D10TEXTURE_H
#define NSBD3D10TEXTURE_H

#include "NiD3D10BinaryShaderLibLibType.h"
#include "NSBD3D10StageAndSamplerStates.h"
#include "NSBD3D10StateGroup.h"
#include "NiShaderAttributeDesc.h"
#include "NiD3D10Pass.h"

class NSBD3D10TextureStage;

class NID3D10BINARYSHADERLIB_ENTRY NSBD3D10Texture : public NiMemObject
{
public:
    NSBD3D10Texture();
    ~NSBD3D10Texture();

    // Name
    inline const char* GetName();
    void SetName(const char* pcName);

    inline unsigned int GetStage();
    inline void SetStage(unsigned int uiStage);

    // 
    NSBD3D10StateGroup* GetTextureStageGroup();
    NSBD3D10StateGroup* GetSamplerStageGroup();

    inline unsigned int GetTextureFlags();

    inline void SetSource(unsigned int uiNDLMap);
    inline void SetSourceDecal(unsigned int uiDecal);
    inline void SetSourceShader(unsigned int uiShaderMap);
    inline void SetSourceObject(NiShaderAttributeDesc::ObjectType eObjectType,
        unsigned int uiObjectIndex);

    bool SetupTextureStage(NiD3D10Pass& kPass);

    // *** begin Emergent internal use only
    bool SaveBinary(NiBinaryStream& kStream);
    bool LoadBinary(NiBinaryStream& kStream);

    bool CreateFromTextureStage(NSBD3D10TextureStage* pkTextureStage, 
        const char* pcTextureName);

#if defined(NIDEBUG)
    void Dump(FILE* pf);
#endif  //#if defined(NIDEBUG)
    // *** end Emergent internal use only

protected:
    char* m_pcName;

    unsigned int m_uiStage;
    unsigned int m_uiTextureFlags;
    unsigned short m_usObjTextureFlags;
};

#include "NSBD3D10Texture.inl"

#endif  //NSBD3D10TEXTURE_H
