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
//---------------------------------------------------------------------------

#ifndef MYIMMERSE_H
#define MYIMMERSE_H

#include <NiNode.h>             // Include needed Ni components
#include <NiAVObject.h>
#include <NiTList.h>

#include "MDt.h"                // Include Needed NDt components
#include "MDtExt.h"             // and their extentsions

#include "MyiMaterialManager.h"
#include "MyiTextureManager.h"
#include "MyiTextureEffect.h"
#include "MyiParticleSystem.h"

#include "MyiCamera.h"
#include "MyiObject.h"
#include <NiControllerExtractor.h>
#include <NiEulerRotKey.h>
#include <NiControllerExtractorSharedData.h>
#include <NiTransformController.h>
#include "maya/MObjectArray.h"


class NiScriptInfo;
class NiPSX2ReadBackData;
class NiBatchExecutionResult;
NiSmartPointer(NiNode);
NiSmartPointer(NiCamera);
NiSmartPointer(NiLight);
NiSmartPointer(NiMaterialProperty);
NiSmartPointer(NiWireframeProperty);
NiSmartPointer(NiScriptInfo);

class MyImmerse : public MyiObject
{
public:
    MyImmerse();

    ~MyImmerse( void );
    bool Export( void );
    bool MakeScene( void );
    bool VerifyUniqueNames(NiAVObject* pkNode);

    void AddSelectedObject(NiAVObject* pkObject);

    static NiXMLLogger* GetXMLLogger();
    static NiXMLLogger* InitXMLLogger();
    static void DestroyXMLLogger();

    static  NiScriptInfo* LoadScriptFromScene();
    static void SaveScriptToScene(NiScriptInfo* pkScriptInfo);

    inline NiNodePtr GetScene(){ return m_spScene; };

    bool GetAnimationTimeRange(NiAVObject* pkObject, float& fStartTime, 
        float& fEndTime);

    enum Consistency
    {
        MUTABLE             = 0x0000,
        STATIC              = 0x4000,
        VOLATILE            = 0x8000,
        CONSISTENCY_MASK    = 0xF000
    };

    enum Keep
    {
        KEEP_XYZ          = 0x01,
        KEEP_NORM         = 0x02,
        KEEP_COLOR        = 0x04,
        KEEP_UV           = 0x08,
        KEEP_INDICES      = 0x10,
        KEEP_BONEDATA     = 0x20,
        KEEP_ALL          = (KEEP_XYZ | KEEP_NORM | KEEP_COLOR | KEEP_UV |
                             KEEP_INDICES | KEEP_BONEDATA)
    };

protected:
    bool MakeNodes( void );

    void ConvertShadows();
    
       
    bool MakeLights( void );
    bool AttachLight( int iComponent );
    bool AttachParticleSystemEmitter(int iComponent, 
        MyiMaterialManager* pkMaterialManager, 
        MyiTextureManager* pkTextureManager);

    bool AttachParticleSystemParticles(int iComponent, 
        MyiMaterialManager* pkMaterialManager, 
        MyiTextureManager* pkTextureManager);

    bool AttachDefaultSceneProperties( void );
    float CalcTime(void) const;

    bool MakeMesh(int iComponentNum, MyiMaterialManager* pkMtlManager,
        MyiTextureManager* pkTexManager);

    bool MakeComponents( void );

    NiTextKeyExtraDataPtr MergeTextKeyExtraData(NiNode* pkNode, 
        const char* pcName1, const char* pcName2);

    void RecursivelyCreateControllerExtractorSequences(
        NiControllerExtractor::InfoArray& kInfoArray,   
        NiControllerExtractor::NiCCAInfoArray& kOverrideArray,
        NiNode* pkNode, NiNode* pkAccumulationNode, NiNode* pkCharacterNode);

    void CreateControllerExtractorSequences(MFnDagNode& dgNode,
        NiNode* pkNode, NiControllerExtractor::InfoArray& kInfoArray,
        NiNode* pkAccumulationNode, NiNode* pkCharacterRoot);

    NiTextKeyExtraDataPtr CreateSequenceTextKeys(
        NiControllerExtractor::SequenceInfo& kSequenceInfo,
        NiTextKeyExtraDataPtr spTextKeys);

    void BuildNameList(NiTList<NiAVObject*> &kNameList, 
        NiAVObject* pkObject);

