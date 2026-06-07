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

#ifndef NIRENDERER_H
#define NIRENDERER_H

#include "Ni2DBuffer.h"
#include "NiAccumulator.h"
#include "NiAVObject.h"
#include "NiBool.h"
#include "NiColor.h"
#include "NiDynamicEffectState.h"
#include "NiMaterial.h"
#include "NiPropertyState.h"
#include "NiRect.h"
#include "NiRTLib.h"
#include "NiShader.h"
#include "NiTexture.h"
#include "NiTList.h"

#include <NiSystemDesc.h>

class NiBound;
class NiDynamicEffect;
class NiDynamicTexture;
class NiFragmentMaterial;
class NiPoint2;
class NiPoint3;
class NiPlane;
class NiCamera;
class NiDepthStencilBuffer;
class NiFrustum;
class NiRenderObject;
class NiMaterialDescriptor;
class NiPalette;
class NiTransform;
class NiShader;
class NiSourceCubeMap;
class NiSourceTexture;
class NiRenderedCubeMap;
class NiRenderedTexture;
class NiRenderTargetGroup;
class NiStandardMaterial;
class NiDataStream;
class NiMesh;
class NiShadowTechnique;
NiSmartPointer(NiStandardMaterial);

typedef NiTMapIterator NiMacrosIterator;

class NIMAIN_ENTRY NiRenderer : public NiObject 
{
    NiDeclareRTTI;
    NiDeclareViewerStrings;
public:
    // local constants -
    // return flags from GetFlags:
    enum
    {
        // indicates that renderer supports palettized textures
        CAPS_PALETTIZED                 = 0x00000001,
        // indicates renderer supports hardware skinning
        CAPS_HARDWARESKINNING           = 0x00000002,
        // indicates renderer has limited support for textures with 
        // non-power-of-2 dimensions
        CAPS_NONPOW2_CONDITIONAL_TEXT   = 0x00000004,
        // indicates renderer has full support for textures with 
        // non-power-of-2 dimensions
        CAPS_NONPOW2_TEXT               = 0x00000008,
        // indicates renderer can antialias rendered textures
        CAPS_AA_RENDERED_TEXTURES       = 0x00000010,
        // indicates renderer supports hardware instancing
        CAPS_HARDWAREINSTANCING         = 0x00000020,
        // indicates renderer supports anisotropic filtering.
        CAPS_ANISO_FILTERING            = 0x00000040,

        CAPS_MAX                        = 0x00000040
    };
      
    enum ClearFlags
    {
        CLEAR_BACKBUFFER = 0x1,
        CLEAR_STENCIL = 0x2,
        CLEAR_ZBUFFER = 0x4,
        CLEAR_NONE = 0, 
        CLEAR_ALL = CLEAR_BACKBUFFER | CLEAR_STENCIL | CLEAR_ZBUFFER
    }; 

    // Note: Check_FrameState needs to be updated when these are updated
    enum EFrameState
    {
        FRAMESTATE_OUTSIDE_FRAME,
        FRAMESTATE_INSIDE_FRAME,
        FRAMESTATE_INSIDE_OFFSCREEN_FRAME,
        FRAMESTATE_WAITING_FOR_DISPLAY,
        FRAMESTATE_INTERNAL_FRAME
    };

    enum DisplayCorner
    {
        CORNER_TOP_LEFT,
        CORNER_TOP_RIGHT,
        CORNER_BOTTOM_LEFT,
        CORNER_BOTTOM_RIGHT
    };

    enum EScreenshotFormat
    {
        FORMAT_TARGA,
        FORMAT_PNG,
        FORMAT_JPEG
    };

    // construction and destruction
    NiRenderer();
    virtual ~NiRenderer();

    // String describing renderer
    virtual const char* GetDriverInfo() const = 0;

    // Renderer capabilities
    virtual unsigned int GetFlags() const = 0;

    // Renderer ID
    virtual NiSystemDesc::RendererID GetRendererID() const = 0;

