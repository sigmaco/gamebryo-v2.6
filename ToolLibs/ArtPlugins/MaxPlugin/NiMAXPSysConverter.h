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


#ifndef NIMAXPSYSCONVERTER_H
#define NIMAXPSYSCONVERTER_H

#include "NiMAX.h"
#include "NiMAXConverter.h"
#define NUM_MAX_PARTICLE_COLOR_KEYS 6

#ifndef NO_PARTICLES
//---------------------------------------------------------------------------
class NiMAXPSysConverter
{
public:
    NiMAXPSysConverter(TimeValue kAnimStart, TimeValue kAnimEnd);
    ~NiMAXPSysConverter() {};

    static bool IsPSys(INode* pkMaxNode);

    int Convert(
        Interface* pkIntf,
        INode* pkMaxNode,
        NiNode* pkParent, 
        ScaleInfo* pkParentScale);

    static void Finish(NiNode* pkNode);

    enum NiMAXEmitterType
    {
        EMITTER_VOLUME = 0,
        EMITTER_OBJECT,
        EMITTER_SPHERE,
        EMITTER_BOX,
        EMITTER_CYLINDER,
        EMITTER_TYPES_MAX
    };

private:
    class NiPSysDesc : public NiMemObject
    {
    public:
        NiPSysDesc();
        ~NiPSysDesc();

        bool  m_bNiPSysLoop;    
        float m_fNiPSysLoopStart;
        float m_fNiPSysLoopEnd; 
        bool  m_bNiPSysUseViewport; 
        bool  m_bNiPSysResetOnLoop;
        unsigned short  m_usNiPSysMaxAlloc;
        float m_fNiPSysRotationSpeed;
        NiFloatInterpolatorPtr m_spRotationSpeedInterp;
        NiTimeController::CycleType m_eRotationSpeedInterpCycle;
        float m_fNiPSysRotationVariation;
        NiFloatInterpolatorPtr m_spRotationVariationInterp;
        NiTimeController::CycleType m_eRotationVariationInterpCycle;
        bool m_bNiPSysBidirectionalRotation;

        NiFloatInterpolatorPtr m_spSpeedInterp;
        NiTimeController::CycleType m_eSpeedInterpCycle;
        float m_fSpeed;
        float m_fSpeedVar;
        NiFloatInterpolatorPtr m_spDeclinationInterp;
        NiTimeController::CycleType m_eDeclinationInterpCycle;
        float m_fDirDeclination;
        NiFloatInterpolatorPtr m_spDeclinationVarInterp;
        NiTimeController::CycleType m_eDeclinationVarInterpCycle;
        float m_fDirDeclinationVar;
        NiFloatInterpolatorPtr m_spPlanarAngleInterp;        
        NiTimeController::CycleType m_ePlanarAngleInterpCycle;
        float m_fDirPlanarAngle;
        NiFloatInterpolatorPtr m_spPlanarAngleVarInterp;
        NiTimeController::CycleType m_ePlanarAngleVarInterpCycle;
        float m_fDirPlanarAngleVar;

        NiFloatInterpolatorPtr m_spSizeInterp;        
        NiTimeController::CycleType m_eSizeInterpCycle;
        float m_fSize;
        float m_fSizeVar;

        float m_fEmitStart;
        float m_fEmitStop;

        NiFloatInterpolatorPtr m_spLifeSpanInterp;
        NiTimeController::CycleType m_eLifeSpanInterpCycle;
        float m_fLifeSpan;
        float m_fLifeSpanVar;
        bool  m_bUseBirthRate;
        bool  m_bSpawnOnDeath;

        NiFloatInterpolatorPtr m_spBirthRateInterp;
        NiTimeController::CycleType m_eBirthRateInterpCycle;
        float m_fBirthRate;


        NiMAXEmitterType m_eEmitterType;
        INode* m_pkMAXEmitter;

        // Mesh emitter info
        NiPSMeshEmitter::InitialVelocityType m_eInitVelocityType;
        NiPSMeshEmitter::MeshEmissionType m_eEmissionType;
        NiPoint3 m_kEmitAxis;

        // Volume emitter info
        float m_fEmitterWidth;
        float m_fEmitterHeight;
        float m_fEmitterDepth;


        bool  m_bDieOnCollide;
        bool  m_bSpawnOnCollide;
        unsigned short m_usNumGenerations;
        float m_fPercentageSpawned;
        unsigned short m_usMultiplier;
        float m_fMultiplierVar;
        float m_fSpeedChaos;
        float m_fDirChaos;

