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

#ifndef NITEXTURINGPROPERTY_H
#define NITEXTURINGPROPERTY_H

#include "NiProperty.h"
#include "NiTArray.h"
#include "NiTexture.h"
#include "NiTextureTransform.h"
#include "NiFlags.h"
#include "NiFixedString.h"

NiSmartPointer(NiTexturingProperty);

class NIMAIN_ENTRY NiTexturingProperty : public NiProperty
{
    NiDeclareRTTI;
    NiDeclareClone(NiTexturingProperty);
    NiDeclareStream;
    NiDeclareViewerStrings;
    NiDeclareFlags(unsigned short);

public:
    NiTexturingProperty();
    ~NiTexturingProperty();

    // creates a texturing property and attaches a base texture
    // created from the given filename
    NiTexturingProperty(const char* pcTextureName);
    
    // creates a texturing property and attaches a base texture
    // created from the given raw data
    NiTexturingProperty(NiPixelData* pkPixelData);

    enum FilterMode
    {
        FILTER_NEAREST,
        FILTER_BILERP,
        FILTER_TRILERP,
        FILTER_NEAREST_MIPNEAREST,
        FILTER_NEAREST_MIPLERP,
        FILTER_BILERP_MIPNEAREST,
        FILTER_ANISOTROPIC,
        FILTER_MAX_MODES
    };

    enum ClampMode
    { 
        CLAMP_S_CLAMP_T,
        CLAMP_S_WRAP_T,
        WRAP_S_CLAMP_T,
        WRAP_S_WRAP_T,
        CLAMP_MAX_MODES
    };

    enum ApplyMode
    { 
        // see table below for apply mode equations
        APPLY_REPLACE,
        APPLY_DECAL,
        APPLY_MODULATE,

        // Modes no longer supported
        APPLY_DEPRECATED,
        APPLY_DEPRECATED2,

        APPLY_MAX_MODES
    };

    enum MapClassID
    {
        MAP_CLASS_BASE = 0,
        MAP_CLASS_BUMP,
        MAP_CLASS_PARALLAX,
        MAP_CLASS_SHADER,
        MAP_CLASS_MAX
    };

    class NIMAIN_ENTRY Map : public NiMemObject
    {
        NiDeclareFlags(unsigned short);
    public:
        Map();
        Map(NiTexture* pkTexture, unsigned int uiIndex,
            ClampMode eClampMode = WRAP_S_WRAP_T, 
            FilterMode eFilterMode = FILTER_BILERP,
            NiTextureTransform* pkTextureTransform = NULL);
        Map(const Map& kObj);

        virtual ~Map();

        bool operator==(const Map& kObj) const;
        inline bool operator!=(const Map& kObj) const;

        // image access functions
        inline NiTexture* GetTexture() const;
        inline void SetTexture(NiTexture* pkTexture);

        // clamp mode access functions
        ClampMode GetClampMode() const;
        inline void SetClampMode(ClampMode eClampMode);

        // filter mode access functions
        FilterMode GetFilterMode() const;
        inline void SetFilterMode(FilterMode eFilterMode);

        inline unsigned short GetMaxAnisotropy() const;
        inline void SetMaxAnisotropy(unsigned short usMaxAnisotropy);

        inline unsigned int GetTextureIndex() const;
        inline void SetTextureIndex(unsigned int uiIndex);

        inline NiTextureTransform* GetTextureTransform();
        inline const NiTextureTransform* GetTextureTransform() const;
        inline void SetTextureTransform(NiTextureTransform* pkTrextureTransform);

        // *** begin Emergent internal use only ***

        bool IsEqual(Map* pObject);
        virtual void LoadBinary(NiStream& stream);
        virtual void SaveBinary(NiStream& stream);
        virtual MapClassID GetClassID() const;

        // *** end Emergent internal use only ***
    protected:

        // flags
        enum
        {
            TEXCOORD_MASK   = 0x00FF,
            TEXCOORD_POS    = 0,
            FILTERMODE_MASK = 0x0F00,
            FILTERMODE_POS  = 8,
            CLAMPMODE_MASK  = 0x3000,
            CLAMPMODE_POS   = 12
        };

        // Note: The ordering of members is important here: m_uFlags and m_usMaxAnisotropy
        // follow one another.
        unsigned short m_usMaxAnisotropy;

        NiTexturePtr m_spTexture;

