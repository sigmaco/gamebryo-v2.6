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

#ifndef NITEXTUREEFFECT_H
#define NITEXTUREEFFECT_H

#include "NiTexture.h"
#include "NiDynamicEffect.h"
#include "NiPlane.h"
#include "NiTexturingProperty.h"

class NIMAIN_ENTRY NiTextureEffect : public NiDynamicEffect 
{
    NiDeclareRTTI;
    NiDeclareClone(NiTextureEffect);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    typedef enum 
    { 
        PROJECTED_LIGHT, 
        PROJECTED_SHADOW,
        ENVIRONMENT_MAP, 
        FOG_MAP,
        TEXTURE_TYPE_MAX
    } TextureType;

    typedef enum
    {
        WORLD_PARALLEL,
        WORLD_PERSPECTIVE,
        SPHERE_MAP,
        SPECULAR_CUBE_MAP,
        DIFFUSE_CUBE_MAP,
        NUM_COORD_GEN
    } CoordGenType;

    NiTextureEffect();
    virtual ~NiTextureEffect();

    // attributes
    
    inline const NiMatrix3& GetModelProjectionMatrix() const;
    inline void SetModelProjectionMatrix(const NiMatrix3& kMat);

    inline const NiPoint3& GetModelProjectionTranslation() const;
    inline void SetModelProjectionTranslation(const NiPoint3& kMat);

    inline const NiMatrix3& GetWorldProjectionMatrix() const;
    inline const NiPoint3& GetWorldProjectionTranslation() const;

    inline NiTexture* GetEffectTexture() const; 
    inline void SetEffectTexture(NiTexture* pkTexture); 

    inline NiTexturingProperty::FilterMode GetTextureFilter() const;
    inline void SetTextureFilter(NiTexturingProperty::FilterMode eFilter);

    inline unsigned short GetMaxAnisotropy() const;
    inline void SetMaxAnisotropy(unsigned short usMaxAnisotropy);

    inline NiTexturingProperty::ClampMode GetTextureClamp() const;
    inline void SetTextureClamp(NiTexturingProperty::ClampMode eClamp);

    TextureType GetTextureType() const;
    inline void SetTextureType(TextureType eTexType);

    CoordGenType GetTextureCoordGen() const;
    inline void SetTextureCoordGen(CoordGenType eGen);

    inline bool GetClippingPlaneEnable() const;
    inline void SetClippingPlaneEnable(bool bEnable);

    inline const NiPlane& GetModelClippingPlane() const;
    inline void SetModelClippingPlane(const NiPlane& kPlane);

    inline const NiPlane& GetWorldClippingPlane() const;

    // *** begin Emergent internal use only ***

    // streaming support
    static char* GetViewerString(const char* pcPrefix, TextureType eMode);
    static char* GetViewerString(const char* pcPrefix, CoordGenType eMode);

    // TextureType to string
    static const NiFixedString& GetTypeNameFromID(unsigned int uiID);
    static bool GetTypeIDFromName(const NiFixedString& kName, 
        unsigned int& uiID);

    // static initialization
    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***
protected:
    virtual void UpdateWorldData();

    void UpdateProjection();

    NiMatrix3 m_kModelProjMat;
    NiPoint3 m_kModelProjTrans;
    NiMatrix3 m_kWorldProjMat;
    NiPoint3 m_kWorldProjTrans;

    NiTexturePtr m_spTexture; 
    NiTexturingProperty::FilterMode m_eFilter;
    NiTexturingProperty::ClampMode m_eClamp;
    TextureType m_eTextureMode;
    CoordGenType m_eCoordMode;

    NiPlane m_kModelPlane;
    NiPlane m_kWorldPlane;

    unsigned short m_usMaxAnisotropy;
    bool m_bPlaneEnable;

    static NiFixedString ms_akTextureTypeStrings[TEXTURE_TYPE_MAX];
};
   

typedef NiPointer<NiTextureEffect> NiTextureEffectPtr;

#include "NiTextureEffect.inl"

#endif // NITEXTUREEFFECT_H
