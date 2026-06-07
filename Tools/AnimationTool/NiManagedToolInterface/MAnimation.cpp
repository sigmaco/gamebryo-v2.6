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

#include "stdafx.h"
#include "MAnimation.h"
#include "MFramework.h"
#include "MSharedData.h"
#include "MSceneGraph.h"
using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MAnimation::MAnimation(MTimeManager* pkClock) : m_pkClock(pkClock),
    m_fTimeIncrement(0.0333f), m_mapIDToSequence(NULL),
    m_mapIDToSequenceGroup(NULL), m_eMode(PlaybackMode::Uninitialized),
    m_pkModeData(NULL), m_fLastTime(-NI_INFINITY), m_fStartTime(-NI_INFINITY),
    m_fTransitionStartTime(INVALID_TIME), m_strKFMFilename(NULL),
    m_fDefaultSeqGroupWeight(1.0f), m_uiDefaultSeqGroupPriority(0),
    m_fDefaultSeqGroupEaseIn(0.25f), m_fDefaultSeqGroupEaseOut(0.25f),
    m_pkCallbackObject(NULL)
{
    m_aSequences = NiExternalNew ArrayList(0);
    m_aSequenceGroups = NiExternalNew ArrayList(0);
    m_mapIDToSequence = NiExternalNew Hashtable();
    m_mapIDToSequenceGroup = NiExternalNew Hashtable();
    m_pkCallbackObject = NiNew CallbackObject;
    m_aUnresolvedSequenceInfos = NiExternalNew ArrayList();

    __hook(&MTimeManager::OnRunUpTime, m_pkClock, 
        &NiManagedToolInterface::MAnimation::OnRunUpTime);
    SetNeedToSave(false);
    m_bCanSave = false;
}
//---------------------------------------------------------------------------
void MAnimation::Shutdown()
{
    SetNeedToSave(false);
    m_bCanSave = false;
    NiDelete m_pkCallbackObject;
    m_pkCallbackObject = NULL;
    ClearSequenceArray();
    ClearSequenceGroupArray();
}
//---------------------------------------------------------------------------
NiActorManager* MAnimation::CreateActorManager(NiAVObject* pkNIFRoot)
{
    bool bLoadFilesFromDisk = (pkNIFRoot == NULL);

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
#if defined(EE_PHYSX_BUILD)
    NiActorManager* pkActorManager = CreatePhysicsActorManager(
        pkData->GetKFMTool(),
        pkData->GetKFMTool()->GetBaseKFMPath(), true, bLoadFilesFromDisk);
#else
    NiActorManager* pkActorManager = NiActorManager::Create(
        pkData->GetKFMTool(),
        pkData->GetKFMTool()->GetBaseKFMPath(), true, bLoadFilesFromDisk);
#endif
    if (!pkActorManager)
    {
        SetLastErrorCode(MANIMATION_ERR_CREATE_ACTOR_MANAGER_FAILED);
        pkData->Unlock();
        return NULL;
    }
    if (!bLoadFilesFromDisk)
    {
        NIASSERT(pkNIFRoot);
        NIVERIFY(pkActorManager->ChangeNIFRoot(pkNIFRoot));
    }
    pkData->SetActorManager(pkActorManager);
    pkData->SetScene(MSharedData::CHARACTER_INDEX,
        pkActorManager->GetNIFRoot());
    SetNeedToSave(true);
    pkData->Unlock();

    OnActorManagerCreated();
    return pkActorManager;
}
//---------------------------------------------------------------------------
#if defined(EE_PHYSX_BUILD)
    void MAnimation::Update(float fTime, const bool bResetPhysics)
#else
    void MAnimation::Update(float fTime, const bool)
#endif
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    // Release refs for all completed sequences.
    ReleaseAllCompletedSequences();

    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (pkActorManager)
    {
        switch (m_eMode)
        {
            case PlaybackMode::Transition:
            {
                MTransition* pkTransition = static_cast<MTransition*>(
                    m_pkModeData);
                if (pkActorManager->GetTargetAnimation() ==
                    pkTransition->SrcID && fTime > m_fTransitionStartTime)
                {
                    bool bSuccess = SetTargetAnimation(pkTransition->DesID);
                    MAssert(bSuccess);
                    pkActorManager->SetCallbackObject(m_pkCallbackObject);
                    pkActorManager->RegisterCallback(
                        NiActorManager::END_OF_SEQUENCE, 
                        pkTransition->DesID);
                }
                else
                {
                    float fEventTime;
                    if (m_pkCallbackObject->CallbackReceived(fEventTime))
                    {
                        bool bSuccess = SetPlaybackMode(
                            PlaybackMode::Transition, pkTransition);
                        MAssert(bSuccess);
                        pkData->Unlock();
                        return;
                    }
                }

                break;
            }
        }

        pkActorManager->Update(fTime);
    }

#if defined(EE_PHYSX_BUILD)
    if (!bResetPhysics)
        MFramework::get_Instance()->get_Physics()->UpdateDestinations(fTime);
#endif

    NiAVObject* pkScene = pkData->GetScene(MSharedData::CHARACTER_INDEX);
    if (pkScene)
    {
        pkScene->Update(fTime);
    }

#if defined(EE_PHYSX_BUILD)
    if (bResetPhysics)
    {
        // Figure out which state to reset to
        MSequence* pkSequence = 0;
        switch (m_eMode)
        {  
            case PlaybackMode::Sequence:
            {
                pkSequence = static_cast<MSequence*>(m_pkModeData);
                // Every time PhysX is reset we also must reset accumulation,
                // because that is the only way we can reasonably ensure 
                // that PhysX state matches Gamebryo state. It is the 
                // caller's responsibility to ensure this is true 
                // (to avoid wasteful multiple calls).
                pkSequence->StoreAccum();
                break;
            }
            
            case PlaybackMode::SequenceGroup:
            {
                MSequenceGroup* pkGroup =
                    static_cast<MSequenceGroup*>(m_pkModeData);
                MSequenceGroup::MSequenceInfo* pmInfo = pkGroup->GetAt(0);
                if (pmInfo != NULL)
                    pkSequence = pmInfo->Sequence;
                break;
            }

            case PlaybackMode::Transition:
            {
                MTransition* pkTransition =
                    static_cast<MTransition*>(m_pkModeData);
                pkSequence = GetSequence(pkTransition->SrcID);
                break;
            }
        }
        
        if (pkSequence &&
            pkSequence->GetControllerSequence() &&
            pkSequence->GetControllerSequence()->GetName().Exists())
        {
            MFramework::get_Instance()->get_Physics()->ResetPhysics(
                pkSequence->GetControllerSequence()->GetName(), fTime);
        }
        else
        {
            NiFixedString kDefault("Default");
            MFramework::get_Instance()->get_Physics()->ResetPhysics(
                kDefault, fTime);
        }   
        switch (m_eMode)
        {
            case PlaybackMode::Sequence:
            {
                // Now restore the accumulation. Without
                // the store and restore of the accumulation
                // the physics can have a state that does not
                // match gamebryo state and sometimes NaNs 
                // in translation can result. See
                // MPhysics::ResetPhysics commands in 
                // MPhysics.cpp line 226 for more.
                pkSequence->RestoreAccum();
                break;
            }
        }
    }
    MFramework::get_Instance()->get_Physics()->UpdateSources(fTime);
    MFramework::get_Instance()->get_Physics()->SimulateStep(fTime);
#endif

    switch (m_eMode)
    {
        case PlaybackMode::Sequence:
        {
            MSequence* pkMSequence = static_cast<MSequence*>(m_pkModeData);
            float fScaledTime = pkMSequence->LastScaledTime;
            if (m_fStartTime == -NI_INFINITY)
            {
                pkMSequence->StoreAccum();
                m_fStartTime = fTime;
            }
            if (fTime == m_fStartTime + pkMSequence->DurationDivFreq)
            {
                fScaledTime = pkMSequence->Duration;
            }
            if (m_fLastTime == -NI_INFINITY)
            {
                m_fLastTime = fTime - m_fStartTime; 
            }

            if (fScaledTime < m_fLastTime)
            {
                // We have looped in the engine.

                if (pkMSequence->Loop)
                {
                    if (m_fLastTime >= pkMSequence->DurationDivFreq)
                    {   
                        // Because of existence of phase, we may not really
                        // have gone a full cycle... we want to detect when
                        // our last time exceeds or equals the length and 
                        // then reset.    
                        if (!MFramework::get_Instance()->get_Physics()->
                            IsSequencePhysical(pkMSequence))
                        {
                            pkMSequence->StoreAccum();
                        }
                        m_fStartTime += pkMSequence->DurationDivFreq;
                    }
                }
            }
            m_fLastTime = fTime - m_fStartTime; 

            break;
        }
        case PlaybackMode::Transition:
        {
            MAssert(pkActorManager != NULL);
            MSequence* pkMSequence = GetSequence(pkActorManager
                ->GetTargetAnimation());
            MAssert(pkMSequence != NULL);

            if (m_fLastTime == -NI_INFINITY)
            {
                m_fLastTime = fTime;
            }
            if (m_fStartTime == -NI_INFINITY)
            {
                m_fStartTime = fTime;
            }
            if (fTime < m_fLastTime)
            {
                // We have looped in Engine

                if (m_fLastTime >= pkMSequence->DurationDivFreq)
                {
                    // Because of existence of phase, we may not really 
                    // have gone a full cycle... we want to detect when our
                    // last time exceeds or equals the length and then reset.

                    pkActorManager->GetControllerManager()
                        ->ClearCumulativeAnimations();
                    m_fStartTime = fTime;
                }
            }
            m_fLastTime = fTime;

            break;
        }
    }

    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MAnimation::NewKFM()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    m_eMode = PlaybackMode::None;

#if defined(EE_PHYSX_BUILD)
    MFramework::Instance->Physics->DeleteContents();