    // Renderer locking methods
    inline void LockRenderer();
    inline void UnlockRenderer();
    inline unsigned int LockCount() const;

    // Begin / End Frame
    inline bool BeginFrame();
    inline unsigned int GetFrameID() const;
    bool EndFrame();
    inline bool DisplayFrame();
    inline bool BeginOffScreenFrame();
    inline bool EndOffScreenFrame();
    EFrameState GetFrameState() const;
    inline bool GetInsideFrameState() const;

    // Render target clearing operations
    inline void ClearBuffer(const NiRect<float>* pkR, unsigned int uiMode);

    virtual void SetDepthClear(const float fZClear) = 0;
    virtual float GetDepthClear() const = 0;
    virtual void SetBackgroundColor(const NiColor& kColor) = 0;
    virtual void SetBackgroundColor(const NiColorA& kColor) = 0;
    virtual void GetBackgroundColor(NiColorA& kColor) const = 0;
    virtual void SetStencilClear(unsigned int uiClear) = 0;
    virtual unsigned int GetStencilClear() const = 0;

    // Camera setup
    void SetCameraData(const NiCamera* pkCamera);
    inline void SetCameraData(
        const NiPoint3& kWorldLoc,
        const NiPoint3& kWorldDir,
        const NiPoint3& kWorldUp, 
        const NiPoint3& kWorldRight,
        const NiFrustum& kFrustum, 
        const NiRect<float>& kPort);
    inline void SetScreenSpaceCameraData(const NiRect<float>* pkPort = NULL);
    void GetCameraData(NiCamera& kCamera);
    inline void GetCameraData(NiPoint3& kWorldLoc, NiPoint3& kWorldDir,
        NiPoint3& kWorldUp, NiPoint3& kWorldRight, NiFrustum& kFrustum,
        NiRect<float>& kPort);

    // Render target access and management
    inline bool BeginUsingRenderTargetGroup(NiRenderTargetGroup* pkTarget, 
        unsigned int uiClearMode);
    inline bool BeginUsingDefaultRenderTargetGroup(unsigned int uiClearMode);
    inline bool IsRenderTargetGroupActive() const;
    bool EndUsingRenderTargetGroup();

    virtual bool ValidateRenderTargetGroup(NiRenderTargetGroup* pkTarget) = 0;
    virtual bool IsDepthBufferCompatible(Ni2DBuffer* pkBuffer, 
        NiDepthStencilBuffer* pkDSBuffer) = 0;

    virtual NiRenderTargetGroup* GetDefaultRenderTargetGroup() const = 0;
    virtual const NiRenderTargetGroup* GetCurrentRenderTargetGroup() const
        = 0;

    virtual NiDepthStencilBuffer* GetDefaultDepthStencilBuffer() const = 0;
    virtual Ni2DBuffer* GetDefaultBackBuffer() const = 0;

    // Pixel format support queries
    virtual const NiPixelFormat* 
        FindClosestPixelFormat(NiTexture::FormatPrefs& kFmtPrefs) const = 0;
    virtual const NiPixelFormat* FindClosestDepthStencilFormat(
        const NiPixelFormat* pkFrontBufferFormat) const;
    virtual const NiPixelFormat* FindClosestDepthStencilFormat(
        const NiPixelFormat* pkFrontBufferFormat, unsigned int uiDepthBPP,
        unsigned int uiStencilBPP) const = 0;

    // Multiple render target queries
    virtual unsigned int GetMaxBuffersPerRenderTargetGroup() const = 0;
    virtual bool GetIndependentBufferBitDepths() const = 0;

    // Sorting
    inline void SetSorter(NiAccumulator* pkAccum);
    inline NiAccumulator* GetSorter() const;

    // Default material
    void SetDefaultMaterial(NiMaterial* pkMaterial);
    inline NiMaterial* GetDefaultMaterial() const;
    virtual void UseLegacyPipelineAsDefaultMaterial() = 0;
    virtual void ApplyDefaultMaterial(NiRenderObject* pkGeometry);