        unsigned short m_usRendererTotal;
        unsigned short m_usViewportTotal;

        // ... emitter and particle modifier data...
        float m_fGrowFor;
        float m_fFadeFor;

        // There are at most three color keys from the color age material and
        // three values from the color opacity material
        NiPSKernelColorKey* m_pkColors;
        unsigned int m_uiNumColorKeys;
        bool m_bColorAges;

        unsigned short m_usNumMeshes;
        NiAVObjectPtr* m_pspMeshes;

        float m_fSpinTime;
        float m_fSpinTimeVar;
        bool m_bRandomSpinAxis;
        NiPoint3 m_kSpinAxis;
        float m_fSpinAxisVar;

        float m_fSpinPhase;
        NiFloatInterpolatorPtr m_spSpinPhaseInterp;
        NiTimeController::CycleType m_eSpinPhaseInterpCycle;
        float m_fSpinPhaseVar;
        NiFloatInterpolatorPtr m_spSpinPhaseVarInterp;
        NiTimeController::CycleType m_eSpinPhaseVarInterpCycle;

        unsigned short m_usNumFrags;
        NiPoint3* m_pkPos;
        NiPoint3* m_pkVel;
        NiTriShapePtr* m_pspFrag;

        NiPSSimulatorCollidersStep* m_spColliderManager;
        NiPSEmitter* m_spEmitter;
        NiPSSpawner* m_pkSpawnModifier;

        bool m_bLocalCoords;    
        int m_iBoundsUpdateType;
        NiPoint3 m_kBoundPosition;
        float m_fBoundRadius;
        int m_iBoundsDynamicSkip;
        INode* m_pkPSysRoot;
    };

    int ExtractOldPSys(
        INode* pkMaxNode,
        SimpleParticle* pkPart, 
        NiPSysDesc* pkDesc);
    int ExtractPCloud(
        INode* pkMaxNode,
        Interface* pkIntf, 
        SimpleParticle* pkPart,
        NiPSysDesc* pkDesc,
        ScaleInfo* pkParentScale);
    int ExtractSuperSpray(
        Interface* pkIntf,
        SimpleParticle* pkPart, 
        NiPSysDesc* pkDesc,
        ScaleInfo* pkParentScale);
    int ExtractBlizzard(
        Interface* pkIntf,
        SimpleParticle* pkPart, 
        NiPSysDesc* pkDesc,
        ScaleInfo* pkParentScale);
    int ExtractPArray(
        INode* pkMaxNode,
        NiNode* pkNode,
        Interface* pkIntf, 
        SimpleParticle* pkPart,
        NiPSysDesc* pkDesc,
        ScaleInfo* pkParentScale);

    int ExtractForces(
        Interface* pkIntf,
        INode* pkMaxNode, 
        NiPSParticleSystem* pkPSystem,
        NiPSysDesc* pkDesc, 
        ScaleInfo* pkParentScale);

    int BuildFlake(NiMesh** ppResult, float fSize);
    
    NiPSParticleSystem* CreatePSys(
        NiPSysDesc* pkDesc,
        SimpleParticle* pkPart,
        float fParentScale,
        NiNode* pkParent);
    NiPSEmitter* BuildEmitterInfo(
        NiPSysDesc* pkDesc, 
        NiPSParticleSystem* pkPSystem);
    NiPSSpawner* BuildSpawnInfo(
        NiPSysDesc* pkDesc,
        NiPSParticleSystem* pkPSystem);
    bool BuildAgeBasedDeathInfo(
        NiPSysDesc* pkDesc, 
        NiPSParticleSystem* pkPSystem);
    bool BuildGrowFadeInfo(
        NiPSysDesc* pkDesc, 
        NiPSParticleSystem* pkPSystem);
    bool BuildColorModInfo(
        NiPSysDesc* pkDesc, 
        NiPSParticleSystem* pkPSystem);
    bool BuildMeshModInfo(
        NiPSysDesc* pkDesc, 
        NiPSParticleSystem* pkPSystem);
    bool BuildRotationInfo(
        NiPSysDesc* pkDesc, 
        NiPSParticleSystem* pkPSystem,
        NiPSEmitter* pkEmitter);

