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

#ifndef NIFRAGMENTMATERIAL_H
#define NIFRAGMENTMATERIAL_H

#include "NiMaterial.h"
#include "NiRenderObject.h"
#include "NiTPtrSet.h"
#include "NiMaterialDescriptor.h"
#include "NiGPUProgramDescriptor.h"
#include "NiMaterialConfigurator.h"
#include "NiMaterialResourceProducerNode.h"
#include "NiMaterialResourceConsumerNode.h"
#include "NiMaterialFragmentNode.h"
#include "NiMaterialNodeLibrary.h"
#include "NiShaderConstantMap.h"
#include "NiFragmentShaderInstanceDescriptor.h"
#include "NiGPUProgramCache.h"

// This class is meant to provide a framework for derived classes to use when
// implementing a shade-tree driven material. The geometry is analyzed and an
// NiMaterialDescriptor is created to describe the configuration of the 
// material. The NiMaterialDescriptor is then checked against a hash table of
// NiShader objects to see if it has already been created. If it has, the 
// cached version is returned. If it hasn't it is decomposed into a set of 
// rendering passes with NiGPUProgramDescriptor pairs for pixel and vertex 
// shaders. If either the pixel or vertex shader has been cached already, the
// cached version is used. Otherwise, a shade-tree is constructed and compiled
// for each GPU program. In some cases, a given rendering pass may not 
// properly be created due to hardware constraints. Derived classes can 
// attempt to recover by specifying fallback techniques which may modify the 
// array of RenderPassDescriptor objects to create shaders that are hopefully 
// capable of being compiled and result in an acceptable visual effect. 
class NIMAIN_ENTRY NiFragmentMaterial : public NiMaterial
{
    NiDeclareRTTI;

public:

    // Enumeration for texture map sampling
    enum TextureMapSamplerType
    {
        // Sample a 2D texture
        TEXTURE_SAMPLER_2D = 0,
        // Sample a cube map texture
        TEXTURE_SAMPLER_CUBE,
        // Maximum enum value
        TEXTURE_SAMPLER_MAX
    };

    // Get the default value for whether or not to load the program cache 
    // when creating an NiGPUProgramCache.
    static bool GetDefaultLoadProgramCacheOnCreation();

    // Get the default value for whether or not to write to the program cache
    // when creating a new program from an NiGPUProgramCache.
    static bool GetDefaultAutoSaveProgramCache();

    // Get the default value for whether or not to write additional 
    // debug information when creating a new program from an NiGPUProgramCache.
    static bool GetDefaultWriteDebugProgramData();

    // Get the default value for whether to keep the NiGPUProgramCache
    // from creating new programs.
    static bool GetDefaultLockProgramCache();

    // Get the default value for creating an NiGPUProgramCache in the
    // constructor for a fragment material.
    static bool GetDefaultAutoCreateProgramCache();

    // Set the default value for whether or not to load the program cache 
    // when creating an NiGPUProgramCache.
    static void SetDefaultLoadProgramCacheOnCreation(bool bSet);

    // Set the default value for whether or not to write to the program cache
    // when creating a new program from an NiGPUProgramCache.
    static void SetDefaultAutoSaveProgramCache(bool bSet);

    // Set the default value for whether or not to write additional 
    // debug information when creating a new program from an NiGPUProgramCache.
    static void SetDefaultWriteDebugProgramData(bool bSet);
    
    // Set the default value for whether to keep the NiGPUProgramCache
    // from creating new programs.
    static void SetDefaultLockProgramCache(bool bSet);

    // Set the default value for creating an NiGPUProgramCache in the
    // constructor for a fragment material.
    static void SetDefaultAutoCreateProgramCache(bool bSet);


    // Get the current version number for the specified program type for this 
    // material.
    unsigned int GetProgramVersion(NiGPUProgram::ProgramType eType) const;

    // Inherited from NiMaterial. Returns whether or not the applied shader 
    // matches the computed material description for a given render object.
    virtual bool IsShaderCurrent(NiShader* pkShader, 
        const NiRenderObject* pkGeometry,
        const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
        unsigned int uiMaterialExtraData);

    // Inherited from NiMaterial. Compute the NiShader needed to render this
    // piece of geometry.
    virtual NiShader* GetCurrentShader(const NiRenderObject* pkGeometry, 
        const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects, 
        unsigned int uiMaterialExtraData);

    // Utility method for generating the NiGPUProgram  objects needed to render
    // a given object without creating the NiShader. 
    virtual bool PrecacheGPUPrograms(const NiRenderObject* pkGeometry, 
        const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects, 
        unsigned int uiMaterialExtraData);

