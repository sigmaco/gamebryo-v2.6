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
#include "MUIToggleTrajectoryCommand.h"
#include "MSharedData.h"
#include <NiTStridedRandomAccessIterator.h>

typedef NiTStridedRandomAccessIterator<NiPoint3> Point3Iter;
typedef NiTStridedRandomAccessIterator<NiColorA> ColorIter;


using namespace NiManagedToolInterface;
#define NUM_VERTICES 400
#define CURRENT_COUNT 30
#define MIN_TIME 1.0f/60.0f
//---------------------------------------------------------------------------
MUIToggleTrajectoryCommand::MUIToggleTrajectoryCommand()
{
    m_bOn = false;
    m_uiTrajectoryIdx = (unsigned int)-1;
    m_fLastTime = -NI_INFINITY;
    m_uiNextVertex = NUM_VERTICES - 1;
}
//---------------------------------------------------------------------------
String* MUIToggleTrajectoryCommand::GetName()
{
    return "Toggle Actor Path";
}
//---------------------------------------------------------------------------
bool MUIToggleTrajectoryCommand::Execute(MUIState*)
{
    NiMesh* pkLines = GetTrajectory();

    if (!pkLines)
        return false;

    if (!m_bOn)
        ResetTrajectory();

    m_bOn = !m_bOn;
    pkLines->SetAppCulled(!m_bOn);
    OnCommandExecuted(this);
    OnCommandStateChanged(this);
    return true;
}
//---------------------------------------------------------------------------
void MUIToggleTrajectoryCommand::DeleteContents()
{
}
//---------------------------------------------------------------------------
bool MUIToggleTrajectoryCommand::IsToggle()
{
    return true;
}
//---------------------------------------------------------------------------
bool MUIToggleTrajectoryCommand::GetActive()
{
    return m_bOn;
}
//---------------------------------------------------------------------------
bool MUIToggleTrajectoryCommand::GetEnabled()
{
    return true;
}
//---------------------------------------------------------------------------
#pragma unmanaged
// A bug in the compiler forces the create methods to live in 
// completely unmanaged code or else an InvalidProgramException
// is thrown in release mode.