    inline NiMaterial* GetInitialDefaultMaterial() const;

    // Texture precaching
    virtual bool PrecacheTexture(NiTexture* pkTexture) = 0;

    // Texture mipmap skip level
    virtual bool SetMipmapSkipLevel(unsigned int uiSkip) = 0;
    virtual unsigned int GetMipmapSkipLevel() const = 0;

    // Geometry precaching
    virtual bool PrecacheMesh(
        NiRenderObject* pkMesh, 
        bool bReleaseSystemMemory = true);

    // Perform the precaches that the renderer has queued up
    virtual bool PerformPrecache();

    // Purge cached renderer data
    virtual void PurgeMaterial(NiMaterialProperty* pkMaterial) = 0;
    virtual void PurgeEffect(NiDynamicEffect* pLight) = 0;
    virtual bool PurgeTexture(NiTexture* pkTexture) = 0;

    virtual bool PurgeAllTextures(bool bPurgeLocked) = 0;
    inline void PurgeAllRendererData(NiAVObject* pkScene); 

    // Screen shot
    virtual NiPixelData* TakeScreenShot(
        const NiRect<unsigned int>* pkScreenRect,
        const NiRenderTargetGroup* pkTarget = NULL) = 0;

    // Screenshot direct to file
    virtual bool SaveScreenShot(const char* pcFilename,
        EScreenshotFormat eFormat);

    // Ni2DBuffer interaction

    // Copy an arbitrary region from one 2D buffer to the specified 
    // coordinates of a different 2D buffer. Note that the NiPixelFormats
    // of both Ni2DBuffers must match for this method. This is the fastest path
    // possible for copying pixel data. Also note that a NULL value for the
    // NiRect will use the dimensions of the Ni2DBuffer.
    virtual bool FastCopy(const Ni2DBuffer* pkSrc, Ni2DBuffer* pkDest, 
        const NiRect<unsigned int>* pkSrcRect = NULL,
        unsigned int uiDestX = 0, unsigned int uiDestY = 0) = 0;
    // Copy an arbitrary region from one 2D buffer to another arbitrary region
    // in a different 2D buffer. Note that the NiPixelFormats
    // of both Ni2DBuffers may differ for this method. Also note that a NULL 
    // value for either NiRect will use the dimensions of the Ni2DBuffer.
    // This path is slower than the previous FastCopy method
    virtual bool Copy(const Ni2DBuffer* pkSrc, Ni2DBuffer* pkDest, 
        const NiRect<unsigned int>* pkSrcRect,
        const NiRect<unsigned int>* pkDestRect,
        Ni2DBuffer::CopyFilterPreference ePref) = 0;

    // Create a depth stencil buffer compatible with the passed-in buffer
    virtual NiDepthStencilBuffer* CreateCompatibleDepthStencilBuffer(
        Ni2DBuffer* pkBuffer) const;

    // Reversed left-right rendering
    virtual bool GetLeftRightSwap() const = 0;
    virtual bool SetLeftRightSwap(bool bSwap) = 0;

    // Global fog settings
    virtual float GetMaxFogValue() const = 0;
    virtual void SetMaxFogValue(float fFogVal) = 0;

    // Anisotropic filtering maximum quality settings
    inline unsigned short GetMaxAnisotropy() const;
    virtual void SetMaxAnisotropy(unsigned short usMaxAnisValue) = 0;

    // Error shader support
    virtual NiShader* GetErrorShader() const;
    virtual void SetErrorShader(NiShader* pkErrorShader);

    // Error status
    static const char* GetLastErrorString();
    static void ClearLastErrorString();

    // safe zone handling
    inline void SetSafeZone(const NiRect<float>& kRect);
    inline NiRect<float> GetSafeZone();
    inline void ForceInSafeZone(unsigned int& uiX, unsigned int& uiY);
    inline void ForceInSafeZoneNDC(float& fX, float& fY);
    inline void ConvertFromPixelsToNDC(const unsigned int uiInX, 
        const unsigned int uiInY, float& fOutX, float& fOutY);
    inline void ConvertFromNDCToPixels(const float fInX, const float fInY,
        unsigned int& uiOutX, unsigned int& uiOutY);