    int ExtractPlanarDeflector(
        Interface* pkIntf,
        Modifier* pkMod, 
        INode* pkMaxNode,
        NiPSParticleSystem* pkPSystem,
        NiPSysDesc* pkDesc, 
        ScaleInfo* pkParentScale);
    int ExtractSphereDeflector(
        Interface* pkIntf,
        Modifier* pkMod, 
        INode* pkMaxNode,
        NiPSParticleSystem* pkPSystem,
        NiPSysDesc* pkDesc, 
        ScaleInfo* pkParentScale);
    int ExtractGravity(
        Interface* pkIntf,
        Modifier* pkMod, 
        INode* pkMaxNode,
        NiPSParticleSystem* pkPSystem,
        NiPSysDesc* pkDesc, 
        ScaleInfo* pkParentScale);
    int ExtractWind(
        Interface* pkIntf,
        Modifier* pkMod, 
        INode* pkMaxNode,
        NiPSParticleSystem* pkPSystem,
        NiPSysDesc* pkDesc, 
        ScaleInfo* pkParentScale);
    int ExtractParticleBomb(
        Interface* pkIntf,
        Modifier* pkMod, 
        INode* pkMaxNode,
        NiPSParticleSystem* pkPSystem,
        NiPSysDesc* pkDesc, 
        ScaleInfo* pkParentScale);
    int ExtractDrag(
        Interface* pkIntf,
        Modifier* pkMod, 
        INode* pkMaxNode,
        NiPSParticleSystem* pkPSystem,
        NiPSysDesc* pkDesc, 
        ScaleInfo* pkParentScale);

    int ExtractCustomAttribs(NiPSysDesc* pkDesc, SimpleParticle* pkPart);
    static void PopulateMeshEmitter(
        NiAVObject* pkObj, 
        NiPSMeshEmitter* pkEmitter);

    TimeValue m_kAnimStart;
    TimeValue m_kAnimEnd;

    class ParticleRemapInfo : public NiMemObject
    {
    public:
        ParticleRemapInfo();
        NiPSParticleSystemPtr m_spSystem;
        INode* m_pkMAXNode;
    };

    class ParticleLinkUpInfo : public NiMemObject
    {
    public:
        ParticleLinkUpInfo();
        NiPSParticleSystemPtr m_spSystem;
        NiPSEmitterPtr m_spEmitter;
        NiPSSimulatorStepPtr m_spSimulator;
        NiPSForcePtr m_spForce;
        NiPSColliderPtr m_spCollider;
        INode* m_pkMAXNode;
    };

    class ParticleStaticBoundInfo : public NiMemObject
    {
    public:
        ParticleStaticBoundInfo();
        NiPSParticleSystemPtr m_spSystem;
        NiNodePtr m_spOldParent;
        NiBound m_kBound;
    };

    static unsigned int ms_uiTotalCount;
    static NiTPointerList<ParticleLinkUpInfo*> ms_kLinkUpList;
    static NiTPointerList<ParticleRemapInfo*> ms_kRemapList;
    static NiTPointerList<ParticleStaticBoundInfo*> ms_kStaticBoundList;

    static NiStepBoolKey* BuildActiveKeys(
        float fLoopStart,
        float fLoopEnd, 
        float fActiveStart,
        float fActiveEnd,
        unsigned int& uiNumKeys);

    NiFloatInterpolator* BuildFloatInterp(
        IParamBlock* pkParams, 
        int iParamIdx,
        NiTimeController::CycleType& eCycleType, 
        float fScalar = 1.0f,
        bool bBoundInterp = false, 
        float fStartTime = 0.0f,
        float fEndTime = 1.0f);
    NiFloatInterpolator* BuildFloatInterp(
        IParamBlock2* pkParams, 
        int iParamIdx,
        NiTimeController::CycleType& eCycleType, 
        float fScalar = 1.0f,
        bool bBoundInterp = false,
        float fStartTime = 0.0f,
        float fEndTime = 1.0f);

    void SetEmitterControllerStartAndStop(
        unsigned int& uiNumEmitterActiveKeys, 
        NiStepBoolKey*& pkEmitterActiveKeys,
        unsigned int uiNumBirthRateKeys,
        NiFloatKey* pkBirthRateKeys,
        NiFloatKey::KeyType eBirthRateKeyType);

    void MergeBirthRateTrackWithActiveKeyTrack(
        unsigned int& uiNumKeys,
        NiStepBoolKey*& pkBoolKeys, 
        unsigned int uiNumBirthRateKeys,
        NiFloatKey* pkBRKeys, 
        NiFloatKey::KeyType eBirthRateKeyType);
};
#endif
//---------------------------------------------------------------------------
#endif

