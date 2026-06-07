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

#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoImpl.h"
#include "CrosswalkGamebryoConvMap.h"
#include "CrosswalkGamebryoSettings.h"

#include "Scene.h"
#include "Model.h"

#include "NiAVObject.h"

namespace
{
    //---------------------------------------------------------------------------
    // Recursively count the number of models in the scene.
    //---------------------------------------------------------------------------
    int CountModels(CSLModel* in_pModel)
    {
        if (!in_pModel)
            return 0;

        int count = 1;

        CSLModel** chidlrenArray = in_pModel->GetChildrenList();
        if (chidlrenArray)
            for (SI_Int i = 0; i < in_pModel->GetChildrenCount(); ++i)
                count += CountModels(chidlrenArray[i]);

        return count;
    }

    //---------------------------------------------------------------------------
}

namespace epg
{
    //---------------------------------------------------------------------------
    // Conversion context.
    //---------------------------------------------------------------------------
    Context::Context(
        CSLScene& in_Scene,
        const SceneInfo& in_SceneInfo,
        const ConverterSettings& in_Settings,
        const ConversionMap& in_ConvMap,
        NodeMap& in_NodeMap,
        MaterialMap& in_MatMap,
        TextureMap& in_TexMap,
        EffectMap& in_EffectMap,
        AnimationMap& in_AnimMap,
        SkinMap& in_SkinMap,
        LightingMap& in_LightingMap,
        Logger& in_Logger,
        ProgressMonitor& in_Progress)

        : m_Scene( in_Scene )
        , m_SceneInfo( in_SceneInfo )
        , m_Settings( in_Settings )
        , m_ConvMap( in_ConvMap )
        , m_NodeMap( in_NodeMap )
        , m_MaterialMap( in_MatMap )
        , m_TextureMap( in_TexMap )
        , m_EffectMap( in_EffectMap )
        , m_AnimationMap( in_AnimMap )
        , m_SkinMap( in_SkinMap )
        , m_LightingMap( in_LightingMap )
        , m_Logger( in_Logger )
        , m_Progress( in_Progress )
        , m_ExpectedModelCount( CountModels(in_Scene.Root()) )
        , m_CurrentModelCount( 0 )
    {
        // Avoid division by zero later on...
        if (m_ExpectedModelCount <= 0)
            m_ExpectedModelCount = 1;
    }

    //---------------------------------------------------------------------------
    // For convenience, it is a logger that forwards calls to the actual logger.
    //---------------------------------------------------------------------------
    void Context::StartConversion()
    {
        m_Logger.StartConversion();
    }

    //---------------------------------------------------------------------------
    void Context::EndConversion()
    {
        m_Logger.EndConversion();
    }

    //---------------------------------------------------------------------------
    bool Context::Log(LogLevel in_Level, const char * in_Msg)
    {
        bool isOK = m_Logger.Log(in_Level, in_Msg);
        return m_Settings.GetIgnoreAllErrors() ? true : isOK;
    }

    //---------------------------------------------------------------------------
    // For convenience, it is a progress monitor that forwards calls to the actual monitor.
    //---------------------------------------------------------------------------
    void Context::StartPhase(ProgressPhase in_Phase)
    {
        m_Progress.StartPhase(in_Phase);
    }

    //---------------------------------------------------------------------------
    void Context::EndPhase(ProgressPhase in_Phase)
    {
        m_Progress.EndPhase(in_Phase);
    }

    //---------------------------------------------------------------------------
    void Context::PhaseProgress(float in_CompletionFraction)
    {
        m_Progress.PhaseProgress(in_CompletionFraction);
    }

    //---------------------------------------------------------------------------
    // Convert the given material using the conversion map.
    //---------------------------------------------------------------------------
    bool Context::Convert(CSLBaseMaterial& in_Material)
    {
        return m_ConvMap.Convert(*this, in_Material);
    }

    //---------------------------------------------------------------------------
    // Convert the given model using the conversion map.
    //---------------------------------------------------------------------------
    bool Context::Convert(CSLModel& in_Model)
    {
        PhaseProgress(m_CurrentModelCount++ * 1.0f / m_ExpectedModelCount);

        if (!m_ConvMap.Convert(*this, in_Model))
            return false;

        return ConvertChildren(in_Model);
    }

    //---------------------------------------------------------------------------
    bool Context::ConvertChildren(CSLModel& in_Model)
    {
        CSLModel** childrenList = in_Model.GetChildrenList();
        for (int i = 0; i < in_Model.GetChildrenCount(); ++i)
        {
            NIASSERT( childrenList[i] != NULL );

            if (! Convert(childrenList[i][0]))
            {
                return false;
            }
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Node map.
    //---------------------------------------------------------------------------
    NodeMap & Context::GetNodeMap()
    {
        return m_NodeMap;
    }

    //---------------------------------------------------------------------------
    // Material map.
    //---------------------------------------------------------------------------
    MaterialMap & Context::GetMaterialMap()
    {
        return m_MaterialMap;
    }

    //---------------------------------------------------------------------------
    // Texture map.
    //---------------------------------------------------------------------------
    TextureMap & Context::GetTextureMap()
    {
        return m_TextureMap;
    }

    //---------------------------------------------------------------------------
    // Effect map.
    //---------------------------------------------------------------------------
    EffectMap& Context::GetEffectMap()
    {
        return m_EffectMap;
    }

    //---------------------------------------------------------------------------
    // Animation map.
    //---------------------------------------------------------------------------
    AnimationMap& Context::GetAnimationMap()
    {
        return m_AnimationMap;
    }

    //---------------------------------------------------------------------------
    // Retrieve the skin map.
    //---------------------------------------------------------------------------
    SkinMap& Context::GetSkinMap()
    {
        return m_SkinMap;
    }

    //---------------------------------------------------------------------------
    // Retrieve the lighting map.
    //---------------------------------------------------------------------------
    LightingMap& Context::GetLightingMap()
    {
        return m_LightingMap;
    }

    //---------------------------------------------------------------------------
    // Retrieve the conversion settings.
    //---------------------------------------------------------------------------
    const ConverterSettings& Context::GetSettings() const
    {
        return m_Settings;
    }

    //---------------------------------------------------------------------------
    // Retrieve global information about the scene.
    //---------------------------------------------------------------------------
    const SceneInfo& Context::GetSceneInfo() const
    {
        return m_SceneInfo;
    }

    //---------------------------------------------------------------------------
    // Retrieve the scene being converted.
    //---------------------------------------------------------------------------
    CSLScene& Context::GetScene()
    {
        return m_Scene;
    }

    //---------------------------------------------------------------------------
}