        NiTextureTransform* m_pkTextureTransform;
    };

    class NIMAIN_ENTRY BumpMap : public Map
    {
    public:
        BumpMap();
        BumpMap(NiTexture* pkTexture, unsigned int uiIndex,
            ClampMode eClampMode = WRAP_S_WRAP_T, 
            FilterMode eFilterMode = FILTER_BILERP,
            float fLumaScale = 1.0f, float fLumaOffset = 0.0f,
            float fBumpMat00 = 0.5f, float fBumpMat01 = 0.0f,  
            float fBumpMat10 = 0.0f, float fBumpMat11 = 0.5f);

        inline float GetLumaScale() const;
        inline void SetLumaScale(float fVal);
        inline float GetLumaOffset() const;
        inline void SetLumaOffset(float fVal);
        inline float GetBumpMat00() const;
        inline void SetBumpMat00(float fVal);
        inline float GetBumpMat01() const;
        inline void SetBumpMat01(float fVal);
        inline float GetBumpMat10() const;
        inline void SetBumpMat10(float fVal);
        inline float GetBumpMat11() const;
        inline void SetBumpMat11(float fVal);

        virtual ~BumpMap();

        bool operator==(const BumpMap& kObj) const;
        bool operator!=(const BumpMap& kObj) const;

        // *** begin Emergent internal use only ***

        bool IsEqual(BumpMap* pkObject);
        virtual void LoadBinary(NiStream& kStream);
        virtual void SaveBinary(NiStream& kStream);
        virtual MapClassID GetClassID() const;
        // *** end Emergent internal use only ***
    protected:
        float m_fLumaScale;
        float m_fLumaOffset;

        float m_fBumpMat00;
        float m_fBumpMat01;
        float m_fBumpMat10;
        float m_fBumpMat11;
    };

    class NIMAIN_ENTRY ShaderMap : public Map
    {
    public:
        ShaderMap();
        ShaderMap(NiTexture* pkTexture, unsigned int uiIndex,
            ClampMode eClampMode = WRAP_S_WRAP_T, 
            FilterMode eFilterMode = FILTER_BILERP,
            unsigned int uiID = 0);

        inline unsigned int GetID() const;
        inline void SetID(unsigned int uiID);

        virtual ~ShaderMap();

        bool operator==(const ShaderMap& kObj) const;
        bool operator!=(const ShaderMap& kObj) const;

        // *** begin Emergent internal use only ***

        bool IsEqual(ShaderMap* pkObject);
        virtual void LoadBinary(NiStream& kStream);
        virtual void SaveBinary(NiStream& kStream);
        virtual MapClassID GetClassID() const;
        // *** end Emergent internal use only ***
    protected:
        unsigned int m_uiID;
    };

    class NIMAIN_ENTRY ParallaxMap : public Map
    {
    public:
        ParallaxMap();
        ParallaxMap(NiTexture* pkTexture, unsigned int uiIndex,
            ClampMode eClampMode = WRAP_S_WRAP_T, 
            FilterMode eFilterMode = FILTER_BILERP,
            float fOffset = 0.05f);

        inline float GetOffset() const;
        inline void SetOffset(float fOffset);

        virtual ~ParallaxMap();

        bool operator==(const ParallaxMap& kObj) const;
        bool operator!=(const ParallaxMap& kObj) const;

        // *** begin Emergent internal use only ***

        bool IsEqual(ParallaxMap* pkObject);
        virtual void LoadBinary(NiStream& kStream);
        virtual void SaveBinary(NiStream& kStream);  
        virtual MapClassID GetClassID() const;
        // *** end Emergent internal use only ***
    protected:
        float m_fOffset;
    };


    // attach / detach access functions
    inline const Map* GetBaseMap() const;
    Map* GetBaseMap();
    inline void SetBaseMap(Map* pkMap);

    inline const Map* GetDarkMap() const;
    Map* GetDarkMap();
    inline void SetDarkMap(Map* pkMap);

    inline const Map* GetDetailMap() const;
    Map* GetDetailMap();
    inline void SetDetailMap(Map* pkMap);

    inline const Map* GetGlossMap() const;
    Map* GetGlossMap();
    inline void SetGlossMap(Map* pkMap);

    inline const Map* GetGlowMap() const;
    Map* GetGlowMap();
    inline void SetGlowMap(Map* pkMap);

