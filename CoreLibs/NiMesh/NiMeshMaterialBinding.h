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
#ifndef NIMESHMATERIALBINDING_H
#define NIMESHMATERIALBINDING_H

class NiMesh;

/// Common code for creating mesh material bindings.
class NIMESH_ENTRY NiMeshMaterialBinding
{
public:
    /**
        Common code for creating a mesh material binding, 
        shared by several renderers.

        Template parameter TContext allows platform implementations to add
        callbacks that implement platform specific code. 
        
        Also, the derived context should add any additional "local" variables
        needed.
    */
    template <typename TContext>
    static bool CreateBinding(
        TContext& kContext,
        const NiSemanticAdapterTable& kAdapterTable);


    /**
        A small struct used as the template argument to 
        NiMeshMaterialBinding::CreateBinding.

        Each platform extends CreateBindingBaseContext to contain data members
        relevant to creating the binding for that platform. For example,
        NiDX9MeshMaterialBinding would invoke 
        CreateBindingContext<DX9CreateBindingContext>. The subclass would 
        include storage for an array of D3DVERTEXELEMENT9 objects necessary
        for creating a DX9 binding. Users are not expected to need to subclass
        CreateBindingBaseContext or interface with it directly.
    */
    struct CreateBindingBaseContext
    {
        /// The mesh this context applies to (caller must set explicitly).
        NiMesh* m_pkMesh;
        
        /// The stream index, set for use by the callback function.
        NiUInt32 m_uiCurrentStream;
        
        /// The element index in the stream, set for use by the callback
        /// function.
        NiUInt32 m_uiCurrentElement;

        inline CreateBindingBaseContext() : 
            m_uiCurrentStream(0),
            m_uiCurrentElement(0)
        {
        }

        /**
            Virtual function that platform specific subclasses override to
            perform relevant binding operations for that platform.

            Since each platform must interface with the underlying graphics
            API, this function is called after each entry is added to the
            mesh material binding to create any platform specific
            data. For example, DX9CreateBindingContext would use this function
            to translate the Gamebryo entries into D3DVERTEXELEMENT9 entries
            that are used to create the instance of 
            IDirect3DVertexDeclaration9 associated with the mesh material 
            binding.
        */
        virtual bool CallBack_EndOfElementLoop(
            NiUInt32 uiStream,
            const NiDataStreamRef* pkStreamRef,
            const NiDataStreamElement kElement,
            NiDataStreamElement::Format ePackedDataFormat, 
            const NiFixedString& kRendererSemantic, 
            const NiUInt8 uiRendererSemanticIndex, 
            NiUInt32 uiPackedDataFormatComponentCount, 
            NiUInt32 uiPackedOffset) = 0;

        /// Destructor.
        inline virtual ~CreateBindingBaseContext() {}
    };

    /// Set the option to create missing streams by default if a shader
    /// requests them.  The data in this stream will be (1,1,1,1) for colors
    /// and (0,0,0,0) for anything else.  This is off by default.
    /// A warning will be printed when data is added.
    static void SetCreateMissingStreams(bool bCreateMissingStreams);

    /// Get whether the option to create missing streams has been turned on.
    static bool GetCreateMissingStreams();

protected:
    static bool ms_bCreateMissingStreams;
};

#include "NiMeshMaterialBinding.inl"

#endif  //#ifndef NIDX9MESHMATERIALBINDING_H