    // Inherited from NiMaterial. This method is called by the renderers or 
    // the host application to force the material to remove all internal 
    // NiShader data members.
    virtual void UnloadShaders();

    // Get and set the program cache for the specified program type. 
    // The cache provides both the capacity for compiling new programs as well
    // as the ability to reuse programs between application runs.
    virtual void SetProgramCache(NiGPUProgramCache* pkCache, 
        NiGPUProgram::ProgramType eType);
    virtual NiGPUProgramCache* GetProgramCache(
        NiGPUProgram::ProgramType eType) const;

    // Tell the material to load all of its active caches if they haven't 
    // previously been loaded.
    virtual void LoadAllCaches();

    /// Convenience method to LoadAllCaches on all materials.
    static void LoadAllCachesForAllMaterials();

    // Inherited from NiMaterial. This method is called in order to set the 
    // working directory for a given material. It is up to the material to 
    // determine what, if any files, it will use from this folder.
    virtual void SetWorkingDirectory(const char* pcWorkingDir);

    // Utility methods for accessing the material node libraries for a 
    // material. These libraries provide the code snippets for the shade-tree..
    unsigned int GetMaterialNodeLibraryCount() const;
    NiMaterialNodeLibrary* GetMaterialNodeLibrary(unsigned int uiIndex) const;

    // Enumeration for return values within this class
    enum ReturnCode
    {
        // Function succeeded
        RC_SUCCESS                  = 0x0000,
        // Pixel shader failed to compile
        RC_COMPILE_FAILURE_PIXEL    = 0x0001,
        // Geometry shader failed to compile
        RC_COMPILE_FAILURE_GEOMETRY = 0x0002,
        // Vertex shader failed to compile
        RC_COMPILE_FAILURE_VERTEX   = 0x0004,
        // Material was not properly configured
        RC_INVALID_MATERIAL         = 0x0008,
        // Shader was not properly configured
        RC_INVALID_SHADER           = 0x0010,
        // Constant map was not properly configured
        RC_INVALID_CONSTANTMAP      = 0x0020
    };

    // This internal class stores the offsets of Object-based variables so that
    // lights and other dynamic effects can be mapped to their correct 
    // index at runtime.
    class ObjectResourceOffset : public NiMemObject
    {
    public:
        // Default constructor
        ObjectResourceOffset();
        
        // The program type of the variable. This is needed to distinguish 
        // between constants being bound in the sampe pass, but different 
        // programs.
        NiGPUProgram::ProgramType m_eProgramType;

        // The object type of this resource
        NiShaderAttributeDesc::ObjectType m_eObjectType;

        // The offset in the list of objects affecting this NiRenderObject
        unsigned int m_uiOffset;
    };

    // This internal class describes a rendering pass.
    class NIMAIN_ENTRY RenderPassDescriptor : public NiMemObject
    {
    public:
        // Default constructor
        RenderPassDescriptor();

        // Copy constructor
        RenderPassDescriptor(RenderPassDescriptor&);

        // Copy operator
        RenderPassDescriptor& operator=(RenderPassDescriptor& kOther);

        // Intitializes this descriptor to default values
        void Initialize();

        // The vertex descriptor for this pass.
        NiGPUProgramDescriptor m_kVertexDesc;

        // The geometry descriptor for this pass.
        NiGPUProgramDescriptor m_kGeometryDesc;

        // The pixel descriptor for this pass.
        NiGPUProgramDescriptor m_kPixelDesc;

        // Should the pass use the render states on the NiRenderObject?
        bool m_bUsesNiRenderState;

        // Should the pass override the alpha states for this object?
        bool m_bAlphaOverride;

        // If alpha is overridden, should alpha blending be enabled?
        bool m_bAlphaBlend;

        // Should the offset accounting be reset?
        bool m_bResetObjectOffsets;

        // The set of object offsets bound to this pass
        NiTObjectSet<ObjectResourceOffset> m_kObjectOffsets;

        // Inherit the source blend mode from the previous pass
        bool m_bUsePreviousSrcBlendMode;

        // Inherit the destination blend mode from the previous pass
        bool m_bUsePreviousDestBlendMode;

        // The specific source blend mode for this pass if not inherited.
        NiAlphaProperty::AlphaFunction m_eSrcBlendMode;

        // The specific destination blend mode for this pass if not inherited.
        NiAlphaProperty::AlphaFunction m_eDestBlendMode; 
    };

    // A fallback function allows an application to handle situations where the
    // generated shader does not properly compile.
    typedef bool (*SHADERFALLBACKFUNC)(
        NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
        unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
        unsigned int uiMaxCount, unsigned int& uiCount, 
        unsigned int& uiFunctionData);