    // Helper methods to create object screen coordinates
    void GetOnScreenCoord(
        const float fXOffset, 
        const float fYOffset,
        const unsigned int uiWidth, 
        const unsigned int uiHeight,
        unsigned int& uiOutX, 
        unsigned int& uiOutY,
        const DisplayCorner eCorner, 
        const bool bForceSafeZone = true);

    // Return the renderer
    static NiRenderer* GetRenderer();

    // Methods for setting/querying/removing shader macro definitions.
    // Most methods operate both on NiFixedString's and char* for convenience.
    // Macro names are case-sensitive, and file type specifier strings are not.

    // Removes all macro definitions and frees memory, occupied by tables
    void DeleteAllMacros();

    // Macro adding. If same named macro exists, its value will be overwritten.
    bool AddGlobalMacro(
        const NiFixedString& kMacroName,
        const NiFixedString& kMacroValue);
    bool AddMacro(
        const char* pcFileType,
        const NiFixedString& kMacroName,
        const NiFixedString& kMacroValue);

    // Macro removal methods
    bool RemoveGlobalMacro(const NiFixedString& kMacroName);
    bool RemoveMacro(const char* pcFileType, const NiFixedString& kMacroName);

    // Count of macros
    NiUInt32 GetGlobalMacroCount();
    NiUInt32 GetMacroCount(const char* pcFileType);

    // Checking for presence of a macro
    bool IsGlobalMacro(const NiFixedString& kMacroName);
    bool IsMacro(
        const char* pcFileType, 
        const NiFixedString& kMacroName);

    // Retrieving macro value for known macro name
    bool GetGlobalMacroValue(
        const NiFixedString& kMacroName,
        NiFixedString& kMacroValue);
    bool GetMacroValue(
        const char* pcFileType,
        const NiFixedString& kMacroName,
        NiFixedString& kMacroValue);

    // Methods to iterate through macros
    NiMacrosIterator GetFirstGlobalMacro();
    NiMacrosIterator GetFirstMacro(const char* pcFileType);

    bool GetNextGlobalMacro(
        NiMacrosIterator& kPos,
        NiFixedString& kMacroName,
        NiFixedString& kMacroValue);
    bool GetNextMacro(
        const char* pcFileType,
        NiMacrosIterator& kPos,
        NiFixedString& kMacroName,
        NiFixedString& kMacroValue);

    // Methods to get/set global and per-shader-type shader creation flags
    NiUInt32 GetGlobalShaderCreationFlags();
    NiUInt32 GetShaderCreationFlags(const char* pcFileType);
    NiUInt32 GetAllShaderCreationFlags(
        const char* pcFileType,
        NiUInt32 uiUserFlags = 0);

    void SetGlobalShaderCreationFlags(NiUInt32 uiFlags);
    void SetShaderCreationFlags(const char* pcFileType, NiUInt32 uiFlags);

    // *** begin Emergent internal use only ***

    // Property state
    inline NiPropertyState* GetPropertyState() const;
    inline void SetPropertyState (NiPropertyState* pkState);

    // Effect state
    inline NiDynamicEffectState* GetEffectState() const;
    inline void SetEffectState(NiDynamicEffectState* pkState);

    // Primitive rendering. This function should never be called directly by
    // an application. It should only be called from RenderImmediate.
    inline void RenderMesh(NiRenderObject* pkMesh);

    bool MapWindowPointToBufferPoint(unsigned int uiX, unsigned int uiY, 
        float& fX, float& fY, const NiRenderTargetGroup* pkTarget = NULL);
    
    virtual bool PreloadTexture(NiTexture* pkTexture);

    static void RemoveEffect(NiDynamicEffect* pkEffect);
    static void RemoveMaterial(NiMaterialProperty* pkMaterial);

    inline static void SetLastErrorString(const char* pcStr);