    inline const BumpMap* GetBumpMap() const;
    BumpMap* GetBumpMap();
    inline void SetBumpMap(BumpMap* pkMap);

    inline const Map* GetNormalMap() const;
    Map* GetNormalMap();
    inline void SetNormalMap(Map* pkMap);

    inline const ParallaxMap* GetParallaxMap() const;
    ParallaxMap* GetParallaxMap();
    inline void SetParallaxMap(ParallaxMap* pkMap);

    inline const Map* GetDecalMap(unsigned int uiIndex) const;
    Map* GetDecalMap(unsigned int uiIndex);
    void SetDecalMap(unsigned int uiIndex, Map* pkMap);
    inline unsigned int GetDecalArrayCount() const;
    inline unsigned int GetDecalMapCount() const;

    inline const ShaderMap* GetShaderMap(unsigned int uiIndex) const;
    ShaderMap* GetShaderMap(unsigned int uiIndex);
    void SetShaderMap(unsigned int uiIndex, ShaderMap* pkMap);
    inline unsigned int GetShaderArrayCount() const;
    inline unsigned int GetShaderMapCount() const;

    // apply modes
    ApplyMode GetApplyMode() const;
    inline void SetApplyMode(ApplyMode eApplymode);

    inline bool IsEqualFast(const NiTexturingProperty& kProp) const;

    virtual int Type() const;
    static int GetType();
    static NiTexturingProperty* GetDefault();

    inline bool IsMultiTexture() const; 

    // Easy access to base map
    inline NiTexture* GetBaseTexture() const;
    inline void SetBaseTexture(NiTexture* pkTexture);
    ClampMode GetBaseClampMode() const;
    inline void SetBaseClampMode(ClampMode eMode);
    FilterMode GetBaseFilterMode() const;
    inline void SetBaseFilterMode(FilterMode eMode);
    inline unsigned int GetBaseTextureIndex() const;
    inline void SetBaseTextureIndex(unsigned int uiIndex);
    inline NiTextureTransform* GetBaseTextureTransform() const;
    inline void SetBaseTextureTransform(NiTextureTransform* pkTransform);

    enum MapEnum
    {
        BASE_INDEX,
        DARK_INDEX,
        DETAIL_INDEX,
        GLOSS_INDEX,
        GLOW_INDEX,
        BUMP_INDEX,
        NORMAL_INDEX,
        PARALLAX_INDEX,
        DECAL_BASE,
        SHADER_BASE,
        INDEX_MAX
    };
    typedef NiTPrimitiveArray<Map*>  NiMapArray;

    // Get non-shader maps array
    inline const NiMapArray& GetMaps() const;

    // Set a single map (NOTE: can not set shader maps)
    void SetMap(unsigned int uiIndex, Map* pkMap);


    // *** begin Emergent internal use only ***

    static void _SDMInit();
    static void _SDMShutdown();

    // streaming support
    static char* GetViewerString(const char* pcPrefix, ClampMode eMode);
    static char* GetViewerString(const char* pcPrefix, FilterMode eMode);
    static char* GetViewerString(const char* pcPrefix, ApplyMode eMode);

    static const NiFixedString& GetMapNameFromID(unsigned int uiID);
    static bool GetMapIDFromName(const NiFixedString& kName, 
        unsigned int& uiID);

    inline bool ValidMapExists(MapEnum eMap, unsigned int uiWhichIndex = 0) const;
    // *** end Emergent internal use only ***

protected:
    void SetMultiTexture(Map* pkMap);
    inline void InitializeDecalCount();
    inline void IncrementDecalCount();
    inline void DecrementDecalCount();

    // flags
    enum
    {
        MULTITEXTURE_MASK   = 0x0001,
        APPLYMODE_MASK      = 0x000E,
        APPLYMODE_POS       = 1,
        DECALCOUNT_MASK     = 0x0FF0,
        DECALCOUNT_POS      = 4
    };

    NiMapArray m_kMaps;

    typedef NiTPrimitiveArray<ShaderMap*> NiShaderMapArray;
    NiShaderMapArray* m_pkShaderMaps;

    static NiTexturingPropertyPtr ms_spDefault;
    static NiFixedString ms_akMapIDtoNames[INDEX_MAX];
};

NiSmartPointer(NiTexturingProperty);

#include "NiTexturingProperty.inl"

#endif // NITEXTURINGPROPERTY_H

