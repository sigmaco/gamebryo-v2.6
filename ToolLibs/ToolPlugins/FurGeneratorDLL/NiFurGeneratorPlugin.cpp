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
#include "NiFurGeneratorPlugin.h"
#include "NiFurGeneratorDLLDefines.h"
#include "FurGeneratorDialog.h"
#include "FurGeneratorDefines.h"
#include <NiExporterOptionsSharedData.h>
#include <NiMesh.h>
#include <NiToolDataStream.h>
#include <NiSkinningMeshModifier.h>
#include <NiMorphMeshModifier.h>
#include <NiMorphWeightsController.h>

#include <NiShaderFactory.h>

// This must match what is in NiOptimization library file: 
// NiOptimize_HelpFunc.cpp
#define NI_BONES_PER_PARTITION_KEY "NiBonesPerPartition"

NiImplementRTTI(NiFurGeneratorPlugin, NiPlugin);
NiFurGeneratorPlugin* NiFurGeneratorPlugin::ms_pkThis = NULL;

//---------------------------------------------------------------------------
bool NeedsSubdivision(const NiPoint2& kTexCoord0,
    const NiPoint2& kTexCoord1, const NiPoint2& kTexCoord2)
{
    NiPoint2 kNewTexCoord0 = (kTexCoord0 - kTexCoord1);
    NiPoint2 kNewTexCoord1 = (kTexCoord1 - kTexCoord2);
    NiPoint2 kNewTexCoord2 = (kTexCoord0 - kTexCoord2);

    if (kNewTexCoord0.SqrLength() > 1.0f ||
        kNewTexCoord1.SqrLength() > 1.0f ||
        kNewTexCoord2.SqrLength() > 1.0f)
    {
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NeedsSubdivision(const NiPoint2& kTexCoord0,
    const NiPoint2& kTexCoord1, const NiPoint2& kTexCoord2, 
    NiUInt32& uiNewVertCount, NiUInt32& uiNewIndexCount)
{
    NiPoint2 kNewTexCoord0 = (kTexCoord0 - kTexCoord1);
    NiPoint2 kNewTexCoord1 = (kTexCoord1 - kTexCoord2);
    NiPoint2 kNewTexCoord2 = (kTexCoord0 - kTexCoord2);

    if (kNewTexCoord0.SqrLength() > 1.0f ||
        kNewTexCoord1.SqrLength() > 1.0f ||
        kNewTexCoord2.SqrLength() > 1.0f)
    {
        kNewTexCoord0 = (kTexCoord0 + kTexCoord1) * 0.5f;
        kNewTexCoord1 = (kTexCoord1 + kTexCoord2) * 0.5f;
        kNewTexCoord2 = (kTexCoord0 + kTexCoord2) * 0.5f;

        NeedsSubdivision(kTexCoord0, kNewTexCoord0, kNewTexCoord2,
            uiNewVertCount, uiNewIndexCount);
        NeedsSubdivision(kNewTexCoord2, kNewTexCoord0, kNewTexCoord1,
            uiNewVertCount, uiNewIndexCount);
        NeedsSubdivision(kNewTexCoord2, kNewTexCoord1, kTexCoord2,
            uiNewVertCount, uiNewIndexCount);
        NeedsSubdivision(kNewTexCoord0, kTexCoord1, kNewTexCoord1,
            uiNewVertCount, uiNewIndexCount);

        uiNewVertCount += 3;
        uiNewIndexCount += 9;

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiFurGeneratorPlugin::NiFurGeneratorPlugin() :
    NiPlugin("Fur Generator", "1.0", 
    "Generates fur data.",
    "This a specialized plug-in used to generate fur geometry and a shell "
    "texture atlas to used by the fur shader."),
    m_pkProgressBar(NULL)
{
}
//---------------------------------------------------------------------------
bool NiFurGeneratorPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "NiFurGeneratorPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiFurGeneratorPlugin::PerObjectCallback(NiAVObject*)
{

    return true;
}
//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::ErrorCallback(NiUInt32, 
    NiObject*, const char*)
{

}
//---------------------------------------------------------------------------

NiExecutionResultPtr NiFurGeneratorPlugin::Execute(
    const NiPluginInfo* pkInfo)
{
    // Set the random seed value so that the data is always the same between
    // runs.
    NiSrand(123456789);

    ms_pkThis = this;
    // Get the Scene graph from the shared data
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));
    if (!pkSGSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }
    
    pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));

    // Check the PluginInfo to double check the type
    if(pkInfo->GetType() != "PROCESS")
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    Init();

    m_pkProgressBar = NiNew NiProgressDialog("Generating Fur");
    m_pkProgressBar->Create();
    m_pkProgressBar->SetPosition(0);

    // Set fur parameters
    m_bGenerateDenseFins = 
        pkInfo->GetBool(OPT_FURGENERATOR_GENERATE_DENSE_FINS);
    m_fFurDensity = pkInfo->GetFloat(OPT_FURGENERATOR_FUR_DENSITY);
    m_kFurTintColor.r = pkInfo->GetFloat(OPT_FURGENERATOR_FUR_TINT_COLOR_R);
    m_kFurTintColor.g = pkInfo->GetFloat(OPT_FURGENERATOR_FUR_TINT_COLOR_G);
    m_kFurTintColor.b = pkInfo->GetFloat(OPT_FURGENERATOR_FUR_TINT_COLOR_B);
    m_kFurTintColor.a = pkInfo->GetFloat(OPT_FURGENERATOR_FUR_TINT_COLOR_A);
    m_fFurLength = pkInfo->GetFloat(OPT_FURGENERATOR_FUR_LENGTH);
    m_fFurLengthRandomness = pkInfo->GetFloat(
        OPT_FURGENERATOR_FUR_LENGTH_RANDOMNESS) * 0.01f;
    m_fTextureTilesPerFin = 
        pkInfo->GetFloat(OPT_FURGENERATOR_TEXTURE_TILES_PER_FIN);
    m_fTextureTilesPerFinRandomness = 
        pkInfo->GetFloat(OPT_FURGENERATOR_TEXTURE_TILES_PER_FIN_RANDOMNESS) *
        0.01f;
    m_uiNumberOfShells = 
        pkInfo->GetInt(OPT_FURGENERATOR_NUM_SHELLS);
    m_fShellSizeFactor = pkInfo->GetFloat(OPT_FURGENERATOR_SHELL_SIZE_FACTOR);
    m_fShellTextureSize = 
        (float)pkInfo->GetInt(OPT_FURGENERATOR_SHELL_TEXTURE_SIZE);

    // Calculate the sqrt to get the number of cells
    m_uiNumberOfCells = (NiUInt32)sqrt((float)m_uiNumberOfShells);
    if (!m_uiNumberOfCells)
        m_uiNumberOfCells = 1;

    if ((m_uiNumberOfCells * m_uiNumberOfCells) != m_uiNumberOfShells)
    {
        m_uiNumberOfShells = m_uiNumberOfCells * m_uiNumberOfCells;
        NILOG("Invalid number of shells changing it to %d.\n",
            m_uiNumberOfShells);
    }

    // Calculate the shell texture cell size in pixels
    m_uiShellTextureCellSize = 
        (NiUInt32)m_fShellTextureSize / m_uiNumberOfCells;

    BuildMeshList(pkSGSharedData);
    GenerateShellTexture();
    GenerateFur();

    m_pkProgressBar->Destroy();
    NiDelete m_pkProgressBar;
    m_pkProgressBar = NULL;

    Destroy();
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
NiPluginInfo* NiFurGeneratorPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NiFurGeneratorPlugin");
    pkPluginInfo->SetType("PROCESS");

    pkPluginInfo->SetFloat(OPT_FURGENERATOR_FUR_LENGTH, 1.0f);
    pkPluginInfo->SetFloat(OPT_FURGENERATOR_FUR_LENGTH_RANDOMNESS, 10.0f);
    pkPluginInfo->SetFloat(OPT_FURGENERATOR_TEXTURE_TILES_PER_FIN, 1.0f);
    pkPluginInfo->SetFloat(OPT_FURGENERATOR_TEXTURE_TILES_PER_FIN_RANDOMNESS,
        10.0f);
    pkPluginInfo->SetInt(OPT_FURGENERATOR_NUM_SHELLS, 9);
    pkPluginInfo->SetFloat(OPT_FURGENERATOR_SHELL_SIZE_FACTOR, 0.7f);
    pkPluginInfo->SetInt(OPT_FURGENERATOR_SHELL_TEXTURE_SIZE, 512);
    pkPluginInfo->SetFloat(OPT_FURGENERATOR_FUR_DENSITY, 4.0f);
    pkPluginInfo->SetFloat(OPT_FURGENERATOR_FUR_TINT_COLOR_R, 2.0f);
    pkPluginInfo->SetFloat(OPT_FURGENERATOR_FUR_TINT_COLOR_G, 2.0f);
    pkPluginInfo->SetFloat(OPT_FURGENERATOR_FUR_TINT_COLOR_B, 2.0f);
    pkPluginInfo->SetFloat(OPT_FURGENERATOR_FUR_TINT_COLOR_A, 1.0f);
    pkPluginInfo->SetBool(OPT_FURGENERATOR_GENERATE_DENSE_FINS, true);
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiFurGeneratorPlugin::HasManagementDialog()
{
    return true;
}
//---------------------------------------------------------------------------
bool NiFurGeneratorPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent)
{
    FurGeneratorDialog kDialog(m_hModule, hWndParent, 
        pkInitialInfo);

    switch(kDialog.DoModal())
    {
    case IDOK:
        // Copy over the results
        pkInitialInfo->RemoveAllParameters();
        kDialog.GetResults()->Clone(pkInitialInfo);
        break;
    case IDCANCEL:
        return false;
        break;
    default:
        NIASSERT(false);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::Init()
{
    m_pkMeshList = NiNew  NiTPointerList<NiMesh*>;
}
//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::Destroy()
{
    m_pkMeshList->RemoveAll();
    NiDelete m_pkMeshList;
}

//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::GenerateFur()
{
    NiTListIterator kPos = m_pkMeshList->GetHeadPos();
    
    m_pkProgressBar->SetLineOne("Generating the fur...");
    m_pkProgressBar->SetRangeSpan(m_pkMeshList->GetSize());

    while (kPos)
    {
        NiMeshPtr spMesh = m_pkMeshList->GetNext(kPos);

        NiSortAdjustNode* pkSortNode = NiNew NiSortAdjustNode();
        pkSortNode->SetSortingMode(NiSortAdjustNode::SORTING_OFF);

        NiNode* pkMeshParent = spMesh->GetParent();
        pkMeshParent->DetachChild(spMesh);

        pkMeshParent->AttachChild(pkSortNode);
        pkSortNode->AttachChild(spMesh);

        const NiMaterial* pkMaterial = spMesh->GetActiveMaterial();
        NIASSERT(pkMaterial);
        if (pkMaterial)
        {
            if (pkMaterial->GetName() == "Fur")
            {
                m_kFinsShaderName = "FurFins";
                m_kShellsShaderName = "FurShells";
                m_uiBonesPerPartition = 0;
            }
            else if (pkMaterial->GetName() == "FurSkinning")
            {
                m_kFinsShaderName = "FurFinsSkinning";
                m_kShellsShaderName = "FurShellsSkinning";
                m_uiBonesPerPartition = 32;
            }
        }

        GenerateShellsFromMesh(spMesh, pkSortNode, "Shells");

        if (m_bGenerateDenseFins)
        {
            NiMeshPtr spFurMesh = GenerateFurMeshFromMesh(spMesh);
            GenerateFinsFromMesh(spFurMesh, pkSortNode, "Fins");
        }
        else
        {
            GenerateFinsFromMesh(spMesh, pkSortNode, "Fins");
        }

        // Remove the Detail Map used to store shell the texture coordinate set
        RemoveDetailMap(spMesh);

        m_pkProgressBar->StepIt();
    }
}
//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::GenerateShellTexture()
{
    NiColorA* pkShellBase = (NiColorA*)NiMalloc(sizeof(NiColorA) 
        * m_uiShellTextureCellSize * m_uiShellTextureCellSize);

    // Create base shell texture that all other shell textures will work from
    for(NiUInt32 uiX = 0; uiX < m_uiShellTextureCellSize; uiX++)
    {
        for(NiUInt32 uiY = 0; uiY < m_uiShellTextureCellSize; uiY++)
        {
            if (NiRand() % 10 == 0)
            {
                pkShellBase[(uiY*m_uiShellTextureCellSize)+uiX].r = 1;
                pkShellBase[(uiY*m_uiShellTextureCellSize)+uiX].g = 1;
                pkShellBase[(uiY*m_uiShellTextureCellSize)+uiX].b = 1;
                pkShellBase[(uiY*m_uiShellTextureCellSize)+uiX].a = 1;
            }
            else
            {
                pkShellBase[(uiY*m_uiShellTextureCellSize)+uiX].r = 0;
                pkShellBase[(uiY*m_uiShellTextureCellSize)+uiX].g = 0;
                pkShellBase[(uiY*m_uiShellTextureCellSize)+uiX].b = 0;
                pkShellBase[(uiY*m_uiShellTextureCellSize)+uiX].a = 0;
            }
        }
    }

    NiPixelData* pkPixData = NiNew NiPixelData(
        (NiUInt32)m_fShellTextureSize, (NiUInt32)m_fShellTextureSize, 
        NiPixelFormat::RGBA32);

    NiRGBA* pkPixels = (NiRGBA*)pkPixData->GetPixels();

    // Create a texture atlas that contains the all the textures for the shells
    for(NiUInt32 uiShell = 0; uiShell < m_uiNumberOfShells; uiShell++)
    {
        NiUInt32 xOffset = (uiShell % m_uiNumberOfCells) * 
            m_uiShellTextureCellSize;
        NiUInt32 yOffset = ((uiShell-(uiShell % m_uiNumberOfCells)) / 
            m_uiNumberOfCells) * m_uiShellTextureCellSize;

        for(NiUInt32 uiX  = 0; uiX < m_uiShellTextureCellSize; uiX++)
        {
            for(NiUInt32 uiY = 0; uiY < m_uiShellTextureCellSize; uiY++)
            {
                float fDownScale = (float)(rand()%100) / 1000.0f;
                fDownScale = 1 - fDownScale;

                NiUInt32 uiIndex = (uiY * m_uiShellTextureCellSize) + uiX;
                pkShellBase[uiIndex].r *= fDownScale;
                pkShellBase[uiIndex].g *= fDownScale;
                pkShellBase[uiIndex].b *= fDownScale;
                pkShellBase[uiIndex].a *= fDownScale;

                pkShellBase[uiIndex].GetAs(
                    pkPixels[((yOffset+uiY)*(NiUInt32)m_fShellTextureSize)+
                    (uiX+xOffset)]);
            }
        }
    }

    pkPixData->MarkAsChanged();

    NiSourceTexture::FormatPrefs kPrefs;
    kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::SINGLE_COLOR_8;
    kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::BINARY;
    kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
    m_pkShellTexture = NiSourceTexture::Create(pkPixData, kPrefs);
    m_pkShellTexture->SetFilename("ShellTexture");

    NiFree(pkShellBase);

}
//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::GenerateShellsFromMesh(NiMesh* pkMesh,
    NiSortAdjustNode* pkSortNode, char* cpName)
{
    if (!m_uiNumberOfShells)
        return;

    NiUInt32 uiVertCount = pkMesh->GetVertexCount();
    NiUInt32 uiIndexCount = pkMesh->GetTotalCount(NiCommonSemantics::INDEX());

    NiUInt32 uiNewVertCount = uiVertCount * m_uiNumberOfShells;
    NiUInt32 uiNewIndexCount = uiIndexCount * m_uiNumberOfShells;

    // Use the texture index assigned to the detail map to determine which 
    // texture coordinate set will be used to assign texture coordinates to the
    // shells.
    NiTexturingProperty* pkTextureProp = 
        (NiTexturingProperty*)pkMesh->GetProperty(NiProperty::TEXTURING);
    if (!pkTextureProp)
    {
        NILOG("Could not find a texture property on mesh.");
        return;
    }

    NiTexturingProperty::Map* pkDetailMap = 
        pkTextureProp->GetDetailMap();
    if (!pkDetailMap)
    {
        NILOG("Could not find detail map on mesh.");
        return;
    }

    // Check to see if we are morphed.
    NiSkinningMeshModifier* pkSkinningMeshModifier = 
        (NiSkinningMeshModifier*)pkMesh->GetModifierByType(
        (&NiSkinningMeshModifier::ms_RTTI));

    // Check to see if we are morphed.
    NiMorphMeshModifier* pkMorphMeshModifier = 
        (NiMorphMeshModifier*)pkMesh->GetModifierByType(
        (&NiMorphMeshModifier::ms_RTTI));

    NiDataStreamElementLock kPositionLock(pkMesh, 
        (!pkSkinningMeshModifier) ? 
        NiCommonSemantics::POSITION() : NiCommonSemantics::POSITION_BP(),
        0, NiDataStreamElement::F_FLOAT32_3,
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
    NiTStridedRandomAccessIterator<NiPoint3> kPositionIter = 
        kPositionLock.begin<NiPoint3>();

    NiDataStreamElementLock kNormalLock(pkMesh, 
        (!pkSkinningMeshModifier) ? 
        NiCommonSemantics::NORMAL() : NiCommonSemantics::NORMAL_BP(),
        0, NiDataStreamElement::F_FLOAT32_3,
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
    NiTStridedRandomAccessIterator<NiPoint3> kNormalIter = 
        kNormalLock.begin<NiPoint3>();

    NiDataStreamElementLock kTexCoordLock0(pkMesh, 
        NiCommonSemantics::TEXCOORD(), 0, NiDataStreamElement::F_FLOAT32_2, 
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
    NiTStridedRandomAccessIterator<NiPoint2> kTexCoordIter0 = 
        kTexCoordLock0.begin<NiPoint2>();

    NiDataStreamElementLock kColorLock(pkMesh, NiCommonSemantics::COLOR(),
        0, NiDataStreamElement::F_FLOAT32_4,
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
    NiTStridedRandomAccessIterator<NiColorA> kColorIter;
    if (kColorLock.DataStreamExists())
        kColorIter = kColorLock.begin<NiColorA>();

    NiDataStreamElementLock kIndexLock(pkMesh, NiCommonSemantics::INDEX(),
        0, NiDataStreamElement::F_UNKNOWN,
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
    NiTStridedRandomAccessIterator<NiUInt32> kIndexIter = 
        kIndexLock.begin<NiUInt32>();

    NiMesh* pkNewMesh = NiNew NiMesh();
    pkNewMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);

    // Add the position stream
    NiDataStreamElementLock kNewPositionLock = pkNewMesh->AddStreamGetLock(
        NiCommonSemantics::POSITION(), 0, NiDataStreamElement::F_FLOAT32_3, 
        uiNewVertCount, (pkMorphMeshModifier) ?
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE :
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX, true);
    NiTStridedRandomAccessIterator<NiPoint3> kNewPositionIter = 
        kNewPositionLock.begin<NiPoint3>();

    // Add the normal stream
    NiDataStreamElementLock kNewNormalLock = pkNewMesh->AddStreamGetLock(
        NiCommonSemantics::NORMAL(), 0, NiDataStreamElement::F_FLOAT32_3, 
        uiNewVertCount, 
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX, true);
    NiTStridedRandomAccessIterator<NiPoint3> kNewNormalIter = 
        kNewNormalLock.begin<NiPoint3>();

    // Add the texture coordinate stream
    NiDataStreamElementLock kNewTexCoordLock0 = pkNewMesh->AddStreamGetLock(
        NiCommonSemantics::TEXCOORD(), 0, NiDataStreamElement::F_FLOAT32_2, 
        uiNewVertCount, 
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX, true);
    NiTStridedRandomAccessIterator<NiPoint2> kNewTexCoordIter0 = 
        kNewTexCoordLock0.begin<NiPoint2>();

    // Add the texture coordinate stream
    NiDataStreamElementLock kNewTexCoordLock1 = pkNewMesh->AddStreamGetLock(
        NiCommonSemantics::TEXCOORD(), 1, NiDataStreamElement::F_FLOAT32_2, 
        uiNewVertCount, 
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX, true);
    NiTStridedRandomAccessIterator<NiPoint2> kNewTexCoordIter1 = 
        kNewTexCoordLock1.begin<NiPoint2>();

    // Add the color stream
    NiDataStreamElementLock kNewColorLock = pkNewMesh->AddStreamGetLock(
        NiCommonSemantics::COLOR(), 0, NiDataStreamElement::F_FLOAT32_4, 
        uiNewVertCount, 
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX, true);
    NiTStridedRandomAccessIterator<NiColorA> kNewColorIter = 
        kNewColorLock.begin<NiColorA>();

    // Add the index stream
    NiDataStreamElementLock kNewIndexLock = pkNewMesh->AddStreamGetLock(
        NiCommonSemantics::INDEX(), 0, NiDataStreamElement::F_UINT32_1,
        uiNewIndexCount, 
        NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX_INDEX, true);
    NiTStridedRandomAccessIterator<NiUInt32> kNewIndexIter = 
        kNewIndexLock.begin<NiUInt32>();

    NiUInt32 uiCurrentVert = 0;
    NiUInt32 uiCurrentIndex = 0;
    float fShellLengthIncrement = 
        (m_fFurLength / (float)(m_uiNumberOfShells+1) * m_fShellSizeFactor);
    float fShellLength = fShellLengthIncrement;

    for(NiUInt32 uiShell = 0; uiShell < m_uiNumberOfShells; uiShell++)
    {
        // Duplicate Geometry Data
        for(NiUInt32 uiVID = 0; uiVID < uiVertCount; uiVID++)
        {
            kNewPositionIter[uiCurrentVert] = kPositionIter[uiVID];
            kNewNormalIter[uiCurrentVert] = kNormalIter[uiVID];
            kNewTexCoordIter0[uiCurrentVert] = kTexCoordIter0[uiVID];

            if (kColorIter.Exists())
                kNewColorIter[uiCurrentVert] = kColorIter[uiVID];
            else
                kNewColorIter[uiCurrentVert] = NiColorA(1,1,1,1);

            float fShellLengthRandom = fShellLength +
                (NiSymmetricRandom() * fShellLengthIncrement * 
                m_fFurLengthRandomness);

            kNewPositionIter[uiCurrentVert].x += kNormalIter[uiVID].x * 
                fShellLengthRandom;
            kNewPositionIter[uiCurrentVert].y += kNormalIter[uiVID].y * 
                fShellLengthRandom;
            kNewPositionIter[uiCurrentVert].z += kNormalIter[uiVID].z * 
                fShellLengthRandom;

            // The further out the shell fade it out.
            kNewColorIter[uiCurrentVert].a *= 
                (1.0f - (fShellLengthRandom / m_fFurLength));

            uiCurrentVert++;
        }

        fShellLength += fShellLengthIncrement;

        // Duplicate TriList Data
        for(NiUInt32 uiIndex = 0; uiIndex < uiIndexCount; uiIndex++)
        {
            kNewIndexIter[uiCurrentIndex++] = kIndexIter[uiIndex] +
                (uiVertCount * uiShell);
        }
    }

    // Generate Texture Coordinates. Two texture coordinates sets are used for
    // the shell texture, the first is the raw texture coordinates pulled in 
    // from the model, the second set is used by the shader to determine which
    // cell in the shell texture atlas to use.
    for(NiUInt32 uiTri = 0; uiTri < uiNewIndexCount;  uiTri+=3)
    {
        NiUInt32 uiIndex1 = kNewIndexIter[uiTri];
        NiUInt32 uiIndex2 = kNewIndexIter[uiTri + 1];
        NiUInt32 uiIndex3 = kNewIndexIter[uiTri + 2];

        NiPoint3 kPoint1 = kNewPositionIter[ uiIndex1 ];
        NiPoint3 kPoint2 = kNewPositionIter[ uiIndex2 ];
        NiPoint3 kPoint3 = kNewPositionIter[ uiIndex3 ];

        NiUInt32 uiShelLevel = uiTri  / uiIndexCount;

        NiUInt32 xOffset = 
            (uiShelLevel%m_uiNumberOfCells) * m_uiShellTextureCellSize;
        NiUInt32 yOffset = 
            ((uiShelLevel-(uiShelLevel%m_uiNumberOfCells))/m_uiNumberOfCells) *
            m_uiShellTextureCellSize;

        xOffset += m_uiShellTextureCellSize / 2;
        yOffset += m_uiShellTextureCellSize / 2;

        kNewTexCoordIter1[uiIndex1].x = (float)xOffset / m_fShellTextureSize;
        kNewTexCoordIter1[uiIndex1].y = (float)yOffset / m_fShellTextureSize;

        kNewTexCoordIter1[uiIndex2].x = (float)xOffset / m_fShellTextureSize;
        kNewTexCoordIter1[uiIndex2].y = (float)yOffset / m_fShellTextureSize;

        kNewTexCoordIter1[uiIndex3].x = (float)xOffset / m_fShellTextureSize;
        kNewTexCoordIter1[uiIndex3].y = (float)yOffset / m_fShellTextureSize;
    }

    pkNewMesh->SetName(cpName);

    NiVertexColorProperty* pkVCProp = NiNew NiVertexColorProperty();
    pkVCProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E_A_D);
    pkVCProp->SetSourceMode(NiVertexColorProperty::SOURCE_AMB_DIFF);
    pkNewMesh->AttachProperty(pkVCProp);

    NiMaterialProperty* pkMatProp = NiNew NiMaterialProperty();
    pkNewMesh->AttachProperty(pkMatProp);

    NiTexturingProperty* pkOutTexProp = NiNew NiTexturingProperty();
    pkOutTexProp->SetBaseTexture(pkTextureProp->GetBaseTexture());

    NiTexturingProperty::ShaderMap* pkShaderMap = 
        NiNew NiTexturingProperty::ShaderMap(m_pkShellTexture, 1);
    pkOutTexProp->SetShaderMap(0, pkShaderMap);
    pkNewMesh->AttachProperty(pkOutTexProp);

    NiSkinningMeshModifier* pkNewSkinningMeshModifier = NULL;

    // Assign shell fur shader to geometry
    if (pkSkinningMeshModifier)
    {
        NiDataStreamElementLock kBoneLock(pkMesh, 
            NiCommonSemantics::BLENDINDICES(), 0, 
            NiDataStreamElement::F_INT16_4,
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);

        NiDataStreamElementLock kWeightLock(pkMesh, 
            NiCommonSemantics::BLENDWEIGHT(), 0, 
            NiDataStreamElement::F_FLOAT32_4,
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);

        NiDataStreamElementLock kRemapLock(pkMesh, 
            NiCommonSemantics::BONE_PALETTE(), 0,
            NiDataStreamElement::F_UINT16_1,
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);

        NiTStridedRandomAccessIterator<NiUInt16> kRemapIter;
        if (kRemapLock.DataStreamExists())
            kRemapIter = kRemapLock.begin<NiUInt16>();

        if (!pkMorphMeshModifier)
        {
            NiToolDataStream* pkDS = 
                (NiToolDataStream*)kNewPositionLock.GetDataStream();
            pkDS->SetAccessMask(NiDataStream::ACCESS_GPU_READ |
                NiDataStream::ACCESS_CPU_WRITE_VOLATILE);

            pkDS = (NiToolDataStream*)kNewNormalLock.GetDataStream();
            pkDS->SetAccessMask(NiDataStream::ACCESS_GPU_READ |
                NiDataStream::ACCESS_CPU_WRITE_VOLATILE);
        }

        pkNewSkinningMeshModifier = 
            CloneSkinningForShells(pkNewMesh, pkSkinningMeshModifier, 
            m_uiNumberOfShells, uiVertCount,
            kBoneLock.begin<NiTSimpleArray<NiInt16,4> >(),
            kWeightLock.begin<NiTSimpleArray<float,4> >(),
            kRemapIter,
            kNewPositionIter,
            kNewNormalIter,
            pkMorphMeshModifier != NULL);

        pkNewMesh->AddModifier(pkNewSkinningMeshModifier);
    }

    NiSingleShaderMaterial* pkShaderMat = 
        NiSingleShaderMaterial::Create(m_kShellsShaderName,false);
    pkNewMesh->ApplyMaterial(pkShaderMat);
    pkNewMesh->SetActiveMaterial(pkShaderMat);

    NiIntegerExtraData* pkBonesPerPart = 
        NiNew NiIntegerExtraData(m_uiBonesPerPartition);
    pkBonesPerPart->SetName(NI_BONES_PER_PARTITION_KEY);
    pkNewMesh->AddExtraData(pkBonesPerPart);

    // Get the morphing modifier.
    if (pkMorphMeshModifier)
    {
        NiFixedString kMorphSemantic;
        bool bIsMorphed = false;
        NiUInt32 uiIndex;
        if (pkNewSkinningMeshModifier)
        {
            bIsMorphed = pkMorphMeshModifier->IsMorphedElement(
                NiCommonSemantics::POSITION_BP(), 0, uiIndex);
            kMorphSemantic = pkMorphMeshModifier->MorphElementSemantic(
                NiCommonSemantics::POSITION_BP());
        }
        else
        {
            bIsMorphed = pkMorphMeshModifier->IsMorphedElement(
                NiCommonSemantics::POSITION(), 0, uiIndex);
            kMorphSemantic = pkMorphMeshModifier->MorphElementSemantic(
                NiCommonSemantics::POSITION());
        }

        if (bIsMorphed)
        {
            NiMorphMeshModifier* pkNewMorphMeshModifier = 
                CloneMorphingForShells(pkNewMesh, pkMesh, pkMorphMeshModifier,
                kMorphSemantic, m_uiNumberOfShells, uiVertCount);
            pkNewMesh->AddModifierAt(0, pkNewMorphMeshModifier);

            // Get the controller
            NiMorphWeightsController* pkMorph = (NiMorphWeightsController*)
                pkMesh->GetController((&NiMorphWeightsController::ms_RTTI));
            if (pkMorph)
            {
                NiMorphWeightsController* pkNewMorph = 
                    (NiMorphWeightsController*)pkMorph->Clone();
                pkNewMorph->SetTarget(pkNewMesh);
            }
        }
    }

    // Copy over all the extra attributes from the source mesh
    NiUInt32 uiCount = pkMesh->GetExtraDataSize();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        NiExtraData* pkExtraData = pkMesh->GetExtraDataAt(
            (unsigned short)ui);
        pkNewMesh->AddExtraData(pkExtraData);
    }

    NiFloatExtraData* pkData = NiNew NiFloatExtraData(m_fFurDensity);
    pkNewMesh->AddExtraData(
        NiFixedString("ShellFurDensity"), pkData);

    NiColorExtraData* pkColorData = NiNew NiColorExtraData(m_kFurTintColor);
    pkNewMesh->AddExtraData(
        NiFixedString("FurTintColor"), pkColorData);

    float fVal = (float)m_uiShellTextureCellSize / m_fShellTextureSize;
    pkData = NiNew NiFloatExtraData(fVal);
    pkNewMesh->AddExtraData(NiFixedString("ShellTextureCellUVSize"), pkData);

    fVal = fVal * 0.5f;
    pkData = NiNew NiFloatExtraData(fVal);
    pkNewMesh->AddExtraData(
        NiFixedString("ShellTextureCellUVHalfSize"), pkData);

    pkNewMesh->RecomputeBounds();

    pkSortNode->AttachChild(pkNewMesh);

    pkNewMesh->UpdateProperties();
    pkNewMesh->UpdateEffects();
}

//---------------------------------------------------------------------------
NiMeshPtr NiFurGeneratorPlugin::GenerateFurMeshFromMesh(NiMesh* pkMesh)
{
    NiUInt32 uiVertCount = pkMesh->GetVertexCount();
    NiUInt32 uiIndexCount = pkMesh->GetTotalCount(NiCommonSemantics::INDEX());

    NiDataStreamElementLock kTexCoordLock(pkMesh, 
        NiCommonSemantics::TEXCOORD(), 0, NiDataStreamElement::F_FLOAT32_2,
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
    NiTStridedRandomAccessIterator<NiPoint2> kTexCoordIter = 
        kTexCoordLock.begin<NiPoint2>();

    NiDataStreamElementLock kIndexLock(pkMesh, NiCommonSemantics::INDEX(),
        0, NiDataStreamElement::F_UNKNOWN,
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
    NiTStridedRandomAccessIterator<NiUInt32> kIndexIter = 
        kIndexLock.begin<NiUInt32>();

    // Count the new number of vertices and polygons
    NiUInt32 uiNewVertCount = uiVertCount;
    NiUInt32 uiNewIndexCount = uiIndexCount;
    for(NiUInt32 uiTri = 0; uiTri < uiIndexCount;  uiTri+=3)
    {
        NiUInt32 uiIndex0 = kIndexIter[uiTri];
        NiUInt32 uiIndex1 = kIndexIter[uiTri + 1];
        NiUInt32 uiIndex2 = kIndexIter[uiTri + 2];

        NiPoint2 kTex0 = kTexCoordIter[uiIndex0] * m_fFurDensity;
        NiPoint2 kTex1 = kTexCoordIter[uiIndex1] * m_fFurDensity;
        NiPoint2 kTex2 = kTexCoordIter[uiIndex2] * m_fFurDensity;
        NeedsSubdivision(kTex0, kTex1, kTex2, uiNewVertCount, uiNewIndexCount);
    }

    NiMeshPtr spNewMesh = pkMesh;

    // Check to see if we need to subdivide.
    if (uiNewVertCount != uiVertCount && uiNewIndexCount != uiIndexCount)
    {
        NiTexturingProperty* pkTextureProp = 
            (NiTexturingProperty*)pkMesh->GetProperty(NiProperty::TEXTURING);
        if (!pkTextureProp)
        {
            NILOG("Could not find a texture property on mesh.");
            return spNewMesh;
        }

        // Get the modifiers
        NiMorphMeshModifier* pkMorphMeshModifier = 
            (NiMorphMeshModifier*)pkMesh->GetModifierByType(
            (&NiMorphMeshModifier::ms_RTTI));

        NiSkinningMeshModifier* pkSkinningMeshModifier = 
            (NiSkinningMeshModifier*)pkMesh->GetModifierByType(
            (&NiSkinningMeshModifier::ms_RTTI));

        NiDataStreamElementLock kPositionLock(pkMesh, 
            (!pkSkinningMeshModifier) ? 
            NiCommonSemantics::POSITION() : NiCommonSemantics::POSITION_BP(),
            0, NiDataStreamElement::F_FLOAT32_3,
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
        NiTStridedRandomAccessIterator<NiPoint3> kPositionIter = 
            kPositionLock.begin<NiPoint3>();

        NiDataStreamElementLock kNormalLock(pkMesh, 
            (!pkSkinningMeshModifier) ? 
            NiCommonSemantics::NORMAL() : NiCommonSemantics::NORMAL_BP(),
            0, NiDataStreamElement::F_FLOAT32_3,
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
        NiTStridedRandomAccessIterator<NiPoint3> kNormalIter = 
            kNormalLock.begin<NiPoint3>();

        NiDataStreamElementLock kColorLock(pkMesh, NiCommonSemantics::COLOR(),
            0, NiDataStreamElement::F_FLOAT32_4,
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
        NiTStridedRandomAccessIterator<NiColorA> kColorIter;
        if (kColorLock.DataStreamExists())
            kColorIter = kColorLock.begin<NiColorA>();

        // Get skinning data if it exists
        NiDataStreamElementLock kBoneLock(pkMesh, 
            NiCommonSemantics::BLENDINDICES(), 0, 
            NiDataStreamElement::F_INT16_4,
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
        NiTStridedRandomAccessIterator<NiTSimpleArray<NiInt16,4> > kBoneIter;
        if (kBoneLock.DataStreamExists())
            kBoneIter = kBoneLock.begin<NiTSimpleArray<NiInt16,4> >();

        NiDataStreamElementLock kWeightLock(pkMesh, 
            NiCommonSemantics::BLENDWEIGHT(), 0, 
            NiDataStreamElement::F_FLOAT32_4,
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
        NiTStridedRandomAccessIterator<NiTSimpleArray<float,4> > kWeightIter;
        if (kWeightLock.DataStreamExists())
            kWeightIter = kWeightLock.begin<NiTSimpleArray<float,4> >();

        NiDataStreamElementLock kRemapLock(pkMesh, 
            NiCommonSemantics::BONE_PALETTE(), 0,
            NiDataStreamElement::F_UINT16_1,
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
        NiTStridedRandomAccessIterator<NiUInt16> kRemapIter;
        if (kRemapLock.DataStreamExists())
            kRemapIter = kRemapLock.begin<NiUInt16>();

        NiDataStreamElementLock kBindPosLock(pkMesh, 
            NiCommonSemantics::POSITION_BP(), 0, 
            NiDataStreamElement::F_FLOAT32_3,
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
        NiTStridedRandomAccessIterator<NiPoint3> kBindPosIter;
        if (kBindPosLock.DataStreamExists())
            kBindPosIter = kBindPosLock.begin<NiPoint3>();

        NiDataStreamElementLock kBindNormLock(pkMesh, 
            NiCommonSemantics::NORMAL_BP(), 0, 
            NiDataStreamElement::F_FLOAT32_3,
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
        NiTStridedRandomAccessIterator<NiPoint3> kBindNormIter;
        if (kBindNormLock.DataStreamExists())
            kBindNormIter = kBindNormLock.begin<NiPoint3>();

        NiDataStreamElementLock kMorphWeightsLock(pkMesh, 
            NiCommonSemantics::MORPHWEIGHTS(), 0,
            NiDataStreamElement::F_FLOAT32_1, 
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
        NiTStridedRandomAccessIterator<float> kMorphWeightsIter;
        if (kMorphWeightsLock.DataStreamExists())
            kMorphWeightsIter = kMorphWeightsLock.begin<float>();

        NiUInt32 uiNumTargets = 0;
        NiDataStreamElementLock** ppkMorphPositionsLock = NULL;
        NiTStridedRandomAccessIterator<NiPoint3>* pkMorphPositionsIter = NULL;

        NiFixedString kMorphSemantic;
        bool bIsMorphed = pkMorphMeshModifier != NULL;

        if (bIsMorphed)
        {
            NiUInt32 uiIndex;
            if (pkSkinningMeshModifier)
            {
                bIsMorphed = pkMorphMeshModifier->IsMorphedElement(
                    NiCommonSemantics::POSITION_BP(), 0, uiIndex);
                kMorphSemantic = pkMorphMeshModifier->MorphElementSemantic(
                    NiCommonSemantics::POSITION_BP());
            }
            else
            {
                bIsMorphed = pkMorphMeshModifier->IsMorphedElement(
                    NiCommonSemantics::POSITION(), 0, uiIndex);
                kMorphSemantic = pkMorphMeshModifier->MorphElementSemantic(
                    NiCommonSemantics::POSITION());
            }

            uiNumTargets = pkMorphMeshModifier->GetNumTargets();
            ppkMorphPositionsLock = 
                NiAlloc(NiDataStreamElementLock*, uiNumTargets);
            pkMorphPositionsIter = 
                NiExternalNew NiTStridedRandomAccessIterator<NiPoint3>[
                    uiNumTargets];
            for (NiUInt32 uiTarget = 0; uiTarget < uiNumTargets; uiTarget++)
            {
                ppkMorphPositionsLock[uiTarget] = 
                    NiNew NiDataStreamElementLock(pkMesh, 
                    kMorphSemantic, uiTarget, 
                    NiDataStreamElement::F_FLOAT32_3,
                    NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
                pkMorphPositionsIter[uiTarget] = 
                    ppkMorphPositionsLock[uiTarget]->begin<NiPoint3>();
            }
        }

        spNewMesh = NiNew NiMesh();
        spNewMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);

        // Add the position stream
        NiDataStreamElementLock kNewPositionLock = spNewMesh->AddStreamGetLock(
            NiCommonSemantics::POSITION(), 0, NiDataStreamElement::F_FLOAT32_3,
            uiNewVertCount, 
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX, true);
        NiTStridedRandomAccessIterator<NiPoint3> kNewPositionIter = 
            kNewPositionLock.begin<NiPoint3>();

        // Add the normal stream
        NiDataStreamElementLock kNewNormalLock = spNewMesh->AddStreamGetLock(
            NiCommonSemantics::NORMAL(), 0, NiDataStreamElement::F_FLOAT32_3, 
            uiNewVertCount, 
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX, true);
        NiTStridedRandomAccessIterator<NiPoint3> kNewNormalIter = 
            kNewNormalLock.begin<NiPoint3>();

        // Add the texture coordinate stream
        NiDataStreamElementLock kNewTexCoordLock = 
            spNewMesh->AddStreamGetLock(
            NiCommonSemantics::TEXCOORD(), 0, NiDataStreamElement::F_FLOAT32_2,
            uiNewVertCount, 
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX, true);
        NiTStridedRandomAccessIterator<NiPoint2> kNewTexCoordIter = 
            kNewTexCoordLock.begin<NiPoint2>();

        // Add the color stream
        NiDataStreamElementLock kNewColorLock = spNewMesh->AddStreamGetLock(
            NiCommonSemantics::COLOR(), 0, NiDataStreamElement::F_FLOAT32_4, 
            uiNewVertCount, 
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX, true);
        NiTStridedRandomAccessIterator<NiColorA> kNewColorIter = 
            kNewColorLock.begin<NiColorA>();

        // Add skinning data if we need to

        // Create the blend indicies
        NiDataStreamElementLock kNewBoneLock;
        NiTStridedRandomAccessIterator<NiTSimpleArray<NiInt16,4> > 
            kNewBoneIter;
        if (kBoneIter.Exists())
        {
            kNewBoneLock = 
                spNewMesh->AddStreamGetLock(
                NiCommonSemantics::BLENDINDICES(), 0, 
                NiDataStreamElement::F_INT16_4, uiNewVertCount, 
                NiDataStream::ACCESS_CPU_READ |
                NiDataStream::ACCESS_CPU_WRITE_STATIC,
                NiDataStream::USAGE_VERTEX, true);
            kNewBoneIter = kNewBoneLock.begin<NiTSimpleArray<NiInt16,4> >();

        }

        // Create the weights
        NiDataStreamElementLock kNewWeightLock;
        NiTStridedRandomAccessIterator<NiTSimpleArray<float,4> > 
            kNewWeightIter;
        if (kWeightIter.Exists())
        {
            kNewWeightLock = 
                spNewMesh->AddStreamGetLock(
                NiCommonSemantics::BLENDWEIGHT(), 0, 
                NiDataStreamElement::F_FLOAT32_4, uiNewVertCount, 
                NiDataStream::ACCESS_CPU_READ |
                NiDataStream::ACCESS_CPU_WRITE_STATIC,
                NiDataStream::USAGE_VERTEX, true);
            kNewWeightIter = kNewWeightLock.begin<NiTSimpleArray<float,4> >();
        }

        // Create the bind position
        NiDataStreamElementLock kNewBindPosLock;
        NiTStridedRandomAccessIterator<NiPoint3> kNewBindPosIter;
        if (kBindPosIter.Exists())
        {
            kNewBindPosLock = 
                spNewMesh->AddStreamGetLock(
                NiCommonSemantics::POSITION_BP(), 0, 
                NiDataStreamElement::F_FLOAT32_3, uiNewVertCount, 
                NiDataStream::ACCESS_CPU_READ |
                NiDataStream::ACCESS_CPU_WRITE_STATIC, 
                NiDataStream::USAGE_VERTEX, true);
            kNewBindPosIter = kNewBindPosLock.begin<NiPoint3>();
        }

        // Create the bind normal
        NiDataStreamElementLock kNewBindNormLock;
        NiTStridedRandomAccessIterator<NiPoint3> kNewBindNormIter;
        if (kBindNormIter.Exists())
        {
            kNewBindNormLock = 
                spNewMesh->AddStreamGetLock(
                NiCommonSemantics::NORMAL_BP(), 0, 
                NiDataStreamElement::F_FLOAT32_3, uiNewVertCount, 
                NiDataStream::ACCESS_CPU_READ |
                NiDataStream::ACCESS_CPU_WRITE_STATIC, 
                NiDataStream::USAGE_VERTEX, true);
            kNewBindNormIter = kNewBindNormLock.begin<NiPoint3>();
        }

        // Create the bone palette
        NiDataStreamElementLock kNewRemapLock;
        NiTStridedRandomAccessIterator<NiUInt16> kNewRemapIter;
        if (kRemapIter.Exists())
        {
            NiUInt32 uiBoneCount = kRemapLock.count(0);
            kNewRemapLock = 
                spNewMesh->AddStreamGetLock(
                NiCommonSemantics::BONE_PALETTE(), 0, 
                NiDataStreamElement::F_UINT16_1, uiBoneCount,
                NiDataStream::ACCESS_CPU_READ |
                NiDataStream::ACCESS_CPU_WRITE_STATIC,
                NiDataStream::USAGE_USER, true);
            kNewRemapIter = kNewRemapLock.begin<NiUInt16>();
            for (NiUInt32 ui=0; ui < uiBoneCount; ui++)
            {
                kNewRemapIter[ui] = kRemapIter[ui];
            }
        }

        NiDataStreamElementLock* pkNewMorphPositionsLock = NULL;
        NiTStridedRandomAccessIterator<NiPoint3>* pkNewMorphPositionsIter = 
            NULL;
        NiDataStreamElementLock kNewMorphWeightsLock;
        NiTStridedRandomAccessIterator<float> kNewMorphWeightsIter;
        if (bIsMorphed)
        {
            // Create the morph weights
            NiDataStreamElementLock kNewMorphWeightsLock = 
                spNewMesh->AddStreamGetLock(
                NiCommonSemantics::MORPHWEIGHTS(), 0, 
                NiDataStreamElement::F_FLOAT32_1, uiNumTargets, 
                NiDataStream::ACCESS_CPU_READ |
                NiDataStream::ACCESS_CPU_WRITE_MUTABLE, 
                NiDataStream::USAGE_USER, true);
            NiTStridedRandomAccessIterator<float> kNewMorphWeightsIter =
                kNewMorphWeightsLock.begin<float>();

            for(NiUInt32 uiTarget = 0; uiTarget < uiNumTargets; uiTarget++)
            {
                kNewMorphWeightsIter[uiTarget] = kMorphWeightsIter[uiTarget];
            }

            pkNewMorphPositionsLock = 
                NiNew NiDataStreamElementLock[uiNumTargets];
            pkNewMorphPositionsIter = 
                NiExternalNew NiTStridedRandomAccessIterator<NiPoint3>[
                    uiNumTargets];
            for (NiUInt32 uiTarget = 0; uiTarget < uiNumTargets; uiTarget++)
            {
                pkNewMorphPositionsLock[uiTarget] = 
                    spNewMesh->AddStreamGetLock(
                    kMorphSemantic, uiTarget, 
                    NiDataStreamElement::F_FLOAT32_3, uiNewVertCount, 
                    NiDataStream::ACCESS_CPU_READ |
                    NiDataStream::ACCESS_CPU_WRITE_STATIC, 
                    NiDataStream::USAGE_VERTEX, true);
                pkNewMorphPositionsIter[uiTarget] = 
                    pkNewMorphPositionsLock[uiTarget].begin<NiPoint3>();
            }
        }

        // Add the index stream
        NiDataStreamElementLock kNewIndexLock = spNewMesh->AddStreamGetLock(
            NiCommonSemantics::INDEX(), 0, NiDataStreamElement::F_UINT32_1,
            uiNewIndexCount, 
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX_INDEX, true);
        NiTStridedRandomAccessIterator<NiUInt32> kNewIndexIter = 
            kNewIndexLock.begin<NiUInt32>();

        NiUInt32 uiNewIndex = 0;
        for(NiUInt32 uiTri = 0; uiTri < uiIndexCount;  uiTri+=3)
        {
            NiUInt32 uiIndex0 = kIndexIter[uiTri];
            NiUInt32 uiIndex1 = kIndexIter[uiTri + 1];
            NiUInt32 uiIndex2 = kIndexIter[uiTri + 2];

            // Copy the vertex data
            kNewPositionIter[uiIndex0] = kPositionIter[uiIndex0];
            kNewPositionIter[uiIndex1] = kPositionIter[uiIndex1];
            kNewPositionIter[uiIndex2] = kPositionIter[uiIndex2];

            kNewNormalIter[uiIndex0] = kNormalIter[uiIndex0];
            kNewNormalIter[uiIndex1] = kNormalIter[uiIndex1];
            kNewNormalIter[uiIndex2] = kNormalIter[uiIndex2];

            kNewTexCoordIter[uiIndex0] = 
                kTexCoordIter[uiIndex0];
            kNewTexCoordIter[uiIndex1] = 
                kTexCoordIter[uiIndex1];
            kNewTexCoordIter[uiIndex2] = 
                kTexCoordIter[uiIndex2];

            if (kColorIter.Exists())
            {
                kNewColorIter[uiIndex0] = kColorIter[uiIndex0];
                kNewColorIter[uiIndex1] = kColorIter[uiIndex1];
                kNewColorIter[uiIndex2] = kColorIter[uiIndex2];
            }
            else
            {
                kNewColorIter[uiIndex0] = NiColorA(1,1,1,1);
                kNewColorIter[uiIndex1] = NiColorA(1,1,1,1);
                kNewColorIter[uiIndex2] = NiColorA(1,1,1,1);
            }

            if (kNewBoneIter.Exists())
            {
                kNewBoneIter[uiIndex0] = kBoneIter[uiIndex0];
                kNewBoneIter[uiIndex1] = kBoneIter[uiIndex1];
                kNewBoneIter[uiIndex2] = kBoneIter[uiIndex2];
            }

            if (kNewWeightIter.Exists())
            {
                kNewWeightIter[uiIndex0] = kWeightIter[uiIndex0];
                kNewWeightIter[uiIndex1] = kWeightIter[uiIndex1];
                kNewWeightIter[uiIndex2] = kWeightIter[uiIndex2];
            }

            if (kNewBindPosIter.Exists())
            {
                kNewBindPosIter[uiIndex0] = kBindPosIter[uiIndex0];
                kNewBindPosIter[uiIndex1] = kBindPosIter[uiIndex1];
                kNewBindPosIter[uiIndex2] = kBindPosIter[uiIndex2];
            }

            if (kNewBindNormIter.Exists())
            {
                kNewBindNormIter[uiIndex0] = kBindNormIter[uiIndex0];
                kNewBindNormIter[uiIndex1] = kBindNormIter[uiIndex1];
                kNewBindNormIter[uiIndex2] = kBindNormIter[uiIndex2];
            }

            if (pkNewMorphPositionsIter)
            {
                for (NiUInt32 uiTarget = 0; uiTarget < uiNumTargets;
                    uiTarget++)
                {
                    pkNewMorphPositionsIter[uiTarget][uiIndex0] = 
                        pkMorphPositionsIter[uiTarget][uiIndex0];
                    pkNewMorphPositionsIter[uiTarget][uiIndex1] = 
                        pkMorphPositionsIter[uiTarget][uiIndex1];
                    pkNewMorphPositionsIter[uiTarget][uiIndex2] = 
                        pkMorphPositionsIter[uiTarget][uiIndex2];
                }
            }

            NiPoint2 kTex0 = kTexCoordIter[uiIndex0] * m_fFurDensity;
            NiPoint2 kTex1 = kTexCoordIter[uiIndex1] * m_fFurDensity;
            NiPoint2 kTex2 = kTexCoordIter[uiIndex2] * m_fFurDensity;

            if (!NeedsSubdivision(kTex0, kTex1, kTex2))
            {
                kNewIndexIter[uiNewIndex++] = uiIndex0;
                kNewIndexIter[uiNewIndex++] = uiIndex1;
                kNewIndexIter[uiNewIndex++] = uiIndex2;
            }
            else
            {
                SubdivideTriangle(uiIndex0, uiIndex1, uiIndex2,
                    kNewPositionIter,
                    kNewNormalIter,
                    kNewTexCoordIter,
                    kNewColorIter,
                    kNewBoneIter,
                    kNewWeightIter,
                    kNewBindPosIter,
                    kNewBindNormIter,
                    uiNumTargets,
                    pkNewMorphPositionsIter,
                    kNewIndexIter,
                    uiVertCount,
                    uiNewIndex);
            }
        }

        // Sanity check
        NIASSERT(uiVertCount == uiNewVertCount && 
            uiNewIndex == uiNewIndexCount);

        // Assign morphing to the new mesh
        if (bIsMorphed)
        {
            NiMorphMeshModifier* pkNewMorphMeshModifier = 
                (NiMorphMeshModifier*)pkMorphMeshModifier->Clone();

            spNewMesh->AddModifier(pkNewMorphMeshModifier);

            // Get the controller
            NiMorphWeightsController* pkMorph = (NiMorphWeightsController*)
                pkMesh->GetController((&NiMorphWeightsController::ms_RTTI));
            if (pkMorph)
            {
                NiMorphWeightsController* pkNewMorph = 
                    (NiMorphWeightsController*)pkMorph->Clone();
                pkNewMorph->SetTarget(spNewMesh);
            }
        }

        // Assign skinning to the new mesh
        if (pkSkinningMeshModifier)
        {
            // Change the position stream to writable
            NiToolDataStream* pkDS = 
                (NiToolDataStream*)kNewPositionLock.GetDataStream();
            if (pkMorphMeshModifier)
            {
                pkDS->SetAccessMask(NiDataStream::ACCESS_GPU_READ |
                    NiDataStream::ACCESS_CPU_WRITE_MUTABLE);
            }
            else
            {
                pkDS->SetAccessMask(NiDataStream::ACCESS_GPU_READ |
                    NiDataStream::ACCESS_CPU_WRITE_VOLATILE);
            }

            // Change the normal stream to writable
            pkDS = (NiToolDataStream*)kNewNormalLock.GetDataStream();
            if (pkMorphMeshModifier)
            {
                pkDS->SetAccessMask(NiDataStream::ACCESS_GPU_READ |
                    NiDataStream::ACCESS_CPU_WRITE_MUTABLE);
            }
            else
            {
                pkDS->SetAccessMask(NiDataStream::ACCESS_GPU_READ |
                    NiDataStream::ACCESS_CPU_WRITE_VOLATILE);
            }

            NiSkinningMeshModifier* pkNewSkinningMeshModifier = 
                (NiSkinningMeshModifier*)pkSkinningMeshModifier->Clone();

            spNewMesh->AddModifier(pkNewSkinningMeshModifier);

            // Destroy the allocated data
            if (bIsMorphed)
            {
                for (NiUInt32 uiTarget = 0; uiTarget < uiNumTargets;
                    uiTarget++)
                {
                    NiDelete ppkMorphPositionsLock[uiTarget];
                }
                NiFree(ppkMorphPositionsLock);
                NiExternalDelete [] pkMorphPositionsIter;

                NiDelete [] pkNewMorphPositionsLock;
                NiExternalDelete [] pkNewMorphPositionsIter;
            }
        }

        spNewMesh->RecomputeBounds();

        // Add the property and effect state to the mesh
        spNewMesh->SetPropertyState(pkMesh->GetPropertyState());
        spNewMesh->SetEffectState(pkMesh->GetEffectState());

        // Copy over all the extra attributes from the source mesh
        NiUInt32 uiCount = pkMesh->GetExtraDataSize();
        for (NiUInt32 ui = 0; ui < uiCount; ui++)
        {
            NiExtraData* pkExtraData = pkMesh->GetExtraDataAt(
                (unsigned short)ui);
            spNewMesh->AddExtraData(pkExtraData);
        }

        // Add all the properties to this mesh
        NiTListIterator kIter = pkMesh->GetPropertyList().GetHeadPos();
        while (kIter != NULL)
        {
            NiProperty* pkProp = pkMesh->GetPropertyList().GetNext(kIter);
            if (pkProp)
                spNewMesh->AttachProperty(pkProp);
        }

        // Add all the materials to this mesh
        for (NiUInt32 ui = 0; ui < pkMesh->GetMaterialCount(); ui++)
        {
            spNewMesh->ApplyAndSetActiveMaterial(
                pkMesh->GetMaterialInstance(ui)->GetMaterial(),
                pkMesh->GetMaterialInstance(ui)->GetMaterialExtraData());
        }
    }

    return spNewMesh;
}
//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::GenerateFinsFromMesh(NiMesh* pkMesh, 
    NiSortAdjustNode* pkSortNode, char* cpName)
{
    pkMesh->GetVertexCount();
    NiUInt32 uiIndexCount = pkMesh->GetTotalCount(NiCommonSemantics::INDEX());
    NiUInt32 uiPolyCount = 
        pkMesh->GetPrimitiveCountFromElementCount(uiIndexCount);

    NiTexturingProperty* pkTextureProp = 
        (NiTexturingProperty*)pkMesh->GetProperty(NiProperty::TEXTURING);
    if (!pkTextureProp)
    {
        NILOG("Could not find a texture property on mesh.");
        return;
    }

    NiTexturingProperty::Map* pkDetailMap = 
        pkTextureProp->GetDetailMap();
    if (!pkDetailMap)
    {
        NILOG("Could not find detail map on mesh.");
        return;
    }

    // Get the skinning modifier.
    NiSkinningMeshModifier* pkSkinningMeshModifier = 
        (NiSkinningMeshModifier*)pkMesh->GetModifierByType(
        (&NiSkinningMeshModifier::ms_RTTI));

    NiMorphMeshModifier* pkMorphMeshModifier = 
        (NiMorphMeshModifier*)pkMesh->GetModifierByType(
        (&NiMorphMeshModifier::ms_RTTI));

    NiDataStreamElementLock kPositionLock(pkMesh, 
        (!pkSkinningMeshModifier) ? 
        NiCommonSemantics::POSITION() : NiCommonSemantics::POSITION_BP(),
        0, NiDataStreamElement::F_FLOAT32_3, 
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
    if (!kPositionLock.DataStreamExists())
    {
        NILOG("Could not lock position data stream.");
        return;
    }
    NiTStridedRandomAccessIterator<NiPoint3> kPositionIter = 
        kPositionLock.begin<NiPoint3>();

    NiDataStreamElementLock kNormalLock(pkMesh, 
        (!pkSkinningMeshModifier) ? 
        NiCommonSemantics::NORMAL() : NiCommonSemantics::NORMAL_BP(),
        0, NiDataStreamElement::F_FLOAT32_3, 
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
    if (!kNormalLock.DataStreamExists())
    {
        NILOG("Could not lock normal data stream.");
        return;
    }
    NiTStridedRandomAccessIterator<NiPoint3> kNormalIter = 
        kNormalLock.begin<NiPoint3>();

    NiDataStreamElementLock kTexCoordLock0(pkMesh, 
        NiCommonSemantics::TEXCOORD(), 0, NiDataStreamElement::F_FLOAT32_2, 
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
    if (!kTexCoordLock0.DataStreamExists())
    {
        NILOG("Could not lock texture data stream.");
        return;
    }
    NiTStridedRandomAccessIterator<NiPoint2> kTexCoordIter0 = 
        kTexCoordLock0.begin<NiPoint2>();

    NiDataStreamElementLock kColorLock(pkMesh, NiCommonSemantics::COLOR(),
        0, NiDataStreamElement::F_FLOAT32_4, 
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
    NiTStridedRandomAccessIterator<NiColorA> kColorIter;
    if (kColorLock.DataStreamExists())
        kColorIter = kColorLock.begin<NiColorA>();

    NiDataStreamElementLock kIndexLock(pkMesh, NiCommonSemantics::INDEX(),
        0, NiDataStreamElement::F_UNKNOWN,
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
    NiTStridedRandomAccessIterator<NiUInt32> kIndexIter =
        kIndexLock.begin<NiUInt32>();

    NiUInt32 uiaEdge1[2];
    NiUInt32 uiaEdge2[2];
    NiUInt32 uiaEdge3[2];

    NiUInt32* puiEdgeList = (NiUInt32*)
        NiMalloc(sizeof(NiUInt32) * uiIndexCount * 2);

    NiUInt32 uiEdgeCount = 0;

    // Create a list of all the edges in the mesh
    for(NiUInt32 uiPoly = 0; uiPoly < uiPolyCount; uiPoly++)
    { 
        NiUInt32 uiIndex = (uiPoly * 3);
        uiaEdge1[0] = kIndexIter[uiIndex];
        uiaEdge1[1] = kIndexIter[uiIndex+1];

        uiaEdge2[0] = kIndexIter[uiIndex+1];
        uiaEdge2[1] = kIndexIter[uiIndex+2];

        uiaEdge3[0] = kIndexIter[uiIndex+2];
        uiaEdge3[1] = kIndexIter[uiIndex];

        // See if edge1 is already in the list, if not add it
        if(!IsEdgeInList(uiaEdge1, puiEdgeList, uiEdgeCount))
        {
            puiEdgeList[(uiEdgeCount<<1)] = uiaEdge1[0];
            puiEdgeList[(uiEdgeCount<<1)+1] = uiaEdge1[1];
            uiEdgeCount++;
        }

        // See if edge1 is already in the list, if not add it
        if(!IsEdgeInList(uiaEdge2, puiEdgeList, uiEdgeCount))
        {
            puiEdgeList[(uiEdgeCount<<1)] = uiaEdge2[0];
            puiEdgeList[(uiEdgeCount<<1)+1] = uiaEdge2[1];
            uiEdgeCount++;
        }

        // See if edge1 is already in the list, if not add it
        if(!IsEdgeInList(uiaEdge3, puiEdgeList, uiEdgeCount))
        {
            puiEdgeList[(uiEdgeCount<<1)] = uiaEdge3[0];
            puiEdgeList[(uiEdgeCount<<1)+1] = uiaEdge3[1];
            uiEdgeCount++;
        }
    }

    // Now that we have a list of all the edges in the Mesh we can cycle
    // through each one and generate a fin for the corresponding edge
    NiUInt32 uiOutVertCount = uiEdgeCount*4;
    NiUInt32 uiOutIndexCount = uiEdgeCount*6;

    NiMesh* pkNewMesh = NiNew NiMesh();
    pkNewMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);

    // Add the position stream
    NiDataStreamElementLock kOutPositionLock = pkNewMesh->AddStreamGetLock(
        NiCommonSemantics::POSITION(), 0, NiDataStreamElement::F_FLOAT32_3, 
        uiOutVertCount, (pkMorphMeshModifier) ?
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE :
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX, true);
    NiTStridedRandomAccessIterator<NiPoint3> kOutPositionIter = 
        kOutPositionLock.begin<NiPoint3>();

    // Add the normal stream
    NiDataStreamElementLock kOutNormalLock = pkNewMesh->AddStreamGetLock(
        NiCommonSemantics::NORMAL(), 0, NiDataStreamElement::F_FLOAT32_3, 
        uiOutVertCount, 
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX, true);
    NiTStridedRandomAccessIterator<NiPoint3> kOutNormalIter = 
        kOutNormalLock.begin<NiPoint3>();

    // Add the texture coordinate stream 0
    NiDataStreamElementLock kOutTexCoordLock0 = pkNewMesh->AddStreamGetLock(
        NiCommonSemantics::TEXCOORD(), 0, NiDataStreamElement::F_FLOAT32_2, 
        uiOutVertCount, 
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX, true);
    NiTStridedRandomAccessIterator<NiPoint2> kOutTexCoordIter0 = 
        kOutTexCoordLock0.begin<NiPoint2>();

    // Add the texture coordinate stream 1
    NiDataStreamElementLock kOutTexCoordLock1 = pkNewMesh->AddStreamGetLock(
        NiCommonSemantics::TEXCOORD(), 1, NiDataStreamElement::F_FLOAT32_2, 
        uiOutVertCount, 
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX, true);
    NiTStridedRandomAccessIterator<NiPoint2> kOutTexCoordIter1 = 
        kOutTexCoordLock1.begin<NiPoint2>();

    // Add the color stream
    NiDataStreamElementLock kOutColorLock = pkNewMesh->AddStreamGetLock(
        NiCommonSemantics::COLOR(), 0, NiDataStreamElement::F_FLOAT32_4, 
        uiOutVertCount, 
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX, true);
    NiTStridedRandomAccessIterator<NiColorA> kOutColorIter = 
        kOutColorLock.begin<NiColorA>();

    // Add the index stream
    NiDataStreamElementLock kOutIndexLock = pkNewMesh->AddStreamGetLock(
        NiCommonSemantics::INDEX(), 0, NiDataStreamElement::F_UINT32_1,
        uiOutIndexCount, 
        NiDataStream::ACCESS_GPU_READ | 
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX_INDEX, true);
    NiTStridedRandomAccessIterator<NiUInt32> kOutIndexIter = 
        kOutIndexLock.begin<NiUInt32>();

    NiPoint3 kaVerts[4];
    NiPoint3 kaNormals[4];
    NiPoint2 kaTexCoords0[4];
    NiPoint2 kaTexCoords1[4];
    NiColorA kaColors[4];

    NiUInt32 uiTotalIndicies = 0;

    // From the list of triangle edges create a quad for each edge extruding 
    // out in the direction of the normals on that edge
    for(NiUInt32 e = 0; e < uiEdgeCount; e++)
    {
        NiUInt32 uiIndex0 = puiEdgeList[(e<<1)];
        NiUInt32 uiIndex1 = puiEdgeList[(e<<1)+1];

        kaVerts[0] = kPositionIter[uiIndex0];
        kaVerts[1] = kPositionIter[uiIndex1];
        kaVerts[2] = kPositionIter[uiIndex0];
        kaVerts[3] = kPositionIter[uiIndex1];

        kaNormals[0] = kNormalIter[uiIndex0];
        kaNormals[1] = kNormalIter[uiIndex1];
        kaNormals[2] = kNormalIter[uiIndex0];
        kaNormals[3] = kNormalIter[uiIndex1];

        if (kColorIter.Exists())
        {
            kaColors[0] = kColorIter[uiIndex0];
            kaColors[1] = kColorIter[uiIndex1];
            kaColors[2] = kColorIter[uiIndex0];
            kaColors[3] = kColorIter[uiIndex1];
        }
        else
        {
            kaColors[0] = NiColorA(1,1,1,1);
            kaColors[1] = NiColorA(1,1,1,1);
            kaColors[2] = NiColorA(1,1,1,1);
            kaColors[3] = NiColorA(1,1,1,1);
        }

        // Get Dist between edge points. This is used to ensure that the fin
        // texture is tiled equal along each fin.
        NiPoint3 kEdgeVector = kaVerts[1] - kaVerts[0];
        float fDist = kEdgeVector.Length();

        // Setup Texture Coordinates
        kaTexCoords0[0] = kTexCoordIter0[uiIndex0];
        kaTexCoords0[1] = kTexCoordIter0[uiIndex1];
        kaTexCoords0[2] = kTexCoordIter0[uiIndex0];
        kaTexCoords0[3] = kTexCoordIter0[uiIndex1];

        float fTextureTilesPerFin = m_fTextureTilesPerFin +
            (NiSymmetricRandom() * m_fTextureTilesPerFin * 
            m_fTextureTilesPerFinRandomness);

        kaTexCoords1[0].x = 0.0f;
        kaTexCoords1[0].y = 0.0f;
        kaTexCoords1[1].x = fDist * fTextureTilesPerFin;
        kaTexCoords1[1].y = 0.0f;

        kaTexCoords1[2].x = 0.0f;
        kaTexCoords1[2].y = 0.95f;
        kaTexCoords1[3].x = fDist * fTextureTilesPerFin;
        kaTexCoords1[3].y = 0.95f;

        float fFurLength = m_fFurLength +
            (NiSymmetricRandom() * m_fFurLength * m_fFurLengthRandomness);

        // Extrude Verts 2 and 3 towards normal direction
        kaVerts[2].x += kaNormals[2].x * fFurLength;
        kaVerts[2].y += kaNormals[2].y * fFurLength;
        kaVerts[2].z += kaNormals[2].z * fFurLength;

        kaVerts[3].x += kaNormals[3].x * fFurLength;
        kaVerts[3].y += kaNormals[3].y * fFurLength;
        kaVerts[3].z += kaNormals[3].z * fFurLength;

        // Adjust the alpha
        kaColors[0].a *= 0.0f;
        kaColors[1].a *= 0.0f;

        NiPoint3 kFaceNormal = NiGeometricUtils::GetTriNormal(kaVerts[0],
            kaVerts[1], kaVerts[2]);

        // Copy Data into output data
        for(int i = 0; i < 4; i++)
        {
            NiUInt32 uiIndex = (e*4)+i;

            kOutPositionIter[uiIndex] = kaVerts[i];
            kOutNormalIter[uiIndex] = kFaceNormal;
            kOutColorIter[uiIndex] = kaColors[i];
            kOutTexCoordIter0[uiIndex] = kaTexCoords0[i];
            kOutTexCoordIter1[uiIndex] = kaTexCoords1[i];
        }

        NiUInt32 uiIndex = (e*4);

        // Tri1 of fin
        kOutIndexIter[uiTotalIndicies] = uiIndex;
        kOutIndexIter[uiTotalIndicies+1] = uiIndex+1;
        kOutIndexIter[uiTotalIndicies+2] = uiIndex+2;

        // Tri2 of fin
        kOutIndexIter[uiTotalIndicies+3] = uiIndex+1;
        kOutIndexIter[uiTotalIndicies+4] = uiIndex+3;
        kOutIndexIter[uiTotalIndicies+5] = uiIndex+2;

        uiTotalIndicies += 6;
    }

    // Create the skinning modifier if we need to.
    NiSkinningMeshModifier* pkNewSkinningMeshModifier = NULL;
    if (pkSkinningMeshModifier)
    {
        NiDataStreamElementLock kBoneLock(pkMesh, 
            NiCommonSemantics::BLENDINDICES(), 0, 
            NiDataStreamElement::F_INT16_4,
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);

        NiDataStreamElementLock kWeightLock(pkMesh, 
            NiCommonSemantics::BLENDWEIGHT(), 0, 
            NiDataStreamElement::F_FLOAT32_4,
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);

        NiDataStreamElementLock kRemapLock(pkMesh, 
            NiCommonSemantics::BONE_PALETTE(), 0,
            NiDataStreamElement::F_UINT16_1,
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);

        NiTStridedRandomAccessIterator<NiUInt16> kRemapIter;
        if (kRemapLock.DataStreamExists())
            kRemapIter = kRemapLock.begin<NiUInt16>();

        // Change the position stream to writable
        if (!pkMorphMeshModifier)
        {
            NiToolDataStream* pkDS = 
                (NiToolDataStream*)kOutPositionLock.GetDataStream();
            pkDS->SetAccessMask(NiDataStream::ACCESS_GPU_READ |
                NiDataStream::ACCESS_CPU_WRITE_VOLATILE);

            pkDS = (NiToolDataStream*)kOutNormalLock.GetDataStream();
            pkDS->SetAccessMask(NiDataStream::ACCESS_GPU_READ |
                NiDataStream::ACCESS_CPU_WRITE_VOLATILE);
        }

        pkNewSkinningMeshModifier = 
            CloneSkinningForFins(pkNewMesh, pkSkinningMeshModifier, 
            puiEdgeList, uiEdgeCount,
            kBoneLock.begin<NiTSimpleArray<NiInt16,4> >(),
            kWeightLock.begin<NiTSimpleArray<float,4> >(),
            kRemapIter,
            kOutPositionIter,
            kOutNormalIter,
            pkMorphMeshModifier != NULL);

        pkNewMesh->AddModifier(pkNewSkinningMeshModifier);
    }

    NiSingleShaderMaterial* pkShaderMat = 
        NiSingleShaderMaterial::Create(m_kFinsShaderName,false);
    pkNewMesh->ApplyMaterial(pkShaderMat);
    pkNewMesh->SetActiveMaterial(pkShaderMat);

    NiIntegerExtraData* pkBonesPerPart = 
        NiNew NiIntegerExtraData(m_uiBonesPerPartition);
    pkBonesPerPart->SetName(NI_BONES_PER_PARTITION_KEY);
    pkNewMesh->AddExtraData(pkBonesPerPart);

    // Get the morphing modifier.
    if (pkMorphMeshModifier)
    {
        NiFixedString kMorphSemantic;
        bool bIsMorphed = false;
        NiUInt32 uiIndex;
        if (pkNewSkinningMeshModifier)
        {
            bIsMorphed = pkMorphMeshModifier->IsMorphedElement(
                NiCommonSemantics::POSITION_BP(), 0, uiIndex);
            kMorphSemantic = pkMorphMeshModifier->MorphElementSemantic(
                NiCommonSemantics::POSITION_BP());
        }
        else
        {
            bIsMorphed = pkMorphMeshModifier->IsMorphedElement(
                NiCommonSemantics::POSITION(), 0, uiIndex);
            kMorphSemantic = pkMorphMeshModifier->MorphElementSemantic(
                NiCommonSemantics::POSITION());
        }

        if (bIsMorphed)
        {
            NiMorphMeshModifier* pkNewMorphMeshModifier = 
                CloneMorphingForFins(pkNewMesh, pkMesh, pkMorphMeshModifier,
                kMorphSemantic, puiEdgeList, uiEdgeCount);
            pkNewMesh->AddModifierAt(0, pkNewMorphMeshModifier);

            // Get the controller
            NiMorphWeightsController* pkMorph = (NiMorphWeightsController*)
                pkMesh->GetController((&NiMorphWeightsController::ms_RTTI));
            if (pkMorph)
            {
                NiMorphWeightsController* pkNewMorph = 
                    (NiMorphWeightsController*)pkMorph->Clone();
                pkNewMorph->SetTarget(pkNewMesh);
            }
        }
    }

    pkNewMesh->SetName(cpName);

    NiTexturingProperty* pkOutTexProp = NiNew NiTexturingProperty();
    pkOutTexProp->SetBaseTexture(pkTextureProp->GetBaseTexture());

    NiTexturingProperty::ShaderMap* pkShaderMap = 
        NiNew NiTexturingProperty::ShaderMap(pkDetailMap->GetTexture(), 1);
    pkOutTexProp->SetShaderMap(0, pkShaderMap);
    pkNewMesh->AttachProperty(pkOutTexProp);

    // Copy over all the extra attributes from the source mesh
    NiUInt32 uiCount = pkMesh->GetExtraDataSize();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        NiExtraData* pkExtraData = pkMesh->GetExtraDataAt(
            (unsigned short)ui);
        pkNewMesh->AddExtraData(pkExtraData);
    }

    NiColorExtraData* pkColorData = NiNew NiColorExtraData(m_kFurTintColor);
    pkNewMesh->AddExtraData(
        NiFixedString("FurTintColor"), pkColorData);

    pkNewMesh->RecomputeBounds();

    pkSortNode->AttachChild(pkNewMesh);

    pkNewMesh->UpdateProperties();
    pkNewMesh->UpdateEffects();

    NiFree(puiEdgeList);
}
//---------------------------------------------------------------------------
NiSkinningMeshModifier* NiFurGeneratorPlugin::CloneSkinningForShells(
    NiMesh* pkMesh, 
    NiSkinningMeshModifier* pkSkinningMeshModifier, 
    const NiUInt32 uiShellCount, const NiUInt32 uiBaseVertCount,
    const NiTStridedRandomAccessIterator<NiTSimpleArray<NiInt16,4> >& 
        kBoneIter,
    const NiTStridedRandomAccessIterator<NiTSimpleArray<float,4> >& 
        kWeightIter,
    const NiTStridedRandomAccessIterator<NiUInt16>& kRemapIter,
    const NiTStridedRandomAccessIterator<NiPoint3>& kPositionIter,
    const NiTStridedRandomAccessIterator<NiPoint3>& kNormalIter,
    bool bIsMorphed)
{  
    NiUInt32 uiBoneCount = pkSkinningMeshModifier->GetBoneCount();
    NiUInt32 uiVertCount = uiBaseVertCount * uiShellCount;

    // Create the blend indicies
    NiDataStreamElementLock kBoneLock = pkMesh->AddStreamGetLock(
        NiCommonSemantics::BLENDINDICES(), 0, 
        NiDataStreamElement::F_INT16_4, uiVertCount, 
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX, true);
    NiTStridedRandomAccessIterator<NiTSimpleArray<NiInt16,4> > kNewBoneIter = 
        kBoneLock.begin<NiTSimpleArray<NiInt16,4> >();
        
    // Create the weights
    NiDataStreamElementLock kWeightLock = pkMesh->AddStreamGetLock(
        NiCommonSemantics::BLENDWEIGHT(), 0, 
        NiDataStreamElement::F_FLOAT32_4, uiVertCount, 
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX, true);
    NiTStridedRandomAccessIterator<NiTSimpleArray<float,4> > kNewWeightIter = 
        kWeightLock.begin<NiTSimpleArray<float,4> >();
    
    // Create the bone palette
    if (kRemapIter.Exists())
    {
        NiDataStreamElementLock kRemapLock = pkMesh->AddStreamGetLock(
            NiCommonSemantics::BONE_PALETTE(), 0, 
            NiDataStreamElement::F_UINT16_1, uiBoneCount,
            NiDataStream::ACCESS_CPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_USER, true);
        NiTStridedRandomAccessIterator<NiUInt16> kNewRemapIter = 
            kRemapLock.begin<NiUInt16>();

        for (NiUInt32 uiBoneID = 0; uiBoneID < uiBoneCount; uiBoneID++)
        {
            kNewRemapIter[uiBoneID] = kRemapIter[uiBoneID];
        }
    }

    // Create the bind position
    NiDataStreamElementLock kBindPosLock = pkMesh->AddStreamGetLock(
        NiCommonSemantics::POSITION_BP(), 0, 
        NiDataStreamElement::F_FLOAT32_3, uiVertCount, 
        (bIsMorphed) ? 
            NiDataStream::ACCESS_CPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE :
            NiDataStream::ACCESS_CPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC, 
        NiDataStream::USAGE_VERTEX, true);
    NiTStridedRandomAccessIterator<NiPoint3> kNewBindPosIter =
        kBindPosLock.begin<NiPoint3>();

    // Copy over the base verts into the bind position
    for (NiUInt32 ui = 0; ui < uiVertCount; ui++)
    {
        kNewBindPosIter[ui] = kPositionIter[ui];
    }

    // Create the bind normal
    NiDataStreamElementLock kBindNormLock = pkMesh->AddStreamGetLock(
        NiCommonSemantics::NORMAL_BP(), 0, 
        NiDataStreamElement::F_FLOAT32_3, uiVertCount, 
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC, 
        NiDataStream::USAGE_VERTEX, true);
    NiTStridedRandomAccessIterator<NiPoint3> kNewBindNormIter =
        kBindNormLock.begin<NiPoint3>();

    // Fill in the normal bp stream
    for(NiUInt32 v = 0; v < uiVertCount; v++)
    {
        kNewBindNormIter[v] = kNormalIter[v];
    }

    // Copy over the bone indices and weights
    for( NiUInt32 uiShell = 0; uiShell < uiShellCount; uiShell++)
    {
        for(NiUInt32 uiVertID = 0; uiVertID < uiBaseVertCount; 
            uiVertID++)
        {
            NiUInt32 uiOutVertID = 
                (uiBaseVertCount * uiShell) + uiVertID;

            kNewBoneIter[uiOutVertID] = kBoneIter[uiVertID];
            kNewWeightIter[uiOutVertID] = kWeightIter[uiVertID];
        }
    }

    return (NiSkinningMeshModifier*)pkSkinningMeshModifier->Clone();
}
//---------------------------------------------------------------------------
NiSkinningMeshModifier* NiFurGeneratorPlugin::CloneSkinningForFins(
    NiMesh* pkMesh, 
    NiSkinningMeshModifier* pkSkinningMeshModifier, 
    const NiUInt32* puiEdgeList, const NiUInt32 uiEdgeCount,
    const NiTStridedRandomAccessIterator<NiTSimpleArray<NiInt16,4> >& 
        kBoneIter,
    const NiTStridedRandomAccessIterator<NiTSimpleArray<float,4> >& 
        kWeightIter,
    const NiTStridedRandomAccessIterator<NiUInt16>& kRemapIter,
    const NiTStridedRandomAccessIterator<NiPoint3>& kPositionIter,
    const NiTStridedRandomAccessIterator<NiPoint3>& kNormalIter,
    bool bIsMorphed)
{
    NiUInt32 uiBoneCount = pkSkinningMeshModifier->GetBoneCount();
    NiUInt32 uiVertCount = uiEdgeCount*4;

    // Create the blend indicies
    NiDataStreamElementLock kBoneLock = pkMesh->AddStreamGetLock(
        NiCommonSemantics::BLENDINDICES(), 0, 
        NiDataStreamElement::F_INT16_4, uiVertCount, 
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX, true);
    NiTStridedRandomAccessIterator<NiTSimpleArray<NiInt16,4> > kNewBoneIter = 
        kBoneLock.begin<NiTSimpleArray<NiInt16,4> >();

    // Create the weights
    NiDataStreamElementLock kWeightLock = pkMesh->AddStreamGetLock(
        NiCommonSemantics::BLENDWEIGHT(), 0, 
        NiDataStreamElement::F_FLOAT32_4, uiVertCount, 
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX, true);
    NiTStridedRandomAccessIterator<NiTSimpleArray<float,4> > kNewWeightIter = 
        kWeightLock.begin<NiTSimpleArray<float,4> >();

    // Create the bone palette
    if (kRemapIter.Exists())
    {
        NiDataStreamElementLock kRemapLock = pkMesh->AddStreamGetLock(
            NiCommonSemantics::BONE_PALETTE(), 0, 
            NiDataStreamElement::F_UINT16_1, uiBoneCount,
            NiDataStream::ACCESS_CPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_USER, true);
        NiTStridedRandomAccessIterator<NiUInt16> kNewRemapIter = 
            kRemapLock.begin<NiUInt16>();

        // Fill in the bones remap from the original
        for (NiUInt32 uiBoneID = 0; uiBoneID < uiBoneCount; uiBoneID++)
        {
            kNewRemapIter[uiBoneID] = kRemapIter[uiBoneID];
        }
    }

    // Create the bind position
    NiDataStreamElementLock kBindPosLock = pkMesh->AddStreamGetLock(
        NiCommonSemantics::POSITION_BP(), 0, 
        NiDataStreamElement::F_FLOAT32_3, uiVertCount, 
        (bIsMorphed) ? 
            NiDataStream::ACCESS_CPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE :
            NiDataStream::ACCESS_CPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC, 
        NiDataStream::USAGE_VERTEX, true);
    NiTStridedRandomAccessIterator<NiPoint3> kNewBindPosIter =
        kBindPosLock.begin<NiPoint3>();

    // Fill in the position bp stream
    for(NiUInt32 v = 0; v < uiVertCount; v++)
    {
        kNewBindPosIter[v] = kPositionIter[v];
    }

    // Create the bind normal
    NiDataStreamElementLock kBindNormLock = pkMesh->AddStreamGetLock(
        NiCommonSemantics::NORMAL_BP(), 0, 
        NiDataStreamElement::F_FLOAT32_3, uiVertCount, 
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC, 
        NiDataStream::USAGE_VERTEX, true);
    NiTStridedRandomAccessIterator<NiPoint3> kNewBindNormIter =
        kBindNormLock.begin<NiPoint3>();

    // Fill in the normal bp stream
    for(NiUInt32 v = 0; v < uiVertCount; v++)
    {
        kNewBindNormIter[v] = kNormalIter[v];
    }

    // From the list of triangle edges create a quad for each edge extruding 
    // out in the direction of the normals on that edge
    for(NiUInt32 e = 0; e < uiEdgeCount; e++)
    {
        NiUInt32 uiIndex0 = puiEdgeList[(e<<1)];
        NiUInt32 uiIndex1 = puiEdgeList[(e<<1)+1];
        
        NiUInt32 uiIndex = (e*4);

        kNewBoneIter[uiIndex] = kBoneIter[uiIndex0];
        kNewWeightIter[uiIndex] = kWeightIter[uiIndex0];
        uiIndex++;

        kNewBoneIter[uiIndex] = kBoneIter[uiIndex1];
        kNewWeightIter[uiIndex] = kWeightIter[uiIndex1];
        uiIndex++;

        kNewBoneIter[uiIndex] = kBoneIter[uiIndex0];
        kNewWeightIter[uiIndex] = kWeightIter[uiIndex0];
        uiIndex++;

        kNewBoneIter[uiIndex] = kBoneIter[uiIndex1];
        kNewWeightIter[uiIndex] = kWeightIter[uiIndex1];
        uiIndex++;
    }

    return (NiSkinningMeshModifier*)pkSkinningMeshModifier->Clone();
}
//---------------------------------------------------------------------------
NiMorphMeshModifier* NiFurGeneratorPlugin::CloneMorphingForShells(
    NiMesh* pkNewMesh, NiMesh* pkOldMesh, 
    NiMorphMeshModifier* pkMorphMeshModifier,
    const NiFixedString& kMorphSemantic,
    const NiUInt32 uiShellCount, const NiUInt32 uiBaseVertCount)
{
    NiUInt32 uiNumTargets = pkMorphMeshModifier->GetNumTargets();
    NiUInt32 uiVertCount = uiBaseVertCount * uiShellCount;

    NiDataStreamElementLock kMorphWeightsLock(pkOldMesh, 
        NiCommonSemantics::MORPHWEIGHTS(), 0,
        NiDataStreamElement::F_FLOAT32_1, 
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
    NiTStridedRandomAccessIterator<float> kMorphWeightsIter = 
        kMorphWeightsLock.begin<float>();

    // Create the morph weights
    NiDataStreamElementLock kNewMorphWeightsLock = 
        pkNewMesh->AddStreamGetLock(
        NiCommonSemantics::MORPHWEIGHTS(), 0, 
        NiDataStreamElement::F_FLOAT32_1, uiNumTargets, 
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_MUTABLE, 
        NiDataStream::USAGE_USER, true);
    NiTStridedRandomAccessIterator<float> kNewMorphWeightsIter =
        kNewMorphWeightsLock.begin<float>();

    for(NiUInt32 uiTarget = 0; uiTarget < uiNumTargets; uiTarget++)
    {
        kNewMorphWeightsIter[uiTarget] = kMorphWeightsIter[uiTarget];
    }

    NiDataStreamElementLock kNormalLock(pkOldMesh, NiCommonSemantics::NORMAL(),
        0, NiDataStreamElement::F_FLOAT32_3, 
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
    NiTStridedRandomAccessIterator<NiPoint3> kNormalIter = 
        kNormalLock.begin<NiPoint3>();

    float fShellLengthIncrement = 
        (m_fFurLength / (float)(m_uiNumberOfShells+1) * m_fShellSizeFactor);
    float fShellLength = fShellLengthIncrement;

    for (NiUInt32 uiTarget = 0; uiTarget < uiNumTargets; uiTarget++)
    {
        NiDataStreamElementLock kMorphPositionLock(pkOldMesh, 
            kMorphSemantic, uiTarget,
            NiDataStreamElement::F_FLOAT32_3, 
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
        NiTStridedRandomAccessIterator<NiPoint3> kMorphPositionIter = 
            kMorphPositionLock.begin<NiPoint3>();

        // Create the morph position
        NiDataStreamElementLock kNewMorphPositionLock = 
            pkNewMesh->AddStreamGetLock(
            kMorphSemantic, uiTarget, 
            NiDataStreamElement::F_FLOAT32_3, uiVertCount, 
            NiDataStream::ACCESS_CPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC, 
            NiDataStream::USAGE_VERTEX, true);
        NiTStridedRandomAccessIterator<NiPoint3> kNewMorphPositionIter =
            kNewMorphPositionLock.begin<NiPoint3>();

        for(NiUInt32 uiShell = 0; uiShell < m_uiNumberOfShells; uiShell++)
        {
            // Duplicate Geometry Data
            for(NiUInt32 uiVID = 0; uiVID < uiBaseVertCount; uiVID++)
            {
                NiUInt32 uiIndex = uiShell * uiBaseVertCount + uiVID;
                kNewMorphPositionIter[uiIndex] = kMorphPositionIter[uiVID];

                if (uiTarget == 0)
                {
                    float fShellLengthRandom = fShellLength +
                        (NiSymmetricRandom() * fShellLengthIncrement * 
                        m_fFurLengthRandomness);

                    kNewMorphPositionIter[uiIndex].x += kNormalIter[uiVID].x *
                        fShellLengthRandom;
                    kNewMorphPositionIter[uiIndex].y += kNormalIter[uiVID].y *
                        fShellLengthRandom;
                    kNewMorphPositionIter[uiIndex].z += kNormalIter[uiVID].z *
                        fShellLengthRandom;
                }
            }

            fShellLength += fShellLengthIncrement;
        }
    }

    return (NiMorphMeshModifier*)pkMorphMeshModifier->Clone();
}
//---------------------------------------------------------------------------
NiMorphMeshModifier* NiFurGeneratorPlugin::CloneMorphingForFins(
    NiMesh* pkNewMesh, NiMesh* pkOldMesh,
    NiMorphMeshModifier* pkMorphMeshModifier, 
    const NiFixedString& kMorphSemantic,
    const NiUInt32* puiEdgeList, const NiUInt32 uiEdgeCount)
{
    NiUInt32 uiNumTargets = pkMorphMeshModifier->GetNumTargets();
    NiUInt32 uiVertCount = uiEdgeCount*4;

    NiDataStreamElementLock kMorphWeightsLock(pkOldMesh, 
        NiCommonSemantics::MORPHWEIGHTS(), 0,
        NiDataStreamElement::F_FLOAT32_1, 
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
    NiTStridedRandomAccessIterator<float> kMorphWeightsIter = 
        kMorphWeightsLock.begin<float>();

    // Create the morph weights
    NiDataStreamElementLock kNewMorphWeightsLock = 
        pkNewMesh->AddStreamGetLock(
        NiCommonSemantics::MORPHWEIGHTS(), 0, 
        NiDataStreamElement::F_FLOAT32_1, uiNumTargets, 
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_MUTABLE, 
        NiDataStream::USAGE_USER, true);
    NiTStridedRandomAccessIterator<float> kNewMorphWeightsIter =
        kNewMorphWeightsLock.begin<float>();

    for(NiUInt32 uiTarget = 0; uiTarget < uiNumTargets; uiTarget++)
    {
        kNewMorphWeightsIter[uiTarget] = kMorphWeightsIter[uiTarget];
    }

    NiDataStreamElementLock kNormalLock(pkOldMesh, NiCommonSemantics::NORMAL(),
        0, NiDataStreamElement::F_FLOAT32_3, 
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
    NiTStridedRandomAccessIterator<NiPoint3> kNormalIter = 
        kNormalLock.begin<NiPoint3>();

    NiPoint3 kaVerts[4];
    NiPoint3 kaNormals[4];

    for (NiUInt32 uiTarget = 0; uiTarget < uiNumTargets; uiTarget++)
    {
        NiDataStreamElementLock kMorphPositionLock(pkOldMesh, 
            kMorphSemantic, uiTarget,
            NiDataStreamElement::F_FLOAT32_3, 
            NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
        NiTStridedRandomAccessIterator<NiPoint3> kMorphPositionIter = 
            kMorphPositionLock.begin<NiPoint3>();

        // Create the morph position
        NiDataStreamElementLock kNewMorphPositionLock = 
            pkNewMesh->AddStreamGetLock(
            kMorphSemantic, uiTarget, 
            NiDataStreamElement::F_FLOAT32_3, uiVertCount, 
            NiDataStream::ACCESS_CPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC, 
            NiDataStream::USAGE_VERTEX, true);
        NiTStridedRandomAccessIterator<NiPoint3> kNewMorphPositionIter =
            kNewMorphPositionLock.begin<NiPoint3>();

        for(NiUInt32 e = 0; e < uiEdgeCount; e++)
        {
            NiUInt32 uiIndex0 = puiEdgeList[(e<<1)];
            NiUInt32 uiIndex1 = puiEdgeList[(e<<1)+1];

            kaVerts[0] = kMorphPositionIter[uiIndex0];
            kaVerts[1] = kMorphPositionIter[uiIndex1];
            kaVerts[2] = kMorphPositionIter[uiIndex0];
            kaVerts[3] = kMorphPositionIter[uiIndex1];

            // We only do this for the first target.
            if (uiTarget == 0)
            {
                kaNormals[0] = kNormalIter[uiIndex0];
                kaNormals[1] = kNormalIter[uiIndex1];
                kaNormals[2] = kNormalIter[uiIndex0];
                kaNormals[3] = kNormalIter[uiIndex1];

                float fFurLength = m_fFurLength +
                    (NiSymmetricRandom() * m_fFurLength * 
                    m_fFurLengthRandomness);

                // Extrude Verts 2 and 3 towards normal direction
                kaVerts[2].x += kaNormals[2].x * fFurLength;
                kaVerts[2].y += kaNormals[2].y * fFurLength;
                kaVerts[2].z += kaNormals[2].z * fFurLength;

                kaVerts[3].x += kaNormals[3].x * fFurLength;
                kaVerts[3].y += kaNormals[3].y * fFurLength;
                kaVerts[3].z += kaNormals[3].z * fFurLength;
            }

            // Copy Data into output data
            for(int i = 0; i < 4; i++)
            {
                NiUInt32 uiIndex = (e*4)+i;
                kNewMorphPositionIter[uiIndex] = kaVerts[i];
            }
        }
    }

    return (NiMorphMeshModifier*)pkMorphMeshModifier->Clone();
}
//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::BuildMeshList(NiSceneGraphSharedData* 
                                         pkSGSharedData)
{
    // Search through the scene for Mesh Objects with "fur" extra data
    for (NiUInt32 ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spScene = pkSGSharedData->GetRootAt(ui);
        RecusiveMeshSearch(spScene);
    }
}
//---------------------------------------------------------------------------
bool NiFurGeneratorPlugin::HasFurData(NiMesh* pkMesh)
{
    if (!pkMesh)
        return false;

    const NiMaterial* pkMaterial = pkMesh->GetActiveMaterial();
    if (pkMaterial)
    {
        if (pkMaterial->GetName() == "Fur")
        {
            return true;
        }
        else if (pkMaterial->GetName() == "FurSkinning")
        {
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::RecusiveMeshSearch(NiNode* pkNode)
{
    NiTPointerList<NiAVObject*> kMeshes;
    pkNode->GetObjectsByType(&NiMesh::ms_RTTI, kMeshes);

    NiTListIterator kIter = kMeshes.GetHeadPos();
    while (kIter)
    {
        NiMesh* pkMesh = (NiMesh*)kMeshes.GetNext(kIter);
        if (HasFurData(pkMesh))
        {
            // Found a Mesh 
            m_pkMeshList->AddTail(pkMesh);
        }
    }
}
//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::SubdivideTriangle(
    NiUInt32 uiIndex0, NiUInt32 uiIndex1, NiUInt32 uiIndex2,
    NiTStridedRandomAccessIterator<NiPoint3>& kPositionIter,
    NiTStridedRandomAccessIterator<NiPoint3>& kNormalIter,
    NiTStridedRandomAccessIterator<NiPoint2>& kTexCoordIter,
    NiTStridedRandomAccessIterator<NiColorA>& kColorIter,
    NiTStridedRandomAccessIterator<NiTSimpleArray<NiInt16,4> >& kBoneIter,
    NiTStridedRandomAccessIterator<NiTSimpleArray<float,4> >& kWeightIter,
    NiTStridedRandomAccessIterator<NiPoint3>& kBindPosIter,
    NiTStridedRandomAccessIterator<NiPoint3>& kBindNormIter,
    NiUInt32 uiNumTargets,
    NiTStridedRandomAccessIterator<NiPoint3>* pkMorphPositionsIter,
    NiTStridedRandomAccessIterator<NiUInt32>& kIndexIter,
    NiUInt32& uiCurrentVertCount,
    NiUInt32& uiNewIndexCounter)
{
    NiUInt32 uiNewIndex0 = uiCurrentVertCount++;
    NiUInt32 uiNewIndex1 = uiCurrentVertCount++;
    NiUInt32 uiNewIndex2 = uiCurrentVertCount++;

    NiPoint3 kPosition0 = kPositionIter[uiIndex0];
    NiPoint3 kPosition1 = kPositionIter[uiIndex1];
    NiPoint3 kPosition2 = kPositionIter[uiIndex2];

    kPositionIter[uiNewIndex0] = (kPosition0 + kPosition1) * 0.5f;
    kPositionIter[uiNewIndex1] = (kPosition1 + kPosition2) * 0.5f;
    kPositionIter[uiNewIndex2] = (kPosition0 + kPosition2) * 0.5f;

    NiPoint3 kNormal0 = kNormalIter[uiIndex0];
    NiPoint3 kNormal1 = kNormalIter[uiIndex1];
    NiPoint3 kNormal2 = kNormalIter[uiIndex2];

    kNormalIter[uiNewIndex0] = (kNormal0 + kNormal1) * 0.5f;
    kNormalIter[uiNewIndex1] = (kNormal1 + kNormal2) * 0.5f;
    kNormalIter[uiNewIndex2] = (kNormal0 + kNormal2) * 0.5f;

    NiPoint2 kTexCoord0 = kTexCoordIter[uiIndex0];
    NiPoint2 kTexCoord1 = kTexCoordIter[uiIndex1];
    NiPoint2 kTexCoord2 = kTexCoordIter[uiIndex2];

    kTexCoordIter[uiNewIndex0] = (kTexCoord0 + kTexCoord1) * 0.5f;
    kTexCoordIter[uiNewIndex1] = (kTexCoord1 + kTexCoord2) * 0.5f;
    kTexCoordIter[uiNewIndex2] = (kTexCoord0 + kTexCoord2) * 0.5f;

    NiColorA kColor0 = NiColorA(1,1,1,1);
    NiColorA kColor1 = NiColorA(1,1,1,1);
    NiColorA kColor2 = NiColorA(1,1,1,1);
    if (kColorIter.Exists())
    {
        kColor0 = kColorIter[uiIndex0];
        kColor1 = kColorIter[uiIndex1];
        kColor2 = kColorIter[uiIndex2];
    }

    kColorIter[uiNewIndex0] = (kColor0 + kColor1) * 0.5f;
    kColorIter[uiNewIndex1] = (kColor1 + kColor2) * 0.5f;
    kColorIter[uiNewIndex2] = (kColor0 + kColor2) * 0.5f;

    if (kBoneIter.Exists())
    {
        kBoneIter[uiNewIndex0] = kBoneIter[uiIndex0];
        kBoneIter[uiNewIndex1] = kBoneIter[uiIndex1];
        kBoneIter[uiNewIndex2] = kBoneIter[uiIndex2];
    }

    if (kWeightIter.Exists())
    {
        kWeightIter[uiNewIndex0] = kWeightIter[uiIndex0];
        kWeightIter[uiNewIndex1] = kWeightIter[uiIndex1];
        kWeightIter[uiNewIndex2] = kWeightIter[uiIndex2];
    }

    if (kBindPosIter.Exists())
    {
        NiPoint3 kBindPos0 = kBindPosIter[uiIndex0];
        NiPoint3 kBindPos1 = kBindPosIter[uiIndex1];
        NiPoint3 kBindPos2 = kBindPosIter[uiIndex2];

        kBindPosIter[uiNewIndex0] = (kBindPos0 + kBindPos1) * 0.5f;
        kBindPosIter[uiNewIndex1] = (kBindPos1 + kBindPos2) * 0.5f;
        kBindPosIter[uiNewIndex2] = (kBindPos0 + kBindPos2) * 0.5f;
    }

    if (kBindNormIter.Exists())
    {
        NiPoint3 kBindNorm0 = kBindNormIter[uiIndex0];
        NiPoint3 kBindNorm1 = kBindNormIter[uiIndex1];
        NiPoint3 kBindNorm2 = kBindNormIter[uiIndex2];

        kBindNormIter[uiNewIndex0] = (kBindNorm0 + kBindNorm1) * 0.5f;
        kBindNormIter[uiNewIndex1] = (kBindNorm1 + kBindNorm2) * 0.5f;
        kBindNormIter[uiNewIndex2] = (kBindNorm0 + kBindNorm2) * 0.5f;
    }

    if (pkMorphPositionsIter)
    {
        for (NiUInt32 uiTarget = 0; uiTarget < uiNumTargets; uiTarget++)
        {
            NiPoint3 kMorphPos0 = pkMorphPositionsIter[uiTarget][uiIndex0];
            NiPoint3 kMorphPos1 = pkMorphPositionsIter[uiTarget][uiIndex1];
            NiPoint3 kMorphPos2 = pkMorphPositionsIter[uiTarget][uiIndex2];

            pkMorphPositionsIter[uiTarget][uiNewIndex0] = 
                (kMorphPos0 + kMorphPos1) * 0.5f;
            pkMorphPositionsIter[uiTarget][uiNewIndex1] = 
                (kMorphPos1 + kMorphPos2) * 0.5f;
            pkMorphPositionsIter[uiTarget][uiNewIndex2] = 
                (kMorphPos0 + kMorphPos2) * 0.5f;
        }
    }

    NiPoint2 kTex0 = kTexCoordIter[uiIndex0] * m_fFurDensity;
    NiPoint2 kTex1 = kTexCoordIter[uiNewIndex0] * m_fFurDensity;
    NiPoint2 kTex2 = kTexCoordIter[uiNewIndex2] * m_fFurDensity;

    if (!NeedsSubdivision(kTex0, kTex1, kTex2))
    {
        kIndexIter[uiNewIndexCounter++] = uiIndex0;
        kIndexIter[uiNewIndexCounter++] = uiNewIndex0;
        kIndexIter[uiNewIndexCounter++] = uiNewIndex2;
    }
    else
    {
        SubdivideTriangle(uiIndex0, uiNewIndex0, uiNewIndex2,
            kPositionIter,
            kNormalIter,
            kTexCoordIter,
            kColorIter,
            kBoneIter,
            kWeightIter,
            kBindPosIter,
            kBindNormIter,
            uiNumTargets,
            pkMorphPositionsIter,
            kIndexIter,
            uiCurrentVertCount,
            uiNewIndexCounter);
    }

    kTex0 = kTexCoordIter[uiNewIndex0] * m_fFurDensity;
    kTex1 = kTexCoordIter[uiNewIndex1] * m_fFurDensity;
    kTex2 = kTexCoordIter[uiNewIndex2] * m_fFurDensity;

    if (!NeedsSubdivision(kTex0, kTex1, kTex2))
    {
        kIndexIter[uiNewIndexCounter++] = uiNewIndex0;
        kIndexIter[uiNewIndexCounter++] = uiNewIndex1;
        kIndexIter[uiNewIndexCounter++] = uiNewIndex2;
    }
    else
    {
        SubdivideTriangle(uiNewIndex0, uiNewIndex1, uiNewIndex2,
            kPositionIter,
            kNormalIter,
            kTexCoordIter,
            kColorIter,
            kBoneIter,
            kWeightIter,
            kBindPosIter,
            kBindNormIter,
            uiNumTargets,
            pkMorphPositionsIter,
            kIndexIter,
            uiCurrentVertCount,
            uiNewIndexCounter);
    }

    kTex0 = kTexCoordIter[uiNewIndex2] * m_fFurDensity;
    kTex1 = kTexCoordIter[uiNewIndex1] * m_fFurDensity;
    kTex2 = kTexCoordIter[uiIndex2] * m_fFurDensity;

    if (!NeedsSubdivision(kTex0, kTex1, kTex2))
    {
        kIndexIter[uiNewIndexCounter++] = uiNewIndex2;
        kIndexIter[uiNewIndexCounter++] = uiNewIndex1;
        kIndexIter[uiNewIndexCounter++] = uiIndex2;
    }
    else
    {
        SubdivideTriangle(uiNewIndex2, uiNewIndex1, uiIndex2,
            kPositionIter,
            kNormalIter,
            kTexCoordIter,
            kColorIter,
            kBoneIter,
            kWeightIter,
            kBindPosIter,
            kBindNormIter,
            uiNumTargets,
            pkMorphPositionsIter,
            kIndexIter,
            uiCurrentVertCount,
            uiNewIndexCounter);
    }

    kTex0 = kTexCoordIter[uiNewIndex0] * m_fFurDensity;
    kTex1 = kTexCoordIter[uiIndex1] * m_fFurDensity;
    kTex2 = kTexCoordIter[uiNewIndex1] * m_fFurDensity;

    if (!NeedsSubdivision(kTex0, kTex1, kTex2))
    {
        kIndexIter[uiNewIndexCounter++] = uiNewIndex0;
        kIndexIter[uiNewIndexCounter++] = uiIndex1;
        kIndexIter[uiNewIndexCounter++] = uiNewIndex1;
    }
    else
    {
        SubdivideTriangle(uiNewIndex0, uiIndex1, uiNewIndex1,
            kPositionIter,
            kNormalIter,
            kTexCoordIter,
            kColorIter,
            kBoneIter,
            kWeightIter,
            kBindPosIter,
            kBindNormIter,
            uiNumTargets,
            pkMorphPositionsIter,
            kIndexIter,
            uiCurrentVertCount,
            uiNewIndexCounter);
    }
}
//---------------------------------------------------------------------------
void NiFurGeneratorPlugin::RemoveDetailMap(NiAVObject* pkNode)
{
    NiTexturingProperty* pkTexProp = 
        (NiTexturingProperty*)pkNode->GetProperty(NiProperty::TEXTURING);

    // Remove the detail map is it is no longer needed. It only served as way
    // of defining which texture coordinate set to use for the shells and for 
    // visualization purposes while modeling.
    if (pkTexProp)
        pkTexProp->SetDetailMap(NULL);
}
//---------------------------------------------------------------------------
bool NiFurGeneratorPlugin::IsEdgeInList(NiUInt32* puiTestEdge, 
    NiUInt32* puiEdgeList, NiInt32 iNumberOfEdges)
{
    // See if edge is already in the list
    for(int i = 0; i < iNumberOfEdges; i++)
    {
        if(puiEdgeList[(i<<1)] == puiTestEdge[0] &&
            puiEdgeList[(i<<1)+1] == puiTestEdge[1])
        {
            return true;
        }
        else if(puiEdgeList[(i<<1)] == puiTestEdge[1] && 
            puiEdgeList[(i<<1)+1] == puiTestEdge[0])
        {
            return true;
        }

    }
    return false;
}
//---------------------------------------------------------------------------