NiMeshPtr CreateTrajectory()
{
    NiMeshPtr spLines = NiNew NiMesh();
    spLines->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINESTRIPS);
    spLines->SetName("TrajectoryLines");

    // Add Position
    NiPoint3 kInitVerts[NUM_VERTICES];
    NiColorA kInitColors[NUM_VERTICES];
    for (int i=0; i<NUM_VERTICES; i++)
    {
        kInitVerts[i] = NiPoint3::ZERO;
        kInitColors[i] = NiColorA::WHITE;
    }

    spLines->AddStream( NiCommonSemantics::POSITION(), 0,
            NiDataStreamElement::F_FLOAT32_3, NUM_VERTICES,
            NiDataStream::ACCESS_CPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE |
            NiDataStream::ACCESS_GPU_READ,
            NiDataStream::USAGE_VERTEX, &kInitVerts[0]);

    // Add Colors
    spLines->AddStream( NiCommonSemantics::COLOR(), 0,
            NiDataStreamElement::F_FLOAT32_4, NUM_VERTICES,
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE |
            NiDataStream::ACCESS_GPU_READ,
            NiDataStream::USAGE_VERTEX, &kInitColors[0]);

    spLines->SetSubmeshCount(2);

    NiDataStreamRef* pkPositionStreamRef = spLines->FindStreamRef(
        NiCommonSemantics::POSITION());
    NiDataStream* pkPositionStream = pkPositionStreamRef->GetDataStream();
    NiDataStream::Region kRegion2(0, 0);
    pkPositionStream->AddRegion(kRegion2);
    pkPositionStreamRef->BindRegionToSubmesh(0, 0);
    pkPositionStreamRef->BindRegionToSubmesh(1, 1);

    NiDataStreamRef* pkColorStreamRef = spLines->FindStreamRef(
        NiCommonSemantics::COLOR());
    NiDataStream* pkColorStream = pkColorStreamRef->GetDataStream();
    NiDataStream::Region kColorRegion2(0, NUM_VERTICES);
    pkColorStream->AddRegion(kColorRegion2);
    pkColorStreamRef->BindRegionToSubmesh(0, 0);
    pkColorStreamRef->BindRegionToSubmesh(1, 1);

    spLines->SetTranslate(NiPoint3::ZERO);
    NiMatrix3 kRot;
    kRot.MakeIdentity();
    spLines->SetRotate(kRot);
    spLines->SetScale(1.0f);

    NiMaterialProperty* pkMatProp = NiNew NiMaterialProperty();
    pkMatProp->SetEmittance(NiColor::WHITE);

    NiWireframeProperty* pkWireProp = NiNew NiWireframeProperty();
    pkWireProp->SetWireframe(true);

    NiVertexColorProperty* pkVertProp = NiNew NiVertexColorProperty();
    pkVertProp->SetSourceMode(NiVertexColorProperty::SOURCE_EMISSIVE);
    pkVertProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);

    NiAlphaProperty* pkAlphaProp = NiNew NiAlphaProperty();
    pkAlphaProp->SetAlphaBlending(true);
    pkAlphaProp->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
    pkAlphaProp->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
    pkAlphaProp->SetAlphaTesting(false);

    spLines->AttachProperty(pkAlphaProp);
    spLines->AttachProperty(pkVertProp);
    spLines->AttachProperty(pkWireProp);
    spLines->AttachProperty(pkMatProp);

    spLines->Update(0.0f);
    NiMesh::CompleteSceneModifiers(spLines);
    spLines->UpdateProperties();
    spLines->UpdateEffects();
    spLines->UpdateNodeBound();

    return spLines;
}
#pragma managed
//---------------------------------------------------------------------------
void MUIToggleTrajectoryCommand::RefreshData()
{
    DeleteContents();
    MSharedData* pkSharedData = MSharedData::Instance;
    if (pkSharedData)
    {
        // A bug in the compiler forces the create methods to live in 
        // completely unmanaged code or else an InvalidProgramException
        // is thrown in release mode.
        pkSharedData->Lock();
        NiMeshPtr spLines = CreateTrajectory();
        m_uiNextVertex = 0;
        spLines->SetAppCulled(!m_bOn);
        if (m_uiTrajectoryIdx != -1)
            pkSharedData->SetScene(m_uiTrajectoryIdx, spLines);
        else
            m_uiTrajectoryIdx = pkSharedData->AddScene(spLines);

        SetRegionRange(spLines, m_uiNextVertex);

        pkSharedData->Unlock();
    }

    AttachTrajectory();
    OnCommandStateChanged(this);
}
//---------------------------------------------------------------------------
void MUIToggleTrajectoryCommand::AttachTrajectory()
{
}
//---------------------------------------------------------------------------
NiMesh* MUIToggleTrajectoryCommand::GetTrajectory()
{
    NiMesh* pkLines = NULL;
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    if (m_uiTrajectoryIdx != -1)
    {
        NiAVObject* pkObj = pkSharedData->GetScene(m_uiTrajectoryIdx);
        if ( pkObj != NULL)
        {
           pkLines = NiDynamicCast(NiMesh, pkObj);
        }
    }
    pkSharedData->Unlock();
    return pkLines;
}
//---------------------------------------------------------------------------
MUICommand::CommandType MUIToggleTrajectoryCommand::GetCommandType()
{
    return MUICommand::TOGGLE_TRAJECTORY;
}
//---------------------------------------------------------------------------
void MUIToggleTrajectoryCommand::Update(float fTime)
{
    if (m_bOn)
    {
        if (fTime - m_fLastTime < MIN_TIME)
            return;

        MSharedData* pkSharedData = MSharedData::Instance;
        pkSharedData->Lock();
        NiActorManager* pkManager = pkSharedData->GetActorManager();
        if (pkManager != NULL)
        {
            NiAVObject* pkAccumRoot = pkManager->GetAccumRoot();
            if (pkAccumRoot)
            {
                NiPoint3 kWorldPos = pkAccumRoot->GetWorldTranslate();
                NiMesh* pkLines = GetTrajectory();

                // Scope the locks appropriately
                {
                    NiDataStreamElementLock kPosLock(pkLines,
                        NiCommonSemantics::POSITION(), 0,
                        NiDataStreamElement::F_FLOAT32_3, 
                        NiDataStream::LOCK_WRITE);
                    Point3Iter kVertIter = kPosLock.begin<NiPoint3>();

                    NiDataStreamElementLock kColorLock(pkLines,
                        NiCommonSemantics::COLOR(), 0,
                        NiDataStreamElement::F_FLOAT32_4, 
                        NiDataStream::LOCK_WRITE);
                    ColorIter kColorIter = kColorLock.begin<NiColorA>();

                    kVertIter[m_uiNextVertex].x = kWorldPos.x;
                    kVertIter[m_uiNextVertex].y = kWorldPos.y;
                    kVertIter[m_uiNextVertex].z = kWorldPos.z;

                    float fLifetimeMax = 
                        (float)CURRENT_COUNT / (float)NUM_VERTICES;
                    for (unsigned int ui = 0; ui < NUM_VERTICES; ui++)
                    {
                        float fLifetime = 0.0f;
                        if (ui < m_uiNextVertex)
                            fLifetime = 
                                (((float)(m_uiNextVertex - ui)) / 
                                 (float)NUM_VERTICES);
                        else if (ui > m_uiNextVertex)
                            fLifetime = 
                                (((float)(m_uiNextVertex + NUM_VERTICES - 
                                    ui)) / (float)NUM_VERTICES);
                        
                        float fAlpha = 1.0f - fLifetime;
                        if (fLifetime < fLifetimeMax)
                        {
                            kColorIter[ui].r = 1.0f;
                            kColorIter[ui].g = 0.0f;
                            kColorIter[ui].b = 0.0f;
                        }
                        else
                        {
                            kColorIter[ui].r = 1.0f;
                            kColorIter[ui].g = 1.0f;
                            kColorIter[ui].b = 1.0f;
                        }

                        kColorIter[ui].a = fAlpha;
                    }

                    if (m_uiNextVertex == NUM_VERTICES - 1)
                    {
                        // Region 0, 0 to 1
                        // Region 1, 2 to end
                        kVertIter[0].x = kVertIter[m_uiNextVertex].x;
                        kVertIter[0].y = kVertIter[m_uiNextVertex].y;
                        kVertIter[0].z = kVertIter[m_uiNextVertex].z;
                        kColorIter[0].r = kColorIter[m_uiNextVertex].r;
                        kColorIter[0].g = kColorIter[m_uiNextVertex].g;
                        kColorIter[0].b = kColorIter[m_uiNextVertex].b;
                        kColorIter[0].a = kColorIter[m_uiNextVertex].a;

                        m_uiNextVertex = 1;
                    }
                    else
                    {
                        // Region 0, 0 to m_uiNextVertex
                        // Region 1, m_uiNextVertex +1 to end
                        m_uiNextVertex++;
                    }

                    NIASSERT(m_uiNextVertex !=0);
                    SetRegionRange(pkLines, m_uiNextVertex);

                }

                //pkModelData->GetBound().ComputeFromData(
                //    NUM_VERTICES, pkVerts);
                pkLines->RecomputeBounds();

                pkLines->Update(0.0f);
                NiMesh::CompleteSceneModifiers(pkLines);
                pkLines->UpdateNodeBound();
                m_fLastTime = fTime;
            }
        }
        pkSharedData->Unlock();
    }
}
//---------------------------------------------------------------------------
void MUIToggleTrajectoryCommand::ResetTrajectory()
{
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();
    NiActorManager* pkManager = pkSharedData->GetActorManager();
    if (pkManager != NULL)
    {
        NiAVObject* pkAccumRoot = pkManager->GetAccumRoot();
        if (pkAccumRoot)
        {
            NiPoint3 kWorldPos = pkAccumRoot->GetWorldTranslate();
            NiMesh* pkLines = GetTrajectory();

            {
                // Initialize the position data
                NiDataStreamElementLock kPosLock(pkLines,
                    NiCommonSemantics::POSITION(), 0,
                    NiDataStreamElement::F_FLOAT32_3, 
                    NiDataStream::LOCK_WRITE);
                Point3Iter kOutIt = kPosLock.begin<NiPoint3>();
                Point3Iter kEndIt = kPosLock.end<NiPoint3>();
                while (kOutIt != kEndIt)
                {    
                    *kOutIt = NiPoint3::ZERO;
                    kOutIt++;
                }
            }

            {
                NiDataStreamElementLock kColorLock(pkLines,
                    NiCommonSemantics::COLOR(), 0,
                    NiDataStreamElement::F_FLOAT32_4, 
                    NiDataStream::LOCK_WRITE);
                ColorIter kColorIter = kColorLock.begin<NiColorA>();
                ColorIter kColorEnd = kColorLock.end<NiColorA>();
                while(kColorIter != kColorEnd)
                {
                    *kColorIter = NiColorA::WHITE;
                    kColorIter++;
                }
            }

            pkLines->RecomputeBounds();

            pkLines->Update(0.0f);
            NiMesh::CompleteSceneModifiers(pkLines);
            pkLines->UpdateNodeBound();

            m_fLastTime = -NI_INFINITY;
            m_uiNextVertex = 0;

            SetRegionRange(pkLines, m_uiNextVertex);
        }
    }
    pkSharedData->Unlock();
}
//---------------------------------------------------------------------------
void MUIToggleTrajectoryCommand::SetRegionRange(NiMesh* pkLines, 
    NiUInt32 uiRange)
{
    // SetAttributesRegionRange sets the region range on the verts and
    // colors.
    NiDataStreamRef* pkStreamRef;
    NiDataStreamElement kElement;

    // Change verts region
    NIVERIFY(pkLines->FindStreamRefAndElementBySemantic(
        NiCommonSemantics::POSITION(), 0, NiDataStreamElement::F_FLOAT32_3,
        pkStreamRef, kElement));
    NIASSERT(pkStreamRef->GetSubmeshRemapCount() == 2);
    NiDataStream* pkStream = pkStreamRef->GetDataStream();
    NiDataStream::Region& kRegion0 = pkStream->GetRegion(0);
    kRegion0.SetRange(uiRange);
    NiDataStream::Region& kRegion1 = pkStream->GetRegion(1);
    kRegion1.SetStartIndex(uiRange);
    kRegion1.SetRange(NUM_VERTICES - uiRange);
    NIASSERT(pkStreamRef->GetRegionForSubmesh(0) == kRegion0);
    NIASSERT(pkStreamRef->GetRegionForSubmesh(1) == kRegion1);

    // Change color regions
    NIVERIFY(pkLines->FindStreamRefAndElementBySemantic(
        NiCommonSemantics::COLOR(), 0, NiDataStreamElement::F_FLOAT32_4,
        pkStreamRef, kElement));
    NIASSERT(pkStreamRef->GetSubmeshRemapCount() == 2);
    pkStream = pkStreamRef->GetDataStream();
    NiDataStream::Region& kColorRegion0 = pkStream->GetRegion(0);
    kColorRegion0.SetRange(uiRange);
    NiDataStream::Region& kColorRegion1 = pkStream->GetRegion(1);
    kColorRegion1.SetStartIndex(uiRange);
    kColorRegion1.SetRange(NUM_VERTICES - uiRange);
    NIASSERT(pkStreamRef->GetRegionForSubmesh(0) == kColorRegion0);
    NIASSERT(pkStreamRef->GetRegionForSubmesh(1) == kColorRegion1);
}
//---------------------------------------------------------------------------
