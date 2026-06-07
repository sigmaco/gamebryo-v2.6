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

// Precompiled Header
#include "NiD3D10RendererPCH.h"

#include "NiD3D10Error.h"

#include <NiSystem.h>
#include "NiRenderer.h"

NiD3D10Error* NiD3D10Error::ms_pkD3D10Error = NULL;

//---------------------------------------------------------------------------
void NiD3D10Error::_SDMInit()
{
    // Don't need critical section because this is only called during startup
    ms_pkD3D10Error = NiNew NiD3D10Error;
}
//---------------------------------------------------------------------------
void NiD3D10Error::_SDMShutdown()
{
    // Don't need critical section because this is only called during startup
    NiDelete ms_pkD3D10Error;
    ms_pkD3D10Error = NULL;
}
//---------------------------------------------------------------------------
NiD3D10Error::NiD3D10Error() :
    m_eLastErrorMessage(NID3D10ERROR_NONE)
{
    memset(m_acLastAdditionalInfo, 0, sizeof(m_acLastAdditionalInfo));
}
//---------------------------------------------------------------------------
NiD3D10Error::~NiD3D10Error()
{
    /* */
}
//---------------------------------------------------------------------------
void NiD3D10Error::ReportWarning(const char* pcWarningMessage, ...)
{
    if (pcWarningMessage == NULL)
        pcWarningMessage = "";

    const size_t stBufferLength = MAX_ADDITIONAL_INFO_STRING_LENGTH;
    char acErrorString[stBufferLength] = "NiD3D10Renderer WARNING: ";
    size_t stIndex = strlen(acErrorString);

    va_list kArgs;
    va_start(kArgs, pcWarningMessage);
#if _MSC_VER >= 1400
    stIndex += vsprintf_s(&acErrorString[stIndex], stBufferLength - stIndex, 
        pcWarningMessage, kArgs);
#else //#if _MSC_VER >= 1400
    stIndex += vsprintf(&acErrorString[stIndex], pcWarningMessage, kArgs);
#endif //#if _MSC_VER >= 1400
    va_end(kArgs);

    // Newline and null terminate
    stIndex = NiMin((int)stIndex, (int)(sizeof(acErrorString) - 2));
    if (acErrorString[stIndex - 1] != '\n')
    {
        acErrorString[stIndex] = '\n';
        acErrorString[stIndex + 1] = NULL;
    }
    else
    {
        acErrorString[stIndex] = NULL;
    }

    NiOutputDebugString(acErrorString);
}
//---------------------------------------------------------------------------
void NiD3D10Error::ReportError(ErrorMessage eError, 
    const char* pcAdditionalMessage, ...)
{
    if (ms_pkD3D10Error == NULL)
        return;

    ms_pkD3D10Error->m_eLastErrorMessage = eError;
    if (pcAdditionalMessage == NULL)
        pcAdditionalMessage = "";

    const size_t stBufferLength = 2 * MAX_ADDITIONAL_INFO_STRING_LENGTH;
    char acErrorString[stBufferLength];
    NiSprintf(acErrorString, stBufferLength, "NiD3D10Renderer ERROR: %s",
        GetErrorText(eError));
    size_t stIndex = strlen(acErrorString);

    va_list kArgs;
    va_start(kArgs, pcAdditionalMessage);
#if _MSC_VER >= 1400
    stIndex += vsprintf_s(&acErrorString[stIndex], stBufferLength - stIndex, 
        pcAdditionalMessage, kArgs);
#else //#if _MSC_VER >= 1400
    stIndex += vsprintf(&acErrorString[stIndex], pcAdditionalMessage, kArgs);
#endif //#if _MSC_VER >= 1400
    va_end(kArgs);

    // Newline and null terminate
    stIndex = NiMin((int)stIndex, (int)(sizeof(acErrorString) - 2));
    if (acErrorString[stIndex - 1] != '\n')
    {
        acErrorString[stIndex] = '\n';
        acErrorString[stIndex + 1] = NULL;
    }
    else
    {
        acErrorString[stIndex] = NULL;
    }

    NiRenderer::SetLastErrorString(acErrorString);

    NiOutputDebugString(acErrorString);
}
//---------------------------------------------------------------------------
NiD3D10Error::ErrorMessage NiD3D10Error::GetLastErrorMessage()
{
    if (ms_pkD3D10Error == NULL)
        return NID3D10ERROR_ERROR_SYSTEM_ERROR;
    return ms_pkD3D10Error->m_eLastErrorMessage;
}
//---------------------------------------------------------------------------
const char* const NiD3D10Error::GetErrorText(ErrorMessage eMessage)
{
    switch (eMessage)
    {
    case NID3D10ERROR_NONE:
        return "No error.";
    case NID3D10ERROR_ERROR_SYSTEM_ERROR:
        return "Error with the NiD3D10Error system.";
    case NID3D10ERROR_D3D10_LIB_MISSING:
        return "Error loading D3D10 library.";
    case NID3D10ERROR_DXGI_LIB_MISSING:
        return "Error loading DXGI library.";
    case NID3D10ERROR_DEVICE_CREATION_FAILED:
        return "Error creating D3D10 device.";
    case NID3D10ERROR_FACTORY_CREATION_FAILED:
        return "Error creating DXGI factory.";
    case NID3D10ERROR_SWAP_CHAIN_CREATION_FAILED:
        return "Error creating swap chain resource.";
    case NID3D10ERROR_TEXTURE1D_CREATION_FAILED:
        return "Error creating 1D texture resource.";
    case NID3D10ERROR_TEXTURE2D_CREATION_FAILED:
        return "Error creating 2D texture resource.";
    case NID3D10ERROR_TEXTURE3D_CREATION_FAILED:
        return "Error creating 3D texture resource.";
    case NID3D10ERROR_BUFFER_CREATION_FAILED:
        return "Error creating buffer resource.";
    case NID3D10ERROR_RENDER_TARGET_VIEW_CREATION_FAILED:
        return "Error creating render target view.";
    case NID3D10ERROR_DEPTH_STENCIL_VIEW_CREATION_FAILED:
        return "Error creating depth stencil view.";
    case NID3D10ERROR_BLEND_STATE_CREATION_FAILED:
        return "Error creating blend state object.";
    case NID3D10ERROR_DEPTH_STENCIL_STATE_CREATION_FAILED:
        return "Error creating depth stencil state object.";
    case NID3D10ERROR_RASTERIZER_STATE_CREATION_FAILED:
        return "Error creating rasterizer state object.";
    case NID3D10ERROR_SAMPLER_CREATION_FAILED:
        return "Error creating sampler object.";
    case NID3D10ERROR_INPUT_LAYOUT_CREATION_FAILED:
        return "Error creating input layout object.";
    case NID3D10ERROR_DISPLAY_SWAP_CHAIN_FAILED:
        return "Error displaying swap chain.";
    case NID3D10ERROR_GET_BUFFER_FROM_SWAP_CHAIN_FAILED:
        return "Error obtaining buffer from swap chain.";
    case NiD3D10ERROR_SHADER_RESOURCE_VIEW_CREATION_FAILED:
        return "Error creating resource view buffer.";
    case NiD3D10ERROR_TEXTURE1D_LOCK_FAILED:
        return "Error locking 1D texture.";
    case NiD3D10ERROR_TEXTURE2D_LOCK_FAILED:
        return "Error locking 2D texture.";
    case NiD3D10ERROR_TEXTURE3D_LOCK_FAILED:
        return "Error locking 3D texture.";
    case NiD3D10ERROR_UNSUPPORTED_RESOURCE_LOCK_FAILED:
        return "Error locking unsupported resource.";
    case NiD3D10ERROR_SHADER_CONSTANT_MAPPING_FAILED:
        return "Error mapping shader constants.";
    case NiD3D10ERROR_SHADER_MISSING:     
        return "Error due to missing shader.";
    default:
        return "Missing error text.";
    }
}
//---------------------------------------------------------------------------
