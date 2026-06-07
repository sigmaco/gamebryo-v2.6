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

#ifndef GamebryoApp_h
#define GamebryoApp_h

#include "CubeMapCamera.h"

#include <NiApplication.h>

class CubeMapMaker : public NiApplication
{
public:
	CubeMapMaker();

    virtual bool Initialize();
    virtual void Terminate();
    virtual bool CreateScene();

private:
	NiPixelData* CubeMapFaceToPixelData(NiRenderedCubeMap* pkRenderedCubeMap,
		 size_t stFaceId);
	bool SaveCubeMapFacesToFiles();

	enum IconType
	{
		INFO_ICON,
		WARN_ICON,
		ERROR_ICON
	};

	void ShowMessage(IconType eType, const char* pcFormat,  ...);

    CubeMapCameraPtr m_spCubeCamera;
    NiRenderedCubeMapPtr m_spRenderedCube;

    char m_acNifFilename[NI_MAX_PATH];
    char m_acCubeMapNodeName[NI_MAX_PATH];
	int m_iCubeMapSize;
};

#endif // GamebryoApp_h