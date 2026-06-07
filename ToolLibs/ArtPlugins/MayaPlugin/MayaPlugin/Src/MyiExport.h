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

#ifndef MYIEXPORT_H
#define MYIEXPORT_H

#include <NiPluginToolkitDefinitions.h>

enum { VRHRC_WORLD=0,VRHRC_FLAT,VRHRC_FULL };
enum { VRSEL_ALL=0,VRSEL_PICKED,VRSEL_ACTIVE };
enum { FILTER_NEAREST=0, FILTER_BILERP, FILTER_TRILERP, 
    FILTER_NEAREST_MIPNEAREST,FILTER_NEAREST_MIPLERP, FILTER_BILERP_MIPNEAREST,
    FILTER_MAX_MODES };
enum { SHADING_FLAT=0, SHADING_GOURAUD };
enum { COLOR_WHITE=0, COLOR_VERTEX, COLOR_MAT_AMB, COLOR_MAT_AMB_X_VERTEX,
       COLOR_LIT, COLOR_LIT_X_VERTEX, COLOR_LIT_PLUS_VERTEX, COLOR_MAX_MODES };

enum
{
    TEXTURE_PLATFORM_ANY = 1,
    TEXTURE_PLATFORM_GAME_CUBE,
    TEXTURE_PLATFORM_OPEN_GL,
    TEXTURE_PLATFORM_PS2,
    TEXTURE_PLATFORM_XBOX
};

enum
{
    TEXTURE_SIZE_REDUCTION_TEXTURE_DEFAULT = 0,
    TEXTURE_SIZE_REDUCTION_1_2,
    TEXTURE_SIZE_REDUCTION_1_4,
    TEXTURE_SIZE_REDUCTION_1_8,
    TEXTURE_SIZE_REDUCTION_1_16,
    TEXTURE_SIZE_REDUCTION_1_32,
    TEXTURE_SIZE_REDUCTION_1_64
};

enum
{
    TEXTURE_SAVE_INTERNAL = 0,
    TEXTURE_SAVE_EXTERNAL_AS_NIF,
    TEXTURE_SAVE_EXTERNAL_AS_ORIGINAL
};

enum
{
    VIEWER_DX8 = 0,
    VIEWER_DX9,
    VIEWER_KFMVIEWER,
    VIEWER_SCENEIMMERSE,
    VIEWER_XENON,
    VIEWER_PS3,
    VIEWER_WII,
    VIEWER_D3D10,
    VIEWER_CUSTOM
};

class MyiExportGlobals
{
public:
    MyiExportGlobals(void);
    ~MyiExportGlobals(void);

    enum TextureFormat
    {
        PLATFORM_DEFAULT = 0,
        TEXTURE_DEFAULT,
        TRUE_COLOR,
        HIGH_COLOR,
        PALETTE_8,
        PALETTE_4,
        BUMP16,
        BUMPLUMA32,
        COMPRESS1,
        COMPRESS3,
        COMPRESS5,
        TEXTURE_FORMAT_MAX,
        TEXTURE_FORMAT_INVALID = 255
    };

    float m_fLinearUnitMultiplier;
    float m_fFramesPerSecond;
    int m_iFilterType;
    bool m_bSaveExport;
    bool m_bViewExport;
    float m_fRed;
    float m_fGreen;
    float m_fBlue;
    int m_iTextureSaveLocation;


    bool m_bLightingOptimizeSceneRootLights;
    bool m_bUseCurrentWorkingUnits;

    bool m_bExportDefaultMayaCameras;
    int m_iWindowWidth;
    int m_iWindowHeight;
    bool m_bFullScreen;
    bool m_bAutoCenterLODs;
    bool m_bFlipTexturesInsteadOfUVs;
    bool m_bAddDefaultAmbientForMayaLights;
    bool m_bGuaranteeObjectNamesAreUnique;
    bool m_bUseOldParticleSystemFields;
    bool m_bUseOldCameraFOV;

    int m_iViewer;
    char m_acCustomViewerScript[NI_MAX_PATH];
    char* m_pcScriptTemplatePath;
    char* m_pcScriptPath;
    bool m_bOptimizeMeshForViewer;

    bool m_bDefaultAmbientInTextureMaterial;

    int m_iGenericTextureFormat;
    int m_iGenericTextureReduction;
    char *m_pcGenericTextureSaveDirectory;

    int m_iXenonTextureFormat;
    int m_iXenonTextureReduction;
    char *m_pcXenonTextureSaveDirectory;

    int m_iDX9TextureFormat;
    int m_iDX9TextureReduction;
    char *m_pcDX9TextureSaveDirectory;

    int m_iD3D10TextureFormat;
    int m_iD3D10TextureReduction;
    char *m_pcD3D10TextureSaveDirectory;

    int m_iPS3TextureFormat;
    int m_iPS3TextureReduction;
    char *m_pcPS3TextureSaveDirectory;

    int m_iWiiTextureFormat;
    int m_iWiiTextureReduction;
    char *m_pcWiiTextureSaveDirectory;

    /*
    int m_TextureForPlatform;
    char *m_szTextureSaveDirectory;
    int m_TextureQuality;
    */

    bool m_bAnimationExportAnimationRange;
    bool m_bAnimationLoopOverRange;
    int m_iAnimationMaximumFrameRate;
    float m_fMinimumBoneInfluence;
    bool m_bSampleAllRotation;
    bool m_bSampleRotationAsQuaternion;
    bool m_bSampleAllTranslation;

    bool m_bExportKeyFrames;
    bool m_bKeyFrameMultipleSequencesPerKeyFrame;
    bool m_bKeyFrameFileName;
    bool m_bKeyFrameCharacterName;

    bool m_bAnimationAutoReduceKeyFrames;
    float m_fAnimationMaxReductionError;


    bool m_bUseDbgSt;
    char* m_pcPS2ViewerDbgStIPAddress;
    int m_iPS2FieldMode;
    bool m_bPS2TwoCircuitAntialiasing;
    int m_iPS2VideoFormat;
    int m_iPS2ZBufferDepth;

    int m_iStripifyType;
    bool m_bStripifyForPreview;

    bool m_bUseLastSavedScript;
    bool m_bForceDefaultScript;

    bool m_bRunSilently;

    char* m_pcLoggerPath;

    char* m_DefaultProfileName;

    void Initialize();
    void Shutdown();
};

extern MyiExportGlobals gExport;


// DtExt Functions
char *MDt_GetPathName (char *LFullStr, char *LRet, int LMaxLen);
void InitDtScene( const MFileObject& fileObject );
void BuildDtDataBase( const MString & options, 
    MPxFileTranslator::FileAccessMode mode );
void ReloadScripts();
void SetDtTextureSearchPaths( void );
bool DtImageLibFileSave(int iTextureID, char *szPath);
bool DtConvertTextureFormatNameToEnum(const MString& kTextureFormatString, 
    int& iTextureFormat);

#endif