    // Utility methods for adding and removing shader fallback functions from 
    // a fragment material. This allows the material to recover when a program 
    // has been generated that isn't capable of being compiled.
    unsigned int AddShaderFallbackFunc(SHADERFALLBACKFUNC pfnFallback);
    bool RemoveShaderFallbackFunc(SHADERFALLBACKFUNC pfnFallback);
    bool RemoveShaderFallbackFunc(unsigned int uiIndex);
    void RemoveAllShaderFallbackFuncs();

    // This function returns the number of non-zero elements in the array.
    // Note: since non-zero elements are not necessarily contiguous, 
    // applications must not use this number as a maximum iteration index or 
    // risk missing non-zero elements. 
    unsigned int GetShaderFallbackFuncCount() const;

    // This function returns the number of contiguous elements over which an 
    // application must iterate in order to encounter all non-zero elements. 
    // This number is greater than or equal to the number of non-zero elements,
    // which is the effective size.
    unsigned int GetShaderFallbackFuncArrayCount() const;

    // Utility methods for accessing the entries in the shader fallback 
    // function array.
    unsigned int FindShaderFallbackFunc(SHADERFALLBACKFUNC pfnFallback) const;
    SHADERFALLBACKFUNC GetShaderFallbackFunc(unsigned int uiIndex) const;

    // This method adds all of standard fallbacks for a given fallback 
    // material. A class derived from NiFragmentMaterial can fill in this 
    // function to provide a set of known fallback techniques.
    virtual void AddDefaultFallbacks();
    
    // Get and set the maximum number of passes allowed by the fallback system.
    // Once a shader has reached this maximum pass count, the error shader 
    // will be displayed. 
    void SetMaxPassCount(unsigned int uiMaxPassCount);
    unsigned int GetMaxPassCount() const;

protected:
    // This is a protected constructor that will be called by derived classes.
    // It creates a fragment material using the given node library. It names 
    // the material kName. If bAutoCreateCaches is true, then a cache for this
    // type of material will be created using the static renderer pointer. The 
    // version numbers will be stored in the cache so that when a material is 
    // updated to a newer version, older cached shaders will be invalidated 
    // and then regenerated.
    NiFragmentMaterial(NiMaterialNodeLibrary* pkLibrary,
        const NiFixedString& kName, unsigned short usVertexVersion, 
        unsigned short usGeometryVersion, unsigned short usPixelVersion, 
        bool bAutoCreateCaches);

    enum
    {
        // The size of the internal material descriptor to NiShader hash table.
        MATERIAL_HASH_BUCKET_COUNT = 37
    };

    // Compute the internal hash code for this material descriptor.
    virtual unsigned int GetHashCode(const NiMaterialDescriptor* pkDescriptor);

    // Compute the internal hash code for this GPU program descriptor.
    virtual unsigned int GetHashCode(
        const NiGPUProgramDescriptor* pkDescriptor);
    // Retrieve the NiShader matching this material descriptor from the hash 
    // table.
    NiShader* RetrieveFromHash(const NiMaterialDescriptor* pkDescriptor);

    // Insert the input NiShader into the hash table.
    void InsertIntoHash(NiShader* pkShader);

    // Remove the input NiShader from the hash table.
    void RemoveFromHash(NiShader* pkShader);

    // Determine whether the shader associated with the given material 
    // descriptor is known to have failed creation in the past.
    bool CheckFailedShader(const NiMaterialDescriptor* pkDescriptor);

    // Set a given material descriptor as being unable to produce a valid
    // shader.
    void SetFailedShader(const NiMaterialDescriptor* pkDescriptor);

    // Set a given program descriptor as being unable to produce a valid 
    // program.
    void SetFailedGPUProgram(NiGPUProgram::ProgramType eProgramType,
        const NiGPUProgramDescriptor* pkDescriptor);

    // Compute the NiShaderAttributeDesc::AttributeType from a given string 
    // and vice versa.
    virtual NiShaderAttributeDesc::AttributeType 
        GetAttributeTypeFromString(const char* pcValue);
    virtual NiFixedString GetStringFromAttributeType(
        NiShaderAttributeDesc::AttributeType eType);

    // Generate the material descriptor for a given input render object. 
    // This must be implemented by derived classes.
    virtual bool GenerateDescriptor(const NiRenderObject* pkGeometry, 
        const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects,
        NiMaterialDescriptor& kMaterialDescriptor) = 0;

    // Generate the NiShader for the given material descriptor.
    virtual NiShader* GenerateShader(
        NiMaterialDescriptor& kMaterialDescriptor);

