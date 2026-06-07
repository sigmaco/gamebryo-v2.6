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

#ifndef NID3D10ERROR_H
#define NID3D10ERROR_H

#include "NiD3D10RendererLibType.h"
#include <NiMemObject.h>

class NID3D10RENDERER_ENTRY NiD3D10Error : public NiMemObject
{
public:
    enum ErrorMessage
    {
        NID3D10ERROR_NONE,

        // Meta-error
        NID3D10ERROR_ERROR_SYSTEM_ERROR,

        // Renderer creation
        NID3D10ERROR_D3D10_LIB_MISSING,
        NID3D10ERROR_DXGI_LIB_MISSING,
        NID3D10ERROR_DEVICE_CREATION_FAILED,
        NID3D10ERROR_FACTORY_CREATION_FAILED,

        // Resource creation
        NID3D10ERROR_SWAP_CHAIN_CREATION_FAILED,
        NID3D10ERROR_TEXTURE1D_CREATION_FAILED,
        NID3D10ERROR_TEXTURE2D_CREATION_FAILED,
        NID3D10ERROR_TEXTURE3D_CREATION_FAILED,
        NID3D10ERROR_BUFFER_CREATION_FAILED,
        NID3D10ERROR_RENDER_TARGET_VIEW_CREATION_FAILED,
        NID3D10ERROR_DEPTH_STENCIL_VIEW_CREATION_FAILED,
        NID3D10ERROR_BLEND_STATE_CREATION_FAILED,
        NID3D10ERROR_DEPTH_STENCIL_STATE_CREATION_FAILED,
        NID3D10ERROR_RASTERIZER_STATE_CREATION_FAILED,
        NID3D10ERROR_SAMPLER_CREATION_FAILED,
        NID3D10ERROR_INPUT_LAYOUT_CREATION_FAILED,

        // Other D3D10 API
        NID3D10ERROR_DISPLAY_SWAP_CHAIN_FAILED,
        NID3D10ERROR_GET_BUFFER_FROM_SWAP_CHAIN_FAILED,
        NiD3D10ERROR_SHADER_RESOURCE_VIEW_CREATION_FAILED,
        NiD3D10ERROR_TEXTURE1D_LOCK_FAILED,
        NiD3D10ERROR_TEXTURE2D_LOCK_FAILED,
        NiD3D10ERROR_TEXTURE3D_LOCK_FAILED,
        NiD3D10ERROR_UNSUPPORTED_RESOURCE_LOCK_FAILED,
        NiD3D10ERROR_SHADER_CONSTANT_MAPPING_FAILED,
        NiD3D10ERROR_SHADER_MISSING,

    };
    static ErrorMessage GetLastErrorMessage();
    static const char* const GetErrorText(ErrorMessage eMessage);

    // *** begin Emergent internal use only ***

    static void ReportMessage();
    static void ReportWarning(const char* pcWarningMessage, ...);
    static void ReportError(ErrorMessage eError, 
        const char* pcAdditionalMessage = NULL, ...);

    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

protected:
    NiD3D10Error();
    ~NiD3D10Error();

    ErrorMessage m_eLastErrorMessage;
    enum
    {
        MAX_ADDITIONAL_INFO_STRING_LENGTH = 1024
    };
    char m_acLastAdditionalInfo[MAX_ADDITIONAL_INFO_STRING_LENGTH];

    static NiD3D10Error* ms_pkD3D10Error;
};

#endif // NID3D10ERROR_H