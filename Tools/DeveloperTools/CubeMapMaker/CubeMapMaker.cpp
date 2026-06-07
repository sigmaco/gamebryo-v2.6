// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
// 
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
// 
// Copyright (c) 1996-2008 Emergent Game Technologies.
// All Rights Reserved.
// 
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#include "CubeMapMaker.h"

// This app only works on Windows since it uses the D3D API to copy 
// cube map textures from GPU memory to CPU memory.
#include <NiDX9Renderer.h>
#include <NiDX92DBufferData.h>
#include <NiDX9RenderedCubeMapData.h>
#include <NiAnimation.h>
#include <NiCollision.h>
#include <NiParticle.h>
#include <NiLicense.h>

NiEmbedGamebryoLicenseCode;

#include "NiTGAWriter.h"

#include <direct.h>

const char g_acUsageMsg[] = "Gamebryo Cube Map Maker Usage:\n\n"
	"%s -nif <NIF file> -node <Node name> -size <Map size>\n\n"
	"Where\n"
	"   <NIF file> - filename of scene to load\n"
	"   <Node name> - name of node to position cube map camera at\n"
	"   <Map size> - the face width of the cube map in pixels\n\n"
    "Example: %s -nif C:\\Assets\\Scene.nif -node Cone01 -size 512\n";

struct FaceInfo
{
	NiRenderedCubeMap::FaceID NiFaceID;
	D3DCUBEMAP_FACES D3DFaceId;
	char acFaceName[3];
};

FaceInfo g_FaceIdTable[]=
{
	{NiRenderedCubeMap::FACE_POS_X, D3DCUBEMAP_FACE_POSITIVE_X, "+X"},
	{NiRenderedCubeMap::FACE_NEG_X, D3DCUBEMAP_FACE_NEGATIVE_X, "-X"},
	{NiRenderedCubeMap::FACE_POS_Y, D3DCUBEMAP_FACE_POSITIVE_Y, "+Y"},
	{NiRenderedCubeMap::FACE_NEG_Y, D3DCUBEMAP_FACE_NEGATIVE_Y, "-Y"},
	{NiRenderedCubeMap::FACE_POS_Z, D3DCUBEMAP_FACE_POSITIVE_Z, "+Z"},
	{NiRenderedCubeMap::FACE_NEG_Z, D3DCUBEMAP_FACE_NEGATIVE_Z, "-Z"}
};
const size_t g_stFaceIdTableLen = sizeof(g_FaceIdTable)/sizeof(FaceInfo);

//---------------------------------------------------------------------------
NiApplication* NiApplication::Create()
{
	return NiNew CubeMapMaker;
}
//---------------------------------------------------------------------------
CubeMapMaker::CubeMapMaker() : 
    NiApplication("CubeMapMaker", 0, 
        0, false)
{
    memset( m_acNifFilename, 0x00, sizeof(m_acNifFilename));
    memset( m_acCubeMapNodeName, 0x00, sizeof(m_acCubeMapNodeName));
	m_iCubeMapSize = 0;

	SetMediaPath(".\\");
	SetRendererDialog(false);
	SetFullscreen(false);
}


