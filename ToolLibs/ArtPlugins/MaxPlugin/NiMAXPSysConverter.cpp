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

#include "MaxImmerse.h"

#include "NiMAXPSysConverter.h"
#include "NiMAXCustAttribConverter.h"
#include "NiMAXHierConverter.h"
#include "NiMAXMaterialConverter.h"
#include "NiMAXMeshConverter.h"
#include "NiUtils.h"
#include "NiMAXAnimationConverter.h"
#include <NiMeshUpdateProcess.h>

#define PDRAGMOD_CLASS_ID Class_ID(705380396, 1574462936)

#ifndef NO_PARTICLES

#define PSYS_INTERVAL_TIME 0.00001f
#define ALWAYS_UPDATE_BOUNDS 1
#define STATIC_BOUNDS        2

static float sfTimeFactor = 1.0f / (float)TIME_TICKSPERSEC;

NiTPointerList<NiMAXPSysConverter::ParticleLinkUpInfo *>
    NiMAXPSysConverter::ms_kLinkUpList;
NiTPointerList<NiMAXPSysConverter::ParticleRemapInfo *>
    NiMAXPSysConverter::ms_kRemapList;
NiTPointerList<NiMAXPSysConverter::ParticleStaticBoundInfo *>
    NiMAXPSysConverter::ms_kStaticBoundList;

unsigned int NiMAXPSysConverter::ms_uiTotalCount = 0;

extern const char* gpcMultiMtlName;

