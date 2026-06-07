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

#ifndef MYIPARTICLESYTEM_H
#define MYIPARTICLESYTEM_H


#include "MyiObject.h"


// Forward Declarations
class NiPSParticleSystem;
class kMDtEmitter;
class kMDtParticles;
class NiPoint3;
class MyiTextureManager;
class MyiMaterialManager;
class kMDtCollider;
class NiPSCollider;
class NiPSSpawner;
class NiPSEmitter;

class MyiParticleSystem : public MyiObject
{
public:
    MyiParticleSystem(int iParticlesID, 
        MyiMaterialManager* pkMaterialManager, 
        MyiTextureManager* pkTextureManager);

    ~MyiParticleSystem();

    NiPSParticleSystem* GetParticleSystem();

    void PostProcess(NiTList<MyiParticleSystem*>& pkParticleSystems);

    bool MatchesParticleShapeName(MString kShapeName);

    bool MatchesParticleName(MString kName);

    void GetLifeSpan(float& fLifeSpan, float& fLifeSpanVar);
    
protected:

    int GetNumParticles();
    bool CreateMeshParticles(int iNumParticles);
    bool CreateAutoNormalParticles(int iNumParticles);

    void CreateUpdateController();

    void CreateEmitter();

    float GetParticleRadiusVariance();
    NiPSEmitter* CreateDirectional();

    NiPSEmitter* CreateOmni();

    NiPSEmitter* CreateSurface();
    void FindSurfaceMesh(NiAVObject* pkObject, 
        NiTObjectArray<NiMeshPtr>& kSurfaceMesh);

    NiPSEmitter* CreateCurve();

    NiPSEmitter* CreateVolume();

    void CreateEmitterSpeedController();

    float GetInitialSize();

    void CreateEmitterController();
    void SetEmitterControllerStartAndStop(
        unsigned int& uiNumEmitterActiveKeys,
        NiStepBoolKey*& pkEmitterActiveKeys);

    void SetEmitterControllerStartAndStop(
        unsigned int& uiNumEmitterActiveKeys, 
        NiStepBoolKey*& pkEmitterActiveKeys,
        unsigned int uiNumBirthRateKeys, NiFloatKey* pkBirthRateKeys,
        NiFloatKey::KeyType eBirthRateKeyType);

    NiPSSpawner* GetSpawner(NiTList<MyiParticleSystem*>& pkParticleSystems,
        kMDtParticles* pkParticles = NULL);

    void CloneMasterParticles();

    void CreateDeathSpawner(
        NiTList<MyiParticleSystem*>& pkParticleSystems);

    void CreateColorModifier();
    bool HasColors();

    void CreateGrowFadeModifier();

    void CreateRotationModifier();
    bool HasRotation();
    void CreateInitialRotationSpeedCtlr();
    void CreateInitialRotationSpeedVarCtlr();
    void CreateRotationAngleCtlr();
    void CreateRotationAngleVarCtlr();

    void CreateBoundUpdateModifier();

    // Create the different fields that affect a particle system
    void CreateForces();

    void CreateGravityField(int iField);

    void CreateRadialField(int iField);

    void CreateDragField(int iField);

    void CreateTurbulenceField(int iField);

    void CreateVortexField(int iField);

    void CreateAirField(int iField);

    // Create controllers for the common field attributes
    void CreateFieldMagnitudeController(MObject MField, 
        NiPSFieldForce* pkForce, const char* pcFieldName);

    void CreateFieldAttenuationController(MObject MField, 
        NiPSFieldForce* pkForce, const char* pcFieldName);

    void CreateFieldMaxDistanceController(MObject MField, 
        NiPSFieldForce* pkForce, const char* pcFieldName);

    // Create controllers for an air field
    void CreateAirFieldAirFrictionController(MObject MField, 
        NiPSFieldForce* pkForce, const char* pcFieldName);

    void CreateAirFieldInheritVelocityController(MObject MField, 
        NiPSFieldForce* pkForce, const char* pcFieldName);

    void CreateAirFieldSpreadController(MObject MField, 
        NiPSFieldForce* pkForce, const char* pcFieldName);

    // Create colliders
    void CreateColliders(NiTList<MyiParticleSystem*>& pkParticleSystems);

    NiPSCollider* CreateSphericalCollider(
        NiTList<MyiParticleSystem*>& pkParticleSystems,
        kMDtCollider* pkCollider);

    void SetSphereColliderNodePosistionAndScale();

    NiPSCollider* CreatePlanarCollider(
        NiTList<MyiParticleSystem*>& pkParticleSystems,
        kMDtCollider* pkCollider);

    void SetPlanarColliderWidthAndHeightFromMesh();

    void GetAxisAlignedBoundingBox(NiAVObject* pkObject, NiPoint3& kMin, 
        NiPoint3& kMax);

    void CheckEvents(NiTList<MyiParticleSystem*>& pkParticleSystems);

    void CreateResetOnLoop();

    void ConvertGravityDirectionToWorldSpace();

    void ConvertDragDirectionToWorldSpace();

    void DeleteCollisionMesh();

    void CreateMaterialAndTexture(MyiMaterialManager* pkMaterialManager, 
        MyiTextureManager* pkTextureManager);

    void CreateZBufferProperties();

    NiPSParticleSystem* m_pkParticleSystem;
    int m_iPSystemEmitterID;
    int m_iPSystemParticlesID;

        // Collision Members
    NiPSSpawner* m_pkCollisionSpawn;
    NiPSParticleSystem* m_pkCollisionParticleSystem;
    bool m_bDieAtCollision;
    int m_iNumEmittedParticles;
    int m_iNumSplitParticles;
    float m_fCollisionSpread;


    kMDtEmitter* m_pMDtEmitter; 
    kMDtParticles* m_pMDtParticles;
};


#endif 