#endif

    ClearSequenceArray();
    ClearSequenceGroupArray();
    pkData->SetScene(MSharedData::CHARACTER_INDEX, NULL);
    pkData->SetActorManager(NULL);
    pkData->SetKFMTool(NULL);
    NiKFMTool* pkKFMTool = NiNew NiKFMTool;
    pkData->SetKFMTool(pkKFMTool);
    HandleUnresolvedSequences();
    BuildSequenceArray();
    BuildSequenceGroupArray();

    SetInitialTransitionTimes();

    m_strKFMFilename = NULL;
    m_bCanSave = false;
    SetNeedToSave(true);
    pkData->Unlock();

    OnNewKFM();
}
//---------------------------------------------------------------------------
bool MAnimation::LoadKFM(String* strFilename)
{
    if (strFilename == String::Empty)
    {
        SetLastErrorCode(MANIMATION_ERR_FILENAME_EMPTY);
        return false;
    }

    const char* pcFilename = MStringToCharPointer(strFilename);
    NiKFMToolPtr spKFMTool = NiNew NiKFMTool;
    NiKFMTool::KFM_RC eRC = spKFMTool->LoadFile(pcFilename);
    
    if (eRC != NiKFMTool::KFM_SUCCESS)
    {
        SetLastErrorCode(MANIMATION_ERR_KFMTOOL_ERROR);
        SetLastErrorString( NiKFMTool::LookupReturnCode(eRC));
        MFreeCharPointer(pcFilename);
        return false;
    }

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

#if defined(EE_PHYSX_BUILD)
    NiActorManager* pkActorManager =
        CreatePhysicsActorManager(spKFMTool, pcFilename, true, true);
#else
    NiActorManager* pkActorManager = 
        NiActorManager::Create(spKFMTool, pcFilename, true, true);
#endif

    bool bChangedNIF = false;
    if (!pkActorManager)
    {
        // If the actor manager was not created, the NIF file could not be
        // loaded, either because it is missing or corrupt.
        //
        // Give the user a chance find the right NIF file
        ModelNIFFailedLoadArgs* pkArgs = NiExternalNew ModelNIFFailedLoadArgs;
        pkArgs->strNIFFilename = spKFMTool->GetFullModelPath();

        pkData->Unlock();
        OnModelNIFLoadFailed(pcFilename, pkArgs);
        pkData->Lock();

        if (!pkArgs->bCancel)
        {
            const char* pcModelPath = 
                MStringToCharPointer(pkArgs->strNIFFilename);
            spKFMTool->SetModelPath(pcModelPath);
            MFreeCharPointer(pcModelPath);

            const char* pcModelRoot = 
                MStringToCharPointer(pkArgs->strModelRootName);
            spKFMTool->SetModelRoot(pcModelRoot);
            MFreeCharPointer(pcModelRoot);

#if defined(EE_PHYSX_BUILD)
            pkActorManager =
                CreatePhysicsActorManager(spKFMTool, pcFilename, true, true);
#else
            pkActorManager = 
                NiActorManager::Create(spKFMTool, pcFilename, true, true);
#endif
            bChangedNIF = true;
        }
    }

    MFreeCharPointer(pcFilename);
    
    if (!pkActorManager)
    {
        // If the actor manager was not created, the NIF file could not be
        // loaded, either because it is missing or corrupt.
        SetLastErrorCode(MANIMATION_ERR_FAILED_TO_LOAD_CHARACTER_NIF);
        SetLastErrorString(
            String::Concat("Unable to load character NIF file at ",
            spKFMTool->GetFullModelPath(), "."));
        pkData->Unlock();
        return false;
    }

    NiAVObject* pkNIFRoot = pkActorManager->GetNIFRoot();
    if (!pkNIFRoot)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_NIF_ROOT_EXIST);
        pkData->Unlock();
        return false;
    }

    m_bCanSave = true;
    SetNeedToSave(bChangedNIF);

    pkData->SetKFMTool(spKFMTool);
    pkData->SetActorManager(pkActorManager);
    pkData->SetScene(MSharedData::CHARACTER_INDEX, pkNIFRoot);

    HandleUnresolvedSequences();
    
    BuildSequenceArray();
    BuildSequenceGroupArray();

    SetInitialTransitionTimes();
   
    pkData->Unlock();
    m_strKFMFilename = strFilename;

    MLogger::LogGeneral(String::Concat("Loading: ", m_strKFMFilename));
    OnKFMLoaded();
    SetPlaybackMode(None, NULL);
    return true;
}
//---------------------------------------------------------------------------
bool MAnimation::SaveKFM(String* strFilename)
{
    if (strFilename == String::Empty)
    {
        SetLastErrorCode(MANIMATION_ERR_FILENAME_EMPTY);
        return false;
    }

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastError(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return false;
    }
    const char* pcFilename = MStringToCharPointer(strFilename);
    NiKFMTool::KFM_RC eRC = pkKFMTool->SaveFile(pcFilename);
    MFreeCharPointer(pcFilename);
    pkData->Unlock();

    if (eRC != NiKFMTool::KFM_SUCCESS)
    {
        SetLastErrorCode(MANIMATION_ERR_KFMTOOL_ERROR);
        SetLastErrorString(pkKFMTool->LookupReturnCode(eRC));
        return false;
    }

    SetNeedToSave(false);

    m_strKFMFilename = strFilename;

    MLogger::LogGeneral(String::Concat("Saving: ", m_strKFMFilename));
    OnKFMSaved();

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Physics functions to manage extraction of PhysX content from loaded NIF
// files.
//---------------------------------------------------------------------------
#if defined(EE_PHYSX_BUILD)
NiActorManager* MAnimation::CreatePhysicsActorManager(NiKFMTool* pkKFMTool,
    const char* pcKFMFilePath, bool bCumulativeAnimations,
    bool bLoadFilesFromDisk)
{
    NiActorManager* pkAM = NiActorManager::Create(pkKFMTool, pcKFMFilePath,
        bCumulativeAnimations, false);

    if (!pkAM || !bLoadFilesFromDisk)
        return pkAM;
    
    // Load the NIF file. We need to do it now, in a separate pass, because we
    // need to pull the PhysX and Camera info out of the stream.
    NiStream* pkStream = NiNew NiStream();
    if (!pkStream->Load(pkKFMTool->GetFullModelPath()))
    {
        NiDelete pkStream;
        NiDelete pkAM;
        return 0;
    }

    // Look for the PhysX content.
    if (MFramework::get_Instance()->get_Physics()->get_Present())
    {
        if (!MFramework::get_Instance()->get_Physics()->
            ProcessStream(pkStream))
        {
            NiDelete pkStream;
            NiDelete pkAM;
            return 0;
        }
    }

    // Set up the actor manager with the loaded file.
    if (!pkAM->ReloadNIFFile(pkStream, false))
    {
        NiDelete pkStream;
        NiDelete pkAM;
        return 0;
    }

    // The return value from this function is not checked here because we will
    // resolve unloaded sequences later.
    pkAM->LoadAllSequenceData(pkStream);
    
    if (MFramework::get_Instance()->get_Physics()->get_Present())
    {
        MFramework::get_Instance()->get_Physics()->
            AttachPhysics(pkAM->GetControllerManager());
    }
    
    NiDelete pkStream;
    
    return pkAM;
}
//---------------------------------------------------------------------------
bool MAnimation::ReloadPhysicsNIF(NiActorManager* pkAM,
    const NiFixedString& kPath)
{
    // Load the NIF file. We need to do it now, in a separate pass, because we
    // need to pull the PhysX and Camera info out of the stream.
    NiStream* pkStream = NiNew NiStream();
    if (!pkStream->Load(kPath))
    {
        SetLastErrorCode(MANIMATION_ERR_FAILED_TO_LOAD_CHARACTER_NIF);
        SetLastErrorString(
            String::Concat("Unable to load character NIF file at ",
            kPath, "."));
        NiDelete pkStream;
        return false;
    }

    // Look for the PhysX content.
    MFramework::get_Instance()->get_Physics()->ProcessStream(pkStream);

    // Set up the actor manager with the loaded file.
    if (!pkAM->ReloadNIFFile(pkStream, false))
    {
        SetLastErrorCode(MANIMATION_ERR_FAILED_TO_LOAD_CHARACTER_NIF);
        SetLastErrorString(
            String::Concat("Unable to load character NIF file at ",
            kPath, "."));
        NiDelete pkStream;
        return false;
    }
    
    NiDelete pkStream;

    MFramework::get_Instance()->get_Physics()->
        AttachPhysics(pkAM->GetControllerManager());
    
    return true;
}
#endif
//---------------------------------------------------------------------------
void MAnimation::ReloadNIF()
{
    SetPlaybackMode(PlaybackMode::None, NULL);

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkAM = pkData->GetActorManager();
    pkData->GetScene(MSharedData::CHARACTER_INDEX);
    if (pkAM)
    {
        bool bResult;
#if defined(EE_PHYSX_BUILD)
        NiKFMTool* pkKFMTool = pkData->GetKFMTool();
        bResult = ReloadPhysicsNIF(pkAM, pkKFMTool->GetFullModelPath());
#else
        bResult = pkAM->ReloadNIFFile();
#endif
        if (bResult)
        {
            pkData->SetScene(MSharedData::CHARACTER_INDEX,
                pkAM->GetNIFRoot());
            OnActorManagerCreated();
        }
    }

    pkData->Unlock();
}

//---------------------------------------------------------------------------
bool MAnimation::SetModelPathAndRoot(String* strModelPath,
    String* strModelRoot)
{
    if (strModelPath == NULL || strModelPath == String::Empty)
    {
        SetLastErrorCode(MANIMATION_ERR_MODEL_PATH_EMPTY);
        return false;
    }

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return false;
    }

    // Check whether or not the model path and model root are the same as
    // those already set.
    bool bModelPathSame = String::Equals(strModelPath,
        pkKFMTool->GetModelPath());
    bool bModelRootSame = String::Equals(strModelRoot,
        pkKFMTool->GetModelRoot());

    // If the model path and root are the same as those already set, don't do
    // anything.
    if (bModelPathSame && bModelRootSame)
    {
        pkData->Unlock();
        return true;
    }

    // If model root passed into this function is empty, grab the name of
    // the first object in the NIF file.
    NiAVObjectPtr spNIFRoot;
    bool bReplacedPhysics = false;
    if (strModelRoot == NULL || strModelRoot == String::Empty)
    {
        const char* pcNIFPath = MStringToCharPointer(strModelPath);
        NiStream kStream;
        int iSuccess = NiVirtualBoolBugWrapper::NiStream_Load(kStream,
            pcNIFPath);
        MFreeCharPointer(pcNIFPath);
        if (iSuccess == 0)
        {
            SetLastErrorCode(MANIMATION_ERR_FAILED_LOADING_NIF);
            pkData->Unlock();
            return false;
        }
        spNIFRoot = NiDynamicCast(NiAVObject, kStream.GetObjectAt(0));
        if (!spNIFRoot)
        {
            SetLastErrorCode(MANIMATION_ERR_NIF_OBJECT_NOT_NIAVOBJECT);
            pkData->Unlock();
            return false;
        }
        const char* pcName = spNIFRoot->GetName();
        if (!pcName)
        {
            SetLastErrorCode(MANIMATION_ERR_NIF_NIF_OBJECT_NO_NAME_FOR_FILE);
            pkData->Unlock();
            return false;
        }
        
#if defined(EE_PHYSX_BUILD)
        if (MFramework::get_Instance()->get_Physics()->get_Present())
        {
            MFramework::get_Instance()->get_Physics()->ProcessStream(&kStream);
            bReplacedPhysics = true;
        }
#endif
        
        strModelRoot = pcName;
    }

    String* strOldModelPath = pkKFMTool->GetModelPath();
    const char* pcModelPath = MStringToCharPointer(strModelPath);
    pkKFMTool->SetModelPath(pcModelPath);
    MFreeCharPointer(pcModelPath);

    String* strOldModelRoot = pkKFMTool->GetModelRoot();
    const char* pcModelRoot = MStringToCharPointer(strModelRoot);
    pkKFMTool->SetModelRoot(pcModelRoot);
    MFreeCharPointer(pcModelRoot);

    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        pkActorManager = CreateActorManager(spNIFRoot);
        if (!pkActorManager)
        {
            SetLastErrorCode(MANIMATION_ERR_CREATE_ACTOR_MANAGER_FAILED);

            pcModelPath = MStringToCharPointer(strOldModelPath);
            pkKFMTool->SetModelPath(pcModelPath);
            MFreeCharPointer(pcModelPath);

            pcModelRoot = MStringToCharPointer(strOldModelRoot);
            pkKFMTool->SetModelRoot(pcModelRoot);
            MFreeCharPointer(pcModelRoot);

            pkData->Unlock();
            return false;
        }
#if defined(EE_PHYSX_BUILD)
        // If we had a nif root but no actor manager, as was the case here,
        // then we are loading a NIF file from scratch, with no existing
        // actor manager. So we did not attach the physics ready for sequences
        // to be added, and there are no physical sequences.
        if (bReplacedPhysics)
        {
            MFramework::get_Instance()->get_Physics()->
                AttachPhysics(pkActorManager->GetControllerManager());
        }
#endif
    }
    else
    {
        if (bModelPathSame && !bReplacedPhysics)
        {
            spNIFRoot = pkActorManager->GetNIFRoot();
        }

        bool bSuccess = false;
        if (spNIFRoot)
        {
            bSuccess = pkActorManager->ChangeNIFRoot(spNIFRoot);
#if defined(EE_PHYSX_BUILD)
            // If we reloaded the NIF file and replaced all the physics data,
            // we need to set it up again now.
            if (bSuccess && bReplacedPhysics)
            {
                MFramework::get_Instance()->get_Physics()->
                    AttachPhysics(pkActorManager->GetControllerManager());
            }
#endif
        }
        else
        {
#if defined(EE_PHYSX_BUILD)
            bSuccess = ReloadPhysicsNIF(pkActorManager,
                pkKFMTool->GetFullModelPath());
#else
            bSuccess = pkActorManager->ReloadNIFFile();
#endif
        }

        if (bSuccess)
        {
            pkData->SetScene(MSharedData::CHARACTER_INDEX,
                pkActorManager->GetNIFRoot());
        }
        else
        {
            SetLastErrorCode(MANIMATION_ERR_FAILED_TO_RELOAD_NIF);

            pcModelPath = MStringToCharPointer(strOldModelPath);
            pkKFMTool->SetModelPath(pcModelPath);
            MFreeCharPointer(pcModelPath);

            pcModelRoot = MStringToCharPointer(strOldModelRoot);
            pkKFMTool->SetModelRoot(pcModelRoot);
            MFreeCharPointer(pcModelRoot);

            pkData->Unlock();
            return false;
        }
    }

    m_bCanSave = true;
    SetNeedToSave(true);
    pkData->Unlock();

    MLogger::LogGeneral(String::Concat("Model Path: ", strModelPath));
    MLogger::LogGeneral(String::Concat("Model Root: ", strModelRoot));

    OnModelPathAndRootChanged(strModelPath, strModelRoot);
    return true;
}
//---------------------------------------------------------------------------
ArrayList* MAnimation::GetModelRootConflicts()
{
    ArrayList* pkConflictingNodeNames = NiExternalNew ArrayList(0);

    String* strModelPath = this->ModelPath;
    String* strModelRoot = this->ModelRoot;
    if (strModelPath == String::Empty || strModelPath == NULL)
    {
        SetLastErrorCode(MANIMATION_ERR_MODEL_PATH_EMPTY);
        return pkConflictingNodeNames;
    }

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        pkData->Unlock();
        return pkConflictingNodeNames;
    }

    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        pkData->Unlock();
        return pkConflictingNodeNames;
    }

    const char* pcModelRoot = MStringToCharPointer(strModelRoot);
    NiAVObject* pkRoot = pkActorManager->GetNIFRoot();
    NiAVObject* pkNewRoot = pkRoot->GetObjectByName(pcModelRoot);

    if (pkNewRoot == NULL)
    {
        SetLastErrorCode(MANIMATION_ERR_MODEL_ROOT_NIF_MISMATCH);
        pkData->Unlock();
        return pkConflictingNodeNames;
    }

    if (Sequences == NULL || Sequences->Count == 0)
    {
        pkData->Unlock();
        return pkConflictingNodeNames;
    }

    for (int i = 0; i < Sequences->Count; i++)
    {
        NiSequenceData* pkSeqData = (static_cast<MSequence*>(
            Sequences->Item[i]))->GetSequenceData();
        
        unsigned int uiNumEvaluators = pkSeqData->GetNumEvaluators();
        for (unsigned int j = 0; j < uiNumEvaluators; j++)
        {
            NiEvaluator* pkEvaluator = pkSeqData->GetEvaluatorAt(j);
            if (!pkEvaluator)
                continue;

            const NiFixedString& kAVObjectName = 
                pkEvaluator->GetIDTag().GetAVObjectName();
            if (!kAVObjectName.Exists())
                continue;

            NiAVObject* pkObj = pkNewRoot->GetObjectByName(kAVObjectName);
            if (pkObj == NULL)
            {
                String* strName = NiExternalNew String(kAVObjectName);
                if (!pkConflictingNodeNames->Contains(strName))
                    pkConflictingNodeNames->Add(strName);  
            }
        }
    }

    MFreeCharPointer(pcModelRoot);
    pkData->Unlock();

    return pkConflictingNodeNames;
} 
//---------------------------------------------------------------------------
ArrayList* MAnimation::GetModelRootConflictsForSequence(
    MSequence* pkMSequence)
{
    ArrayList* pkConflictingNodeNames = NiExternalNew ArrayList(0);

    String* strModelPath = this->ModelPath;
    String* strModelRoot = this->ModelRoot;
    if (strModelPath == String::Empty || strModelPath == NULL)
    {
        SetLastErrorCode(MANIMATION_ERR_MODEL_PATH_EMPTY);
        return pkConflictingNodeNames;
    }

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        pkData->Unlock();
        return pkConflictingNodeNames;
    }

    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        pkData->Unlock();
        return pkConflictingNodeNames;
    }

    const char* pcModelRoot = MStringToCharPointer(strModelRoot);
    NiAVObject* pkRoot = pkActorManager->GetNIFRoot();
    NiAVObject* pkNewRoot = pkRoot->GetObjectByName(pcModelRoot);

    if (pkNewRoot == NULL)
    {
        SetLastErrorCode(MANIMATION_ERR_MODEL_ROOT_NIF_MISMATCH);
        pkData->Unlock();
        return pkConflictingNodeNames;
    }

    NiSequenceData* pkSeqData = pkMSequence->GetSequenceData();
    
    unsigned int uiNumEvaluators = pkSeqData->GetNumEvaluators();
    for (unsigned int j = 0; j < uiNumEvaluators; j++)
    {
        NiEvaluator* pkEvaluator = pkSeqData->GetEvaluatorAt(j);
        if (!pkEvaluator)
            continue;

        const NiFixedString& kAVObjectName = 
            pkEvaluator->GetIDTag().GetAVObjectName();
        if (!kAVObjectName.Exists())
            continue;

        NiAVObject* pkObj = pkNewRoot->GetObjectByName(kAVObjectName);
        if (pkObj == NULL)
        {
            String* strName = NiExternalNew String(kAVObjectName);
            if (!pkConflictingNodeNames->Contains(strName))
                pkConflictingNodeNames->Add(strName);  
        }
    }

    MFreeCharPointer(pcModelRoot);
    pkData->Unlock();

    return pkConflictingNodeNames;
}
//---------------------------------------------------------------------------
ArrayList* MAnimation::AddSequencesFromKF(String* strFilename,
    String*& strErrors)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        strErrors = String::Concat(strErrors, GetLastErrorString(), "\\par\n");
        pkData->Unlock();
        return NULL;
    }

    NiStream kStream;
    const char* pcFilename = MStringToCharPointer(strFilename);
    NiFixedString kFilename(pcFilename);
    MFreeCharPointer(pcFilename);

    int iSuccess = NiVirtualBoolBugWrapper::NiStream_Load(kStream, kFilename);
    if (iSuccess == 0)
    {
        SetLastErrorCode(MANIMATION_ERR_FAILED_LOADING_KF);
        strErrors = String::Concat(strErrors, GetLastErrorString(), "\\par\n");
        pkData->Unlock();
        return NULL;
    }

    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        pkActorManager = CreateActorManager(NULL);
        if (!pkActorManager)
        {
            SetLastErrorCode(MANIMATION_ERR_CREATE_ACTOR_MANAGER_FAILED);
            strErrors = String::Concat(strErrors, GetLastErrorString(),
                "\\par\n");
            pkData->Unlock();
            return NULL;
        }
    }

    const unsigned int uiSeqDataCount = kStream.GetObjectCount();

    ArrayList* pkMSequences = NiExternalNew ArrayList(uiSeqDataCount);

    for (unsigned int uiAnimIndex = 0; uiAnimIndex < uiSeqDataCount;
        uiAnimIndex++)
    {
        NiSequenceData* pkSeqData = NiDynamicCast(NiSequenceData,
            kStream.GetObjectAt(uiAnimIndex));
        if (!pkSeqData)
        {
            SetLastErrorCode(MANIMATION_ERR_OBJECT_IN_KF_NOT_SEQUENCE);
            strErrors = String::Concat(strErrors, GetLastErrorString(),
                "\\par\n");
            continue;
        }

        bool bSequenceAlreadyExists = false;
        unsigned int* puiSequenceIDs;
        unsigned int uiNumIDs;
        pkKFMTool->GetSequenceIDs(puiSequenceIDs, uiNumIDs);
        for (unsigned int ui = 0; ui < uiNumIDs; ui++)
        {
            unsigned int uiTempSeqID = puiSequenceIDs[ui];

            NiKFMTool::Sequence* pkExistingKFMSequence =
                pkKFMTool->GetSequence(uiTempSeqID);
            NIASSERT(pkExistingKFMSequence);
            if (pkKFMTool->GetFullKFFilename(uiTempSeqID) == kFilename &&
                pkExistingKFMSequence->GetAnimIndex() == (int)uiAnimIndex)
            {
                bSequenceAlreadyExists = true;
                break;
            }
        }
        NiFree(puiSequenceIDs);
        if (bSequenceAlreadyExists)
        {
            strErrors = String::Concat(strErrors, String::Concat("The \"",
                pkSeqData->GetName(), "\" sequence already exists."),
                "\\par\n");
            continue;
        }

        unsigned int uiSequenceID = FindUnusedSequenceID();
        NiKFMTool::KFM_RC eRC = pkKFMTool->AddSequence(uiSequenceID, kFilename,
            pkSeqData->GetName());
        if (eRC != NiKFMTool::KFM_SUCCESS)
        {
            SetLastErrorCode(MANIMATION_ERR_KFMTOOL_ERROR);
            SetLastErrorString(pkKFMTool->LookupReturnCode(eRC));
            strErrors = String::Concat(strErrors, GetLastErrorString(),
                "\\par\n");
            continue;
        }

        if (!pkActorManager->ChangeSequenceData(uiSequenceID, pkSeqData))
        {
            eRC = pkKFMTool->RemoveSequence(uiSequenceID);
            SetLastErrorCode(MANIMATION_ERR_KFMTOOL_ERROR);
            SetLastErrorString(pkKFMTool->LookupReturnCode(eRC));
            strErrors = String::Concat(strErrors, GetLastErrorString(),
                "\\par\n");
            NIASSERT(eRC == NiKFMTool::KFM_SUCCESS);
            continue;
        }

#if defined(EE_PHYSX_BUILD)
        // Add this sequence to those known about by physics.
        if (MFramework::get_Instance()->get_Physics()->get_Present())
        {
            MFramework::get_Instance()->get_Physics()->
                AddKeySequence(pkSeqData);
        }
#endif

        pkData->Unlock();

        AddToSequenceArray(NiExternalNew MSequence(this, 
            pkKFMTool->GetSequence(uiSequenceID),
            pkActorManager->GetSequenceData(uiSequenceID)), true);

        MSequence* pkMSequence = GetSequence(uiSequenceID);
        NIASSERT(pkMSequence);
        pkMSequences->Add(pkMSequence);

        MLogger::LogGeneral(String::Concat("Adding Sequence: ",
            pkMSequence->Name));

        OnSequenceAdded(pkMSequence);
        AddDefaultTransitionsForSequence(pkMSequence);

        if (uiAnimIndex < uiSeqDataCount - 1)
        {
            pkData->Lock();
        }

        SetNeedToSave(true);
    }

    return pkMSequences;
}
//---------------------------------------------------------------------------
void MAnimation::AddDefaultTransitionsForSequence(MSequence* pkMSequence)
{
    unsigned int uiSrcID = pkMSequence->SequenceID;
    for (int i = 0; i < this->Sequences->Count; i++)
    {
        MSequence* pkDesMSequence = static_cast<MSequence*>(
            this->Sequences->Item[i]);
        unsigned int uiDesID = pkDesMSequence->SequenceID;
        if (uiDesID != uiSrcID)
        {
            if (pkMSequence->CanSyncTo(pkDesMSequence))
            {
                if (pkMSequence->GetTransition(uiDesID) == NULL)
                {
                    AddTransition(uiSrcID, uiDesID,
                        MTransition::TransitionType::Trans_DefaultSync,
                        this->DefaultSyncTransDuration);
                }
                if (pkDesMSequence->GetTransition(uiSrcID) == NULL)
                {
                    AddTransition(uiDesID, uiSrcID,
                        MTransition::TransitionType::Trans_DefaultSync,
                        this->DefaultSyncTransDuration);
                }
            }
            else
            {
                if (pkMSequence->GetTransition(uiDesID) == NULL)
                {
                    AddTransition(uiSrcID, uiDesID,
                        MTransition::TransitionType::Trans_DefaultNonSync,
                        this->DefaultNonSyncTransDuration);
                }
                if (pkDesMSequence->GetTransition(uiSrcID) == NULL)
                {
                    AddTransition(uiDesID, uiSrcID,
                        MTransition::TransitionType::Trans_DefaultNonSync,
                        this->DefaultNonSyncTransDuration);
                }
            }

            SetInitialTransitionTimeRowCol(pkMSequence);
        }
    }

    MLogger::LogGeneral(String::Concat("Adding default transitions"
        " for sequence ", pkMSequence->Name));

}
//---------------------------------------------------------------------------
void MAnimation::RemoveSequence(unsigned int uiSequenceID)
{
    MSequence* pkMSequence = GetSequence(uiSequenceID);
    if (!pkMSequence)
    {
        SetLastErrorCode(MANIMATION_ERR_SEQUENCE_DOES_NOT_EXIST);
        return;
    }

    RemoveTransitionsContainingSequence(uiSequenceID);

    for (int i = 0; i < m_aSequenceGroups->Count; i++)
    {
        MSequenceGroup* pkTempMSeqGroup = dynamic_cast<MSequenceGroup*>(
            m_aSequenceGroups->get_Item(i));
        pkTempMSeqGroup->RemoveSequence(uiSequenceID);
    }

    SetPlaybackMode(PlaybackMode::None, NULL);

#if defined(EE_PHYSX_BUILD)
    if (MFramework::get_Instance()->get_Physics()->get_Present())
    {
        MFramework::get_Instance()->get_Physics()->
            RemoveSequence(pkMSequence);
    }
#endif

    RemoveFromSequenceArray(pkMSequence);
    pkMSequence->DeleteContents();

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    NIASSERT(pkKFMTool);
    NiKFMTool::KFM_RC eRC = 
        pkKFMTool->RemoveSequence(uiSequenceID);
    NI_UNUSED_ARG(eRC);
    NIASSERT(eRC == NiKFMTool::KFM_SUCCESS);

    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (pkActorManager)
    {
        pkActorManager->UnloadSequenceData(uiSequenceID);
    }

    SetNeedToSave(true);
    pkData->Unlock();

    OnSequenceRemoved(uiSequenceID);
}
//---------------------------------------------------------------------------
void MAnimation::RemoveTransitionsContainingSequence(
    unsigned int uiSequenceID)
{
    if (m_aSequences == NULL)
    {
        return;
    }

    ArrayList* aToRemove = NiExternalNew ArrayList();
    for (int i = 0; i < m_aSequences->Count; i++)
    {
        MSequence* pkMSequence = dynamic_cast<MSequence*>(
            m_aSequences->Item[i]);
        if (pkMSequence->Transitions == NULL)
        {
            continue;
        }
        for (int j = 0; j < pkMSequence->Transitions->Count; j++)
        {
            MTransition* pkTransition = pkMSequence->Transitions[j];
            NIASSERT(pkTransition != NULL);
            if (pkTransition->SrcID == uiSequenceID ||
                pkTransition->DesID == uiSequenceID)
            {
                aToRemove->Add(pkTransition);
            }
            else if (pkTransition->Type ==
                MTransition::TransitionType::Trans_Chain)
            {
                if (pkTransition->ChainInfo == NULL)
                {
                    continue;
                }
                for (int k = 0; k < pkTransition->ChainInfo->Count; k++)
                {
                    MTransition::MChainInfo* pkChainInfo =
                        pkTransition->ChainInfo[k];
                    if (pkChainInfo->SequenceID == uiSequenceID)
                    {
                        aToRemove->Add(pkTransition);
                        break;
                    }
                }
            }
        }
    }

    for (int i = 0; i < aToRemove->Count; i++)
    {
        MTransition* pkTransToRemove = dynamic_cast<MTransition*>(
            aToRemove->Item[i]);
        RemoveTransition(pkTransToRemove->SrcID, pkTransToRemove->DesID);
    }
}
//---------------------------------------------------------------------------
void MAnimation::RemoveChainsContainingTransitions(
    unsigned int uiSrcID, unsigned int uiDesID)
{
    if (m_aSequences == NULL)
    {
        return;
    }

    ArrayList* aToRemove = NiExternalNew ArrayList();
    for (int i = 0; i < m_aSequences->Count; i++)
    {
        MSequence* pkMSequence = dynamic_cast<MSequence*>(
            m_aSequences->Item[i]);
        if (pkMSequence->Transitions == NULL)
        {
            continue;
        }
        for (int j = 0; j < pkMSequence->Transitions->Count; j++)
        {
            MTransition* pkTransition = pkMSequence->Transitions[j];
            NIASSERT(pkTransition != NULL);

            if (pkTransition->Type != 
                MTransition::TransitionType::Trans_Chain)
                continue;

            unsigned int uiChainCnt = pkTransition->ChainInfo->Count;

            if (uiChainCnt == 0)
                continue;

            // Now cycle through the chain to see if we can find 
            // the transition.

            // check first...
            if (pkTransition->SrcID == uiSrcID)
            {
                if (pkTransition->ChainInfo[0]->SequenceID == uiDesID)
                {
                    aToRemove->Add(pkTransition);
                    continue;
                }
            }

            // check last...
            if (pkTransition->ChainInfo[uiChainCnt-1]->SequenceID == uiSrcID)
            {
                if (pkTransition->DesID == uiDesID)
                {
                    aToRemove->Add(pkTransition);
                    continue;
                }
            }

            // check inbetween...
            for (unsigned int k = 1; k < uiChainCnt-1; k++)
            {
                MTransition::MChainInfo* pkChainInfo =
                    pkTransition->ChainInfo[k];

                // If we find the same pair, report this chain
                if (pkChainInfo->SequenceID == uiSrcID)
                {                        
                    if (pkTransition->ChainInfo[k+1]->SequenceID == uiDesID)
                    {
                        aToRemove->Add(pkTransition);
                        break;
                    }
                }
            }
        }
    }

    for (int l = 0; l < aToRemove->Count; l++)
    {
        MTransition* pkTransToRemove = dynamic_cast<MTransition*>(
            aToRemove->Item[l]);
        RemoveTransition(pkTransToRemove->SrcID, pkTransToRemove->DesID);
    }
}
//---------------------------------------------------------------------------
NiKFMTool::Transition* MAnimation::ChangeTransitionType(unsigned int uiSrcID,
    unsigned int uiDesID, NiKFMTool::Transition* pkTransition,
    NiKFMTool::TransitionType eNewType)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return NULL;
    }

    NiKFMTool::KFM_RC eRC = pkKFMTool->UpdateTransition(uiSrcID, uiDesID,
        eNewType, pkTransition->GetDuration());
    if (eRC != NiKFMTool::KFM_SUCCESS)
    {
        SetLastErrorCode(MANIMATION_ERR_KFMTOOL_ERROR);
        SetLastErrorString(pkKFMTool->LookupReturnCode(eRC));
        pkData->Unlock();
        return NULL;
    }

    pkTransition = pkKFMTool->GetTransition(uiSrcID, uiDesID);
    if (!pkTransition)
    {
        SetLastErrorCode(MANIMATION_ERR_TRANSITION_DOES_NOT_EXIST);
        pkData->Unlock();
        return NULL;
    }

    pkData->Unlock();

    return pkTransition;
}
//---------------------------------------------------------------------------
MTransition* MAnimation::AddTransition(unsigned int uiSrcID,
    unsigned int uiDesID, MTransition::TransitionType eType, float fDuration)
{
    MSequence* pkSrcMSequence = GetSequence(uiSrcID);
    MSequence* pkDesMSequence = GetSequence(uiDesID);
    if (!pkSrcMSequence || !pkDesMSequence)
    {
        SetLastErrorCode(MANIMATION_ERR_SRC_OR_DEST_DOES_NOT_EXIST);
        return NULL;
    }

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return NULL;
    }

    NiKFMTool::KFM_RC eRC = pkKFMTool->AddTransition(uiSrcID, uiDesID,
        MTransition::TranslateTransitionType(eType), fDuration);
    if (eRC != NiKFMTool::KFM_SUCCESS)
    {
        SetLastErrorCode(MANIMATION_ERR_KFMTOOL_ERROR);
        SetLastErrorString(pkKFMTool->LookupReturnCode(eRC));
        pkData->Unlock();
        return NULL;
    }

    SetNeedToSave(true);
    pkData->Unlock();

    pkSrcMSequence->RebuildTransitionsArray();
    MTransition* pkTransition = pkSrcMSequence->GetTransition(uiDesID);
    NIASSERT(pkTransition != NULL);

    if (eType == MTransition::Trans_DelayedBlend)
    {
        pkTransition->StoredType = eType;
    }

    OnTransitionAdded(pkTransition);
    return pkTransition;
}
//---------------------------------------------------------------------------
void MAnimation::RemoveTransition(unsigned int uiSrcID, unsigned int uiDesID)
{
    MSequence* pkSrcMSequence = GetSequence(uiSrcID);
    if (!pkSrcMSequence)
    {
        SetLastErrorCode(MANIMATION_ERR_SRC_SEQUENCE_DOES_NOT_EXIST);
        return;
    }

    MTransition* pkTransition = pkSrcMSequence->GetTransition(uiDesID);
    if (!pkTransition)
    {
        SetLastErrorCode(MANIMATION_ERR_TRANSITION_DOES_NOT_EXIST);
        return;
    }
   
    if (pkTransition->Type != MTransition::TransitionType::Trans_Chain)
    {
        RemoveChainsContainingTransitions(
            pkTransition->SrcID, pkTransition->DesID);
    }

    SetNeedToSave(true);
    OnTransitionRemoved(pkTransition);

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    NIASSERT(pkKFMTool);
    NiKFMTool::KFM_RC eRC = 
        pkKFMTool->RemoveTransition(uiSrcID, uiDesID);
    NI_UNUSED_ARG(eRC);
    NIASSERT(eRC == NiKFMTool::KFM_SUCCESS);
    
    pkData->Unlock();

    pkSrcMSequence->RebuildTransitionsArray();
}
//---------------------------------------------------------------------------
MSequenceGroup* MAnimation::AddSequenceGroup(unsigned int uiGroupID,
    String* strName)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return NULL;
    }

    const char* pcName = MStringToCharPointer(strName);
    NiKFMTool::KFM_RC eRC = pkKFMTool->AddSequenceGroup(uiGroupID, pcName);
    MFreeCharPointer(pcName);
    if (eRC != NiKFMTool::KFM_SUCCESS)
    {
        SetLastErrorCode(MANIMATION_ERR_KFMTOOL_ERROR);
        SetLastErrorString(pkKFMTool->LookupReturnCode(eRC));
        pkData->Unlock();
        return NULL;
    }

    SetNeedToSave(true);
    pkData->Unlock();

    AddToSequenceGroupArray(NiExternalNew MSequenceGroup(this,
        pkKFMTool->GetSequenceGroup(uiGroupID)), true);
    MSequenceGroup* pkMSeqGroup = GetSequenceGroup(uiGroupID);
    NIASSERT(pkMSeqGroup);

    OnSequenceGroupAdded(pkMSeqGroup);
    MLogger::LogGeneral(String::Concat("Adding Sequence Group: ", 
        pkMSeqGroup->Name));
    return pkMSeqGroup;
}
//---------------------------------------------------------------------------
void MAnimation::RemoveSequenceGroup(unsigned int uiGroupID)
{
    MSequenceGroup* pkMSeqGroup = GetSequenceGroup(uiGroupID);
    if (!pkMSeqGroup)
    {
        return;
    }

    MLogger::LogGeneral(String::Concat("Removing Sequence Group: ", 
        pkMSeqGroup->Name));
    SetPlaybackMode(PlaybackMode::None, NULL);

    RemoveFromSequenceGroupArray(pkMSeqGroup);
    OnSequenceGroupRemoved(pkMSeqGroup);

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    NIASSERT(pkKFMTool);
    NiKFMTool::KFM_RC eRC = 
        pkKFMTool->RemoveSequenceGroup(uiGroupID);
    NI_UNUSED_ARG(eRC);
    NIASSERT(eRC == NiKFMTool::KFM_SUCCESS);

    SetNeedToSave(true);
    pkMSeqGroup->DeleteContents();
    pkData->Unlock();
}
//---------------------------------------------------------------------------
MSequence* MAnimation::GetSequence(unsigned int uiSequenceID)
{
    if (m_mapIDToSequence == NULL)
    {
        return NULL;
    }

    return dynamic_cast<MSequence*>(m_mapIDToSequence->Item
        [__box(uiSequenceID)]);
}
//---------------------------------------------------------------------------
MSequence* MAnimation::GetSequenceByName(String* strName)
{
    if ( m_aSequences != NULL)
    {
        for (int i = 0; i < m_aSequences->Count; i++)
        {
            MSequence* pkMSequence = dynamic_cast<MSequence*>(
                m_aSequences->get_Item(i));
            if (pkMSequence && pkMSequence->Name->CompareTo(strName) == 0)
                return pkMSequence;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
MSequenceGroup* MAnimation::GetSequenceGroup(unsigned int uiGroupID)
{
    if (m_mapIDToSequenceGroup == NULL)
    {
        return NULL;
    }

    return dynamic_cast<MSequenceGroup*>(m_mapIDToSequenceGroup->Item
        [__box(uiGroupID)]);
}
//---------------------------------------------------------------------------
bool MAnimation::SetPlaybackMode(PlaybackMode eMode, Object* pkModeData)
{
    float fMinTime = 0;

    PlaybackMode eOldMode = m_eMode;
    Object* pkOldModeData = m_pkModeData;

    MPhysics* pkPhysics = MFramework::get_Instance()->get_Physics();
    bool bNeedPhysicsReset = false;

    switch (eMode)
    {
        case PlaybackMode::None:
            if (pkModeData != NULL)
            {
                return false;
            }
            ResetAnimations();
            break;
        case PlaybackMode::Sequence:
        {
            MSequence* pkMSequence = dynamic_cast<MSequence*>(pkModeData);
            if (pkMSequence == NULL)
            {
                return false;
            }
            if (pkPhysics->IsSequencePhysical(pkMSequence))
            {
                ResetAnimations(true);
            }
            else
            {
                ResetAnimations(false);
            }
            bNeedPhysicsReset = true;
            bool bSuccess = ActivateSequence(pkMSequence->SequenceID);
            if (!bSuccess)
            {
                return false;
            }
            fMinTime = 0;
            break;
        }
        case PlaybackMode::SequenceGroup:
        {
            MSequenceGroup* pkMSeqGroup =
                dynamic_cast<MSequenceGroup*>(pkModeData);
            if (pkMSeqGroup == NULL)
            {
                return false;
            }
            if (pkPhysics->IsSequenceGroupPhysical(pkMSeqGroup))
            {
                ResetAnimations(true);
            }
            else
            {
                ResetAnimations();
            }
            bNeedPhysicsReset = true;
            break;
        }
        case PlaybackMode::Transition:
        {
            MTransition* pkTransition = dynamic_cast<MTransition*>(
                pkModeData);
            if (pkTransition == NULL)
            {
                return false;
            }
            ResetAnimations(true);
            bool bSuccess = SetTargetAnimation(pkTransition->SrcID);
            if (!bSuccess)
            {
                return false;
            }
            MSequence* pkSrcMSequence = GetSequence(pkTransition->SrcID);
            MSequence* pkDesMSequence = GetSequence(pkTransition->DesID);
            MAssert(pkSrcMSequence != NULL && pkDesMSequence != NULL,
                "Transition sequence not found.");

            // Transition looping causes time to reset to zero,
            // so we always need to reset physics too.
            bNeedPhysicsReset = true;

            if (eMode != eOldMode || pkModeData != pkOldModeData)
            {
                // We assume that the worst case for a frame is
                // at IncrementTime - we subtract off this amount
                // to ensure that the transition will get triggered.
                //m_fTransitionStartTime =
                //    (pkSrcMSequence->Duration / pkSrcMSequence->Frequency) -
                //    IncrementTime;
            }

            fMinTime = 0;
            break;
        }
        case PlaybackMode::Interactive:
            if (pkModeData != NULL)
            {
                return false;
            }
            ResetAnimations();
            break;
    }

    m_eMode = eMode;
    m_pkModeData = pkModeData;
    m_fLastTime = -NI_INFINITY;
    m_fStartTime = -NI_INFINITY;
    m_pkClock->ResetTime(fMinTime);
    
    if (!bNeedPhysicsReset)
    {
        // We only reset the physics time if we will not be resetting
        // all the physics state.
        pkPhysics->ResetTime(fMinTime);
    }

    Update(fMinTime, bNeedPhysicsReset);

    if (m_eMode != eOldMode || m_pkModeData != pkOldModeData)
    {
        if (m_eMode != PlaybackMode::Transition)
            m_fTransitionStartTime = INVALID_TIME;

        OnPlaybackModeChanged(m_eMode);
    }

    return true;
}
//---------------------------------------------------------------------------
bool MAnimation::ActivateSequence(unsigned int uiSequenceID)
{
    return ActivateSequence(uiSequenceID, 0, 1.0f, 0.0f);
}
//---------------------------------------------------------------------------
bool MAnimation::ActivateSequence(unsigned int uiSequenceID, int iPriority,
    float fWeight, float fEaseInTime)
{
    return ActivateSequence(uiSequenceID, iPriority, fWeight, fEaseInTime,
        NiKFMTool::SYNC_SEQUENCE_ID_NONE);
}
//---------------------------------------------------------------------------
bool MAnimation::ActivateSequence(unsigned int uiSequenceID, int iPriority,
    float fWeight, float fEaseInTime, unsigned int uiSyncToSequenceID)
{
    // Release refs for all completed sequences before we start a new one.
    ReleaseAllCompletedSequences();

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        SetLastErrorCode(MANIMATION_ERR_ACTORMANAGER_DOES_NOT_EXIST);
        pkData->Unlock();
        return false;
    }

    // Activate the sequence.
    NiControllerSequence* pkSequence = pkActorManager->ActivateSequence(
        uiSequenceID, iPriority, fWeight, fEaseInTime, uiSyncToSequenceID);

    // Store newly activated sequence with its sequence ID.
    MSequence* pkMSequence = GetSequence(uiSequenceID);
    pkMSequence->SetControllerSequence(pkSequence);

    // Set up a callback so the sequence can be released upon completion.
    if (pkSequence)
    {
        pkActorManager->SetCallbackObject(m_pkCallbackObject);
        pkActorManager->RegisterCallback(
            NiActorManager::ANIM_COMPLETED, uiSequenceID);
    }

    pkData->Unlock();

    OnSequenceActivated(pkMSequence);

    return (pkSequence != NULL);
}
//---------------------------------------------------------------------------
bool MAnimation::DeactivateSequence(unsigned int uiSequenceID)
{
    return DeactivateSequence(uiSequenceID, 0.0f);
}
//---------------------------------------------------------------------------
bool MAnimation::DeactivateSequence(unsigned int uiSequenceID,
    float fEaseOutTime)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        SetLastErrorCode(MANIMATION_ERR_ACTORMANAGER_DOES_NOT_EXIST);
        pkData->Unlock();
        return false;
    }
    bool bSuccess = pkActorManager->DeactivateSequence(uiSequenceID,
        fEaseOutTime);
    pkData->Unlock();

    OnSequenceDeactivated(dynamic_cast<MSequence*>(m_mapIDToSequence->Item
        [__box(uiSequenceID)]));

    // Release refs for all completed sequences.
    ReleaseAllCompletedSequences();

    return bSuccess;
}
//---------------------------------------------------------------------------
bool MAnimation::SetTargetAnimation(unsigned int uiSequenceID)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        SetLastErrorCode(MANIMATION_ERR_ACTORMANAGER_DOES_NOT_EXIST);
        pkData->Unlock();
        return false;
    }
    bool bSuccess = pkActorManager->SetTargetAnimation(uiSequenceID);
    pkData->Unlock();

    OnTargetAnimationSet(dynamic_cast<MSequence*>(m_mapIDToSequence->Item
        [__box(uiSequenceID)]));

    return bSuccess;
}
//---------------------------------------------------------------------------
void MAnimation::RunUpTime(float fTime)
{
    m_pkClock->RunUpTime(fTime);
}
//---------------------------------------------------------------------------
void MAnimation::ResetAnimations()
{
    ResetAnimations(false);
}
//---------------------------------------------------------------------------
void MAnimation::ResetAnimations(bool bResetAccum)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        SetLastErrorCode(MANIMATION_ERR_ACTORMANAGER_DOES_NOT_EXIST);
        pkData->Unlock();
        return;
    }
    pkActorManager->Reset();
    pkActorManager->ClearAllRegisteredCallbacks();
    pkActorManager->SetCallbackObject(NULL);
    if (bResetAccum)
    {
        pkActorManager->GetControllerManager()->ClearCumulativeAnimations();
    }
    pkData->Unlock();

    // Release refs for all completed sequences.
    ReleaseAllCompletedSequences();

    OnResetAnimations();
}
//---------------------------------------------------------------------------
bool MAnimation::ChangeSequenceID(unsigned int uiOldID, unsigned int uiNewID)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        SetLastErrorCode(MANIMATION_ERR_ACTORMANAGER_DOES_NOT_EXIST);
        pkData->Unlock();
        return false;
    }
    NiKFMTool::KFM_RC eRC = pkActorManager->ChangeSequenceID(uiOldID,
        uiNewID);

    // We must now cycle through the MTransitions and update them
    ArrayList* pkMSequenceList = get_Sequences();
    if (pkMSequenceList != NULL)
    {
        for (int i = 0; i < pkMSequenceList->Count; i++)
        {
            MSequence* pkMSequence = dynamic_cast<MSequence*>(
                pkMSequenceList->get_Item(i));
            if (pkMSequence == NULL)
            {
                continue;
            }

            // Now get the MTransition list
            MTransition* pkTranList[] = pkMSequence->get_Transitions();
            if (pkTranList != NULL)
            {
                for (int t=0; t<pkTranList->Count; t++)
                {
                    MTransition* pkTran = dynamic_cast<MTransition*>(
                        pkTranList->get_Item(t));

                    if (pkTran == NULL)
                    {
                        continue;
                    }

                    pkTran->ChangeSequenceID(uiOldID, uiNewID);
                }
            }
        }
    }


    pkData->Unlock();
    if (eRC != NiKFMTool::KFM_SUCCESS)
    {
        SetLastErrorCode(MANIMATION_ERR_KFMTOOL_ERROR);
        SetLastErrorString( NiKFMTool::LookupReturnCode(eRC));
        return false;
    }

    SetNeedToSave(true);
    MSequence* pkMSequence = dynamic_cast<MSequence*>(m_mapIDToSequence->Item
        [__box(uiOldID)]);
    m_mapIDToSequence->Remove(__box(uiOldID));
    m_mapIDToSequence->Add(__box(uiNewID), pkMSequence);
    MLogger::LogGeneral(String::Concat("Changing Sequence ID: ", 
        pkMSequence->Name));

    return true;
}
//---------------------------------------------------------------------------
bool MAnimation::ChangeGroupID(unsigned int uiOldID, unsigned int uiNewID)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return false;
    }
    NiKFMTool::KFM_RC eRC = pkKFMTool->UpdateGroupID(uiOldID, uiNewID);
    pkData->Unlock();
    if (eRC != NiKFMTool::KFM_SUCCESS)
    {
        SetLastErrorCode(MANIMATION_ERR_KFMTOOL_ERROR);
        SetLastErrorString(pkKFMTool->LookupReturnCode(eRC));
        return false;
    }

    SetNeedToSave(true);
    MSequenceGroup* pkMSeqGroup = dynamic_cast<MSequenceGroup*>(
        m_mapIDToSequenceGroup->Item[__box(uiOldID)]);
    m_mapIDToSequenceGroup->Remove(__box(uiOldID));
    m_mapIDToSequenceGroup->Add(__box(uiNewID), pkMSeqGroup);
    MLogger::LogGeneral(String::Concat("Changing Group ID: ", 
        pkMSeqGroup->Name));

    return true;
}
//---------------------------------------------------------------------------
unsigned int MAnimation::FindUnusedGroupID()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return 0;
    }
    unsigned int uiGroupID = pkKFMTool->FindUnusedGroupID();
    pkData->Unlock();
    return uiGroupID;
}
//---------------------------------------------------------------------------
unsigned int MAnimation::FindUnusedSequenceID()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return 0;
    }
    unsigned int uiID = pkKFMTool->FindUnusedSequenceID();
    pkData->Unlock();
    return uiID;
}
//---------------------------------------------------------------------------
String* MAnimation::FindUnusedSequenceName(String* strBaseName)
{
    Object* pkClone = strBaseName->Clone();
    String* strNewName = dynamic_cast<String*>(pkClone);
    int iPostScript = 1;
    if (m_aSequences != NULL && strBaseName != NULL)
    {
        bool bFoundUnique = false;
        while (!bFoundUnique)
        {
            bool bFoundMatch = false;
            int iCount = m_aSequences->Count;
            for (int i = 0; i < iCount; i++)
            {
                if (dynamic_cast<MSequence*>(
                    m_aSequences->get_Item(i)) != NULL)
                {
                    if (String::Compare(dynamic_cast<MSequence*>(
                        m_aSequences->get_Item(i))->Name, strNewName) == 0)
                    {
                        strNewName = String::Concat(strBaseName, " ", 
                            iPostScript.ToString());
                        bFoundMatch = true;
                        iPostScript++;
                        break;
                    }
                }
            }

            if (!bFoundMatch)
                bFoundUnique = true;
        }
    }

    return strNewName;
}
//---------------------------------------------------------------------------
String* MAnimation::FindUnusedSequenceGroupName(String* strBaseName)
{
    Object* pkClone = strBaseName->Clone();
    String* strNewName = dynamic_cast<String*>(pkClone);
    int iPostScript = 1;
    if (m_aSequenceGroups != NULL && strBaseName != NULL)
    {
        bool bFoundUnique = false;
        while (!bFoundUnique)
        {
            bool bFoundMatch = false;
            int iCount = m_aSequenceGroups->Count;
            for (int i = 0; i < iCount; i++)
            {
                if (dynamic_cast<MSequenceGroup*>(
                    m_aSequenceGroups->get_Item(i)) != NULL)
                {
                    if (String::Compare(dynamic_cast<MSequenceGroup*>(
                        m_aSequenceGroups->get_Item(i))->Name,
                        strNewName) == 0)
                    {
                        strNewName = String::Concat(strBaseName, " ", 
                            iPostScript.ToString());
                        iPostScript++;
                        bFoundMatch = true;
                        break;
                    }
                }
            }

            if (!bFoundMatch)
                bFoundUnique = true;
        }
    }

    return strNewName;
}
//---------------------------------------------------------------------------
void MAnimation::AddToSequenceArray(MSequence* pkMSequence, 
    bool bTrimToSize)
{
    m_aSequences->Add(pkMSequence);
    if (bTrimToSize)
        m_aSequences->TrimToSize();
    m_mapIDToSequence->Add(__box(pkMSequence->SequenceID), pkMSequence);
}
//---------------------------------------------------------------------------
void MAnimation::AddToSequenceGroupArray(MSequenceGroup* pkMSeqGroup,
    bool bTrimToSize)
{
    m_aSequenceGroups->Add(pkMSeqGroup);
    if (bTrimToSize)
        m_aSequenceGroups->TrimToSize();
    m_mapIDToSequenceGroup->Add(__box(pkMSeqGroup->GroupID), 
        pkMSeqGroup);
}
//---------------------------------------------------------------------------
void MAnimation::RemoveFromSequenceArray(MSequence* pkMSequence)
{
    m_aSequences->Remove(pkMSequence);
    m_aSequences->TrimToSize();
    m_mapIDToSequence->Remove(__box(pkMSequence->SequenceID));
    
}
//---------------------------------------------------------------------------
void MAnimation::RemoveFromSequenceGroupArray(MSequenceGroup* pkMSeqGroup)
{
    m_aSequenceGroups->Remove(pkMSeqGroup);
    m_aSequenceGroups->TrimToSize();
    m_mapIDToSequenceGroup->Remove(__box(pkMSeqGroup->GroupID));
}
//---------------------------------------------------------------------------
void MAnimation::BuildSequenceArray()
{
    ClearSequenceArray();
    m_mapIDToSequence->Clear();

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return;
    }

    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        SetLastErrorCode(MANIMATION_ERR_ACTORMANAGER_DOES_NOT_EXIST);
        pkData->Unlock();
        return;
    }

    unsigned int* puiSequenceIDs;
    unsigned int uiNumIDs;
    pkKFMTool->GetSequenceIDs(puiSequenceIDs, uiNumIDs);

    m_aSequences = NiExternalNew ArrayList(uiNumIDs);
    for (unsigned int ui = 0; ui < uiNumIDs; ui++)
    {
        unsigned int uiSequenceID = puiSequenceIDs[ui];
        NiKFMTool::Sequence* pkKFMSequence = pkKFMTool->GetSequence(
            uiSequenceID);
        NIASSERT(pkKFMSequence);
        NiSequenceData* pkSeqData =
            pkActorManager->GetSequenceData(uiSequenceID);
        NIASSERT(pkSeqData);
        MSequence* pkMSequence = NiExternalNew MSequence(this, pkKFMSequence,
            pkSeqData);
        AddToSequenceArray(pkMSequence, false);
    }
    m_aSequences->TrimToSize();
    pkData->Unlock();

    NiFree(puiSequenceIDs);
}
//---------------------------------------------------------------------------
void MAnimation::SetInitialTransitionTimeRowCol(MSequence* pkMSequence)
{
    // This function is used when adding a new kf file incrementally.
    // If a number of kf's have been added all at once, then the
    // SetIntialTransitionTimes function should be used.
    SetInitialTransitionTimeRow(pkMSequence);
    SetInitialTransitionTimeCol(pkMSequence);
}
//---------------------------------------------------------------------------
void MAnimation::SetInitialTransitionTimeCol(MSequence* pkMSequence)
{

    if (m_aSequences == NULL)
    {
        return;
    }

    for (int i = 0; i < m_aSequences->Count; i++)
    {
        MSequence* pkSrcMSequence = dynamic_cast<MSequence*>(
            m_aSequences->Item[i]);

        for (int j=0; j < pkSrcMSequence->Transitions->Count; j++)
        {
            MTransition* pkTransition = pkSrcMSequence->Transitions[j];
            NIASSERT(pkTransition != NULL);

            if (pkTransition->Source == NULL)
                continue;

            if (pkTransition->Destination->SequenceID != 
                pkMSequence->SequenceID)
                continue;

            pkTransition->DefaultTransitionTime = 
                pkTransition->Source->DurationDivFreq;
        }
    }
}
//---------------------------------------------------------------------------
void MAnimation::SetInitialTransitionTimeRow(MSequence* pkMSequence)
{
    if (pkMSequence->Transitions == NULL)
        return;

    for (int j=0; j < pkMSequence->Transitions->Count; j++)
    {
        MTransition* pkTransition = pkMSequence->Transitions[j];
        NIASSERT(pkTransition != NULL);

        if (pkTransition->Source == NULL)
            return;

        pkTransition->DefaultTransitionTime = pkMSequence->DurationDivFreq;
    }
}
//---------------------------------------------------------------------------
void MAnimation::SetInitialTransitionTimes()
{
    // This function is used when adding a number of kf's at once.
    // If adding only one kf at at time, then SetInitialTransitionTimeRowCol
    // should be used.

    if (m_aSequences == NULL)
    {
        return;
    }

    for (int i = 0; i < m_aSequences->Count; i++)
    {
        MSequence* pkMSequence = dynamic_cast<MSequence*>(
            m_aSequences->Item[i]);

        SetInitialTransitionTimeRow(pkMSequence);
    }
}
//---------------------------------------------------------------------------
void MAnimation::BuildSequenceGroupArray()
{
    ClearSequenceGroupArray();
    m_mapIDToSequenceGroup->Clear();

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return;
    }

    unsigned int* puiGroupIDs;
    unsigned int uiNumIDs;
    pkKFMTool->GetGroupIDs(puiGroupIDs, uiNumIDs);

    m_aSequenceGroups = NiExternalNew ArrayList(uiNumIDs);
    for (unsigned int ui = 0; ui < uiNumIDs; ui++)
    {
        unsigned int uiGroupID = puiGroupIDs[ui];
        NiKFMTool::SequenceGroup* pkKFMSeqGroup = pkKFMTool->GetSequenceGroup(
            uiGroupID);
        NIASSERT(pkKFMSeqGroup);
        MSequenceGroup* pkMSeqGroup = NiExternalNew MSequenceGroup(this,
            pkKFMSeqGroup);
        AddToSequenceGroupArray(pkMSeqGroup, false);
    }
    m_aSequenceGroups->TrimToSize();
    pkData->Unlock();

    NiFree(puiGroupIDs);
}
//---------------------------------------------------------------------------
void MAnimation::ClearSequenceArray()
{
    if (m_aSequences == NULL)
    {
        return;
    }

    for (int i = 0; i < m_aSequences->Count; i++)
    {
        MSequence* pkMSequence = dynamic_cast<MSequence*>(
            m_aSequences->get_Item(i));

        if (pkMSequence)
            pkMSequence->DeleteContents();
    }
    m_aSequences->Clear();
}
//---------------------------------------------------------------------------
void MAnimation::ClearSequenceGroupArray()
{
    if (m_aSequenceGroups == NULL)
    {
        return;
    }

    for (int i = 0; i < m_aSequenceGroups->Count; i++)
    {
        MSequenceGroup* pkMSeqGroup = dynamic_cast<MSequenceGroup*>(
            m_aSequenceGroups->get_Item(i));

        if (pkMSeqGroup)
            pkMSeqGroup->DeleteContents();
    }
    m_aSequenceGroups->Clear();
}
//---------------------------------------------------------------------------
float MAnimation::FindTimeForAnimationToComplete(
    MSequence* pkMSequence, float fBeginTime)
{
    NIASSERT(pkMSequence);
    NIASSERT(fBeginTime > pkMSequence->DurationDivFreq);

    if (fBeginTime < pkMSequence->DurationDivFreq)
        return (pkMSequence->DurationDivFreq - fBeginTime);

    return 0;
}
//---------------------------------------------------------------------------
MCompletionInfo* MAnimation::FindTimeForAnimationToCompleteTransition(
    MTransition* pkTransition, float fBeginTime, float fDesiredTransitionTime)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        pkData->Unlock();
        return NULL;
    }
    MCompletionInfo* pkResult = NiExternalNew MCompletionInfo();
    bool bSuccess = pkActorManager->FindTimeForAnimationToCompleteTransition(
        pkTransition->SrcID, pkTransition->DesID,
        pkTransition->GetTransition(), fBeginTime, fDesiredTransitionTime,
        pkResult->GetCompletionInfo());

    pkData->Unlock();

    if (bSuccess)
    {
        pkResult->BuildChainCompletionInfoArray();
        return pkResult;
    }

    pkResult->Dispose();
    return NULL;
}
//---------------------------------------------------------------------------
void MAnimation::OnRunUpTime(float fEndTime)
{
    if (m_fStartTime == -NI_INFINITY)
    {
        return;
    }

    // Update at start time.
    float fStartTime = m_fStartTime;
    switch (m_eMode)
    {
        case PlaybackMode::Sequence:
        {
            // Reset last time.
            m_fLastTime = -NI_INFINITY;
            break;
        }
        case PlaybackMode::Transition:
        {
            // Reset playback mode.
            bool bSuccess = SetPlaybackMode(PlaybackMode::Transition,
                m_pkModeData);
            MAssert(bSuccess);
            break;
        }
    }

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

#if EE_PHYSX_BUILD
    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (pkActorManager)
        pkActorManager->GetControllerManager()->ClearCumulativeAnimations();
#endif

    OnLongOperationStarted();

    Update(fStartTime, true);

    // Restore accumulation
    switch (m_eMode)
    {
        case PlaybackMode::Sequence:
            MSequence* pkMSequence = static_cast<MSequence*>(m_pkModeData);
            pkMSequence->RestoreAccum();
            break;
    }

    pkData->Unlock();

    // Update at rest of times.
    for (float fTime = m_fStartTime + m_fTimeIncrement; fTime < fEndTime;
        fTime += m_fTimeIncrement)
    {
        Update(fTime, false);
    }
    Update(fEndTime, false);

    OnLongOperationCompleted();
}
//---------------------------------------------------------------------------
MSequence::AnimState MAnimation::GetAnimState(unsigned int uiSequenceID)
{
    MSequence::AnimState eAnimState = MSequence::INACTIVE;
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    MSequence* pkMSequence = GetSequence(uiSequenceID);
    NiControllerSequence* pkSequence = pkMSequence->GetControllerSequence();
    if (pkSequence)
    {
        eAnimState = (MSequence::AnimState)pkSequence->GetState();
    }

    pkData->Unlock();
    return eAnimState;
}
//---------------------------------------------------------------------------
float MAnimation::GetSequenceWeight(unsigned int uiSequenceID)
{
    float fWeight = 0.0f;
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    MSequence* pkMSequence = GetSequence(uiSequenceID);
    NiControllerSequence* pkSequence = pkMSequence->GetControllerSequence();
    if (pkSequence)
    {
        fWeight = pkSequence->GetWeight();
    }

    pkData->Unlock();
    return fWeight;
}
//---------------------------------------------------------------------------
void MAnimation::SetSequenceWeight(unsigned int uiSequenceID, float fWeight)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    MSequence* pkMSequence = GetSequence(uiSequenceID);
    NiControllerSequence* pkSequence = pkMSequence->GetControllerSequence();
    if (pkSequence)
    {
        pkSequence->SetWeight(fWeight);
    }

    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MAnimation::ReleaseAllCompletedSequences()
{
    unsigned int uiNumCompletedSequences = 
        m_pkCallbackObject->GetNumCompletedSequences();
    for (unsigned int ui = 0; ui < uiNumCompletedSequences; ui++)
    {
        unsigned int uiSequenceID = 
            m_pkCallbackObject->GetCompletedSequenceID(ui);

        MSequence* pkMSequence = GetSequence(uiSequenceID);
        pkMSequence->SetControllerSequence(NULL);
    }
    m_pkCallbackObject->RemoveAllCompletedSequences();
}
//---------------------------------------------------------------------------
void MAnimation::HandleUnresolvedSequences()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkActorManager = pkData->GetActorManager();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();

    // Collect all unresolved sequences
    m_aUnresolvedSequenceInfos = NiExternalNew ArrayList();
    if (pkKFMTool && pkActorManager)
    {
        unsigned int* puiSequenceIDs = NULL;
        unsigned int uiNumIDs = 0;

        pkKFMTool->GetSequenceIDs(puiSequenceIDs, uiNumIDs);
        for (unsigned int ui = 0; ui < uiNumIDs; ui++)
        {
            unsigned int uiSeqID = puiSequenceIDs[ui];
            NiSequenceData* pkSeqData = 
                pkActorManager->GetSequenceData(uiSeqID);

            if (pkSeqData == NULL)
            {
                NiKFMTool::Sequence* pkKFMSequence = pkKFMTool->GetSequence(
                    uiSeqID);
                NIASSERT(pkKFMSequence);

                MSequence::MInvalidSequenceInfo* pkInfo = NiExternalNew 
                    MSequence::MInvalidSequenceInfo();
                pkInfo->SequenceID = uiSeqID;
                pkInfo->Filename = pkKFMTool->GetFullKFFilename(uiSeqID);
                pkInfo->OriginalFilename = 
                    pkKFMTool->GetFullKFFilename(uiSeqID);
                pkInfo->AnimIndex = pkKFMSequence->GetAnimIndex();
                if (pkKFMSequence->GetSequenceName().Exists())
                    pkInfo->SequenceName = pkKFMSequence->GetSequenceName();
                else
                    pkInfo->SequenceName = NULL;
                m_aUnresolvedSequenceInfos->Add(pkInfo);
            }
        }
        NiFree(puiSequenceIDs);
    }

    // Allow the user to find the missing kf files
    bool bCancel = false;
    if (m_aUnresolvedSequenceInfos->Count != 0)
    {
        KFFFailedLoadArgs* pkArgs = NiExternalNew KFFFailedLoadArgs();
        pkArgs->aMissingSequences = m_aUnresolvedSequenceInfos;
        pkArgs->bCancel = false;

        OnKFLoadFailed(pkArgs);

        bCancel = pkArgs->bCancel;
    }

    // Try to reload if possible, otherwise remove the sequence
    ArrayList* pkSuccessfullyReloaded = NiExternalNew ArrayList();
    for (int ij = 0; ij < m_aUnresolvedSequenceInfos->Count; ij++)
    {
        MSequence::MInvalidSequenceInfo* pkInfo = 
            dynamic_cast<MSequence::MInvalidSequenceInfo*>(
            m_aUnresolvedSequenceInfos->get_Item(ij));
        if (pkInfo)
        {
            bool bRemove = true;
            if (!bCancel)
            {
                NiKFMTool::Sequence* pkKFMSequence = pkKFMTool->GetSequence(
                    pkInfo->SequenceID);
                NIASSERT(pkKFMSequence);

                const char* pcNewFilename = 
                    MStringToCharPointer(pkInfo->Filename);
                pkKFMSequence->SetFilename(pcNewFilename);
                pkKFMSequence->SetSequenceName(MStringToCharPointer(
                    pkInfo->SequenceName));
                MFreeCharPointer(pcNewFilename);

                if (pkActorManager->LoadSequenceData(pkInfo->SequenceID))
                {
                    pkSuccessfullyReloaded->Add(pkInfo);
                    bRemove = false;
                }
            }

            if (bRemove)
            {
                unsigned int uiSeqID = pkInfo->SequenceID;
                NiKFMTool::KFM_RC eRC = 
                    pkKFMTool->RemoveSequence(uiSeqID);
                NI_UNUSED_ARG(eRC);
                NIASSERT(eRC == NiKFMTool::KFM_SUCCESS);
            }
        }
    }

    // If we successfully reloaded a file, remove it from
    // the unresolved list
    for (int k = 0; k < pkSuccessfullyReloaded->Count; k++)
    {
        m_aUnresolvedSequenceInfos->Remove(
            pkSuccessfullyReloaded->get_Item(k));
    }

    m_aUnresolvedSequenceInfos->TrimToSize();
    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MAnimation::ThrowSequenceGroupModifiedEvent(
    MSequenceGroupEventArgs* pkArgs, MSequenceGroup* pkMSeqGroup)
{
    SetNeedToSave(true);
    OnSequenceGroupModified(pkArgs, pkMSeqGroup);
}
//---------------------------------------------------------------------------
void MAnimation::ThrowSequenceGroupSequenceInfoModifiedEvent(
    MSequenceGroup::MSequenceInfo::PropertyType ePropChanged, 
    MSequenceGroup::MSequenceInfo* pkSequenceInfo, 
    MSequenceGroup* pkMSeqGroup)
{
    SetNeedToSave(true);
    OnSequenceGroup_SequenceInfoModifiedEvent(ePropChanged, 
        pkSequenceInfo, pkMSeqGroup);
}
//---------------------------------------------------------------------------
void MAnimation::ThrowSequenceModifiedEvent(
    MSequence::PropertyType ePropType,
    MSequence* pkMSequence)
{
    SetNeedToSave(true);
    OnSequenceModified(ePropType, pkMSequence);
}
//---------------------------------------------------------------------------
void MAnimation::ThrowTransitionModifiedEvent(
    MTransition::PropertyType ePropChanged, MTransition* pkTransition)
{
    SetNeedToSave(true);
    OnTransitionModified(ePropChanged, pkTransition);
}
//---------------------------------------------------------------------------
MAnimation::MANIMATION_RC MAnimation::GetLastErrorCode()
{
    return m_LastErrorCode;
}
//---------------------------------------------------------------------------
void MAnimation::SetLastErrorCode(MANIMATION_RC rc)
{
    m_LastErrorCode = rc;
    SetLastErrorString(LookupReturnCode(rc));
}
//---------------------------------------------------------------------------
String* MAnimation::GetLastErrorString()
{
    return m_LastErrorString;
}
//---------------------------------------------------------------------------
void MAnimation::SetLastErrorString(String* strError)
{
    m_LastErrorString = strError;
    MLogger::LogWarning(strError);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Properties
//---------------------------------------------------------------------------
ArrayList* MAnimation::get_Sequences()
{
    return m_aSequences;
}
//---------------------------------------------------------------------------
ArrayList* MAnimation::get_SequenceGroups()
{
    return m_aSequenceGroups;
}
//---------------------------------------------------------------------------
String* MAnimation::get_ModelPath()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return NULL;
    }
    String* strModelPath = pkKFMTool->GetModelPath();
    pkData->Unlock();

    return strModelPath;
}
//---------------------------------------------------------------------------
String* MAnimation::get_FullModelPath()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return NULL;
    }
    String* strFullModelPath = pkKFMTool->GetFullModelPath();
    pkData->Unlock();

    return strFullModelPath;
}
//---------------------------------------------------------------------------
String* MAnimation::get_ModelRoot()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return NULL;
    }
    String* strModelRoot = pkKFMTool->GetModelRoot();
    pkData->Unlock();

    return strModelRoot;
}
//---------------------------------------------------------------------------
String* MAnimation::get_KFMFilename()
{
    return m_strKFMFilename;
}
//---------------------------------------------------------------------------
String* MAnimation::get_BaseKFMPath()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return NULL;
    }
    String* strKFMPath = pkKFMTool->GetBaseKFMPath();
    pkData->Unlock();
    return strKFMPath;
}
//---------------------------------------------------------------------------
void MAnimation::set_BaseKFMPath(String* strPath)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return;
    }
    SetNeedToSave(true);
    const char* pcPath = MStringToCharPointer(strPath);
    pkKFMTool->SetBaseKFMPath(pcPath);
    MFreeCharPointer(pcPath);
    pkData->Unlock();
}
//---------------------------------------------------------------------------
float MAnimation::get_DefaultSeqGroupEaseIn()
{
    return m_fDefaultSeqGroupEaseIn;
}
//---------------------------------------------------------------------------
float MAnimation::get_DefaultSeqGroupEaseOut()
{
    return m_fDefaultSeqGroupEaseOut;
}
//---------------------------------------------------------------------------
unsigned int MAnimation::get_DefaultSeqGroupPriority()
{
    return m_uiDefaultSeqGroupPriority;
}
//---------------------------------------------------------------------------
float MAnimation::get_DefaultSeqGroupWeight()
{
    return m_fDefaultSeqGroupWeight;
}
//---------------------------------------------------------------------------
MAnimation::PlaybackMode MAnimation::get_Mode()
{
    return m_eMode;
}
//---------------------------------------------------------------------------
float MAnimation::get_CurrentTime()
{
    float fCurrentTime = 0.0f;
    switch (m_eMode)
    {
        case PlaybackMode::Sequence:
            fCurrentTime = m_pkClock->CurrentTime - m_fStartTime;
            break;
        case PlaybackMode::Transition:
            fCurrentTime = m_pkClock->CurrentTime;
            break;
    }

    return fCurrentTime;
}
//---------------------------------------------------------------------------
void MAnimation::set_CurrentTime(float fCurrentTime)
{
    switch (m_eMode)
    {
        case PlaybackMode::Sequence:
            m_pkClock->CurrentTime = m_fStartTime + fCurrentTime;
            break;
        case PlaybackMode::Transition:
            m_pkClock->CurrentTime = fCurrentTime;
            break;
    }
}
//---------------------------------------------------------------------------
float MAnimation::get_IncrementTime()
{
    return m_fTimeIncrement;
}
//---------------------------------------------------------------------------
void MAnimation::set_IncrementTime(float fTime)
{
    m_fTimeIncrement = fTime;
}
//---------------------------------------------------------------------------
MSequence* MAnimation::get_ActiveSequence()
{
    if (m_eMode != PlaybackMode::Sequence)
    {
        return NULL;
    }

    return static_cast<MSequence*>(m_pkModeData);
}
//---------------------------------------------------------------------------
MSequenceGroup* MAnimation::get_ActiveSequenceGroup()
{
    if (m_eMode != PlaybackMode::SequenceGroup)
    {
        return NULL;
    }

    return static_cast<MSequenceGroup*>(m_pkModeData);
}
//---------------------------------------------------------------------------
MTransition* MAnimation::get_ActiveTransition()
{
    if (m_eMode != PlaybackMode::Transition)
    {
        return NULL;
    }

    return static_cast<MTransition*>(m_pkModeData);
}
//---------------------------------------------------------------------------
float MAnimation::get_TransitionStartTime()
{
    if (m_eMode != PlaybackMode::Transition)
    {
        MAssert(false, "Invalid PlaybackMode for TransitionStartTime "
            "access", "TransitionStartTime can only be accessed in "
            "Transition playback mode.");
        return INVALID_TIME;
    }

    return m_fTransitionStartTime;
}
//---------------------------------------------------------------------------
ArrayList* MAnimation::get_UnresolvedSequenceInfo()
{
    return m_aUnresolvedSequenceInfos;
}
//---------------------------------------------------------------------------
void MAnimation::set_TransitionStartTime(float fTransitionStartTime)
{
    if (m_eMode != PlaybackMode::Transition)
    {
        MAssert(false, "Invalid PlaybackMode for TransitionStartTime "
            "access", "TransitionStartTime can only be accessed in "
            "Transition playback mode.");
        return;
    }

    m_fTransitionStartTime = fTransitionStartTime;
}
//---------------------------------------------------------------------------
unsigned int MAnimation::get_LoopCounterRange()
{
    return m_pkCallbackObject->GetCounterRange();
}
//---------------------------------------------------------------------------
void MAnimation::set_LoopCounterRange(unsigned int uiLoopCounterRange)
{
    m_pkCallbackObject->SetCounterRange(uiLoopCounterRange);
}
//---------------------------------------------------------------------------
MTransition::TransitionType MAnimation::get_DefaultSyncTransType()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NIASSERT(pkData->GetKFMTool());
    MTransition::TransitionType eType = MTransition::TranslateTransitionType(
        pkData->GetKFMTool()->GetDefaultSyncTransType(), NULL);
    pkData->Unlock();

    return eType;
}
//---------------------------------------------------------------------------
void MAnimation::set_DefaultSyncTransType(MTransition::TransitionType eType)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NIASSERT(pkData->GetKFMTool());
    NiKFMTool::KFM_RC eRC = 
        pkData->GetKFMTool()->SetDefaultSyncTransType(
        MTransition::TranslateTransitionType(eType));
    NI_UNUSED_ARG(eRC);
    NIASSERT(eRC == NiKFMTool::KFM_SUCCESS);
    pkData->Unlock();

    SetNeedToSave(true);
    OnDefaultTransitionSettingsChanged(MTransition::Trans_DefaultSync);
}
//---------------------------------------------------------------------------
MTransition::TransitionType MAnimation::get_DefaultNonSyncTransType()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NIASSERT(pkData->GetKFMTool());
    MTransition::TransitionType eType = MTransition::TranslateTransitionType(
        pkData->GetKFMTool()->GetDefaultNonSyncTransType(), NULL);
    pkData->Unlock();

    return eType;
}
//---------------------------------------------------------------------------
void MAnimation::set_DefaultNonSyncTransType(
    MTransition::TransitionType eType)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NIASSERT(pkData->GetKFMTool());
    NiKFMTool::KFM_RC eRC = 
        pkData->GetKFMTool()->SetDefaultNonSyncTransType(
        MTransition::TranslateTransitionType(eType));
    NI_UNUSED_ARG(eRC);
    NIASSERT(eRC == NiKFMTool::KFM_SUCCESS);
    pkData->Unlock();

    SetNeedToSave(true);
    OnDefaultTransitionSettingsChanged(MTransition::Trans_DefaultNonSync);
}
//---------------------------------------------------------------------------
float MAnimation::get_DefaultSyncTransDuration()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NIASSERT(pkData->GetKFMTool());
    float fDuration = pkData->GetKFMTool()->GetDefaultSyncTransDuration();
    pkData->Unlock();

    return fDuration;
}
//---------------------------------------------------------------------------
void MAnimation::set_DefaultSyncTransDuration(float fDuration)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NIASSERT(pkData->GetKFMTool());
    pkData->GetKFMTool()->SetDefaultSyncTransDuration(fDuration);
    pkData->Unlock();

    SetNeedToSave(true);
    OnDefaultTransitionSettingsChanged(MTransition::Trans_DefaultSync);
}
//---------------------------------------------------------------------------
float MAnimation::get_DefaultNonSyncTransDuration()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NIASSERT(pkData->GetKFMTool());
    float fDuration = pkData->GetKFMTool()->GetDefaultNonSyncTransDuration();
    pkData->Unlock();

    return fDuration;
}
//---------------------------------------------------------------------------
void MAnimation::set_DefaultNonSyncTransDuration(float fDuration)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NIASSERT(pkData->GetKFMTool());
    pkData->GetKFMTool()->SetDefaultNonSyncTransDuration(fDuration);
    pkData->Unlock();

    SetNeedToSave(true);
    OnDefaultTransitionSettingsChanged(MTransition::Trans_DefaultNonSync);
}
//---------------------------------------------------------------------------
MSequence* MAnimation::get_TargetAnimation()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        pkData->Unlock();
        return NULL;
    }
    unsigned int uiSequenceID = pkActorManager->GetTargetAnimation();
    pkData->Unlock();

    return GetSequence(uiSequenceID);
}
//---------------------------------------------------------------------------
bool MAnimation::get_NeedToSave()
{
    return m_bNeedToSave;
}
//---------------------------------------------------------------------------
void MAnimation::SetNeedToSave(bool bNeedToSave)
{
    m_bNeedToSave = bNeedToSave;
}
//---------------------------------------------------------------------------
bool MAnimation::get_CanSave()
{
    return this->m_bCanSave;
}
//---------------------------------------------------------------------------
inline const char* MAnimation::LookupReturnCode(
    MAnimation::MANIMATION_RC eReturnCode)
{
    switch (eReturnCode)
    {
        case MANIMATION_ERR_KFMTOOL_ERROR:
            return "KFMTool Error";
        case MANIMATION_ERR_CREATE_ACTOR_MANAGER_FAILED:
            return "The actor manager could not be created.";
            // SetLastError("Failed to create NiActorManager!");
        case MANIMATION_ERR_FILENAME_EMPTY:
            return "The filename is empty.";
        case MANIMATION_ERR_KF_FILENAME_EMPTY:
            return "The KF filename was empty.";
        case MANIMATION_ERR_NO_NIF_ROOT_EXIST:
            return "The NIF root does not exist.";
        case MANIMATION_ERR_NO_KFMTOOL:
            return "The KFMTool does not exist.";
        case MANIMATION_ERR_MODEL_PATH_EMPTY:
            return "The model path is empty.";
        case MANIMATION_ERR_FAILED_LOADING_NIF:
            return "Unable to load NIF file.";
        case MANIMATION_ERR_NIF_OBJECT_NOT_NIAVOBJECT:
            return "First object in NIF file is not an NiAVObject.";
        case MANIMATION_ERR_NIF_NIF_OBJECT_NO_NAME_FOR_FILE:
            return "First object in NIF file does not have a name.";
        case MANIMATION_ERR_FAILED_TO_RELOAD_NIF:
            return "Failed to reload the NIF file.";
        case MANIMATION_ERR_MODEL_ROOT_NIF_MISMATCH:
            return "Model root does not match NIF file";
        case MANIMATION_ERR_SEQUENCE_DOES_NOT_EXIST:
            return "The sequence did not exist";
        case MANIMATION_ERR_SRC_SEQUENCE_DOES_NOT_EXIST:
            return "The source sequence did not exist";
        case MANIMATION_ERR_SRC_OR_DEST_DOES_NOT_EXIST:
            return "The source or destination sequences do not exist.";
        case MANIMATION_ERR_TRANSITION_DOES_NOT_EXIST:
            return "The transition does not exist";
        case MANIMATION_ERR_ACTORMANAGER_DOES_NOT_EXIST:
            return "The actor manager does not exist.";
        case MANIMATION_ERR_FAILED_TO_LOAD_CHARACTER_NIF:
            return "Unable to load character NIF file";
        case MANIMATION_ERR_SEQUENCE_CAN_NOT_BE_LOADED:
            return "Sequence could not be loaded";
        case MANIMATION_ERR_FAILED_LOADING_KF:
            return "Error loading KF file.";
        case MANIMATION_ERR_OBJECT_IN_KF_NOT_SEQUENCE:
            return "A top-level object in the KF file is not a sequence.";
    }
    // unknown type
    MAssert(false);
    return NULL;
}
//---------------------------------------------------------------------------
String* MAnimation::GetFullKFFilename(unsigned int uiSequenceID)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return NULL;
    }
    String* strFullKFFilename = pkKFMTool->GetFullKFFilename(uiSequenceID);
    pkData->Unlock();

    return strFullKFFilename;
}
//---------------------------------------------------------------------------