    // Generate the GPU programs for the given material descriptor.
    virtual bool PrecacheGPUPrograms(
        NiMaterialDescriptor& kMaterialDescriptor);
    
    // This internal class stores the set of variables for the currently 
    // compiling shader program.
    class Context : public NiMemObject
    {
    public:
        // The owner of the shade tree being generated
        NiMaterialConfiguratorPtr m_spConfigurator;

        // Convenience pointer for all static variables.
        NiMaterialResourceProducerNodePtr m_spStatics;

        // Convenience pointer for all uniform variables.
        NiMaterialResourceProducerNodePtr m_spUniforms;

        // Convenience pointer for all input variables.
        NiMaterialResourceProducerNodePtr m_spInputs;

        // Convenience pointer for all output variables.
        NiMaterialResourceConsumerNodePtr m_spOutputs;

        // Convenience pointer for all "stack" variables used when constructing
        // the shade-tree. Only resources from nodes added to the tree should
        // be added here.
        NiTPrimitiveArray<NiMaterialResource*> m_akVariables;

    };
    
    // This method takes the current context and generates shader code from the
    // shade tree. This code is then saved in the program cache for this 
    // program type.
    virtual NiGPUProgram* EvaluateProgram(Context& kContext,
        const char* pcFilename, NiGPUProgram::ProgramType eProgType, 
        NiTObjectPtrSet<NiMaterialResourcePtr>& kUniforms);

    // This method is called on each rendering pass to compute the pixel, 
    // geometry, and vertex shader programs for that pass and set up the 
    // constant maps for that pass.
    virtual ReturnCode GenerateShaderProgramsForPass(
        NiGPUProgramDescriptor* pkVertexDesc, 
        NiGPUProgramDescriptor* pkGeometryDesc,
        NiGPUProgramDescriptor* pkPixelDesc,
        const NiTObjectSet<ObjectResourceOffset>& kObjectOffsets, 
        NiShader* pkShader, unsigned int uiRenderPass);
    
    // This method controls the wiring of the vertex shader material nodes.
    virtual bool GenerateVertexShadeTree(Context& kContext, 
        NiGPUProgramDescriptor* pkDesc);

    // This method controls the wiring of the geometry shader material nodes.
    virtual bool GenerateGeometryShadeTree(Context& kContext, 
        NiGPUProgramDescriptor* pkDesc);

    // This method controls the wiring of the pixel shader material nodes.
    virtual bool GeneratePixelShadeTree(Context& kContext, 
        NiGPUProgramDescriptor* pkDesc);

    // This method converts a GPU descriptor into a GPU program for the given
    // program type. A failure to compile will result in NULL being returned.
    NiGPUProgram* GenerateShaderProgram(
        NiGPUProgram::ProgramType eProgType,
        NiGPUProgramDescriptor* pkDesc, 
        NiTObjectPtrSet<NiMaterialResourcePtr>& kUniforms);

    // This method creates render passes for a given material descriptor by 
    // decomposing into one or more NiStandardPixelProgramDescriptors and 
    // NiStandardVertexProgramDescriptors.
    virtual ReturnCode GenerateShaderDescArray(
        NiMaterialDescriptor* pkMaterialDescriptor,
        RenderPassDescriptor* pkRenderPasses, unsigned int uiMaxCount,
        unsigned int& uiCountAdded) = 0;

    // This method takes GPU programs generated by the set of shader generating
    // functions and verifies that they were created correctly. It should 
    // return RC_SUCCESS if all the required shaders were created and a 
    // relevant failure code in other cases. This function needs to be 
    // implemented by derived classes.
    virtual unsigned int VerifyShaderPrograms(NiGPUProgram* pkVertexShader,
        NiGPUProgram* pkGeometryShader, NiGPUProgram* pkPixelShader) = 0;

    // This method will try to add the given material resource to the shader 
    // constant map for a given pass and shader program.
    virtual bool AddResourceToShaderConstantMap(NiShader* pkShader,
        NiShaderConstantMap* pkMap, NiMaterialResource* pkRes, 
        NiGPUProgramDescriptor* pkVertexDesc, 
        NiGPUProgramDescriptor* pkGeometryDesc, 
        NiGPUProgramDescriptor* pkPixelDesc, 
        const NiTObjectSet<ObjectResourceOffset>& kObjectOffsets, 
        unsigned int uiPass, NiGPUProgram::ProgramType eCurrentType);

    // This method creates the shader for a given material descriptor. This 
    // method should simply create an NiShader derived class. Later 
    // methods will populate it. 
    virtual NiShader* CreateShader(NiMaterialDescriptor* pkDesc) = 0;