//---------------------------------------------------------------------------
NiMAXPSysConverter::NiMAXPSysConverter(
    TimeValue kAnimStart, 
    TimeValue kAnimEnd)
{
    m_kAnimStart = kAnimStart;
    m_kAnimEnd = kAnimEnd;
}
//---------------------------------------------------------------------------
bool NiMAXPSysConverter::IsPSys(INode* pkMaxNode)
{
    Class_ID eID = pkMaxNode->GetObjectRef()->ClassID();

    return ((eID == Class_ID(RAIN_CLASS_ID, 0) ||
        eID == Class_ID(SNOW_CLASS_ID, 0) ||
        eID == PCLOUD_CLASS_ID ||
        eID == SUPRSPRAY_CLASS_ID ||
        eID == BLIZZARD_CLASS_ID ||
        eID == PArray_CLASS_ID));
}
//---------------------------------------------------------------------------
// When using instanced geometry we need to remove the root transform
// of the subtree. To do so we create a new root and copy all the
// data to it.
static int ReplaceRoot(NiNodePtr* pspRoot)
{
    CHECK_MEMORY();
    NiNode* pkNewRoot;
    if (NiIsExactKindOf(NiBillboardNode, (*pspRoot)))
    {
        NiBillboardNode* pkBB;

        pkNewRoot = pkBB = NiNew NiBillboardNode;
        pkBB->SetMode(NiBillboardNode::ALWAYS_FACE_CAMERA);
    }
    else
    {
        pkNewRoot = NiNew NiNode;
    }
    if (pkNewRoot == NULL)
        return(W3D_STAT_NO_MEMORY);

    pkNewRoot->SetName("NewRoot");

    NiAVObject* pkChild;
    for (unsigned int i = 0; i < (*pspRoot)->GetArrayCount(); i++)
    {
        pkChild = (*pspRoot)->GetAt(i);
        if (pkChild)
            pkNewRoot->AttachChild((NiAVObject*) pkChild->Clone());
    }

    NiPropertyList* pkPropList = &((*pspRoot)->GetPropertyList());
    NiTListIterator kIter = pkPropList->GetHeadPos();
    while (kIter)
    {
        NiProperty* pkProperty = pkPropList->GetNext(kIter);

        if(pkProperty)
            pkNewRoot->AttachProperty(pkProperty);
   }

    *pspRoot = pkNewRoot;

    // don't cleanup or remove pOldRoot because it is in the 
    // Node Hash table and messing w/ it could cause trouble
    // elsewhere. It will get removed when the hash table is 
    // freed

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
NiMAXPSysConverter::NiPSysDesc::NiPSysDesc()
{
    CHECK_MEMORY();
    m_spSpeedInterp = NULL;
    m_fSpeed = 0.0f;
    m_fSpeedVar = 0.0f;
    m_fDirDeclination = 0.0f;
    m_fDirDeclinationVar = 0.0f;
    m_fDirPlanarAngle = 0.0f;
    m_fDirPlanarAngleVar = 0.0f;

    m_fSize = 0.0f;
    m_fSizeVar = 0.0f;

    m_fEmitStart = 0.0f;
    m_fEmitStop = 0.0f;

    m_spBirthRateInterp = NULL;
    m_fBirthRate = 0.0f;
    m_fLifeSpan = 0.0f;
    m_fLifeSpanVar = 0.0f;
    m_bUseBirthRate = false;
    m_bSpawnOnDeath = false;

    m_eEmitterType = EMITTER_VOLUME;
    m_pkMAXEmitter = 0;

    m_eInitVelocityType = NiPSMeshEmitter::VELOCITY_USE_NORMALS; 
    m_eEmissionType = NiPSMeshEmitter::EMIT_FROM_VERTICES;
    m_kEmitAxis = NiPoint3::UNIT_X;

    m_fEmitterWidth = 0.0f;
    m_fEmitterHeight = 0.0f;
    m_fEmitterDepth = 0.0f;

    
    m_bDieOnCollide = false;
    m_bSpawnOnCollide = false;
    m_usNumGenerations = 0;
    m_fPercentageSpawned = 0.0f;
    m_usMultiplier = 0;
    m_fSpeedChaos = 0.0f;
    m_fDirChaos = 0.0f;

    m_usRendererTotal = 0;
    m_usViewportTotal = 0;

    m_fGrowFor = 0.0f;
    m_fFadeFor = 0.0f;
   
    m_uiNumColorKeys = 0;
    m_bColorAges = false;

    m_usNumMeshes = 0;
    m_pspMeshes = NULL;

    m_fSpinTime = 0.0f;
    m_fSpinTimeVar = 0.0f;
    m_bRandomSpinAxis = false;
    m_kSpinAxis = NiPoint3::ZERO;
    m_fSpinAxisVar = 0.0f;

    m_fSpinPhase = 0.0f;
    m_fSpinPhaseVar = 0.0f;

    m_usNumFrags = 0;
    m_pkPos = 0;
    m_pkVel = 0;
    m_pspFrag = NULL;

    m_spColliderManager = NULL;
    m_spEmitter = NULL;

    m_bLocalCoords = false; 
    m_iBoundsUpdateType = 1;
    m_kBoundPosition = NiPoint3(0.0f,0.0f,0.0f);
    m_fBoundRadius = 1.0f;
    m_iBoundsDynamicSkip = 0;
    m_pkPSysRoot = NULL;
    m_eDeclinationInterpCycle = NiTimeController::CLAMP;
    m_eSpeedInterpCycle = NiTimeController::CLAMP;
    m_ePlanarAngleInterpCycle = NiTimeController::CLAMP;
    m_eSizeInterpCycle = NiTimeController::CLAMP;
    m_eLifeSpanInterpCycle = NiTimeController::CLAMP;
    m_eBirthRateInterpCycle = NiTimeController::CLAMP;

    m_fNiPSysRotationSpeed = 0.0f;
    m_eRotationSpeedInterpCycle = NiTimeController::CLAMP;
    m_fNiPSysRotationVariation = 0.0f;
    m_eRotationVariationInterpCycle = NiTimeController::CLAMP;
    m_bNiPSysBidirectionalRotation = false;
    m_eDeclinationVarInterpCycle = NiTimeController::CLAMP;
    m_ePlanarAngleVarInterpCycle = NiTimeController::CLAMP;
    m_eSpinPhaseInterpCycle = NiTimeController::CLAMP;
    m_eSpinPhaseVarInterpCycle = NiTimeController::CLAMP;
}
//---------------------------------------------------------------------------
NiMAXPSysConverter::NiPSysDesc::~NiPSysDesc()
{
    CHECK_MEMORY();
    unsigned short i;
    for (i = 0; i < m_usNumMeshes; i++)
    {
        m_pspMeshes[i] = 0;
    }
    NiDelete [] m_pspMeshes;

    for (i = 0; i < m_usNumFrags; i++)
    {
        m_pspFrag[i] = 0;
    }
    NiDelete[] m_pkPos;
    NiDelete[] m_pkVel;
    NiFree(m_pspFrag);
}
//---------------------------------------------------------------------------
NiMAXPSysConverter::ParticleLinkUpInfo::ParticleLinkUpInfo()
{
    m_spSystem = NULL;
    m_spEmitter = NULL;
    m_spCollider = NULL;
    m_pkMAXNode = NULL;
    m_spSimulator = NULL;
    m_spForce = NULL;
}

//---------------------------------------------------------------------------
NiMAXPSysConverter::ParticleRemapInfo::ParticleRemapInfo()
{
    m_spSystem = NULL;
    m_pkMAXNode = NULL;
}
//---------------------------------------------------------------------------
NiMAXPSysConverter::ParticleStaticBoundInfo::ParticleStaticBoundInfo()
{
    m_spSystem = NULL;
    m_spOldParent = NULL;
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractSuperSpray(
    Interface* pkIntf, 
    SimpleParticle* pkPart, 
    NiPSysDesc* pkDesc, 
    ScaleInfo* pkParentScale)
{
    CHECK_MEMORY();
    CommonParticle* pkCommon = (CommonParticle*) pkPart;
    IParamBlock* pkPBlock = pkPart->pblock;
    Class_ID eID = pkPart->ClassID();

    // Basic Parameters Rollout
    float fDirDeclination, fDirDeclinationVar;
    Interval intForever = FOREVER;
    pkPBlock->GetValue(PBBLIZ_OFFAXIS, 0, fDirDeclination, intForever);
    pkPBlock->GetValue(PBBLIZ_AXISSPREAD, 0, fDirDeclinationVar, intForever);
    float fDirPlanarAngle, fDirPlanarAngleVar;
    pkPBlock->GetValue(PBBLIZ_OFFPLANE, 0, fDirPlanarAngle, intForever);
    pkPBlock->GetValue(PBBLIZ_PLANESPREAD, 0, fDirPlanarAngleVar, intForever);
    float fWidth;
    pkPBlock->GetValue(PBBLIZ_EMITRWID, 0, fWidth, intForever);
    int iShowEmitter, iViewportDisplayType;
    pkPBlock->GetValue(PBBLIZ_EMITRHID, 0, iShowEmitter, intForever);
    pkPBlock->GetValue(PBBLIZ_VIEWPORTSHOWS, 0, iViewportDisplayType, 
        intForever);
    float fPercentageShown;
    pkPBlock->GetValue(PBBLIZ_DISPLAYPORTION, 0, fPercentageShown, intForever);
                        
    // Particle Generation Rollout
    int iLimitUsingTotal, iBirthRate, iTotalParticles;
    pkPBlock->GetValue(PBBLIZ_BIRTHMETHOD, 0, iLimitUsingTotal, intForever);
    pkPBlock->GetValue(PBBLIZ_PBIRTHRATE, 0, iBirthRate, intForever);
    
    pkPBlock->GetValue(PBBLIZ_PTOTALNUMBER, 0, iTotalParticles, intForever);
    float fSpeed, fSpeedVar;
    pkPBlock->GetValue(PBBLIZ_SPEED, 0, fSpeed, intForever);
    pkPBlock->GetValue(PBBLIZ_SPEEDVAR, 0, fSpeedVar, intForever);
    TimeValue kEmitStart, kEmitStop;
    pkPBlock->GetValue(PBBLIZ_EMITSTART, 0, kEmitStart, intForever);
    pkPBlock->GetValue(PBBLIZ_EMITSTOP, 0, kEmitStop, intForever);
    TimeValue kDisplayUntil, kLifeSpan, kLifeSpanVar;
    pkPBlock->GetValue(PBBLIZ_DISPUNTIL, 0, kDisplayUntil, intForever);
    pkPBlock->GetValue(PBBLIZ_LIFE, 0, kLifeSpan, intForever);
    pkPBlock->GetValue(PBBLIZ_LIFEVAR, 0, kLifeSpanVar, intForever);
    int iSubFrameTime, iSubFrameTrans, iSubFrameRot;
    pkPBlock->GetValue(PBBLIZ_SUBFRAMETIME, 0, iSubFrameTime, intForever);
    pkPBlock->GetValue(PBBLIZ_SUBFRAMEMOVE, 0, iSubFrameTrans, intForever);
    pkPBlock->GetValue(PBBLIZ_SUBFRAMEROT, 0, iSubFrameRot, intForever);
    float fSize, fSizeVar;
    pkPBlock->GetValue(PBBLIZ_SIZE, 0, fSize, intForever);
    pkPBlock->GetValue(PBBLIZ_SIZEVAR, 0, fSizeVar, intForever);
    TimeValue kGrowFor, kFadeFor;
    pkPBlock->GetValue(PBBLIZ_GROWTIME, 0, kGrowFor, intForever);
    pkPBlock->GetValue(PBBLIZ_FADETIME, 0, kFadeFor, intForever);
    int iRandSeed;
    pkPBlock->GetValue(PBBLIZ_RNDSEED, 0, iRandSeed, intForever);

    // Particle Type Rollout
    int iParticleType, iStandardParticleType;
    pkPBlock->GetValue(PBBLIZ_PARTICLECLASS, 0, iParticleType, intForever);
    pkPBlock->GetValue(PBBLIZ_PARTICLETYPE, 0, iStandardParticleType, 
        intForever);
    float fMetaTension, fMetaTensionVar;
    pkPBlock->GetValue(PBBLIZ_METATENSION, 0, fMetaTension, intForever);
    pkPBlock->GetValue(PBBLIZ_METATENSIONVAR, 0, fMetaTensionVar, intForever);
    float fMetaCourseness, fMetaViewportCourseness;
    pkPBlock->GetValue(PBBLIZ_METACOURSE, 0, fMetaCourseness, intForever);
    pkPBlock->GetValue(PBBLIZ_METACOURSEV, 0, fMetaViewportCourseness, 
        intForever);
    int iMetaAutoCoarseness;
    pkPBlock->GetValue(PBBLIZ_METAAUTOCOARSE, 0, iMetaAutoCoarseness, 
        intForever);
    int iInstancedSubtree;
    pkPBlock->GetValue(PBBLIZ_USESUBTREE, 0, iInstancedSubtree, intForever);
    int iInstancedOffsetType;
    pkPBlock->GetValue(PBBLIZ_ANIMATIONOFFSET, 0, iInstancedOffsetType, 
        intForever);
    float fInstancedOffsetAmount;
    pkPBlock->GetValue(PBBLIZ_OFFSETAMOUNT, 0, fInstancedOffsetAmount, 
        intForever);
    int iMaterialMappingType;
    pkPBlock->GetValue(PBBLIZ_MAPPINGTYPE, 0, iMaterialMappingType, intForever);
    TimeValue kMappingTime;
    pkPBlock->GetValue(PBBLIZ_MAPPINGTIME, 0, kMappingTime, intForever);
    float fMappingDistance;
    pkPBlock->GetValue(PBBLIZ_MAPPINGDIST, 0, fMappingDistance, intForever);

    // Rotation And Collision Rollout
    TimeValue kSpinTime;
    pkPBlock->GetValue(PBBLIZ_SPINTIME, 0, kSpinTime, intForever);
    float fSpinTimeVar;
    pkPBlock->GetValue(PBBLIZ_SPINTIMEVAR, 0, fSpinTimeVar, intForever);
    float fSpinPhase, fSpinPhaseVar;
    pkPBlock->GetValue(PBBLIZ_SPINPHASE, 0, fSpinPhase, intForever);
    pkPBlock->GetValue(PBBLIZ_SPINPHASEVAR, 0, fSpinPhaseVar, intForever);
    int iSpinAxisType;
    pkPBlock->GetValue(PBBLIZ_SPINAXISTYPE, 0, iSpinAxisType, intForever);
    float fSpinStretch;
    pkPBlock->GetValue(PBBLIZ_STRETCH, 0, fSpinStretch, intForever);
    NiPoint3 kSpinAxis;
    pkPBlock->GetValue(PBBLIZ_SPINAXISX, 0, kSpinAxis.x, intForever);
    pkPBlock->GetValue(PBBLIZ_SPINAXISY, 0, kSpinAxis.y, intForever);
    pkPBlock->GetValue(PBBLIZ_SPINAXISZ, 0, kSpinAxis.z, intForever);
    float fSpinAxisVar;
    pkPBlock->GetValue(PBBLIZ_SPINAXISVAR, 0, fSpinAxisVar, intForever);
    int iParticleCollisionOn;
    pkPBlock->GetValue(PBBLIZ_IPCOLLIDE_ON, 0, iParticleCollisionOn, 
        intForever);
    int iParticleCollisionSteps;
    pkPBlock->GetValue(PBBLIZ_IPCOLLIDE_STEPS, 0, iParticleCollisionSteps, 
        intForever);
    float fParticleCollisionBounce, fParticleCollisionBounceVar;
    pkPBlock->GetValue(PBBLIZ_IPCOLLIDE_BOUNCE, 0, fParticleCollisionBounce, 
        intForever);
    pkPBlock->GetValue(PBBLIZ_IPCOLLIDE_BOUNCEVAR, 0, 
        fParticleCollisionBounceVar, intForever);

    // Object Motion Inheritance Rollout
    float fOMIInfluence, fOMIMultiplier, fOMIMultiplierVar;
    pkPBlock->GetValue(PBBLIZ_EMITVINFL, 0, fOMIInfluence, intForever);
    pkPBlock->GetValue(PBBLIZ_EMITVMULT, 0, fOMIMultiplier, intForever);
    pkPBlock->GetValue(PBBLIZ_EMITVMULTVAR, 0, fOMIMultiplierVar, intForever);

    // Bubble Motion Rollout
    float fBubbleAmp, fBubbleAmpVar;
    pkPBlock->GetValue(PBBLIZ_BUBLAMP, 0, fBubbleAmp, intForever);
    pkPBlock->GetValue(PBBLIZ_BUBLAMPVAR, 0, fBubbleAmpVar, intForever);
    TimeValue kBubblePeriod;
    pkPBlock->GetValue(PBBLIZ_BUBLPER, 0, kBubblePeriod, intForever);
    float fBubblePeriodVar;
    pkPBlock->GetValue(PBBLIZ_BUBLPERVAR, 0, fBubblePeriodVar, intForever);
    float fBubblePhase, fBubblePhaseVar;
    pkPBlock->GetValue(PBBLIZ_BUBLPHAS, 0, fBubblePhase, intForever);
    pkPBlock->GetValue(PBBLIZ_BUBLPHASVAR, 0, fBubblePhaseVar, intForever);

    // Particle Spawn Rollout
    int iSpawnType;
    pkPBlock->GetValue(PBBLIZ_SPAWNTYPE, 0, iSpawnType, intForever);
    TimeValue kPersist;
    pkPBlock->GetValue(PBBLIZ_SSSPAWNDIEAFTER, 0, kPersist, intForever);
    float fPersistVar;
    pkPBlock->GetValue(PBBLIZ_SSSPAWNDIEAFTERVAR, 0, fPersistVar, intForever);
    int iNumSpawns;
    pkPBlock->GetValue(PBBLIZ_SPAWNGENS, 0, iNumSpawns, intForever);
    int iSpawnPercent;
    pkPBlock->GetValue(PBBLIZ_SPAWNPERCENT, 0, iSpawnPercent, intForever);
    int iSpawnMultiplier;
    pkPBlock->GetValue(PBBLIZ_SPAWNCOUNT, 0, iSpawnMultiplier, intForever);
    float fSpawnMultiplierVar;
    pkPBlock->GetValue(PBBLIZ_SPAWNMULTVAR, 0, fSpawnMultiplierVar, intForever);
    float fSpawnDirChaos;
    pkPBlock->GetValue(PBBLIZ_SPAWNDIRCHAOS, 0, fSpawnDirChaos, intForever);
    float fSpawnSpeedChaos;
    pkPBlock->GetValue(PBBLIZ_SPAWNSPEEDCHAOS, 0, fSpawnSpeedChaos, intForever);
    int iSpawnSpeedSign;
    pkPBlock->GetValue(PBBLIZ_SPAWNSPEEDSIGN, 0, iSpawnSpeedSign, intForever);
    int iSpawnInheritSpeed;
    pkPBlock->GetValue(PBBLIZ_SPAWNINHERITV, 0, iSpawnInheritSpeed, intForever);
    int iSpawnFixedSpeed;
    pkPBlock->GetValue(PBBLIZ_SPAWNSPEEDFIXED, 0, iSpawnFixedSpeed, intForever);
    float fSpawnScaleChaos;
    pkPBlock->GetValue(PBBLIZ_SPAWNSCALECHAOS, 0, fSpawnScaleChaos, intForever);
    int iSpawnScaleSign;
    pkPBlock->GetValue(PBBLIZ_SPAWNSCALESIGN, 0, iSpawnScaleSign, intForever);
    int iSpawnFixedScale;
    pkPBlock->GetValue(PBBLIZ_SPAWNSCALEFIXED, 0, iSpawnFixedScale, intForever);
    TimeValue kSpawnLifeValue;
    pkPBlock->GetValue(PBBLIZ_SPAWNLIFEVLUE, 0, kSpawnLifeValue, intForever);

    // Unknown
    int iCustomMtl, iPCNotDraft;
    pkPBlock->GetValue(PBBLIZ_CUSTOMMTL, 0, iCustomMtl, intForever);
    pkPBlock->GetValue(PBBLIZ_SSNOTDRAFT, 0, iPCNotDraft, intForever);

    // Set up parameters
    pkDesc->m_fSpeed = fSpeed * GetFrameRate();
    // we must account for the fact that NiParticle calculates speed by
    // taking Speed + SpeedVar * (UnitRandom - 0.5);
    pkDesc->m_fSpeedVar = fSpeedVar * pkDesc->m_fSpeed * 2.0f;

    pkDesc->m_fDirDeclination = fDirDeclination;
    pkDesc->m_fDirDeclinationVar = fDirDeclinationVar;
    pkDesc->m_fDirPlanarAngle = fDirPlanarAngle;
    pkDesc->m_fDirPlanarAngleVar = fDirPlanarAngleVar;

    pkDesc->m_fSize = fSize;
    // account for SizeVar being in units (not %) in engine
    pkDesc->m_fSizeVar = fSizeVar * pkDesc->m_fSize;

    pkDesc->m_fEmitStart = kEmitStart * sfTimeFactor;

    pkDesc->m_fEmitStop = kEmitStop * sfTimeFactor;
    
    if (pkDesc->m_bNiPSysUseViewport)
        pkDesc->m_fBirthRate = fPercentageShown * iBirthRate * GetFrameRate();
    else 
        pkDesc->m_fBirthRate = (float)iBirthRate * GetFrameRate();

    pkDesc->m_fLifeSpan = kLifeSpan * sfTimeFactor;
    // we must account for the fact that NiParticle calculates lifespan by
    // taking Lifespan + LifeSpanVar * (UnitRandom - 0.5);
    pkDesc->m_fLifeSpanVar = kLifeSpanVar * sfTimeFactor * 2.0f;
    pkDesc->m_bUseBirthRate = true;
    pkDesc->m_bSpawnOnDeath = (iSpawnType == 3);

    if (pkDesc->m_fEmitStart == pkDesc->m_fEmitStop)
    {
        pkDesc->m_fEmitStop = pkDesc->m_fEmitStart + PSYS_INTERVAL_TIME;
        if (iLimitUsingTotal == 0)
            iTotalParticles = iBirthRate;

        iLimitUsingTotal = 1;
    }
    
    if (iLimitUsingTotal != 0)
    {
        float fLifetime = pkDesc->m_fEmitStop - pkDesc->m_fEmitStart;

        if (pkDesc->m_bNiPSysUseViewport)
        {
            pkDesc->m_fBirthRate = fPercentageShown * iTotalParticles /  
                fLifetime;
        }
        else
        {
            pkDesc->m_fBirthRate = iTotalParticles /  fLifetime;
        }
    }

    if (IsTrue(NiMAXOptions::GetValue(NI_USEAREASUPERSPRAY)))
    {
        pkDesc->m_fEmitterWidth = fWidth;
        pkDesc->m_fEmitterHeight = fWidth;
    }
    else
    {
        pkDesc->m_fEmitterWidth = 0.0f;
        pkDesc->m_fEmitterHeight = 0.0f;
    }

    pkDesc->m_fEmitterDepth = 0.0f;

    pkDesc->m_bDieOnCollide = (iSpawnType == 1 || iSpawnType == 2);
    pkDesc->m_bSpawnOnCollide = (iSpawnType == 2);
    pkDesc->m_usNumGenerations = (unsigned short)(iSpawnType == 0 ? 0 : iNumSpawns);
    pkDesc->m_fPercentageSpawned = (float)iSpawnPercent * 0.01f;
    pkDesc->m_usMultiplier = (unsigned short)iSpawnMultiplier;
    pkDesc->m_fMultiplierVar = fSpawnMultiplierVar;
    pkDesc->m_fSpeedChaos = fSpawnSpeedChaos * 0.01f;
    switch (iSpawnSpeedSign)
    {
        case 0:
            pkDesc->m_fSpeedChaos *= -1.0f;
            break;
        case 1:
        case 2:
            break;
    };
    pkDesc->m_fDirChaos = fSpawnDirChaos;

    pkDesc->m_usViewportTotal = (unsigned short)
        (fPercentageShown * iTotalParticles);
    pkDesc->m_usRendererTotal = (unsigned short)
        (iTotalParticles);

    pkDesc->m_fGrowFor = kGrowFor * sfTimeFactor;
    pkDesc->m_fFadeFor = kFadeFor * sfTimeFactor;

    pkDesc->m_fSpinTime = kSpinTime * sfTimeFactor;
    pkDesc->m_fSpinTimeVar = fSpinTimeVar;
    pkDesc->m_bRandomSpinAxis = (iSpinAxisType != 2);
    pkDesc->m_kSpinAxis = kSpinAxis;
    pkDesc->m_fSpinAxisVar = fSpinAxisVar;
    pkDesc->m_fSpinPhase = fSpinPhase;
    pkDesc->m_fSpinPhaseVar = fSpinPhaseVar;

    if (iParticleType == 2 && pkCommon->custnode)
    {
        // Instanced particles
        NiMAXHierConverter kHier(m_kAnimStart, m_kAnimEnd);

        unsigned int uiNumMutations = pkCommon->nlist.Count();

        pkDesc->m_usNumMeshes = (unsigned short)(uiNumMutations + 1);

        pkDesc->m_pspMeshes = NiNew NiAVObjectPtr[uiNumMutations + 1];

        NiNodePtr spResult;
        for (unsigned int i = 0; i < uiNumMutations; i++)
        {
            kHier.BuildNode(pkIntf, pkCommon->nlist[i], 
                &spResult, pkParentScale, false, false);
            ReplaceRoot(&spResult);
            pkDesc->m_pspMeshes[i + 1] = spResult;
        }

        kHier.BuildNode(pkIntf, pkCommon->custnode, &spResult, 
            pkParentScale, false, false);
        ReplaceRoot(&spResult);
        pkDesc->m_pspMeshes[0] = spResult;
    }

    // Extract animations
    if (iLimitUsingTotal == 0)
    {
        float fBRScalar = 1.0f;
        if (pkDesc->m_bNiPSysUseViewport)
            fBRScalar = fPercentageShown * GetFrameRate();
        else 
            fBRScalar = (float) GetFrameRate();

        pkDesc->m_spBirthRateInterp = BuildFloatInterp(pkPBlock, 
            PBBLIZ_PBIRTHRATE, pkDesc->m_eBirthRateInterpCycle, fBRScalar);
    }
    pkDesc->m_spLifeSpanInterp = BuildFloatInterp(pkPBlock, PBBLIZ_LIFE, 
        pkDesc->m_eLifeSpanInterpCycle, sfTimeFactor);
    pkDesc->m_spSpeedInterp = BuildFloatInterp(pkPBlock, PBBLIZ_SPEED, 
        pkDesc->m_eSpeedInterpCycle, (float) GetFrameRate());
    pkDesc->m_spSizeInterp = BuildFloatInterp(pkPBlock, PBBLIZ_SIZE,
        pkDesc->m_eSizeInterpCycle);
    pkDesc->m_spPlanarAngleInterp = BuildFloatInterp(pkPBlock, 
        PBBLIZ_OFFPLANE, pkDesc->m_ePlanarAngleInterpCycle);
    pkDesc->m_spPlanarAngleVarInterp = BuildFloatInterp(pkPBlock, 
        PBBLIZ_PLANESPREAD, pkDesc->m_ePlanarAngleVarInterpCycle);
    pkDesc->m_spDeclinationInterp = BuildFloatInterp(pkPBlock, 
        PBBLIZ_OFFAXIS, pkDesc->m_eDeclinationInterpCycle);
    pkDesc->m_spDeclinationVarInterp = BuildFloatInterp(pkPBlock,
        PBBLIZ_AXISSPREAD, pkDesc->m_eDeclinationVarInterpCycle);
    pkDesc->m_spSpinPhaseInterp = BuildFloatInterp(pkPBlock, PBBLIZ_SPINPHASE,
        pkDesc->m_eSpinPhaseInterpCycle);
    pkDesc->m_spSpinPhaseVarInterp = BuildFloatInterp(pkPBlock, 
        PBBLIZ_SPINPHASEVAR, pkDesc->m_eSpinPhaseVarInterpCycle);

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractBlizzard(
    Interface* pkIntf, 
    SimpleParticle* pkPart, 
    NiPSysDesc* pkDesc, 
    ScaleInfo* pkParentScale)
{
    CHECK_MEMORY();
    CommonParticle* pkCommon = (CommonParticle*) pkPart;
    IParamBlock* pkPBlock = pkPart->pblock;
    Class_ID eID = pkPart->ClassID();
    Interval intForever = FOREVER;

    // Basic Parameters Rollout
    float fTumble, fTumbleRate;
    pkPBlock->GetValue(PBBLIZ_TUMBLE, 0, fTumble, intForever);
    pkPBlock->GetValue(PBBLIZ_TUMBLERATE, 0, fTumbleRate, intForever);
    float fWidth, fHeight;
    pkPBlock->GetValue(PBBLIZ_EMITRWID, 0, fWidth, intForever);
    pkPBlock->GetValue(PBBLIZ_EMITRLENGTH, 0, fHeight, intForever);
    int iShowEmitter, iViewportDisplayType;
    pkPBlock->GetValue(PBBLIZ_EMITRHID, 0, iShowEmitter, intForever);
    pkPBlock->GetValue(PBBLIZ_VIEWPORTSHOWS, 0, iViewportDisplayType, 
        intForever);
    float fPercentageShown;
    pkPBlock->GetValue(PBBLIZ_DISPLAYPORTION, 0, fPercentageShown, intForever);
                        
    // Particle Generation Rollout
    int iLimitUsingTotal, iBirthRate, iTotalParticles;
    pkPBlock->GetValue(PBBLIZ_BIRTHMETHOD, 0, iLimitUsingTotal, intForever);
    pkPBlock->GetValue(PBBLIZ_PBIRTHRATE, 0, iBirthRate, intForever);
    pkPBlock->GetValue(PBBLIZ_PTOTALNUMBER, 0, iTotalParticles, intForever);
    float fSpeed, fSpeedVar;
    pkPBlock->GetValue(PBBLIZ_SPEED, 0, fSpeed, intForever);
    pkPBlock->GetValue(PBBLIZ_SPEEDVAR, 0, fSpeedVar, intForever);
    TimeValue kEmitStart, kEmitStop;
    pkPBlock->GetValue(PBBLIZ_EMITSTART, 0, kEmitStart, intForever);
    pkPBlock->GetValue(PBBLIZ_EMITSTOP, 0, kEmitStop, intForever);
    TimeValue kDisplayUntil, kLifeSpan, kLifeSpanVar;
    pkPBlock->GetValue(PBBLIZ_DISPUNTIL, 0, kDisplayUntil, intForever);
    pkPBlock->GetValue(PBBLIZ_LIFE, 0, kLifeSpan, intForever);
    pkPBlock->GetValue(PBBLIZ_LIFEVAR, 0, kLifeSpanVar, intForever);
    int iSubFrameTime, iSubFrameTrans, iSubFrameRot;
    pkPBlock->GetValue(PBBLIZ_SUBFRAMETIME, 0, iSubFrameTime, intForever);
    pkPBlock->GetValue(PBBLIZ_SUBFRAMEMOVE, 0, iSubFrameTrans, intForever);
    pkPBlock->GetValue(PBBLIZ_SUBFRAMEROT2, 0, iSubFrameRot, intForever);
    float fSize, fSizeVar;
    pkPBlock->GetValue(PBBLIZ_SIZE, 0, fSize, intForever);
    pkPBlock->GetValue(PBBLIZ_SIZEVAR, 0, fSizeVar, intForever);
    TimeValue kGrowFor, kFadeFor;
    pkPBlock->GetValue(PBBLIZ_GROWTIME, 0, kGrowFor, intForever);
    pkPBlock->GetValue(PBBLIZ_FADETIME, 0, kFadeFor, intForever);
    int iRandSeed;
    pkPBlock->GetValue(PBBLIZ_RNDSEED, 0, iRandSeed, intForever);

    // Particle Type Rollout
    int iParticleType, iStandardParticleType;
    pkPBlock->GetValue(PBBLIZ_PARTICLECLASS, 0, iParticleType, intForever);
    pkPBlock->GetValue(PBBLIZ_PARTICLETYPE, 0, iStandardParticleType, 
        intForever);
    float fMetaTension, fMetaTensionVar;
    pkPBlock->GetValue(PBBLIZ_METATENSION, 0, fMetaTension, intForever);
    pkPBlock->GetValue(PBBLIZ_METATENSIONVAR, 0, fMetaTensionVar, intForever);
    float fMetaCourseness, fMetaViewportCourseness;
    pkPBlock->GetValue(PBBLIZ_METACOURSE, 0, fMetaCourseness, intForever);
    pkPBlock->GetValue(PBBLIZ_METACOURSEVB, 0, fMetaViewportCourseness, 
        intForever);
    int iMetaAutoCoarseness;
    pkPBlock->GetValue(PBBLIZ_METAAUTOCOARSE, 0, iMetaAutoCoarseness, 
        intForever);
    int iInstancedSubtree;
    pkPBlock->GetValue(PBBLIZ_USESUBTREE, 0, iInstancedSubtree, intForever);
    int iInstancedOffsetType;
    pkPBlock->GetValue(PBBLIZ_ANIMATIONOFFSET, 0, iInstancedOffsetType, 
        intForever);
    float fInstancedOffsetAmount;
    pkPBlock->GetValue(PBBLIZ_OFFSETAMOUNT, 0, fInstancedOffsetAmount, 
        intForever);
    int iEmitterFitPlanar;
    pkPBlock->GetValue(PBBLIZ_EMITMAP, 0, iEmitterFitPlanar, intForever);
    int iMaterialMappingType;
    pkPBlock->GetValue(PBBLIZ_MAPPINGTYPE, 0, iMaterialMappingType, intForever);
    TimeValue kMappingTime;
    pkPBlock->GetValue(PBBLIZ_MAPPINGTIME, 0, kMappingTime, intForever);
    float fMappingDistance;
    pkPBlock->GetValue(PBBLIZ_MAPPINGDIST, 0, fMappingDistance, intForever);

    // Rotation And Collision Rollout
    TimeValue kSpinTime;
    pkPBlock->GetValue(PBBLIZ_SPINTIME, 0, kSpinTime, intForever);
    float fSpinTimeVar;
    pkPBlock->GetValue(PBBLIZ_SPINTIMEVAR, 0, fSpinTimeVar, intForever);
    float fSpinPhase, fSpinPhaseVar;
    pkPBlock->GetValue(PBBLIZ_SPINPHASE, 0, fSpinPhase, intForever);
    pkPBlock->GetValue(PBBLIZ_SPINPHASEVAR, 0, fSpinPhaseVar, intForever);
    int iSpinAxisType;
    pkPBlock->GetValue(PBBLIZ_SPINAXISTYPE, 0, iSpinAxisType, intForever);
    float fSpinStretch;
    pkPBlock->GetValue(PBBLIZ_STRETCH, 0, fSpinStretch, intForever);
    NiPoint3 kSpinAxis;
    pkPBlock->GetValue(PBBLIZ_SPINAXISX, 0, kSpinAxis.x, intForever);
    pkPBlock->GetValue(PBBLIZ_SPINAXISY, 0, kSpinAxis.y, intForever);
    pkPBlock->GetValue(PBBLIZ_SPINAXISZ, 0, kSpinAxis.z, intForever);
    float fSpinAxisVar;
    pkPBlock->GetValue(PBBLIZ_SPINAXISVAR, 0, fSpinAxisVar, intForever);
    int iParticleCollisionOn;
    pkPBlock->GetValue(PBBLIZ_BLIPCOLLIDE_ON, 0, iParticleCollisionOn, 
        intForever);
    int iParticleCollisionSteps;
    pkPBlock->GetValue(PBBLIZ_BLIPCOLLIDE_STEPS, 0, iParticleCollisionSteps, 
        intForever);
    float fParticleCollisionBounce, fParticleCollisionBounceVar;
    pkPBlock->GetValue(PBBLIZ_BLIPCOLLIDE_BOUNCE, 0, fParticleCollisionBounce,
        intForever);
    pkPBlock->GetValue(PBBLIZ_BLIPCOLLIDE_BOUNCEVAR, 0, 
        fParticleCollisionBounceVar, intForever);

    // Object Motion Inheritance Rollout
    float fOMIInfluence, fOMIMultiplier, fOMIMultiplierVar;
    pkPBlock->GetValue(PBBLIZ_EMITVINFL, 0, fOMIInfluence, intForever);
    pkPBlock->GetValue(PBBLIZ_EMITVMULT, 0, fOMIMultiplier, intForever);
    pkPBlock->GetValue(PBBLIZ_EMITVMULTVAR, 0, fOMIMultiplierVar, intForever);

    // Particle Spawn Rollout
    int iSpawnType;
    pkPBlock->GetValue(PBBLIZ_SPAWNTYPE, 0, iSpawnType, intForever);
    TimeValue kPersist;
    pkPBlock->GetValue(PBBLIZ_BLSPAWNDIEAFTER, 0, kPersist, intForever);
    float fPersistVar;
    pkPBlock->GetValue(PBBLIZ_BLSPAWNDIEAFTERVAR, 0, fPersistVar, intForever);
    int iNumSpawns;
    pkPBlock->GetValue(PBBLIZ_SPAWNGENS, 0, iNumSpawns, intForever);
    int iSpawnPercent;
    pkPBlock->GetValue(PBBLIZ_SPAWNPERCENT2, 0, iSpawnPercent, intForever);
    int iSpawnMultiplier;
    pkPBlock->GetValue(PBBLIZ_SPAWNCOUNT, 0, iSpawnMultiplier, intForever);
    float fSpawnMultiplierVar;
    pkPBlock->GetValue(PBBLIZ_SPAWNMULTVAR2, 0, fSpawnMultiplierVar, intForever);
    float fSpawnDirChaos;
    pkPBlock->GetValue(PBBLIZ_SPAWNDIRCHAOS, 0, fSpawnDirChaos, intForever);
    float fSpawnSpeedChaos;
    pkPBlock->GetValue(PBBLIZ_SPAWNSPEEDCHAOS, 0, fSpawnSpeedChaos, intForever);
    int iSpawnSpeedSign;
    pkPBlock->GetValue(PBBLIZ_SPAWNSPEEDSIGN, 0, iSpawnSpeedSign, intForever);
    int iSpawnInheritSpeed;
    pkPBlock->GetValue(PBBLIZ_SPAWNINHERITV, 0, iSpawnInheritSpeed, intForever);
    int iSpawnFixedSpeed;
    pkPBlock->GetValue(PBBLIZ_SPAWNSPEEDFIXED, 0, iSpawnFixedSpeed, intForever);
    float fSpawnScaleChaos;
    pkPBlock->GetValue(PBBLIZ_SPAWNSCALECHAOS, 0, fSpawnScaleChaos, intForever);
    int iSpawnScaleSign;
    pkPBlock->GetValue(PBBLIZ_SPAWNSCALESIGN, 0, iSpawnScaleSign, intForever);
    int iSpawnFixedScale;
    pkPBlock->GetValue(PBBLIZ_SPAWNSCALEFIXED, 0, iSpawnFixedScale, intForever);
    TimeValue kSpawnLifeValue;
    pkPBlock->GetValue(PBBLIZ_SPAWNLIFEVLUE, 0, kSpawnLifeValue, intForever);

    // Unknown
    int iCustomMtl, iBLNotDraft;
    pkPBlock->GetValue(PBBLIZ_CUSTOMMTL2, 0, iCustomMtl, intForever);
    pkPBlock->GetValue(PBBLIZ_BLNOTDRAFT, 0, iBLNotDraft, intForever);

    // Set up parameters
    pkDesc->m_fSpeed = fSpeed * GetFrameRate();
    // we must account for the fact that NiParticle calculates speed by
    // taking Speed + SpeedVar * (UnitRandom - 0.5);
    pkDesc->m_fSpeedVar = fSpeedVar * pkDesc->m_fSpeed * 2.0f;

    pkDesc->m_fDirDeclination = NI_PI;
    pkDesc->m_fDirDeclinationVar = 0.0f;
    pkDesc->m_fDirPlanarAngle = 0.0f;
    pkDesc->m_fDirPlanarAngleVar = 0.0f;

    pkDesc->m_fSize = fSize;
    pkDesc->m_fSizeVar = fSizeVar * pkDesc->m_fSize;

    pkDesc->m_fEmitStart = kEmitStart * sfTimeFactor;
    pkDesc->m_fEmitStop = kEmitStop * sfTimeFactor;

    if (pkDesc->m_bNiPSysUseViewport)
        pkDesc->m_fBirthRate = fPercentageShown * iBirthRate * GetFrameRate();
    else 
        pkDesc->m_fBirthRate = (float) iBirthRate * GetFrameRate();

    pkDesc->m_fLifeSpan = kLifeSpan * sfTimeFactor;
    // we must account for the fact that NiParticle calculates lifespan by
    // taking Lifespan + LifeSpanVar * (UnitRandom - 0.5);
    pkDesc->m_fLifeSpanVar = kLifeSpanVar * sfTimeFactor * 2.0f;
    pkDesc->m_bUseBirthRate = true;
    pkDesc->m_bSpawnOnDeath = (iSpawnType == 3);

    if (pkDesc->m_fEmitStart == pkDesc->m_fEmitStop)
    {
        pkDesc->m_fEmitStop = pkDesc->m_fEmitStart + PSYS_INTERVAL_TIME;
        if (iLimitUsingTotal == 0)
            iTotalParticles = iBirthRate;

        iLimitUsingTotal = 1;
    }

    if (iLimitUsingTotal != 0)
    {
        float fLifetime = pkDesc->m_fEmitStop - pkDesc->m_fEmitStart;

        if (pkDesc->m_bNiPSysUseViewport)
        {
            pkDesc->m_fBirthRate = fPercentageShown * iTotalParticles / 
                fLifetime;
        }
        else
        {
            pkDesc->m_fBirthRate = iTotalParticles / fLifetime;
        }
    }

    pkDesc->m_fEmitterWidth = fWidth;
    pkDesc->m_fEmitterHeight = fHeight;
    pkDesc->m_fEmitterDepth = 0.0f;

    pkDesc->m_bDieOnCollide = (iSpawnType == 1 || iSpawnType == 2);
    pkDesc->m_bSpawnOnCollide = (iSpawnType == 2);
    pkDesc->m_usNumGenerations = (unsigned short)(iSpawnType == 0 ? 0 : iNumSpawns);
    pkDesc->m_fPercentageSpawned = (float)iSpawnPercent * 0.01f;
    pkDesc->m_usMultiplier = (unsigned short)iSpawnMultiplier;
    pkDesc->m_fMultiplierVar = fSpawnMultiplierVar;
    pkDesc->m_fSpeedChaos = fSpawnSpeedChaos * 0.01f;
    switch (iSpawnSpeedSign)
    {
        case 0:
            pkDesc->m_fSpeedChaos *= -1.0f;
            break;
        case 1:
        case 2:
            break;
    };
    pkDesc->m_fDirChaos = fSpawnDirChaos;

    pkDesc->m_usViewportTotal = (unsigned short)
        (fPercentageShown * iTotalParticles);
    pkDesc->m_usRendererTotal = (unsigned short)
        (iTotalParticles);

    pkDesc->m_fGrowFor = kGrowFor * sfTimeFactor;
    pkDesc->m_fFadeFor = kFadeFor * sfTimeFactor;

    pkDesc->m_fSpinTime = kSpinTime * sfTimeFactor;
    pkDesc->m_fSpinTimeVar = fSpinTimeVar;
    pkDesc->m_bRandomSpinAxis = (iSpinAxisType != 1);
    pkDesc->m_kSpinAxis = kSpinAxis;
    pkDesc->m_fSpinAxisVar = fSpinAxisVar;
    pkDesc->m_fSpinPhase = fSpinPhase;
    pkDesc->m_fSpinPhaseVar = fSpinPhaseVar;

    if (iParticleType == 2 && pkCommon->custnode)
    {
        // Instanced particles
        NiMAXHierConverter kHier(m_kAnimStart, m_kAnimEnd);

        unsigned int uiNumMutations = pkCommon->nlist.Count();

        pkDesc->m_usNumMeshes = (unsigned short)(uiNumMutations + 1);

        pkDesc->m_pspMeshes = NiNew NiAVObjectPtr[uiNumMutations + 1];

        NiNodePtr spResult;
        for (unsigned int i = 0; i < uiNumMutations; i++)
        {
            kHier.BuildNode(pkIntf, pkCommon->nlist[i], 
                &spResult, pkParentScale, false, false);
            ReplaceRoot(&spResult);
            pkDesc->m_pspMeshes[i + 1] = spResult;
        }

        kHier.BuildNode(pkIntf, pkCommon->custnode, 
            &spResult, pkParentScale, false, false);
        ReplaceRoot(&spResult);
        pkDesc->m_pspMeshes[0] = spResult;
    }
    CHECK_MEMORY();

    // Extract animations
    if (iLimitUsingTotal == 0)
    {
        float fBRScalar = 1.0f;
        if (pkDesc->m_bNiPSysUseViewport)
            fBRScalar = fPercentageShown * GetFrameRate();
        else 
            fBRScalar = (float) GetFrameRate();

        pkDesc->m_spBirthRateInterp = BuildFloatInterp(pkPBlock,
            PBBLIZ_PBIRTHRATE, pkDesc->m_eBirthRateInterpCycle, fBRScalar);
    }
    pkDesc->m_spLifeSpanInterp = BuildFloatInterp(pkPBlock, PBBLIZ_LIFE, 
        pkDesc->m_eLifeSpanInterpCycle, (float) sfTimeFactor);
    pkDesc->m_spSpeedInterp = BuildFloatInterp(pkPBlock, PBBLIZ_SPEED, 
        pkDesc->m_eSpeedInterpCycle, (float) GetFrameRate());
    pkDesc->m_spSizeInterp = BuildFloatInterp(pkPBlock, PBBLIZ_SIZE, 
        pkDesc->m_eSizeInterpCycle);
    pkDesc->m_spPlanarAngleInterp = BuildFloatInterp(pkPBlock, 
        PBBLIZ_OFFPLANE,pkDesc->m_ePlanarAngleInterpCycle);
    pkDesc->m_spPlanarAngleVarInterp = BuildFloatInterp(pkPBlock, 
        PBBLIZ_PLANESPREAD, pkDesc->m_ePlanarAngleVarInterpCycle);
    pkDesc->m_spDeclinationInterp = BuildFloatInterp(pkPBlock, 
        PBBLIZ_OFFAXIS, pkDesc->m_eDeclinationInterpCycle);
    pkDesc->m_spDeclinationVarInterp = BuildFloatInterp(pkPBlock,
        PBBLIZ_AXISSPREAD, pkDesc->m_eDeclinationVarInterpCycle);
    pkDesc->m_spSpinPhaseInterp = BuildFloatInterp(pkPBlock, PBBLIZ_SPINPHASE,
        pkDesc->m_eSpinPhaseInterpCycle);
    pkDesc->m_spSpinPhaseVarInterp = BuildFloatInterp(pkPBlock, 
        PBBLIZ_SPINPHASEVAR, pkDesc->m_eSpinPhaseVarInterpCycle);

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractPArray(
    INode*, 
    NiNode*,
    Interface* pkIntf, 
    SimpleParticle* pkPart, 
    NiPSysDesc* pkDesc, 
    ScaleInfo* pkParentScale)
{
    CHECK_MEMORY();
    PArrayParticle* pkPArray = (PArrayParticle*) pkPart;
    IParamBlock* pkPBlock = pkPart->pblock;
    
    // Emitter
    pkDesc->m_pkMAXEmitter = pkPArray->distnode;
    Interval intForever = FOREVER;

    // Basic Parameters Rollout
    int iParticleFormation;
    pkPBlock->GetValue(PBPARR_DISTRIBUTION, 0, iParticleFormation, intForever);
    int iDistinctPoints;
    pkPBlock->GetValue(PBPARR_EMITTERCOUNT, 0, iDistinctPoints, intForever);
    int iUseSelectedSubobjects;
    pkPBlock->GetValue(PBPARR_USESELECTED, 0, iUseSelectedSubobjects, 
        intForever);
    float fWidth;
    pkPBlock->GetValue(PBPARR_EMITRWID, 0, fWidth, intForever);
    int iShowEmitter, iViewportDisplayType;
    pkPBlock->GetValue(PBPARR_EMITRHID, 0, iShowEmitter, intForever);
    pkPBlock->GetValue(PBPARR_VIEWPORTSHOWS, 0, iViewportDisplayType, 
        intForever);
    float fPercentageShown;
    pkPBlock->GetValue(PBPARR_DISPLAYPORTION, 0, fPercentageShown, intForever);

    // Particle Generation Rollout
    int iLimitUsingTotal, iBirthRate, iTotalParticles;
    pkPBlock->GetValue(PBPARR_BIRTHMETHOD, 0, iLimitUsingTotal, intForever);
    pkPBlock->GetValue(PBPARR_PBIRTHRATE, 0, iBirthRate, intForever);
    pkPBlock->GetValue(PBPARR_PTOTALNUMBER, 0, iTotalParticles, intForever);
    float fSpeed, fSpeedVar;
    pkPBlock->GetValue(PBPARR_SPEED, 0, fSpeed, intForever);
    pkPBlock->GetValue(PBPARR_SPEEDVAR, 0, fSpeedVar, intForever);
    float fDivergence;
    pkPBlock->GetValue(PBPARR_ANGLEDIV, 0, fDivergence, intForever);
    TimeValue kEmitStart, kEmitStop;
    pkPBlock->GetValue(PBPARR_EMITSTART, 0, kEmitStart, intForever);
    pkPBlock->GetValue(PBPARR_EMITSTOP, 0, kEmitStop, intForever);
    TimeValue kDisplayUntil, kLifeSpan, kLifeSpanVar;
    pkPBlock->GetValue(PBPARR_DISPUNTIL, 0, kDisplayUntil, intForever);
    pkPBlock->GetValue(PBPARR_LIFE, 0, kLifeSpan, intForever);
    pkPBlock->GetValue(PBPARR_LIFEVAR, 0, kLifeSpanVar, intForever);
    int iSubFrameTime, iSubFrameTrans, iSubFrameRot;
    pkPBlock->GetValue(PBPARR_SUBFRAMETIME, 0, iSubFrameTime, intForever);
    pkPBlock->GetValue(PBPARR_SUBFRAMEMOVE, 0, iSubFrameTrans, intForever);
    pkPBlock->GetValue(PBPARR_SUBFRAMEROT, 0, iSubFrameRot, intForever);
    float fSize, fSizeVar;
    pkPBlock->GetValue(PBPARR_SIZE, 0, fSize, intForever);
    pkPBlock->GetValue(PBPARR_SIZEVAR, 0, fSizeVar, intForever);
    TimeValue kGrowFor, kFadeFor;
    pkPBlock->GetValue(PBPARR_GROWTIME, 0, kGrowFor, intForever);
    pkPBlock->GetValue(PBPARR_FADETIME, 0, kFadeFor, intForever);
    int iRandSeed;
    pkPBlock->GetValue(PBPARR_RNDSEED, 0, iRandSeed, intForever);

    // Particle Type Rollout
    int iParticleType, iStandardParticleType;
    pkPBlock->GetValue(PBPARR_PARTICLECLASS, 0, iParticleType, intForever);
    pkPBlock->GetValue(PBPARR_PARTICLETYPE, 0, iStandardParticleType, 
        intForever);
    float fMetaTension, fMetaTensionVar;
    pkPBlock->GetValue(PBPARR_METATENSION, 0, fMetaTension, intForever);
    pkPBlock->GetValue(PBPARR_METATENSIONVAR, 0, fMetaTensionVar, intForever);
    float fMetaCourseness, fMetaViewportCourseness;
    pkPBlock->GetValue(PBPARR_METACOURSE, 0, fMetaCourseness, intForever);
    pkPBlock->GetValue(PBPARR_METACOURSEV, 0, fMetaViewportCourseness, 
        intForever);
    int iMetaAutoCoarseness;
    pkPBlock->GetValue(PBPARR_METAAUTOCOARSE, 0, iMetaAutoCoarseness, 
        intForever);
    float fFragThickness;
    pkPBlock->GetValue(PBPARR_FRAGTHICKNESS, 0, fFragThickness, intForever);
    int iFragMethod, iFragCount;
    pkPBlock->GetValue(PBPARR_FRAGMETHOD, 0, iFragMethod, intForever);
    pkPBlock->GetValue(PBPARR_FRAGCOUNT, 0, iFragCount, intForever);
    float fFragSmoothingAngle;
    pkPBlock->GetValue(PBPARR_SMOOTHANG, 0, fFragSmoothingAngle, intForever);
    int iInstancedSubtree;
    pkPBlock->GetValue(PBPARR_USESUBTREE, 0, iInstancedSubtree, intForever);
    int iInstancedOffsetType;
    pkPBlock->GetValue(PBPARR_ANIMATIONOFFSET, 0, iInstancedOffsetType, 
        intForever);
    float fInstancedOffsetAmount;
    pkPBlock->GetValue(PBPARR_OFFSETAMOUNT, 0, fInstancedOffsetAmount, 
        intForever);
    int iMaterialMappingType;
    pkPBlock->GetValue(PBPARR_MAPPINGTYPE, 0, iMaterialMappingType, intForever);
    TimeValue kMappingTime;
    pkPBlock->GetValue(PBPARR_MAPPINGTIME, 0, kMappingTime, intForever);
    float fMappingDistance;
    pkPBlock->GetValue(PBPARR_MAPPINGDIST, 0, fMappingDistance, intForever);
    
    int iOutsideMat, iEdgeMat, iBacksideMat;
    pkPBlock->GetValue(PBPARR_FMAT, 0, iOutsideMat, intForever);
    pkPBlock->GetValue(PBPARR_EMAT, 0, iEdgeMat, intForever);
    pkPBlock->GetValue(PBPARR_BMAT, 0, iBacksideMat, intForever);

    // Rotation And Collision Rollout
    TimeValue kSpinTime;
    pkPBlock->GetValue(PBPARR_SPINTIME, 0, kSpinTime, intForever);
    float fSpinTimeVar;
    pkPBlock->GetValue(PBPARR_SPINTIMEVAR, 0, fSpinTimeVar, intForever);
    float fSpinPhase, fSpinPhaseVar;
    pkPBlock->GetValue(PBPARR_SPINPHASE, 0, fSpinPhase, intForever);
    pkPBlock->GetValue(PBPARR_SPINPHASEVAR, 0, fSpinPhaseVar, intForever);
    int iSpinAxisType;
    pkPBlock->GetValue(PBPARR_SPINAXISTYPE, 0, iSpinAxisType, intForever);
    float fSpinStretch;
    pkPBlock->GetValue(PBPARR_STRETCH, 0, fSpinStretch, intForever);
    NiPoint3 kSpinAxis;
    pkPBlock->GetValue(PBPARR_SPINAXISX, 0, kSpinAxis.x, intForever);
    pkPBlock->GetValue(PBPARR_SPINAXISY, 0, kSpinAxis.y, intForever);
    pkPBlock->GetValue(PBPARR_SPINAXISZ, 0, kSpinAxis.z, intForever);
    float fSpinAxisVar;
    pkPBlock->GetValue(PBPARR_SPINAXISVAR, 0, fSpinAxisVar, intForever);
    int iParticleCollisionOn;
    pkPBlock->GetValue(PBPARR_PAIPCOLLIDE_ON, 0, iParticleCollisionOn, 
        intForever);
    int iParticleCollisionSteps;
    pkPBlock->GetValue(PBPARR_PAIPCOLLIDE_STEPS, 0, iParticleCollisionSteps, 
        intForever);
    float fParticleCollisionBounce, fParticleCollisionBounceVar;
    pkPBlock->GetValue(PBPARR_PAIPCOLLIDE_BOUNCE, 0, 
        fParticleCollisionBounce, intForever);
    pkPBlock->GetValue(PBPARR_PAIPCOLLIDE_BOUNCEVAR, 0, 
        fParticleCollisionBounceVar, intForever);

    // Object Motion Inheritance Rollout
    float fOMIInfluence, fOMIMultiplier, fOMIMultiplierVar;
    pkPBlock->GetValue(PBPARR_EMITVINFL, 0, fOMIInfluence, intForever);
    pkPBlock->GetValue(PBPARR_EMITVMULT, 0, fOMIMultiplier, intForever);
    pkPBlock->GetValue(PBPARR_EMITVMULTVAR, 0, fOMIMultiplierVar, intForever);

    // Bubble Motion Rollout
    float fBubbleAmp, fBubbleAmpVar;
    pkPBlock->GetValue(PBPARR_BUBLAMP, 0, fBubbleAmp, intForever);
    pkPBlock->GetValue(PBPARR_BUBLAMPVAR, 0, fBubbleAmpVar, intForever);
    TimeValue kBubblePeriod;
    pkPBlock->GetValue(PBPARR_BUBLPER, 0, kBubblePeriod, intForever);
    float fBubblePeriodVar;
    pkPBlock->GetValue(PBPARR_BUBLPERVAR, 0, fBubblePeriodVar, intForever);
    float fBubblePhase, fBubblePhaseVar;
    pkPBlock->GetValue(PBPARR_BUBLPHAS, 0, fBubblePhase, intForever);
    pkPBlock->GetValue(PBPARR_BUBLPHASVAR, 0, fBubblePhaseVar, intForever);

    // Particle Spawn Rollout
    int iSpawnType;
    pkPBlock->GetValue(PBPARR_SPAWNTYPE, 0, iSpawnType, intForever);
    TimeValue kPersist;
    pkPBlock->GetValue(PBPARR_PASPAWNDIEAFTER, 0, kPersist, intForever);
    float fPersistVar;
    pkPBlock->GetValue(PBPARR_PASPAWNDIEAFTERVAR, 0, fPersistVar, intForever);
    int iNumSpawns;
    pkPBlock->GetValue(PBPARR_SPAWNGENS, 0, iNumSpawns, intForever);
    int iSpawnPercent;
    pkPBlock->GetValue(PBPARR_SPAWNPERCENT, 0, iSpawnPercent, intForever);
    int iSpawnMultiplier;
    pkPBlock->GetValue(PBPARR_SPAWNCOUNT, 0, iSpawnMultiplier, intForever);
    float fSpawnMultiplierVar;
    pkPBlock->GetValue(PBPARR_SPAWNMULTVAR, 0, fSpawnMultiplierVar, intForever);
    float fSpawnDirChaos;
    pkPBlock->GetValue(PBPARR_SPAWNDIRCHAOS, 0, fSpawnDirChaos, intForever);
    float fSpawnSpeedChaos;
    pkPBlock->GetValue(PBPARR_SPAWNSPEEDCHAOS, 0, fSpawnSpeedChaos, intForever);
    int iSpawnSpeedSign;
    pkPBlock->GetValue(PBPARR_SPAWNSPEEDSIGN, 0, iSpawnSpeedSign, intForever);
    int iSpawnInheritSpeed;
    pkPBlock->GetValue(PBPARR_SPAWNINHERITV, 0, iSpawnInheritSpeed, intForever);
    int iSpawnFixedSpeed;
    pkPBlock->GetValue(PBPARR_SPAWNSPEEDFIXED, 0, iSpawnFixedSpeed, intForever);
    float fSpawnScaleChaos;
    pkPBlock->GetValue(PBPARR_SPAWNSCALECHAOS, 0, fSpawnScaleChaos, intForever);
    int iSpawnScaleSign;
    pkPBlock->GetValue(PBPARR_SPAWNSCALESIGN, 0, iSpawnScaleSign, intForever);
    int iSpawnFixedScale;
    pkPBlock->GetValue(PBPARR_SPAWNSCALEFIXED, 0, iSpawnFixedScale, intForever);
    TimeValue kSpawnLifeValue;
    pkPBlock->GetValue(PBPARR_SPAWNLIFEVLUE, 0, kSpawnLifeValue, intForever);

    // Unknown
    int iCustomMtl, iPANotDraft;
    pkPBlock->GetValue(PBPARR_CUSTOMMATERIAL, 0, iCustomMtl, intForever);
    pkPBlock->GetValue(PBPARR_PANOTDRAFT, 0, iPANotDraft, intForever);

    // Set up emitter info
    if (pkDesc->m_pkMAXEmitter)
    {
        pkDesc->m_eEmitterType = EMITTER_OBJECT;
        switch (iParticleFormation)
        {
            default:
            case 0:
                pkDesc->m_eEmissionType = 
                    NiPSMeshEmitter::EMIT_FROM_FACE_SURFACE;
                break;
            case 1:
                pkDesc->m_eEmissionType = 
                    NiPSMeshEmitter::EMIT_FROM_EDGE_SURFACE;
                break;
            case 2:
                pkDesc->m_eEmissionType = 
                    NiPSMeshEmitter::EMIT_FROM_VERTICES;
                break;
            case 3:
                pkDesc->m_eEmissionType = 
                    NiPSMeshEmitter::EMIT_FROM_VERTICES;
                break;
            case 4:
                pkDesc->m_eEmissionType = 
                    NiPSMeshEmitter::EMIT_FROM_FACE_CENTER;
                break;
        }
    }

    // Set up parameters
    pkDesc->m_fSpeed = fSpeed * GetFrameRate();
    // we must account for the fact that NiParticle calculates speed by
    // taking Speed + SpeedVar * (UnitRandom - 0.5);
    pkDesc->m_fSpeedVar = fSpeedVar * pkDesc->m_fSpeed * 2.0f;

    pkDesc->m_fDirDeclination = 0.0f;
    pkDesc->m_fDirDeclinationVar = fDivergence * 0.5f;
    pkDesc->m_fDirPlanarAngle = 0.0f;
    pkDesc->m_fDirPlanarAngleVar = fDivergence * 0.5f;

    pkDesc->m_fSize = fSize;
    pkDesc->m_fSizeVar = fSizeVar * pkDesc->m_fSize;

    pkDesc->m_fEmitStart = kEmitStart * sfTimeFactor;
    pkDesc->m_fEmitStop = kEmitStop * sfTimeFactor;

    if (pkDesc->m_bNiPSysUseViewport)
        pkDesc->m_fBirthRate = fPercentageShown * iBirthRate * GetFrameRate();
    else 
        pkDesc->m_fBirthRate = (float) iBirthRate * (float) GetFrameRate();

    pkDesc->m_fLifeSpan = kLifeSpan * sfTimeFactor;
    // we must account for the fact that NiParticle calculates lifespan by
    // taking Lifespan + LifeSpanVar * (UnitRandom - 0.5);
    pkDesc->m_fLifeSpanVar = kLifeSpanVar * sfTimeFactor * 2.0f;
    pkDesc->m_bUseBirthRate = true;
    pkDesc->m_bSpawnOnDeath = (iSpawnType == 3);

    if (pkDesc->m_fEmitStart == pkDesc->m_fEmitStop)
    {
        pkDesc->m_fEmitStop = pkDesc->m_fEmitStart + PSYS_INTERVAL_TIME;
        if (iLimitUsingTotal == 0)
            iTotalParticles = iBirthRate;
        iLimitUsingTotal = 1;
    }

    if (iLimitUsingTotal != 0)
    {
        float fLifetime = pkDesc->m_fEmitStop - pkDesc->m_fEmitStart;

        if (pkDesc->m_bNiPSysUseViewport)
        {
            pkDesc->m_fBirthRate = fPercentageShown * iTotalParticles / 
                fLifetime;
        }
        else
        {
            pkDesc->m_fBirthRate = (float) iTotalParticles / fLifetime;
        }
    }
    pkDesc->m_fEmitterWidth = 0.0f;
    pkDesc->m_fEmitterHeight = 0.0f;
    pkDesc->m_fEmitterDepth = 0.0f;

    pkDesc->m_bDieOnCollide = (iSpawnType == 1 || iSpawnType == 2);
    pkDesc->m_bSpawnOnCollide = (iSpawnType == 2);
    pkDesc->m_usNumGenerations = (unsigned short)(iSpawnType == 0 ? 0 : iNumSpawns);
    pkDesc->m_fPercentageSpawned = (float)iSpawnPercent * 0.01f;
    pkDesc->m_usMultiplier = (unsigned short)iSpawnMultiplier;
    pkDesc->m_fMultiplierVar = fSpawnMultiplierVar;
    pkDesc->m_fSpeedChaos = fSpawnSpeedChaos * 0.01f;
    switch (iSpawnSpeedSign)
    {
        case 0:
            pkDesc->m_fSpeedChaos *= -1.0f;
            break;
        case 1:
        case 2:
            break;
    };
    pkDesc->m_fDirChaos = fSpawnDirChaos;

    pkDesc->m_usViewportTotal = (unsigned short)
        (fPercentageShown * iTotalParticles);
    pkDesc->m_usRendererTotal = (unsigned short)
        (iTotalParticles);

    pkDesc->m_fGrowFor = kGrowFor * sfTimeFactor;
    pkDesc->m_fFadeFor = kFadeFor * sfTimeFactor;

    pkDesc->m_fSpinTime = kSpinTime * sfTimeFactor;
    pkDesc->m_fSpinTimeVar = fSpinTimeVar;
    pkDesc->m_bRandomSpinAxis = (iSpinAxisType != 2);
    pkDesc->m_kSpinAxis = kSpinAxis;
    pkDesc->m_fSpinAxisVar = fSpinAxisVar;
    pkDesc->m_fSpinPhase = fSpinPhase;
    pkDesc->m_fSpinPhaseVar = fSpinPhaseVar;

    if (iParticleType == 3 && pkPArray->custnode)
    {
        // Instanced particles
        NiMAXHierConverter kHier(m_kAnimStart, m_kAnimEnd);

        unsigned int uiNumMutations = pkPArray->nlist.Count();

        pkDesc->m_usNumMeshes = (unsigned short)(uiNumMutations + 1);

        pkDesc->m_pspMeshes = NiNew NiAVObjectPtr[uiNumMutations + 1];

        NiNodePtr spResult;
        for (unsigned int i = 0; i < uiNumMutations; i++)
        {
            kHier.BuildNode(pkIntf, pkPArray->nlist[i], 
                &spResult, pkParentScale, false, false);
            ReplaceRoot(&spResult);
            pkDesc->m_pspMeshes[i + 1] = spResult;
        }

        kHier.BuildNode(pkIntf, pkPArray->custnode, &spResult, 
            pkParentScale, false, false);
        ReplaceRoot(&spResult);
        pkDesc->m_pspMeshes[0] = spResult;
    }
    else if (iParticleType == 2)
    {
        // Object fragments
        pkPArray->UpdateParticles(kEmitStart, NULL);
        unsigned int uiNumFrags = pkPArray->parts.Count();
        if (uiNumFrags)
        {
            NiMAXMaterialConverter kMtl(m_kAnimStart, m_kAnimEnd);
            pkDesc->m_usViewportTotal = (unsigned short)uiNumFrags;
            pkDesc->m_usRendererTotal = (unsigned short)uiNumFrags;
            pkDesc->m_usNumFrags = (unsigned short)uiNumFrags;
            pkDesc->m_pkPos = NiNew NiPoint3[uiNumFrags];
            pkDesc->m_pkVel = NiNew NiPoint3[uiNumFrags];
            pkDesc->m_pspFrag = NiNew NiTriShapePtr[uiNumFrags];
        }
    }

    // Extract animations
    if (iLimitUsingTotal == 0)
    {
        float fBRScalar = 1.0f;
        if (pkDesc->m_bNiPSysUseViewport)
            fBRScalar = fPercentageShown * GetFrameRate();
        else 
            fBRScalar = (float) GetFrameRate();

        pkDesc->m_spBirthRateInterp = BuildFloatInterp(pkPBlock, 
            PBPARR_PBIRTHRATE, pkDesc->m_eBirthRateInterpCycle, fBRScalar);
    }
    pkDesc->m_spLifeSpanInterp = BuildFloatInterp(pkPBlock, PBPARR_LIFE,
        pkDesc->m_eLifeSpanInterpCycle, sfTimeFactor);
    pkDesc->m_spSpeedInterp = BuildFloatInterp(pkPBlock, PBPARR_SPEED,
        pkDesc->m_eSpeedInterpCycle, (float) GetFrameRate());
    pkDesc->m_spSizeInterp = BuildFloatInterp(pkPBlock, PBPARR_SIZE,
        pkDesc->m_eSizeInterpCycle);
    pkDesc->m_spSpinPhaseInterp = BuildFloatInterp(pkPBlock, PBPARR_SPINPHASE,
        pkDesc->m_eSpinPhaseInterpCycle);
    pkDesc->m_spSpinPhaseVarInterp = BuildFloatInterp(pkPBlock, 
        PBPARR_SPINPHASEVAR, pkDesc->m_eSpinPhaseVarInterpCycle);

    CHECK_MEMORY();

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractPCloud(
    INode*, 
    Interface* pkIntf, 
    SimpleParticle* pkPart, 
    NiPSysDesc* pkDesc, 
    ScaleInfo* pkParentScale)
{
    CHECK_MEMORY();
    PCloudParticle* pkPCld = (PCloudParticle*)pkPart;
    IParamBlock* pkPBlock = pkPart->pblock;

    // Basic Parameters Rollout
    int iEmitterType;
    Interval intForever = FOREVER;

    pkPBlock->GetValue(PB_PCLD_CREATEIN, 0, iEmitterType, intForever);
    switch (iEmitterType)
    {
        default:
        case 0: // Box
            pkDesc->m_eEmitterType = EMITTER_BOX;
            break;
        case 1: // Sphere
            pkDesc->m_eEmitterType = EMITTER_SPHERE;
            break;
        case 2: // Cylinder
            pkDesc->m_eEmitterType = EMITTER_CYLINDER;
            break;
        case 3: // Generic mesh
            pkDesc->m_eEmitterType = EMITTER_OBJECT;
            pkDesc->m_eEmissionType = 
                NiPSMeshEmitter::EMIT_FROM_FACE_SURFACE;
            pkDesc->m_pkMAXEmitter = pkPCld->distnode;
            break;
    }
    float fWidth, fHeight, fDepth;
    pkPBlock->GetValue(PB_PCLD_EMITRHEIGHT, 0, fWidth, intForever);
    pkPBlock->GetValue(PB_PCLD_EMITRWID, 0, fHeight, intForever);
    pkPBlock->GetValue(PB_PCLD_EMITRDEPTH, 0, fDepth, intForever);
    int iShowEmitter, iViewportDisplayType;
    pkPBlock->GetValue(PB_PCLD_EMITRHID, 0, iShowEmitter, intForever);
    pkPBlock->GetValue(PB_PCLD_VIEWPORTSHOWS, 0, iViewportDisplayType, 
        intForever);
    float fPercentageShown;
    pkPBlock->GetValue(PB_PCLD_DISPLAYPORTION, 0, fPercentageShown, intForever);

    // Particle Generation Rollout
    int iLimitUsingTotal, iBirthRate, iTotalParticles;
    pkPBlock->GetValue(PB_PCLD_BIRTHMETHOD, 0, iLimitUsingTotal, intForever);
    pkPBlock->GetValue(PB_PCLD_PBIRTHRATE, 0, iBirthRate, intForever);
    pkPBlock->GetValue(PB_PCLD_PTOTALNUMBER, 0, iTotalParticles, intForever);
    float fSpeed, fSpeedVar;
    pkPBlock->GetValue(PB_PCLD_SPEED, 0, fSpeed, intForever);
    pkPBlock->GetValue(PB_PCLD_SPEEDVAR, 0, fSpeedVar, intForever);
    int iSpeedDirectionType;
    pkPBlock->GetValue(PB_PCLD_SPEEDDIR, 0, iSpeedDirectionType, intForever);
    NiPoint3 kSpeedDirection;
    pkPBlock->GetValue(PB_PCLD_DIRX, 0, kSpeedDirection.x, intForever);
    pkPBlock->GetValue(PB_PCLD_DIRY, 0, kSpeedDirection.y, intForever);
    pkPBlock->GetValue(PB_PCLD_DIRZ, 0, kSpeedDirection.z, intForever);
    float fSpeedDirectionVar;
    pkPBlock->GetValue(PB_PCLD_DIRVAR, 0, fSpeedDirectionVar, intForever);
    TimeValue kEmitStart, kEmitStop;
    pkPBlock->GetValue(PB_PCLD_EMITSTART, 0, kEmitStart, intForever);
    pkPBlock->GetValue(PB_PCLD_EMITSTOP, 0, kEmitStop, intForever);
    TimeValue kDisplayUntil, kLifeSpan, kLifeSpanVar;
    pkPBlock->GetValue(PB_PCLD_DISPUNTIL, 0, kDisplayUntil, intForever);
    pkPBlock->GetValue(PB_PCLD_LIFE, 0, kLifeSpan, intForever);
    pkPBlock->GetValue(PB_PCLD_LIFEVAR, 0, kLifeSpanVar, intForever);
    float fSize, fSizeVar;
    pkPBlock->GetValue(PB_PCLD_SIZE, 0, fSize, intForever);
    pkPBlock->GetValue(PB_PCLD_SIZEVAR, 0, fSizeVar, intForever);
    TimeValue kGrowFor, kFadeFor;
    pkPBlock->GetValue(PB_PCLD_GROWTIME, 0, kGrowFor, intForever);
    pkPBlock->GetValue(PB_PCLD_FADETIME, 0, kFadeFor, intForever);
    int iRandSeed;
    pkPBlock->GetValue(PB_PCLD_RNDSEED, 0, iRandSeed, intForever);

    // Particle Type Rollout
    int iParticleType, iStandardParticleType;
    pkPBlock->GetValue(PB_PCLD_PARTICLECLASS, 0, iParticleType, intForever);
    pkPBlock->GetValue(PB_PCLD_PARTICLETYPE, 0, iStandardParticleType, 
        intForever);
    float fMetaTension, fMetaTensionVar;
    pkPBlock->GetValue(PB_PCLD_METATENSION, 0, fMetaTension, intForever);
    pkPBlock->GetValue(PB_PCLD_METATENSIONVAR, 0, fMetaTensionVar, intForever);
    float fMetaCourseness, fMetaViewportCourseness;
    pkPBlock->GetValue(PB_PCLD_METACOURSE, 0, fMetaCourseness, intForever);
    pkPBlock->GetValue(PB_PCLD_METACOURSEV, 0, fMetaViewportCourseness, 
        intForever);
    int iMetaAutoCoarseness;
    pkPBlock->GetValue(PB_PCLD_METAAUTOCOARSE, 0, iMetaAutoCoarseness, 
        intForever);
    int iInstancedSubtree;
    pkPBlock->GetValue(PB_PCLD_USESUBTREE, 0, iInstancedSubtree, intForever);
    int iInstancedOffsetType;
    pkPBlock->GetValue(PB_PCLD_ANIMATIONOFFSET, 0, iInstancedOffsetType, 
        intForever);
    float fInstancedOffsetAmount;
    pkPBlock->GetValue(PB_PCLD_OFFSETAMOUNT, 0, fInstancedOffsetAmount, 
        intForever);
    int iMaterialMappingType;
    pkPBlock->GetValue(PB_PCLD_MAPPINGTYPE, 0, iMaterialMappingType, intForever);
    TimeValue kMappingTime;
    pkPBlock->GetValue(PB_PCLD_MAPPINGTIME, 0, kMappingTime, intForever);
    float fMappingDistance;
    pkPBlock->GetValue(PB_PCLD_MAPPINGDIST, 0, fMappingDistance, intForever);

    // Rotation And Collision Rollout
    TimeValue kSpinTime;
    pkPBlock->GetValue(PB_PCLD_SPINTIME, 0, kSpinTime, intForever);
    float fSpinTimeVar;
    pkPBlock->GetValue(PB_PCLD_SPINTIMEVAR, 0, fSpinTimeVar, intForever);
    float fSpinPhase, fSpinPhaseVar;
    pkPBlock->GetValue(PB_PCLD_SPINPHASE, 0, fSpinPhase, intForever);
    pkPBlock->GetValue(PB_PCLD_SPINPHASEVAR, 0, fSpinPhaseVar, intForever);
    int iSpinAxisType;
    pkPBlock->GetValue(PB_PCLD_SPINAXISTYPE, 0, iSpinAxisType, intForever);
    float fSpinStretch;
    pkPBlock->GetValue(PB_PCLD_STRETCH, 0, fSpinStretch, intForever);
    NiPoint3 kSpinAxis;
    pkPBlock->GetValue(PB_PCLD_SPINAXISX, 0, kSpinAxis.x, intForever);
    pkPBlock->GetValue(PB_PCLD_SPINAXISY, 0, kSpinAxis.y, intForever);
    pkPBlock->GetValue(PB_PCLD_SPINAXISZ, 0, kSpinAxis.z, intForever);
    float fSpinAxisVar;
    pkPBlock->GetValue(PB_PCLD_SPINAXISVAR, 0, fSpinAxisVar, intForever);
    int iParticleCollisionOn;
    pkPBlock->GetValue(PB_PCLD_PCIPCOLLIDE_ON, 0, iParticleCollisionOn, 
        intForever);
    int iParticleCollisionSteps;
    pkPBlock->GetValue(PB_PCLD_PCIPCOLLIDE_STEPS, 0, 
        iParticleCollisionSteps, intForever);
    float fParticleCollisionBounce, fParticleCollisionBounceVar;
    pkPBlock->GetValue(PB_PCLD_PCIPCOLLIDE_BOUNCE, 0, 
        fParticleCollisionBounce, intForever);
    pkPBlock->GetValue(PB_PCLD_PCIPCOLLIDE_BOUNCEVAR, 0, 
        fParticleCollisionBounceVar, intForever);

    // Object Motion Inheritance Rollout
    float fOMIInfluence, fOMIMultiplier, fOMIMultiplierVar;
    pkPBlock->GetValue(PB_PCLD_EMITVINFL, 0, fOMIInfluence, intForever);
    pkPBlock->GetValue(PB_PCLD_EMITVMULT, 0, fOMIMultiplier, intForever);
    pkPBlock->GetValue(PB_PCLD_EMITVMULTVAR, 0, fOMIMultiplierVar, intForever);

    // Bubble Motion Rollout
    float fBubbleAmp, fBubbleAmpVar;
    pkPBlock->GetValue(PB_PCLD_BUBLAMP, 0, fBubbleAmp, intForever);
    pkPBlock->GetValue(PB_PCLD_BUBLAMPVAR, 0, fBubbleAmpVar, intForever);
    TimeValue kBubblePeriod;
    pkPBlock->GetValue(PB_PCLD_BUBLPER, 0, kBubblePeriod, intForever);
    float fBubblePeriodVar;
    pkPBlock->GetValue(PB_PCLD_BUBLPERVAR, 0, fBubblePeriodVar, intForever);
    float fBubblePhase, fBubblePhaseVar;
    pkPBlock->GetValue(PB_PCLD_BUBLPHAS, 0, fBubblePhase, intForever);
    pkPBlock->GetValue(PB_PCLD_BUBLPHASVAR, 0, fBubblePhaseVar, intForever);

    // Particle Spawn Rollout
    int iSpawnType;
    pkPBlock->GetValue(PB_PCLD_SPAWNTYPE, 0, iSpawnType, intForever);
    TimeValue kPersist;
    pkPBlock->GetValue(PB_PCLD_SPAWNDIEAFTER, 0, kPersist, intForever);
    float fPersistVar;
    pkPBlock->GetValue(PB_PCLD_SPAWNDIEAFTERVAR, 0, fPersistVar, intForever);
    int iNumSpawns;
    pkPBlock->GetValue(PB_PCLD_SPAWNGENS, 0, iNumSpawns, intForever);
    int iSpawnPercent;
    pkPBlock->GetValue(PB_PCLD_SPAWNPERCENT, 0, iSpawnPercent, intForever);
    int iSpawnMultiplier;
    pkPBlock->GetValue(PB_PCLD_SPAWNCOUNT, 0, iSpawnMultiplier, intForever);
    float fSpawnMultiplierVar;
    pkPBlock->GetValue(PB_PCLD_SPAWNMULTVAR, 0, fSpawnMultiplierVar, intForever);
    float fSpawnDirChaos;
    pkPBlock->GetValue(PB_PCLD_SPAWNDIRCHAOS, 0, fSpawnDirChaos, intForever);
    float fSpawnSpeedChaos;
    pkPBlock->GetValue(PB_PCLD_SPAWNSPEEDCHAOS, 0, fSpawnSpeedChaos, intForever);
    int iSpawnSpeedSign;
    pkPBlock->GetValue(PB_PCLD_SPAWNSPEEDSIGN, 0, iSpawnSpeedSign, intForever);
    int iSpawnInheritSpeed;
    pkPBlock->GetValue(PB_PCLD_SPAWNINHERITV, 0, iSpawnInheritSpeed, intForever);
    int iSpawnFixedSpeed;
    pkPBlock->GetValue(PB_PCLD_SPAWNSPEEDFIXED, 0, iSpawnFixedSpeed, intForever);
    float fSpawnScaleChaos;
    pkPBlock->GetValue(PB_PCLD_SPAWNSCALECHAOS, 0, fSpawnScaleChaos, intForever);
    int iSpawnScaleSign;
    pkPBlock->GetValue(PB_PCLD_SPAWNSCALESIGN, 0, iSpawnScaleSign, intForever);
    int iSpawnFixedScale;
    pkPBlock->GetValue(PB_PCLD_SPAWNSCALEFIXED, 0, iSpawnFixedScale, intForever);
    TimeValue kSpawnLifeValue;
    pkPBlock->GetValue(PB_PCLD_SPAWNLIFEVLUE, 0, kSpawnLifeValue, intForever);

    // Unknown
    int iCustomMtl, iPCNotDraft;
    pkPBlock->GetValue(PB_PCLD_CUSTOMMTL, 0, iCustomMtl, intForever);
    pkPBlock->GetValue(PB_PCLD_PCNOTDRAFT, 0, iPCNotDraft, intForever);

    // Set up parameters
    pkDesc->m_fSpeed = fSpeed * GetFrameRate();
    // we must account for the fact that NiParticle calculates speed by
    // taking Speed + SpeedVar * (UnitRandom - 0.5);
    pkDesc->m_fSpeedVar = fSpeedVar * pkDesc->m_fSpeed * 2.0f;

    if (iSpeedDirectionType != 1)
    {
        // Random direction or "reference object" - random
        pkDesc->m_fDirDeclination = 0.0f;
        pkDesc->m_fDirDeclinationVar = NI_PI;
        pkDesc->m_fDirPlanarAngle = 0.0f;
        pkDesc->m_fDirPlanarAngleVar = NI_TWO_PI;
    }
    else
    {
        // Specified direction
        pkDesc->m_fDirDeclination = NiACos(kSpeedDirection.z /
            kSpeedDirection.Length());
        pkDesc->m_fDirDeclinationVar = NI_PI * fSpeedDirectionVar;
        if (kSpeedDirection.x != 0.0f)
        {
            pkDesc->m_fDirPlanarAngle = NiATan(kSpeedDirection.y / 
                kSpeedDirection.x);
            if (kSpeedDirection.y < 0.0f)
            {
                pkDesc->m_fDirPlanarAngle += NI_PI;
            }
        }
        else
        {
            if (kSpeedDirection.y > 0.0f)
            {
                pkDesc->m_fDirPlanarAngle = NI_HALF_PI;
            }
            else if (kSpeedDirection.y < 0.0f)
            {
                pkDesc->m_fDirPlanarAngle = 3.0f * NI_HALF_PI;
            }
            else
            {
                pkDesc->m_fDirPlanarAngle = 0.0f;
            }
        }
        pkDesc->m_fDirPlanarAngleVar = NI_TWO_PI * fSpeedDirectionVar;
    }

    pkDesc->m_fSize = fSize;
    pkDesc->m_fSizeVar = fSizeVar * pkDesc->m_fSize;

    pkDesc->m_fEmitStart = kEmitStart * sfTimeFactor;
    pkDesc->m_fEmitStop = kEmitStop * sfTimeFactor;

    pkDesc->m_fLifeSpan = kLifeSpan * sfTimeFactor;
    // we must account for the fact that NiParticle calculates lifespan by
    // taking Lifespan + LifeSpanVar * (UnitRandom - 0.5);
    pkDesc->m_fLifeSpanVar = kLifeSpanVar * sfTimeFactor * 2.0f;
    pkDesc->m_bUseBirthRate = true;
    pkDesc->m_bSpawnOnDeath = (iSpawnType == 3);

    if (pkDesc->m_fEmitStart == pkDesc->m_fEmitStop)
    {
        pkDesc->m_fEmitStop = pkDesc->m_fEmitStart + PSYS_INTERVAL_TIME;
        if (iLimitUsingTotal == 0)
            iTotalParticles = iBirthRate;

        iLimitUsingTotal = 1;

    }

    if (iLimitUsingTotal == 0)
    {
        if (pkDesc->m_bNiPSysUseViewport)
        {
            pkDesc->m_fBirthRate = fPercentageShown * iBirthRate * 
                GetFrameRate();
        }
        else 
        {
            pkDesc->m_fBirthRate = (float) iBirthRate * (float)
                GetFrameRate();
        }
    }
    else
    {
        float fTimeSpan = pkDesc->m_fEmitStop - pkDesc->m_fEmitStart;

        if (pkDesc->m_bNiPSysUseViewport)
        {
            pkDesc->m_fBirthRate = fPercentageShown * iTotalParticles  / 
                fTimeSpan;
        }
        else
        {
            pkDesc->m_fBirthRate = iTotalParticles  / fTimeSpan;
        }
    }
    
    pkDesc->m_fEmitterWidth = fWidth;
    pkDesc->m_fEmitterHeight = fHeight;
    pkDesc->m_fEmitterDepth = fDepth;

    pkDesc->m_bDieOnCollide = (iSpawnType == 1 || iSpawnType == 2);
    pkDesc->m_bSpawnOnCollide = (iSpawnType == 2);
    pkDesc->m_usNumGenerations = (unsigned short)(iSpawnType == 0 ? 0 : iNumSpawns);
    pkDesc->m_fPercentageSpawned = (float)iSpawnPercent * 0.01f;
    pkDesc->m_usMultiplier = (unsigned short)iSpawnMultiplier;
    pkDesc->m_fMultiplierVar = fSpawnMultiplierVar;
    pkDesc->m_fSpeedChaos = fSpawnSpeedChaos * 0.01f;
    switch (iSpawnSpeedSign)
    {
        case 0:
            pkDesc->m_fSpeedChaos *= -1.0f;
            break;
        case 1:
        case 2:
            break;
    };
    pkDesc->m_fDirChaos = fSpawnDirChaos;

    pkDesc->m_usViewportTotal = (unsigned short)
        (fPercentageShown * iTotalParticles);
    pkDesc->m_usRendererTotal = (unsigned short)
        (iTotalParticles);

    pkDesc->m_fGrowFor = kGrowFor * sfTimeFactor;
    pkDesc->m_fFadeFor = kFadeFor * sfTimeFactor;

    pkDesc->m_fSpinTime = kSpinTime * sfTimeFactor;
    pkDesc->m_fSpinTimeVar = fSpinTimeVar;
    pkDesc->m_bRandomSpinAxis = (iSpinAxisType != 2);
    pkDesc->m_kSpinAxis = kSpinAxis;
    pkDesc->m_fSpinAxisVar = fSpinAxisVar;
    pkDesc->m_fSpinPhase = fSpinPhase;
    pkDesc->m_fSpinPhaseVar = fSpinPhaseVar;

    if (iParticleType == 2 && pkPCld->custnode)
    {
        // Instanced particles
        NiMAXHierConverter kHier(m_kAnimStart, m_kAnimEnd);

        unsigned int uiNumMutations = pkPCld->nlist.Count();

        pkDesc->m_usNumMeshes = (unsigned short)(uiNumMutations + 1);

        pkDesc->m_pspMeshes = NiNew NiAVObjectPtr[uiNumMutations + 1];

        NiNodePtr spResult;
        for (unsigned int i = 0; i < uiNumMutations; i++)
        {
            kHier.BuildNode(pkIntf, pkPCld->nlist[i], 
                &spResult, pkParentScale, false, false);
            ReplaceRoot(&spResult);
            pkDesc->m_pspMeshes[i + 1] = spResult;
        }

        kHier.BuildNode(pkIntf, pkPCld->custnode, &spResult, 
            pkParentScale, false, false);
        ReplaceRoot(&spResult);
        pkDesc->m_pspMeshes[0] = spResult;
    }

    // Extract animations
    if (iLimitUsingTotal == 0)
    {
        float fBRScalar = 1.0f;
        if (pkDesc->m_bNiPSysUseViewport)
            fBRScalar = fPercentageShown * GetFrameRate();
        else 
            fBRScalar = (float) GetFrameRate();

        pkDesc->m_spBirthRateInterp = BuildFloatInterp(pkPBlock, 
            PB_PCLD_PBIRTHRATE, pkDesc->m_eBirthRateInterpCycle, fBRScalar);
    }
    pkDesc->m_spLifeSpanInterp = BuildFloatInterp(pkPBlock, PB_PCLD_LIFE,
        pkDesc->m_eLifeSpanInterpCycle, sfTimeFactor);
    pkDesc->m_spSpeedInterp = BuildFloatInterp(pkPBlock, PB_PCLD_SPEED, 
        pkDesc->m_eSpeedInterpCycle, (float) GetFrameRate());
    pkDesc->m_spSizeInterp = BuildFloatInterp(pkPBlock, PB_PCLD_SIZE,
        pkDesc->m_eSizeInterpCycle);
    pkDesc->m_spSpinPhaseInterp = BuildFloatInterp(pkPBlock, 
        PB_PCLD_SPINPHASE, pkDesc->m_eSpinPhaseInterpCycle);
    pkDesc->m_spSpinPhaseVarInterp = BuildFloatInterp(pkPBlock, 
        PB_PCLD_SPINPHASEVAR, pkDesc->m_eSpinPhaseVarInterpCycle);

    CHECK_MEMORY();

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractOldPSys(
    INode*, 
    SimpleParticle* pkPart,
    NiPSysDesc* pkDesc)
{
    CHECK_MEMORY();
    Class_ID eID = pkPart->ClassID();
    IParamBlock* pkPBlock = pkPart->pblock;

    // Particles section
    int iViewportNum, iRenderNum;
    Interval intForever = FOREVER;

    pkPBlock->GetValue(PB_VPTPARTICLES, m_kAnimStart, iViewportNum, intForever);
    pkPBlock->GetValue(PB_RNDPARTICLES, m_kAnimStart, iRenderNum, intForever);
    float fSize;
    pkPBlock->GetValue(PB_DROPSIZE, m_kAnimStart, fSize, intForever);
    float fSpeed, fSpeedVar;
    pkPBlock->GetValue(PB_SPEED, m_kAnimStart, fSpeed, intForever);
    pkPBlock->GetValue(PB_VARIATION, m_kAnimStart, fSpeedVar, intForever);
    float fTumbleAmount = 0.0f, fTumbleRate = 0.0f;
    if (eID == Class_ID(SNOW_CLASS_ID, 0))
    {
        pkPBlock->GetValue(PB_TUMBLE, m_kAnimStart, fTumbleAmount, intForever);
        pkPBlock->GetValue(PB_SCALE, m_kAnimStart, fTumbleRate, intForever);
    }

    // Render
    int iDisplayType;
    pkPBlock->GetValue(PB_DISPTYPE, m_kAnimStart, iDisplayType, intForever);

    // Timing
    TimeValue kEmitStart, kLifeSpan;
    pkPBlock->GetValue(PB_STARTTIME, m_kAnimStart, kEmitStart, intForever);
    pkPBlock->GetValue(PB_LIFETIME, m_kAnimStart, kLifeSpan, intForever);
    float fBirthRate;
    pkPBlock->GetValue(PB_BIRTHRATE, m_kAnimStart, fBirthRate, intForever);
    
    int iMaxSustainable;
    pkPBlock->GetValue(PB_CONSTANT, m_kAnimStart, iMaxSustainable, intForever);

    // Emitter
    float fWidth, fHeight;
    pkPBlock->GetValue(PB_EMITTERWIDTH, m_kAnimStart, fWidth, intForever);
    pkPBlock->GetValue(PB_EMITTERHEIGHT, m_kAnimStart, fHeight, intForever);
    int iHideEmitter;
    pkPBlock->GetValue(PB_HIDEEMITTER, m_kAnimStart, iHideEmitter, intForever);

    // Unknown
    int iRender;
    pkPBlock->GetValue(PB_RENDER, m_kAnimStart, iRender, intForever);

    // Set up parameters
    pkDesc->m_fSpeed = 0.4f * fSpeed * GetFrameRate();
    pkDesc->m_fSpeedVar = 0.4f * fSpeedVar * GetFrameRate();

    pkDesc->m_fDirDeclination = NI_PI;
    pkDesc->m_fDirDeclinationVar = NiATan(fSpeedVar / (2.0f * fSpeed));
    pkDesc->m_fDirPlanarAngle = 0.0f;
    pkDesc->m_fDirPlanarAngleVar = 0.0f;

    pkDesc->m_fSize = fSize;

    pkDesc->m_fEmitStart = kEmitStart * sfTimeFactor;
    pkDesc->m_fEmitStop = m_kAnimEnd * sfTimeFactor;
 
    pkDesc->m_fBirthRate = fBirthRate / sfTimeFactor;
    pkDesc->m_fLifeSpan = kLifeSpan * sfTimeFactor;
    pkDesc->m_bUseBirthRate = true;
    pkDesc->m_fEmitterWidth = fWidth;
    pkDesc->m_fEmitterHeight = fHeight;
    pkDesc->m_fEmitterDepth = 0.0f;

    pkDesc->m_usViewportTotal = (unsigned short)iViewportNum;
    pkDesc->m_usRendererTotal = (unsigned short)iRenderNum;

    if (iMaxSustainable  != 0 && pkDesc->m_bNiPSysUseViewport)
    {
        pkDesc->m_fBirthRate = iViewportNum  / pkDesc->m_fLifeSpan;
    }
    else if (iMaxSustainable  != 0)
    {
        pkDesc->m_fBirthRate = iRenderNum  / pkDesc->m_fLifeSpan;
    }
    else
    {
        pkDesc->m_spBirthRateInterp = BuildFloatInterp(pkPBlock, PB_BIRTHRATE,
            pkDesc->m_eBirthRateInterpCycle, 160.0f * (float)GetFrameRate());
    }
    
    pkDesc->m_fSpinTime = fTumbleAmount * fTumbleRate;
    pkDesc->m_bRandomSpinAxis = true;

    if (eID == Class_ID(SNOW_CLASS_ID, 0) && iRender < 2)
    {
        NiMesh* pkResult;

        // Build flake
        BuildFlake(&pkResult, fSize);

        pkDesc->m_usNumMeshes = 1;
        pkDesc->m_pspMeshes = NiNew NiAVObjectPtr[1];
        pkDesc->m_pspMeshes[0] = pkResult;
    }

    // Extract animations
    pkDesc->m_spLifeSpanInterp = BuildFloatInterp(pkPBlock, PB_LIFETIME,
        pkDesc->m_eLifeSpanInterpCycle, sfTimeFactor);
    pkDesc->m_spSpeedInterp = BuildFloatInterp(pkPBlock, PB_SPEED,
        pkDesc->m_eSpeedInterpCycle, 0.4f *  GetFrameRate());
    pkDesc->m_spSizeInterp = BuildFloatInterp(pkPBlock, PB_DROPSIZE,
        pkDesc->m_eSizeInterpCycle);

    CHECK_MEMORY();
    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::BuildFlake(NiMesh** ppkResult, float fSize)
{
    *ppkResult = NiNew NiMesh();

    if (*ppkResult == NULL) 
        return(W3D_STAT_NO_MEMORY);

    (*ppkResult)->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);

    int iNumVerts = 8;
     //position
    NiDataStreamElementLock kPositionLock = (*ppkResult)->AddStreamGetLock(
        NiCommonSemantics::POSITION(), 
        0,
        NiDataStreamElement::F_FLOAT32_3,   
        iNumVerts, 
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX,
        true); 

    NIASSERT(kPositionLock.IsLocked()); 

    NiTStridedRandomAccessIterator<NiPoint3> kPositionIterator =
        kPositionLock.begin<NiPoint3>(0); 

    //indices
    NiDataStreamElementLock kIndexLock = (*ppkResult)->AddStreamGetLock(
        NiCommonSemantics::INDEX(), 
        0,
        NiDataStreamElement::F_UINT32_1,
        iNumVerts+4, 
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX_INDEX,
        true);

    NIASSERT(kIndexLock.IsLocked()); 

    NiTStridedRandomAccessIterator<unsigned int> kIndexIterator
        = kIndexLock.begin<unsigned int>(0); 

    //normal stream
    NiDataStreamElementLock kNormalLock = (*ppkResult)->AddStreamGetLock(
        NiCommonSemantics::NORMAL(),
        0, 
        NiDataStreamElement::F_FLOAT32_3, 
        iNumVerts,
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX,
        true);

    NIASSERT(kNormalLock.IsLocked()); 

    NiTStridedRandomAccessIterator<NiPoint3> kNormalIterator
        = kNormalLock.begin<NiPoint3>(0); 

    NiDataStreamElementLock kTexCoordLock = (*ppkResult)->AddStreamGetLock(
        NiCommonSemantics::TEXCOORD(), 
        0,
        NiDataStreamElement::F_FLOAT32_2,
        iNumVerts, 
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX,
        true);

    NIASSERT(kTexCoordLock.IsLocked()); 

    NiTStridedRandomAccessIterator<NiPoint2> kTexCoordIterator =
        kTexCoordLock.begin<NiPoint2>(0); 

    //lets create a flake
    kPositionIterator[0] = kPositionIterator[4]
        = NiPoint3(-fSize, 0.0f, -fSize);
    kPositionIterator[1] = kPositionIterator[5]
        = NiPoint3(fSize, 0.0f, -fSize);
    kPositionIterator[2] = kPositionIterator[6]
        = NiPoint3(fSize, 0.0f, fSize);
    kPositionIterator[3] = kPositionIterator[7]
        = NiPoint3(-fSize, 0.0f, fSize);

    for (unsigned int i = 0; i < 4; i++)
    {
        kNormalIterator[i] = NiPoint3(0.0f, 1.0f, 0.0f);
        kNormalIterator[i+4] = NiPoint3(0.0f, -1.0f, 0.0f);
    }

    kTexCoordIterator[0] = kTexCoordIterator[4] = NiPoint2(0.0f, 1.0f);
    kTexCoordIterator[1] = kTexCoordIterator[5] = NiPoint2(1.0f, 1.0f);
    kTexCoordIterator[2] = kTexCoordIterator[6] = NiPoint2(1.0f, 0.0f);
    kTexCoordIterator[3] = kTexCoordIterator[7] = NiPoint2(0.0f, 0.0f);

    kIndexIterator[0] = 0;
    kIndexIterator[1] = 1;
    kIndexIterator[2] = 2;

    kIndexIterator[3] = 0;
    kIndexIterator[4] = 2;
    kIndexIterator[5] = 3;

    kIndexIterator[6] = 4;
    kIndexIterator[7] = 6;
    kIndexIterator[8] = 5;

    kIndexIterator[9] = 4;
    kIndexIterator[10] = 7;
    kIndexIterator[11] = 6;

    (*ppkResult)->SetName("Flakes");
    (*ppkResult)->SetSubmeshCount(1); 
    (*ppkResult)->RecomputeBounds(); 
    (*ppkResult)->Update(0.0);

    // Setup the mesh profile extra data, just use the default mesh profile
    // for the scene since this geometry is generated by the plugin
    TSTR pcPropBuffer;
    NiString pcDefaultProfileName = 
        NiMAXOptions::GetValue(NI_SCENEMESHPROFILENAME);
    
    const char *pcProfilePropertyName = "MeshProfileName";

    NiStringExtraData* pkMeshProfileExtraData;   
    pkMeshProfileExtraData = 
            NiNew NiStringExtraData((const char*)pcDefaultProfileName);

    pkMeshProfileExtraData->SetName(pcProfilePropertyName);

    (*ppkResult)->AddExtraData(pkMeshProfileExtraData);

    kPositionLock.Unlock();
    kIndexLock.Unlock();
    kNormalLock.Unlock();
    kTexCoordLock.Unlock();

    return(W3D_STAT_OK);
}

//---------------------------------------------------------------------------
void NiMAXPSysConverter::MergeBirthRateTrackWithActiveKeyTrack(
    unsigned int& uiNumKeys,
    NiStepBoolKey*& pkBoolKeys, 
    unsigned int uiNumBirthRateKeys,
    NiFloatKey* pkBRKeys, 
    NiFloatKey::KeyType eBirthRateKeyType)
{

    if (uiNumBirthRateKeys == 1)
        return;

    unsigned int uiNumEmitterActiveKeys;
    NiStepBoolKey* pkEmitterActiveKeys;

    SetEmitterControllerStartAndStop(uiNumEmitterActiveKeys, 
        pkEmitterActiveKeys, uiNumBirthRateKeys, pkBRKeys,
        eBirthRateKeyType);

    unsigned int uiFinalKeyCount = uiNumKeys + uiNumEmitterActiveKeys;
    NiStepBoolKey* pkFinalBoolKeys = NiNew NiStepBoolKey[uiFinalKeyCount];
    unsigned int uiIndex = 0;
    unsigned int uiTrack1Index = 0;
    unsigned int ui = 0;

    for (; ui < uiNumKeys; ui++)
    {
        pkFinalBoolKeys[uiIndex].SetTime(pkBoolKeys[ui].GetTime());
        pkFinalBoolKeys[uiIndex].SetBool(pkBoolKeys[ui].GetBool());
        uiIndex++;
        uiTrack1Index++;

        if (pkBoolKeys[ui].GetBool() == true)
            break;    
    }

    for (unsigned int uiTrack2Index = 0; uiTrack2Index < 
        uiNumEmitterActiveKeys; uiTrack2Index++)
    {
        if (pkEmitterActiveKeys[uiTrack2Index].GetTime() < 
            pkFinalBoolKeys[uiIndex-1].GetTime())
        {
            continue;
        }
        else if(pkEmitterActiveKeys[uiTrack2Index].GetTime() == 
            pkFinalBoolKeys[uiIndex-1].GetTime())
        {
            uiIndex--;
        }

        if (pkEmitterActiveKeys[uiTrack2Index].GetTime() >=
            pkBoolKeys[uiTrack1Index].GetTime())
        {
            break;
        }

        pkFinalBoolKeys[uiIndex].SetTime(
            pkEmitterActiveKeys[uiTrack2Index].GetTime());
        pkFinalBoolKeys[uiIndex].SetBool(
            pkEmitterActiveKeys[uiTrack2Index].GetBool());
        uiIndex++;
    }

    for (ui = uiTrack1Index; ui < uiNumKeys; ui++)
    {
        pkFinalBoolKeys[uiIndex].SetTime(pkBoolKeys[ui].GetTime());
        pkFinalBoolKeys[uiIndex].SetBool(pkBoolKeys[ui].GetBool());
        uiIndex++;
    }

    NiDelete [] pkBoolKeys;
    NiDelete [] pkEmitterActiveKeys;
    uiNumKeys = uiIndex;
    pkBoolKeys = pkFinalBoolKeys;
}
//---------------------------------------------------------------------------
NiPSEmitter* NiMAXPSysConverter::BuildEmitterInfo(
    NiPSysDesc* pkDesc, 
    NiPSParticleSystem* pkPSystem)
{
    NiAVObject* pkMAXEmitterObj = NULL;
    char acName[256];
    
    NiPSEmitter* pkEmitter = NULL;
    bool bCreateLinkup = true;

    switch (pkDesc->m_eEmitterType)
    {
        default:
        case EMITTER_VOLUME:
            NiSprintf(acName, 256, "NiPSysBoxEmitter:%d", ms_uiTotalCount);
            pkEmitter = NiNew NiPSBoxEmitter(
                acName, 
                pkDesc->m_fEmitterWidth, 
                pkDesc->m_fEmitterHeight,
                pkDesc->m_fEmitterDepth,
                pkMAXEmitterObj);
            break;
        case EMITTER_OBJECT:
            NiSprintf(acName, 256, "NiPSysMeshEmitter:%d", ms_uiTotalCount);
            pkEmitter = NiNew NiPSMeshEmitter(
                acName, NULL, pkDesc->m_eEmissionType, 
                pkDesc->m_eInitVelocityType,
                NiPoint3::UNIT_X);
            break;
        case EMITTER_SPHERE:
            NiSprintf(acName, 256, "NiPSysSphereEmitter:%d", ms_uiTotalCount);
            pkEmitter = NiNew NiPSSphereEmitter(
                acName,
                pkDesc->m_fEmitterHeight,
                pkMAXEmitterObj);
            break;
        case EMITTER_BOX:
            NiSprintf(acName, 256, "NiPSysBoxEmitter:%d", ms_uiTotalCount);
            pkEmitter = NiNew NiPSBoxEmitter(
                acName, 
                pkDesc->m_fEmitterWidth, 
                pkDesc->m_fEmitterHeight,
                pkDesc->m_fEmitterDepth,
                pkMAXEmitterObj);
            break;
        case EMITTER_CYLINDER:
            NiSprintf(acName, 256, "NiPSysCylinderEmitter:%d", 
                ms_uiTotalCount);
            pkEmitter = NiNew NiPSCylinderEmitter(
                acName, 
                pkDesc->m_fEmitterHeight / 2.0f, 
                pkDesc->m_fEmitterDepth,
                pkMAXEmitterObj);
            break;
    };

    NIASSERT(pkEmitter);
    pkEmitter->SetSpeed(pkDesc->m_fSpeed);
    pkEmitter->SetSpeedVar(pkDesc->m_fSpeedVar);
    pkEmitter->SetDeclination(pkDesc->m_fDirDeclination);
    pkEmitter->SetDeclinationVar(pkDesc->m_fDirDeclinationVar);
    pkEmitter->SetPlanarAngle(pkDesc->m_fDirPlanarAngle);
    pkEmitter->SetPlanarAngleVar(pkDesc->m_fDirPlanarAngleVar);
    pkEmitter->SetLifeSpan(pkDesc->m_fLifeSpan);
    pkEmitter->SetLifeSpanVar(pkDesc->m_fLifeSpanVar);
    pkEmitter->SetRadius(pkDesc->m_fSize);
    pkEmitter->SetRadiusVar(pkDesc->m_fSizeVar);

    // Put particle control and emitter on a list to resolve emitter pointer 
    //   after all the nodes are converted
    if (bCreateLinkup)
    {
        ParticleLinkUpInfo* pkParticleLinkUpInfo = NiNew ParticleLinkUpInfo;
        pkParticleLinkUpInfo->m_pkMAXNode = pkDesc->m_pkMAXEmitter;
        pkParticleLinkUpInfo->m_spSystem = pkPSystem;
        pkParticleLinkUpInfo->m_spEmitter = pkEmitter;
        pkParticleLinkUpInfo->m_spSimulator = NULL;
        ms_kLinkUpList.AddHead(pkParticleLinkUpInfo);
    }
    pkPSystem->AddEmitter(pkEmitter);
    ms_uiTotalCount++;
    pkDesc->m_spEmitter = pkEmitter;

    unsigned int uiNumBirthRateKeys = 1;
    NiFloatKey* pkBRKeys = NULL;
    NiAnimationKey::KeyType eType;

    if (pkDesc->m_spBirthRateInterp)
    {
        unsigned char ucSize;
        NiFloatKey* pkOldBRKeys = (NiFloatKey*) 
            pkDesc->m_spBirthRateInterp->GetAnim(
            uiNumBirthRateKeys, eType, ucSize);

        NiAnimationKey::ArrayFunction pArrayFunction = 
            NiAnimationKey::GetArrayFunction(NiAnimationKey::FLOATKEY, eType);
        pkBRKeys = (NiFloatKey*) pArrayFunction(uiNumBirthRateKeys);

        NiAnimationKey::CopyFunction pCopyFunc = 
            NiAnimationKey::GetCopyFunction(NiAnimationKey::FLOATKEY, eType);
        for (unsigned int ui = 0; ui < uiNumBirthRateKeys; ui++)
        {
            NiFloatKey* pkNewKey = pkBRKeys->GetKeyAt(ui, ucSize);
            NiFloatKey* pkOldKey = pkOldBRKeys->GetKeyAt(ui, ucSize);
            pCopyFunc(pkNewKey, pkOldKey);
        }
    }
    else
    {
        NiLinFloatKey* pkBRTempKeys = NiNew NiLinFloatKey[1];
        pkBRTempKeys[0].SetTime(pkDesc->m_fEmitStart);
        pkBRTempKeys[0].SetValue(pkDesc->m_fBirthRate);
        uiNumBirthRateKeys = 1;
        eType = NiAnimationKey::LINKEY;
        pkBRKeys = pkBRTempKeys;
    }

    unsigned int uiNumKeys = 0;
    
    NiStepBoolKey* pkBoolKeys = BuildActiveKeys(pkDesc->m_fNiPSysLoopStart, 
        pkDesc->m_fNiPSysLoopEnd, pkDesc->m_fEmitStart, pkDesc->m_fEmitStop, 
        uiNumKeys);  

    MergeBirthRateTrackWithActiveKeyTrack(uiNumKeys, pkBoolKeys, 
        uiNumBirthRateKeys, pkBRKeys, eType);

    NiBoolInterpolator* pkEmitInterp = NiNew NiBoolInterpolator();
    NiFloatInterpolator* pkBirthRateInterp = NiNew NiFloatInterpolator();
    pkEmitInterp->SetKeys(pkBoolKeys, uiNumKeys, NiAnimationKey::STEPKEY);
    pkBirthRateInterp->SetKeys(pkBRKeys, uiNumBirthRateKeys, eType);

    NiPSEmitParticlesCtlr* pkEmitController = 
        NiNew NiPSEmitParticlesCtlr(pkEmitter->GetName());
    pkEmitController->SetBirthRateInterpolator(pkBirthRateInterp);
    pkEmitController->SetEmitterActiveInterpolator(pkEmitInterp);
    pkEmitController->ResetTimeExtrema();

    pkEmitInterp->Collapse();
    pkBirthRateInterp->Collapse();
    
    if (pkDesc->m_bNiPSysLoop)
        pkEmitController->SetCycleType(NiTimeController::LOOP);
    else
        pkEmitController->SetCycleType(NiTimeController::CLAMP);
    pkEmitController->SetTarget(pkPSystem);
    
    if (pkDesc->m_spSpeedInterp)
    {
        NiPSEmitterSpeedCtlr* pkEmitSpeedController = 
            NiNew NiPSEmitterSpeedCtlr(pkEmitter->GetName());
        pkEmitSpeedController->SetInterpolator(pkDesc->m_spSpeedInterp);
        pkEmitSpeedController->ResetTimeExtrema();
        pkEmitSpeedController->SetCycleType(pkDesc->m_eSpeedInterpCycle);
        pkEmitSpeedController->SetTarget(pkPSystem);
    }
    if (pkDesc->m_spDeclinationInterp)
    {
        NiPSEmitterDeclinationCtlr* pkEmitDeclinationController = 
            NiNew NiPSEmitterDeclinationCtlr(pkEmitter->GetName());
        pkEmitDeclinationController->SetInterpolator(
            pkDesc->m_spDeclinationInterp);
        pkEmitDeclinationController->ResetTimeExtrema();
        pkEmitDeclinationController->SetCycleType(
            pkDesc->m_eDeclinationInterpCycle);
        pkEmitDeclinationController->SetTarget(pkPSystem);
    }
    if (pkDesc->m_spDeclinationVarInterp)
    {
        NiPSEmitterDeclinationVarCtlr* pkEmitDeclinationVarController = 
            NiNew NiPSEmitterDeclinationVarCtlr(pkEmitter->GetName());
        pkEmitDeclinationVarController->SetInterpolator(
            pkDesc->m_spDeclinationVarInterp);
        pkEmitDeclinationVarController->ResetTimeExtrema();
        pkEmitDeclinationVarController->SetCycleType(
            pkDesc->m_eDeclinationVarInterpCycle);
        pkEmitDeclinationVarController->SetTarget(pkPSystem);
    }
    if (pkDesc->m_spPlanarAngleInterp)
    {
        NiPSEmitterPlanarAngleCtlr* pkEmitPlanarAngleController = 
            NiNew NiPSEmitterPlanarAngleCtlr(pkEmitter->GetName());
        pkEmitPlanarAngleController->SetInterpolator(
            pkDesc->m_spPlanarAngleInterp);
        pkEmitPlanarAngleController->ResetTimeExtrema();
        pkEmitPlanarAngleController->SetCycleType(
            pkDesc->m_ePlanarAngleInterpCycle);
        pkEmitPlanarAngleController->SetTarget(pkPSystem);
    }
    if (pkDesc->m_spPlanarAngleVarInterp)
    {
        NiPSEmitterPlanarAngleVarCtlr* pkEmitPlanarAngleVarController = 
            NiNew NiPSEmitterPlanarAngleVarCtlr(pkEmitter->GetName());
        pkEmitPlanarAngleVarController->SetInterpolator(
            pkDesc->m_spPlanarAngleVarInterp);
        pkEmitPlanarAngleVarController->ResetTimeExtrema();
        pkEmitPlanarAngleVarController->SetCycleType(
            pkDesc->m_ePlanarAngleVarInterpCycle);
        pkEmitPlanarAngleVarController->SetTarget(pkPSystem);
    }
    if (pkDesc->m_spSizeInterp)
    {
        NiPSEmitterRadiusCtlr* pkEmitSizeController = 
            NiNew NiPSEmitterRadiusCtlr(pkEmitter->GetName());
        pkEmitSizeController->SetInterpolator(pkDesc->m_spSizeInterp);
        pkEmitSizeController->ResetTimeExtrema();
        pkEmitSizeController->SetCycleType(pkDesc->m_eSizeInterpCycle);
        pkEmitSizeController->SetTarget(pkPSystem);
    }
    if (pkDesc->m_spLifeSpanInterp)
    {
        NiPSEmitterLifeSpanCtlr* pkEmitLifeSpanController = 
            NiNew NiPSEmitterLifeSpanCtlr(pkEmitter->GetName());
        pkEmitLifeSpanController->SetInterpolator(
            pkDesc->m_spLifeSpanInterp);
        pkEmitLifeSpanController->ResetTimeExtrema();
        pkEmitLifeSpanController->SetCycleType(
            pkDesc->m_eLifeSpanInterpCycle);
        pkEmitLifeSpanController->SetTarget(pkPSystem);
    }

    return pkEmitter;
}
//---------------------------------------------------------------------------
NiPSSpawner* NiMAXPSysConverter::BuildSpawnInfo(
    NiPSysDesc* pkDesc,
    NiPSParticleSystem* pkPSystem)
{
    unsigned short usMinNumToSpawn = pkDesc->m_usMultiplier - (unsigned short)
        (pkDesc->m_fMultiplierVar * pkDesc->m_usMultiplier);
    if (usMinNumToSpawn == 0)
    {
        usMinNumToSpawn = 1;
    }

    char acName[256];
    NiSprintf(acName, 256, "NiPSSpawner:%d", ms_uiTotalCount);
    NiPSSpawner* pkSpawnModifier = NiNew NiPSSpawner(
        pkDesc->m_usNumGenerations, 
        pkDesc->m_fPercentageSpawned,
        usMinNumToSpawn, 
        pkDesc->m_usMultiplier, 
        pkDesc->m_fSpeedChaos,
        pkDesc->m_fDirChaos, 
        pkDesc->m_fLifeSpan, 
        pkDesc->m_fLifeSpanVar);

    pkDesc->m_pkSpawnModifier = pkSpawnModifier;

    pkPSystem->AddSpawner(pkSpawnModifier);
    ms_uiTotalCount++;

    return pkSpawnModifier;
}
//---------------------------------------------------------------------------
bool NiMAXPSysConverter::BuildAgeBasedDeathInfo(
    NiPSysDesc* pkDesc, 
    NiPSParticleSystem* pkPSystem)
{
    if(pkDesc->m_bSpawnOnDeath)
    {
        char acName[256];
        NiSprintf(acName, 256, "NiPSysAgeDeath:%d", ms_uiTotalCount);

        NiPSSpawner* pkSpawnModifier = BuildSpawnInfo(pkDesc, pkPSystem);

        pkPSystem->SetDeathSpawner(pkSpawnModifier);        

        ms_uiTotalCount++;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiMAXPSysConverter::BuildGrowFadeInfo(
    NiPSysDesc* pkDesc, 
    NiPSParticleSystem* pkPSystem)
{
    if (pkDesc->m_fGrowFor > 0.0f || pkDesc->m_fFadeFor > 0.0f)
    {
        NiPSSimulatorGeneralStep* pkGeneralStep = NULL;
        
        pkGeneralStep = 
            NiGetSimulatorStep(NiPSSimulatorGeneralStep,
            pkPSystem->GetSimulator());

        NIASSERT(pkGeneralStep);
        if(pkGeneralStep)
        {
            pkGeneralStep->SetGrowTime(pkDesc->m_fGrowFor);
            pkGeneralStep->SetGrowGeneration(0);
            pkGeneralStep->SetShrinkTime(pkDesc->m_fFadeFor);
            pkGeneralStep->SetShrinkGeneration(pkDesc->m_usNumGenerations);
        }

        ms_uiTotalCount++;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiMAXPSysConverter::BuildColorModInfo(
    NiPSysDesc* pkDesc, 
    NiPSParticleSystem* pkPSystem)
{   
    if (pkDesc->m_bColorAges)
    {
        char acName[256];
        NiSprintf(acName, 256, "NiPSysColorModifier:%d", ms_uiTotalCount);
        ms_uiTotalCount++;

        NiPSSimulatorGeneralStep* pkGeneralStep = NULL;

        pkGeneralStep = 
            NiGetSimulatorStep(NiPSSimulatorGeneralStep,
            pkPSystem->GetSimulator());
        NIASSERT(pkGeneralStep);
        if(pkGeneralStep)
        {
            NiPSKernelColorKey* pKeys = pkDesc->m_pkColors;
            NIASSERT(pKeys);

            pkGeneralStep->CopyColorKeys(pKeys, (NiUInt8) pkDesc->m_uiNumColorKeys);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void RecursivelyUnhide(NiAVObject* pkObj)
{
    if (!pkObj)
        return;

    pkObj->SetAppCulled(false);
    if (NiIsKindOf(NiNode, pkObj))
    {
        NiNode* pkNode = (NiNode*) pkObj;
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
            RecursivelyUnhide(pkNode->GetAt(ui));
    }
}
//---------------------------------------------------------------------------
bool NiMAXPSysConverter::BuildMeshModInfo(
    NiPSysDesc* pkDesc, 
    NiPSParticleSystem* pkPSystem)
{   
    if (pkDesc->m_usNumMeshes > 0)
    {
        char acName[256];
        NiSprintf(acName, 256, "NiPSysMeshUpdateModifier:%d", 
            ms_uiTotalCount);
            
        NiPSMeshParticleSystem* pkMeshPSystem = (NiPSMeshParticleSystem*)
            pkPSystem;

        for (unsigned int i = 0; i < pkDesc->m_usNumMeshes; i++)
        {

            pkMeshPSystem->SetMasterParticle((NiUInt16)i, pkDesc->m_pspMeshes[i]);
            if (pkDesc->m_pspMeshes[i])
                RecursivelyUnhide(pkDesc->m_pspMeshes[i]);
        }
        ms_uiTotalCount++;
    }
    return true;
}
//---------------------------------------------------------------------------
void ScaleFloatInterpolator(NiFloatInterpolator* pkInterp, float fScalar)
{
    if (pkInterp)
    {
        if (fScalar != 1.0f)
        {
            for (unsigned int ui = 0; ui < pkInterp->GetKeyCount(0); ui++)
            {
                NiFloatKey* pkKey = (NiFloatKey*)pkInterp->GetKeyAt(ui,0);
                pkKey->SetValue(pkKey->GetValue() * fScalar);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiMAXPSysConverter::BuildRotationInfo(
    NiPSysDesc* pkDesc, 
    NiPSParticleSystem* pkPSystem,
    NiPSEmitter* pkEmitter)
{   
    if (pkDesc->m_fNiPSysRotationSpeed != 0.0f || 
        pkDesc->m_fNiPSysRotationVariation != 0.0f ||
        pkDesc->m_fSpinPhase != 0.0f ||
        pkDesc->m_fSpinPhaseVar != 0.0f  ||
        pkDesc->m_spRotationSpeedInterp ||
        pkDesc->m_spRotationVariationInterp ||
        pkDesc->m_spSpinPhaseInterp ||
        pkDesc->m_spSpinPhaseVarInterp)
    {
        char acName[256];
        NiSprintf(acName, 256, "NiPSysRotationModifier:%d", ms_uiTotalCount);

        pkEmitter->SetRandomRotAxis(pkDesc->m_bRandomSpinAxis);
        pkDesc->m_kSpinAxis.Unitize();
        pkEmitter->SetRotAxis(pkDesc->m_kSpinAxis);
        
        pkEmitter->SetRotSpeed(
            pkDesc->m_fNiPSysRotationSpeed * (NI_TWO_PI / 360.0f));
        pkEmitter->SetRotSpeedVar(
            pkDesc->m_fNiPSysRotationVariation * (NI_TWO_PI / 360.0f));
        pkEmitter->SetRotAngle(-pkDesc->m_fSpinPhase);
        pkEmitter->SetRotAngleVar(pkDesc->m_fSpinPhaseVar 
            * pkDesc->m_fSpinPhase);
        pkEmitter->SetRandomRotSpeedSign(
            pkDesc->m_bNiPSysBidirectionalRotation);

        if (pkDesc->m_spRotationSpeedInterp)
        {
            ScaleFloatInterpolator(pkDesc->m_spRotationSpeedInterp, 
                (NI_TWO_PI / 360.0f));
            NiPSEmitterRotSpeedCtlr* pkRotationSpeedController = 
                NiNew NiPSEmitterRotSpeedCtlr(pkEmitter->GetName());
            pkRotationSpeedController->SetInterpolator(
                pkDesc->m_spRotationSpeedInterp);
            pkRotationSpeedController->ResetTimeExtrema();
            pkRotationSpeedController->SetCycleType(
                pkDesc->m_eRotationSpeedInterpCycle);
            pkRotationSpeedController->SetTarget(pkPSystem);
        }
        if (pkDesc->m_spRotationVariationInterp)
        {
            ScaleFloatInterpolator(pkDesc->m_spRotationVariationInterp,
                (NI_TWO_PI / 360.0F));
            NiPSEmitterRotSpeedVarCtlr* pkRotationSpeedVarController = 
                NiNew NiPSEmitterRotSpeedVarCtlr(pkEmitter->GetName());
            pkRotationSpeedVarController->SetInterpolator(
                pkDesc->m_spRotationVariationInterp);
            pkRotationSpeedVarController->ResetTimeExtrema();
            pkRotationSpeedVarController->SetCycleType(
                pkDesc->m_eRotationVariationInterpCycle);
            pkRotationSpeedVarController->SetTarget(pkPSystem);
        }
        if (pkDesc->m_spSpinPhaseInterp)
        {
            // reverse the direction of our rotation keys
            ScaleFloatInterpolator(pkDesc->m_spSpinPhaseInterp, -1.0f);
            NiPSEmitterRotAngleCtlr* pkInitialRotationController = 
                NiNew NiPSEmitterRotAngleCtlr(pkEmitter->GetName());
            pkInitialRotationController->SetInterpolator(
                pkDesc->m_spSpinPhaseInterp);
            pkInitialRotationController->ResetTimeExtrema();
            pkInitialRotationController->SetCycleType(
                pkDesc->m_eSpinPhaseInterpCycle);
            pkInitialRotationController->SetTarget(pkPSystem);
        }
        if (pkDesc->m_spSpinPhaseVarInterp)
        {
            ScaleFloatInterpolator(pkDesc->m_spSpinPhaseVarInterp,
                pkDesc->m_fSpinPhase);
            NiPSEmitterRotAngleVarCtlr* pkInitialRotationVarController = 
                NiNew NiPSEmitterRotAngleVarCtlr(pkEmitter->GetName());
            pkInitialRotationVarController->SetInterpolator(
                pkDesc->m_spSpinPhaseVarInterp);
            pkInitialRotationVarController->ResetTimeExtrema();
            pkInitialRotationVarController->SetCycleType(
                pkDesc->m_eSpinPhaseVarInterpCycle);
            pkInitialRotationVarController->SetTarget(pkPSystem);
        }

        ms_uiTotalCount++;
    }
    return true;
}

//---------------------------------------------------------------------------

#define PSYSLOOP_STRING         "NiPSysLoop"    
#define PSYSLOOPSTART_STRING    "NiPSysLoopStart"
#define PSYSLOOPEND_STRING      "NiPSysLoopEnd"
#define PSYSLUSEVIEWPORT_STRING "NiPSysUseViewport"
#define PSYSLOOPRESET_STRING    "NiPSysResetOnLoop"
#define PSYSMAXALLOC_STRING     "NiPSysMaxAlloc"
#define PSYSLOCALCOORDS_STRING      "NiPSysLocalCoords"
#define PSYSBOUNDSUPDATETYPE_STRING "NiPSysBoundsUpdateType" 
#define PSYSBOUNDPOSITION_STRING    "NiPSysBoundPosition"
#define PSYSBOUNDRADIUS_STRING      "NiPSysBoundRadius"
#define PSYSBOUNDSDYNAMICSKIP_STRING   "NiPSysDynamicBoundsSkip"
#define PSYSPARENTNODE_STRING   "NiPSysParentNode"
#define PSYSROTATIONSPEED_STRING "NiPSysRotationSpeed"
#define PSYSROTATIONVARIATION_STRING "NiPSysRotationVariation"
#define PSYSBIDIRECTIONALROTATION_STRING "NiPSysBidirectionalRotation"

int NiMAXPSysConverter::ExtractCustomAttribs(
    NiPSysDesc* pkDesc, 
    SimpleParticle* pkPart)
{
    NiNodePtr m_spTempNode = NiNew NiNode;
    NiMAXCustAttribConverter kConverter(m_kAnimStart, m_kAnimEnd);
    kConverter.Convert(pkPart, m_spTempNode, false);
    
    NiBooleanExtraData* pkLoopED = 
        (NiBooleanExtraData*) m_spTempNode->GetExtraData(PSYSLOOP_STRING);
    NIASSERT(pkLoopED);
    if (pkLoopED)
        pkDesc->m_bNiPSysLoop = pkLoopED->GetValue();
    else
        pkDesc->m_bNiPSysLoop = true;

    Interface* pIntf = GetCOREInterface();
    float animStart = (float) pIntf->GetAnimRange().Start();
    float animEnd = (float) pIntf->GetAnimRange().End();

    NiIntegerExtraData* pkLoopStartED = 
        (NiIntegerExtraData*) 
        m_spTempNode->GetExtraData(PSYSLOOPSTART_STRING);
    NIASSERT(pkLoopStartED);
    if (pkLoopStartED)
    {
        pkDesc->m_fNiPSysLoopStart = ((float)pkLoopStartED->GetValue())/
            GetFrameRate();
    }
    else
    {
        pkDesc->m_fNiPSysLoopStart = animStart * sfTimeFactor;
    }

    NiIntegerExtraData* pkLoopEndED = 
        (NiIntegerExtraData*) m_spTempNode->GetExtraData(PSYSLOOPEND_STRING);
    NIASSERT(pkLoopEndED);
    if (pkLoopEndED)
    {
        pkDesc->m_fNiPSysLoopEnd = ((float)pkLoopEndED->GetValue())/
            GetFrameRate();
    }
    else
    {
        pkDesc->m_fNiPSysLoopEnd =  animEnd * sfTimeFactor;
    }

    NiBooleanExtraData* pkUseViewportED = 
        (NiBooleanExtraData*) m_spTempNode->GetExtraData(
        PSYSLUSEVIEWPORT_STRING);
    NIASSERT(pkUseViewportED);
    if (pkUseViewportED)
        pkDesc->m_bNiPSysUseViewport = pkUseViewportED->GetValue();
    else
        pkDesc->m_bNiPSysUseViewport = true;

    NiBooleanExtraData* pkLoopResetED = 
        (NiBooleanExtraData*) m_spTempNode->GetExtraData(
        PSYSLOOPRESET_STRING);
    NIASSERT(pkLoopResetED);
    if (pkLoopResetED)
        pkDesc->m_bNiPSysResetOnLoop = pkLoopResetED->GetValue();
    else
        pkDesc->m_bNiPSysResetOnLoop = true;

    NiIntegerExtraData* pkMaxAllocED = 
        (NiIntegerExtraData*) m_spTempNode->GetExtraData(
        PSYSMAXALLOC_STRING);
    NIASSERT(pkMaxAllocED);
    if (pkMaxAllocED)
        pkDesc->m_usNiPSysMaxAlloc = (unsigned short)pkMaxAllocED->GetValue();
    else
        pkDesc->m_usNiPSysMaxAlloc = 100;
    
    NiBooleanExtraData* pkLocalCoordsED = 
        (NiBooleanExtraData*) m_spTempNode->GetExtraData(
        PSYSLOCALCOORDS_STRING);
    NIASSERT(pkLocalCoordsED);
    if (pkLocalCoordsED)
        pkDesc->m_bLocalCoords = pkLocalCoordsED->GetValue();
    else
        pkDesc->m_bLocalCoords = false;

    NiIntegerExtraData* pkBoundsUpdateTypeED = 
        (NiIntegerExtraData*) m_spTempNode->GetExtraData(
        PSYSBOUNDSUPDATETYPE_STRING);
    NIASSERT(pkBoundsUpdateTypeED);
    if (pkBoundsUpdateTypeED)
        pkDesc->m_iBoundsUpdateType = pkBoundsUpdateTypeED->GetValue();
    else
        pkDesc->m_iBoundsUpdateType = 1;

    NiIntegerExtraData* pkBoundsDynamicSkipED = 
        (NiIntegerExtraData*) m_spTempNode->GetExtraData(
        PSYSBOUNDSDYNAMICSKIP_STRING);
    NIASSERT(pkBoundsDynamicSkipED);
    if (pkBoundsDynamicSkipED)
        pkDesc->m_iBoundsDynamicSkip = pkBoundsDynamicSkipED->GetValue()-1;
    else
        pkDesc->m_iBoundsDynamicSkip = 0;


    NiVectorExtraData* pkBoundPositionED = 
        (NiVectorExtraData*) m_spTempNode->GetExtraData(
        PSYSBOUNDPOSITION_STRING);
    NIASSERT(pkBoundPositionED);
    if (pkBoundPositionED)
    {
        pkDesc->m_kBoundPosition.x = pkBoundPositionED->GetValue()[0];
        pkDesc->m_kBoundPosition.y = pkBoundPositionED->GetValue()[1];
        pkDesc->m_kBoundPosition.z = pkBoundPositionED->GetValue()[2];
    }
    else
    {
        pkDesc->m_kBoundPosition = NiPoint3(0.0f, 0.0f, 0.0f);
    }

    NiBinaryExtraData* pkParentNodeED = 
        (NiBinaryExtraData*) m_spTempNode->GetExtraData(
        PSYSPARENTNODE_STRING);
    NIASSERT(pkParentNodeED);
    if (pkParentNodeED)
    {
        unsigned int uiSize = 0;
        char* pcBinaryInterp = NULL;
        pkParentNodeED->GetValue(uiSize, pcBinaryInterp);
        if (pcBinaryInterp == NULL || uiSize < sizeof(INode*))
        {
            pkDesc->m_pkPSysRoot = NULL;
        }
        else
        {
            unsigned long* pkPointer = (unsigned long*) pcBinaryInterp;
            pkDesc->m_pkPSysRoot = (INode*) ULongToPtr(pkPointer[0]);
        }
    }
    else
    {
        pkDesc->m_pkPSysRoot = NULL;
    }

    NiFloatExtraData* pkBoundsRadiusED = 
        (NiFloatExtraData*) m_spTempNode->GetExtraData(
        PSYSBOUNDRADIUS_STRING);
    NIASSERT(pkBoundsRadiusED);
    if (pkBoundsRadiusED)
        pkDesc->m_fBoundRadius = pkBoundsRadiusED->GetValue();
    else
        pkDesc->m_fBoundRadius = 1.0f;

    NiFloatExtraData* pkRotationSpeedED = 
        (NiFloatExtraData*) 
        m_spTempNode->GetExtraData(PSYSROTATIONSPEED_STRING);
    NIASSERT(pkRotationSpeedED);
    if (pkRotationSpeedED)
    {
        pkDesc->m_fNiPSysRotationSpeed = 
            ((float)pkRotationSpeedED->GetValue());
    }
    else
    {
        pkDesc->m_fNiPSysRotationSpeed = 0;
    }

    NiFloatExtraData* pkRotationVariationED = 
        (NiFloatExtraData*) 
        m_spTempNode->GetExtraData(PSYSROTATIONVARIATION_STRING);
    NIASSERT(pkRotationVariationED);
    if (pkRotationVariationED)
    {
        pkDesc->m_fNiPSysRotationVariation = 
            ((float)pkRotationVariationED->GetValue());
    }
    else
    {
        pkDesc->m_fNiPSysRotationVariation = 0;
    }

    NiBooleanExtraData* pkBidirectionalED = 
        (NiBooleanExtraData*) m_spTempNode->GetExtraData(
        PSYSBIDIRECTIONALROTATION_STRING);
    NIASSERT(pkBidirectionalED);
    if (pkBidirectionalED)
        pkDesc->m_bNiPSysBidirectionalRotation = pkBidirectionalED->GetValue();
    else
        pkDesc->m_bNiPSysBidirectionalRotation = false;
    
    // Now we loop through the object's time controllers to find ones for
    // the animatable extra data
    NiTimeController* pkController = m_spTempNode->GetControllers();
    while (pkController != NULL)
    {
        if (NiIsKindOf(NiFloatExtraDataController, pkController))
        {
            NiFloatExtraDataController* pkEDController = 
                (NiFloatExtraDataController*) pkController;
            const char* pcExtraDataName = pkEDController->GetExtraDataName();
            if (strcmp(pcExtraDataName, PSYSROTATIONSPEED_STRING) == 0)
            {
                pkDesc->m_spRotationSpeedInterp = NiDynamicCast(
                    NiFloatInterpolator, pkEDController->GetInterpolator(0));
                pkDesc->m_eRotationSpeedInterpCycle = 
                    pkEDController->GetCycleType();
            }
            else if (strcmp(pcExtraDataName, PSYSROTATIONVARIATION_STRING) 
                == 0)
            {
                pkDesc->m_spRotationVariationInterp = NiDynamicCast(
                    NiFloatInterpolator, pkEDController->GetInterpolator(0));
                pkDesc->m_eRotationVariationInterpCycle = 
                    pkEDController->GetCycleType();
            }
        }
        pkController = pkController->GetNext();
    }

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
NiNode* GetRoot(NiAVObject* pkObject)
{
    NIASSERT(pkObject);
    while (pkObject->GetParent() != NULL)
    {
        pkObject = pkObject->GetParent();
    }
    if (NiIsKindOf(NiNode, pkObject))
        return (NiNode*)pkObject;
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiPSParticleSystem* NiMAXPSysConverter::CreatePSys(
    NiPSysDesc* pkDesc, 
    SimpleParticle* pkPart,
    float fParentScale,
    NiNode* pkParent)
{
    CHECK_MEMORY();
    NiPSParticleSystem* pkPSystem = NULL;
    // to create the particle system, we need to know 4 things: 
    // how many total particles it has, if it needs a color array, 
    // if it needs rotation angles, and if it is in world space
    unsigned short usNumParticles = pkDesc->m_usNiPSysMaxAlloc;
    bool bUsesColor = pkDesc->m_bColorAges;
    bool bWorldSpace = !pkDesc->m_bLocalCoords;
    bool bDynamicBounds = (pkDesc->m_iBoundsUpdateType == 1);
    bool bUsesRotation = false;

    if (pkDesc->m_fNiPSysRotationSpeed != 0.0f || 
        pkDesc->m_fNiPSysRotationVariation != 0.0f ||
        pkDesc->m_fSpinPhase != 0.0f ||
        pkDesc->m_fSpinPhaseVar != 0.0f  ||
        pkDesc->m_spRotationSpeedInterp ||
        pkDesc->m_spRotationVariationInterp ||
        pkDesc->m_spSpinPhaseInterp ||
        pkDesc->m_spSpinPhaseVarInterp)
    {
        bUsesRotation = true;
    }

    if (pkDesc->m_usNumMeshes != 0)
    {
        unsigned int uiPoolSize = (unsigned int)(0.25f * usNumParticles);
        uiPoolSize = NiMax(uiPoolSize, 10);

        pkPSystem = (NiPSParticleSystem*) NiPSMeshParticleSystem::Create(
            usNumParticles,
            bUsesColor,
            bUsesRotation,
            bWorldSpace,
            bDynamicBounds,
            true,
            uiPoolSize,
            pkDesc->m_usNumMeshes);
    }
    else
    {
        pkPSystem = (NiPSParticleSystem*) NiPSParticleSystem::Create(
            usNumParticles,
            bUsesColor,
            bUsesRotation, 
            bWorldSpace,
            bDynamicBounds,
            true,
            true);
    }

    pkPSystem->SetName(pkPart->GetObjectName());

    NiPSEmitter* pkEmitter = BuildEmitterInfo(pkDesc, pkPSystem);    
    BuildAgeBasedDeathInfo(pkDesc, pkPSystem);
    BuildGrowFadeInfo(pkDesc, pkPSystem);
    BuildColorModInfo(pkDesc, pkPSystem);
    BuildMeshModInfo(pkDesc, pkPSystem);
    BuildRotationInfo(pkDesc, pkPSystem, pkEmitter);

    if (pkDesc->m_usNumMeshes != 0)
    {
        // NiMesh particleSystems
        NiPSMeshParticleSystem* pkMeshPSystem = 
            (NiPSMeshParticleSystem* )pkPSystem;

        if (pkDesc->m_usNumFrags)
        {
            // PArray Fragments
            NIASSERT(usNumParticles == pkDesc->m_usNumFrags);

            for (unsigned int i = 0; i < usNumParticles; i++)
            {
                pkMeshPSystem->SetMasterParticle((NiUInt16)i, pkDesc->m_pspMeshes[i]);
            }          
            bool bFillPoolsOnLoad = false;            
            pkMeshPSystem->SetAutoFillPools(bFillPoolsOnLoad);
        }
    }


    char acName[256];

    fParentScale = fabs(fParentScale);
    pkPSystem->SetScale(fParentScale);
    pkPSystem->SetWorldSpace(bWorldSpace);

    if (pkDesc->m_iBoundsUpdateType == 1)
    {
        NiSprintf(acName, 256, "NiPSysBoundUpdateModifier:%d",
            ms_uiTotalCount);

        pkPSystem->GetBoundUpdater()->SetUpdateSkip((unsigned short) 
            pkDesc->m_iBoundsDynamicSkip);

        ms_uiTotalCount++;
    }
    else if (pkDesc->m_iBoundsUpdateType == 2)
    {
        NiBound kStaticBound;
        NiPoint3 kBoundPosition = pkDesc->m_kBoundPosition;
        float fBoundRadius = pkDesc->m_fBoundRadius;
        kStaticBound.SetCenterAndRadius(kBoundPosition, fBoundRadius);
        ParticleStaticBoundInfo* pkParticleStaticBoundInfo = 
            NiNew ParticleStaticBoundInfo;
        pkParticleStaticBoundInfo->m_spSystem = pkPSystem;
        pkParticleStaticBoundInfo->m_kBound = kStaticBound;
        pkParticleStaticBoundInfo->m_spOldParent = pkParent;
        ms_kStaticBoundList.AddHead(pkParticleStaticBoundInfo);
    }

    if (pkDesc->m_bNiPSysResetOnLoop)
    {
        NiPSResetOnLoopCtlr* pkLoopResetCtrl = NiNew NiPSResetOnLoopCtlr;
        pkLoopResetCtrl->SetBeginKeyTime(pkDesc->m_fNiPSysLoopStart);
        pkLoopResetCtrl->SetEndKeyTime(pkDesc->m_fNiPSysLoopEnd);
        pkLoopResetCtrl->SetTarget(pkPSystem);
    }

    return(pkPSystem);  
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractPlanarDeflector(
    Interface*, 
    Modifier* pkMod,
    INode* pkMaxNode,
    NiPSParticleSystem* pkPSystem, 
    NiPSysDesc* pkDesc,
    ScaleInfo*)
{
    DeflectMod* pkDef = (DeflectMod*)pkMod;

    NiPSSimulatorCollidersStep* pkColliderStep = NULL;
    
    pkColliderStep = NiGetSimulatorStep(NiPSSimulatorCollidersStep,
        pkPSystem->GetSimulator());

    NIASSERT(pkColliderStep);

    if (pkDesc->m_spColliderManager == NULL)
    {
        char acName[256];
        NiSprintf(acName, 256, "NiPSysColliderManagerModifier:%d",
            ms_uiTotalCount);
        pkDesc->m_spColliderManager=pkColliderStep;
        
        ms_uiTotalCount++;
    }
    
    NiPSSimulatorCollidersStep* spCollideManager =
        pkDesc->m_spColliderManager;

    float fBounce, fWidth, fHeight;
    IParamBlock2* pkPBlock2 = pkDef->deflect.obj->pblock2;
    Interval intForever = FOREVER;

    pkPBlock2->GetValue(deflect_bounce,m_kAnimStart,fBounce,intForever);
    pkPBlock2->GetValue(deflect_width,m_kAnimStart,fWidth,intForever);
    pkPBlock2->GetValue(deflect_height,m_kAnimStart,fHeight,intForever);

    Matrix3 kDeflectXform =
        pkDef->deflect.node->GetNodeTM(m_kAnimStart);
    Matrix3 kParticleXform = 
        pkMaxNode->GetNodeTM(m_kAnimStart);
    Matrix3 kDef2PartXform = 
        kDeflectXform*Inverse(kParticleXform);

    Point3 kNorm;
    kNorm.x = 0.0f;
    kNorm.y = 0.0f;
    kNorm.z = 1.0f;
    kNorm = VectorTransform(kDef2PartXform, kNorm);
    kNorm = Normalize(kNorm);

    Point3 kX;
    kX.x = fWidth;
    kX.y = 0.0f;
    kX.z = 0.0f;
    kX = VectorTransform(kDef2PartXform, kX);
    fWidth = Length(kX);
    kX = Normalize(kX);

    Point3 kY;
    kY.x = 0.0f;
    kY.y = fHeight;
    kY.z = 0.0f;
    kY = VectorTransform(kDef2PartXform, kY);
    fHeight = Length(kY);
    kY = Normalize(kY);
        
    Point3 kPos = kDef2PartXform.GetTrans();

    DotProd(kNorm, kDef2PartXform.GetTrans());
    NiPoint3 kNiNorm;
    kNiNorm.x = kNorm.x;
    kNiNorm.y = kNorm.y;
    kNiNorm.z = kNorm.z;

    NiPSPlanarCollider* pkColl = NiNew NiPSPlanarCollider;

    ParticleLinkUpInfo* pkParticleLinkUpInfo = NiNew ParticleLinkUpInfo;
    if (pkParticleLinkUpInfo)
    {
        pkParticleLinkUpInfo->m_pkMAXNode = pkDef->deflect.node;
        pkParticleLinkUpInfo->m_spSystem = pkPSystem;
        pkParticleLinkUpInfo->m_spCollider = pkColl;
        pkParticleLinkUpInfo->m_spSimulator = spCollideManager;
        ms_kLinkUpList.AddHead(pkParticleLinkUpInfo);
    }

    pkColl->SetXAxis(NiPoint3::UNIT_X);
    pkColl->SetYAxis(NiPoint3::UNIT_Y);
    pkColl->SetWidth(fWidth);
    pkColl->SetHeight(fHeight);

    pkColl->SetBounce(fBounce);
    pkColl->SetSpawnOnCollide(pkDesc->m_bSpawnOnCollide);
    pkColl->SetDieOnCollide(pkDesc->m_bDieOnCollide);

    if(pkDesc->m_bSpawnOnCollide)
    {    
        BuildSpawnInfo(pkDesc, pkPSystem);
        pkColl->SetSpawner(pkDesc->m_pkSpawnModifier);    
    }
    spCollideManager->AddCollider(pkColl);

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractSphereDeflector(
    Interface*, 
    Modifier* pkMod,
    INode* pkMaxNode, 
    NiPSParticleSystem* pkPSystem, 
    NiPSysDesc* pkDesc, 
    ScaleInfo*)
{
    SphereDeflectMod* pkSphDef = (SphereDeflectMod*) pkMod;

    NiPSSimulatorCollidersStep* pkColliderStep = NULL;
    
    pkColliderStep = 
        NiGetSimulatorStep(NiPSSimulatorCollidersStep,
        pkPSystem->GetSimulator());

    NIASSERT(pkColliderStep);

    if (pkDesc->m_spColliderManager == NULL)
    {
        char acName[256];
        NiSprintf(acName, 256, "NiPSysColliderManagerModifier:%d",
            ms_uiTotalCount);
    
        pkDesc->m_spColliderManager = pkColliderStep;
        ms_uiTotalCount++;

    }
    
    NiPSSimulatorCollidersStep* spCollideManager = 
        pkDesc->m_spColliderManager;
    
    float fBounce, fChaos, fDiameter, fBounceVar, fVInher;

    IParamBlock2* pkPBlock2 = pkSphDef->deflect.obj->pblock2;
    Interval intForever = FOREVER;

    pkPBlock2->GetValue(sdeflectrobj_bounce, m_kAnimStart, fBounce,
        intForever);
    pkPBlock2->GetValue(sdeflectrobj_bouncevar, m_kAnimStart, fBounceVar,
        intForever);
    pkPBlock2->GetValue(sdeflectrobj_chaos, m_kAnimStart, fChaos,
        intForever);
    pkPBlock2->GetValue(sdeflectrobj_radius, m_kAnimStart, fDiameter,
        intForever);
    pkPBlock2->GetValue(sdeflectrobj_velocity, m_kAnimStart, fVInher,
        intForever);

    Matrix3 kDeflectXform = 
        pkSphDef->deflect.node->GetNodeTM(m_kAnimStart);
    Matrix3 kParticleXform = 
        pkMaxNode->GetNodeTM(m_kAnimStart);

    Point3 kPos = kDeflectXform.GetTrans() - 
        kParticleXform.GetTrans();

    NiPSSphericalCollider* pkColl = NiNew NiPSSphericalCollider;

    ParticleLinkUpInfo* pkParticleLinkUpInfo = NiNew ParticleLinkUpInfo;
    if (pkParticleLinkUpInfo)
    {
        pkParticleLinkUpInfo->m_pkMAXNode = pkSphDef->deflect.node;
        pkParticleLinkUpInfo->m_spSystem = pkPSystem;
        pkParticleLinkUpInfo->m_spCollider = pkColl;
        pkParticleLinkUpInfo->m_spSimulator = spCollideManager;
        ms_kLinkUpList.AddHead(pkParticleLinkUpInfo);
    }

    pkColl->SetRadius(fDiameter * 0.5f);

    pkColl->SetBounce(fBounce);
    pkColl->SetSpawnOnCollide(pkDesc->m_bSpawnOnCollide);
    pkColl->SetDieOnCollide(pkDesc->m_bDieOnCollide);

    if(pkDesc->m_bSpawnOnCollide)
    {    
        BuildSpawnInfo(pkDesc, pkPSystem);
        pkColl->SetSpawner(pkDesc->m_pkSpawnModifier);    
    }
    spCollideManager->AddCollider(pkColl);

    return(W3D_STAT_OK);
}

//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractGravity(
    Interface*,
    Modifier* pkMod, 
    INode* pkMaxNode,
    NiPSParticleSystem* pkPSystem,
    NiPSysDesc*, 
    ScaleInfo*)
{
    GravityMod* pkGrav = (GravityMod*) pkMod;

    NiPSSimulatorForcesStep* pkForcesStep = NULL;
    
    pkForcesStep = 
        NiGetSimulatorStep(NiPSSimulatorForcesStep, pkPSystem->GetSimulator());

    NIASSERT(pkForcesStep);
    
    float fStrength, fDecay, fDispLength;
    int iType;

    IParamBlock2* pkPBlock2 = pkGrav->force.obj->pblock2;
    Interval intForever = FOREVER;

    pkPBlock2->GetValue(PB_GRAV_STRENGTH, m_kAnimStart, fStrength,
        intForever);
    pkPBlock2->GetValue(PB_GRAV_DECAY, m_kAnimStart, fDecay,
        intForever);
    pkPBlock2->GetValue(PB_GRAV_TYPE, m_kAnimStart, iType, intForever);
    pkPBlock2->GetValue(PB_GRAV_DISPLENGTH, m_kAnimStart, 
        fDispLength, intForever);

    // Here we need to get the object transform of the gravity relative to
    // it's pivot.  If the orientation of the pivot has been altered, we
    // must reflect that change in our gravity's axis
    Quat kQuat = pkGrav->force.node->GetObjOffsetRot();
    Matrix3 kObjectTransform(1);
    PreRotateMatrix(kObjectTransform, kQuat);
    Point3 kMaxAxis;
    kMaxAxis.x = 0.0f;
    kMaxAxis.y = 0.0f; 
    kMaxAxis.z = 1.0f;
    kMaxAxis = VectorTransform(kObjectTransform, kMaxAxis);
    kMaxAxis = Normalize(kMaxAxis);

    NiPoint3 kAxis = NiPoint3(kMaxAxis.x, kMaxAxis.y, kMaxAxis.z);
    
    NiAVObject* pkGravityObj = NULL;
    char acName[256];
        NiSprintf(acName, 256, "NiPSysGravityModifier:%d", ms_uiTotalCount);
    NiPSGravityForce* pkNIGrav = 
        NiNew NiPSGravityForce(acName, pkGravityObj);

    if (iType == 0)
        pkNIGrav->SetForceType(
        NiPSForceDefinitions::GravityData::FORCE_PLANAR);
    else
        pkNIGrav->SetForceType(
        NiPSForceDefinitions::GravityData::FORCE_SPHERICAL);
    
    ParticleLinkUpInfo* pkParticleLinkUpInfo = NiNew ParticleLinkUpInfo;
    if (pkParticleLinkUpInfo)
    {
        pkParticleLinkUpInfo->m_pkMAXNode = pkGrav->force.node;
        pkParticleLinkUpInfo->m_spSystem = pkPSystem;
        pkParticleLinkUpInfo->m_spSimulator = pkForcesStep;
        pkParticleLinkUpInfo->m_spCollider = NULL;
        pkParticleLinkUpInfo->m_spForce = pkNIGrav;
        ms_kLinkUpList.AddHead(pkParticleLinkUpInfo);
    }

    pkNIGrav->SetDecay(fDecay);

    float fModifier = 1.0f;
    // Hack to account for behavior differences between
    //   spray/snow and other particle systems
    float fGravity = fStrength * 
        GetFrameRate() * GetFrameRate();
    Class_ID eID = pkMaxNode->GetObjectRef()->ClassID();
    if (eID == Class_ID(RAIN_CLASS_ID, 0) ||
        eID == Class_ID(SNOW_CLASS_ID, 0))
    {
        fModifier = 0.1f;
        fGravity *= 0.1f;
    }

    pkNIGrav->SetStrength(fGravity);
    pkNIGrav->SetGravityAxis(kAxis);

    pkForcesStep->AddForce(pkNIGrav);
    ms_uiTotalCount++;

    NiTimeController::CycleType eCycleType = NiTimeController::CLAMP;
    NiFloatInterpolatorPtr spStrengthInterp = BuildFloatInterp(pkPBlock2, 
        PB_GRAV_STRENGTH, eCycleType, GetFrameRate() * GetFrameRate() * 
        fModifier);

    if (spStrengthInterp)
    {
        NiPSGravityStrengthCtlr* pkGravStrengthController = 
            NiNew NiPSGravityStrengthCtlr(pkNIGrav->GetName());
        pkGravStrengthController->SetInterpolator(spStrengthInterp);
        pkGravStrengthController->ResetTimeExtrema();
        pkGravStrengthController->SetCycleType(eCycleType);
        pkGravStrengthController->SetTarget(pkPSystem);
    }
    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractDrag(
    Interface*,
    Modifier* pkMod, 
    INode*,
    NiPSParticleSystem* pkPSystem,
    NiPSysDesc* pkDesc, 
    ScaleInfo*)
{
    SimpleWSMMod* pkDrag = (SimpleWSMMod*) pkMod;

    NiPSSimulatorForcesStep* pkForcesStep = NULL;
    
    pkForcesStep = 
        NiGetSimulatorStep(NiPSSimulatorForcesStep, pkPSystem->GetSimulator());

    NIASSERT(pkForcesStep);

    float fTimeOn = 0.0f;
    float fTimeOff = 0.0f;
    int iType = 0;

    int X = 0;
    int Y = 1;
    int Z = 2;

    float fDamping[3];
    float fRange[3];
    float fFalloff[3];

    bool bUnlimitedRange = false;

    bool bFound = false;
    if (pkDrag->nodeRef)
    {
        TSTR kName = pkDrag->nodeRef->GetName();
        for (unsigned int ui = 0; ui < (unsigned int )
            pkDrag->nodeRef->NumSubs(); ui++)
        {
            Animatable* pkAnim = pkDrag->nodeRef->SubAnim(ui);
            NiString strName = pkDrag->nodeRef->SubAnimName(ui);

            if (strName == "Object (Drag)")
            {
                bFound = true;
                for (unsigned int uj = 0; uj < (unsigned int)
                    pkAnim->NumParamBlocks(); uj++)
                {
                    IParamBlock2* pkParams = pkAnim->GetParamBlock(uj);
                    if (!pkParams)
                        continue;

                    for (unsigned int up = 0; up < 
                        (unsigned int) pkParams->NumParams(); up++)
                    {
                        NiString strName = pkParams->GetLocalName((ParamID)up);
                        pkParams->GetParameterType((ParamID)up);
                        Interval kInt = FOREVER;

                        if (strName == "Time On")
                        {
                            int iVal;
                            pkParams->GetValue((ParamID)up, 0, iVal, kInt);
                            fTimeOn = iVal  * sfTimeFactor;
                        }
                        else if (strName == "Time Off")
                        {
                            int iVal;
                            pkParams->GetValue((ParamID)up, 0, iVal, kInt);
                            fTimeOff = iVal * sfTimeFactor;
                        }
                        else if (strName == "Damping Symmetry")
                        {
                            int iVal;
                            pkParams->GetValue((ParamID)up, 0, iVal, kInt);
                            iType = iVal;
                        }
                        else if (strName == "X Damping")
                        {
                            pkParams->GetValue((ParamID)up, 0, fDamping[X], kInt); 
                        }
                        else if (strName == "X Range")
                        {
                            pkParams->GetValue((ParamID)up, 0, fRange[X], kInt); 
                        }
                        else if (strName == "X Falloff")
                        {
                            pkParams->GetValue((ParamID)up, 0, fFalloff[X], kInt); 
                        }
                        else if (strName == "Y Damping")
                        {
                            pkParams->GetValue((ParamID)up, 0, fDamping[Y], kInt); 
                        }
                        else if (strName == "Y Range")
                        {
                            pkParams->GetValue((ParamID)up, 0, fRange[Y], kInt); 
                        }
                        else if (strName == "Y Falloff")
                        {
                            pkParams->GetValue((ParamID)up, 0, fFalloff[Y], kInt); 
                        }
                        else if (strName == "Z Damping")
                        {
                            pkParams->GetValue((ParamID)up, 0, fDamping[Z], kInt); 
                        }
                        else if (strName == "Z Range")
                        {
                            pkParams->GetValue((ParamID)up, 0, fRange[Z], kInt); 
                        }
                        else if (strName == "Z Falloff")
                        {
                            pkParams->GetValue((ParamID)up, 0, fFalloff[Z], kInt); 
                        }
                        else if (strName == "Unlimited Range")
                        {
                            int iVal;
                            pkParams->GetValue((ParamID)up, 0, iVal, kInt);
                            bUnlimitedRange = iVal ? true : false;
                        }
                    }
                }

            }
        }
    }
    
    if (bFound && iType == 0)
    {
        for (unsigned int ui = 0; ui < 3; ui++)
        {
            unsigned int uiNumKeys = 0;
            NiStepBoolKey* pkBoolKeys = BuildActiveKeys(
                pkDesc->m_fNiPSysLoopStart, pkDesc->m_fNiPSysLoopEnd, 
                fTimeOn, fTimeOff, uiNumKeys);

            NiAVObject* pkDragObj = NULL;
            char acAxisName[256];
            NiPoint3 kWhichAxis;
            // Do all the iteration-dependant assignment here
            switch (ui)
            {
            case 0:
                NiSprintf(acAxisName, 256, "X");
                kWhichAxis = NiPoint3::UNIT_X;
                break;
            case 1:
                NiSprintf(acAxisName, 256, "Y");
                kWhichAxis = NiPoint3::UNIT_Y;
                break;
            case 2:
                NiSprintf(acAxisName, 256, "Z");
                kWhichAxis = NiPoint3::UNIT_Z;
                break;
            }

            if (fDamping[ui] > 0.0f)
            {
                char acName[256];
                NiSprintf(acName, 256, "NiPSysDragModifier(%s-Axis):%d", 
                    acAxisName, ms_uiTotalCount);

                NiPSDragForcePtr spNIDrag = 
                    NiNew NiPSDragForce(acName, pkDragObj);

                ParticleLinkUpInfo* pkParticleLinkUpInfo = 
                    NiNew ParticleLinkUpInfo;
                if (pkParticleLinkUpInfo)
                {
                    pkParticleLinkUpInfo->m_pkMAXNode = pkDrag->nodeRef;
                    pkParticleLinkUpInfo->m_spSystem = pkPSystem;
                    pkParticleLinkUpInfo->m_spSimulator = pkForcesStep;
                    pkParticleLinkUpInfo->m_spForce = spNIDrag;
                    ms_kLinkUpList.AddHead(pkParticleLinkUpInfo);
                }

                spNIDrag->SetPercentage(fDamping[ui]);
                spNIDrag->SetDragAxis(kWhichAxis);

                if (!bUnlimitedRange)
                {
                    spNIDrag->SetRange(fRange[ui]);
                    spNIDrag->SetRangeFalloff(fFalloff[ui]);
                }
                else
                {
                    spNIDrag->SetRange(NI_INFINITY);
                    spNIDrag->SetRangeFalloff(NI_INFINITY);
                }

                pkForcesStep->AddForce(spNIDrag);
                NiBoolData* pkBoolData = NiNew NiBoolData();
                pkBoolData->ReplaceAnim(pkBoolKeys, uiNumKeys, 
                    NiAnimationKey::STEPKEY);

                NiPSForceActiveCtlr* pkCtrl = NiNew 
                    NiPSForceActiveCtlr(acName);
                NiBoolTimelineInterpolator* pkInterp = NiNew 
                    NiBoolTimelineInterpolator(pkBoolData);

                pkCtrl->SetInterpolator(pkInterp);
                pkCtrl->ResetTimeExtrema();
                if (pkDesc->m_bNiPSysLoop)
                    pkCtrl->SetCycleType(NiTimeController::LOOP);
                else
                    pkCtrl->SetCycleType(NiTimeController::CLAMP);
                pkCtrl->SetTarget(pkPSystem);
                ms_uiTotalCount++;
            }
            else // if damping is zero, clean up memory
            {
                if (pkBoolKeys)
                    NiDelete[] pkBoolKeys;
            }
        }

    }
    else
    {
        NILOGWARNING("Warning: Gamebryo only supports Linear Drag.\n");
    }

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------

int NiMAXPSysConverter::ExtractWind(
    Interface*,
    Modifier* pkMod, 
    INode* pkMaxNode,
    NiPSParticleSystem* pkPSystem,
    NiPSysDesc*, 
    ScaleInfo*)
{
    WindMod* pkWind = (WindMod*) pkMod;
    
    NiPSSimulatorForcesStep* pkForcesStep = NULL;
    
    pkForcesStep = 
        NiGetSimulatorStep(NiPSSimulatorForcesStep, pkPSystem->GetSimulator());

    NIASSERT(pkForcesStep);
    
    float fStrength, fDecay, fDispLength, fTurb, fScale;
    int iType;

    IParamBlock2* pkPBlock2 = pkWind->force.obj->pblock2;
    Interval intForever = FOREVER;

    pkPBlock2->GetValue(PB_WIND_STRENGTH, m_kAnimStart, fStrength,
        intForever);
    pkPBlock2->GetValue(PB_WIND_DECAY, m_kAnimStart, fDecay,
        intForever);
    pkPBlock2->GetValue(PB_WIND_TYPE, m_kAnimStart, iType, intForever);
    pkPBlock2->GetValue(PB_WIND_DISPLENGTH, m_kAnimStart, 
        fDispLength, intForever);
    pkPBlock2->GetValue(PB_WIND_TURBULENCE, m_kAnimStart, fTurb, intForever);
    pkPBlock2->GetValue(PB_WIND_SCALE, m_kAnimStart, fScale, intForever);

    Matrix3 kWindXform = 
        pkWind->force.node->GetNodeTM(m_kAnimStart);
    Matrix3 kParticleXform = 
        pkMaxNode->GetNodeTM(m_kAnimStart);
    Matrix3 kWind2PartXform = 
        kWindXform*Inverse(kParticleXform);
    
    Point3 kPos = kWind2PartXform.GetTrans();

    Point3 kNorm;
    kNorm.x = 0.0f;
    kNorm.y = 0.0f;
    kNorm.z = 1.0f;
    kNorm = VectorTransform(kWind2PartXform, kNorm);
    kNorm = Normalize(kNorm);
    
    NiAVObject* pkGravityObj = NULL;
    char acName[256];
    NiSprintf(acName, 256, "NiPSysGravityModifier:%d",
            ms_uiTotalCount);
    
    NiPSGravityForce* pkNIGravity = 
        NiNew NiPSGravityForce(acName, pkGravityObj);

    if (iType == 0)
    {
        pkNIGravity->SetForceType(
        NiPSForceDefinitions::GravityData::FORCE_PLANAR);
    }
    else
    {
        pkNIGravity->SetForceType(
        NiPSForceDefinitions::GravityData::FORCE_SPHERICAL);

        //Spherical wind thinks positive force means 'away' from the
        //modifier, which contrasts gravity.  Negate the strength
        //to compensate for this discrepancy.
        fStrength = -fStrength;
    }

    ParticleLinkUpInfo* pkParticleLinkUpInfo = NiNew ParticleLinkUpInfo;
    if (pkParticleLinkUpInfo)
    {
        pkParticleLinkUpInfo->m_pkMAXNode = pkWind->force.node;
        pkParticleLinkUpInfo->m_spSystem = pkPSystem;
        pkParticleLinkUpInfo->m_spSimulator = pkForcesStep;
        pkParticleLinkUpInfo->m_spForce = pkNIGravity;
        ms_kLinkUpList.AddHead(pkParticleLinkUpInfo);
    }

    pkNIGravity->SetDecay(fDecay);

    // Hack to account for behavior differences between
    //   spray/snow and other particle systems
    float fModifier = 1.0f;
    float fGravity = fStrength * 
        GetFrameRate() * GetFrameRate();
    Class_ID eID = pkMaxNode->GetObjectRef()->ClassID();
    if (eID == Class_ID(RAIN_CLASS_ID, 0) ||
        eID == Class_ID(SNOW_CLASS_ID, 0))
    {
        fModifier = 0.1f;
        fGravity *= 0.1f;
        fTurb *= 0.01f;
    }

    pkNIGravity->SetStrength(fGravity);
    pkNIGravity->SetGravityAxis(NiPoint3(0.0f, 0.0f, 1.0f));
    pkNIGravity->SetTurbulence(fTurb);
    pkNIGravity->SetTurbulenceScale(fScale);

    pkForcesStep->AddForce(pkNIGravity);
    ms_uiTotalCount++;

    NiTimeController::CycleType eCycleType = NiTimeController::CLAMP;
    NiFloatInterpolatorPtr spStrengthInterp = BuildFloatInterp(pkPBlock2,
        PB_WIND_STRENGTH, eCycleType, GetFrameRate() * GetFrameRate() * 
        fModifier);

    
    if (spStrengthInterp)
    {
        NiPSGravityStrengthCtlr* pkGravStrengthController = 
            NiNew NiPSGravityStrengthCtlr(pkNIGravity->GetName());
        pkGravStrengthController->SetInterpolator(spStrengthInterp);
        pkGravStrengthController->ResetTimeExtrema();
        pkGravStrengthController->SetCycleType(eCycleType);
        pkGravStrengthController->SetTarget(pkPSystem);
    }
    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractParticleBomb(
    Interface*, 
    Modifier* pkMod,
    INode*,
    NiPSParticleSystem* pkPSystem, 
    NiPSysDesc* pkDesc,
    ScaleInfo*)
{
    PBombMod* pkPBomb = (PBombMod*) pkMod;
    IParamBlock* pkPBlock = pkPBomb->force.obj->pblock;

    NiPSSimulatorForcesStep* pkForcesStep = NULL;
    
    pkForcesStep = 
        NiGetSimulatorStep(NiPSSimulatorForcesStep, pkPSystem->GetSimulator());

    NIASSERT(pkForcesStep);
    
    TimeValue kStart, kDuration;
    int iDecayType, iSymm;
    float fDecay, fChaos, fDeltaV;
    Interval intForever = FOREVER;
    
    pkPBlock->GetValue(PBOMB_STARTTIME, m_kAnimStart, kStart,
        intForever);
    pkPBlock->GetValue(PBOMB_LASTSFOR, m_kAnimStart, kDuration,
        intForever);
    pkPBlock->GetValue(PBOMB_DECAY, m_kAnimStart, fDecay,
        intForever);
    pkPBlock->GetValue(PBOMB_DECAYTYPE, m_kAnimStart, 
        iDecayType, intForever);
    pkPBlock->GetValue(PBOMB_DELTA_V, m_kAnimStart, fDeltaV,
        intForever);
    pkPBlock->GetValue(PBOMB_CHAOS, m_kAnimStart, fChaos,
        intForever);
    pkPBlock->GetValue(PBOMB_SYMMETRY, m_kAnimStart, iSymm,
        intForever);
    
    // Here we need to get the object transform of the bomb relative to
    // it's pivot.  If the orientation of the pivot has been altered, we
    // must reflect that change in our bomb's axis
    Quat kQuat = pkPBomb->force.node->GetObjOffsetRot();
    Matrix3 kObjectTransform(1);
    PreRotateMatrix(kObjectTransform, kQuat);
    Point3 kMaxAxis;
    kMaxAxis.x = 0.0f;
    kMaxAxis.y = 0.0f; 
    kMaxAxis.z = 1.0f;
    kMaxAxis = VectorTransform(kObjectTransform, kMaxAxis);
    kMaxAxis = Normalize(kMaxAxis);

    NiPoint3 kAxis = NiPoint3(kMaxAxis.x, kMaxAxis.y, kMaxAxis.z);
    
    char acName[256];
    
    NiSprintf(acName, 256, "NiPSysBombModifier:%d", ms_uiTotalCount);
    NiPSBombForce* pkPBombModifier = NiNew NiPSBombForce(acName);
    ParticleLinkUpInfo* pkParticleLinkUpInfo = NiNew ParticleLinkUpInfo;
    if (pkParticleLinkUpInfo)
    {
        pkParticleLinkUpInfo->m_pkMAXNode = pkPBomb->force.node;
        pkParticleLinkUpInfo->m_spSystem = pkPSystem;
        pkParticleLinkUpInfo->m_spSimulator = pkForcesStep;
        pkParticleLinkUpInfo->m_spForce = pkPBombModifier;
        ms_kLinkUpList.AddHead(pkParticleLinkUpInfo);
    }

    // our DeltaV value is a modification of Max's.
    // we multiply their strength value (deltaV) by the framerate squared, 
    // which effectively divides by delta time, putting it in units per 
    // second squared. We multiply by 10 because that is a magic number 
    // that shows up in the max sdk.
    pkPBombModifier->SetDeltaV(fDeltaV * GetFrameRate() * 
        GetFrameRate() * 10.0f);
   
    pkPBombModifier->SetDecay(fDecay);

    // DECAY_NONE is a define in the MAX header files
    // so it needs to be undefined to allow the bomb data DECAY_NONE type
    #undef DECAY_NONE
    switch(iDecayType)
    {
        case 0:
            pkPBombModifier->SetDecayType(
                NiPSForceDefinitions::BombData::DECAY_NONE);
            break;
        case 1:
            pkPBombModifier->SetDecayType(
                NiPSForceDefinitions::BombData::DECAY_LINEAR);
            break;
        case 2:
            pkPBombModifier->SetDecayType(
                NiPSForceDefinitions::BombData::DECAY_EXPONENTIAL);
            break;
    }

    // Redefine it as it is defined in the MAX header files
    #define DECAY_NONE 0

    switch(iSymm)
    {
        case 0:
            pkPBombModifier->SetSymmType(
                NiPSForceDefinitions::BombData::SYMM_SPHERICAL);
            break;
        case 1:
            pkPBombModifier->SetSymmType(
                NiPSForceDefinitions::BombData::SYMM_CYLINDRICAL);
            break;
        case 2:
            pkPBombModifier->SetSymmType(
                NiPSForceDefinitions::BombData::SYMM_PLANAR);
            break;
    }

    pkPBombModifier->SetBombAxis(kAxis);

    pkForcesStep->AddForce(pkPBombModifier);
    ms_uiTotalCount++;

    // Create the modifier active controller
    unsigned int uiNumKeys = 0;
    NiStepBoolKey* pkBoolKeys = BuildActiveKeys(pkDesc->m_fNiPSysLoopStart, 
        pkDesc->m_fNiPSysLoopEnd, kStart * sfTimeFactor, (kStart + kDuration)
        * sfTimeFactor, uiNumKeys);  
    NiBoolData* pkBoolData = NiNew NiBoolData();
    pkBoolData->ReplaceAnim(pkBoolKeys, uiNumKeys, NiAnimationKey::STEPKEY);
    NiPSForceActiveCtlr* pkCtrl = NiNew NiPSForceActiveCtlr(acName);
    NiBoolTimelineInterpolator* pkInterp = 
        NiNew NiBoolTimelineInterpolator(pkBoolData);
    pkCtrl->SetInterpolator(pkInterp);
    pkCtrl->ResetTimeExtrema();

    if (pkDesc->m_bNiPSysLoop)
        pkCtrl->SetCycleType(NiTimeController::LOOP);
    else
        pkCtrl->SetCycleType(NiTimeController::CLAMP);
    pkCtrl->SetTarget(pkPSystem);

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractForces(
    Interface* pkIntf,
    INode* pkMaxNode, 
    NiPSParticleSystem* pkPSystem,
    NiPSysDesc* pkDesc, 
    ScaleInfo* pkParentScale)
{
    CHECK_MEMORY();
    if (!pkPSystem)
        return W3D_STAT_FAILED;

    for (int i = 0; i < pkMaxNode->NumRefs(); i++)
    {
        ReferenceTarget* pkRefTarg = pkMaxNode->GetReference(i);
        
        if (pkRefTarg && 
            pkRefTarg->ClassID() == Class_ID(WSM_DERIVOB_CLASS_ID, 0))
        {
            IDerivedObject* pkWSMDerObj = (IDerivedObject*)pkRefTarg;
            
            for (int j = 0; j < pkWSMDerObj->NumModifiers(); j++)
            {
                Modifier* pkMod = pkWSMDerObj->GetModifier(j);
                
                Class_ID eModID = pkMod->ClassID();
                
                if (eModID == Class_ID(DEFLECTMOD_CLASS_ID, 0))
                {
                    ExtractPlanarDeflector(pkIntf, pkMod, pkMaxNode, 
                        pkPSystem, pkDesc, pkParentScale);
                }
                else if (eModID == SPHEREDEFMOD_CLASS_ID)
                {
                    ExtractSphereDeflector(pkIntf, pkMod, pkMaxNode,
                        pkPSystem, pkDesc, pkParentScale);
                }
                else if (eModID == Class_ID(GRAVITYMOD_CLASS_ID, 0))
                {
                    ExtractGravity(pkIntf, pkMod, pkMaxNode,
                        pkPSystem, pkDesc, pkParentScale);
                }
                else if (eModID == Class_ID(WINDMOD_CLASS_ID, 0))
                {
                    ExtractWind(pkIntf, pkMod, pkMaxNode,
                        pkPSystem, pkDesc, pkParentScale);
                }
                else if (eModID == PBOMBMOD_CLASS_ID)
                {
                    ExtractParticleBomb(pkIntf, pkMod, pkMaxNode,
                        pkPSystem, pkDesc, pkParentScale);
                }
                else if (eModID == PDRAGMOD_CLASS_ID)
                {
                    ExtractDrag(pkIntf, pkMod, pkMaxNode,
                        pkPSystem, pkDesc, pkParentScale);
                }
            }
        }
    }
    CHECK_MEMORY();
    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::Convert(
    Interface* pkIntf,
    INode* pkMaxNode, 
    NiNode* pkParent,
    ScaleInfo* pkParentScale)
{
    ms_uiTotalCount = 0;
    CHECK_MEMORY();
    Object* pkObjRef = pkMaxNode->GetObjectRef();
    SimpleParticle* pkPart = (SimpleParticle*) pkObjRef;
    Class_ID eID = pkObjRef->ClassID();

    NiPSysDesc kDesc;

    ExtractCustomAttribs(&kDesc, pkPart);

    if (eID == SUPRSPRAY_CLASS_ID)
    {
        ExtractSuperSpray(pkIntf, pkPart, &kDesc, pkParentScale);
    }
    else if (eID == BLIZZARD_CLASS_ID)
    {
        ExtractBlizzard(pkIntf, pkPart, &kDesc, pkParentScale);
    }
    else if (eID == PArray_CLASS_ID)
    {
        ExtractPArray(pkMaxNode, pkParent, pkIntf, pkPart, &kDesc, 
            pkParentScale);
    }
    else if (eID == PCLOUD_CLASS_ID)
    {
        ExtractPCloud(pkMaxNode, pkIntf, pkPart, &kDesc, pkParentScale);
    }
    else if (eID == Class_ID(RAIN_CLASS_ID, 0) || 
        eID == Class_ID(SNOW_CLASS_ID, 0))
    {
        ExtractOldPSys(pkMaxNode, pkPart, &kDesc);
    }
    else
    {
        NILOGWARNING("We are being asked to convert an unknown "
            "particle system!\n");
        NIASSERT(0);
        return 0;
    }

    NiMAXMaterialConverter::GetPSysColorInfo(pkMaxNode->GetMtl(), 0, 
        kDesc.m_pkColors, kDesc.m_bColorAges, kDesc.m_uiNumColorKeys);

    float fScale = 1.0f;
    if (pkParentScale && !pkParentScale->bIsIdent)
        fScale = pkParentScale->scale.s.x;

    NiPSParticleSystem* pkPSys = CreatePSys(&kDesc, pkPart, fScale, pkParent);
    
    ExtractForces(pkIntf, pkMaxNode, pkPSys, &kDesc, pkParentScale);

    pkParent->AttachChild(pkPSys);
    pkParent->SetSelectiveUpdate(true);
    pkParent->SetSelectiveUpdatePropertyControllers(true);
    pkParent->SetSelectiveUpdateRigid(false);
    pkParent->SetSelectiveUpdateTransforms(true);

    if (kDesc.m_pkPSysRoot != NULL && kDesc.m_pkPSysRoot != 
        pkMaxNode && kDesc.m_bLocalCoords)
    {
        // We're actually using a different root than the emitter, so 
        // place it in the remap list 
        // for after all the nodes are converted
    
        ParticleRemapInfo* pkParticleRemapInfo = NiNew ParticleRemapInfo;
        pkParticleRemapInfo->m_pkMAXNode = kDesc.m_pkPSysRoot;
        pkParticleRemapInfo->m_spSystem = pkPSys;
        ms_kRemapList.AddHead(pkParticleRemapInfo);
    }
    else if (kDesc.m_bLocalCoords)
    {
        ParticleRemapInfo* pkParticleRemapInfo = NiNew ParticleRemapInfo;
        pkParticleRemapInfo->m_pkMAXNode = NULL;
        pkParticleRemapInfo->m_spSystem = pkPSys;
        ms_kRemapList.AddHead(pkParticleRemapInfo);
    }
    
    NiString strName = pkParent->GetName();
    pkPSys->SetName((const char*)strName);
    strName += "-Emitter";
    pkParent->SetName((const char*)strName);

    NiMAXUV* pkNiMAXUV = NULL;
    NiMAXMaterialConverter::GetUVInfo(pkMaxNode->GetMtl(), m_kAnimStart, 
        pkNiMAXUV, -1);

    // For materials applied to particle systems, we assign the first
    // converted MAX UV channel to NI channel 0.
    if (pkNiMAXUV->IsChannel(0))
        pkNiMAXUV->SetMAXtoNI(0, 0);

    NiMAXMaterialConverter kMtl(m_kAnimStart, m_kAnimEnd);
    NiNode* pkEffectNode = NULL;
    int iStatus = kMtl.Convert(pkMaxNode->GetMtl(), pkParent, pkPSys, 
        pkMaxNode, 0, pkNiMAXUV, pkEffectNode);
    NiDelete pkEffectNode;

    NiDelete pkNiMAXUV;
    if (iStatus)
        return(iStatus);

    // move pkParent visibility controller, if any, to pkParticles
    NiVisController* pkVC = NiGetController(NiVisController, pkParent);
    if (pkVC)
    {
        pkVC->SetTarget(pkPSys);
    }

    if (iStatus)
        return(iStatus);

    NiTimeController* pkController = pkPSys->GetControllers();
    bool bUseAppTime = NiMAXConverter::GetUseAppTime();
    while (pkController != NULL)
    {
        if (bUseAppTime)
            pkController->SetAnimType(NiTimeController::APP_TIME);
        else
            pkController->SetAnimType(NiTimeController::APP_INIT);
        pkController = pkController->GetNext();
    }
    CHECK_MEMORY();
    return(0);
}
//---------------------------------------------------------------------------
void NiMAXPSysConverter::PopulateMeshEmitter(
    NiAVObject* pkObj,
    NiPSMeshEmitter* pkEmitter)
{
    if (!pkObj || !pkEmitter)
        return;

    NiNode* pkNode = NiDynamicCast(NiNode, pkObj);
    if (!pkNode)
    {
        // Only NiNodes should be passed into this function.
        NIASSERT(false);
        return;
    }

    unsigned int ui;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (NiIsKindOf(NiMesh, pkChild))
        {
            pkEmitter->AddMeshEmitter((NiMesh*) pkChild);

            // Make sure NiOptimize will not remove the emitter object.
            NiStringExtraData* pkStringED = NiNew NiStringExtraData(
                "NiOptimizeKeep\n");

            pkChild->AddExtraData(pkStringED);
        }
        else if (NiIsKindOf(NiNode, pkChild))
        {
            const char* pcName = pkChild->GetName();
            if (pcName != NULL && strcmp(pcName, gpcMultiMtlName) == 0)
            {
                PopulateMeshEmitter((NiNode*) pkChild, pkEmitter);
            }
        }
    }
}

//---------------------------------------------------------------------------
void NiMAXPSysConverter::Finish(NiNode*)
{
    CHECK_MEMORY();

    // Create an update process
    NiMeshUpdateProcess kUpdateProcess;
    kUpdateProcess.SetTime(0.0f);
    kUpdateProcess.SetUpdateControllers(false);

    // Assign emitters
    if (!ms_kLinkUpList.IsEmpty())
    {
        NiTListIterator kIter = ms_kLinkUpList.GetHeadPos();
        
        while (kIter)
        {
            ParticleLinkUpInfo* pkMAXEmitterInfo = 
                ms_kLinkUpList.GetNext(kIter);
            NiNode* pkMAXEmitterNode = NiMAXHierConverter::Lookup(
                pkMAXEmitterInfo->m_pkMAXNode, NULL);
            
            if (!pkMAXEmitterNode)
            {
                NiNode* pkParent = pkMAXEmitterInfo->m_spSystem->GetParent();
                pkMAXEmitterNode = (NiNode*)pkParent;

            }

            if (NiIsKindOf(NiPSVolumeEmitter, 
                pkMAXEmitterInfo->m_spEmitter))
            {
                NiPSVolumeEmitter* pkEmitter = 
                    NiDynamicCast(NiPSVolumeEmitter,
                    pkMAXEmitterInfo->m_spEmitter);
                pkEmitter->SetEmitterObj(pkMAXEmitterNode);
            }
            else if (NiIsKindOf(NiPSMeshEmitter, 
                pkMAXEmitterInfo->m_spEmitter))
            {
                NiPSMeshEmitter* pkEmitter = 
                    NiDynamicCast(NiPSMeshEmitter,
                    pkMAXEmitterInfo->m_spEmitter);
                PopulateMeshEmitter(pkMAXEmitterNode, pkEmitter);
                continue;
            }            
            if (NiIsKindOf(NiPSSimulatorForcesStep, 
                pkMAXEmitterInfo->m_spSimulator))
            {
                if (NiIsKindOf(NiPSGravityForce,
                    pkMAXEmitterInfo->m_spForce))
                {
                    NiPSGravityForce* pkModifier = 
                        NiDynamicCast(NiPSGravityForce,
                        pkMAXEmitterInfo->m_spForce);
                    pkModifier->SetGravityObj(pkMAXEmitterNode);
                }
                else if (NiIsKindOf(NiPSBombForce, 
                    pkMAXEmitterInfo->m_spForce))
                {
                    NiPSBombForce* pkModifier = 
                        NiDynamicCast(NiPSBombForce, 
                        pkMAXEmitterInfo->m_spForce);
                    pkModifier->SetBombObj(pkMAXEmitterNode);
                }
                else if (NiIsKindOf(NiPSDragForce,
                    pkMAXEmitterInfo->m_spForce))
                {
                    NiPSDragForce* pkModifier = 
                        NiDynamicCast(NiPSDragForce,
                        pkMAXEmitterInfo->m_spForce);
                    pkModifier->SetDragObj(pkMAXEmitterNode);
                }
            }

            if (NiIsKindOf(NiPSSimulatorCollidersStep, 
                pkMAXEmitterInfo->m_spSimulator))
            {
                if (NiIsKindOf(NiPSPlanarCollider, 
                    pkMAXEmitterInfo->m_spCollider))
                {
                    NiPSPlanarCollider* pkCollider =
                        NiDynamicCast(NiPSPlanarCollider,
                        pkMAXEmitterInfo->m_spCollider);
                    pkCollider->SetColliderObj(pkMAXEmitterNode);
                }
                else if (NiIsKindOf(NiPSSphericalCollider,
                    pkMAXEmitterInfo->m_spCollider))
                {
                    NiPSSphericalCollider* pkCollider = 
                        NiDynamicCast(NiPSSphericalCollider,
                        pkMAXEmitterInfo->m_spCollider);
                    pkCollider->SetColliderObj(pkMAXEmitterNode);
                }
            }

            // Make sure NiOptimize will not remove the emitter node.
            NiStringExtraData* pkString = 
                NiNew NiStringExtraData("sgoKeep");

            pkMAXEmitterNode->AddExtraData(pkString);
        }
    }

    if (!ms_kStaticBoundList.IsEmpty())
    {
        NiTListIterator kIter = ms_kStaticBoundList.GetHeadPos();

        while (kIter)
        {
            ParticleStaticBoundInfo* pkMAXBoundInfo = 
                ms_kStaticBoundList.GetNext(kIter);

            NiPSParticleSystem* pkPSystem = pkMAXBoundInfo->m_spSystem;
            NiNode* pkParent = pkPSystem->GetParent();
           
            NiPoint3 kBoundPosition = pkMAXBoundInfo->m_kBound.GetCenter();
            float fBoundRadius = pkMAXBoundInfo->m_kBound.GetRadius();
            NiBound kStaticBound;

            GetRoot(pkParent)->UpdateDownwardPass(kUpdateProcess);
                
            if (!pkPSystem->GetWorldSpace())
            {
                NiTransform kPSysCoordSystem = pkPSystem->GetWorldTransform();
                NiTransform kPSysWorldBounds;
                kPSysWorldBounds.m_Translate = kBoundPosition;
                kPSysWorldBounds.m_Rotate.MakeIdentity();
                kPSysWorldBounds.m_fScale = fBoundRadius;
                NiTransform kInvPSysCoordSystem;
                kPSysCoordSystem.Invert(kInvPSysCoordSystem);
                NiTransform kPSysLocalBounds = kInvPSysCoordSystem * 
                    kPSysWorldBounds;
                kBoundPosition = kPSysLocalBounds.m_Translate;
                fBoundRadius = kPSysLocalBounds.m_fScale;
            }
            else
            {
                float fInverseScale = 1.0f / pkPSystem->GetWorldScale();
                fBoundRadius *= fInverseScale;
                kBoundPosition *= fInverseScale;
            }
            kStaticBound.SetCenterAndRadius(kBoundPosition, fBoundRadius);
            pkPSystem->SetModelBound(kStaticBound);
        }
    }

    if (!ms_kRemapList.IsEmpty())
    {
        NiTListIterator kIter = ms_kRemapList.GetHeadPos();
        
        while (kIter)
        {
            ParticleRemapInfo* pkMAXRootInfo = ms_kRemapList.GetNext(kIter);

            NiNode* pkParent = NULL;
            if (pkMAXRootInfo->m_pkMAXNode)
            {
                pkParent = NiMAXHierConverter::Lookup(
                    pkMAXRootInfo->m_pkMAXNode, NULL);
            }
            else
            {
                pkParent = pkMAXRootInfo->m_spSystem->GetParent();
                while (pkParent->GetParent() != NULL)
                {
                    pkParent = pkParent->GetParent();
                }
            }

            if (pkParent != NULL)
            {
                NiAVObject* pkOldParent = 
                    pkMAXRootInfo->m_spSystem->GetParent();
                if (pkOldParent != NULL)
                {
                    NiZBufferPropertyPtr spProp = (NiZBufferProperty*)
                       pkOldParent->GetProperty(NiZBufferProperty::GetType());
                    if (spProp)
                    {
                        spProp = (NiZBufferProperty*) spProp->Clone();
                        pkMAXRootInfo->m_spSystem->AttachProperty(spProp);
                    }
                }

                if (!pkMAXRootInfo->m_spSystem->GetWorldSpace())
                {
                    GetRoot(pkParent)->UpdateDownwardPass(kUpdateProcess);
                    pkMAXRootInfo->m_spSystem->Update(kUpdateProcess);
                    NiTransform kNewParent = pkParent->GetWorldTransform();
                    NiTransform kOldParent;
                    kOldParent = 
                        pkMAXRootInfo->m_spSystem->GetWorldTransform();
                    
                    NiTransform kInvNewParent;
                    kNewParent.Invert(kInvNewParent);
                    NiTransform kOldParentToNewParent = kInvNewParent * 
                        kOldParent;
                    pkMAXRootInfo->m_spSystem->SetTranslate(
                        kOldParentToNewParent.m_Translate);
                    pkMAXRootInfo->m_spSystem->SetRotate(
                        kOldParentToNewParent.m_Rotate);
                    pkMAXRootInfo->m_spSystem->SetScale(
                        kOldParentToNewParent.m_fScale);
                }
                pkParent->AttachChild(pkMAXRootInfo->m_spSystem);
                pkParent->SetSelectiveUpdate(true);
                pkParent->SetSelectiveUpdatePropertyControllers(true);
                pkParent->SetSelectiveUpdateRigid(false);
                pkParent->SetSelectiveUpdateTransforms(true);

                // Make sure NiOptimize will not remove the psys parent node.
                NiStringExtraData* pkString = 
                    NiNew NiStringExtraData("sgoKeep");

                pkParent->AddExtraData(pkString);
            }
        }
    }

    // Delete the list
    NiTListIterator kIter = ms_kLinkUpList.GetHeadPos();
    
    while (kIter)
    {
        NiDelete ms_kLinkUpList.RemovePos(kIter);
    }
    ms_kLinkUpList.RemoveAll();
    
    kIter = ms_kStaticBoundList.GetHeadPos();
    while (kIter)
    {
        NiDelete ms_kStaticBoundList.RemovePos(kIter);
    }
    ms_kStaticBoundList.RemoveAll();

    kIter = ms_kRemapList.GetHeadPos();
    while (kIter)
    {
        NiDelete ms_kRemapList.RemovePos(kIter);
    }
    ms_kRemapList.RemoveAll();
}
//---------------------------------------------------------------------------
NiFloatInterpolator* NiMAXPSysConverter::BuildFloatInterp(
    IParamBlock* pkParams,
    int iParamIdx, 
    NiTimeController::CycleType& eCycleType,
    float fScalar,
    bool,
    float,
    float)
{
    if (!pkParams)
        return NULL;

    NiMAXAnimationConverter kConverter(m_kAnimStart, m_kAnimEnd);

    Control* pkControl = pkParams->GetController(iParamIdx);

    if (pkControl == NULL)
        return NULL;

    kConverter.SetORT(pkControl, eCycleType);
    NiInterpolator* pkInterp = NULL;

    pkInterp = kConverter.ConvertFloatAnim(pkControl);

    if (!pkInterp)
        return NULL;

    NiFloatInterpolator* pkFloatInterp = NiDynamicCast(
        NiFloatInterpolator, pkInterp);
    if (!pkFloatInterp || pkFloatInterp->GetKeyCount(0) == 0)
        return NULL;

    if (fScalar != 1.0f)
    {
        for (unsigned int ui = 0; ui < pkFloatInterp->GetKeyCount(0); ui++)
        {
            NiFloatKey* pkKey = (NiFloatKey*)pkFloatInterp->GetKeyAt(ui, 0);
            pkKey->SetValue(pkKey->GetValue() * fScalar);
        }
    }

    return pkFloatInterp;
}
//---------------------------------------------------------------------------
NiFloatInterpolator* NiMAXPSysConverter::BuildFloatInterp(
    IParamBlock2* pkParams,
    int iParamIdx, 
    NiTimeController::CycleType& eCycleType,
    float fScalar, 
    bool,
    float,
    float)
{
    if (!pkParams)
        return NULL;

    NiMAXAnimationConverter kConverter(m_kAnimStart, m_kAnimEnd);

    Control* pkControl = pkParams->GetController(iParamIdx);

    if (pkControl == NULL)
        return NULL;

    eCycleType = kConverter.GetORT(pkControl);
    NiInterpolator* pkInterp;
    NiFloatInterpolator* pkFloatInterp;

    pkInterp = kConverter.ConvertFloatAnim(pkControl);

    if (!pkInterp || !NiIsKindOf(NiFloatInterpolator, pkInterp))
        return NULL;
    pkFloatInterp = NiDynamicCast(NiFloatInterpolator, pkInterp);

    if (fScalar != 1.0f)
    {
        for (unsigned int ui = 0; ui < pkFloatInterp->GetKeyCount(0); ui++)
        {
            NiFloatKey* pkKey = (NiFloatKey*)pkFloatInterp->GetKeyAt(ui,0);
            pkKey->SetValue(pkKey->GetValue() * fScalar);
        }
    }

    return pkFloatInterp;
}
//---------------------------------------------------------------------------
NiStepBoolKey* NiMAXPSysConverter::BuildActiveKeys(
    float fLoopStart, 
    float fLoopEnd,
    float fActiveStart,
    float fActiveEnd, 
    unsigned int& uiNumKeys)
{
    uiNumKeys = 0;
    NiStepBoolKey* pkBoolKeys = NiNew NiStepBoolKey[4];
    unsigned int uiIndex = 0;

    if (fActiveStart == fActiveEnd)
        fActiveEnd = fActiveStart + PSYS_INTERVAL_TIME;

    if (fLoopEnd <= fActiveStart)
    {
        NILOGWARNING("WARNING! A particle system has a loop end time less "
            "than the emission start time.\nSetting the loop end time to "
            "equal the emission stop time.\n");
        fLoopEnd = fActiveEnd;
    }

    if (fActiveStart <= fLoopStart)
    {
        pkBoolKeys[uiIndex].SetTime(fActiveStart);
        pkBoolKeys[uiIndex].SetBool(true);
        uiIndex++;
    }
    else
    {
        pkBoolKeys[uiIndex].SetTime(fLoopStart);
        pkBoolKeys[uiIndex].SetBool(false);
        uiIndex++;

        pkBoolKeys[uiIndex].SetTime(fActiveStart);
        pkBoolKeys[uiIndex].SetBool(true);
        uiIndex++;
    }
    
    if (fActiveEnd < fLoopEnd)
    {
        pkBoolKeys[uiIndex].SetTime(fActiveEnd);
        pkBoolKeys[uiIndex].SetBool(false);
        uiIndex++;

        pkBoolKeys[uiIndex].SetTime(fLoopEnd);
        pkBoolKeys[uiIndex].SetBool(false);
        uiIndex++;
    }
    else
    {
        pkBoolKeys[uiIndex].SetTime(fLoopEnd);
        pkBoolKeys[uiIndex].SetBool(false);
        uiIndex++;
    }
    
    uiNumKeys = uiIndex;

    return pkBoolKeys;
}
//---------------------------------------------------------------------------
void NiMAXPSysConverter::SetEmitterControllerStartAndStop(
    unsigned int& uiNumEmitterActiveKeys, 
    NiStepBoolKey*& pkEmitterActiveKeys,
    unsigned int uiNumBirthRateKeys, 
    NiFloatKey* pkBirthRateKeys,
    NiFloatKey::KeyType eBirthRateKeyType)
{
    double dTime;
    double dStep = 1.0f / (GetFrameRate() * 4.0);
    double dStartTime = 0.0;
    double dEndTime = 0.0;

    // Determine the Start and End Keys
    switch(eBirthRateKeyType)
    {
    case NiAnimationKey::BEZKEY:
        dStartTime = ((NiBezFloatKey*)pkBirthRateKeys)[0].GetTime();
        dEndTime = ((NiBezFloatKey*)pkBirthRateKeys)
            [uiNumBirthRateKeys-1].GetTime();
        break;
    case NiAnimationKey::LINKEY:
        dStartTime = ((NiLinFloatKey*)pkBirthRateKeys)[0].GetTime();
        dEndTime = ((NiLinFloatKey*)pkBirthRateKeys)
            [uiNumBirthRateKeys-1].GetTime();
        break;
    default:
        NIASSERT(false);
    }

    // Count the number of necessary keys
    uiNumEmitterActiveKeys = 0;

    float fRate = 0.0f;
    float fLastRate = 0.0f;
    unsigned int uiLastIdx = 0;

    unsigned char ucBirthRateSize = NiFloatKey::GetKeySize(eBirthRateKeyType);
    
    // Super Sample the Keys looking for where keys become positive
    for(dTime = dStartTime; dTime <= (dEndTime + (dStep / 2.0)); 
        dTime += dStep)
    {
        fRate = NiFloatKey::GenInterp((float) dTime, pkBirthRateKeys, 
            eBirthRateKeyType, uiNumBirthRateKeys, uiLastIdx, 
            ucBirthRateSize);

        // First Key
        if(NiOptimize::CloseTo((float) dTime, (float) dStartTime, (float)
            (dStep / 10.0)))
        {
            uiNumEmitterActiveKeys++;
        }
        else
        {
            // Starting Positive
            if ((fRate > 0.0f) && (fLastRate <= 0.0f))
            {
                uiNumEmitterActiveKeys++;
            }
        }

        // Last Key
        if(NiOptimize::CloseTo((float) dTime, (float) dEndTime, (float)
            (dStep / 10.0)))
        {
            uiNumEmitterActiveKeys++;
        }
        else
        {
            // Starting Negative
            if ((fRate <= 0.0f) && (fLastRate > 0.0f))
            {
                uiNumEmitterActiveKeys++;
            }
        }

        fLastRate = fRate;
    }

    // Allocate the necessary Keys
    pkEmitterActiveKeys = NiNew NiStepBoolKey[uiNumEmitterActiveKeys];

    // Create the Keys
    fRate = 0.0f;
    fLastRate = 0.0f;
    uiLastIdx = 0;
    unsigned int uiCurrentKey = 0;

    // Super Sample the Keys looking for where keys become positive
    for(dTime = dStartTime; dTime <= (dEndTime + (dStep / 2.0)); 
        dTime += dStep)
    {
        fRate = NiFloatKey::GenInterp((float) dTime, pkBirthRateKeys, 
            eBirthRateKeyType, uiNumBirthRateKeys, uiLastIdx, 
            ucBirthRateSize);

        // First Key
        if(NiOptimize::CloseTo((float) dTime, (float) dStartTime, (float)
            (dStep / 10.0)))
        {
            // Create a begin key at the current time
            pkEmitterActiveKeys[uiCurrentKey].SetTime((float) dTime);
            pkEmitterActiveKeys[uiCurrentKey++].SetBool(fRate > 0.0f);
        }
        else
        {
            // Starting Positive
            if ((fRate > 0.0f) && (fLastRate <= 0.0f))
            {
                // Create a key at the last time
                pkEmitterActiveKeys[uiCurrentKey].SetTime((float)
                    (dTime - dStep));
                pkEmitterActiveKeys[uiCurrentKey++].SetBool(true);
            }
        }

        // Last Key
        if(NiOptimize::CloseTo((float) dTime, (float) dEndTime, (float)
            (dStep / 10.0)))
        {
            // Create an end key at the current time
            pkEmitterActiveKeys[uiCurrentKey].SetTime((float) dTime);
            pkEmitterActiveKeys[uiCurrentKey++].SetBool(fRate > 0.0f);
        }
        else
        {
            // Starting Negative
            if ((fRate <= 0.0f) && (fLastRate > 0.0f))
            {
                // Create a key at the last time
                pkEmitterActiveKeys[uiCurrentKey].SetTime((float)
                    (dTime - dStep));
                pkEmitterActiveKeys[uiCurrentKey++].SetBool(false);
            }
        }

        fLastRate = fRate;
    }
}
#endif
//---------------------------------------------------------------------------