    // Source renderer data primitive protection methods.
    inline void LockSourceDataCriticalSection();
    inline void UnlockSourceDataCriticalSection();

    // Renderer specific data for textures and palettes
    virtual bool CreateSourceTextureRendererData(
        NiSourceTexture* pkTexture) = 0;
    virtual bool CreateRenderedTextureRendererData(
        NiRenderedTexture* pkTexture, 
        Ni2DBuffer::MultiSamplePreference eMSAAPref = 
        Ni2DBuffer::MULTISAMPLE_NONE) = 0;
    virtual bool CreateSourceCubeMapRendererData(
        NiSourceCubeMap* pkCubeMap) = 0;
    virtual bool CreateRenderedCubeMapRendererData(
        NiRenderedCubeMap* pkCubeMap) = 0;
    virtual bool CreateDynamicTextureRendererData(
        NiDynamicTexture* pkTexture) = 0;
    virtual void CreatePaletteRendererData(
        NiPalette* pkPalette) = 0;
    virtual bool CreateDepthStencilRendererData(
        NiDepthStencilBuffer* pkDSBuffer, const NiPixelFormat* pkFormat,
        Ni2DBuffer::MultiSamplePreference eMSAAPref = 
        Ni2DBuffer::MULTISAMPLE_NONE) = 0;

    // Dynamic texture support
    virtual void* LockDynamicTexture(const NiTexture::RendererData* pkRData,
        int& iPitch) = 0;
    virtual bool UnLockDynamicTexture(const NiTexture::RendererData* pkRData)
        = 0;

    // Fragment shader support
    virtual NiShader* GetFragmentShader(
        NiMaterialDescriptor* pkMaterialDescriptor) = 0;
    virtual void SetDefaultProgramCache(NiFragmentMaterial* pkMaterial);
    virtual void SetDefaultProgramCache(NiFragmentMaterial* pkMaterial, 
        bool bAutoWriteToDisk, bool bWriteDebugFile, bool bLoad, 
        bool bNoNewProgramCreation, const char* pcWorkingDir) = 0;

    // ShadowMap shader support
    virtual NiShader* GetShadowWriteShader(
        NiMaterialDescriptor* pkMaterialDescriptor) = 0;
    virtual void SetRenderShadowCasterBackfaces(bool bRenderBackfaces) = 0;
    virtual void SetRenderShadowTechnique(NiShadowTechnique* pkTechnique);

    // Message/Warning/Error reporting functions
    static void Message(const char* pcFormat, ...);
    static void Warning(const char* pcFormat, ...);
    static void Error(const char* pcFormat, ...);

    static void _SDMShutdown();

    // *** end Emergent internal use only ***

protected:
    // Renderer specific implementations
    inline virtual void Do_LockRenderer() {};
    inline virtual void Do_UnlockRenderer() {};
    virtual bool Do_BeginFrame() = 0;
    virtual bool Do_EndFrame() = 0;
    virtual bool Do_DisplayFrame() = 0;
    virtual void Do_ClearBuffer(const NiRect<float>* pkR,
        unsigned int uiMode) = 0;
    virtual void Do_SetCameraData(const NiPoint3& kWorldLoc,
        const NiPoint3& kWorldDir, const NiPoint3& kWorldUp, 
        const NiPoint3& kWorldRight, const NiFrustum& kFrustum, 
        const NiRect<float>& kPort) = 0;
    virtual void Do_SetScreenSpaceCameraData(
        const NiRect<float>* pkPort = NULL) = 0;
    virtual void Do_GetCameraData(NiPoint3& kWorldLoc, NiPoint3& kWorldDir,
        NiPoint3& kWorldUp, NiPoint3& kWorldRight, NiFrustum& kFrustum,
        NiRect<float>& kPort) = 0;
    virtual bool Do_BeginUsingRenderTargetGroup(NiRenderTargetGroup* pkTarget, 
        unsigned int uiClearMode) = 0;
    virtual bool Do_EndUsingRenderTargetGroup() = 0;
    virtual void Do_RenderMesh(NiMesh* pkMesh) = 0;