    // This method sets the packing requirements for the given shader.
    // This is used to determine what vertex data is packed by the renderer. 
    // Note that as of Gamebryo 2.3, this method is superseded by 
    // NiMaterial::GetVertexInputSemantics.
    virtual bool SetupPackingRequirements(NiShader* pkShader,
        NiMaterialDescriptor* pkMaterialDescriptor,
        RenderPassDescriptor* pkRenderPasses, unsigned int uiMaxCount) = 0;

    // Generate the UV set name for a given UV coordinate input.
    virtual NiFixedString GenerateUVSetName(unsigned int uiIndex);

    // Add a texture sampler to the uniforms for this shade-tree.
    virtual NiMaterialResource* InsertTextureSampler(
        Context& kContext, 
        const NiFixedString& kMapName,
        TextureMapSamplerType eSamplerType,
        unsigned int uiOccurance,
        NiShaderAttributeDesc::ObjectType eObjectType = 
            NiShaderAttributeDesc::OT_UNDEFINED);

    // Adds the input, output, uniform, and static nodes to the current
    // shade tree.
    virtual bool AddDefaultMaterialNodes(Context& kContext,
        NiGPUProgramDescriptor* pkDesc, NiGPUProgram::ProgramType eType);
   
    // Add a NiShaderConstantMap::DefinedMappings shader constant to the 
    // uniforms for this shade-tree.
    virtual NiMaterialResource* AddOutputPredefined(NiMaterialNode* pkNode,
        NiShaderConstantMap::DefinedMappings eMapping, 
        unsigned int uiNumRegisters = 1, unsigned int uiCount = 1,
        unsigned int uiExtraData = 0);

    // Add a NiShaderConstantMap::ObjectMappings shader constant to the 
    // uniforms for this shade-tree.
    virtual NiMaterialResource* AddOutputObject(NiMaterialNode* pkNode,
        NiShaderConstantMap::ObjectMappings eMapping, 
        NiShaderAttributeDesc::ObjectType eObjectType,
        unsigned int uiObjectCount, const char* pcVariableModifier = NULL,
        unsigned int uiCount = 1);

    // Add a custom extra data shader constant to the uniforms for this 
    // shade-tree.
    virtual NiMaterialResource* AddOutputAttribute(NiMaterialNode* pkNode,
        const NiFixedString& kVariableName,
        NiShaderAttributeDesc::AttributeType eType, 
        unsigned int uiCount = 1);

    // Add a global attribute shader constant to the uniforms for this 
    // shade-tree.
    virtual NiMaterialResource* AddOutputGlobal(NiMaterialNode* pkNode,
        const NiFixedString& kVariableName,
        NiShaderAttributeDesc::AttributeType eType,
        unsigned int uiCount = 1);

    // Utility method that searches through the object offsets for all render 
    // passes and finds the highest offset thus far for the given ObjectType.
    static unsigned int GetHighestObjectOffset(
        NiShaderAttributeDesc::ObjectType eObjectType, 
        RenderPassDescriptor* pkRenderPasses, unsigned int uiCount);

    // Find a material node from the material libraries that matches the input 
    // name and clone it for use in a shade-tree.
    virtual NiMaterialNode* GetAttachableNodeFromLibrary(const NiFixedString&
        kNodeName);

    NiTObjectPtrSet<NiMaterialNodeLibraryPtr> m_kLibraries;
    NiGPUProgramCachePtr m_aspProgramCaches[NiGPUProgram::PROGRAM_MAX];

    NiTObjectPtrSet<NiShaderPtr> 
        m_akShaderHashTable[MATERIAL_HASH_BUCKET_COUNT];
    NiTObjectSet<NiMaterialDescriptor> 
        m_akFailedShaderDescriptorHashTable[MATERIAL_HASH_BUCKET_COUNT];

    NiTPrimitiveArray<SHADERFALLBACKFUNC> m_kShaderFallbackFuncs;

    unsigned int m_uiMaxPassCount;

    unsigned short m_usVertexVersion;
    unsigned short m_usGeometryVersion;
    unsigned short m_usPixelVersion;

#if defined(NIDEBUG)
    NiFixedString m_kDebugIdentifier;
#endif

    static bool ms_bLoadProgramCacheOnCreation;
    static bool ms_bAutoWriteProgramCache;
    static bool ms_bWriteDebugProgram;
    static bool ms_bLockProgramCache;
    static bool ms_bAutoCreateCache;
};

typedef NiPointer<NiFragmentMaterial> NiFragmentMaterialPtr;

#endif  //#ifndef NIFRAGMENTMATERIAL_H