    void ParseName(const char* pcName, char* pcBase, 
        unsigned int uiBaseLen, int& iIndex);
    bool CheckUniqueName(const char* pcAgainst, char* szCurrentBase, 
        int iCurrentIndex, int& iMaxIndex);
    void GenerateUniqueNames(NiAVObject* pkObject);

    bool HasAnimations(NiAVObject* pkObject);

    void CheckForLightsAtSceneRoot();
    bool HasMeshChild(NiAVObject* pkObject);

    void CenterAllLODOnBounds(NiNodePtr spNode);

    void GetAllAnimations(NiTList<NiTimeController*>& kTList, 
        NiAVObject* pkObject);

    bool DuplicateAnimationData(NiTimeController* pkCtrl, 
        NiTimeController* pkCtrl2);

    bool DuplicateAnimationData(NiTransformController* pkCtrl, 
        NiTransformController*  pkCtrl2);

    void ShareAllDuplicateAnimationData();

    void SetMeshStability(NiAVObject* pkObject,
        Consistency kConsistency);

    void GetKeepFlags(NiNode* pkNode, unsigned int& uiKeepFlags);

    void SetMeshAttributes(NiAVObject* pkObject, unsigned int uiKeepFlags);

    void SetAnimationLoopingControls(NiAVObject* pkObject, 
        NiTimeController::CycleType kCycleType);

    void CreateBoneLODTags(NiNode* pkObject);

    void RemoveExportOptionsNode();

    void CreateSceneNameAndVersionTag();

    void CreateFullPath(char* pcFullPath, unsigned int uiBufferLen);
    bool ExecuteScript();
    NiScriptInfoPtr CreateScript();

    void BuildSceneGraphSharedData();
    void BuildControllerExtractorSharedData();
    void RecursivelyBuildControllerExtractorSharedData(
        NiControllerExtractor::InfoArray& kInfoArray,  
        NiControllerExtractor::NiCCAGlobalInfoArray& kGlobalArray,
        NiControllerExtractor::NiCCAInfoArray& kOverrideArray,
        NiNode* pkNode);

    NiNode* RecursivelyFindFirstNodeWithAttribute(NiNode* pkNode, 
        char* pcAttributeName);

    bool FindDagNode(NiNode* pkNode, MFnDagNode& dgNode);

    void SetOptimizeKeepFlag(NiNode* pkNode);

    void AddControllerExtractorCCAGlobalInfo(
        NiControllerExtractor::NiCCAGlobalInfoArray& kGlobalArray,
        MFnDagNode& dgNode, NiNode* pkCharacterNode);

    void AddControllerExtractorCCAInfo(
        NiControllerExtractor::NiCCAInfoArray& kInfoArray,
        MFnDagNode& dgNode,  NiNode* pkCurrent, NiNode* pkCharacterNode);

    bool GetCompactKey(const char* pcAttributeName, MFnDagNode& dgNode);
    void GetCharacterCompressedSequenceNameFromAttribute(
        const char* pcAttributeName, char* pcSequenceName);

    void BuildExportOptionsSharedData(NiScriptInfo* pkCurrentScript);
    void BuildRendererSharedData();
    void BuildTimerSharedData();

    void DeleteAllSharedData();

    bool HandleScriptResults(NiBatchExecutionResult* pkResults,
        NiScriptInfo* pkScript);

    bool PostScriptProcess();

    NiScriptInfo* GetViewerScript();

    void RemoveExportPluginsForViewOnly(NiScriptInfo* pkScriptInfo);

    NiScriptInfo* MyImmerse::SeperateFinalExportScripts(
        NiScriptInfo* pkScriptInfo);

    void RecursiveFindShadowFlags(NiNode* pkNode, NiShadowGenerator* 
        pkShadowGenerator);

    void CleanExtraData(NiObjectNET* pkObject);

    void CreateDefaultViewerScripts();

    static int MyImmerse::CompareNiTextKey(const void* pkFirst, 
        const void* pkSecond);



    bool m_bAnimate;
    float m_fElapsedTime;
    int m_iCurrentFrame;
    double m_dFreq;
    unsigned short m_uiLights;
    NiNodePtr   m_spScene;
    NiLightPtr *m_sppLights;
    MObjectArray m_kMayaLightObjects;
    NiWireframePropertyPtr m_spWire;
    NiTList<NiAVObject*> m_pkSelectedObjects;
    NiTList<MyiParticleSystem*> m_pkParticleSystems;
    MyiTextureEffectManager *m_pkTextureEffectManager;
    bool m_bExportingCharacter;
    static NiXMLLogger* ms_pkLogger;
    
};


#endif