//---------------------------------------------------------------------------
bool CubeMapMaker::Initialize()
{
	if (!(GetCommand()->String("nif", m_acNifFilename, 
			sizeof(m_acNifFilename)) && 
		GetCommand()->String("node", m_acCubeMapNodeName, 
			sizeof(m_acCubeMapNodeName)) && 
		GetCommand()->Integer("size", m_iCubeMapSize)))
	{
		ShowMessage(WARN_ICON, g_acUsageMsg, 
			m_pkAppWindow->GetWindowCaption(),
			m_pkAppWindow->GetWindowCaption());
		return false;
	}

	bool bSuccess = NiApplication::Initialize();
	if (bSuccess)
	{
		// Render cube map sides and save to files
		m_spRenderer->BeginOffScreenFrame();
		if (m_spCubeCamera)
		{
			m_spCubeCamera->PerformRendering();
			bSuccess = SaveCubeMapFacesToFiles();
		}
		m_spRenderer->EndOffScreenFrame();

		if (bSuccess)
		{
			char acAbsMediaPath[NI_MAX_PATH];
			NiStrcpy(acAbsMediaPath, NI_MAX_PATH, GetMediaPath());
			NiPath::ConvertToAbsolute(acAbsMediaPath, NI_MAX_PATH);
			ShowMessage(INFO_ICON, "Successfully created cube map images "
			"in this folder:\n   %s", acAbsMediaPath);
		}
		// Else an error message should have already been shown by another
		// method.
	}

	// Exit the application since we just wanted to render the cube map
	NiApplication::QuitApplication();

	return bSuccess;
}
//---------------------------------------------------------------------------
void CubeMapMaker::Terminate()
{
    m_spRenderedCube = NULL;
    m_spCubeCamera = NULL;

    NiApplication::Terminate();
}
//---------------------------------------------------------------------------
bool CubeMapMaker::CreateScene() 
{
	if (!NiApplication::CreateScene())
    {
        ShowMessage(ERROR_ICON, "An error occured in "
			"NiApplication::CreateScene()");
        return false;
    }

    // load the model
    NiStream kStream;
    if (!kStream.Load(m_acNifFilename))
    {
        ShowMessage(ERROR_ICON, "Unable to load %s file", 
			m_acNifFilename);
        return false;
    }

    m_spScene = (NiNode*)kStream.GetObjectAt(0);
    if (!m_spScene)
    {
        ShowMessage(ERROR_ICON, "Unable to find scene object");
        return false;
    }

	m_spRenderedCube = NiRenderedCubeMap::Create(m_iCubeMapSize, m_spRenderer);
    if (!m_spRenderedCube)
    {
		ShowMessage(ERROR_ICON, "Unsupported cube map size OR device does not "
			"support cube maps.");
		return false;
    }

    NiAVObject* pCubeMapNode = m_spScene->GetObjectByName(m_acCubeMapNodeName);
    if (!pCubeMapNode)
    {
        ShowMessage(ERROR_ICON, "Unable to find cube map location node: %s",
			m_acCubeMapNodeName);
        return false;
    }

    m_spCubeCamera = CubeMapCamera::Create(m_spRenderedCube, m_spRenderer, 
        m_spScene, pCubeMapNode);
    if (!m_spCubeCamera)
    {
        ShowMessage(ERROR_ICON, "Unable to create cube map camera");
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool CubeMapMaker::SaveCubeMapFacesToFiles()
{
	for (size_t stFaceId = 0; stFaceId < g_stFaceIdTableLen; ++stFaceId)
	{
		// Open TGA file handle
		char acName[NI_MAX_PATH];
		NiSprintf(acName, NI_MAX_PATH, "%s_%d_%s.tga", m_acCubeMapNodeName,
			m_iCubeMapSize, g_FaceIdTable[stFaceId].acFaceName);
		NiFile kFile(ConvertMediaFilename(acName), NiFile::WRITE_ONLY);
		if (!kFile)
		{
			ShowMessage(ERROR_ICON, "Unable to open image file: %s", 
				ConvertMediaFilename(acName));
			return false;
		}

		// Retrieve pixel data from GPU mem
		NiPixelDataPtr spPixelData = CubeMapFaceToPixelData(
			m_spRenderedCube, stFaceId);
		if (!spPixelData)
		{
			ShowMessage(ERROR_ICON, "Unable to retrieve cube map pixel data "
				"from GPU memory. Face being processed: %d = %s", 
				stFaceId, g_FaceIdTable[stFaceId].acFaceName);
			return false;
		}

		// Write to file
		if (!NiTGAWriter::Dump(spPixelData, kFile))
		{
			ShowMessage(ERROR_ICON, "Unable to write pixel data to file: %s",
				acName);
			return false;
		}
	}

	return true;
}
//---------------------------------------------------------------------------
NiPixelData* CubeMapMaker::CubeMapFaceToPixelData(
	NiRenderedCubeMap* pkRenderedCubeMap, size_t stFaceId)
{
	NIASSERT(pkRenderedCubeMap && stFaceId < g_stFaceIdTableLen);

	// Get pointer to D3D device
	NiDX9Renderer* pkDX9Renderer = NiDynamicCast(NiDX9Renderer, m_spRenderer);
	if (!pkDX9Renderer)
	{
		return NULL;
	}
	D3DDevicePtr pkD3DDevice = pkDX9Renderer->GetD3DDevice();

	// Get D3D surface pointer to source pixel data in GPU memory
	NiDX9RenderedCubeMapData* pkRenderedCubeMapData = 
		NiDynamicCast(NiDX9RenderedCubeMapData, 
			pkRenderedCubeMap->GetRendererData());
	if (!pkRenderedCubeMapData)
	{
		return NULL;
	}

	D3DSurfacePtr pkSrc;
	((LPDIRECT3DCUBETEXTURE9)pkRenderedCubeMapData->GetD3DTexture())->
		GetCubeMapSurface(g_FaceIdTable[stFaceId].D3DFaceId, 0, &pkSrc);
	if (!pkSrc)
	{
		return NULL;
	}

	D3DSURFACE_DESC kSurfDesc;
	pkSrc->GetDesc(&kSurfDesc);

	// Create a destination texture to copy source pixel data into, which
	// can be accessed from CPU
	D3DFORMAT eFormat = kSurfDesc.Format;
	LPDIRECT3DTEXTURE9 pkD3DReadTexture = NULL;
	pkD3DDevice->CreateTexture(kSurfDesc.Width, kSurfDesc.Height, 1, 0,
		kSurfDesc.Format, D3DPOOL_SYSTEMMEM, &pkD3DReadTexture, 0);
	if (!pkD3DReadTexture)
	{
		return NULL;
	}

	D3DSurfacePtr pkDst;
	pkD3DReadTexture->GetSurfaceLevel(0, &pkDst);

	// Copy pixel data
	pkD3DDevice->GetRenderTargetData(pkSrc, pkDst);

	pkSrc->Release();
	pkDst->Release();

	// Lock and copy the destination texture into NiPixelData object
	NiPixelData* pkPixelData = NULL;
	D3DLOCKED_RECT kLockRect;
	if (SUCCEEDED(pkD3DReadTexture->LockRect(0, &kLockRect, NULL, 
		D3DLOCK_READONLY)))
	{
		// Create an NiPixelData object that matches the buffer format & size
		pkPixelData = NiNew NiPixelData(m_spRenderedCube->GetWidth(),
			m_spRenderedCube->GetHeight(), 
			*m_spCubeCamera->GetPixelFormat(g_FaceIdTable[stFaceId].NiFaceID), 1);
		if (pkPixelData)
		{
			unsigned int uiRowSize 
				= pkPixelData->GetWidth(0) * pkPixelData->GetPixelStride();
			unsigned char* pucDest = pkPixelData->GetPixels(0);
			const unsigned char* pucSrc = (unsigned char*)kLockRect.pBits;

			for (unsigned int i = 0; i < m_spRenderedCube->GetHeight(); i++)
			{
				NiMemcpy(pucDest, pucSrc, uiRowSize);
				pucDest += uiRowSize;
				pucSrc += kLockRect.Pitch;
			}
		}
		pkD3DReadTexture->UnlockRect(0);
	}

	if (pkD3DReadTexture)
	{
		D3D_POINTER_RELEASE(pkD3DReadTexture);
	}

	return pkPixelData;
}
//---------------------------------------------------------------------------
void CubeMapMaker::ShowMessage(IconType eType, 
	const char* pcFormat, ...)
{
	NIASSERT(this->GetWindowReference());
	if (!this->GetWindowReference())
	{
		return;
	}

	va_list kArgs;
	va_start( kArgs, pcFormat );

	char acMsg[512];
	NiVsprintf( acMsg, sizeof(acMsg), pcFormat, kArgs);

	UINT uType;
	switch(eType)
	{
	case WARN_ICON:
		uType = MB_ICONWARNING;
		break;
	case ERROR_ICON:
		uType = MB_ICONERROR;
		break;
	default:
	case INFO_ICON:
		uType = MB_ICONINFORMATION;
		break;
	}

	MessageBoxA(this->GetWindowReference(), acMsg,
		m_pkAppWindow->GetWindowCaption(), 
		uType);
}
//---------------------------------------------------------------------------