    // Thread synchronization 
    inline void LockPrecacheCriticalSection();
    inline void UnlockPrecacheCriticalSection();

    NiAccumulatorPtr m_spAccum;
    NiPropertyState* m_pkCurrProp;
    NiDynamicEffectState* m_pkCurrEffects;
    NiRenderTargetGroup* m_pkCurrentRenderTargetGroup;

    // Default material
    NiMaterialPtr m_spInitialDefaultMaterial;
    NiMaterialPtr m_spCurrentDefaultMaterial;

    // Error shader
    NiShaderPtr m_spErrorShader;

    static char ms_acErrorString[256];
    static NiRenderer* ms_pkRenderer;

    // Safe zone rectangle
    NiRect<float> m_kDisplaySafeZone;

    bool BeginInternalFrame();
    bool EndInternalFrame();

    // Structure, that represents global or per-shader-type macro definitions
    // and shader creation flags. Contains methods to manipulate its data.
    struct ShaderData : public NiMemObject
    {
    public:
        ShaderData();

        void DeleteAllMacros();

        bool AddMacro(
            const NiFixedString& pcMacroName,
            const NiFixedString& pcMacroValue);
        bool RemoveMacro(const NiFixedString& kMacroName);

        NiUInt32 GetMacroCount() const;
        bool IsMacro(const NiFixedString& kMacroName) const;
        bool GetMacroValue(
            const NiFixedString& kMacroName,
            NiFixedString& kMacroValue) const;

        NiMacrosIterator GetFirstMacro() const;
        bool GetNextMacro(
            NiMacrosIterator& kPos,
            NiFixedString& kMacroName,
            NiFixedString& kMacroValue) const;

        NiUInt32 GetShaderCreationFlags() const;
        void SetShaderCreationFlags(NiUInt32 uiFlags);

    private:
        NiTFixedStringMap<NiFixedString> m_kStringMap;
        NiUInt32 m_uiShaderFlags;
        bool m_bIsChanged;
    };

    // Returns pointer to global ShaderData structure
    ShaderData* GetGlobalShaderData();
    // Returns pointer to ShaderData structure for specified shader type.
    // If it is not found, returns NULL or creates a new one if bCreate == 1
    ShaderData* GetShaderData(const char* pcFileType, bool bCreate = false);

    // Builds list of global + per-filetype + user-defined macros. Same named
    // per-filetype macros overrides global, user-defined overrides all others.
    // All fields can be NULL.
    void BuildMacroList(
        const char* pcFileType,
        ShaderData* kUserMacros,
        NiTFixedStringMap<NiFixedString>& kFinalList);

private:
    // Checking code for API call validity 
    inline bool Check_FrameState(
        const char* pcCallingFunction,
        EFrameState eExpectedValue) const;

    inline bool Check_RenderTargetGroupActive(
        const char* pcCallingFunction,
        bool bExpectedValue) const;

    // Thread synchronization primitives
    NiCriticalSection m_kRendererLock;
    NiCriticalSection m_kPrecacheCriticalSection; // requires m_kRendererLock

    NiCriticalSection m_kSourceDataCriticalSection;

    // Render target group bracketing
    bool m_bRenderTargetGroupActive;

    // Frame bracketing
    EFrameState m_eFrameState;
    EFrameState m_eSavedFrameState;
    unsigned int m_uiFrameID;

    // Instance of ShaderData, that contains global macros and creation flags
    ShaderData m_kGlobalShaderData;
    // Array of per-shader-type ShaderData structures
    NiTPrimitiveArray<ShaderData*> m_kShaderData;
    // Array, which maps file type string to corresponding ShaderData*
    NiTFixedStringMap<ShaderData*> m_kFiletypeMap;

protected:

   // max anosotropy value 
   unsigned short m_usMaxAnisotropy;

};

typedef NiPointer<NiRenderer> NiRendererPtr;

#include "NiRenderer.inl"

#endif // NIRENDERER_H
